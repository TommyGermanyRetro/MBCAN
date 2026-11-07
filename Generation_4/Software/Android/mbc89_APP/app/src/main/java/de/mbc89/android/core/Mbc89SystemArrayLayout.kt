package de.mbc89.android.core

object Mbc89SystemArrayLayout {
    const val SIZE = 1208
    const val SUPPLY_VOLTAGE_OFFSET = 64
    const val TEMPERATURE_OFFSET = 67
    const val GUID_OFFSET = 42
    const val DATABASE_OFFSET = 46
    const val KENNUNG_OFFSET = 58
    const val CS2_GROUP_OFFSET = 62
    const val MODULE_EXTENSION_OFFSET = 328
    const val LOCOMOTIVE_OFFSET = MODULE_EXTENSION_OFFSET + 2
    const val PROTOCOL_SETTINGS_OFFSET = MODULE_EXTENSION_OFFSET + 2 + 810
    const val MAPPING_OFFSET = PROTOCOL_SETTINGS_OFFSET + 65
    const val KEYBOARD_BASE_OFFSET = MAPPING_OFFSET + 1
    const val I2C_FREQUENCY_OFFSET = KEYBOARD_BASE_OFFSET + 1
}
