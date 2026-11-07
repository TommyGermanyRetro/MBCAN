package de.mbc89.android.transport

import kotlinx.coroutines.flow.Flow
import kotlinx.coroutines.flow.StateFlow

interface CanTransport {
    val state: StateFlow<ConnectionState>
    val receivedPackets: Flow<CanPacket>

    suspend fun connect(host: String, port: Int)
    suspend fun send(packet: CanPacket)
    suspend fun disconnect()
    fun dispose()
}
