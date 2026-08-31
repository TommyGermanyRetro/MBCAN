//*************************************************************************************************
//* HEADER - Boot-Speicher-Chip fuer Firmware-Updates: 24LC1024-EEPROM oder W25Q64-Flash          *
//* (MBCANLib)                                                                                    *
//*************************************************************************************************
//* Ein Objekt = ein physischer externer SPI-Speicherchip, den MBCAN als Zwischenspeicher fuer    *
//* Firmware-Updates nutzt (Boot-EEPROM/Bootloader-Mechanismus, PC_BOOT/PC_UPGRADE_DATA). Zwei    *
//* austauschbare Backends in einer Klasse (analog zu MbcCan: MCP2515-SPI- vs.                    *
//* natives-TWAI-Backend, oder MbcMcp23x17: vereinte I2C+SPI-Klasse) - welches Backend ein Objekt *
//* nutzt, entscheidet der Konstruktor-Parameter backend: EEPROM_24LC1024 (1 MBit/128 KByte,      *
//* 256-Byte-Pages, byteweise ueberschreibbar) fuer den ATmega644P, FLASH_W25Q64 (8 MByte         *
//* NOR-Flash, 256-Byte-Pages, 4-KByte-Sektor-Granularitaet, erfordert Sector-Erase vor jedem     *
//* erneuten Beschreiben eines Sektors) fuer RP2040 und ESP32-C3, wo die 128-KByte-Kapazitaet     *
//* des bisherigen EEPROMs fuer groessere Firmware-Images zu klein war.                           *
//*                                                                                               *
//* Die eigentliche Staging-Flaeche dafuer (MBC_S_BOOT/MBC_L_BOOT/MBC_BOOT_PGSZ/MBC_BOOT_PGMAX)   *
//* bleibt Teil des Systemarrays in Mbc89Utils - nur der SPI-Zugriff auf den Chip selbst wandert  *
//* hierher, analog zu den anderen Peripherietreibern (MbcPcf8574, MbcMcp23x17, ...).             *
//*                                                                                               *
//* read()/write() dauern bauartbedingt mehrere Millisekunden (Schreib-/Loeschzyklus des Chips)   *
//* und nehmen dafuer optional eine Mbc89Utils-Instanz entgegen: ist sie angegeben, wird waehrend *
//* der Wartezeit Mbc89Utils::update() weitergetickt (Status-LED/Ereigniszaehler laufen weiter),  *
//* statt mit einem einfachen delay() den gesamten Sketch anzuhalten. Ohne Angabe (Standard)      *
//* verhaelt sich die Wartezeit wie ein gewoehnliches delay().                                    *
//*************************************************************************************************
//* Datei: Mbc89BootStorage.h                                                                     *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen:                                                                                  *
//* V1.0.0 - 09.08.2026 - Nullversion (aus Mbc24Lc1024 hervorgegangen, W25Q64-Flash-Backend neu)  *
//*************************************************************************************************

#pragma once

//###### Start Quellcode Mbc89BootStorage.h #######################################################

#ifndef _MBC89BOOTSTORAGE_H_
#define _MBC89BOOTSTORAGE_H_     "V1.0.0 - 09.08.2026"

//------ Einbindung Bibliotheken ------------------------------------------------------------------

#include <Arduino.h>
#include <SPI.h>
#include "Mbc89SpiDevice.h"
#include "Mbc89Utils.h"

//------ Start globale Deklarationen --------------------------------------------------------------

    // Seitengroesse - bei beiden Backends identisch (256-Byte-Page-Write/-Program)

    #define MBC_BOOTSTORAGE_PGSZ   256

    // SPI-Opcodes 24LC1024 (EEPROM-Backend) - Datenblatt 24LC1024

    #define MBC_24LC1024_WREN   0x06    // Write Enable
    #define MBC_24LC1024_RDSR   0x05    // Read Status Register
    #define MBC_24LC1024_READ   0x03    // Read Data
    #define MBC_24LC1024_WRITE  0x02    // Write Data

    // SPI-Opcodes W25Q64 (Flash-Backend) - JEDEC-Standardopcodes, Datenblatt W25Q64

    #define MBC_W25Q64_WREN     0x06    // Write Enable
    #define MBC_W25Q64_RDSR     0x05    // Read Status Register-1 (Bit0 = BUSY)
    #define MBC_W25Q64_READ     0x03    // Read Data
    #define MBC_W25Q64_PP       0x02    // Page Program
    #define MBC_W25Q64_SE       0x20    // Sector Erase (4 KByte)
    #define MBC_W25Q64_JEDEC_ID 0x9F    // Read JEDEC-ID (3 Byte: Hersteller/Typ/Kapazitaet)

    #define MBC_W25Q64_SECTOR_PAGES   16   // 4096 Byte / 256 Byte je Page
    #define MBC_W25Q64_JEDEC_ID_WINBOND_64  0xEF4017ul   // erwartete ID: Hersteller EF, Typ 40, Kapazitaet 17

    //* Welcher Speicherchip hinter einem Mbc89BootStorage-Objekt steckt

    enum class Mbc89BootStorageBackend { EEPROM_24LC1024, FLASH_W25Q64 };

//------ Ende globale Deklarationen ---------------------------------------------------------------

//------ Start Klassendefinition Mbc89BootStorage -------------------------------------------------

    //* Klasse fuer <Mbc89BootStorage>
    //* Ein Objekt = ein physischer externer Boot-Speicherchip (24LC1024-EEPROM oder W25Q64-Flash,
    //* siehe backend-Konstruktorparameter).

    class Mbc89BootStorage
    {
        public:

            //*************************************************************************************
            //* Konstruktor zur Deklaration von Objektvariablen                                   *
            //*************************************************************************************
            //* pinCs:      /CS-Pin dieses Chips                                                  *
            //* backend:    EEPROM_24LC1024 (Default, ATmega644P) oder FLASH_W25Q64 (RP2040/      *
            //*             ESP32-C3)                                                             *
            //* spi:        zu nutzende SPI-Schnittstelle (Standard: SPI)                         *
            //* spiClockHz: SPI-Takt in Hz (Standard: 4 MHz - fuer beide Backends konservativ     *
            //*             innerhalb der Spezifikation, bei Bedarf ueber den Konstruktor         *
            //*             anpassen)                                                             *
            //*************************************************************************************

            explicit Mbc89BootStorage(uint8_t pinCs,
                                     Mbc89BootStorageBackend backend = Mbc89BootStorageBackend::EEPROM_24LC1024,
                                     SPIClass &spi = SPI, uint32_t spiClockHz = 4000000);

            //*************************************************************************************
            //* /CS-Pin initialisieren. Beim Flash-Backend zusaetzlich die JEDEC-ID auslesen und  *
            //* gegen den erwarteten Winbond-W25Q64-Wert (EF/40/17) pruefen.                      *
            //*************************************************************************************
            //* Return: EEPROM-Backend immer true; Flash-Backend true nur bei passender JEDEC-ID  *
            //*************************************************************************************

            bool begin();

            //*************************************************************************************
            //* Lese eine Page (MBC_BOOTSTORAGE_PGSZ Byte)                                        *
            //*************************************************************************************
            //* buffer: Zielpuffer (mind. MBC_BOOTSTORAGE_PGSZ Byte)                              *
            //* page:   Speicherseite                                                             *
            //* utils:  optional - waehrenddessen mit update() weiterticken statt delay()         *
            //*************************************************************************************

            void read(uint8_t *buffer, uint16_t page, Mbc89Utils *utils = nullptr);

            //*************************************************************************************
            //* Schreibe eine Page (MBC_BOOTSTORAGE_PGSZ Byte). Beim Flash-Backend wird der       *
            //* enthaltende 4-KByte-Sektor bei Bedarf automatisch vorher geloescht (siehe .cpp).  *
            //*************************************************************************************
            //* buffer: Quellpuffer (MBC_BOOTSTORAGE_PGSZ Byte)                                   *
            //* page:   Speicherseite                                                             *
            //* utils:  optional - waehrenddessen mit update() weiterticken statt delay()         *
            //*************************************************************************************

            void write(uint8_t *buffer, uint16_t page, Mbc89Utils *utils = nullptr);

        private:

            Mbc89BootStorageBackend _backend;
            Mbc89SpiDevice          _spiDevice;

            // Nur Flash-Backend: Sektor-Loesch-Strategie fuer write() (siehe .cpp) - -1 = noch
            // kein Sektor in dieser Objekt-Lebensdauer geloescht/keine Page geschrieben

            int32_t _lastErasedSector;
            int32_t _lastWrittenPage;

            static void waitMs(unsigned long ms, Mbc89Utils *utils);

            bool beginEeprom();
            void readEeprom(uint8_t *buffer, uint16_t page, Mbc89Utils *utils);
            void writeEeprom(uint8_t *buffer, uint16_t page, Mbc89Utils *utils);

            bool beginFlash();
            void readFlash(uint8_t *buffer, uint16_t page, Mbc89Utils *utils);
            void writeFlash(uint8_t *buffer, uint16_t page, Mbc89Utils *utils);
            void eraseSectorFlash(uint16_t sector, Mbc89Utils *utils);
            void waitBusyFlash(Mbc89Utils *utils);
    };

//------ Ende Klassendefinition Mbc89BootStorage --------------------------------------------------

#endif  /* _MBC89BOOTSTORAGE_H_ */

//###### Ende Quellcode Mbc89BootStorage.h ########################################################
