//*************************************************************************************************
//* HEADER - PC-/CS2-Kommunikations-Zustandsautomat (MBCANLib)                                    *
//*************************************************************************************************
//* Haelt die CAN-Nachrichtentabelle (can_msg[]) und die CS2-Konfigurationstabelle (cs2_index[])  *
//* und implementiert den PC-/CS2-Anmelde- und Datentransfer-Zustandsautomaten. Die               *
//* modulspezifischen Erweiterungspunkte sind als protected virtual Methoden umgesetzt, die eine  *
//* konkrete Modul-Unterklasse ueberschreibt. Ohne Ueberschreibung verhaelt sich Mbc89ComBase wie *
//* ein generisches Zubehoermodul ohne modulspezifische Erweiterungen (die feste                  *
//* CS2-Geraetekennung einzelner Modultypen ist dabei in den einen Hook kennerFixedExtern()       *
//* zusammengefasst, siehe Mbc89ComBase.cpp).                                                     *
//*************************************************************************************************
//* Datei: Mbc89ComBase.h                                                                         *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen:                                                                                  *
//* V1.0.0 - 02.08.2026 - Nullversion                                                             *
//*************************************************************************************************

#pragma once

//###### Start Quellcode Mbc89ComBase.h ###########################################################

#ifndef _MBC89COMBASE_H_
#define _MBC89COMBASE_H_     "V1.0.0 - 02.08.2026"

//------ Einbindung Bibliotheken ------------------------------------------------------------------

#include <Arduino.h>
#include "Mbc89Utils.h"
#include "Mbc89Can.h"
#include "Mbc89BootStorage.h"

//------ Start globale Deklarationen --------------------------------------------------------------

    // Sonderindizes fuer PC_ARRAY_DATA

    #define MBC_COM_EEPROM   0xfffd
    #define MBC_COM_READ     0xfffe
    #define MBC_COM_WRITE    0xffff

    // Vordefinierte CAN-Nachrichtentemplates (Index in can_msg[])

    #define MBC_COM_MSG_UV                 0x00
    #define MBC_COM_MSG_INFO                0x01
    #define MBC_COM_MSG_ACK_MODUL           0x02
    #define MBC_COM_MSG_ACK_IOREQUEST       0x03
    #define MBC_COM_MSG_ACK_IOACT           0x04
    #define MBC_COM_MSG_ACK_UPREQUEST       0x05
    #define MBC_COM_MSG_ACK_UPACT           0x06
    #define MBC_COM_MSG_S88                 0x07
    #define MBC_COM_MSG_ZYKLUS              0x08
    #define MBC_COM_MSG_CS2_0x00_5_0x0C     0x09
    #define MBC_COM_MSG_CS2_0x00_R_7_0x0C   0x0A
    #define MBC_COM_MSG_CS2_0x00_R_8_0x0B   0x0B
    #define MBC_COM_MSG_CS2_0x00_R_7_0x0B   0x0C
    #define MBC_COM_MSG_CS2_0x16            0x0D
    #define MBC_COM_MSG_CS2_0x22_R          0x0E
    #define MBC_COM_MSG_CS2_0x30_R          0x0F
    #define MBC_COM_MSG_CS2_0x3A_R_8        0x10
    #define MBC_COM_MSG_CS2_0x3A_R_6        0x11

    #define MBC_COM_MSG_ANZAHL   0x12
    #define MBC_COM_MSG_LAENGE   13

    // Basis-Kanalzahlen ohne modulspezifische Erweiterung (fuer Erweiterungen durch
    // Unterklassen siehe Klassenkommentar)

    #define MBC_COM_ANZMESSWERTE   2
    #define MBC_COM_ANZKONFIG      4
    #define MBC_COM_CS2PAKETE      5

//------ Ende globale Deklarationen ---------------------------------------------------------------

//------ Start Klassendefinition Mbc89ComBase -----------------------------------------------------

    //* Klasse fuer <Mbc89ComBase>
    //* PC-/CS2-Protokoll-Zustandsautomat eines MBCAN-Zubehoermoduls. Haengt an einer
    //* Mbc89Utils-Instanz (Systemarray, EEPROM, LED-Anzeige), einer Mbc89Can-Instanz (CAN-Bus) und
    //* einer Mbc89BootStorage-Instanz (externer Boot-Speicherchip fuer PC_BOOT/PC_UPGRADE_DATA).
    //* Eine Modul-Unterklasse erweitert das Verhalten ueber die protected virtual
    //* Erweiterungspunkte eepromReadParaExtern()/eepromWriteParaExtern()/eepromEraseParaExtern()/
    //* hashInitExtern()/swInitExtern()/cs2IndexMesswerteExtern()/cs2IndexParameterExtern()/
    //* kennerFixedExtern()/sendMessageExtern()/cs2IndexExtern() (feste CS2-Kennung, zusaetzlicher
    //* Versandweg, modulspezifische CS2-Konfigurationskanaele).

    class Mbc89ComBase
    {
        public:

            //*************************************************************************************
            //* Konstruktor zur Deklaration von Objektvariablen                                   *
            //*************************************************************************************
            //* utils:            zugehoerige Mbc89Utils-Instanz                                  *
            //* can:              zugehoerige Mbc89Can-Instanz (PC-/CS2-CAN-Kanal dieses Moduls)  *
            //* bootStorage:      zugehoerige Mbc89BootStorage-Instanz (Firmware-Update-Staging)  *
            //* pinReset:         NUR auf AVR (ATmega644P) vorhanden - Pin, ueber den sich das    *
            //*                   Modul per Watchdog-Trick selbst neu startet (PC_RESET/PC_BOOT). *
            //*                   Auf RP2040/ESP32-C3 entfaellt dieser Parameter, dort wird       *
            //*                   stattdessen der jeweilige Software-Reset                        *
            //*                   des Cores genutzt (siehe triggerReset() in Mbc89ComBase.cpp).   *
            //* anzMesswerteModul: Anzahl modulspezifischer Messwertkanaele (ANZMESSWERTEMODUL    *
            //*                   der Unterklasse, Default 0) - wird der CSx in INDEX_0 als       *
            //*                   Gesamtanzahl (Basis + Modul) mitgeteilt, damit sie die          *
            //*                   modulspezifischen Kanaele ueberhaupt abfragt (siehe             *
            //*                   cs2IndexExtern()).                                              *
            //* anzKonfigModul:   Anzahl modulspezifischer Konfigurationskanaele                  *
            //*                   (ANZKONFIGMODUL der Unterklasse, Default 0), analog             *
            //*                   anzMesswerteModul.                                              *
            //*************************************************************************************

            #if defined(__AVR__)
                Mbc89ComBase(Mbc89Utils &utils, Mbc89Can &can, Mbc89BootStorage &bootStorage, uint8_t pinReset,
                       uint8_t anzMesswerteModul = 0, uint8_t anzKonfigModul = 0);
            #else
                Mbc89ComBase(Mbc89Utils &utils, Mbc89Can &can, Mbc89BootStorage &bootStorage,
                       uint8_t anzMesswerteModul = 0, uint8_t anzKonfigModul = 0);
            #endif

            virtual ~Mbc89ComBase() {}

            //*************************************************************************************
            //* Kommunikation vorbereiten (Flags initialisieren, HASH-Werte berechnen, wenn das   *
            //* Modul bereits eine gueltige GUiD besitzt). Nach Mbc89Utils::begin() rufen.        *
            //*************************************************************************************

            void begin();

            //*************************************************************************************
            //* Empfangene CAN-Nachricht auswerten und ggf. beantworten                           *
            //*************************************************************************************
            //* msg: MBC_COM_MSG_LAENGE Byte (siehe Mbc89Can MBC_CAN_*-Indexkonstanten)           *
            //*************************************************************************************

            void check(const uint8_t msg[MBC_COM_MSG_LAENGE]);

            //*************************************************************************************
            //* JEDEN loop()-Durchlauf aufrufen, UNABHAENGIG davon, ob eine Nachricht empfangen   *
            //* wurde - bedient nicht-blockierend zeitverzoegerte Aktionen (aktuell: die gestreute*
            //* Ping-Antwort, siehe checkCs2Connection()/CAN_ID_PING). Ohne regelmaessige         *
            //* poll()-Aufrufe wuerde eine gestreute Ping-Antwort verspaetet oder gar nicht       *
            //* versendet.                                                                        *
            //*************************************************************************************

            void poll();

        protected:

            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //+ Erweiterungspunkte fuer Modul-Unterklassen, siehe Klassenkommentar oben           +
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

                //* Modulspezifische Parameterdaten aus dem EEPROM lesen. Wird NICHT von
                //* Mbc89ComBase selbst aufgerufen, sondern muss von der Unterklasse an passender
                //* Stelle aufgerufen werden (z.B. beginModule()). Default: nichts zu tun.

                virtual void eepromReadParaExtern() {}

                //* Zusaetzliches EEPROM-Schreiben fuer modulspezifische Bloecke (Blocknummer
                //* >= 10). Default: nichts zu tun.

                virtual void eepromWriteParaExtern(uint8_t block) { (void) block; }

                //* Modulspezifische Parameterdaten im EEPROM loeschen. Default: nichts zu tun.

                virtual void eepromEraseParaExtern() {}

                //* Zusaetzliche modulspezifische can_msg[]/cs2_index[]-Eintraege nachziehen,
                //* nachdem der HASH neu berechnet wurde. Default: nichts zu tun.

                virtual void hashInitExtern() {}

                //* Softwareversion fuer CS2-Ping-Antwort ueberschreiben (D4/D5 von
                //* can_msg[MBC_COM_MSG_CS2_0x30_R]). Default: unveraendert (Standard-SW-Byte).

                virtual void swInitExtern() {}

                //* Modulspezifischen Messwert an die CS2 senden. Default: nichts zu tun.

                virtual void cs2IndexMesswerteExtern(uint8_t kanal) { (void) kanal; }

                //* Modulspezifischen Parameterwert von der CS2 uebernehmen (msg[D4]=Kanal,
                //* msg[D6..D7]=Wert). Default: nichts zu tun.

                virtual void cs2IndexParameterExtern(const uint8_t msg[MBC_COM_MSG_LAENGE]) { (void) msg; }

            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //+ Weitere Erweiterungspunkte, siehe Klassenkommentar oben                           +
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

                //* Feste CS2-Geraetekennung dieses Modultyps liefern.
                //* Return: true und knH/knL gesetzt, wenn der Modultyp eine feste Kennung
                //* vorschreibt; false, wenn die Kennung vom Parametriercenter der CS2
                //* uebernommen werden soll (Standardverhalten generischer Module).

                virtual bool kennerFixedExtern(uint8_t &knH, uint8_t &knL)
                {
                    (void) knH; (void) knL;
                    return false;
                }

                //* Zusaetzlicher Versandweg fuer ausgehende Nachrichten (z.B. eine RS232-/
                //* Bluetooth-Bruecke). Wird IMMER zusaetzlich zum CAN-Versand aufgerufen.
                //* Default: nichts zu tun.

                virtual void sendMessageExtern(const uint8_t msg[MBC_COM_MSG_LAENGE]) { (void) msg; }

                //* Zeile aus dem modulspezifischen cs2_index_ex[]-Array der Unterklasse liefern
                //* (40
                //* Byte, CS2PAKETE*8) oder nullptr, wenn laufendeNr keinen Kanal mehr bezeichnet.
                //* laufendeNr zaehlt durchgehend 0..ANZMESSWERTEMODUL+ANZKONFIGMODUL-1 (erst die
                //* Messwert-, dann die Konfigkanaele der Unterklasse) - Mbc89ComBase sortiert die
                //* Messwertkanaele davon (laufendeNr 0..ANZMESSWERTEMODUL-1) direkt hinter die
                //* Basis-Messwerte ein und verschiebt die nachfolgenden Basis-Konfigkanaele
                //* entsprechend; die restlichen laufendeNr (Konfigkanaele der Unterklasse)
                //* haengen ganz am Ende an - siehe checkCs2Connection() in Mbc89ComBase.cpp.
                //* Default: nullptr (keine Erweiterung).

                virtual const uint8_t* cs2IndexExtern(uint8_t laufendeNr) { (void) laufendeNr; return nullptr; }

            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //+ Fuer Unterklassen zugaengliche Zustandsdaten                                      +
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

                Mbc89Utils       *_utils;
                Mbc89Can         *_can;
                Mbc89BootStorage *_bootStorage;

                uint8_t can_msg[MBC_COM_MSG_ANZAHL][MBC_COM_MSG_LAENGE];
                uint8_t cs2_index[MBC_COM_ANZMESSWERTE + MBC_COM_ANZKONFIG + 1][MBC_COM_CS2PAKETE * 8];

                bool sendMessage(const uint8_t msg[MBC_COM_MSG_LAENGE]);
                void hashInit();

                //* Loest einen sofortigen Neustart des Moduls aus (kehrt nicht zurueck) - fuer
                //* Unterklassen zugaenglich, damit z.B. ein modulspezifischer CS2-Konfigkanal
                //* nach einer echten Wertaenderung gezielt einen Neustart ausloesen kann (siehe
                //* Mbc89Com::cs2IndexParameterExtern()). NUR bei einer tatsaechlichen Aenderung
                //* aufrufen, NIE ungefragt bei jedem erfolgreichen Schreibzugriff - die CSx sendet
                //* Konfigwerte auch als Echo/Polling erneut, nicht nur bei einer echten Aenderung
                //* durch den Nutzer (hardwarebestaetigter Dauerreset-Bug bei der ersten,
                //* ungefilterten Version dieses Feature).

                void triggerReset();

            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //+ Fuer Unterklassen lesbarer Verbindungsstatus (z.B. fuer eigene, modulspezifische  +
            //+ Zyklen wie eine Automatik-Kenner-Anfrage)                                         +
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

                //* Modulidentitaet (Name/GUiD/Kennung) ist initialisiert.

                bool initDone() const { return _init.init; }

                //* Verbindung zur CS2 (PC-/CS2-Anmeldung) besteht.

                bool csConnected() const { return _cs.connect; }

                //* Automatik-Kenner wurde bereits von der CS2 vergeben.

                bool csAutoMode() const { return _cs.autoMode; }
                void setCsAutoMode(bool on) { _cs.autoMode = on; }

                //* Ob aktuell eine echte CS/MS2 ueber den CS2-Bus verbunden ist (im Unterschied
                //* zu einem PC/Software-Client, der nur ueber WLAN/PC-Schnittstelle verbunden
                //* ist) - relevant, um einen versehentlichen Not-Stopp zu vermeiden.

                bool csMbcan() const { return _cs.csMbcan; }
                void setCsMbcan(bool on) { _cs.csMbcan = on; }

        private:

            #if defined(__AVR__)
                uint8_t _pinReset;
            #endif

            // Anzahl modulspezifischer Messwertkanaele (Konstruktor-Parameter, siehe oben) -
            // wird zur Laufzeit gebraucht, um in checkCs2Connection() modulspezifische
            // Messwertkanaele DIREKT HINTER die Basis-Messwerte (Versorgungsspannung/Temperatur)
            // einzuschieben und die nachfolgenden Basis-Konfigkanaele (Seriennummer/GUiD/
            // Firmware/Hardware) entsprechend zu verschieben - unabhaengig davon, wie viele
            // modulspezifische Messwertkanaele eine Unterklasse deklariert (0, 1, 2 oder mehr)

            uint8_t _anzMesswerteModul;

            struct { bool transfer, write, read; }                             _io;
            struct { bool init, newPhase; uint8_t step; uint16_t sysindex; bool startup; } _init;
            struct { bool dbH, dbM, dbL, dbValid; uint8_t db[12]; }              _db;
            struct { bool init, connect, csMbcan, autoMode, startup, s88Startup, autoRec; uint16_t autoCnt; } _cs;

            // Nicht-blockierender Aufschub der gestreuten Ping-Antwort (siehe poll()) - dueUs ist
            // ein micros()-Zielzeitpunkt, dessen ueberlaufsicherer Vergleich in poll() per
            // vorzeichenbehafteter Subtraktion erfolgt (Standard-Arduino-Idiom).

            struct { bool pending; unsigned long dueUs; } _ping;

            void moduleFlagsReset();
            void moduleBlank();
            void guidInit();

            void checkPcConnection(const uint8_t msg[MBC_COM_MSG_LAENGE], bool isPcGuid);
            void checkCs2Connection(const uint8_t msg[MBC_COM_MSG_LAENGE], bool isCs2Guid);
    };

//------ Ende Klassendefinition Mbc89ComBase ------------------------------------------------------

#endif  /* _MBC89COMBASE_H_ */

//###### Ende Quellcode Mbc89ComBase.h ############################################################
