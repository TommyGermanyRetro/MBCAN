package de.mbc89.android.transport

class CanPacketStreamDecoder {
    private val packetBuffer = ByteArray(CanPacket.PACKET_SIZE)
    private var bufferedBytes = 0

    val pendingByteCount: Int
        get() = bufferedBytes

    fun append(input: ByteArray, offset: Int = 0, length: Int = input.size - offset): List<CanPacket> {
        require(offset >= 0 && length >= 0 && offset <= input.size - length) {
            "Input range is outside the supplied byte array."
        }

        val packets = ArrayList<CanPacket>()
        var sourceOffset = offset
        var remaining = length

        while (remaining > 0) {
            val copyLength = minOf(CanPacket.PACKET_SIZE - bufferedBytes, remaining)
            input.copyInto(
                destination = packetBuffer,
                destinationOffset = bufferedBytes,
                startIndex = sourceOffset,
                endIndex = sourceOffset + copyLength,
            )
            bufferedBytes += copyLength
            sourceOffset += copyLength
            remaining -= copyLength

            if (bufferedBytes == CanPacket.PACKET_SIZE) {
                packets += CanPacket.fromBytes(packetBuffer)
                packetBuffer.fill(0)
                bufferedBytes = 0
            }
        }
        return packets
    }

    fun reset() {
        packetBuffer.fill(0)
        bufferedBytes = 0
    }
}
