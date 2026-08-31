//*************************************************************************************************
//* Boot-Speicher-Chip fuer Firmware-Updates: 24LC1024-EEPROM oder W25Q64-Flash (MBCANLib)        *
//*************************************************************************************************
//* Datei: Mbc89BootStorage.cpp                                                                   *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen:                                                                                  *
//* V1.0.0 - 09.08.2026 - Nullversion (aus Mbc24Lc1024 hervorgegangen, W25Q64-Flash-Backend neu)  *
//*************************************************************************************************

//###### Start Quellcode Mbc89BootStorage.cpp #####################################################

#include "Mbc89BootStorage.h"

//------ Start Klasse Mbc89BootStorage ------------------------------------------------------------

    //*********************************************************************************************
    //* Konstruktor zur Deklaration von Objektvariablen                                           *
    //*********************************************************************************************
    //* pinCs:      /CS-Pin dieses Chips                                                          *
    //* backend:    EEPROM_24LC1024 oder FLASH_W25Q64                                             *
    //* spi:        zu nutzende SPI-Schnittstelle                                                 *
    //* spiClockHz: SPI-Takt in Hz                                                                *
    //*********************************************************************************************

    Mbc89BootStorage::Mbc89BootStorage(uint8_t pinCs, Mbc89BootStorageBackend backend,
                                    SPIClass &spi, uint32_t spiClockHz)
        : _backend(backend), _spiDevice(pinCs, spiClockHz, spi),
          _lastErasedSector(-1), _lastWrittenPage(-1)
    {
    }

    //*********************************************************************************************
    //* Wartet ms Millisekunden. Ist utils angegeben, wird waehrenddessen Mbc89Utils::update()    *
    //* weitergetickt, statt mit delay() den gesamten Sketch anzuhalten.                          *
    //*********************************************************************************************
    //* ms:    Wartezeit in Millisekunden                                                         *
    //* utils: optional - waehrenddessen mit update() weiterticken statt delay()                  *
    //*********************************************************************************************

    void Mbc89BootStorage::waitMs(unsigned long ms, Mbc89Utils *utils)
    {
        if (utils == nullptr)
        {
            delay(ms);
            return;
        }

        unsigned long start = millis();

        while ((unsigned long) (millis() - start) < ms)
        {
            utils->update();
        }
    }

    //*********************************************************************************************
    //* /CS-Pin initialisieren, dann an das gewaehlte Backend weiterreichen                       *
    //*********************************************************************************************
    //* Return: EEPROM-Backend immer true; Flash-Backend true nur bei passender JEDEC-ID          *
    //*********************************************************************************************

    bool Mbc89BootStorage::begin()
    {
        return (_backend == Mbc89BootStorageBackend::FLASH_W25Q64) ? beginFlash() : beginEeprom();
    }

    //*********************************************************************************************
    //* Lese eine Page - an das gewaehlte Backend weiterreichen                                   *
    //*********************************************************************************************
    //* buffer: Zielpuffer (mind. MBC_BOOTSTORAGE_PGSZ Byte)                                      *
    //* page:   Speicherseite                                                                     *
    //* utils:  optional - waehrenddessen mit update() weiterticken statt delay()                 *
    //*********************************************************************************************

    void Mbc89BootStorage::read(uint8_t *buffer, uint16_t page, Mbc89Utils *utils)
    {
        if (_backend == Mbc89BootStorageBackend::FLASH_W25Q64) readFlash(buffer, page, utils);
        else                                                 readEeprom(buffer, page, utils);
    }

    //*********************************************************************************************
    //* Schreibe eine Page - an das gewaehlte Backend weiterreichen                               *
    //*********************************************************************************************
    //* buffer: Quellpuffer (MBC_BOOTSTORAGE_PGSZ Byte)                                           *
    //* page:   Speicherseite                                                                     *
    //* utils:  optional - waehrenddessen mit update() weiterticken statt delay()                 *
    //*********************************************************************************************

    void Mbc89BootStorage::write(uint8_t *buffer, uint16_t page, Mbc89Utils *utils)
    {
        if (_backend == Mbc89BootStorageBackend::FLASH_W25Q64) writeFlash(buffer, page, utils);
        else                                                 writeEeprom(buffer, page, utils);
    }

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //+ EEPROM-Backend (24LC1024) - unveraendert aus der Vorgaengerklasse Mbc24Lc1024             +
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    //*********************************************************************************************
    //* /CS-Pin initialisieren                                                                    *
    //*********************************************************************************************
    //* Return: immer true                                                                        *
    //*********************************************************************************************

    bool Mbc89BootStorage::beginEeprom()
    {
        _spiDevice.begin();

        return true;
    }

    //*********************************************************************************************
    //* Lese eine Page (MBC_BOOTSTORAGE_PGSZ Byte)                                                *
    //*********************************************************************************************
    //* buffer: Zielpuffer (mind. MBC_BOOTSTORAGE_PGSZ Byte)                                      *
    //* page:   Speicherseite                                                                     *
    //* utils:  optional - waehrenddessen mit update() weiterticken statt delay()                 *
    //*********************************************************************************************

    void Mbc89BootStorage::readEeprom(uint8_t *buffer, uint16_t page, Mbc89Utils *utils)
    {
        uint32_t adresse = (uint32_t) MBC_BOOTSTORAGE_PGSZ * page;

        // 4 MHz SPI-Takt (Standard) liegt innerhalb der Spezifikation dieses Chips auf allen
        // drei Zielprozessoren; bei Bedarf ueber den Konstruktor anpassen.

        _spiDevice.beginTransaction();

        _spiDevice.transfer(MBC_24LC1024_READ);

        _spiDevice.transfer((uint8_t) ((adresse & 0x00ff0000) >> 16));
        _spiDevice.transfer((uint8_t) ((adresse & 0x0000ff00) >> 8));
        _spiDevice.transfer((uint8_t)  (adresse & 0x000000ff));

        for (uint16_t index = 0; index < MBC_BOOTSTORAGE_PGSZ; index++)
        {
            *(buffer + index) = _spiDevice.transfer(0xff);
        }

        _spiDevice.endTransaction();

        waitMs(50, utils);
    }

    //*********************************************************************************************
    //* Schreibe eine Page (MBC_BOOTSTORAGE_PGSZ Byte)                                            *
    //*********************************************************************************************
    //* buffer: Quellpuffer (MBC_BOOTSTORAGE_PGSZ Byte)                                           *
    //* page:   Speicherseite                                                                     *
    //* utils:  optional - waehrenddessen mit update() weiterticken statt delay()                 *
    //*********************************************************************************************

    void Mbc89BootStorage::writeEeprom(uint8_t *buffer, uint16_t page, Mbc89Utils *utils)
    {
        uint32_t adresse = (uint32_t) MBC_BOOTSTORAGE_PGSZ * page;

        // Write-Enable-Puls

        _spiDevice.beginTransaction();
        _spiDevice.transfer(MBC_24LC1024_WREN);
        _spiDevice.endTransaction();

        waitMs(10, utils);

        // Eigentlicher Schreibvorgang

        _spiDevice.beginTransaction();

        _spiDevice.transfer(MBC_24LC1024_WRITE);

        _spiDevice.transfer((uint8_t) ((adresse & 0x00ff0000) >> 16));
        _spiDevice.transfer((uint8_t) ((adresse & 0x0000ff00) >> 8));
        _spiDevice.transfer((uint8_t)  (adresse & 0x000000ff));

        for (uint16_t index = 0; index < MBC_BOOTSTORAGE_PGSZ; index++)
        {
            _spiDevice.transfer(*(buffer + index));
        }

        _spiDevice.endTransaction();

        waitMs(50, utils);
    }

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //+ Flash-Backend (W25Q64) - neu                                                              +
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    //*********************************************************************************************
    //* /CS-Pin initialisieren und JEDEC-ID pruefen                                               *
    //*********************************************************************************************
    //* Return: true nur bei passender Winbond-W25Q64-JEDEC-ID (EF/40/17)                         *
    //*********************************************************************************************

    bool Mbc89BootStorage::beginFlash()
    {
        _spiDevice.begin();

        _spiDevice.beginTransaction();

        _spiDevice.transfer(MBC_W25Q64_JEDEC_ID);

        uint32_t id = 0;
        id = ((uint32_t) _spiDevice.transfer(0xff)) << 16;
        id |= ((uint32_t) _spiDevice.transfer(0xff)) << 8;
        id |= (uint32_t) _spiDevice.transfer(0xff);

        _spiDevice.endTransaction();

        return id == MBC_W25Q64_JEDEC_ID_WINBOND_64;
    }

    //*********************************************************************************************
    //* Wartet, bis das BUSY-Bit im Statusregister zurueckgeht (laufender Schreib-/Loeschzyklus   *
    //* abgeschlossen) - anders als beim EEPROM (fixe Wartezeit reicht aus) sind die Zeiten fuer  *
    //* Sector-Erase (typ. ~400 ms, max. ~3 s) und Page-Program (typ. <1 ms, max. ~3 ms) zu       *
    //* unterschiedlich fuer eine sinnvolle feste Wartezeit. Ist utils angegeben, wird            *
    //* waehrenddessen Mbc89Utils::update() weitergetickt.                                        *
    //*********************************************************************************************
    //* utils: optional - waehrenddessen mit update() weiterticken statt delay()                  *
    //*********************************************************************************************

    void Mbc89BootStorage::waitBusyFlash(Mbc89Utils *utils)
    {
        while (true)
        {
            _spiDevice.beginTransaction();
            _spiDevice.transfer(MBC_W25Q64_RDSR);
            uint8_t status = _spiDevice.transfer(0xff);
            _spiDevice.endTransaction();

            if ((status & 0x01) == 0) return;

            if (utils != nullptr) utils->update();
        }
    }

    //*********************************************************************************************
    //* Lese eine Page (MBC_BOOTSTORAGE_PGSZ Byte) - identisches Adressformat/Opcode wie beim     *
    //* EEPROM-Backend, aber ohne Nachlaufzeit (Lesen ist beim Flash sofort abgeschlossen, kein   *
    //* Schreib-/Loeschzyklus)                                                                    *
    //*********************************************************************************************
    //* buffer: Zielpuffer (mind. MBC_BOOTSTORAGE_PGSZ Byte)                                      *
    //* page:   Speicherseite                                                                     *
    //* utils:  unbenutzt (Lesen ist beim Flash sofort abgeschlossen)                             *
    //*********************************************************************************************

    void Mbc89BootStorage::readFlash(uint8_t *buffer, uint16_t page, Mbc89Utils *utils)
    {
        (void) utils;

        uint32_t adresse = (uint32_t) MBC_BOOTSTORAGE_PGSZ * page;

        _spiDevice.beginTransaction();

        _spiDevice.transfer(MBC_W25Q64_READ);

        _spiDevice.transfer((uint8_t) ((adresse & 0x00ff0000) >> 16));
        _spiDevice.transfer((uint8_t) ((adresse & 0x0000ff00) >> 8));
        _spiDevice.transfer((uint8_t)  (adresse & 0x000000ff));

        for (uint16_t index = 0; index < MBC_BOOTSTORAGE_PGSZ; index++)
        {
            *(buffer + index) = _spiDevice.transfer(0xff);
        }

        _spiDevice.endTransaction();
    }

    //*********************************************************************************************
    //* 4-KByte-Sektor loeschen (setzt alle Bytes auf 0xff - Voraussetzung, um darin per          *
    //* Page-Program etwas anderes als eine Teilmenge der bisherigen Bits zu schreiben)           *
    //*********************************************************************************************
    //* sector: Sektorindex (0-basiert, ein Sektor = MBC_W25Q64_SECTOR_PAGES Pages)               *
    //* utils:  optional - waehrenddessen mit update() weiterticken statt delay()                 *
    //*********************************************************************************************

    void Mbc89BootStorage::eraseSectorFlash(uint16_t sector, Mbc89Utils *utils)
    {
        uint32_t adresse = (uint32_t) sector * MBC_W25Q64_SECTOR_PAGES * MBC_BOOTSTORAGE_PGSZ;

        _spiDevice.beginTransaction();
        _spiDevice.transfer(MBC_W25Q64_WREN);
        _spiDevice.endTransaction();

        _spiDevice.beginTransaction();

        _spiDevice.transfer(MBC_W25Q64_SE);

        _spiDevice.transfer((uint8_t) ((adresse & 0x00ff0000) >> 16));
        _spiDevice.transfer((uint8_t) ((adresse & 0x0000ff00) >> 8));
        _spiDevice.transfer((uint8_t)  (adresse & 0x000000ff));

        _spiDevice.endTransaction();

        waitBusyFlash(utils);
    }

    //*********************************************************************************************
    //* Schreibe eine Page (MBC_BOOTSTORAGE_PGSZ Byte). Loescht den enthaltenden 4-KByte-Sektor   *
    //* vorher automatisch, wenn dieser in der aktuellen Objekt-Lebensdauer noch nicht geloescht  *
    //* wurde ODER die Page-Nummer gegenueber dem letzten Schreibzugriff nicht angestiegen ist    *
    //* (deutet auf einen Neustart/Retry eines Uploads hin - erzwingt ein erneutes Loeschen, auch *
    //* wenn der Sektor vorher schon als geloescht galt, siehe Klassen-Kopfkommentar). Beim       *
    //* normalen, streng aufsteigenden PC_UPGRADE_DATA-Seitenstrom wird dadurch jeder Sektor      *
    //* genau einmal beim ersten Betreten geloescht, nicht vor jeder einzelnen Page.              *
    //*********************************************************************************************
    //* buffer: Quellpuffer (MBC_BOOTSTORAGE_PGSZ Byte)                                           *
    //* page:   Speicherseite                                                                     *
    //* utils:  optional - waehrenddessen mit update() weiterticken statt delay()                 *
    //*********************************************************************************************

    void Mbc89BootStorage::writeFlash(uint8_t *buffer, uint16_t page, Mbc89Utils *utils)
    {
        int32_t sector = (int32_t) (page / MBC_W25Q64_SECTOR_PAGES);

        if (((int32_t) page <= _lastWrittenPage) || (sector != _lastErasedSector))
        {
            eraseSectorFlash((uint16_t) sector, utils);
            _lastErasedSector = sector;
        }

        uint32_t adresse = (uint32_t) MBC_BOOTSTORAGE_PGSZ * page;

        _spiDevice.beginTransaction();
        _spiDevice.transfer(MBC_W25Q64_WREN);
        _spiDevice.endTransaction();

        _spiDevice.beginTransaction();

        _spiDevice.transfer(MBC_W25Q64_PP);

        _spiDevice.transfer((uint8_t) ((adresse & 0x00ff0000) >> 16));
        _spiDevice.transfer((uint8_t) ((adresse & 0x0000ff00) >> 8));
        _spiDevice.transfer((uint8_t)  (adresse & 0x000000ff));

        for (uint16_t index = 0; index < MBC_BOOTSTORAGE_PGSZ; index++)
        {
            _spiDevice.transfer(*(buffer + index));
        }

        _spiDevice.endTransaction();

        waitBusyFlash(utils);

        _lastWrittenPage = (int32_t) page;
    }

//------ Ende Klasse Mbc89BootStorage -------------------------------------------------------------

//###### Ende Quellcode Mbc89BootStorage.cpp ######################################################
