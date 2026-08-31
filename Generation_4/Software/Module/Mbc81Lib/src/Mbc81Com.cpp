//*************************************************************************************************
//* Modul mbc-81: Stellpult mit 32 Tastern, 16 Stellungs-LEDs (Mbc81Lib)                          *
//*************************************************************************************************
//* Datei: Mbc81Com.cpp                                                                           *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen:                                                                                  *
//* V1.0.0 - 22.08.2026 - Nullversion                                                             *
//* V1.0.1 - 29.08.2026 - RAM-Optimierung                                                         *
//*************************************************************************************************

//###### Start Quellcode Mbc81Com.cpp #############################################################

#include "Mbc81Com.h"

//------ Start interne Konstanten -----------------------------------------------------------------

    // CAN-Konstanten (CAN-ID/Sub-Code gemaess Maerklin-Definition), unabhaengig von der
    // MbcCom-Basisklasse fuer die eigene Schaltbefehl-/S88-Auswertung dieses Moduls

    static const uint8_t CAN_ID_ZUBEHOER_SCHALTEN = 0x16;
    static const uint8_t CAN_ID_CS2_S88_REQUEST   = 0x22;

    // Modulspezifische EEPROM-Blockindizes (siehe PC_ARRAY_DATA/MBC_COM_EEPROM in MbcCom.cpp) -
    // MUESSEN exakt den vom Parametriercenter erwarteten EE_81_*-Werten entsprechen, da dort
    // genau diese Hex-IDs per COMUpdateStart() gesendet werden

    static const uint8_t EE_81_MMAddr  = 0x0A;
    static const uint8_t EE_81_Nutzung = 0x0B;

    // Gueltiger MM-Adressbereich (Plausibilitaetscheck "wurde ueberhaupt parametriert") - gilt
    // fuer beide Betriebsarten (siehe Klassenkommentar)

    static const uint16_t MMADDR_MIN = 0x3000;
    static const uint16_t MMADDR_MAX = 0x33ff;

    // CS2-Kanalnummer/Zeilenindex des MM-Addr-Konfigkanals, Label "KeyB:" - Faktor 16: mbc-81
    // belegt 16 MM-Adressen je Modul (MM-Addr...MM-Addr+15). Die CSx-GUI kann keine Hex-Eingabe,
    // daher wird nicht die Basisadresse selbst uebertragen, sondern eine einfache Modulnummer,
    // die intern in MMADDR_MIN + 16*Modulnummer (0x3000...0x33F0) umgerechnet wird

    static const uint8_t KANAL_MODNR = MBC_COM_ANZMESSWERTE + MBC_COM_ANZKONFIG + 1;
    static const uint8_t ROW_MODNR   = 0;
    static const uint8_t MODNR_MAX   = 63;

    // CS2-Kanalnummer/Zeilenindex des Nutzung-Konfigkanals (Auswahlliste, Kenner 0x01, 3 Punkte
    // "Nutz:" 0=Switch/1=B0 1..32/2=B0 33..63 - Werte 1:1 wie vom Nutzer vorgegeben). Offset 3
    // innerhalb der Zeile ist bei diesem Kenner die "Jetzige Einstellung" (Index in die Liste)

    static const uint8_t KANAL_NUTZUNG   = KANAL_MODNR + 1;
    static const uint8_t ROW_NUTZUNG     = 1;
    static const uint8_t NUTZUNG_OFFSET  = 3;
    static const uint8_t NUTZUNG_MAX     = 2;

    // Entprellzeit je Taster-Bit (millis()-basiert, nicht-blockierend, fester Wert - mbc-81 hat
    // kein CS2-Konfigkanal fuer eine einstellbare Zeit)

    static const unsigned long TASTER_DEBOUNCE_MS = 30UL;

    // PCB-Pin-Remap je PCF8574-Byte (reale Verdrahtung, siehe Klassenkommentar in Mbc81Com.h) -
    // Bit i des gelesenen (invertierten) Bytes gehoert zu Position TASTER_REMAP[i] der 4
    // Adresspaare

    static const uint8_t TASTER_REMAP[8] = {0, 1, 2, 3, 7, 6, 5, 4};

//------ Ende interne Konstanten ------------------------------------------------------------------

//------ Start EEPROM-Zusatzregion-Byteoffsets ----------------------------------------------------

    static const uint16_t EEX_81_MMAddr  = 0;   // 2 Byte
    static const uint16_t EEX_81_Nutzung = 2;   // 1 Byte

//------ Ende EEPROM-Zusatzregion-Byteoffsets -----------------------------------------------------

//------ Start Systemarray-Feldoffsets (Layout verbindlich, siehe Mbc81Com.h) ----------------------

    static const uint16_t O_MM_H     = MBC_S_GENPURP;
    static const uint16_t O_MM_L     = MBC_S_GENPURP + 1;
    static const uint16_t O_NUTZUNG  = MBC_S_GENPURP + 2;

//------ Ende Systemarray-Feldoffsets -------------------------------------------------------------

//------ Start Klasse Mbc81Com --------------------------------------------------------------------

    // PROGMEM: nur einmal hier deklariert, in beiden Konstruktor-Zweigen per memcpy_P gelesen

    static const uint8_t initCs2Ex[MBC81_ANZMESSWERTEMODUL + MBC81_ANZKONFIGMODUL]
                                   [MBC_COM_CS2PAKETE * 8] PROGMEM =
    {
        // INDEX_CTRL 1.EX.1 - Modulnummer (0...63), Basisadresse = MMADDR_MIN + 16*Modulnummer

        {   KANAL_MODNR,0x02,0x00,0x00,0x00,0x3f,0x00,0x00,              // 1...8
            'K','e','y','B',':',0x00,'0',0x00,                          // 9...16
            '6','3',0x00,'#',0x00,0x00,0x00,0x00,                       // 17...24
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,                    // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                     // 33...40
        },

        // INDEX_CTRL 1.EX.2 - Nutzung (Auswahlliste: 0=Switch, 1=B0 1..32, 2=B0 33..63)

        {   KANAL_NUTZUNG,0x01,0x03,0x00,0x00,0x00,0x00,0x00,            // 1...8
            'T','y','p','e',':',0x00,'S','w',                           // 9...16
            'i','t','c','h',0x00,'B','0',' ',                           // 17...24
            '1','.','.','3','2',0x00,'B','0',                           // 25...32
            ' ','3','3','.','.','6','3',0x00                            // 33...40
        }
    };

    static const uint8_t initMsgKonfigOk[MBC_COM_MSG_LAENGE] PROGMEM =
        {0x00,0x01,0x00,0x00,0x07,0x00,0x00,0x00,0x00,0x0b,0x00,0x01,0x00};

    //*********************************************************************************************
    //* Konstruktor zur Deklaration von Objektvariablen                                           *
    //*********************************************************************************************

    #if defined(__AVR__)

        Mbc81Com::Mbc81Com(MbcUtils &utils, MbcCan &can, MbcBootStorage &bootStorage,
                            MbcPcf8574 *(&tasterChips)[4], MbcDigitalOut *(&leds)[16],
                            uint8_t pinReset)
            : MbcCom(utils, can, bootStorage, pinReset,
                     MBC81_ANZMESSWERTEMODUL, MBC81_ANZKONFIGMODUL),
              _pinReset(pinReset)
        {

    #else

        Mbc81Com::Mbc81Com(MbcUtils &utils, MbcCan &can, MbcBootStorage &bootStorage,
                            MbcPcf8574 *(&tasterChips)[4], MbcDigitalOut *(&leds)[16])
            : MbcCom(utils, can, bootStorage,
                     MBC81_ANZMESSWERTEMODUL, MBC81_ANZKONFIGMODUL)
        {

    #endif

        memcpy_P(_msgKonfigOk, initMsgKonfigOk, MBC_COM_MSG_LAENGE);

        for (uint8_t i = 0; i < 4; i++)  _tasterChips[i] = tasterChips[i];
        for (uint8_t i = 0; i < 16; i++) _leds[i]        = leds[i];

        for (uint8_t i = 0; i < 32; i++)
        {
            _pinActive[i]       = false;
            _pinRaw[i]          = false;
            _pinStartMs[i]      = 0;
            _pinLastChangeMs[i] = 0;
        }

        _baseAddr = 0;
        _nutzung  = 0;
    }

    //*********************************************************************************************
    //* Einmalig nach dem begin() der Basisklasse aufrufen: LEDs auf OUTPUT setzen und in den     *
    //* Startzustand bringen (alle 16 LEDs "gesetzt"), gespeicherte Parameter aus der EEPROM-     *
    //* Zusatzregion laden und die Entprellzustaende zuruecksetzen.                               *
    //*********************************************************************************************

    void Mbc81Com::beginModule()
    {
        for (uint8_t i = 0; i < 16; i++)
        {
            _leds[i]->begin();
            _leds[i]->write(true);
        }

        eepromReadParaExtern();

        for (uint8_t i = 0; i < 32; i++)
        {
            _pinActive[i]       = false;
            _pinRaw[i]          = false;
            _pinStartMs[i]      = millis();
            _pinLastChangeMs[i] = millis();
        }
    }

    //*********************************************************************************************
    //* Je loop()-Durchlauf aufrufen: alle 32 Taster einlesen/entprellen sowie CAN-Nachrichten-   *
    //* empfang/-Auswertung (ruft intern auch die geerbte check() auf).                           *
    //*********************************************************************************************

    void Mbc81Com::update()
    {
        poll();

        tasterPinsCheck();

        uint8_t msg[MBC_CAN_MSG_LEN];

        if (_can->receive(msg))
        {
            check(msg);

            if ((msg[MBC_CAN_ID] == CAN_ID_CS2_S88_REQUEST) && (msg[MBC_CAN_DLC] == 0x04))
            {
                handleS88Request(msg);
            }

            bool isSchaltbefehl = (msg[MBC_CAN_ID] == CAN_ID_ZUBEHOER_SCHALTEN)
                                && ((msg[MBC_CAN_D2] & 0x30) == 0x30);

            if (isSchaltbefehl && (_nutzung != 0)) handleSwitchCommand(msg);
        }
    }

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //+ Erweiterungspunkte der Basisklasse (siehe MbcCom.h)                                       +
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    //*********************************************************************************************
    //* Laedt MM-Adresse und Nutzung aus der EEPROM-Zusatzregion (0xff-Sentinel-Defaults: MM-Addr *
    //* 0x3000, Nutzung 0 = Switchboard).                                                         *
    //*********************************************************************************************

    void Mbc81Com::eepromReadParaExtern()
    {
        uint8_t *arr = _utils->array();

        uint8_t addr[2];

        _utils->eepromReadExtra(EEX_81_MMAddr, addr, 2);

        if (addr[0] == 0xff) { addr[0] = 0x30; addr[1] = 0x00; }

        arr[O_MM_H] = addr[0];
        arr[O_MM_L] = addr[1];

        _baseAddr = (uint16_t) (((uint16_t) arr[O_MM_H] << 8) | (uint16_t) arr[O_MM_L]);

        _utils->eepromReadExtra(EEX_81_Nutzung, &arr[O_NUTZUNG], 1);

        if (arr[O_NUTZUNG] == 0xff) arr[O_NUTZUNG] = 0;

        _nutzung = arr[O_NUTZUNG];
    }

    //*********************************************************************************************
    //* Persistiert den betroffenen Block (MM-Addr oder Nutzung) in der EEPROM-Zusatzregion und    *
    //* haelt die intern gecachte Basisadresse/Nutzung synchron.                                    *
    //*********************************************************************************************
    //* block: Identifier des zu schreibenden Blocks (EE_81_MMAddr/EE_81_Nutzung)                  *
    //*********************************************************************************************

    void Mbc81Com::eepromWriteParaExtern(uint8_t block)
    {
        uint8_t *arr = _utils->array();

        if (block == EE_81_MMAddr)
        {
            _utils->eepromWriteExtra(EEX_81_MMAddr, &arr[O_MM_H], 2);

            _baseAddr = (uint16_t) (((uint16_t) arr[O_MM_H] << 8) | (uint16_t) arr[O_MM_L]);
        }
        else if (block == EE_81_Nutzung)
        {
            _utils->eepromWriteExtra(EEX_81_Nutzung, &arr[O_NUTZUNG], 1);

            _nutzung = arr[O_NUTZUNG];
        }
    }

    //*********************************************************************************************
    //* Traegt GUiD/HASH in die Konfig-Bestaetigungsnachricht _msgKonfigOk ein.                    *
    //*********************************************************************************************

    void Mbc81Com::hashInitExtern()
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
    //* Uebernimmt Konfigkanaele von der CS2: msg[D5]==KANAL_MODNR - neue Modulnummer (Label       *
    //* "KeyB:", 0...63), rechnet sie in die MM-Basisadresse um (MMADDR_MIN + 16*Modulnummer);     *
    //* msg[D5]==KANAL_NUTZUNG - neue Nutzung-Auswahl (Label "Nutz:", 0=Switch/1=B0 1..32/          *
    //* 2=B0 33..63) uebernehmen. Ein ungueltiger Wert bleibt bei BEIDEN Kanaelen ohne Wirkung      *
    //* (kein Schreiben/Persistieren) und wird mit FALSE quittiert.                                *
    //*********************************************************************************************
    //* msg: CAN-Nachricht (msg[D5]=Kanal, msg[D6..D7]=neuer Wert)                                 *
    //*********************************************************************************************

    void Mbc81Com::cs2IndexParameterExtern(const uint8_t msg[MBC_COM_MSG_LAENGE])
    {
        uint8_t  kanal = msg[MBC_CAN_D5];
        uint16_t wert  = (uint16_t) (((uint16_t) msg[MBC_CAN_D6] << 8) | (uint16_t) msg[MBC_CAN_D7]);

        if (kanal == KANAL_MODNR)
        {
            bool gueltig = (wert <= MODNR_MAX);

            if (gueltig)
            {
                uint16_t addr    = (uint16_t) (MMADDR_MIN + 16 * wert);
                uint8_t  *arr    = _utils->array();
                uint16_t oldAddr = (uint16_t) (((uint16_t) arr[O_MM_H] << 8) | arr[O_MM_L]);

                // EEPROM-Schreibzugriff nur bei echter Aenderung (die CSx sendet Konfigwerte
                // auch als Echo/Polling erneut)

                if (addr != oldAddr)
                {
                    arr[O_MM_H] = (uint8_t) (addr >> 8);
                    arr[O_MM_L] = (uint8_t) (addr & 0x00ff);

                    eepromWriteParaExtern(EE_81_MMAddr);   // persistiert + aktualisiert _baseAddr
                }
            }

            _msgKonfigOk[MBC_CAN_D5] = KANAL_MODNR;
            _msgKonfigOk[MBC_CAN_D6] = gueltig ? 0x01 : 0x00;
        }
        else if (kanal == KANAL_NUTZUNG)
        {
            bool gueltig = (wert <= NUTZUNG_MAX);

            if (gueltig)
            {
                uint8_t *arr = _utils->array();

                if (arr[O_NUTZUNG] != (uint8_t) wert)
                {
                    arr[O_NUTZUNG] = (uint8_t) wert;

                    eepromWriteParaExtern(EE_81_Nutzung);   // persistiert + aktualisiert _nutzung
                }
            }

            _msgKonfigOk[MBC_CAN_D5] = KANAL_NUTZUNG;
            _msgKonfigOk[MBC_CAN_D6] = gueltig ? 0x01 : 0x00;
        }
        else
        {
            return;
        }

        sendMessage(_msgKonfigOk);
    }

    //*********************************************************************************************
    //* Baut die angeforderte Kanalzeile frisch in _cs2ScratchRow zusammen - ROW_MODNR = KeyB,     *
    //* ROW_NUTZUNG = Nutz, sonst nullptr. Vorlage (initCs2Ex[], PROGMEM) bleibt statisch, nur     *
    //* der aktuelle Wert wird bei jedem Aufruf frisch aus dem Systemarray/_baseAddr gepatcht.     *
    //*********************************************************************************************
    //* laufendeNr: 0..ANZMESSWERTEMODUL+ANZKONFIGMODUL-1                                          *
    //*********************************************************************************************
    //* Return: Zeiger auf _cs2ScratchRow (nur bis zum naechsten Aufruf gueltig) oder nullptr      *
    //*********************************************************************************************

    const uint8_t* Mbc81Com::cs2IndexExtern(uint8_t laufendeNr)
    {
        if (laufendeNr >= (MBC81_ANZMESSWERTEMODUL + MBC81_ANZKONFIGMODUL)) return nullptr;

        memcpy_P(_cs2ScratchRow, initCs2Ex[laufendeNr], sizeof(_cs2ScratchRow));

        uint8_t *arr = _utils->array();

        if (laufendeNr == ROW_MODNR)
        {
            _cs2ScratchRow[7] = (uint8_t) ((_baseAddr - MMADDR_MIN) / 16);
        }
        else if (laufendeNr == ROW_NUTZUNG)
        {
            _cs2ScratchRow[NUTZUNG_OFFSET] = arr[O_NUTZUNG];
        }

        return _cs2ScratchRow;
    }

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //+ Interne Funktionen                                                                        +
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    //*********************************************************************************************
    //* Liest alle 4 PCF8574, wendet den PCB-Pin-Remap an und reicht jedes der 32 Taster-Bits an  *
    //* tasterEdge() weiter - nur wenn die MM-Adresse im gueltigen Bereich liegt                  *
    //* (Plausibilitaetspruefung gilt fuer beide Betriebsarten, siehe Klassenkommentar in         *
    //* Mbc81Com.h).                                                                              *
    //*********************************************************************************************

    void Mbc81Com::tasterPinsCheck()
    {
        if ((_baseAddr < MMADDR_MIN) || (_baseAddr > MMADDR_MAX)) return;

        for (uint8_t chip = 0; chip < 4; chip++)
        {
            uint8_t raw = 0xff;

            _tasterChips[chip]->read(raw);

            uint8_t inverted = (uint8_t) ~raw;
            uint8_t remapped = 0;

            for (uint8_t bit = 0; bit < 8; bit++)
            {
                if (inverted & (uint8_t) (1 << bit)) remapped |= (uint8_t) (1 << TASTER_REMAP[bit]);
            }

            for (uint8_t pair = 0; pair < 4; pair++)
            {
                uint8_t indexRot   = (uint8_t) (chip * 8 + pair * 2);
                uint8_t indexGruen = (uint8_t) (indexRot + 1);

                tasterEdge(indexRot,   (remapped & (uint8_t) (1 << (pair * 2)))     != 0);
                tasterEdge(indexGruen, (remapped & (uint8_t) (1 << (pair * 2 + 1))) != 0);
            }
        }
    }

    //*********************************************************************************************
    //* Entprellt ein einzelnes Taster-Bit (millis()-basiert, TASTER_DEBOUNCE_MS stabil in        *
    //* gleicher Rohzustands-Phase, bevor ein Wechsel bestaetigt wird - SYMMETRISCH auf beiden    *
    //* Flanken, da hier echte, prellende Taster statt Gleiskontakte gelesen werden) und loest    *
    //* bei einer bestaetigten Flanke je nach Nutzung einen Schaltbefehl oder eine S88-Meldung    *
    //* aus.                                                                                      *
    //*********************************************************************************************
    //* index: 0...31 (siehe Mbc81Com.h)                                                           *
    //* raw:   aktueller Rohzustand dieses Taster-Bits                                             *
    //*********************************************************************************************

    void Mbc81Com::tasterEdge(uint8_t index, bool raw)
    {
        if (raw != _pinRaw[index])
        {
            _pinRaw[index]     = raw;
            _pinStartMs[index] = millis();
            return;
        }

        if (raw == _pinActive[index]) return;
        if ((millis() - _pinStartMs[index]) < TASTER_DEBOUNCE_MS) return;

        _pinActive[index] = raw;

        if (_nutzung == 0) sendSwitchCommand(index, raw);
        else                sendS88Event(index, raw);
    }

    //*********************************************************************************************
    //* Sendet einen Motorola-Schaltbefehl fuer die betroffene Adresse (nur Switchboard-Modus)    *
    //* und setzt bei einer Aktivierung zusaetzlich die eigene LED (Echo, siehe Klassenkommentar  *
    //* in Mbc81Com.h) - die "Aus"-Flanke sendet ebenfalls ein Telegramm, aendert die LED aber    *
    //* NICHT.                                                                                    *
    //*********************************************************************************************
    //* index:  0...31 (siehe Mbc81Com.h)                                                          *
    //* active: true = Taster aktiviert, false = losgelassen                                       *
    //*********************************************************************************************

    void Mbc81Com::sendSwitchCommand(uint8_t index, bool active)
    {
        uint8_t pair      = (uint8_t) (index / 2);
        uint8_t direction = (uint8_t) (index % 2);

        uint16_t addr = (uint16_t) (_baseAddr + pair);

        can_msg[MBC_COM_MSG_CS2_0x16][MBC_CAN_D2] = (uint8_t) ((addr & 0xff00) >> 8);
        can_msg[MBC_COM_MSG_CS2_0x16][MBC_CAN_D3] = (uint8_t) (addr & 0x00ff);
        can_msg[MBC_COM_MSG_CS2_0x16][MBC_CAN_D4] = direction;
        can_msg[MBC_COM_MSG_CS2_0x16][MBC_CAN_D5] = active ? 0x01 : 0x00;

        sendMessage(can_msg[MBC_COM_MSG_CS2_0x16]);

        if (active) ledSet(pair, direction);
    }

    //*********************************************************************************************
    //* Sendet eine S88-Ereignismeldung fuer die betroffene Pinnummer (nur Bus-0-Modus) - D6/D7   *
    //* tragen die seit der letzten Zustandsaenderung dieses Kontakts vergangene Zeit in ms (auf  *
    //* 0xffff geklemmt).                                                                         *
    //*********************************************************************************************
    //* index:  0...31 (siehe Mbc81Com.h)                                                          *
    //* active: true = Kontakt ausgeloest, false = Kontakt wieder frei                             *
    //*********************************************************************************************

    void Mbc81Com::sendS88Event(uint8_t index, bool active)
    {
        uint16_t pinNr = (uint16_t) (101 + (_nutzung - 1) * 32 + index);

        unsigned long elapsedMs = (unsigned long) (millis() - _pinLastChangeMs[index]);
        uint16_t      elapsed16 = (elapsedMs > 0xffffUL) ? 0xffff : (uint16_t) elapsedMs;

        can_msg[MBC_COM_MSG_CS2_0x22_R][MBC_CAN_D2] = (uint8_t) ((pinNr & 0xff00) >> 8);
        can_msg[MBC_COM_MSG_CS2_0x22_R][MBC_CAN_D3] = (uint8_t) (pinNr & 0x00ff);
        can_msg[MBC_COM_MSG_CS2_0x22_R][MBC_CAN_D4] = active ? 0x00 : 0x01;
        can_msg[MBC_COM_MSG_CS2_0x22_R][MBC_CAN_D5] = active ? 0x01 : 0x00;
        can_msg[MBC_COM_MSG_CS2_0x22_R][MBC_CAN_D6] = (uint8_t) ((elapsed16 & 0xff00) >> 8);
        can_msg[MBC_COM_MSG_CS2_0x22_R][MBC_CAN_D7] = (uint8_t) (elapsed16 & 0x00ff);

        sendMessage(can_msg[MBC_COM_MSG_CS2_0x22_R]);

        _pinLastChangeMs[index] = millis();
    }

    //*********************************************************************************************
    //* Beantwortet eine explizite CS2-Statusabfrage (CAN_ID 0x22, DLC 0x04) mit dem echten,       *
    //* zuletzt erkannten Tasterzustand (D4=D5=Zustand, siehe Klassenkommentar in Mbc81Com.h).     *
    //* Antwortet NUR fuer Pins innerhalb des eigenen, konfigurierten Matrix-Slots                 *
    //* (101+(Nutzung-1)*32 ... +31), da dieses Modul den jeweils anderen Slot gar nicht selbst    *
    //* fuehrt.                                                                                    *
    //*********************************************************************************************
    //* msg: empfangene CAN-Nachricht                                                              *
    //*********************************************************************************************

    void Mbc81Com::handleS88Request(const uint8_t msg[MBC_COM_MSG_LAENGE])
    {
        if (!initDone() || !csConnected() || (_nutzung == 0)) return;

        uint16_t requested = (uint16_t) (((uint16_t) msg[MBC_CAN_D2] << 8) | (uint16_t) msg[MBC_CAN_D3]);
        uint16_t base      = (uint16_t) (101 + (_nutzung - 1) * 32);

        if ((requested < base) || (requested > (uint16_t) (base + 31))) return;

        uint8_t index = (uint8_t) (requested - base);

        can_msg[MBC_COM_MSG_CS2_0x22_R][MBC_CAN_D2] = msg[MBC_CAN_D2];
        can_msg[MBC_COM_MSG_CS2_0x22_R][MBC_CAN_D3] = msg[MBC_CAN_D3];
        can_msg[MBC_COM_MSG_CS2_0x22_R][MBC_CAN_D4] = _pinActive[index] ? 0x01 : 0x00;
        can_msg[MBC_COM_MSG_CS2_0x22_R][MBC_CAN_D5] = _pinActive[index] ? 0x01 : 0x00;
        can_msg[MBC_COM_MSG_CS2_0x22_R][MBC_CAN_D6] = 0x00;
        can_msg[MBC_COM_MSG_CS2_0x22_R][MBC_CAN_D7] = 0x00;

        sendMessage(can_msg[MBC_COM_MSG_CS2_0x22_R]);

    }

    //*********************************************************************************************
    //* Spiegelt einen eingehenden Motorola-Schaltbefehl im konfigurierten Adressbereich auf die  *
    //* passende LED (nur Bus-0-Modus, siehe Klassenkommentar in Mbc81Com.h) - reine passive      *
    //* Anzeige, keine CAN-Antwort.                                                               *
    //*********************************************************************************************
    //* msg: empfangene CAN-Nachricht                                                              *
    //*********************************************************************************************

    void Mbc81Com::handleSwitchCommand(const uint8_t msg[MBC_COM_MSG_LAENGE])
    {
        uint16_t addr = (uint16_t) (((uint16_t) msg[MBC_CAN_D2] << 8) | (uint16_t) msg[MBC_CAN_D3]);

        if ((addr < _baseAddr) || (addr > (uint16_t) (_baseAddr + 15))) return;

        ledSet((uint8_t) (addr - _baseAddr), msg[MBC_CAN_D4]);
    }

    //*********************************************************************************************
    //* Setzt eine einzelne Stellungs-LED.                                                        *
    //*********************************************************************************************
    //* pairIndex: 0...15 (Index = MM-Adressoffset)                                                *
    //* direction: 0 = Rot (LED aus), ungleich 0 = Gruen (LED an)                                  *
    //*********************************************************************************************

    void Mbc81Com::ledSet(uint8_t pairIndex, uint8_t direction)
    {
        _leds[pairIndex]->write(direction != 0);
    }

//------ Ende Klasse Mbc81Com ---------------------------------------------------------------------

//###### Ende Quellcode Mbc81Com.cpp ##############################################################
