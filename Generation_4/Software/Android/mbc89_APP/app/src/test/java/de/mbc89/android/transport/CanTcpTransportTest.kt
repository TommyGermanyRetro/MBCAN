package de.mbc89.android.transport

import java.net.ServerSocket
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.async
import kotlinx.coroutines.cancelAndJoin
import kotlinx.coroutines.flow.first
import kotlinx.coroutines.runBlocking
import kotlinx.coroutines.withTimeout
import org.junit.Assert.assertEquals
import org.junit.Assert.assertTrue
import org.junit.Test

class CanTcpTransportTest {
    @Test
    fun discardsIncompletePacketWhenConnectionClosesAndReconnectsCleanly(): Unit = runBlocking {
        val expected = samplePacket()
        val server = ServerSocket(0)
        val serverJob = async(Dispatchers.IO) {
            server.accept().use { client ->
                client.getOutputStream().apply {
                    write(expected.toByteArray(), 0, 7)
                    flush()
                }
            }
            server.accept().use { client ->
                client.getOutputStream().apply {
                    write(expected.toByteArray())
                    flush()
                }
            }
        }
        val transport = CanTcpTransport(this)

        try {
            transport.connect("127.0.0.1", server.localPort)
            val failedState = withTimeout(TIMEOUT_MS) {
                transport.state.first { it is ConnectionState.Failed }
            }
            assertTrue(
                (failedState as ConnectionState.Failed).reason.contains("closed the connection"),
            )

            transport.connect("127.0.0.1", server.localPort)
            val received = withTimeout(TIMEOUT_MS) {
                transport.receivedPackets.first()
            }

            assertEquals(expected, received)
            serverJob.await()
        } finally {
            transport.dispose()
            server.close()
            serverJob.cancelAndJoin()
        }
    }

    private fun samplePacket(): CanPacket =
        CanPacket.create(
            priority = 0,
            canId = 0x44,
            hashHigh = 0x03,
            hashLow = 0x00,
            data = byteArrayOf(0x08, 0x01, 0x02, 0x03),
        )

    companion object {
        private const val TIMEOUT_MS = 5_000L
    }
}
