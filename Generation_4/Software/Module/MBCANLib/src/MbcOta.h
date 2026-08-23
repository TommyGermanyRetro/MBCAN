//*************************************************************************************************
//* HEADER - OTA-Firmware-Update aus dem externen Boot-Speicherchip (MBCANLib, RP2040/ESP32)      *
//*************************************************************************************************
//* Wendet ein per PC_BOOT/PC_UPGRADE_DATA in einem MbcBootStorage (Flash-Backend auf RP2040/     *
//* ESP32-C3, siehe dessen Kopfkommentar) zwischengespeichertes Firmware-Image auf die inaktive   *
//* Flash-Partition des arduino-pico- bzw. arduino-esp32-Cores an (jeweils eine global            *
//* instanzierte Update-Klasse, RP2040: <Updater.h>, ESP32: <Update.h> - beide mit zueinander     *
//* kompatibler begin()/write()/end()-Schnittstelle). Ein Abbruch bei Schreibfehler/CRC-Mismatch  *
//* erfolgt auf beiden Cores einheitlich ueber end(false) statt abort() - RP2040s UpdaterClass    *
//* kennt kein abort(), end(false) hat auf beiden Cores laut deren Doku-Kommentar dieselbe        *
//* Wirkung (verwirft ein unvollstaendiges/fehlerhaftes Update und setzt den internen Zustand     *
//* zurueck). Diese schreibt ausschliesslich in die inaktive Partition und schaltet den           *
//* Boot-Zeiger erst nach vollstaendiger, geprueft korrekter Uebertragung um - ein Abbruch        *
//* (Spannungsausfall, Fehler) laesst die aktuell laufende Firmware unangetastet.                 *
//*                                                                                               *
//* WICHTIG - IDE-Einstellung (Voraussetzung, kein Code): Beide Cores benoetigen eine             *
//* Board-Konfiguration mit reservierter zweiter (OTA-)Partition, sonst existiert keine Flaeche   *
//* zum Beschreiben: - RP2040 (arduino-pico): Tools -> Flash Size -> eine Variante mit            *
//* "...OTA..." waehlen (reserviert Sketch- und OTA-Partition je zur Haelfte/im gewaehlten        *
//* Verhaeltnis). - ESP32: Tools -> Partition Scheme -> ein Schema mit "...OTA..." waehlen (oder  *
//* ein eigenes partitions.csv mit otadata + zwei app-Partitionen hinterlegen). WICHTIG: die      *
//* tatsaechlich nutzbare Firmware-Groesse ist zusaetzlich durch die eigene OTA-Partitionsgroesse *
//* des Boards begrenzt, unabhaengig von der Kapazitaet des externen W25Q64 (8 MByte) - eine      *
//* kleine OTA-Partition begrenzt die reale Obergrenze weiter, auch wenn der Speicherchip mehr    *
//* Platz haette.                                                                                 *
//*                                                                                               *
//* Kontrakt mit der PC-seitigen Update-Software (nicht Teil dieser Bibliothek): Nach der letzten *
//* regulaeren PC_UPGRADE_DATA-Seite muss zusaetzlich ein 12-Byte-Header (MBC_OTA_MAGIC,          *
//* Gesamtlaenge, CRC32 - alle little-endian) auf MBC_OTA_HEADER_PAGE geschrieben werden. Ohne    *
//* gueltigen Header (falsches Magic, Laenge 0 oder zu gross, CRC-Mismatch) wird kein Update      *
//* angewendet.                                                                                   *
//*                                                                                               *
//* Verifiziert gegen die tatsaechlich installierten Core-Header (arduino-pico 6.0.0 Updater.h,   *
//* arduino-esp32 3.3.11 Update.h): begin()/write()/end() sind auf beiden Cores kompatibel,       *
//* abort() gibt es nur bei ESP32 (RP2040s UpdaterClass hat kein Gegenstueck, siehe oben).        *
//* MBC_OTA_HEADER_PAGE=32767 ist die letzte physische Page eines W25Q64 (8 MByte/32768           *
//* 256-Byte-Pages) und begrenzt nutzbare Imagegroessen auf 32767*256 Byte (~7,81 MByte).         *
//*************************************************************************************************
//* Datei: MbcOta.h                                                                               *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen:                                                                                  *
//* V1.0.0 - 05.08.2026 - Nullversion                                                             *
//*************************************************************************************************

#pragma once

//###### Start Quellcode MbcOta.h #################################################################

#if defined(ARDUINO_ARCH_RP2040) || defined(ESP32)

#ifndef _MBCOTA_H_
#define _MBCOTA_H_     "V1.0.0 - 05.08.2026"

//------ Einbindung Bibliotheken ------------------------------------------------------------------

#include <Arduino.h>

#if defined(ARDUINO_ARCH_RP2040)
    #include <Updater.h>
#elif defined(ESP32)
    #include <Update.h>
#endif

#include "MbcBootStorage.h"
#include "MbcUtils.h"

//------ Start globale Deklarationen --------------------------------------------------------------

    #define MBC_OTA_MAGIC          0x544F424Dul   // "MBOT" als little-endian uint32
    #define MBC_OTA_HEADER_PAGE    32767          // letzte physische Page eines W25Q64 (siehe oben)

//------ Ende globale Deklarationen ---------------------------------------------------------------

//------ Start Klassendefinition MbcOta -----------------------------------------------------------

    //* Klasse fuer <MbcOta>
    //* Statische Hilfsklasse (kein Objekt): prueft beim Neustart, ob im Boot-Speicherchip ein
    //* vollstaendiges, gueltiges Firmware-Image wartet, und wendet es ueber die Update-Klasse
    //* des jeweiligen Cores (RP2040: arduino-pico, ESP32: arduino-esp32) an.

    class MbcOta
    {
        public:

            MbcOta() = delete;

            //*************************************************************************************
            //* Anstehendes OTA-Update pruefen und ggf. anwenden. Nach dem Aufruf (egal ob true   *
            //* oder false) folgt in triggerReset() weiterhin ein normaler Neustart - bei true    *
            //* startet dieser in das neue Image, sonst unveraendert in das alte. Ein gueltiger   *
            //* Header wird nach einem abschliessenden Ergebnis (Erfolg oder definitiver Fehler   *
            //* wie CRC-Mismatch) immer invalidiert, damit er nicht bei jedem folgenden Reset     *
            //* (auch einem gewoehnlichen PC_RESET) erneut angewendet wird. Ein Stromausfall      *
            //* waehrend des Kopierens laesst den Header dagegen gueltig, sodass der Vorgang beim *
            //* naechsten Start automatisch wiederholt wird.                                      *
            //*************************************************************************************
            //* bootStorage: Boot-Speicherchip, in dem PC_UPGRADE_DATA das Image abgelegt hat     *
            //* utils:       optional - waehrenddessen mit update() weiterticken                  *
            //*************************************************************************************
            //* Return: true, wenn ein gueltiges Image gefunden und erfolgreich uebernommen wurde *
            //* (Update.end() erfolgreich); false, wenn kein/kein gueltiges Update anstand oder   *
            //* die Uebertragung fehlgeschlagen ist (kein Image angewendet).                      *
            //*************************************************************************************

            static bool apply(MbcBootStorage &bootStorage, MbcUtils *utils = nullptr);

        private:

            struct Header
            {
                uint32_t magic;
                uint32_t length;
                uint32_t crc32;
            };

            //* Header-Page mit Nullen ueberschreiben (Magic wird damit ungueltig), damit ein
            //* abgeschlossener (erfolgreicher oder endgueltig fehlgeschlagener) Update-Versuch
            //* nicht bei einem spaeteren Reset erneut angewendet wird.

            static void invalidateHeader(uint8_t *page, MbcBootStorage &bootStorage, MbcUtils *utils);

            static uint32_t crc32Update(uint32_t crc, const uint8_t *data, size_t len);
    };

//------ Ende Klassendefinition MbcOta ------------------------------------------------------------

#endif  /* _MBCOTA_H_ */

#endif  /* defined(ARDUINO_ARCH_RP2040) || defined(ESP32) */

//###### Ende Quellcode MbcOta.h ##################################################################
