//*************************************************************************************************
//* HEADER - Duenner SPI-Geraete-Wrapper (MBCANLib)                                               *
//*************************************************************************************************
//* Buendelt CS-Pin-Handling und SPI-Transaktionsklammer (SPISettings mit konfigurierbarem Takt)  *
//* an einer Stelle, statt dass jede SPI-Peripherieklasse (MbcCan, MbcBootStorage) ihre eigene    *
//* csLow()/csHigh()/beginTransaction()-Buchhaltung mitschleppt. Ein Objekt = ein SPI-Teilnehmer  *
//* (ein physischer Chip an einem /CS-Pin). Analog zu MbcI2cBus fuer I2C.                         *
//*************************************************************************************************
//* Datei: Mbc89SpiDevice.h                                                                       *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen:                                                                                  *
//* V1.0.0 - 05.08.2026 - Nullversion                                                             *
//*************************************************************************************************

#pragma once

//###### Start Quellcode Mbc89SpiDevice.h #########################################################

#ifndef _MBC89SPIDEVICE_H_
#define _MBC89SPIDEVICE_H_     "V1.0.0 - 05.08.2026"

//------ Einbindung Bibliotheken ------------------------------------------------------------------

#include <Arduino.h>
#include <SPI.h>

//------ Start Klassendefinition Mbc89SpiDevice ---------------------------------------------------

    //* Klasse fuer <Mbc89SpiDevice>
    //* Ein Objekt = ein SPI-Teilnehmer (ein Chip an einem /CS-Pin). Eine Transaktion besteht aus
    //* beginTransaction() (setzt Takt/Modus, zieht /CS auf LOW), ein oder mehreren transfer()-
    //* Aufrufen und abschliessendem endTransaction() (setzt /CS auf HIGH zurueck).

    class Mbc89SpiDevice
    {
        public:

            //*************************************************************************************
            //* Konstruktor zur Deklaration von Objektvariablen                                   *
            //*************************************************************************************
            //* pinCs:   /CS-Pin dieses SPI-Teilnehmers                                           *
            //* clockHz: SPI-Takt in Hz fuer diesen Teilnehmer                                    *
            //* spi:     zu nutzende SPI-Schnittstelle (Standard: SPI)                            *
            //*************************************************************************************

            explicit Mbc89SpiDevice(uint8_t pinCs, uint32_t clockHz = 4000000, SPIClass &spi = SPI);

            //*************************************************************************************
            //* /CS-Pin initialisieren (Ausgang, HIGH = inaktiv)                                  *
            //*************************************************************************************

            void begin();

            //*************************************************************************************
            //* Transaktion beginnen: SPI-Takt/-Modus setzen, /CS auf LOW                         *
            //*************************************************************************************

            void beginTransaction();

            //*************************************************************************************
            //* Transaktion beenden: /CS auf HIGH, SPI-Bus wieder freigeben                       *
            //*************************************************************************************

            void endTransaction();

            //*************************************************************************************
            //* Ein Byte senden/empfangen (nur innerhalb einer Transaktion gueltig)               *
            //*************************************************************************************
            //* data: zu sendendes Byte                                                           *
            //*************************************************************************************
            //* Return: empfangenes Byte                                                          *
            //*************************************************************************************

            uint8_t transfer(uint8_t data);

        private:

            uint8_t   _pinCs;
            uint32_t  _clockHz;
            SPIClass *_spi;

            // Direkter Port-Registerzugriff fuers /CS-Toggle statt digitalWrite() (nur AVR - dort
            // kostet jeder digitalWrite()-Aufruf durch Tabellen-Lookup und Interrupt-Sperre ein
            // Vielfaches eines direkten Registerzugriffs). Die Pin-/Bitmaske wird einmalig in
            // begin() ueber die Standard-Arduino-Makros digitalPinToPort()/digitalPinToBitMask()
            // ermittelt - kein hartkodiertes Register, also unabhaengig vom konkreten AVR-Board.

            #if defined(__AVR__)
                volatile uint8_t *_csPort;
                uint8_t           _csBitMask;
            #endif
    };

//------ Ende Klassendefinition Mbc89SpiDevice ----------------------------------------------------

#endif  /* _MBC89SPIDEVICE_H_ */

//###### Ende Quellcode Mbc89SpiDevice.h ##########################################################
