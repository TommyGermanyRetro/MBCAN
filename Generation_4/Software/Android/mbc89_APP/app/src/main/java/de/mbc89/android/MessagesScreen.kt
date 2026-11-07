package de.mbc89.android

import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.items
import androidx.compose.material3.HorizontalDivider
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.OutlinedButton
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp
import java.text.SimpleDateFormat
import java.util.Date
import java.util.Locale

// Messages of the running session, newest first, with time of day.
@Composable
fun MessagesScreen(messages: List<AppMessage>, onClear: () -> Unit) {
    val format = SimpleDateFormat("HH:mm:ss", Locale.GERMANY)
    Column(modifier = Modifier.fillMaxSize().padding(horizontal = 24.dp, vertical = 12.dp)) {
        Row(
            modifier = Modifier.fillMaxWidth(),
            horizontalArrangement = Arrangement.SpaceBetween,
            verticalAlignment = Alignment.CenterVertically,
        ) {
            Text("Meldungen dieser Sitzung", style = MaterialTheme.typography.titleMedium)
            OutlinedButton(onClick = onClear, enabled = messages.isNotEmpty()) { Text("Leeren") }
        }
        if (messages.isEmpty()) {
            Box(modifier = Modifier.fillMaxSize(), contentAlignment = Alignment.Center) {
                Text("Keine Meldungen", style = MaterialTheme.typography.bodyLarge)
            }
            return@Column
        }
        LazyColumn(modifier = Modifier.fillMaxSize()) {
            items(messages, key = { it.seq }) { message ->
                Row(modifier = Modifier.fillMaxWidth().padding(vertical = 6.dp)) {
                    Text(
                        format.format(Date(message.time)),
                        style = MaterialTheme.typography.bodyMedium,
                        modifier = Modifier.width(TIME_WIDTH),
                    )
                    Text(message.text, style = MaterialTheme.typography.bodyMedium)
                }
                HorizontalDivider()
            }
        }
    }
}

private val TIME_WIDTH = 90.dp
