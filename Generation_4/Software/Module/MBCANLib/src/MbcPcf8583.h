//*************************************************************************************************
//* HEADER - PCF8583 I2C-Echtzeituhr (MBCANLib)                                                   *
//*************************************************************************************************
//* Die Zeit-/Datumsstrukturen heissen MbcRtcTime/MbcRtcDate statt "time_t"/"date_t", um eine     *
//* Kollision mit den gleichnamigen Typen der C-Standardbibliothek zu vermeiden.                  *
//*                                                                                               *
//* Das Jahr wird ausschliesslich ueber das RAM-Byte 0x10 (Jahr-2000, siehe setClock()) verwaltet *
//* - die Century-Bits im Tagesregister werden beim Lesen bewusst NICHT zusaetzlich einbezogen,   *
//* da setClock() sie nicht schreibt und eine Kombination beider Quellen zu inkonsistenten        *
//* Jahreswerten fuehren wuerde.                                                                  *
//*************************************************************************************************
//* Datei: MbcPcf8583.h                                                                           *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen:                                                                                  *
//* V1.0.0 - 02.08.2026 - Nullversion                                                             *
//*************************************************************************************************

#pragma once

//###### Start Quellcode MbcPcf8583.h #############################################################

#ifndef _MBCPCF8583_H_
#define _MBCPCF8583_H_     "V1.0.0 - 02.08.2026"

//------ Einbindung Bibliotheken ------------------------------------------------------------------

#include <Arduino.h>
#include "MbcI2cBus.h"

//------ Start globale Deklarationen --------------------------------------------------------------

    #define MBC_PCF8583_ADDRESS   0x50    // 7-Bit-I2C-Adresse (8-Bit-Form: 0xA0)

    struct MbcRtcTime
    {
        uint8_t hour;      // Stunde (0-23)
        uint8_t minute;    // Minute (0-59)
        uint8_t second;    // Sekunde (0-59)
        uint8_t hsecond;   // Hundertstel-Sekunde
    };

    struct MbcRtcDate
    {
        uint8_t  day;         // Tag (1-31)
        uint8_t  month;       // Monat (1-12)
        uint8_t  dayofweek;   // Wochentag (0-6)
        uint16_t year;        // Jahr vierstellig (z.B. 2026)
    };

//------ Ende globale Deklarationen ---------------------------------------------------------------

//------ Start Klassendefinition MbcPcf8583 -------------------------------------------------------

    //* Klasse fuer <MbcPcf8583>
    //* Ein Objekt = ein physischer PCF8583 (I2C-Echtzeituhr mit RAM).

    class MbcPcf8583
    {
        public:

            //*************************************************************************************
            //* Konstruktor zur Deklaration von Objektvariablen                                   *
            //*************************************************************************************
            //* bus: zu nutzender I2C-Bus                                                         *
            //*************************************************************************************

            explicit MbcPcf8583(MbcI2cBus &bus);

            //*************************************************************************************
            //* Uhrzeit und Datum setzen                                                          *
            //*************************************************************************************
            //* t: zu setzende Uhrzeit                                                            *
            //* d: zu setzendes Datum                                                             *
            //*************************************************************************************
            //* Return: false bei I2C-Fehler                                                      *
            //*************************************************************************************

            bool setClock(const MbcRtcTime &t, const MbcRtcDate &d);

            //*************************************************************************************
            //* Uhrzeit und Datum lesen                                                           *
            //*************************************************************************************
            //* t: Zielvariable fuer die Uhrzeit                                                  *
            //* d: Zielvariable fuer das Datum                                                    *
            //*************************************************************************************
            //* Return: false bei I2C-Fehler                                                      *
            //*************************************************************************************

            bool getClock(MbcRtcTime &t, MbcRtcDate &d);

        private:

            MbcI2cBus *_bus;

            static uint8_t toBcd(uint8_t val);
            static uint8_t fromBcd(uint8_t val);
    };

//------ Ende Klassendefinition MbcPcf8583 --------------------------------------------------------

#endif  /* _MBCPCF8583_H_ */

//###### Ende Quellcode MbcPcf8583.h ##############################################################
