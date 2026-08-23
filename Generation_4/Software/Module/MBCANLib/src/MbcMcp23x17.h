//*************************************************************************************************
//* HEADER - MCP23017/MCP23S17 I2C-/SPI-GPIO-Expander (MBCANLib)                                  *
//*************************************************************************************************
//* Vereint den I2C-Baustein MCP23017 und seinen SPI-Schwesterbaustein MCP23S17 in einer Klasse - *
//* beide haben bei IOCON.BANK=0 (Reset-Zustand) identische Register, nur die Bus-Anbindung       *
//* unterscheidet sich. Der Konstruktor waehlt das Backend: MbcI2cBus+Bausteinnummer -> MCP23017, *
//* CS-Pin(+SPI-Bus) -> MCP23S17. Da beim SPI-Baustein jedes Geraet ohnehin einen eigenen /CS-Pin *
//* hat, wird dort - anders als beim I2C-Baustein - immer Opcode-Adresse 0 verwendet (IOCON.HAEN  *
//* bleibt aus).                                                                                  *
//*                                                                                               *
//* SPI hat kein ACK/NACK wie I2C - write*() melden dort daher, anders als beim I2C-Backend,      *
//* keine echten Uebertragungsfehler; nur begin() prueft per Ruecklesen von IODIRA, ob ueberhaupt *
//* ein Baustein antwortet.                                                                       *
//*                                                                                               *
//* Haelt zusaetzlich je Port einen Schattenwert des zuletzt geschriebenen GPIO-Registers         *
//* (writeGpioABit()/writeGpioBBit()), damit einzelne Bits (z.B. fuer mehrere unabhaengig         *
//* angesteuerte LEDs am selben Port, siehe MbcDigitalOut) geaendert werden koennen, ohne vor     *
//* jeder Aenderung eine Bus-Leseanfrage zu benoetigen - unabhaengig vom gewaehlten Backend.      *
//*************************************************************************************************
//* Datei: MbcMcp23x17.h                                                                          *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen:                                                                                  *
//* V1.0.0 - 05.08.2026 - Nullversion (Zusammenfuehrung von MbcMcp23017 und MbcMcp23s17)          *
//*************************************************************************************************

#pragma once

//###### Start Quellcode MbcMcp23x17.h ############################################################

#ifndef _MBCMCP23X17_H_
#define _MBCMCP23X17_H_     "V1.0.0 - 05.08.2026"

//------ Einbindung Bibliotheken ------------------------------------------------------------------

#include <Arduino.h>
#include <SPI.h>
#include "MbcI2cBus.h"
#include "MbcSpiDevice.h"

//------ Start globale Deklarationen --------------------------------------------------------------

    #define MBC_MCP23X17_BASE_ADDRESS  0x20    // I2C-Backend: 7-Bit-Basisadresse (8-Bit-Form: 0x40)

    enum MbcMcp23x17Backend
    {
        MBC_MCP23X17_BACKEND_I2C = 0,
        MBC_MCP23X17_BACKEND_SPI = 1
    };

//------ Ende globale Deklarationen ---------------------------------------------------------------

//------ Start Klassendefinition MbcMcp23x17 ------------------------------------------------------

    //* Klasse fuer <MbcMcp23x17>
    //* Ein Objekt = ein physischer MCP23017 (I2C) oder MCP23S17 (SPI), je nach genutztem
    //* Konstruktor (16-Bit-GPIO-Expander).

    class MbcMcp23x17
    {
        public:

            //*************************************************************************************
            //* Konstruktor fuer das I2C-Backend (MCP23017)                                       *
            //*************************************************************************************
            //* bus: zu nutzender I2C-Bus                                                         *
            //* adr: Bausteinnummer 0...7 (A2/A1/A0-Adresspins)                                   *
            //*************************************************************************************

            MbcMcp23x17(MbcI2cBus &bus, uint8_t adr);

            //*************************************************************************************
            //* Konstruktor fuer das SPI-Backend (MCP23S17)                                       *
            //*************************************************************************************
            //* pinCs:      /CS-Pin dieses Bausteins                                              *
            //* spi:        zu nutzende SPI-Schnittstelle (Standard: SPI)                         *
            //* spiClockHz: SPI-Takt in Hz (Standard: 10 MHz, siehe Datenblatt-Maximum)           *
            //*************************************************************************************

            explicit MbcMcp23x17(uint8_t pinCs, SPIClass &spi = SPI, uint32_t spiClockHz = 10000000);

            //*************************************************************************************
            //* Initialisieren: Datenrichtung und Ausgangswert je Port setzen                     *
            //*************************************************************************************
            //* dirA/dirB: je Bit 0 = Ausgang, 1 = Eingang                                        *
            //* ioA/ioB:   Ausgabewert (bei Eingangsbits ohne Wirkung)                            *
            //*************************************************************************************
            //* Return: false bei Bus-Fehler (I2C) bzw. wenn der Baustein nicht antwortet (SPI,   *
            //* IODIRA-Ruecklesepruefung)                                                         *
            //*************************************************************************************

            bool begin(uint8_t dirA, uint8_t ioA, uint8_t dirB, uint8_t ioB);

            //*************************************************************************************
            //* Datenrichtung eines Ports nachtraeglich aendern (0 = Ausgang, 1 = Eingang) Wird   *
            //* z.B. fuer den HD44780-Lesezugriff ueber MbcI2cLcd benoetigt, wo D4...D7 nur       *
            //* waehrend des Lesevorgangs auf Eingang geschaltet werden.                          *
            //*************************************************************************************
            //* dirA/dirB: neue Datenrichtung des jeweiligen Ports (0 = Ausgang, 1 = Eingang)     *
            //*************************************************************************************
            //* Return: false bei Bus-Fehler (I2C) bzw. wenn der Baustein nicht antwortet (SPI)   *
            //*************************************************************************************

            bool setDirA(uint8_t dirA);
            bool setDirB(uint8_t dirB);

            //*************************************************************************************
            //* Internen Pull-Up eines Ports setzen (nur wirksam fuer als Eingang konfigurierte   *
            //* Bits, siehe setDirA()/setDirB()/begin() - MCP23017/MCP23S17 kennen dafuer ein     *
            //* eigenes GPPU-Register, unabhaengig vom Ausgabewert aus writeGpioA()/writeGpioB()) *
            //*************************************************************************************
            //* pullA/pullB: je Bit 1 = Pull-Up aktiv, 0 = Pull-Up aus                            *
            //*************************************************************************************
            //* Return: false bei Bus-Fehler (I2C) bzw. wenn der Baustein nicht antwortet (SPI)   *
            //*************************************************************************************

            bool setPullUpA(uint8_t pullA);
            bool setPullUpB(uint8_t pullB);

            //*************************************************************************************
            //* Datenrichtung eines Ports auslesen (Gegenstueck zu setDirA()/setDirB())           *
            //*************************************************************************************
            //* Return: aktuelle Datenrichtung des jeweiligen Ports (0 = Ausgang, 1 = Eingang)    *
            //*************************************************************************************

            uint8_t getDirA();
            uint8_t getDirB();

            //*************************************************************************************
            //* GPIOA/GPIOB einzeln schreiben (aktualisiert auch den Schattenwert)                *
            //*************************************************************************************
            //* data: neuer Ausgabewert des jeweiligen Ports                                      *
            //*************************************************************************************
            //* Return: false bei Bus-Fehler (I2C) bzw. wenn der Baustein nicht antwortet (SPI)   *
            //*************************************************************************************

            bool writeGpioA(uint8_t data);
            bool writeGpioB(uint8_t data);

            //*************************************************************************************
            //* GPIOA und GPIOB gemeinsam schreiben (16 Bit)                                      *
            //*************************************************************************************
            //* dataA/dataB: neuer Ausgabewert je Port                                            *
            //*************************************************************************************
            //* Return: false bei Bus-Fehler (I2C) bzw. wenn der Baustein nicht antwortet (SPI)   *
            //*************************************************************************************

            bool writeGpio(uint8_t dataA, uint8_t dataB);

            //*************************************************************************************
            //* Einzelnes Bit von GPIOA/GPIOB aendern, Rest unveraendert (auf Basis des           *
            //* Schattenwerts, keine vorherige Leseanfrage noetig)                                *
            //*************************************************************************************
            //* bit:   0...7                                                                      *
            //* value: neuer Zustand des Bits                                                     *
            //*************************************************************************************
            //* Return: false bei Bus-Fehler (I2C) bzw. wenn der Baustein nicht antwortet (SPI)   *
            //*************************************************************************************

            bool writeGpioABit(uint8_t bit, bool value);
            bool writeGpioBBit(uint8_t bit, bool value);

            //*************************************************************************************
            //* GPIOA/GPIOB einzeln lesen                                                         *
            //*************************************************************************************
            //* Return: aktueller Zustand des jeweiligen Ports (bei Bus-Fehler unveraendert bzw.  *
            //* letzter bekannter Wert - siehe error())                                           *
            //*************************************************************************************

            uint8_t readGpioA();
            uint8_t readGpioB();

            //*************************************************************************************
            //* GPIOA und GPIOB gemeinsam lesen (16 Bit) - Gegenstueck zu writeGpio()             *
            //*************************************************************************************
            //* dataA/dataB: Zielvariablen je Port                                                *
            //*************************************************************************************
            //* Return: false bei I2C-Fehler (SPI: immer true, siehe Klassenkommentar)            *
            //*************************************************************************************

            bool readGpio(uint8_t &dataA, uint8_t &dataB);

            //*************************************************************************************
            //* Mehrere Werte nacheinander auf GPIOA/GPIOB ausgeben (z.B. Blinkmuster)            *
            //*************************************************************************************
            //* data: len auszugebende Werte, nacheinander                                        *
            //* len:  Anzahl der Werte                                                            *
            //*************************************************************************************
            //* Return: false bei Bus-Fehler (I2C) bzw. wenn der Baustein nicht antwortet (SPI)   *
            //*************************************************************************************

            bool writeGpioAPacket(const uint8_t *data, uint8_t len);
            bool writeGpioBPacket(const uint8_t *data, uint8_t len);

            //*************************************************************************************
            //* Ist beim letzten Zugriff ein Fehler aufgetreten? (SPI: nur nach begin()           *
            //* aussagekraeftig, siehe Klassenkommentar)                                          *
            //*************************************************************************************
            //* Return: true, wenn beim letzten Zugriff ein Fehler aufgetreten ist                *
            //*************************************************************************************

            bool error() const;

        private:

            MbcMcp23x17Backend _backend;

            MbcI2cBus *_bus;         // nur I2C-Backend
            uint8_t    _address7;    // nur I2C-Backend

            MbcSpiDevice _spiDevice; // nur SPI-Backend

            uint8_t _shadowA;
            uint8_t _shadowB;
            bool    _error;

            bool    writeReg(uint8_t addr, uint8_t data);
            uint8_t readReg(uint8_t addr);
    };

//------ Ende Klassendefinition MbcMcp23x17 -------------------------------------------------------

#endif  /* _MBCMCP23X17_H_ */

//###### Ende Quellcode MbcMcp23x17.h #############################################################
