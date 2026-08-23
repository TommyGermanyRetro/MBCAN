//*************************************************************************************************
//* Modul mbc-80: MBCAN-Bus <-> CS2-Bus <-> WLAN-Gateway (Mbc80Lib)                               *
//*************************************************************************************************
//* Datei: Mbc80Com.cpp                                                                           *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen:                                                                                  *
//* V1.0.0 - 05.08.2026 - Nullversion                                                             *
//*************************************************************************************************

//###### Start Quellcode Mbc80Com.cpp #############################################################

#include "Mbc80Com.h"

//------ Start interne Konstanten -----------------------------------------------------------------

    // CAN-Befehlsnummern (CAN-ID gemaess Maerklin-Definition), fuer die eigene, von der
    // MbcCom-Basisklasse unabhaengige MBCAN<->CS2<->WLAN-Weiterleitung dieses Gateway-Moduls

    static const uint8_t CAN_ID_SYSTEM_BEFEHLE          = 0x00;
    static const uint8_t CAN_ID_SYSTEM_BEFEHLE_RESPONSE = 0x01;
    static const uint8_t CAN_ID_ZUBEHOER_SCHALTEN       = 0x16;
    static const uint8_t CAN_ID_S88_EVENT               = 0x22;
    static const uint8_t CAN_ID_S88_EVENT_RESPONSE      = 0x23;
    static const uint8_t CAN_ID_SYSTEM_BEFEHLE_SUB_KN   = 0x0C;
    static const uint8_t CAN_LOCID_MBCAN                = 0x18;
    static const uint8_t CAN_ID_STREAMING               = 0x42;
    static const uint8_t CAN_ID_STREAMING_REQUEST       = 0x40;

    // Modulspezifischer EEPROM-Blockindex

    static const uint8_t EE_L88_SW = 0x0C;

    // Byteoffset (2 Byte) in der EEPROM-Zusatzregion

    static const uint16_t EEX_L88SW = 0;

    // CS2-Kanalnummern/Zeilenindizes der modulspezifischen Konfigkanaele "SW Major"/"SW Minor" -
    // beide teilen sich denselben EEPROM-Block EE_L88_SW (MBC_80_L88SW_1/2 werden immer
    // gemeinsam persistiert)

    static const uint8_t KANAL_L88SW_MAJ = MBC_COM_ANZMESSWERTE + MBC_COM_ANZKONFIG + 1;  // 7
    static const uint8_t KANAL_L88SW_MIN = KANAL_L88SW_MAJ + 1;                           // 8
    static const uint8_t ROW_L88SW_MAJ   = 0;
    static const uint8_t ROW_L88SW_MIN   = 1;
    static const uint8_t L88SW_OFFSET_H  = 6;   // "Aktueller Wert High" innerhalb der Zeile
    static const uint8_t L88SW_OFFSET_L  = 7;   // "Aktueller Wert Low" innerhalb der Zeile

    // Systemarray-Layout ab MBC_S_GENPURP (Kompatibilitaet zum Parametriercenter, MBC_80_S_BT_PW
    // ff.) - die ersten 9 Byte (BT-Passwort/BT-Name) sind ein Relikt einer aelteren
    // Bluetooth-Hardwarerevision und bleiben ungenutzt, muessen aber als Luecke erhalten bleiben,
    // da das Parametriercenter L88SW_1/2 fest an Offset 9/10 erwartet - sonst liefert die
    // Parametriercenter-Seite (PC_ARRAY_DATA) dort immer 0 zurueck, auch wenn der CS2-Konfigkanal
    // (cs2IndexParameterExtern()) bereits korrekt funktioniert

    #define MBC_80_S_MM      MBC_S_GENPURP
    #define MBC_80_S_L88SW   (MBC_80_S_MM + 9)
    #define MBC_80_L88SW_1   _utils->array()[MBC_80_S_L88SW]
    #define MBC_80_L88SW_2   _utils->array()[MBC_80_S_L88SW + 1]

//------ Ende interne Konstanten ------------------------------------------------------------------

//------ Start Klasse Mbc80Com --------------------------------------------------------------------

    // PROGMEM: nur einmal hier beim Konstruieren gelesen

    static const uint8_t initCs2Ex[MBC80_ANZMESSWERTEMODUL + MBC80_ANZKONFIGMODUL]
                                   [MBC_COM_CS2PAKETE * 8] PROGMEM =
    {
        // INDEX_CTRL 1.EX.1 - vorgetaeuschte L88-Softwareversion, Major-Byte (Slider, 1...255)

        {   KANAL_L88SW_MAJ,0x02,0x00,0x01,0x00,0xff,0x00,0x00,          // 1...8
            'S','W',' ','M','a','j','o','r',                             // 9...16
            ':',0x00,'1',0x00,'2','5','5',0x00,                          // 17...24
            '#',0x00,0x00,0x00,0x00,0x00,0x00,0x00,                      // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                      // 33...40
        },

        // INDEX_CTRL 1.EX.2 - vorgetaeuschte L88-Softwareversion, Minor-Byte (Slider, 1...255)

        {   KANAL_L88SW_MIN,0x02,0x00,0x01,0x00,0xff,0x00,0x00,          // 1...8
            'S','W',' ','M','i','n','o','r',                             // 9...16
            ':',0x00,'1',0x00,'2','5','5',0x00,                          // 17...24
            '#',0x00,0x00,0x00,0x00,0x00,0x00,0x00,                      // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                      // 33...40
        }
    };

    static const uint8_t initMsgKonfigOk[MBC_COM_MSG_LAENGE] PROGMEM =
        {0x00,0x01,0x00,0x00,0x07,0x00,0x00,0x00,0x00,0x0b,0x00,0x01,0x00};

    //*********************************************************************************************
    //* Konstruktor zur Deklaration von Objektvariablen                                           *
    //*********************************************************************************************
    //* utils:       zugehoerige MbcUtils-Instanz                                                 *
    //* canCs2:      CAN-Kanal zum Maerklin-CS2-Bus (= can-Parameter der Basisklasse)             *
    //* canMbcan:    CAN-Kanal zum modulinternen MBCAN-Bus                                        *
    //* bootStorage: zugehoerige MbcBootStorage-Instanz (Firmware-Update-Staging)                 *
    //* bridge:      zugehoerige Nachrichten-Bruecke (MbcWlanBridge, MbcWifiBridge, ...)          *
    //* pinReset:    NUR auf AVR (ATmega644P) vorhanden, siehe MbcCom-Konstruktor                 *
    //*********************************************************************************************

    #if defined(__AVR__)

        Mbc80Com::Mbc80Com(MbcUtils &utils, MbcCan &canCs2, MbcCan &canMbcan, MbcBootStorage &bootStorage,
                            MbcMsgBridge &bridge, uint8_t pinReset)
            : MbcCom(utils, canCs2, bootStorage, pinReset,
                     MBC80_ANZMESSWERTEMODUL, MBC80_ANZKONFIGMODUL),
              _canMbcan(&canMbcan), _bridge(&bridge), _baseAddr(0), _autoCnt(0), _streaming(false)
        {
            memcpy_P(_cs2IndexEx, initCs2Ex, sizeof(_cs2IndexEx));
            memcpy_P(_msgKonfigOk, initMsgKonfigOk, MBC_COM_MSG_LAENGE);
        }

    #else

        Mbc80Com::Mbc80Com(MbcUtils &utils, MbcCan &canCs2, MbcCan &canMbcan, MbcBootStorage &bootStorage,
                            MbcMsgBridge &bridge)
            : MbcCom(utils, canCs2, bootStorage,
                     MBC80_ANZMESSWERTEMODUL, MBC80_ANZKONFIGMODUL),
              _canMbcan(&canMbcan), _bridge(&bridge), _baseAddr(0), _autoCnt(0), _streaming(false)
        {
            memcpy_P(_cs2IndexEx, initCs2Ex, sizeof(_cs2IndexEx));
            memcpy_P(_msgKonfigOk, initMsgKonfigOk, MBC_COM_MSG_LAENGE);
        }

    #endif

    //*********************************************************************************************
    //* Einmalig nach dem begin() der Basisklasse aufrufen: gespeicherte L88-Softwareversion aus  *
    //* der EEPROM-Zusatzregion laden                                                             *
    //*********************************************************************************************

    void Mbc80Com::beginModule()
    {
        eepromReadParaExtern();
    }

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //+ Die 7 Erweiterungspunkte der Basisklasse (siehe MbcCom.h fuer die volle Beschreibung je   +
    //+ Hook) - hier anpassen, um mbc-80-spezifisches EEPROM-/CS2-Verhalten zu ergaenzen          +
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    //*********************************************************************************************
    //* Modulspezifische Parameterdaten aus dem EEPROM lesen                                      *
    //* Laedt die gespeicherte L88-Softwareversion aus der EEPROM-Zusatzregion (mit Defaultwert   *
    //* [1,1] fuer den unbeschriebenen Zustand, 0xff-Sentinel), aktualisiert die SW-Major-/       *
    //* SW-Minor-Anzeige in _cs2IndexEx UND die CS2-Ping-Antwort (swInitExtern()) - MbcCom::      *
    //* begin() ruft hashInit()/swInitExtern() bereits VOR diesem beginModule()-Aufruf auf, zu    *
    //* diesem fruehen Zeitpunkt stehen die EEPROM-Werte in MBC_80_L88SW_1/2 aber noch nicht;     *
    //* ohne den erneuten swInitExtern()-Aufruf hier wuerde die Ping-Antwort bis zur ersten       *
    //* Aenderung einen undefinierten Startwert zeigen                                            *
    //*********************************************************************************************

    void Mbc80Com::eepromReadParaExtern()
    {
        uint8_t buf[2];

        _utils->eepromReadExtra(EEX_L88SW, buf, 2);

        MBC_80_L88SW_1 = (buf[0] == 0xff) ? 1 : buf[0];
        MBC_80_L88SW_2 = (buf[1] == 0xff) ? 1 : buf[1];

        _cs2IndexEx[ROW_L88SW_MAJ][L88SW_OFFSET_H] = 0;
        _cs2IndexEx[ROW_L88SW_MAJ][L88SW_OFFSET_L] = MBC_80_L88SW_1;
        _cs2IndexEx[ROW_L88SW_MIN][L88SW_OFFSET_H] = 0;
        _cs2IndexEx[ROW_L88SW_MIN][L88SW_OFFSET_L] = MBC_80_L88SW_2;

        swInitExtern();
    }

    //*********************************************************************************************
    //* Modulspezifische Parameterdaten in das EEPROM schreiben                                   *
    //* Persistiert die L88-SW-Version in der EEPROM-Zusatzregion, haelt can_msg[] konsistent zum *
    //* RAM-Wert und aktualisiert die SW-Major-/SW-Minor-Anzeige in _cs2IndexEx                   *
    //*********************************************************************************************
    //* block: Identifier des zu schreibenden Blocks (0x0A....)                                   *
    //*********************************************************************************************

    void Mbc80Com::eepromWriteParaExtern(uint8_t block)
    {
        if (block == EE_L88_SW)
        {
            uint8_t buf[2] = {MBC_80_L88SW_1, MBC_80_L88SW_2};

            _utils->eepromWriteExtra(EEX_L88SW, buf, 2);
            swInitExtern();

            _cs2IndexEx[ROW_L88SW_MAJ][L88SW_OFFSET_L] = MBC_80_L88SW_1;
            _cs2IndexEx[ROW_L88SW_MIN][L88SW_OFFSET_L] = MBC_80_L88SW_2;
        }
    }

    //*********************************************************************************************
    //* Modulspezifische Parameterdaten im EEPROM loeschen                                        *
    //* Deklariert, aber nie aufgerufen                                                           *
    //*********************************************************************************************

    void Mbc80Com::eepromEraseParaExtern()
    {
    }

    //*********************************************************************************************
    //* Zusaetzliche modulspezifische can_msg[]/cs2_index[]-Eintraege nach HASH-Neuberechnung     *
    //* nachziehen - traegt GUiD/HASH in die modulspezifische Konfig-Bestaetigungsnachricht       *
    //* _msgKonfigOk ein.                                                                         *
    //*********************************************************************************************

    void Mbc80Com::hashInitExtern()
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
    //* Softwareversion fuer CS2-Ping-Antwort auf die simulierte L88-SW-Version setzen            *
    //*********************************************************************************************

    void Mbc80Com::swInitExtern()
    {
        can_msg[MBC_COM_MSG_CS2_0x30_R][MBC_CAN_D4] = MBC_80_L88SW_1;
        can_msg[MBC_COM_MSG_CS2_0x30_R][MBC_CAN_D5] = MBC_80_L88SW_2;
    }

    //*********************************************************************************************
    //* Modulspezifischen Messwert an die CS2 senden                                              *
    //* mbc-80 hat keine (ANZMESSWERTEMODUL=0), nichts zu tun                                     *
    //*********************************************************************************************
    //* kanal: Messkanalnummer                                                                    *
    //*********************************************************************************************

    void Mbc80Com::cs2IndexMesswerteExtern(uint8_t kanal)
    {
        (void) kanal;
    }

    //*********************************************************************************************
    //* Modulspezifischen Parameterwert von der CS2 uebernehmen:                                  *
    //* msg[D5]==KANAL_L88SW_MAJ/KANAL_L88SW_MIN: neues Major-/Minor-Byte der vorgetaeuschten     *
    //* L88-Softwareversion (Rohwert 1...255) uebernehmen, ueber eepromWriteParaExtern(EE_L88_SW) *
    //* persistieren (schreibt beide Bytes gemeinsam UND aktualisiert can_msg[] via               *
    //* swInitExtern()) und der CSx bestaetigen. Ein Wert ausserhalb 1...255 bleibt ohne Wirkung  *
    //* und wird per FALSE quittiert. Der Konfigurationswert ist laut CS2-Protokoll ein           *
    //* vollstaendiges Word ueber D6 (High) + D7 (Low), nicht nur D7 - Wertebereich hier stets <= *
    //* 255, D6 des eingehenden Worts daher zwar immer 0, wird aber der Vollstaendigkeit halber   *
    //* mitausgewertet.                                                                           *
    //*********************************************************************************************
    //* msg: CAN-Nachricht (msg[D5]==KANAL_L88SW_MAJ/KANAL_L88SW_MIN, msg[D6..D7]=neuer Wert)     *
    //*********************************************************************************************

    void Mbc80Com::cs2IndexParameterExtern(const uint8_t msg[MBC_COM_MSG_LAENGE])
    {
        uint8_t  kanal = msg[MBC_CAN_D5];
        uint16_t wert  = (uint16_t) (((uint16_t) msg[MBC_CAN_D6] << 8) | msg[MBC_CAN_D7]);
        bool     gueltig;

        if (kanal == KANAL_L88SW_MAJ)
        {
            gueltig = (wert >= 1) && (wert <= 255);

            if (gueltig) MBC_80_L88SW_1 = (uint8_t) wert;
        }
        else if (kanal == KANAL_L88SW_MIN)
        {
            gueltig = (wert >= 1) && (wert <= 255);

            if (gueltig) MBC_80_L88SW_2 = (uint8_t) wert;
        }
        else
        {
            return;
        }

        if (gueltig) eepromWriteParaExtern(EE_L88_SW);

        _msgKonfigOk[MBC_CAN_D5] = kanal;
        _msgKonfigOk[MBC_CAN_D6] = gueltig ? 0x01 : 0x00;

        sendMessage(_msgKonfigOk);
    }

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //+ Zusaetzliche, mbc-80-spezifische Erweiterungspunkte (siehe Kopfkommentar von MbcCom.h)    +
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    //*********************************************************************************************
    //* Feste CS2-Geraetekennung 0x0040 (Maerklin-Vorgabe fuer L88-Automatikgeraete)              *
    //*********************************************************************************************
    //* knH/knL: gesetzt auf 0x00/0x40                                                            *
    //*********************************************************************************************
    //* Return: true (mbc-80 hat immer eine feste Kennung)                                        *
    //*********************************************************************************************

    bool Mbc80Com::kennerFixedExtern(uint8_t &knH, uint8_t &knL)
    {
        knH = 0x00;
        knL = 0x40;
        return true;
    }

    //*********************************************************************************************
    //* Zusaetzlicher Versandweg (Nachrichten-Bruecke) fuer ausgehende Nachrichten der            *
    //* Basisklasse (Modulidentitaets-/Parametrier-Kommunikation)                                 *
    //*********************************************************************************************
    //* msg: Nachrichtenarray                                                                     *
    //*********************************************************************************************

    void Mbc80Com::sendMessageExtern(const uint8_t msg[MBC_COM_MSG_LAENGE])
    {
        _bridge->send(msg);
    }

    //*********************************************************************************************
    //* Zeile aus dem modulspezifischen cs2_index_ex[]-Array liefern - mbc-80 hat zwei Kanaele    *
    //* (SW-Major/SW-Minor der vorgetaeuschten L88-Softwareversion), sonst nullptr                *
    //*********************************************************************************************
    //* laufendeNr: 0..ANZMESSWERTEMODUL+ANZKONFIGMODUL-1                                         *
    //*********************************************************************************************
    //* Return: Zeiger auf die 40-Byte-Zeile oder nullptr                                         *
    //*********************************************************************************************

    const uint8_t* Mbc80Com::cs2IndexExtern(uint8_t laufendeNr)
    {
        if (laufendeNr >= (MBC80_ANZMESSWERTEMODUL + MBC80_ANZKONFIGMODUL)) return nullptr;
        return _cs2IndexEx[laufendeNr];
    }

    //*********************************************************************************************
    //* Je loop()-Durchlauf aufrufen: MBCAN<->CS2<->WLAN-Weiterleitung, zyklische                 *
    //* Schaltdecoder-Basisadress-Rueckmeldung und S88-Automatik-Kenner-Verwaltung.               *
    //*********************************************************************************************

    void Mbc80Com::bridge()
    {
        poll();

        uint8_t msg[MBC_CAN_MSG_LEN];

        if (!_streaming)
        {
            // Zyklische Rueckmeldung der Schaltdecoder-Basisadresse (1x/s) auf den MBCAN-Bus

            if (_utils->eventSekunde())
            {
                can_msg[MBC_COM_MSG_ZYKLUS][MBC_CAN_D6] = (uint8_t) ((_baseAddr & 0xff00) >> 8);
                can_msg[MBC_COM_MSG_ZYKLUS][MBC_CAN_D7] = (uint8_t) (_baseAddr & 0x00ff);

                _canMbcan->send(can_msg[MBC_COM_MSG_ZYKLUS]);
                _bridge->send(can_msg[MBC_COM_MSG_ZYKLUS]);

                _baseAddr = (uint16_t) (_baseAddr + 4);
                if (_baseAddr == 0x400) _baseAddr = 0;
            }

            // Automatik-Kenner bei der CS2 anfragen, solange noch keiner vergeben ist

            requestAutoKenner();

            // MBCAN-Bus: Nachricht abrufen und an CS2 + WLAN weiterreichen

            if (_canMbcan->receive(msg))
            {
                if (msg[MBC_CAN_ID] == CAN_ID_S88_EVENT_RESPONSE)
                {
                    msg[MBC_CAN_D0] = _utils->array()[MBC_S_KN + 2];   // Automatik-Kennung High
                    msg[MBC_CAN_D1] = _utils->array()[MBC_S_KN + 3];   // Automatik-Kennung Low
                }

                _bridge->send(msg);

                if ((msg[MBC_CAN_D2] == CAN_LOCID_MBCAN) && (msg[MBC_CAN_ID] == CAN_ID_ZUBEHOER_SCHALTEN))
                {
                    // Schaltbefehl aus dem Loc-ID-Bereich 0x18xx: nur weiterreichen, wenn eine
                    // echte CS/MS2 verbunden ist (verhindert versehentlichen Not-Stopp)

                    if (csMbcan()) _can->send(msg);
                }
                else
                {
                    _can->send(msg);
                }
            }
        }

        // CS2-Bus: Nachricht abrufen (immer, auch waehrend eines aktiven Streams)

        if (_can->receive(msg))
        {
            _streaming = (msg[MBC_CAN_ID] == CAN_ID_STREAMING);

            if ((msg[MBC_CAN_ID] == CAN_ID_ZUBEHOER_SCHALTEN) && (msg[MBC_CAN_D2] == CAN_LOCID_MBCAN))
            {
                setCsMbcan(true);
            }

            _bridge->send(msg);

            if (!_streaming)
            {
                bool isS88Kommando = (msg[MBC_CAN_ID] == CAN_ID_S88_EVENT)
                    || (((msg[MBC_CAN_ID] & 0xfe) == 0x00) && (msg[MBC_CAN_D4] == CAN_ID_SYSTEM_BEFEHLE_SUB_KN));

                if (isS88Kommando) handleS88(msg);
                else                _canMbcan->send(msg);

                check(msg);
            }
        }

        // WLAN: Nachricht abrufen (immer, auch waehrend eines aktiven Streams)

        if (_bridge->check(msg))
        {
            if (!_streaming || (msg[MBC_CAN_ID] == CAN_ID_STREAMING_REQUEST))
            {
                if (msg[MBC_CAN_ID] != CAN_ID_STREAMING_REQUEST)
                {
                    _canMbcan->send(msg);
                    check(msg);
                }

                if (!((msg[MBC_CAN_D2] == CAN_LOCID_MBCAN) && (msg[MBC_CAN_ID] == CAN_ID_ZUBEHOER_SCHALTEN)))
                {
                    _can->send(msg);
                }
                else
                {
                    setCsMbcan(false);
                }
            }
        }
    }

    //*********************************************************************************************
    //* CS2-S88-Kommando auswerten: Automatik-Kenner-Vergabe/-Entzug uebernehmen und passende     *
    //* S88-Ereignisse an den MBCAN-Bus weiterreichen                                             *
    //*********************************************************************************************
    //* msg: empfangene CAN-Nachricht                                                             *
    //*********************************************************************************************

    void Mbc80Com::handleS88(const uint8_t msg[MBC_CAN_MSG_LEN])
    {
        uint8_t *arr = _utils->array();

        bool isGuid = (msg[MBC_CAN_D0] == arr[MBC_S_GUID])     && (msg[MBC_CAN_D1] == arr[MBC_S_GUID + 1])
                   && (msg[MBC_CAN_D2] == arr[MBC_S_GUID + 2]) && (msg[MBC_CAN_D3] == arr[MBC_S_GUID + 3]);

        bool isGeraet = (msg[MBC_CAN_D0] == can_msg[MBC_COM_MSG_CS2_0x22_R][MBC_CAN_D0])
                     && (msg[MBC_CAN_D1] == can_msg[MBC_COM_MSG_CS2_0x22_R][MBC_CAN_D1]);

        // Anfrage einer gueltigen Kennung mit DLC=5 nach Anmeldung -> antworten, wenn bereits
        // im Automatikmodus

        if ((msg[MBC_CAN_ID] == CAN_ID_SYSTEM_BEFEHLE) && (msg[MBC_CAN_D4] == CAN_ID_SYSTEM_BEFEHLE_SUB_KN) && isGuid)
        {
            if ((msg[MBC_CAN_DLC] == 0x07) && csAutoMode())
            {
                _can->send(can_msg[MBC_COM_MSG_CS2_0x00_R_7_0x0C]);
            }
        }

        // Uebergabe oder Loeschung einer Automatik-Kennung

        if ((msg[MBC_CAN_ID] == CAN_ID_SYSTEM_BEFEHLE_RESPONSE) && (msg[MBC_CAN_D4] == CAN_ID_SYSTEM_BEFEHLE_SUB_KN) && isGuid)
        {
            if (msg[MBC_CAN_DLC] == 0x07)
            {
                if ((msg[MBC_CAN_D5] != 0xff) && (msg[MBC_CAN_D6] != 0xff))
                {
                    can_msg[MBC_COM_MSG_CS2_0x22_R][MBC_CAN_D0] = msg[MBC_CAN_D5];
                    can_msg[MBC_COM_MSG_CS2_0x22_R][MBC_CAN_D1] = msg[MBC_CAN_D6];

                    can_msg[MBC_COM_MSG_CS2_0x00_R_7_0x0C][MBC_CAN_D5] = msg[MBC_CAN_D5];
                    can_msg[MBC_COM_MSG_CS2_0x00_R_7_0x0C][MBC_CAN_D6] = msg[MBC_CAN_D6];

                    arr[MBC_S_KN + 2] = msg[MBC_CAN_D5];   // Automatik-Kennung High
                    arr[MBC_S_KN + 3] = msg[MBC_CAN_D6];   // Automatik-Kennung Low

                    setCsAutoMode(true);

                    _can->send(can_msg[MBC_COM_MSG_CS2_0x00_R_7_0x0C]);
                }
                else
                {
                    setCsAutoMode(false);
                }
            }
        }

        // S88-Ereignis mit passender Geraetekennung im Automatikmodus -> an MBCAN weiterreichen

        if ((msg[MBC_CAN_ID] == CAN_ID_S88_EVENT) && isGeraet && csAutoMode())
        {
            _canMbcan->send(msg);
        }
    }

    //*********************************************************************************************
    //* Automatik-Kenner bei der CS2 anfragen, solange noch keiner vergeben ist                   *
    //*********************************************************************************************

    void Mbc80Com::requestAutoKenner()
    {
        if (initDone() && csConnected() && !csAutoMode())
        {
            _autoCnt++;

            if (_autoCnt == 65535)
            {
                _can->send(can_msg[MBC_COM_MSG_CS2_0x00_5_0x0C]);
                _bridge->send(can_msg[MBC_COM_MSG_CS2_0x00_5_0x0C]);

                _autoCnt = 0;
            }
        }
    }

//------ Ende Klasse Mbc80Com ---------------------------------------------------------------------

//###### Ende Quellcode Mbc80Com.cpp ##############################################################
