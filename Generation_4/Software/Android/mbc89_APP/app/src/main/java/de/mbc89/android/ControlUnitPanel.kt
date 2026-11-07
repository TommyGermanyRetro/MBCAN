package de.mbc89.android

import androidx.compose.foundation.Canvas
import androidx.compose.foundation.clickable
import androidx.compose.foundation.gestures.detectDragGestures
import androidx.compose.foundation.gestures.detectTapGestures
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.offset
import androidx.compose.foundation.layout.size
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.rememberUpdatedState
import androidx.compose.runtime.setValue
import androidx.compose.ui.Modifier
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.Path
import androidx.compose.ui.graphics.StrokeCap
import androidx.compose.ui.graphics.drawscope.DrawScope
import androidx.compose.ui.graphics.drawscope.Stroke
import androidx.compose.ui.input.pointer.pointerInput
import androidx.compose.ui.text.TextMeasurer
import androidx.compose.ui.text.TextStyle
import androidx.compose.ui.text.drawText
import androidx.compose.ui.text.rememberTextMeasurer
import androidx.compose.ui.unit.Density
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import de.mbc89.android.core.LocomotiveState
import de.mbc89.android.core.Mbc89Locomotives
import kotlin.math.atan2
import kotlin.math.cos
import kotlin.math.roundToInt
import kotlin.math.sin
import kotlinx.coroutines.delay

private val DisplayRed = Color(0xFFFF2B1C)
private val KnobRed = Color(0xFFE02418)
private val KnobRim = Color(0xFF8E140C)
private val KnobLever = Color(0xFFF0372A)
private val KnobShade = Color(0xFFB8180F)
private val ScaleWhite = Color(0xFFEDEDED)

// Knob scale like the CU6021: 0 at the lower left, 250 at the lower right, over the top.
private const val SCALE_START_DEGREES = 180f
private const val SCALE_SWEEP_DEGREES = 180f
private const val SCALE_TICKS = 30
// Both ends of the lever stick out equally; total length 2.27 knob radii.
private const val POINTER_REACH = 1.135f
private val KNOB_RADIUS = 72.dp
private val KNOB_AREA_WIDTH = 300.dp
private val KNOB_AREA_HEIGHT = 236.dp
private val KNOB_CENTER_FROM_TOP = 122.dp
private val KNOB_CENTER_ABOVE_FACE_EDGE = 14.dp

class ControlUnitState(
    val addressText: String,
    val locomotive: LocomotiveState?,
    val stopped: Boolean,
    val ownedElsewhere: Boolean = false,
    val functionAddressText: String = addressText,
    val functionLocomotive: LocomotiveState? = locomotive,
    val entry: AddressEntry = AddressEntry.LINKED,
) {
    val address: Int? = if (addressText.length == 2) addressText.toInt() else null
    val controllable: Boolean =
        address != null && locomotive?.controllable == true && !ownedElsewhere
    val controlAllowed: Boolean = controllable && !stopped
    private val functionAddress: Int? = if (functionAddressText.length == 2) functionAddressText.toInt() else null
    private val linked: Boolean = entry == AddressEntry.LINKED && functionAddressText == addressText
    val functionControllable: Boolean =
        if (linked) controllable else functionAddress != null && functionLocomotive?.controllable == true
    val functionAllowed: Boolean = functionControllable && !stopped
    val displayText: String = if (entry == AddressEntry.FUNCTION) functionAddressText else addressText
    val displayInvalid: Boolean =
        if (entry == AddressEntry.FUNCTION) functionAddress != null && !functionControllable
        else address != null && !controllable
    val locoLed: Boolean = entry != AddressEntry.FUNCTION
    val functionLed: Boolean = entry != AddressEntry.LOCO
}

@Composable
fun ControlUnitPanel(
    state: ControlUnitState,
    connected: Boolean,
    onDigit: (Int) -> Unit,
    onCommand: (LocoAction) -> Unit,
    onStopGo: (go: Boolean) -> Unit,
    label: String = "control unit",
    onSelectAddress: (AddressEntry) -> Unit = {},
) {
    var blinkOn by remember { mutableStateOf(true) }
    val invalid = connected && state.displayInvalid
    LaunchedEffect(invalid) {
        blinkOn = true
        while (invalid) {
            delay(500)
            blinkOn = !blinkOn
        }
    }
    val loco = state.locomotive?.takeIf { state.controllable }
    val allowed = state.controlAllowed
    val functionLed = { on: Boolean -> if (allowed && on) LedRed else LedOff }
    val decoder = state.functionLocomotive?.takeIf { state.functionControllable }
    val decoderAllowed = state.functionAllowed
    val decoderLed = { on: Boolean -> if (decoderAllowed && on) LedRed else LedOff }

    DeviceHousing(
        label = label,
        size = 600.dp,
        overlay = { geometry ->
            // Like the original the knob sits low, centred on the lower face edge between f2 and
            // function, in a recess of housing grey.
            val centerX = (geometry.columnCenter(5) + geometry.columnCenter(6)) / 2
            SpeedKnob(
                step = loco?.speed ?: 0,
                enabled = connected && allowed,
                onStep = { onCommand(LocoAction.Speed(it)) },
                modifier = Modifier
                    .offset(
                        x = centerX - KNOB_AREA_WIDTH / 2,
                        y = geometry.faceBottom - KNOB_CENTER_ABOVE_FACE_EDGE - KNOB_CENTER_FROM_TOP,
                    )
                    .size(width = KNOB_AREA_WIDTH, height = KNOB_AREA_HEIGHT),
            )
        },
    ) {
        FaceHeader(
            // Without a connection the display stays dark like a CU6021 without power.
            display = if (connected && blinkOn) state.displayText.padEnd(2) else "  ",
            displayColor = DisplayRed,
            besideDisplay = {
                DirectionTriangles(
                    forward = loco?.forward,
                    enabled = connected && allowed,
                    onDirection = { onCommand(LocoAction.Direction(it)) },
                )
            },
        )
        Spacer(modifier = Modifier.height(16.dp))
        PanelMatrix(
            enabled = connected,
            cell = { row, column ->
                when {
                    column < 3 -> digitCell(row, column, onDigit, state, onSelectAddress)
                    row == 0 && column == 4 -> functionCell("f1", decoderLed(decoder.fx(1)), decoderAllowed) {
                        onCommand(LocoAction.Function(1, !decoder.fx(1)))
                    }
                    row == 0 && column == 5 -> functionCell("f2", decoderLed(decoder.fx(2)), decoderAllowed) {
                        onCommand(LocoAction.Function(2, !decoder.fx(2)))
                    }
                    row == 0 && column == 6 -> functionCell("off", null, allowed) {
                        onCommand(LocoAction.Function(0, false))
                    }
                    row == 0 && column == 7 -> PanelCell(label = "stop", key = KeyRed, onClick = { onStopGo(false) })
                    row == 1 && column == 4 -> functionCell("f3", decoderLed(decoder.fx(3)), decoderAllowed) {
                        onCommand(LocoAction.Function(3, !decoder.fx(3)))
                    }
                    row == 1 && column == 5 -> functionCell("f4", decoderLed(decoder.fx(4)), decoderAllowed) {
                        onCommand(LocoAction.Function(4, !decoder.fx(4)))
                    }
                    row == 1 && column == 6 -> functionCell("function", functionLed(loco?.f0 == true), allowed) {
                        onCommand(LocoAction.Function(0, true))
                    }
                    row == 1 && column == 7 -> PanelCell(
                        label = "go",
                        led = if (state.stopped) LedOff else LedRed,
                        key = KeyGreen,
                        onClick = { onStopGo(true) },
                    )
                    else -> null
                }
            },
        )
    }
}

sealed interface LocoAction {
    data class Speed(val step: Int) : LocoAction
    data class Function(val index: Int, val on: Boolean) : LocoAction
    data class Direction(val forward: Boolean) : LocoAction
}

// Direction display of the MSX port (up = forward, down = backward, lit only for a controllable
// loco); tapping a triangle replaces the MSX <R> key.
@Composable
private fun DirectionTriangles(forward: Boolean?, enabled: Boolean, onDirection: (Boolean) -> Unit) {
    Column(modifier = Modifier.padding(start = 12.dp)) {
        for (up in listOf(true, false)) {
            val lit = enabled && forward == up
            Canvas(
                modifier = Modifier
                    .size(width = 48.dp, height = 42.dp)
                    .clickable(enabled = enabled) { onDirection(up) },
            ) {
                val width = 26.dp.toPx()
                val height = 18.dp.toPx()
                val left = (size.width - width) / 2f
                val top = (size.height - height) / 2f
                val path = Path().apply {
                    if (up) {
                        moveTo(left + width / 2f, top)
                        lineTo(left + width, top + height)
                        lineTo(left, top + height)
                    } else {
                        moveTo(left, top)
                        lineTo(left + width, top)
                        lineTo(left + width / 2f, top + height)
                    }
                    close()
                }
                drawPath(path, if (lit) LedRed else LedOff)
            }
        }
    }
}

private fun LocomotiveState?.fx(index: Int): Boolean =
    this != null && functions and (1 shl (index - 1)) != 0

// "L"/"F" select which address the digits enter; their LEDs show it (both lit = same address).
private fun digitCell(
    row: Int,
    column: Int,
    onDigit: (Int) -> Unit,
    state: ControlUnitState,
    onSelectAddress: (AddressEntry) -> Unit,
): PanelCell {
    if (row < 3) {
        val digit = row * 3 + column + 1
        return PanelCell(label = digit.toString(), key = KeyGreen, onClick = { onDigit(digit) })
    }
    return when (column) {
        0 -> PanelCell(
            label = "L",
            led = if (state.locoLed) LedRed else LedOff,
            key = KeyRed,
            onClick = { onSelectAddress(AddressEntry.LOCO) },
        )
        1 -> PanelCell(label = "0", key = KeyGreen, onClick = { onDigit(0) })
        else -> PanelCell(
            label = "F",
            led = if (state.functionLed) LedRed else LedOff,
            key = KeyRed,
            onClick = { onSelectAddress(AddressEntry.FUNCTION) },
        )
    }
}

private fun functionCell(label: String, led: Color?, allowed: Boolean, onClick: () -> Unit) =
    PanelCell(label = label, led = led, key = KeyGreen, onClick = if (allowed) onClick else null)

@Composable
private fun SpeedKnob(
    step: Int,
    enabled: Boolean,
    onStep: (Int) -> Unit,
    modifier: Modifier,
) {
    val textMeasurer = rememberTextMeasurer()
    var dragStep by remember { mutableStateOf<Int?>(null) }
    val currentStep by rememberUpdatedState(step)
    val currentEnabled by rememberUpdatedState(enabled)
    val currentOnStep by rememberUpdatedState(onStep)

    fun moveTo(position: Offset, center: Offset) {
        val delta = position - center
        val angle = (Math.toDegrees(atan2(delta.y, delta.x).toDouble()).toFloat() + 360f) % 360f
        val relative = (angle - SCALE_START_DEGREES + 360f) % 360f
        val shown = dragStep ?: currentStep
        if (relative <= SCALE_SWEEP_DEGREES) {
            val target = (relative / SCALE_SWEEP_DEGREES * Mbc89Locomotives.MAX_SPEED_STEP).roundToInt()
            if (target != shown) {
                dragStep = target
                currentOnStep(target)
            }
        }
    }

    Canvas(
        modifier = modifier
            .pointerInput(Unit) {
                detectTapGestures { position ->
                    if (currentEnabled) {
                        moveTo(position, knobCenter(size.width.toFloat()))
                        dragStep = null
                    }
                }
            }
            .pointerInput(Unit) {
                detectDragGestures(
                    onDragStart = { position ->
                        if (currentEnabled) moveTo(position, knobCenter(size.width.toFloat()))
                    },
                    onDragEnd = { dragStep = null },
                    onDragCancel = { dragStep = null },
                ) { change, _ ->
                    if (currentEnabled) {
                        moveTo(change.position, knobCenter(size.width.toFloat()))
                    }
                }
            },
    ) {
        val center = knobCenter(size.width)
        val knobRadius = KNOB_RADIUS.toPx()
        val scaleRadius = knobRadius + 26.dp.toPx()
        val angle = angleFor(dragStep ?: step)
        drawCircle(HousingGrey, knobRadius + 18.dp.toPx(), center)
        drawScale(center, scaleRadius, textMeasurer)
        drawCircle(KnobShade, knobRadius + 1.dp.toPx(), center)
        drawCircle(KnobRed, knobRadius, center)
        val pointer = pointerPath(center, knobRadius, angle)
        drawPath(pointer, KnobLever)
        drawPath(pointer, KnobRim, style = Stroke(width = 1.5.dp.toPx()))
    }
}

// Wedge-shaped lever across the whole knob like the CU6021: narrow tip at the scale value, wide
// squared end sticking out on the opposite side.
private fun DrawScope.pointerPath(center: Offset, knobRadius: Float, degrees: Float): Path {
    val radians = Math.toRadians(degrees.toDouble())
    val direction = Offset(cos(radians).toFloat(), sin(radians).toFloat())
    val normal = Offset(-direction.y, direction.x)
    val tip = center + direction * (knobRadius * POINTER_REACH)
    val tail = center - direction * (knobRadius * POINTER_REACH)
    val tipHalf = 2.5.dp.toPx()
    val tailHalf = knobRadius * 0.14f
    return Path().apply {
        moveTo((tip + normal * tipHalf).x, (tip + normal * tipHalf).y)
        lineTo((tail + normal * tailHalf).x, (tail + normal * tailHalf).y)
        lineTo((tail - normal * tailHalf).x, (tail - normal * tailHalf).y)
        lineTo((tip - normal * tipHalf).x, (tip - normal * tipHalf).y)
        close()
    }
}

private fun Density.knobCenter(width: Float): Offset = Offset(width / 2f, KNOB_CENTER_FROM_TOP.toPx())

private fun angleFor(step: Int): Float =
    SCALE_START_DEGREES + SCALE_SWEEP_DEGREES * step / Mbc89Locomotives.MAX_SPEED_STEP

private fun pointAt(center: Offset, radius: Float, degrees: Float): Offset {
    val radians = Math.toRadians(degrees.toDouble())
    return Offset(center.x + radius * cos(radians).toFloat(), center.y + radius * sin(radians).toFloat())
}

private fun DrawScope.drawScale(
    center: Offset,
    radius: Float,
    textMeasurer: TextMeasurer,
) {
    for (tick in 0..SCALE_TICKS) {
        val angle = SCALE_START_DEGREES + SCALE_SWEEP_DEGREES * tick / SCALE_TICKS
        drawLine(
            ScaleWhite,
            pointAt(center, radius - 9.dp.toPx(), angle),
            pointAt(center, radius, angle),
            strokeWidth = 2.5.dp.toPx(),
        )
    }
    for (label in 0..5) {
        val text = textMeasurer.measure(
            (label * 50).toString(),
            TextStyle(color = ScaleWhite, fontSize = 13.sp),
        )
        val anchor = pointAt(center, radius + 12.dp.toPx(), SCALE_START_DEGREES + SCALE_SWEEP_DEGREES * label / 5)
        drawText(
            text,
            topLeft = Offset(anchor.x - text.size.width / 2f, anchor.y - text.size.height / 2f),
        )
    }
}
