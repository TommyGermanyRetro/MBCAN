//*************************************************************************************************
//* HEADER - Duenner I2C-Bus-Wrapper (MBCANLib)                                                   *
//*************************************************************************************************
//* Auf Arduino uebernimmt die Wire-Bibliothek Start-/Stop-/ACK-Handling bereits selbst -         *
//* MbcI2cBus beschraenkt sich daher auf die kleine Menge an                                      *
//* Bytepaket-/Register-Grundoperationen, die alle Peripherietreiber (MbcMcp23x17, MbcPcf8574,    *
//* MbcSsd1306, MbcAs5600, MbcPcf8583, MbcI2cLcd) tatsaechlich benoetigen.                        *
//*                                                                                               *
//* Wire arbeitet mit 7-Bit-Adressen; alle abgeleiteten Peripherieklassen nutzen daher die        *
//* halbierten Basisadressen ihrer jeweiligen Chip-Datenblaetter (z.B. 0x40 -> 0x20 fuer den      *
//* PCF8574), was rechnerisch identisch zu den 8-Bit-Adressbytes (mit verknuepftem R/W-Bit) auf   *
//* dem Bus ist.                                                                                  *
//*************************************************************************************************
//* Datei: MbcI2cBus.h                                                                            *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen:                                                                                  *
//* V1.0.0 - 02.08.2026 - Nullversion                                                             *
//*************************************************************************************************

#pragma once

//###### Start Quellcode MbcI2cBus.h ##############################################################

#ifndef _MBCI2CBUS_H_
#define _MBCI2CBUS_H_     "V1.0.0 - 02.08.2026"

//------ Einbindung Bibliotheken ------------------------------------------------------------------

#include <Arduino.h>
#include <Wire.h>

//------ Start Klassendefinition MbcI2cBus --------------------------------------------------------

    //* Klasse fuer <MbcI2cBus>
    //* Duenner Wrapper um Wire fuer die MBCAN-I2C-Peripherie. Ein Objekt = ein I2C-Bus
    //* (auf den meisten Zielen der einzige, per Default TWI/I2C0).

    class MbcI2cBus
    {
        public:

            //*************************************************************************************
            //* Konstruktor zur Deklaration von Objektvariablen                                   *
            //*************************************************************************************
            //* wire: zu nutzende TwoWire-Instanz (Standard: Wire)                                *
            //*************************************************************************************

            explicit MbcI2cBus(TwoWire &wire = Wire);

            //*************************************************************************************
            //* I2C-Bus initialisieren                                                            *
            //*************************************************************************************
            //* frequencyHz: Bustakt in Hz (Standard: 100 kHz)                                    *
            //*************************************************************************************

            void begin(uint32_t frequencyHz = 100000UL);

            //*************************************************************************************
            //* Baustein auf dem Bus suchen                                                       *
            //*************************************************************************************
            //* address7: 7-Bit-I2C-Adresse                                                       *
            //*************************************************************************************
            //* Return: true = Baustein antwortet mit ACK                                         *
            //*************************************************************************************

            bool scan(uint8_t address7);

            //*************************************************************************************
            //* Rohes Byte-Paket schreiben (eine Transaktion: Start-Adresse-Daten-Stop)           *
            //*************************************************************************************
            //* address7: 7-Bit-I2C-Adresse                                                       *
            //* data:     zu schreibende Byte                                                     *
            //* len:      Anzahl der Byte                                                         *
            //*************************************************************************************
            //* Return: true = Baustein antwortet mit ACK                                         *
            //*************************************************************************************

            bool writeBytes(uint8_t address7, const uint8_t *data, uint8_t len);

            //*************************************************************************************
            //* Rohes Byte-Paket lesen (eine Transaktion: Start-Adresse-Daten-Stop)               *
            //*************************************************************************************
            //* address7: 7-Bit-I2C-Adresse                                                       *
            //* data:     Zielpuffer (mind. len Byte)                                             *
            //* len:      Anzahl zu lesender Byte                                                 *
            //*************************************************************************************
            //* Return: true = Baustein antwortet mit ACK                                         *
            //*************************************************************************************

            bool readBytes(uint8_t address7, uint8_t *data, uint8_t len);

            //*************************************************************************************
            //* Ein Register schreiben (Registeradresse + 1 Datenbyte in einer Transaktion)       *
            //*************************************************************************************
            //* address7: 7-Bit-I2C-Adresse                                                       *
            //* reg:      Registeradresse                                                         *
            //* data:     zu schreibendes Byte                                                    *
            //*************************************************************************************
            //* Return: true = Baustein antwortet mit ACK                                         *
            //*************************************************************************************

            bool writeRegister(uint8_t address7, uint8_t reg, uint8_t data);

            //*************************************************************************************
            //* Ein Register lesen (Registeradresse schreiben, dann Datenbyte lesen)              *
            //*************************************************************************************
            //* address7: 7-Bit-I2C-Adresse                                                       *
            //* reg:      Registeradresse                                                         *
            //* data:     Zielvariable                                                            *
            //*************************************************************************************
            //* Return: true = Baustein antwortet mit ACK                                         *
            //*************************************************************************************

            bool readRegister(uint8_t address7, uint8_t reg, uint8_t &data);

            //*************************************************************************************
            //* Zugriff auf die zugrundeliegende TwoWire-Instanz (fuer Spezialfaelle)             *
            //*************************************************************************************
            //* Return: Referenz auf die zugrundeliegende TwoWire-Instanz                         *
            //*************************************************************************************

            TwoWire& wire();

        private:

            TwoWire *_wire;
    };

//------ Ende Klassendefinition MbcI2cBus ---------------------------------------------------------

#endif  /* _MBCI2CBUS_H_ */

//###### Ende Quellcode MbcI2cBus.h ###############################################################
