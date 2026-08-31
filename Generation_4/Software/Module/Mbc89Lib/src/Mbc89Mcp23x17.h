//*************************************************************************************************
//* HEADER - MCP23S17 SPI-GPIO-Expander (Mbc89Lib, ESP32-C3)                                      *
//*************************************************************************************************
//* Abgespeckte, umbenannte Kopie von MBCANLib/MbcMcp23x17.h (siehe Mbc89Lib/README.md, "Warum     *
//* eigenstaendig") - NUR das SPI-Backend (MCP23S17). Das I2C-Backend (MCP23017) der Originalklasse*
//* wurde komplett entfernt, nicht nur ungenutzt gelassen: es haengt an MbcI2cBus/<Wire.h>, was    *
//* denselben TWI-Hardware-Konflikt ausloesen wuerde, den Mbc89Lib bewusst vermeidet (mbc-89 muss  *
//* die TWI-Hardware auf ATmega644P exklusiv besitzen, siehe Klassenkommentar in Mbc89Com.h).      *
//* Jedes SPI-Geraet hat ohnehin einen eigenen /CS-Pin, daher immer Opcode-Adresse 0 (IOCON.HAEN   *
//* bleibt aus). SPI hat kein ACK/NACK wie I2C - write*() melden daher keine echten                *
//* Uebertragungsfehler; nur begin() prueft per Ruecklesen von IODIRA, ob ueberhaupt ein Baustein  *
//* antwortet. Haelt zusaetzlich je Port einen Schattenwert des zuletzt geschriebenen GPIO-        *
//* Registers (writeGpioABit()/writeGpioBBit()), damit einzelne Bits geaendert werden koennen,     *
//* ohne vor jeder Aenderung eine Bus-Leseanfrage zu benoetigen.                                    *
//*************************************************************************************************
//* Datei: Mbc89Mcp23x17.h                                                                         *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen:                                                                                  *
//* V1.0.0 - 27.08.2026 - Nullversion                                                             *
//*************************************************************************************************

#pragma once

//###### Start Quellcode Mbc89Mcp23x17.h ##########################################################

#ifndef _MBC89MCP23X17_H_
#define _MBC89MCP23X17_H_     "V1.0.0 - 27.08.2026"

//------ Einbindung Bibliotheken ------------------------------------------------------------------

#include <Arduino.h>
#include <SPI.h>
#include "Mbc89SpiDevice.h"

//------ Start Klassendefinition Mbc89Mcp23x17 ----------------------------------------------------

    //* Klasse fuer <Mbc89Mcp23x17>
    //* Ein Objekt = ein physischer MCP23S17 (16-Bit-GPIO-Expander an SPI).

    class Mbc89Mcp23x17
    {
        public:

            //*************************************************************************************
            //* Konstruktor                                                                        *
            //*************************************************************************************
            //* pinCs:      /CS-Pin dieses Bausteins                                               *
            //* spi:        zu nutzende SPI-Schnittstelle (Standard: SPI)                          *
            //* spiClockHz: SPI-Takt in Hz (Standard: 10 MHz, siehe Datenblatt-Maximum)            *
            //*************************************************************************************

            explicit Mbc89Mcp23x17(uint8_t pinCs, SPIClass &spi = SPI, uint32_t spiClockHz = 10000000);

            //*************************************************************************************
            //* Initialisieren: Datenrichtung und Ausgangswert je Port setzen                     *
            //*************************************************************************************
            //* dirA/dirB: je Bit 0 = Ausgang, 1 = Eingang                                        *
            //* ioA/ioB:   Ausgabewert (bei Eingangsbits ohne Wirkung)                            *
            //*************************************************************************************
            //* Return: false, wenn der Baustein nicht antwortet (IODIRA-Ruecklesepruefung)        *
            //*************************************************************************************

            bool begin(uint8_t dirA, uint8_t ioA, uint8_t dirB, uint8_t ioB);

            //*************************************************************************************
            //* Datenrichtung eines Ports nachtraeglich aendern (0 = Ausgang, 1 = Eingang)         *
            //*************************************************************************************
            //* dirA/dirB: neue Datenrichtung des jeweiligen Ports                                *
            //*************************************************************************************
            //* Return: immer true (SPI hat kein ACK, siehe Klassenkommentar)                      *
            //*************************************************************************************

            bool setDirA(uint8_t dirA);
            bool setDirB(uint8_t dirB);

            //*************************************************************************************
            //* Internen Pull-Up eines Ports setzen (nur wirksam fuer als Eingang konfigurierte    *
            //* Bits)                                                                              *
            //*************************************************************************************
            //* pullA/pullB: je Bit 1 = Pull-Up aktiv, 0 = Pull-Up aus                             *
            //*************************************************************************************
            //* Return: immer true (SPI hat kein ACK, siehe Klassenkommentar)                      *
            //*************************************************************************************

            bool setPullUpA(uint8_t pullA);
            bool setPullUpB(uint8_t pullB);

            //*************************************************************************************
            //* Datenrichtung eines Ports auslesen                                                *
            //*************************************************************************************
            //* Return: aktuelle Datenrichtung des jeweiligen Ports (0 = Ausgang, 1 = Eingang)     *
            //*************************************************************************************

            uint8_t getDirA();
            uint8_t getDirB();

            //*************************************************************************************
            //* GPIOA/GPIOB einzeln schreiben (aktualisiert auch den Schattenwert)                *
            //*************************************************************************************
            //* data: neuer Ausgabewert des jeweiligen Ports                                      *
            //*************************************************************************************
            //* Return: immer true (SPI hat kein ACK, siehe Klassenkommentar)                      *
            //*************************************************************************************

            bool writeGpioA(uint8_t data);
            bool writeGpioB(uint8_t data);

            //*************************************************************************************
            //* GPIOA und GPIOB gemeinsam schreiben (16 Bit)                                      *
            //*************************************************************************************

            bool writeGpio(uint8_t dataA, uint8_t dataB);

            //*************************************************************************************
            //* Einzelnes Bit von GPIOA/GPIOB aendern, Rest unveraendert (auf Basis des           *
            //* Schattenwerts, keine vorherige Leseanfrage noetig)                                *
            //*************************************************************************************
            //* bit:   0...7                                                                      *
            //* value: neuer Zustand des Bits                                                     *
            //*************************************************************************************

            bool writeGpioABit(uint8_t bit, bool value);
            bool writeGpioBBit(uint8_t bit, bool value);

            //*************************************************************************************
            //* GPIOA/GPIOB einzeln lesen                                                         *
            //*************************************************************************************

            uint8_t readGpioA();
            uint8_t readGpioB();

            //*************************************************************************************
            //* GPIOA und GPIOB gemeinsam lesen (16 Bit) - Gegenstueck zu writeGpio()             *
            //*************************************************************************************
            //* Return: immer true (SPI hat kein ACK, siehe Klassenkommentar)                      *
            //*************************************************************************************

            bool readGpio(uint8_t &dataA, uint8_t &dataB);

            //*************************************************************************************
            //* Ist beim letzten begin() ein Fehler aufgetreten? (danach nicht mehr aussagekraeftig,*
            //* siehe Klassenkommentar)                                                            *
            //*************************************************************************************

            bool error() const;

        private:

            Mbc89SpiDevice _spiDevice;

            uint8_t _shadowA;
            uint8_t _shadowB;
            bool    _error;

            bool    writeReg(uint8_t addr, uint8_t data);
            uint8_t readReg(uint8_t addr);
    };

//------ Ende Klassendefinition Mbc89Mcp23x17 -----------------------------------------------------

#endif  /* _MBC89MCP23X17_H_ */

//###### Ende Quellcode Mbc89Mcp23x17.h ############################################################
