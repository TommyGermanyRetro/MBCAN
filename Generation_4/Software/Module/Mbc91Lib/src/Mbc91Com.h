//*************************************************************************************************
//* HEADER - Modul mbc-91: Magnetartikel-Schaltdecoder mit Stellpult (Mbc91Lib)                   *
//*************************************************************************************************
//* Modulspezifische MbcCom-Unterklasse fuer mbc-91: 8 Relaisausgaenge (4 Doppelspulenantriebe    *
//* fuer Magnetartikel, gepulst mit konfigurierbarer Impulszeit und Zwangsabschaltung), 8         *
//* Stellpult-Eingaenge (lokale Taster, loesen denselben Schaltvorgang wie ein CS2-Kommando aus)  *
//* sowie ein Positionsanzeige-MbcMcp23x17. Die GPIOA/GPIOB-Rollen sind plattformabhaengig (siehe *
//* stateDisplay()/beginModule()): auf dem ATmega644P eigener, nicht geteilter I2C-MCP23017 mit   *
//* GPIOA=8 Stellungs-LEDs, GPIOB Bit 0...3=Bestueckungserkennung; auf RP2040/ESP32-C3 vertauscht *
//* (GPIOB=LEDs, GPIOA Bit 3...6=Bestueckung), damit sich der Baustein dort mit einem             *
//* LED-/Plug-Expander einen Chip teilen laesst, siehe Beispiel-Sketches. Relais/Stellpult sind   *
//* ueber MbcDigitalOut/MbcDigitalIn angebunden - je nach Sketch als natives GPIO oder als Bit    *
//* eines zusaetzlichen MCP23S17 konstruiert. can (MbcCom-Basisklasse) = der einzige CAN-Kanal    *
//* dieses Moduls (identisch zur CS2-/MBCAN-Anbindung anderer Zubehoermodule).                    *
//*************************************************************************************************
//* Datei: Mbc91Com.h                                                                             *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen:                                                                                  *
//* V1.0.0 - 06.08.2026 - Nullversion                                                             *
//*************************************************************************************************

#pragma once

//###### Start Quellcode Mbc91Com.h ###############################################################

#ifndef _MBC91COM_H_
#define _MBC91COM_H_     "V1.0.0 - 06.08.2026"

//------ Einbindung Bibliotheken ------------------------------------------------------------------

#include <Arduino.h>
#include <MbcCom.h>
#include <MbcMcp23x17.h>
#include <MbcDigitalOut.h>
#include <MbcDigitalIn.h>

//------ Start Klassendefinition Mbc91Com ---------------------------------------------------------

    //* Klasse fuer <Mbc91Com>
    //* Modulspezifische MbcCom-Unterklasse fuer mbc-91. relais/stellpult: Index 0..7 =
    //* REL_1_L,REL_1_R,REL_2_L,REL_2_R,REL_3_L,REL_3_R,REL_4_L,REL_4_R bzw.
    //* IN_1_G,IN_1_R,IN_2_G,IN_2_R,IN_3_G,IN_3_R,IN_4_G,IN_4_R.
    //* AVR/ATmega644P kennt ZWEI Hardware-Generationen mit eigenem Konstruktor: die neuere
    //* (Hardware ab 24.04.01, siehe oben) mit I2C-Positionsanzeige (MbcMcp23x17) UND
    //* Stellpult; die AELTERE (Hardware 16.10.02, kein I2C an Bord) mit 8 direkten
    //* GPIO-Positions-LEDs (kein Expander) und OHNE Stellpult - stattdessen liest sie einmalig
    //* beim Start 4 direkte GPIO-"Bestueckungs"-Jumper (BEST_1..4) fuer
    //* dieselbe Art-Erkennung, die die neuere Hardware ueber GPIOB des Indicator-Expanders
    //* bezieht - siehe stateDisplay()/beginModule() fuer die interne Fallunterscheidung
    //* (_indicator==nullptr => aeltere Hardware).
    //* Erweiterung: elf CS2-Konfigkanaele melden/aendern direkt in der CSx die Decoder-
    //* Basisadresse als einfache Modulnummer (Slider, Label "MM-Addr", 0...255), die Auto-Aus-Zeit
    //* (Slider, Rohwert 0...200, Einheit x10ms) und die Zwangsrueckmeldung (Auswahlliste
    //* "disabled"/"enabled"); vier weitere zeigen je Doppelspulenantrieb die Art der Aktoren an
    //* (Auswahlliste mit genau einer, per Bestueckung fest vorgegebenen Option "Impuls" oder
    //* "Wechsel", ReadOnly - siehe unten) und vier die Impulszeit (Slider, Rohwert 1...250,
    //* Einheit x10ms), siehe cs2IndexParameterExtern().

    class Mbc91Com : public MbcCom
    {
        public:

            //*************************************************************************************
            //* Konstruktor zur Deklaration von Objektvariablen                                   *
            //*************************************************************************************
            //* utils:      zugehoerige MbcUtils-Instanz                                          *
            //* can:        zugehoeriger CAN-Kanal dieses Moduls (Basisklassen-Parameter can)     *
            //* bootStorage: zugehoerige MbcBootStorage-Instanz (Firmware-Update-Staging)         *
            //* indicator:  Positionsanzeige-MbcMcp23x17 (muss vor beginModule() bereits          *
            //*             begin()t sein - ggf. derselbe Baustein wie der LED-/Plug-             *
            //*             Expander, siehe Sketch)                                               *
            //* relais:     8 Relaisausgaenge, Index-Konvention siehe Klassenkommentar            *
            //* stellpult:  8 Stellpult-Eingaenge, Index-Konvention siehe Klassenkommentar        *
            //* pinReset:   NUR auf AVR (ATmega644P) vorhanden, siehe MbcCom-Konstruktor          *
            //*************************************************************************************

            #if defined(__AVR__)
                Mbc91Com(MbcUtils &utils, MbcCan &can, MbcBootStorage &bootStorage,
                         MbcMcp23x17 &indicator, MbcDigitalOut *(&relais)[8],
                         MbcDigitalIn *(&stellpult)[8], uint8_t pinReset);

                //*********************************************************************************
                //* ZWEITER Konstruktor NUR fuer die aeltere ATmega644P-Hardware (16.10.02, siehe *
                //* Klassenkommentar) - keine I2C-Positionsanzeige, kein Stellpult. posLed/       *
                //* bestueckung sind native GPIO-Pins (kein Expander-Backend noetig).             *
                //*********************************************************************************
                //* relais:      8 Relaisausgaenge, Index-Konvention siehe Klassenkommentar       *
                //* posLed:      8 direkte Positions-LEDs, Index-Konvention wie relais            *
                //* bestueckung: 4 direkte Bestueckungs-Jumper (Index 0..3 = Doppelspulenantrieb  *
                //*              1..4), read()==true muss "Wechsel" bedeuten (siehe               *
                //*              beginModule()) - MbcDigitalIn(pin, activeLow=false,              *
                //*              pullup=true)                                                     *
                //* pinReset:    siehe MbcCom-Konstruktor                                         *
                //*********************************************************************************

                Mbc91Com(MbcUtils &utils, MbcCan &can, MbcBootStorage &bootStorage,
                         MbcDigitalOut *(&relais)[8], MbcDigitalOut *(&posLed)[8],
                         MbcDigitalIn *(&bestueckung)[4], uint8_t pinReset);
            #else
                Mbc91Com(MbcUtils &utils, MbcCan &can, MbcBootStorage &bootStorage,
                         MbcMcp23x17 &indicator, MbcDigitalOut *(&relais)[8],
                         MbcDigitalIn *(&stellpult)[8]);
            #endif

            //*************************************************************************************
            //* Einmalig nach dem begin() der Basisklasse aufrufen: Relais in Ruhezustand         *
            //* bringen, Bestueckung vom Positionsanzeige-Expander einlesen, gespeicherte         *
            //* Parameter aus der EEPROM-Zusatzregion laden und die                               *
            //* Magnetartikel/Positionsanzeige synchronisieren.                                   *
            //*************************************************************************************

            void beginModule();

            //*************************************************************************************
            //* Je loop()-Durchlauf aufrufen: Puls-Abschaltung, Zwangsrueckmeldezyklus,           *
            //* Stellpult-Abfrage und CAN-Nachrichtenempfang/-Auswertung - ruft intern auch die   *
            //* geerbte check() auf.                                                              *
            //*************************************************************************************

            void update();

        protected:

            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //+ Die 7 Erweiterungspunkte der Basisklasse (siehe MbcCom.h fuer die volle           +
            //+ Beschreibung je Hook) - bei mbc-91 nur eepromReadParaExtern()/                    +
            //+ eepromWriteParaExtern() mit echter Logik, der Rest bleibt leer                    +
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

                //*********************************************************************************
                //* Laedt Impulszeiten/Aktorstellung/Basisadresse/Auto-Aus/Zwangsrueckmeldung aus *
                //* der EEPROM-Zusatzregion (mit Defaultwerten fuer den unbeschriebenen Zustand,  *
                //* 0xff-Sentinel).                                                               *
                //*********************************************************************************

                void eepromReadParaExtern() override;

                //*********************************************************************************
                //* Schreibt den betroffenen Parameterblock in die EEPROM-Zusatzregion und        *
                //* uebernimmt bei Bedarf die RAM-Arbeitswerte (Impulszeiten/Basisadresse) neu.   *
                //*********************************************************************************
                //* block: Identifier des zu schreibenden Blocks (0x0A....)                       *
                //*********************************************************************************

                void eepromWriteParaExtern(uint8_t block) override;

                //*********************************************************************************
                //* mbc-91 hat nichts zu loeschen. Default: nichts zu tun.                        *
                //*********************************************************************************

                void eepromEraseParaExtern() override;

                //*********************************************************************************
                //* Traegt GUiD/HASH in die Konfig-Bestaetigungsnachricht _msgKonfigOk ein - erst *
                //* durch den neuen MM-Addr-Konfigkanal gebraucht.                                *
                //*********************************************************************************

                void hashInitExtern() override;

                //*********************************************************************************
                //* mbc-91 simuliert keine fremde Softwareversion. Default: nichts zu tun.        *
                //*********************************************************************************

                void swInitExtern() override;

                //*********************************************************************************
                //* mbc-91 hat keine modulspezifischen Messwerte (ANZMESSWERTEMODUL=0). Default:  *
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
                //* Impulszeiten je Doppelspulenantrieb (Rohwert 1...250, Einheit x10ms) - oder   *
                //* eine neue Zwangsrueckmeldung-Auswahl (0=disabled/1=enabled). Ein ungueltiger  *
                //* Wert bleibt bei diesen Kanaelen ohne Wirkung und wird mit FALSE quittiert.    *
                //* Art/Timing liegen interleaved je Doppelspulenantrieb (Art Sp1, Timing Sp1,    *
                //* Art Sp2, ...); die 4 "Art der Aktoren"-Kanaele werden bewusst NICHT           *
                //* verarbeitet - ReadOnly, mbc-91 kennt selbst keinen Wechselschalter-Betrieb,   *
                //* siehe Klassenkommentar.                                                       *
                //*********************************************************************************
                //* msg: CAN-Nachricht (msg[D5]=Kanal, msg[D6..D7]=neuer Wert)                    *
                //*********************************************************************************

                void cs2IndexParameterExtern(const uint8_t msg[MBC_COM_MSG_LAENGE]) override;

            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //+ Zusaetzliche Erweiterungspunkte (siehe Kopfkommentar von MbcCom.h)                +
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

                //*********************************************************************************
                //* mbc-91 hat keine feste Kennung - sie kommt vom Parametriercenter der CS2      *
                //* (Standardverhalten).                                                          *
                //*********************************************************************************
                //* knH/knL: unveraendert                                                         *
                //*********************************************************************************
                //* Return: false (keine feste Kennung)                                           *
                //*********************************************************************************

                bool kennerFixedExtern(uint8_t &knH, uint8_t &knL) override;

                //*********************************************************************************
                //* mbc-91 hat keinen Zusatz-Versandweg. Default: nichts zu tun.                  *
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
            //+ Doppelspulenantrieb interleaved Art der Aktoren (Auswahlliste mit genau 1 Punkt   +
            //+ entsprechend der per Bestueckung erkannten Art, ReadOnly, siehe artRowFix())      +
            //+ gefolgt von Impulszeit (Rohwert 1...250 x10ms) - siehe cs2IndexParameterExtern()) +
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

            #define MBC91_ANZMESSWERTEMODUL 0   // Anzahl der modulspez. Messwerte
            #define MBC91_ANZKONFIGMODUL    11  // Anzahl der modulspez. Konfigurationskanaele

            uint8_t _cs2IndexEx[MBC91_ANZMESSWERTEMODUL + MBC91_ANZKONFIGMODUL][MBC_COM_CS2PAKETE * 8];

            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //+ Erweiterung der CAN-Nachrichten                                                   +
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

            uint8_t _msgKonfigOk[MBC_COM_MSG_LAENGE];   // Bestaetigung "MM-Addr uebernommen"/abgelehnt

            MbcMcp23x17   *_indicator;        // nullptr => aeltere Hardware, siehe _posLed/_bestueckung
            MbcDigitalOut *_relais[8];
            MbcDigitalIn  *_stellpult[8];      // alle nullptr, wenn _hasStellpult false ist

            MbcDigitalOut *_posLed[8];         // nur belegt, wenn _indicator == nullptr
            MbcDigitalIn  *_bestueckung[4];    // nur belegt, wenn _indicator == nullptr
            bool          _hasStellpult;

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
            void     artRowFix(uint8_t row, bool wechsel);
    };

//------ Ende Klassendefinition Mbc91Com ----------------------------------------------------------

#endif  /* _MBC91COM_H_ */

//###### Ende Quellcode Mbc91Com.h ################################################################
