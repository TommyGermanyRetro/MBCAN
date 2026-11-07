package de.mbc89.android

import android.Manifest
import androidx.core.view.WindowInsetsControllerCompat
import androidx.core.view.WindowInsetsCompat
import androidx.core.view.WindowCompat
import androidx.compose.ui.platform.LocalView
import androidx.compose.ui.platform.LocalConfiguration
import android.app.Activity
import android.content.pm.PackageManager
import android.os.Build
import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.enableEdgeToEdge
import androidx.activity.compose.rememberLauncherForActivityResult
import androidx.activity.compose.setContent
import androidx.activity.result.contract.ActivityResultContracts
import androidx.compose.foundation.Image
import androidx.compose.foundation.gestures.awaitEachGesture
import androidx.compose.foundation.gestures.awaitFirstDown
import androidx.compose.foundation.gestures.waitForUpOrCancellation
import androidx.compose.foundation.layout.BoxWithConstraints
import androidx.compose.foundation.layout.offset
import androidx.compose.foundation.layout.requiredSize
import androidx.compose.ui.graphics.graphicsLayer
import androidx.compose.ui.input.pointer.pointerInput
import androidx.compose.foundation.clickable
import de.mbc89.android.core.Mbc89SystemArrayLayout
import androidx.compose.foundation.shape.RoundedCornerShape
import kotlin.math.roundToInt
import androidx.compose.ui.unit.Constraints
import androidx.compose.ui.layout.layout
import androidx.compose.material3.Switch
import androidx.compose.ui.draw.clip
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.ui.unit.Dp
import androidx.compose.ui.draw.clipToBounds
import androidx.compose.foundation.border
import androidx.compose.foundation.pager.HorizontalPager
import androidx.compose.foundation.pager.rememberPagerState
import androidx.compose.runtime.rememberCoroutineScope
import kotlinx.coroutines.launch
import androidx.compose.foundation.background
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.safeDrawingPadding
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.verticalScroll
import androidx.compose.foundation.text.KeyboardOptions
import androidx.compose.material3.AlertDialog
import androidx.compose.material3.Button
import androidx.compose.material3.ButtonDefaults
import androidx.compose.material3.OutlinedButton
import androidx.compose.foundation.layout.PaddingValues
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.material3.TextButton
import androidx.compose.material3.Card
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.OutlinedTextField
import androidx.compose.material3.Surface
import androidx.compose.material3.Tab
import androidx.compose.material3.TabRow
import androidx.compose.runtime.remember
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.mutableIntStateOf
import kotlinx.coroutines.delay
import androidx.compose.runtime.saveable.rememberSaveable
import androidx.compose.runtime.setValue
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.text.SpanStyle
import androidx.compose.ui.text.buildAnnotatedString
import androidx.compose.ui.text.withStyle
import androidx.compose.ui.text.input.KeyboardType
import androidx.compose.ui.unit.dp
import androidx.lifecycle.compose.collectAsStateWithLifecycle
import androidx.lifecycle.viewmodel.compose.viewModel
import de.mbc89.android.core.Mbc89Core
import de.mbc89.android.core.Mbc89Memory
import de.mbc89.android.transport.ConnectionState

class MainActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()
        setContent {
            MaterialTheme {
                Surface(
                    modifier = Modifier
                        .fillMaxSize()
                        .safeDrawingPadding(),
                ) {
                    var splashDone by rememberSaveable { mutableStateOf(false) }
                    if (splashDone) MainScreen() else SplashScreen { splashDone = true }
                }
            }
        }
    }
}

@Composable
private fun SplashScreen(onFinished: () -> Unit) {
    var seconds by remember { mutableIntStateOf(0) }
    LaunchedEffect(Unit) {
        while (seconds < SPLASH_SECONDS) {
            delay(1_000)
            seconds += 1
        }
        onFinished()
    }
    Column(
        modifier = Modifier
            .fillMaxSize()
            .background(Color.White),
        horizontalAlignment = Alignment.CenterHorizontally,
        verticalArrangement = Arrangement.Center,
    ) {
        Image(
            painter = painterResource(R.drawable.mbc_start),
            contentDescription = "MBCAN - Fernsteuerung einer Märklin-Modelleisenbahn",
            modifier = Modifier.width(690.dp),
        )
        Spacer(modifier = Modifier.height(32.dp))
        Text(
            buildAnnotatedString {
                append("Loading")
                append(".".repeat(seconds))
                withStyle(SpanStyle(color = Color.Transparent)) {
                    append(".".repeat(SPLASH_SECONDS - seconds))
                }
            },
            color = Color.Black,
            style = MaterialTheme.typography.headlineSmall,
        )
    }
}

private const val SPLASH_SECONDS = 5

@Composable
private fun MainScreen(viewModel: ConnectionViewModel = viewModel()) {
    val state by viewModel.state.collectAsStateWithLifecycle()
    val systemStopped by viewModel.systemStopped.collectAsStateWithLifecycle()
    val keyboardCount by viewModel.keyboardCount.collectAsStateWithLifecycle()
    val keyboardStates by viewModel.keyboardStates.collectAsStateWithLifecycle()
    val systemArray by viewModel.systemArray.collectAsStateWithLifecycle()
    val persistedSystemArray by viewModel.persistedSystemArray.collectAsStateWithLifecycle()
    val controlUnits by viewModel.controlUnits.collectAsStateWithLifecycle()
    val control80fCount by viewModel.control80fCount.collectAsStateWithLifecycle()
    val memoryCount by viewModel.memoryCount.collectAsStateWithLifecycle()
    val memoryLeds by viewModel.memoryLeds.collectAsStateWithLifecycle()
    val messages by viewModel.messages.collectAsStateWithLifecycle()
    val lastReadMessage by viewModel.lastReadMessage.collectAsStateWithLifecycle()
    val unread = messages.count { it.seq > lastReadMessage }
    val tabs = listOf(
        TAB_PANEL to "Panel",
        TAB_ROUTES to "Fahrstraßen",
        TAB_CONNECTION to "Verbindung",
        TAB_SYSTEM_ARRAY to "Systemarray",
        TAB_MESSAGES to if (unread > 0) "Meldungen ($unread)" else "Meldungen",
        TAB_MANUAL to "Anleitung",
    )
    var selectedTab by rememberSaveable { mutableStateOf(TAB_PANEL) }
    // Phones: panels are shrunk, so the panel tab runs full screen; the system bars come back with a
    // swipe from the edge.
    val compact = LocalConfiguration.current.smallestScreenWidthDp < COMPACT_SCREEN_DP
    val view = LocalView.current
    LaunchedEffect(compact, selectedTab) {
        val window = (view.context as? Activity)?.window ?: return@LaunchedEffect
        val controller = WindowCompat.getInsetsController(window, view)
        if (compact && selectedTab == TAB_PANEL) {
            controller.systemBarsBehavior = WindowInsetsControllerCompat.BEHAVIOR_SHOW_TRANSIENT_BARS_BY_SWIPE
            controller.hide(WindowInsetsCompat.Type.systemBars())
        } else {
            controller.show(WindowInsetsCompat.Type.systemBars())
        }
    }

    Column(modifier = Modifier.fillMaxSize()) {
        TabRow(selectedTabIndex = tabs.indexOfFirst { it.first == selectedTab }) {
            for ((id, title) in tabs) {
                Tab(
                    selected = selectedTab == id,
                    onClick = { selectedTab = id },
                    text = { Text(title) },
                )
            }
        }
        when (selectedTab) {
            TAB_CONNECTION -> ConnectionScreen(viewModel)
            TAB_SYSTEM_ARRAY -> SystemArrayScreen(systemArray, persistedSystemArray)
            TAB_ROUTES -> RoutesScreen(viewModel)
            TAB_MANUAL -> ManualScreen()
            TAB_MESSAGES -> {
                LaunchedEffect(messages) { viewModel.markMessagesRead() }
                MessagesScreen(messages, onClear = viewModel::clearMessages)
            }
            else -> PanelCarousel(
                keyboardCount = keyboardCount,
                control80fCount = control80fCount,
                memoryCount = memoryCount,
                compact = compact,
                miniature = { id -> PanelMiniature(id) },
                topRight = {
                    PanelConnectionControl(viewModel, onOpenConnection = { selectedTab = TAB_CONNECTION })
                },
                topLeft = { id ->
                    if (id.startsWith(PAGE_KEYBOARD_PREFIX)) {
                        KeyboardProtocolMatrix(systemArray, id.removePrefix(PAGE_KEYBOARD_PREFIX).toInt() - 1)
                    }
                },
            ) { id ->
                if (id.startsWith(PAGE_MEMORY_PREFIX)) {
                    val instance = id.removePrefix(PAGE_MEMORY_PREFIX).toInt() - 1
                    MemoryPanel(
                        number = instance + 1,
                        leds = memoryLeds.getOrElse(instance) { emptyList() },
                        enabled = state is ConnectionState.Connected && !systemStopped,
                        onPress = { viewModel.pressMemory(instance, it) },
                    )
                } else if (id.startsWith(PAGE_CONTROL_PREFIX)) {
                    val index = id.removePrefix(PAGE_CONTROL_PREFIX).toInt()
                    val activeSlots = controlUnits.take(1 + control80fCount)
                    val slot = activeSlots.getOrElse(index) { ControlUnitSlot() }
                    val address = slot.address.takeIf { it.length == 2 }?.toInt()
                    val functionAddress = slot.functionTarget.takeIf { it.length == 2 }?.toInt()
                    ControlUnitPanel(
                        state = ControlUnitState(
                            addressText = slot.address,
                            locomotive = address?.let { locomotiveState(systemArray, it) },
                            stopped = systemStopped,
                            ownedElsewhere = addressOwnedElsewhere(activeSlots, index),
                            functionAddressText = slot.functionTarget,
                            functionLocomotive = functionAddress?.let { locomotiveState(systemArray, it) },
                            entry = slot.entry,
                        ),
                        connected = state is ConnectionState.Connected,
                        onDigit = { viewModel.enterControlUnitDigit(index, it) },
                        onCommand = { viewModel.controlUnitAction(index, it) },
                        onStopGo = viewModel::setSystemGo,
                        label = if (index == 0) "control unit" else "control 80f",
                        onSelectAddress = { viewModel.selectControlUnitAddress(index, it) },
                    )
                } else {
                    val kbIndex = id.removePrefix(PAGE_KEYBOARD_PREFIX).toInt() - 1
                    KeyboardPanel(
                        number = kbIndex + 1,
                        state = keyboardStates.getOrElse(kbIndex) { 0 },
                        enabled = state is ConnectionState.Connected && !systemStopped,
                        onSwitch = { column, green ->
                            viewModel.switchAccessory(kbIndex, column, green)
                        },
                    )
                }
            }
        }
    }
}

// Idle panel without live state for the overview in the footer.
@Composable
private fun PanelMiniature(id: String) {
    when {
        id.startsWith(PAGE_MEMORY_PREFIX) -> MemoryPanel(
            number = id.removePrefix(PAGE_MEMORY_PREFIX).toInt(),
            leds = emptyList(),
            enabled = false,
            onPress = {},
        )
        id.startsWith(PAGE_CONTROL_PREFIX) -> ControlUnitPanel(
            state = ControlUnitState(addressText = "", locomotive = null, stopped = false),
            connected = false,
            onDigit = {},
            onCommand = {},
            onStopGo = {},
            label = if (id == "${PAGE_CONTROL_PREFIX}0") "control unit" else "control 80f",
        )
        else -> KeyboardPanel(
            number = id.removePrefix(PAGE_KEYBOARD_PREFIX).toInt(),
            state = 0,
            enabled = false,
            onSwitch = { _, _ -> },
        )
    }
}

// Carousel like the MSX: Memory n ... Memory 1 | Keyboard n ... Keyboard 1 | Control Unit |
// Control 80f 1 ... n, opened on the Control Unit every time the panel tab is entered.
@Composable
private fun PanelCarousel(
    keyboardCount: Int,
    control80fCount: Int,
    memoryCount: Int,
    compact: Boolean,
    miniature: @Composable (id: String) -> Unit,
    topRight: @Composable () -> Unit,
    topLeft: @Composable (id: String) -> Unit,
    panel: @Composable (id: String) -> Unit,
) {
    val pages = (memoryCount downTo 1).map { "$PAGE_MEMORY_PREFIX$it" } +
        (keyboardCount downTo 1).map { "$PAGE_KEYBOARD_PREFIX$it" } +
        "${PAGE_CONTROL_PREFIX}0" +
        (1..control80fCount).map { "$PAGE_CONTROL_PREFIX$it" }
    val controlUnitPage = memoryCount + keyboardCount
    var currentPageId by remember { mutableStateOf("${PAGE_CONTROL_PREFIX}0") }
    var overviewShown by rememberSaveable { mutableStateOf(false) }
    val pagerState = rememberPagerState(initialPage = controlUnitPage, pageCount = { pages.size })
    val scope = rememberCoroutineScope()
    LaunchedEffect(pages) {
        val index = pages.indexOf(currentPageId)
        pagerState.scrollToPage(if (index >= 0) index else controlUnitPage)
    }
    LaunchedEffect(pagerState.currentPage, pages) {
        pages.getOrNull(pagerState.currentPage)?.let { currentPageId = it }
    }

    Column(
        modifier = Modifier
            .fillMaxSize()
            .background(PanelBackgroundGrey),
    ) {
        // On small screens the whole panel area (panel, side miniatures, connection switch) shrinks
        // evenly so that it fits the height; on the tablet the factor stays 1.
        Box(
            modifier = Modifier.fillMaxWidth().weight(1f),
            contentAlignment = Alignment.Center,
        ) {
            Box(
                modifier = Modifier
                    .fillMaxSize()
                    .scaleToHeight(PANEL_AREA_HEIGHT),
            ) {
                HorizontalPager(
                    state = pagerState,
                    userScrollEnabled = false,
                    modifier = Modifier.fillMaxSize(),
                ) { page ->
                    val id = pages.getOrNull(page) ?: return@HorizontalPager
                    Box(
                        modifier = Modifier
                            .fillMaxSize()
                            .verticalScroll(rememberScrollState())
                            .padding(horizontal = 24.dp, vertical = 12.dp),
                        contentAlignment = Alignment.TopCenter,
                    ) {
                        panel(id)
                    }
                }
                // Miniatures of the neighbouring panels, only for switching; centred on the panel height
                // and in the free space beside the panel.
                BoxWithConstraints(modifier = Modifier.fillMaxSize()) {
                    val side = ((maxWidth - PANEL_SIZE) / 2).coerceAtLeast(THUMBNAIL_SIZE)
                    val inset = (side - THUMBNAIL_SIZE) / 2
                    val top = PANEL_TOP + (PANEL_SIZE - THUMBNAIL_SIZE) / 2
                    Box(
                        modifier = Modifier
                            .offset(x = maxWidth - side)
                            .size(width = side, height = top),
                        contentAlignment = Alignment.Center,
                    ) {
                        topRight()
                    }
                    pages.getOrNull(pagerState.currentPage)?.let { id ->
                        Box(
                            modifier = Modifier.size(width = side, height = top),
                            contentAlignment = Alignment.Center,
                        ) {
                            topLeft(id)
                        }
                    }
                    pages.getOrNull(pagerState.currentPage - 1)?.let { id ->
                        PanelThumbnail(modifier = Modifier.offset(x = inset, y = top), onClick = {
                            scope.launch { pagerState.animateScrollToPage(pagerState.currentPage - 1) }
                        }) { panel(id) }
                    }
                    pages.getOrNull(pagerState.currentPage + 1)?.let { id ->
                        PanelThumbnail(
                            modifier = Modifier.offset(x = maxWidth - side + inset, y = top),
                            onClick = {
                                scope.launch { pagerState.animateScrollToPage(pagerState.currentPage + 1) }
                            },
                        ) { panel(id) }
                    }
                }
            }
            if (compact) {
                TextButton(
                    onClick = { overviewShown = !overviewShown },
                    modifier = Modifier.align(Alignment.BottomStart),
                ) {
                    Text(if (overviewShown) "Übersicht ausblenden" else "Übersicht einblenden")
                }
            }
        }
        if (!compact || overviewShown) {
            PanelOverview(
                pages = pages,
                currentPage = pagerState.currentPage,
                miniature = miniature,
                onSelect = { page -> scope.launch { pagerState.animateScrollToPage(page) } },
            )
        }
    }
}

// Footer with all 29 panel places in carousel order; the Control Unit keeps its place no matter how
// many devices are configured, unused places stay as faint frames, the shown panel is framed yellow.
@Composable
private fun PanelOverview(
    pages: List<String>,
    currentPage: Int,
    miniature: @Composable (id: String) -> Unit,
    onSelect: (page: Int) -> Unit,
) {
    val places = (Mbc89Memory.MEMORY_COUNT_MAX downTo 1).map { "$PAGE_MEMORY_PREFIX$it" } +
        (Mbc89Core.KEYBOARD_COUNT_MAX downTo 1).map { "$PAGE_KEYBOARD_PREFIX$it" } +
        "${PAGE_CONTROL_PREFIX}0" +
        (1..Mbc89Core.CONTROL80F_COUNT_MAX).map { "$PAGE_CONTROL_PREFIX$it" }
    BoxWithConstraints(modifier = Modifier.fillMaxWidth().padding(vertical = OVERVIEW_PADDING)) {
        // The 29 places shrink to the screen width on small screens.
        val slot = ((maxWidth - OVERVIEW_SPACING * (places.size - 1)) / places.size).coerceAtMost(OVERVIEW_SLOT_SIZE)
        Row(
            modifier = Modifier.fillMaxWidth(),
            horizontalArrangement = Arrangement.spacedBy(OVERVIEW_SPACING, Alignment.CenterHorizontally),
        ) {
            for (id in places) {
                val page = pages.indexOf(id)
                val frame = when {
                    page < 0 -> OverviewEmptyFrame
                    page == currentPage -> OverviewActiveFrame
                    else -> Color.Transparent
                }
                Box(
                    modifier = Modifier
                        .size(slot)
                        .border(OVERVIEW_FRAME_WIDTH, frame),
                    contentAlignment = Alignment.Center,
                ) {
                    if (page >= 0) {
                        PanelThumbnail(
                            modifier = Modifier,
                            onClick = { onSelect(page) },
                            size = slot - OVERVIEW_FRAME_WIDTH * 2,
                        ) { miniature(id) }
                    }
                }
            }
        }
    }
}

// Lays the content out for at least the given height and shrinks it evenly to the available height.
// A plain layout modifier instead of BoxWithConstraints: a subcomposition around the pager made Compose
// update modifiers of recycled pager pages ("modifier is updated when deactivated").
private fun Modifier.scaleToHeight(designHeight: Dp): Modifier = layout { measurable, constraints ->
    val scale = (constraints.maxHeight / designHeight.toPx()).coerceAtMost(1f)
    val width = (constraints.maxWidth / scale).roundToInt()
    val height = (constraints.maxHeight / scale).roundToInt()
    val placeable = measurable.measure(Constraints.fixed(width, height))
    layout(constraints.maxWidth, constraints.maxHeight) {
        placeable.placeWithLayer((constraints.maxWidth - width) / 2, (constraints.maxHeight - height) / 2) {
            scaleX = scale
            scaleY = scale
        }
    }
}

@Composable
private fun PanelThumbnail(
    modifier: Modifier,
    onClick: () -> Unit,
    size: Dp = THUMBNAIL_SIZE,
    panel: @Composable () -> Unit,
) {
    Box(modifier = modifier.size(size).clipToBounds(), contentAlignment = Alignment.Center) {
        Box(
            modifier = Modifier
                .requiredSize(PANEL_SIZE)
                .graphicsLayer {
                    scaleX = size / PANEL_SIZE
                    scaleY = size / PANEL_SIZE
                },
        ) {
            panel()
        }
        Box(
            modifier = Modifier
                .matchParentSize()
                .pointerInput(onClick) {
                    awaitEachGesture {
                        awaitFirstDown().consume()
                        val up = waitForUpOrCancellation()
                        if (up != null) {
                            up.consume()
                            onClick()
                        }
                    }
                },
        )
    }
}

private val PANEL_SIZE = 600.dp
private val PANEL_TOP = 12.dp
private val THUMBNAIL_SIZE = 200.dp
private val PANEL_AREA_HEIGHT = PANEL_SIZE + PANEL_TOP * 2
private val OVERVIEW_SLOT_SIZE = 40.dp
private val OVERVIEW_FRAME_WIDTH = 2.dp
private val OVERVIEW_SPACING = 4.dp
private val OVERVIEW_PADDING = 8.dp
private val OverviewActiveFrame = Color(0xFFFFD600)
private val OverviewEmptyFrame = Color(0xFFBDBDBD)

private val PanelBackgroundGrey = Color(0xFFE6E6E6)

private const val TAB_CONNECTION = "connection"
private const val TAB_SYSTEM_ARRAY = "systemArray"
private const val TAB_ROUTES = "routes"
private const val TAB_MESSAGES = "messages"
private const val TAB_MANUAL = "manual"
private const val TAB_PANEL = "panel"
private const val COMPACT_SCREEN_DP = 600
private const val PAGE_CONTROL_PREFIX = "control:"
private const val PAGE_KEYBOARD_PREFIX = "keyboard:"
private const val PAGE_MEMORY_PREFIX = "memory:"

@Composable
private fun ConnectionScreen(viewModel: ConnectionViewModel = viewModel()) {
    val state by viewModel.state.collectAsStateWithLifecycle()
    val receivedCount by viewModel.receivedCount.collectAsStateWithLifecycle()
    val moduleRegistered by viewModel.moduleRegistered.collectAsStateWithLifecycle()
    val paraCenterDatabaseValid by viewModel.paraCenterDatabaseValid.collectAsStateWithLifecycle()
    val paraCenterRegistrationStep by viewModel.paraCenterRegistrationStep.collectAsStateWithLifecycle()
    val registeredGuid by viewModel.registeredGuid.collectAsStateWithLifecycle()
    val cs2Connected by viewModel.cs2Connected.collectAsStateWithLifecycle()
    val systemStopped by viewModel.systemStopped.collectAsStateWithLifecycle()
    val keyboardCount by viewModel.keyboardCount.collectAsStateWithLifecycle()
    val control80fCount by viewModel.control80fCount.collectAsStateWithLifecycle()
    val memoryCount by viewModel.memoryCount.collectAsStateWithLifecycle()
    val memoryInterlock by viewModel.memoryInterlock.collectAsStateWithLifecycle()
    val memoryS88 by viewModel.memoryS88.collectAsStateWithLifecycle()
    val memoryOccupied by viewModel.memoryOccupied.collectAsStateWithLifecycle()
    val inputError by viewModel.inputError.collectAsStateWithLifecycle()
    val protocolError by viewModel.protocolError.collectAsStateWithLifecycle()
    val context = LocalContext.current
    val savedEndpoint by viewModel.savedEndpoint.collectAsStateWithLifecycle()
    var host by rememberSaveable { mutableStateOf("") }
    var clearMemoryNumber by remember { mutableStateOf<Int?>(null) }
    var portText by rememberSaveable { mutableStateOf("15731") }
    var endpointApplied by rememberSaveable { mutableStateOf(false) }
    LaunchedEffect(savedEndpoint) {
        val endpoint = savedEndpoint
        if (!endpointApplied && endpoint != null) {
            host = endpoint.first
            portText = endpoint.second.toString()
            endpointApplied = true
        }
    }
    val connect = {
        val port = portText.toIntOrNull()
        if (port != null && port in 1..65535) {
            viewModel.connect(host.trim(), port)
        } else {
            viewModel.showInputError("TCP-Port muss zwischen 1 und 65535 liegen.")
        }
    }
    val notificationPermissionLauncher = rememberLauncherForActivityResult(
        ActivityResultContracts.RequestPermission(),
    ) { granted ->
        if (granted) {
            connect()
        } else {
            viewModel.showInputError(
                "Zum Starten des CAN-Dienstes wird die Benachrichtigungsberechtigung benötigt.",
            )
        }
    }

    Column(
        modifier = Modifier
            .fillMaxSize()
            .verticalScroll(rememberScrollState())
            .padding(24.dp),
        verticalArrangement = Arrangement.spacedBy(10.dp),
    ) {
        Text("mbc89 – CAN-TCP-Verbindung zum ESP32 (mbc80)", style = MaterialTheme.typography.headlineSmall)

        Row(horizontalArrangement = Arrangement.spacedBy(12.dp)) {
            OutlinedTextField(
                value = host,
                onValueChange = { host = it },
                label = { Text("IP-Adresse oder Hostname") },
                singleLine = true,
                modifier = Modifier.weight(2f),
            )
            OutlinedTextField(
                value = portText,
                onValueChange = { portText = it.filter(Char::isDigit) },
                label = { Text("TCP-Port") },
                keyboardOptions = KeyboardOptions(keyboardType = KeyboardType.Number),
                singleLine = true,
                modifier = Modifier.weight(1f),
            )
        }

        Row {
            Button(
                enabled = state !is ConnectionState.Connecting &&
                    state !is ConnectionState.Connected &&
                    state !is ConnectionState.Reconnecting,
                onClick = {
                    val port = portText.toIntOrNull()
                    if (host.isBlank()) {
                        viewModel.showInputError("Bitte die IP-Adresse oder den Hostnamen eingeben.")
                    } else if (port == null || port !in 1..65535) {
                        viewModel.showInputError("TCP-Port muss zwischen 1 und 65535 liegen.")
                    } else if (
                        Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU &&
                        context.checkSelfPermission(Manifest.permission.POST_NOTIFICATIONS) !=
                        PackageManager.PERMISSION_GRANTED
                    ) {
                        notificationPermissionLauncher.launch(Manifest.permission.POST_NOTIFICATIONS)
                    } else {
                        connect()
                    }
                },
            ) {
                Text("Verbinden")
            }
            Spacer(modifier = Modifier.width(12.dp))
            Button(
                enabled = state is ConnectionState.Connected ||
                    state is ConnectionState.Connecting ||
                    state is ConnectionState.Reconnecting,
                onClick = viewModel::disconnect,
            ) {
                Text("Trennen")
            }
        }

        Card(modifier = Modifier.fillMaxWidth()) {
            Row(modifier = Modifier.padding(16.dp)) {
                Column(
                    modifier = Modifier.weight(1f),
                    verticalArrangement = Arrangement.spacedBy(8.dp),
                ) {
                    Text("Status", style = MaterialTheme.typography.titleMedium)
                    Text(state.label)
                    Text(
                        registeredGuid?.let { guid ->
                            "GUiD: " + guid.joinToString(" ") { "%02X".format(it) }
                        } ?: "GUiD: nicht registriert",
                    )
                    Text(
                        if (paraCenterRegistrationStep > 0) {
                            "ParaCenter-Anmeldung: Schritt $paraCenterRegistrationStep/42"
                        } else if (moduleRegistered) {
                            "ParaCenter-Anmeldung: abgeschlossen"
                        } else if (paraCenterDatabaseValid) {
                            "ParaCenter-Anmeldung: wartet auf PC_NEU"
                        } else {
                            "ParaCenter-Anmeldung: wartet auf gültige Datenbank"
                        },
                    )
                    val failureReason = when (val current = state) {
                        is ConnectionState.Failed -> current.reason
                        is ConnectionState.Reconnecting -> current.reason
                        else -> null
                    }
                    if (failureReason != null) {
                        Text(text = failureReason, color = MaterialTheme.colorScheme.error)
                    }
                    Text(if (cs2Connected) "CS2/CS3: erkannt" else "CS2/CS3: nicht erkannt")
                    Text(if (systemStopped) "Anlage: Stopp" else "Anlage: Go")
                    Text("Empfangene CAN-Nachrichten: $receivedCount")
                }
                Column(
                    horizontalAlignment = Alignment.End,
                    verticalArrangement = Arrangement.spacedBy(8.dp),
                ) {
                    Text("Geräte", style = MaterialTheme.typography.titleMedium)
                    StepperRow(
                        label = "Keyboards",
                        value = keyboardCount.toString(),
                        minusEnabled = state is ConnectionState.Connected && keyboardCount > 0,
                        plusEnabled = state is ConnectionState.Connected &&
                            keyboardCount < Mbc89Core.KEYBOARD_COUNT_MAX,
                        onMinus = { viewModel.setKeyboardCount(keyboardCount - 1) },
                        onPlus = { viewModel.setKeyboardCount(keyboardCount + 1) },
                    )
                    StepperRow(
                        label = "Control 80f",
                        value = control80fCount.toString(),
                        minusEnabled = state is ConnectionState.Connected && control80fCount > 0,
                        plusEnabled = state is ConnectionState.Connected &&
                            control80fCount < Mbc89Core.CONTROL80F_COUNT_MAX,
                        onMinus = { viewModel.setControl80fCount(control80fCount - 1) },
                        onPlus = { viewModel.setControl80fCount(control80fCount + 1) },
                    )
                    StepperRow(
                        label = "Memory",
                        value = memoryCount.toString(),
                        minusEnabled = state is ConnectionState.Connected && memoryCount > 0,
                        plusEnabled = state is ConnectionState.Connected &&
                            memoryCount < Mbc89Memory.MEMORY_COUNT_MAX,
                        onMinus = { viewModel.setMemoryCount(memoryCount - 1) },
                        onPlus = { viewModel.setMemoryCount(memoryCount + 1) },
                    )
                    if (memoryCount > 0) {
                        val connected = state is ConnectionState.Connected
                        MemoryColumnsRow("Verriegelung:", memoryCount) { number ->
                            val interlocked = memoryInterlock[number - 1]
                            MemoryButton(number, interlocked, connected) {
                                viewModel.setMemoryInterlock(number - 1, !interlocked)
                            }
                        }
                        MemoryColumnsRow("s88 Reihe A:", memoryCount) { number ->
                            S88ModuleField(
                                number = number,
                                module = memoryS88[number - 1],
                                enabled = connected,
                                overlaps = { Mbc89Memory.s88Overlaps(memoryS88, memoryCount, number - 1, it) },
                                onModule = { viewModel.setMemoryS88(number - 1, it) },
                            )
                        }
                        MemoryColumnsRow("Leeren:", memoryCount) { number ->
                            MemoryButton(number, memoryOccupied[number - 1], connected) {
                                clearMemoryNumber = number
                            }
                        }
                    }
                }
            }
        }
        clearMemoryNumber?.let { number ->
            AlertDialog(
                onDismissRequest = { clearMemoryNumber = null },
                title = { Text("Memory $number leeren") },
                text = { Text("Alle Fahrstraßen dieses Memory werden gelöscht.") },
                confirmButton = {
                    TextButton(onClick = {
                        viewModel.clearMemory(number - 1)
                        clearMemoryNumber = null
                    }) { Text("Leeren") }
                },
                dismissButton = {
                    TextButton(onClick = { clearMemoryNumber = null }) { Text("Abbrechen") }
                },
            )
        }
        if (inputError != null) {
            Text(
                text = inputError.orEmpty(),
                color = MaterialTheme.colorScheme.error,
            )
        }
        if (protocolError != null) {
            Text(
                text = protocolError.orEmpty(),
                color = MaterialTheme.colorScheme.error,
            )
        }
        Spacer(modifier = Modifier.height(8.dp))
    }
}

@Composable
private fun StepperRow(
    label: String,
    value: String,
    minusEnabled: Boolean,
    plusEnabled: Boolean,
    onMinus: () -> Unit,
    onPlus: () -> Unit,
) {
    Row(verticalAlignment = Alignment.CenterVertically) {
        Text(label, style = MaterialTheme.typography.bodyMedium, modifier = Modifier.width(96.dp))
        Button(
            enabled = minusEnabled,
            onClick = onMinus,
            contentPadding = PaddingValues(0.dp),
            modifier = Modifier.size(width = 40.dp, height = 36.dp),
        ) {
            Text("−")
        }
        Text(
            value,
            style = MaterialTheme.typography.titleMedium,
            textAlign = TextAlign.Center,
            modifier = Modifier.width(84.dp),
        )
        Button(
            enabled = plusEnabled,
            onClick = onPlus,
            contentPadding = PaddingValues(0.dp),
            modifier = Modifier.size(width = 40.dp, height = 36.dp),
        ) {
            Text("+")
        }
    }
}

private val ConnectionState.label: String
    get() = when (this) {
        ConnectionState.Disconnected -> "Getrennt"
        ConnectionState.Connecting -> "Verbindung wird aufgebaut …"
        is ConnectionState.Connected -> "Verbunden mit $host:$port"
        is ConnectionState.Failed -> "Verbindungsfehler"
        is ConnectionState.Reconnecting -> "Verbindung verloren – neuer Versuch …"
    }

// Memory button M1..M4: filled = on/occupied, outlined = off/empty; the filled look stays visible
// while disabled so the stored state can be read without a connection.
@Composable
private fun MemoryButton(number: Int, filled: Boolean, enabled: Boolean, onClick: () -> Unit) {
    val modifier = Modifier.size(width = 40.dp, height = 36.dp)
    if (filled) {
        Button(
            onClick = onClick,
            enabled = enabled,
            contentPadding = PaddingValues(0.dp),
            modifier = modifier,
            colors = ButtonDefaults.buttonColors(
                disabledContainerColor = MaterialTheme.colorScheme.primary.copy(alpha = DISABLED_FILL_ALPHA),
                disabledContentColor = MaterialTheme.colorScheme.onPrimary,
            ),
        ) {
            Text("M$number")
        }
    } else {
        OutlinedButton(
            onClick = onClick,
            enabled = enabled,
            contentPadding = PaddingValues(0.dp),
            modifier = modifier,
        ) {
            Text("M$number")
        }
    }
}

private const val DISABLED_FILL_ALPHA = 0.38f

// s88 module of row A for one Memory (0 = none); a valid entry that does not overlap the modules of
// another Memory is stored at once, otherwise the field shows an error and the old setting stays.
@Composable
private fun S88ModuleField(
    number: Int,
    module: Int,
    enabled: Boolean,
    overlaps: (Int) -> Boolean,
    onModule: (Int) -> Unit,
) {
    var text by remember(module) { mutableStateOf(module.toString()) }
    val value = text.toIntOrNull()
    OutlinedTextField(
        value = text,
        onValueChange = { input ->
            text = input.filter(Char::isDigit).take(2)
            text.toIntOrNull()
                ?.takeIf { it in 0..Mbc89Memory.S88_MODULE_MAX && it != module && !overlaps(it) }
                ?.let(onModule)
        },
        label = { Text("M$number") },
        singleLine = true,
        enabled = enabled,
        isError = value == null || value > Mbc89Memory.S88_MODULE_MAX || (value != module && overlaps(value)),
        keyboardOptions = KeyboardOptions(keyboardType = KeyboardType.Number),
        modifier = Modifier.width(S88_FIELD_WIDTH),
    )
}

private val S88_FIELD_WIDTH = 64.dp

// One row of per-Memory controls: fixed label width and one equally wide column per Memory, so the
// controls of all rows line up and sit centred in their column.
@Composable
private fun MemoryColumnsRow(label: String, memoryCount: Int, cell: @Composable (number: Int) -> Unit) {
    Row(verticalAlignment = Alignment.CenterVertically) {
        Text(label, style = MaterialTheme.typography.bodyMedium, modifier = Modifier.width(MEMORY_LABEL_WIDTH))
        for (number in 1..memoryCount) {
            Box(modifier = Modifier.width(MEMORY_COLUMN_WIDTH), contentAlignment = Alignment.Center) {
                cell(number)
            }
        }
    }
}

private val MEMORY_LABEL_WIDTH = 112.dp
private val MEMORY_COLUMN_WIDTH = 72.dp

// Connect/disconnect switch on the panel tab, in step with the connection tab; it uses the stored
// endpoint. The status line below names a lost connection or an unreachable address and opens the
// connection tab when tapped.
@Composable
private fun PanelConnectionControl(viewModel: ConnectionViewModel, onOpenConnection: () -> Unit) {
    val state by viewModel.state.collectAsStateWithLifecycle()
    val savedEndpoint by viewModel.savedEndpoint.collectAsStateWithLifecycle()
    val context = LocalContext.current
    var missingEndpoint by remember { mutableStateOf(false) }
    val connectStored = {
        val endpoint = savedEndpoint
        if (endpoint == null) {
            missingEndpoint = true
        } else {
            missingEndpoint = false
            viewModel.connect(endpoint.first, endpoint.second)
        }
    }
    val permissionLauncher = rememberLauncherForActivityResult(
        ActivityResultContracts.RequestPermission(),
    ) { granted ->
        if (granted) {
            connectStored()
        } else {
            viewModel.showInputError(
                "Zum Starten des CAN-Dienstes wird die Benachrichtigungsberechtigung benötigt.",
            )
        }
    }
    val on = state is ConnectionState.Connected ||
        state is ConnectionState.Connecting ||
        state is ConnectionState.Reconnecting
    val address = savedEndpoint?.let { "${it.first}:${it.second}" } ?: ""
    val (dotColor, lines) = when {
        missingEndpoint && !on -> StatusRed to listOf("Keine Adresse", "im Reiter Verbindung eintragen")
        else -> when (state) {
            is ConnectionState.Connected -> StatusGreen to listOf("Verbunden", address)
            ConnectionState.Connecting -> StatusYellow to listOf("Verbinde mit", address)
            is ConnectionState.Reconnecting -> StatusOrange to listOf("Verbindung verloren", "neuer Versuch …")
            is ConnectionState.Failed -> StatusRed to listOf(
                "Nicht erreichbar: $address",
                "Adresse im Reiter Verbindung prüfen",
            )
            ConnectionState.Disconnected -> StatusGrey to listOf("Getrennt", address)
        }
    }
    var blinkOn by remember { mutableStateOf(true) }
    val blinking = state is ConnectionState.Reconnecting || state is ConnectionState.Connecting
    LaunchedEffect(blinking) {
        blinkOn = true
        while (blinking) {
            delay(STATUS_BLINK_MS)
            blinkOn = !blinkOn
        }
    }

    Column(horizontalAlignment = Alignment.CenterHorizontally) {
        Text("Verbindung", color = StatusText, style = MaterialTheme.typography.titleMedium)
        Switch(
            checked = on,
            onCheckedChange = { wanted ->
                if (!wanted) {
                    viewModel.disconnect()
                } else if (
                    Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU &&
                    context.checkSelfPermission(Manifest.permission.POST_NOTIFICATIONS) !=
                    PackageManager.PERMISSION_GRANTED
                ) {
                    permissionLauncher.launch(Manifest.permission.POST_NOTIFICATIONS)
                } else {
                    connectStored()
                }
            },
        )
        Row(
            verticalAlignment = Alignment.CenterVertically,
            modifier = Modifier.clickable(onClick = onOpenConnection).padding(4.dp),
        ) {
            Box(
                modifier = Modifier
                    .size(12.dp)
                    .clip(CircleShape)
                    .background(if (!blinking || blinkOn) dotColor else Color.Transparent),
            )
            Spacer(modifier = Modifier.width(8.dp))
            Column {
                for (line in lines.filter { it.isNotEmpty() }) {
                    Text(line, color = StatusText, style = MaterialTheme.typography.bodyMedium)
                }
            }
        }
    }
}

private val StatusGreen = Color(0xFF2E9E5B)
private val StatusYellow = Color(0xFFE0B000)
private val StatusOrange = Color(0xFFF07800)
private val StatusRed = Color(0xFFD32F2F)
private val StatusGrey = Color(0xFF9E9E9E)
private val StatusText = Color(0xFF303030)
private const val STATUS_BLINK_MS = 500L

// MM/DCC of the four key groups of a Keyboard (keys 1-4, 5-8 / 9-12, 13-16, laid out like the panel),
// as set in the protocol array by the ParaCenter or the CS configuration channels.
@Composable
private fun KeyboardProtocolMatrix(systemArray: ByteArray, kbIndex: Int) {
    if (systemArray.size < Mbc89SystemArrayLayout.SIZE) return
    Column(horizontalAlignment = Alignment.CenterHorizontally) {
        Text("Protokoll", color = StatusText, style = MaterialTheme.typography.titleMedium)
        Spacer(modifier = Modifier.height(4.dp))
        for (row in 0 until 2) {
            Row(horizontalArrangement = Arrangement.spacedBy(6.dp)) {
                for (column in 0 until 2) {
                    val group = row * 2 + column
                    val dcc = Mbc89Core.keyboardUsesDcc(systemArray, kbIndex, group)
                    val first = group * Mbc89Core.KEYS_PER_GROUP + 1
                    Column(
                        modifier = Modifier
                            .padding(vertical = 3.dp)
                            .size(width = PROTOCOL_CELL_WIDTH, height = PROTOCOL_CELL_HEIGHT)
                            .border(1.dp, StatusGrey, RoundedCornerShape(4.dp)),
                        horizontalAlignment = Alignment.CenterHorizontally,
                        verticalArrangement = Arrangement.Center,
                    ) {
                        Text(if (dcc) "DCC" else "MM", color = StatusText, style = MaterialTheme.typography.titleMedium)
                        Text(
                            "$first–${first + Mbc89Core.KEYS_PER_GROUP - 1}",
                            color = StatusText,
                            style = MaterialTheme.typography.bodySmall,
                        )
                    }
                }
            }
        }
    }
}

private val PROTOCOL_CELL_WIDTH = 64.dp
private val PROTOCOL_CELL_HEIGHT = 52.dp
