//*************************************************************************************************
//* HEADER - Modul mbc-80: MBCAN-Bus <-> CS2-Bus <-> WLAN-Gateway (Mbc80Lib)                      *
//*************************************************************************************************
//* mbc-80 ist kein Modul mit eigenen Schalt-/Rueckmelde-I/Os, sondern ein reines Gateway: es     *
//* haengt an drei Kommunikationswegen und leitet CAN-Nachrichten zwischen ihnen weiter - dem     *
//* modulinternen MBCAN-Bus (eigener MCP2515-Kanal), dem echten Maerklin-CS2-Bus (zweiter         *
//* CAN-Kanal, gleichzeitig auch der Kanal, ueber den die normale                                 *
//* MbcCom-Modulidentitaets-/Parametrierkommunikation laeuft - can-Parameter des                  *
//* MbcCom-Konstruktors) und einer dritten, austauschbaren Nachrichten-Bruecke (MbcMsgBridge -    *
//* z.B. MbcWlanBridge fuer ein externes WLAN-Modul per UART, oder MbcWifiBridge fuer natives     *
//* WLAN auf dem ESP32; welche Implementierung verwendet wird, entscheidet allein der Sketch).    *
//* Simuliert dabei ein Maerklin-L88-Geraet (feste Kennung 0x0040) inkl.                          *
//* S88-Automatik-Kenner-Anfrage/-Verwaltung.                                                     *
//*************************************************************************************************
//* Datei: Mbc80Com.h                                                                             *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen:                                                                                  *
//* V1.0.0 - 05.08.2026 - Nullversion                                                             *
//* V1.0.1 - 29.08.2026 - RAM-Optimierung                                                         *
//*************************************************************************************************

#pragma once

//###### Start Quellcode Mbc80Com.h ###############################################################

#ifndef _MBC80COM_H_
#define _MBC80COM_H_     "V1.0.1 - 29.08.2026"

//------ Einbindung Bibliotheken ------------------------------------------------------------------

#include <Arduino.h>
#include <MbcCom.h>
#include <MbcMsgBridge.h>

//------ Start Klassendefinition Mbc80Com ---------------------------------------------------------

    //* Klasse fuer <Mbc80Com>
    //* Modulspezifische MbcCom-Unterklasse fuer mbc-80. can (MbcCom-Basisklasse) = CS2-Bus,
    //* canMbcan (zusaetzlich) = modulinterner MBCAN-Bus, bridge = austauschbare Nachrichten-
    //* Bruecke (MbcWlanBridge/MbcWifiBridge/...). Zusaetzlich melden/aendern zwei
    //* CS2-Konfigkanaele direkt in der CSx die vorgetaeuschte L88-Softwareversion
    //* (Slider, Label "SW Major:"/"SW Minor:", je 1...255), siehe cs2IndexParameterExtern().

    class Mbc80Com : public MbcCom
    {
        public:

            //*************************************************************************************
            //* Konstruktor zur Deklaration von Objektvariablen                                   *
            //*************************************************************************************
            //* utils:      zugehoerige MbcUtils-Instanz                                          *
            //* canCs2:     CAN-Kanal zum Maerklin-CS2-Bus (= can-Parameter der Basisklasse)      *
            //* canMbcan:   CAN-Kanal zum modulinternen MBCAN-Bus                                 *
            //* bootStorage: zugehoerige MbcBootStorage-Instanz (Firmware-Update-Staging)         *
            //* bridge:     zugehoerige Nachrichten-Bruecke (MbcWlanBridge, MbcWifiBridge, ...)   *
            //* pinReset:   NUR auf AVR (ATmega644P) vorhanden, siehe MbcCom-Konstruktor          *
            //*************************************************************************************

            #if defined(__AVR__)
                Mbc80Com(MbcUtils &utils, MbcCan &canCs2, MbcCan &canMbcan, MbcBootStorage &bootStorage,
                         MbcMsgBridge &bridge, uint8_t pinReset);
            #else
                Mbc80Com(MbcUtils &utils, MbcCan &canCs2, MbcCan &canMbcan, MbcBootStorage &bootStorage,
                         MbcMsgBridge &bridge);
            #endif

            //*************************************************************************************
            //* Einmalig nach dem begin() der Basisklasse aufrufen: gespeicherte                  *
            //* L88-Softwareversion aus der EEPROM-Zusatzregion laden.                            *
            //*************************************************************************************

            void beginModule();

            //*************************************************************************************
            //* Je loop()-Durchlauf aufrufen: MBCAN<->CS2<->WLAN-Weiterleitung, zyklische         *
            //* Schaltdecoder-Basisadress-Rueckmeldung und S88-Automatik-Kenner-Verwaltung.       *
            //*************************************************************************************

            void bridge();

        protected:

            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //+ Die 7 Erweiterungspunkte der Basisklasse (siehe MbcCom.h fuer die volle           +
            //+ Beschreibung je Hook) - bei mbc-80 haben swInitExtern()/                          +
            //+ eepromWriteParaExtern()/eepromReadParaExtern()/hashInitExtern()/                  +
            //+ cs2IndexParameterExtern() echte Logik (simulierte L88-Softwareversion, jetzt per  +
            //+ CS2-Konfigkanal aenderbar, siehe Klassenkommentar), der Rest bleibt leer          +
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

                //*********************************************************************************
                //* Laedt die gespeicherte L88-Softwareversion aus der EEPROM-Zusatzregion (mit   *
                //* Defaultwert [1,1] fuer den unbeschriebenen Zustand, 0xff-Sentinel).           *
                //*********************************************************************************

                void eepromReadParaExtern() override;

                //*********************************************************************************
                //* Persistiert die L88-Softwareversion in der EEPROM-Zusatzregion und haelt      *
                //* can_msg[] konsistent zum RAM-Wert.                                            *
                //*********************************************************************************
                //* block: Identifier des zu schreibenden Blocks (0x0A....)                       *
                //*********************************************************************************

                void eepromWriteParaExtern(uint8_t block) override;

                //*********************************************************************************
                //* mbc-80 hat nichts zu loeschen. Default: nichts zu tun.                        *
                //*********************************************************************************

                void eepromEraseParaExtern() override;

                //*********************************************************************************
                //* Traegt GUiD/HASH in die modulspezifische Konfig-Bestaetigungsnachricht        *
                //* _msgKonfigOk ein.                                                             *
                //*********************************************************************************

                void hashInitExtern() override;

                //*********************************************************************************
                //* Setzt die simulierte L88-Softwareversion in die CS2-Ping-Antwort.             *
                //*********************************************************************************

                void swInitExtern() override;

                //*********************************************************************************
                //* mbc-80 hat keine modulspezifischen Messwerte (ANZMESSWERTEMODUL=0). Default:  *
                //* nichts zu tun.                                                                *
                //*********************************************************************************
                //* kanal: Messkanalnummer                                                        *
                //*********************************************************************************

                void cs2IndexMesswerteExtern(uint8_t kanal) override;

                //*********************************************************************************
                //* Modulspezifischen Parameterwert von der CS2 uebernehmen:                      *
                //* msg[D5]==KANAL_L88SW_MAJ/KANAL_L88SW_MIN - neues Major-/Minor-Byte der        *
                //* vorgetaeuschten L88-Softwareversion (Rohwert 1...255) uebernehmen,            *
                //* persistieren und der CSx bestaetigen. Ein Wert ausserhalb 1...255 bleibt ohne *
                //* Wirkung und wird per FALSE quittiert.                                         *
                //*********************************************************************************
                //* msg: CAN-Nachricht (msg[D5]==KANAL_L88SW_MAJ/KANAL_L88SW_MIN msg[D6..D7]=Wert)*
                //*********************************************************************************

                void cs2IndexParameterExtern(const uint8_t msg[MBC_COM_MSG_LAENGE]) override;

            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //+ Zusaetzliche, mbc-80-spezifische Erweiterungspunkte (siehe                        +
            //+ Kopfkommentar von MbcCom.h)                                                       +
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

                //*********************************************************************************
                //* Liefert die feste CS2-Geraetekennung 0x0040 (Maerklin-Vorgabe fuer            *
                //* L88-Automatikgeraete).                                                        *
                //*********************************************************************************
                //* knH/knL: gesetzt auf die feste Kennung, wenn Return true                      *
                //*********************************************************************************
                //* Return: true (mbc-80 hat immer eine feste Kennung)                            *
                //*********************************************************************************

                bool kennerFixedExtern(uint8_t &knH, uint8_t &knL) override;

                //*********************************************************************************
                //* Reicht ausgehende Nachrichten der Basisklasse zusaetzlich an die              *
                //* Nachrichten-Bruecke weiter (WLAN/RS232,                                       *
                //* Modulidentitaets-/Parametrier-Kommunikation).                                 *
                //*********************************************************************************
                //* msg: Nachrichtenarray                                                         *
                //*********************************************************************************

                void sendMessageExtern(const uint8_t msg[MBC_COM_MSG_LAENGE]) override;

                //*********************************************************************************
                //* Baut die angeforderte Kanalzeile (die beiden L88-Softwareversion-             *
                //* Konfigkanaele) frisch in _cs2ScratchRow zusammen (PROGMEM-Vorlage + aktueller *
                //* Wert aus dem Systemarray) und gibt sie zurueck, sonst nullptr, wenn           *
                //* laufendeNr keinen Kanal mehr bezeichnet.                                       *
                //*********************************************************************************
                //* laufendeNr: 0..ANZMESSWERTEMODUL+ANZKONFIGMODUL-1                             *
                //*********************************************************************************
                //* Return: Zeiger auf _cs2ScratchRow (nur bis zum naechsten Aufruf gueltig)      *
                //*         oder nullptr                                                          *
                //*********************************************************************************

                const uint8_t* cs2IndexExtern(uint8_t laufendeNr) override;

        private:

            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //+ Modulspezifische CSx-GUI-Parameter (ANZMESSWERTEMODUL=0, ANZKONFIGMODUL=2,        +
            //+ cs2_index_ex[]): zwei Konfigkanaele (Zeile 0 = Label "SW Major:", Zeile 1 = Label +
            //+ "SW Minor:", je Slider 1...255) - siehe cs2IndexParameterExtern(). Die            +
            //+ Kanaldefinition (initCs2Ex[], PROGMEM) bleibt komplett im Flash -                 +
            //+ cs2IndexExtern() baut die angeforderte Zeile bei jedem Aufruf frisch in           +
            //+ _cs2ScratchRow zusammen.                                                           +
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

            #define MBC80_ANZMESSWERTEMODUL 0   // Anzahl der modulspez. Messwerte
            #define MBC80_ANZKONFIGMODUL    2   // Anzahl der modulspez. Konfigurationskanaele

            uint8_t _cs2ScratchRow[MBC_COM_CS2PAKETE * 8];

            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //+ Erweiterung der CAN-Nachrichten                                                   +
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

            uint8_t _msgKonfigOk[MBC_COM_MSG_LAENGE];   // Bestaetigung "Konfigwert uebernommen"

            MbcCan       *_canMbcan;
            MbcMsgBridge *_bridge;

            // Vorgetaeuschte L88-Softwareversion: KEIN eigenes RAM-Member mehr, sondern
            // systemarray-backed (MBC_80_L88SW_1/2, siehe .cpp) - Offset muss exakt zur
            // PC_ARRAY_DATA-Kompatibilitaet zum Parametriercenter passen

            uint16_t _baseAddr;    // Basisadresse der Schaltdecoder (0x6000...0x6fff), zyklisch an die CS2 gemeldet
            uint16_t _autoCnt;
            bool     _streaming;

            void handleS88(const uint8_t msg[MBC_CAN_MSG_LEN]);
            void requestAutoKenner();
    };

//------ Ende Klassendefinition Mbc80Com ----------------------------------------------------------

#endif  /* _MBC80COM_H_ */

//###### Ende Quellcode Mbc80Com.h ################################################################
