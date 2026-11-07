package de.mbc89.android

import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.height
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp

private const val COLUMNS_PER_BLOCK = 8

@Composable
fun KeyboardPanel(
    number: Int,
    state: Int,
    enabled: Boolean,
    onSwitch: (column: Int, green: Boolean) -> Unit,
) {
    DeviceHousing(label = "keyboard", size = 600.dp) {
        FaceHeader(display = "%02d".format(number))
        Spacer(modifier = Modifier.height(16.dp))
        PanelMatrix(
            enabled = enabled,
            cell = { row, index ->
                val column = (row / 2) * COLUMNS_PER_BLOCK + index
                if (row % 2 == 0) {
                    PanelCell(key = KeyRed, onClick = { onSwitch(column, false) })
                } else {
                    PanelCell(
                        label = (column + 1).toString(),
                        led = if (state and (1 shl column) != 0) LedGreen else LedRed,
                        key = KeyGreen,
                        onClick = { onSwitch(column, true) },
                    )
                }
            },
        )
    }
}
