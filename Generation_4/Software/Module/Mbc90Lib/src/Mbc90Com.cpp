//*************************************************************************************************
//* Modul mbc-90: S88-Rueckmeldedecoder, Vollmodul 16 Kontakte (Mbc90Lib)                         *
//*************************************************************************************************
//* Datei: Mbc90Com.cpp                                                                           *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen:                                                                                  *
//* V1.0.0 - 14.08.2026 - Nullversion                                                             *
//*************************************************************************************************

//###### Start Quellcode Mbc90Com.cpp #############################################################

#include "Mbc90Com.h"

//------ Start interne Konstanten -----------------------------------------------------------------

    // CAN-Befehlsnummer der CS2-S88-Anfrage (Anfrage 0x22/Antwort 0x23 sind bereits als
    // MBC_COM_MSG_CS2_0x22_R-Template in der MbcCom-Basisklasse vorgefuellt, siehe MbcCom.cpp)

    static const uint8_t CAN_ID_CS2_S88_REQUEST = 0x22;

    // Modulspezifische EEPROM-Blockindizes (Kompatibilitaet zum Parametriercenter) - diese Werte
    // werden von externer PC-/CS2-Parametriersoftware direkt referenziert, siehe MbcCom.cpp
    // PC_ARRAY_DATA/MBC_COM_EEPROM)

    static const uint8_t EE_90_S88ADDR   = 0x0A;
    static const uint8_t EE_90_S88TIMING = 0x0B;

    // Byte-Layout der EEPROM-Zusatzregion (18 Byte, siehe MbcUtils::eepromReadExtra()/
    // eepromWriteExtra()) - unabhaengig von den EE_90_*-Blockindizes oben. KEIN Halbmodul-Byte
    // (mbc-90 ist selbst ein volles 16er-Modul), 16 Byte fuer die Integrationszeiten (1 je Kontakt)

    static const uint16_t EEX_S88ADDR   = 0;    // 2 Byte: S88Addr (H,L)
    static const uint16_t EEX_S88TIMING = 2;    // 16 Byte: Integrationszeit je Kontakt

    // Bit-Remap der Stellungsanzeige je PCF8574 (fuer beide Bausteine identisch) - bildet den
    // Kontakt-Index (0...7 innerhalb des jeweiligen Bausteins) auf die tatsaechliche
    // Ausgangsbitposition ab

    static const uint8_t DISPLAY_REMAP[8] = {0, 1, 2, 3, 7, 6, 5, 4};

    // CS2-Kanalnummern/Zeilenindizes der 17 modulspezifischen Konfigkanaele (S88-Modulnummer plus
    // 16 Integrationszeiten, siehe Klassenkommentar). Byte-Offset "Aktueller Wert Low" ist bei
    // allen Zeilen identisch 7, da jede Zeile ein in sich abgeschlossener 40-Byte-Block ist.

    static const uint8_t KANAL_S88MOD         = MBC_COM_ANZMESSWERTE + MBC_COM_ANZKONFIG + 1;  // 7
    static const uint8_t KANAL_S88TIMING_BASE = KANAL_S88MOD + 1;                          // 8..23
    static const uint8_t ROW_S88MOD           = 0;
    static const uint8_t ROW_S88TIMING_BASE   = 1;                                          // 1..16
    static const uint8_t CS2ROW_OFFSET_L      = 7;   // "Aktueller Wert Low" innerhalb jeder Zeile

    // Systemarray-Layout ab MBC_S_GENPURP (kompatibel zum Parametriercenter:
    // MBC_90_L_S88=4 + MBC_90_L_INTS88=16)

    #define MBC_90_S_S88     MBC_S_GENPURP
    #define MBC_90_S88_H     _utils->array()[MBC_90_S_S88]
    #define MBC_90_S88_L     _utils->array()[MBC_90_S_S88 + 1]
    #define MBC_90_RM_H      _utils->array()[MBC_90_S_S88 + 2]
    #define MBC_90_RM_L      _utils->array()[MBC_90_S_S88 + 3]
    #define MBC_90_S_INTS88  (MBC_90_S_S88 + 4)
    #define MBC_90_INTS88(i) _utils->array()[MBC_90_S_INTS88 + (i)]

//------ Ende interne Konstanten ------------------------------------------------------------------

//------ Start Klasse Mbc90Com --------------------------------------------------------------------

    //*********************************************************************************************
    //* Konstruktor zur Deklaration von Objektvariablen (initialisiert ausserdem die              *
    //* modulspezifische CS2-Konfigurationstabelle _cs2IndexEx[] und die                          *
    //* Konfig-Bestaetigungsnachricht _msgKonfigOk[])                                             *
    //*********************************************************************************************
    //* utils:       zugehoerige MbcUtils-Instanz                                                 *
    //* can:         zugehoeriger CAN-Kanal dieses Moduls (Basisklassen-Parameter can)            *
    //* bootStorage: zugehoerige MbcBootStorage-Instanz (Firmware-Update-Staging)                 *
    //* inputs:      16 Kontakteingaenge, Index-Konvention siehe Klassenkommentar                 *
    //* displayLow:  NUR AVR - PCF8574 Stellungsanzeige Kontakt 1-8 (Bausteinnummer 0)            *
    //* displayHigh: NUR AVR - PCF8574 Stellungsanzeige Kontakt 9-16 (Bausteinnummer 1)           *
    //* display:     NUR NICHT-AVR - MCP23S17 Stellungsanzeige (GPIOA=Kontakt 1-8,                *
    //*              GPIOB=Kontakt 9-16)                                                          *
    //* pinReset:    NUR auf AVR (ATmega644P) vorhanden, siehe MbcCom-Konstruktor                 *
    //*********************************************************************************************

    // PROGMEM: nur einmal hier beim Konstruieren gelesen

    static const uint8_t initCs2Ex[MBC90_ANZMESSWERTEMODUL + MBC90_ANZKONFIGMODUL]
                                   [MBC_COM_CS2PAKETE * 8] PROGMEM =
    {
        // INDEX_CTRL 1.EX.1 - S88-Modulnummer

        {   KANAL_S88MOD,0x02,0x00,0x00,0x00,0x5D,0x00,0x00,              // 1...8
            'S','8','8',' ','M','o','d','u',                              // 9...16
            'l',':',0x00,'0',0x00,'9','3',0x00,                           // 17...24
            '#',0x00,0x00,0x00,0x00,0x00,0x00,0x00,                       // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                       // 33...40
        },

        // INDEX_CTRL 1.EX.2..17 - Integrationszeit je Kontakt (K1..K16, Rohwert x10ms, 1...250).
        // Byte-Zaehlung geprueft per Python-Simulation des NUL-terminierten Feld-Parsings (siehe
        // [[project_mbcan_cs2_konfigkanal_pattern]] - KEIN Padding-0x00 vor dem ':' einfuegen, das
        // Feldende ergibt sich allein aus der Stringlaenge, nicht aus einer festen Spaltenbreite)

        // INDEX_CTRL 1.EX.2 - Integrationszeit Kontakt 1 (K1)

        {   KANAL_S88TIMING_BASE,0x02,0x00,0x01,0x00,0xFA,0x00,0x00,      // 1...8
            'K','1','x','1','0','m','s',':',                              // 9...16
            0x00,'1',0x00,'2','5','0',0x00,'#',                           // 17...24
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,                      // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                       // 33...40
        },

        // INDEX_CTRL 1.EX.3 - Integrationszeit Kontakt 2 (K2, Rohwert x10ms, 1...250)

        {   KANAL_S88TIMING_BASE + 1,0x02,0x00,0x01,0x00,0xFA,0x00,0x00,  // 1...8
            'K','2','x','1','0','m','s',':',                              // 9...16
            0x00,'1',0x00,'2','5','0',0x00,'#',                           // 17...24
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,                      // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                       // 33...40
        },

        // INDEX_CTRL 1.EX.4 - Integrationszeit Kontakt 3 (K3, Rohwert x10ms, 1...250)

        {   KANAL_S88TIMING_BASE + 2,0x02,0x00,0x01,0x00,0xFA,0x00,0x00,  // 1...8
            'K','3','x','1','0','m','s',':',                              // 9...16
            0x00,'1',0x00,'2','5','0',0x00,'#',                           // 17...24
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,                      // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                       // 33...40
        },

        // INDEX_CTRL 1.EX.5 - Integrationszeit Kontakt 4 (K4, Rohwert x10ms, 1...250)

        {   KANAL_S88TIMING_BASE + 3,0x02,0x00,0x01,0x00,0xFA,0x00,0x00,  // 1...8
            'K','4','x','1','0','m','s',':',                              // 9...16
            0x00,'1',0x00,'2','5','0',0x00,'#',                           // 17...24
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,                      // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                       // 33...40
        },

        // INDEX_CTRL 1.EX.6 - Integrationszeit Kontakt 5 (K5, Rohwert x10ms, 1...250)

        {   KANAL_S88TIMING_BASE + 4,0x02,0x00,0x01,0x00,0xFA,0x00,0x00,  // 1...8
            'K','5','x','1','0','m','s',':',                              // 9...16
            0x00,'1',0x00,'2','5','0',0x00,'#',                           // 17...24
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,                      // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                       // 33...40
        },

        // INDEX_CTRL 1.EX.7 - Integrationszeit Kontakt 6 (K6, Rohwert x10ms, 1...250)

        {   KANAL_S88TIMING_BASE + 5,0x02,0x00,0x01,0x00,0xFA,0x00,0x00,  // 1...8
            'K','6','x','1','0','m','s',':',                              // 9...16
            0x00,'1',0x00,'2','5','0',0x00,'#',                           // 17...24
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,                      // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                       // 33...40
        },

        // INDEX_CTRL 1.EX.8 - Integrationszeit Kontakt 7 (K7, Rohwert x10ms, 1...250)

        {   KANAL_S88TIMING_BASE + 6,0x02,0x00,0x01,0x00,0xFA,0x00,0x00,  // 1...8
            'K','7','x','1','0','m','s',':',                              // 9...16
            0x00,'1',0x00,'2','5','0',0x00,'#',                           // 17...24
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,                      // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                       // 33...40
        },

        // INDEX_CTRL 1.EX.9 - Integrationszeit Kontakt 8 (K8, Rohwert x10ms, 1...250)

        {   KANAL_S88TIMING_BASE + 7,0x02,0x00,0x01,0x00,0xFA,0x00,0x00,  // 1...8
            'K','8','x','1','0','m','s',':',                              // 9...16
            0x00,'1',0x00,'2','5','0',0x00,'#',                           // 17...24
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,                      // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                       // 33...40
        },

        // INDEX_CTRL 1.EX.10 - Integrationszeit Kontakt 9 (K9, Rohwert x10ms, 1...250)

        {   KANAL_S88TIMING_BASE + 8,0x02,0x00,0x01,0x00,0xFA,0x00,0x00,  // 1...8
            'K','9','x','1','0','m','s',':',                              // 9...16
            0x00,'1',0x00,'2','5','0',0x00,'#',                           // 17...24
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,                      // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                       // 33...40
        },

        // INDEX_CTRL 1.EX.11 - Integrationszeit Kontakt 10 (K10, Rohwert x10ms, 1...250)

        {   KANAL_S88TIMING_BASE + 9,0x02,0x00,0x01,0x00,0xFA,0x00,0x00,  // 1...8
            'K','1','0','x','1','0','m','s',                              // 9...16
            ':',0x00,'1',0x00,'2','5','0',0x00,                           // 17...24
            '#',0x00,0x00,0x00,0x00,0x00,0x00,0x00,                       // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                       // 33...40
        },

        // INDEX_CTRL 1.EX.12 - Integrationszeit Kontakt 11 (K11, Rohwert x10ms, 1...250)

        {   KANAL_S88TIMING_BASE + 10,0x02,0x00,0x01,0x00,0xFA,0x00,0x00, // 1...8
            'K','1','1','x','1','0','m','s',                              // 9...16
            ':',0x00,'1',0x00,'2','5','0',0x00,                           // 17...24
            '#',0x00,0x00,0x00,0x00,0x00,0x00,0x00,                       // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                       // 33...40
        },

        // INDEX_CTRL 1.EX.13 - Integrationszeit Kontakt 12 (K12, Rohwert x10ms, 1...250)

        {   KANAL_S88TIMING_BASE + 11,0x02,0x00,0x01,0x00,0xFA,0x00,0x00, // 1...8
            'K','1','2','x','1','0','m','s',                              // 9...16
            ':',0x00,'1',0x00,'2','5','0',0x00,                           // 17...24
            '#',0x00,0x00,0x00,0x00,0x00,0x00,0x00,                       // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                       // 33...40
        },

        // INDEX_CTRL 1.EX.14 - Integrationszeit Kontakt 13 (K13, Rohwert x10ms, 1...250)

        {   KANAL_S88TIMING_BASE + 12,0x02,0x00,0x01,0x00,0xFA,0x00,0x00, // 1...8
            'K','1','3','x','1','0','m','s',                              // 9...16
            ':',0x00,'1',0x00,'2','5','0',0x00,                           // 17...24
            '#',0x00,0x00,0x00,0x00,0x00,0x00,0x00,                       // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                       // 33...40
        },

        // INDEX_CTRL 1.EX.15 - Integrationszeit Kontakt 14 (K14, Rohwert x10ms, 1...250)

        {   KANAL_S88TIMING_BASE + 13,0x02,0x00,0x01,0x00,0xFA,0x00,0x00, // 1...8
            'K','1','4','x','1','0','m','s',                              // 9...16
            ':',0x00,'1',0x00,'2','5','0',0x00,                           // 17...24
            '#',0x00,0x00,0x00,0x00,0x00,0x00,0x00,                       // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                       // 33...40
        },

        // INDEX_CTRL 1.EX.16 - Integrationszeit Kontakt 15 (K15, Rohwert x10ms, 1...250)

        {   KANAL_S88TIMING_BASE + 14,0x02,0x00,0x01,0x00,0xFA,0x00,0x00, // 1...8
            'K','1','5','x','1','0','m','s',                              // 9...16
            ':',0x00,'1',0x00,'2','5','0',0x00,                           // 17...24
            '#',0x00,0x00,0x00,0x00,0x00,0x00,0x00,                       // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                       // 33...40
        },

        // INDEX_CTRL 1.EX.17 - Integrationszeit Kontakt 16 (K16, Rohwert x10ms, 1...250)

        {   KANAL_S88TIMING_BASE + 15,0x02,0x00,0x01,0x00,0xFA,0x00,0x00, // 1...8
            'K','1','6','x','1','0','m','s',                              // 9...16
            ':',0x00,'1',0x00,'2','5','0',0x00,                           // 17...24
            '#',0x00,0x00,0x00,0x00,0x00,0x00,0x00,                       // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                       // 33...40
        }
    };

    static const uint8_t initMsgKonfigOk[MBC_COM_MSG_LAENGE] PROGMEM =
        {0x00,0x01,0x00,0x00,0x07,0x00,0x00,0x00,0x00,0x0b,0x00,0x01,0x00};

    #if defined(__AVR__)

        Mbc90Com::Mbc90Com(MbcUtils &utils, MbcCan &can, MbcBootStorage &bootStorage,
                            MbcDigitalIn *(&inputs)[16], MbcPcf8574 &displayLow,
                            MbcPcf8574 &displayHigh, uint8_t pinReset)
            : MbcCom(utils, can, bootStorage, pinReset,
                     MBC90_ANZMESSWERTEMODUL, MBC90_ANZKONFIGMODUL)
        {
            for (uint8_t index = 0; index < 16; index++) _inputs[index]    = inputs[index];
            for (uint8_t index = 0; index < 16; index++) _pinActive[index] = false;

            _displayLow  = &displayLow;
            _displayHigh = &displayHigh;

            _displayLowPrev  = 0;
            _displayHighPrev = 0;

            memcpy_P(_cs2IndexEx, initCs2Ex, sizeof(_cs2IndexEx));
            memcpy_P(_msgKonfigOk, initMsgKonfigOk, MBC_COM_MSG_LAENGE);
        }

    #else

        Mbc90Com::Mbc90Com(MbcUtils &utils, MbcCan &can, MbcBootStorage &bootStorage,
                            MbcDigitalIn *(&inputs)[16], MbcMcp23x17 &display)
            : MbcCom(utils, can, bootStorage,
                     MBC90_ANZMESSWERTEMODUL, MBC90_ANZKONFIGMODUL)
        {
            for (uint8_t index = 0; index < 16; index++) _inputs[index]    = inputs[index];
            for (uint8_t index = 0; index < 16; index++) _pinActive[index] = false;

            _display = &display;

            _displayLowPrev  = 0;
            _displayHighPrev = 0;

            memcpy_P(_cs2IndexEx, initCs2Ex, sizeof(_cs2IndexEx));
            memcpy_P(_msgKonfigOk, initMsgKonfigOk, MBC_COM_MSG_LAENGE);
        }

    #endif

    //*********************************************************************************************
    //* Busnummer (1/2/3, oder 0 fuer eine (noch) unkonfigurierte S88-Adresse) aus dem            *
    //* niederwertigen Byte der S88Addr bestimmen - der 0-Fall wird aktiv ausgewertet, nicht auf  *
    //* 1 zurueckgefallen.                                                                        *
    //*********************************************************************************************
    //* s88AddrLow: niederwertiges Byte der S88Addr                                               *
    //*********************************************************************************************
    //* Return: Busnummer 0 (unkonfiguriert), 1, 2 oder 3                                         *
    //*********************************************************************************************

    uint16_t Mbc90Com::busNrFromS88AddrLow(uint8_t s88AddrLow)
    {
        if (s88AddrLow == 0) return 0;
        if (s88AddrLow < 32) return 1;
        if (s88AddrLow < 63) return 2;

        return 3;
    }

    //*********************************************************************************************
    //* Ereignismeldung fuer einen Kontakt versenden: immer als Broadcast an das PC-Interface,    *
    //* zusaetzlich als CS2-Meldung wenn das Modul initialisiert und mit der CS2 verbunden ist -  *
    //* ohne Halbmodul-Offset (mbc-90 hat kein Halbmodul), die Basisadresse nutzt Shift 4 statt 3 *
    //* (16 statt 8 Kontakte je Modul)                                                            *
    //*********************************************************************************************
    //* index:        S88-Kontakt 0..15                                                           *
    //* eventActive:  0x01 = Kontakt ausgeloest, 0x00 = Kontakt wieder frei                       *
    //*********************************************************************************************

    void Mbc90Com::sendS88Event(uint8_t index, uint8_t eventActive)
    {
        can_msg[MBC_COM_MSG_S88][MBC_CAN_D4] = MBC_90_S88_H;
        can_msg[MBC_COM_MSG_S88][MBC_CAN_D5] = MBC_90_S88_L;
        can_msg[MBC_COM_MSG_S88][MBC_CAN_D6] = index;
        can_msg[MBC_COM_MSG_S88][MBC_CAN_D7] = eventActive;

        sendMessage(can_msg[MBC_COM_MSG_S88]);

        if (!initDone() || !csConnected()) return;

        uint16_t busNr    = busNrFromS88AddrLow(MBC_90_S88_L);
        uint16_t baseAddr = 0;

        if (busNr != 0)
        {
            baseAddr = (uint16_t) (((((uint16_t) MBC_90_S88_L - 31UL * (busNr - 1UL)) - 1UL)) << 4);
        }

        uint16_t s88PinNr = (uint16_t) (1000UL * busNr + baseAddr + index + 1);

        unsigned long elapsedMs = (unsigned long) (millis() - _pinLastChangeMs[index]);
        uint16_t      elapsed16 = (elapsedMs > 0xffffUL) ? 0xffff : (uint16_t) elapsedMs;

        can_msg[MBC_COM_MSG_CS2_0x22_R][MBC_CAN_D2] = (uint8_t) ((s88PinNr & 0xff00) >> 8);
        can_msg[MBC_COM_MSG_CS2_0x22_R][MBC_CAN_D3] = (uint8_t) (s88PinNr & 0x00ff);
        can_msg[MBC_COM_MSG_CS2_0x22_R][MBC_CAN_D4] = eventActive ? 0x00 : 0x01;
        can_msg[MBC_COM_MSG_CS2_0x22_R][MBC_CAN_D5] = eventActive ? 0x01 : 0x00;
        can_msg[MBC_COM_MSG_CS2_0x22_R][MBC_CAN_D6] = (uint8_t) ((elapsed16 & 0xff00) >> 8);
        can_msg[MBC_COM_MSG_CS2_0x22_R][MBC_CAN_D7] = (uint8_t) (elapsed16 & 0x00ff);

        sendMessage(can_msg[MBC_COM_MSG_CS2_0x22_R]);

        _pinLastChangeMs[index] = millis();
    }

    //*********************************************************************************************
    //* Kontakte einlesen und entprellen (millis()-basierter Integrator, nicht interrupt-         *
    //* getrieben), Rueckmeldestatus und Stellungsanzeige aktualisieren.                          *
    //*********************************************************************************************

    void Mbc90Com::s88PinsCheck()
    {
        uint8_t rmLow = 0, rmHigh = 0;
        uint8_t dispLow = 0, dispHigh = 0;

        for (uint8_t index = 0; index < 16; index++)
        {
            bool active = _inputs[index]->read();   // true = Kontakt aktuell ausgeloest

            // Kontakt wieder frei, Integrator laeuft noch (Entprellung/Haltezeit) - erst nach
            // Ablauf der Integrationszeit endgueltig als beendet melden

            if (!active && _pinActive[index])
            {
                unsigned long integrationMs = (unsigned long) MBC_90_INTS88(index) * 10;
                unsigned long elapsedMs     = (unsigned long) (millis() - _pinStartMs[index]);

                if (elapsedMs >= integrationMs)
                {
                    _pinActive[index] = false;
                    sendS88Event(index, 0x00);
                }
            }

            // Kontakt aktuell ausgeloest

            if (active)
            {
                if (!_pinActive[index])
                {
                    _pinStartMs[index] = millis();
                    _pinActive[index]  = true;
                    sendS88Event(index, 0x01);
                }
                else
                {
                    _pinStartMs[index] = millis();   // Haltezeit verlaengern
                }
            }

            // Rueckmeldestatus (Systemarray) UND Stellungsanzeige (PCF8574) aus dem aktuellen
            // Kontaktzustand ableiten - Rueckmeldestatus bewusst NICHT invertiert, Bit i = Kontakt
            // i+1, gegen das Parametriercenter verifiziert. Stellungsanzeige dagegen mit dem
            // DISPLAY_REMAP-Bit-Remap UND (nach der Schleife) invertiert, da der PCF8574-Ausgang
            // aktiv-LOW ist

            uint8_t level = _pinActive[index] ? 1 : 0;

            if (index < 8)
            {
                rmLow   |= (uint8_t) (level << index);
                dispLow |= (uint8_t) (level << DISPLAY_REMAP[index]);
            }
            else
            {
                rmHigh   |= (uint8_t) (level << (index - 8));
                dispHigh |= (uint8_t) (level << DISPLAY_REMAP[index - 8]);
            }
        }

        MBC_90_RM_L = rmLow;
        MBC_90_RM_H = rmHigh;

        dispLow  = (uint8_t) ~dispLow;
        dispHigh = (uint8_t) ~dispHigh;

        // Nur bei Aenderung tatsaechlich schreiben - Backend (I2C-PCF8574 je Haelfte auf AVR,
        // GPIOA/GPIOB desselben MCP23S17 sonst) ist hier gekapselt

        #if defined(__AVR__)

            if (dispLow != _displayLowPrev)
            {
                _displayLow->write(dispLow);
                _displayLowPrev = dispLow;
            }

            if (dispHigh != _displayHighPrev)
            {
                _displayHigh->write(dispHigh);
                _displayHighPrev = dispHigh;
            }

        #else

            if (dispLow != _displayLowPrev)
            {
                _display->writeGpioA(dispLow);
                _displayLowPrev = dispLow;
            }

            if (dispHigh != _displayHighPrev)
            {
                _display->writeGpioB(dispHigh);
                _displayHighPrev = dispHigh;
            }

        #endif
    }

    //*********************************************************************************************
    //* CS2-S88-Statusanfrage beantworten - nur DLC==4 (Statusabfrage) wird verarbeitet, andere   *
    //* DLC-Werte werden ignoriert. Grenzwerte (500/1500/2500) und die Pin-Maskierung (& 0x0f, 16 *
    //* moegliche Kontakte) sind direkt auf das Vollmodul zugeschnitten, kein Halbmodul-Remapping *
    //* noetig                                                                                    *
    //*********************************************************************************************
    //* msg: empfangene CAN-Nachricht                                                             *
    //*********************************************************************************************

    void Mbc90Com::handleS88Request(const uint8_t msg[MBC_CAN_MSG_LEN])
    {
        if ((msg[MBC_CAN_ID] != CAN_ID_CS2_S88_REQUEST) || (msg[MBC_CAN_DLC] != 0x04)) return;
        if (!initDone() || !csConnected()) return;

        uint16_t busNr     = busNrFromS88AddrLow(MBC_90_S88_L);
        uint16_t baseModul = MBC_90_S88_L;

        if (busNr == 2) baseModul -= 31;
        if (busNr == 3) baseModul -= 62;

        uint16_t msgAddr = (uint16_t) (((uint16_t) msg[MBC_CAN_D2] << 8) | (uint16_t) msg[MBC_CAN_D3]);

        if ((msgAddr > 500) && (busNr == 0)) return;
        if (((msgAddr < 500) || (msgAddr > 1500)) && (busNr == 1)) return;
        if (((msgAddr < 1500) || (msgAddr > 2500)) && (busNr == 2)) return;
        if ((msgAddr < 2500) && (busNr == 3)) return;

        if (msgAddr > 500)
        {
            uint16_t reduced = (uint16_t) (msgAddr - 1000UL * busNr);
            reduced = (uint16_t) (((reduced - 1UL) >> 4) + 1UL);

            if (reduced != baseModul) return;
        }

        // pin ist durch die Maske "& 0x0f" bereits zwingend 0...15, kein zusaetzlicher
        // Bereichscheck noetig

        uint16_t pin = (uint16_t) ((msgAddr - (1000UL * busNr + 1UL)) & 0x0f);

        can_msg[MBC_COM_MSG_CS2_0x22_R][MBC_CAN_D2] = msg[MBC_CAN_D2];
        can_msg[MBC_COM_MSG_CS2_0x22_R][MBC_CAN_D3] = msg[MBC_CAN_D3];
        can_msg[MBC_COM_MSG_CS2_0x22_R][MBC_CAN_D4] = _pinActive[pin] ? 0x01 : 0x00;
        can_msg[MBC_COM_MSG_CS2_0x22_R][MBC_CAN_D5] = _pinActive[pin] ? 0x01 : 0x00;
        can_msg[MBC_COM_MSG_CS2_0x22_R][MBC_CAN_D6] = 0x00;
        can_msg[MBC_COM_MSG_CS2_0x22_R][MBC_CAN_D7] = 0x00;

        sendMessage(can_msg[MBC_COM_MSG_CS2_0x22_R]);
    }

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //+ Die 7 Erweiterungspunkte der Basisklasse (siehe MbcCom.h fuer die volle Beschreibung je   +
    //+ Hook) - hier anpassen, um mbc-90-spezifisches EEPROM-/CS2-Verhalten zu ergaenzen          +
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    //*********************************************************************************************
    //* Modulspezifische Parameterdaten aus dem EEPROM lesen: Modulparameter aus der EEPROM-      *
    //* Zusatzregion laden (mit Defaultwerten fuer den unbeschriebenen Zustand, 0xff-Sentinel),   *
    //* aktualisiert die S88-Modulnummer-Anzeige in _cs2IndexEx.                                  *
    //*********************************************************************************************

    void Mbc90Com::eepromReadParaExtern()
    {
        uint8_t buf[18];

        _utils->eepromReadExtra(0, buf, 18);

        if (buf[EEX_S88ADDR] == 0xff)
        {
            MBC_90_S88_H = 0x00;
            MBC_90_S88_L = 0x01;
        }
        else
        {
            MBC_90_S88_H = buf[EEX_S88ADDR];
            MBC_90_S88_L = buf[EEX_S88ADDR + 1];
        }

        for (uint8_t index = 0; index < 16; index++)
        {
            uint8_t value = buf[EEX_S88TIMING + index];
            MBC_90_INTS88(index) = (value == 0xff) ? 1 : value;
        }

        _cs2IndexEx[ROW_S88MOD][CS2ROW_OFFSET_L] = MBC_90_S88_L;

        for (uint8_t index = 0; index < 16; index++)
        {
            _cs2IndexEx[ROW_S88TIMING_BASE + index][CS2ROW_OFFSET_L] = MBC_90_INTS88(index);
        }
    }

    //*********************************************************************************************
    //* Modulspezifischen Parameterblock in die EEPROM-Zusatzregion schreiben - kein RAM-Resync   *
    //* noetig, MBC_90_INTS88()/_S88_H/_L lesen ohnehin direkt aus dem Systemarray - aktualisiert *
    //* bei EE_90_S88ADDR zusaetzlich die Anzeige in _cs2IndexEx                                  *
    //*********************************************************************************************
    //* block: Identifier des zu schreibenden Blocks (0x0A/0x0B)                                  *
    //*********************************************************************************************

    void Mbc90Com::eepromWriteParaExtern(uint8_t block)
    {
        uint8_t buf[16];

        switch (block)
        {
            case EE_90_S88ADDR:

                buf[0] = MBC_90_S88_H;
                buf[1] = MBC_90_S88_L;
                _utils->eepromWriteExtra(EEX_S88ADDR, buf, 2);

                _cs2IndexEx[ROW_S88MOD][CS2ROW_OFFSET_L] = MBC_90_S88_L;

            break;

            case EE_90_S88TIMING:

                for (uint8_t index = 0; index < 16; index++) buf[index] = MBC_90_INTS88(index);
                _utils->eepromWriteExtra(EEX_S88TIMING, buf, 16);

                for (uint8_t index = 0; index < 16; index++)
                {
                    _cs2IndexEx[ROW_S88TIMING_BASE + index][CS2ROW_OFFSET_L] = MBC_90_INTS88(index);
                }

            break;
        }
    }

    //*********************************************************************************************
    //* Modulspezifische Parameterdaten im EEPROM loeschen                                        *
    //* Deklariert, aber nie aufgerufen - nichts zu tun                                           *
    //*********************************************************************************************

    void Mbc90Com::eepromEraseParaExtern()
    {
    }

    //*********************************************************************************************
    //* Traegt GUiD/HASH in die modulspezifische Konfig-Bestaetigungsnachricht _msgKonfigOk ein   *
    //*********************************************************************************************

    void Mbc90Com::hashInitExtern()
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
    //* Softwareversion fuer CS2-Ping-Antwort ueberschreiben - mbc-90 simuliert keine fremde SW-  *
    //* Version, nichts zu tun                                                                    *
    //*********************************************************************************************

    void Mbc90Com::swInitExtern()
    {
    }

    //*********************************************************************************************
    //* Modulspezifischen Messwert an die CS2 senden                                              *
    //* mbc-90 hat keine (ANZMESSWERTEMODUL=0), nichts zu tun                                     *
    //*********************************************************************************************
    //* kanal: Messkanalnummer                                                                    *
    //*********************************************************************************************

    void Mbc90Com::cs2IndexMesswerteExtern(uint8_t kanal)
    {
        (void) kanal;
    }

    //*********************************************************************************************
    //* Modulspezifischen Parameterwert von der CS2 uebernehmen - msg[D5]==KANAL_S88MOD: neue     *
    //* S88-Modulnummer (Rohwert 0...93) uebernehmen; msg[D5] im Bereich                          *
    //* KANAL_S88TIMING_BASE..+15: neue Integrationszeit (Rohwert 1...250) fuer den betroffenen   *
    //* Kontakt uebernehmen. Beide Faelle persistieren und bestaetigen der CSx die Uebernahme,    *
    //* ein Wert ausserhalb des jeweiligen Bereichs bleibt ohne Wirkung und wird per FALSE        *
    //* quittiert. Der Konfigurationswert ist laut CS2-Protokoll ein vollstaendiges Word ueber D6 *
    //* (High) + D7 (Low), nicht nur D7 - der Wertebereich ist zwar stets <= 255 (D6 des          *
    //* eingehenden Worts damit immer 0), wird aber der Vollstaendigkeit halber mitausgewertet.   *
    //*********************************************************************************************
    //* msg: CAN-Nachricht (msg[D5]=Kanalnummer, msg[D6..D7]=neuer Wert)                          *
    //*********************************************************************************************

    void Mbc90Com::cs2IndexParameterExtern(const uint8_t msg[MBC_COM_MSG_LAENGE])
    {
        uint8_t  kanal = msg[MBC_CAN_D5];
        uint16_t wert  = (uint16_t) (((uint16_t) msg[MBC_CAN_D6] << 8) | msg[MBC_CAN_D7]);
        bool     gueltig;

        if (kanal == KANAL_S88MOD)
        {
            gueltig = (wert <= 93);

            if (gueltig)
            {
                MBC_90_S88_L = (uint8_t) wert;
                eepromWriteParaExtern(EE_90_S88ADDR);
            }
        }
        else if ((kanal >= KANAL_S88TIMING_BASE) && (kanal < (KANAL_S88TIMING_BASE + 16)))
        {
            gueltig = (wert >= 1) && (wert <= 250);

            if (gueltig)
            {
                MBC_90_INTS88(kanal - KANAL_S88TIMING_BASE) = (uint8_t) wert;
                eepromWriteParaExtern(EE_90_S88TIMING);
            }
        }
        else
        {
            return;
        }

        _msgKonfigOk[MBC_CAN_D5] = kanal;
        _msgKonfigOk[MBC_CAN_D6] = gueltig ? 0x01 : 0x00;

        sendMessage(_msgKonfigOk);
    }

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //+ Zusaetzliche Erweiterungspunkte (siehe Kopfkommentar von MbcCom.h)                        +
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    //*********************************************************************************************
    //* Feste CS2-Geraetekennung - mbc-90 hat keine, die Kennung kommt vom Parametriercenter      *
    //*********************************************************************************************
    //* knH/knL: unveraendert                                                                     *
    //*********************************************************************************************
    //* Return: false (keine feste Kennung)                                                       *
    //*********************************************************************************************

    bool Mbc90Com::kennerFixedExtern(uint8_t &knH, uint8_t &knL)
    {
        (void) knH;
        (void) knL;
        return false;
    }

    //*********************************************************************************************
    //* Zusaetzlicher Versandweg fuer ausgehende Nachrichten der Basisklasse - mbc-90 hat keinen, *
    //* nichts zu tun                                                                             *
    //*********************************************************************************************
    //* msg: Nachrichtenarray                                                                     *
    //*********************************************************************************************

    void Mbc90Com::sendMessageExtern(const uint8_t msg[MBC_COM_MSG_LAENGE])
    {
        (void) msg;
    }

    //*********************************************************************************************
    //* Zeile aus dem modulspezifischen cs2_index_ex[]-Array liefern - laufendeNr 0 = einzige     *
    //* Zeile (S88-Modulnummer), sonst nullptr                                                    *
    //*********************************************************************************************
    //* laufendeNr: 0..MBC90_ANZMESSWERTEMODUL+MBC90_ANZKONFIGMODUL-1                             *
    //*********************************************************************************************
    //* Return: Zeiger auf die 40-Byte-Zeile oder nullptr                                         *
    //*********************************************************************************************

    const uint8_t* Mbc90Com::cs2IndexExtern(uint8_t laufendeNr)
    {
        if (laufendeNr >= (MBC90_ANZMESSWERTEMODUL + MBC90_ANZKONFIGMODUL)) return nullptr;
        return _cs2IndexEx[laufendeNr];
    }

    //*********************************************************************************************
    //* Einmalig nach dem begin() der Basisklasse aufrufen: gespeicherte Parameter aus der        *
    //* EEPROM-Zusatzregion laden, Integratoren zuruecksetzen und die Stellungsanzeige auf "alles *
    //* aus" vorbelegen.                                                                          *
    //*********************************************************************************************

    void Mbc90Com::beginModule()
    {
        eepromReadParaExtern();

        unsigned long now = millis();

        for (uint8_t index = 0; index < 16; index++)
        {
            _pinActive[index]       = false;
            _pinStartMs[index]      = now;
            _pinLastChangeMs[index] = now;
        }

        #if defined(__AVR__)
            _displayLow->write(0xff);
            _displayHigh->write(0xff);
        #else
            _display->writeGpioA(0xff);
            _display->writeGpioB(0xff);
        #endif
    }

    //*********************************************************************************************
    //* Je loop()-Durchlauf aufrufen: Kontakte einlesen/entprellen, Rueckmeldestatus und          *
    //* Stellungsanzeige aktualisieren sowie CAN-Nachrichtenempfang/-Auswertung                   *
    //*********************************************************************************************

    void Mbc90Com::update()
    {
        s88PinsCheck();
        poll();

        uint8_t msg[MBC_CAN_MSG_LEN];

        if (_can->receive(msg))
        {
            check(msg);
            handleS88Request(msg);
        }
    }

//------ Ende Klasse Mbc90Com ---------------------------------------------------------------------

//###### Ende Quellcode Mbc90Com.cpp ##############################################################
