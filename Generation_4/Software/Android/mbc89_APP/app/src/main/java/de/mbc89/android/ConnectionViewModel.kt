package de.mbc89.android

import android.app.Application
import android.content.ComponentName
import android.content.Context
import android.content.Intent
import android.content.ServiceConnection
import android.os.Build
import android.os.IBinder
import androidx.lifecycle.AndroidViewModel
import androidx.lifecycle.viewModelScope
import de.mbc89.android.transport.ConnectionState
import de.mbc89.android.core.LedState
import de.mbc89.android.core.LocomotiveCommand
import de.mbc89.android.core.LocomotiveState
import de.mbc89.android.core.Mbc89Core
import de.mbc89.android.core.Mbc89Locomotives
import de.mbc89.android.core.Mbc89Memory
import de.mbc89.android.core.MemoryRouteView
import de.mbc89.android.core.Mbc89SystemArrayLayout
import de.mbc89.android.storage.Mbc89IdentityStore
import java.io.IOException
import kotlinx.coroutines.Job
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.asStateFlow
import kotlinx.coroutines.flow.collect
import kotlinx.coroutines.launch

class ConnectionViewModel(application: Application) : AndroidViewModel(application) {
    private val appContext = application.applicationContext
    private val identityStore = Mbc89IdentityStore(appContext)
    private val _savedEndpoint = MutableStateFlow<Pair<String, Int>?>(null)
    val savedEndpoint: StateFlow<Pair<String, Int>?> = _savedEndpoint.asStateFlow()
    private val _state = MutableStateFlow<ConnectionState>(ConnectionState.Disconnected)
    val state: StateFlow<ConnectionState> = _state.asStateFlow()
    private val _receivedCount = MutableStateFlow(0L)
    val receivedCount: StateFlow<Long> = _receivedCount.asStateFlow()
    private val _inputError = MutableStateFlow<String?>(null)
    val inputError: StateFlow<String?> = _inputError.asStateFlow()
    private val _protocolError = MutableStateFlow<String?>(null)
    val protocolError: StateFlow<String?> = _protocolError.asStateFlow()
    private val _moduleRegistered = MutableStateFlow(false)
    val moduleRegistered: StateFlow<Boolean> = _moduleRegistered.asStateFlow()
    private val _cs2Connected = MutableStateFlow(false)
    val cs2Connected: StateFlow<Boolean> = _cs2Connected.asStateFlow()
    private val _paraCenterDatabaseValid = MutableStateFlow(false)
    val paraCenterDatabaseValid: StateFlow<Boolean> = _paraCenterDatabaseValid.asStateFlow()
    private val _paraCenterRegistrationStep = MutableStateFlow(0)
    val paraCenterRegistrationStep: StateFlow<Int> = _paraCenterRegistrationStep.asStateFlow()
    private val _registeredGuid = MutableStateFlow<List<Int>?>(null)
    val registeredGuid: StateFlow<List<Int>?> = _registeredGuid.asStateFlow()
    private val _systemArray = MutableStateFlow(ByteArray(0))
    val systemArray: StateFlow<ByteArray> = _systemArray.asStateFlow()
    private val _persistedSystemArray = MutableStateFlow(ByteArray(0))
    val persistedSystemArray: StateFlow<ByteArray> = _persistedSystemArray.asStateFlow()
    private val _systemStopped = MutableStateFlow(false)
    val systemStopped: StateFlow<Boolean> = _systemStopped.asStateFlow()
    private val _controlUnits = MutableStateFlow(List(1 + Mbc89Core.CONTROL80F_COUNT_MAX) { ControlUnitSlot() })
    val controlUnits: StateFlow<List<ControlUnitSlot>> = _controlUnits.asStateFlow()
    private var nextClaimSeq = 1L
    private val _control80fCount = MutableStateFlow(0)
    val control80fCount: StateFlow<Int> = _control80fCount.asStateFlow()
    private val _memoryCount = MutableStateFlow(0)
    val memoryCount: StateFlow<Int> = _memoryCount.asStateFlow()
    private val _memoryInterlock = MutableStateFlow(List(Mbc89Memory.MEMORY_COUNT_MAX) { false })
    val memoryInterlock: StateFlow<List<Boolean>> = _memoryInterlock.asStateFlow()
    private val _memoryS88 = MutableStateFlow(List(Mbc89Memory.MEMORY_COUNT_MAX) { 0 })
    val memoryS88: StateFlow<List<Int>> = _memoryS88.asStateFlow()
    private val _memoryLeds = MutableStateFlow<List<List<LedState>>>(emptyList())
    val memoryLeds: StateFlow<List<List<LedState>>> = _memoryLeds.asStateFlow()
    private val _memoryOccupied = MutableStateFlow(List(Mbc89Memory.MEMORY_COUNT_MAX) { false })
    val memoryOccupied: StateFlow<List<Boolean>> = _memoryOccupied.asStateFlow()
    private val _memoryRoutes = MutableStateFlow<List<MemoryRouteView>>(emptyList())
    val memoryRoutes: StateFlow<List<MemoryRouteView>> = _memoryRoutes.asStateFlow()
    private val _keyboardCount = MutableStateFlow(1)
    val keyboardCount: StateFlow<Int> = _keyboardCount.asStateFlow()
    private val _keyboardStates = MutableStateFlow<List<Int>>(emptyList())
    val keyboardStates: StateFlow<List<Int>> = _keyboardStates.asStateFlow()
    private var service: Mbc89Service? = null
    private var serviceBound = false
    private var pendingConnection: PendingConnection? = null
    private var stateJob: Job? = null
    private var receivedCountJob: Job? = null
    private var protocolErrorJob: Job? = null
    private var registrationJob: Job? = null
    private var cs2ConnectionJob: Job? = null
    private var paraCenterDatabaseJob: Job? = null
    private var paraCenterRegistrationJob: Job? = null
    private var registeredGuidJob: Job? = null
    private var systemStoppedJob: Job? = null
    private var systemArrayJob: Job? = null
    private var persistedSystemArrayJob: Job? = null
    private var keyboardCountJob: Job? = null
    private var keyboardStatesJob: Job? = null
    private var control80fCountJob: Job? = null
    private var memoryCountJob: Job? = null
    private var memoryInterlockJob: Job? = null
    private var memoryS88Job: Job? = null
    private var memoryLedsJob: Job? = null
    private var memoryOccupiedJob: Job? = null
    private var memoryRoutesJob: Job? = null
    private val serviceIntent = Intent(appContext, Mbc89Service::class.java)

    private val serviceConnection = object : ServiceConnection {
        override fun onServiceConnected(name: ComponentName?, binder: IBinder?) {
            val connectedService = (binder as? Mbc89Service.LocalBinder)?.service
            if (connectedService == null) {
                _state.value = ConnectionState.Failed("Could not connect to the CAN service.")
                return
            }
            service = connectedService
            stateJob?.cancel()
            receivedCountJob?.cancel()
            protocolErrorJob?.cancel()
            registrationJob?.cancel()
            cs2ConnectionJob?.cancel()
            paraCenterDatabaseJob?.cancel()
            paraCenterRegistrationJob?.cancel()
            registeredGuidJob?.cancel()
            systemStoppedJob?.cancel()
            systemArrayJob?.cancel()
            persistedSystemArrayJob?.cancel()
            keyboardCountJob?.cancel()
            keyboardStatesJob?.cancel()
            control80fCountJob?.cancel()
            memoryCountJob?.cancel()
            memoryInterlockJob?.cancel()
            memoryS88Job?.cancel()
            memoryLedsJob?.cancel()
            memoryOccupiedJob?.cancel()
            memoryRoutesJob?.cancel()
            stateJob = viewModelScope.launch {
                connectedService.state.collect { _state.value = it }
            }
            receivedCountJob = viewModelScope.launch {
                connectedService.receivedCount.collect { _receivedCount.value = it }
            }
            protocolErrorJob = viewModelScope.launch {
                connectedService.protocolError.collect { _protocolError.value = it }
            }
            registrationJob = viewModelScope.launch {
                connectedService.moduleRegistered.collect { _moduleRegistered.value = it }
            }
            cs2ConnectionJob = viewModelScope.launch {
                connectedService.cs2Connected.collect { _cs2Connected.value = it }
            }
            paraCenterDatabaseJob = viewModelScope.launch {
                connectedService.paraCenterDatabaseValid.collect {
                    _paraCenterDatabaseValid.value = it
                }
            }
            paraCenterRegistrationJob = viewModelScope.launch {
                connectedService.paraCenterRegistrationStep.collect {
                    _paraCenterRegistrationStep.value = it
                }
            }
            registeredGuidJob = viewModelScope.launch {
                connectedService.registeredGuid.collect { _registeredGuid.value = it }
            }
            systemArrayJob = viewModelScope.launch {
                connectedService.systemArray.collect { _systemArray.value = it }
            }
            persistedSystemArrayJob = viewModelScope.launch {
                connectedService.savedSystemArray.collect { _persistedSystemArray.value = it }
            }
            systemStoppedJob = viewModelScope.launch {
                connectedService.systemStopped.collect { _systemStopped.value = it }
            }
            keyboardCountJob = viewModelScope.launch {
                connectedService.keyboardCount.collect { _keyboardCount.value = it }
            }
            keyboardStatesJob = viewModelScope.launch {
                connectedService.keyboardStates.collect { _keyboardStates.value = it }
            }
            control80fCountJob = viewModelScope.launch {
                connectedService.control80fCount.collect { _control80fCount.value = it }
            }
            memoryCountJob = viewModelScope.launch {
                connectedService.memoryCount.collect { _memoryCount.value = it }
            }
            memoryInterlockJob = viewModelScope.launch {
                connectedService.memoryInterlock.collect { _memoryInterlock.value = it }
            }
            memoryS88Job = viewModelScope.launch {
                connectedService.memoryS88.collect { _memoryS88.value = it }
            }
            memoryLedsJob = viewModelScope.launch {
                connectedService.memoryLeds.collect { _memoryLeds.value = it }
            }
            memoryOccupiedJob = viewModelScope.launch {
                connectedService.memoryOccupied.collect { _memoryOccupied.value = it }
            }
            memoryRoutesJob = viewModelScope.launch {
                connectedService.memoryRoutes.collect { _memoryRoutes.value = it }
            }
            pendingConnection?.let { request ->
                pendingConnection = null
                connectedService.connect(request.host, request.port)
            }
        }

        override fun onServiceDisconnected(name: ComponentName?) {
            service = null
            _state.value = ConnectionState.Failed("The CAN service connection was interrupted.")
        }
    }

    init {
        if (Mbc89Service.isRunning) {
            bindToService(Context.BIND_AUTO_CREATE)
        }
        viewModelScope.launch {
            _savedEndpoint.value = try {
                identityStore.loadEndpoint()
            } catch (exception: IOException) {
                null
            }
        }
        viewModelScope.launch { loadStoredDevices() }
    }

    // Shows the stored device setup before the service runs; the service takes over once bound.
    private suspend fun loadStoredDevices() {
        try {
            val keyboards = identityStore.loadKeyboardCount()
            val control80f = identityStore.loadControl80fCount()
            val memories = identityStore.loadMemoryCount()
            val interlock = identityStore.loadMemoryInterlock()
            val s88 = identityStore.loadMemoryS88()
            val stored = identityStore.loadMemory() ?: Mbc89Memory()
            stored.restoreState(identityStore.loadMemoryState())
            if (service != null) return
            keyboards?.let { _keyboardCount.value = it }
            control80f?.let { _control80fCount.value = it }
            memories?.let { _memoryCount.value = it }
            _memoryInterlock.value = interlock
            _memoryS88.value = s88
            _memoryOccupied.value = List(Mbc89Memory.MEMORY_COUNT_MAX, stored::hasRoutes)
            _memoryRoutes.value = stored.routeViews()
            _memoryLeds.value = List(Mbc89Memory.MEMORY_COUNT_MAX, stored::ledStates)
        } catch (exception: IOException) {
            reportError(exception.message ?: "Stored devices could not be loaded.")
        } catch (exception: IllegalArgumentException) {
            reportError(exception.message ?: "Stored devices could not be loaded.")
        }
    }

    fun connect(host: String, port: Int) {
        if (host.isBlank()) {
            _inputError.value = "Bitte die IP-Adresse oder den Hostnamen eingeben."
            return
        }
        if (port !in 1..65535) {
            _inputError.value = "TCP-Port muss zwischen 1 und 65535 liegen."
            return
        }
        _inputError.value = null
        _protocolError.value = null
        _state.value = ConnectionState.Connecting
        viewModelScope.launch {
            try {
                identityStore.saveEndpoint(host, port)
                _savedEndpoint.value = host to port
            } catch (exception: IOException) {
                reportError(exception.message ?: "Endpoint could not be saved.")
            }
        }

        val connectedService = service
        if (connectedService != null) {
            try {
                startForegroundService()
                connectedService.connect(host, port)
            } catch (exception: IllegalStateException) {
                _state.value = ConnectionState.Failed(
                    exception.message ?: "The CAN service could not be started.",
                )
            } catch (exception: SecurityException) {
                _state.value = ConnectionState.Failed(
                    exception.message ?: "Android denied starting the CAN service.",
                )
            }
            return
        }

        pendingConnection = PendingConnection(host, port)
        try {
            startForegroundService()
            bindToService(Context.BIND_AUTO_CREATE)
        } catch (exception: IllegalStateException) {
            pendingConnection = null
            _state.value = ConnectionState.Failed(
                exception.message ?: "The CAN service could not be started.",
            )
        } catch (exception: SecurityException) {
            pendingConnection = null
            _state.value = ConnectionState.Failed(
                exception.message ?: "Android denied starting the CAN service.",
            )
        }
    }

    fun disconnect() {
        pendingConnection = null
        val connectedService = service
        if (connectedService != null) {
            connectedService.disconnect()
        } else {
            appContext.stopService(serviceIntent)
            _state.value = ConnectionState.Disconnected
        }
    }

    fun setSystemGo(go: Boolean) {
        service?.setSystemGo(go)
    }

    fun switchAccessory(kbIndex: Int, column: Int, green: Boolean) {
        service?.switchAccessory(kbIndex, column, green)
    }

    fun enterControlUnitDigit(index: Int, digit: Int) {
        val slots = _controlUnits.value
        if (index !in slots.indices) return
        val updated = slots[index].withDigit(digit, nextClaimSeq)
        val locoEntered = updated.complete && updated.address != slots[index].address
        if (locoEntered) nextClaimSeq++
        _controlUnits.value = slots.toMutableList().also { it[index] = updated }
        if (locoEntered) {
            val address = updated.address.toInt()
            if (address in 1..Mbc89Locomotives.SLOT_80) service?.takeOverLocomotive(address)
        }
    }

    fun selectControlUnitAddress(index: Int, target: AddressEntry) {
        val slots = _controlUnits.value
        if (index !in slots.indices) return
        _controlUnits.value = slots.toMutableList().also { it[index] = slots[index].select(target) }
    }

    fun controlUnitAction(index: Int, action: LocoAction) {
        val slots = _controlUnits.value.take(1 + _control80fCount.value)
        val slot = slots.getOrNull(index) ?: return
        // f1-f4 go to the function decoder address (6021 manual 5.5), everything else to the loco.
        if (action is LocoAction.Function && action.index in 1..FUNCTION_DECODER_KEYS) {
            val function = slot.functionTarget.takeIf { it.length == 2 }?.toInt() ?: return
            if (function !in 1..Mbc89Locomotives.SLOT_80) return
            if (slot.functionAddress == null && addressOwnedElsewhere(slots, index)) return
            service?.sendLocomotiveCommand(LocomotiveCommand.Function(function, action.index, action.on))
            return
        }
        if (!slot.complete || addressOwnedElsewhere(slots, index)) return
        val address = slot.address.toInt()
        if (address !in 1..Mbc89Locomotives.SLOT_80) return
        val command = when (action) {
            is LocoAction.Speed -> LocomotiveCommand.Speed(address, action.step)
            is LocoAction.Function -> LocomotiveCommand.Function(address, action.index, action.on)
            is LocoAction.Direction -> LocomotiveCommand.Direction(address, action.forward)
        }
        service?.sendLocomotiveCommand(command)
    }

    fun setKeyboardCount(count: Int) {
        service?.setKeyboardCount(count)
    }

    fun setControl80fCount(count: Int) {
        service?.setControl80fCount(count)
    }

    fun clearMemory(instance: Int) {
        service?.clearMemory(instance)
    }

    fun setMemoryCount(count: Int) {
        service?.setMemoryCount(count)
    }

    fun setMemoryS88(instance: Int, module: Int) {
        service?.setMemoryS88(instance, module)
    }

    fun setMemoryInterlock(instance: Int, enabled: Boolean) {
        service?.setMemoryInterlock(instance, enabled)
    }

    fun pressMemory(instance: Int, index: Int) {
        service?.pressMemory(instance, index)
    }

    val messages: StateFlow<List<AppMessage>> = MessageLog.messages
    private val _lastReadMessage = MutableStateFlow(0L)
    val lastReadMessage: StateFlow<Long> = _lastReadMessage.asStateFlow()

    fun markMessagesRead() {
        MessageLog.messages.value.firstOrNull()?.let { _lastReadMessage.value = it.seq }
    }

    fun clearMessages() {
        MessageLog.clear()
    }

    private fun reportError(text: String) {
        _protocolError.value = text
        MessageLog.add(text)
    }

    fun showInputError(message: String) {
        _inputError.value = message
    }

    private fun startForegroundService() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            appContext.startForegroundService(serviceIntent)
        } else {
            appContext.startService(serviceIntent)
        }
    }

    private fun bindToService(flags: Int) {
        if (serviceBound) return
        serviceBound = appContext.bindService(serviceIntent, serviceConnection, flags)
        if (!serviceBound) {
            pendingConnection = null
            appContext.stopService(serviceIntent)
            _state.value = ConnectionState.Failed("Could not bind to the CAN service.")
        }
    }

    override fun onCleared() {
        stateJob?.cancel()
        receivedCountJob?.cancel()
        protocolErrorJob?.cancel()
        registrationJob?.cancel()
        cs2ConnectionJob?.cancel()
        paraCenterDatabaseJob?.cancel()
        paraCenterRegistrationJob?.cancel()
        registeredGuidJob?.cancel()
        systemStoppedJob?.cancel()
        systemArrayJob?.cancel()
        persistedSystemArrayJob?.cancel()
        keyboardCountJob?.cancel()
        keyboardStatesJob?.cancel()
        control80fCountJob?.cancel()
        memoryCountJob?.cancel()
        memoryInterlockJob?.cancel()
        memoryS88Job?.cancel()
        memoryLedsJob?.cancel()
        memoryOccupiedJob?.cancel()
        memoryRoutesJob?.cancel()
        if (serviceBound) {
            appContext.unbindService(serviceConnection)
            serviceBound = false
        }
        super.onCleared()
    }

    private data class PendingConnection(val host: String, val port: Int)
}

fun locomotiveState(systemArray: ByteArray, address: Int): LocomotiveState? {
    if (systemArray.size != Mbc89SystemArrayLayout.SIZE || address !in 1..Mbc89Locomotives.SLOT_80) {
        return null
    }
    return Mbc89Locomotives(systemArray.copyOf()).state(address)
}

private const val FUNCTION_DECODER_KEYS = 4
