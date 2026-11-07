package de.mbc89.android.transport

import org.junit.Assert.assertEquals
import org.junit.Assert.assertThrows
import org.junit.Test

class CanPacketStreamDecoderTest {
    @Test
    fun preservesDocumentedPcAliveFrameByteForByte() {
        val documentedFrame = byteArrayOf(
            0x00,
            0x16,
            0x5f,
            0x38,
            0x08,
            0x00,
            0x00,
            0x18,
            0x09,
            0x6d,
            0x38,
            0x34,
            0x01,
        )

        assertEquals(documentedFrame.toList(), CanPacket.fromBytes(documentedFrame).toByteArray().toList())
    }

    @Test
    fun decodesPacketSplitAcrossReads() {
        val expected = samplePacket()
        val encoded = expected.toByteArray()
        val decoder = CanPacketStreamDecoder()

        assertEquals(emptyList<CanPacket>(), decoder.append(encoded, length = 4))
        assertEquals(4, decoder.pendingByteCount)
        assertEquals(listOf(expected), decoder.append(encoded, offset = 4, length = 9))
        assertEquals(0, decoder.pendingByteCount)
    }

    @Test
    fun decodesSeveralPacketsFromOneRead() {
        val first = samplePacket()
        val second = CanPacket.create(1, 0x45, 0x12, 0x34, byteArrayOf(0x55, 0x66))
        val combined = first.toByteArray() + second.toByteArray()

        assertEquals(
            listOf(first, second),
            CanPacketStreamDecoder().append(combined),
        )
    }

    @Test
    fun rejectsInvalidDlc() {
        val invalid = samplePacket().toByteArray().apply { this[4] = 9 }

        assertThrows(IllegalArgumentException::class.java) {
            CanPacketStreamDecoder().append(invalid)
        }
    }

    @Test
    fun resetDiscardsIncompletePacket() {
        val decoder = CanPacketStreamDecoder()
        decoder.append(samplePacket().toByteArray(), length = 7)
        decoder.reset()

        assertEquals(0, decoder.pendingByteCount)
        assertEquals(
            listOf(samplePacket()),
            decoder.append(samplePacket().toByteArray()),
        )
    }

    private fun samplePacket(): CanPacket =
        CanPacket.create(
            priority = 0,
            canId = 0x44,
            hashHigh = 0x03,
            hashLow = 0x00,
            data = byteArrayOf(0x08, 0x01, 0x02, 0x03),
        )
}
