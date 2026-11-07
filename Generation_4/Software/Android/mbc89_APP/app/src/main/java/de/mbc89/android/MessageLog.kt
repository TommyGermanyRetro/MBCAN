package de.mbc89.android

import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.asStateFlow

data class AppMessage(val seq: Long, val time: Long, val text: String)

// Messages of the running session (connection events, unsent commands, storage and protocol errors)
// for the messages tab; service and view model write here, newest first, at most MAX_MESSAGES.
object MessageLog {
    private const val MAX_MESSAGES = 200
    private var nextSeq = 1L
    private val _messages = MutableStateFlow<List<AppMessage>>(emptyList())
    val messages: StateFlow<List<AppMessage>> = _messages.asStateFlow()

    @Synchronized
    fun add(text: String) {
        val message = AppMessage(nextSeq++, System.currentTimeMillis(), text)
        _messages.value = (listOf(message) + _messages.value).take(MAX_MESSAGES)
    }

    @Synchronized
    fun clear() {
        _messages.value = emptyList()
    }
}
