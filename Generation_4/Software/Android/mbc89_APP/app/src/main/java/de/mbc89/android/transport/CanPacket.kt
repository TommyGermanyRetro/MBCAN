package de.mbc89.android.transport

class CanPacket private constructor(private val bytes: ByteArray) {
    val priority: Int
        get() = bytes[PRIORITY_INDEX].toInt() and 0xff
    val canId: Int
        get() = bytes[CAN_ID_INDEX].toInt() and 0xff
    val hashHigh: Int
        get() = bytes[HASH_HIGH_INDEX].toInt() and 0xff
    val hashLow: Int
        get() = bytes[HASH_LOW_INDEX].toInt() and 0xff
    val dataLength: Int
        get() = bytes[DLC_INDEX].toInt() and 0xff
    val data: ByteArray
        get() = bytes.copyOfRange(DATA_OFFSET, DATA_OFFSET + dataLength)

    fun toByteArray(): ByteArray = bytes.copyOf()

    fun withCanId(canId: Int): CanPacket {
        require(canId in 0..0xff) { "CAN ID must be between 0 and 255." }
        val updated = bytes.copyOf()
        updated[CAN_ID_INDEX] = canId.toByte()
        return CanPacket(updated)
    }

    override fun equals(other: Any?): Boolean =
        other is CanPacket && bytes.contentEquals(other.bytes)

    override fun hashCode(): Int = bytes.contentHashCode()

    companion object {
        const val PACKET_SIZE = 13
        private const val PRIORITY_INDEX = 0
        private const val CAN_ID_INDEX = 1
        private const val HASH_HIGH_INDEX = 2
        private const val HASH_LOW_INDEX = 3
        private const val DLC_INDEX = 4
        private const val DATA_OFFSET = 5
        private const val MAX_DATA_LENGTH = 8

        fun fromBytes(bytes: ByteArray): CanPacket {
            require(bytes.size == PACKET_SIZE) {
                "CAN packet must be exactly $PACKET_SIZE bytes."
            }
            val dlc = bytes[DLC_INDEX].toInt() and 0xff
            require(dlc <= MAX_DATA_LENGTH) {
                "CAN packet DLC $dlc exceeds $MAX_DATA_LENGTH."
            }
            return CanPacket(bytes.copyOf())
        }

        fun create(
            priority: Int,
            canId: Int,
            hashHigh: Int,
            hashLow: Int,
            data: ByteArray,
        ): CanPacket {
            require(data.size <= MAX_DATA_LENGTH) {
                "CAN payload must contain at most $MAX_DATA_LENGTH bytes."
            }
            val packet = ByteArray(PACKET_SIZE)
            packet[PRIORITY_INDEX] = priority.toByteValue("priority")
            packet[CAN_ID_INDEX] = canId.toByteValue("CAN ID")
            packet[HASH_HIGH_INDEX] = hashHigh.toByteValue("hashHigh")
            packet[HASH_LOW_INDEX] = hashLow.toByteValue("hashLow")
            packet[DLC_INDEX] = data.size.toByte()
            data.copyInto(packet, destinationOffset = DATA_OFFSET)
            return CanPacket(packet)
        }

        private fun Int.toByteValue(field: String): Byte {
            require(this in 0..0xff) { "$field must be between 0 and 255." }
            return toByte()
        }
    }
}
