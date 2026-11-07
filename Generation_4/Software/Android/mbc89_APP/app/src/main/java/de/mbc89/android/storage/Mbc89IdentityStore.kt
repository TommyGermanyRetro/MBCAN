package de.mbc89.android.storage

import android.content.Context
import androidx.datastore.core.DataStore
import androidx.datastore.preferences.core.Preferences
import androidx.datastore.preferences.core.booleanPreferencesKey
import androidx.datastore.preferences.core.edit
import androidx.datastore.preferences.core.intPreferencesKey
import androidx.datastore.preferences.core.stringPreferencesKey
import androidx.datastore.preferences.preferencesDataStore
import de.mbc89.android.core.Mbc89Core
import de.mbc89.android.core.Mbc89Memory
import de.mbc89.android.core.Mbc89SystemArrayLayout
import java.io.IOException
import java.security.SecureRandom
import java.util.Base64
import kotlinx.coroutines.flow.first

private val Context.identityDataStore by preferencesDataStore(name = "mbc89_identity")

class Mbc89IdentityStore(private val dataStore: DataStore<Preferences>) {
    constructor(context: Context) : this(context.identityDataStore)

    // Format version of the stored data: data without a version is the first format and is marked
    // as such; data written by a newer app version is not interpreted.
    suspend fun checkSchemaVersion() {
        dataStore.edit { preferences ->
            val stored = preferences[SCHEMA_VERSION_KEY]
            if (stored != null && stored > SCHEMA_VERSION) {
                throw IOException("Stored data come from a newer app version (format $stored).")
            }
            preferences[SCHEMA_VERSION_KEY] = SCHEMA_VERSION
        }
    }

    suspend fun loadGuid(): List<Int>? {
        val storedValue = dataStore.data.first()[GUID_KEY] ?: return null
        val guid = storedValue.split(',').map { value ->
            value.toIntOrNull()
                ?: throw IOException("Stored module identity contains an invalid byte.")
        }
        if (guid.size != GUID_LENGTH || guid.any { it !in 0..0xff }) {
            throw IOException("Stored module identity is incomplete or out of range.")
        }
        return guid
    }

    suspend fun loadSystemArray(): ByteArray? {
        val storedValue = dataStore.data.first()[SYSTEM_ARRAY_KEY] ?: return null
        val systemArray = try {
            Base64.getDecoder().decode(storedValue)
        } catch (exception: IllegalArgumentException) {
            throw IOException("Stored system array is not valid Base64.", exception)
        }
        if (systemArray.size != Mbc89SystemArrayLayout.SIZE) {
            throw IOException("Stored system array has an invalid length.")
        }
        return systemArray
    }

    // Serial in the mbc89 format (DS18B20 ROM, family 0x28, 6 bytes number, CRC) taken from the device:
    // Android no longer gives apps the hardware serial, so the 8 bytes of ANDROID_ID are used. Bytes 0-5
    // become the number, bytes 6 and 7 are folded into number bytes 0 and 1 by XOR (as the RP2040 MAC
    // from the flash ID). Without a usable ID the stored random serial is used.
    suspend fun loadDeviceSerial(androidId: String?): ByteArray =
        serialFromAndroidId(androidId) ?: loadOrCreateSyntheticSerial()

    suspend fun loadOrCreateSyntheticSerial(): ByteArray {
        var result: ByteArray? = null
        dataStore.edit { preferences ->
            val storedValue = preferences[SYNTHETIC_SERIAL_KEY]
            if (storedValue != null) {
                val serial = try {
                    Base64.getDecoder().decode(storedValue)
                } catch (exception: IllegalArgumentException) {
                    throw IOException("Stored synthetic serial is not valid Base64.", exception)
                }
                if (!isValidSystemArraySerial(serial)) {
                    throw IOException("Stored synthetic serial is invalid.")
                }
                result = serial
            } else {
                val serial = createSyntheticSystemArraySerial()
                preferences[SYNTHETIC_SERIAL_KEY] = Base64.getEncoder().encodeToString(serial)
                result = serial
            }
        }
        return result?.copyOf() ?: throw IOException("Synthetic serial could not be loaded.")
    }

    suspend fun saveGuid(guid: List<Int>?) {
        if (guid != null && (guid.size != GUID_LENGTH || guid.any { it !in 0..0xff })) {
            throw IllegalArgumentException("Module identity must contain four bytes.")
        }
        dataStore.edit { preferences ->
            if (guid == null) {
                preferences.remove(GUID_KEY)
            } else {
                preferences[GUID_KEY] = guid.joinToString(",")
            }
        }
    }

    suspend fun loadEndpoint(): Pair<String, Int>? {
        val preferences = dataStore.data.first()
        val host = preferences[ENDPOINT_HOST_KEY] ?: return null
        val port = preferences[ENDPOINT_PORT_KEY] ?: return null
        return host to port
    }

    suspend fun saveEndpoint(host: String, port: Int) {
        require(host.isNotBlank()) { "Host must not be blank." }
        require(port in 1..65535) { "TCP port must be between 1 and 65535." }
        dataStore.edit { preferences ->
            preferences[ENDPOINT_HOST_KEY] = host
            preferences[ENDPOINT_PORT_KEY] = port
        }
    }

    suspend fun loadKeyboardCount(): Int? {
        val count = dataStore.data.first()[KEYBOARD_COUNT_KEY] ?: return null
        if (count !in 0..Mbc89Core.KEYBOARD_COUNT_MAX) {
            throw IOException("Stored keyboard count is out of range.")
        }
        return count
    }

    suspend fun saveKeyboardCount(count: Int) {
        require(count in 0..Mbc89Core.KEYBOARD_COUNT_MAX) { "Keyboard count is out of range." }
        dataStore.edit { preferences ->
            preferences[KEYBOARD_COUNT_KEY] = count
        }
    }

    suspend fun loadControl80fCount(): Int? {
        val count = dataStore.data.first()[CONTROL80F_COUNT_KEY] ?: return null
        if (count !in 0..Mbc89Core.CONTROL80F_COUNT_MAX) {
            throw IOException("Stored Control 80f count is out of range.")
        }
        return count
    }

    suspend fun saveControl80fCount(count: Int) {
        require(count in 0..Mbc89Core.CONTROL80F_COUNT_MAX) { "Control 80f count is out of range." }
        dataStore.edit { preferences ->
            preferences[CONTROL80F_COUNT_KEY] = count
        }
    }

    suspend fun loadMemoryCount(): Int? {
        val count = dataStore.data.first()[MEMORY_COUNT_KEY] ?: return null
        if (count !in 0..Mbc89Memory.MEMORY_COUNT_MAX) {
            throw IOException("Stored Memory count is out of range.")
        }
        return count
    }

    suspend fun saveMemoryCount(count: Int) {
        require(count in 0..Mbc89Memory.MEMORY_COUNT_MAX) { "Memory count is out of range." }
        dataStore.edit { preferences ->
            preferences[MEMORY_COUNT_KEY] = count
        }
    }

    // One character per Memory ("0"/"1"); the former single setting applies to all Memory.
    suspend fun loadMemoryInterlock(): List<Boolean> {
        val preferences = dataStore.data.first()
        val text = preferences[MEMORY_INTERLOCKS_KEY]
        if (text == null || text.length != Mbc89Memory.MEMORY_COUNT_MAX) {
            val all = preferences[MEMORY_INTERLOCK_KEY] ?: false
            return List(Mbc89Memory.MEMORY_COUNT_MAX) { all }
        }
        return text.map { it == '1' }
    }

    suspend fun saveMemoryInterlock(enabled: List<Boolean>) {
        require(enabled.size == Mbc89Memory.MEMORY_COUNT_MAX) { "Memory interlocking list is incomplete." }
        dataStore.edit { preferences ->
            preferences[MEMORY_INTERLOCKS_KEY] = enabled.joinToString("") { if (it) "1" else "0" }
            preferences.remove(MEMORY_INTERLOCK_KEY)
        }
    }

    // s88 module of row A per Memory, comma separated (0 = none).
    suspend fun loadMemoryS88(): List<Int> {
        val modules = dataStore.data.first()[MEMORY_S88_KEY]?.split(",")?.map { it.toIntOrNull() }
        if (modules == null || modules.size != Mbc89Memory.MEMORY_COUNT_MAX ||
            modules.any { it == null || it !in 0..Mbc89Memory.S88_MODULE_MAX }
        ) {
            return List(Mbc89Memory.MEMORY_COUNT_MAX) { 0 }
        }
        return modules.map { it ?: 0 }
    }

    suspend fun saveMemoryS88(modules: List<Int>) {
        require(modules.size == Mbc89Memory.MEMORY_COUNT_MAX) { "s88 module list is incomplete." }
        require(modules.all { it in 0..Mbc89Memory.S88_MODULE_MAX }) { "s88 module is out of range." }
        dataStore.edit { preferences ->
            preferences[MEMORY_S88_KEY] = modules.joinToString(",")
        }
    }

    suspend fun loadMemoryState(): String? = dataStore.data.first()[MEMORY_STATE_KEY]

    suspend fun saveMemoryState(text: String) {
        dataStore.edit { preferences ->
            preferences[MEMORY_STATE_KEY] = text
        }
    }

    suspend fun loadMemory(): Mbc89Memory? {
        val text = dataStore.data.first()[MEMORY_ROUTES_KEY] ?: return null
        return try {
            Mbc89Memory.decode(text)
        } catch (exception: IllegalArgumentException) {
            throw IOException(exception.message ?: "Stored Memory routes are invalid.", exception)
        }
    }

    suspend fun saveMemory(memory: Mbc89Memory) {
        val text = memory.encode()
        dataStore.edit { preferences ->
            preferences[MEMORY_ROUTES_KEY] = text
        }
    }

    suspend fun saveModuleState(guid: List<Int>?, systemArray: ByteArray) {
        if (guid != null && (guid.size != GUID_LENGTH || guid.any { it !in 0..0xff })) {
            throw IllegalArgumentException("Module identity must contain four bytes.")
        }
        require(systemArray.size == Mbc89SystemArrayLayout.SIZE) {
            "System array must contain ${Mbc89SystemArrayLayout.SIZE} bytes."
        }
        val encoded = Base64.getEncoder().encodeToString(systemArray)
        dataStore.edit { preferences ->
            if (guid == null) {
                preferences.remove(GUID_KEY)
            } else {
                preferences[GUID_KEY] = guid.joinToString(",")
            }
            preferences[SYSTEM_ARRAY_KEY] = encoded
        }
    }

    suspend fun saveSystemArray(systemArray: ByteArray) {
        require(systemArray.size == Mbc89SystemArrayLayout.SIZE) {
            "System array must contain ${Mbc89SystemArrayLayout.SIZE} bytes."
        }
        val encoded = Base64.getEncoder().encodeToString(systemArray)
        dataStore.edit { preferences ->
            preferences[SYSTEM_ARRAY_KEY] = encoded
        }
    }

    companion object {
        private const val GUID_LENGTH = 4
        private const val SERIAL_LENGTH = 8
        private const val ONE_WIRE_ROM_LENGTH = 8
        private const val ONE_WIRE_FAMILY_DS18B20 = 0x28
        const val SCHEMA_VERSION = 1
        private val SCHEMA_VERSION_KEY = intPreferencesKey("schema_version")
        private val GUID_KEY = stringPreferencesKey("module_guid")
        private val SYSTEM_ARRAY_KEY = stringPreferencesKey("system_array")
        private val SYNTHETIC_SERIAL_KEY = stringPreferencesKey("synthetic_serial")
        private val KEYBOARD_COUNT_KEY = intPreferencesKey("keyboard_count")
        private val CONTROL80F_COUNT_KEY = intPreferencesKey("control80f_count")
        private val MEMORY_COUNT_KEY = intPreferencesKey("memory_count")
        private val MEMORY_INTERLOCK_KEY = booleanPreferencesKey("memory_interlock")
        private val MEMORY_INTERLOCKS_KEY = stringPreferencesKey("memory_interlocks")
        private val MEMORY_S88_KEY = stringPreferencesKey("memory_s88_modules")
        private val MEMORY_STATE_KEY = stringPreferencesKey("memory_state")
        private val MEMORY_ROUTES_KEY = stringPreferencesKey("memory_routes")
        private val ENDPOINT_HOST_KEY = stringPreferencesKey("endpoint_host")
        private val ENDPOINT_PORT_KEY = intPreferencesKey("endpoint_port")
        private val secureRandom = SecureRandom()

        fun serialFromAndroidId(androidId: String?): ByteArray? {
            val value = androidId?.trim()?.takeIf { it.length in 1..16 }?.toULongOrNull(16) ?: return null
            if (value == 0UL) return null
            val id = ByteArray(8) { index -> (value shr (56 - index * 8)).toByte() }
            val number = id.copyOfRange(0, 6)
            number[0] = (number[0].toInt() xor id[6].toInt()).toByte()
            number[1] = (number[1].toInt() xor id[7].toInt()).toByte()
            return oneWireSerial(number)
        }

        private fun oneWireSerial(number: ByteArray): ByteArray {
            val rom = ByteArray(ONE_WIRE_ROM_LENGTH)
            rom[0] = ONE_WIRE_FAMILY_DS18B20.toByte()
            number.copyInto(rom, destinationOffset = 1)
            rom[7] = oneWireCrc8(rom, length = 7).toByte()
            return rom.reversedArray()
        }

        private fun createSyntheticSystemArraySerial(): ByteArray {
            val serialNumber = ByteArray(6)
            secureRandom.nextBytes(serialNumber)
            return oneWireSerial(serialNumber)
        }

        private fun isValidSystemArraySerial(serial: ByteArray): Boolean {
            if (serial.size != SERIAL_LENGTH) return false
            val rom = serial.reversedArray()
            return (rom[0].toInt() and 0xff) == ONE_WIRE_FAMILY_DS18B20 &&
                (rom[7].toInt() and 0xff) == oneWireCrc8(rom, length = 7)
        }

        private fun oneWireCrc8(bytes: ByteArray, length: Int): Int {
            var crc = 0
            for (index in 0 until length) {
                var value = bytes[index].toInt() and 0xff
                repeat(Byte.SIZE_BITS) {
                    val mix = (crc xor value) and 1
                    crc = crc ushr 1
                    if (mix != 0) crc = crc xor 0x8c
                    value = value ushr 1
                }
            }
            return crc
        }
    }
}
