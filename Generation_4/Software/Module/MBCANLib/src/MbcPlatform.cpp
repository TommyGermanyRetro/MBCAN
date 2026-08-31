//*************************************************************************************************
//* Plattformabstraktion ATmega644P/RP2040/ESP32-C3 (MBCANLib)                                    *
//*************************************************************************************************
//* Datei: MbcPlatform.cpp                                                                        *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen:                                                                                  *
//* V1.0.0 - 05.08.2026 - Nullversion                                                             *
//*************************************************************************************************

//###### Start Quellcode MbcPlatform.cpp ##########################################################

#include "MbcPlatform.h"

#if defined(ESP32) || defined(ARDUINO_ARCH_RP2040)
    #include <EEPROM.h>
#endif

#if defined(ESP32)
    #include <esp_task_wdt.h>
#endif

//------ Start Klasse MbcPlatform -----------------------------------------------------------------

    //*********************************************************************************************
    //* Internes EEPROM vor dem ersten Zugriff reservieren (No-Op auf AVR)                        *
    //*********************************************************************************************
    //* size: benoetigte Groesse in Byte                                                          *
    //*********************************************************************************************

    void MbcPlatform::eepromBegin(size_t size)
    {
        #if defined(ESP32) || defined(ARDUINO_ARCH_RP2040)
            EEPROM.begin(size);
        #else
            (void) size;
        #endif
    }

    //*********************************************************************************************
    //* Internes EEPROM nach einem Schreibzugriff persistieren (No-Op auf AVR)                    *
    //*********************************************************************************************

    void MbcPlatform::eepromCommit()
    {
        #if defined(ESP32) || defined(ARDUINO_ARCH_RP2040)
            EEPROM.commit();
        #endif
    }

    //*********************************************************************************************
    //* Analog-Digital-Wandler auf die Aufloesung des ATmega644P (10 Bit) einstellen (No-Op auf   *
    //* AVR, dort ohnehin fest 10 Bit)                                                            *
    //*********************************************************************************************

    void MbcPlatform::adcConfigure()
    {
        #if !defined(__AVR__)
            analogReadResolution(10);
        #endif
    }

    //*********************************************************************************************
    //* Sofortneustart ueber den Software-Reset des Controllers (No-Op auf AVR)                   *
    //*********************************************************************************************

    void MbcPlatform::resetDevice()
    {
        #if defined(ARDUINO_ARCH_RP2040)
            rp2040.reboot();
        #elif defined(ESP32)
            ESP.restart();
        #endif
    }

    //*********************************************************************************************
    //* Neustart ueber einen extern beschalteten Reset-Pin (NUR AVR, sonst No-Op)                 *
    //*********************************************************************************************
    //* pin: Reset-Pin                                                                            *
    //*********************************************************************************************

    void MbcPlatform::resetViaPin(uint8_t pin)
    {
        #if defined(__AVR__)
            pinMode(pin, INPUT_PULLUP);
            digitalWrite(pin, HIGH);
            pinMode(pin, OUTPUT);
            digitalWrite(pin, LOW);
        #else
            (void) pin;
        #endif
    }

    //*********************************************************************************************
    //* Watchdog-Timer starten (Timeout ca. 8 s) - No-Op auf AVR (siehe MbcPlatform.h)            *
    //*********************************************************************************************

    void MbcPlatform::watchdogBegin()
    {
        #if defined(ARDUINO_ARCH_RP2040)
            rp2040.wdt_begin(8000);
        #elif defined(ESP32)
            esp_task_wdt_config_t wdtConfig = {8000, 1, true};   // ms, Core-0-Idle-Task, Panic (IDF5/Core-3.x-API)
            esp_task_wdt_init(&wdtConfig);
            esp_task_wdt_add(nullptr);
        #endif
    }

    //*********************************************************************************************
    //* Watchdog-Timer fuettern - No-Op auf AVR (siehe MbcPlatform.h)                             *
    //*********************************************************************************************

    void MbcPlatform::watchdogReset()
    {
        #if defined(ARDUINO_ARCH_RP2040)
            rp2040.wdt_reset();
        #elif defined(ESP32)
            esp_task_wdt_reset();
        #endif
    }

//------ Ende Klasse MbcPlatform ------------------------------------------------------------------

//###### Ende Quellcode MbcPlatform.cpp ###########################################################
