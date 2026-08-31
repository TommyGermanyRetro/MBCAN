//*************************************************************************************************
//* HEADER - OTA-Firmware-Update aus dem externen Boot-Speicherchip (Mbc89Lib, ESP32-C3)          *
//*************************************************************************************************
//* Eigene, umbenannte Kopie von MBCANLib/MbcOta.h (siehe Mbc89Lib/README.md, "Warum                *
//* eigenstaendig") - wendet ein per PC_BOOT/PC_UPGRADE_DATA in einem Mbc89BootStorage              *
//* zwischengespeichertes Firmware-Image ueber <Update.h> auf die inaktive Flash-Partition des       *
//* arduino-esp32-Cores an. Ein Abbruch bei Schreibfehler/CRC-Mismatch erfolgt ueber end(false).     *
//* Schreibt ausschliesslich in die inaktive Partition und schaltet den Boot-Zeiger erst nach       *
//* vollstaendiger, geprueft korrekter Uebertragung um - ein Abbruch (Spannungsausfall, Fehler)      *
//* laesst die aktuell laufende Firmware unangetastet. NUR ESP32-C3 - auf ATmega644P entfaellt      *
//* diese Datei ersatzlos (kein Software-Reset, kein OTA-faehiger Flash).                            *
//*                                                                                                 *
//* WICHTIG - IDE-Einstellung (Voraussetzung, kein Code): Tools -> Partition Scheme -> ein Schema   *
//* mit "...OTA..." waehlen, oder ein eigenes partitions.csv mit otadata + zwei app-Partitionen     *
//* hinterlegen (siehe Mbc89_ESP32C3.ino/partitions.csv) - sonst existiert keine Flaeche zum        *
//* Beschreiben. Die tatsaechlich nutzbare Firmware-Groesse ist zusaetzlich durch die eigene         *
//* OTA-Partitionsgroesse des Boards begrenzt, unabhaengig von der Kapazitaet des externen W25Q64   *
//* (8 MByte).                                                                                       *
//*                                                                                                 *
//* Kontrakt mit der PC-seitigen Update-Software (nicht Teil dieser Bibliothek): Nach der letzten   *
//* regulaeren PC_UPGRADE_DATA-Seite muss zusaetzlich ein 12-Byte-Header (MBC_OTA_MAGIC,            *
//* Gesamtlaenge, CRC32 - alle little-endian) auf MBC_OTA_HEADER_PAGE geschrieben werden. Ohne      *
//* gueltigen Header (falsches Magic, Laenge 0 oder zu gross, CRC-Mismatch) wird kein Update        *
//* angewendet.                                                                                     *
//*                                                                                                 *
//* MBC_OTA_HEADER_PAGE=32767 ist die letzte physische Page eines W25Q64 (8 MByte/32768             *
//* 256-Byte-Pages) und begrenzt nutzbare Imagegroessen auf 32767*256 Byte (~7,81 MByte).            *
//*************************************************************************************************
//* Datei: Mbc89Ota.h                                                                              *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen:                                                                                  *
//* V1.0.0 - 27.08.2026 - Nullversion                                                             *
//*************************************************************************************************

#pragma once

//###### Start Quellcode Mbc89Ota.h ###############################################################

#if defined(ESP32)

#ifndef _MBC89OTA_H_
#define _MBC89OTA_H_     "V1.0.0 - 27.08.2026"

//------ Einbindung Bibliotheken ------------------------------------------------------------------

#include <Arduino.h>
#include <Update.h>

#include "Mbc89BootStorage.h"
#include "Mbc89Utils.h"

//------ Start globale Deklarationen --------------------------------------------------------------

    #define MBC_OTA_MAGIC          0x544F424Dul   // "MBOT" als little-endian uint32
    #define MBC_OTA_HEADER_PAGE    32767          // letzte physische Page eines W25Q64 (siehe oben)

//------ Ende globale Deklarationen ---------------------------------------------------------------

//------ Start Klassendefinition Mbc89Ota ----------------------------------------------------------

    //* Klasse fuer <Mbc89Ota>
    //* Statische Hilfsklasse (kein Objekt): prueft beim Neustart, ob im Boot-Speicherchip ein
    //* vollstaendiges, gueltiges Firmware-Image wartet, und wendet es ueber <Update.h> an.

    class Mbc89Ota
    {
        public:

            Mbc89Ota() = delete;

            //*************************************************************************************
            //* Anstehendes OTA-Update pruefen und ggf. anwenden. Nach dem Aufruf (egal ob true    *
            //* oder false) folgt in triggerReset() weiterhin ein normaler Neustart - bei true     *
            //* startet dieser in das neue Image, sonst unveraendert in das alte. Ein gueltiger    *
            //* Header wird nach einem abschliessenden Ergebnis (Erfolg oder definitiver Fehler    *
            //* wie CRC-Mismatch) immer invalidiert, damit er nicht bei jedem folgenden Reset      *
            //* (auch einem gewoehnlichen PC_RESET) erneut angewendet wird. Ein Stromausfall       *
            //* waehrend des Kopierens laesst den Header dagegen gueltig, sodass der Vorgang beim  *
            //* naechsten Start automatisch wiederholt wird.                                       *
            //*************************************************************************************
            //* bootStorage: Boot-Speicherchip, in dem PC_UPGRADE_DATA das Image abgelegt hat      *
            //* utils:       optional - waehrenddessen mit update() weiterticken                   *
            //*************************************************************************************
            //* Return: true, wenn ein gueltiges Image gefunden und erfolgreich uebernommen wurde  *
            //* (Update.end() erfolgreich); false, wenn kein/kein gueltiges Update anstand oder    *
            //* die Uebertragung fehlgeschlagen ist (kein Image angewendet).                       *
            //*************************************************************************************

            static bool apply(Mbc89BootStorage &bootStorage, Mbc89Utils *utils = nullptr);

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

            static void invalidateHeader(uint8_t *page, Mbc89BootStorage &bootStorage,
                                          Mbc89Utils *utils);

            static uint32_t crc32Update(uint32_t crc, const uint8_t *data, size_t len);
    };

//------ Ende Klassendefinition Mbc89Ota -----------------------------------------------------------

#endif  /* _MBC89OTA_H_ */

#endif  /* defined(ESP32) */

//###### Ende Quellcode Mbc89Ota.h #################################################################
