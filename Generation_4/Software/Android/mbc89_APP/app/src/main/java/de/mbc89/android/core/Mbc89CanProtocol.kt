package de.mbc89.android.core

object Mbc89CanProtocol {
    const val CAN_ID_SYSTEM_COMMAND = 0x00
    const val CAN_ID_SYSTEM_COMMAND_RESPONSE = 0x01
    const val CAN_ID_MBCAN_ACCESSORY = 0x16
    const val CAN_ID_S88_EVENT_RESPONSE = 0x23
    const val CAN_ID_PING = 0x30
    const val CAN_ID_PING_RESPONSE = 0x31
    const val CAN_ID_CONFIGURATION = 0x3a
    const val CAN_ID_CONFIGURATION_RESPONSE = 0x3b
    const val CAN_ID_CONNECT6021 = 0x44
    const val CAN_ID_CONNECT6021_RESPONSE = 0x45

    const val FULL_DLC = 8
    const val MBCAN_LOCID = 0x18
    const val PC_LOCID_INDEX = 2
    const val PC_COMMAND_INDEX = 3

    const val PC_DB_HIGH = 0x00
    const val PC_DB_MIDDLE = 0x01
    const val PC_DB_LOW = 0x02
    const val PC_KENNER = 0x03
    const val PC_NEU = 0x04
    const val PC_NEU_DATA = 0x05
    const val PC_MD_NEU_DATA = 0x06
    const val PC_RESET = 0x07
    const val PC_MD_DEL = 0x08
    const val PC_ALIVE = 0x09
    const val PC_MD_ALIVE = 0x0a
    const val PC_ARRAY = 0x0b
    const val PC_MD_ARRAY = 0x0c
    const val PC_ARRAY_DATA = 0x0d
    const val PC_MD_ARRAY_DATA = 0x0e
    const val PC_UPGRADE = 0x0f
}
