//*************************************************************************************************
//* Modul mbc-93: mfx-Booster-Monitor auf Basis Maerklin 6017 (Mbc93Lib)                          *
//*************************************************************************************************
//* Datei: Mbc93Com.cpp                                                                           *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen:                                                                                  *
//* V1.0.0 - 23.08.2026 - Nullversion                                                             *
//* V1.0.1 - 29.08.2026 - RAM-Optimierung                                                         *
//*************************************************************************************************

//###### Start Quellcode Mbc93Com.cpp #############################################################

#include "Mbc93Com.h"

//------ Start interne Konstanten ------------------------------------------------------------------

    // Rohe CAN-ID der Basisklasse fuer System-Befehle (Sub-CMD in D4) - mbc-93 wertet
    // eingehende Stopp-/Go-Telegramme direkt in update() aus, unabhaengig von der
    // PC-/CS2-Dispatch-Logik der Basisklasse (siehe handleSystemCommand())

    static const uint8_t CAN_ID_SYSTEM_BEFEHLE = 0x00;

    // Modulspezifische EEPROM-Blockindizes (kommen 1:1 aus mbc_modul_93.vb/mbc_defs.h -
    // PC_ARRAY_DATA sendet genau diese Werte als Schreib-Identifier)

    static const uint8_t EE_93_SW = 0x0A;
    static const uint8_t EE_93_SH = 0x0B;

    // Byteoffsets in der EEPROM-Zusatzregion (extraEepromSize=3, siehe Beispiel-Sketch)

    static const uint16_t EEX_93_SW = 0;   // 2 Byte: SW_1, SW_2
    static const uint16_t EEX_93_SH = 2;   // 1 Byte: SH

    // CS2-Kanalnummern/Zeilenindizes der 5 modulspezifischen Kanaele (2 Messwerte + 3
    // Konfigkanaele). MbcCom::checkCs2Connection() sortiert modulspezifische Messwertkanaele
    // DIREKT HINTER die Basis-Messwerte (Versorgungsspannung/Temperatur, Kanal 1/2) ein und
    // verschiebt die nachfolgenden Basis-Konfigkanaele (Seriennummer/GUiD/Firmware/Hardware)
    // entsprechend - die CSx zeigt sonst die ersten (Basis-Messwerte + anzMesswerteModul)
    // Kanaele als Messwerte an, unabhaengig von deren eigener Kennung (hardwarebestaetigter
    // Fund: ohne diese Einsortierung erschien die Seriennummer faelschlich als Messwert).
    // Deklarationsreihenfolge = Zeilenreihenfolge in initCs2Ex[] unten

    static const uint8_t KANAL_ON_TRACK    = MBC_COM_ANZMESSWERTE + 1;                     // 3
    static const uint8_t KANAL_KURZSCHLUSS = KANAL_ON_TRACK + 1;                           // 4
    static const uint8_t KANAL_SW_MAJOR    = MBC_COM_ANZMESSWERTE + MBC93_ANZMESSWERTEMODUL
                                            + MBC_COM_ANZKONFIG + 1;                        // 9
    static const uint8_t KANAL_SW_MINOR    = KANAL_SW_MAJOR + 1;                           // 10
    static const uint8_t KANAL_KS_LATENZ   = KANAL_SW_MAJOR + 2;                           // 11

    static const uint8_t ROW_ON_TRACK    = 0;
    static const uint8_t ROW_KURZSCHLUSS = 1;
    static const uint8_t ROW_SW_MAJOR    = 2;
    static const uint8_t ROW_SW_MINOR    = 3;
    static const uint8_t ROW_KS_LATENZ   = 4;
    static const uint8_t OFFSET_KSLAT    = 7;   // "Aktueller Wert Low" innerhalb der Zeile
    static const uint8_t OFFSET_SWVAL    = 7;   // "Aktueller Wert Low" innerhalb der Zeile (SW)

    // ParaCenter-Grenzen der Kurzschluss-Latenzzeit (nud_mbc93_Latenz.Minimum/Maximum/Increment
    // in frmMain.Designer.vb) - die CS2 sendet denselben Wert als rohe Vielfache von
    // LATENZ_SCHRITT_MS (Kenner 0x02, Unterer/Oberer Wert 1...20 in initCs2Ex[] unten), der
    // Faktor 50 wird hier bewusst ueber die Division sichtbar gemacht statt vorgerechnet

    static const uint16_t LATENZ_MIN_MS     = 50;
    static const uint16_t LATENZ_MAX_MS     = 1000;
    static const uint16_t LATENZ_SCHRITT_MS = 50;

    static const uint16_t KS_LATENZ_MIN = LATENZ_MIN_MS / LATENZ_SCHRITT_MS;   // 1
    static const uint16_t KS_LATENZ_MAX = LATENZ_MAX_MS / LATENZ_SCHRITT_MS;   // 20

    // Gueltiger Rohwertbereich der simulierten SW-Version Major/Minor (analog Mbc80Com fuer L88)

    static const uint16_t SW_VAL_MIN = 1;
    static const uint16_t SW_VAL_MAX = 255;

    // Bitmuster der Statusrueckmeldung an das Parametriercenter (Original: ON_OK_STATUS/
    // OFF_SHORT_STATUS/OFF_OK_STATUS)

    static const uint8_t STATUS_OFF_OK    = 0x00;
    static const uint8_t STATUS_ON_OK     = 0x20;
    static const uint8_t STATUS_OFF_SHORT = 0x10;

    // Latenzzeit-Basis je Konfigwert-Schritt (Original: 750 Timer1-Ticks a 64 us = 48 ms)

    static const unsigned long SHORT_LATENCY_BASE_MS = 48UL;

    // Systemarray-Layout ab MBC_S_GENPURP (verbindlich aus mbc_modul_93.vb/mbc_defs.h,
    // genPurpSize=4): STATUS, SW_1, SW_2, SH - PC_ARRAY_DATA-kompatibel, daher
    // systemarray-backed statt eigener RAM-Member (gleiches Muster wie MBC_80_L88SW_1/2)

    #define MBC_93_S_STATUS   MBC_S_GENPURP
    #define MBC_93_S_SW       (MBC_93_S_STATUS + 1)
    #define MBC_93_S_SH       (MBC_93_S_SW + 2)
    #define MBC_93_STATUS     _utils->array()[MBC_93_S_STATUS]
    #define MBC_93_SW_1       _utils->array()[MBC_93_S_SW]
    #define MBC_93_SW_2       _utils->array()[MBC_93_S_SW + 1]
    #define MBC_93_SH         _utils->array()[MBC_93_S_SH]

//------ Ende interne Konstanten -------------------------------------------------------------------

//------ Start Klasse Mbc93Com ---------------------------------------------------------------------

    Mbc93Com *Mbc93Com::_instance = nullptr;   // fuer die statische ISR-Trampolinfunktion

    // PROGMEM: nur einmal hier beim Konstruieren gelesen. Kanalnummer (Byte 0) ist die
    // NEUE, fortlaufende KANAL_*-Nummer dieser Architektur, nicht die alten MARK1/MARK3-
    // Werte der Original-Firmware (siehe Klassenkommentar Mbc93Com.h).

    static const uint8_t initCs2Ex[MBC93_ANZMESSWERTEMODUL + MBC93_ANZKONFIGMODUL]
                                   [MBC_COM_CS2PAKETE * 8] PROGMEM =
    {
        // Zeile 0 - Messwert "ON Track" (0=aus/grau, 1=an/gruen)

        {   KANAL_ON_TRACK,0x00,0x30,0x30,0x30,0x30,0x00,0x00,             // 1...8
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,                      // 9...16
            'O','N',' ','T','r','a','c','k',                              // 17...24
            0x00,'0',0x00,'1',0x00,'#',0x00,0x00,                         // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                       // 33...40
        },

        // Zeile 1 - Messwert "Kurzschluss" (0=aus/grau, 1=an/rot)

        {   KANAL_KURZSCHLUSS,0x00,0xc0,0xc0,0xc0,0xc0,0x00,0x00,          // 1...8
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,                      // 9...16
            'K','u','r','z','s','c','h','l',                              // 17...24
            'u','s','s',0x00,'0',0x00,'1',0x00,                           // 25...32
            '#',0x00,0x00,0x00,0x00,0x00,0x00,0x00                        // 33...40
        },

        // Zeile 2 - Konfigkanal "SW Major:" (Slider 1...255, Kenner 0x02, simulierte
        // GFP/Booster-Softwareversion, analog Mbc80Com fuer L88)

        {   KANAL_SW_MAJOR,0x02,0x00,0x01,0x00,0xff,0x00,0x00,            // 1...8
            'S','W',' ','M','a','j','o','r',                              // 9...16
            ':',0x00,'1',0x00,'2','5','5',0x00,                           // 17...24
            '#',0x00,0x00,0x00,0x00,0x00,0x00,0x00,                       // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                       // 33...40
        },

        // Zeile 3 - Konfigkanal "SW Minor:" (Slider 1...255, Kenner 0x02)

        {   KANAL_SW_MINOR,0x02,0x00,0x01,0x00,0xff,0x00,0x00,            // 1...8
            'S','W',' ','M','i','n','o','r',                              // 9...16
            ':',0x00,'1',0x00,'2','5','5',0x00,                           // 17...24
            '#',0x00,0x00,0x00,0x00,0x00,0x00,0x00,                       // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                       // 33...40
        },

        // Zeile 4 - Konfigkanal "KS x 50 ms:" (Slider 1...20, Kenner 0x02)

        {   KANAL_KS_LATENZ,0x02,0x00,0x01,0x00,0x14,0x00,0x00,           // 1...8
            'K','S',' ','x',' ','5','0',' ',                              // 9...16
            'm','s',':',0x00,'1',0x00,'2','0',                            // 17...24
            0x00,'#',0x00,0x00,0x00,0x00,0x00,0x00,                       // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                       // 33...40
        }
    };

    // PRIO,CAN_ID,HASH_H,HASH_L,DLC,D0,D1,D2,D3,D4,D5,D6,D7 - 1:1 aus can_msg_booster_*
    // in mbc_defs.c uebernommen (nur die ersten 13 Byte, das alte 17-Byte-Format dieser
    // Architektur hatte zusaetzliche, hier entfallende ID_1/ID_2/COMMAND/RESPONSE-Felder)

    static const uint8_t initMsgBoosterStop[MBC_COM_MSG_LAENGE] PROGMEM =
        {0x00,0x00,0x03,0x00,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

    static const uint8_t initMsgBoosterStopResponse[MBC_COM_MSG_LAENGE] PROGMEM =
        {0x00,0x01,0x03,0x00,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

    static const uint8_t initMsgBoosterGoResponse[MBC_COM_MSG_LAENGE] PROGMEM =
        {0x00,0x01,0x03,0x00,0x05,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00};

    static const uint8_t initMsgKonfigOk[MBC_COM_MSG_LAENGE] PROGMEM =
        {0x00,0x01,0x03,0x00,0x07,0x00,0x00,0x00,0x00,0x0b,0x00,0x01,0x00};

    //*********************************************************************************************
    //* Konstruktor zur Deklaration von Objektvariablen                                           *
    //*********************************************************************************************
    //* utils:         zugehoerige MbcUtils-Instanz                                               *
    //* can:           zugehoeriger CAN-Kanal dieses Moduls (Basisklassen-Parameter can)          *
    //* bootStorage:   zugehoerige MbcBootStorage-Instanz (Firmware-Update-Staging)               *
    //* pinOn:         Ausgangsbit Booster an (Original PORTD Bit0)                               *
    //* pinShort:      Ausgangsbit Kurzschluss signalisieren (Original PORTD Bit1)                *
    //* pinShortDetect: nativer, interruptfaehiger Digitalpin der Kurzschlusserkennung            *
    //* pinFan:        nativer PWM-Pin des Luefters                                               *
    //* pinReset:      NUR auf AVR (ATmega644P) vorhanden, siehe MbcCom-Konstruktor               *
    //*********************************************************************************************

    #if defined(__AVR__)

        Mbc93Com::Mbc93Com(MbcUtils &utils, MbcCan &can, MbcBootStorage &bootStorage,
                            MbcDigitalOut &pinOn, MbcDigitalOut &pinShort,
                            uint8_t pinShortDetect, uint8_t pinFan, uint8_t pinReset)
            : MbcCom(utils, can, bootStorage, pinReset,
                     MBC93_ANZMESSWERTEMODUL, MBC93_ANZKONFIGMODUL),
              _pinOn(&pinOn), _pinShort(&pinShort), _pinShortDetect(pinShortDetect),
              _pinFan(pinFan), _boosterOn(false), _shortArmed(false), _shortStartMs(0)
        {
            _instance = this;

            memcpy_P(_msgBoosterStop, initMsgBoosterStop, MBC_COM_MSG_LAENGE);
            memcpy_P(_msgBoosterStopResponse, initMsgBoosterStopResponse, MBC_COM_MSG_LAENGE);
            memcpy_P(_msgBoosterGoResponse, initMsgBoosterGoResponse, MBC_COM_MSG_LAENGE);
            memcpy_P(_msgKonfigOk, initMsgKonfigOk, MBC_COM_MSG_LAENGE);
        }

    #else

        Mbc93Com::Mbc93Com(MbcUtils &utils, MbcCan &can, MbcBootStorage &bootStorage,
                            MbcDigitalOut &pinOn, MbcDigitalOut &pinShort,
                            uint8_t pinShortDetect, uint8_t pinFan)
            : MbcCom(utils, can, bootStorage,
                     MBC93_ANZMESSWERTEMODUL, MBC93_ANZKONFIGMODUL),
              _pinOn(&pinOn), _pinShort(&pinShort), _pinShortDetect(pinShortDetect),
              _pinFan(pinFan), _boosterOn(false), _shortArmed(false), _shortStartMs(0)
        {
            _instance = this;

            memcpy_P(_msgBoosterStop, initMsgBoosterStop, MBC_COM_MSG_LAENGE);
            memcpy_P(_msgBoosterStopResponse, initMsgBoosterStopResponse, MBC_COM_MSG_LAENGE);
            memcpy_P(_msgBoosterGoResponse, initMsgBoosterGoResponse, MBC_COM_MSG_LAENGE);
            memcpy_P(_msgKonfigOk, initMsgKonfigOk, MBC_COM_MSG_LAENGE);
        }

    #endif

    //*********************************************************************************************
    //* Einmalig nach dem begin() der Basisklasse aufrufen: Ausgangsbit-Pins/Luefter-Pin          *
    //* konfigurieren, Kurzschluss-Interrupt scharfschalten, gespeicherte Parameter aus der       *
    //* EEPROM-Zusatzregion laden.                                                                *
    //*********************************************************************************************

    void Mbc93Com::beginModule()
    {
        _pinOn->begin();
        _pinShort->begin();

        pinMode(_pinFan, OUTPUT);
        pinMode(_pinShortDetect, INPUT);

        attachInterrupt(digitalPinToInterrupt(_pinShortDetect), shortIsr, CHANGE);

        eepromReadParaExtern();
    }

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //+ Die 7 Erweiterungspunkte der Basisklasse (siehe MbcCom.h) - hier mbc-93-spezifisches      +
    //+ EEPROM-/CS2-Verhalten ergaenzen                                                           +
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    //*********************************************************************************************
    //* Modulspezifische Parameterdaten aus dem EEPROM lesen. Laedt Softwareversion (Default 3/85 *
    //* bei unbeschriebenem EEPROM) und Kurzschluss-Latenzzeit (Rohwert >= KS_LATENZ_MAX wird auf *
    //* 0 korrigiert - abweichend vom Original, das grosszuegiger ">100" prueft: da der gueltige  *
    //* Bereich ueber CS2 UND ParaCenter auf 0...KS_LATENZ_MAX-1 begrenzt ist, waere ">100" ein   *
    //* zu lascher EEPROM-Korruptionsschutz - NICHT sofort zurueckgeschrieben), aktualisiert die  *
    //* Konfigkanal-Anzeige und die simulierte Softwareversion in der CS2-Ping-Antwort            *
    //* (swInitExtern()) - MbcCom::begin() ruft hashInit()/swInitExtern() bereits VOR diesem      *
    //* beginModule()-Aufruf auf, zu diesem fruehen Zeitpunkt stehen die EEPROM-Werte aber noch   *
    //* nicht fest.                                                                               *
    //*********************************************************************************************

    void Mbc93Com::eepromReadParaExtern()
    {
        uint8_t buf[2];

        _utils->eepromReadExtra(EEX_93_SW, buf, 2);

        if (buf[0] == 0xff)
        {
            buf[0] = 3;
            buf[1] = 85;

            _utils->eepromWriteExtra(EEX_93_SW, buf, 2);
        }

        MBC_93_SW_1 = buf[0];
        MBC_93_SW_2 = buf[1];

        uint8_t sh;

        _utils->eepromReadExtra(EEX_93_SH, &sh, 1);

        if (sh >= KS_LATENZ_MAX) sh = 0;

        MBC_93_SH = sh;

        swInitExtern();
    }

    //*********************************************************************************************
    //* Modulspezifische Parameterdaten in das EEPROM schreiben - Softwareversion und             *
    //* Kurzschluss-Latenzzeit liegen in getrennten EEPROM-Bloecken (Original: EE_93_SW/EE_93_SH) *
    //* und werden unabhaengig voneinander persistiert.                                           *
    //*********************************************************************************************
    //* block: Identifier des zu schreibenden Blocks (0x0A/0x0B)                                  *
    //*********************************************************************************************

    void Mbc93Com::eepromWriteParaExtern(uint8_t block)
    {
        if (block == EE_93_SW)
        {
            uint8_t buf[2] = {MBC_93_SW_1, MBC_93_SW_2};

            _utils->eepromWriteExtra(EEX_93_SW, buf, 2);

            swInitExtern();
        }
        else if (block == EE_93_SH)
        {
            uint8_t sh = MBC_93_SH;

            _utils->eepromWriteExtra(EEX_93_SH, &sh, 1);
        }
    }

    //*********************************************************************************************
    //* Modulspezifische Parameterdaten im EEPROM loeschen - mbc-93 hat nichts zu loeschen        *
    //* (Original ebenfalls leer).                                                                *
    //*********************************************************************************************

    void Mbc93Com::eepromEraseParaExtern()
    {
    }

    //*********************************************************************************************
    //* Zusaetzliche modulspezifische can_msg[]/cs2_index[]-Eintraege nach HASH-Neuberechnung     *
    //* nachziehen - traegt GUiD/HASH in die 3 "echten" Antwort-/Bestaetigungsnachrichten ein.    *
    //* _msgBoosterStop (spontanes Kurzschluss-Stopp) bekommt bewusst NUR den HASH - seine GUiD   *
    //* bleibt die universelle Broadcast-GUiD 0x00000000 (Original:                               *
    //* mbc_interface_hash_init_extern() setzt hier ebenfalls keine GUiD, 1:1 uebernommen).       *
    //*********************************************************************************************

    void Mbc93Com::hashInitExtern()
    {
        uint8_t *arr = _utils->array();

        _msgBoosterStopResponse[MBC_CAN_D0] = arr[MBC_S_GUID];
        _msgBoosterStopResponse[MBC_CAN_D1] = arr[MBC_S_GUID + 1];
        _msgBoosterStopResponse[MBC_CAN_D2] = arr[MBC_S_GUID + 2];
        _msgBoosterStopResponse[MBC_CAN_D3] = arr[MBC_S_GUID + 3];

        _msgBoosterGoResponse[MBC_CAN_D0] = arr[MBC_S_GUID];
        _msgBoosterGoResponse[MBC_CAN_D1] = arr[MBC_S_GUID + 1];
        _msgBoosterGoResponse[MBC_CAN_D2] = arr[MBC_S_GUID + 2];
        _msgBoosterGoResponse[MBC_CAN_D3] = arr[MBC_S_GUID + 3];

        _msgKonfigOk[MBC_CAN_D0] = arr[MBC_S_GUID];
        _msgKonfigOk[MBC_CAN_D1] = arr[MBC_S_GUID + 1];
        _msgKonfigOk[MBC_CAN_D2] = arr[MBC_S_GUID + 2];
        _msgKonfigOk[MBC_CAN_D3] = arr[MBC_S_GUID + 3];

        _msgBoosterStopResponse[MBC_CAN_HASH_H] = can_msg[0][MBC_CAN_HASH_H];
        _msgBoosterStopResponse[MBC_CAN_HASH_L] = can_msg[0][MBC_CAN_HASH_L];

        _msgBoosterGoResponse[MBC_CAN_HASH_H] = can_msg[0][MBC_CAN_HASH_H];
        _msgBoosterGoResponse[MBC_CAN_HASH_L] = can_msg[0][MBC_CAN_HASH_L];

        _msgKonfigOk[MBC_CAN_HASH_H] = can_msg[0][MBC_CAN_HASH_H];
        _msgKonfigOk[MBC_CAN_HASH_L] = can_msg[0][MBC_CAN_HASH_L];

        _msgBoosterStop[MBC_CAN_HASH_H] = can_msg[0][MBC_CAN_HASH_H];
        _msgBoosterStop[MBC_CAN_HASH_L] = can_msg[0][MBC_CAN_HASH_L];
    }

    //*********************************************************************************************
    //* Softwareversion fuer CS2-Ping-Antwort auf die simulierte GFP/Booster-SW-Version setzen.   *
    //*********************************************************************************************

    void Mbc93Com::swInitExtern()
    {
        can_msg[MBC_COM_MSG_CS2_0x30_R][MBC_CAN_D4] = MBC_93_SW_1;
        can_msg[MBC_COM_MSG_CS2_0x30_R][MBC_CAN_D5] = MBC_93_SW_2;
    }

    //*********************************************************************************************
    //* Modulspezifischen Messwert an die CS2 senden: KANAL_ON_TRACK/KANAL_KURZSCHLUSS liefern    *
    //* den jeweiligen Statusbit aus MBC_93_STATUS als 0/1, andere Kanalnummern werden ignoriert. *
    //*********************************************************************************************
    //* kanal: Messkanalnummer                                                                    *
    //*********************************************************************************************

    void Mbc93Com::cs2IndexMesswerteExtern(uint8_t kanal)
    {
        if ((kanal != KANAL_ON_TRACK) && (kanal != KANAL_KURZSCHLUSS)) return;

        can_msg[MBC_COM_MSG_CS2_0x00_R_8_0x0B][MBC_CAN_D5] = kanal;
        can_msg[MBC_COM_MSG_CS2_0x00_R_8_0x0B][MBC_CAN_D6] = 0;

        bool aktiv = (kanal == KANAL_ON_TRACK) ? ((MBC_93_STATUS & STATUS_ON_OK) != 0)
                                                : ((MBC_93_STATUS & STATUS_OFF_SHORT) != 0);

        can_msg[MBC_COM_MSG_CS2_0x00_R_8_0x0B][MBC_CAN_D7] = aktiv ? 0x01 : 0x00;

        sendMessage(can_msg[MBC_COM_MSG_CS2_0x00_R_8_0x0B]);
    }

    //*********************************************************************************************
    //* Modulspezifischen Parameterwert von der CS2 uebernehmen: msg[D5]==KANAL_SW_MAJOR/         *
    //* KANAL_SW_MINOR (simulierte GFP/Booster-Softwareversion, je 1...255, analog Mbc80Com fuer  *
    //* L88) oder KANAL_KS_LATENZ (Kurzschluss-Latenzzeit, 1...20, entspricht 50...1000 ms in     *
    //* 50-ms-Schritten - gegen dieselben Grenzen wie im ParaCenter nud_mbc93_Latenz geprueft,    *
    //* siehe KS_LATENZ_MIN/MAX oben). Der Rohwert (16 Bit, msg[D6]=High/msg[D7]=Low) wird gegen  *
    //* die jeweils gueltigen Grenzen geprueft; ein Wert ausserhalb bleibt ohne Wirkung und wird  *
    //* per FALSE quittiert - anders als PC_ARRAY_DATA, das unveraendert (ohne Bereichspruefung)  *
    //* uebernimmt, siehe MBCAN CS2-Reject-vs-ParaCenter-Clamp-Konvention.                        *
    //*********************************************************************************************
    //* msg: CAN-Nachricht (msg[D5]==Kanal, msg[D6..D7]=neuer Rohwert)                            *
    //*********************************************************************************************

    void Mbc93Com::cs2IndexParameterExtern(const uint8_t msg[MBC_COM_MSG_LAENGE])
    {
        uint8_t  kanal = msg[MBC_CAN_D5];
        uint16_t wert  = (uint16_t) (((uint16_t) msg[MBC_CAN_D6] << 8) | msg[MBC_CAN_D7]);
        bool     gueltig;
        bool     geaendert = false;

        if (kanal == KANAL_SW_MAJOR)
        {
            gueltig = (wert >= SW_VAL_MIN) && (wert <= SW_VAL_MAX);

            if (gueltig && (MBC_93_SW_1 != (uint8_t) wert)) { MBC_93_SW_1 = (uint8_t) wert; geaendert = true; }
        }
        else if (kanal == KANAL_SW_MINOR)
        {
            gueltig = (wert >= SW_VAL_MIN) && (wert <= SW_VAL_MAX);

            if (gueltig && (MBC_93_SW_2 != (uint8_t) wert)) { MBC_93_SW_2 = (uint8_t) wert; geaendert = true; }
        }
        else if (kanal == KANAL_KS_LATENZ)
        {
            gueltig = (wert >= KS_LATENZ_MIN) && (wert <= KS_LATENZ_MAX);

            if (gueltig && (MBC_93_SH != (uint8_t) (wert - 1))) { MBC_93_SH = (uint8_t) (wert - 1); geaendert = true; }
        }
        else
        {
            return;
        }

        if (geaendert) eepromWriteParaExtern((kanal == KANAL_KS_LATENZ) ? EE_93_SH : EE_93_SW);

        _msgKonfigOk[MBC_CAN_D5] = kanal;
        _msgKonfigOk[MBC_CAN_D6] = gueltig ? 0x01 : 0x00;

        sendMessage(_msgKonfigOk);
    }

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //+ Zusaetzliche, mbc-93-spezifische Erweiterungspunkte (siehe Kopfkommentar von MbcCom.h)    +
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    //*********************************************************************************************
    //* Feste CS2-Geraetekennung 0x0000 (Maerklin-Vorgabe fuer Booster-Geraete wie mbc-93)        *
    //*********************************************************************************************
    //* knH/knL: gesetzt auf 0x00/0x00                                                            *
    //*********************************************************************************************
    //* Return: true (mbc-93 hat immer eine feste Kennung)                                        *
    //*********************************************************************************************

    bool Mbc93Com::kennerFixedExtern(uint8_t &knH, uint8_t &knL)
    {
        knH = 0x00;
        knL = 0x00;
        return true;
    }

    //*********************************************************************************************
    //* Baut die angeforderte Kanalzeile frisch in _cs2ScratchRow zusammen - mbc-93 hat fuenf     *
    //* Kanaele (ON Track, Kurzschluss, SW Major, SW Minor, KS-Latenzzeit), sonst nullptr. Die    *
    //* beiden Messwertzeilen (ROW_ON_TRACK/ROW_KURZSCHLUSS) brauchen keinen Live-Patch - ihr     *
    //* aktueller Zustand geht ausschliesslich ueber cs2IndexMesswerteExtern() als eigene         *
    //* Nachricht raus, nicht ueber diese Zeile. Vorlage (initCs2Ex[], PROGMEM) bleibt statisch,  *
    //* nur der aktuelle Wert der 3 Konfigkanaele wird bei jedem Aufruf frisch aus dem            *
    //* Systemarray gepatcht.                                                                      *
    //*********************************************************************************************
    //* laufendeNr: 0..ANZMESSWERTEMODUL+ANZKONFIGMODUL-1                                         *
    //*********************************************************************************************
    //* Return: Zeiger auf _cs2ScratchRow (nur bis zum naechsten Aufruf gueltig) oder nullptr     *
    //*********************************************************************************************

    const uint8_t* Mbc93Com::cs2IndexExtern(uint8_t laufendeNr)
    {
        if (laufendeNr >= (MBC93_ANZMESSWERTEMODUL + MBC93_ANZKONFIGMODUL)) return nullptr;

        memcpy_P(_cs2ScratchRow, initCs2Ex[laufendeNr], sizeof(_cs2ScratchRow));

        if (laufendeNr == ROW_SW_MAJOR)
        {
            _cs2ScratchRow[OFFSET_SWVAL] = MBC_93_SW_1;
        }
        else if (laufendeNr == ROW_SW_MINOR)
        {
            _cs2ScratchRow[OFFSET_SWVAL] = MBC_93_SW_2;
        }
        else if (laufendeNr == ROW_KS_LATENZ)
        {
            _cs2ScratchRow[OFFSET_KSLAT] = (uint8_t) (MBC_93_SH + 1);
        }

        return _cs2ScratchRow;
    }

    //*********************************************************************************************
    //* Je loop()-Durchlauf aufrufen: Kurzschluss-Latenzzeit pruefen, Luefter nachfuehren, CAN-   *
    //* Nachrichtenempfang/-Auswertung. Eingehende System-Stopp-/Go-Telegramme (CAN-ID 0x00)      *
    //* werden direkt hier ausgewertet, unabhaengig von der PC-/CS2-Dispatch-Logik der            *
    //* Basisklasse (check()).                                                                    *
    //*********************************************************************************************

    void Mbc93Com::update()
    {
        poll();

        shortCheck();
        fanUpdate();

        uint8_t msg[MBC_CAN_MSG_LEN];

        if (_can->receive(msg))
        {
            check(msg);

            if (msg[MBC_CAN_ID] == CAN_ID_SYSTEM_BEFEHLE) handleSystemCommand(msg);
        }
    }

    //*********************************************************************************************
    //* Statische ISR-Trampolinfunktion fuer attachInterrupt() (nimmt keine Objektmethoden direkt *
    //* an) - Ersatz fuer SIGNAL(INT1_vect): steigende Flanke startet die Latenzzeitmessung       *
    //* (Zeitstempel + Arm-Flag), eine fallende Flanke vor Ablauf bricht sie ab.                  *
    //*********************************************************************************************

    void Mbc93Com::shortIsr()
    {
        if (_instance == nullptr) return;

        if (digitalRead(_instance->_pinShortDetect) == HIGH)
        {
            _instance->_shortStartMs = millis();
            _instance->_shortArmed   = true;
        }
        else
        {
            _instance->_shortArmed = false;
        }
    }

    //*********************************************************************************************
    //* Kurzschluss-Latenzzeit pruefen - Ersatz fuer SIGNAL(TIMER1_OVF_vect) plus die Auswertung  *
    //* in main(): ist die Latenzzeit seit der letzten steigenden Flanke verstrichen, ohne dass   *
    //* zwischenzeitlich eine fallende Flanke die Messung abgebrochen hat, gilt ein echter        *
    //* Kurzschluss als erkannt - der Booster wird abgeschaltet und (nur bei initDone(), wie im   *
    //* Original) ein spontanes System-Stopp-Telegramm mit universeller Broadcast-GUiD gesendet.  *
    //*********************************************************************************************

    void Mbc93Com::shortCheck()
    {
        if (!_shortArmed) return;

        unsigned long latenzMs = SHORT_LATENCY_BASE_MS * (MBC_93_SH + 1UL);

        if ((millis() - _shortStartMs) < latenzMs) return;

        _shortArmed = false;

        if (!_boosterOn) return;

        _pinOn->write(false);
        _pinShort->write(true);

        _boosterOn = false;

        MBC_93_STATUS = STATUS_OFF_SHORT;

        if (initDone()) sendMessage(_msgBoosterStop);
    }

    //*********************************************************************************************
    //* Luefter entsprechend der 1-Wire-Temperatur einstellen (1:1 aus mbc_fan_set() uebernommen, *
    //* per analogWrite() statt roher Timer2-PWM): < 20 Grad C = 55/255, 20...60 Grad C linear    *
    //* 55->255, > 60 Grad C = 255/255.                                                           *
    //*********************************************************************************************

    void Mbc93Com::fanUpdate()
    {
        float temp = _utils->owiTemperature();
        uint8_t duty;

        if (temp < 20.0f)
        {
            duty = 55;
        }
        else if (temp < 60.0f)
        {
            duty = (uint8_t) (55.0f + (temp - 20.0f) * 5.0f);
        }
        else
        {
            duty = 255;
        }

        analogWrite(_pinFan, duty);
    }

    //*********************************************************************************************
    //* System-Stopp/Go auswerten (CAN-ID 0x00) - 1:1 aus main()s Empfangsblock uebernommen,      *
    //* inklusive eines auffaelligen Vergleichs: die modulbezogene GUiD-Pruefung vergleicht D2    *
    //* gegen dieselbe GUiD-Stelle wie D1 statt gegen arr[MBC_S_GUID+2] - ein Copy-Paste-Fehler   *
    //* der Original-Firmware, bewusst NICHT korrigiert (siehe Klassenkommentar/Portierungsplan). *
    //* Reagiert nur, wenn die GUiD universell (D0-D3 alle 0) oder modulbezogen ist UND           *
    //* initDone() (entspricht Original com.init.f_init).                                         *
    //*********************************************************************************************
    //* msg: empfangene CAN-Nachricht                                                             *
    //*********************************************************************************************

    void Mbc93Com::handleSystemCommand(const uint8_t msg[MBC_CAN_MSG_LEN])
    {
        if (!initDone()) return;

        uint8_t *arr = _utils->array();

        bool istUniversell = (msg[MBC_CAN_D0] == 0x00) && (msg[MBC_CAN_D1] == 0x00)
                          && (msg[MBC_CAN_D2] == 0x00) && (msg[MBC_CAN_D3] == 0x00);

        bool istModul = (msg[MBC_CAN_D0] == arr[MBC_S_GUID])
                     && (msg[MBC_CAN_D1] == arr[MBC_S_GUID + 1])
                     && (msg[MBC_CAN_D2] == arr[MBC_S_GUID + 1])
                     && (msg[MBC_CAN_D3] == arr[MBC_S_GUID + 3]);

        if (!istUniversell && !istModul) return;

        if (msg[MBC_CAN_D4] == 0x00)
        {
            _pinOn->write(false);
            _pinShort->write(false);

            _boosterOn = false;

            MBC_93_STATUS = STATUS_OFF_OK;

            sendMessage(_msgBoosterStopResponse);
        }
        else if (msg[MBC_CAN_D4] == 0x01)
        {
            _pinOn->write(true);
            _pinShort->write(false);

            _boosterOn  = true;
            _shortArmed = false;

            MBC_93_STATUS = STATUS_ON_OK;

            sendMessage(_msgBoosterGoResponse);
        }
    }

//------ Ende Klasse Mbc93Com ----------------------------------------------------------------------

//###### Ende Quellcode Mbc93Com.cpp ###############################################################

