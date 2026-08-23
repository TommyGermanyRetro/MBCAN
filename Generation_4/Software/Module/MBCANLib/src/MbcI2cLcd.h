//*************************************************************************************************
//* HEADER - HD44780-kompatibles I2C-LCD, PCF8574(A)- oder MCP23017-Backend (MBCANLib)            *
//*************************************************************************************************
//* Vereint den PCF8574(A)-Multi-Display-Treiber und den MCP23017-Display+8-Tasten-Keypad-Treiber *
//* (mbc-94-Bedienteil) in einer Klasse: Beide steuern denselben HD44780-4-Bit-Nibble-Bang-Bus    *
//* ueber die IDENTISCHE Pinbelegung eines I2C-GPIO-Expanders an (P0=/Enable, P1=RW, P2=RS,       *
//* P3=Backlight LOW-aktiv, P4...P7=D0...D3/D4...D7) - lediglich die Low-Level-Ansteuerung des    *
//* Expanders selbst unterscheidet sich (PCF8574(A) quasibidirektional ohne                       *
//* Datenrichtungsregister vs. MCP23017 mit explizitem IODIRA-Register). Ein Objekt = ein         *
//* physisches Display (mehrere Displays werden durch mehrere Instanzen abgebildet).              *
//*                                                                                               *
//* Zwei Konstruktoren waehlen das Backend. Die Keypad-Methode readKey() ist nur beim             *
//* MCP23017-Backend sinnvoll (mbc-94-Bedienteil) und liefert bei PCF8574(A)-Backend immer        *
//* MBC_I2CLCD_KEY_NONE.                                                                          *
//*************************************************************************************************
//* Datei: MbcI2cLcd.h                                                                            *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen:                                                                                  *
//* V1.0.0 - 02.08.2026 - Nullversion                                                             *
//*************************************************************************************************

#pragma once

//###### Start Quellcode MbcI2cLcd.h ##############################################################

#ifndef _MBCI2CLCD_H_
#define _MBCI2CLCD_H_     "V1.0.0 - 02.08.2026"

//------ Einbindung Bibliotheken ------------------------------------------------------------------

#include <Arduino.h>
#include "MbcI2cBus.h"
#include "MbcPcf8574.h"
#include "MbcMcp23x17.h"

//------ Start globale Deklarationen --------------------------------------------------------------

    #define MBC_I2CLCD_IR   0x00    // Instruction Register
    #define MBC_I2CLCD_DR   0x01    // Data Register

    #define MBC_I2CLCD_CURSOR_ON      0x01
    #define MBC_I2CLCD_CURSOR_OFF     0x02
    #define MBC_I2CLCD_CURSOR_BLINK   0x04
    #define MBC_I2CLCD_CURSOR_NORM    0x08
    #define MBC_I2CLCD_CURSOR_HOME    0x10
    #define MBC_I2CLCD_DISPLAY_ON     0x20
    #define MBC_I2CLCD_DISPLAY_OFF    0x40
    #define MBC_I2CLCD_DISPLAY_CLR    0x80

    #define MBC_I2CLCD_KEY_NONE   0
    #define MBC_I2CLCD_KEY_1      1
    #define MBC_I2CLCD_KEY_2      2
    #define MBC_I2CLCD_KEY_3      3
    #define MBC_I2CLCD_KEY_4      4
    #define MBC_I2CLCD_KEY_5      5
    #define MBC_I2CLCD_KEY_6      6
    #define MBC_I2CLCD_KEY_7      7
    #define MBC_I2CLCD_KEY_8      8

    enum MbcI2cLcdBackend
    {
        MBC_I2CLCD_BACKEND_PCF8574  = 0,
        MBC_I2CLCD_BACKEND_MCP23017 = 1
    };

    // Maximale Wartezeit in wbyte()/rbyte() auf das BUSY-Flag des Displays, bevor mit
    // Fehler abgebrochen wird (verhindert einen unbegrenzt langen Blockiervorgang bei
    // einem nicht angeschlossenen/defekten Display).

    #define MBC_I2CLCD_BUSY_TIMEOUT_MS   200

//------ Ende globale Deklarationen ---------------------------------------------------------------

//------ Start Klassendefinition MbcI2cLcd --------------------------------------------------------

    //* Klasse fuer <MbcI2cLcd>
    //* Ein Objekt = ein physisches HD44780-kompatibles LCD (bis zu 4 Zeilen), angesteuert
    //* ueber PCF8574(A) (mehrere Displays am Bus moeglich) oder MCP23017 (mbc-94-Stil,
    //* zusaetzlich 8-Tasten-Keypad ueber GPIOB desselben Bausteins).

    class MbcI2cLcd
    {
        public:

            //*************************************************************************************
            //* Konstruktor PCF8574(A)-Backend                                                    *
            //*************************************************************************************
            //* bus:      zu nutzender I2C-Bus                                                    *
            //* pcfAdr:   Bausteinnummer 0...7                                                    *
            //* pcfTyp:   MBC_PCF8574_TYP_8574 oder MBC_PCF8574_TYP_8574A                         *
            //* zeilen:   Anzahl der Zeilen des Displays (1...4)                                  *
            //* zeichen:  Anzahl der Zeichen je Zeile                                             *
            //*************************************************************************************

            MbcI2cLcd(MbcI2cBus &bus, uint8_t pcfAdr, MbcPcf8574Typ pcfTyp,
                      uint8_t zeilen, uint8_t zeichen);

            //*************************************************************************************
            //* Konstruktor MCP23017-Backend (mbc-94-Stil, mit Keypad)                            *
            //*************************************************************************************
            //* bus:      zu nutzender I2C-Bus                                                    *
            //* mcpAdr:   Bausteinnummer 0...7 des MCP23017                                       *
            //* zeilen:   Anzahl der Zeilen des Displays (1...4)                                  *
            //* zeichen:  Anzahl der Zeichen je Zeile                                             *
            //*************************************************************************************

            MbcI2cLcd(MbcI2cBus &bus, uint8_t mcpAdr, uint8_t zeilen, uint8_t zeichen);

            //*************************************************************************************
            //* Display initialisieren                                                            *
            //*************************************************************************************
            //* Return: false bei I2C-Fehler                                                      *
            //*************************************************************************************

            bool begin();

            //*************************************************************************************
            //* Cursor auf Zeile/Zeichenposition setzen (1-basiert)                               *
            //*************************************************************************************
            //* zeile: Zeile (1-basiert)                                                          *
            //* adr:   Zeichenposition (1-basiert)                                                *
            //*************************************************************************************
            //* Return: false bei I2C-Fehler                                                      *
            //*************************************************************************************

            bool gotoxy(uint8_t zeile, uint8_t adr);

            //*************************************************************************************
            //* Ein Zeichen an Position zeile/adr schreiben                                       *
            //*************************************************************************************
            //* zeile: Zeile (1-basiert)                                                          *
            //* adr:   Zeichenposition (1-basiert)                                                *
            //* data:  zu schreibendes Zeichen                                                    *
            //*************************************************************************************
            //* Return: false bei I2C-Fehler                                                      *
            //*************************************************************************************

            bool writeChar(uint8_t zeile, uint8_t adr, char data);

            //*************************************************************************************
            //* String ab Position zeile/adr schreiben (wird an Zeilenende abgeschnitten)         *
            //*************************************************************************************
            //* zeile: Zeile (1-basiert)                                                          *
            //* adr:   Zeichenposition (1-basiert)                                                *
            //* data:  nullterminierter zu schreibender String                                    *
            //*************************************************************************************
            //* Return: false bei I2C-Fehler                                                      *
            //*************************************************************************************

            bool writeStr(uint8_t zeile, uint8_t adr, const char *data);

            //*************************************************************************************
            //* String ab Position zeile/adr lesen (setzt RW-Verdrahtung des Displays voraus)     *
            //*************************************************************************************
            //* zeile: Zeile (1-basiert)                                                          *
            //* adr:   Zeichenposition (1-basiert)                                                *
            //* data:  Zielpuffer (mind. len+1 Byte, wird nullterminiert)                         *
            //* len:   Anzahl zu lesender Zeichen                                                 *
            //*************************************************************************************
            //* Return: false bei I2C-Fehler                                                      *
            //*************************************************************************************

            bool readStr(uint8_t zeile, uint8_t adr, char *data, uint8_t len);

            //*************************************************************************************
            //* Beleuchtung ein-/ausschalten                                                      *
            //*************************************************************************************
            //* on: true = Beleuchtung ein                                                        *
            //*************************************************************************************
            //* Return: false bei I2C-Fehler                                                      *
            //*************************************************************************************

            bool backlight(bool on);

            //*************************************************************************************
            //* Display-Kommando senden (Bitmasken MBC_I2CLCD_CURSOR_*/DISPLAY_*, kombinierbar)   *
            //*************************************************************************************
            //* command: zu sendendes Kommando (Bitmasken MBC_I2CLCD_CURSOR_*/DISPLAY_*)          *
            //*************************************************************************************
            //* Return: false bei I2C-Fehler                                                      *
            //*************************************************************************************

            bool command(uint8_t command);

            //*************************************************************************************
            //* Benutzerdefiniertes Zeichen in den Character-Generator-RAM schreiben              *
            //*************************************************************************************
            //* index:   0...7 im CGRAM                                                           *
            //* pattern: 8 Byte Bitmuster (je 5 unterste Bit je Zeile)                            *
            //*************************************************************************************

            void generateChar(uint8_t index, const uint8_t pattern[8]);

            //*************************************************************************************
            //* Tastatur lesen (nur MCP23017-Backend, sonst immer MBC_I2CLCD_KEY_NONE)            *
            //*************************************************************************************
            //* Return: MBC_I2CLCD_KEY_1...8 oder MBC_I2CLCD_KEY_NONE                             *
            //*************************************************************************************

            uint8_t readKey();

        private:

            MbcI2cLcdBackend _backend;

            MbcPcf8574  _pcf;
            MbcMcp23x17 _mcp;

            uint8_t _zeilen;
            uint8_t _zeichen;
            uint8_t _displaymode;
            bool    _backlightOn;

            uint8_t _keyIntegrator[8];   // Entprellung Tastatur (nur MCP-Backend genutzt)

            bool lowWrite(uint8_t data);
            bool lowRead(uint8_t &data);

            bool pollBusyOnce();
            bool waitReady();

            bool wbyte(uint8_t regdir, uint8_t data);
            bool rbyte(uint8_t regdir, uint8_t &data);

            uint8_t rowOffset(uint8_t zeile) const;
    };

//------ Ende Klassendefinition MbcI2cLcd ---------------------------------------------------------

#endif  /* _MBCI2CLCD_H_ */

//###### Ende Quellcode MbcI2cLcd.h ###############################################################
