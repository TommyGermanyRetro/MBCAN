package de.mbc89.android.core

import de.mbc89.android.transport.CanPacket

data class LocomotiveMapping(
    val guid: List<Int>,
    val assigned: Boolean,
)

data class S88Contact(val pin: Int, val occupied: Boolean)

data class Mbc89CoreResult(
    val responses: List<CanPacket>,
    val persistSystemArray: Boolean,
    val restartRequested: Boolean = false,
    val switchedAccessory: MemoryCommand? = null,
    val s88Contact: S88Contact? = null,
) {
    val response: CanPacket?
        get() = responses.firstOrNull()
}

class Mbc89Core(
    initialGuid: List<Int>? = null,
    initialSystemArray: ByteArray? = null,
    syntheticSerial: ByteArray = DEFAULT_SYNTHETIC_SERIAL,
    keyboardCount: Int = DEFAULT_KEYBOARD_COUNT,
) {
    private val syntheticSerial = syntheticSerial.copyOf().also {
        require(it.size == SERIAL_LENGTH) {
            "Synthetic serial must contain $SERIAL_LENGTH bytes."
        }
    }
    private val moduleInfo = MODULE_INFO_TEMPLATE.copyOf()
    private val systemArray = ByteArray(Mbc89SystemArrayLayout.SIZE).apply {
        moduleInfo.copyInto(this)
        initialSystemArray?.also {
            require(it.size == size) { "System array must contain $size bytes." }
            it.copyInto(this)
        } ?: apply {
            fill(
                DATABASE_EMPTY_DIGIT,
                Mbc89SystemArrayLayout.DATABASE_OFFSET,
                Mbc89SystemArrayLayout.DATABASE_OFFSET + DATABASE_LENGTH,
            )
            this[Mbc89SystemArrayLayout.CS2_GROUP_OFFSET] = DEFAULT_CS2_GROUP.toByte()
            this[Mbc89SystemArrayLayout.MODULE_EXTENSION_OFFSET] = 1
            this[Mbc89SystemArrayLayout.MODULE_EXTENSION_OFFSET + 1] = 0
            this[Mbc89SystemArrayLayout.KEYBOARD_BASE_OFFSET] = 1
            this[Mbc89SystemArrayLayout.I2C_FREQUENCY_OFFSET] = 40
        }
        this[Mbc89SystemArrayLayout.KENNUNG_OFFSET] = FIXED_CONNECT6021_KENNUNG_HIGH.toByte()
        this[Mbc89SystemArrayLayout.KENNUNG_OFFSET + 1] = FIXED_CONNECT6021_KENNUNG_LOW.toByte()
    }
    private val locomotives = Mbc89Locomotives(systemArray)
    private val databaseBytes = ByteArray(DATABASE_LENGTH)
    private var databasePartsReceived = 0
    private var databaseSnapshot: ByteArray? = null
    private var databaseValid = false
    private var registrationInProgress = false
    private var registrationStep = 0
    private var registrationInfoIndex = 0
    private var arrayTransferActive = false
    private var persistSystemArrayRequested = false
    private var restartRequested = false
    private var switchedAccessory: MemoryCommand? = null
    private var s88Contact: S88Contact? = null
    private var cs2Group = initialGuid?.firstOrNull() ?: 0
    private var moduleGuid: List<Int>? = initialGuid?.also {
        require(it.size == GUID_LENGTH && it.all { byte -> byte in 0..0xff }) {
            "Module identity must contain four bytes."
        }
    }?.toList()

    init {
        applyModuleIdentity(keepStoredName = initialSystemArray != null)
        applySimulatedMeasurements()
        systemArray.copyInto(moduleInfo, endIndex = moduleInfo.size)
        updateGuidInSystemArray(initialGuid)
        moduleInfo[GUID_OFFSET] = systemArray[GUID_OFFSET]
        cs2Group = initialGuid?.firstOrNull()
            ?: (systemArray[Mbc89SystemArrayLayout.CS2_GROUP_OFFSET].toInt() and 0xff)
        systemArray.copyInto(
            databaseBytes,
            destinationOffset = 0,
            startIndex = Mbc89SystemArrayLayout.DATABASE_OFFSET,
            endIndex = Mbc89SystemArrayLayout.DATABASE_OFFSET + DATABASE_LENGTH,
        )
        databaseSnapshot = initialSystemArray?.let { databaseBytes.copyOf() }
        locomotives.reset(byte(Mbc89SystemArrayLayout.MAPPING_OFFSET))
    }

    // Name the module got from a serial ("0x" + 16 hex digits + 0x81 0x81); a name given by the user
    // is kept when the serial changes.
    private fun isGeneratedName(): Boolean {
        val stored = systemArray.copyOfRange(NAME_OFFSET, NAME_OFFSET + GENERATED_NAME_LENGTH)
        val text = stored.copyOfRange(0, GENERATED_NAME_LENGTH - 2).decodeToString()
        return text.startsWith("0x") && text.drop(2).all { it in '0'..'9' || it in 'A'..'F' } &&
            stored[GENERATED_NAME_LENGTH - 2] == 0x81.toByte() && stored[GENERATED_NAME_LENGTH - 1] == 0x81.toByte()
    }

    private fun applyModuleIdentity(keepStoredName: Boolean) {
        syntheticSerial.copyInto(systemArray, destinationOffset = SERIAL_OFFSET)
        ARTICLE_NUMBER.encodeToByteArray().copyInto(systemArray, destinationOffset = ARTICLE_OFFSET)
        SOFTWARE_VERSION.encodeToByteArray().copyInto(systemArray, destinationOffset = SOFTWARE_OFFSET)
        HARDWARE_VERSION.encodeToByteArray().copyInto(systemArray, destinationOffset = HARDWARE_OFFSET)
        val name = "0x${syntheticSerial.joinToString("") { "%02X".format(it.toInt() and 0xff) }}"
            .encodeToByteArray() + byteArrayOf(0x81.toByte(), 0x81.toByte())
        if (!keepStoredName || isGeneratedName()) name.copyInto(systemArray, destinationOffset = NAME_OFFSET)
    }

    private fun applySimulatedMeasurements() {
        SIMULATED_VOLTAGE.encodeToByteArray().copyInto(
            systemArray,
            destinationOffset = Mbc89SystemArrayLayout.SUPPLY_VOLTAGE_OFFSET,
        )
        SIMULATED_TEMPERATURE.encodeToByteArray().copyInto(
            systemArray,
            destinationOffset = Mbc89SystemArrayLayout.TEMPERATURE_OFFSET,
        )
    }

    var connect6021Streaming: Boolean = false
        private set
    private var foreignStreamOpen = false
    var cs2Connected: Boolean = false
        private set
    var systemStopped: Boolean = false
        private set
    var keyboardCount: Int = DEFAULT_KEYBOARD_COUNT
        set(value) {
            require(value in 0..KEYBOARD_COUNT_MAX) { "Keyboard count is out of range." }
            field = value
        }
    private val keyboardStates = IntArray(KEYBOARD_COUNT_MAX)

    init {
        this.keyboardCount = keyboardCount
    }

    fun keyboardState(kbIndex: Int): Int {
        require(kbIndex in 0 until KEYBOARD_COUNT_MAX) { "Keyboard index is out of range." }
        return keyboardStates[kbIndex]
    }

    fun buildKeyboardSwitch(kbIndex: Int, column: Int, green: Boolean): CanPacket? {
        require(kbIndex in 0 until keyboardCount) { "Keyboard index is out of range." }
        require(column in 0 until KEYBOARD_COLUMNS) { "Keyboard column is out of range." }
        return buildAccessorySwitch(keyboardAddress(kbIndex, column), green)
    }

    // switchTime in 10 ms units; 0 sends the 6-byte form with the command station's default time.
    fun buildAccessorySwitch(address: Int, green: Boolean, switchTime: Int = 0): CanPacket? {
        require(address in 0..0xffff) { "Accessory address is out of range." }
        require(switchTime in 0..0xffff) { "Switch time is out of range." }
        if (systemStopped) return null
        val head = byteArrayOf(
            0,
            0,
            ((address ushr 8) and 0xff).toByte(),
            (address and 0xff).toByte(),
            if (green) 1 else 0,
            ACCESSORY_POWER_ON,
        )
        val data = if (switchTime == 0) {
            head
        } else {
            head + byteArrayOf(((switchTime ushr 8) and 0xff).toByte(), (switchTime and 0xff).toByte())
        }
        return CanPacket.create(
            priority = 0,
            canId = CAN_ID_MBCAN_ACCESSORY,
            hashHigh = ACCESSORY_HASH_HIGH,
            hashLow = ACCESSORY_HASH_LOW,
            data = data,
        )
    }

    fun markKeyboardSwitchSent(kbIndex: Int, column: Int, green: Boolean) {
        applyKeyboardState(kbIndex, column, green)
    }

    // Like the firmware (Mbc89Com cuI2cCheckFn): every group of 4 keys switches MM (0x3000) or DCC
    // (0x3800) as set in the protocol array (ParaCenter / CS configuration channels).
    fun keyboardAddress(kbIndex: Int, column: Int): Int {
        val dcc = keyboardUsesDcc(systemArray, kbIndex, column / KEYS_PER_GROUP)
        val base = if (dcc) ACCESSORY_DCC_BASE else ACCESSORY_MM_BASE
        return base + basisOffset() + kbIndex * KEYBOARD_COLUMNS + column
    }

    fun markAccessorySent(address: Int, green: Boolean) {
        val offset = keyboardOffset(address) ?: return
        applyKeyboardState(offset / KEYBOARD_COLUMNS, offset % KEYBOARD_COLUMNS, green)
    }

    private fun basisOffset(): Int {
        val basis = (systemArray[Mbc89SystemArrayLayout.KEYBOARD_BASE_OFFSET].toInt() and 0xff)
            .coerceIn(1, KEYBOARD_BASIS_MAX)
        return (basis - 1) * 256
    }

    // An address from the bus belongs to the Keyboards in either range (firmware cuSendKbFn).
    private fun keyboardOffset(address: Int): Int? {
        val base = if (address >= ACCESSORY_DCC_BASE) ACCESSORY_DCC_BASE else ACCESSORY_MM_BASE
        val offset = address - base - basisOffset()
        return offset.takeIf { it in 0 until keyboardCount * KEYBOARD_COLUMNS }
    }

    private fun processAccessorySwitch(data: ByteArray) {
        val address = ((data[2].toInt() and 0xff) shl 8) or (data[3].toInt() and 0xff)
        // Only the power-on command counts; power off belongs to the same key press.
        switchedAccessory = if (data[5] != 0.toByte()) MemoryCommand(address, data[4] != 0.toByte()) else null
        val offset = keyboardOffset(address) ?: return
        applyKeyboardState(offset / KEYBOARD_COLUMNS, offset % KEYBOARD_COLUMNS, data[4] != 0.toByte())
    }

    private fun applyKeyboardState(kbIndex: Int, column: Int, green: Boolean) {
        val mask = 1 shl column
        keyboardStates[kbIndex] = if (green) {
            keyboardStates[kbIndex] or mask
        } else {
            keyboardStates[kbIndex] and mask.inv()
        }
    }
    val isRegistered: Boolean
        get() = moduleGuid != null
    val isParaCenterDatabaseValid: Boolean
        get() = databaseValid
    val paraCenterRegistrationStep: Int
        get() = if (registrationInProgress) registrationStep else 0
    val registeredGuid: List<Int>?
        get() = moduleGuid?.toList()
    val systemArraySnapshot: ByteArray
        get() = systemArray.copyOf()

    fun onTransportConnecting() {
        cs2Connected = false
        arrayTransferActive = false
    }

    fun locomotiveMapping(slot: Int): LocomotiveMapping = locomotives.mapping(slot)

    fun locomotiveState(address: Int): LocomotiveState = locomotives.state(address)

    fun buildLocomotiveCommand(command: LocomotiveCommand): CanPacket? =
        if (systemStopped) null else locomotives.buildCommand(command)

    fun markLocomotiveCommandSent(command: LocomotiveCommand) {
        locomotives.markCommandSent(command)
    }

    fun buildLocomotiveSync(address: Int): List<CanPacket> =
        if (systemStopped) emptyList() else locomotives.buildSync(address)

    fun process(packet: CanPacket): CanPacket? = processDetailed(packet).response

    fun processDetailed(packet: CanPacket): Mbc89CoreResult {
        persistSystemArrayRequested = false
        restartRequested = false
        switchedAccessory = null
        s88Contact = null
        val responses = when (packet.canId) {
            CAN_ID_CONFIGURATION -> processCs2Configuration(packet)
            Mbc89Locomotives.CAN_ID_SPEED,
            Mbc89Locomotives.CAN_ID_DIRECTION,
            Mbc89Locomotives.CAN_ID_FUNCTION -> locomotives.answerQuery(packet)
            else -> listOfNotNull(processPacket(packet))
        }
        return Mbc89CoreResult(
            responses,
            persistSystemArrayRequested,
            restartRequested,
            switchedAccessory,
            s88Contact,
        )
    }

    private fun processPacket(packet: CanPacket): CanPacket? {
        if (packet.canId == CAN_ID_PING) {
            return processCs2Ping(packet)
        }
        if (packet.canId == CAN_ID_MBCAN_ACCESSORY) {
            // The 8-byte form carries a switching time; with D2 = 0x18 it is ParaCenter traffic instead.
            if (packet.dataLength == ACCESSORY_SWITCH_DLC ||
                (packet.dataLength == FULL_DLC && (packet.data[PC_LOCID_INDEX].toInt() and 0xff) != MBCAN_LOCID)
            ) {
                processAccessorySwitch(packet.data)
                return null
            }
            return processPcMessage(packet)
        }
        if (packet.canId == CAN_ID_S88_EVENT_RESPONSE) {
            // s88 event of mbc88/mbc90 for the Memory automatic mode: D2/D3 contact, D5 new state.
            if (packet.dataLength == FULL_DLC) {
                val pin = ((packet.data[2].toInt() and 0xff) shl 8) or (packet.data[3].toInt() and 0xff)
                s88Contact = S88Contact(pin, packet.data[5] != 0.toByte())
            }
            return null
        }
        if (packet.canId == CAN_ID_SYSTEM_COMMAND) {
            return when (packet.dataLength) {
                STOP_GO_DLC -> processSystemStopGo(packet.data)
                CONFIGURATION_WRITE_DLC -> processCs2ConfigurationWrite(packet.data)
                else -> processCs2MeasurementRequest(packet)
            }
        }
        if (packet.canId != CAN_ID_CONNECT6021) return null

        val data = packet.data
        return when (packet.dataLength) {
            STREAM_CONTROL_DLC -> processStreamControl(packet, data)
            SLOT_ASSIGNMENT_DLC -> processSlotAssignment(packet, data)
            else -> null
        }
    }

    private fun processCs2Ping(packet: CanPacket): CanPacket? {
        // Die CS sendet die Ping-Anfrage mit DLC 0; die Mbc89Lib wertet die DLC nicht aus.
        cs2Connected = true
        val guid = moduleGuid ?: return null
        val hash = registrationHash(guid)
        return CanPacket.create(
            priority = 0,
            canId = CAN_ID_PING_RESPONSE,
            hashHigh = hash.first,
            hashLow = hash.second,
            data = byteArrayOf(
                guid[0].toByte(),
                guid[1].toByte(),
                guid[2].toByte(),
                guid[3].toByte(),
                systemArray[Mbc89SystemArrayLayout.MODULE_EXTENSION_OFFSET],
                systemArray[Mbc89SystemArrayLayout.MODULE_EXTENSION_OFFSET + 1],
                FIXED_CONNECT6021_KENNUNG_HIGH.toByte(),
                FIXED_CONNECT6021_KENNUNG_LOW.toByte(),
            ),
        )
    }

    fun buildSystemStopGo(go: Boolean): CanPacket {
        val hash = moduleGuid?.let(::registrationHash)
            ?: Pair(STOP_GO_DEFAULT_HASH_HIGH, STOP_GO_DEFAULT_HASH_LOW)
        return CanPacket.create(
            priority = 0,
            canId = CAN_ID_SYSTEM_COMMAND,
            hashHigh = hash.first,
            hashLow = hash.second,
            data = byteArrayOf(0, 0, 0, 0, stopGoState(go)),
        )
    }

    fun markSystemStopGoSent(go: Boolean) {
        systemStopped = !go
    }

    private fun processSystemStopGo(data: ByteArray): CanPacket? {
        val guid = moduleGuid ?: return null
        val isUniversal = (0 until GUID_LENGTH).all { data[it] == 0.toByte() }
        // D2 is compared against GUiD byte 1 as in Mbc89Com.cpp; not a bug to fix.
        val isModule = (data[0].toInt() and 0xff) == guid[0] &&
            (data[1].toInt() and 0xff) == guid[1] &&
            (data[2].toInt() and 0xff) == guid[1] &&
            (data[3].toInt() and 0xff) == guid[3]
        if (!isUniversal && !isModule) return null

        val go = when (data[STOP_GO_STATE_INDEX].toInt() and 0xff) {
            0 -> false
            1 -> true
            else -> return null
        }
        systemStopped = !go
        val hash = registrationHash(guid)
        return CanPacket.create(
            priority = 0,
            canId = CAN_ID_SYSTEM_COMMAND_RESPONSE,
            hashHigh = hash.first,
            hashLow = hash.second,
            data = byteArrayOf(
                guid[0].toByte(),
                guid[1].toByte(),
                guid[2].toByte(),
                guid[3].toByte(),
                stopGoState(go),
            ),
        )
    }

    private fun stopGoState(go: Boolean): Byte = if (go) 1 else 0

    private fun processCs2ConfigurationWrite(data: ByteArray): CanPacket? {
        if ((data[MEASUREMENT_SUBCOMMAND_INDEX].toInt() and 0xff) != SUBCOMMAND_MEASUREMENT) return null
        val guid = moduleGuid ?: return null
        if (!guidMatches(data, 0, guid)) return null
        cs2Connected = true

        val channel = data[CONFIGURATION_CHANNEL_INDEX].toInt() and 0xff
        val value = ((data[CONFIGURATION_VALUE_INDEX].toInt() and 0xff) shl 8) or
            (data[CONFIGURATION_VALUE_INDEX + 1].toInt() and 0xff)
        val valid = writeConfigurationValue(channel, value) ?: return null

        val hash = registrationHash(guid)
        return CanPacket.create(
            priority = 0,
            canId = CAN_ID_SYSTEM_COMMAND_RESPONSE,
            hashHigh = hash.first,
            hashLow = hash.second,
            data = byteArrayOf(
                cs2Group.toByte(),
                guid[1].toByte(),
                guid[2].toByte(),
                guid[3].toByte(),
                SUBCOMMAND_MEASUREMENT.toByte(),
                channel.toByte(),
                if (valid) 1 else 0,
            ),
        )
    }

    // Mbc89Com::cs2IndexParameterExtern(): out-of-range values are rejected, never clamped.
    private fun writeConfigurationValue(channel: Int, value: Int): Boolean? = when (channel) {
        CHANNEL_SW_MAJOR ->
            writeIfValid(value in SW_MAJOR_MIN..255, Mbc89SystemArrayLayout.MODULE_EXTENSION_OFFSET, value)
        CHANNEL_SW_MINOR ->
            writeIfValid(value in SW_MINOR_MIN..255, Mbc89SystemArrayLayout.MODULE_EXTENSION_OFFSET + 1, value)
        CHANNEL_MAPPING -> {
            val changed = value <= 1 && byte(Mbc89SystemArrayLayout.MAPPING_OFFSET) != value
            writeIfValid(value <= 1, Mbc89SystemArrayLayout.MAPPING_OFFSET, value).also {
                if (changed) locomotives.applyMapping(value)
            }
        }
        CHANNEL_BASIS -> writeIfValid(
            value in 1..KEYBOARD_BASIS_MAX,
            Mbc89SystemArrayLayout.KEYBOARD_BASE_OFFSET,
            value,
        )
        CHANNEL_I2C ->
            writeIfValid(value in 40..100, Mbc89SystemArrayLayout.I2C_FREQUENCY_OFFSET, value)
        in CHANNEL_PROTOCOL_FIRST..LAST_MODULE_CONFIG_CHANNEL -> writeIfValid(
            value <= 1,
            Mbc89SystemArrayLayout.PROTOCOL_SETTINGS_OFFSET + channel - CHANNEL_PROTOCOL_FIRST,
            value,
        )
        else -> null
    }

    private fun writeIfValid(valid: Boolean, offset: Int, value: Int): Boolean {
        if (valid && byte(offset) != value) {
            systemArray[offset] = value.toByte()
            persistSystemArrayRequested = true
        }
        return valid
    }

    private fun byte(offset: Int): Int = systemArray[offset].toInt() and 0xff

    private fun processCs2MeasurementRequest(packet: CanPacket): CanPacket? {
        if (packet.dataLength != MEASUREMENT_REQUEST_DLC) return null
        val data = packet.data
        if ((data[MEASUREMENT_SUBCOMMAND_INDEX].toInt() and 0xff) != SUBCOMMAND_MEASUREMENT) return null
        val guid = moduleGuid ?: return null
        if (!guidMatches(data, 0, guid)) return null
        cs2Connected = true

        val channel = data[MEASUREMENT_CHANNEL_INDEX].toInt() and 0xff
        val value = when (channel) {
            1 -> (asciiDigits(SUPPLY_VOLTAGE_OFFSET, 3) * 10 - 1000) / 2
            2 -> asciiDigits(TEMPERATURE_OFFSET, 3) / 4
            else -> return null
        }
        val hash = registrationHash(guid)
        return CanPacket.create(
            priority = 0,
            canId = CAN_ID_SYSTEM_COMMAND_RESPONSE,
            hashHigh = hash.first,
            hashLow = hash.second,
            data = byteArrayOf(
                guid[0].toByte(),
                guid[1].toByte(),
                guid[2].toByte(),
                guid[3].toByte(),
                SUBCOMMAND_MEASUREMENT.toByte(),
                channel.toByte(),
                ((value ushr 8) and 0xff).toByte(),
                (value and 0xff).toByte(),
            ),
        )
    }

    private fun asciiDigits(offset: Int, count: Int): Int {
        var result = 0
        repeat(count) { index ->
            result = result * 10 + ((systemArray[offset + index].toInt() and 0xff) - '0'.code)
        }
        return result
    }

    private fun processPcMessage(packet: CanPacket): CanPacket? {
        if (packet.dataLength != FULL_DLC) return null
        val data = packet.data
        if ((data[PC_LOCID_INDEX].toInt() and 0xff) != MBCAN_LOCID) {
            return null
        }
        val guid = moduleGuid
        val isPcGuid = guid != null && guidMatches(data, PC_GUID_INDEX, guid)

        return when (data[PC_COMMAND_INDEX].toInt() and 0xff) {
            PC_DB_HIGH -> {
                copyDatabasePart(data, 0)
                null
            }
            PC_DB_MIDDLE -> {
                copyDatabasePart(data, 4)
                null
            }
            PC_DB_LOW -> {
                copyDatabasePart(data, 8)
                null
            }
            PC_KENNER -> {
                val group = data[PC_D5_INDEX].toInt() and 0xff
                if (group != byte(Mbc89SystemArrayLayout.CS2_GROUP_OFFSET)) {
                    persistSystemArrayRequested = true
                }
                cs2Group = group
                moduleInfo[LAST_INFO_INDEX + 1] = cs2Group.toByte()
                moduleGuid = moduleGuid?.toMutableList()?.apply { set(0, cs2Group) }
                systemArray[Mbc89SystemArrayLayout.CS2_GROUP_OFFSET] = cs2Group.toByte()
                moduleGuid?.let(::updateGuidInSystemArray)
                null
            }
            PC_RESET -> {
                if (isPcGuid) restartRequested = true
                null
            }
            PC_MD_DEL -> {
                if (isPcGuid) blankModule()
                null
            }
            PC_NEU -> {
                if (!isRegistered && databaseValid && data[PC_D7_INDEX] == 0.toByte()) {
                    startRegistration()
                    registrationResponse()
                } else {
                    null
                }
            }
            PC_NEU_DATA -> processRegistrationAcknowledgement(data)
            PC_ALIVE -> {
                val guid = moduleGuid
                if (guid != null && guidMatches(data, PC_GUID_INDEX, guid)) {
                    pcResponse(PC_MD_ALIVE, guid)
                } else {
                    null
                }
            }
            PC_ARRAY -> processArrayRequest(data)
            PC_UPGRADE -> {
                val registeredGuid = moduleGuid
                if (arrayTransferActive &&
                    (registeredGuid == null || !guidMatches(data, PC_GUID_INDEX, registeredGuid))
                ) {
                    arrayTransferActive = false
                }
                null
            }
            PC_ARRAY_DATA -> processArrayData(data, isPcGuid)
            else -> null
        }
    }

    private fun processCs2Configuration(packet: CanPacket): List<CanPacket> {
        if (packet.dataLength != CONFIG_REQUEST_DLC) return emptyList()
        val data = packet.data
        val channel = data[CONFIG_CHANNEL_INDEX].toInt() and 0xff
        if (channel !in 0..LAST_CONFIG_CHANNEL) return emptyList()
        cs2Connected = true

        val guid = moduleGuid ?: return emptyList()
        if (!guidMatches(data, 0, guid)) return emptyList()

        val row = cs2ConfigurationRow(channel, guid)
        val responses = ArrayList<CanPacket>(CONFIG_PACKET_COUNT + 1)
        repeat(CONFIG_PACKET_COUNT) { packetIndex ->
            responses += CanPacket.create(
                priority = 0,
                canId = CAN_ID_CONFIGURATION_RESPONSE,
                hashHigh = CONFIG_HASH_HIGH,
                hashLow = packetIndex + 1,
                data = row.copyOfRange(
                    packetIndex * CONFIG_PACKET_DATA_LENGTH,
                    (packetIndex + 1) * CONFIG_PACKET_DATA_LENGTH,
                ),
            )
        }
        val hash = registrationHash(guid)
        responses += CanPacket.create(
            priority = 0,
            canId = CAN_ID_CONFIGURATION_RESPONSE,
            hashHigh = hash.first,
            hashLow = hash.second,
            data = byteArrayOf(
                guid[0].toByte(),
                guid[1].toByte(),
                guid[2].toByte(),
                guid[3].toByte(),
                channel.toByte(),
                CONFIG_PACKET_COUNT.toByte(),
            ),
        )
        return responses
    }

    private fun cs2ConfigurationRow(channel: Int, guid: List<Int>): ByteArray {
        val row = when (channel) {
            0 -> ByteArray(CONFIG_ROW_LENGTH).apply {
                this[0] = BASE_MEASUREMENT_COUNT.toByte()
                this[1] = TOTAL_CONFIGURATION_COUNT.toByte()
                "MBC".encodeToByteArray().copyInto(this, destinationOffset = 8)
                this[15] = ' '.code.toByte()
                "MBCAN mbc-".encodeToByteArray().copyInto(this, destinationOffset = 16)
                systemArray.copyInto(
                    this,
                    destinationOffset = 11,
                    startIndex = ARTICLE_OFFSET,
                    endIndex = ARTICLE_OFFSET + ARTICLE_LENGTH,
                )
                systemArray.copyInto(
                    this,
                    destinationOffset = 26,
                    startIndex = ARTICLE_OFFSET,
                    endIndex = ARTICLE_OFFSET + 2,
                )
                val moduleNumber = ((systemArray[GUID_OFFSET + 3].toInt() and 0xff) + 1)
                this[6] = ((moduleNumber ushr 8) and 0xff).toByte()
                this[7] = (moduleNumber and 0xff).toByte()
            }
            1 -> paddedRow(
                1, 0, 0xc3, 0xd2, 0x31, 0x08, 0, 0,
                0, 0x32, 1, 0x2c, 2, 0x58, 3, 0x52,
                'V'.code, 'O'.code, 'L'.code, 'T'.code, 0,
                '1'.code, '0'.code, '.'.code, '0'.code, '0'.code, 0,
                '2'.code, '7'.code, '.'.code, '0'.code, '0'.code, 0,
                'V'.code, 0,
            )
            2 -> paddedRow(
                2, 0, 0x0c, 0x08, 0xf0, 0xc0, 0, 0,
                0, 0x7d, 0, 0x96, 0, 0xaf, 0, 0xc8,
                'T'.code, 'E'.code, 'M'.code, 'P'.code, 0,
                '0'.code, '.'.code, '0'.code, 0,
                '8'.code, '0'.code, '.'.code, '0'.code, 0,
                'C'.code, 0,
            )
            3 -> baseConfigurationRow(3, byteArrayOf(
                1, 1, 0, 0, 0, 0, 0, 0,
                'S'.code.toByte(), '/'.code.toByte(), 'N'.code.toByte(), ':'.code.toByte(),
                0, '0'.code.toByte(), 'x'.code.toByte(), 0,
            )).also { result ->
                val serial = systemArray.copyOfRange(INFO_OFFSET, INFO_OFFSET + SERIAL_LENGTH)
                serial.forEachIndexed { index, byte ->
                    val value = byte.toInt() and 0xff
                    result[15 + index * 2] = hexDigit(value ushr 4)
                    result[16 + index * 2] = hexDigit(value and 0x0f)
                }
            }
            4 -> baseConfigurationRow(4, byteArrayOf(
                1, 1, 0, 0, 0, 0, 0, 0,
                'G'.code.toByte(), 'U'.code.toByte(), 'i'.code.toByte(), 'D'.code.toByte(),
                ':'.code.toByte(), 0, '#'.code.toByte(), 0,
                '-'.code.toByte(), 0, 0, '-'.code.toByte(), '0'.code.toByte(),
                'x'.code.toByte(), 0, 0,
            )).also { result ->
                result[15] = systemArray[GUID_OFFSET]
                result[17] = systemArray[GUID_OFFSET + 1]
                result[18] = systemArray[GUID_OFFSET + 2]
                val lastGuidByte = systemArray[GUID_OFFSET + 3].toInt() and 0xff
                result[22] = hexDigit(lastGuidByte ushr 4)
                result[23] = hexDigit(lastGuidByte and 0x0f)
            }
            5 -> baseConfigurationRow(5, byteArrayOf(
                1, 1, 0, 0, 0, 0, 0, 0,
                'F'.code.toByte(), 'W'.code.toByte(), ':'.code.toByte(), 0,
                '#'.code.toByte(), 0, '.'.code.toByte(), 0,
            )).also { result ->
                val versionOffsets = intArrayOf(13, 15, 16)
                repeat(SOFTWARE_LENGTH) { index ->
                    val versionDigit = (systemArray[SOFTWARE_OFFSET + index].toInt() and 0xff) -
                        '0'.code
                    result[versionOffsets[index]] = hexDigit(versionDigit)
                }
            }
            6 -> baseConfigurationRow(6, byteArrayOf(
                1, 1, 0, 0, 0, 0, 0, 0,
                'H'.code.toByte(), 'W'.code.toByte(), ':'.code.toByte(), 0,
                '#'.code.toByte(), 0, 0, '.'.code.toByte(),
                0, 0, '.'.code.toByte(), 0,
            )).also { result ->
                val hardwareOffset = HARDWARE_OFFSET
                result[13] = systemArray[hardwareOffset]
                result[14] = systemArray[hardwareOffset + 1]
                result[16] = systemArray[hardwareOffset + 2]
                result[17] = systemArray[hardwareOffset + 3]
                result[19] = systemArray[hardwareOffset + 4]
                result[20] = systemArray[hardwareOffset + 5]
            }
            in FIRST_MODULE_CONFIG_CHANNEL..LAST_MODULE_CONFIG_CHANNEL ->
                moduleConfigurationRow(channel)
            else -> ByteArray(CONFIG_ROW_LENGTH)
        }
        if (channel != 0) row[0] = channel.toByte()
        return row
    }

    private fun baseConfigurationRow(channel: Int, headerAndLabel: ByteArray): ByteArray =
        ByteArray(CONFIG_ROW_LENGTH).apply {
            headerAndLabel.copyInto(this)
            this[0] = channel.toByte()
            this[2] = 1
        }

    private fun moduleConfigurationRow(channel: Int): ByteArray {
        val rowIndex = channel - FIRST_MODULE_CONFIG_CHANNEL
        val row = ByteArray(CONFIG_ROW_LENGTH)
        row[0] = channel.toByte()
        if (rowIndex < CLASSIC_CONFIG_COUNT) {
            when (rowIndex) {
                0, 1 -> {
                    val minimum = if (rowIndex == 0) SW_MAJOR_MIN else SW_MINOR_MIN
                    row[1] = SLIDER_CONFIG_TYPE.toByte()
                    row[3] = minimum.toByte()
                    row[5] = 0xff.toByte()
                    val label = if (rowIndex == 0) "SW Major" else "SW Minor"
                    writeAsciiTerminated(row, 8, "$label:")
                    writeAsciiTerminated(row, 18, minimum.toString())
                    writeAsciiTerminated(row, 20, "255")
                    row[24] = '#'.code.toByte()
                    row[7] = if (rowIndex == 0) {
                        systemArray[Mbc89SystemArrayLayout.MODULE_EXTENSION_OFFSET]
                    } else {
                        systemArray[Mbc89SystemArrayLayout.MODULE_EXTENSION_OFFSET + 1]
                    }
                }
                2 -> {
                    row[1] = SELECT_CONFIG_TYPE.toByte()
                    row[2] = 2
                    row[3] = systemArray[Mbc89SystemArrayLayout.MAPPING_OFFSET]
                    writeAsciiTerminated(row, 8, "Mapping:")
                    writeAsciiTerminated(row, 17, "Mapping")
                    writeAsciiTerminated(row, 25, "Mapping&Direkt")
                }
                3, 4 -> {
                    row[1] = SLIDER_CONFIG_TYPE.toByte()
                    val isBasis = rowIndex == 3
                    val minimum = if (isBasis) 1 else 40
                    val maximum = if (isBasis) 4 else 100
                    val valueOffset = if (isBasis) {
                        Mbc89SystemArrayLayout.KEYBOARD_BASE_OFFSET
                    } else {
                        Mbc89SystemArrayLayout.I2C_FREQUENCY_OFFSET
                    }
                    row[3] = minimum.toByte()
                    row[5] = maximum.toByte()
                    writeAsciiTerminated(row, 8, if (isBasis) "Basis:" else "I2C:")
                    writeAsciiTerminated(row, if (isBasis) 15 else 13, minimum.toString())
                    writeAsciiTerminated(row, if (isBasis) 17 else 16, maximum.toString())
                    if (!isBasis) writeAsciiTerminated(row, 20, "kHz") else row[19] = '#'.code.toByte()
                    row[7] = systemArray[valueOffset]
                }
            }
        } else {
            val protocolIndex = rowIndex - CLASSIC_CONFIG_COUNT
            val keyboard = protocolIndex / GROUPS_PER_KEYBOARD + 1
            val group = protocolIndex % GROUPS_PER_KEYBOARD + 1
            row[1] = SELECT_CONFIG_TYPE.toByte()
            row[2] = 2
            row[3] = systemArray[Mbc89SystemArrayLayout.PROTOCOL_SETTINGS_OFFSET + protocolIndex]
            val label = "KeyB$keyboard Group$group:"
            writeAsciiTerminated(row, 8, label)
            val optionsOffset = 8 + label.length + 1
            writeAsciiTerminated(row, optionsOffset, "MM")
            writeAsciiTerminated(row, optionsOffset + 3, "DCC")
        }
        return row
    }

    private fun paddedRow(vararg values: Int): ByteArray =
        ByteArray(CONFIG_ROW_LENGTH).apply {
            values.forEachIndexed { index, value -> this[index] = value.toByte() }
        }

    private fun writeAsciiTerminated(target: ByteArray, offset: Int, text: String) {
        text.encodeToByteArray().copyInto(target, destinationOffset = offset)
        target[offset + text.length] = 0
    }

    private fun hexDigit(value: Int): Byte =
        (if (value < 10) '0'.code + value else 'A'.code + value - 10).toByte()

    private fun processArrayRequest(data: ByteArray): CanPacket? {
        val guid = moduleGuid ?: return null
        if (!guidMatches(data, PC_GUID_INDEX, guid)) {
            arrayTransferActive = false
            return null
        }
        arrayTransferActive = true
        return pcResponse(PC_MD_ARRAY, guid)
    }

    private fun processArrayData(data: ByteArray, isPcGuid: Boolean): CanPacket? {
        val guid = moduleGuid ?: return null
        if (!arrayTransferActive || isPcGuid) return null

        val operation = data[PC_ARRAY_OPERATION_INDEX].toInt() and 0xff
        val index = ((data[PC_ARRAY_INDEX_HIGH_INDEX].toInt() and 0xff) shl 8) or
            (data[PC_ARRAY_INDEX_LOW_INDEX].toInt() and 0xff)
        val value = data[PC_ARRAY_VALUE_INDEX].toInt() and 0xff
        val responseData = data.copyOf()

        when (index) {
            SYSTEM_ARRAY_WRITE_INDEX -> {
                if (value != 1) arrayTransferActive = false
            }
            SYSTEM_ARRAY_READ_INDEX -> {
                if (value != 1) arrayTransferActive = false
            }
            SYSTEM_ARRAY_EEPROM_INDEX -> persistSystemArrayRequested = true
            else -> if (index < systemArray.size) {
                if (operation == ARRAY_WRITE_OPERATION) {
                    systemArray[index] = value.toByte()
                    synchronizeSystemArrayValue(index, value)
                } else {
                    responseData[PC_ARRAY_VALUE_INDEX] = systemArray[index]
                }
            }
        }

        responseData[PC_COMMAND_INDEX] = PC_MD_ARRAY_DATA.toByte()
        val hash = registrationHash(guid)
        return CanPacket.create(
            priority = 0,
            canId = CAN_ID_MBCAN_ACCESSORY,
            hashHigh = hash.first,
            hashLow = hash.second,
            data = responseData,
        )
    }

    private fun synchronizeSystemArrayValue(index: Int, value: Int) {
        if (index in MODULE_INFO_OFFSET until MODULE_INFO_TEMPLATE.size) {
            moduleInfo[index] = value.toByte()
        }
        when (index) {
            in GUID_OFFSET until GUID_OFFSET + GUID_LENGTH -> {
                moduleGuid = (GUID_OFFSET until GUID_OFFSET + GUID_LENGTH).map {
                    systemArray[it].toInt() and 0xff
                }
                cs2Group = moduleGuid?.first() ?: 0
            }
            in DATABASE_OFFSET until DATABASE_OFFSET + DATABASE_LENGTH -> {
                systemArray.copyInto(
                    databaseBytes,
                    destinationOffset = 0,
                    startIndex = DATABASE_OFFSET,
                    endIndex = DATABASE_OFFSET + DATABASE_LENGTH,
                )
                databaseSnapshot = databaseBytes.copyOf()
            }
            CS2_GROUP_OFFSET -> {
                cs2Group = value
                moduleInfo[LAST_INFO_INDEX + 1] = value.toByte()
            }
        }
    }

    private fun copyDatabasePart(data: ByteArray, offset: Int) {
        for (index in 0 until DATABASE_PART_LENGTH) {
            databaseBytes[offset + index] = data[PC_VALUE_INDEX + index]
        }
        databasePartsReceived = databasePartsReceived or (1 shl (offset / DATABASE_PART_LENGTH))
        if (databasePartsReceived != ALL_DATABASE_PARTS) return

        databaseSnapshot?.let { previous ->
            if (!previous.contentEquals(databaseBytes) && isRegistered) {
                moduleGuid = null
                updateGuidInSystemArray(null)
                arrayTransferActive = false
                registrationInProgress = false
                registrationStep = 0
                registrationInfoIndex = 0
            }
        }
        databaseBytes.copyInto(
            systemArray,
            destinationOffset = Mbc89SystemArrayLayout.DATABASE_OFFSET,
        )
        databaseSnapshot = databaseBytes.copyOf()
        databaseValid = true
        databasePartsReceived = 0
    }

    // Mbc89ComBase::moduleBlank(): database "000000000000" and GUiD 0xFF, both persisted.
    private fun blankModule() {
        databaseBytes.fill(DATABASE_EMPTY_DIGIT)
        databaseBytes.copyInto(systemArray, destinationOffset = Mbc89SystemArrayLayout.DATABASE_OFFSET)
        databaseSnapshot = databaseBytes.copyOf()
        databasePartsReceived = 0
        moduleGuid = null
        updateGuidInSystemArray(null)
        arrayTransferActive = false
        registrationInProgress = false
        registrationStep = 0
        registrationInfoIndex = 0
        persistSystemArrayRequested = true
    }

    private fun startRegistration() {
        registrationInProgress = true
        registrationStep = 1
        registrationInfoIndex = FIRST_INFO_INDEX
    }

    private fun processRegistrationAcknowledgement(data: ByteArray): CanPacket? {
        if (isRegistered || !registrationInProgress) return null
        if ((data[PC_STEP_INDEX].toInt() and 0xff) != registrationStep ||
            (data[PC_D7_INDEX].toInt() and 0xff) !=
            (moduleInfo[registrationInfoIndex].toInt() and 0xff)
        ) {
            registrationInProgress = false
            registrationStep = 0
            registrationInfoIndex = 0
            return null
        }

        if (registrationInfoIndex <= LAST_INFO_INDEX) {
            registrationInfoIndex += 1
            registrationStep += 1
            return registrationResponse()
        }

        moduleGuid = listOf(
            cs2Group,
            moduleInfo[ARTICLE_INFO_OFFSET].toInt() and 0xff,
            moduleInfo[ARTICLE_INFO_OFFSET + 1].toInt() and 0xff,
            data[PC_FINAL_GUID_INDEX].toInt() and 0xff,
        )
        updateGuidInSystemArray(moduleGuid)
        registrationInProgress = false
        registrationStep = 0
        registrationInfoIndex = 0
        persistSystemArrayRequested = true
        return null
    }

    private fun updateGuidInSystemArray(guid: List<Int>?) {
        val identity = guid ?: List(GUID_LENGTH) { 0xff }
        identity.forEachIndexed { index, byte ->
            systemArray[Mbc89SystemArrayLayout.GUID_OFFSET + index] = byte.toByte()
        }
    }

    private fun registrationResponse(): CanPacket {
        val data = ByteArray(MAX_DATA_LENGTH)
        data[MBCAN_LOCID_HIGH_INDEX] = MBCAN_LOCID_HIGH.toByte()
        data[PC_LOCID_INDEX] = MBCAN_LOCID.toByte()
        data[PC_COMMAND_INDEX] = PC_MD_NEU_DATA.toByte()
        data[PC_STEP_INDEX] = registrationStep.toByte()
        data[PC_D7_INDEX] = moduleInfo[registrationInfoIndex]
        return CanPacket.create(
            priority = 0,
            canId = CAN_ID_MBCAN_ACCESSORY,
            hashHigh = UNREGISTERED_HASH_HIGH,
            hashLow = UNREGISTERED_HASH_LOW,
            data = data,
        )
    }

    private fun pcResponse(command: Int, guid: List<Int>): CanPacket {
        val data = byteArrayOf(
            0,
            0,
            MBCAN_LOCID.toByte(),
            command.toByte(),
            guid[0].toByte(),
            guid[1].toByte(),
            guid[2].toByte(),
            guid[3].toByte(),
        )
        val hash = registrationHash(guid)
        return CanPacket.create(
            priority = 0,
            canId = CAN_ID_MBCAN_ACCESSORY,
            hashHigh = hash.first,
            hashLow = hash.second,
            data = data,
        )
    }

    private fun guidMatches(data: ByteArray, startIndex: Int, guid: List<Int>): Boolean =
        guid.indices.all { index ->
            (data[startIndex + index].toInt() and 0xff) == guid[index]
        }

    // Maerklin CS2 protocol 2.0, section 1.2.4: UID high word XOR low word, then bits 8 and 9 set and
    // bit 7 cleared (distinction from the CS1 protocol).
    private fun registrationHash(guid: List<Int>): Pair<Int, Int> {
        val highWord = (cs2Group shl 8) or guid[1]
        val lowWord = (guid[2] shl 8) or guid[3]
        val hash = ((highWord xor lowWord) or HASH_REQUIRED_BITS) and HASH_CLEARED_BIT.inv()
        return Pair((hash ushr 8) and 0xff, hash and 0xff)
    }

    // The CS opens one block per Connect6021 module (GUiD + 00 00), sends its locomotive assignments
    // without GUiD and closes the block (GUiD + FF FF). Blocks of another module are ignored together
    // with their assignments, otherwise several modules on one bus take over each other's locomotives.
    private fun processStreamControl(packet: CanPacket, data: ByteArray): CanPacket? {
        val start = data[STREAM_COMMAND_INDEX] == 0x00.toByte() && data[STREAM_STATE_INDEX] == 0x00.toByte()
        val end = data[STREAM_COMMAND_INDEX] == 0xff.toByte() && data[STREAM_STATE_INDEX] == 0xff.toByte()
        val guid = moduleGuid
        val universal = (0 until GUID_LENGTH).all { data[it] == 0.toByte() }
        if (guid != null && !universal && !guidMatches(data, 0, guid)) {
            if (start) foreignStreamOpen = true else if (end) foreignStreamOpen = false
            return null
        }
        foreignStreamOpen = false
        if (start) {
            connect6021Streaming = true
            locomotives.resetAddresses()
        } else if (end) {
            connect6021Streaming = false
        }
        return packet.withCanId(CAN_ID_CONNECT6021_RESPONSE)
    }

    private fun processSlotAssignment(packet: CanPacket, data: ByteArray): CanPacket? {
        val slot = data[SLOT_INDEX].toInt() and 0xff
        if (slot > FIRST_RESERVED_SLOT || foreignStreamOpen) return null
        locomotives.assign(slot, data)
        return packet.withCanId(CAN_ID_CONNECT6021_RESPONSE)
    }

    companion object {
        private const val FIRST_RESERVED_SLOT = 80
        private const val GUID_LENGTH = 4
        private const val CAN_ID_CONNECT6021 = Mbc89CanProtocol.CAN_ID_CONNECT6021
        private const val CAN_ID_CONNECT6021_RESPONSE = Mbc89CanProtocol.CAN_ID_CONNECT6021_RESPONSE
        private const val CAN_ID_PING = Mbc89CanProtocol.CAN_ID_PING
        private const val CAN_ID_PING_RESPONSE = Mbc89CanProtocol.CAN_ID_PING_RESPONSE
        private const val CAN_ID_CONFIGURATION = Mbc89CanProtocol.CAN_ID_CONFIGURATION
        private const val CAN_ID_CONFIGURATION_RESPONSE = Mbc89CanProtocol.CAN_ID_CONFIGURATION_RESPONSE
        private const val STREAM_CONTROL_DLC = 6
        private const val SLOT_ASSIGNMENT_DLC = 5
        private const val STREAM_COMMAND_INDEX = 4
        private const val STREAM_STATE_INDEX = 5
        private const val SLOT_INDEX = 4
        private const val CAN_ID_MBCAN_ACCESSORY = Mbc89CanProtocol.CAN_ID_MBCAN_ACCESSORY
        private const val MBCAN_LOCID_HIGH = 0
        private const val MBCAN_LOCID_HIGH_INDEX = 0
        private const val MBCAN_LOCID = Mbc89CanProtocol.MBCAN_LOCID
        private const val PC_LOCID_INDEX = Mbc89CanProtocol.PC_LOCID_INDEX
        private const val PC_COMMAND_INDEX = Mbc89CanProtocol.PC_COMMAND_INDEX
        private const val PC_VALUE_INDEX = 4
        private const val PC_D5_INDEX = 5
        private const val PC_GUID_INDEX = 4
        private const val PC_STEP_INDEX = 6
        private const val PC_D7_INDEX = 7
        private const val PC_FINAL_GUID_INDEX = 5
        private const val PC_ARRAY_OPERATION_INDEX = 4
        private const val PC_ARRAY_INDEX_HIGH_INDEX = 5
        private const val PC_ARRAY_INDEX_LOW_INDEX = 6
        private const val PC_ARRAY_VALUE_INDEX = 7
        private const val PC_DB_HIGH = Mbc89CanProtocol.PC_DB_HIGH
        private const val PC_DB_MIDDLE = Mbc89CanProtocol.PC_DB_MIDDLE
        private const val PC_DB_LOW = Mbc89CanProtocol.PC_DB_LOW
        private const val PC_KENNER = Mbc89CanProtocol.PC_KENNER
        private const val PC_NEU = Mbc89CanProtocol.PC_NEU
        private const val PC_NEU_DATA = Mbc89CanProtocol.PC_NEU_DATA
        private const val PC_MD_NEU_DATA = Mbc89CanProtocol.PC_MD_NEU_DATA
        private const val PC_RESET = Mbc89CanProtocol.PC_RESET
        private const val PC_MD_DEL = Mbc89CanProtocol.PC_MD_DEL
        private const val PC_ALIVE = Mbc89CanProtocol.PC_ALIVE
        private const val PC_MD_ALIVE = Mbc89CanProtocol.PC_MD_ALIVE
        private const val PC_ARRAY = Mbc89CanProtocol.PC_ARRAY
        private const val PC_MD_ARRAY = Mbc89CanProtocol.PC_MD_ARRAY
        private const val PC_ARRAY_DATA = Mbc89CanProtocol.PC_ARRAY_DATA
        private const val PC_MD_ARRAY_DATA = Mbc89CanProtocol.PC_MD_ARRAY_DATA
        private const val PC_UPGRADE = Mbc89CanProtocol.PC_UPGRADE
        private const val CAN_ID_SYSTEM_COMMAND = Mbc89CanProtocol.CAN_ID_SYSTEM_COMMAND
        private const val CAN_ID_S88_EVENT_RESPONSE = Mbc89CanProtocol.CAN_ID_S88_EVENT_RESPONSE
        private const val CAN_ID_SYSTEM_COMMAND_RESPONSE = Mbc89CanProtocol.CAN_ID_SYSTEM_COMMAND_RESPONSE
        private const val SUBCOMMAND_MEASUREMENT = 0x0b
        const val KEYBOARD_COUNT_MAX = 16
        const val CONTROL80F_COUNT_MAX = 8
        const val KEYBOARD_COLUMNS = 16
        private const val DEFAULT_KEYBOARD_COUNT = 1
        private const val KEYBOARD_BASIS_MAX = 4
        private const val ACCESSORY_MM_BASE = 0x3000
        private const val ACCESSORY_DCC_BASE = 0x3800
        const val KEYS_PER_GROUP = 4

        fun keyboardUsesDcc(systemArray: ByteArray, kbIndex: Int, group: Int): Boolean =
            systemArray[Mbc89SystemArrayLayout.PROTOCOL_SETTINGS_OFFSET + kbIndex * GROUPS_PER_KEYBOARD + group]
                .toInt() == 1
        private const val ACCESSORY_SWITCH_DLC = 6
        private const val ACCESSORY_HASH_HIGH = 0x03
        private const val ACCESSORY_HASH_LOW = 0x00
        private const val ACCESSORY_POWER_ON: Byte = 1
        private const val DEFAULT_CS2_GROUP = 0x6d
        private const val DATABASE_EMPTY_DIGIT = '0'.code.toByte()
        private const val SW_MAJOR_MIN = 1
        // Mbc89Com allows 1...255; minor 0 is valid (user decision, default version is 1.0).
        private const val SW_MINOR_MIN = 0
        private const val STOP_GO_DLC = 5
        private const val CONFIGURATION_WRITE_DLC = 8
        private const val CONFIGURATION_CHANNEL_INDEX = 5
        private const val CONFIGURATION_VALUE_INDEX = 6
        private const val STOP_GO_STATE_INDEX = 4
        private const val STOP_GO_DEFAULT_HASH_HIGH = 0x03
        private const val STOP_GO_DEFAULT_HASH_LOW = 0x00
        private const val MEASUREMENT_REQUEST_DLC = 6
        private const val MEASUREMENT_SUBCOMMAND_INDEX = 4
        private const val MEASUREMENT_CHANNEL_INDEX = 5
        private const val SUPPLY_VOLTAGE_OFFSET = Mbc89SystemArrayLayout.SUPPLY_VOLTAGE_OFFSET
        private const val TEMPERATURE_OFFSET = Mbc89SystemArrayLayout.TEMPERATURE_OFFSET
        private const val CONFIG_REQUEST_DLC = 5
        private const val CONFIG_CHANNEL_INDEX = 4
        private const val CONFIG_ROW_LENGTH = 40
        private const val CONFIG_PACKET_COUNT = 5
        private const val CONFIG_PACKET_DATA_LENGTH = 8
        private const val CONFIG_HASH_HIGH = 0x03
        private const val BASE_MEASUREMENT_COUNT = 2
        private const val BASE_CONFIGURATION_COUNT = 4
        private const val MODULE_CONFIGURATION_COUNT = 69
        private const val TOTAL_CONFIGURATION_COUNT =
            BASE_CONFIGURATION_COUNT + MODULE_CONFIGURATION_COUNT
        private const val LAST_CONFIG_CHANNEL =
            BASE_MEASUREMENT_COUNT + TOTAL_CONFIGURATION_COUNT
        private const val FIRST_MODULE_CONFIG_CHANNEL = BASE_MEASUREMENT_COUNT + BASE_CONFIGURATION_COUNT + 1
        private const val LAST_MODULE_CONFIG_CHANNEL =
            FIRST_MODULE_CONFIG_CHANNEL + MODULE_CONFIGURATION_COUNT - 1
        private const val CLASSIC_CONFIG_COUNT = 5
        private const val CHANNEL_SW_MAJOR = FIRST_MODULE_CONFIG_CHANNEL
        private const val CHANNEL_SW_MINOR = FIRST_MODULE_CONFIG_CHANNEL + 1
        private const val CHANNEL_MAPPING = FIRST_MODULE_CONFIG_CHANNEL + 2
        private const val CHANNEL_BASIS = FIRST_MODULE_CONFIG_CHANNEL + 3
        private const val CHANNEL_I2C = FIRST_MODULE_CONFIG_CHANNEL + 4
        private const val CHANNEL_PROTOCOL_FIRST = FIRST_MODULE_CONFIG_CHANNEL + CLASSIC_CONFIG_COUNT
        const val GROUPS_PER_KEYBOARD = 4
        private const val SLIDER_CONFIG_TYPE = 0x02
        private const val SELECT_CONFIG_TYPE = 0x01
        private const val INFO_OFFSET = 1
        private const val ARTICLE_OFFSET = 9
        private const val ARTICLE_LENGTH = 4
        private const val SOFTWARE_LENGTH = 3
        private const val SERIAL_LENGTH = 8
        private const val DATABASE_LENGTH = 12
        private const val DATABASE_PART_LENGTH = 4
        private const val ALL_DATABASE_PARTS = 0b111
        private const val FULL_DLC = Mbc89CanProtocol.FULL_DLC
        private const val MAX_DATA_LENGTH = 8
        private const val FIRST_INFO_INDEX = 1
        private const val LAST_INFO_INDEX = 41
        private const val ARTICLE_INFO_OFFSET = 9
        private const val MODULE_INFO_OFFSET = 1
        private const val SERIAL_OFFSET = 1
        private const val SOFTWARE_OFFSET = 13
        private const val HARDWARE_OFFSET = 16
        private const val NAME_OFFSET = 22
        private const val GENERATED_NAME_LENGTH = 2 + SERIAL_LENGTH * 2 + 2
        private const val GUID_OFFSET = Mbc89SystemArrayLayout.GUID_OFFSET
        private const val DATABASE_OFFSET = Mbc89SystemArrayLayout.DATABASE_OFFSET
        private const val CS2_GROUP_OFFSET = Mbc89SystemArrayLayout.CS2_GROUP_OFFSET
        private const val UNREGISTERED_HASH_HIGH = 0x5f
        private const val UNREGISTERED_HASH_LOW = 0x38
        private const val HASH_REQUIRED_BITS = 0x0300
        private const val HASH_CLEARED_BIT = 0x0080
        private const val FIXED_CONNECT6021_KENNUNG_HIGH = 0x00
        private const val FIXED_CONNECT6021_KENNUNG_LOW = 0x20
        private const val ARRAY_WRITE_OPERATION = 1
        private const val SYSTEM_ARRAY_EEPROM_INDEX = 0xfffd
        private const val SYSTEM_ARRAY_READ_INDEX = 0xfffe
        private const val SYSTEM_ARRAY_WRITE_INDEX = 0xffff
        private const val ARTICLE_NUMBER = "8980"
        private const val SOFTWARE_VERSION = "100"
        private const val HARDWARE_VERSION = "261103"
        private const val SIMULATED_VOLTAGE = "187"
        private const val SIMULATED_TEMPERATURE = "221"
        private val DEFAULT_SYNTHETIC_SERIAL = byteArrayOf(0, 0, 0, 0, 0, 0, 0, 1)
        private val MODULE_INFO_TEMPLATE = ByteArray(43).apply {
            val serial = byteArrayOf(0, 0, 0, 0, 0, 0, 0, 1)
            val article = ARTICLE_NUMBER.encodeToByteArray()
            val software = "100".encodeToByteArray()
            val hardware = HARDWARE_VERSION.encodeToByteArray()
            val name = "0x0000000000000001".encodeToByteArray() +
                byteArrayOf(0x81.toByte(), 0x81.toByte())
            serial.copyInto(this, destinationOffset = 1)
            article.copyInto(this, destinationOffset = 9)
            software.copyInto(this, destinationOffset = 13)
            hardware.copyInto(this, destinationOffset = 16)
            name.copyInto(this, destinationOffset = 22)
        }
    }
}
