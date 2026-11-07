package de.mbc89.android

import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.height
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.unit.dp
import de.mbc89.android.core.LedState
import de.mbc89.android.core.Mbc89Memory
import kotlinx.coroutines.delay

private val DisplayYellow = Color(0xFFE6DC96)
private const val BLINK_HALF_PERIOD_MS = 300L
private val CONTROL_LABELS = mapOf(0 to "extern", 1 to "off", 3 to "input", 4 to "end", 7 to "clear")
private val CONTROL_BUTTONS = mapOf(
    0 to Mbc89Memory.BUTTON_EX,
    1 to Mbc89Memory.BUTTON_OFF,
    3 to Mbc89Memory.BUTTON_IN,
    4 to Mbc89Memory.BUTTON_END,
    7 to Mbc89Memory.BUTTON_CLR,
)

@Composable
fun MemoryPanel(
    number: Int,
    leds: List<LedState>,
    enabled: Boolean,
    onPress: (index: Int) -> Unit,
) {
    var blinkOn by remember { mutableStateOf(true) }
    val anyBlink = leds.any { it == LedState.BLINK }
    LaunchedEffect(anyBlink) {
        blinkOn = true
        while (anyBlink) {
            delay(BLINK_HALF_PERIOD_MS)
            blinkOn = !blinkOn
        }
    }
    fun ledColor(button: Int): Color = when (leds.getOrElse(button) { LedState.OFF }) {
        LedState.ON -> LedRed
        LedState.BLINK -> if (blinkOn) LedRed else LedOff
        LedState.OFF -> LedOff
    }

    DeviceHousing(label = "memory", size = 600.dp) {
        FaceHeader(display = "%02d".format(number), displayColor = DisplayYellow)
        Spacer(modifier = Modifier.height(16.dp))
        PanelMatrix(
            enabled = enabled,
            cell = { row, column ->
                if (row == 0) {
                    val button = CONTROL_BUTTONS[column]
                    if (button == null) {
                        null
                    } else {
                        PanelCell(
                            label = CONTROL_LABELS[column],
                            led = ledColor(button),
                            key = KeyRed,
                            onClick = { onPress(button) },
                        )
                    }
                } else {
                    val button = Mbc89Memory.FIRST_ROUTE_BUTTON + (row - 1) * 8 + column
                    PanelCell(
                        label = "${'A' + row - 1}${column + 1}",
                        led = ledColor(button),
                        key = KeyGreen,
                        onClick = { onPress(button) },
                    )
                }
            },
        )
    }
}
