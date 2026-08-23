//*************************************************************************************************
//* HEADER - Modul mbc-92: Magnetartikel-Schaltdecoder mit Stellpult (Mbc92Lib)                   *
//*************************************************************************************************
//* Modulspezifische MbcCom-Unterklasse fuer mbc-92: 8 Relaisausgaenge (4 Doppelspulenantriebe    *
//* fuer Magnetartikel/Weichen, gepulst mit konfigurierbarer Impulszeit und Zwangsabschaltung), 8 *
//* Stellpult-Eingaenge (lokale Taster, loesen denselben Schaltvorgang wie ein CS2-Kommando aus)  *
//* sowie 4 direkte Stellungsanzeige-LEDs (eine je Doppelspulenantrieb). mbc-92 hat KEINE         *
//* I2C-Positionsanzeige und KEINE Bestueckungserkennung ueber Hardware - stattdessen ein         *
//* EEPROM-persistiertes "Art der Aktoren"-Flag (MBC_92_MM_ART, 2 Bit je Paar, nur als 0/!=0      *
//* ausgewertet): 0 = Impulsausgang (nach Ablauf der Impulszeit werden beide Ausgaenge des Paars  *
//* wieder deaktiviert), !=0 = Wechselschalter (die zuletzt geschaltete Seite bleibt dauerhaft    *
//* aktiv, kein automatisches Zuruecksetzen). Beim Einschalten werden dafuer IMMER beide          *
//* Ausgaenge eines Paars gleichzeitig angesteuert (Zielseite RESET, Gegenseite SET) statt nur    *
//* eines einzelnen Ausgangs - siehe relaisOn()/relaisAllOff(). can (MbcCom-Basisklasse) = der    *
//* einzige CAN-Kanal dieses Moduls.                                                              *
//*************************************************************************************************
//* Datei: Mbc92Com.h                                                                             *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen:                                                                                  *
//* V1.0.0 - 14.08.2026 - Nullversion                                                             *
//*************************************************************************************************

#pragma once

//###### Start Quellcode Mbc92Com.h ###############################################################

#ifndef _MBC92COM_H_
#define _MBC92COM_H_     "V1.0.0 - 14.08.2026"

//------ Einbindung Bibliotheken ------------------------------------------------------------------

#include <Arduino.h>
#include <MbcCom.h>
#include <MbcDigitalOut.h>
#include <MbcDigitalIn.h>

//------ Start Klassendefinition Mbc92Com ---------------------------------------------------------

    //* Klasse fuer <Mbc92Com>
    //* Modulspezifische MbcCom-Unterklasse fuer mbc-92. relais/stellpult: Index 0..7 =
    //* REL_1_L,REL_1_R,REL_2_L,REL_2_R,REL_3_L,REL_3_R,REL_4_L,REL_4_R bzw.
    //* IN_1_G,IN_1_R,IN_2_G,IN_2_R,IN_3_G,IN_3_R,IN_4_G,IN_4_R.
    //* anzeige: Index 0..3 = Stellungsanzeige-LED Doppelspulenantrieb 1..4.
    //* Zusaetzlich elf CS2-Konfigkanaele melden/aendern direkt in der CSx die Decoder-Basisadresse
    //* als einfache Modulnummer (Slider, Label "MM-Addr", 0...255), die Auto-Aus-Zeit (Slider,
    //* Rohwert 0...200, Einheit x10ms) sowie die 4 Impulszeiten je Doppelspulenantrieb (je Slider,
    //* Rohwert 1...250, Einheit x10ms), die Zwangsrueckmeldung (Auswahlliste "disabled"/"enabled")
    //* sowie die Art der Aktoren je Doppelspulenantrieb (Auswahlliste "Impuls"/"Wechsel", echt
    //* editierbar), siehe cs2IndexParameterExtern().

    class Mbc92Com : public MbcCom
    {
        public:

            //*************************************************************************************
            //* Konstruktor zur Deklaration von Objektvariablen                                   *
            //*************************************************************************************
            //* utils:      zugehoerige MbcUtils-Instanz                                          *
            //* can:        zugehoeriger CAN-Kanal dieses Moduls (Basisklassen-Parameter can)     *
            //* bootStorage: zugehoerige MbcBootStorage-Instanz (Firmware-Update-Staging)         *
            //* anzeige:    4 Stellungsanzeige-LEDs, Index-Konvention siehe Klassenkommentar      *
            //* relais:     8 Relaisausgaenge, Index-Konvention siehe Klassenkommentar            *
            //* stellpult:  8 Stellpult-Eingaenge, Index-Konvention siehe Klassenkommentar        *
            //* pinReset:   NUR auf AVR (ATmega644P) vorhanden, siehe MbcCom-Konstruktor          *
            //*************************************************************************************

            #if defined(__AVR__)
                Mbc92Com(MbcUtils &utils, MbcCan &can, MbcBootStorage &bootStorage,
                         MbcDigitalOut *(&anzeige)[4], MbcDigitalOut *(&relais)[8],
                         MbcDigitalIn *(&stellpult)[8], uint8_t pinReset);
            #else
                Mbc92Com(MbcUtils &utils, MbcCan &can, MbcBootStorage &bootStorage,
                         MbcDigitalOut *(&anzeige)[4], MbcDigitalOut *(&relais)[8],
                         MbcDigitalIn *(&stellpult)[8]);
            #endif

            //*************************************************************************************
            //* Einmalig nach dem begin() der Basisklasse aufrufen: Relais in Ruhezustand         *
            //* bringen, gespeicherte Parameter aus der EEPROM-Zusatzregion laden und die         *
            //* Magnetartikel/Positionsanzeige synchronisieren.                                   *
            //*************************************************************************************

            void beginModule();

            //*************************************************************************************
            //* Je loop()-Durchlauf aufrufen: Puls-Abschaltung, Zwangsrueckmeldezyklus,           *
            //* Stellpult-Abfrage und CAN-Nachrichtenempfang/-Auswertung (ruft intern auch die    *
            //* geerbte check() auf).                                                             *
            //*************************************************************************************

            void update();

        protected:

            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //+ Die 7 Erweiterungspunkte der Basisklasse (siehe MbcCom.h fuer die volle           +
            //+ Beschreibung je Hook) - bei mbc-92 nur eepromReadParaExtern()/                    +
            //+ eepromWriteParaExtern() mit echter Logik, der Rest bleibt ungenutzt/leer          +
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

                //*********************************************************************************
                //* Laedt Impulszeiten/Aktorstellung/Basisadresse/Auto-Aus/Zwangsrueckmeldung/    *
                //* Art der Aktoren aus der EEPROM-Zusatzregion (mit Defaultwerten fuer den       *
                //* unbeschriebenen Zustand, 0xff-Sentinel).                                      *
                //*********************************************************************************

                void eepromReadParaExtern() override;

                //*********************************************************************************
                //* Schreibt den betroffenen Parameterblock in die EEPROM-Zusatzregion. Bei       *
                //* EE_92_MM_ART wird die neue Nutzungsart sofort angewandt (relaisAllOff()).     *
                //*********************************************************************************
                //* block: Identifier des zu schreibenden Blocks (0x0A....)                       *
                //*********************************************************************************

                void eepromWriteParaExtern(uint8_t block) override;

                //*********************************************************************************
                //* mbc-92 hat nichts zu loeschen. Default: nichts zu tun.                        *
                //*********************************************************************************

                void eepromEraseParaExtern() override;

                //*********************************************************************************
                //* Traegt GUiD/HASH in die Konfig-Bestaetigungsnachricht _msgKonfigOk ein - erst *
                //* durch die neuen Konfigkanaele gebraucht.                                      *
                //*********************************************************************************

                void hashInitExtern() override;

                //*********************************************************************************
                //* mbc-92 simuliert keine fremde Softwareversion. Default: nichts zu tun.        *
                //*********************************************************************************

                void swInitExtern() override;

                //*********************************************************************************
                //* mbc-92 hat keine modulspezifischen Messwerte (ANZMESSWERTEMODUL=0). Default:  *
                //* nichts zu tun.                                                                *
                //*********************************************************************************
                //* kanal: Messkanalnummer                                                        *
                //*********************************************************************************

                void cs2IndexMesswerteExtern(uint8_t kanal) override;

                //*********************************************************************************
                //* Uebernimmt Konfigkanaele von der CS2: eine neue Modulnummer (Label "MM-Addr", *
                //* 0...255) - die CSx-GUI kann keine Hex-Eingabe, daher wird nicht die           *
                //* Basisadresse selbst uebertragen, sondern eine einfache Modulnummer, die       *
                //* intern in MMADDR_BASE + 4*Modulnummer (0x3000...0x33FC) umgerechnet wird -    *
                //* eine neue Auto-Aus-Zeit (Rohwert 0...200, Einheit x10ms) oder eine der 4      *
                //* Impulszeiten je Doppelspulenantrieb (Rohwert 1...250, Einheit x10ms) - eine   *
                //* neue Zwangsrueckmeldung-Auswahl (0=disabled/1=enabled) - oder eine neue Art   *
                //* der Aktoren je Doppelspulenantrieb (0=Impuls/1=Wechsel, setzt/loescht nur das *
                //* LOW-Bit des jeweiligen 2-Bit-Paarfeldes in MBC_92_MM_ART). Ein ungueltiger    *
                //* Wert bleibt bei allen Kanaelen ohne Wirkung und wird mit FALSE quittiert.     *
                //*********************************************************************************
                //* msg: CAN-Nachricht (msg[D5]=Kanal, msg[D6..D7]=neuer Wert)                    *
                //*********************************************************************************

                void cs2IndexParameterExtern(const uint8_t msg[MBC_COM_MSG_LAENGE]) override;

            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //+ Zusaetzliche Erweiterungspunkte (kein Teil der 7 Basis-Hooks, siehe               +
            //+ Kopfkommentar von MbcCom.h)                                                       +
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

                //*********************************************************************************
                //* mbc-92 hat keine feste Kennung - sie kommt vom Parametriercenter der CS2      *
                //* (Standardverhalten).                                                          *
                //*********************************************************************************
                //* knH/knL: unveraendert                                                         *
                //*********************************************************************************
                //* Return: false (keine feste Kennung)                                           *
                //*********************************************************************************

                bool kennerFixedExtern(uint8_t &knH, uint8_t &knL) override;

                //*********************************************************************************
                //* mbc-92 hat keinen Zusatz-Versandweg. Default: nichts zu tun.                  *
                //*********************************************************************************
                //* msg: Nachrichtenarray                                                         *
                //*********************************************************************************

                void sendMessageExtern(const uint8_t msg[MBC_COM_MSG_LAENGE]) override;

                //*********************************************************************************
                //* Liefert eine Zeile aus _cs2IndexEx[] (MM-Addr-, Auto-Aus-,                    *
                //* Zwangsrueckmeldung-, eine der 4 Art-der-Aktoren- oder eine der 4              *
                //* Impulszeit-Konfigkanaele) oder nullptr, wenn laufendeNr keinen Kanal mehr     *
                //* bezeichnet.                                                                   *
                //*********************************************************************************
                //* laufendeNr: 0..ANZMESSWERTEMODUL+ANZKONFIGMODUL-1                             *
                //*********************************************************************************
                //* Return: Zeiger auf die 40-Byte-Zeile oder nullptr                             *
                //*********************************************************************************

                const uint8_t* cs2IndexExtern(uint8_t laufendeNr) override;

        private:

            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //+ Modulspezifische CSx-GUI-Parameter: elf Konfigkanaele (Zeile 0 = Label "MM-Addr", +
            //+ Modulnummer 0...255; Zeile 1 = Auto-Aus, Rohwert 0...200 x10ms; Zeile 2 =         +
            //+ Zwangsrueckmeldung, Auswahlliste disabled/enabled; Zeilen 3...10 = je             +
            //+ Doppelspulenantrieb interleaved Art der Aktoren (Auswahlliste Impuls/Wechsel)     +
            //+ gefolgt von Impulszeit (Rohwert 1...250 x10ms) - siehe cs2IndexParameterExtern()) +
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

            #define MBC92_ANZMESSWERTEMODUL 0   // Anzahl der modulspez. Messwerte
            #define MBC92_ANZKONFIGMODUL    11  // Anzahl der modulspez. Konfigurationskanaele

            uint8_t _cs2IndexEx[MBC92_ANZMESSWERTEMODUL + MBC92_ANZKONFIGMODUL][MBC_COM_CS2PAKETE * 8];

            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //+ Erweiterung der CAN-Nachrichten                                                   +
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

            uint8_t _msgKonfigOk[MBC_COM_MSG_LAENGE];   // Bestaetigung "MM-Addr uebernommen"/abgelehnt

            MbcDigitalOut *_anzeige[4];
            MbcDigitalOut *_relais[8];
            MbcDigitalIn  *_stellpult[8];

            uint16_t      _baseAddr;
            uint8_t       _relaisTime[4];
            uint8_t       _zyklusstep;
            bool          _relaisStrom;
            unsigned long _pulseStartMs;
            unsigned long _pulseMaxMs;

            void     relaisAllOff();
            void     relaisOn(uint8_t index, uint8_t timeUnits);
            void     relaisOffFinish(uint8_t zyklusstepAtStart);
            bool     pulseElapsed();
            void     stateDisplay();
            uint16_t relaisSync();
            void     switchboardCheck();
            void     handleSwitchCommand(const uint8_t msg[MBC_CAN_MSG_LEN]);
    };

//------ Ende Klassendefinition Mbc92Com ----------------------------------------------------------

#endif  /* _MBC92COM_H_ */

//###### Ende Quellcode Mbc92Com.h ################################################################
