//*************************************************************************************************
//* Modul mbc-83: 4-fach Relais-/Magnetartikeldecoder mit Dimmer (Mbc83Lib)                       *
//*************************************************************************************************
//* Datei: Mbc83Com.cpp                                                                           *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen:                                                                                  *
//* V1.0.0 - 21.08.2026 - Nullversion                                                             *
//* V1.0.1 - 29.08.2026 - RAM-Optimierung                                                         *
//*************************************************************************************************

//###### Start Quellcode Mbc83Com.cpp #############################################################

#include "Mbc83Com.h"
#include <string.h>

//------ Start interne Konstanten -----------------------------------------------------------------

    // CAN-Konstanten (CAN-ID/Sub-Code gemaess Maerklin-Definition), unabhaengig von der
    // MbcCom-Basisklasse fuer die eigene Schaltbefehl-/Rueckmeldezyklus-Auswertung dieses Moduls

    static const uint8_t CAN_ID_ZUBEHOER_SCHALTEN = 0x16;
    static const uint8_t CAN_LOCID_MBCAN          = 0x18;
    static const uint8_t MD_ZYKLUS                = 0x15;   // Anstupsen Rueckmeldung Schaltdecoder

    // Modulspezifische EEPROM-Blockindizes (siehe PC_ARRAY_DATA/MBC_COM_EEPROM in MbcCom.cpp) -
    // diese Hex-IDs werden bei einem Firmware-Update per COMUpdateStart() vom Parametriercenter
    // gesendet und muessen daher exakt mit dessen Erwartung uebereinstimmen (siehe
    // Klassenkommentar in Mbc83Com.h)

    static const uint8_t EE_83_MMAddr    = 0x0A;
    static const uint8_t EE_83_MM_TIMING = 0x0B;
    static const uint8_t EE_83_MM_ART    = 0x0C;
    static const uint8_t EE_83_AUS       = 0x0D;
    static const uint8_t EE_83_RUECK     = 0x0E;
    static const uint8_t EE_83_DIMM      = 0x0F;

    // CS2-Kanalnummer/Zeilenindex des MM-Addr-Konfigkanals (siehe Klassenkommentar in
    // Mbc83Com.h). Die CSx-GUI kann keine Hex-Eingabe, daher wird der CS2
    // nicht die Basisadresse selbst, sondern eine einfache Modulnummer 0...255
    // gemeldet/uebergeben - die Basisadresse ergibt sich daraus als MMADDR_BASE +
    // 4*Modulnummer (0x3000...0x33FC)

    static const uint8_t  KANAL_MODNR = MBC_COM_ANZMESSWERTE + MBC_COM_ANZKONFIG + 1;
    static const uint8_t  ROW_MODNR   = 0;
    static const uint16_t MMADDR_BASE = 0x3000;
    static const uint8_t  MODNR_MAX   = 255;

    // CS2-Kanalnummer/Zeilenindex des Auto-Aus-Konfigkanals (Rohwert x10ms, Bereich 0...200 -
    // deckungsgleich mit dem Parametriercenter). CS2-Konfigkanal lehnt einen Wert ausserhalb ab
    // (siehe cs2IndexParameterExtern()), der ParaCenter-Weg (PC_ARRAY_DATA) klemmt stattdessen
    // auf die obere Grenze (siehe eepromWriteParaExtern())

    static const uint8_t KANAL_AUS = KANAL_MODNR + 1;
    static const uint8_t ROW_AUS   = 1;

    // CS2-Kanalnummer/Zeilenindex des Meldezyklus-Konfigkanals (Auswahlliste, Kenner 0x01, 2
    // Punkte "disabled"/"enabled") - Offset 3 innerhalb der Zeile ist bei diesem Kenner die
    // "Jetzige Einstellung" (Index in die Liste)

    static const uint8_t KANAL_RUECK  = KANAL_AUS + 1;
    static const uint8_t ROW_RUECK    = 2;
    static const uint8_t RUECK_OFFSET = 3;

    // CS2-Kanalnummer/Zeilenindex des Dimmer-Konfigkanals (Rohwert 0...200, gleiches
    // Slider-Muster wie MM-Addr/Auto-Aus - mit Bereichspruefung in
    // cs2IndexParameterExtern(), anders als Auto-Aus)

    static const uint8_t KANAL_DIMM  = KANAL_RUECK + 1;
    static const uint8_t ROW_DIMM    = 3;
    static const uint8_t DIMM_MAX    = 200;

    // CS2-Kanalnummer/Zeilenindex-Basis der 8 interleaved "Art"-/Timing-Konfigkanaele je Ausgang
    // (Art Sp1, Timing Sp1, Art Sp2, Timing Sp2, ...). Art ist eine echte 3-Punkte-Auswahlliste
    // (0=Impuls, 1=Wechsel, 2=StartUp), editierbar aus der CSx-GUI. Timing-Bereich 1...250 x10ms
    // (entspricht 10...2500 ms).

    static const uint8_t KANAL_ART_TIMING_BASE = KANAL_DIMM + 1;   // 11..18
    static const uint8_t ROW_ART_TIMING_BASE   = 4;                 // Zeilen 4..11

    // ART_OFFSET wie RUECK_OFFSET: bei Kenner 0x01 (Auswahlliste) sitzt die "Jetzige
    // Einstellung" auf Offset 3, NICHT auf Offset 7 (das ist nur bei Kenner 0x02/Slider die
    // "aktuelle Einstellung"-Bytes)

    static const uint8_t ART_OFFSET = 3;

//------ Ende interne Konstanten ------------------------------------------------------------------

//------ Start Systemarray-Feldoffsets (Layout verbindlich, siehe Mbc83Com.h) ----------------------

    static const uint16_t O_MM_H  = MBC_S_GENPURP;
    static const uint16_t O_MM_L  = MBC_S_GENPURP + 1;
    static const uint16_t O_MM    = MBC_S_GENPURP + 2;   // Aktorstellung, Bit(2n)=Gruen,(2n+1)=Rot
    static const uint16_t O_ART   = MBC_S_GENPURP + 3;   // Art der Ausgaenge, 2 Bit je Ausgang
    static const uint16_t O_AUS   = MBC_S_GENPURP + 4;   // Auto-Aus, Rohwert x10ms
    static const uint16_t O_RUECK = MBC_S_GENPURP + 5;
    static const uint16_t O_DIMM  = MBC_S_GENPURP + 6;   // Dimmer-PWM-Duty 0...255
    static const uint16_t O_TIO   = MBC_S_GENPURP + 7;   // 4 Byte: Spulentiming je Ausgang, x10ms

//------ Ende Systemarray-Feldoffsets -------------------------------------------------------------

//------ Start Klasse Mbc83Com --------------------------------------------------------------------

    // PROGMEM: nur einmal hier deklariert, in allen Konstruktor-Zweigen per memcpy_P gelesen

    static const uint8_t initCs2Ex[MBC83_ANZMESSWERTEMODUL + MBC83_ANZKONFIGMODUL]
                                   [MBC_COM_CS2PAKETE * 8] PROGMEM =
    {
        // INDEX_CTRL 1.EX.1 - Modulnummer (0...255), Basisadresse = MMADDR_BASE + 4*Modulnummer

        {   KANAL_MODNR,0x02,0x00,0x00,0x00,0xff,0x00,0x00,              // 1...8
            'M','M','-','A','d','d','r',':',                             // 9...16
            0x00,'0',0x00,'2','5','5',0x00,'#',                          // 17...24
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,                     // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                      // 33...40
        },

        // INDEX_CTRL 1.EX.2 - Auto-Aus (Rohwert, Einheit x10ms)

        {   KANAL_AUS,0x02,0x00,0x00,0x00,0xc8,0x00,0x00,                // 1...8
            'A','u','t','o','-','A','u','s',                             // 9...16
            ' ','x','1','0','m','s',':',0x00,                            // 17...24
            '0',0x00,'2','0','0',0x00,'#',0x00,                          // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                      // 33...40
        },

        // INDEX_CTRL 1.EX.3 - Meldezyklus (Auswahlliste: 0=disabled, 1=enabled)

        {   KANAL_RUECK,0x01,0x02,0x00,0x00,0x00,0x00,0x00,              // 1...8
            'M','e','l','d','e','z','y','k',                             // 9...16
            'l','u','s',':',0x00,'d','i','s',                            // 17...24
            'a','b','l','e','d',0x00,'e','n',                            // 25...32
            'a','b','l','e','d',0x00,0x00,0x00                           // 33...40
        },

        // INDEX_CTRL 1.EX.4 - Dimmer-Startwert (Rohwert 0...200)

        {   KANAL_DIMM,0x02,0x00,0x00,0x00,0xc8,0x00,0x00,               // 1...8
            'D','i','m','m','e','r',':',0x00,                            // 9...16
            '0',0x00,'2','0','0',0x00,'#',0x00,                          // 17...24
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,                     // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                      // 33...40
        },

        // INDEX_CTRL 1.EX.5 - Art der Aktoren Doppelspulenantrieb 1 (Auswahlliste mit 3 Punkten,
        // editierbar: 0=Impuls, 1=Wechsel, 2=StartUp)

        {   KANAL_ART_TIMING_BASE,0x01,0x03,0x00,0x00,0x00,0x00,0x00,    // 1...8
            'S','p','1',' ','A','r','t',':',                            // 9...16
            0x00,'I','m','p','u','l','s',0x00,                          // 17...24
            'W','e','c','h','s','e','l',0x00,                           // 25...32
            'S','t','a','r','t','U','p',0x00                            // 33...40
        },

        // INDEX_CTRL 1.EX.6 - Impulszeit Doppelspulenantrieb 1 (Rohwert, Einheit x10ms)

        {   KANAL_ART_TIMING_BASE + 1,0x02,0x00,0x01,0x00,0xfa,0x00,0x00, // 1...8
            'S','p','1',' ','x','1','0','m',                            // 9...16
            's',':',0x00,'1',0x00,'2','5','0',                          // 17...24
            0x00,'#',0x00,0x00,0x00,0x00,0x00,0x00,                     // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                     // 33...40
        },

        // INDEX_CTRL 1.EX.7 - Art der Aktoren Doppelspulenantrieb 2 (Auswahlliste mit 3 Punkten)

        {   KANAL_ART_TIMING_BASE + 2,0x01,0x03,0x00,0x00,0x00,0x00,0x00, // 1...8
            'S','p','2',' ','A','r','t',':',                            // 9...16
            0x00,'I','m','p','u','l','s',0x00,                          // 17...24
            'W','e','c','h','s','e','l',0x00,                           // 25...32
            'S','t','a','r','t','U','p',0x00                            // 33...40
        },

        // INDEX_CTRL 1.EX.8 - Impulszeit Doppelspulenantrieb 2 (Rohwert, Einheit x10ms)

        {   KANAL_ART_TIMING_BASE + 3,0x02,0x00,0x01,0x00,0xfa,0x00,0x00, // 1...8
            'S','p','2',' ','x','1','0','m',                            // 9...16
            's',':',0x00,'1',0x00,'2','5','0',                          // 17...24
            0x00,'#',0x00,0x00,0x00,0x00,0x00,0x00,                     // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                     // 33...40
        },

        // INDEX_CTRL 1.EX.9 - Art der Aktoren Doppelspulenantrieb 3 (Auswahlliste mit 3 Punkten)

        {   KANAL_ART_TIMING_BASE + 4,0x01,0x03,0x00,0x00,0x00,0x00,0x00, // 1...8
            'S','p','3',' ','A','r','t',':',                            // 9...16
            0x00,'I','m','p','u','l','s',0x00,                          // 17...24
            'W','e','c','h','s','e','l',0x00,                           // 25...32
            'S','t','a','r','t','U','p',0x00                            // 33...40
        },

        // INDEX_CTRL 1.EX.10 - Impulszeit Doppelspulenantrieb 3 (Rohwert, Einheit x10ms)

        {   KANAL_ART_TIMING_BASE + 5,0x02,0x00,0x01,0x00,0xfa,0x00,0x00, // 1...8
            'S','p','3',' ','x','1','0','m',                            // 9...16
            's',':',0x00,'1',0x00,'2','5','0',                          // 17...24
            0x00,'#',0x00,0x00,0x00,0x00,0x00,0x00,                     // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                     // 33...40
        },

        // INDEX_CTRL 1.EX.11 - Art der Aktoren Doppelspulenantrieb 4 (Auswahlliste mit 3 Punkten)

        {   KANAL_ART_TIMING_BASE + 6,0x01,0x03,0x00,0x00,0x00,0x00,0x00, // 1...8
            'S','p','4',' ','A','r','t',':',                            // 9...16
            0x00,'I','m','p','u','l','s',0x00,                          // 17...24
            'W','e','c','h','s','e','l',0x00,                           // 25...32
            'S','t','a','r','t','U','p',0x00                            // 33...40
        },

        // INDEX_CTRL 1.EX.12 - Impulszeit Doppelspulenantrieb 4 (Rohwert, Einheit x10ms)

        {   KANAL_ART_TIMING_BASE + 7,0x02,0x00,0x01,0x00,0xfa,0x00,0x00, // 1...8
            'S','p','4',' ','x','1','0','m',                            // 9...16
            's',':',0x00,'1',0x00,'2','5','0',                          // 17...24
            0x00,'#',0x00,0x00,0x00,0x00,0x00,0x00,                     // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                     // 33...40
        }
    };

    static const uint8_t initMsgKonfigOk[MBC_COM_MSG_LAENGE] PROGMEM =
        {0x00,0x01,0x00,0x00,0x07,0x00,0x00,0x00,0x00,0x0b,0x00,0x01,0x00};

    //*********************************************************************************************
    //* Konstruktor zur Deklaration von Objektvariablen                                           *
    //*********************************************************************************************

    #if defined(__AVR__)

        Mbc83Com::Mbc83Com(MbcUtils &utils, MbcCan &can, MbcBootStorage &bootStorage,
                            MbcDigitalOut *(&relayOut)[8], MbcDigitalIn *(&relayIn)[4],
                            MbcDigitalIn *(&switchboard)[8], uint8_t pinDimmer, uint8_t pinReset)
            : MbcCom(utils, can, bootStorage, pinReset,
                     MBC83_ANZMESSWERTEMODUL, MBC83_ANZKONFIGMODUL),
              _pinReset(pinReset)
        {

    #else

        Mbc83Com::Mbc83Com(MbcUtils &utils, MbcCan &can, MbcBootStorage &bootStorage,
                            MbcDigitalOut *(&relayOut)[8], MbcDigitalIn *(&relayIn)[4],
                            MbcDigitalIn *(&switchboard)[8], uint8_t pinDimmer)
            : MbcCom(utils, can, bootStorage,
                     MBC83_ANZMESSWERTEMODUL, MBC83_ANZKONFIGMODUL)
        {

    #endif

        memcpy_P(_msgKonfigOk, initMsgKonfigOk, MBC_COM_MSG_LAENGE);

        for (uint8_t i = 0; i < 8; i++)
        {
            _relayOut[i]     = relayOut[i];
            _switchboard[i]  = switchboard[i];
            _switchState[i]  = 1;   // Ruhezustand (Taste losgelassen)
        }

        for (uint8_t i = 0; i < 4; i++) _relayIn[i] = relayIn[i];

        _pinDimmer = pinDimmer;

        _pulseActive     = false;
        _pulseOutput     = 0;
        _pulseState      = 0;
        _pulseStartMs    = 0;
        _pulseDurationMs = 0;

        _zyklusstep = 0;
        _baseAddr   = 0;
    }

    //*********************************************************************************************
    //* Einmalig nach dem begin() der Basisklasse aufrufen: Relais-/Rueckmelde-/Stellpultpins     *
    //* konfigurieren, gespeicherte Parameter aus der EEPROM-Zusatzregion laden und die           *
    //* Aktorstellung aus den Rueckmelde-Eingaengen ableiten (siehe Klassenkommentar in           *
    //* Mbc83Com.h) - NICHT aus dem EEPROM.                                                       *
    //*********************************************************************************************

    void Mbc83Com::beginModule()
    {
        for (uint8_t i = 0; i < 8; i++)
        {
            _relayOut[i]->begin();
            _switchboard[i]->begin();
        }

        for (uint8_t i = 0; i < 4; i++) _relayIn[i]->begin();

        pinMode(_pinDimmer, OUTPUT);

        eepromReadParaExtern();

        outputStateInit();
    }

    //*********************************************************************************************
    //* Je loop()-Durchlauf aufrufen: Stellpult auswerten, faelligen Rueckmeldezyklus-Schritt     *
    //* abarbeiten (startet je Schritt einen echten Timing-Puls und wartet auf dessen Ende -      *
    //* siehe Klassenkommentar in Mbc83Com.h, KEIN fester 100ms-Takt), faelligen Pulsabbruch      *
    //* pruefen und CAN-Nachrichtenempfang/-Auswertung (ruft intern auch die geerbte check()      *
    //* auf).                                                                                     *
    //*********************************************************************************************

    void Mbc83Com::update()
    {
        poll();

        if (!_pulseActive) switchboardCheck();

        if (!_pulseActive && (_zyklusstep > 0))
        {
            uint8_t number = (uint8_t) (_zyklusstep - 1);

            if (_zyklusstep <= 4)
            {
                uint8_t *arr   = _utils->array();
                bool     gruen = _relayIn[number]->read();   // aktiv-LOW: true = Gruen bestaetigt
                uint8_t  state = gruen ? 0x01 : 0x00;

                arr[O_MM] = (uint8_t) ((arr[O_MM] & ~(0x03 << (2 * number)))
                                        | ((gruen ? 0x01 : 0x02) << (2 * number)));

                uint16_t addr = (uint16_t) (_baseAddr + number);

                can_msg[MBC_COM_MSG_CS2_0x16][MBC_CAN_D2] = (uint8_t) ((addr & 0xff00) >> 8);
                can_msg[MBC_COM_MSG_CS2_0x16][MBC_CAN_D3] = (uint8_t) (addr & 0x00ff);
                can_msg[MBC_COM_MSG_CS2_0x16][MBC_CAN_D4] = state;
                can_msg[MBC_COM_MSG_CS2_0x16][MBC_CAN_D5] = 0x01;

                sendMessage(can_msg[MBC_COM_MSG_CS2_0x16]);

                // Sperre fuer virtuelle Ausgaenge (z.B. zur Ansteuerung eines vorgelagerten
                // Adapters) auswerten: nur wenn die Art dieses Ausgangs 0 ist, wird das Relais
                // physisch gepulst

                uint8_t artShift = (uint8_t) (2 * number);
                bool    real     = (((arr[O_ART] >> artShift) & 0x03) == 0);

                outputOn(number, state, real);
            }

            if (++_zyklusstep > 4) _zyklusstep = 0;
        }

        if (_pulseActive && ((millis() - _pulseStartMs) >= _pulseDurationMs)) outputOff();

        // CAN-Nachricht empfangen?

        uint8_t msg[MBC_CAN_MSG_LEN];

        if (_can->receive(msg))
        {
            check(msg);

            bool isSchaltbefehl = (msg[MBC_CAN_ID] == CAN_ID_ZUBEHOER_SCHALTEN)
                               && (msg[MBC_CAN_D2] != CAN_LOCID_MBCAN);

            if (isSchaltbefehl) handleSwitchCommand(msg);

            // Rueckmeldeaufforderung vom vorgelagerten LinkS88: Rueckmeldezyklus starten, wenn
            // gerade keiner laeuft, Rueckmeldung freigegeben und die Basisadresse passt

            bool isZyklusAnstoss = (msg[MBC_CAN_D2] == CAN_LOCID_MBCAN)
                                && (msg[MBC_CAN_D3] == MD_ZYKLUS);

            if (isZyklusAnstoss && (_zyklusstep == 0))
            {
                uint8_t *arr = _utils->array();

                if (arr[O_RUECK])
                {
                    uint16_t addr = (uint16_t) (((uint16_t) msg[MBC_CAN_D6] << 8)
                                               | (uint16_t) msg[MBC_CAN_D7]);

                    if (addr == (uint16_t) (_baseAddr - 0x3000)) _zyklusstep = 1;
                }
            }
        }
    }

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //+ Erweiterungspunkte der Basisklasse (siehe MbcCom.h)                                       +
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    //*********************************************************************************************
    //* Laedt Spulentiming, Dimmerstellung, Art der Ausgaenge (KEINE 0xff-Korrektur, siehe        *
    //* Klassenkommentar in Mbc83Com.h), Auto-Aus, Zwangsrueckmeldezyklus und MM-Adresse aus der  *
    //* EEPROM-Zusatzregion (0xff-Sentinel-Defaults exakt wie mbc_eeprom_read_para()) und wendet  *
    //* den geladenen Dimmerwert an. Die Aktorstellung wird NICHT hier geladen, siehe             *
    //* outputStateInit().                                                                        *
    //*********************************************************************************************

    void Mbc83Com::eepromReadParaExtern()
    {
        uint8_t *arr = _utils->array();

        // Spulentiming je Ausgang - unbeschrieben (0xff) auf 20 (=200ms)

        _utils->eepromReadExtra(EEX_83_TIO, &arr[O_TIO], 4);

        for (uint8_t i = 0; i < 4; i++) if (arr[O_TIO + i] == 0xff) arr[O_TIO + i] = 20;

        // Dimmerstellung - unbeschrieben (0xff) auf 0 (aus)

        _utils->eepromReadExtra(EEX_83_DIMM, &arr[O_DIMM], 1);

        if (arr[O_DIMM] == 0xff) arr[O_DIMM] = 0;

        // Art der Ausgaenge - bewusst KEINE 0xff-Korrektur (0xff bedeutet "alle gesperrt" und
        // bleibt als Default erhalten, siehe Klassenkommentar in Mbc83Com.h)

        _utils->eepromReadExtra(EEX_83_ART, &arr[O_ART], 1);

        // Auto-Aus - unbeschrieben (0xff) auf 100 (=1000ms)

        _utils->eepromReadExtra(EEX_83_AUS, &arr[O_AUS], 1);

        if (arr[O_AUS] == 0xff) arr[O_AUS] = 100;

        // Zwangsrueckmeldezyklus - unbeschrieben (0xff) auf 0

        _utils->eepromReadExtra(EEX_83_RUECK, &arr[O_RUECK], 1);

        if (arr[O_RUECK] == 0xff) arr[O_RUECK] = 0;

        // MM-Adresse - Default 0x3000, wenn das High-Byte unbeschrieben (0xff) ist

        uint8_t addr[2];

        _utils->eepromReadExtra(EEX_83_MMAddr, addr, 2);

        if (addr[0] == 0xff) { addr[0] = 0x30; addr[1] = 0x00; }

        arr[O_MM_H] = addr[0];
        arr[O_MM_L] = addr[1];

        _baseAddr = (uint16_t) (((uint16_t) arr[O_MM_H] << 8) | (uint16_t) arr[O_MM_L]);

        applyDimmer();
    }

    //*********************************************************************************************
    //* Persistiert je nach block den passenden Ausschnitt des Systemarrays in der EEPROM-        *
    //* Zusatzregion und wendet danach UNBEDINGT den aktuellen Dimmerwert erneut an (nicht nur    *
    //* beim eigenen Dimmer-Block). Bei block==EE_83_AUS wird der Wert                            *
    //* zusaetzlich auf 200 geklemmt (nicht verworfen) - betrifft in der Praxis nur den           *
    //* ParaCenter-Weg (PC_ARRAY_DATA), da der CS2-Konfigkanal ungueltige Auto-Aus-Werte bereits  *
    //* in cs2IndexParameterExtern() ablehnt, bevor diese Funktion ueberhaupt aufgerufen wird.    *
    //* Bei block==EE_83_MMAddr wird zusaetzlich sofort ein Rueckmeldezyklus ausgeloest, wenn der *
    //* Meldezyklus aktuell aktiviert ist; bei block==EE_83_RUECK genau dann, wenn der            *
    //* Meldezyklus dabei von deaktiviert auf aktiviert wechselt - unabhaengig davon, ob die      *
    //* Aenderung ueber CS2 oder ParaCenter kam, da beide Wege hier zusammenlaufen.               *
    //*********************************************************************************************

    void Mbc83Com::eepromWriteParaExtern(uint8_t block)
    {
        uint8_t *arr = _utils->array();

        if (block == EE_83_MMAddr)
        {
            _utils->eepromWriteExtra(EEX_83_MMAddr, &arr[O_MM_H], 2);

            _baseAddr = (uint16_t) (((uint16_t) arr[O_MM_H] << 8) | (uint16_t) arr[O_MM_L]);

            // MM-Addr geaendert: Rueckmeldeprozess sofort anstossen, wenn der Meldezyklus
            // aktiviert ist und gerade keiner laeuft

            if (arr[O_RUECK] && (_zyklusstep == 0)) _zyklusstep = 1;
        }
        else if (block == EE_83_MM_TIMING)
        {
            _utils->eepromWriteExtra(EEX_83_TIO, &arr[O_TIO], 4);
        }
        else if (block == EE_83_MM_ART)
        {
            _utils->eepromWriteExtra(EEX_83_ART, &arr[O_ART], 1);
        }
        else if (block == EE_83_AUS)
        {
            // Bereichspruefung 0...200 fuer den ParaCenter-Weg (PC_ARRAY_DATA) - ein zu hoher
            // Wert wird auf die obere Grenze geklemmt statt verworfen; der CS2-Konfigkanal
            // validiert stattdessen bereits vorher in cs2IndexParameterExtern() und lehnt
            // ungueltige Werte dort ab

            if (arr[O_AUS] > 200) arr[O_AUS] = 200;

            _utils->eepromWriteExtra(EEX_83_AUS, &arr[O_AUS], 1);
        }
        else if (block == EE_83_RUECK)
        {
            // Alten, noch persistierten Zustand vor dem Ueberschreiben lesen, um den
            // disabled->enabled-Uebergang zu erkennen (siehe unten) - arr[O_RUECK] traegt hier
            // bereits den NEUEN Wert (vom Aufrufer vorher gesetzt)

            uint8_t alt;
            _utils->eepromReadExtra(EEX_83_RUECK, &alt, 1);
            bool warAktiviert = (alt != 0);

            _utils->eepromWriteExtra(EEX_83_RUECK, &arr[O_RUECK], 1);

            // Meldezyklus wurde soeben aktiviert: Rueckmeldeprozess sofort anstossen, wenn
            // gerade keiner laeuft

            if (!warAktiviert && arr[O_RUECK] && (_zyklusstep == 0)) _zyklusstep = 1;
        }
        else if (block == EE_83_DIMM)
        {
            _utils->eepromWriteExtra(EEX_83_DIMM, &arr[O_DIMM], 1);
        }

        applyDimmer();
    }

    //*********************************************************************************************
    //* Traegt GUiD/HASH in die Konfig-Bestaetigungsnachricht _msgKonfigOk ein.                   *
    //*********************************************************************************************

    void Mbc83Com::hashInitExtern()
    {
        uint8_t *arr = _utils->array();

        _msgKonfigOk[MBC_CAN_D0] = arr[MBC_S_KN + 4];
        _msgKonfigOk[MBC_CAN_D1] = arr[MBC_S_GUID + 1];
        _msgKonfigOk[MBC_CAN_D2] = arr[MBC_S_GUID + 2];
        _msgKonfigOk[MBC_CAN_D3] = arr[MBC_S_GUID + 3];

        _msgKonfigOk[MBC_CAN_HASH_H] = can_msg[0][MBC_CAN_HASH_H];
        _msgKonfigOk[MBC_CAN_HASH_L] = can_msg[0][MBC_CAN_HASH_L];
    }

    //*********************************************************************************************
    //* Uebernimmt Konfigkanaele von der CS2: msg[D5]==KANAL_MODNR - neue Modulnummer (0...255)   *
    //* in MMADDR_BASE + 4*Modulnummer umrechnen; msg[D5]==KANAL_AUS - neue Auto-Aus-Zeit         *
    //* (Rohwert 0...200, x10ms) uebernehmen; msg[D5]==KANAL_RUECK - neue Meldezyklus-Auswahl     *
    //* (0=disabled/1=enabled) uebernehmen; msg[D5]==KANAL_DIMM - neuer Dimmer-Startwert (Rohwert *
    //* 0...200) uebernehmen; msg[D5] im Bereich KANAL_ART_TIMING_BASE..+7 - je                   *
    //* Doppelspulenantrieb interleaved (Art Sp1, Timing Sp1, Art Sp2, ...): die Art-Kanaele      *
    //* (gerader Offset) uebernehmen eine neue Nutzungsart (Auswahlliste                          *
    //* 0=Impuls/1=Wechsel/2=StartUp, setzt/loescht nur die betroffenen 2 Bit des jeweiligen      *
    //* Paarfeldes in O_ART), die Timing-Kanaele (ungerader Offset) eine neue Impulszeit (Rohwert *
    //* 1...250, x10ms). Alle Kanaele werden bei einem ungueltigen Wert mit FALSE quittiert       *
    //* (keine Wirkung, kein Schreiben/Persistieren) - nur der ParaCenter-Weg (PC_ARRAY_DATA,     *
    //* siehe eepromWriteParaExtern()) klemmt Auto-Aus stattdessen auf den gueltigen Bereich.     *
    //*********************************************************************************************
    //* msg: CAN-Nachricht (msg[D5]=Kanal, msg[D6..D7]=neuer Wert)                                *
    //*********************************************************************************************

    void Mbc83Com::cs2IndexParameterExtern(const uint8_t msg[MBC_COM_MSG_LAENGE])
    {
        uint8_t  kanal = msg[MBC_CAN_D5];
        uint16_t wert  = (uint16_t) (((uint16_t) msg[MBC_CAN_D6] << 8) | (uint16_t) msg[MBC_CAN_D7]);

        if (kanal == KANAL_MODNR)
        {
            bool gueltig = (wert <= MODNR_MAX);

            if (gueltig)
            {
                uint16_t addr    = (uint16_t) (MMADDR_BASE + 4 * wert);
                uint8_t  *arr    = _utils->array();
                uint16_t oldAddr = (uint16_t) (((uint16_t) arr[O_MM_H] << 8) | arr[O_MM_L]);

                if (addr != oldAddr)
                {
                    arr[O_MM_H] = (uint8_t) (addr >> 8);
                    arr[O_MM_L] = (uint8_t) (addr & 0x00ff);

                    eepromWriteParaExtern(EE_83_MMAddr);   // persistiert + aktualisiert _baseAddr
                }
            }

            _msgKonfigOk[MBC_CAN_D5] = KANAL_MODNR;
            _msgKonfigOk[MBC_CAN_D6] = gueltig ? 0x01 : 0x00;
        }
        else if (kanal == KANAL_AUS)
        {
            bool gueltig = (wert <= 200);

            if (gueltig)
            {
                uint8_t *arr = _utils->array();

                if (arr[O_AUS] != (uint8_t) wert)
                {
                    arr[O_AUS] = (uint8_t) wert;

                    eepromWriteParaExtern(EE_83_AUS);   // persistiert
                }
            }

            _msgKonfigOk[MBC_CAN_D5] = KANAL_AUS;
            _msgKonfigOk[MBC_CAN_D6] = gueltig ? 0x01 : 0x00;
        }
        else if (kanal == KANAL_RUECK)
        {
            bool gueltig = (wert <= 1);

            if (gueltig)
            {
                uint8_t *arr = _utils->array();

                if (arr[O_RUECK] != (uint8_t) wert)
                {
                    arr[O_RUECK] = (uint8_t) wert;

                    eepromWriteParaExtern(EE_83_RUECK);   // persistiert + stoesst ggf. Zyklus an
                }
            }

            _msgKonfigOk[MBC_CAN_D5] = KANAL_RUECK;
            _msgKonfigOk[MBC_CAN_D6] = gueltig ? 0x01 : 0x00;
        }
        else if (kanal == KANAL_DIMM)
        {
            bool gueltig = (wert <= DIMM_MAX);

            if (gueltig)
            {
                uint8_t *arr = _utils->array();

                if (arr[O_DIMM] != (uint8_t) wert)
                {
                    arr[O_DIMM] = (uint8_t) wert;

                    eepromWriteParaExtern(EE_83_DIMM);   // persistiert
                }
            }

            _msgKonfigOk[MBC_CAN_D5] = KANAL_DIMM;
            _msgKonfigOk[MBC_CAN_D6] = gueltig ? 0x01 : 0x00;
        }
        else if ((kanal >= KANAL_ART_TIMING_BASE) && (kanal < (uint8_t) (KANAL_ART_TIMING_BASE + 8)))
        {
            uint8_t offset = (uint8_t) (kanal - KANAL_ART_TIMING_BASE);
            uint8_t idx    = (uint8_t) (offset / 2);

            if ((offset & 0x01) == 0)
            {
                // Art (gerader Offset) - 3 Punkte: 0=Impuls, 1=Wechsel, 2=StartUp

                bool gueltig = (wert <= 2);

                if (gueltig)
                {
                    uint8_t *arr      = _utils->array();
                    uint8_t  artShift = (uint8_t) (2 * idx);
                    uint8_t  altWert  = (uint8_t) ((arr[O_ART] >> artShift) & 0x03);

                    if (altWert != (uint8_t) wert)
                    {
                        arr[O_ART] = (uint8_t) ((arr[O_ART] & ~(0x03 << artShift))
                                                 | ((uint8_t) wert << artShift));

                        eepromWriteParaExtern(EE_83_MM_ART);   // persistiert
                    }
                }

                _msgKonfigOk[MBC_CAN_D5] = kanal;
                _msgKonfigOk[MBC_CAN_D6] = gueltig ? 0x01 : 0x00;
            }
            else
            {
                // Impulszeit (ungerader Offset), Rohwert 1...250 x10ms

                bool gueltig = (wert >= 1) && (wert <= 250);

                if (gueltig)
                {
                    uint8_t *arr = _utils->array();

                    if (arr[O_TIO + idx] != (uint8_t) wert)
                    {
                        arr[O_TIO + idx] = (uint8_t) wert;

                        eepromWriteParaExtern(EE_83_MM_TIMING);   // persistiert
                    }
                }

                _msgKonfigOk[MBC_CAN_D5] = kanal;
                _msgKonfigOk[MBC_CAN_D6] = gueltig ? 0x01 : 0x00;
            }
        }
        else
        {
            return;
        }

        sendMessage(_msgKonfigOk);
    }

    //*********************************************************************************************
    //* Baut die angeforderte Kanalzeile frisch in _cs2ScratchRow zusammen - mbc-83 hat 12        *
    //* Kanaele (ROW_MODNR = MM-Addr, ROW_AUS = Auto-Aus, ROW_RUECK = Meldezyklus, ROW_DIMM =     *
    //* Dimmer, ROW_ART_TIMING_BASE+0..7 = je Doppelspulenantrieb interleaved Art/Timing), sonst  *
    //* nullptr. Vorlage (initCs2Ex[], PROGMEM) bleibt statisch, nur der aktuelle Wert wird bei   *
    //* jedem Aufruf frisch aus dem Systemarray/_baseAddr gepatcht.                                *
    //*********************************************************************************************
    //* laufendeNr: 0..ANZMESSWERTEMODUL+ANZKONFIGMODUL-1                                         *
    //*********************************************************************************************
    //* Return: Zeiger auf _cs2ScratchRow (nur bis zum naechsten Aufruf gueltig) oder nullptr     *
    //*********************************************************************************************

    const uint8_t* Mbc83Com::cs2IndexExtern(uint8_t laufendeNr)
    {
        if (laufendeNr >= (MBC83_ANZMESSWERTEMODUL + MBC83_ANZKONFIGMODUL)) return nullptr;

        memcpy_P(_cs2ScratchRow, initCs2Ex[laufendeNr], sizeof(_cs2ScratchRow));

        uint8_t *arr = _utils->array();

        if (laufendeNr == ROW_MODNR)
        {
            _cs2ScratchRow[7] = (uint8_t) ((_baseAddr - MMADDR_BASE) / 4);
        }
        else if (laufendeNr == ROW_AUS)
        {
            _cs2ScratchRow[7] = arr[O_AUS];
        }
        else if (laufendeNr == ROW_RUECK)
        {
            _cs2ScratchRow[RUECK_OFFSET] = arr[O_RUECK];
        }
        else if (laufendeNr == ROW_DIMM)
        {
            _cs2ScratchRow[7] = arr[O_DIMM];
        }
        else if ((laufendeNr >= ROW_ART_TIMING_BASE)
                 && (laufendeNr < (uint8_t) (ROW_ART_TIMING_BASE + 8)))
        {
            uint8_t offset = (uint8_t) (laufendeNr - ROW_ART_TIMING_BASE);
            uint8_t idx    = (uint8_t) (offset / 2);

            if ((offset & 0x01) == 0)
            {
                _cs2ScratchRow[ART_OFFSET] = (uint8_t) ((arr[O_ART] >> (2 * idx)) & 0x03);
            }
            else
            {
                _cs2ScratchRow[7] = arr[O_TIO + idx];
            }
        }

        return _cs2ScratchRow;
    }

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //+ Interne Funktionen                                                                        +
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    //*********************************************************************************************
    //* Setzt alle Relais aus und leitet die Aktorstellung aus den 4 Rueckmelde-Eingaengen ab     *
    //* (bistabile Relaisstellung, aktiv-LOW: Pin LOW = Gruen bestaetigt, sonst Rot angenommen) - *
    //* NUR beim Booten aufgerufen, siehe beginModule() und Klassenkommentar in Mbc83Com.h.       *
    //*********************************************************************************************

    void Mbc83Com::outputStateInit()
    {
        outputSwitch(0, false);

        uint8_t *arr = _utils->array();

        for (uint8_t i = 0; i < 4; i++)
        {
            bool gruen = _relayIn[i]->read();

            arr[O_MM] = (uint8_t) ((arr[O_MM] & ~(0x03 << (2 * i)))
                                    | ((gruen ? 0x01 : 0x02) << (2 * i)));
        }
    }

    //*********************************************************************************************
    //* Schaltet eine einzelne Relaisspule ein oder alle Spulen aus.                              *
    //*********************************************************************************************
    //* coil: 0...7 (Ausgang*2+Zustand, nur bei on==true ausgewertet)                             *
    //* on:   true = adressierte Spule einschalten, sonst alle 8 Spulen aus                       *
    //*********************************************************************************************

    void Mbc83Com::outputSwitch(uint8_t coil, bool on)
    {
        if (on)
        {
            _relayOut[coil]->write(true);
        }
        else
        {
            for (uint8_t i = 0; i < 8; i++) _relayOut[i]->write(false);
        }
    }

    //*********************************************************************************************
    //* Startet einen Ausgangspuls: merkt sich Ausgang/Zustand und die (aus Spulentiming UND      *
    //* Auto-Aus abgeleitete) Pulsdauer, energisiert bei real==true die passende Relaisspule. Der *
    //* Aufrufer setzt(!) die Aktorstellung selbst, VOR dem Aufruf (siehe                         *
    //* update()/switchboardCheck()/handleSwitchCommand()).                                       *
    //*********************************************************************************************
    //* number: 0...3                                                                             *
    //* state:  0 = Rot, 1 = Gruen                                                                *
    //* real:   false unterdrueckt das physische Schalten (Rueckmeldezyklus-Sperre, siehe         *
    //*         Klassenkommentar in Mbc83Com.h) - der Pulstimer laeuft trotzdem mit               *
    //*********************************************************************************************

    void Mbc83Com::outputOn(uint8_t number, uint8_t state, bool real)
    {
        uint8_t *arr = _utils->array();

        if (real) outputSwitch((uint8_t) (number * 2 + state), true);

        unsigned long tioMs = (unsigned long) arr[O_TIO + number] * 10UL;
        unsigned long ausMs = (unsigned long) arr[O_AUS] * 10UL;

        _pulseOutput      = number;
        _pulseState       = state;
        _pulseStartMs     = millis();
        _pulseDurationMs  = (tioMs < ausMs) ? tioMs : ausMs;
        _pulseActive      = true;
    }

    //*********************************************************************************************
    //* Beendet einen laufenden Ausgangspuls: schaltet alle Relais aus und sendet - falls ein     *
    //* Puls aktiv war - ein Strom-Aus-Telegramm fuer den zuletzt gepulsten Ausgang. Laeuft       *
    //* sowohl bei Timeout als auch bei einem unterbrechenden direkten Schaltbefehl (siehe        *
    //* handleSwitchCommand()), daher IMMER dieselbe Aus-Telegramm-Logik, unabhaengig vom         *
    //* Ausloeser.                                                                                *
    //*********************************************************************************************

    void Mbc83Com::outputOff()
    {
        outputSwitch(0, false);

        if (_pulseActive)
        {
            uint16_t addr = (uint16_t) (_baseAddr + _pulseOutput);

            can_msg[MBC_COM_MSG_CS2_0x16][MBC_CAN_D2] = (uint8_t) ((addr & 0xff00) >> 8);
            can_msg[MBC_COM_MSG_CS2_0x16][MBC_CAN_D3] = (uint8_t) (addr & 0x00ff);
            can_msg[MBC_COM_MSG_CS2_0x16][MBC_CAN_D4] = _pulseState;
            can_msg[MBC_COM_MSG_CS2_0x16][MBC_CAN_D5] = 0x00;

            sendMessage(can_msg[MBC_COM_MSG_CS2_0x16]);
        }

        _pulseActive = false;
    }

    //*********************************************************************************************
    //* Wendet die aktuelle Dimmerstellung als PWM-Duty auf pinDimmer an - wird bei JEDER         *
    //* Parameteraenderung ueber diese gemeinsame Funktion (siehe                                 *
    //* eepromReadParaExtern()/eepromWriteParaExtern()) neu angewendet.                           *
    //*********************************************************************************************

    void Mbc83Com::applyDimmer()
    {
        analogWrite(_pinDimmer, _utils->array()[O_DIMM]);
    }

    //*********************************************************************************************
    //* Auswerten des Stellpults. Ein Tastendruck wirkt IMMER real (keine Rueckmeldezyklus-       *
    //* Sperre, siehe Klassenkommentar in Mbc83Com.h).                                            *
    //*********************************************************************************************

    void Mbc83Com::switchboardCheck()
    {
        uint8_t number  = 0;
        uint8_t state   = 0;
        bool    pressed = false;

        for (uint8_t i = 0; i < 8; i++)
        {
            bool active = _switchboard[i]->read();

            if (active && (_switchState[i] == 1))
            {
                number  = (uint8_t) (i / 2);
                state   = (uint8_t) (i % 2);   // gerade i = Rot, ungerade i = Gruen
                pressed = true;
                _switchState[i] = 0;
            }

            if (!active && (_switchState[i] == 0)) _switchState[i] = 1;
        }

        if (!pressed) return;

        uint8_t *arr = _utils->array();

        arr[O_MM] = (uint8_t) ((arr[O_MM] & ~(0x03 << (2 * number)))
                                | ((state == 0 ? 0x02 : 0x01) << (2 * number)));

        uint16_t addr = (uint16_t) (_baseAddr + number);

        can_msg[MBC_COM_MSG_CS2_0x16][MBC_CAN_D2] = (uint8_t) ((addr & 0xff00) >> 8);
        can_msg[MBC_COM_MSG_CS2_0x16][MBC_CAN_D3] = (uint8_t) (addr & 0x00ff);
        can_msg[MBC_COM_MSG_CS2_0x16][MBC_CAN_D4] = state;
        can_msg[MBC_COM_MSG_CS2_0x16][MBC_CAN_D5] = 0x01;

        sendMessage(can_msg[MBC_COM_MSG_CS2_0x16]);

        outputOn(number, state, true);
    }

    //*********************************************************************************************
    //* Eingehenden Schaltbefehl (CAN_ID 0x16, kein S88-Anstupsen) auswerten: bei gueltiger, zu   *
    //* diesem Decoder gehoerender Adresse einen laufenden Puls unterbrechen (sendet dabei ggf.   *
    //* noch das Aus-Telegramm des unterbrochenen Pulses, siehe outputOff()) und den neuen Puls   *
    //* starten - IMMER real (keine Rueckmeldezyklus-Sperre). Bestaetigt der CSx/MS2 in jedem     *
    //* Fall (auch bei einem Aus-Kommando) mit eigener ACK-Nachricht (CAN_ID 0x17).               *
    //*********************************************************************************************
    //* msg: empfangene CAN-Nachricht                                                             *
    //*********************************************************************************************

    void Mbc83Com::handleSwitchCommand(const uint8_t msg[MBC_COM_MSG_LAENGE])
    {
        uint16_t addr = (uint16_t) (((uint16_t) msg[MBC_CAN_D2] << 8)
                                     | (uint16_t) msg[MBC_CAN_D3]);

        if ((addr < _baseAddr) || (addr > (uint16_t) (_baseAddr + 3))) return;

        if (msg[MBC_CAN_D5])
        {
            uint8_t number = (uint8_t) (addr - _baseAddr);
            uint8_t state  = msg[MBC_CAN_D4];

            outputOff();

            uint8_t *arr = _utils->array();

            arr[O_MM] = (uint8_t) ((arr[O_MM] & ~(0x03 << (2 * number)))
                                    | ((state == 0 ? 0x02 : 0x01) << (2 * number)));

            outputOn(number, state, true);
        }

        // Bestaetigung an die CSx/MS2 senden - unabhaengig davon, ob D5 gesetzt war

        uint8_t ack[MBC_COM_MSG_LAENGE];

        for (uint8_t i = 0; i < MBC_COM_MSG_LAENGE; i++) ack[i] = msg[i];

        ack[MBC_CAN_HASH_H] = can_msg[MBC_COM_MSG_CS2_0x16][MBC_CAN_HASH_H];
        ack[MBC_CAN_HASH_L] = can_msg[MBC_COM_MSG_CS2_0x16][MBC_CAN_HASH_L];
        ack[MBC_CAN_ID]     = 0x17;

        sendMessage(ack);
    }

//------ Ende Klasse Mbc83Com ---------------------------------------------------------------------

//###### Ende Quellcode Mbc83Com.cpp ##############################################################
