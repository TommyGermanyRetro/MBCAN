package de.mbc89.android

import androidx.compose.foundation.background
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.items
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.material3.HorizontalDivider
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.Dp
import androidx.compose.ui.unit.dp
import androidx.lifecycle.compose.collectAsStateWithLifecycle
import de.mbc89.android.core.Mbc89Memory
import de.mbc89.android.core.MemoryCommand
import de.mbc89.android.core.MemoryEntry
import de.mbc89.android.core.MemoryRouteCall
import de.mbc89.android.core.MemoryRouteView

// Read-only list of all recorded routes of the configured Memory instances, one row per entry.
@Composable
fun RoutesScreen(viewModel: ConnectionViewModel) {
    val routes by viewModel.memoryRoutes.collectAsStateWithLifecycle()
    val memoryCount by viewModel.memoryCount.collectAsStateWithLifecycle()
    val shown = routes.filter { it.instance < memoryCount }

    if (shown.isEmpty()) {
        Box(modifier = Modifier.fillMaxSize(), contentAlignment = Alignment.Center) {
            Text("Keine Fahrstraßen aufgenommen", style = MaterialTheme.typography.bodyLarge)
        }
        return
    }
    LazyColumn(modifier = Modifier.fillMaxSize().padding(horizontal = 24.dp, vertical = 12.dp)) {
        item { RouteRow(HEADER, bold = true) }
        items(shown, key = { it.instance * Mbc89Memory.ROUTES_PER_INSTANCE + it.localRoute }) { route ->
            HorizontalDivider()
            route.entries.forEachIndexed { index, entry ->
                RouteRow(cells(route, index, entry), dot = (entry as? MemoryCommand)?.green)
            }
        }
    }
}

private fun cells(route: MemoryRouteView, index: Int, entry: MemoryEntry): List<String> {
    val first = index == 0
    val common = listOf(
        if (first) "M${route.instance + 1}" else "",
        if (first) routeName(route.localRoute) else "",
        if (first && route.active) "gestellt" else "",
        (index + 1).toString(),
    )
    return common + when (entry) {
        is MemoryCommand -> listOf(articleName(entry.address), if (entry.green) "grün" else "rot", switchTime(entry))
        is MemoryRouteCall -> listOf("Folgefahrstraße ${routeName(entry.localRoute)}", "", "")
    }
}

private fun routeName(localRoute: Int): String =
    "${'A' + localRoute / ROUTES_PER_ROW}${localRoute % ROUTES_PER_ROW + 1}"

// Accessory number as on the Keyboard and in the CS: MM from 0x3000, DCC from 0x3800, counting from 1.
private fun articleName(address: Int): String =
    if (address >= DCC_BASE) "DCC ${address - DCC_BASE + 1}" else "MM ${address - MM_BASE + 1}"

private fun switchTime(command: MemoryCommand): String =
    if (command.presses == 1) "Standard" else "%.1f s".format(command.switchTime / 100.0)

@Composable
private fun RouteRow(texts: List<String>, bold: Boolean = false, dot: Boolean? = null) {
    Row(modifier = Modifier.fillMaxWidth().padding(vertical = 4.dp), verticalAlignment = Alignment.CenterVertically) {
        texts.forEachIndexed { column, text ->
            Row(modifier = Modifier.width(COLUMN_WIDTHS[column]), verticalAlignment = Alignment.CenterVertically) {
                if (column == POSITION_COLUMN && dot != null) {
                    Box(
                        modifier = Modifier
                            .padding(end = 6.dp)
                            .size(10.dp)
                            .clip(CircleShape)
                            .background(if (dot) LedGreen else LedRed),
                    )
                }
                Text(
                    text,
                    style = MaterialTheme.typography.bodyMedium,
                    fontWeight = if (bold) FontWeight.Bold else FontWeight.Normal,
                )
            }
        }
    }
}

private val HEADER = listOf("Memory", "Fahrstraße", "Zustand", "Nr.", "Artikel", "Stellung", "Schaltdauer")
private val COLUMN_WIDTHS: List<Dp> = listOf(80.dp, 100.dp, 100.dp, 50.dp, 220.dp, 110.dp, 110.dp)
private const val POSITION_COLUMN = 5
private const val ROUTES_PER_ROW = 8
private const val MM_BASE = 0x3000
private const val DCC_BASE = 0x3800
