//*************************************************************************************************
//* MCP23017/MCP23S17 I2C-/SPI-GPIO-Expander (MBCANLib)                                           *
//*************************************************************************************************
//* Datei: MbcMcp23x17.cpp                                                                        *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen:                                                                                  *
//* V1.0.0 - 05.08.2026 - Nullversion (Zusammenfuehrung von MbcMcp23017 und MbcMcp23s17)          *
//*************************************************************************************************

//###### Start Quellcode MbcMcp23x17.cpp ##########################################################

#include "MbcMcp23x17.h"

//------ Start interne Konstanten (Register bei IOCON.BANK=0) -------------------------------------

    static const uint8_t REG_IODIRA = 0x00;
    static const uint8_t REG_IODIRB = 0x01;
    static const uint8_t REG_GPPUA  = 0x0C;
    static const uint8_t REG_GPPUB  = 0x0D;
    static const uint8_t REG_GPIOA  = 0x12;
    static const uint8_t REG_GPIOB  = 0x13;

    // SPI-Backend: Opcode immer mit Adresse 0 (IOCON.HAEN aus), siehe Klassenkommentar

    static const uint8_t SPI_OPCODE_WRITE = 0x40;
    static const uint8_t SPI_OPCODE_READ  = 0x41;

//------ Ende interne Konstanten ------------------------------------------------------------------

//------ Start Klasse MbcMcp23x17 -----------------------------------------------------------------

    //*********************************************************************************************
    //* Konstruktor fuer das I2C-Backend (MCP23017)                                               *
    //*********************************************************************************************
    //* bus: zu nutzender I2C-Bus                                                                 *
    //* adr: Bausteinnummer 0...7 (A2/A1/A0-Adresspins)                                           *
    //*********************************************************************************************

    MbcMcp23x17::MbcMcp23x17(MbcI2cBus &bus, uint8_t adr)
        : _backend(MBC_MCP23X17_BACKEND_I2C), _bus(&bus), _address7(MBC_MCP23X17_BASE_ADDRESS + adr),
          _spiDevice(255), _shadowA(0), _shadowB(0), _error(false)
    {
    }

    //*********************************************************************************************
    //* Konstruktor fuer das SPI-Backend (MCP23S17)                                               *
    //*********************************************************************************************
    //* pinCs:      /CS-Pin dieses Bausteins                                                      *
    //* spi:        zu nutzende SPI-Schnittstelle                                                 *
    //* spiClockHz: SPI-Takt in Hz                                                                *
    //*********************************************************************************************

    MbcMcp23x17::MbcMcp23x17(uint8_t pinCs, SPIClass &spi, uint32_t spiClockHz)
        : _backend(MBC_MCP23X17_BACKEND_SPI), _bus(nullptr), _address7(0),
          _spiDevice(pinCs, spiClockHz, spi), _shadowA(0), _shadowB(0), _error(false)
    {
    }

    //*********************************************************************************************
    //* Register schreiben - verzweigt je nach Konstruktor auf das I2C- oder das SPI-Backend      *
    //*********************************************************************************************
    //* addr: Registeradresse                                                                     *
    //* data: zu schreibendes Byte                                                                *
    //*********************************************************************************************
    //* Return: false bei Bus-Fehler (I2C) - beim SPI-Backend immer true, siehe Klassenkommentar  *
    //*********************************************************************************************

    bool MbcMcp23x17::writeReg(uint8_t addr, uint8_t data)
    {
        if (_backend == MBC_MCP23X17_BACKEND_I2C)
        {
            _error = !_bus->writeRegister(_address7, addr, data);
            return !_error;
        }

        _spiDevice.beginTransaction();

        _spiDevice.transfer(SPI_OPCODE_WRITE);
        _spiDevice.transfer(addr);
        _spiDevice.transfer(data);

        _spiDevice.endTransaction();

        _error = false;
        return true;
    }

    //*********************************************************************************************
    //* Register lesen - verzweigt je nach Konstruktor auf das I2C- oder das SPI-Backend          *
    //*********************************************************************************************
    //* addr: Registeradresse                                                                     *
    //*********************************************************************************************
    //* Return: Registerinhalt                                                                    *
    //*********************************************************************************************

    uint8_t MbcMcp23x17::readReg(uint8_t addr)
    {
        if (_backend == MBC_MCP23X17_BACKEND_I2C)
        {
            uint8_t data = 0;
            _error = !_bus->readRegister(_address7, addr, data);
            return data;
        }

        _spiDevice.beginTransaction();

        _spiDevice.transfer(SPI_OPCODE_READ);
        _spiDevice.transfer(addr);
        uint8_t data = _spiDevice.transfer(0x00);

        _spiDevice.endTransaction();

        _error = false;
        return data;
    }

    //*********************************************************************************************
    //* Initialisieren: Datenrichtung und Ausgangswert je Port setzen                             *
    //*********************************************************************************************
    //* dirA/dirB: je Bit 0 = Ausgang, 1 = Eingang                                                *
    //* ioA/ioB:   Ausgabewert (bei Eingangsbits ohne Wirkung)                                    *
    //*********************************************************************************************
    //* Return: false bei Bus-Fehler (I2C) bzw. wenn der Baustein nicht antwortet (SPI,           *
    //* IODIRA-Ruecklesepruefung)                                                                 *
    //*********************************************************************************************

    bool MbcMcp23x17::begin(uint8_t dirA, uint8_t ioA, uint8_t dirB, uint8_t ioB)
    {
        if (_backend == MBC_MCP23X17_BACKEND_SPI) _spiDevice.begin();

        _error = false;

        if (!writeReg(REG_IODIRA, dirA)) return false;
        if (!writeReg(REG_IODIRB, dirB)) return false;
        if (!writeGpioA(ioA))            return false;
        if (!writeGpioB(ioB))            return false;

        if (_backend == MBC_MCP23X17_BACKEND_SPI)
        {
            // SPI hat kein ACK - stattdessen Praesenzpruefung per Ruecklesen von IODIRA

            _error = (readReg(REG_IODIRA) != dirA);
            return !_error;
        }

        return true;
    }

    //*********************************************************************************************
    //* Datenrichtung von Port A nachtraeglich aendern                                            *
    //*********************************************************************************************
    //* dirA: neue Datenrichtung (0 = Ausgang, 1 = Eingang)                                       *
    //*********************************************************************************************
    //* Return: false bei Bus-Fehler (I2C) bzw. wenn der Baustein nicht antwortet (SPI)           *
    //*********************************************************************************************

    bool MbcMcp23x17::setDirA(uint8_t dirA)
    {
        return writeReg(REG_IODIRA, dirA);
    }

    //*********************************************************************************************
    //* Datenrichtung von Port B nachtraeglich aendern                                            *
    //*********************************************************************************************
    //* dirB: neue Datenrichtung (0 = Ausgang, 1 = Eingang)                                       *
    //*********************************************************************************************
    //* Return: false bei Bus-Fehler (I2C) bzw. wenn der Baustein nicht antwortet (SPI)           *
    //*********************************************************************************************

    bool MbcMcp23x17::setDirB(uint8_t dirB)
    {
        return writeReg(REG_IODIRB, dirB);
    }

    //*********************************************************************************************
    //* Internen Pull-Up von Port A setzen (nur wirksam fuer als Eingang konfigurierte Bits)      *
    //*********************************************************************************************
    //* pullA: je Bit 1 = Pull-Up aktiv, 0 = Pull-Up aus                                          *
    //*********************************************************************************************
    //* Return: false bei Bus-Fehler (I2C) bzw. wenn der Baustein nicht antwortet (SPI)           *
    //*********************************************************************************************

    bool MbcMcp23x17::setPullUpA(uint8_t pullA)
    {
        return writeReg(REG_GPPUA, pullA);
    }

    //*********************************************************************************************
    //* Internen Pull-Up von Port B setzen (nur wirksam fuer als Eingang konfigurierte Bits)      *
    //*********************************************************************************************
    //* pullB: je Bit 1 = Pull-Up aktiv, 0 = Pull-Up aus                                          *
    //*********************************************************************************************
    //* Return: false bei Bus-Fehler (I2C) bzw. wenn der Baustein nicht antwortet (SPI)           *
    //*********************************************************************************************

    bool MbcMcp23x17::setPullUpB(uint8_t pullB)
    {
        return writeReg(REG_GPPUB, pullB);
    }

    //*********************************************************************************************
    //* Datenrichtung von Port A auslesen                                                         *
    //*********************************************************************************************
    //* Return: aktuelle Datenrichtung (0 = Ausgang, 1 = Eingang)                                 *
    //*********************************************************************************************

    uint8_t MbcMcp23x17::getDirA()
    {
        return readReg(REG_IODIRA);
    }

    //*********************************************************************************************
    //* Datenrichtung von Port B auslesen                                                         *
    //*********************************************************************************************
    //* Return: aktuelle Datenrichtung (0 = Ausgang, 1 = Eingang)                                 *
    //*********************************************************************************************

    uint8_t MbcMcp23x17::getDirB()
    {
        return readReg(REG_IODIRB);
    }

    //*********************************************************************************************
    //* GPIOA schreiben                                                                           *
    //*********************************************************************************************
    //* data: neuer Ausgabewert                                                                   *
    //*********************************************************************************************
    //* Return: false bei Bus-Fehler (I2C) bzw. wenn der Baustein nicht antwortet (SPI)           *
    //*********************************************************************************************

    bool MbcMcp23x17::writeGpioA(uint8_t data)
    {
        _shadowA = data;
        return writeReg(REG_GPIOA, data);
    }

    //*********************************************************************************************
    //* GPIOB schreiben                                                                           *
    //*********************************************************************************************
    //* data: neuer Ausgabewert                                                                   *
    //*********************************************************************************************
    //* Return: false bei Bus-Fehler (I2C) bzw. wenn der Baustein nicht antwortet (SPI)           *
    //*********************************************************************************************

    bool MbcMcp23x17::writeGpioB(uint8_t data)
    {
        _shadowB = data;
        return writeReg(REG_GPIOB, data);
    }

    //*********************************************************************************************
    //* GPIOA und GPIOB gemeinsam schreiben (16 Bit)                                              *
    //*********************************************************************************************
    //* dataA/dataB: neuer Ausgabewert je Port                                                    *
    //*********************************************************************************************
    //* Return: false bei Bus-Fehler (I2C) bzw. wenn der Baustein nicht antwortet (SPI)           *
    //*********************************************************************************************

    bool MbcMcp23x17::writeGpio(uint8_t dataA, uint8_t dataB)
    {
        return writeGpioA(dataA) && writeGpioB(dataB);
    }

    //*********************************************************************************************
    //* Einzelnes Bit von GPIOA aendern, Rest unveraendert (Schattenwert)                         *
    //*********************************************************************************************
    //* bit:   0...7                                                                              *
    //* value: neuer Zustand des Bits                                                             *
    //*********************************************************************************************
    //* Return: false bei Bus-Fehler (I2C) bzw. wenn der Baustein nicht antwortet (SPI)           *
    //*********************************************************************************************

    bool MbcMcp23x17::writeGpioABit(uint8_t bit, bool value)
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
    //* Return: false bei Bus-Fehler (I2C) bzw. wenn der Baustein nicht antwortet (SPI)           *
    //*********************************************************************************************

    bool MbcMcp23x17::writeGpioBBit(uint8_t bit, bool value)
    {
        uint8_t data = _shadowB;

        if (value) data |= (uint8_t) (1 << bit);
        else       data &= (uint8_t) ~(1 << bit);

        return writeGpioB(data);
    }

    //*********************************************************************************************
    //* GPIOA lesen                                                                               *
    //*********************************************************************************************
    //* Return: aktueller Zustand von GPIOA                                                       *
    //*********************************************************************************************

    uint8_t MbcMcp23x17::readGpioA()
    {
        return readReg(REG_GPIOA);
    }

    //*********************************************************************************************
    //* GPIOB lesen                                                                               *
    //*********************************************************************************************
    //* Return: aktueller Zustand von GPIOB                                                       *
    //*********************************************************************************************

    uint8_t MbcMcp23x17::readGpioB()
    {
        return readReg(REG_GPIOB);
    }

    //*********************************************************************************************
    //* GPIOA und GPIOB gemeinsam lesen (16 Bit) - Gegenstueck zu writeGpio()                     *
    //*********************************************************************************************
    //* dataA/dataB: Zielvariablen je Port                                                        *
    //*********************************************************************************************
    //* Return: false bei I2C-Fehler (SPI: immer true, siehe Klassenkommentar)                    *
    //*********************************************************************************************

    bool MbcMcp23x17::readGpio(uint8_t &dataA, uint8_t &dataB)
    {
        dataA = readGpioA();
        if (_error) return false;

        dataB = readGpioB();
        return !_error;
    }

    //*********************************************************************************************
    //* Mehrere Werte nacheinander auf GPIOA ausgeben (z.B. Blinkmuster)                          *
    //*********************************************************************************************
    //* data: len auszugebende Werte, nacheinander                                                *
    //* len:  Anzahl der Werte                                                                    *
    //*********************************************************************************************
    //* Return: false bei Bus-Fehler (I2C) bzw. wenn der Baustein nicht antwortet (SPI)           *
    //*********************************************************************************************

    bool MbcMcp23x17::writeGpioAPacket(const uint8_t *data, uint8_t len)
    {
        for (uint8_t index = 0; index < len; index++)
        {
            if (!writeGpioA(data[index])) return false;
        }

        return true;
    }

    //*********************************************************************************************
    //* Mehrere Werte nacheinander auf GPIOB ausgeben (z.B. Blinkmuster)                          *
    //*********************************************************************************************
    //* data: len auszugebende Werte, nacheinander                                                *
    //* len:  Anzahl der Werte                                                                    *
    //*********************************************************************************************
    //* Return: false bei Bus-Fehler (I2C) bzw. wenn der Baustein nicht antwortet (SPI)           *
    //*********************************************************************************************

    bool MbcMcp23x17::writeGpioBPacket(const uint8_t *data, uint8_t len)
    {
        for (uint8_t index = 0; index < len; index++)
        {
            if (!writeGpioB(data[index])) return false;
        }

        return true;
    }

    //*********************************************************************************************
    //* Ist beim letzten Zugriff ein Fehler aufgetreten?                                          *
    //*********************************************************************************************
    //* Return: true, wenn beim letzten Zugriff ein Fehler aufgetreten ist                        *
    //*********************************************************************************************

    bool MbcMcp23x17::error() const
    {
        return _error;
    }

//------ Ende Klasse MbcMcp23x17 ------------------------------------------------------------------

//###### Ende Quellcode MbcMcp23x17.cpp ###########################################################
