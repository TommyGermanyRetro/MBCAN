//*************************************************************************************************
//* Duenner I2C-Bus-Wrapper (MBCANLib)                                                            *
//*************************************************************************************************
//* Datei: MbcI2cBus.cpp                                                                          *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen:                                                                                  *
//* V1.0.0 - 02.08.2026 - Nullversion                                                             *
//*************************************************************************************************

//###### Start Quellcode MbcI2cBus.cpp ############################################################

#include "MbcI2cBus.h"

//------ Start Klasse MbcI2cBus -------------------------------------------------------------------

    //*********************************************************************************************
    //* Konstruktor zur Deklaration von Objektvariablen                                           *
    //*********************************************************************************************
    //* wire: zu nutzende TwoWire-Instanz                                                         *
    //*********************************************************************************************

    MbcI2cBus::MbcI2cBus(TwoWire &wire)
        : _wire(&wire)
    {
    }

    //*********************************************************************************************
    //* I2C-Bus initialisieren                                                                    *
    //*********************************************************************************************
    //* frequencyHz: Bustakt in Hz                                                                *
    //*********************************************************************************************

    void MbcI2cBus::begin(uint32_t frequencyHz)
    {
        _wire->begin();
        _wire->setClock(frequencyHz);
    }

    //*********************************************************************************************
    //* Baustein auf dem Bus suchen                                                               *
    //*********************************************************************************************
    //* address7: 7-Bit-I2C-Adresse                                                               *
    //*********************************************************************************************
    //* Return: true, wenn der Baustein mit ACK antwortet                                         *
    //*********************************************************************************************

    bool MbcI2cBus::scan(uint8_t address7)
    {
        _wire->beginTransmission(address7);
        return (_wire->endTransmission() == 0);
    }

    //*********************************************************************************************
    //* Rohes Byte-Paket schreiben (eine Transaktion: Start-Adresse-Daten-Stop)                   *
    //*********************************************************************************************
    //* address7: 7-Bit-I2C-Adresse                                                               *
    //* data:     zu schreibende Bytes                                                            *
    //* len:      Anzahl der zu schreibenden Bytes                                                *
    //*********************************************************************************************
    //* Return: true bei Erfolg. false auch, wenn Wire.write() wegen eines zu kleinen             *
    //* Sendepuffers (typ. 32 Byte auf AVR) weniger Bytes als len uebernommen hat - in diesem     *
    //* Fall wird nicht gesendet, um ein stilles Abschneiden der Nachricht zu vermeiden.          *
    //*********************************************************************************************

    bool MbcI2cBus::writeBytes(uint8_t address7, const uint8_t *data, uint8_t len)
    {
        _wire->beginTransmission(address7);

        if (_wire->write(data, len) != len)
        {
            _wire->endTransmission();
            return false;
        }

        return (_wire->endTransmission() == 0);
    }

    //*********************************************************************************************
    //* Rohes Byte-Paket lesen (eine Transaktion: Start-Adresse-Daten-Stop)                       *
    //*********************************************************************************************
    //* address7: 7-Bit-I2C-Adresse                                                               *
    //* data:     Zielpuffer                                                                      *
    //* len:      Anzahl der zu lesenden Bytes                                                    *
    //*********************************************************************************************
    //* Return: true bei Erfolg                                                                   *
    //*********************************************************************************************

    bool MbcI2cBus::readBytes(uint8_t address7, uint8_t *data, uint8_t len)
    {
        if (_wire->requestFrom((int) address7, (int) len) != len) return false;

        for (uint8_t index = 0; index < len; index++)
        {
            data[index] = _wire->read();
        }

        return true;
    }

    //*********************************************************************************************
    //* Ein Register schreiben (Registeradresse + 1 Datenbyte in einer Transaktion)               *
    //*********************************************************************************************
    //* address7: 7-Bit-I2C-Adresse                                                               *
    //* reg:      Registeradresse                                                                 *
    //* data:     zu schreibendes Byte                                                            *
    //*********************************************************************************************
    //* Return: true bei Erfolg                                                                   *
    //*********************************************************************************************

    bool MbcI2cBus::writeRegister(uint8_t address7, uint8_t reg, uint8_t data)
    {
        uint8_t buf[2] = { reg, data };

        return writeBytes(address7, buf, 2);
    }

    //*********************************************************************************************
    //* Ein Register lesen (Registeradresse schreiben, dann Datenbyte lesen)                      *
    //*********************************************************************************************
    //* address7: 7-Bit-I2C-Adresse                                                               *
    //* reg:      Registeradresse                                                                 *
    //* data:     Zielvariable                                                                    *
    //*********************************************************************************************
    //* Return: true bei Erfolg                                                                   *
    //*********************************************************************************************

    bool MbcI2cBus::readRegister(uint8_t address7, uint8_t reg, uint8_t &data)
    {
        if (!writeBytes(address7, &reg, 1)) return false;

        return readBytes(address7, &data, 1);
    }

    //*********************************************************************************************
    //* Zugriff auf die zugrundeliegende TwoWire-Instanz (fuer Spezialfaelle)                     *
    //*********************************************************************************************
    //* Return: Referenz auf die zugrundeliegende TwoWire-Instanz                                 *
    //*********************************************************************************************

    TwoWire& MbcI2cBus::wire()
    {
        return *_wire;
    }

//------ Ende Klasse MbcI2cBus --------------------------------------------------------------------

//###### Ende Quellcode MbcI2cBus.cpp #############################################################
