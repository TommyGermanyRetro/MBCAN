package de.mbc89.android

import androidx.compose.foundation.background
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.verticalScroll
import androidx.compose.material3.Card
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableLongStateOf
import androidx.compose.runtime.mutableStateMapOf
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import de.mbc89.android.core.Mbc89SystemArrayLayout
import kotlinx.coroutines.delay

private val ChangedBackground = Color(0xFFFFF59D)
private val UnsavedText = Color(0xFFC62828)
private const val CHANGE_HIGHLIGHT_MS = 10_000L

private class ArrayField(
    val label: String,
    val offset: Int,
    val length: Int,
    val ramOnly: Boolean = false,
    val decode: (ByteArray) -> String,
)

private class ArrayGroup(val title: String, val fields: List<ArrayField>)

private fun u(bytes: ByteArray, index: Int = 0): Int = bytes[index].toInt() and 0xff

private fun hex(bytes: ByteArray): String = bytes.joinToString(" ") { "%02X".format(it.toInt() and 0xff) }

private fun ascii(bytes: ByteArray): String =
    bytes.map { it.toInt() and 0xff }.takeWhile { it in 0x20..0x7e }.map(Int::toChar).joinToString("")

private fun digits(bytes: ByteArray, divisor: Int, unit: String): String {
    val text = ascii(bytes)
    val value = text.toIntOrNull() ?: return "\"$text\""
    return "%.1f %s".format(value.toDouble() / divisor, unit)
}

private val arrayGroups: List<ArrayGroup> = listOf(
    ArrayGroup(
        "Infoblock (Anmeldung ParaCenter)",
        listOf(
            ArrayField("Seriennummer", 1, 8) { hex(it) },
            ArrayField("Artikelnummer", 9, 4) { ascii(it) },
            ArrayField("Softwareversion", 13, 3) { ascii(it) },
            ArrayField("Hardwareversion", 16, 6) { ascii(it) },
            ArrayField("Modulname", 22, 20) { ascii(it) },
        ),
    ),
    ArrayGroup(
        "Identität",
        listOf(
            ArrayField("GUiD", Mbc89SystemArrayLayout.GUID_OFFSET, 4) {
                if (it.all { b -> b == 0xff.toByte() }) "nicht registriert" else hex(it)
            },
            ArrayField("Datenbank", Mbc89SystemArrayLayout.DATABASE_OFFSET, 12) { "\"${ascii(it)}\"" },
            ArrayField("Kennung", Mbc89SystemArrayLayout.KENNUNG_OFFSET, 2) { hex(it) },
            ArrayField("CS2-Gerätegruppe", Mbc89SystemArrayLayout.CS2_GROUP_OFFSET, 1) {
                "0x%02X".format(u(it))
            },
        ),
    ),
    ArrayGroup(
        "Messwerte (simuliert)",
        listOf(
            ArrayField("Netzteil gesteckt", 63, 1) { if (u(it) != 0) "ja" else "nein" },
            ArrayField("Spannung", Mbc89SystemArrayLayout.SUPPLY_VOLTAGE_OFFSET, 3) { digits(it, 10, "V") },
            ArrayField("Temperatur", Mbc89SystemArrayLayout.TEMPERATURE_OFFSET, 3) { digits(it, 10, "°C") },
        ),
    ),
    ArrayGroup(
        "mbc89-Konfiguration (ParaCenter / CSx-Kanal)",
        listOf(
            ArrayField("SW Major (Kanal 7)", Mbc89SystemArrayLayout.MODULE_EXTENSION_OFFSET, 1) {
                u(it).toString()
            },
            ArrayField("SW Minor (Kanal 8)", Mbc89SystemArrayLayout.MODULE_EXTENSION_OFFSET + 1, 1) {
                u(it).toString()
            },
            ArrayField("Mapping (Kanal 9)", Mbc89SystemArrayLayout.MAPPING_OFFSET, 1) {
                when (u(it)) {
                    0 -> "Mapping"
                    1 -> "Mapping&Direkt"
                    else -> "ungültig ${u(it)}"
                }
            },
            ArrayField("Basis (Kanal 10)", Mbc89SystemArrayLayout.KEYBOARD_BASE_OFFSET, 1) {
                "${u(it)} (ab Adresse ${(u(it) - 1) * 256 + 1})"
            },
            ArrayField("I2C (Kanal 11)", Mbc89SystemArrayLayout.I2C_FREQUENCY_OFFSET, 1) {
                "${u(it)} kHz"
            },
        ),
    ),
    ArrayGroup(
        "Zubehörprotokoll (Kanal 12–75)",
        (0 until 16).map { keyboard ->
            ArrayField(
                "KeyB${keyboard + 1} Group1–4",
                Mbc89SystemArrayLayout.PROTOCOL_SETTINGS_OFFSET + keyboard * 4,
                4,
            ) { bytes ->
                bytes.joinToString("  ") { if (it.toInt() == 0) "MM" else if (it.toInt() == 1) "DCC" else "?" }
            }
        } + ArrayField("Reservebyte", Mbc89SystemArrayLayout.PROTOCOL_SETTINGS_OFFSET + 64, 1) {
            u(it).toString()
        },
    ),
)

private const val LOCO_RECORD_LENGTH = 10
private const val LOCO_COUNT = 80

@Composable
fun SystemArrayScreen(current: ByteArray, persisted: ByteArray) {
    val changedAt = remember { mutableStateMapOf<Int, Long>() }
    var previous by remember { mutableStateOf(ByteArray(0)) }
    var now by remember { mutableLongStateOf(System.currentTimeMillis()) }

    LaunchedEffect(current) {
        val stamp = System.currentTimeMillis()
        if (previous.size == current.size) {
            current.indices.filter { current[it] != previous[it] }.forEach { changedAt[it] = stamp }
        }
        previous = current
    }
    LaunchedEffect(Unit) {
        while (true) {
            now = System.currentTimeMillis()
            delay(1_000)
        }
    }

    Column(
        modifier = Modifier
            .fillMaxSize()
            .verticalScroll(rememberScrollState())
            .padding(24.dp),
        verticalArrangement = Arrangement.spacedBy(16.dp),
    ) {
        if (current.size != Mbc89SystemArrayLayout.SIZE) {
            Text("Kein Systemarray – der CAN-Dienst läuft noch nicht.")
            return@Column
        }
        Text(
            "Gelb: in den letzten 10 s geändert. Rot \"nicht gespeichert\": RAM weicht vom " +
                "gespeicherten Stand ab.",
            style = MaterialTheme.typography.bodyMedium,
        )
        for (group in arrayGroups) {
            ArrayCard(group.title) {
                for (field in group.fields) {
                    val range = field.offset until field.offset + field.length
                    FieldRow(
                        label = field.label,
                        offset = field.offset,
                        value = field.decode(current.copyOfRange(range.first, range.last + 1)),
                        raw = hex(current.copyOfRange(range.first, range.last + 1)),
                        changed = range.any { now - (changedAt[it] ?: 0L) < CHANGE_HIGHLIGHT_MS },
                        unsaved = !field.ramOnly && persisted.size == current.size &&
                            range.any { current[it] != persisted[it] },
                    )
                }
            }
        }
        ArrayCard("Lokdatenbank (nur RAM, zugeordnete Loks)") {
            val mapped = (0 until LOCO_COUNT).filter { index ->
                current[Mbc89SystemArrayLayout.LOCOMOTIVE_OFFSET + index * LOCO_RECORD_LENGTH + 8] != 0.toByte()
            }
            if (mapped.isEmpty()) Text("keine")
            for (index in mapped) {
                val offset = Mbc89SystemArrayLayout.LOCOMOTIVE_OFFSET + index * LOCO_RECORD_LENGTH
                val record = current.copyOfRange(offset, offset + LOCO_RECORD_LENGTH)
                FieldRow(
                    label = "Lok ${if (index == 0) 80 else index}",
                    offset = offset,
                    value = "GUiD ${hex(record.copyOfRange(4, 8))}, Stufe ${u(record)}, " +
                        (if (u(record, 1) == 1) "vorwärts" else "rückwärts") +
                        ", F0 ${u(record, 2)}, F1–F4 " +
                        (0 until 4).joinToString("") { if (u(record, 3) and (1 shl it) != 0) "1" else "0" } +
                        ", Zuordnung 0x%02X".format(u(record, 8)),
                    raw = hex(record),
                    changed = (offset until offset + LOCO_RECORD_LENGTH)
                        .any { now - (changedAt[it] ?: 0L) < CHANGE_HIGHLIGHT_MS },
                    unsaved = false,
                )
            }
        }
    }
}

@Composable
private fun ArrayCard(title: String, content: @Composable () -> Unit) {
    Card(modifier = Modifier.fillMaxWidth()) {
        Column(
            modifier = Modifier.padding(16.dp),
            verticalArrangement = Arrangement.spacedBy(4.dp),
        ) {
            Text(title, style = MaterialTheme.typography.titleMedium)
            ColumnHeader()
            content()
        }
    }
}

@Composable
private fun ColumnHeader() {
    Row(modifier = Modifier.fillMaxWidth()) {
        HeaderText("Feld", Modifier.width(220.dp))
        HeaderText("Wert", Modifier.weight(1f))
        HeaderText("Index", Modifier.width(48.dp))
        HeaderText("Inhalt (hex)", Modifier.width(320.dp))
        HeaderText("", Modifier.width(120.dp))
    }
}

@Composable
private fun HeaderText(text: String, modifier: Modifier) {
    Text(
        text,
        style = MaterialTheme.typography.labelMedium,
        color = MaterialTheme.colorScheme.onSurfaceVariant,
        modifier = modifier,
    )
}

@Composable
private fun FieldRow(
    label: String,
    offset: Int,
    value: String,
    raw: String,
    changed: Boolean,
    unsaved: Boolean,
) {
    Row(
        modifier = Modifier
            .fillMaxWidth()
            .background(if (changed) ChangedBackground else Color.Transparent)
            .padding(vertical = 2.dp),
    ) {
        Text(label, modifier = Modifier.width(220.dp))
        Text(value, modifier = Modifier.weight(1f))
        Text(
            "%4d".format(offset),
            fontFamily = FontFamily.Monospace,
            fontSize = 12.sp,
            modifier = Modifier.width(48.dp),
        )
        Text(
            raw,
            fontFamily = FontFamily.Monospace,
            fontSize = 12.sp,
            modifier = Modifier.width(320.dp),
        )
        Text(
            if (unsaved) "nicht gespeichert" else "",
            color = UnsavedText,
            fontSize = 12.sp,
            modifier = Modifier.width(120.dp),
        )
    }
}
