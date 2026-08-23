//*************************************************************************************************
//* PCF8583 I2C-Echtzeituhr (MBCANLib)                                                            *
//*************************************************************************************************
//* Datei: MbcPcf8583.cpp                                                                         *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen:                                                                                  *
//* V1.0.0 - 02.08.2026 - Nullversion                                                             *
//*************************************************************************************************

//###### Start Quellcode MbcPcf8583.cpp ###########################################################

#include "MbcPcf8583.h"

//------ Start Klasse MbcPcf8583 ------------------------------------------------------------------

    //*********************************************************************************************
    //* Konstruktor zur Deklaration von Objektvariablen                                           *
    //*********************************************************************************************
    //* bus: zu nutzender I2C-Bus                                                                 *
    //*********************************************************************************************

    MbcPcf8583::MbcPcf8583(MbcI2cBus &bus)
        : _bus(&bus)
    {
    }

    //*********************************************************************************************
    //* Wandle eine Dezimalzahl (0...99) in BCD um                                                *
    //*********************************************************************************************
    //* val: Dezimalwert                                                                          *
    //*********************************************************************************************
    //* Return: BCD-kodierter Wert                                                                *
    //*********************************************************************************************

    uint8_t MbcPcf8583::toBcd(uint8_t val)
    {
        return (uint8_t) (((val / 10) << 4) | (val % 10));
    }

    //*********************************************************************************************
    //* Wandle einen BCD-Wert in eine Dezimalzahl (0...99) um                                     *
    //*********************************************************************************************
    //* val: BCD-kodierter Wert                                                                   *
    //*********************************************************************************************
    //* Return: Dezimalwert                                                                       *
    //*********************************************************************************************

    uint8_t MbcPcf8583::fromBcd(uint8_t val)
    {
        return (uint8_t) ((val >> 4) * 10 + (val & 0x0f));
    }

    //*********************************************************************************************
    //* Uhrzeit und Datum setzen                                                                  *
    //*********************************************************************************************
    //* t: zu setzende Uhrzeit                                                                    *
    //* d: zu setzendes Datum                                                                     *
    //*********************************************************************************************
    //* Return: false bei I2C-Fehler                                                              *
    //*********************************************************************************************

    bool MbcPcf8583::setClock(const MbcRtcTime &t, const MbcRtcDate &d)
    {
        uint8_t reg[10];

        reg[0] = 0x00;                                 // Control/Status-Register
        reg[1] = 0x00;                                  // Control/Status-Wert
        reg[2] = 0x00;                                  // Hundertstelsekunden
        reg[3] = toBcd(t.second);
        reg[4] = toBcd(t.minute);
        reg[5] = toBcd(t.hour);
        reg[6] = toBcd(d.day);
        reg[7] = (uint8_t) (toBcd(d.month) | (d.dayofweek << 5));
        reg[8] = 0x00;                                  // Timer
        reg[9] = 0x00;                                  // Alarm-Control-Register

        if (!_bus->writeBytes(MBC_PCF8583_ADDRESS, reg, 10)) return false;

        // Jahr (als Jahr-2000) in RAM-Byte 0x10 ablegen

        uint8_t yearReg[2] = { 0x10, (uint8_t) (d.year - 2000) };

        return _bus->writeBytes(MBC_PCF8583_ADDRESS, yearReg, 2);
    }

    //*********************************************************************************************
    //* Uhrzeit und Datum lesen                                                                   *
    //*********************************************************************************************
    //* t: Zielvariable fuer die Uhrzeit                                                          *
    //* d: Zielvariable fuer das Datum                                                            *
    //*********************************************************************************************
    //* Return: false bei I2C-Fehler                                                              *
    //*********************************************************************************************

    bool MbcPcf8583::getClock(MbcRtcTime &t, MbcRtcDate &d)
    {
        uint8_t ptr;

        ptr = 0x02;
        if (!_bus->writeBytes(MBC_PCF8583_ADDRESS, &ptr, 1)) return false;

        uint8_t reg[5];
        if (!_bus->readBytes(MBC_PCF8583_ADDRESS, reg, 5)) return false;

        ptr = 0x10;
        if (!_bus->writeBytes(MBC_PCF8583_ADDRESS, &ptr, 1)) return false;

        uint8_t yearOffset;
        if (!_bus->readBytes(MBC_PCF8583_ADDRESS, &yearOffset, 1)) return false;

        t.second   = fromBcd(reg[0]);
        t.minute   = fromBcd(reg[1]);
        t.hour     = fromBcd(reg[2] & 0x3f);
        t.hsecond  = 0;

        d.day        = fromBcd(reg[3] & 0x3f);
        d.dayofweek  = (reg[4] & 0xe0) >> 5;
        d.month      = fromBcd(reg[4] & 0x1f);
        d.year       = (uint16_t) (2000 + yearOffset);

        return true;
    }

//------ Ende Klasse MbcPcf8583 -------------------------------------------------------------------

//###### Ende Quellcode MbcPcf8583.cpp ############################################################
