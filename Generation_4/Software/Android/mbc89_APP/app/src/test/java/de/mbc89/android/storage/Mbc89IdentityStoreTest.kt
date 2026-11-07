package de.mbc89.android.storage

import androidx.datastore.preferences.core.PreferenceDataStoreFactory
import java.io.IOException
import org.junit.Assert.assertThrows
import kotlinx.coroutines.flow.first
import androidx.datastore.preferences.core.intPreferencesKey
import androidx.datastore.preferences.core.edit
import de.mbc89.android.core.Mbc89SystemArrayLayout
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.SupervisorJob
import kotlinx.coroutines.cancel
import kotlinx.coroutines.runBlocking
import org.junit.Assert.assertArrayEquals
import org.junit.Assert.assertEquals
import org.junit.Assert.assertNull
import org.junit.Rule
import org.junit.Test
import org.junit.rules.TemporaryFolder

class Mbc89IdentityStoreTest {
    @get:Rule
    val temporaryFolder = TemporaryFolder()

    @Test
    fun persistsGuidAndSystemArray() = runBlocking {
        val scope = CoroutineScope(SupervisorJob() + Dispatchers.IO)
        val file = temporaryFolder.newFile("identity.preferences_pb")
        val dataStore = PreferenceDataStoreFactory.create(
            scope = scope,
            produceFile = { file },
        )
        val identityStore = Mbc89IdentityStore(dataStore)
        val guid = listOf(0x22, 0x38, 0x39, 0x55)
        val systemArray = ByteArray(Mbc89SystemArrayLayout.SIZE).apply {
            this[700] = 0x6a
        }

        try {
            identityStore.saveGuid(guid)
            identityStore.saveSystemArray(systemArray)

            assertEquals(guid, identityStore.loadGuid())
            assertArrayEquals(systemArray, identityStore.loadSystemArray())
        } finally {
            scope.cancel()
        }
    }

    @Test
    fun createsAndPersistsAValidSyntheticOneWireSerial() = runBlocking {
        val scope = CoroutineScope(SupervisorJob() + Dispatchers.IO)
        val file = temporaryFolder.newFile("synthetic-identity.preferences_pb")
        val dataStore = PreferenceDataStoreFactory.create(
            scope = scope,
            produceFile = { file },
        )
        val identityStore = Mbc89IdentityStore(dataStore)

        try {
            val serial = identityStore.loadOrCreateSyntheticSerial()
            val loadedAgain = identityStore.loadOrCreateSyntheticSerial()
            val rom = serial.reversedArray()

            assertEquals(8, serial.size)
            assertEquals(0x28, rom[0].toInt() and 0xff)
            assertEquals(oneWireCrc8(rom, 7), rom[7].toInt() and 0xff)
            assertArrayEquals(serial, loadedAgain)
        } finally {
            scope.cancel()
        }
    }

    @Test
    fun savesGuidAndSystemArrayTogether() = runBlocking {
        val scope = CoroutineScope(SupervisorJob() + Dispatchers.IO)
        val file = temporaryFolder.newFile("module-state.preferences_pb")
        val dataStore = PreferenceDataStoreFactory.create(
            scope = scope,
            produceFile = { file },
        )
        val identityStore = Mbc89IdentityStore(dataStore)
        val systemArray = ByteArray(Mbc89SystemArrayLayout.SIZE).apply { this[22] = 0x41 }

        try {
            identityStore.saveModuleState(listOf(0x22, 0x38, 0x39, 0x55), systemArray)
            assertEquals(listOf(0x22, 0x38, 0x39, 0x55), identityStore.loadGuid())
            assertArrayEquals(systemArray, identityStore.loadSystemArray())

            identityStore.saveModuleState(null, systemArray)
            assertNull(identityStore.loadGuid())
        } finally {
            scope.cancel()
        }
    }

    @Test
    fun persistsEndpoint() = runBlocking {
        val scope = CoroutineScope(SupervisorJob() + Dispatchers.IO)
        val file = temporaryFolder.newFile("endpoint.preferences_pb")
        val dataStore = PreferenceDataStoreFactory.create(
            scope = scope,
            produceFile = { file },
        )
        val identityStore = Mbc89IdentityStore(dataStore)

        try {
            assertNull(identityStore.loadEndpoint())
            identityStore.saveEndpoint("192.168.202.221", 15731)
            assertEquals("192.168.202.221" to 15731, identityStore.loadEndpoint())
        } finally {
            scope.cancel()
        }
    }

    @Test
    fun persistsKeyboardCount() = runBlocking {
        val scope = CoroutineScope(SupervisorJob() + Dispatchers.IO)
        val file = temporaryFolder.newFile("keyboard.preferences_pb")
        val dataStore = PreferenceDataStoreFactory.create(
            scope = scope,
            produceFile = { file },
        )
        val identityStore = Mbc89IdentityStore(dataStore)

        try {
            assertNull(identityStore.loadKeyboardCount())
            identityStore.saveKeyboardCount(3)
            assertEquals(3, identityStore.loadKeyboardCount())
        } finally {
            scope.cancel()
        }
    }

    @Test
    fun persistsControl80fCount() = runBlocking {
        val scope = CoroutineScope(SupervisorJob() + Dispatchers.IO)
        val file = temporaryFolder.newFile("control80f.preferences_pb")
        val dataStore = PreferenceDataStoreFactory.create(
            scope = scope,
            produceFile = { file },
        )
        val identityStore = Mbc89IdentityStore(dataStore)

        try {
            assertNull(identityStore.loadControl80fCount())
            identityStore.saveControl80fCount(4)
            assertEquals(4, identityStore.loadControl80fCount())
        } finally {
            scope.cancel()
        }
    }

    private fun oneWireCrc8(bytes: ByteArray, length: Int): Int {
        var crc = 0
        for (index in 0 until length) {
            var value = bytes[index].toInt() and 0xff
            repeat(8) {
                val mix = (crc xor value) and 1
                crc = crc ushr 1
                if (mix != 0) crc = crc xor 0x8c
                value = value ushr 1
            }
        }
        return crc
    }

    @Test
    fun serialFromAndroidIdFoldsBytesSixAndSevenByXor() {
        val serial = Mbc89IdentityStore.serialFromAndroidId("0123456789abcdef")!!
        val rom = serial.reversedArray()

        assertEquals(0x28, rom[0].toInt() and 0xff)
        assertEquals(0x01 xor 0xcd, rom[1].toInt() and 0xff)
        assertEquals(0x23 xor 0xef, rom[2].toInt() and 0xff)
        assertEquals(listOf(0x45, 0x67, 0x89, 0xab), (3..6).map { rom[it].toInt() and 0xff })
        assertEquals(oneWireCrc8(rom, 7), rom[7].toInt() and 0xff)
        assertArrayEquals(serial, Mbc89IdentityStore.serialFromAndroidId("0123456789ABCDEF"))
        assertNull(Mbc89IdentityStore.serialFromAndroidId(null))
        assertNull(Mbc89IdentityStore.serialFromAndroidId("0"))
        assertNull(Mbc89IdentityStore.serialFromAndroidId("not hex"))
    }

    @Test
    fun marksTheSchemaVersionAndRejectsDataOfANewerApp() = runBlocking<Unit> {
        val scope = CoroutineScope(SupervisorJob() + Dispatchers.IO)
        val file = temporaryFolder.newFile("schema.preferences_pb")
        val dataStore = PreferenceDataStoreFactory.create(scope = scope, produceFile = { file })
        val identityStore = Mbc89IdentityStore(dataStore)

        try {
            identityStore.checkSchemaVersion()
            assertEquals(
                Mbc89IdentityStore.SCHEMA_VERSION,
                dataStore.data.first()[intPreferencesKey("schema_version")],
            )
            dataStore.edit { it[intPreferencesKey("schema_version")] = Mbc89IdentityStore.SCHEMA_VERSION + 1 }
            assertThrows(IOException::class.java) { runBlocking { identityStore.checkSchemaVersion() } }
        } finally {
            scope.cancel()
        }
    }
}
