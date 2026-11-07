package de.mbc89.android.core

import de.mbc89.android.transport.CanPacket

data class LocomotiveState(
    val speed: Int,
    val forward: Boolean,
    val f0: Boolean,
    val functions: Int,
    val controllable: Boolean,
)

sealed interface LocomotiveCommand {
    val address: Int

    data class Speed(override val address: Int, val step: Int) : LocomotiveCommand
    data class Direction(override val address: Int, val forward: Boolean) : LocomotiveCommand
    data class Function(override val address: Int, val index: Int, val on: Boolean) : LocomotiveCommand
}

// Loco database inside the system array (Mbc89Com::lokAt(): 80 records of 10 bytes from offset
// MBC_89_S_LOKS); address 80 is stored in record 0.
class Mbc89Locomotives(private val array: ByteArray) {
    fun reset(mapping: Int) {
        for (index in 0 until COUNT) {
            val base = recordOffset(index)
            array[base + SPEED] = 0
            array[base + DIR] = DIR_FORWARD.toByte()
            array[base + F0] = 0
            array[base + FX] = 0
            array[base + ADDR4] = 0
            array[base + ADDR3] = 0
            array[base + ADDR2] = 0
            array[base + ADDR1] = index.toByte()
            array[base + MAPPED] = mapping.toByte()
            array[base + INFRA] = 0
        }
        array[recordOffset(0) + ADDR1] = SLOT_80.toByte()
    }

    fun resetAddresses() {
        for (index in 0 until COUNT) {
            val base = recordOffset(index)
            array[base + ADDR4] = 0
            array[base + ADDR3] = 0
            array[base + ADDR2] = 0
            array[base + ADDR1] = index.toByte()
        }
        array[recordOffset(0) + ADDR1] = SLOT_80.toByte()
    }

    fun assign(slot: Int, guid: ByteArray) {
        val base = recordOffset(if (slot == SLOT_80) 0 else slot)
        guid.copyInto(array, destinationOffset = base + ADDR4, endIndex = GUID_LENGTH)
        array[base + MAPPED] = (MAPPED_ON or MAPPED_ECHT).toByte()
    }

    fun applyMapping(mapping: Int) {
        for (index in 0 until COUNT) {
            val base = recordOffset(index)
            if (byte(base + MAPPED) and MAPPED_ECHT == 0) array[base + MAPPED] = mapping.toByte()
        }
    }

    fun mapping(slot: Int): LocomotiveMapping {
        val base = recordOffset(slot)
        return LocomotiveMapping(
            guid = (0 until GUID_LENGTH).map { byte(base + ADDR4 + it) },
            assigned = byte(base + MAPPED) != 0,
        )
    }

    fun state(address: Int): LocomotiveState {
        val base = addressOffset(address)
        return LocomotiveState(
            speed = byte(base + SPEED),
            forward = byte(base + DIR) == DIR_FORWARD,
            f0 = byte(base + F0) != 0,
            functions = byte(base + FX),
            controllable = isControllable(base),
        )
    }

    fun buildCommand(command: LocomotiveCommand): CanPacket? {
        val base = addressOffset(command.address)
        if (!isControllable(base)) return null
        return when (command) {
            is LocomotiveCommand.Speed -> {
                require(command.step in 0..MAX_SPEED_STEP) { "Speed step is out of range." }
                if (command.step == byte(base + SPEED)) null else speedPacket(base, command.step)
            }
            is LocomotiveCommand.Direction -> {
                val dir = directionValue(command.forward)
                if (dir == byte(base + DIR)) null else directionPacket(base, dir)
            }
            is LocomotiveCommand.Function -> {
                require(command.index in 0..FUNCTION_MAX) { "Function index is out of range." }
                if (command.on == functionOn(base, command.index)) {
                    null
                } else {
                    functionPacket(base, command.index, command.on)
                }
            }
        }
    }

    fun markCommandSent(command: LocomotiveCommand) {
        val base = addressOffset(command.address)
        when (command) {
            is LocomotiveCommand.Speed -> array[base + SPEED] = command.step.toByte()
            is LocomotiveCommand.Direction -> array[base + DIR] = directionValue(command.forward).toByte()
            is LocomotiveCommand.Function -> if (command.index == 0) {
                array[base + F0] = if (command.on) 1 else 0
            } else {
                val bit = 1 shl (command.index - 1)
                val fx = byte(base + FX)
                array[base + FX] = (if (command.on) fx or bit else fx and bit.inv()).toByte()
            }
        }
    }

    fun buildSync(address: Int): List<CanPacket> {
        val base = addressOffset(address)
        if (!isControllable(base)) return emptyList()
        return listOf(
            speedPacket(base, byte(base + SPEED)),
            directionPacket(base, byte(base + DIR)),
        ) + (0..FUNCTION_MAX).map { index -> functionPacket(base, index, functionOn(base, index)) }
    }

    fun answerQuery(packet: CanPacket): List<CanPacket> {
        val canId = packet.canId
        if (canId != CAN_ID_SPEED && canId != CAN_ID_DIRECTION && canId != CAN_ID_FUNCTION) {
            return emptyList()
        }
        // Only real queries are answered (cs2CAN: speed/direction DLC 4, function DLC 5). A drive command
        // of another device must not be answered: the answer carries this module's own state and would
        // act as a new command, two modules would answer each other endlessly.
        val queryDlc = if (canId == CAN_ID_FUNCTION) FUNCTION_QUERY_DLC else ADDRESS_QUERY_DLC
        if (packet.dataLength != queryDlc) return emptyList()
        val request = packet.toByteArray()
        val responses = ArrayList<CanPacket>()
        for (index in 0 until COUNT) {
            val base = recordOffset(index)
            val matches = (0 until GUID_LENGTH).all {
                array[base + ADDR4 + it] == request[PACKET_DATA + it]
            } && byte(base + MAPPED) != 0
            if (!matches) continue

            val response = request.copyOf()
            when (canId) {
                CAN_ID_DIRECTION -> {
                    response[PACKET_DLC] = DIRECTION_DLC.toByte()
                    response[PACKET_DATA + 4] = array[base + DIR]
                    responses += CanPacket.fromBytes(response.copyOf())
                    val speed = speedValue(byte(base + SPEED))
                    response[PACKET_ID] = CAN_ID_SPEED.toByte()
                    response[PACKET_DLC] = SPEED_DLC.toByte()
                    response[PACKET_DATA + 4] = (speed ushr 8).toByte()
                    response[PACKET_DATA + 5] = speed.toByte()
                    responses += CanPacket.fromBytes(response)
                }
                CAN_ID_FUNCTION -> {
                    val function = request[PACKET_DATA + 4].toInt() and 0xff
                    response[PACKET_DLC] = FUNCTION_DLC.toByte()
                    if (function in 0..FUNCTION_MAX) {
                        response[PACKET_DATA + 5] = if (functionOn(base, function)) 1 else 0
                    }
                    responses += CanPacket.fromBytes(response)
                }
                else -> {
                    val speed = speedValue(byte(base + SPEED))
                    response[PACKET_DLC] = SPEED_DLC.toByte()
                    response[PACKET_DATA + 4] = (speed ushr 8).toByte()
                    response[PACKET_DATA + 5] = speed.toByte()
                    responses += CanPacket.fromBytes(response)
                }
            }
        }
        return responses
    }

    private fun isControllable(base: Int): Boolean =
        byte(base + MAPPED) != 0 && byte(base + INFRA) == 0

    private fun functionOn(base: Int, index: Int): Boolean =
        if (index == 0) byte(base + F0) != 0 else byte(base + FX) and (1 shl (index - 1)) != 0

    private fun speedPacket(base: Int, step: Int): CanPacket {
        val speed = speedValue(step)
        return command(CAN_ID_SPEED, base, byteArrayOf((speed ushr 8).toByte(), speed.toByte()))
    }

    private fun directionPacket(base: Int, dir: Int): CanPacket =
        command(CAN_ID_DIRECTION, base, byteArrayOf(dir.toByte()))

    private fun functionPacket(base: Int, index: Int, on: Boolean): CanPacket =
        command(CAN_ID_FUNCTION, base, byteArrayOf(index.toByte(), if (on) 1 else 0))

    private fun command(canId: Int, base: Int, payload: ByteArray): CanPacket =
        CanPacket.create(
            priority = 0,
            canId = canId,
            hashHigh = COMMAND_HASH_HIGH,
            hashLow = COMMAND_HASH_LOW,
            data = array.copyOfRange(base + ADDR4, base + ADDR4 + GUID_LENGTH) + payload,
        )

    private fun directionValue(forward: Boolean): Int = if (forward) DIR_FORWARD else DIR_BACKWARD

    // Linear like the MSX port (user decision for digital controls) instead of the potentiometer
    // mapping (step - 1) * 68 of Mbc89Com.
    private fun speedValue(step: Int): Int = step * MM_SPEED_STEP

    private fun addressOffset(address: Int): Int {
        require(address in 1..SLOT_80) { "Locomotive address is out of range." }
        return recordOffset(if (address == SLOT_80) 0 else address)
    }

    private fun recordOffset(index: Int): Int {
        require(index in 0 until COUNT) { "Locomotive slot is out of range." }
        return Mbc89SystemArrayLayout.LOCOMOTIVE_OFFSET + index * RECORD_LENGTH
    }

    private fun byte(offset: Int): Int = array[offset].toInt() and 0xff

    companion object {
        const val COUNT = 80
        const val SLOT_80 = 80
        const val MAX_SPEED_STEP = 15
        const val FUNCTION_MAX = 4
        const val CAN_ID_SPEED = 0x08
        const val CAN_ID_DIRECTION = 0x0a
        const val CAN_ID_FUNCTION = 0x0c
        private const val RECORD_LENGTH = 10
        private const val SPEED = 0
        private const val DIR = 1
        private const val F0 = 2
        private const val FX = 3
        private const val ADDR4 = 4
        private const val ADDR3 = 5
        private const val ADDR2 = 6
        private const val ADDR1 = 7
        private const val MAPPED = 8
        private const val INFRA = 9
        private const val MAPPED_ON = 0x01
        private const val MAPPED_ECHT = 0x02
        private const val DIR_FORWARD = 1
        private const val DIR_BACKWARD = 2
        private const val MM_SPEED_STEP = 68
        private const val GUID_LENGTH = 4
        private const val SPEED_DLC = 6
        private const val DIRECTION_DLC = 5
        private const val FUNCTION_DLC = 6
        private const val ADDRESS_QUERY_DLC = 4
        private const val FUNCTION_QUERY_DLC = 5
        private const val COMMAND_HASH_HIGH = 0x03
        private const val COMMAND_HASH_LOW = 0x00
        private const val PACKET_ID = 1
        private const val PACKET_DLC = 4
        private const val PACKET_DATA = 5
    }
}
