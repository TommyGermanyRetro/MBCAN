//*************************************************************************************************
//* Systemarray, Konvertierung, ADC, internes EEPROM, Ereignisse/LED, 1-Wire (MBCANLib)           *
//*************************************************************************************************
//* Datei: Mbc89Utils.cpp                                                                         *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen:                                                                                  *
//* V1.0.0 - 02.08.2026 - Nullversion                                                             *
//* V1.1.0 - 06.08.2026 - eepromReadExtra()/eepromWriteExtra() fuer Unterklassen-Parameter        *
//*************************************************************************************************

//###### Start Quellcode Mbc89Utils.cpp ###########################################################

#include "Mbc89Utils.h"
#include "Mbc89Platform.h"
#include <EEPROM.h>

//------ Start interne Konstanten -----------------------------------------------------------------

    static const uint8_t  TIMER_CNT_100_MS    = 10;   // tick10ms()-Aufrufe je 100 ms
    static const uint16_t TIMER_TRAFFIC_START = 0;
    static const uint16_t TIMER_TRAFFIC_STOPP = 2;
    static const uint16_t TIMER_BLINKEN_MAX   = 5;     // 100-ms-Schritte je Blinkbild (=500 ms)
    static const uint16_t TIMER_EVENT_SEKUNDE = 10;    // 100-ms-Schritte je Sekunden-Event
    static const uint8_t  LED_STEPS           = 13;
    static const uint8_t  EVENT_INTERVAL      = 10;    // Anzahl der Sekunden-Events (mbc_hw_filled)

    static const uint8_t  LED_AUS    = 0x00;
    static const uint8_t  LED_GRUEN  = 0x01;
    static const uint8_t  LED_ROT    = 0x02;
    static const uint8_t  LED_ORANGE = 0x03;

    // 1-Wire (DS1820) Kommandos/Typen

    static const uint8_t OWI_SKIP_ROM      = 0xCC;
    static const uint8_t OWI_SEARCH_ROM    = 0xF0;
    static const uint8_t OWI_CONVERT_T     = 0x44;
    static const uint8_t OWI_READ          = 0xBE;
    static const uint8_t OWI_SEARCH_FIRST  = 0xFF;
    static const uint8_t OWI_PRESENCE_ERR  = 0xFF;
    static const uint8_t OWI_DATA_ERR      = 0xFE;
    static const uint8_t OWI_LAST_DEVICE   = 0x00;

    // Internes EEPROM: Byteadressen der vier Bloecke

    static const uint16_t EEA_NAME    = 0;
    static const uint16_t EEA_GUID    = EEA_NAME + MBC_L_NAMEBLOCK;
    static const uint16_t EEA_KENNUNG = EEA_GUID + MBC_L_GUID;
    static const uint16_t EEA_DB      = EEA_KENNUNG + MBC_L_KN;
    static const uint16_t EEA_ENDE    = EEA_DB + MBC_L_DB;

//------ Ende interne Konstanten ------------------------------------------------------------------

//------ Start LED-Blinkmustertabelle -------------------------------------------------------------

    const uint8_t Mbc89Utils::PATTERN[20][13] PROGMEM =
    {
        { LED_GRUEN,LED_GRUEN,LED_GRUEN,LED_GRUEN,LED_GRUEN,LED_GRUEN,LED_GRUEN,LED_GRUEN,LED_GRUEN,LED_GRUEN,LED_GRUEN,LED_GRUEN,LED_GRUEN },
        // GRUEN
        { LED_GRUEN,LED_GRUEN,LED_GRUEN,LED_GRUEN,LED_AUS,LED_AUS,LED_GRUEN,LED_AUS,LED_GRUEN,LED_GRUEN,LED_GRUEN,LED_GRUEN,LED_GRUEN },      // 1x -> MBC_LED_PC_OK
        { LED_GRUEN,LED_GRUEN,LED_GRUEN,LED_AUS,LED_AUS,LED_GRUEN,LED_AUS,LED_GRUEN,LED_AUS,LED_GRUEN,LED_GRUEN,LED_GRUEN,LED_GRUEN },        // 2x -> MBC_LED_CS2_OK
        { LED_GRUEN,LED_GRUEN,LED_AUS,LED_AUS,LED_GRUEN,LED_AUS,LED_GRUEN,LED_AUS,LED_GRUEN,LED_AUS,LED_GRUEN,LED_GRUEN,LED_GRUEN },          // 3x -> MBC_LED_IO_OK
        { LED_GRUEN,LED_AUS,LED_AUS,LED_GRUEN,LED_AUS,LED_GRUEN,LED_AUS,LED_GRUEN,LED_AUS,LED_GRUEN,LED_AUS,LED_GRUEN,LED_GRUEN },            // 4x -> MBC_LED_BT_OK
        { LED_AUS,LED_AUS,LED_GRUEN,LED_AUS,LED_GRUEN,LED_AUS,LED_GRUEN,LED_AUS,LED_GRUEN,LED_AUS,LED_GRUEN,LED_AUS,LED_GRUEN },              // 5x -> MBC_LED_UP_OK
        { LED_AUS,LED_GRUEN,LED_AUS,LED_GRUEN,LED_AUS,LED_GRUEN,LED_AUS,LED_GRUEN,LED_AUS,LED_GRUEN,LED_AUS,LED_GRUEN,LED_AUS },              // 6x
        // ROT
        { LED_GRUEN,LED_GRUEN,LED_GRUEN,LED_GRUEN,LED_GRUEN,LED_AUS,LED_ROT,LED_AUS,LED_GRUEN,LED_GRUEN,LED_GRUEN,LED_GRUEN,LED_GRUEN },      // 1x -> MBC_LED_MCP_ERROR
        { LED_GRUEN,LED_GRUEN,LED_GRUEN,LED_GRUEN,LED_AUS,LED_ROT,LED_AUS,LED_ROT,LED_AUS,LED_GRUEN,LED_GRUEN,LED_GRUEN,LED_GRUEN },          // 2x -> MBC_LED_SPG_ERROR
        { LED_GRUEN,LED_GRUEN,LED_GRUEN,LED_AUS,LED_ROT,LED_AUS,LED_ROT,LED_AUS,LED_ROT,LED_AUS,LED_GRUEN,LED_GRUEN,LED_GRUEN },              // 3x
        { LED_GRUEN,LED_GRUEN,LED_AUS,LED_ROT,LED_AUS,LED_ROT,LED_AUS,LED_ROT,LED_AUS,LED_ROT,LED_AUS,LED_GRUEN,LED_GRUEN },                  // 4x
        { LED_GRUEN,LED_AUS,LED_ROT,LED_AUS,LED_ROT,LED_AUS,LED_ROT,LED_AUS,LED_ROT,LED_AUS,LED_ROT,LED_AUS,LED_GRUEN },                      // 5x -> MBC_LED_FW_ERROR
        { LED_AUS,LED_ROT,LED_AUS,LED_ROT,LED_AUS,LED_ROT,LED_AUS,LED_ROT,LED_AUS,LED_ROT,LED_AUS,LED_ROT,LED_AUS },                          // 6x -> MBC_LED_BOOT_ERROR
        // ORANGE
        { LED_GRUEN,LED_GRUEN,LED_GRUEN,LED_GRUEN,LED_GRUEN,LED_AUS,LED_ORANGE,LED_AUS,LED_GRUEN,LED_GRUEN,LED_GRUEN,LED_GRUEN,LED_GRUEN },   // 1x -> MBC_LED_NORMAL_START
        { LED_GRUEN,LED_GRUEN,LED_GRUEN,LED_GRUEN,LED_AUS,LED_ORANGE,LED_AUS,LED_ORANGE,LED_AUS,LED_GRUEN,LED_GRUEN,LED_GRUEN,LED_GRUEN },    // 2x -> MBC_LED_FW_UPGRADE_P
        { LED_GRUEN,LED_GRUEN,LED_GRUEN,LED_AUS,LED_ORANGE,LED_AUS,LED_ORANGE,LED_AUS,LED_ORANGE,LED_AUS,LED_GRUEN,LED_GRUEN,LED_GRUEN },     // 3x -> MBC_LED_FW_UPGRADE_S
        { LED_GRUEN,LED_GRUEN,LED_AUS,LED_ORANGE,LED_AUS,LED_ORANGE,LED_AUS,LED_ORANGE,LED_AUS,LED_ORANGE,LED_AUS,LED_GRUEN,LED_GRUEN },      // 4x
        { LED_GRUEN,LED_AUS,LED_ORANGE,LED_AUS,LED_ORANGE,LED_AUS,LED_ORANGE,LED_AUS,LED_ORANGE,LED_AUS,LED_ORANGE,LED_AUS,LED_GRUEN },       // 5x
        { LED_AUS,LED_ORANGE,LED_AUS,LED_ORANGE,LED_AUS,LED_ORANGE,LED_AUS,LED_ORANGE,LED_AUS,LED_ORANGE,LED_AUS,LED_ORANGE,LED_AUS }         // 6x -> MBC_LED_MODKONFIG
        // Zeile 20 (Index 19) bleibt ungenutzt/nullinitialisiert (LED_AUS)
    };

//------ Ende LED-Blinkmustertabelle --------------------------------------------------------------

//------ Start Klasse Mbc89Utils ------------------------------------------------------------------

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //+ KONSTRUKTOREN                                                                             +
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

        //*****************************************************************************************
        //* Konstruktor zur Deklaration von Objektvariablen                                       *
        //*****************************************************************************************

        Mbc89Utils::Mbc89Utils(uint8_t *systemArray, uint16_t genPurpSize, uint8_t pinAdcVspg,
                            uint8_t pinAdcPlug, uint8_t pinLedGruen, uint8_t pinLedRot,
                            uint8_t pinOneWire, uint16_t extraEepromSize)
            : _extraEepromSize(extraEepromSize),
              _pinAdcVspg(pinAdcVspg), _pinAdcPlug(pinAdcPlug),
              _pinLedGruen(pinLedGruen), _pinLedRot(pinLedRot),
              _pinOneWire(pinOneWire),
              _ledDriverGruen(nullptr), _ledDriverRot(nullptr), _plugDetector(nullptr),
              _lastTickMs(0), _eventCnt10ms(0),
              _trafficCnt(0), _blinktaktCnt(0), _blinkbildCnt(0), _eventCnt100ms(0),
              _fReady(false), _fSekunde(false), _f500ms(false), _f100ms(false), _eventNummer(0),
              _ledTraffic(false), _ledBlinken(false), _ledBlinkenContinuous(false),
              _ledBlinkenNeustart(false), _ledBlinkenMuster(0),
              _spgHurdle(false),
              _owiPresent(false), _owiType(MBC_OWI_DS18S20), _owiValue(0.0f)
        {
            _array    = systemArray;
            _arrayMax = MBC_S_GENPURP + genPurpSize;

            #if defined(__AVR__)
                _owiDdr    = portModeRegister(digitalPinToPort(pinOneWire));
                _owiPort   = portOutputRegister(digitalPinToPort(pinOneWire));
                _owiPinReg = portInputRegister(digitalPinToPort(pinOneWire));
                _owiMask   = digitalPinToBitMask(pinOneWire);
            #endif
        }

        //*****************************************************************************************
        //* Destruktor - das Systemarray gehoert dem Sketch (siehe Konstruktor), hier nichts      *
        //* freizugeben                                                                           *
        //*****************************************************************************************

        Mbc89Utils::~Mbc89Utils()
        {
        }

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //+ SYSTEMARRAY / MODULIDENTITAET                                                             +
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

        //*****************************************************************************************
        //* Initialisiert das Systemarray: loescht es, setzt die GUiD auf 0xff (unangemeldet),    *
        //* laedt Name/GUiD/Kennung/Datenbank aus dem internen EEPROM (mit Defaultwerten fuer den *
        //* unbeschriebenen Zustand), initialisiert Ereignis-/LED- und ADC-Subsystem, sucht den   *
        //* 1-Wire-Sensor und traegt Artikel-/Software-/Hardwareversion ein.                      *
        //*****************************************************************************************
        //* artikelnummer: 4 Byte Artikelnummer                                                   *
        //* swVersion:     3 Byte Softwareversion                                                 *
        //* hwVersion:     6 Byte Hardwareversion                                                 *
        //*****************************************************************************************

        void Mbc89Utils::begin(const uint8_t artikelnummer[4], const uint8_t swVersion[3],
                              const uint8_t hwVersion[6])
        {
            // Systemarray loeschen

            for (uint16_t index = 0; index < _arrayMax; index++)
            {
                _array[index] = 0;
            }

            // GUiD vorbereiten (unangemeldet)

            _array[MBC_S_GUID]     = 0xff;
            _array[MBC_S_GUID + 1] = 0xff;
            _array[MBC_S_GUID + 2] = 0xff;
            _array[MBC_S_GUID + 3] = 0xff;

            // Internes EEPROM einlesen (Name/GUiD/Kennung/Datenbank) - eepromBegin() deckt
            // zusaetzlich die Zusatzregion einer Unterklasse ab (siehe
            // eepromReadExtra()/eepromWriteExtra()), damit dafuer kein zweiter, potenziell den
            // emulierten EEPROM-Bereich neu initialisierender eepromBegin()-Aufruf noetig wird

            Mbc89Platform::eepromBegin(EEA_ENDE + _extraEepromSize);

            eepromRead();

            // Ereignis-/LED-Subsystem initialisieren - GRUEN sofort beim Start, statt bis zum Ende
            // von waitFilled() dunkel zu bleiben. Ueber setLedDrivers() umgeleitete Ausgaenge (vom
            // Sketch bereits vor begin() konfiguriert) statt der nativen Pins, falls gesetzt.

            if (_ledDriverGruen != nullptr) _ledDriverGruen->write(true);
            else { pinMode(_pinLedGruen, OUTPUT); digitalWrite(_pinLedGruen, HIGH); }

            if (_ledDriverRot != nullptr) _ledDriverRot->write(false);
            else { pinMode(_pinLedRot, OUTPUT); digitalWrite(_pinLedRot, LOW); }

            // Internen ADC vorbereiten (siehe Mbc89Platform::adcConfigure())

            Mbc89Platform::adcConfigure();

            analogRead(_pinAdcVspg);   // erste Wandlung verwerfen (Kalibrierung des ADC)

            // 1-Wire-Sensor suchen und Seriennummer auslesen

            owiInit();

            // Artikel-/Software-/Hardwareversion eintragen

            _array[MBC_S_ART]     = artikelnummer[0];
            _array[MBC_S_ART + 1] = artikelnummer[1];
            _array[MBC_S_ART + 2] = artikelnummer[2];
            _array[MBC_S_ART + 3] = artikelnummer[3];

            _array[MBC_S_SW]     = swVersion[0];
            _array[MBC_S_SW + 1] = swVersion[1];
            _array[MBC_S_SW + 2] = swVersion[2];

            _array[MBC_S_HW]     = hwVersion[0];
            _array[MBC_S_HW + 1] = hwVersion[1];
            _array[MBC_S_HW + 2] = hwVersion[2];
            _array[MBC_S_HW + 3] = hwVersion[3];
            _array[MBC_S_HW + 4] = hwVersion[4];
            _array[MBC_S_HW + 5] = hwVersion[5];

            _lastTickMs = millis();

            Mbc89Platform::watchdogBegin();
        }

        //*****************************************************************************************
        //* Muss regelmaessig aufgerufen werden (z.B. bei jedem loop()-Durchlauf) und tickt       *
        //* intern im 10-ms-Raster nach (Traffic-/Blinkanzeige, Sekunden-/                        *
        //* 500-ms-/100-ms-Ereigniszaehler). Fuettert dabei bei jedem Aufruf den Watchdog-Timer   *
        //* (siehe Mbc89Platform::watchdogBegin()/watchdogReset()).                               *
        //*****************************************************************************************

        void Mbc89Utils::update()
        {
            Mbc89Platform::watchdogReset();

            unsigned long now = millis();

            while ((unsigned long) (now - _lastTickMs) >= 10)
            {
                _lastTickMs += 10;
                tick10ms();
            }
        }

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //+ ZYKLISCHER SERVICE                                                                        +
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

        //*****************************************************************************************
        //* Zyklischer Service: wertet die Sekunden-Ereignisnummer aus und stoesst                *
        //* Spannungs-/Temperaturmessungen an. In loop() nach update() aufrufen.                  *
        //*****************************************************************************************

        void Mbc89Utils::check()
        {
            if (!_fReady) return;

            // Start Vspg-Wandlung/Lese Vspg und Starte Plug-Wandlung/Lese Plug lassen sich
            // je in einem Schritt erledigen, da analogRead() auf Arduino synchron arbeitet
            // und kein separates Anstossen+spaeteres Abholen der Wandlung braucht.

            switch (_eventNummer)
            {
                case 1:

                    owiStartTempMeasurement();
                    adcReadVspg();

                    _fReady = false;

                break;

                case 2:

                    adcReadPlugged();

                    _fReady = false;

                break;

                case 9:

                    owiReadTemperature();

                    _fReady = false;

                break;

                default:

                    _fReady = false;
            }
        }

        //*****************************************************************************************
        //* Blockiert, bis alle Ereignisse einmal durchlaufen wurden (10 s) - z.B. um nach dem    *
        //* Start sicherzustellen, dass alle Statuswerte einmal ermittelt wurden. Tickt           *
        //* update()/check() waehrenddessen selbst weiter.                                        *
        //*****************************************************************************************

        void Mbc89Utils::waitFilled()
        {
            // Aktiviere kontinuierliches Blinken der ORANGE-LED

            ledToggleOn(MBC_LED_MODKONFIG);

            // Eventnummer zuruecksetzen um Abarbeitung zu synchronisieren

            _eventNummer = 0;

            // Warten bis alle Events einmal abgearbeitet wurden

            while (eventNummer() < EVENT_INTERVAL)
            {
                update();
                check();
            }

            ledToggleOff();
        }

        //*****************************************************************************************
        //* Direkter Zugriff auf das Systemarray                                                  *
        //*****************************************************************************************
        //* Return: Zeiger auf das Systemarray                                                    *
        //*****************************************************************************************

        uint8_t* Mbc89Utils::array()
        {
            return _array;
        }

        //*****************************************************************************************
        //* Groesse des Systemarrays in Byte                                                      *
        //*****************************************************************************************
        //* Return: Groesse des Systemarrays in Byte                                              *
        //*****************************************************************************************

        uint16_t Mbc89Utils::arrayMax() const
        {
            return _arrayMax;
        }

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //+ KONVERTIERUNG                                                                             +
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

        //*****************************************************************************************
        //* Wandle Hex-ASCII-Ziffernfolge in eine Dezimalnummer um                                *
        //*****************************************************************************************
        //* ascii: Buffer mit ASCII-Hex-Ziffern                                                   *
        //* num:   Anzahl der auszuwertenden Ziffern                                              *
        //*****************************************************************************************
        //* Return: Umgewandelter Wert                                                            *
        //*****************************************************************************************

        uint16_t Mbc89Utils::hex2num(const uint8_t *ascii, uint8_t num)
        {
            uint16_t val = 0;

            for (uint8_t i = 0; i < num; i++)
            {
                uint8_t c = ascii[i];

                if      (c >= '0' && c <= '9') c -= '0';
                else if (c >= 'A' && c <= 'F') c -= 'A' - 10;
                else if (c >= 'a' && c <= 'f') c -= 'a' - 10;

                val = 16 * val + c;
            }

            return val;
        }

        //*****************************************************************************************
        //* Wandle eine Zahl (0...999) in einen rechtsbuendigen 3-stelligen String um             *
        //*****************************************************************************************
        //* num: umzuwandelnder Wert                                                              *
        //*****************************************************************************************
        //* Return: Zeiger auf internen statischen Buffer (nicht rekursions-/threadsicher)        *
        //*****************************************************************************************

        char* Mbc89Utils::num2string(uint16_t num)
        {
            static char ascii[4];
            uint16_t stelle;

            if (num < 1000)
            {
                ascii[3] = 0;

                if (num < 10)
                {
                    ascii[0] = 0x20;
                    ascii[1] = 0x20;
                    ascii[2] = 0x30 + (uint8_t) num;
                }
                else if (num < 100)
                {
                    ascii[0] = 0x20;
                    stelle = num / 10;
                    ascii[1] = 0x30 + (uint8_t) stelle;
                    stelle *= 10;
                    stelle = num - stelle;
                    ascii[2] = 0x30 + (uint8_t) stelle;
                }
                else
                {
                    stelle = num / 100;
                    ascii[0] = 0x30 + (uint8_t) stelle;
                    stelle *= 100;
                    num = num - stelle;
                    stelle = num / 10;
                    ascii[1] = 0x30 + (uint8_t) stelle;
                    stelle *= 10;
                    stelle = num - stelle;
                    ascii[2] = 0x30 + (uint8_t) stelle;
                }
            }
            else
            {
                ascii[0] = 0;
            }

            return ascii;
        }

        //*****************************************************************************************
        //* Wandle eine Zahl in einen HEX-String mit fester Stellenzahl um                        *
        //*****************************************************************************************
        //* num:     umzuwandelnder Wert                                                          *
        //* stellen: Anzahl der HEX-Nibble (2, 3 oder 4)                                          *
        //*****************************************************************************************
        //* Return: Zeiger auf internen statischen Buffer (nicht rekursions-/threadsicher)        *
        //*****************************************************************************************

        char* Mbc89Utils::num2hexstring(uint16_t num, uint8_t stellen)
        {
            static char ascii[4];
            uint8_t i;

            if (stellen == 2)
            {
                i = (uint8_t) (num & 0x000f);
                ascii[1] = hex2ascii(i);

                i = (uint8_t) ((num & 0x00f0) >> 4);
                ascii[0] = hex2ascii(i);
            }
            else if (stellen == 3)
            {
                i = (uint8_t) (num & 0x000f);
                ascii[2] = hex2ascii(i);

                i = (uint8_t) ((num & 0x00f0) >> 4);
                ascii[1] = hex2ascii(i);

                i = (uint8_t) ((num & 0x0f00) >> 8);
                ascii[0] = hex2ascii(i);
            }
            else if (stellen == 4)
            {
                i = (uint8_t) (num & 0x000f);
                ascii[3] = hex2ascii(i);

                i = (uint8_t) ((num & 0x00f0) >> 4);
                ascii[2] = hex2ascii(i);

                i = (uint8_t) ((num & 0x0f00) >> 8);
                ascii[1] = hex2ascii(i);

                i = (uint8_t) ((num & 0xf000) >> 12);
                ascii[0] = hex2ascii(i);
            }

            return ascii;
        }

        //*****************************************************************************************
        //* Wandle 4-Bit-HEX-Wert in ASCII um                                                     *
        //*****************************************************************************************
        //* digit: 4-Bit-Wert auf HEX-Basis 0x0...0xf                                             *
        //*****************************************************************************************
        //* Return: ASCII-Zeichen (0...9 und A...F)                                               *
        //*****************************************************************************************

        uint8_t Mbc89Utils::hex2ascii(uint8_t digit)
        {
            if (digit < 10) return (0x30 + digit);
            else             return (0x37 + digit);
        }

        //*****************************************************************************************
        //* Wandle ASCII in 4-Bit-HEX-Wert um                                                     *
        //*****************************************************************************************
        //* digit: ASCII-Zeichen (0...9 und A...F)                                                *
        //*****************************************************************************************
        //* Return: Wert auf HEX-Basis 0x0...0xf                                                  *
        //*****************************************************************************************

        uint8_t Mbc89Utils::ascii2hex(uint8_t digit)
        {
            if (digit < 0x3a) return (digit - 0x30);
            else               return (digit - 0x37);
        }

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //+ INTERNES EEPROM (Modulidentitaet)                                                         +
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

        //*****************************************************************************************
        //* Liest Name/GUiD/Kennung/Datenbank aus dem internen EEPROM in das Systemarray (mit     *
        //* Defaultwerten fuer den unbeschriebenen Zustand)                                       *
        //*****************************************************************************************

        void Mbc89Utils::eepromRead()
        {
            // Modulname

            for (uint16_t index = 0; index < MBC_L_NAMEBLOCK; index++)
            {
                _array[MBC_S_NAMEBLOCK + index] = EEPROM.read(EEA_NAME + index);
            }

            // GUiD

            for (uint16_t index = 0; index < MBC_L_GUID; index++)
            {
                _array[MBC_S_GUID + index] = EEPROM.read(EEA_GUID + index);
            }

            // Geraetekennung - unbeschrieben (0xff) -> Default eintragen

            for (uint16_t index = 0; index < MBC_L_KN; index++)
            {
                _array[MBC_S_KN + index] = EEPROM.read(EEA_KENNUNG + index);
            }

            if (_array[MBC_S_KN]     == 0xff) _array[MBC_S_KN]     = 0xaa;
            if (_array[MBC_S_KN + 1] == 0xff) _array[MBC_S_KN + 1] = 0xaa;
            if (_array[MBC_S_KN + 4] == 0xff) _array[MBC_S_KN + 4] = 0x6d;

            // Datenbank - unbeschrieben (0xff) -> "0" eintragen

            for (uint16_t index = 0; index < MBC_L_DB; index++)
            {
                uint8_t val = EEPROM.read(EEA_DB + index);
                if (val == 0xff) val = 0x30;
                _array[MBC_S_DB + index] = val;
            }
        }

        //*****************************************************************************************
        //* Schreibt einen Block des Systemarrays in das interne EEPROM                           *
        //*****************************************************************************************
        //* block: MBC_EE_NAME / MBC_EE_GUID / MBC_EE_KENNUNG / MBC_EE_DB                         *
        //*****************************************************************************************

        void Mbc89Utils::eepromWrite(uint8_t block)
        {
            switch (block)
            {
                case MBC_EE_NAME:

                    for (uint16_t index = 0; index < MBC_L_NAMEBLOCK; index++)
                    {
                        EEPROM.write(EEA_NAME + index, _array[MBC_S_NAMEBLOCK + index]);
                    }

                break;

                case MBC_EE_GUID:

                    for (uint16_t index = 0; index < MBC_L_GUID; index++)
                    {
                        EEPROM.write(EEA_GUID + index, _array[MBC_S_GUID + index]);
                    }

                break;

                case MBC_EE_KENNUNG:

                    for (uint16_t index = 0; index < MBC_L_KN; index++)
                    {
                        EEPROM.write(EEA_KENNUNG + index, _array[MBC_S_KN + index]);
                    }

                break;

                case MBC_EE_DB:

                    for (uint16_t index = 0; index < MBC_L_DB; index++)
                    {
                        EEPROM.write(EEA_DB + index, _array[MBC_S_DB + index]);
                    }

                break;
            }

            Mbc89Platform::eepromCommit();
        }

        //*****************************************************************************************
        //* Liest Bytes aus der Zusatzregion des internen EEPROM (siehe extraEepromSize im        *
        //* Konstruktor)                                                                          *
        //*****************************************************************************************
        //* offset: Byte-Offset innerhalb der Zusatzregion (0...extraEepromSize-1)                *
        //* buffer: Zielpuffer (mind. len Byte)                                                   *
        //* len:    Anzahl zu lesender Byte                                                       *
        //*****************************************************************************************

        bool Mbc89Utils::eepromReadExtra(uint16_t offset, uint8_t *buffer, uint16_t len)
        {
            if ((uint32_t) offset + len > _extraEepromSize) return false;

            for (uint16_t index = 0; index < len; index++)
            {
                buffer[index] = EEPROM.read(EEA_ENDE + offset + index);
            }

            return true;
        }

        //*****************************************************************************************
        //* Schreibt Bytes in die Zusatzregion des internen EEPROM (siehe eepromReadExtra())      *
        //*****************************************************************************************
        //* offset: Byte-Offset innerhalb der Zusatzregion (0...extraEepromSize-1)                *
        //* buffer: Quellpuffer (mind. len Byte)                                                  *
        //* len:    Anzahl zu schreibender Byte                                                   *
        //*****************************************************************************************

        bool Mbc89Utils::eepromWriteExtra(uint16_t offset, const uint8_t *buffer, uint16_t len)
        {
            if ((uint32_t) offset + len > _extraEepromSize) return false;

            for (uint16_t index = 0; index < len; index++)
            {
                EEPROM.write(EEA_ENDE + offset + index, buffer[index]);
            }

            Mbc89Platform::eepromCommit();

            return true;
        }

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //+ ADC                                                                                       +
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

        //*****************************************************************************************
        //* Ermittelt, ob ein Steckernetzteil gesteckt ist, und traegt das Ergebnis (0/1) im      *
        //* Systemarray ein - immer ueber den nativen Analogpin (siehe Klassenkommentar).         *
        //*****************************************************************************************

        void Mbc89Utils::adcReadPlugged()
        {
            if (_plugDetector != nullptr)
            {
                _array[MBC_S_PARA] = _plugDetector->read() ? 1 : 0;
                return;
            }

            uint16_t adc = analogRead(_pinAdcPlug);

            // Spannungswert < 18 V ? Dann ist kein Steckernetzteil gesteckt.

            _array[MBC_S_PARA] = (adc < 340) ? 0 : 1;
        }

        //*****************************************************************************************
        //* Liest den Versorgungsspannungs-Analogpin aus, formatiert das Ergebnis als String ins  *
        //* Systemarray und steuert bei Unterspannung die Status-LED an                           *
        //*****************************************************************************************

        void Mbc89Utils::adcReadVspg()
        {
            uint16_t adc = analogRead(_pinAdcVspg);

            double dadc = ((double) adc) / 1024.0 * Mbc89Platform::adcRefVolts() * 10.2 / 1.1;

            char sadc[5];
            dtostrf(dadc, 4, 1, sadc);

            _array[MBC_S_PARA + 1] = sadc[0];
            _array[MBC_S_PARA + 2] = sadc[1];
            _array[MBC_S_PARA + 3] = sadc[3];

            // Spannungswert < 11 V ? Dann rote LED (2x Blinken kontinuierlich).

            if (adc < 243)
            {
                if (!_spgHurdle)
                {
                    ledToggleOn(MBC_LED_SPG_ERROR);
                    _spgHurdle = true;
                }
            }
            else
            {
                if (_spgHurdle)
                {
                    ledToggleOff();
                    _spgHurdle = false;
                }
            }
        }

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //+ EREIGNISSE / STATUS-LED                                                                   +
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

        //*****************************************************************************************
        //* Steuert die gruene/rote Status-LED direkt an - immer ueber die nativen Pins (siehe    *
        //* Klassenkommentar).                                                                    *
        //*****************************************************************************************
        //* colorBits: Bit0 = gruen, Bit1 = rot (0x00 = aus, 0x03 = orange)                       *
        //*****************************************************************************************

        void Mbc89Utils::setLed(uint8_t colorBits)
        {
            bool gruen = (colorBits & LED_GRUEN) != 0;
            bool rot   = (colorBits & LED_ROT)   != 0;

            if (_ledDriverGruen != nullptr) _ledDriverGruen->write(gruen);
            else                             digitalWrite(_pinLedGruen, gruen ? HIGH : LOW);

            if (_ledDriverRot != nullptr) _ledDriverRot->write(rot);
            else                           digitalWrite(_pinLedRot, rot ? HIGH : LOW);
        }

        //*****************************************************************************************
        //* Status-LED-Ausgaenge optional auf einen Mbc89DigitalOut umleiten (siehe Klassenkommentar*
        //* in Mbc89Utils.h)                                                                      *
        //*****************************************************************************************
        //* gruen/rot: Zielausgang oder nullptr fuer den nativen Pin                              *
        //*****************************************************************************************

        void Mbc89Utils::setLedDrivers(Mbc89DigitalOut *gruen, Mbc89DigitalOut *rot)
        {
            _ledDriverGruen = gruen;
            _ledDriverRot   = rot;
        }

        //*****************************************************************************************
        //* Steckernetzteil-Erkennung optional auf einen Mbc89DigitalIn umleiten (siehe           *
        //* Klassenkommentar in Mbc89Utils.h)                                                     *
        //*****************************************************************************************
        //* detector: Zielquelle oder nullptr fuer die native analoge Auswertung                  *
        //*****************************************************************************************

        void Mbc89Utils::setPlugDetector(Mbc89DigitalIn *detector)
        {
            _plugDetector = detector;
        }

        //*****************************************************************************************
        //* Wird alle 10 ms von update() aufgerufen - Zeitbasis fuer Traffic-/ Blinkanzeige sowie *
        //* die 100-ms-/500-ms-/Sekunden-Ereigniszaehler                                          *
        //*****************************************************************************************

        void Mbc89Utils::tick10ms()
        {
            _eventCnt10ms++;

            if (_eventCnt10ms != TIMER_CNT_100_MS) return;

            _eventCnt10ms = 0;

            // Flag fuer 100-ms-Zyklus setzen

            _f100ms = true;

            // Wenn neues Blinken angefordert, Index-Zaehler zuruecksetzen

            if (_ledBlinkenNeustart)
            {
                _ledBlinkenNeustart = false;
                _blinkbildCnt = 0;
            }

            if (_ledTraffic)  _trafficCnt++;
            if (_ledBlinken)  _blinktaktCnt++;

            _eventCnt100ms++;

            //*********** Traffic-LED **********                                                  *

            if (_trafficCnt > TIMER_TRAFFIC_START)
            {
                if (!_ledBlinken)
                {
                    if (_trafficCnt == TIMER_TRAFFIC_STOPP)
                    {
                        setLed(LED_GRUEN);

                        _ledTraffic = false;
                        _trafficCnt = 0;
                    }
                    else
                    {
                        setLed(LED_ORANGE);
                    }
                }
                else
                {
                    _ledTraffic = false;
                    _trafficCnt = 0;
                }
            }

            //*********** LED-Blinken **********                                                  *

            if (_blinktaktCnt == TIMER_BLINKEN_MAX)
            {
                _f500ms = true;

                _blinktaktCnt = 0;

                if (_blinkbildCnt == (LED_STEPS - (_ledBlinkenContinuous ? 1 : 0)))
                {
                    _blinkbildCnt = 0;

                    if (!_ledBlinkenContinuous)
                    {
                        _ledBlinken = false;

                        setLed(LED_GRUEN);
                    }
                    else
                    {
                        setLed(pgm_read_byte(&PATTERN[_ledBlinkenMuster][_blinkbildCnt]));

                        _blinkbildCnt++;
                    }
                }
                else
                {
                    if (_ledBlinken)
                    {
                        setLed(pgm_read_byte(&PATTERN[_ledBlinkenMuster][_blinkbildCnt]));

                        _blinkbildCnt++;
                    }
                }
            }

            //*********** Event-Zaehler **********                                                *

            if (_eventCnt100ms == TIMER_EVENT_SEKUNDE)
            {
                _eventCnt100ms = 0;

                if (_eventNummer < EVENT_INTERVAL)
                {
                    _eventNummer++;
                    _fReady = true;
                }
                else
                {
                    _eventNummer = 0;
                }

                _fSekunde = true;
            }
        }

        //*****************************************************************************************
        //* LED-Blinken einschalten                                                               *
        //*****************************************************************************************
        //* muster: Zeile in der LED-Blinkmustertabelle (siehe MBC_LED_*-Konstanten)              *
        //* block:  0 = einmalig, 1 = kontinuierlich                                              *
        //*****************************************************************************************

        void Mbc89Utils::ledToggleOn(uint8_t muster, uint8_t block)
        {
            _ledBlinkenMuster    = muster;
            _ledBlinkenNeustart  = true;
            _ledBlinken          = true;
            _ledBlinkenContinuous = (block != 0);
        }

        //*****************************************************************************************
        //* LED-Blinken ausschalten, LED zeigt danach GRUEN                                       *
        //*****************************************************************************************

        void Mbc89Utils::ledToggleOff()
        {
            _ledBlinken = false;
            _ledBlinkenContinuous = false;

            setLed(LED_GRUEN);
        }

        //*****************************************************************************************
        //* Traffic-Anzeige anfordern (kurzes ORANGE-Aufblitzen, wenn kein Blinkmuster aktiv ist) *
        //*****************************************************************************************

        void Mbc89Utils::ledTraffic()
        {
            _ledTraffic = true;
        }

        //*****************************************************************************************
        //* Sekunden-Ereignis abfragen (liest und loescht das Flag)                               *
        //*****************************************************************************************
        //* Return: true, wenn ein Ereignis vorlag                                                *
        //*****************************************************************************************

        bool Mbc89Utils::eventSekunde()
        {
            if (_fSekunde) { _fSekunde = false; return true; }
            return false;
        }

        //*****************************************************************************************
        //* 500-ms-Ereignis abfragen (liest und loescht das Flag)                                 *
        //*****************************************************************************************
        //* Return: true, wenn ein Ereignis vorlag                                                *
        //*****************************************************************************************

        bool Mbc89Utils::event500ms()
        {
            if (_f500ms) { _f500ms = false; return true; }
            return false;
        }

        //*****************************************************************************************
        //* 100-ms-Ereignis abfragen (liest und loescht das Flag)                                 *
        //*****************************************************************************************
        //* Return: true, wenn ein Ereignis vorlag                                                *
        //*****************************************************************************************

        bool Mbc89Utils::event100ms()
        {
            if (_f100ms) { _f100ms = false; return true; }
            return false;
        }

        //*****************************************************************************************
        //* Aktuelle Ereignisnummer (0...9) abfragen, wird jede Sekunde hochgezaehlt              *
        //*****************************************************************************************
        //* Return: Ereignisnummer 0...9                                                          *
        //*****************************************************************************************

        uint8_t Mbc89Utils::eventNummer()
        {
            return _eventNummer;
        }

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //+ 1-WIRE-TEMPERATURSENSOR                                                                   +
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

        //*****************************************************************************************
        //* Ist ein 1-Wire-Sensor gefunden worden? (Ergebnis von begin())                         *
        //*****************************************************************************************
        //* Return: true, wenn beim begin() ein 1-Wire-Sensor gefunden wurde                      *
        //*****************************************************************************************

        bool Mbc89Utils::owiPresent() const
        {
            return _owiPresent;
        }

        //*****************************************************************************************
        //* Zuletzt gemessene Temperatur in GradC (nur gueltig, wenn owiPresent())                *
        //*****************************************************************************************
        //* Return: Temperatur in GradC                                                           *
        //*****************************************************************************************

        float Mbc89Utils::owiTemperature() const
        {
            return _owiValue;
        }

        //*****************************************************************************************
        //* Sendet den 1-Wire-Reset-Impuls und wertet den Praesenzpuls aus. Auf AVR ueber rohe    *
        //* Portregister statt digitalWrite()/digitalRead()/pinMode() (siehe _owiDdr/_owiPort/    *
        //* _owiPinReg in Mbc89Utils.h), auf ESP32 ueber ein permanent OUTPUT_OPEN_DRAIN|PULLUP   *
        //* gesetztes Pin (siehe owiInit()) ohne pinMode() im Ablauf - in beiden Faellen, weil    *
        //* deren Aufrufoverhead die engen 1-Wire-Zeitfenster (wenige Mikrosekunden) sprengen und *
        //* den Sensor desynchronisieren wuerde. AVR-Variante hardwarebestaetigt (ATmega644P),    *
        //* ESP32-Variante analog zum baugleichen Fix in MbcUtils uebernommen, fuer mbc-89 noch   *
        //* nicht hardwaregetestet.                                                               *
        //*****************************************************************************************
        //* Return: true, wenn ein Praesenzpuls erkannt wurde (kein Fehler)                       *
        //*****************************************************************************************

        #if defined(__AVR__)

            bool Mbc89Utils::owiReset()
            {
                *_owiPort &= (uint8_t) ~_owiMask;
                *_owiDdr  |= _owiMask;

                delayMicroseconds(480);

                *_owiPort |= _owiMask;
                *_owiDdr  &= (uint8_t) ~_owiMask;

                delayMicroseconds(66);

                bool present = ((*_owiPinReg & _owiMask) == 0);

                delayMicroseconds(480 - 66);

                if ((*_owiPinReg & _owiMask) == 0) present = false;   // Kurzschluss

                return present;
            }

        #elif defined(ESP32)

            // Pin steht permanent auf OUTPUT_OPEN_DRAIN|PULLUP (siehe owiInit()) - im
            // zeitkritischen Pfad NIE pinMode() aufrufen (GPIO-Matrix-Rekonfiguration kostet auf
            // ESP32-C3 mehrere Mikrosekunden und sprengt die 1-Wire-Zeitfenster, exakt wie beim
            // urspruenglich AVR-seitig hardwarebestaetigten Bug oben, siehe Klassenkommentar).
            // digitalWrite(HIGH) gibt im Open-Drain-Modus lediglich den Bus frei (Transistor aus,
            // PULLUP zieht hoch) - entspricht dem AVR-INPUT_PULLUP-Aequivalent.

            bool Mbc89Utils::owiReset()
            {
                digitalWrite(_pinOneWire, LOW);

                delayMicroseconds(480);

                digitalWrite(_pinOneWire, HIGH);

                delayMicroseconds(66);

                bool present = (digitalRead(_pinOneWire) == 0);

                delayMicroseconds(480 - 66);

                if (digitalRead(_pinOneWire) == 0) present = false;   // Kurzschluss

                return present;
            }

        #else

            bool Mbc89Utils::owiReset()
            {
                pinMode(_pinOneWire, OUTPUT);
                digitalWrite(_pinOneWire, LOW);

                delayMicroseconds(480);

                digitalWrite(_pinOneWire, HIGH);
                pinMode(_pinOneWire, INPUT_PULLUP);

                delayMicroseconds(66);

                bool present = (digitalRead(_pinOneWire) == 0);

                delayMicroseconds(480 - 66);

                if (digitalRead(_pinOneWire) == 0) present = false;   // Kurzschluss

                return present;
            }

        #endif

        //*****************************************************************************************
        //* Kommunikationsschicht BIT auf dem 1-Wire-Bus (siehe owiReset() zum Hintergrund der    *
        //* AVR-Portregister- bzw. ESP32-Open-Drain-Variante)                                     *
        //*****************************************************************************************
        //* b: zu schreibendes Bit                                                                *
        //*****************************************************************************************
        //* Return: gelesenes Bit vom Bus                                                         *
        //*****************************************************************************************

        #if defined(__AVR__)

            uint8_t Mbc89Utils::owiBitIo(uint8_t b)
            {
                *_owiPort &= (uint8_t) ~_owiMask;
                *_owiDdr  |= _owiMask;

                delayMicroseconds(1);

                if (b)
                {
                    *_owiPort |= _owiMask;
                    *_owiDdr  &= (uint8_t) ~_owiMask;
                }

                delayMicroseconds(14);

                if ((*_owiPinReg & _owiMask) == 0) b = 0;

                delayMicroseconds(45);

                *_owiPort |= _owiMask;
                *_owiDdr  &= (uint8_t) ~_owiMask;   // PULL-UP aktivieren

                return b;
            }

        #elif defined(ESP32)

            // Siehe owiReset() - Pin bleibt permanent OUTPUT_OPEN_DRAIN|PULLUP, kein pinMode()
            // im zeitkritischen Pfad.

            uint8_t Mbc89Utils::owiBitIo(uint8_t b)
            {
                digitalWrite(_pinOneWire, LOW);

                delayMicroseconds(1);

                if (b) digitalWrite(_pinOneWire, HIGH);

                delayMicroseconds(14);

                if (digitalRead(_pinOneWire) == 0) b = 0;

                delayMicroseconds(45);

                digitalWrite(_pinOneWire, HIGH);   // Bus wieder freigeben (PULLUP aktiv)

                return b;
            }

        #else

            uint8_t Mbc89Utils::owiBitIo(uint8_t b)
            {
                pinMode(_pinOneWire, OUTPUT);
                digitalWrite(_pinOneWire, LOW);

                delayMicroseconds(1);

                if (b)
                {
                    digitalWrite(_pinOneWire, HIGH);
                    pinMode(_pinOneWire, INPUT_PULLUP);
                }

                delayMicroseconds(14);

                if (digitalRead(_pinOneWire) == 0) b = 0;

                delayMicroseconds(45);

                pinMode(_pinOneWire, INPUT_PULLUP);    // PULL-UP aktivieren

                return b;
            }

        #endif

        //*****************************************************************************************
        //* Kommunikationsschicht BYTE auf dem 1-Wire-Bus (schreiben)                             *
        //*****************************************************************************************
        //* b: zu schreibendes Byte                                                               *
        //*****************************************************************************************
        //* Return: gelesenes Byte auf dem Bus (fuer den Lesefall via owiByteRd())                *
        //*****************************************************************************************

        uint8_t Mbc89Utils::owiByteWr(uint8_t b)
        {
            uint8_t i = 8, j;

            do
            {
                j = owiBitIo(b & 1);

                b >>= 1;

                if (j) b |= 0x80;

            } while (--i);

            return b;
        }

        //*****************************************************************************************
        //* Kommunikationsschicht BYTE auf dem 1-Wire-Bus (lesen)                                 *
        //*****************************************************************************************
        //* Return: gelesenes Byte                                                                *
        //*****************************************************************************************

        uint8_t Mbc89Utils::owiByteRd()
        {
            return owiByteWr(0xff);
        }

        //*****************************************************************************************
        //* ROM-Search auf dem 1-Wire-Bus ausfuehren                                              *
        //*****************************************************************************************
        //* diff: Verzweigungspunkt der Suche (Device-Unterscheidung)                             *
        //* id:   8 Byte Zielpuffer fuer die gefundene ROM-ID                                     *
        //*****************************************************************************************
        //* Return: naechster Verzweigungspunkt bzw. OWI_PRESENCE_ERR/OWI_DATA_ERR                *
        //*****************************************************************************************

        uint8_t Mbc89Utils::owiRomSearch(uint8_t diff, uint8_t *id)
        {
            uint8_t next_diff;
            uint8_t b;

            if (!owiReset()) return OWI_PRESENCE_ERR;

            owiByteWr(OWI_SEARCH_ROM);

            next_diff = OWI_LAST_DEVICE;

            uint8_t i = 8 * 8;

            do
            {
                uint8_t j = 8;

                do
                {
                    b = owiBitIo(1);

                    if (owiBitIo(1))
                    {
                        if (b) return OWI_DATA_ERR;
                    }
                    else
                    {
                        if (!b)
                        {
                            if (diff > i || ((*id & 1) && diff != i))
                            {
                                b = 1;
                                next_diff = i;
                            }
                        }
                    }

                    owiBitIo(b);

                    *id >>= 1;

                    if (b) *id |= 0x80;

                    i--;

                } while (--j);

                id++;

            } while (i);

            return next_diff;
        }

        //*****************************************************************************************
        //* Startet die Temperaturmessung (Convert-Kommando)                                      *
        //*****************************************************************************************

        void Mbc89Utils::owiStartTempMeasurement()
        {
            owiReset();

            owiByteWr(OWI_SKIP_ROM);
            owiByteWr(OWI_CONVERT_T);
        }

        //*****************************************************************************************
        //* Liest das Temperaturmessergebnis aus und traegt es formatiert im Systemarray ein      *
        //*****************************************************************************************

        void Mbc89Utils::owiReadTemperature()
        {
            owiReset();

            owiByteWr(OWI_SKIP_ROM);
            owiByteWr(OWI_READ);

            uint16_t tmp = (uint16_t) owiByteRd();
            tmp |= ((uint16_t) owiByteRd()) << 8;

            float dtmp = ((float) tmp) * 0.5f;

            if (_owiType == MBC_OWI_DS18B20) dtmp /= 8.0f;

            _owiValue = dtmp;

            char stmp[5];
            dtostrf(dtmp, 4, 1, stmp);

            _array[MBC_S_PARA + 4] = stmp[0];
            _array[MBC_S_PARA + 5] = stmp[1];
            _array[MBC_S_PARA + 6] = stmp[3];
        }

        //*****************************************************************************************
        //* Initialisiert die 1-Wire-Schnittstelle: sucht den Sensor, liest die Seriennummer aus  *
        //* und vergibt bei unbeschriebenem Namen einen Default-Namen auf Basis der Seriennummer. *
        //* Ein Protokollfehler mitten in der ROM-Suche (OWI_DATA_ERR, z.B. weil der Sensor durch *
        //* Timing-Drift desynchronisiert) zaehlt ebenso als "kein Sensor gefunden" wie           *
        //* OWI_PRESENCE_ERR - andernfalls wuerde eine nur teilweise gelesene, mit Stack-Muell    *
        //* aufgefuellte Seriennummer unbemerkt uebernommen.                                      *
        //*****************************************************************************************
        //* Return: true, wenn ein Sensor vollstaendig und fehlerfrei gefunden wurde              *
        //*****************************************************************************************

        bool Mbc89Utils::owiInit()
        {
            #if defined(ESP32)

                // Einmalig hier statt in jedem owiReset()/owiBitIo()-Aufruf - siehe Kommentar dort.
                // PULLUP wird zusaetzlich zu OUTPUT_OPEN_DRAIN gesetzt, da dieses Projekt (wie im
                // AVR-Original) auf den internen Pull-up des Controllers setzt, kein externer
                // Pull-up-Widerstand am 1-Wire-Pin vorausgesetzt wird.

                pinMode(_pinOneWire, (uint8_t) (OUTPUT_OPEN_DRAIN | PULLUP));
                digitalWrite(_pinOneWire, HIGH);   // Bus freigeben (Ruhezustand)

            #endif

            _owiPresent = false;
            _owiType = MBC_OWI_DS18S20;

            uint8_t id[8];

            uint8_t searchResult = owiRomSearch(OWI_SEARCH_FIRST, id);

            if ((searchResult == OWI_PRESENCE_ERR) || (searchResult == OWI_DATA_ERR))
            {
                return false;
            }

            // Starte Messung um Sensor zu kalibrieren

            owiStartTempMeasurement();

            delay(10);

            _owiPresent = true;

            // Seriennummer auslesen und im Systemarray speichern

            for (uint8_t index = 0; index < 8; index++)
            {
                _array[MBC_S_SNR + 7 - index] = id[index];
            }

            // DS-Typ fuer die Temperaturberechnung sichern

            if (id[0] == MBC_OWI_DS18B20) _owiType = MBC_OWI_DS18B20;

            // Wenn noch kein Name vergeben war, Seriennummer mit '0x'-Praefix eintragen

            if (_array[MBC_S_NAMEBLOCK] == 0xff)
            {
                uint16_t p = MBC_S_NAMEBLOCK;

                _array[p++] = 0x30;
                _array[p]   = 0x78;

                for (uint8_t index = 0; index < 8; index++)
                {
                    p++;
                    _array[p] = hex2ascii(id[7 - index] >> 4);

                    p++;
                    _array[p] = hex2ascii(id[7 - index] & 0x0f);
                }

                p++;
                _array[p++] = 0x81;
                _array[p]   = 0x81;

                eepromWrite(MBC_EE_NAME);
            }

            return true;
        }

//------ Ende Klasse Mbc89Utils -------------------------------------------------------------------

//###### Ende Quellcode Mbc89Utils.cpp ############################################################
