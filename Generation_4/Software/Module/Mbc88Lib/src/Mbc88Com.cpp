//*************************************************************************************************
//* Modul mbc-88: S88-Rueckmeldedecoder (Mbc88Lib)                                                *
//*************************************************************************************************
//* Datei: Mbc88Com.cpp                                                                           *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen:                                                                                  *
//* V1.0.0 - 08.08.2026 - Nullversion                                                             *
//*************************************************************************************************

//###### Start Quellcode Mbc88Com.cpp #############################################################

#include "Mbc88Com.h"

//------ Start interne Konstanten -----------------------------------------------------------------

    // CAN-Befehlsnummer der CS2-S88-Anfrage (Anfrage 0x22/Antwort 0x23 sind bereits als
    // MBC_COM_MSG_CS2_0x22_R-Template in der MbcCom-Basisklasse vorgefuellt, siehe MbcCom.cpp)

    static const uint8_t CAN_ID_CS2_S88_REQUEST = 0x22;

    // Modulspezifische EEPROM-Blockindizes - diese Werte werden von externer PC-/CS2-
    // Parametriersoftware direkt referenziert, siehe MbcCom.cpp PC_ARRAY_DATA/MBC_COM_EEPROM)

    static const uint8_t EE_88_S88ADDR   = 0x0A;
    static const uint8_t EE_88_S88TIMING = 0x0B;
    static const uint8_t EE_88_S88MOD    = 0x0C;

    // Byte-Layout der EEPROM-Zusatzregion (11 Byte, siehe MbcUtils::eepromReadExtra()/
    // eepromWriteExtra()) - unabhaengig von den EE_88_*-Blockindizes oben

    static const uint16_t EEX_S88ADDR   = 0;    // 2 Byte: S88Addr (H,L)
    static const uint16_t EEX_S88TIMING = 2;    // 8 Byte: Integrationszeit je Kontakt
    static const uint16_t EEX_S88MOD    = 10;   // 1 Byte: Halbmodul

    // CS2-Kanalnummern/Zeilenindizes der modulspezifischen Konfigkanaele "S88-Modulnummer",
    // "Halbmodul" und der 8 Integrationszeiten

    static const uint8_t KANAL_S88MOD         = MBC_COM_ANZMESSWERTE + MBC_COM_ANZKONFIG + 1;  // 7
    static const uint8_t KANAL_S88HALBMODUL   = KANAL_S88MOD + 1;                              // 8
    static const uint8_t KANAL_S88TIMING_BASE = KANAL_S88HALBMODUL + 1;                        // 9..16
    static const uint8_t ROW_S88MOD           = 0;
    static const uint8_t ROW_S88HALBMODUL     = 1;
    static const uint8_t ROW_S88TIMING_BASE   = 2;   // Zeilen 2..9 (1 je Kontakt)
    static const uint8_t S88MOD_OFFSET        = 7;   // "Aktueller Wert Low" innerhalb der Zeile
    static const uint8_t S88HALBMODUL_OFFSET  = 7;   // "Aktueller Wert Low" innerhalb der Zeile
    static const uint8_t S88TIMING_OFFSET_H   = 6;   // "Aktueller Wert High" (Rohwert <= 254, stets 0)
    static const uint8_t S88TIMING_OFFSET_L   = 7;   // "Aktueller Wert Low" innerhalb der Zeile

    // Systemarray-Layout ab MBC_S_GENPURP (deckt sich mit dem Parametriercenter:
    // MBC_88_L_S88=4 + MBC_88_L_INTS88=8)

    #define MBC_88_S_S88     MBC_S_GENPURP
    #define MBC_88_S88_H     _utils->array()[MBC_88_S_S88]
    #define MBC_88_S88_L     _utils->array()[MBC_88_S_S88 + 1]
    #define MBC_88_MOD       _utils->array()[MBC_88_S_S88 + 2]
    #define MBC_88_RM        _utils->array()[MBC_88_S_S88 + 3]
    #define MBC_88_S_INTS88  (MBC_88_S_S88 + 4)
    #define MBC_88_INTS88(i) _utils->array()[MBC_88_S_INTS88 + (i)]

//------ Ende interne Konstanten ------------------------------------------------------------------

//------ Start Klasse Mbc88Com --------------------------------------------------------------------

    //*********************************************************************************************
    //* Konstruktor zur Deklaration von Objektvariablen (initialisiert ausserdem die              *
    //* modulspezifische CS2-Konfigurationstabelle _cs2IndexEx[] und die                          *
    //* Konfig-Bestaetigungsnachricht _msgKonfigOk[])                                             *
    //*********************************************************************************************
    //* utils:       zugehoerige MbcUtils-Instanz                                                 *
    //* can:         zugehoeriger CAN-Kanal dieses Moduls (Basisklassen-Parameter can)            *
    //* bootStorage: zugehoerige MbcBootStorage-Instanz (Firmware-Update-Staging)                 *
    //* inputs:      8 Kontakteingaenge, Index-Konvention siehe Klassenkommentar                  *
    //* outputs:     8 Status-LED-Ausgaenge, Index-Konvention siehe Klassenkommentar              *
    //* pinReset:    NUR auf AVR (ATmega644P) vorhanden, siehe MbcCom-Konstruktor                 *
    //*********************************************************************************************

    // PROGMEM: nur einmal hier beim Konstruieren gelesen

    static const uint8_t initCs2Ex[MBC88_ANZMESSWERTEMODUL + MBC88_ANZKONFIGMODUL]
                                   [MBC_COM_CS2PAKETE * 8] PROGMEM =
    {
        // INDEX_CTRL 1.EX.1 - S88-Modulnummer

        {   KANAL_S88MOD,0x02,0x00,0x01,0x00,0x5D,0x00,0x00,             // 1...8
            'S','8','8',' ','M','o','d','u',                             // 9...16
            'l',':',0x00,'1',0x00,'9','3',0x00,                          // 17...24
            '#',0x00,0x00,0x00,0x00,0x00,0x00,0x00,                      // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                      // 33...40
        },

        // INDEX_CTRL 1.EX.2 - Halbmodul (0 = Kontakte 0...7, 1 = Kontakte 8...15)

        {   KANAL_S88HALBMODUL,0x02,0x00,0x00,0x00,0x01,0x00,0x00,       // 1...8
            'H','a','l','b','m','o','d','u',                             // 9...16
            'l',':',0x00,'0',0x00,'1',0x00,'#',                          // 17...24
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,                     // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                      // 33...40
        },

        // INDEX_CTRL 1.EX.3 - Integrationszeit Kontakt 1 (Rohwert, Einheit x10ms)

        {   KANAL_S88TIMING_BASE,0x02,0x00,0x01,0x00,0xFA,0x00,0x00,     // 1...8
            'K','1',' ','x','1','0','m','s',                             // 9...16
            ':',0x00,'1',0x00,'2','5','0',0x00,                          // 17...24
            '#',0x00,0x00,0x00,0x00,0x00,0x00,0x00,                      // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                      // 33...40
        },

        // INDEX_CTRL 1.EX.4 - Integrationszeit Kontakt 2 (Rohwert, Einheit x10ms)

        {   KANAL_S88TIMING_BASE + 1,0x02,0x00,0x01,0x00,0xFA,0x00,0x00, // 1...8
            'K','2',' ','x','1','0','m','s',                             // 9...16
            ':',0x00,'1',0x00,'2','5','0',0x00,                          // 17...24
            '#',0x00,0x00,0x00,0x00,0x00,0x00,0x00,                      // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                      // 33...40
        },

        // INDEX_CTRL 1.EX.5 - Integrationszeit Kontakt 3 (Rohwert, Einheit x10ms)

        {   KANAL_S88TIMING_BASE + 2,0x02,0x00,0x01,0x00,0xFA,0x00,0x00, // 1...8
            'K','3',' ','x','1','0','m','s',                             // 9...16
            ':',0x00,'1',0x00,'2','5','0',0x00,                          // 17...24
            '#',0x00,0x00,0x00,0x00,0x00,0x00,0x00,                      // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                      // 33...40
        },

        // INDEX_CTRL 1.EX.6 - Integrationszeit Kontakt 4 (Rohwert, Einheit x10ms)

        {   KANAL_S88TIMING_BASE + 3,0x02,0x00,0x01,0x00,0xFA,0x00,0x00, // 1...8
            'K','4',' ','x','1','0','m','s',                             // 9...16
            ':',0x00,'1',0x00,'2','5','0',0x00,                          // 17...24
            '#',0x00,0x00,0x00,0x00,0x00,0x00,0x00,                      // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                      // 33...40
        },

        // INDEX_CTRL 1.EX.7 - Integrationszeit Kontakt 5 (Rohwert, Einheit x10ms)

        {   KANAL_S88TIMING_BASE + 4,0x02,0x00,0x01,0x00,0xFA,0x00,0x00, // 1...8
            'K','5',' ','x','1','0','m','s',                             // 9...16
            ':',0x00,'1',0x00,'2','5','0',0x00,                          // 17...24
            '#',0x00,0x00,0x00,0x00,0x00,0x00,0x00,                      // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                      // 33...40
        },

        // INDEX_CTRL 1.EX.8 - Integrationszeit Kontakt 6 (Rohwert, Einheit x10ms)

        {   KANAL_S88TIMING_BASE + 5,0x02,0x00,0x01,0x00,0xFA,0x00,0x00, // 1...8
            'K','6',' ','x','1','0','m','s',                             // 9...16
            ':',0x00,'1',0x00,'2','5','0',0x00,                          // 17...24
            '#',0x00,0x00,0x00,0x00,0x00,0x00,0x00,                      // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                      // 33...40
        },

        // INDEX_CTRL 1.EX.9 - Integrationszeit Kontakt 7 (Rohwert, Einheit x10ms)

        {   KANAL_S88TIMING_BASE + 6,0x02,0x00,0x01,0x00,0xFA,0x00,0x00, // 1...8
            'K','7',' ','x','1','0','m','s',                             // 9...16
            ':',0x00,'1',0x00,'2','5','0',0x00,                          // 17...24
            '#',0x00,0x00,0x00,0x00,0x00,0x00,0x00,                      // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                      // 33...40
        },

        // INDEX_CTRL 1.EX.10 - Integrationszeit Kontakt 8 (Rohwert, Einheit x10ms)

        {   KANAL_S88TIMING_BASE + 7,0x02,0x00,0x01,0x00,0xFA,0x00,0x00, // 1...8
            'K','8',' ','x','1','0','m','s',                             // 9...16
            ':',0x00,'1',0x00,'2','5','0',0x00,                          // 17...24
            '#',0x00,0x00,0x00,0x00,0x00,0x00,0x00,                      // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                      // 33...40
        }
    };

    static const uint8_t initMsgKonfigOk[MBC_COM_MSG_LAENGE] PROGMEM =
        {0x00,0x01,0x00,0x00,0x07,0x00,0x00,0x00,0x00,0x0b,0x00,0x01,0x00};

    #if defined(__AVR__)

        Mbc88Com::Mbc88Com(MbcUtils &utils, MbcCan &can, MbcBootStorage &bootStorage,
                            MbcDigitalIn *(&inputs)[8], MbcDigitalOut *(&outputs)[8], uint8_t pinReset)
            : MbcCom(utils, can, bootStorage, pinReset,
                     MBC88_ANZMESSWERTEMODUL, MBC88_ANZKONFIGMODUL)
        {
            for (uint8_t index = 0; index < 8; index++) _inputs[index]  = inputs[index];
            for (uint8_t index = 0; index < 8; index++) _outputs[index] = outputs[index];
            for (uint8_t index = 0; index < 8; index++) _pinActive[index] = false;

            memcpy_P(_cs2IndexEx, initCs2Ex, sizeof(_cs2IndexEx));
            memcpy_P(_msgKonfigOk, initMsgKonfigOk, MBC_COM_MSG_LAENGE);
        }

    #else

        Mbc88Com::Mbc88Com(MbcUtils &utils, MbcCan &can, MbcBootStorage &bootStorage,
                            MbcDigitalIn *(&inputs)[8], MbcDigitalOut *(&outputs)[8])
            : MbcCom(utils, can, bootStorage,
                     MBC88_ANZMESSWERTEMODUL, MBC88_ANZKONFIGMODUL)
        {
            for (uint8_t index = 0; index < 8; index++) _inputs[index]  = inputs[index];
            for (uint8_t index = 0; index < 8; index++) _outputs[index] = outputs[index];
            for (uint8_t index = 0; index < 8; index++) _pinActive[index] = false;

            memcpy_P(_cs2IndexEx, initCs2Ex, sizeof(_cs2IndexEx));
            memcpy_P(_msgKonfigOk, initMsgKonfigOk, MBC_COM_MSG_LAENGE);
        }

    #endif

    //*********************************************************************************************
    //* Busnummer (1/2/3) aus dem niederwertigen Byte der S88Addr bestimmen - ausgewertet wird    *
    //* dafuer nur MBC_88_S88_L, nie die volle 16-Bit-Adresse (Modulnummern liegen immer unter    *
    //* 256)                                                                                      *
    //*********************************************************************************************
    //* s88AddrLow: niederwertiges Byte der S88Addr                                               *
    //*********************************************************************************************
    //* Return: Busnummer 1, 2 oder 3                                                             *
    //*********************************************************************************************

    uint16_t Mbc88Com::busNrFromS88AddrLow(uint8_t s88AddrLow)
    {
        if ((s88AddrLow > 31) && (s88AddrLow < 63)) return 2;
        if (s88AddrLow > 62) return 3;

        return 1;
    }

    //*********************************************************************************************
    //* Ereignismeldung fuer einen Kontakt versenden: immer als Broadcast an das                  *
    //* mbc80-PC-Interface, zusaetzlich als CS2-Meldung wenn das Modul initialisiert und mit der  *
    //* CS2 verbunden ist.                                                                        *
    //*********************************************************************************************
    //* index:        S88-Kontakt 0..7 (dieses Halbmoduls)                                        *
    //* eventActive:  0x01 = Kontakt ausgeloest, 0x00 = Kontakt wieder frei                       *
    //*********************************************************************************************

    void Mbc88Com::sendS88Event(uint8_t index, uint8_t eventActive)
    {
        can_msg[MBC_COM_MSG_S88][MBC_CAN_D4] = MBC_88_S88_H;
        can_msg[MBC_COM_MSG_S88][MBC_CAN_D5] = MBC_88_S88_L;
        can_msg[MBC_COM_MSG_S88][MBC_CAN_D6] = (uint8_t) (index + MBC_88_MOD * 8);
        can_msg[MBC_COM_MSG_S88][MBC_CAN_D7] = eventActive;

        sendMessage(can_msg[MBC_COM_MSG_S88]);

        if (!initDone() || !csConnected()) return;

        uint16_t busNr      = busNrFromS88AddrLow(MBC_88_S88_L);
        uint16_t baseAddr   = (uint16_t) (((((uint16_t) MBC_88_S88_L - 31UL * (busNr - 1UL)) - 1UL)) << 4);
        uint16_t s88PinNr   = (uint16_t) (1000UL * busNr + baseAddr + index + 1 + MBC_88_MOD * 8);

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
    //* Kontakte einlesen und entprellen (millis()-basierter Integrator als Entpreller-Ersatz)    *
    //* sowie Status-LEDs aktualisieren                                                           *
    //*********************************************************************************************

    void Mbc88Com::s88PinsCheck()
    {
        uint8_t rm = 0;

        for (uint8_t index = 0; index < 8; index++)
        {
            bool active = _inputs[index]->read();   // true = Kontakt aktuell ausgeloest

            // Kontakt wieder frei, Integrator laeuft noch (Entprellung/Haltezeit) - erst nach
            // Ablauf der Integrationszeit endgueltig als beendet melden

            if (!active && _pinActive[index])
            {
                unsigned long integrationMs = (unsigned long) MBC_88_INTS88(index) * 10;
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

            // Status-LED direkt mit logischem Index ansteuern: an, wenn der Kontakt ausgeloest
            // ist (Belegtmelder-Anzeige), aus im Ruhezustand - die physische Pin-/Expander-Bit-
            // Zuordnung waehlt ohnehin frei der Sketch, siehe Klassenkommentar. Das
            // Rueckmeldestatus-Byte MBC_88_RM dagegen ist ein externer Vertrag: es ist invertiert
            // (Bit=1 fuer Ruhezustand) mit Bit(7-index) je Kontakt gepackt, und exakt diese
            // Bitreihenfolge liest das Parametriercenter aus - deshalb bewusst
            // NICHT wie die LED-Ausgabe umgestellt, sondern diese Bitpackung beibehalten

            bool idle = !_pinActive[index];

            _outputs[index]->write(_pinActive[index]);

            if (idle) rm |= (uint8_t) (1 << (7 - index));
        }

        MBC_88_RM = (uint8_t) ~rm;
    }

    //*********************************************************************************************
    //* CS2-S88-Statusanfrage beantworten - nur DLC==4 (Statusabfrage) wird verarbeitet, andere   *
    //* DLC-Werte werden ignoriert                                                                *
    //*********************************************************************************************
    //* msg: empfangene CAN-Nachricht                                                             *
    //*********************************************************************************************

    void Mbc88Com::handleS88Request(const uint8_t msg[MBC_CAN_MSG_LEN])
    {
        if ((msg[MBC_CAN_ID] != CAN_ID_CS2_S88_REQUEST) || (msg[MBC_CAN_DLC] != 0x04)) return;
        if (!initDone() || !csConnected()) return;

        uint16_t busNr      = busNrFromS88AddrLow(MBC_88_S88_L);
        uint16_t baseModul  = MBC_88_S88_L;

        if (busNr == 2) baseModul -= 31;
        if (busNr == 3) baseModul -= 62;

        uint16_t msgAddr = (uint16_t) (((uint16_t) msg[MBC_CAN_D2] << 8) | (uint16_t) msg[MBC_CAN_D3]);

        if ((msgAddr > 1500) && (busNr == 1)) return;
        if (((msgAddr < 1500) || (msgAddr > 2500)) && (busNr == 2)) return;
        if ((msgAddr < 2500) && (busNr == 3)) return;

        uint16_t reduced = (uint16_t) (msgAddr - 1000UL * busNr);
        reduced = (uint16_t) (((reduced - 1UL) >> 4) + 1UL);

        if (reduced != baseModul) return;

        uint16_t pin = (uint16_t) ((msgAddr - (1000UL * busNr + 1UL)) & 0x0f);

        // Kontaktnummer passt nicht zum Halbmodul - Anfrage ignorieren; bei Abfrage der Kontakte
        // 9-16 auf das lokale Halbmodul (Eingaenge 1-8 dieses Moduls) ummappen

        if ((MBC_88_MOD == 0) && (pin > 7)) return;
        if (pin > 7) pin -= 8;

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
    //+ Hook) - hier anpassen, um mbc-88-spezifisches EEPROM-/CS2-Verhalten zu ergaenzen          +
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    //*********************************************************************************************
    //* Modulspezifische Parameterdaten aus dem EEPROM lesen: Modulparameter aus der EEPROM-      *
    //* Zusatzregion laden (mit Defaultwerten fuer den unbeschriebenen Zustand, 0xff-Sentinel),   *
    //* aktualisiert die S88-Modulnummer-/Halbmodul-/Integrationszeit-Anzeige in _cs2IndexEx      *
    //*********************************************************************************************

    void Mbc88Com::eepromReadParaExtern()
    {
        uint8_t buf[11];

        _utils->eepromReadExtra(0, buf, 11);

        if (buf[EEX_S88ADDR] == 0xff)
        {
            MBC_88_S88_H = 0x00;
            MBC_88_S88_L = 0x01;
        }
        else
        {
            MBC_88_S88_H = buf[EEX_S88ADDR];
            MBC_88_S88_L = buf[EEX_S88ADDR + 1];
        }

        for (uint8_t index = 0; index < 8; index++)
        {
            uint8_t value = buf[EEX_S88TIMING + index];
            MBC_88_INTS88(index) = (value == 0xff) ? 1 : value;

            _cs2IndexEx[ROW_S88TIMING_BASE + index][S88TIMING_OFFSET_H] = 0;
            _cs2IndexEx[ROW_S88TIMING_BASE + index][S88TIMING_OFFSET_L] = MBC_88_INTS88(index);
        }

        MBC_88_MOD = (buf[EEX_S88MOD] == 0xff) ? 0 : buf[EEX_S88MOD];

        _cs2IndexEx[ROW_S88MOD][S88MOD_OFFSET]             = MBC_88_S88_L;
        _cs2IndexEx[ROW_S88HALBMODUL][S88HALBMODUL_OFFSET] = MBC_88_MOD;
    }

    //*********************************************************************************************
    //* Modulspezifischen Parameterblock in die EEPROM-Zusatzregion schreiben kein RAM-Resync     *
    //* noetig, MBC_88_INTS88()/_S88_H/_L/_MOD lesen ohnehin direkt aus dem Systemarray -         *
    //* EE_88_S88ADDR/EE_88_S88MOD/EE_88_S88TIMING aktualisieren zusaetzlich die jeweilige        *
    //* Anzeige in _cs2IndexEx                                                                    *
    //*********************************************************************************************
    //* block: Identifier des zu schreibenden Blocks (0x0A....)                                   *
    //*********************************************************************************************

    void Mbc88Com::eepromWriteParaExtern(uint8_t block)
    {
        uint8_t buf[8];

        switch (block)
        {
            case EE_88_S88ADDR:

                buf[0] = MBC_88_S88_H;
                buf[1] = MBC_88_S88_L;
                _utils->eepromWriteExtra(EEX_S88ADDR, buf, 2);

                _cs2IndexEx[ROW_S88MOD][S88MOD_OFFSET] = MBC_88_S88_L;

            break;

            case EE_88_S88TIMING:

                for (uint8_t index = 0; index < 8; index++) buf[index] = MBC_88_INTS88(index);
                _utils->eepromWriteExtra(EEX_S88TIMING, buf, 8);

                for (uint8_t index = 0; index < 8; index++)
                {
                    _cs2IndexEx[ROW_S88TIMING_BASE + index][S88TIMING_OFFSET_H] = 0;
                    _cs2IndexEx[ROW_S88TIMING_BASE + index][S88TIMING_OFFSET_L] = MBC_88_INTS88(index);
                }

            break;

            case EE_88_S88MOD:

                buf[0] = MBC_88_MOD;
                _utils->eepromWriteExtra(EEX_S88MOD, buf, 1);

                _cs2IndexEx[ROW_S88HALBMODUL][S88HALBMODUL_OFFSET] = MBC_88_MOD;

            break;
        }
    }

    //*********************************************************************************************
    //* Modulspezifische Parameterdaten im EEPROM loeschen                                        *
    //* Deklariert, aber nie aufgerufen - nichts zu tun                                           *
    //*********************************************************************************************

    void Mbc88Com::eepromEraseParaExtern()
    {
    }

    //*********************************************************************************************
    //* Traegt GUiD/HASH in die modulspezifische Konfig-Bestaetigungsnachricht _msgKonfigOk ein   *
    //*********************************************************************************************

    void Mbc88Com::hashInitExtern()
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
    //* Softwareversion fuer CS2-Ping-Antwort ueberschreiben - mbc-88 simuliert keine fremde      *
    //* SW-Version, nichts zu tun                                                                 *
    //*********************************************************************************************

    void Mbc88Com::swInitExtern()
    {
    }

    //*********************************************************************************************
    //* Modulspezifischen Messwert an die CS2 senden                                              *
    //* mbc-88 hat keine (ANZMESSWERTEMODUL=0), nichts zu tun                                     *
    //*********************************************************************************************
    //* kanal: Messkanalnummer                                                                    *
    //*********************************************************************************************

    void Mbc88Com::cs2IndexMesswerteExtern(uint8_t kanal)
    {
        (void) kanal;
    }

    //*********************************************************************************************
    //* Modulspezifischen Parameterwert von der CS2 uebernehmen: msg[D5]==KANAL_S88MOD: neue      *
    //* S88-Modulnummer (Rohwert 1...93), msg[D5]==KANAL_S88HALBMODUL: neues Halbmodul (0/1),     *
    //* msg[D5]==KANAL_S88TIMING_BASE+0..7: neue Integrationszeit (Rohwert 1...250, Einheit x10ms *
    //* - der Multiplikator steht im Label statt in einer Werteumrechnung) fuer Kontakt 1...8     *
    //* uebernehmen, jeweils persistieren und der CSx bestaetigen. Ein Wert ausserhalb des        *
    //* jeweiligen Bereichs bleibt ohne Wirkung und wird per FALSE quittiert. Der                 *
    //* Konfigurationswert ist laut CS2-Protokoll ein vollstaendiges Word ueber D6 (High) + D7    *
    //* (Low), nicht nur D7 - fuer alle drei Kanaltypen hier (Wertebereich stets <= 255) ist D6   *
    //* des eingehenden Worts zwar immer 0, wird aber der Vollstaendigkeit halber mitausgewertet. *
    //*********************************************************************************************
    //* msg: CAN-Nachricht (msg[D5]==KANAL_S88MOD/KANAL_S88HALBMODUL/KANAL_S88TIMING_BASE+0..7,   *
    //* msg[D6..D7]=neuer Wert)                                                                   *
    //*********************************************************************************************

    void Mbc88Com::cs2IndexParameterExtern(const uint8_t msg[MBC_COM_MSG_LAENGE])
    {
        uint8_t  kanal = msg[MBC_CAN_D5];
        uint16_t wert  = (uint16_t) (((uint16_t) msg[MBC_CAN_D6] << 8) | msg[MBC_CAN_D7]);
        bool     gueltig;

        if (kanal == KANAL_S88MOD)
        {
            gueltig = (wert >= 1) && (wert <= 93);

            if (gueltig)
            {
                MBC_88_S88_L = (uint8_t) wert;
                eepromWriteParaExtern(EE_88_S88ADDR);
            }
        }
        else if (kanal == KANAL_S88HALBMODUL)
        {
            gueltig = (wert <= 1);

            if (gueltig)
            {
                MBC_88_MOD = (uint8_t) wert;
                eepromWriteParaExtern(EE_88_S88MOD);
            }
        }
        else if ((kanal >= KANAL_S88TIMING_BASE) && (kanal < (uint8_t) (KANAL_S88TIMING_BASE + 8)))
        {
            gueltig = (wert >= 1) && (wert <= 250);

            if (gueltig)
            {
                uint8_t idx = (uint8_t) (kanal - KANAL_S88TIMING_BASE);

                MBC_88_INTS88(idx) = (uint8_t) wert;
                eepromWriteParaExtern(EE_88_S88TIMING);
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
    //* Feste CS2-Geraetekennung - mbc-88 hat keine, die Kennung kommt vom Parametriercenter      *
    //*********************************************************************************************
    //* knH/knL: unveraendert                                                                     *
    //*********************************************************************************************
    //* Return: false (keine feste Kennung)                                                       *
    //*********************************************************************************************

    bool Mbc88Com::kennerFixedExtern(uint8_t &knH, uint8_t &knL)
    {
        (void) knH;
        (void) knL;
        return false;
    }

    //*********************************************************************************************
    //* Zusaetzlicher Versandweg fuer ausgehende Nachrichten der Basisklasse - mbc-88 hat keinen, *
    //* nichts zu tun                                                                             *
    //*********************************************************************************************
    //* msg: Nachrichtenarray                                                                     *
    //*********************************************************************************************

    void Mbc88Com::sendMessageExtern(const uint8_t msg[MBC_COM_MSG_LAENGE])
    {
        (void) msg;
    }

    //*********************************************************************************************
    //* Zeile aus dem modulspezifischen Konfigkanal-Array liefern - laufendeNr 0 = S88-           *
    //* Modulnummer, laufendeNr 1 = Halbmodul, laufendeNr 2..9 = Integrationszeit Kontakt 1...8,  *
    //* sonst nullptr                                                                             *
    //*********************************************************************************************
    //* laufendeNr: 0..MBC88_ANZMESSWERTEMODUL+MBC88_ANZKONFIGMODUL-1                             *
    //*********************************************************************************************
    //* Return: Zeiger auf die 40-Byte-Zeile oder nullptr                                         *
    //*********************************************************************************************

    const uint8_t* Mbc88Com::cs2IndexExtern(uint8_t laufendeNr)
    {
        if (laufendeNr >= (MBC88_ANZMESSWERTEMODUL + MBC88_ANZKONFIGMODUL)) return nullptr;
        return _cs2IndexEx[laufendeNr];
    }

    //*********************************************************************************************
    //* Einmalig nach dem begin() der Basisklasse aufrufen: gespeicherte Parameter aus der        *
    //* EEPROM-Zusatzregion laden                                                                 *
    //*********************************************************************************************

    void Mbc88Com::beginModule()
    {
        eepromReadParaExtern();

        unsigned long now = millis();

        for (uint8_t index = 0; index < 8; index++)
        {
            _pinActive[index]       = false;
            _pinStartMs[index]      = now;
            _pinLastChangeMs[index] = now;
        }
    }

    //*********************************************************************************************
    //* Je loop()-Durchlauf aufrufen: Kontakte einlesen/entprellen, Status-LEDs aktualisieren und *
    //* CAN-Nachrichtenempfang/-Auswertung                                                        *
    //*********************************************************************************************

    void Mbc88Com::update()
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

//------ Ende Klasse Mbc88Com ---------------------------------------------------------------------

//###### Ende Quellcode Mbc88Com.cpp ##############################################################
