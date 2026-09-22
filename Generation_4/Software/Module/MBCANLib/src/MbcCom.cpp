//*************************************************************************************************
//* PC-/CS2-Kommunikations-Zustandsautomat (MBCANLib)                                             *
//*************************************************************************************************
//* Datei: MbcCom.cpp                                                                             *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen:                                                                                  *
//* V1.0.0 - 02.08.2026 - Nullversion                                                             *
//*************************************************************************************************

//###### Start Quellcode MbcCom.cpp ###############################################################

#include "MbcCom.h"
#include "MbcPlatform.h"
#include <string.h>

#if defined(ARDUINO_ARCH_RP2040) || defined(ESP32)
    #include "MbcOta.h"
#endif

//------ Start interne Konstanten -----------------------------------------------------------------

    // CAN-Befehlsnummern (CAN-ID gemaess Maerklin-Definition)

    static const uint8_t CAN_ID_SYSTEM_BEFEHLE            = 0x00;
    static const uint8_t CAN_ID_SYSTEM_BEFEHLE_RESPONSE   = 0x01;
    static const uint8_t CAN_ID_PING                      = 0x30;
    static const uint8_t CAN_ID_PING_RESPONSE             = 0x31;
    static const uint8_t CAN_ID_KONFIG                    = 0x3A;
    static const uint8_t CAN_ID_KONFIG_RESPONSE           = 0x3B;
    static const uint8_t CAN_ID_ZUBEHOER_SCHALTEN         = 0x16;
    static const uint8_t CAN_ID_S88_EVENT_RESPONSE        = 0x23;

    static const uint8_t CAN_ID_SYSTEM_BEFEHLE_SUB_MW = 0x0B;
    static const uint8_t CAN_ID_SYSTEM_BEFEHLE_SUB_KN = 0x0C;

    static const uint8_t CAN_LOCID_MBCAN = 0x18;

    // Zweiter Teil der Loc-ID 0x18xx (PC-Kommunikation)

    static const uint8_t PC_DB_H = 0x00, PC_DB_M = 0x01, PC_DB_L = 0x02, PC_KENNER = 0x03;
    static const uint8_t PC_NEU = 0x04, PC_NEU_DATA = 0x05, MD_NEU_DATA = 0x06;
    static const uint8_t PC_RESET = 0x07, PC_MD_DEL = 0x08, PC_ALIVE = 0x09, MD_ALIVE = 0x0A;
    static const uint8_t PC_ARRAY = 0x0B, MD_ARRAY = 0x0C, PC_ARRAY_DATA = 0x0D, MD_ARRAY_DATA = 0x0E;
    static const uint8_t PC_UPGRADE = 0x0F, MD_UPGRADE = 0x10, PC_UPGRADE_DATA = 0x11, MD_UPGRADE_DATA = 0x12;
    static const uint8_t PC_BOOT = 0x13, MD_S88 = 0x14, MD_ZYKLUS = 0x15;

    // Feste Indizes in der CS2-Konfigurationstabelle (gemeinsamer Teil aller Module)

    static const uint8_t INDEX_0 = 0, INDEX_0_ART_NR = 11, INDEX_0_TYP = 26, INDEX_0_SERNR = 4;
    static const uint8_t INDEX_1_1 = 1, INDEX_1_2 = 2, INDEX_2_1 = 3, INDEX_2_1_SERNRDS = 15;
    static const uint8_t INDEX_2_2 = 4, INDEX_2_2_GU_1 = 15, INDEX_2_2_GU_2 = 17, INDEX_2_2_GU_3 = 18, INDEX_2_2_GU_4 = 22;
    static const uint8_t INDEX_2_3 = 5, INDEX_2_3_FW_1 = 13, INDEX_2_3_FW_2 = 15, INDEX_2_3_FW_3 = 16;
    static const uint8_t INDEX_2_4 = 6, INDEX_2_4_HW_1 = 13, INDEX_2_4_HW_2 = 14, INDEX_2_4_HW_3 = 16;
    static const uint8_t INDEX_2_4_HW_4 = 17, INDEX_2_4_HW_5 = 19, INDEX_2_4_HW_6 = 20;

    // Anzahl Sendeversuche bei fehlgeschlagenem CAN-Versand (siehe sendMessage()) - MbcCan::send()
    // kann bei belegtem TX-Puffer fehlschlagen (false liefern). Besonders bei mehrzeiligen
    // CS2-Konfigkanal-Uebertragungen (checkCs2Connection(), CAN_ID_KONFIG) reicht ein einziges
    // verlorenes Paket, damit die CSx die gesamte Zeilenuebertragung als fehlerhaft verwirft und
    // den weiteren Zeilen-Walk abbricht.

    static const uint8_t SEND_RETRIES = 5;

    // Pause zwischen den 5 CAN-Datenpaketen einer CS2-Konfigkanal-Zeile (checkCs2Connection(),
    // CAN_ID_KONFIG). Bei realer Bus-Last durch parallelen Lok-Traffic (MFX Seek) verliert die CSx
    // sonst bevorzugt das letzte der 5 Pakete einer Buerste - 2000us Pause geben ihr mehr
    // Verarbeitungszeit je Paket. Noch NICHT hardwaregetestet.

    static const uint16_t CS2_PAKET_DELAY_US = 2000;

    // Streuung der Ping-Antwortzeit (checkCs2Connection(), CAN_ID_PING) ueber den je Modul
    // eindeutigen HASH-Wert (siehe hashInit()). Ohne diese Streuung antworten baugleiche Module
    // (identische Firmware/Timing) auf denselben Broadcast-Ping nahezu zeitgleich - per
    // CAN-Mitschnitt bestaetigt fuer mbc88+mbc92: in ~85% der Ping-Zyklen kam nur eines der beiden
    // durch, nie beide gleichzeitig ausgefallen. Da der HASH-Wert pro Modul fest, aber zwischen
    // Modulen praktisch beliebig verteilt ist, skaliert die Streuung auf beliebig viele
    // gleichzeitige Module am Bus, nicht nur auf Modulpaare. 8000us Streubereich sind gegenueber
    // dem ca. 10s-Ping-Zyklus vernachlaessigbar.

    static const uint16_t PING_STAGGER_RANGE_US = 8000;

//------ Ende interne Konstanten ------------------------------------------------------------------

//------ Start Systemarray-Feldoffsets (aus MbcUtils.h abgeleitet) --------------------------------

    static const uint16_t O_SNR   = MBC_S_SNR;          // 8 Byte
    static const uint16_t O_ART   = MBC_S_ART;          // 4 Byte
    static const uint16_t O_SW    = MBC_S_SW;           // 3 Byte
    static const uint16_t O_HW    = MBC_S_HW;           // 6 Byte
    static const uint16_t O_UID   = MBC_S_GUID;         // 4 Byte
    static const uint16_t O_DB    = MBC_S_DB;           // 12 Byte
    static const uint16_t O_KN_H  = MBC_S_KN;
    static const uint16_t O_KN_L  = MBC_S_KN + 1;
    static const uint16_t O_CS2GER = MBC_S_KN + 4;
    static const uint16_t O_SPG   = MBC_S_PARA + 1;     // 3 Byte
    static const uint16_t O_TMP   = MBC_S_PARA + 4;     // 3 Byte
    static const uint16_t O_BOOT  = MBC_S_BOOT;          // 2 + MBC_BOOT_PGSZ Byte

//------ Ende Systemarray-Feldoffsets -------------------------------------------------------------

//------ Start Klasse MbcCom ----------------------------------------------------------------------

    //*********************************************************************************************
    //* Konstruktor zur Deklaration von Objektvariablen (initialisiert ausserdem die              *
    //* CAN-Nachrichtentabelle can_msg[] und die CS2-Konfigurationstabelle cs2_index[] - INDEX_0  *
    //* traegt dabei die Gesamtanzahl Messwert-/Konfigkanaele (Basis + Unterklasse) ein, damit    *
    //* die CSx die modulspezifischen Kanaele der Unterklasse ueberhaupt abfragt)                 *
    //*********************************************************************************************
    //* utils:             zugehoerige MbcUtils-Instanz                                           *
    //* can:               zugehoerige MbcCan-Instanz (PC-/CS2-CAN-Kanal dieses Moduls)           *
    //* bootStorage:       zugehoerige MbcBootStorage-Instanz (Firmware-Update-Staging)           *
    //* pinReset:          NUR auf AVR vorhanden - siehe Klassenkommentar in MbcCom.h             *
    //* anzMesswerteModul: Anzahl modulspezifischer Messwertkanaele der Unterklasse (Default 0)   *
    //* anzKonfigModul:    Anzahl modulspezifischer Konfigurationskanaele der Unterklasse         *
    //*                    (Default 0)                                                            *
    //*********************************************************************************************

    #if defined(__AVR__)
        MbcCom::MbcCom(MbcUtils &utils, MbcCan &can, MbcBootStorage &bootStorage, uint8_t pinReset,
                       uint8_t anzMesswerteModul, uint8_t anzKonfigModul)
            : _utils(&utils), _can(&can), _bootStorage(&bootStorage), _pinReset(pinReset),
              _anzMesswerteModul(anzMesswerteModul)
        {
    #else
        MbcCom::MbcCom(MbcUtils &utils, MbcCan &can, MbcBootStorage &bootStorage,
                       uint8_t anzMesswerteModul, uint8_t anzKonfigModul)
            : _utils(&utils), _can(&can), _bootStorage(&bootStorage),
              _anzMesswerteModul(anzMesswerteModul)
        {
    #endif
        // Vordefinierte CAN-Nachrichtentemplates
        // PRIO,CAN_ID,HASH_H,HASH_L,DLC,D0,D1,D2,D3,D4,D5,D6,D7

        // PROGMEM: nur einmal hier beim Konstruieren gelesen, muss nicht dauerhaft
        // zusaetzlich zur Arbeitskopie can_msg[] im RAM liegen.

        static const uint8_t initMsg[MBC_COM_MSG_ANZAHL][MBC_COM_MSG_LAENGE] PROGMEM =
        {
            {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
            {0x00,CAN_ID_ZUBEHOER_SCHALTEN,0x03,0x00,0x08,0x00,0x00,CAN_LOCID_MBCAN,MD_NEU_DATA,0x00,0x00,0x00,0x00},
            {0x00,CAN_ID_ZUBEHOER_SCHALTEN,0x03,0x00,0x08,0x00,0x00,CAN_LOCID_MBCAN,MD_ALIVE,0x00,0x00,0x00,0x00},
            {0x00,CAN_ID_ZUBEHOER_SCHALTEN,0x03,0x00,0x08,0x00,0x00,CAN_LOCID_MBCAN,MD_ARRAY,0x00,0x00,0x00,0x00},
            {0x00,CAN_ID_ZUBEHOER_SCHALTEN,0x03,0x00,0x08,0x00,0x00,CAN_LOCID_MBCAN,MD_ARRAY_DATA,0x00,0x00,0x00,0x00},
            {0x00,CAN_ID_ZUBEHOER_SCHALTEN,0x03,0x00,0x08,0x00,0x00,CAN_LOCID_MBCAN,MD_UPGRADE,0x00,0x00,0x00,0x00},
            {0x00,CAN_ID_ZUBEHOER_SCHALTEN,0x03,0x00,0x08,0x00,0x00,CAN_LOCID_MBCAN,MD_UPGRADE_DATA,0x00,0x00,0x00,0x00},
            {0x00,CAN_ID_ZUBEHOER_SCHALTEN,0x03,0x00,0x08,0x00,0x00,CAN_LOCID_MBCAN,MD_S88,0x00,0x00,0x00,0x00},
            {0x00,CAN_ID_ZUBEHOER_SCHALTEN,0x03,0x00,0x08,0x00,0x00,CAN_LOCID_MBCAN,MD_ZYKLUS,0x00,0x00,0x00,0x00},
            {0x00,CAN_ID_SYSTEM_BEFEHLE,0x03,0x00,0x05,0x00,0x00,0x00,0x00,CAN_ID_SYSTEM_BEFEHLE_SUB_KN,0x00,0x00,0x00},
            {0x00,CAN_ID_SYSTEM_BEFEHLE_RESPONSE,0x03,0x00,0x07,0x00,0x00,0x00,0x00,CAN_ID_SYSTEM_BEFEHLE_SUB_KN,0x00,0x00,0x00},
            {0x00,CAN_ID_SYSTEM_BEFEHLE_RESPONSE,0x03,0x00,0x08,0x00,0x00,0x00,0x00,CAN_ID_SYSTEM_BEFEHLE_SUB_MW,0x00,0x00,0x00},
            {0x00,CAN_ID_SYSTEM_BEFEHLE_RESPONSE,0x03,0x00,0x07,0x00,0x00,0x00,0x00,CAN_ID_SYSTEM_BEFEHLE_SUB_MW,0x00,0x01,0x00},
            {0x00,CAN_ID_ZUBEHOER_SCHALTEN,0x03,0x00,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
            {0x00,CAN_ID_S88_EVENT_RESPONSE,0x03,0x00,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
            {0x00,CAN_ID_PING_RESPONSE,0x03,0x00,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0xaa,0xaa},
            {0x00,CAN_ID_KONFIG_RESPONSE,0x03,0x00,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
            {0x00,CAN_ID_KONFIG_RESPONSE,0x03,0x00,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}
        };

        memcpy_P(can_msg, initMsg, sizeof(can_msg));

        // CS2-Konfigurationstabelle (gemeinsamer Teil aller Module)
        // PROGMEM: nur einmal hier beim Konstruieren gelesen, muss nicht dauerhaft
        // zusaetzlich zur Arbeitskopie cs2_index[] im RAM liegen.

        static const uint8_t initCs2[MBC_COM_ANZMESSWERTE + MBC_COM_ANZKONFIG + 1][MBC_COM_CS2PAKETE * 8] PROGMEM =
        {
            // INDEX 0 - Allgemeine Angaben zum angeschlossenen Geraet (39 Werte + 1 implizite 0)
            {   2,4,0x00,0x00, 0x00,0x00,0x00,0x00,                          // 1...8
                'M','B','C', 0x00,0x00,0x00,0x00, ' ',                       // 9...16
                'M','B','C','A','N',' ','m','b',                             // 17...24
                'c','-', 0x00,0x00, 0x00, 0x00,0x00,0x00,                    // 25...32
                0x00,0x00,0x00,0x00,0x00,0x00,0x00                           // 33...39 (Pos.40 implizit 0)
            },

            // INDEX_ALLG 1.1 - Erster Messwert - Spannungsangabe (40 Werte). Farbbereiche:
            // <11.0V=Rot, 11.0...15.9V=Orange, 16.0...21.9V=Gruen, >22.0V=Blau (Rohwert =
            // 50 * Volt - 500, siehe MbcUtils::adcReadVspg()/Kanal-1-Handler oben)
            {   INDEX_1_1,0x00,0xc3,0xd2,0x31,0x08,0x00,0x00,                // 1...8
                0x00,0x32,0x01,0x2c,0x02,0x58,0x03,0x52,                     // 9...16
                'V','O','L','T',0x00,'1','0','.',                           // 17...24
                '0','0',0x00,'2','7','.','0','0',                           // 25...32
                0x00,'V',0x00,0x00,0x00,0x00,0x00,0x00                       // 33...40
            },

            // INDEX_ALLG 1.2 - Zweiter Messwert - Temperaturangabe (40 Werte)
            {   INDEX_1_2,0x00,0x0c,0x08,0xf0,0xc0,0x00,0x00,                // 1...8
                0x00,0x7D,0x00,0x96,0x00,0xaf,0x00,0xc8,                     // 9...16
                'T','E','M','P',0x00,'0','.','0',                           // 17...24
                0x00,'8','0','.','0',0x00,'C',0x00,                         // 25...32
                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                     // 33...40
            },

            // INDEX_CTRL 2.1 - Seriennummer aus dem Dallas-Chip (40 Werte)
            {   INDEX_2_1, 0x01,0x01,0x00,0x00,0x00,0x00,0x00,                // 1...8
                'S','/','N',':',0x00,'0','x',0x00,                          // 9...16
                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,                     // 17...24
                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,                     // 25...32 (Pos.32 = Stringabschluss)
                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                      // 33...40
            },

            // INDEX_CTRL 2.2 - GUiD (40 Werte)
            {   INDEX_2_2, 0x01,0x01,0x00,0x00,0x00,0x00,0x00,                // 1...8
                'G','U','i','D',':',0x00,'#',0x00,                          // 9...16
                '-',0x00,0x00,'-','0','x',0x00,0x00,                         // 17...24
                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,                     // 25...32
                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                      // 33...40
            },

            // INDEX_CTRL 2.3 - Firmware (40 Werte)
            {   INDEX_2_3, 0x01,0x01,0x00,0x00,0x00,0x00,0x00,                // 1...8
                'F','W',':',0x00,'#',0x00,'.',0x00,                         // 9...16
                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,                     // 17...24
                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,                     // 25...32
                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                      // 33...40
            },

            // INDEX_CTRL 2.4 - Hardware (40 Werte)
            {   INDEX_2_4, 0x01,0x01,0x00,0x00,0x00,0x00,0x00,                // 1...8
                'H','W',':',0x00,'#',0x00,0x00,'.',                         // 9...16
                0x00,0x00,'.',0x00,0x00,0x00,0x00,0x00,                      // 17...24
                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,                     // 25...32
                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                      // 33...40
            }
        };

        memcpy_P(cs2_index, initCs2, sizeof(cs2_index));

        // Gesamtanzahl Messwert-/Konfigkanaele (Basis + Unterklasse) in INDEX_0 eintragen - ohne
        // das haelt die CSx die Unterklasse fuer kanallos und fragt cs2IndexExtern() nie ab

        cs2_index[INDEX_0][0] = (uint8_t) (MBC_COM_ANZMESSWERTE + anzMesswerteModul);
        cs2_index[INDEX_0][1] = (uint8_t) (MBC_COM_ANZKONFIG + anzKonfigModul);

        // Kanalnummern der Basis-Konfigkanaele (Seriennummer/GUiD/Firmware/Hardware) um die
        // Anzahl modulspezifischer Messwertkanaele verschieben - die CSx zeigt die ersten
        // (MBC_COM_ANZMESSWERTE + anzMesswerteModul) Kanaele als Messwerte an, unabhaengig von
        // deren eigener Kennung (siehe checkCs2Connection()); modulspezifische Messwertkanaele
        // muessen deshalb direkt hinter die Basis-Messwerte einsortiert werden, nicht hinter die
        // Basis-Konfigkanaele. Bei anzMesswerteModul=0 (alle bisherigen Module) bleiben die
        // Werte unveraendert bei 3/4/5/6.

        cs2_index[INDEX_2_1][0] = (uint8_t) (MBC_COM_ANZMESSWERTE + anzMesswerteModul + 1);
        cs2_index[INDEX_2_2][0] = (uint8_t) (MBC_COM_ANZMESSWERTE + anzMesswerteModul + 2);
        cs2_index[INDEX_2_3][0] = (uint8_t) (MBC_COM_ANZMESSWERTE + anzMesswerteModul + 3);
        cs2_index[INDEX_2_4][0] = (uint8_t) (MBC_COM_ANZMESSWERTE + anzMesswerteModul + 4);
    }

    //*********************************************************************************************
    //* Loesche alle Zustandsflags (Neuanmeldung, Datenbank, Datentransfer, CS2)                  *
    //*********************************************************************************************

    void MbcCom::moduleFlagsReset()
    {
        _init.init = false;
        _init.newPhase = false;
        _init.step = 0;
        _init.sysindex = 0;
        _init.startup = false;

        _cs.autoMode = false;
        _cs.init = false;
        _cs.startup = true;
        _cs.s88Startup = false;
        _cs.connect = false;
        _cs.csMbcan = false;
        _cs.autoRec = false;
        _cs.autoCnt = 0;

        _db.dbH = false;
        _db.dbM = false;
        _db.dbL = false;
        _db.dbValid = false;

        _io.transfer = false;
        _io.write = false;
        _io.read = false;

        _ping.pending = false;
    }

    //*********************************************************************************************
    //* Loescht Datenbanknummer und GUiD im Systemarray und EEPROM (Modul wird aus der            *
    //* PC-Datenbank entfernt) und setzt alle Zustandsflags zurueck                               *
    //*********************************************************************************************

    void MbcCom::moduleBlank()
    {
        uint8_t *arr = _utils->array();

        for (uint8_t index = 0; index < 12; index++)
        {
            _db.db[index] = 0;
            arr[O_DB + index] = 0x30;
        }

        _utils->eepromWrite(MBC_EE_DB);

        for (uint8_t index = 0; index < 4; index++) arr[O_UID + index] = 0xff;

        _utils->eepromWrite(MBC_EE_GUID);

        moduleFlagsReset();
    }

    //*********************************************************************************************
    //* Interne Sendefunktion: routet ueber MbcCan und zusaetzlich (falls von einer               *
    //* Modul-Unterklasse ueberschrieben) ueber sendMessageExtern(). Wiederholt einen             *
    //* fehlgeschlagenen CAN-Versand bis zu SEND_RETRIES mal, bevor aufgegeben wird - send()      *
    //* kann bei kurzzeitig belegtem TX-Puffer fehlschlagen, was frueher stillschweigend          *
    //* hingenommen wurde (siehe SEND_RETRIES-Kommentar oben).                                    *
    //*********************************************************************************************
    //* msg: Nachrichtenarray                                                                     *
    //*********************************************************************************************
    //* Return: true, wenn der CAN-Versand (ggf. nach Wiederholung) erfolgreich war               *
    //*********************************************************************************************

    bool MbcCom::sendMessage(const uint8_t msg[MBC_COM_MSG_LAENGE])
    {
        bool ok = false;

        for (uint8_t attempt = 0; (attempt < SEND_RETRIES) && !ok; attempt++)
        {
            ok = _can->send(msg);
        }

        sendMessageExtern(msg);

        return ok;
    }

    //*********************************************************************************************
    //* HASH-Wert fuer die CS2-Kommunikation berechnen und zuweisen. Dabei werden auch die        *
    //* anderen Modulparameter (Modulnummer, Hardwareversion, feste Geraetekennung, GUiD) in      *
    //* can_msg[]/cs2_index[] eingetragen.                                                        *
    //*********************************************************************************************

    void MbcCom::hashInit()
    {
        uint8_t *arr = _utils->array();

        // Modulnummer fuer CS2-Meldung ablegen (entspricht der Modulnummer aus der
        // PC-Konfiguration, z.B. GUID = "m" "8" "3" #1 -> Modulnummer = #1)

        uint16_t sernum = ((uint16_t) arr[O_UID + 3]) + 1;
        cs2_index[INDEX_0][INDEX_0_SERNR + 2] = (uint8_t) ((sernum & 0xff00) >> 8);
        cs2_index[INDEX_0][INDEX_0_SERNR + 3] = (uint8_t) (sernum & 0xff);

        // HASH-Wert fuer die CS2-Kommunikation berechnen

        uint16_t uidH = ((uint16_t) (arr[O_CS2GER] << 8)) | (uint16_t) arr[O_UID + 1];
        uint16_t uidL = ((uint16_t) (arr[O_UID + 2] << 8)) | (uint16_t) arr[O_UID + 3];
        uint16_t uidHashG = uidH ^ uidL;
        uint16_t uidHashH = (uidHashG >> 8) & 0x00ff;
        uint16_t uidHashL = (uidHashG & 0x00ff) << 8;
        uint16_t uidHash  = (uidHashL | uidHashH) | 0x0300;

        for (uint8_t index = 0; index < MBC_COM_MSG_ANZAHL; index++)
        {
            can_msg[index][MBC_CAN_HASH_H] = (uint8_t) ((uidHash & 0xff00) >> 8);
            can_msg[index][MBC_CAN_HASH_L] = (uint8_t) (uidHash & 0x00ff);
        }

        // Kommando: Softwarestand Anfrage / Teilnehmer-Ping 0x30_R

        can_msg[MBC_COM_MSG_CS2_0x30_R][MBC_CAN_D0] = arr[O_CS2GER];
        can_msg[MBC_COM_MSG_CS2_0x30_R][MBC_CAN_D1] = arr[O_UID + 1];
        can_msg[MBC_COM_MSG_CS2_0x30_R][MBC_CAN_D2] = arr[O_UID + 2];
        can_msg[MBC_COM_MSG_CS2_0x30_R][MBC_CAN_D3] = arr[O_UID + 3];

        can_msg[MBC_COM_MSG_CS2_0x30_R][MBC_CAN_D4] = (uint8_t) (arr[O_SW] - 0x30);
        can_msg[MBC_COM_MSG_CS2_0x30_R][MBC_CAN_D5] =
            (uint8_t) ((arr[O_SW + 1] - 0x30) * 10 + (arr[O_SW + 2] - 0x30));

        // Modulspezifische Softwareversion (z.B. Booster/L88) nachziehen lassen

        swInitExtern();

        // Hardwareversion eintragen

        cs2_index[INDEX_2_4][INDEX_2_4_HW_1] = arr[O_HW];
        cs2_index[INDEX_2_4][INDEX_2_4_HW_2] = arr[O_HW + 1];
        cs2_index[INDEX_2_4][INDEX_2_4_HW_3] = arr[O_HW + 2];
        cs2_index[INDEX_2_4][INDEX_2_4_HW_4] = arr[O_HW + 3];
        cs2_index[INDEX_2_4][INDEX_2_4_HW_5] = arr[O_HW + 4];
        cs2_index[INDEX_2_4][INDEX_2_4_HW_6] = arr[O_HW + 5];

        // Feste CS2-Geraetekennung, falls der Modultyp eine vorschreibt

        uint8_t knH, knL;
        if (kennerFixedExtern(knH, knL))
        {
            arr[O_KN_H] = knH;
            arr[O_KN_L] = knL;
        }

        can_msg[MBC_COM_MSG_CS2_0x30_R][MBC_CAN_D6] = arr[O_KN_H];
        can_msg[MBC_COM_MSG_CS2_0x30_R][MBC_CAN_D7] = arr[O_KN_L];

        // Kommando: Statusdaten Konfiguration 0x3A_R_6

        can_msg[MBC_COM_MSG_CS2_0x3A_R_6][MBC_CAN_D0] = arr[O_CS2GER];
        can_msg[MBC_COM_MSG_CS2_0x3A_R_6][MBC_CAN_D1] = arr[O_UID + 1];
        can_msg[MBC_COM_MSG_CS2_0x3A_R_6][MBC_CAN_D2] = arr[O_UID + 2];
        can_msg[MBC_COM_MSG_CS2_0x3A_R_6][MBC_CAN_D3] = arr[O_UID + 3];

        // Kommando: Status/Messwerte 0x00_R_8_0x0B

        can_msg[MBC_COM_MSG_CS2_0x00_R_8_0x0B][MBC_CAN_D0] = arr[O_CS2GER];
        can_msg[MBC_COM_MSG_CS2_0x00_R_8_0x0B][MBC_CAN_D1] = arr[O_UID + 1];
        can_msg[MBC_COM_MSG_CS2_0x00_R_8_0x0B][MBC_CAN_D2] = arr[O_UID + 2];
        can_msg[MBC_COM_MSG_CS2_0x00_R_8_0x0B][MBC_CAN_D3] = arr[O_UID + 3];

        // Kommando: Bestaetigung Konfigdaten 0x00_R_7_0x0B

        can_msg[MBC_COM_MSG_CS2_0x00_R_7_0x0B][MBC_CAN_D0] = arr[O_CS2GER];
        can_msg[MBC_COM_MSG_CS2_0x00_R_7_0x0B][MBC_CAN_D1] = arr[O_UID + 1];
        can_msg[MBC_COM_MSG_CS2_0x00_R_7_0x0B][MBC_CAN_D2] = arr[O_UID + 2];
        can_msg[MBC_COM_MSG_CS2_0x00_R_7_0x0B][MBC_CAN_D3] = arr[O_UID + 3];

        // Kommando: Geraetekennung Anfrage 0x00_5_0x0C

        can_msg[MBC_COM_MSG_CS2_0x00_5_0x0C][MBC_CAN_D0] = arr[O_CS2GER];
        can_msg[MBC_COM_MSG_CS2_0x00_5_0x0C][MBC_CAN_D1] = arr[O_UID + 1];
        can_msg[MBC_COM_MSG_CS2_0x00_5_0x0C][MBC_CAN_D2] = arr[O_UID + 2];
        can_msg[MBC_COM_MSG_CS2_0x00_5_0x0C][MBC_CAN_D3] = arr[O_UID + 3];

        // Kommando: Bestaetigung Geraetekennung 0x00_R_7_0x0C

        can_msg[MBC_COM_MSG_CS2_0x00_R_7_0x0C][MBC_CAN_D0] = arr[O_CS2GER];
        can_msg[MBC_COM_MSG_CS2_0x00_R_7_0x0C][MBC_CAN_D1] = arr[O_UID + 1];
        can_msg[MBC_COM_MSG_CS2_0x00_R_7_0x0C][MBC_CAN_D2] = arr[O_UID + 2];
        can_msg[MBC_COM_MSG_CS2_0x00_R_7_0x0C][MBC_CAN_D3] = arr[O_UID + 3];

        // GUiD in Konfigurationstabelle fuer CS2 sichern

        cs2_index[INDEX_2_2][INDEX_2_2_GU_1] = arr[O_UID];
        cs2_index[INDEX_2_2][INDEX_2_2_GU_2] = arr[O_UID + 1];
        cs2_index[INDEX_2_2][INDEX_2_2_GU_3] = arr[O_UID + 2];
        cs2_index[INDEX_2_2][INDEX_2_2_GU_4]     = MbcUtils::hex2ascii((arr[O_UID + 3] & 0xf0) >> 4);
        cs2_index[INDEX_2_2][INDEX_2_2_GU_4 + 1] = MbcUtils::hex2ascii(arr[O_UID + 3] & 0x0f);

        // GUiD und HASH fuer modulspezifische CAN-Nachrichten nachziehen lassen

        hashInitExtern();
    }

    //*********************************************************************************************
    //* GUiD-Wert fuer die PC-Kommunikation in den entsprechenden CAN-Nachrichtentemplates        *
    //* eintragen                                                                                 *
    //*********************************************************************************************

    void MbcCom::guidInit()
    {
        uint8_t *arr = _utils->array();

        can_msg[MBC_COM_MSG_ACK_MODUL][MBC_CAN_D4] = arr[O_UID];
        can_msg[MBC_COM_MSG_ACK_MODUL][MBC_CAN_D5] = arr[O_UID + 1];
        can_msg[MBC_COM_MSG_ACK_MODUL][MBC_CAN_D6] = arr[O_UID + 2];
        can_msg[MBC_COM_MSG_ACK_MODUL][MBC_CAN_D7] = arr[O_UID + 3];

        can_msg[MBC_COM_MSG_ACK_IOREQUEST][MBC_CAN_D4] = arr[O_UID];
        can_msg[MBC_COM_MSG_ACK_IOREQUEST][MBC_CAN_D5] = arr[O_UID + 1];
        can_msg[MBC_COM_MSG_ACK_IOREQUEST][MBC_CAN_D6] = arr[O_UID + 2];
        can_msg[MBC_COM_MSG_ACK_IOREQUEST][MBC_CAN_D7] = arr[O_UID + 3];

        can_msg[MBC_COM_MSG_ACK_UPREQUEST][MBC_CAN_D4] = arr[O_UID];
        can_msg[MBC_COM_MSG_ACK_UPREQUEST][MBC_CAN_D5] = arr[O_UID + 1];
        can_msg[MBC_COM_MSG_ACK_UPREQUEST][MBC_CAN_D6] = arr[O_UID + 2];
        can_msg[MBC_COM_MSG_ACK_UPREQUEST][MBC_CAN_D7] = arr[O_UID + 3];
    }

    //*********************************************************************************************
    //* Loest einen sofortigen Neustart des Moduls aus (PC_RESET/PC_BOOT); kehrt nicht zurueck    *
    //*********************************************************************************************

    void MbcCom::triggerReset()
    {
        // Nur der ATmega644P (AVR) besitzt keinen per Software auszuloesenden Chip-Reset -
        // dort wird ueber den extern beschalteten Reset-Pin (_pinReset) neu gestartet. Auf
        // RP2040/ESP32-C3 genuegt der Software-Reset des jeweiligen Cores.

        #if defined(__AVR__)
            MbcPlatform::resetViaPin(_pinReset);
        #else
            #if defined(ARDUINO_ARCH_RP2040) || defined(ESP32)
                // Steht im Boot-EEPROM ein vollstaendiges, gueltiges OTA-Image an, wird es
                // hier in die inaktive Flash-Partition uebernommen; ohne gueltiges Image
                // aendert apply() nichts und der folgende Reset startet unveraendert neu.

                MbcOta::apply(*_bootStorage, _utils);
            #endif

            MbcPlatform::resetDevice();
        #endif

        while (true) { }
    }

    //*********************************************************************************************
    //* Kommunikation vorbereiten: Flags initialisieren und - falls das Modul bereits eine        *
    //* gueltige GUiD besitzt - die HASH-Werte berechnen sowie Serien-/Artikelnummer/Firmware     *
    //* in die CS2-Konfigurationstabelle eintragen. Nach MbcUtils::begin() aufrufen.              *
    //*********************************************************************************************

    void MbcCom::begin()
    {
        uint8_t *arr = _utils->array();

        moduleFlagsReset();

        // Hatte das Modul schon eine gueltige GUiD? Dann Modul fuer CS2-Kommunikation
        // vorbereiten und die HASH-Werte berechnen.

        if (arr[O_UID] != 0xff)
        {
            guidInit();
            hashInit();

            _init.init = true;
            _init.startup = true;
            _cs.autoMode = false;
        }
        else
        {
            // Hilfs-Hashwerte fuer m80#1 (PC) eintragen

            for (uint8_t index = 1; index <= 5; index++)
            {
                can_msg[index][MBC_CAN_HASH_H] = 0x5f;
                can_msg[index][MBC_CAN_HASH_L] = 0x38;
            }

            can_msg[MBC_COM_MSG_CS2_0x16][MBC_CAN_HASH_H] = 0x5f;
            can_msg[MBC_COM_MSG_CS2_0x16][MBC_CAN_HASH_L] = 0x38;

            _init.init = false;
        }

        // Serien-/Artikelnummer/Modultyp in INDEX_0 speichern

        for (uint8_t index = 0; index < 4; index++)
        {
            cs2_index[INDEX_0][INDEX_0_ART_NR + index] = arr[O_ART + index];
        }

        for (uint8_t index = 0; index < 2; index++)
        {
            cs2_index[INDEX_0][INDEX_0_TYP + index] = arr[O_ART + index];
        }

        // Seriennummer in INDEX_2_1 speichern (als Hex-ASCII)

        for (uint8_t index = 0; index < 8; index++)
        {
            cs2_index[INDEX_2_1][INDEX_2_1_SERNRDS + (2 * index)]     = MbcUtils::hex2ascii((arr[O_SNR + index] & 0xf0) >> 4);
            cs2_index[INDEX_2_1][INDEX_2_1_SERNRDS + (2 * index) + 1] = MbcUtils::hex2ascii(arr[O_SNR + index] & 0x0f);
        }

        // Firmwarenummer in INDEX_2_3 speichern

        cs2_index[INDEX_2_3][INDEX_2_3_FW_1] = MbcUtils::hex2ascii(arr[O_SW]     - 0x30);
        cs2_index[INDEX_2_3][INDEX_2_3_FW_2] = MbcUtils::hex2ascii(arr[O_SW + 1] - 0x30);
        cs2_index[INDEX_2_3][INDEX_2_3_FW_3] = MbcUtils::hex2ascii(arr[O_SW + 2] - 0x30);
    }

    //*********************************************************************************************
    //* Empfangene CAN-Nachricht auswerten und ggf. beantworten - ermittelt, ob                   *
    //* GUiD-Uebereinstimmung fuer PC- bzw. CS2-Verbindung vorliegt, und verzweigt entsprechend   *
    //* an checkPcConnection()/checkCs2Connection()                                               *
    //*********************************************************************************************
    //* msg: MBC_COM_MSG_LAENGE Byte (siehe MbcCan MBC_CAN_*-Indexkonstanten)                     *
    //*********************************************************************************************

    void MbcCom::check(const uint8_t msg[MBC_COM_MSG_LAENGE])
    {
        uint8_t *arr = _utils->array();

        bool isPcGuid = (msg[MBC_CAN_D4] == arr[O_UID]) && (msg[MBC_CAN_D5] == arr[O_UID + 1])
                      && (msg[MBC_CAN_D6] == arr[O_UID + 2]) && (msg[MBC_CAN_D7] == arr[O_UID + 3]);

        bool isCs2Guid = (msg[MBC_CAN_D0] == arr[O_UID]) && (msg[MBC_CAN_D1] == arr[O_UID + 1])
                       && (msg[MBC_CAN_D2] == arr[O_UID + 2]) && (msg[MBC_CAN_D3] == arr[O_UID + 3]);

        //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        //+ PC-Verbindung                                                                         +
        //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

        if (msg[MBC_CAN_ID] == CAN_ID_ZUBEHOER_SCHALTEN)
        {
            if (msg[MBC_CAN_D2] == CAN_LOCID_MBCAN) checkPcConnection(msg, isPcGuid);
        }

        //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        //+ CS2-Verbindung                                                                        +
        //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

        if (_init.init && ((msg[MBC_CAN_ID] & 0x01) == 0x00))
        {
            checkCs2Connection(msg, isCs2Guid);
        }
    }

    //*********************************************************************************************
    //* Bedient nicht-blockierend zeitverzoegerte Aktionen - aktuell die gestreute Ping-Antwort   *
    //* (siehe checkCs2Connection(), CAN_ID_PING). MUSS jeden loop()-Durchlauf aufgerufen werden, *
    //* unabhaengig davon, ob eine Nachricht empfangen wurde, sonst verzoegert/entfaellt der      *
    //* Versand.                                                                                  *
    //*********************************************************************************************

    void MbcCom::poll()
    {
        if (_ping.pending && ((int32_t) (micros() - _ping.dueUs) >= 0))
        {
            sendMessage(can_msg[MBC_COM_MSG_CS2_0x30_R]);
            _cs.connect = true;

            _ping.pending = false;
        }
    }

    //*********************************************************************************************
    //* Wertet die PC-Kommunikation (Loc-ID 0x18xx) aus: Datenbanksynchronisation,                *
    //* Neuanmeldeautomat, Modul-/Datentransferbefehle (PC_RESET, PC_ALIVE, PC_ARRAY_DATA,        *
    //* PC_UPGRADE_DATA, PC_BOOT, ...)                                                            *
    //*********************************************************************************************
    //* msg:      empfangene CAN-Nachricht                                                        *
    //* isPcGuid: true, wenn D4...D7 der Nachricht der eigenen GUiD entsprechen                   *
    //*********************************************************************************************

    void MbcCom::checkPcConnection(const uint8_t msg[MBC_COM_MSG_LAENGE], bool isPcGuid)
    {
        uint8_t *arr = _utils->array();

        // Datenbanknummer synchronisieren, sobald alle drei Teile vorliegen

        if (_db.dbH && _db.dbM && _db.dbL)
        {
            _db.dbValid = true;

            for (uint8_t index = 0; index < 12; index++)
            {
                if (arr[O_DB + index] != _db.db[index])
                {
                    _db.dbValid = false;
                    arr[O_DB + index] = _db.db[index];
                }
            }

            if (_init.init && !_db.dbValid)
            {
                arr[O_UID]     = 0xff;
                arr[O_UID + 1] = 0xff;
                arr[O_UID + 2] = 0xff;
                arr[O_UID + 3] = 0xff;

                _init.newPhase = false;
                _io.transfer = false;
                _io.write = false;
                _io.read = false;
                _init.init = false;
                _cs.autoMode = false;
            }

            _db.dbH = false;
            _db.dbM = false;
            _db.dbL = false;
            _db.dbValid = true;
        }

        switch (msg[MBC_CAN_D3])
        {
            case PC_KENNER:
            {
                if (msg[MBC_CAN_D5] != arr[O_CS2GER])
                {
                    arr[O_CS2GER] = msg[MBC_CAN_D5];

                    // GUiD-Byte 0 (arr[O_UID]) synchron halten - sonst zeigt die Ping-Antwort
                    // (baut auf O_CS2GER auf) sofort die neue GUiD, waehrend isCs2Guid/isPcGuid
                    // (bauen auf O_UID auf) noch die alte erwarten und CSx-Anfragen unter der
                    // neuen GUiD (z.B. Statusdaten Konf 0x3A) daher folgenlos verworfen wuerden.

                    arr[O_UID] = arr[O_CS2GER];

                    hashInit();

                    _utils->eepromWrite(MBC_EE_KENNUNG);
                    _utils->eepromWrite(MBC_EE_GUID);
                }

                uint8_t knH, knL;

                if (kennerFixedExtern(knH, knL))
                {
                    if ((arr[O_KN_H] != knH) || (arr[O_KN_L] != knL))
                    {
                        arr[O_KN_H] = knH;
                        arr[O_KN_L] = knL;
                        hashInit();
                    }
                }
                else
                {
                    if ((arr[O_KN_H] != msg[MBC_CAN_D6]) || (arr[O_KN_L] != msg[MBC_CAN_D7]))
                    {
                        arr[O_KN_H] = msg[MBC_CAN_D6];
                        arr[O_KN_L] = msg[MBC_CAN_D7];
                        hashInit();

                        _utils->eepromWrite(MBC_EE_KENNUNG);
                    }
                }
            }
            break;

            case PC_DB_H:

                for (uint8_t index = 0; index < 4; index++) _db.db[index] = msg[MBC_CAN_D4 + index];
                _db.dbH = true;

            break;

            case PC_DB_M:

                for (uint8_t index = 4; index < 8; index++) _db.db[index] = msg[MBC_CAN_D4 + index - 4];
                _db.dbM = true;

            break;

            case PC_DB_L:

                for (uint8_t index = 8; index < 12; index++) _db.db[index] = msg[MBC_CAN_D4 + index - 8];
                _db.dbL = true;

            break;

            case PC_NEU:

                // Neuanmeldung nur, wenn das Modul noch nicht angemeldet ist, keine
                // Fremdantwort vorliegt (D7==0) und eine gueltige Datenbanknummer existiert.

                if (!_init.init && (msg[MBC_CAN_D7] == 0) && _db.dbValid)
                {
                    _init.newPhase = true;
                    _init.step = 1;
                    _init.sysindex = MBC_INFO_START;

                    can_msg[MBC_COM_MSG_INFO][MBC_CAN_D6] = _init.step;
                    can_msg[MBC_COM_MSG_INFO][MBC_CAN_D7] = arr[_init.sysindex];

                    sendMessage(can_msg[MBC_COM_MSG_INFO]);
                }

            break;

            case PC_NEU_DATA:

                // Neuanmeldeautomat: Infoblock (Seriennummer/Artikelnummer/SW/HW/Name)
                // schrittweise senden, solange der PC jeden Wert korrekt spiegelt.

                if (!_init.init && _init.newPhase)
                {
                    if ((_init.step == msg[MBC_CAN_D6]) && (arr[_init.sysindex] == msg[MBC_CAN_D7]))
                    {
                        if (_init.sysindex <= MBC_INFO_ENDE)
                        {
                            _init.step++;
                            _init.sysindex++;

                            can_msg[MBC_COM_MSG_INFO][MBC_CAN_D6] = _init.step;
                            can_msg[MBC_COM_MSG_INFO][MBC_CAN_D7] = arr[_init.sysindex];

                            sendMessage(can_msg[MBC_COM_MSG_INFO]);
                        }
                        else
                        {
                            // Infoblock komplett - GUiD aus CS2-Geraetegruppe/Artikelnummer/
                            // Modulindex (letzte PC-Nachricht) zusammensetzen

                            arr[O_UID]     = arr[O_CS2GER];
                            arr[O_UID + 1] = arr[O_ART];
                            arr[O_UID + 2] = arr[O_ART + 1];
                            arr[O_UID + 3] = msg[MBC_CAN_D5];

                            guidInit();

                            _utils->eepromWrite(MBC_EE_GUID);
                            _utils->eepromWrite(MBC_EE_DB);

                            _init.init = true;
                            _init.startup = true;
                            _init.newPhase = false;

                            hashInit();

                            _utils->ledToggleOn(MBC_LED_PC_OK);
                        }
                    }
                    else
                    {
                        _init.init = false;
                        _init.newPhase = false;
                        _io.transfer = false;
                        _io.write = false;
                        _io.read = false;
                        _cs.autoMode = false;

                        _init.step = 0;
                        _init.sysindex = 0;
                    }
                }

            break;

            default: break;

        } // Ende switch(msg[D3])

        if (_init.init)
        {
            if (isPcGuid)
            {
                switch (msg[MBC_CAN_D3])
                {
                    case PC_RESET:

                        moduleFlagsReset();
                        triggerReset();

                    break;

                    case PC_MD_DEL:

                        moduleBlank();

                    break;

                    case PC_ALIVE:

                        sendMessage(can_msg[MBC_COM_MSG_ACK_MODUL]);

                    break;

                    case PC_ARRAY:

                        sendMessage(can_msg[MBC_COM_MSG_ACK_IOREQUEST]);
                        _io.transfer = true;

                    break;

                    case PC_UPGRADE:

                        sendMessage(can_msg[MBC_COM_MSG_ACK_UPREQUEST]);
                        _io.transfer = true;

                    break;

                    case PC_BOOT:
                    {
                        // NUR AVR (ATmega644P): Page 0 des Boot-Speicherchips mit 0xaa fluten -
                        // Trigger-Konvention fuer den ATmega-eigenen Bootloader. Auf RP2040/
                        // ESP32-C3 liegt in genau diesem Speicherchip (W25Q64) das gerade per
                        // PC_UPGRADE_DATA uebertragene Firmware-Image (MbcOta) - dort wuerde
                        // dieser Schritt Page 0 des frisch geschriebenen Images vor dem
                        // Anwenden zerstoeren.

                        #if defined(__AVR__)
                            uint8_t *bootArr = arr + O_BOOT + 2;

                            for (uint16_t count = 0; count < MBC_BOOT_PGSZ; count++) bootArr[count] = 0xaa;

                            _bootStorage->write(bootArr, 0, _utils);

                            delay(10);
                        #endif

                        moduleBlank();

                        // NUR AVR: Interrupts vor dem Reset-Pin-Trick global sperren, damit kein
                        // Interrupt-Handler waehrend des Sprungs in den Bootloader mehr feuert.
                        // Auf RP2040/ESP32-C3 wuerde das direkt vor MbcOta::apply() (siehe
                        // triggerReset()) dessen Update.write()-Aufrufe stoeren - der ESP32-/
                        // RP2040-Flash-Treiber verwaltet seine eigenen kritischen Abschnitte samt
                        // Watchdog-/Scheduler-Interaktion selbst und erwartet dafuer laufende
                        // Interrupts; ein global vorab gesperrter Zustand kann dort zu einem
                        // Watchdog-Reset MITTEN im Schreibvorgang fuehren, bevor die neue
                        // Firmware fertig uebernommen ist - das Modul startet dann per Panic statt
                        // per regulaerem Reset neu und landet unveraendert in der alten Firmware.

                        #if defined(__AVR__)
                            noInterrupts();
                        #endif

                        triggerReset();
                    }
                    break;

                    default: break;
                }
            }
            else if (_io.transfer)
            {
                switch (msg[MBC_CAN_D3])
                {
                    case PC_ARRAY:
                    case PC_UPGRADE:

                        _io.transfer = false;
                        _io.write = false;
                        _io.read = false;

                    break;

                    case PC_ARRAY_DATA:
                    {
                        can_msg[MBC_COM_MSG_ACK_IOACT][MBC_CAN_D4] = msg[MBC_CAN_D4];
                        can_msg[MBC_COM_MSG_ACK_IOACT][MBC_CAN_D5] = msg[MBC_CAN_D5];
                        can_msg[MBC_COM_MSG_ACK_IOACT][MBC_CAN_D6] = msg[MBC_CAN_D6];
                        can_msg[MBC_COM_MSG_ACK_IOACT][MBC_CAN_D7] = msg[MBC_CAN_D7];

                        uint16_t index = (((uint16_t) msg[MBC_CAN_D5]) << 8) | (uint16_t) msg[MBC_CAN_D6];

                        // Gueltige Indizes sind 0...arrayMax()-1 (kein Byte des Systemarrays ist
                        // reserviert - die Sentinel-Werte MBC_COM_WRITE/READ/EEPROM liegen weit
                        // ausserhalb bei 0xfffd...0xffff). Ein "- 1" hier wuerde das letzte Byte
                        // des Systemarrays fuer PC_ARRAY_DATA unerreichbar machen, falls eine
                        // Unterklasse dort noch ein eigenes Feld belegt.

                        if (index < _utils->arrayMax())
                        {
                            if (msg[MBC_CAN_D4] == 1) arr[index] = msg[MBC_CAN_D7];
                            else                        can_msg[MBC_COM_MSG_ACK_IOACT][MBC_CAN_D7] = arr[index];
                        }

                        if (index == MBC_COM_WRITE)
                        {
                            if (msg[MBC_CAN_D7] == 1) _io.write = true;
                            else { _io.write = false; _io.transfer = false; }
                        }

                        if (index == MBC_COM_READ)
                        {
                            if (msg[MBC_CAN_D7] == 1) _io.read = true;
                            else { _io.read = false; _io.transfer = false; }
                        }

                        if (index == MBC_COM_EEPROM)
                        {
                            if (msg[MBC_CAN_D7] < 10)
                            {
                                _utils->eepromWrite(msg[MBC_CAN_D7]);
                            }
                            else
                            {
                                eepromWriteParaExtern(msg[MBC_CAN_D7]);
                            }

                            _utils->ledToggleOn(MBC_LED_IO_OK);
                        }

                        sendMessage(can_msg[MBC_COM_MSG_ACK_IOACT]);
                    }
                    break;

                    case PC_UPGRADE_DATA:
                    {
                        uint8_t *bootArr = arr + O_BOOT;

                        uint16_t bootindex = (((uint16_t) (msg[MBC_CAN_HASH_H] - 0x03) << 6)
                                              | (uint16_t) msg[MBC_CAN_HASH_L]) - 1;

                        // Schutz gegen fehlerhafte/beschaedigte Nachrichten: bootindex liegt
                        // ausserhalb des gueltigen Bereichs von bootArr (0...MBC_BOOT_PGMAX),
                        // ein Schreibzugriff wuerde sonst weit ausserhalb des Boot-Staging-
                        // Puffers im Systemarray landen.

                        if (bootindex > MBC_BOOT_PGMAX) break;

                        if (bootindex == MBC_BOOT_PGMAX)
                        {
                            bootArr[4 * bootindex]     = msg[MBC_CAN_D4];
                            bootArr[4 * bootindex + 1] = msg[MBC_CAN_D5];

                            uint16_t page = ((uint16_t) bootArr[0] << 8) | (uint16_t) bootArr[1];

                            _bootStorage->write(bootArr + 2, page, _utils);

                            _io.transfer = false;
                        }
                        else
                        {
                            bootArr[4 * bootindex]     = msg[MBC_CAN_D4];
                            bootArr[4 * bootindex + 1] = msg[MBC_CAN_D5];
                            bootArr[4 * bootindex + 2] = msg[MBC_CAN_D6];
                            bootArr[4 * bootindex + 3] = msg[MBC_CAN_D7];
                        }

                        can_msg[MBC_COM_MSG_ACK_UPACT][MBC_CAN_D4] = msg[MBC_CAN_D4];
                        can_msg[MBC_COM_MSG_ACK_UPACT][MBC_CAN_D5] = msg[MBC_CAN_D5];
                        can_msg[MBC_COM_MSG_ACK_UPACT][MBC_CAN_D6] = msg[MBC_CAN_D6];
                        can_msg[MBC_COM_MSG_ACK_UPACT][MBC_CAN_D7] = msg[MBC_CAN_D7];

                        sendMessage(can_msg[MBC_COM_MSG_ACK_UPACT]);
                    }
                    break;

                    default: break;
                }
            }
        }
    }

    //*********************************************************************************************
    //* Wertet die CS2-Kommunikation aus: Ping-Antwort, Konfigurationsdaten-Uebertragung          *
    //* (CAN_ID_KONFIG) sowie Status-/Messwertabfragen (CAN_ID_SYSTEM_BEFEHLE)                    *
    //*********************************************************************************************
    //* msg:       empfangene CAN-Nachricht                                                       *
    //* isCs2Guid: true, wenn D0...D3 der Nachricht der eigenen GUiD entsprechen                  *
    //*********************************************************************************************

    void MbcCom::checkCs2Connection(const uint8_t msg[MBC_COM_MSG_LAENGE], bool isCs2Guid)
    {
        uint8_t *arr = _utils->array();

        switch (msg[MBC_CAN_ID])
        {
            case CAN_ID_PING:

                // Streuung NICHT-BLOCKIEREND ueber poll() nachziehen lassen (statt hier per
                // delayMicroseconds() zu warten) - ein blockierender Wartezeitraum wuerde JEDE
                // andere Nachrichtenverarbeitung im selben loop() ausbremsen, u.a. die eng
                // getaktete PC_UPGRADE_DATA-Chunk-Quittierung (checkPcConnection()) - so
                // hardwarebestaetigt als Ursache eines Upgrade-Stillstands nach ca. 10
                // Datenpaketen gefunden. Waehrend einer laufenden PC_ARRAY-/PC_UPGRADE-Uebertragung
                // (_io.transfer) ohnehin sofort ohne Streuung antworten - Kollisionsvermeidung
                // zwischen baugleichen Modulen ist dort nicht relevant (typischerweise nur ein
                // Modul aktiv im Upgrade).

                if (_io.transfer)
                {
                    sendMessage(can_msg[MBC_COM_MSG_CS2_0x30_R]);
                    _cs.connect = true;
                }
                else
                {
                    uint16_t hash = (uint16_t) (((uint16_t) can_msg[MBC_COM_MSG_CS2_0x30_R][MBC_CAN_HASH_H] << 8)
                                                | (uint16_t) can_msg[MBC_COM_MSG_CS2_0x30_R][MBC_CAN_HASH_L]);

                    _ping.dueUs   = micros() + (hash % PING_STAGGER_RANGE_US);
                    _ping.pending = true;
                }

            break;

            case CAN_ID_KONFIG:

                if (isCs2Guid)
                {
                    uint8_t idNr = msg[MBC_CAN_D4];

                    // Kanal-Layout (wire-Kanalnummer idNr): 0=Geraeteinfo, 1..ANZMESSWERTE=Basis-
                    // Messwerte, direkt dahinter anzMesswerteModul modulspezifische Messwertkanaele
                    // (siehe Konstruktor), dann ANZKONFIG Basis-Konfigkanaele (dort ebenfalls um
                    // anzMesswerteModul verschoben), ganz am Ende die modulspezifischen
                    // Konfigkanaele - generisch fuer jede Anzahl modulspezifischer Messwert-/
                    // Konfigkanaele, nicht nur fuer den aktuell groessten bekannten Fall

                    const uint8_t *sourceArr;

                    if (idNr <= MBC_COM_ANZMESSWERTE)
                    {
                        sourceArr = cs2_index[idNr];
                    }
                    else if (idNr <= (uint8_t) (MBC_COM_ANZMESSWERTE + _anzMesswerteModul))
                    {
                        sourceArr = cs2IndexExtern((uint8_t) (idNr - MBC_COM_ANZMESSWERTE - 1));
                    }
                    else if (idNr <= (uint8_t) (MBC_COM_ANZMESSWERTE + _anzMesswerteModul + MBC_COM_ANZKONFIG))
                    {
                        sourceArr = cs2_index[idNr - _anzMesswerteModul];
                    }
                    else
                    {
                        sourceArr = cs2IndexExtern((uint8_t) (idNr - MBC_COM_ANZMESSWERTE - MBC_COM_ANZKONFIG - 1));
                    }

                    if (sourceArr == nullptr) break;

                    for (uint8_t paket = 0; paket < MBC_COM_CS2PAKETE; paket++)
                    {
                        for (uint8_t index = 0; index < 8; index++)
                        {
                            can_msg[MBC_COM_MSG_CS2_0x3A_R_8][MBC_CAN_D0 + index] = sourceArr[index + (paket * 8)];
                        }

                        can_msg[MBC_COM_MSG_CS2_0x3A_R_8][MBC_CAN_HASH_H] = 0x03;
                        can_msg[MBC_COM_MSG_CS2_0x3A_R_8][MBC_CAN_HASH_L] = (uint8_t) (paket + 1);

                        sendMessage(can_msg[MBC_COM_MSG_CS2_0x3A_R_8]);

                        delayMicroseconds(CS2_PAKET_DELAY_US);
                    }

                    can_msg[MBC_COM_MSG_CS2_0x3A_R_6][MBC_CAN_D4] = idNr;
                    can_msg[MBC_COM_MSG_CS2_0x3A_R_6][MBC_CAN_D5] = MBC_COM_CS2PAKETE;

                    sendMessage(can_msg[MBC_COM_MSG_CS2_0x3A_R_6]);

                    _utils->ledToggleOn(MBC_LED_CS2_OK);

                    _cs.init = true;
                    _cs.startup = true;
                    _cs.autoMode = false;
                }

            break;

            case CAN_ID_SYSTEM_BEFEHLE:

                if (isCs2Guid && (msg[MBC_CAN_D4] == CAN_ID_SYSTEM_BEFEHLE_SUB_MW))
                {
                    if (msg[MBC_CAN_DLC] == 8)
                    {
                        cs2IndexParameterExtern(msg);
                    }

                    if (msg[MBC_CAN_DLC] == 6)
                    {
                        uint16_t index;

                        switch (msg[MBC_CAN_D5])
                        {
                            case 1:

                                index = ((uint16_t) (arr[O_SPG] - 0x30)) * 1000
                                      + ((uint16_t) (arr[O_SPG + 1] - 0x30)) * 100
                                      + (uint16_t) (arr[O_SPG + 2] - 0x30) * 10;
                                index = (index - 1000) / 2;

                                can_msg[MBC_COM_MSG_CS2_0x00_R_8_0x0B][MBC_CAN_D5] = INDEX_1_1;
                                can_msg[MBC_COM_MSG_CS2_0x00_R_8_0x0B][MBC_CAN_D6] = (uint8_t) ((index & 0xff00) >> 8);
                                can_msg[MBC_COM_MSG_CS2_0x00_R_8_0x0B][MBC_CAN_D7] = (uint8_t) (index & 0x00ff);

                                sendMessage(can_msg[MBC_COM_MSG_CS2_0x00_R_8_0x0B]);

                            break;

                            case 2:

                                index = ((uint16_t) (arr[O_TMP] - 0x30)) * 100
                                      + ((uint16_t) (arr[O_TMP + 1] - 0x30)) * 10
                                      + (uint16_t) (arr[O_TMP + 2] - 0x30);
                                index = index / 4;

                                can_msg[MBC_COM_MSG_CS2_0x00_R_8_0x0B][MBC_CAN_D5] = INDEX_1_2;
                                can_msg[MBC_COM_MSG_CS2_0x00_R_8_0x0B][MBC_CAN_D6] = (uint8_t) ((index & 0xff00) >> 8);
                                can_msg[MBC_COM_MSG_CS2_0x00_R_8_0x0B][MBC_CAN_D7] = (uint8_t) (index & 0x00ff);

                                sendMessage(can_msg[MBC_COM_MSG_CS2_0x00_R_8_0x0B]);

                            break;

                            default:

                                cs2IndexMesswerteExtern(msg[MBC_CAN_D5]);

                            break;
                        }
                    }
                }

            break;

            default: break;
        }
    }

//------ Ende Klasse MbcCom -----------------------------------------------------------------------

//###### Ende Quellcode MbcCom.cpp ################################################################
