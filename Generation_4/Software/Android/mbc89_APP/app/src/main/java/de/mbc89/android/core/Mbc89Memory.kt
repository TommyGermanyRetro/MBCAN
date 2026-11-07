package de.mbc89.android.core

sealed interface MemoryEntry

// presses: how often the same Keyboard key was pressed while recording (Memory 6043 manual 3.2.3);
// each further press lengthens the switching pulse by about 0.3 s, at most 7 presses.
data class MemoryCommand(val address: Int, val green: Boolean, val presses: Int = 1) : MemoryEntry {
    init {
        require(presses in 1..Mbc89Memory.PRESSES_MAX) { "Memory command press count is out of range." }
    }

    // 0 = command station default (6-byte form), otherwise 10 ms units for the 8-byte form.
    val switchTime: Int
        get() = if (presses == 1) 0 else presses * Mbc89Memory.PRESS_STEP_10MS
}

// Follow-up route of the same Memory, recorded instead of a switching command (manual 1.3, 3.1).
data class MemoryRouteCall(val localRoute: Int) : MemoryEntry {
    init {
        require(localRoute in 0 until Mbc89Memory.ROUTES_PER_INSTANCE) { "Memory route is out of range." }
    }
}

class MemoryPressResult(
    val commands: List<MemoryCommand>,
    val routesChanged: Boolean,
    val warningToken: Int? = null,
    val clearFlash: Boolean = false,
    val deleteShown: Boolean = false,
)

// One recorded route for the route list: its entries and whether it is set.
data class MemoryRouteView(
    val instance: Int,
    val localRoute: Int,
    val entries: List<MemoryEntry>,
    val active: Boolean,
)

// Route set by the automatic mode; the service sends its commands like a manual playback.
class MemoryRoutePlay(val instance: Int, val localRoute: Int, val commands: List<MemoryCommand>)

enum class LedState {
    OFF,
    ON,
    BLINK;

    companion object {
        fun of(on: Boolean) = if (on) ON else OFF
    }
}

// Memory 6043 (up to four instances, 24 routes A1..C8 each, up to 20 entries per route).
// Button indices as in the MSX port: 0 ex, 1 off, 2 in, 3 end, 4 clr, 5..28 routes A1..C8.
class Mbc89Memory(initialRoutes: List<List<MemoryEntry>>? = null) {
    private val routes: Array<List<MemoryEntry>> =
        Array(TOTAL_ROUTES) { initialRoutes?.getOrNull(it) ?: emptyList() }
    private val recording = IntArray(MEMORY_COUNT_MAX) { REC_IDLE }
    private val pendingWholeDelete = BooleanArray(MEMORY_COUNT_MAX)
    private val pendingRelease = BooleanArray(MEMORY_COUNT_MAX)
    private val automatic = BooleanArray(MEMORY_COUNT_MAX)
    private val active = BooleanArray(TOTAL_ROUTES)
    private val playing = BooleanArray(TOTAL_ROUTES)
    private val playLit = BooleanArray(TOTAL_ROUTES)
    private val endLit = BooleanArray(MEMORY_COUNT_MAX)
    private val clearLit = BooleanArray(MEMORY_COUNT_MAX)
    private val deleteShown = IntArray(MEMORY_COUNT_MAX) { REC_IDLE }
    private val warning = BooleanArray(TOTAL_ROUTES)
    private val warningActive = BooleanArray(MEMORY_COUNT_MAX)
    private val warningTokens = IntArray(MEMORY_COUNT_MAX)
    private var nextWarningToken = 1
    private var startedWarning: Int? = null

    // Automatic mode (manual 4): stored requests and occupied release inputs per route.
    private val pending = BooleanArray(TOTAL_ROUTES)
    private val releaseHeld = BooleanArray(TOTAL_ROUTES)
    private val autoPlays = ArrayList<MemoryRoutePlay>()

    // Interlocking (manual 3.3): a route crossing a valid route of the same Memory is not set.
    val interlock = BooleanArray(MEMORY_COUNT_MAX)

    // s88 module address of row A per Memory (1-93, 0 = none); rows B and C use the next two modules.
    val s88Module = IntArray(MEMORY_COUNT_MAX)

    // During a system stop s88 requests are ignored and stored requests are not set.
    var systemStopped = false

    fun route(instance: Int, localRoute: Int): List<MemoryEntry> =
        routes[globalRoute(instance, localRoute)]

    fun isValid(instance: Int, localRoute: Int): Boolean = route(instance, localRoute).isNotEmpty()

    fun routeViews(): List<MemoryRouteView> =
        (0 until TOTAL_ROUTES).filter { routes[it].isNotEmpty() }.map { global ->
            val instance = global / ROUTES_PER_INSTANCE
            MemoryRouteView(instance, global % ROUTES_PER_INSTANCE, routes[global], active[global])
        }

    fun hasRoutes(instance: Int): Boolean = (0 until ROUTES_PER_INSTANCE).any { isValid(instance, it) }

    fun isActive(instance: Int, localRoute: Int): Boolean = active[globalRoute(instance, localRoute)]

    fun isPending(instance: Int, localRoute: Int): Boolean = pending[globalRoute(instance, localRoute)]

    fun takeAutoPlays(): List<MemoryRoutePlay> = autoPlays.toList().also { autoPlays.clear() }

    fun ledState(instance: Int, index: Int): LedState {
        requireInstance(instance)
        val waiting = recording[instance] == REC_WAITING
        return when (index) {
            BUTTON_EX -> LedState.of(automatic[instance])
            BUTTON_OFF -> LedState.of(!automatic[instance])
            BUTTON_IN -> when {
                deleteShown[instance] >= 0 -> LedState.ON
                waiting && pendingWholeDelete[instance] -> LedState.OFF
                waiting -> LedState.BLINK
                recording[instance] >= 0 -> LedState.ON
                else -> LedState.OFF
            }
            BUTTON_END -> LedState.of(endLit[instance])
            BUTTON_CLR -> when {
                pendingRelease[instance] || (waiting && pendingWholeDelete[instance]) -> LedState.BLINK
                else -> LedState.of(clearLit[instance])
            }
            in FIRST_ROUTE_BUTTON until BUTTON_COUNT -> {
                val local = index - FIRST_ROUTE_BUTTON
                val global = globalRoute(instance, local)
                when {
                    warningActive[instance] && warning[global] -> LedState.BLINK
                    deleteShown[instance] == local -> LedState.ON
                    playing[global] -> LedState.of(playLit[global])
                    recording[instance] == local ->
                        if (routes[global].size == COMMANDS_PER_ROUTE - 1) LedState.BLINK else LedState.ON
                    waiting && routes[global].isNotEmpty() -> LedState.BLINK
                    else -> LedState.of(active[global])
                }
            }
            else -> throw IllegalArgumentException("Memory button index is out of range.")
        }
    }

    fun ledStates(instance: Int): List<LedState> = List(BUTTON_COUNT) { ledState(instance, it) }

    fun clearInstance(instance: Int) {
        requireInstance(instance)
        for (local in 0 until ROUTES_PER_INSTANCE) {
            val global = globalRoute(instance, local)
            routes[global] = emptyList()
            active[global] = false
            playing[global] = false
            pending[global] = false
        }
        recording[instance] = REC_IDLE
        pendingWholeDelete[instance] = false
        pendingRelease[instance] = false
        automatic[instance] = false
        endLit[instance] = false
        clearLit[instance] = false
        deleteShown[instance] = REC_IDLE
        clearWarning(instance)
    }

    fun setPlayLit(instance: Int, localRoute: Int, lit: Boolean) {
        playLit[globalRoute(instance, localRoute)] = lit
    }

    // Ends the short display of a deleted route ("input" and the route lit, manual 3.2.6); "end"
    // lights up afterwards unless another key was pressed meanwhile.
    fun finishDeleteDisplay(instance: Int): Boolean {
        requireInstance(instance)
        if (deleteShown[instance] < 0) return false
        deleteShown[instance] = REC_IDLE
        endLit[instance] = true
        return true
    }

    fun finishClearFlash(instance: Int) {
        requireInstance(instance)
        clearLit[instance] = false
    }

    fun finishPlay(instance: Int, localRoute: Int) {
        val global = globalRoute(instance, localRoute)
        playing[global] = false
        playLit[global] = false
    }

    // Ends the crossing warning after its time unless a newer warning replaced it; returns true if it
    // was still running.
    fun expireWarning(instance: Int, token: Int): Boolean {
        requireInstance(instance)
        if (!warningActive[instance] || warningTokens[instance] != token) return false
        clearWarning(instance)
        return true
    }

    fun press(instance: Int, index: Int): MemoryPressResult {
        requireInstance(instance)
        require(index in 0 until BUTTON_COUNT) { "Memory button index is out of range." }
        endLit[instance] = false
        deleteShown[instance] = REC_IDLE
        if (warningActive[instance]) {
            // Any key of this Memory ends the warning; pressing a blinking route sets it anyway (3.3.3).
            val blinking = index >= FIRST_ROUTE_BUTTON &&
                warning[globalRoute(instance, index - FIRST_ROUTE_BUTTON)]
            clearWarning(instance)
            if (blinking) return playRoute(globalRoute(instance, index - FIRST_ROUTE_BUTTON), force = true)
        }
        return when (index) {
            BUTTON_IN -> {
                recording[instance] = REC_WAITING
                pendingWholeDelete[instance] = false
                pendingRelease[instance] = false
                NOTHING
            }
            BUTTON_END -> {
                pendingRelease[instance] = false
                pendingWholeDelete[instance] = false
                recording[instance] = REC_IDLE
                endLit[instance] = true
                NOTHING
            }
            BUTTON_CLR -> pressClear(instance)
            BUTTON_EX -> {
                automatic[instance] = true
                NOTHING
            }
            BUTTON_OFF -> {
                // Back to manual mode; stored requests expire.
                automatic[instance] = false
                val first = instance * ROUTES_PER_INSTANCE
                for (global in first until first + ROUTES_PER_INSTANCE) pending[global] = false
                NOTHING
            }
            else -> pressRoute(instance, index - FIRST_ROUTE_BUTTON)
        }
    }

    private fun pressClear(instance: Int): MemoryPressResult {
        val state = recording[instance]
        return when {
            state == REC_WAITING -> {
                pendingWholeDelete[instance] = true
                NOTHING
            }
            state >= 0 -> {
                // Each press deletes the last entry and flashes "clear" once (manual 3.2.4).
                val global = globalRoute(instance, state)
                val entries = routes[global]
                clearLit[instance] = true
                if (entries.isNotEmpty()) routes[global] = entries.dropLast(1)
                MemoryPressResult(emptyList(), entries.isNotEmpty(), clearFlash = true)
            }
            else -> {
                pendingRelease[instance] = true
                NOTHING
            }
        }
    }

    private fun pressRoute(instance: Int, local: Int): MemoryPressResult {
        val global = globalRoute(instance, local)
        val state = recording[instance]
        if (state == REC_WAITING) {
            return if (pendingWholeDelete[instance]) {
                val changed = routes[global].isNotEmpty()
                routes[global] = emptyList()
                active[global] = false
                recording[instance] = REC_IDLE
                pendingWholeDelete[instance] = false
                deleteShown[instance] = local
                MemoryPressResult(emptyList(), changed, deleteShown = true)
            } else {
                recording[instance] = local
                NOTHING
            }
        }
        if (state >= 0) return recordRouteCall(instance, state, local)
        if (pendingRelease[instance]) {
            pendingRelease[instance] = false
            active[global] = false
            tryPending()
            return NOTHING
        }
        if (routes[global].isEmpty()) return NOTHING
        return playRoute(global, force = false)
    }

    // A route key pressed while recording adds that route as follow-up route; as 20th entry it ends
    // the recording automatically ("end" lights up, manual 3.1).
    private fun recordRouteCall(instance: Int, recordingLocal: Int, local: Int): MemoryPressResult {
        if (local == recordingLocal) return NOTHING
        val global = globalRoute(instance, recordingLocal)
        val entries = routes[global]
        if (entries.size >= COMMANDS_PER_ROUTE) return NOTHING
        routes[global] = entries + MemoryRouteCall(local)
        if (routes[global].size == COMMANDS_PER_ROUTE) {
            recording[instance] = REC_IDLE
            endLit[instance] = true
        }
        return MemoryPressResult(emptyList(), true)
    }

    private fun playRoute(global: Int, force: Boolean): MemoryPressResult {
        startedWarning = null
        val commands = ArrayList<MemoryCommand>()
        setRoute(global, force, warn = true, depth = 0, visited = mutableSetOf(global), commands)
        return MemoryPressResult(commands, false, startedWarning)
    }

    // Automatic call: no warning blink, only whether the route was set.
    private fun autoRoute(global: Int): Boolean {
        val commands = ArrayList<MemoryCommand>()
        val set = setRoute(global, force = false, warn = false, depth = 0, mutableSetOf(global), commands)
        if (set) {
            autoPlays += MemoryRoutePlay(global / ROUTES_PER_INSTANCE, global % ROUTES_PER_INSTANCE, commands)
        }
        return set
    }

    // Sets stored requests of the automatic mode once nothing is against them; fixed priority A1 before
    // A2 ... before C8 (manual 4.4). Called after every release.
    private fun tryPending() {
        if (systemStopped) return
        for (global in 0 until TOTAL_ROUTES) {
            if (!pending[global]) continue
            if (!automatic[global / ROUTES_PER_INSTANCE] || releaseHeld[global] || autoRoute(global)) {
                pending[global] = false
            }
        }
    }

    // s88 event: pin = 1000 x bus + 16 x (module in bus - 1) + contact 1-16, 31 modules per bus
    // (mbc88/mbc90).
    // Inputs 1-8 request routes 1-8 of the row, inputs 9-16 are their release inputs (manual 4.1, 4.4).
    fun s88Event(pin: Int, occupied: Boolean): Boolean {
        val bus = pin / 1000
        val rest = pin % 1000
        if (bus !in 1..S88_BUS_COUNT || rest == 0 || rest > S88_MODULES_PER_BUS * S88_CONTACTS) return false
        val module = (rest - 1) / S88_CONTACTS + 1 + S88_MODULES_PER_BUS * (bus - 1)
        val contact = (rest - 1) % S88_CONTACTS
        val instance = (0 until MEMORY_COUNT_MAX).firstOrNull {
            s88Module[it] != 0 && module in s88Module[it] until s88Module[it] + S88_ROWS
        } ?: return false
        val global = instance * ROUTES_PER_INSTANCE +
            (module - s88Module[instance]) * S88_ROUTES_PER_ROW + contact % S88_ROUTES_PER_ROW
        if (contact >= S88_ROUTES_PER_ROW) {
            // Release input: while occupied it blocks the request; occupying it releases the valid route so
            // a crossing one can be set - only with interlocking.
            releaseHeld[global] = occupied
            if (!occupied || !automatic[instance] || !interlock[instance] || !active[global]) return false
            active[global] = false
            tryPending()
            return true
        }
        if (!occupied || !automatic[instance] || routes[global].isEmpty() || systemStopped) return false
        if (interlock[instance] && releaseHeld[global]) return false
        if (!autoRoute(global) && interlock[instance]) pending[global] = true
        return true
    }

    // Sets a route and its follow-up routes in recorded order. Setting a route ends every valid route
    // that switches one of its accessories the other way; with interlocking a crossing valid route of
    // the same Memory blocks it instead - the called route warns, a follow-up route is skipped
    // silently (manual 3.3.3).
    private fun setRoute(
        global: Int,
        force: Boolean,
        warn: Boolean,
        depth: Int,
        visited: MutableSet<Int>,
        commands: MutableList<MemoryCommand>,
    ): Boolean {
        val instance = global / ROUTES_PER_INSTANCE
        val switches = routes[global].filterIsInstance<MemoryCommand>()
        if (interlock[instance] && !force) {
            val first = instance * ROUTES_PER_INSTANCE
            val blocking = (first until first + ROUTES_PER_INSTANCE)
                .filter { it != global && active[it] && crosses(it, switches) }
            if (blocking.isNotEmpty()) {
                if (depth == 0 && warn) startWarning(instance, blocking + global)
                return false
            }
        }
        for (other in 0 until TOTAL_ROUTES) {
            if (other != global && active[other] && crosses(other, switches)) active[other] = false
        }
        active[global] = true
        if (depth == 0) playing[global] = true
        for (entry in routes[global]) {
            when (entry) {
                is MemoryCommand -> commands += entry
                is MemoryRouteCall -> {
                    val target = instance * ROUTES_PER_INSTANCE + entry.localRoute
                    if (depth < FOLLOW_DEPTH_MAX && routes[target].isNotEmpty() && visited.add(target)) {
                        setRoute(target, force = false, warn = false, depth + 1, visited, commands)
                    }
                }
            }
        }
        return true
    }

    private fun crosses(other: Int, candidate: List<MemoryCommand>): Boolean =
        routes[other].filterIsInstance<MemoryCommand>().any { e ->
            candidate.any { c -> c.address == e.address && c.green != e.green }
        }

    private fun startWarning(instance: Int, blinking: List<Int>) {
        clearWarning(instance)
        blinking.forEach { warning[it] = true }
        warningActive[instance] = true
        warningTokens[instance] = nextWarningToken
        startedWarning = nextWarningToken
        nextWarningToken += 1
    }

    private fun clearWarning(instance: Int) {
        val first = instance * ROUTES_PER_INSTANCE
        for (global in first until first + ROUTES_PER_INSTANCE) warning[global] = false
        warningActive[instance] = false
    }

    // An accessory switched the other way anywhere (Keyboard, CS2/CS3, other device) ends every valid
    // route that contains it (manual 3.3.3); returns true if a route went out.
    fun noteSwitch(command: MemoryCommand): Boolean {
        var changed = false
        for (global in 0 until TOTAL_ROUTES) {
            if (active[global] && routes[global].any {
                    it is MemoryCommand && it.address == command.address && it.green != command.green
                }
            ) {
                active[global] = false
                changed = true
            }
        }
        if (changed) tryPending()
        return changed
    }

    // Records a command into every instance that is recording: the same accessory replaces its earlier
    // command, the same key again lengthens its switching time. Commands from the bus (CSx, MS2 toggle
    // their keys) never lengthen it.
    fun tapKeyboardCommand(command: MemoryCommand, lengthen: Boolean = true): Boolean {
        var changed = false
        for (instance in 0 until MEMORY_COUNT_MAX) {
            val local = recording[instance]
            if (local < 0) continue
            val global = globalRoute(instance, local)
            val entries = routes[global]
            val index = entries.indexOfFirst { it is MemoryCommand && it.address == command.address }
            val updated = when {
                index >= 0 -> entries.toMutableList().also {
                    val existing = it[index] as MemoryCommand
                    it[index] = if (existing.green == command.green) {
                        val presses = if (lengthen) existing.presses + 1 else existing.presses
                        existing.copy(presses = minOf(presses, PRESSES_MAX))
                    } else {
                        command.copy(presses = 1)
                    }
                }
                entries.size < COMMANDS_PER_ROUTE -> entries + command.copy(presses = 1)
                else -> entries
            }
            if (updated != entries) {
                routes[global] = updated
                changed = true
            }
        }
        return changed
    }

    // Last operating state (manual 1.3): set routes and extern/off per Memory, e.g. "0100...|1000".
    fun encodeState(): String =
        active.joinToString("") { if (it) "1" else "0" } + "|" +
            automatic.joinToString("") { if (it) "1" else "0" }

    // Restores the last operating state after a start; an unreadable state is ignored. As after
    // switching on (manual 2.3) "end" lights up; stored automatic requests are not kept.
    fun restoreState(text: String?) {
        val parts = text?.split("|")
        if (parts != null && parts.size == 2 && parts[0].length == TOTAL_ROUTES &&
            parts[1].length == MEMORY_COUNT_MAX && (parts[0] + parts[1]).all { it == '0' || it == '1' }
        ) {
            for (global in 0 until TOTAL_ROUTES) {
                active[global] = parts[0][global] == '1' && routes[global].isNotEmpty()
            }
            for (instance in 0 until MEMORY_COUNT_MAX) automatic[instance] = parts[1][instance] == '1'
        }
        endLit.fill(true)
    }

    fun encode(): String = routes.joinToString(";") { entries ->
        entries.joinToString(",") { entry ->
            when (entry) {
                is MemoryCommand -> "%04X:%d:%d".format(entry.address, if (entry.green) 1 else 0, entry.presses)
                is MemoryRouteCall -> "R%02d".format(entry.localRoute)
            }
        }
    }

    companion object {
        const val MEMORY_COUNT_MAX = 4
        const val ROUTES_PER_INSTANCE = 24
        const val COMMANDS_PER_ROUTE = 20
        const val PRESSES_MAX = 7
        const val PRESS_STEP_10MS = 30
        const val S88_MODULE_MAX = 93
        private const val S88_BUS_COUNT = 3
        private const val S88_MODULES_PER_BUS = 31
        private const val S88_CONTACTS = 16
        private const val S88_ROWS = 3
        private const val S88_ROUTES_PER_ROW = 8
        const val TOTAL_ROUTES = MEMORY_COUNT_MAX * ROUTES_PER_INSTANCE
        const val BUTTON_EX = 0
        const val BUTTON_OFF = 1
        const val BUTTON_IN = 2
        const val BUTTON_END = 3
        const val BUTTON_CLR = 4
        const val FIRST_ROUTE_BUTTON = 5
        const val BUTTON_COUNT = FIRST_ROUTE_BUTTON + ROUTES_PER_INSTANCE

        // At most five routes per call chain, like the MSX port.
        private const val FOLLOW_DEPTH_MAX = 4
        private const val REC_IDLE = -1
        private const val REC_WAITING = -2
        private val NOTHING = MemoryPressResult(emptyList(), false)

        // True if the three modules (rows A-C) starting at module share a module with another configured
        // Memory; 0 = none never overlaps.
        fun s88Overlaps(modules: List<Int>, count: Int, instance: Int, module: Int): Boolean =
            module != 0 && (0 until count).any { other ->
                other != instance && modules[other] != 0 && kotlin.math.abs(modules[other] - module) < S88_ROWS
            }

        fun decode(text: String): Mbc89Memory {
            val parts = text.split(";")
            require(parts.size == TOTAL_ROUTES) { "Stored Memory routes are incomplete." }
            val routes = parts.map { part ->
                if (part.isEmpty()) emptyList() else part.split(",").map(::decodeEntry)
            }
            require(routes.all { it.size <= COMMANDS_PER_ROUTE }) { "Stored Memory route is too long." }
            return Mbc89Memory(routes)
        }

        private fun decodeEntry(entry: String): MemoryEntry {
            if (entry.startsWith("R")) {
                val local = entry.substring(1).toIntOrNull()
                require(local != null && local in 0 until ROUTES_PER_INSTANCE) {
                    "Stored Memory route call is invalid."
                }
                return MemoryRouteCall(local)
            }
            val fields = entry.split(":")
            require(fields.size in 2..3 && fields[1] in listOf("0", "1")) {
                "Stored Memory command is invalid."
            }
            val address = fields[0].toIntOrNull(16)
            require(address != null && address in 0..0xffff) { "Stored Memory command is invalid." }
            val presses = fields.getOrNull(2)?.toIntOrNull() ?: 1
            require(presses in 1..PRESSES_MAX) { "Stored Memory command is invalid." }
            return MemoryCommand(address, fields[1] == "1", presses)
        }

        private fun globalRoute(instance: Int, localRoute: Int): Int {
            requireInstance(instance)
            require(localRoute in 0 until ROUTES_PER_INSTANCE) { "Memory route is out of range." }
            return instance * ROUTES_PER_INSTANCE + localRoute
        }

        private fun requireInstance(instance: Int) {
            require(instance in 0 until MEMORY_COUNT_MAX) { "Memory instance is out of range." }
        }
    }

    private fun globalRoute(instance: Int, localRoute: Int) = Companion.globalRoute(instance, localRoute)

    private fun requireInstance(instance: Int) = Companion.requireInstance(instance)
}
