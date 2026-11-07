package de.mbc89.android.core

import de.mbc89.android.transport.CanPacket
import org.junit.Assert.assertEquals
import org.junit.Assert.assertFalse
import org.junit.Assert.assertNull
import org.junit.Assert.assertTrue
import org.junit.Test

class Mbc89CorePersistenceTest {
    @Test
    fun initializesUnwrittenModuleLikeMbc89Utils() {
        val array = Mbc89Core().systemArraySnapshot

        assertEquals("000000000000", array.sliceArray(46..57).decodeToString())
        assertEquals(0x6d, array[Mbc89SystemArrayLayout.CS2_GROUP_OFFSET].toInt())
        assertEquals(listOf(0x00, 0x20), array.sliceArray(58..59).map { it.toInt() })
        assertEquals(List(4) { 0xff }, array.sliceArray(42..45).map { it.toInt() and 0xff })
        assertEquals("0x0000000000000001", array.sliceArray(22..39).decodeToString())
    }

    @Test
    fun keepsModuleNameFromParaCenterAcrossRestart() {
        val core = registeredCore()
        val name = "Tablet Stellwerk"
        transferStart(core)
        name.forEachIndexed { index, char -> core.process(arrayWrite(22 + index, char.code)) }
        (name.length until 20).forEach { core.process(arrayWrite(22 + it, 0x81)) }
        val commit = core.processDetailed(arrayWrite(0xfffd, 0x01))
        assertTrue(commit.persistSystemArray)

        val restarted = Mbc89Core(core.registeredGuid, core.systemArraySnapshot)

        assertEquals(name, restarted.systemArraySnapshot.sliceArray(22 until 22 + name.length).decodeToString())
        assertEquals(listOf(0x00, 0x20), restarted.systemArraySnapshot.sliceArray(58..59).map { it.toInt() })
    }

    @Test
    fun persistsGuidAndDatabaseWhenRegistrationCompletes() {
        val core = Mbc89Core()
        core.process(pcPacket(0x00, byteArrayOf(1, 2, 3, 4)))
        core.process(pcPacket(0x01, byteArrayOf(5, 6, 7, 8)))
        core.process(pcPacket(0x02, byteArrayOf(9, 10, 11, 12)))
        core.process(pcPacket(0x03, byteArrayOf(0, 0x22, 0, 0x20)))

        var response = core.process(pcPacket(0x04, byteArrayOf(0, 0, 0, 0)))
        var last: Mbc89CoreResult? = null
        for (step in 1..42) {
            val data = response!!.data
            last = core.processDetailed(pcPacket(0x05, byteArrayOf(0, 0x55, step.toByte(), data[7])))
            response = last.response
            if (step < 42) assertFalse(last.persistSystemArray)
        }

        assertTrue(last!!.persistSystemArray)
        assertTrue(core.isRegistered)
        assertEquals((1..12).toList(), core.systemArraySnapshot.sliceArray(46..57).map { it.toInt() })
    }

    @Test
    fun persistsOnlyChangedCs2GroupFromKenner() {
        val core = Mbc89Core()

        assertTrue(core.processDetailed(pcPacket(0x03, byteArrayOf(0, 0x22, 0, 0x20))).persistSystemArray)
        assertFalse(core.processDetailed(pcPacket(0x03, byteArrayOf(0, 0x22, 0, 0x20))).persistSystemArray)
        assertEquals(0x22, core.systemArraySnapshot[Mbc89SystemArrayLayout.CS2_GROUP_OFFSET].toInt())
    }

    @Test
    fun keepsPersistedGuidWhenOnlyTheDatabaseChanges() {
        val core = registeredCore()
        core.process(pcPacket(0x00, byteArrayOf(9, 9, 9, 9)))
        core.process(pcPacket(0x01, byteArrayOf(9, 9, 9, 9)))

        val result = core.processDetailed(pcPacket(0x02, byteArrayOf(9, 9, 9, 9)))

        assertFalse(core.isRegistered)
        assertFalse(result.persistSystemArray)
    }

    @Test
    fun blanksModuleOnParaCenterDeleteForOwnGuidOnly() {
        val core = registeredCore()
        assertFalse(core.processDetailed(pcPacket(0x08, byteArrayOf(1, 2, 3, 4))).persistSystemArray)
        assertTrue(core.isRegistered)

        val result = core.processDetailed(pcPacket(0x08, ownGuid))

        assertTrue(result.persistSystemArray)
        assertFalse(core.isRegistered)
        assertNull(core.registeredGuid)
        assertEquals("000000000000", core.systemArraySnapshot.sliceArray(46..57).decodeToString())
        assertEquals(List(4) { 0xff }, core.systemArraySnapshot.sliceArray(42..45).map { it.toInt() and 0xff })
    }

    @Test
    fun requestsRestartOnParaCenterResetForOwnGuidOnly() {
        val core = registeredCore()

        assertFalse(core.processDetailed(pcPacket(0x07, byteArrayOf(1, 2, 3, 4))).restartRequested)
        assertTrue(core.processDetailed(pcPacket(0x07, ownGuid)).restartRequested)
        assertFalse(core.processDetailed(pcPacket(0x09, ownGuid)).restartRequested)
    }

    @Test
    fun paraCenterSoftwareVersionWriteReachesPingAndPersistence() {
        val core = registeredCore()
        transferStart(core)
        core.process(arrayWrite(328, 1))
        core.process(arrayWrite(329, 1))
        val commit = core.processDetailed(arrayWrite(0xfffd, 0x0a))
        core.process(arrayWrite(0xffff, 0))

        assertTrue(commit.persistSystemArray)
        val ping = core.process(CanPacket.create(0, 0x30, 0, 0, ByteArray(0)))!!
        assertEquals(listOf(1, 1), ping.data.slice(4..5).map { it.toInt() })

        val restarted = Mbc89Core(core.registeredGuid, core.systemArraySnapshot)
        val pingAfterRestart = restarted.process(CanPacket.create(0, 0x30, 0, 0, ByteArray(0)))!!
        assertEquals(listOf(1, 1), pingAfterRestart.data.slice(4..5).map { it.toInt() })
    }

    @Test
    fun cs3SoftwareMinorWriteReachesPingAndPersistence() {
        val core = registeredCore()
        val write = CanPacket.create(0, 0x00, 0, 0, ownGuid + byteArrayOf(0x0b, 8, 0, 1))

        val result = core.processDetailed(write)

        assertTrue(result.persistSystemArray)
        assertEquals(1, result.response!!.data[6].toInt())
        val ping = core.process(CanPacket.create(0, 0x30, 0, 0, ByteArray(0)))!!
        assertEquals(listOf(1, 1), ping.data.slice(4..5).map { it.toInt() })
    }

    private val ownGuid = byteArrayOf(0x22, '8'.code.toByte(), '9'.code.toByte(), 0x55)

    private fun transferStart(core: Mbc89Core) {
        core.process(pcPacket(0x0b, ownGuid))
        core.process(arrayWrite(0xffff, 1))
    }

    private fun arrayWrite(index: Int, value: Int): CanPacket = pcPacket(
        0x0d,
        byteArrayOf(1, (index ushr 8).toByte(), index.toByte(), value.toByte()),
    )

    private fun pcPacket(command: Int, fields: ByteArray): CanPacket =
        CanPacket.create(
            priority = 0,
            canId = 0x16,
            hashHigh = 0x5f,
            hashLow = 0x38,
            data = byteArrayOf(0, 0, 0x18, command.toByte()) + fields,
        )

    private fun registeredCore(): Mbc89Core {
        val core = Mbc89Core()
        core.process(pcPacket(0x00, byteArrayOf(0, 0, 0, 0)))
        core.process(pcPacket(0x01, byteArrayOf(0, 0, 0, 0)))
        core.process(pcPacket(0x02, byteArrayOf(0, 0, 0, 0)))
        core.process(pcPacket(0x03, byteArrayOf(0, 0x22, 0, 0)))
        var response = core.process(pcPacket(0x04, byteArrayOf(0, 0, 0, 0)))
        for (step in 1..42) {
            val data = response!!.data
            response = core.process(pcPacket(0x05, byteArrayOf(0, 0x55, step.toByte(), data[7])))
        }
        assertTrue(core.isRegistered)
        return core
    }
}
