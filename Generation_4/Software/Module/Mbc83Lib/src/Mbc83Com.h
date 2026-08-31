//*************************************************************************************************
//* HEADER - Modul mbc-83: 4-fach Relais-/Magnetartikeldecoder mit Dimmer (Mbc83Lib)              *
//*************************************************************************************************
//* Modulspezifische MbcCom-Unterklasse fuer mbc-83: 4x2 gepulste Relaisausgaenge (klassischer    *
//* Maerklin-K83/K84-Stil - Spulenstrom kurz an, dann automatisch aus) mit 4                      *
//* Rueckmelde-Eingaengen, je Ausgang konfigurierbarem Spulentiming und globalem                  *
//* Auto-Aus-Sicherheitswert, 4x2 Stellpulteingaenge und ein PWM-Dimmerausgang. can               *
//* (MbcCom-Basisklasse) = der einzige CAN-Kanal dieses Moduls. mbc-83 wertet rohe                *
//* Motorola-Zubehoer-Schaltbefehle (CAN_ID_ZUBEHOER_SCHALTEN, Adresse per D2/D3) sowie eine      *
//* Rueckmeldeanforderung eines vorgelagerten LinkS88-Gateways (Loc-ID-Subkommando MD_ZYKLUS)     *
//* direkt in update() aus, unabhaengig von der PC-/CS2-Dispatch-Logik der Basisklasse - siehe    *
//* handleSwitchCommand()/update(). 12 CS2-Konfigkanaele (Label MM-Addr, Modulnummer 0...255;     *
//* Label Auto-Aus x10ms, Rohwert 0...200; Label Meldezyklus, Auswahlliste disabled/enabled;      *
//* Label Dimmer, Rohwert 0...200; sowie je Doppelspulenantrieb interleaved Art (Auswahlliste     *
//* Impuls/Wechsel/StartUp) und Impulszeit (Rohwert 1...250 x10ms) - siehe                        *
//* cs2IndexParameterExtern()) erlauben das Setzen von Basisadresse, Auto-Aus-Zeit,               *
//* Zwangsrueckmeldung, Dimmer-Startwert sowie Art/Timing je Ausgang direkt aus der CSx-GUI - der *
//* rohe PC_ARRAY_DATA-Zugriff auf das Systemarray bleibt daneben weiterhin moeglich, siehe       *
//* Systemarray-Feldoffsets in Mbc83Com.cpp. WICHTIG: die Aktorstellung wird NIE ins EEPROM       *
//* geschrieben - sie wird bei jedem beginModule() frisch aus den 4 Rueckmelde-Eingaengen         *
//* (bistabile Relaisstellung) gelesen, siehe outputStateInit(). Je Zeitpunkt ist hoechstens EIN  *
//* Ausgang aktiv (ein einzelner Pulszustand, kein Array je Ausgang). Ein direkter Schaltbefehl   *
//* unterbricht sofort einen laufenden Puls, Stellpult/Rueckmeldezyklus wirken dagegen nur, wenn  *
//* gerade keiner laeuft - siehe Klassenkommentar in Mbc83Com.cpp.                                *
//*************************************************************************************************
//* Datei: Mbc83Com.h                                                                             *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen:                                                                                  *
//* V1.0.0 - 21.08.2026 - Nullversion                                                             *
//* V1.0.1 - 29.08.2026 - RAM-Optimierung                                                         *
//*************************************************************************************************

#pragma once

//###### Start Quellcode Mbc83Com.h ###############################################################

#ifndef _MBC83COM_H_
#define _MBC83COM_H_     "V1.0.1 - 29.08.2026"

//------ Einbindung Bibliotheken ------------------------------------------------------------------

#include <Arduino.h>
#include <MbcCom.h>
#include <MbcDigitalIn.h>
#include <MbcDigitalOut.h>

//------ Start Klassendefinition Mbc83Com ---------------------------------------------------------

    //* Klasse fuer <Mbc83Com>
    //* Modulspezifische MbcCom-Unterklasse fuer mbc-83. relayOut[0..7] sind die 8 Relaisausgaenge
    //* (Reihenfolge: Ausgang1-Rot(L),Ausgang1-Gruen(R),Ausgang2-Rot,Ausgang2-Gruen,...,
    //* Ausgang4-Gruen). relayIn[0..3] sind die 4
    //* Rueckmelde-Eingaenge (bistabile Relaisstellung, aktiv-LOW). switchboard[0..7] sind die 8
    //* Stellpulteingaenge (gleiche Rot/Gruen-Reihenfolge wie relayOut). pinDimmer ist ein nativer
    //* PWM-Pin (analogWrite(), siehe Klassenkommentar in Mbc83Com.cpp) - kann anders als die
    //* uebrige I/O NICHT auf einen MbcMcp23x17-Expander gelegt werden.

    class Mbc83Com : public MbcCom
    {
        public:

            //*************************************************************************************
            //* Konstruktor zur Deklaration von Objektvariablen                                   *
            //* utils:       zugehoerige MbcUtils-Instanz                                         *
            //* can:         zugehoeriger CAN-Kanal dieses Moduls (Basisklassen-Parameter can)    *
            //* bootStorage: zugehoerige MbcBootStorage-Instanz (Firmware-Update-Staging)         *
            //* relayOut:    8 Relaisausgaenge, siehe Klassenkommentar                            *
            //* relayIn:     4 Rueckmelde-Eingaenge, siehe Klassenkommentar                       *
            //* switchboard: 8 Stellpulteingaenge, siehe Klassenkommentar                         *
            //* pinDimmer:   nativer PWM-Pin des Dimmerausgangs                                   *
            //* pinReset:    NUR auf AVR (ATmega644) vorhanden, siehe MbcCom-Konstruktor          *
            //*************************************************************************************

            #if defined(__AVR__)
                Mbc83Com(MbcUtils &utils, MbcCan &can, MbcBootStorage &bootStorage,
                         MbcDigitalOut *(&relayOut)[8], MbcDigitalIn *(&relayIn)[4],
                         MbcDigitalIn *(&switchboard)[8], uint8_t pinDimmer, uint8_t pinReset);
            #else
                Mbc83Com(MbcUtils &utils, MbcCan &can, MbcBootStorage &bootStorage,
                         MbcDigitalOut *(&relayOut)[8], MbcDigitalIn *(&relayIn)[4],
                         MbcDigitalIn *(&switchboard)[8], uint8_t pinDimmer);
            #endif

            //*************************************************************************************
            //* Einmalig nach dem begin() der Basisklasse aufrufen:                               *
            //* Relais-/Stellpult-/Rueckmelde-Pins konfigurieren, gespeicherte Parameter aus der  *
            //* EEPROM-Zusatzregion laden, Aktorstellung aus den Rueckmelde-Eingaengen ableiten   *
            //* (siehe Klassenkommentar) und den Dimmer-Startwert anwenden.                       *
            //*************************************************************************************

            void beginModule();

            //*************************************************************************************
            //* Je loop()-Durchlauf aufrufen: faelligen Pulsabbruch, Rueckmeldezyklus, Stellpult  *
            //* und CAN-Nachrichtenempfang/-Auswertung (ruft intern auch die geerbte check()      *
            //* auf).                                                                             *
            //*************************************************************************************

            void update();

        protected:

            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //+ Erweiterungspunkte der Basisklasse (siehe MbcCom.h) - mbc-83 hat 12               +
            //+ CS2-Konfigkanaele (MM-Addr, Auto-Aus, Meldezyklus, Dimmer, sowie je               +
            //+ Doppelspulenantrieb interleaved Art/Timing), daher echte Logik in                 +
            //+ hashInitExtern()/cs2IndexParameterExtern()/cs2IndexExtern() zusaetzlich zu        +
            //+ eepromReadParaExtern()/eepromWriteParaExtern()                                    +
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

                //*********************************************************************************
                //* Laedt MM-Adresse, Art der Ausgaenge, Auto-Aus, Zwangsrueckmeldezyklus,        *
                //* Dimmerstellung und Spulentiming aus der EEPROM-Zusatzregion in das            *
                //* Systemarray (0xff-Sentinel-Defaults, siehe Mbc83Com.cpp) - NICHT die          *
                //* Aktorstellung, siehe Klassenkommentar.                                        *
                //*********************************************************************************

                void eepromReadParaExtern() override;

                //*********************************************************************************
                //* Persistiert je nach block den passenden Ausschnitt des Systemarrays in der    *
                //* EEPROM-Zusatzregion und wendet den Dimmerwert unbedingt neu an (die           *
                //* Dimmerstellung wird bei JEDER Parameteraenderung neu angewendet, unabhaengig  *
                //* vom Block).                                                                   *
                //*********************************************************************************
                //* block: Identifier des zu schreibenden Blocks                                  *
                //* (EE_83_MMAddr/_MM_TIMING/_MM_ART/_AUS/_RUECK/_DIMM)                           *
                //*********************************************************************************

                void eepromWriteParaExtern(uint8_t block) override;

                //*********************************************************************************
                //* Traegt GUiD/HASH in die Konfig-Bestaetigungsnachricht _msgKonfigOk ein - vom  *
                //* MM-Addr/Auto-Aus/Meldezyklus/Dimmer-Konfigkanal gebraucht.                    *
                //*********************************************************************************

                void hashInitExtern() override;

                //*********************************************************************************
                //* Uebernimmt Konfigkanaele von der CS2: eine neue Modulnummer (Label MM-Addr,   *
                //* 0...255) - die CSx-GUI kann keine Hex-Eingabe, daher wird nicht die           *
                //* Basisadresse selbst uebertragen, sondern eine einfache Modulnummer, die       *
                //* intern in MMADDR_BASE + 4*Modulnummer (0x3000...0x33FC) umgerechnet wird -    *
                //* eine neue Auto-Aus-Zeit (Rohwert 0...200, x10ms), eine neue Meldezyklus-      *
                //* Auswahl (0=disabled/1=enabled), ein neuer Dimmer-Startwert (Rohwert 0...200), *
                //* oder je Doppelspulenantrieb eine neue Art (Auswahlliste                       *
                //* 0=Impuls/1=Wechsel/2=StartUp, setzt/loescht die betroffenen 2 Bit des         *
                //* jeweiligen Paarfeldes) bzw. eine neue Impulszeit (Rohwert 1...250, x10ms).    *
                //* Ein ungueltiger Wert bleibt bei ALLEN Kanaelen ohne Wirkung und wird mit      *
                //* FALSE quittiert - nur der ParaCenter-Weg (PC_ARRAY_DATA) klemmt Auto-Aus      *
                //* stattdessen auf die obere Grenze.                                             *
                //*********************************************************************************
                //* msg: CAN-Nachricht (msg[D5]=Kanal, msg[D6..D7]=neuer Wert)                    *
                //*********************************************************************************

                void cs2IndexParameterExtern(const uint8_t msg[MBC_COM_MSG_LAENGE]) override;

                //*********************************************************************************
                //* Baut die angeforderte Kanalzeile (MM-Addr-, Auto-Aus-, Meldezyklus-, Dimmer-  *
                //* oder Art-/Timing-Konfigkanal) frisch in _cs2ScratchRow zusammen (PROGMEM-     *
                //* Vorlage + aktueller Wert aus dem Systemarray/_baseAddr) und gibt sie zurueck, *
                //* sonst nullptr.                                                                *
                //*********************************************************************************
                //* laufendeNr: 0..ANZMESSWERTEMODUL+ANZKONFIGMODUL-1                             *
                //*********************************************************************************
                //* Return: Zeiger auf _cs2ScratchRow (nur bis zum naechsten Aufruf gueltig)      *
                //*         oder nullptr                                                          *
                //*********************************************************************************

                const uint8_t* cs2IndexExtern(uint8_t laufendeNr) override;

        private:

            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //+ EEPROM-Zusatzregion-Byteoffsets                                                   +
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

            #define EEX_83_MMAddr 0    // 2 Byte
            #define EEX_83_ART    2    // 1 Byte
            #define EEX_83_AUS    3    // 1 Byte
            #define EEX_83_RUECK  4    // 1 Byte
            #define EEX_83_DIMM   5    // 1 Byte
            #define EEX_83_TIO    6    // 4 Byte

            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //+ Modulspezifische CSx-GUI-Parameter: 12 Konfigkanaele (Zeile 0 = Label MM-Addr,    +
            //+ Modulnummer 0...255; Zeile 1 = Label Auto-Aus x10ms, Rohwert 0...200; Zeile 2 =   +
            //+ Label Meldezyklus, Auswahlliste disabled/enabled; Zeile 3 = Label Dimmer, Rohwert +
            //+ 0...200; Zeilen 4...11 = je Doppelspulenantrieb interleaved Art (Auswahlliste     +
            //+ Impuls/Wechsel/StartUp) gefolgt von Impulszeit (Rohwert 1...250 x10ms)) - siehe   +
            //+ cs2IndexParameterExtern() in Mbc83Com.cpp). Die Kanaldefinition (initCs2Ex[],     +
            //+ PROGMEM) bleibt komplett im Flash - cs2IndexExtern() baut die angeforderte Zeile  +
            //+ bei jedem Aufruf frisch in _cs2ScratchRow zusammen statt 12 dauerhafte            +
            //+ 40-Byte-Zeilen im RAM zu halten.                                                   +
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

            #define MBC83_ANZMESSWERTEMODUL 0   // Anzahl der modulspez. Messwerte
            #define MBC83_ANZKONFIGMODUL    12  // Anzahl der modulspez. Konfigurationskanaele

            uint8_t _cs2ScratchRow[MBC_COM_CS2PAKETE * 8];

            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //+ Erweiterung der CAN-Nachrichten                                                   +
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

            uint8_t _msgKonfigOk[MBC_COM_MSG_LAENGE];   // Bestaetigung "Konfigwert uebernommen"

            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //+ Relais-/Rueckmelde-/Stellpult-Hardware, Dimmer                                    +
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

            MbcDigitalOut *_relayOut[8];
            MbcDigitalIn  *_relayIn[4];
            MbcDigitalIn  *_switchboard[8];
            uint8_t        _switchState[8];   // 1 = Taste losgelassen (Ruhezustand), 0 = gedrueckt
            uint8_t        _pinDimmer;

            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //+ Pulszustand (Aktueller Ausgang/Zustand des laufenden Ausgangspulses) - hoechstens +
            //+ EIN Ausgang zu jedem Zeitpunkt aktiv, siehe Klassenkommentar                      +
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

            bool          _pulseActive;
            uint8_t       _pulseOutput;      // 0...3
            uint8_t       _pulseState;       // 0 = Rot, 1 = Gruen
            unsigned long _pulseStartMs;
            unsigned long _pulseDurationMs;

            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //+ Rueckmeldezyklus (einfacher Schrittzaehler, direkt aus update() bedient)          +
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

            // 0 = inaktiv, 1...4 = Rueckmeldung Ausgang (Wert-1), >4 = fertig

            uint8_t _zyklusstep;

            uint16_t _baseAddr;   // MM-Basisadresse, siehe eepromReadParaExtern()

            #if defined(__AVR__)
                uint8_t _pinReset;
            #endif

            void outputStateInit();
            void outputSwitch(uint8_t coil, bool on);
            void outputOn(uint8_t number, uint8_t state, bool real);
            void outputOff();
            void applyDimmer();
            void switchboardCheck();
            void handleSwitchCommand(const uint8_t msg[MBC_COM_MSG_LAENGE]);
    };

//------ Ende Klassendefinition Mbc83Com ----------------------------------------------------------

#endif  /* _MBC83COM_H_ */

//###### Ende Quellcode Mbc83Com.h ################################################################
