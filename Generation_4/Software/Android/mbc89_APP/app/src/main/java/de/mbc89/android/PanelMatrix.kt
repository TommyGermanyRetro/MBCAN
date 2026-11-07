package de.mbc89.android

import androidx.compose.foundation.background
import androidx.compose.foundation.clickable
import androidx.compose.foundation.interaction.MutableInteractionSource
import androidx.compose.foundation.interaction.PressInteraction
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.BoxScope
import androidx.compose.foundation.layout.BoxWithConstraints
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.offset
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.lerp
import kotlinx.coroutines.delay
import androidx.compose.ui.unit.Dp
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp

val KeyRed = Color(0xFFE8231B)
val KeyGreen = Color(0xFF2E9E5B)
val LedRed = Color(0xFFFF3B2F)
val LedGreen = Color(0xFF3DFF7A)
val LedOff = Color(0xFF6A2620)
private val LabelColor = Color(0xFFEDEDED)
private val LABEL_RAISE = (-2).dp
private val KEY_SIZE_WIDTH = 40.dp
private val KEY_SIZE_HEIGHT = 18.dp
private val KEY_PRESSED_INSET = 2.dp
private const val KEY_PRESSED_DARKEN = 0.55f
private const val KEY_PRESSED_MIN_MS = 150L

// Shared key matrix of all panels: 8 columns x 4 key rows, each key with its label and LED above.
object PanelGrid {
    const val COLUMNS = 8
    const val ROWS = 4
    val COLUMN_WIDTH = 62.dp
    val LABEL_HEIGHT = 18.dp
    val LED_HEIGHT = 14.dp
    val KEY_HEIGHT = 36.dp
    val ROW_HEIGHT = LABEL_HEIGHT + LED_HEIGHT + KEY_HEIGHT
    val HEIGHT = ROW_HEIGHT * ROWS
}

class PanelCell(
    val label: String? = null,
    val led: Color? = null,
    val key: Color? = null,
    val onClick: (() -> Unit)? = null,
)

class PanelGeometry(private val spacing: Dp) {
    fun columnX(column: Int): Dp = (PanelGrid.COLUMN_WIDTH + spacing) * column
    fun rowY(row: Int): Dp = PanelGrid.ROW_HEIGHT * row
    fun width(columns: Int): Dp = PanelGrid.COLUMN_WIDTH * columns + spacing * (columns - 1)
}

@Composable
fun PanelMatrix(
    enabled: Boolean,
    cell: (row: Int, column: Int) -> PanelCell?,
    overlay: @Composable BoxScope.(PanelGeometry) -> Unit = {},
) {
    BoxWithConstraints(modifier = Modifier.fillMaxWidth().height(PanelGrid.HEIGHT)) {
        val spacing = (maxWidth - PanelGrid.COLUMN_WIDTH * PanelGrid.COLUMNS) / (PanelGrid.COLUMNS - 1)
        val geometry = PanelGeometry(spacing)
        Column {
            for (row in 0 until PanelGrid.ROWS) {
                Row {
                    for (column in 0 until PanelGrid.COLUMNS) {
                        if (column > 0) Box(modifier = Modifier.width(spacing))
                        PanelCellView(cell(row, column), enabled)
                    }
                }
            }
        }
        overlay(geometry)
    }
}

@Composable
private fun PanelCellView(cell: PanelCell?, enabled: Boolean) {
    val interactionSource = remember { MutableInteractionSource() }
    var pressed by remember { mutableStateOf(false) }
    LaunchedEffect(interactionSource) {
        var pressedAt = 0L
        interactionSource.interactions.collect { interaction ->
            when (interaction) {
                is PressInteraction.Press -> {
                    pressedAt = System.currentTimeMillis()
                    pressed = true
                }
                is PressInteraction.Release, is PressInteraction.Cancel -> {
                    delay((KEY_PRESSED_MIN_MS - (System.currentTimeMillis() - pressedAt)).coerceAtLeast(0L))
                    pressed = false
                }
            }
        }
    }
    // The whole cell (label, LED and key) takes the touch, the key alone would be small on phones.
    Column(
        modifier = Modifier
            .width(PanelGrid.COLUMN_WIDTH)
            .clickable(
                interactionSource = interactionSource,
                indication = null,
                enabled = enabled && cell?.onClick != null,
            ) { cell?.onClick?.invoke() },
        horizontalAlignment = Alignment.CenterHorizontally,
    ) {
        Box(modifier = Modifier.height(PanelGrid.LABEL_HEIGHT), contentAlignment = Alignment.TopCenter) {
            cell?.label?.let {
                Text(
                    it,
                    color = LabelColor,
                    fontSize = 13.sp,
                    lineHeight = 15.sp,
                    modifier = Modifier.offset(y = LABEL_RAISE),
                )
            }
        }
        Box(modifier = Modifier.height(PanelGrid.LED_HEIGHT), contentAlignment = Alignment.Center) {
            cell?.led?.let {
                Box(modifier = Modifier.size(10.dp).clip(CircleShape).background(it))
            }
        }
        Box(
            modifier = Modifier.size(width = PanelGrid.COLUMN_WIDTH, height = PanelGrid.KEY_HEIGHT),
            contentAlignment = Alignment.Center,
        ) {
            cell?.key?.let {
                val inset = if (pressed) KEY_PRESSED_INSET else 0.dp
                Box(
                    modifier = Modifier
                        .size(width = KEY_SIZE_WIDTH - inset * 2, height = KEY_SIZE_HEIGHT - inset * 2)
                        .clip(RoundedCornerShape(3.dp))
                        .background(if (pressed) lerp(it, Color.Black, KEY_PRESSED_DARKEN) else it),
                )
            }
        }
    }
}
