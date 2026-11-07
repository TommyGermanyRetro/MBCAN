package de.mbc89.android.transport

import android.os.ParcelFileDescriptor
import android.system.ErrnoException
import android.system.Os
import android.system.OsConstants
import java.io.EOFException
import java.io.IOException
import java.net.InetSocketAddress
import java.net.Socket
import kotlinx.coroutines.CancellationException
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.Job
import kotlinx.coroutines.channels.Channel
import kotlinx.coroutines.currentCoroutineContext
import kotlinx.coroutines.ensureActive
import kotlinx.coroutines.flow.Flow
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.asStateFlow
import kotlinx.coroutines.flow.receiveAsFlow
import kotlinx.coroutines.launch
import kotlinx.coroutines.sync.Mutex
import kotlinx.coroutines.sync.withLock
import kotlinx.coroutines.withContext

sealed interface ConnectionState {
    data object Disconnected : ConnectionState
    data object Connecting : ConnectionState
    data class Connected(val host: String, val port: Int) : ConnectionState
    data class Failed(val reason: String) : ConnectionState

    // Set by the service only: an established connection was lost and is being re-established.
    data class Reconnecting(val reason: String) : ConnectionState
}

class CanTcpTransport(private val scope: CoroutineScope) : CanTransport {
    private val connectionMutex = Mutex()
    private val sendMutex = Mutex()
    private val packets = Channel<CanPacket>(capacity = 64)
    private val _state = MutableStateFlow<ConnectionState>(ConnectionState.Disconnected)
    @Volatile
    private var socket: Socket? = null
    private var readerJob: Job? = null

    override val state: StateFlow<ConnectionState> = _state.asStateFlow()
    override val receivedPackets: Flow<CanPacket> = packets.receiveAsFlow()

    override suspend fun connect(host: String, port: Int) {
        require(host.isNotBlank()) { "Host must not be blank." }
        require(port in 1..65535) { "TCP port must be between 1 and 65535." }

        connectionMutex.withLock {
            val closeError = closeCurrentConnection()
            if (closeError != null) {
                _state.value = ConnectionState.Failed(
                    closeError.message ?: "Failed to close the previous TCP connection.",
                )
                return
            }
            _state.value = ConnectionState.Connecting
            val candidate = Socket()
            try {
                // Nagle would batch the small CAN frames of a configuration burst.
                candidate.tcpNoDelay = true
                withContext(Dispatchers.IO) {
                    candidate.connect(InetSocketAddress(host, port), CONNECT_TIMEOUT_MS)
                    enableLivenessCheck(candidate)
                }
                socket = candidate
                _state.value = ConnectionState.Connected(host, port)
                readerJob = scope.launchReader(candidate)
            } catch (exception: CancellationException) {
                closeSocket(candidate)?.let(exception::addSuppressed)
                throw exception
            } catch (exception: IOException) {
                val closeFailure = closeSocket(candidate)
                _state.value = ConnectionState.Failed(
                    buildString {
                        append(exception.message ?: "TCP connection could not be established.")
                        if (closeFailure != null) {
                            append(" Socket close also failed: ")
                            append(closeFailure.message ?: "unknown I/O error")
                        }
                    },
                )
            } catch (exception: SecurityException) {
                closeSocket(candidate)?.let(exception::addSuppressed)
                _state.value = ConnectionState.Failed(
                    exception.message ?: "Android denied the TCP connection.",
                )
            }
        }
    }

    override suspend fun send(packet: CanPacket) {
        sendMutex.withLock {
            val activeSocket = socket
                ?: throw IOException("No CAN-TCP connection is active.")
            if (_state.value !is ConnectionState.Connected) {
                throw IOException("No CAN-TCP connection is active.")
            }

            try {
                withContext(Dispatchers.IO) {
                    activeSocket.getOutputStream().apply {
                        write(packet.toByteArray())
                        flush()
                    }
                }
            } catch (exception: IOException) {
                failConnection(activeSocket, exception.message ?: "TCP write failed.")
                throw exception
            }
        }
    }

    override suspend fun disconnect() {
        connectionMutex.withLock {
            val closeError = closeCurrentConnection()
            _state.value = closeError?.let {
                ConnectionState.Failed(it.message ?: "Failed to close the TCP connection.")
            } ?: ConnectionState.Disconnected
        }
    }

    override fun dispose() {
        val currentSocket = socket
        val currentReader = readerJob
        socket = null
        readerJob = null
        currentReader?.cancel()
        val closeError = currentSocket?.let(::closeSocket)
        packets.close()
        if (closeError != null) {
            _state.value = ConnectionState.Failed(
                closeError.message ?: "Failed to close the TCP connection.",
            )
        }
    }

    private suspend fun closeCurrentConnection(): IOException? = sendMutex.withLock {
        val currentSocket = socket
        val currentReader = readerJob
        socket = null
        readerJob = null
        val closeError = currentSocket?.let(::closeSocket)
        currentReader?.cancel()
        currentReader?.join()
        closeError
    }

    // A peer that loses power never closes the connection, so a waiting read would hang for hours.
    // Short TCP keepalive probes (answered by the peer's TCP stack, no CAN traffic) and a timeout for
    // unacknowledged sends report the loss after about five seconds.
    private fun enableLivenessCheck(candidate: Socket) {
        candidate.keepAlive = true
        try {
            ParcelFileDescriptor.fromSocket(candidate)?.use { descriptor ->
                val fd = descriptor.fileDescriptor
                Os.setsockoptInt(fd, OsConstants.IPPROTO_TCP, TCP_KEEPIDLE, KEEPALIVE_IDLE_S)
                Os.setsockoptInt(fd, OsConstants.IPPROTO_TCP, TCP_KEEPINTVL, KEEPALIVE_INTERVAL_S)
                Os.setsockoptInt(fd, OsConstants.IPPROTO_TCP, TCP_KEEPCNT, KEEPALIVE_PROBES)
                Os.setsockoptInt(fd, OsConstants.IPPROTO_TCP, OsConstants.TCP_USER_TIMEOUT, USER_TIMEOUT_MS)
            }
        } catch (exception: ErrnoException) {
            throw IOException("TCP keepalive could not be set: ${exception.message}", exception)
        }
    }

    private fun closeSocket(socket: Socket): IOException? {
        try {
            socket.close()
            return null
        } catch (exception: IOException) {
            return exception
        }
    }

    private fun failConnection(activeSocket: Socket, reason: String) {
        if (socket !== activeSocket) return
        socket = null
        _state.value = ConnectionState.Failed(reason)
        val closeError = closeSocket(activeSocket)
        if (closeError != null) {
            _state.value = ConnectionState.Failed(
                "$reason Socket close failed: ${closeError.message ?: "unknown I/O error"}",
            )
        }
    }

    private fun CoroutineScope.launchReader(activeSocket: Socket): Job = launch {
        val decoder = CanPacketStreamDecoder()
        val input = try {
            activeSocket.getInputStream()
        } catch (exception: IOException) {
            failConnection(activeSocket, exception.message ?: "Failed to open TCP input.")
            return@launch
        }
        val chunk = ByteArray(256)

        try {
            while (true) {
                currentCoroutineContext().ensureActive()
                val count = withContext(Dispatchers.IO) { input.read(chunk) }
                if (count < 0) throw EOFException("Remote CAN-TCP endpoint closed the connection.")
                for (packet in decoder.append(chunk, length = count)) {
                    packets.send(packet)
                }
            }
        } catch (exception: CancellationException) {
            throw exception
        } catch (exception: IOException) {
            failConnection(activeSocket, exception.message ?: "TCP read failed.")
        } catch (exception: IllegalArgumentException) {
            failConnection(activeSocket, "Invalid CAN-TCP packet: ${exception.message}")
        } finally {
            decoder.reset()
        }
    }

    companion object {
        private const val CONNECT_TIMEOUT_MS = 5_000

        // Linux socket option numbers; android.system.OsConstants has no names for them.
        private const val TCP_KEEPIDLE = 4
        private const val TCP_KEEPINTVL = 5
        private const val TCP_KEEPCNT = 6
        private const val KEEPALIVE_IDLE_S = 2
        private const val KEEPALIVE_INTERVAL_S = 1
        private const val KEEPALIVE_PROBES = 3
        private const val USER_TIMEOUT_MS = 5_000
    }
}
