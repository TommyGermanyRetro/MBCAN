package de.mbc89.android

import android.annotation.SuppressLint
import android.app.Notification
import android.app.NotificationChannel
import android.app.NotificationManager
import android.app.PendingIntent
import android.app.Service
import android.content.Intent
import android.content.pm.ApplicationInfo
import android.net.wifi.WifiManager
import android.os.Binder
import android.os.Build
import android.os.IBinder
import android.os.PowerManager
import android.provider.Settings
import android.util.Log
import de.mbc89.android.core.LocomotiveCommand
import de.mbc89.android.core.LedState
import de.mbc89.android.core.Mbc89Core
import de.mbc89.android.core.Mbc89Memory
import de.mbc89.android.core.MemoryCommand
import de.mbc89.android.core.MemoryRoutePlay
import de.mbc89.android.core.MemoryRouteView
import de.mbc89.android.transport.CanPacket
import de.mbc89.android.transport.CanTransport
import de.mbc89.android.transport.CanTcpTransport
import de.mbc89.android.transport.ConnectionState
import de.mbc89.android.storage.Mbc89IdentityStore
import java.io.IOException
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.Job
import kotlinx.coroutines.SupervisorJob
import kotlinx.coroutines.cancel
import kotlinx.coroutines.delay
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.asStateFlow
import kotlinx.coroutines.flow.collect
import kotlinx.coroutines.launch
import kotlinx.coroutines.sync.Mutex
import kotlinx.coroutines.sync.withLock
import kotlinx.coroutines.withContext

class Mbc89Service : Service() {
    private val serviceScope = CoroutineScope(SupervisorJob() + Dispatchers.IO)
    private lateinit var transport: CanTransport
    private lateinit var core: Mbc89Core
    private var packetCollector: Job? = null
    private var wifiLock: WifiManager.WifiLock? = null
    private var lowLatencyLock: WifiManager.WifiLock? = null
    private var wakeLock: PowerManager.WakeLock? = null
    private var transportStateCollector: Job? = null
    private var identityInitialization: Job? = null
    private lateinit var identityStore: Mbc89IdentityStore
    private var identityLoadError: String? = null
    private var persistedGuid: List<Int>? = null
    private var persistedSystemArray: ByteArray? = null
    private lateinit var syntheticSerial: ByteArray
    private var configuredKeyboardCount = 1
    @Volatile
    private var persistenceFailure: String? = null
    private val coreMutex = Mutex()
    private val binder = LocalBinder()
    private val _state = MutableStateFlow<ConnectionState>(ConnectionState.Disconnected)
    private val _receivedCount = MutableStateFlow(0L)
    private val _protocolError = MutableStateFlow<String?>(null)
    private val _moduleRegistered = MutableStateFlow(false)
    private val _cs2Connected = MutableStateFlow(false)
    private val _paraCenterDatabaseValid = MutableStateFlow(false)
    private val _paraCenterRegistrationStep = MutableStateFlow(0)
    private val _registeredGuid = MutableStateFlow<List<Int>?>(null)
    private val _systemArray = MutableStateFlow(ByteArray(0))
    private val _savedSystemArray = MutableStateFlow(ByteArray(0))
    private val _systemStopped = MutableStateFlow(false)
    private val _keyboardCount = MutableStateFlow(1)
    private val _keyboardStates = MutableStateFlow<List<Int>>(emptyList())
    private val _control80fCount = MutableStateFlow(0)
    private val _memoryCount = MutableStateFlow(0)
    private val _memoryInterlock = MutableStateFlow(List(Mbc89Memory.MEMORY_COUNT_MAX) { false })
    private val _memoryS88 = MutableStateFlow(List(Mbc89Memory.MEMORY_COUNT_MAX) { 0 })
    private val _memoryLeds = MutableStateFlow<List<List<LedState>>>(emptyList())
    private val _memoryOccupied = MutableStateFlow(List(Mbc89Memory.MEMORY_COUNT_MAX) { false })
    private val _memoryRoutes = MutableStateFlow<List<MemoryRouteView>>(emptyList())
    private var memory = Mbc89Memory()
    private var savedMemoryState = ""
    @Volatile
    private var reconnectTarget: Pair<String, Int>? = null
    @Volatile
    private var connectionLossReason: String? = null
    private var reconnectJob: Job? = null

    val state: StateFlow<ConnectionState> = _state.asStateFlow()
    val receivedCount: StateFlow<Long> = _receivedCount.asStateFlow()
    val protocolError: StateFlow<String?> = _protocolError.asStateFlow()
    val moduleRegistered: StateFlow<Boolean> = _moduleRegistered.asStateFlow()
    val cs2Connected: StateFlow<Boolean> = _cs2Connected.asStateFlow()
    val paraCenterDatabaseValid: StateFlow<Boolean> = _paraCenterDatabaseValid.asStateFlow()
    val paraCenterRegistrationStep: StateFlow<Int> = _paraCenterRegistrationStep.asStateFlow()
    val registeredGuid: StateFlow<List<Int>?> = _registeredGuid.asStateFlow()
    val systemArray: StateFlow<ByteArray> = _systemArray.asStateFlow()
    val savedSystemArray: StateFlow<ByteArray> = _savedSystemArray.asStateFlow()
    val systemStopped: StateFlow<Boolean> = _systemStopped.asStateFlow()
    val keyboardCount: StateFlow<Int> = _keyboardCount.asStateFlow()
    val keyboardStates: StateFlow<List<Int>> = _keyboardStates.asStateFlow()
    val control80fCount: StateFlow<Int> = _control80fCount.asStateFlow()
    val memoryCount: StateFlow<Int> = _memoryCount.asStateFlow()
    val memoryInterlock: StateFlow<List<Boolean>> = _memoryInterlock.asStateFlow()
    val memoryS88: StateFlow<List<Int>> = _memoryS88.asStateFlow()
    val memoryLeds: StateFlow<List<List<LedState>>> = _memoryLeds.asStateFlow()
    val memoryOccupied: StateFlow<List<Boolean>> = _memoryOccupied.asStateFlow()
    val memoryRoutes: StateFlow<List<MemoryRouteView>> = _memoryRoutes.asStateFlow()

    inner class LocalBinder : Binder() {
        val service: Mbc89Service
            get() = this@Mbc89Service
    }

    override fun onCreate() {
        super.onCreate()
        transport = CanTcpTransport(serviceScope)
        identityStore = Mbc89IdentityStore(applicationContext)
        createNotificationChannel()
        transportStateCollector = serviceScope.launch {
            transport.state.collect { transportState ->
                val failure = persistenceFailure
                val target = reconnectTarget
                val lossReason = connectionLossReason
                _state.value = when {
                    failure != null && transportState is ConnectionState.Disconnected ->
                        ConnectionState.Failed(failure)
                    transportState is ConnectionState.Connected -> {
                        reconnectTarget = transportState.host to transportState.port
                        if (lossReason != null) MessageLog.add("Verbindung wiederhergestellt")
                        connectionLossReason = null
                        transportState
                    }
                    target != null && transportState is ConnectionState.Failed -> {
                        if (lossReason == null) MessageLog.add("Verbindung verloren: ${transportState.reason}")
                        connectionLossReason = transportState.reason
                        startReconnect(target)
                        ConnectionState.Reconnecting(transportState.reason)
                    }
                    target != null && lossReason != null && transportState is ConnectionState.Connecting ->
                        ConnectionState.Reconnecting(lossReason)
                    transportState is ConnectionState.Failed -> {
                        MessageLog.add("Verbindung fehlgeschlagen: ${transportState.reason}")
                        releaseWifiLock()
                        transportState
                    }
                    else -> transportState
                }
            }
        }
        identityInitialization = serviceScope.launch {
            try {
                identityStore.checkSchemaVersion()
                persistedGuid = identityStore.loadGuid()
                val storedSystemArray = identityStore.loadSystemArray()
                syntheticSerial = identityStore.loadDeviceSerial(androidId())
                configuredKeyboardCount =
                    identityStore.loadKeyboardCount() ?: configuredKeyboardCount
                _control80fCount.value = identityStore.loadControl80fCount() ?: 0
                _memoryCount.value = identityStore.loadMemoryCount() ?: 0
                memory = identityStore.loadMemory() ?: Mbc89Memory()
                memory.restoreState(identityStore.loadMemoryState())
                savedMemoryState = memory.encodeState()
                _memoryInterlock.value = identityStore.loadMemoryInterlock()
                _memoryInterlock.value.forEachIndexed { instance, enabled -> memory.interlock[instance] = enabled }
                _memoryS88.value = identityStore.loadMemoryS88()
                applyS88Modules()
                core = Mbc89Core(
                    persistedGuid,
                    storedSystemArray,
                    syntheticSerial,
                    configuredKeyboardCount,
                )
                val initializedSystemArray = core.systemArraySnapshot
                if (storedSystemArray == null ||
                    !storedSystemArray.contentEquals(initializedSystemArray)
                ) {
                    identityStore.saveSystemArray(initializedSystemArray)
                }
                persistedSystemArray = initializedSystemArray
                _savedSystemArray.value = initializedSystemArray
                publishCoreState()
                packetCollector = serviceScope.launch {
                    transport.receivedPackets.collect { packet ->
                        _receivedCount.value += 1
                        logPacket("rx", packet)
                        var autoPlays = emptyList<MemoryRoutePlay>()
                        val result = try {
                            coreMutex.withLock {
                                core.processDetailed(packet).also { processed ->
                                    memory.systemStopped = core.systemStopped
                                    processed.switchedAccessory?.let { command ->
                                        memory.noteSwitch(command)
                                        if (memory.tapKeyboardCommand(command, lengthen = false)) saveMemory()
                                    }
                                    processed.s88Contact?.let { memory.s88Event(it.pin, it.occupied) }
                                    autoPlays = memory.takeAutoPlays()
                                    if (processed.switchedAccessory != null || processed.s88Contact != null) {
                                        saveMemoryState()
                                    }
                                }
                            }
                        } catch (exception: RuntimeException) {
                            reportError("CAN packet could not be processed: ${exception.message}")
                            return@collect
                        }
                        publishCoreState()
                        launchAutoPlays(autoPlays)
                        if (result.persistSystemArray) {
                            val updatedGuid = core.registeredGuid
                            val updatedSystemArray = core.systemArraySnapshot
                            try {
                                identityStore.saveModuleState(updatedGuid, updatedSystemArray)
                                if (debugLog) {
                                    Log.d(LOG_TAG, "saved SW=${updatedSystemArray[328]}.${updatedSystemArray[329]}")
                                }
                                persistedGuid = updatedGuid
                                persistedSystemArray = updatedSystemArray
                                _savedSystemArray.value = updatedSystemArray
                            } catch (exception: IOException) {
                                restorePersistedCore(
                                    exception.message ?: "System array could not be saved.",
                                )
                                return@collect
                            }
                        }
                        for (response in result.responses) {
                            delay(CONFIG_RESPONSE_DELAY_MS)
                            try {
                                transport.send(response)
                                logPacket("tx", response)
                            } catch (exception: IOException) {
                                reportError(exception.message ?: "CAN response could not be sent.")
                                break
                            }
                        }
                        if (result.restartRequested) {
                            coreMutex.withLock {
                                core = Mbc89Core(
                                    persistedGuid,
                                    persistedSystemArray,
                                    syntheticSerial,
                                    configuredKeyboardCount,
                                )
                                publishCoreState()
                            }
                        }
                    }
                }
            } catch (exception: IOException) {
                identityLoadError = exception.message ?: "Stored module state could not be read."
                reportError(identityLoadError)
            }
        }
        isRunning = true
    }

    override fun onStartCommand(intent: Intent?, flags: Int, startId: Int): Int {
        startForeground(NOTIFICATION_ID, buildNotification())
        return START_NOT_STICKY
    }

    override fun onBind(intent: Intent?): IBinder = binder

    // CAN traffic goes to the Android log only in debuggable builds; errors are in the message tab.
    private val debugLog: Boolean by lazy {
        applicationInfo.flags and ApplicationInfo.FLAG_DEBUGGABLE != 0
    }

    private fun logPacket(direction: String, packet: CanPacket) {
        if (!debugLog) return
        val relevant = when (packet.canId) {
            0x00, 0x01, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x16, 0x30, 0x31, 0x3a, 0x44, 0x45 -> true
            else -> false
        }
        if (!relevant) return
        Log.d(
            LOG_TAG,
            "%s id=%02X hash=%02X%02X dlc=%d %s".format(
                direction,
                packet.canId,
                packet.hashHigh,
                packet.hashLow,
                packet.dataLength,
                packet.data.joinToString(" ") { "%02X".format(it.toInt() and 0xff) },
            ),
        )
    }

    private fun reportError(text: String?) {
        _protocolError.value = text
        text?.let(MessageLog::add)
    }

    private fun publishCoreState() {
        _systemArray.value = core.systemArraySnapshot
        _moduleRegistered.value = core.isRegistered
        _registeredGuid.value = core.registeredGuid
        _cs2Connected.value = core.cs2Connected
        _paraCenterDatabaseValid.value = core.isParaCenterDatabaseValid
        _paraCenterRegistrationStep.value = core.paraCenterRegistrationStep
        _systemStopped.value = core.systemStopped
        _keyboardCount.value = core.keyboardCount
        _keyboardStates.value = List(core.keyboardCount, core::keyboardState)
        _memoryLeds.value = List(Mbc89Memory.MEMORY_COUNT_MAX, memory::ledStates)
        _memoryOccupied.value = List(Mbc89Memory.MEMORY_COUNT_MAX, memory::hasRoutes)
        _memoryRoutes.value = memory.routeViews()
    }

    fun setMemoryCount(count: Int) {
        serviceScope.launch {
            identityInitialization?.join()
            if (identityLoadError != null) return@launch
            try {
                identityStore.saveMemoryCount(count)
            } catch (exception: IOException) {
                reportError(exception.message ?: "Memory count could not be saved.")
                return@launch
            }
            coreMutex.withLock {
                for (instance in count until Mbc89Memory.MEMORY_COUNT_MAX) memory.clearInstance(instance)
                saveMemory()
                saveMemoryState()
                _memoryCount.value = count
                applyS88Modules()
                publishCoreState()
            }
        }
    }

    fun setMemoryS88(instance: Int, module: Int) {
        require(instance in 0 until Mbc89Memory.MEMORY_COUNT_MAX) { "Memory instance is out of range." }
        require(module in 0..Mbc89Memory.S88_MODULE_MAX) { "s88 module is out of range." }
        serviceScope.launch {
            identityInitialization?.join()
            if (identityLoadError != null) return@launch
            if (Mbc89Memory.s88Overlaps(_memoryS88.value, _memoryCount.value, instance, module)) return@launch
            val updated = _memoryS88.value.toMutableList().also { it[instance] = module }
            try {
                identityStore.saveMemoryS88(updated)
            } catch (exception: IOException) {
                reportError(exception.message ?: "s88 modules could not be saved.")
                return@launch
            }
            coreMutex.withLock {
                _memoryS88.value = updated
                applyS88Modules()
            }
        }
    }

    // Only configured Memory instances take part in the automatic mode.
    private fun applyS88Modules() {
        _memoryS88.value.forEachIndexed { instance, module ->
            memory.s88Module[instance] = if (instance < _memoryCount.value) module else 0
        }
    }

    fun setMemoryInterlock(instance: Int, enabled: Boolean) {
        require(instance in 0 until Mbc89Memory.MEMORY_COUNT_MAX) { "Memory instance is out of range." }
        serviceScope.launch {
            identityInitialization?.join()
            if (identityLoadError != null) return@launch
            val updated = _memoryInterlock.value.toMutableList().also { it[instance] = enabled }
            try {
                identityStore.saveMemoryInterlock(updated)
            } catch (exception: IOException) {
                reportError(exception.message ?: "Memory interlocking could not be saved.")
                return@launch
            }
            coreMutex.withLock {
                memory.interlock[instance] = enabled
                publishCoreState()
            }
            _memoryInterlock.value = updated
        }
    }

    fun clearMemory(instance: Int) {
        serviceScope.launch {
            identityInitialization?.join()
            if (identityLoadError != null) return@launch
            coreMutex.withLock {
                memory.clearInstance(instance)
                saveMemory()
                saveMemoryState()
                publishCoreState()
            }
        }
    }

    fun pressMemory(instance: Int, index: Int) {
        serviceScope.launch {
            identityInitialization?.join()
            if (identityLoadError != null) return@launch
            var autoPlays = emptyList<MemoryRoutePlay>()
            val result = coreMutex.withLock {
                if (core.systemStopped) return@launch
                memory.systemStopped = false
                val pressed = memory.press(instance, index)
                if (pressed.routesChanged) saveMemory()
                saveMemoryState()
                autoPlays = memory.takeAutoPlays()
                publishCoreState()
                pressed
            }
            launchAutoPlays(autoPlays)
            if (result.deleteShown) {
                serviceScope.launch {
                    delay(MEMORY_DELETE_DISPLAY_MS)
                    coreMutex.withLock {
                        if (memory.finishDeleteDisplay(instance)) publishCoreState()
                    }
                }
            }
            if (result.clearFlash) {
                serviceScope.launch {
                    delay(MEMORY_COMMAND_GAP_MS)
                    coreMutex.withLock {
                        memory.finishClearFlash(instance)
                        publishCoreState()
                    }
                }
            }
            result.warningToken?.let { token ->
                serviceScope.launch {
                    delay(MEMORY_WARNING_MS)
                    coreMutex.withLock {
                        if (memory.expireWarning(instance, token)) publishCoreState()
                    }
                }
            }
            if (result.commands.isEmpty()) return@launch
            playMemoryCommands(instance, index - Mbc89Memory.FIRST_ROUTE_BUTTON, result.commands)
        }
    }

    private fun launchAutoPlays(plays: List<MemoryRoutePlay>) {
        for (play in plays) {
            serviceScope.launch { playMemoryCommands(play.instance, play.localRoute, play.commands) }
        }
    }

    // Sends the commands of a set route; its LED flashes once per command.
    private suspend fun playMemoryCommands(instance: Int, local: Int, commands: List<MemoryCommand>) {
        try {
            for (command in commands) {
                val packet = coreMutex.withLock {
                    core.buildAccessorySwitch(command.address, command.green, command.switchTime)
                } ?: break
                transport.send(packet)
                logPacket("ui", packet)
                coreMutex.withLock {
                    core.markAccessorySent(command.address, command.green)
                    memory.setPlayLit(instance, local, true)
                    publishCoreState()
                }
                delay(MEMORY_COMMAND_GAP_MS)
                coreMutex.withLock {
                    memory.setPlayLit(instance, local, false)
                    publishCoreState()
                }
                delay(MEMORY_COMMAND_GAP_MS)
            }
        } catch (exception: IOException) {
            reportError(exception.message ?: "Accessory command could not be sent.")
        } finally {
            coreMutex.withLock {
                memory.finishPlay(instance, local)
                publishCoreState()
            }
        }
    }

    // Stores the last operating state (manual 1.3) whenever it changed.
    private suspend fun saveMemoryState() {
        val text = memory.encodeState()
        if (text == savedMemoryState) return
        try {
            identityStore.saveMemoryState(text)
            savedMemoryState = text
        } catch (exception: IOException) {
            reportError(exception.message ?: "Memory state could not be saved.")
        }
    }

    private suspend fun saveMemory() {
        try {
            identityStore.saveMemory(memory)
        } catch (exception: IOException) {
            reportError(exception.message ?: "Memory routes could not be saved.")
        }
    }

    private suspend fun restorePersistedCore(reason: String) {
        core = Mbc89Core(
            persistedGuid,
            persistedSystemArray,
            syntheticSerial,
            configuredKeyboardCount,
        )
        publishCoreState()
        reportError(reason)
        persistenceFailure = reason
        stopReconnect()
        transport.disconnect()
        _state.value = ConnectionState.Failed(reason)
    }

    // After losing an established connection the service tries again every few seconds until it is
    // back or the user disconnects; nothing that failed to send meanwhile is repeated.
    private fun startReconnect(target: Pair<String, Int>) {
        if (reconnectJob?.isActive == true) return
        reconnectJob = serviceScope.launch {
            while (reconnectTarget == target) {
                delay(RECONNECT_DELAY_MS)
                if (reconnectTarget != target) break
                core.onTransportConnecting()
                _cs2Connected.value = false
                transport.connect(target.first, target.second)
                if (transport.state.value is ConnectionState.Connected) break
            }
        }
    }

    private fun stopReconnect() {
        reconnectTarget = null
        connectionLossReason = null
        reconnectJob?.cancel()
        reconnectJob = null
    }

    fun connect(host: String, port: Int) {
        persistenceFailure = null
        stopReconnect()
        _protocolError.value = null
        serviceScope.launch {
            identityInitialization?.join()
            val loadError = identityLoadError
            if (loadError != null) {
                _state.value = ConnectionState.Failed(loadError)
                return@launch
            }
            core.onTransportConnecting()
            _cs2Connected.value = false
            acquireWifiLock()
            transport.connect(host, port)
        }
    }

    fun setSystemGo(go: Boolean) {
        serviceScope.launch {
            identityInitialization?.join()
            if (identityLoadError != null) return@launch
            val packet = coreMutex.withLock { core.buildSystemStopGo(go) }
            try {
                transport.send(packet)
                logPacket("ui", packet)
            } catch (exception: IOException) {
                reportError(exception.message ?: "Stop/Go could not be sent.")
                return@launch
            }
            coreMutex.withLock { core.markSystemStopGoSent(go) }
            _systemStopped.value = core.systemStopped
        }
    }

    fun switchAccessory(kbIndex: Int, column: Int, green: Boolean) {
        serviceScope.launch {
            identityInitialization?.join()
            if (identityLoadError != null) return@launch
            val packet = coreMutex.withLock {
                core.buildKeyboardSwitch(kbIndex, column, green)
            } ?: return@launch
            try {
                transport.send(packet)
                logPacket("ui", packet)
            } catch (exception: IOException) {
                reportError(exception.message ?: "Accessory command could not be sent.")
                return@launch
            }
            var autoPlays = emptyList<MemoryRoutePlay>()
            coreMutex.withLock {
                core.markKeyboardSwitchSent(kbIndex, column, green)
                memory.systemStopped = core.systemStopped
                memory.noteSwitch(MemoryCommand(core.keyboardAddress(kbIndex, column), green))
                autoPlays = memory.takeAutoPlays()
                saveMemoryState()
                if (memory.tapKeyboardCommand(
                        MemoryCommand(core.keyboardAddress(kbIndex, column), green),
                    )
                ) {
                    saveMemory()
                }
                publishCoreState()
            }
            launchAutoPlays(autoPlays)
        }
    }

    fun sendLocomotiveCommand(command: LocomotiveCommand) {
        serviceScope.launch {
            identityInitialization?.join()
            if (identityLoadError != null) return@launch
            val packet = coreMutex.withLock { core.buildLocomotiveCommand(command) } ?: return@launch
            try {
                transport.send(packet)
                logPacket("ui", packet)
            } catch (exception: IOException) {
                reportError(exception.message ?: "Locomotive command could not be sent.")
                return@launch
            }
            coreMutex.withLock {
                core.markLocomotiveCommandSent(command)
                publishCoreState()
            }
        }
    }

    fun takeOverLocomotive(address: Int) {
        serviceScope.launch {
            identityInitialization?.join()
            if (identityLoadError != null) return@launch
            val packets = coreMutex.withLock { core.buildLocomotiveSync(address) }
            for (packet in packets) {
                try {
                    transport.send(packet)
                    logPacket("ui", packet)
                } catch (exception: IOException) {
                    reportError(exception.message ?: "Locomotive state could not be sent.")
                    return@launch
                }
            }
        }
    }

    fun setKeyboardCount(count: Int) {
        serviceScope.launch {
            identityInitialization?.join()
            if (identityLoadError != null) return@launch
            try {
                identityStore.saveKeyboardCount(count)
            } catch (exception: IOException) {
                reportError(exception.message ?: "Keyboard count could not be saved.")
                return@launch
            }
            configuredKeyboardCount = count
            coreMutex.withLock {
                core.keyboardCount = count
                publishCoreState()
            }
        }
    }

    fun setControl80fCount(count: Int) {
        serviceScope.launch {
            identityInitialization?.join()
            if (identityLoadError != null) return@launch
            try {
                identityStore.saveControl80fCount(count)
            } catch (exception: IOException) {
                reportError(exception.message ?: "Control 80f count could not be saved.")
                return@launch
            }
            _control80fCount.value = count
        }
    }

    // Wi-Fi power save delays frames by up to ~50 ms, which stretches every CS3 configuration read.
    // Low latency only works with the screen on; with the screen off the high performance lock keeps
    // Wi-Fi awake and the partial wake lock keeps the CPU reading. A client that stops reading fills
    // the mbc80 send buffer and blocked its loop until the watchdog restarted it.
    @Suppress("DEPRECATION")
    @SuppressLint("WakelockTimeout")
    private fun acquireWifiLock() {
        if (wifiLock?.isHeld != true) {
            val wifiManager = applicationContext.getSystemService(WifiManager::class.java)
            if (wifiManager != null) {
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q) {
                    lowLatencyLock = wifiManager.createWifiLock(
                        WifiManager.WIFI_MODE_FULL_LOW_LATENCY,
                        "mbc89:can-tcp-low-latency",
                    ).apply {
                        setReferenceCounted(false)
                        acquire()
                    }
                }
                wifiLock = wifiManager.createWifiLock(
                    WifiManager.WIFI_MODE_FULL_HIGH_PERF,
                    "mbc89:can-tcp",
                ).apply {
                    setReferenceCounted(false)
                    acquire()
                }
            }
        }
        if (wakeLock?.isHeld != true) {
            val powerManager = applicationContext.getSystemService(PowerManager::class.java) ?: return
            wakeLock = powerManager.newWakeLock(PowerManager.PARTIAL_WAKE_LOCK, "mbc89:can-tcp").apply {
                setReferenceCounted(false)
                acquire()
            }
        }
    }

    @SuppressLint("HardwareIds")
    private fun androidId(): String? =
        Settings.Secure.getString(contentResolver, Settings.Secure.ANDROID_ID)

    private fun releaseWifiLock() {
        wifiLock?.takeIf { it.isHeld }?.release()
        wifiLock = null
        lowLatencyLock?.takeIf { it.isHeld }?.release()
        lowLatencyLock = null
        wakeLock?.takeIf { it.isHeld }?.release()
        wakeLock = null
    }

    fun disconnect() {
        persistenceFailure = null
        stopReconnect()
        serviceScope.launch {
            transport.disconnect()
            releaseWifiLock()
            withContext(Dispatchers.Main) {
                stopForeground(STOP_FOREGROUND_REMOVE)
                stopSelf()
            }
        }
    }

    override fun onDestroy() {
        isRunning = false
        packetCollector?.cancel()
        transportStateCollector?.cancel()
        serviceScope.cancel()
        transport.dispose()
        releaseWifiLock()
        super.onDestroy()
    }

    private fun createNotificationChannel() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            val channel = NotificationChannel(
                CHANNEL_ID,
                "mbc89 CAN-Verbindung",
                NotificationManager.IMPORTANCE_LOW,
            )
            getSystemService(NotificationManager::class.java).createNotificationChannel(channel)
        }
    }

    private fun buildNotification(): Notification {
        val activityIntent = PendingIntent.getActivity(
            this,
            0,
            Intent(this, MainActivity::class.java),
            PendingIntent.FLAG_UPDATE_CURRENT or PendingIntent.FLAG_IMMUTABLE,
        )
        return Notification.Builder(this, CHANNEL_ID)
            .setSmallIcon(android.R.drawable.stat_notify_sync)
            .setContentTitle("mbc89 CAN-TCP")
            .setContentText("CAN-Verbindungsdienst aktiv")
            .setContentIntent(activityIntent)
            .setOngoing(true)
            .build()
    }

    companion object {
        @Volatile
        var isRunning: Boolean = false
            private set

        private const val CHANNEL_ID = "mbc89_can_connection"
        private const val LOG_TAG = "mbc89can"
        private const val NOTIFICATION_ID = 15731
        private const val CONFIG_RESPONSE_DELAY_MS = 0L
        private const val MEMORY_COMMAND_GAP_MS = 300L
        private const val MEMORY_WARNING_MS = 5_000L
        private const val RECONNECT_DELAY_MS = 5_000L
        private const val MEMORY_DELETE_DISPLAY_MS = 1_000L
    }
}
