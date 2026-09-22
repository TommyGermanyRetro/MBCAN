//*************************************************************************************************
//* OTA-Firmware-Update aus dem externen Boot-Speicherchip (MBCANLib, RP2040/ESP32)               *
//*************************************************************************************************
//* Datei: MbcOta.cpp                                                                             *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen:                                                                                  *
//* V1.0.0 - 05.08.2026 - Nullversion                                                             *
//*************************************************************************************************

//###### Start Quellcode MbcOta.cpp ###############################################################

#include "MbcOta.h"

#if defined(ARDUINO_ARCH_RP2040) || defined(ESP32)

#include <string.h>

#if defined(ESP32)
    #include <esp_task_wdt.h>
#endif

//------ Start Klasse MbcOta ----------------------------------------------------------------------

    //*********************************************************************************************
    //* Anstehendes OTA-Update pruefen und ggf. anwenden                                          *
    //*********************************************************************************************
    //* bootStorage: Boot-Speicherchip, in dem PC_UPGRADE_DATA das Image abgelegt hat             *
    //* utils:       optional - waehrenddessen mit update() weiterticken                          *
    //*********************************************************************************************
    //* Return: true, wenn ein gueltiges Image gefunden und erfolgreich uebernommen wurde         *
    //* (Update.end() erfolgreich); false, wenn kein/kein gueltiges Update anstand oder die       *
    //* Uebertragung fehlgeschlagen ist (kein Image angewendet)                                   *
    //*********************************************************************************************

    bool MbcOta::apply(MbcBootStorage &bootStorage, MbcUtils *utils)
    {
        uint8_t page[MBC_BOOTSTORAGE_PGSZ];

        bootStorage.read(page, MBC_OTA_HEADER_PAGE, utils);

        Header header;
        memcpy(&header, page, sizeof(header));

        // Header ungueltig oder Image wuerde die Header-Page selbst ueberschreiben

        if (header.magic != MBC_OTA_MAGIC || header.length == 0
            || header.length > (uint32_t) MBC_OTA_HEADER_PAGE * MBC_BOOTSTORAGE_PGSZ)
        {
            return false;
        }

        if (!Update.begin(header.length))
        {
            invalidateHeader(page, bootStorage, utils);
            return false;
        }

        uint32_t crc = 0xFFFFFFFFul;
        uint32_t remaining = header.length;
        uint16_t otaPage = 0;

        while (remaining > 0)
        {
            uint16_t chunk = (remaining < MBC_BOOTSTORAGE_PGSZ) ? (uint16_t) remaining : MBC_BOOTSTORAGE_PGSZ;

            bootStorage.read(page, otaPage, utils);

            crc = crc32Update(crc, page, chunk);

            if (Update.write(page, chunk) != chunk)
            {
                Update.end(false);
                invalidateHeader(page, bootStorage, utils);
                return false;
            }

            remaining -= chunk;
            otaPage++;

            // Zwei UNABHAENGIGE ESP32-Watchdog-Mechanismen muessen hier bedient werden, da diese
            // Schleife (bis zu ~4200 Durchlaeufe fuer ein grosses Image) komplett innerhalb EINES
            // einzigen Aufrufs von loop() laeuft:
            //  1) CPU0-Idle-Task-Watchdog - braucht echte Rechenzeit fuer den Idle-Task. Nur
            //     delay() (nicht yield()/vPortYield(), das nur zu einem bereits lauffaehigen Task
            //     wechselt) erzwingt das zuverlaessig. Hardwarebestaetigt: ohne dieses delay(1)
            //     stuerzte das Modul mitten in crc32Update() per Panic ab.
            //  2) Der "loopTask"-eigene Watchdog-Eintrag - der wird laut arduino-esp32-Core
            //     (main.cpp) NUR ZWISCHEN zwei Aufrufen von loop() per esp_task_wdt_reset()
            //     zurueckgesetzt, niemals waehrenddessen - delay()/yield() fuettern ihn NICHT.
            //     Hardwarebestaetigt: mit delay(1) allein stuerzte das Modul weiterhin ab
            //     ("loopTask (CPU 0) did not reset the watchdog in time", Crash-PC in
            //     spi_flash_enable_interrupts_caches_and_other_cpu). Beide Mechanismen sind
            //     unabhaengig konfiguriert, beide werden deshalb explizit bedient.

            delay(1);

            #if defined(ESP32)
                esp_task_wdt_reset();
            #endif
        }

        crc ^= 0xFFFFFFFFul;

        if (crc != header.crc32)
        {
            Update.end(false);
            invalidateHeader(page, bootStorage, utils);
            return false;
        }

        bool ok = Update.end();

        invalidateHeader(page, bootStorage, utils);

        return ok;
    }

    //*********************************************************************************************
    //* Header-Page mit Nullen ueberschreiben (Magic wird damit ungueltig)                        *
    //*********************************************************************************************
    //* page:        Arbeitspuffer (mind. MBC_BOOTSTORAGE_PGSZ Byte, Inhalt wird ueberschrieben)  *
    //* bootStorage: Boot-Speicherchip, dessen Header-Page ungueltig gemacht wird                 *
    //* utils:       optional - waehrenddessen mit update() weiterticken                          *
    //*********************************************************************************************

    void MbcOta::invalidateHeader(uint8_t *page, MbcBootStorage &bootStorage, MbcUtils *utils)
    {
        memset(page, 0x00, MBC_BOOTSTORAGE_PGSZ);
        bootStorage.write(page, MBC_OTA_HEADER_PAGE, utils);
    }

    //*********************************************************************************************
    //* CRC32 (Poly 0xEDB88320, wie zlib/PNG) ueber einen Datenblock fortschreiben, ausgehend von *
    //* 0xFFFFFFFF und mit finalem XOR 0xFFFFFFFF durch den Aufrufer                              *
    //*********************************************************************************************
    //* crc:  bisheriger CRC-Zwischenstand                                                        *
    //* data: len Byte, ueber die fortgeschrieben wird                                            *
    //* len:  Anzahl der Byte in data                                                             *
    //*********************************************************************************************
    //* Return: neuer CRC-Zwischenstand                                                           *
    //*********************************************************************************************

    uint32_t MbcOta::crc32Update(uint32_t crc, const uint8_t *data, size_t len)
    {
        for (size_t index = 0; index < len; index++)
        {
            crc ^= data[index];

            for (uint8_t bit = 0; bit < 8; bit++)
            {
                crc = (crc & 1) ? ((crc >> 1) ^ 0xEDB88320ul) : (crc >> 1);
            }
        }

        return crc;
    }

//------ Ende Klasse MbcOta -----------------------------------------------------------------------

#endif  /* defined(ARDUINO_ARCH_RP2040) || defined(ESP32) */

//###### Ende Quellcode MbcOta.cpp ################################################################
