package de.mbc89.android.core

import de.mbc89.android.transport.CanPacket
import org.junit.Assert.assertEquals
import org.junit.Assert.assertFalse
import org.junit.Assert.assertNull
import org.junit.Assert.assertTrue
import org.junit.Test

class Mbc89CoreHandlingTest {
    private val guid = byteArrayOf(0x22, '8'.code.toByte(), '9'.code.toByte(), 0x55)
    private val guidList = listOf(0x22, '8'.code, '9'.code, 0x55)

    @Test
    fun acceptsValidConfigurationValueAndPersistsOnlyOnChange() {
        val core = Mbc89Core(guidList)

        val result = core.processDetailed(configurationWrite(channel = 10, value = 2))

        assertEquals(
            CanPacket.create(0, 0x01, 0x1b, 0x6d, guid + byteArrayOf(0x0b, 10, 1)),
            result.response,
        )
        assertTrue(result.persistSystemArray)
        assertEquals(2, core.systemArraySnapshot[Mbc89SystemArrayLayout.KEYBOARD_BASE_OFFSET].toInt())

        val unchanged = core.processDetailed(configurationWrite(channel = 10, value = 2))
        assertEquals(1, unchanged.response!!.data[6].toInt())
        assertFalse(unchanged.persistSystemArray)
    }

    @Test
    fun rejectsOutOfRangeConfigurationValuesWithoutWriting() {
        val core = Mbc89Core(guidList)
        val before = core.systemArraySnapshot
        val invalid = listOf(
            7 to 0,
            7 to 256,
            8 to 256,
            9 to 2,
            10 to 0,
            10 to 5,
            11 to 39,
            11 to 101,
            12 to 2,
            75 to 0x0101,
        )

        for ((channel, value) in invalid) {
            val result = core.processDetailed(configurationWrite(channel, value))
            assertEquals("channel $channel value $value", 0, result.response!!.data[6].toInt())
            assertEquals(channel, result.response!!.data[5].toInt() and 0xff)
            assertFalse(result.persistSystemArray)
        }
        assertTrue(before.contentEquals(core.systemArraySnapshot))
    }

    @Test
    fun acceptsRangeLimitsOfEveryConfigurationChannel() {
        val core = Mbc89Core(guidList)
        val valid = listOf(7 to 1, 7 to 255, 8 to 0, 8 to 255, 9 to 1, 9 to 0, 10 to 4, 11 to 40, 11 to 100, 12 to 1, 75 to 1)

        for ((channel, value) in valid) {
            assertEquals(
                "channel $channel value $value",
                1,
                core.process(configurationWrite(channel, value))!!.data[6].toInt(),
            )
        }
        assertEquals(1, core.systemArraySnapshot[Mbc89SystemArrayLayout.PROTOCOL_SETTINGS_OFFSET].toInt())
        assertEquals(1, core.systemArraySnapshot[Mbc89SystemArrayLayout.PROTOCOL_SETTINGS_OFFSET + 63].toInt())
    }

    @Test
    fun ignoresConfigurationWritesForUnknownChannelsForeignGuidOrUnregisteredModule() {
        val core = Mbc89Core(guidList)

        assertNull(core.process(configurationWrite(channel = 3, value = 1)))
        assertNull(core.process(configurationWrite(channel = 76, value = 1)))
        assertNull(core.process(configurationWrite(channel = 10, value = 2, target = ByteArray(4))))
        assertNull(Mbc89Core().process(configurationWrite(channel = 10, value = 2)))
    }

    @Test
    fun answersPingWithSoftwareVersionFromSystemArray() {
        val core = Mbc89Core(guidList)
        core.process(configurationWrite(channel = 7, value = 2))
        core.process(configurationWrite(channel = 8, value = 5))

        val ping = core.process(CanPacket.create(0, 0x30, 0, 0, ByteArray(0)))!!

        assertEquals(listOf(2, 5), ping.data.slice(4..5).map { it.toInt() })
    }

    @Test
    fun mappingChangeOnlyTouchesLocomotivesWithoutConnect6021Assignment() {
        val core = Mbc89Core(guidList)
        core.process(slotAssignment(slot = 5, locoGuid = byteArrayOf(0, 0, 0x40, 0x05)))

        assertFalse(core.locomotiveMapping(6).assigned)
        core.process(configurationWrite(channel = 9, value = 1))
        assertTrue(core.locomotiveMapping(6).assigned)
        assertTrue(core.locomotiveMapping(5).assigned)

        core.process(configurationWrite(channel = 9, value = 0))
        assertFalse(core.locomotiveMapping(6).assigned)
        assertTrue(core.locomotiveMapping(5).assigned)
    }

    @Test
    fun initializesLocomotiveDatabaseLikeCuInit() {
        val systemArray = ByteArray(Mbc89SystemArrayLayout.SIZE).apply {
            fill(0x7f.toByte(), Mbc89SystemArrayLayout.LOCOMOTIVE_OFFSET, Mbc89SystemArrayLayout.LOCOMOTIVE_OFFSET + 800)
            this[Mbc89SystemArrayLayout.MAPPING_OFFSET] = 1
        }
        val core = Mbc89Core(guidList, systemArray)

        assertEquals(LocomotiveState(0, true, false, 0, true), core.locomotiveState(12))
        assertEquals(listOf(0, 0, 0, 80), core.locomotiveMapping(0).guid)
        assertEquals(listOf(0, 0, 0, 12), core.locomotiveMapping(12).guid)
        val record = core.systemArraySnapshot.sliceArray(
            Mbc89SystemArrayLayout.LOCOMOTIVE_OFFSET + 120 until Mbc89SystemArrayLayout.LOCOMOTIVE_OFFSET + 130,
        )
        assertEquals(listOf(0, 1, 0, 0, 0, 0, 0, 12, 1, 0), record.map { it.toInt() })
    }

    @Test
    fun buildsLocomotiveCommandsOnlyForControllableChangedValuesAndAppliesAfterSend() {
        val core = Mbc89Core(guidList)
        val loco = byteArrayOf(0, 0, 0x40, 0x05)
        val speed = LocomotiveCommand.Speed(address = 5, step = 3)

        assertNull(core.buildLocomotiveCommand(speed))
        core.process(slotAssignment(slot = 5, locoGuid = loco))

        assertEquals(
            CanPacket.create(0, 0x08, 0x03, 0x00, loco + byteArrayOf(0x00, 0xcc.toByte())),
            core.buildLocomotiveCommand(speed),
        )
        assertEquals(0, core.locomotiveState(5).speed)
        core.markLocomotiveCommandSent(speed)
        assertEquals(3, core.locomotiveState(5).speed)
        assertNull(core.buildLocomotiveCommand(speed))

        val backward = LocomotiveCommand.Direction(address = 5, forward = false)
        assertEquals(
            CanPacket.create(0, 0x0a, 0x03, 0x00, loco + byteArrayOf(2)),
            core.buildLocomotiveCommand(backward),
        )
        core.markLocomotiveCommandSent(backward)
        assertFalse(core.locomotiveState(5).forward)

        val f2 = LocomotiveCommand.Function(address = 5, index = 2, on = true)
        assertEquals(
            CanPacket.create(0, 0x0c, 0x03, 0x00, loco + byteArrayOf(2, 1)),
            core.buildLocomotiveCommand(f2),
        )
        core.markLocomotiveCommandSent(f2)
        core.markLocomotiveCommandSent(LocomotiveCommand.Function(address = 5, index = 0, on = true))
        assertEquals(0b0010, core.locomotiveState(5).functions)
        assertTrue(core.locomotiveState(5).f0)
    }

    @Test
    fun blocksLocomotiveCommandsAndSyncWhileStopped() {
        val core = Mbc89Core(guidList)
        core.process(slotAssignment(slot = 5, locoGuid = byteArrayOf(0, 0, 0x40, 0x05)))
        core.process(CanPacket.create(0, 0x00, 0, 0, byteArrayOf(0, 0, 0, 0, 0)))

        assertNull(core.buildLocomotiveCommand(LocomotiveCommand.Speed(5, 1)))
        assertTrue(core.buildLocomotiveSync(5).isEmpty())
    }

    @Test
    fun buildsFullStateSyncForTakeover() {
        val core = Mbc89Core(guidList)
        val loco = byteArrayOf(0, 0, 0x40, 0x05)
        core.process(slotAssignment(slot = 5, locoGuid = loco))

        val sync = core.buildLocomotiveSync(5)

        assertEquals(listOf(0x08, 0x0a, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c), sync.map { it.canId })
        assertEquals(loco.toList() + listOf<Byte>(1), sync[1].data.toList())
        assertEquals((0..4).toList(), sync.drop(2).map { it.data[4].toInt() })
    }

    @Test
    fun answersLocomotiveQueriesFromTheDatabase() {
        val core = Mbc89Core(guidList)
        val loco = byteArrayOf(0, 0, 0x40, 0x05)
        core.process(slotAssignment(slot = 5, locoGuid = loco))
        core.markLocomotiveCommandSent(LocomotiveCommand.Speed(5, 2))
        core.markLocomotiveCommandSent(LocomotiveCommand.Function(5, 3, true))

        val direction = core.processDetailed(CanPacket.create(0, 0x0a, 0x12, 0x34, loco))
        assertEquals(2, direction.responses.size)
        assertEquals(1, direction.responses[0].data[4].toInt())
        assertEquals(
            CanPacket.create(0, 0x08, 0x12, 0x34, loco + byteArrayOf(0, 136.toByte())),
            direction.responses[1],
        )

        val function = core.process(CanPacket.create(0, 0x0c, 0x12, 0x34, loco + byteArrayOf(3)))
        assertEquals(1, function!!.data[5].toInt())

        val unknown = core.processDetailed(CanPacket.create(0, 0x08, 0, 0, byteArrayOf(0, 0, 0x40, 0x06)))
        assertTrue(unknown.responses.isEmpty())
    }

    @Test
    fun usesTheHashOfTheCs2Protocol() {
        val first = Mbc89Core(listOf(0x6d, '8'.code, '9'.code, 0x00)).buildSystemStopGo(go = true)
        val second = Mbc89Core(listOf(0x6d, '8'.code, '9'.code, 0x01)).buildSystemStopGo(go = true)
        val highBit = Mbc89Core(listOf(0x6d, 0x80, 0x00, 0x00)).buildSystemStopGo(go = true)

        assertEquals(0x57 to 0x38, first.hashHigh to first.hashLow)
        assertEquals(0x57 to 0x39, second.hashHigh to second.hashLow)
        assertEquals(0x6f to 0x00, highBit.hashHigh to highBit.hashLow)
    }

    @Test
    fun doesNotAnswerDriveCommandsOfOtherDevices() {
        val core = Mbc89Core(guidList)
        val loco = byteArrayOf(0, 0, 0x40, 0x05)
        core.process(slotAssignment(slot = 5, locoGuid = loco))
        core.markLocomotiveCommandSent(LocomotiveCommand.Speed(5, 2))

        assertTrue(core.processDetailed(CanPacket.create(0, 0x08, 0x03, 0x00, loco + byteArrayOf(0, 0))).responses.isEmpty())
        assertTrue(core.processDetailed(CanPacket.create(0, 0x0a, 0x03, 0x00, loco + byteArrayOf(2))).responses.isEmpty())
        assertTrue(core.processDetailed(CanPacket.create(0, 0x0c, 0x03, 0x00, loco + byteArrayOf(3, 1))).responses.isEmpty())
        assertEquals(1, core.processDetailed(CanPacket.create(0, 0x08, 0x03, 0x00, loco)).responses.size)
    }

    private fun configurationWrite(channel: Int, value: Int, target: ByteArray = guid): CanPacket =
        CanPacket.create(
            priority = 0,
            canId = 0x00,
            hashHigh = 0,
            hashLow = 0,
            data = target + byteArrayOf(0x0b, channel.toByte(), (value ushr 8).toByte(), value.toByte()),
        )

    private fun slotAssignment(slot: Int, locoGuid: ByteArray): CanPacket =
        CanPacket.create(0, 0x44, 0x03, 0x00, locoGuid + byteArrayOf(slot.toByte()))
}
