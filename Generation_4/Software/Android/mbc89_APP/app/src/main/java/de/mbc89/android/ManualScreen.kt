package de.mbc89.android

import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.widthIn
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.items
import androidx.compose.material3.HorizontalDivider
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.remember
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.text.AnnotatedString
import androidx.compose.ui.text.SpanStyle
import androidx.compose.ui.text.buildAnnotatedString
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.withStyle
import androidx.compose.ui.unit.dp

// The user manual (assets/anleitung.md) with the Markdown subset it uses: headings #/##/###, bullets
// "- ", numbered steps "1. ", tables ("| a | b |", header row above a "|---|" separator), paragraphs
// and **bold**.
@Composable
fun ManualScreen() {
    val context = LocalContext.current
    val entries = remember {
        val lines = context.assets.open(MANUAL_ASSET).bufferedReader(Charsets.UTF_8).use { it.readLines() }
            .filter { it.isNotBlank() }
        lines.mapIndexedNotNull { index, line ->
            if (TABLE_SEPARATOR.matches(line)) {
                null
            } else {
                line to (line.startsWith("|") && lines.getOrNull(index + 1)?.let(TABLE_SEPARATOR::matches) == true)
            }
        }
    }
    Box(modifier = Modifier.fillMaxSize(), contentAlignment = Alignment.TopCenter) {
        LazyColumn(
            modifier = Modifier
                .widthIn(max = MANUAL_MAX_WIDTH)
                .fillMaxSize()
                .padding(horizontal = 24.dp, vertical = 12.dp),
        ) {
            items(entries) { (line, tableHeader) -> ManualLine(line, tableHeader) }
        }
    }
}

@Composable
private fun ManualLine(line: String, tableHeader: Boolean) {
    val typography = MaterialTheme.typography
    when {
        line.startsWith("|") -> TableRow(line, tableHeader)
        line.startsWith("### ") -> Text(
            inline(line.removePrefix("### ")),
            style = typography.titleSmall,
            fontWeight = FontWeight.Bold,
            modifier = Modifier.padding(top = 12.dp, bottom = 4.dp),
        )
        line.startsWith("## ") -> Text(
            inline(line.removePrefix("## ")),
            style = typography.titleLarge,
            modifier = Modifier.padding(top = 20.dp, bottom = 6.dp),
        )
        line.startsWith("# ") -> Text(
            inline(line.removePrefix("# ")),
            style = typography.headlineSmall,
            modifier = Modifier.padding(bottom = 8.dp),
        )
        line.startsWith("- ") -> Marked("•", line.removePrefix("- "))
        NUMBERED.matches(line) -> {
            val (number, text) = NUMBERED.find(line)!!.destructured
            Marked("$number.", text)
        }
        else -> Text(inline(line), style = typography.bodyMedium, modifier = Modifier.padding(vertical = 4.dp))
    }
}

@Composable
private fun Marked(mark: String, text: String) {
    Row(modifier = Modifier.fillMaxWidth().padding(start = 8.dp, top = 2.dp, bottom = 2.dp)) {
        Text(mark, style = MaterialTheme.typography.bodyMedium, modifier = Modifier.width(MARK_WIDTH))
        Text(inline(text), style = MaterialTheme.typography.bodyMedium)
    }
}

@Composable
private fun TableRow(line: String, header: Boolean) {
    val cells = line.trim().removePrefix("|").removeSuffix("|").split("|").map { it.trim() }
    Column(modifier = Modifier.fillMaxWidth()) {
        Row(modifier = Modifier.fillMaxWidth().padding(vertical = 4.dp)) {
            cells.forEach { cell ->
                Text(
                    inline(cell),
                    style = MaterialTheme.typography.bodyMedium,
                    fontWeight = if (header) FontWeight.Bold else null,
                    modifier = Modifier.weight(1f).padding(horizontal = 6.dp),
                )
            }
        }
        HorizontalDivider()
    }
}

private fun inline(text: String): AnnotatedString = buildAnnotatedString {
    text.split("**").forEachIndexed { index, part ->
        if (index % 2 == 1) withStyle(SpanStyle(fontWeight = FontWeight.Bold)) { append(part) } else append(part)
    }
}

private const val MANUAL_ASSET = "anleitung.md"
private val NUMBERED = Regex("""^(\d+)\.\s+(.*)$""")
private val TABLE_SEPARATOR = Regex("""^\|[\s:|-]+\|\s*$""")
private val MANUAL_MAX_WIDTH = 900.dp
private val MARK_WIDTH = 24.dp
