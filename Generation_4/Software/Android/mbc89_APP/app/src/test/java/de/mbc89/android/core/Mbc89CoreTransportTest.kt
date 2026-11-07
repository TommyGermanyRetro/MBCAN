package de.mbc89.android.core

import de.mbc89.android.transport.CanPacket
import de.mbc89.android.transport.CanTransport
import de.mbc89.android.transport.ConnectionState
import kotlinx.coroutines.channels.Channel
import kotlinx.coroutines.flow.Flow
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.asStateFlow
import kotlinx.coroutines.flow.collect
import kotlinx.coroutines.flow.receiveAsFlow
import kotlinx.coroutines.cancel
import kotlinx.coroutines.launch
import kotlinx.coroutines.runBlocking
import kotlinx.coroutines.withTimeout
import org.junit.Assert.assertEquals
import org.junit.Assert.assertTrue
import org.junit.Test

class Mbc89CoreTransportTest {
    @Test
    fun coreAcknowledgesReceivedConnect6021PacketThroughFakeTransport(): Unit = runBlocking {
        val transport = FakeCanTransport()
        val core = Mbc89Core()
        val collector = launch {
            transport.receivedPackets.collect { packet ->
                core.process(packet)?.let { transport.send(it) }
            }
        }
        val request = CanPacket.create(
            priority = 0,
            canId = 0x44,
            hashHigh = 0x03,
            hashLow = 0x00,
            data = byteArrayOf(1, 2, 3, 4, 0, 0),
        )

        try {
            transport.connect("fake", 15731)
            transport.emit(request)

            assertEquals(
                request.withCanId(0x45),
                withTimeout(TIMEOUT_MS) { transport.sentPackets.receive() },
            )
            assertEquals(ConnectionState.Connected("fake", 15731), transport.state.value)
            assertTrue(core.connect6021Streaming)
        } finally {
            collector.cancel()
            transport.dispose()
        }
    }

    private class FakeCanTransport : CanTransport {
        private val incoming = Channel<CanPacket>(Channel.UNLIMITED)
        private val outgoing = Channel<CanPacket>(Channel.UNLIMITED)
        private val mutableState = MutableStateFlow<ConnectionState>(ConnectionState.Disconnected)

        override val state: StateFlow<ConnectionState> = mutableState.asStateFlow()
        override val receivedPackets: Flow<CanPacket> = incoming.receiveAsFlow()
        val sentPackets: Channel<CanPacket> = outgoing

        override suspend fun connect(host: String, port: Int) {
            mutableState.value = ConnectionState.Connected(host, port)
        }

        override suspend fun send(packet: CanPacket) {
            outgoing.send(packet)
        }

        override suspend fun disconnect() {
            mutableState.value = ConnectionState.Disconnected
        }

        fun emit(packet: CanPacket) {
            incoming.trySend(packet).getOrThrow()
        }

        override fun dispose() {
            incoming.close()
            outgoing.close()
        }
    }

    companion object {
        private const val TIMEOUT_MS = 5_000L
    }
}
