//*************************************************************************************************
//* HD44780-kompatibles I2C-LCD, PCF8574(A)- oder MCP23017-Backend (MBCANLib)                     *
//*************************************************************************************************
//* Datei: MbcI2cLcd.cpp                                                                          *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen:                                                                                  *
//* V1.0.0 - 02.08.2026 - Nullversion                                                             *
//*************************************************************************************************

//###### Start Quellcode MbcI2cLcd.cpp ############################################################

#include "MbcI2cLcd.h"
#include <string.h>

//------ Start interne Konstanten -----------------------------------------------------------------

    // Zeilen-Offsets im DDRAM (deckt 2- und 4-zeilige HD44780-Displays ab)

    static const uint8_t ROW_OFFSET[4] = { 0x00, 0x40, 0x10, 0x50 };

//------ Ende interne Konstanten ------------------------------------------------------------------

//------ Start Klasse MbcI2cLcd -------------------------------------------------------------------

    //*********************************************************************************************
    //* Konstruktor PCF8574(A)-Backend                                                            *
    //*********************************************************************************************
    //* bus:      zu nutzender I2C-Bus                                                            *
    //* pcfAdr:   Bausteinnummer 0...7                                                            *
    //* pcfTyp:   MBC_PCF8574_TYP_8574 oder MBC_PCF8574_TYP_8574A                                 *
    //* zeilen:   Anzahl der Zeilen des Displays (1...4)                                          *
    //* zeichen:  Anzahl der Zeichen je Zeile                                                     *
    //*********************************************************************************************

    MbcI2cLcd::MbcI2cLcd(MbcI2cBus &bus, uint8_t pcfAdr, MbcPcf8574Typ pcfTyp,
                          uint8_t zeilen, uint8_t zeichen)
        : _backend(MBC_I2CLCD_BACKEND_PCF8574),
          _pcf(bus, pcfAdr, pcfTyp),
          _mcp(bus, 0),
          _zeilen(zeilen), _zeichen(zeichen), _displaymode(0x0f), _backlightOn(false)
    {
        for (uint8_t i = 0; i < 8; i++) _keyIntegrator[i] = 1;
    }

    //*********************************************************************************************
    //* Konstruktor MCP23017-Backend (mbc-94-Stil, mit Keypad)                                    *
    //*********************************************************************************************
    //* bus:      zu nutzender I2C-Bus                                                            *
    //* mcpAdr:   Bausteinnummer 0...7 des MCP23017                                               *
    //* zeilen:   Anzahl der Zeilen des Displays (1...4)                                          *
    //* zeichen:  Anzahl der Zeichen je Zeile                                                     *
    //*********************************************************************************************

    MbcI2cLcd::MbcI2cLcd(MbcI2cBus &bus, uint8_t mcpAdr, uint8_t zeilen, uint8_t zeichen)
        : _backend(MBC_I2CLCD_BACKEND_MCP23017),
          _pcf(bus, 0),
          _mcp(bus, mcpAdr),
          _zeilen(zeilen), _zeichen(zeichen), _displaymode(0x0f), _backlightOn(false)
    {
        for (uint8_t i = 0; i < 8; i++) _keyIntegrator[i] = 1;
    }

    //*********************************************************************************************
    //* Ein Byte an den Expander schreiben - je nach Backend an PCF8574(A) oder an GPIOA des      *
    //* MCP23017                                                                                  *
    //*********************************************************************************************
    //* data: zu schreibendes Byte                                                                *
    //*********************************************************************************************
    //* Return: false bei I2C-Fehler                                                              *
    //*********************************************************************************************

    bool MbcI2cLcd::lowWrite(uint8_t data)
    {
        if (_backend == MBC_I2CLCD_BACKEND_PCF8574) return _pcf.write(data);

        return _mcp.writeGpioA(data);
    }

    //*********************************************************************************************
    //* Ein Byte vom Expander lesen - je nach Backend von PCF8574(A) oder von GPIOA des MCP23017  *
    //*********************************************************************************************
    //* data: Zielvariable                                                                        *
    //*********************************************************************************************
    //* Return: false bei I2C-Fehler                                                              *
    //*********************************************************************************************

    bool MbcI2cLcd::lowRead(uint8_t &data)
    {
        if (_backend == MBC_I2CLCD_BACKEND_PCF8574) return _pcf.read(data);

        data = _mcp.readGpioA();
        return !_mcp.error();
    }

    //*********************************************************************************************
    //* BUSY-Flag des LCD-Controllers einmalig abfragen (kein Timeout, siehe waitReady())         *
    //*********************************************************************************************
    //* Return: true, wenn das Display noch BUSY ist                                              *
    //*********************************************************************************************

    bool MbcI2cLcd::pollBusyOnce()
    {
        // PCF8574(A) ist quasibidirektional: D4...D7 muessen zum Lesen explizit auf HIGH
        // gelegt werden (releaseBits). Der MCP23017 hat dagegen ein echtes Datenrichtungs-
        // register (IODIRA) - dort spielt der ausgegebene Pegel fuer die auf Eingang
        // geschalteten Bits keine Rolle.

        uint8_t releaseBits = (_backend == MBC_I2CLCD_BACKEND_PCF8574) ? 0xf0 : 0x00;

        if (_backend == MBC_I2CLCD_BACKEND_MCP23017) _mcp.setDirA(0xf0);

        uint8_t ctrl = (uint8_t) (releaseBits | 0x02);   // RW = 1 (lesen), EN = 0
        if (_backlightOn) ctrl |= 0x08;

        uint8_t c0 = ctrl;
        uint8_t c1 = (uint8_t) (ctrl | 0x01);

        uint8_t nibble;

        lowWrite(c0);
        lowWrite(c1);
        lowRead(nibble);
        uint8_t busyNibble = (uint8_t) (nibble & 0xf0);

        lowWrite(c0);
        lowWrite(c1);
        lowRead(nibble);          // 2. Nibble, Wert wird nicht ausgewertet

        lowWrite(c0);

        if (_backend == MBC_I2CLCD_BACKEND_MCP23017) _mcp.setDirA(0x00);

        return (busyNibble & 0x80) != 0;
    }

    //*********************************************************************************************
    //* Wartet, bis das BUSY-Flag des LCD-Controllers zurueckgeht (Timeout                        *
    //* MBC_I2CLCD_BUSY_TIMEOUT_MS, verhindert einen unbegrenzt langen Blockiervorgang bei einem  *
    //* nicht angeschlossenen/defekten Display)                                                   *
    //*********************************************************************************************
    //* Return: false bei Timeout                                                                 *
    //*********************************************************************************************

    bool MbcI2cLcd::waitReady()
    {
        unsigned long start = millis();

        do
        {
            if (!pollBusyOnce()) return true;

        } while ((unsigned long) (millis() - start) < MBC_I2CLCD_BUSY_TIMEOUT_MS);

        return false;
    }

    //*********************************************************************************************
    //* Ein Byte im 4-Bit-Nibble-Bang-Protokoll an das Display schreiben                          *
    //*********************************************************************************************
    //* regdir: MBC_I2CLCD_IR (Instruktion) oder MBC_I2CLCD_DR (Daten)                            *
    //* data:   zu schreibendes Byte                                                              *
    //*********************************************************************************************
    //* Return: false bei Timeout (waitReady()) oder I2C-Fehler                                   *
    //*********************************************************************************************

    bool MbcI2cLcd::wbyte(uint8_t regdir, uint8_t data)
    {
        if (!waitReady()) return false;

        uint8_t temp_h = (uint8_t) ((data >> 4) * 16);
        uint8_t temp_l = (uint8_t) ((data & 0x0f) * 16);

        if (_backlightOn) { temp_h = (uint8_t) (temp_h + 0x08); temp_l = (uint8_t) (temp_l + 0x08); }

        temp_h &= 0xfe;   // P0 = 0 (Enable)
        temp_l &= 0xfe;

        if (regdir == MBC_I2CLCD_DR) { temp_h |= 0x04; temp_l |= 0x04; }    // P2 = 1 (Daten)
        else                          { temp_h &= 0xfb; temp_l &= 0xfb; }   // P2 = 0 (Instruktion)

        temp_h &= 0xfc;   // P1 = 0 (schreiben)
        temp_l &= 0xfc;

        uint8_t seq[5];
        seq[0] = (uint8_t) (temp_h | 0x01);
        seq[1] = (uint8_t) (temp_h & 0xfe);
        seq[2] = temp_l;
        seq[3] = (uint8_t) (temp_l | 0x01);
        seq[4] = (uint8_t) (temp_l & 0xfe);

        for (uint8_t i = 0; i < 5; i++)
        {
            if (!lowWrite(seq[i])) return false;
        }

        return true;
    }

    //*********************************************************************************************
    //* Ein Byte im 4-Bit-Nibble-Bang-Protokoll vom Display lesen                                 *
    //*********************************************************************************************
    //* regdir: MBC_I2CLCD_IR (Instruktion) oder MBC_I2CLCD_DR (Daten)                            *
    //* data:   Zielvariable                                                                      *
    //*********************************************************************************************
    //* Return: false bei Timeout (waitReady()) oder I2C-Fehler                                   *
    //*********************************************************************************************

    bool MbcI2cLcd::rbyte(uint8_t regdir, uint8_t &data)
    {
        if (!waitReady()) return false;

        uint8_t releaseBits = (_backend == MBC_I2CLCD_BACKEND_PCF8574) ? 0xf0 : 0x00;

        if (_backend == MBC_I2CLCD_BACKEND_MCP23017) _mcp.setDirA(0xf0);

        uint8_t ctrl = (uint8_t) (releaseBits | 0x02);
        if (_backlightOn) ctrl |= 0x08;

        if (regdir == MBC_I2CLCD_DR) ctrl |= 0x04;
        else                          ctrl &= 0xfb;

        uint8_t c0 = ctrl;
        uint8_t c1 = (uint8_t) (ctrl | 0x01);

        uint8_t nibble;

        lowWrite(c0);
        lowWrite(c1);
        lowRead(nibble);
        uint8_t temp_h = (uint8_t) (nibble & 0xf0);

        c0 &= 0xfe;
        c1 = (uint8_t) (c0 | 0x01);

        lowWrite(c0);
        lowWrite(c1);
        lowRead(nibble);
        uint8_t temp_l = (uint8_t) (nibble >> 4);

        c0 &= 0xfe;
        lowWrite(c0);

        if (_backend == MBC_I2CLCD_BACKEND_MCP23017) _mcp.setDirA(0x00);

        data = (uint8_t) (temp_h + temp_l);

        return true;
    }

    //*********************************************************************************************
    //* DDRAM-Zeilen-Offset ermitteln (deckt 2- und 4-zeilige HD44780-Displays ab)                *
    //*********************************************************************************************
    //* zeile: Zeile (1...4)                                                                      *
    //*********************************************************************************************
    //* Return: DDRAM-Offset dieser Zeile (0x00 bei ungueltiger Zeile)                            *
    //*********************************************************************************************

    uint8_t MbcI2cLcd::rowOffset(uint8_t zeile) const
    {
        if (zeile < 1 || zeile > 4) return 0x00;

        return ROW_OFFSET[zeile - 1];
    }

    //*********************************************************************************************
    //* Display initialisieren                                                                    *
    //*********************************************************************************************
    //* Return: false bei I2C-Fehler                                                              *
    //*********************************************************************************************

    bool MbcI2cLcd::begin()
    {
        if (_backend == MBC_I2CLCD_BACKEND_MCP23017)
        {
            // GPIOA = Ausgang (LCD-Steuerleitungen), GPIOB = Eingang mit Pull-Up (8-Tasten-Keypad,
            // aktiv-LOW - siehe readKey())

            if (!_mcp.begin(0x00, 0x08, 0xff, 0x00)) return false;

            _mcp.setPullUpB(0xff);
        }

        backlight(false);

        bool ok = true;

        ok &= wbyte(MBC_I2CLCD_IR, 0x33);
        ok &= wbyte(MBC_I2CLCD_IR, 0x32);
        ok &= wbyte(MBC_I2CLCD_IR, 0x28);   // 4-Bit-Modus, 2 Zeilen
        ok &= wbyte(MBC_I2CLCD_IR, 0x08);   // Display aus
        ok &= wbyte(MBC_I2CLCD_IR, 0x01);   // Display loeschen
        ok &= wbyte(MBC_I2CLCD_IR, 0x02);   // Cursor Home
        ok &= wbyte(MBC_I2CLCD_IR, 0x06);   // Cursor-Richtung +1

        _displaymode = 0x0f;          // Display an, Cursor an, Blinken an
        ok &= wbyte(MBC_I2CLCD_IR, _displaymode);

        backlight(true);

        return ok;
    }

    //*********************************************************************************************
    //* Cursor auf Zeile/Zeichenposition setzen (1-basiert)                                       *
    //*********************************************************************************************
    //* zeile: Zeile des Displays                                                                 *
    //* adr:   Zeichen 1...n der Zeile                                                            *
    //*********************************************************************************************
    //* Return: false bei ungueltiger Position oder Timeout/I2C-Fehler                            *
    //*********************************************************************************************

    bool MbcI2cLcd::gotoxy(uint8_t zeile, uint8_t adr)
    {
        if (adr > _zeichen || adr == 0 || zeile == 0 || zeile > _zeilen) return false;

        return wbyte(MBC_I2CLCD_IR, (uint8_t) (0x80 + rowOffset(zeile) + adr - 1));
    }

    //*********************************************************************************************
    //* Ein Zeichen an Position zeile/adr schreiben                                               *
    //*********************************************************************************************
    //* zeile: Zeile des Displays                                                                 *
    //* adr:   Zeichen 1...n der Zeile                                                            *
    //* data:  zu schreibendes Zeichen                                                            *
    //*********************************************************************************************
    //* Return: false bei ungueltiger Position oder Timeout/I2C-Fehler                            *
    //*********************************************************************************************

    bool MbcI2cLcd::writeChar(uint8_t zeile, uint8_t adr, char data)
    {
        if (!gotoxy(zeile, adr)) return false;

        // Letzte Adresse der Zeile: Cursor danach eine Position zurueckversetzen, da der
        // HD44780-Controller sonst bei der naechsten automatischen Adressinkrementierung
        // in einen internen Fehlerzustand laeuft.

        if (adr == _zeichen)
        {
            wbyte(MBC_I2CLCD_IR, 0x04);
            wbyte(MBC_I2CLCD_DR, (uint8_t) data);
            wbyte(MBC_I2CLCD_IR, 0x06);
        }
        else
        {
            wbyte(MBC_I2CLCD_DR, (uint8_t) data);
        }

        return true;
    }

    //*********************************************************************************************
    //* String ab Position zeile/adr schreiben (wird an Zeilenende abgeschnitten)                 *
    //*********************************************************************************************
    //* zeile: Zeile des Displays                                                                 *
    //* adr:   Zeichen 1...n der Zeile                                                            *
    //* data:  zu schreibender String                                                             *
    //*********************************************************************************************
    //* Return: false bei ungueltiger Position oder Timeout/I2C-Fehler                            *
    //*********************************************************************************************

    bool MbcI2cLcd::writeStr(uint8_t zeile, uint8_t adr, const char *data)
    {
        if (adr > _zeichen || adr == 0 || zeile == 0 || zeile > _zeilen) return false;

        uint8_t laenge = (uint8_t) strlen(data);

        if ((uint16_t) (adr + laenge - 1) > _zeichen) laenge = (uint8_t) (_zeichen - adr + 1);

        if (!wbyte(MBC_I2CLCD_IR, (uint8_t) (0x80 + rowOffset(zeile) + adr - 1))) return false;

        uint8_t base = adr;

        for (uint8_t i = 0; i < laenge; i++)
        {
            if (base == _zeichen)
            {
                wbyte(MBC_I2CLCD_IR, 0x04);
                wbyte(MBC_I2CLCD_DR, (uint8_t) data[i]);
                wbyte(MBC_I2CLCD_IR, 0x06);
            }
            else
            {
                wbyte(MBC_I2CLCD_DR, (uint8_t) data[i]);
            }

            base++;
        }

        return true;
    }

    //*********************************************************************************************
    //* String ab Position zeile/adr lesen (setzt RW-Verdrahtung des Displays voraus)             *
    //*********************************************************************************************
    //* zeile: Zeile des Displays                                                                 *
    //* adr:   Zeichen 1...n der Zeile                                                            *
    //* data:  Zielpuffer (mind. len+1 Byte)                                                      *
    //* len:   Anzahl der zu lesenden Zeichen                                                     *
    //*********************************************************************************************
    //* Return: false bei ungueltiger Position oder Timeout/I2C-Fehler                            *
    //*********************************************************************************************

    bool MbcI2cLcd::readStr(uint8_t zeile, uint8_t adr, char *data, uint8_t len)
    {
        if (adr > _zeichen || adr == 0 || zeile == 0 || zeile > _zeilen) return false;

        if ((uint16_t) (adr + len - 1) > _zeichen) len = (uint8_t) (_zeichen - adr + 1);

        if (!wbyte(MBC_I2CLCD_IR, (uint8_t) (0x80 + rowOffset(zeile) + adr - 1))) return false;

        uint8_t base = adr;

        for (uint8_t i = 0; i < len; i++)
        {
            uint8_t ch;

            if (base == _zeichen)
            {
                wbyte(MBC_I2CLCD_IR, 0x04);
                rbyte(MBC_I2CLCD_DR, ch);
                wbyte(MBC_I2CLCD_IR, 0x06);
            }
            else
            {
                rbyte(MBC_I2CLCD_DR, ch);
            }

            data[i] = (char) ch;
            base++;
        }

        data[len] = '\0';

        return true;
    }

    //*********************************************************************************************
    //* Beleuchtung ein-/ausschalten                                                              *
    //*********************************************************************************************
    //* on: true = ein, false = aus                                                               *
    //*********************************************************************************************
    //* Return: false bei I2C-Fehler                                                              *
    //*********************************************************************************************

    bool MbcI2cLcd::backlight(bool on)
    {
        _backlightOn = on;

        return lowWrite((uint8_t) (on ? 0x08 : 0x00));
    }

    //*********************************************************************************************
    //* Display-Kommando senden (Bitmasken MBC_I2CLCD_CURSOR_*/DISPLAY_*, kombinierbar)           *
    //*********************************************************************************************
    //* cmd: Kommando-Bitmaske                                                                    *
    //*********************************************************************************************
    //* Return: true                                                                              *
    //*********************************************************************************************

    bool MbcI2cLcd::command(uint8_t cmd)
    {
        uint8_t mode = _displaymode;
        bool    change = false;

        if (cmd & MBC_I2CLCD_CURSOR_ON)    { mode |= 0x02; change = true; }
        if (cmd & MBC_I2CLCD_CURSOR_OFF)   { mode &= 0x0d; change = true; }
        if (cmd & MBC_I2CLCD_CURSOR_BLINK) { mode |= 0x01; change = true; }
        if (cmd & MBC_I2CLCD_CURSOR_NORM)  { mode &= 0x0e; change = true; }
        if (cmd & MBC_I2CLCD_DISPLAY_OFF)  { mode &= 0xfb; change = true; }
        if (cmd & MBC_I2CLCD_DISPLAY_ON)   { mode |= 0x04; change = true; }

        if (cmd & MBC_I2CLCD_DISPLAY_CLR) wbyte(MBC_I2CLCD_IR, 0x01);
        if (cmd & MBC_I2CLCD_CURSOR_HOME) wbyte(MBC_I2CLCD_IR, 0x02);

        if (change)
        {
            wbyte(MBC_I2CLCD_IR, mode);
            _displaymode = mode;
        }

        return true;
    }

    //*********************************************************************************************
    //* Benutzerdefiniertes Zeichen in den Character-Generator-RAM schreiben                      *
    //*********************************************************************************************
    //* index:   0...7 im CGRAM                                                                   *
    //* pattern: 8 Byte Bitmuster (je 5 unterste Bit je Zeile)                                    *
    //*********************************************************************************************

    void MbcI2cLcd::generateChar(uint8_t index, const uint8_t pattern[8])
    {
        wbyte(MBC_I2CLCD_IR, (uint8_t) (0x40 | (index << 3)));

        for (uint8_t i = 0; i < 8; i++)
        {
            wbyte(MBC_I2CLCD_DR, pattern[i]);
        }

        wbyte(MBC_I2CLCD_IR, 0x80);   // Cursor zurueck auf Zeile 1, Spalte 1
    }

    //*********************************************************************************************
    //* Tastatur lesen (nur MCP23017-Backend, sonst immer MBC_I2CLCD_KEY_NONE)                    *
    //*********************************************************************************************
    //* Return: MBC_I2CLCD_KEY_1...8 oder MBC_I2CLCD_KEY_NONE                                     *
    //*********************************************************************************************

    uint8_t MbcI2cLcd::readKey()
    {
        if (_backend != MBC_I2CLCD_BACKEND_MCP23017) return MBC_I2CLCD_KEY_NONE;

        uint8_t vector = _mcp.readGpioB();

        static const uint8_t MASK[8] =
        {
            0xfe, 0xfd, 0xfb, 0xf7, 0xef, 0xdf, 0xbf, 0x7f
        };

        for (uint8_t key = 0; key < 8; key++)
        {
            if (vector == MASK[key])
            {
                if (_keyIntegrator[key] == 10)
                {
                    _keyIntegrator[key]++;
                    return (uint8_t) (key + 1);
                }

                if (_keyIntegrator[key] < 11) _keyIntegrator[key]++;

                return MBC_I2CLCD_KEY_NONE;
            }
        }

        for (uint8_t key = 0; key < 8; key++)
        {
            if (--_keyIntegrator[key] == 0) _keyIntegrator[key] = 1;
        }

        return MBC_I2CLCD_KEY_NONE;
    }

//------ Ende Klasse MbcI2cLcd --------------------------------------------------------------------

//###### Ende Quellcode MbcI2cLcd.cpp #############################################################
