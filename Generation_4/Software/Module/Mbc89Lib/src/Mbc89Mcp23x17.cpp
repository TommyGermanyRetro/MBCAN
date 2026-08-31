//*************************************************************************************************
//* MCP23S17 SPI-GPIO-Expander (Mbc89Lib, ESP32-C3)                                               *
//*************************************************************************************************
//* Datei: Mbc89Mcp23x17.cpp                                                                      *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen:                                                                                  *
//* V1.0.0 - 27.08.2026 - Nullversion                                                             *
//*************************************************************************************************

//###### Start Quellcode Mbc89Mcp23x17.cpp ########################################################

#include "Mbc89Mcp23x17.h"

//------ Start interne Konstanten (Register bei IOCON.BANK=0) -------------------------------------

    static const uint8_t REG_IODIRA = 0x00;
    static const uint8_t REG_IODIRB = 0x01;
    static const uint8_t REG_GPPUA  = 0x0C;
    static const uint8_t REG_GPPUB  = 0x0D;
    static const uint8_t REG_GPIOA  = 0x12;
    static const uint8_t REG_GPIOB  = 0x13;

    // Opcode immer mit Adresse 0 (IOCON.HAEN aus), siehe Klassenkommentar

    static const uint8_t SPI_OPCODE_WRITE = 0x40;
    static const uint8_t SPI_OPCODE_READ  = 0x41;

//------ Ende interne Konstanten ------------------------------------------------------------------

//------ Start Klasse Mbc89Mcp23x17 ----------------------------------------------------------------

    //*********************************************************************************************
    //* Konstruktor                                                                               *
    //*********************************************************************************************
    //* pinCs:      /CS-Pin dieses Bausteins                                                      *
    //* spi:        zu nutzende SPI-Schnittstelle                                                 *
    //* spiClockHz: SPI-Takt in Hz                                                                *
    //*********************************************************************************************

    Mbc89Mcp23x17::Mbc89Mcp23x17(uint8_t pinCs, SPIClass &spi, uint32_t spiClockHz)
        : _spiDevice(pinCs, spiClockHz, spi), _shadowA(0), _shadowB(0), _error(false)
    {
    }

    //*********************************************************************************************
    //* Register schreiben                                                                        *
    //*********************************************************************************************
    //* addr: Registeradresse                                                                     *
    //* data: zu schreibendes Byte                                                                *
    //*********************************************************************************************
    //* Return: immer true (SPI hat kein ACK, siehe Klassenkommentar)                              *
    //*********************************************************************************************

    bool Mbc89Mcp23x17::writeReg(uint8_t addr, uint8_t data)
    {
        _spiDevice.beginTransaction();

        _spiDevice.transfer(SPI_OPCODE_WRITE);
        _spiDevice.transfer(addr);
        _spiDevice.transfer(data);

        _spiDevice.endTransaction();

        return true;
    }

    //*********************************************************************************************
    //* Register lesen                                                                            *
    //*********************************************************************************************
    //* addr: Registeradresse                                                                     *
    //*********************************************************************************************
    //* Return: Registerinhalt                                                                    *
    //*********************************************************************************************

    uint8_t Mbc89Mcp23x17::readReg(uint8_t addr)
    {
        _spiDevice.beginTransaction();

        _spiDevice.transfer(SPI_OPCODE_READ);
        _spiDevice.transfer(addr);
        uint8_t data = _spiDevice.transfer(0x00);

        _spiDevice.endTransaction();

        return data;
    }

    //*********************************************************************************************
    //* Initialisieren: Datenrichtung und Ausgangswert je Port setzen                             *
    //*********************************************************************************************
    //* dirA/dirB: je Bit 0 = Ausgang, 1 = Eingang                                                *
    //* ioA/ioB:   Ausgabewert (bei Eingangsbits ohne Wirkung)                                    *
    //*********************************************************************************************
    //* Return: false, wenn der Baustein nicht antwortet (IODIRA-Ruecklesepruefung)                *
    //*********************************************************************************************

    bool Mbc89Mcp23x17::begin(uint8_t dirA, uint8_t ioA, uint8_t dirB, uint8_t ioB)
    {
        _spiDevice.begin();

        writeReg(REG_IODIRA, dirA);
        writeReg(REG_IODIRB, dirB);
        writeGpioA(ioA);
        writeGpioB(ioB);

        // SPI hat kein ACK - stattdessen Praesenzpruefung per Ruecklesen von IODIRA

        _error = (readReg(REG_IODIRA) != dirA);
        return !_error;
    }

    //*********************************************************************************************
    //* Datenrichtung von Port A nachtraeglich aendern                                            *
    //*********************************************************************************************

    bool Mbc89Mcp23x17::setDirA(uint8_t dirA)
    {
        return writeReg(REG_IODIRA, dirA);
    }

    //*********************************************************************************************
    //* Datenrichtung von Port B nachtraeglich aendern                                            *
    //*********************************************************************************************

    bool Mbc89Mcp23x17::setDirB(uint8_t dirB)
    {
        return writeReg(REG_IODIRB, dirB);
    }

    //*********************************************************************************************
    //* Internen Pull-Up von Port A setzen                                                        *
    //*********************************************************************************************

    bool Mbc89Mcp23x17::setPullUpA(uint8_t pullA)
    {
        return writeReg(REG_GPPUA, pullA);
    }

    //*********************************************************************************************
    //* Internen Pull-Up von Port B setzen                                                        *
    //*********************************************************************************************

    bool Mbc89Mcp23x17::setPullUpB(uint8_t pullB)
    {
        return writeReg(REG_GPPUB, pullB);
    }

    //*********************************************************************************************
    //* Datenrichtung von Port A auslesen                                                         *
    //*********************************************************************************************

    uint8_t Mbc89Mcp23x17::getDirA()
    {
        return readReg(REG_IODIRA);
    }

    //*********************************************************************************************
    //* Datenrichtung von Port B auslesen                                                         *
    //*********************************************************************************************

    uint8_t Mbc89Mcp23x17::getDirB()
    {
        return readReg(REG_IODIRB);
    }

    //*********************************************************************************************
    //* GPIOA schreiben                                                                           *
    //*********************************************************************************************

    bool Mbc89Mcp23x17::writeGpioA(uint8_t data)
    {
        _shadowA = data;
        return writeReg(REG_GPIOA, data);
    }

    //*********************************************************************************************
    //* GPIOB schreiben                                                                           *
    //*********************************************************************************************

    bool Mbc89Mcp23x17::writeGpioB(uint8_t data)
    {
        _shadowB = data;
        return writeReg(REG_GPIOB, data);
    }

    //*********************************************************************************************
    //* GPIOA und GPIOB gemeinsam schreiben (16 Bit)                                              *
    //*********************************************************************************************

    bool Mbc89Mcp23x17::writeGpio(uint8_t dataA, uint8_t dataB)
    {
        return writeGpioA(dataA) && writeGpioB(dataB);
    }

    //*********************************************************************************************
    //* Einzelnes Bit von GPIOA aendern, Rest unveraendert (Schattenwert)                         *
    //*********************************************************************************************
    //* bit:   0...7                                                                              *
    //* value: neuer Zustand des Bits                                                             *
    //*********************************************************************************************

    bool Mbc89Mcp23x17::writeGpioABit(uint8_t bit, bool value)
    {
        uint8_t data = _shadowA;

        if (value) data |= (uint8_t) (1 << bit);
        else       data &= (uint8_t) ~(1 << bit);

        return writeGpioA(data);
    }

    //*********************************************************************************************
    //* Einzelnes Bit von GPIOB aendern, Rest unveraendert (Schattenwert)                         *
    //*********************************************************************************************
    //* bit:   0...7                                                                              *
    //* value: neuer Zustand des Bits                                                             *
    //*********************************************************************************************

    bool Mbc89Mcp23x17::writeGpioBBit(uint8_t bit, bool value)
    {
        uint8_t data = _shadowB;

        if (value) data |= (uint8_t) (1 << bit);
        else       data &= (uint8_t) ~(1 << bit);

        return writeGpioB(data);
    }

    //*********************************************************************************************
    //* GPIOA lesen                                                                               *
    //*********************************************************************************************

    uint8_t Mbc89Mcp23x17::readGpioA()
    {
        return readReg(REG_GPIOA);
    }

    //*********************************************************************************************
    //* GPIOB lesen                                                                               *
    //*********************************************************************************************

    uint8_t Mbc89Mcp23x17::readGpioB()
    {
        return readReg(REG_GPIOB);
    }

    //*********************************************************************************************
    //* GPIOA und GPIOB gemeinsam lesen (16 Bit) - Gegenstueck zu writeGpio()                     *
    //*********************************************************************************************

    bool Mbc89Mcp23x17::readGpio(uint8_t &dataA, uint8_t &dataB)
    {
        dataA = readGpioA();
        dataB = readGpioB();
        return true;
    }

    //*********************************************************************************************
    //* Ist beim letzten begin() ein Fehler aufgetreten?                                          *
    //*********************************************************************************************

    bool Mbc89Mcp23x17::error() const
    {
        return _error;
    }

//------ Ende Klasse Mbc89Mcp23x17 ------------------------------------------------------------------

//###### Ende Quellcode Mbc89Mcp23x17.cpp ##########################################################
