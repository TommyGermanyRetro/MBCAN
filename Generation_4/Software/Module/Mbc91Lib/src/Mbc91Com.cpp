//*************************************************************************************************
//* Modul mbc-91: Magnetartikel-Schaltdecoder mit Stellpult (Mbc91Lib)                            *
//*************************************************************************************************
//* Datei: Mbc91Com.cpp                                                                           *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen:                                                                                  *
//* V1.0.0 - 06.08.2026 - Nullversion                                                             *
//* V1.0.1 - 29.08.2026 - RAM-Optimierung                                                         *
//*************************************************************************************************

//###### Start Quellcode Mbc91Com.cpp #############################################################

#include "Mbc91Com.h"
#include <string.h>

//------ Start interne Konstanten -----------------------------------------------------------------

    // CAN-Befehlsnummern (CAN-ID/Sub-Code gemaess Maerklin-Definition), unabhaengig von der
    // MbcCom-Basisklasse fuer die eigene Schaltbefehl-/Rueckmeldezyklus-Auswertung dieses Moduls

    static const uint8_t CAN_ID_ZUBEHOER_SCHALTEN = 0x16;
    static const uint8_t CAN_LOCID_MBCAN          = 0x18;
    static const uint8_t MD_ZYKLUS                = 0x15;   // Anstupsen Rueckmeldung Schaltdecoder

    // Modulspezifische EEPROM-Blockindizes - diese Werte werden von externer PC-/CS2-
    // Parametriersoftware direkt referenziert (siehe MbcCom.cpp PC_ARRAY_DATA/
    // MBC_COM_EEPROM-Behandlung)

    static const uint8_t EE_91_MMAddr     = 0x0A;
    static const uint8_t EE_91_MM_TIMING  = 0x0B;
    static const uint8_t EE_91_AUS        = 0x0D;
    static const uint8_t EE_91_RUECK      = 0x0E;
    static const uint8_t EE_91_AKTOR      = 0x0F;

    // Byte-Layout der EEPROM-Zusatzregion (9 Byte, siehe MbcUtils::eepromReadExtra()/
    // eepromWriteExtra()) - unabhaengig von den EE_91_*-Blockindizes oben

    static const uint16_t EEX_TIMING = 0;   // 4 Byte: Impulszeiten je Doppelspulenantrieb
    static const uint16_t EEX_AKTOR  = 4;   // 1 Byte: Aktorstellung (MBC_91_MM)
    static const uint16_t EEX_MMADDR = 5;   // 2 Byte: Decoder-Basisadresse (H,L)
    static const uint16_t EEX_AUS    = 7;   // 1 Byte: Auto-Aus-Zeit
    static const uint16_t EEX_RUECK  = 8;   // 1 Byte: Zwangsrueckmeldung-Flag

    // CS2-Kanalnummer/Zeilenindex des neuen MM-Addr-Konfigkanals (siehe Klassenkommentar).
    // Die CSx-GUI kann keine Hex-Eingabe, daher wird der CS2 nicht die
    // Basisadresse selbst, sondern eine einfache Modulnummer 0...255 gemeldet/uebergeben - die
    // Basisadresse ergibt sich daraus als MMADDR_BASE + 4*Modulnummer (0x3000...0x33FC)

    static const uint8_t  KANAL_MODNR  = MBC_COM_ANZMESSWERTE + MBC_COM_ANZKONFIG + 1;  // 7
    static const uint8_t  ROW_MODNR    = 0;
    static const uint16_t MMADDR_BASE  = 0x3000;
    static const uint8_t  MODNR_MAX    = 255;

    // CS2-Kanalnummer/Zeilenindex des neuen Auto-Aus-Konfigkanals (Slider mit Rohwert in
    // x10ms-Einheiten)

    static const uint8_t KANAL_AUS = KANAL_MODNR + 1;  // 8
    static const uint8_t ROW_AUS   = 1;

    // CS2-Kanalnummer/Zeilenindex des neuen Zwangsrueckmeldung-Konfigkanals - anders als
    // MM-Addr/Auto-Aus (Slider, Kenner 0x02) eine echte Auswahlliste
    // (Kenner 0x01, 2 Punkte "disabled"/"enabled"), da hier nur 0/1 sinnvoll sind. Offset 3
    // innerhalb der Zeile ist bei diesem Kenner die "Jetzige Einstellung" (Index in die Liste,
    // siehe CS2-Protokoll "Statusdaten Konfiguration") - nicht Offset 6/7 wie beim Slider.

    static const uint8_t KANAL_RUECK   = KANAL_AUS + 1;  // 9
    static const uint8_t ROW_RUECK     = 2;
    static const uint8_t RUECK_OFFSET  = 3;

    // CS2-Kanalnummer/Zeilenindex-Basis der 8 INTERLEAVED "Art der Aktoren"-/Impulszeit-
    // Konfigkanaele - je Doppelspulenantrieb abwechselnd Art gefolgt
    // von Timing (Art Sp1, Timing Sp1, Art Sp2, Timing Sp2, ...), nicht blockweise gruppiert.
    // Fuer Paar p (0..3): Art-Kanal/-Zeile = BASE + 2*p, Timing-Kanal/-Zeile = BASE + 2*p + 1.
    // Art ist eine Auswahlliste, hier aber REIN INFORMATIV und echt ReadOnly: MBC_91_MM_ART wird
    // nur einmalig beim Start aus der Bestueckungserkennung gelesen (siehe beginModule()),
    // mbc-91 kennt selbst keinen Wechselschalter-Betrieb. Da die Art per Hardware und nicht per
    // Software feststeht, traegt cs2IndexExtern() bei jeder Anfrage NUR die tatsaechlich
    // erkannte Option (entweder "Impuls" oder "Wechsel", nie beide) in die Zeile ein - anders als
    // bei einer echten Auswahl (z.B. Zwangsrueckmeldung) wird kein zweiter, per Software
    // gesperrter Listenpunkt angeboten. cs2IndexParameterExtern() verarbeitet trotzdem nur die
    // Timing-Kanaele (ungerade Offsets) - die Art-Kanaele (gerade Offsets) fallen zusaetzlich
    // auf den Default-Zweig durch, ein Aenderungsversuch in der CSx bleibt damit wirkungslos,
    // analog zu den schreibgeschuetzten Seriennummer/GUiD/FW/HW-Zeilen der Basisklasse
    // (MbcCom.cpp).

    static const uint8_t KANAL_ART_TIMING_BASE = KANAL_RUECK + 1;  // 10..17
    static const uint8_t ROW_ART_TIMING_BASE   = 3;                 // Zeilen 3..10
    static const uint8_t ART_OFFSET            = 3;

    // Systemarray-Layout ab MBC_S_GENPURP (MBC_91_S_MM ff.)

    #define MBC_91_S_MM      MBC_S_GENPURP
    #define MBC_91_MM_H      _utils->array()[MBC_91_S_MM]
    #define MBC_91_MM_L      _utils->array()[MBC_91_S_MM + 1]
    #define MBC_91_MM        _utils->array()[MBC_91_S_MM + 2]
    #define MBC_91_MM_ART    _utils->array()[MBC_91_S_MM + 3]
    #define MBC_91_AUS       _utils->array()[MBC_91_S_MM + 4]
    #define MBC_91_RUECK     _utils->array()[MBC_91_S_MM + 5]
    #define MBC_91_S_ZEIT    (MBC_91_S_MM + 6)
    #define MBC_91_TIO       _utils->array()[MBC_91_S_ZEIT]

//------ Ende interne Konstanten ------------------------------------------------------------------

//------ Start Klasse Mbc91Com --------------------------------------------------------------------

    // PROGMEM: nur einmal hier deklariert, in beiden Konstruktor-Zweigen per memcpy_P gelesen

    static const uint8_t initCs2Ex[MBC91_ANZMESSWERTEMODUL + MBC91_ANZKONFIGMODUL]
                                   [MBC_COM_CS2PAKETE * 8] PROGMEM =
    {
        // INDEX_CTRL 1.EX.1 - Modulnummer (0...255), Basisadresse = MMADDR_BASE + 4*Modulnummer

        {   KANAL_MODNR,0x02,0x00,0x00,0x00,0xff,0x00,0x00,              // 1...8
            'M','M','-','A','d','d','r',':',                           // 9...16
            0x00,'0',0x00,'2','5','5',0x00,'#',                        // 17...24
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,                    // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                     // 33...40
        },

        // INDEX_CTRL 1.EX.2 - Auto-Aus (Rohwert, Einheit x10ms)

        {   KANAL_AUS,0x02,0x00,0x00,0x00,0xc8,0x00,0x00,                // 1...8
            'A','u','t','o','-','A','u','s',                            // 9...16
            ' ','x','1','0','m','s',':',0x00,                           // 17...24
            '0',0x00,'2','0','0',0x00,'#',0x00,                         // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                     // 33...40
        },

        // INDEX_CTRL 1.EX.3 - Meldezyklus (Auswahlliste: 0=disabled, 1=enabled)

        {   KANAL_RUECK,0x01,0x02,0x00,0x00,0x00,0x00,0x00,              // 1...8
            'M','e','l','d','e','z','y','k',                            // 9...16
            'l','u','s',':',0x00,'d','i','s',                           // 17...24
            'a','b','l','e','d',0x00,'e','n',                           // 25...32
            'a','b','l','e','d',0x00,0x00,0x00                          // 33...40
        },

        // INDEX_CTRL 1.EX.4 - Art der Aktoren Doppelspulenantrieb 1 (Auswahlliste mit genau 1
        // Punkt, ReadOnly - Platzhalter "Impuls", cs2IndexExtern() traegt bei jeder Anfrage die
        // tatsaechlich per Bestueckung erkannte Option ein, siehe Klassenkommentar)

        {   KANAL_ART_TIMING_BASE,0x01,0x01,0x00,0x00,0x00,0x00,0x00,    // 1...8
            'S','p','1',' ','A','r','t',':',                            // 9...16
            0x00,'I','m','p','u','l','s',0x00,                          // 17...24
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,                    // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                     // 33...40
        },

        // INDEX_CTRL 1.EX.5 - Impulszeit Doppelspulenantrieb 1 (Rohwert, Einheit x10ms)

        {   KANAL_ART_TIMING_BASE + 1,0x02,0x00,0x01,0x00,0xfa,0x00,0x00, // 1...8
            'S','p','1',' ','x','1','0','m',                            // 9...16
            's',':',0x00,'1',0x00,'2','5','0',                          // 17...24
            0x00,'#',0x00,0x00,0x00,0x00,0x00,0x00,                     // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                     // 33...40
        },

        // INDEX_CTRL 1.EX.6 - Art der Aktoren Doppelspulenantrieb 2 (Auswahlliste mit genau 1
        // Punkt, ReadOnly - Platzhalter, siehe INDEX_CTRL 1.EX.4)

        {   KANAL_ART_TIMING_BASE + 2,0x01,0x01,0x00,0x00,0x00,0x00,0x00, // 1...8
            'S','p','2',' ','A','r','t',':',                            // 9...16
            0x00,'I','m','p','u','l','s',0x00,                          // 17...24
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,                    // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                     // 33...40
        },

        // INDEX_CTRL 1.EX.7 - Impulszeit Doppelspulenantrieb 2 (Rohwert, Einheit x10ms)

        {   KANAL_ART_TIMING_BASE + 3,0x02,0x00,0x01,0x00,0xfa,0x00,0x00, // 1...8
            'S','p','2',' ','x','1','0','m',                            // 9...16
            's',':',0x00,'1',0x00,'2','5','0',                          // 17...24
            0x00,'#',0x00,0x00,0x00,0x00,0x00,0x00,                     // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                     // 33...40
        },

        // INDEX_CTRL 1.EX.8 - Art der Aktoren Doppelspulenantrieb 3 (Auswahlliste mit genau 1
        // Punkt, ReadOnly - Platzhalter, siehe INDEX_CTRL 1.EX.4)

        {   KANAL_ART_TIMING_BASE + 4,0x01,0x01,0x00,0x00,0x00,0x00,0x00, // 1...8
            'S','p','3',' ','A','r','t',':',                            // 9...16
            0x00,'I','m','p','u','l','s',0x00,                          // 17...24
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,                    // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                     // 33...40
        },

        // INDEX_CTRL 1.EX.9 - Impulszeit Doppelspulenantrieb 3 (Rohwert, Einheit x10ms)

        {   KANAL_ART_TIMING_BASE + 5,0x02,0x00,0x01,0x00,0xfa,0x00,0x00, // 1...8
            'S','p','3',' ','x','1','0','m',                            // 9...16
            's',':',0x00,'1',0x00,'2','5','0',                          // 17...24
            0x00,'#',0x00,0x00,0x00,0x00,0x00,0x00,                     // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                     // 33...40
        },

        // INDEX_CTRL 1.EX.10 - Art der Aktoren Doppelspulenantrieb 4 (Auswahlliste mit genau 1
        // Punkt, ReadOnly - Platzhalter, siehe INDEX_CTRL 1.EX.4)

        {   KANAL_ART_TIMING_BASE + 6,0x01,0x01,0x00,0x00,0x00,0x00,0x00, // 1...8
            'S','p','4',' ','A','r','t',':',                            // 9...16
            0x00,'I','m','p','u','l','s',0x00,                          // 17...24
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,                    // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                     // 33...40
        },

        // INDEX_CTRL 1.EX.11 - Impulszeit Doppelspulenantrieb 4 (Rohwert, Einheit x10ms)

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
    //* utils:       zugehoerige MbcUtils-Instanz                                                 *
    //* can:         zugehoeriger CAN-Kanal dieses Moduls (Basisklassen-Parameter can)            *
    //* bootStorage: zugehoerige MbcBootStorage-Instanz (Firmware-Update-Staging)                 *
    //* indicator:   Positionsanzeige-MbcMcp23x17 (muss vor beginModule() bereits begin()t sein)  *
    //* relais:      8 Relaisausgaenge, Index-Konvention siehe Klassenkommentar                   *
    //* stellpult:   8 Stellpult-Eingaenge, Index-Konvention siehe Klassenkommentar               *
    //* pinReset:    NUR auf AVR (ATmega644P) vorhanden, siehe MbcCom-Konstruktor                 *
    //*********************************************************************************************

    #if defined(__AVR__)

        Mbc91Com::Mbc91Com(MbcUtils &utils, MbcCan &can, MbcBootStorage &bootStorage,
                            MbcMcp23x17 &indicator, MbcDigitalOut *(&relais)[8],
                            MbcDigitalIn *(&stellpult)[8], uint8_t pinReset)
            : MbcCom(utils, can, bootStorage, pinReset,
                     MBC91_ANZMESSWERTEMODUL, MBC91_ANZKONFIGMODUL),
              _indicator(&indicator), _hasStellpult(true), _baseAddr(0), _zyklusstep(0),
              _relaisStrom(false), _pulseStartMs(0), _pulseMaxMs(0)
        {
            for (uint8_t index = 0; index < 8; index++) _relais[index]     = relais[index];
            for (uint8_t index = 0; index < 8; index++) _stellpult[index]  = stellpult[index];
            for (uint8_t index = 0; index < 8; index++) _posLed[index]     = nullptr;
            for (uint8_t index = 0; index < 4; index++) _bestueckung[index] = nullptr;
            for (uint8_t index = 0; index < 4; index++) _relaisTime[index] = 20;

            memcpy_P(_msgKonfigOk, initMsgKonfigOk, MBC_COM_MSG_LAENGE);
        }

        //*****************************************************************************************
        //* ZWEITER Konstruktor NUR fuer die aeltere ATmega644P-Hardware (16.10.02) - siehe       *
        //* Klassenkommentar/Mbc91Com.h                                                           *
        //*****************************************************************************************

        Mbc91Com::Mbc91Com(MbcUtils &utils, MbcCan &can, MbcBootStorage &bootStorage,
                            MbcDigitalOut *(&relais)[8], MbcDigitalOut *(&posLed)[8],
                            MbcDigitalIn *(&bestueckung)[4], uint8_t pinReset)
            : MbcCom(utils, can, bootStorage, pinReset,
                     MBC91_ANZMESSWERTEMODUL, MBC91_ANZKONFIGMODUL),
              _indicator(nullptr), _hasStellpult(false), _baseAddr(0), _zyklusstep(0),
              _relaisStrom(false), _pulseStartMs(0), _pulseMaxMs(0)
        {
            for (uint8_t index = 0; index < 8; index++) _relais[index]      = relais[index];
            for (uint8_t index = 0; index < 8; index++) _stellpult[index]   = nullptr;
            for (uint8_t index = 0; index < 8; index++) _posLed[index]      = posLed[index];
            for (uint8_t index = 0; index < 4; index++) _bestueckung[index] = bestueckung[index];
            for (uint8_t index = 0; index < 4; index++) _relaisTime[index]  = 20;

            memcpy_P(_msgKonfigOk, initMsgKonfigOk, MBC_COM_MSG_LAENGE);
        }

    #else

        Mbc91Com::Mbc91Com(MbcUtils &utils, MbcCan &can, MbcBootStorage &bootStorage,
                            MbcMcp23x17 &indicator, MbcDigitalOut *(&relais)[8],
                            MbcDigitalIn *(&stellpult)[8])
            : MbcCom(utils, can, bootStorage,
                     MBC91_ANZMESSWERTEMODUL, MBC91_ANZKONFIGMODUL),
              _indicator(&indicator), _hasStellpult(true), _baseAddr(0), _zyklusstep(0),
              _relaisStrom(false), _pulseStartMs(0), _pulseMaxMs(0)
        {
            for (uint8_t index = 0; index < 8; index++) _relais[index]     = relais[index];
            for (uint8_t index = 0; index < 8; index++) _stellpult[index]  = stellpult[index];
            for (uint8_t index = 0; index < 8; index++) _posLed[index]     = nullptr;
            for (uint8_t index = 0; index < 4; index++) _bestueckung[index] = nullptr;
            for (uint8_t index = 0; index < 4; index++) _relaisTime[index] = 20;

            memcpy_P(_msgKonfigOk, initMsgKonfigOk, MBC_COM_MSG_LAENGE);
        }

    #endif

    //*********************************************************************************************
    //* Alle Relaisausgaenge ausschalten                                                          *
    //*********************************************************************************************

    void Mbc91Com::relaisAllOff()
    {
        for (uint8_t index = 0; index < 8; index++) _relais[index]->write(false);
    }

    //*********************************************************************************************
    //* Magnetartikel einschalten (Relaisspule bestromen, Puls-Timing starten)                    *
    //*********************************************************************************************
    //* index:      Relaisausgang, siehe Klassenkommentar                                         *
    //* timeUnits:  Impulszeit in 10-ms-Einheiten                                                 *
    //*********************************************************************************************

    void Mbc91Com::relaisOn(uint8_t index, uint8_t timeUnits)
    {
        _relais[index]->write(true);

        _pulseMaxMs   = (unsigned long) timeUnits * 10;
        _pulseStartMs = millis();
        _relaisStrom  = true;
    }

    //*********************************************************************************************
    //* Ist die Impulszeit der aktuell bestromten Spule abgelaufen? Zusaetzlich Zwangsabschaltung *
    //* nach MBC_91_AUS als Sicherheitsnetz gegen nicht endabgeschaltete Magnetartikel.           *
    //*********************************************************************************************

    bool Mbc91Com::pulseElapsed()
    {
        unsigned long elapsedMs = (unsigned long) (millis() - _pulseStartMs);
        unsigned long ausMs     = (unsigned long) MBC_91_AUS * 10;

        return (elapsedMs >= _pulseMaxMs) || (elapsedMs >= ausMs);
    }

    //*********************************************************************************************
    //* Magnetartikel ausschalten und, falls im Rueckmeldezyklus (zyklusstepAtStart > 1), den     *
    //* Abschluss des vorangegangenen Schaltkommandos an die CS2 melden                           *
    //*********************************************************************************************
    //* zyklusstepAtStart: Zyklusschritt zu Beginn des gerade beendeten Impulses                  *
    //*********************************************************************************************

    void Mbc91Com::relaisOffFinish(uint8_t zyklusstepAtStart)
    {
        relaisAllOff();

        if (zyklusstepAtStart > 1)
        {
            can_msg[MBC_COM_MSG_CS2_0x16][MBC_CAN_D5] = 0x00;

            sendMessage(can_msg[MBC_COM_MSG_CS2_0x16]);
        }

        _relaisStrom = false;
    }

    //*********************************************************************************************
    //* Aktuelle Aktorstellung (MBC_91_MM) auf die 8 Stellungs-LEDs ausgeben. Zwei Backends       *
    //* (siehe Klassenkommentar): ueber den Positionsanzeige-Expander (_indicator != nullptr) -   *
    //* Port GPIOA/GPIOB ist dabei plattformabhaengig (ATmega644P: GPIOA=LEDs, eigener nicht      *
    //* geteilter I2C-Baustein; RP2040/ESP32-C3 vertauscht: GPIOB=LEDs, da sich der Baustein dort *
    //* einen Chip mit einem LED-/Plug-Expander teilen kann, siehe Sketch) - oder ueber 8 direkte *
    //* GPIO-LEDs (_indicator == nullptr, aeltere ATmega644P-Hardware 16.10.02). Bit i von        *
    //* MBC_91_MM entspricht direkt LED-Pin i (kein Remap noetig - anders als beim                *
    //* Expander-Backend, dessen Bit-Reihenfolge invertiert verdrahtet ist).                      *
    //*********************************************************************************************

    void Mbc91Com::stateDisplay()
    {
        uint8_t mm = MBC_91_MM;

        if (_indicator != nullptr)
        {
            uint8_t stellung = 0;

            if (mm & 0x01) stellung |= 0x80;
            if (mm & 0x02) stellung |= 0x40;
            if (mm & 0x04) stellung |= 0x20;
            if (mm & 0x08) stellung |= 0x10;
            if (mm & 0x10) stellung |= 0x08;
            if (mm & 0x20) stellung |= 0x04;
            if (mm & 0x40) stellung |= 0x02;
            if (mm & 0x80) stellung |= 0x01;

            #if defined(__AVR__)
                _indicator->writeGpioA(stellung);
            #else
                _indicator->writeGpioB(stellung);
            #endif
        }
        else
        {
            for (uint8_t index = 0; index < 8; index++)
            {
                _posLed[index]->write((mm & (1 << index)) != 0);
            }
        }
    }

    //*********************************************************************************************
    //* Gibt die Stellung der bistabilen Relais nochmal aus, um externe Magnetartikel und die     *
    //* Positionsanzeige nach dem Start zu synchronisieren (hilft, manuell verstellte             *
    //* Magnetartikel wieder in den Synchronbetrieb zu zwingen). Blockiert kurz je Antrieb, bis   *
    //* dessen Impulszeit abgelaufen ist - nur beim Start relevant.                               *
    //*********************************************************************************************
    //* Return: Decoder-Basisadresse                                                              *
    //*********************************************************************************************

    uint16_t Mbc91Com::relaisSync()
    {
        for (uint8_t relais = 0; relais < 4; relais++)
        {
            uint8_t stellung = (MBC_91_MM & (0x03 << (relais * 2))) >> (relais * 2);
            if (stellung == 2) stellung = 0;

            uint8_t relaisSpule = relais * 2 + stellung;

            relaisOn(relaisSpule, _relaisTime[relais]);

            while (!pulseElapsed());

            relaisOffFinish(0);
        }

        stateDisplay();

        return (uint16_t) (((uint16_t) MBC_91_MM_H << 8) | (uint16_t) MBC_91_MM_L);
    }

    //*********************************************************************************************
    //* Stellpult auswerten: loest bei gedruecktem Taster denselben Schaltvorgang wie ein         *
    //* CS2-Kommando aus. Wertet alle 8 Eingaenge der Reihe nach aus (bei mehreren gleichzeitig   *
    //* gedrueckten Tastern gewinnt der zuletzt gepruefte).                                       *
    //*********************************************************************************************

    void Mbc91Com::switchboardCheck()
    {
        uint8_t relais    = 0;
        uint8_t direction = 0;
        bool    active    = false;

        for (uint8_t index = 0; index < 4; index++)
        {
            if (_stellpult[index * 2]->read())
            {
                relais    = index;
                direction = 0x00;
                active    = true;
            }

            if (_stellpult[index * 2 + 1]->read())
            {
                relais    = index;
                direction = 0x01;
                active    = true;
            }
        }

        if (!active) return;

        uint16_t outputAddr = (uint16_t) (_baseAddr + relais);

        can_msg[MBC_COM_MSG_CS2_0x16][MBC_CAN_D2] = (uint8_t) ((outputAddr & 0xff00) >> 8);
        can_msg[MBC_COM_MSG_CS2_0x16][MBC_CAN_D3] = (uint8_t) (outputAddr & 0x00ff);
        can_msg[MBC_COM_MSG_CS2_0x16][MBC_CAN_D5] = 0x01;
        can_msg[MBC_COM_MSG_CS2_0x16][MBC_CAN_D4] = direction;

        sendMessage(can_msg[MBC_COM_MSG_CS2_0x16]);

        uint8_t relaisSpule = relais * 2 + direction;

        if (direction == 0) direction = 2;

        MBC_91_MM = (MBC_91_MM & ~(0x03 << (relais * 2))) | (direction << (relais * 2));

        stateDisplay();
        eepromWriteParaExtern(EE_91_AKTOR);

        relaisOffFinish(_zyklusstep);
        relaisOn(relaisSpule, _relaisTime[relais]);
    }

    //*********************************************************************************************
    //* Eingehenden Schaltbefehl (CAN_ID 0x16, kein S88-Anstupsen) auswerten: bei gueltiger, zu   *
    //* diesem Decoder gehoerender Adresse den Magnetartikel schalten und die Bestaetigung an die *
    //* CSx/MS2 zuruecksenden.                                                                    *
    //*********************************************************************************************
    //* msg: empfangene CAN-Nachricht                                                             *
    //*********************************************************************************************

    void Mbc91Com::handleSwitchCommand(const uint8_t msg[MBC_CAN_MSG_LEN])
    {
        uint16_t addr      = (uint16_t) (((uint16_t) msg[MBC_CAN_D2] << 8)
                                        | (uint16_t) msg[MBC_CAN_D3]);
        uint8_t  direction = msg[MBC_CAN_D4];
        uint8_t  current   = msg[MBC_CAN_D5];

        if ((addr < _baseAddr) || (addr > (uint16_t) (_baseAddr + 3))) return;

        uint8_t relais      = (uint8_t) (addr - _baseAddr);
        uint8_t relaisSpule = relais * 2 + direction;

        if (current)
        {
            uint8_t dir = direction;
            if (dir == 0) dir = 2;

            MBC_91_MM = (MBC_91_MM & ~(0x03 << (relais * 2))) | (dir << (relais * 2));

            stateDisplay();
            eepromWriteParaExtern(EE_91_AKTOR);

            relaisOffFinish(_zyklusstep);
            relaisOn(relaisSpule, _relaisTime[relais]);
        }

        // Bestaetigung an die CSx/MS2 senden

        uint8_t ack[MBC_CAN_MSG_LEN];
        memcpy(ack, msg, MBC_CAN_MSG_LEN);

        ack[MBC_CAN_HASH_H] = can_msg[MBC_COM_MSG_CS2_0x16][MBC_CAN_HASH_H];
        ack[MBC_CAN_HASH_L] = can_msg[MBC_COM_MSG_CS2_0x16][MBC_CAN_HASH_L];
        ack[MBC_CAN_ID]     = 0x17;

        sendMessage(ack);
    }

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //+ Die 7 Erweiterungspunkte der Basisklasse (siehe MbcCom.h) - hier anpassen, um             +
    //+ mbc-91-spezifisches EEPROM-/CS2-Verhalten zu ergaenzen                                    +
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    //*********************************************************************************************
    //* Modulspezifische Parameterdaten aus dem EEPROM lesen                                      *
    //* - hier: Modulparameter aus der EEPROM-Zusatzregion laden (mit Defaultwerten fuer den      *
    //* unbeschriebenen Zustand, 0xff-Sentinel). Seedet ausserdem das Systemarray (MBC_91_TIO)    *
    //* aus _relaisTime[], damit dieses ab dem                                                    *
    //* Start ein korrektes Abbild ist - sowohl der rohe PC_ARRAY_DATA-Schreibweg als auch der    *
    //* neue Impulszeit-Konfigkanal (siehe cs2IndexParameterExtern()) schreiben dort hinein,      *
    //* bevor eepromWriteParaExtern(EE_91_MM_TIMING) alle 4 Werte persistiert.                    *
    //*********************************************************************************************

    void Mbc91Com::eepromReadParaExtern()
    {
        uint8_t buf[9];

        _utils->eepromReadExtra(0, buf, 9);

        for (uint8_t index = 0; index < 4; index++)
        {
            _relaisTime[index] = (buf[EEX_TIMING + index] == 0xff) ? 20 : buf[EEX_TIMING + index];

            *(&MBC_91_TIO + index) = _relaisTime[index];
        }

        MBC_91_MM = (buf[EEX_AKTOR] == 0xff) ? 0b01010101 : buf[EEX_AKTOR];

        if (buf[EEX_MMADDR] == 0xff)
        {
            MBC_91_MM_H = 0x30;
            MBC_91_MM_L = 0x00;
        }
        else
        {
            MBC_91_MM_H = buf[EEX_MMADDR];
            MBC_91_MM_L = buf[EEX_MMADDR + 1];
        }

        MBC_91_AUS   = (buf[EEX_AUS] == 0xff)   ? 100 : buf[EEX_AUS];
        MBC_91_RUECK = (buf[EEX_RUECK] == 0xff) ? 0   : buf[EEX_RUECK];
    }

    //*********************************************************************************************
    //* Modulspezifischen Parameterblock in die EEPROM-Zusatzregion schreiben und die RAM-        *
    //* Arbeitswerte (Impulszeiten/Basisadresse) bei Bedarf neu uebernehmen. Bei EE_91_MM_TIMING  *
    //* wird _relaisTime[] VOR dem Persistieren aus dem Systemarray (MBC_91_TIO) aufgefrischt,    *
    //* nicht erst danach - sowohl der rohe PC_ARRAY_DATA-Schreibweg als auch der neue            *
    //* Impulszeit-Konfigkanal (siehe cs2IndexParameterExtern()) hinterlegen den neuen Wert dort  *
    //* VOR diesem Aufruf; wuerde man erst persistieren und danach auffrischen, landete im EEPROM *
    //* immer der alte statt des neuen Werts. Bei EE_91_MMAddr/_RUECK wird zusaetzlich, wenn      *
    //* passend, sofort ein Rueckmeldezyklus angestossen (siehe unten) - unabhaengig davon, ob    *
    //* die Aenderung ueber CS2 (cs2IndexParameterExtern()) oder ParaCenter kam, da beide Wege    *
    //* hier zusammenlaufen.                                                                      *
    //*********************************************************************************************
    //* block: Identifier des zu schreibenden Blocks (0x0A....)                                   *
    //*********************************************************************************************

    void Mbc91Com::eepromWriteParaExtern(uint8_t block)
    {
        uint8_t buf[4];

        if (block == EE_91_MM_TIMING)
        {
            // Bereichspruefung 1...250 fuer den ParaCenter-Weg (PC_ARRAY_DATA) - ausserhalb
            // liegende Werte werden auf die jeweils naechste Grenze geklemmt statt verworfen;
            // der CS2-Konfigkanal validiert bereits vorher in cs2IndexParameterExtern() und
            // lehnt ungueltige Werte dort ab

            for (uint8_t index = 0; index < 4; index++)
            {
                uint8_t wert = *(&MBC_91_TIO + index);

                if (wert < 1)   wert = 1;
                if (wert > 250) wert = 250;

                *(&MBC_91_TIO + index) = wert;

                _relaisTime[index] = wert;
            }
        }

        switch (block)
        {
            case EE_91_MM_TIMING:

                for (uint8_t index = 0; index < 4; index++) buf[index] = _relaisTime[index];
                _utils->eepromWriteExtra(EEX_TIMING, buf, 4);

            break;

            case EE_91_AKTOR:

                buf[0] = MBC_91_MM;
                _utils->eepromWriteExtra(EEX_AKTOR, buf, 1);

            break;

            case EE_91_MMAddr:

                buf[0] = MBC_91_MM_H;
                buf[1] = MBC_91_MM_L;
                _utils->eepromWriteExtra(EEX_MMADDR, buf, 2);

            break;

            case EE_91_AUS:

                // Bereichspruefung 0...200 fuer den ParaCenter-Weg (PC_ARRAY_DATA) - ein zu
                // hoher Wert wird auf die obere Grenze geklemmt statt verworfen; der
                // CS2-Konfigkanal validiert bereits vorher in cs2IndexParameterExtern() und
                // lehnt ungueltige Werte dort ab

                if (MBC_91_AUS > 200) MBC_91_AUS = 200;

                buf[0] = MBC_91_AUS;
                _utils->eepromWriteExtra(EEX_AUS, buf, 1);

            break;

            case EE_91_RUECK:
            {
                // Alten, noch persistierten Zustand vor dem Ueberschreiben lesen, um den
                // disabled->enabled-Uebergang zu erkennen (siehe unten) - MBC_91_RUECK traegt
                // hier bereits den NEUEN Wert (vom Aufrufer vorher gesetzt)

                uint8_t alt;
                _utils->eepromReadExtra(EEX_RUECK, &alt, 1);
                bool warAktiviert = (alt != 0);

                buf[0] = MBC_91_RUECK;
                _utils->eepromWriteExtra(EEX_RUECK, buf, 1);

                // Rueckmeldung wurde soeben aktiviert: Rueckmeldeprozess sofort anstossen,
                // wenn gerade keiner laeuft

                if (!warAktiviert && MBC_91_RUECK && (_zyklusstep == 0)) _zyklusstep = 1;
            }
            break;
        }

        if (block == EE_91_MMAddr)
        {
            _baseAddr = (uint16_t) (((uint16_t) MBC_91_MM_H << 8) | (uint16_t) MBC_91_MM_L);

            // MM-Addr geaendert: Rueckmeldeprozess sofort anstossen, wenn die Rueckmeldung
            // aktiviert ist und gerade keiner laeuft

            if (MBC_91_RUECK && (_zyklusstep == 0)) _zyklusstep = 1;
        }
    }

    //*********************************************************************************************
    //* Modulspezifische Parameterdaten im EEPROM loeschen - nicht gebraucht, nichts zu tun       *
    //*********************************************************************************************

    void Mbc91Com::eepromEraseParaExtern()
    {
    }

    //*********************************************************************************************
    //* Traegt GUiD/HASH in die Konfig-Bestaetigungsnachricht _msgKonfigOk ein - erst durch den   *
    //* neuen MM-Addr-Konfigkanal gebraucht                                                       *
    //*********************************************************************************************

    void Mbc91Com::hashInitExtern()
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
    //* Softwareversion fuer CS2-Ping-Antwort ueberschreiben - mbc-91 simuliert keine fremde      *
    //* SW-Version, nichts zu tun                                                                 *
    //*********************************************************************************************

    void Mbc91Com::swInitExtern()
    {
    }

    //*********************************************************************************************
    //* Modulspezifischen Messwert an die CS2 senden - mbc-91 hat keine (ANZMESSWERTEMODUL=0),    *
    //* nichts zu tun                                                                             *
    //*********************************************************************************************
    //* kanal: Messkanalnummer                                                                    *
    //*********************************************************************************************

    void Mbc91Com::cs2IndexMesswerteExtern(uint8_t kanal)
    {
        (void) kanal;
    }

    //*********************************************************************************************
    //* Modulspezifischen Parameterwert von der CS2 uebernehmen - msg[D5]==KANAL_MODNR: neue      *
    //* Modulnummer (0...255, CS2-seitiges Label "MM-Addr"). Die CSx-GUI kann keine Hex-Eingabe,  *
    //* daher wird nicht die Basisadresse selbst, sondern eine einfache Modulnummer               *
    //* gemeldet/uebernommen und intern in MMADDR_BASE + 4*Modulnummer umgerechnet. Ein Wert >    *
    //* 255 bleibt ohne Wirkung (_baseAddr/EEPROM unveraendert) und wird per FALSE quittiert -    *
    //* siehe "System Status"-Antwortformat des CS2-Protokolls (D6 = TRUE/FALSE).                 *
    //* msg[D5]==KANAL_AUS: neue Auto-Aus-Zeit (Rohwert 0...200, Einheit x10ms) uebernehmen und   *
    //* persistieren; ein Wert ausserhalb 0...200 bleibt ohne Wirkung und wird per FALSE          *
    //* quittiert - nur der ParaCenter-Weg (PC_ARRAY_DATA, siehe eepromWriteParaExtern()) klemmt  *
    //* Auto-Aus stattdessen auf den gueltigen Bereich. msg[D5]==KANAL_RUECK: neue                *
    //* Zwangsrueckmeldung-Auswahl (Auswahlliste 0=disabled/1=enabled) uebernehmen und            *
    //* persistieren; ein Wert > 1 bleibt ohne Wirkung und wird per FALSE quittiert. Die 8        *
    //* interleaved Kanaele ab KANAL_ART_TIMING_BASE (Art Sp1, Timing Sp1, Art Sp2, ...): nur die *
    //* Timing-Kanaele (ungerade Offset) uebernehmen eine neue Impulszeit (Rohwert 1...250,       *
    //* Einheit x10ms); ein Wert ausserhalb 1...250 bleibt ohne Wirkung und wird per FALSE        *
    //* quittiert - nur der ParaCenter-Weg (PC_ARRAY_DATA) klemmt stattdessen auf den gueltigen   *
    //* Bereich (gleiche Politik wie Auto-Aus). Gueltige Werte werden ins Systemarray             *
    //* (MBC_91_TIO) geschrieben, bevor eepromWriteParaExtern(EE_91_MM_TIMING) alle 4 Werte       *
    //* persistiert und _relaisTime[] auffrischt. Die Art-Kanaele (gerade Offset) werden bewusst  *
    //* NICHT verarbeitet (ReadOnly, siehe Klassenkommentar).                                     *
    //*********************************************************************************************
    //* msg: CAN-Nachricht (msg[D5]==KANAL_MODNR/KANAL_AUS/KANAL_RUECK/                           *
    //* KANAL_ART_TIMING_BASE+0..7, msg[D6..D7]=neuer Wert)                                       *
    //*********************************************************************************************

    void Mbc91Com::cs2IndexParameterExtern(const uint8_t msg[MBC_COM_MSG_LAENGE])
    {
        uint8_t  kanal = msg[MBC_CAN_D5];
        uint16_t wert  = (uint16_t) (((uint16_t) msg[MBC_CAN_D6] << 8) | (uint16_t) msg[MBC_CAN_D7]);

        if (kanal == KANAL_MODNR)
        {
            bool gueltig = (wert <= MODNR_MAX);

            if (gueltig)
            {
                uint16_t addr    = (uint16_t) (MMADDR_BASE + 4 * wert);
                uint16_t altAddr = (uint16_t) (((uint16_t) MBC_91_MM_H << 8) | MBC_91_MM_L);

                if (altAddr != addr)
                {
                    MBC_91_MM_H = (uint8_t) (addr >> 8);
                    MBC_91_MM_L = (uint8_t) (addr & 0x00ff);

                    eepromWriteParaExtern(EE_91_MMAddr);   // persistiert + aktualisiert _baseAddr
                }
            }

            _msgKonfigOk[MBC_CAN_D5] = KANAL_MODNR;
            _msgKonfigOk[MBC_CAN_D6] = gueltig ? 0x01 : 0x00;
        }
        else if (kanal == KANAL_AUS)
        {
            bool gueltig = (wert <= 200);

            if (gueltig && (MBC_91_AUS != (uint8_t) wert))
            {
                MBC_91_AUS = (uint8_t) wert;
                eepromWriteParaExtern(EE_91_AUS);
            }

            _msgKonfigOk[MBC_CAN_D5] = KANAL_AUS;
            _msgKonfigOk[MBC_CAN_D6] = gueltig ? 0x01 : 0x00;
        }
        else if (kanal == KANAL_RUECK)
        {
            bool gueltig = (wert <= 1);

            if (gueltig && (MBC_91_RUECK != (uint8_t) wert))
            {
                MBC_91_RUECK = (uint8_t) wert;
                eepromWriteParaExtern(EE_91_RUECK);
            }

            _msgKonfigOk[MBC_CAN_D5] = KANAL_RUECK;
            _msgKonfigOk[MBC_CAN_D6] = gueltig ? 0x01 : 0x00;
        }
        else if ((kanal >= KANAL_ART_TIMING_BASE) && (kanal < (uint8_t) (KANAL_ART_TIMING_BASE + 8))
                 && (((kanal - KANAL_ART_TIMING_BASE) & 0x01) != 0))
        {
            // Nur die Timing-Kanaele (ungerade Offset) verarbeiten - die Art-Kanaele (gerade
            // Offset) sind ReadOnly und fallen unten auf den Default-Zweig durch

            bool gueltig = (wert >= 1) && (wert <= 250);

            if (gueltig)
            {
                uint8_t idx = (uint8_t) ((kanal - KANAL_ART_TIMING_BASE) / 2);

                if (*(&MBC_91_TIO + idx) != (uint8_t) wert)
                {
                    *(&MBC_91_TIO + idx) = (uint8_t) wert;

                    eepromWriteParaExtern(EE_91_MM_TIMING);   // persistiert alle 4 + synchronisiert
                                                               // _relaisTime[]/CS2-Anzeige
                }
            }

            _msgKonfigOk[MBC_CAN_D5] = kanal;
            _msgKonfigOk[MBC_CAN_D6] = gueltig ? 0x01 : 0x00;
        }
        else
        {
            // Art-Kanaele (gerade Offset ab KANAL_ART_TIMING_BASE) landen bewusst hier:
            // ReadOnly, siehe Klassenkommentar - ein Aenderungsversuch bleibt ohne
            // Reaktion/Bestaetigung

            return;
        }

        sendMessage(_msgKonfigOk);
    }

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //+ Zusaetzliche Erweiterungspunkte (siehe Kopfkommentar von MbcCom.h)                        +
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    //*********************************************************************************************
    //* Feste CS2-Geraetekennung - mbc-91 hat keine, die Kennung kommt vom Parametriercenter      *
    //*********************************************************************************************
    //* knH/knL: unveraendert                                                                     *
    //*********************************************************************************************
    //* Return: false (keine feste Kennung)                                                       *
    //*********************************************************************************************

    bool Mbc91Com::kennerFixedExtern(uint8_t &knH, uint8_t &knL)
    {
        (void) knH;
        (void) knL;
        return false;
    }

    //*********************************************************************************************
    //* Zusaetzlicher Versandweg fuer ausgehende Nachrichten der Basisklasse - mbc-91 hat keinen, *
    //* nichts zu tun                                                                             *
    //*********************************************************************************************
    //* msg: Nachrichtenarray                                                                     *
    //*********************************************************************************************

    void Mbc91Com::sendMessageExtern(const uint8_t msg[MBC_COM_MSG_LAENGE])
    {
        (void) msg;
    }

    //*********************************************************************************************
    //* Zeile aus dem modulspezifischen cs2_index_ex[]-Array liefern - mbc-91 hat elf (ROW_MODNR  *
    //* = MM-Addr, ROW_AUS = Auto-Aus, ROW_RUECK = Zwangsrueckmeldung, ROW_ART_TIMING_BASE+0..7 = *
    //* interleaved Art (ReadOnly)/Impulszeit je Doppelspulenantrieb 1...4), sonst nullptr        *
    //*********************************************************************************************
    //* laufendeNr: 0..ANZMESSWERTEMODUL+ANZKONFIGMODUL-1                                         *
    //*********************************************************************************************
    //* Return: nullptr (keine Erweiterung)                                                       *
    //*********************************************************************************************

    //*********************************************************************************************
    //* Baut die angeforderte Kanalzeile frisch in _cs2ScratchRow zusammen - mbc-91 hat elf       *
    //* Kanaele (ROW_MODNR = MM-Addr, ROW_AUS = Auto-Aus, ROW_RUECK = Zwangsrueckmeldung,         *
    //* ROW_ART_TIMING_BASE+0..7 = je Doppelspulenantrieb interleaved Art/Timing), sonst nullptr. *
    //* Vorlage (initCs2Ex[], PROGMEM) bleibt statisch, nur der aktuelle Wert wird bei jedem      *
    //* Aufruf frisch aus dem Systemarray gepatcht. Die Art-der-Aktoren-Zeilen (gerader Offset ab *
    //* ROW_ART_TIMING_BASE) werden dabei auf genau die per Bestueckung erkannte Art fixiert -    *
    //* echtes ReadOnly heisst hier: nicht zwei Optionen anbieten und eine davon per Software     *
    //* sperren, sondern von vornherein nur die eine tatsaechlich vorhandene Option eintragen     *
    //* (Anzahl der Auswahlpunkte = 1, "Jetzige Einstellung" = Index 0) - MBC_91_MM_ART wird      *
    //* nur einmalig beim Start aus der Bestueckungserkennung gesetzt (siehe beginModule()) und   *
    //* danach hier bei jeder Anfrage neu ausgewertet.                                            *
    //*********************************************************************************************
    //* laufendeNr: 0..ANZMESSWERTEMODUL+ANZKONFIGMODUL-1                                         *
    //*********************************************************************************************
    //* Return: Zeiger auf _cs2ScratchRow (nur bis zum naechsten Aufruf gueltig) oder nullptr     *
    //*********************************************************************************************

    const uint8_t* Mbc91Com::cs2IndexExtern(uint8_t laufendeNr)
    {
        if (laufendeNr >= (MBC91_ANZMESSWERTEMODUL + MBC91_ANZKONFIGMODUL)) return nullptr;

        memcpy_P(_cs2ScratchRow, initCs2Ex[laufendeNr], sizeof(_cs2ScratchRow));

        if (laufendeNr == ROW_MODNR)
        {
            uint16_t addr = (uint16_t) (((uint16_t) MBC_91_MM_H << 8) | (uint16_t) MBC_91_MM_L);

            _cs2ScratchRow[7] = (uint8_t) ((addr - MMADDR_BASE) / 4);
        }
        else if (laufendeNr == ROW_AUS)
        {
            _cs2ScratchRow[7] = MBC_91_AUS;
        }
        else if (laufendeNr == ROW_RUECK)
        {
            _cs2ScratchRow[RUECK_OFFSET] = MBC_91_RUECK;
        }
        else if ((laufendeNr >= ROW_ART_TIMING_BASE)
                 && (laufendeNr < (uint8_t) (ROW_ART_TIMING_BASE + 8)))
        {
            uint8_t offset = (uint8_t) (laufendeNr - ROW_ART_TIMING_BASE);
            uint8_t pair   = (uint8_t) (offset / 2);

            if ((offset & 0x01) == 0)
            {
                static const uint8_t textImpuls[MBC_COM_CS2PAKETE * 8 - 17] PROGMEM =
                    {'I','m','p','u','l','s',0x00,
                     0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

                static const uint8_t textWechsel[MBC_COM_CS2PAKETE * 8 - 17] PROGMEM =
                    {'W','e','c','h','s','e','l',0x00,
                     0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

                bool wechsel = (MBC_91_MM_ART & (0x03 << (pair * 2))) != 0;

                _cs2ScratchRow[2]          = 0x01;   // Anzahl der Auswahlpunkte: nur die eine
                                                      // erkannte Art
                _cs2ScratchRow[ART_OFFSET] = 0x00;   // Jetzige Einstellung: immer Index 0

                memcpy_P(&_cs2ScratchRow[17], wechsel ? textWechsel : textImpuls,
                         MBC_COM_CS2PAKETE * 8 - 17);
            }
            else
            {
                _cs2ScratchRow[7] = *(&MBC_91_TIO + pair);
            }
        }

        return _cs2ScratchRow;
    }

    //*********************************************************************************************
    //* Einmalig nach dem begin() der Basisklasse aufrufen: Relais in Ruhezustand bringen,        *
    //* Bestueckung vom Positionsanzeige-Expander einlesen, gespeicherte Parameter aus der        *
    //* EEPROM-Zusatzregion laden und die Magnetartikel/Positionsanzeige synchronisieren.         *
    //*********************************************************************************************

    void Mbc91Com::beginModule()
    {
        relaisAllOff();

        // Bestueckung (Art der Aktoren) lesen - zwei Backends (siehe Klassenkommentar): ueber
        // GPIOA/GPIOB des Positionsanzeige-Expanders (_indicator != nullptr, Quellport
        // plattformabhaengig: ATmega644P GPIOB Bit 0-3, RP2040/ESP32-C3 vertauscht GPIOA Bit
        // 3-6) oder ueber 4 direkte GPIO-Jumper (_indicator == nullptr, aeltere
        // ATmega644P-Hardware 16.10.02). Die Ziel-Bitspreizung in MBC_91_MM_ART
        // (0x01/0x04/0x10/0x40) ist in beiden Faellen gleich.

        MBC_91_MM_ART = 0;

        if (_indicator != nullptr)
        {
            #if defined(__AVR__)

                uint8_t gpiob = _indicator->readGpioB();

                if (gpiob & 0x08) MBC_91_MM_ART |= 0x40;
                if (gpiob & 0x04) MBC_91_MM_ART |= 0x10;
                if (gpiob & 0x02) MBC_91_MM_ART |= 0x04;
                if (gpiob & 0x01) MBC_91_MM_ART |= 0x01;

            #else

                uint8_t gpioa = _indicator->readGpioA();

                if (gpioa & 0x40) MBC_91_MM_ART |= 0x40;
                if (gpioa & 0x20) MBC_91_MM_ART |= 0x10;
                if (gpioa & 0x10) MBC_91_MM_ART |= 0x04;
                if (gpioa & 0x08) MBC_91_MM_ART |= 0x01;

            #endif
        }
        else
        {
            if (_bestueckung[0]->read()) MBC_91_MM_ART |= 0x40;
            if (_bestueckung[1]->read()) MBC_91_MM_ART |= 0x10;
            if (_bestueckung[2]->read()) MBC_91_MM_ART |= 0x04;
            if (_bestueckung[3]->read()) MBC_91_MM_ART |= 0x01;
        }

        // "Art der Aktoren"-Anzeige (ReadOnly, siehe Klassenkommentar) liest MBC_91_MM_ART jetzt
        // erst bei Bedarf direkt in cs2IndexExtern() aus - kein Vorab-Fixieren mehr noetig

        eepromReadParaExtern();

        _baseAddr = relaisSync();
    }

    //*********************************************************************************************
    //* Je loop()-Durchlauf aufrufen: Puls-Abschaltung, Zwangsrueckmeldezyklus, Stellpult-Abfrage *
    //* und CAN-Nachrichtenempfang/-Auswertung - ruft intern auch die geerbte check() auf.        *
    //*********************************************************************************************

    void Mbc91Com::update()
    {
        poll();

        // Impulszeit abgelaufen? Dann Relais aus, ggf. Rueckmeldezyklus abschliessen

        if (_relaisStrom && pulseElapsed())
        {
            relaisOffFinish(_zyklusstep);
        }

        // Rueckmeldezyklus aktiv? Dann naechsten Schritt abarbeiten

        if (!_relaisStrom && (_zyklusstep > 0))
        {
            if (_zyklusstep < 5)
            {
                uint8_t relais    = _zyklusstep - 1;
                uint8_t stellung  = (MBC_91_MM & (0x03 << (relais * 2))) >> (relais * 2);
                uint16_t addr     = (uint16_t) (_baseAddr + relais);
                uint8_t relaisSpule = 0;

                can_msg[MBC_COM_MSG_CS2_0x16][MBC_CAN_D2] = (uint8_t) ((addr & 0xff00) >> 8);
                can_msg[MBC_COM_MSG_CS2_0x16][MBC_CAN_D3] = (uint8_t) (addr & 0x00ff);
                can_msg[MBC_COM_MSG_CS2_0x16][MBC_CAN_D5] = 0x01;

                if (stellung == 0x01)
                {
                    can_msg[MBC_COM_MSG_CS2_0x16][MBC_CAN_D4] = 0x01;
                    relaisSpule = relais * 2 + 1;
                }

                if (stellung == 0x02)
                {
                    can_msg[MBC_COM_MSG_CS2_0x16][MBC_CAN_D4] = 0x00;
                    relaisSpule = relais * 2;
                }

                sendMessage(can_msg[MBC_COM_MSG_CS2_0x16]);

                relaisOn(relaisSpule, _relaisTime[relais]);

                _zyklusstep++;
            }
            else
            {
                _zyklusstep = 0;
            }
        }

        // Stellpult auswerten (nur wenn vorhanden - aeltere Hardware 16.10.02 hat keins, siehe
        // Klassenkommentar)

        if (_hasStellpult && !_relaisStrom) switchboardCheck();

        // CAN-Nachricht empfangen?

        uint8_t msg[MBC_CAN_MSG_LEN];

        if (_can->receive(msg))
        {
            check(msg);

            bool isSchaltbefehl = (msg[MBC_CAN_ID] == CAN_ID_ZUBEHOER_SCHALTEN)
                               && (msg[MBC_CAN_D2] != CAN_LOCID_MBCAN);

            if (isSchaltbefehl)
            {
                handleSwitchCommand(msg);
            }

            // Rueckmeldeaufforderung vom vorgelagerten LinkS88: Rueckmeldezyklus starten,
            // wenn gerade keine Spule aktiv ist, Rueckmeldung freigegeben und die
            // Basisadresse passt

            bool isZyklusAnstoss = (msg[MBC_CAN_D2] == CAN_LOCID_MBCAN)
                                && (msg[MBC_CAN_D3] == MD_ZYKLUS);

            if (isZyklusAnstoss && MBC_91_RUECK && !_relaisStrom)
            {
                uint16_t addr = (uint16_t) (((uint16_t) msg[MBC_CAN_D6] << 8)
                                           | (uint16_t) msg[MBC_CAN_D7]);

                if (addr == (uint16_t) (_baseAddr - 0x3000)) _zyklusstep = 1;
            }
        }
    }

//------ Ende Klasse Mbc91Com ---------------------------------------------------------------------

//###### Ende Quellcode Mbc91Com.cpp ##############################################################
