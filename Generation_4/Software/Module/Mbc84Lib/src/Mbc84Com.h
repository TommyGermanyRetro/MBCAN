//*************************************************************************************************
//* HEADER - Modul mbc-84: 4-fach Servo-/Weichendecoder mit Stellpult (Mbc84Lib)                  *
//*************************************************************************************************
//* Modulspezifische MbcCom-Unterklasse fuer mbc-84: 4 Servoausgaenge (Rot/Gruen-Endlagen,        *
//* optional stufenweise Bewegung entlang eines programmierbaren 60-Schritt-Servowegs je          *
//* Richtung), 4x2 Stellpulteingaenge (Rot/Gruen je Servo) und 8 Positions-LEDs. can (MbcCom-     *
//* Basisklasse) = der einzige CAN-Kanal dieses Moduls. mbc-84 wertet rohe Motorola-Zubehoer-     *
//* Schaltbefehle (CAN_ID_ZUBEHOER_SCHALTEN, Adresse per D2/D3) sowie eine Rueckmeldeanforderung  *
//* eines vorgelagerten LinkS88-Gateways (Loc-ID-Subkommando MD_ZYKLUS) direkt in update() aus,   *
//* unabhaengig von der PC-/CS2-Basisklassen-Dispatch-Logik in check() - siehe                    *
//* handleSwitchCommand()/update(). 14 CS2-Konfigkanaele (Label "MM-Addr", Modulnummer 0...255;   *
//* Label "Meldezyklus", Auswahlliste disabled/enabled; je 3 Kanaele pro Servo - Label "SvN RT    *
//* max"/"SvN GN max", je 0...30, Label "SvN Delay", 0...10, N=1...4) erlauben das Setzen der     *
//* Basisadresse, der Zwangsrueckmeldung bzw. der Endanschlaege/Verzoegerung je Servo direkt aus  *
//* der CSx-GUI, siehe cs2IndexParameterExtern(); weitere Konfigkanaele koennen nach demselben    *
//* Muster ergaenzt werden. Bewegungsgeschwindigkeit: Verzoegerung 0 ist NICHT sofort, sondern    *
//* die Basisgeschwindigkeit - eine volle Rot-Gruen-Bewegung ueber den maximal moeglichen         *
//* Stellbereich dauert dabei ca. 1 s, eine echte Verzoegerung (1...10) verlangsamt das um das    *
//* (Verzoegerung+1)-fache. Ist der tatsaechlich konfigurierte Stellbereich (maxGruen-maxRot)     *
//* kleiner, wird automatisch frueher geklemmt - die Winkelgeschwindigkeit bleibt konstant, nur   *
//* die Zeit bis zur Endlage verkuerzt sich proportional. Damit die Bewegung auch bei hoher       *
//* Verzoegerung nicht ruckelt, sendet servoStep() unabhaengig von vz alle INTERP_STEP_MS (~8 ms, *
//* bewusst unabhaengig von SERVO_MAX_STEPS/BASE_STEP_MS) ein Update an den Servo - linear        *
//* interpoliert zwischen dem aktuellen und dem naechsten der 61 Tabellenschritte (rawAtStep()),  *
//* waehrend der Tabellenschritt selbst weiterhin erst nach dem vollen, vz-gestreckten Intervall  *
//* wechselt. Siehe BASE_STEP_MS/INTERP_STEP_MS/rawAtStep()/writeServoRaw() in Mbc84Com.cpp.      *
//* Dabei faehrt zu jedem Zeitpunkt hoechstens EIN Servo aktiv (sequenziell, kein gleichzeitiges  *
//* Fahren mehrerer Servos) - haelt Stromspitzen am Spannungsregler klein, siehe                  *
//* servoStep()/servoNeedsStep(). Der 480 Byte grosse Servoweg-Tabellenspeicher (fuer eine frei   *
//* programmierbare Kurvenform, siehe Systemarray-Layout unten) steht auf allen 4 Zielprozessoren *
//* gleichermassen zur Verfuegung, auch auf AVR - jeder Sketch legt dafuer einen exakt bemessenen *
//* Puffer an (genPurpSize=497, siehe MBC_SYSARRAY_SIZE() in MbcUtils.h sowie den                 *
//* Konstruktoraufruf in den Beispiel-Sketches), real am ATmega644P-Sketch dieses Moduls gemessen *
//* (siehe README.md).                                                                            *
//*************************************************************************************************
//* Datei: Mbc84Com.h                                                                             *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen:                                                                                  *
//* V1.0.0 - 18.08.2026 - Nullversion                                                             *
//* V1.0.1 - 29.08.2026 - RAM-Optimierung                                                         *
//*************************************************************************************************

#pragma once

//###### Start Quellcode Mbc84Com.h ###############################################################

#ifndef _MBC84COM_H_
#define _MBC84COM_H_     "V1.0.1 - 29.08.2026"

//------ Einbindung Bibliotheken ------------------------------------------------------------------

#include <Arduino.h>
#if defined(ESP32)
    #include <ESP32Servo.h>   // Servo.h ist auf ESP32 nicht verfuegbar - ESP32Servo.h stellt
                              // dieselbe Servo-Klasse/API (attach/write/writeMicroseconds/read)
                              // ueber die LEDC-Peripherie bereit, separat zu installieren
#elif !defined(__AVR__)
    #include <Servo.h>
#endif
#include <MbcCom.h>
#include <MbcDigitalIn.h>
#include <MbcDigitalOut.h>

//------ Start Klassendefinition Mbc84Com ---------------------------------------------------------

    //* Klasse fuer <Mbc84Com>
    //* Modulspezifische MbcCom-Unterklasse fuer mbc-84. servoPins[0..3] sind die 4 nativen Servo-
    //* Signalpins - auf AVR (ATmega644P) ueber eine rohe Timer1-CTC-ISR angesteuert (16-Bit statt
    //* 8-Bit, fuer eine feinere Pulsbreitenaufloesung bei hohem VZ - siehe
    //* raw2fine()/writeServoRawFine()); die gespeicherte 480-Byte-Servoweg-
    //* Tabelle bleibt dabei bewusst grob (8-Bit), nur der Live-Interpolations-/Ausgabepfad rechnet
    //* fein. Dafuer MUESSEN alle 4 servoPins auf demselben AVR-Port liegen (ein gemeinsames PORTx-
    //* Register wird pro Puls getoggelt, siehe beginModule()/writeServoRawFine()). Auf
    //* RP2040/ESP32-C3 weiterhin ueber die Arduino-Standardbibliothek Servo (kein
    //* Timer1-Aequivalent dort noetig, schreibt aber denselben feinen raw2fine()-Wert).
    //* switchboard[0..7] sind die 8 Stellpulteingaenge (Reihenfolge: Servo0-Rot, Servo0-Gruen,
    //* Servo1-Rot, Servo1-Gruen, Servo2-Rot, Servo2-Gruen, Servo3-Rot, Servo3-Gruen). led[0..7]
    //* sind die 8 Positions-LEDs (gleiche Reihenfolge).

    class Mbc84Com : public MbcCom
    {
        public:

            //*************************************************************************************
            //* Konstruktor zur Deklaration von Objektvariablen                                   *
            //* utils:       zugehoerige MbcUtils-Instanz                                         *
            //* can:         zugehoeriger CAN-Kanal dieses Moduls (Basisklassen-Parameter can)    *
            //* bootStorage: zugehoerige MbcBootStorage-Instanz (Firmware-Update-Staging)         *
            //* servoPins:   4 native Digitalpins der Servosignale                                *
            //* switchboard: 8 Stellpulteingaenge (Rot/Gruen je Servo, siehe Klassenkommentar)    *
            //* led:         8 Positions-LEDs (Rot/Gruen je Servo, siehe Klassenkommentar)        *
            //* pinReset:    NUR auf AVR (ATmega644P) vorhanden, siehe MbcCom-Konstruktor         *
            //*************************************************************************************

            #if defined(__AVR__)
                Mbc84Com(MbcUtils &utils, MbcCan &can, MbcBootStorage &bootStorage,
                         const uint8_t servoPins[4], MbcDigitalIn *(&switchboard)[8],
                         MbcDigitalOut *(&led)[8], uint8_t pinReset);
            #else
                Mbc84Com(MbcUtils &utils, MbcCan &can, MbcBootStorage &bootStorage,
                         const uint8_t servoPins[4], MbcDigitalIn *(&switchboard)[8],
                         MbcDigitalOut *(&led)[8]);
            #endif

            //*************************************************************************************
            //* Einmalig nach dem begin() der Basisklasse aufrufen: Stellpult-/LED-Pins           *
            //* konfigurieren, gespeicherte Parameter aus der EEPROM-Zusatzregion laden, Servos   *
            //* an ihre Pins binden und in die gespeicherte Stellung fahren.                      *
            //*************************************************************************************

            void beginModule();

            //*************************************************************************************
            //* Je loop()-Durchlauf aufrufen: Stellpult auswerten, Servobewegung nachfuehren,     *
            //* faellige Rueckmeldungen versenden und CAN-Nachrichtenempfang/-Auswertung (ruft    *
            //* intern auch die geerbte check() auf).                                             *
            //*************************************************************************************

            void update();

        protected:

            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //+ Erweiterungspunkte der Basisklasse (siehe MbcCom.h) - mbc-84 hat einen            +
            //+ CS2-Konfigkanal (MM-Addr, siehe                                                   +
            //+ hashInitExtern()/cs2IndexExtern()/cs2IndexParameterExtern()), daneben bleibt der  +
            //+ rohe PC_ARRAY_DATA-Zugriff auf das Systemarray (siehe Klassenkommentar) weiterhin +
            //+ moeglich; die uebrigen CS2-bezogenen Erweiterungspunkte bleiben unveraendert auf  +
            //+ dem Default der Basisklasse                                                       +
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

                //*********************************************************************************
                //* Laedt MM-Adresse, Aktorstellung, Servo-Endlagen/-Verzoegerung, Servoweg und   *
                //* Zwangsrueckmeldezyklus aus der EEPROM-Zusatzregion in das Systemarray.        *
                //*********************************************************************************

                void eepromReadParaExtern() override;

                //*********************************************************************************
                //* Persistiert je nach block den passenden Ausschnitt des Systemarrays in der    *
                //* EEPROM-Zusatzregion. Bei block==EE_84_MMAddr wird zusaetzlich sofort ein      *
                //* Rueckmeldezyklus ausgeloest, wenn der Meldezyklus aktuell aktiviert ist; bei  *
                //* block==EE_84_RUECK genau dann, wenn der Meldezyklus dabei von deaktiviert auf *
                //* aktiviert wechselt (siehe Mbc84Com.cpp) - unabhaengig davon, ob die Aenderung *
                //* ueber CS2 oder ParaCenter kam, da beide Wege hier zusammenlaufen.             *
                //*********************************************************************************
                //* block: Identifier des zu schreibenden Blocks                                  *
                //* (EE_84_MMAddr/_AKTOR/_SERVOSMAX/_SERVOSWEG/_RUECK)                            *
                //*********************************************************************************

                void eepromWriteParaExtern(uint8_t block) override;

                //*********************************************************************************
                //* Traegt GUiD/HASH in die Konfig-Bestaetigungsnachricht _msgKonfigOk ein - vom  *
                //* MM-Addr-Konfigkanal gebraucht.                                                *
                //*********************************************************************************

                void hashInitExtern() override;

                //*********************************************************************************
                //* Uebernimmt Konfigkanaele von der CS2: eine neue Modulnummer (Label "MM-Addr", *
                //* 0...255) - die CSx-GUI kann keine Hex-Eingabe, daher wird nicht die           *
                //* Basisadresse selbst uebertragen, sondern eine einfache Modulnummer, die       *
                //* intern in MMADDR_BASE + 4*Modulnummer (0x3000...0x33FC) umgerechnet wird -    *
                //* eine neue Meldezyklus-Auswahl (0=disabled/1=enabled) - oder je Servo einen    *
                //* neuen Rot-/Gruen-Endanschlag bzw. eine neue Verzoegerung (Label "SvN RT       *
                //* max"/"SvN GN max", je 0...30; Label "SvN Delay", 0...10, N=1...4). Ein        *
                //* ungueltiger Wert bleibt bei allen Kanaelen ohne Wirkung und wird mit FALSE    *
                //* quittiert.                                                                    *
                //*********************************************************************************
                //* msg: CAN-Nachricht (msg[D5]==KANAL_MODNR/KANAL_RUECK, msg[D6..D7]=neuer Wert) *
                //*********************************************************************************

                void cs2IndexParameterExtern(const uint8_t msg[MBC_COM_MSG_LAENGE]) override;

                //*********************************************************************************
                //* Baut die angeforderte Kanalzeile (MM-Addr-, Meldezyklus- oder Servo-          *
                //* Endanschlag/-Verzoegerung-Konfigkanal) frisch in _cs2ScratchRow zusammen       *
                //* (PROGMEM-Vorlage initCs2Ex[] + aktueller Wert aus dem Systemarray/_baseAddr)   *
                //* und gibt sie zurueck, sonst nullptr. Spart gegenueber einer dauerhaften        *
                //* RAM-Kopie 14x40 Byte RAM. Der zurueckgegebene Zeiger ist NUR bis zum           *
                //* naechsten Aufruf gueltig.                                                      *
                //*********************************************************************************
                //* laufendeNr: 0..ANZMESSWERTEMODUL+ANZKONFIGMODUL-1                             *
                //*********************************************************************************
                //* Return: Zeiger auf die 40-Byte-Zeile oder nullptr                             *
                //*********************************************************************************

                const uint8_t* cs2IndexExtern(uint8_t laufendeNr) override;

        private:

            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //+ EEPROM-Zusatzregion-Byteoffsets                                                   +
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

            #define EEX_84_MMAddr  0    // 2 Byte
            #define EEX_84_AKTOR   2    // 1 Byte
            #define EEX_84_SVMAX   3    // 13 Byte
            #define EEX_84_RUECK   16   // 1 Byte
            #define EEX_84_SVWEG   17   // 480 Byte

            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //+ Modulspezifische CSx-GUI-Parameter: 14 Konfigkanaele (Zeile 0 = Label "MM-Addr",  +
            //+ Modulnummer 0...255; Zeile 1 = Label "Meldezyklus", Auswahlliste                  +
            //+ disabled/enabled; Zeilen 2..13 = je 3 Kanaele pro Servo ("SvN RT max"/"SvN GN     +
            //+ max", je 0...30; "SvN Delay", 0...10, N=1...4) - siehe                            +
            //+ KANAL_SVBASE/cs2IndexParameterExtern() in Mbc84Com.cpp). Die Kanaldefinition       +
            //+ (initCs2Ex[], PROGMEM) bleibt komplett im Flash - cs2IndexExtern() baut die        +
            //+ angeforderte Zeile bei jedem Aufruf frisch in _cs2ScratchRow zusammen statt 14     +
            //+ dauerhafte 40-Byte-Zeilen im RAM zu halten (siehe Projekt-Memory                  +
            //+ project_mbcan_cs2_konfigkanal_pattern).                                            +
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

            #define MBC84_ANZMESSWERTEMODUL 0   // Anzahl der modulspez. Messwerte
            #define MBC84_ANZKONFIGMODUL    14  // Anzahl der modulspez. Konfigurationskanaele

            uint8_t _cs2ScratchRow[MBC_COM_CS2PAKETE * 8];

            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //+ Erweiterung der CAN-Nachrichten                                                   +
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

            uint8_t _msgKonfigOk[MBC_COM_MSG_LAENGE];   // Bestaetigung "MM-Addr uebernommen"/abgelehnt

            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //+ Servo-Hardware - auf RP2040/ESP32-C3 ueber die Arduino-Bibliothek Servo, auf AVR  +
            //+ ueber eine rohe Timer1-ISR (dateiweite statische Variablen in Mbc84Com.cpp, siehe +
            //+ Klassenkommentar - kein Instanzmember noetig, die ISR kennt kein this)            +
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

            #if !defined(__AVR__)
                Servo _servo[4];
            #endif

            uint8_t _servoPin[4];

            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //+ Bewegungszustand je Servo                                                         +
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

            struct ServoState
            {
                uint8_t step;      // aktueller Index im Servoweg (0...2*SERVO_MAX_STEPS-1)
                uint8_t dir;       // 0 = naechstes Ziel Rot, 1 = naechstes Ziel Gruen
                uint8_t maxRot;    // aktueller Rot-Endanschlag (Rohwert, siehe raw2fine())
                uint8_t maxGruen;  // aktueller Gruen-Endanschlag (Rohwert, siehe raw2fine())
                uint8_t vz;        // konfigurierte Verzoegerung, siehe Klassenkommentar
                unsigned long lastStepMs;    // Zeitstempel des letzten Tabellenschritt-Wechsels
                                             // (je Servo individuell getaktet, siehe servoStep())
                unsigned long lastWriteMs;   // Zeitstempel des letzten interpolierten
                                             // Pulsbreiten-Updates (immer INTERP_STEP_MS-Takt,
                                             // unabhaengig von vz - siehe servoStep())
            };

            ServoState _servoState[4];

            // Rundlaufzeiger: ab welchem Servo servoStep() beim naechsten Aufruf nach einem
            // faelligen Schritt sucht. Zu jedem Zeitpunkt faehrt hoechstens EIN Servo aktiv
            // (sequenziell) - die uebrigen halten einfach ihre zuletzt gesendete Pulsbreite
            // (auf AVR ueber die rohe Timer2-ISR, sonst ueber die Servo-Bibliothek, siehe
            // servoStep()/servoNeedsStep()). NO_SERVO_ACTIVE (siehe Mbc84Com.cpp) = Startwert
            // vor dem ersten Schritt.

            uint8_t _activeServo;

            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //+ Stellpult/Positions-LEDs                                                          +
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

            MbcDigitalIn  *_switchboard[8];
            MbcDigitalOut *_led[8];
            uint8_t        _switchState[8];   // 1 = Taste losgelassen (Ruhezustand), 0 = gedrueckt

            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //+ Rueckmeldezyklus (einfacher Schrittzaehler, direkt aus update() bedient)          +
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

            uint8_t _zyklusstep;   // 0 = inaktiv, 1...4 = Rueckmeldung Servo (Wert-1), >4 = fertig

            uint16_t _baseAddr;   // MM-Basisadresse, siehe eepromReadParaExtern()

            #if defined(__AVR__)
                uint8_t _pinReset;
            #endif

            void servoCurveInit();
            void servoStep();
            bool servoNeedsStep(uint8_t servo);
            void servoValueDirect(uint8_t servoIdx);
            uint8_t rawAtStep(uint8_t servo, uint8_t step);
            long rawAtStepScaledFine(uint8_t servo, uint8_t step);
            void writeServoRaw(uint8_t servo, uint8_t raw);
            void writeServoRawFine(uint8_t servo, uint16_t rawFine);
            static uint16_t raw2fine(uint8_t raw);
            static uint16_t pulseUsFine(uint16_t rawFine);
            void switchboardCheck();
            void handleSwitchCommand(const uint8_t msg[MBC_COM_MSG_LAENGE]);
            void ledShow();
            void setAktorstellung(uint8_t servoIdx, bool gruen);
    };

//------ Ende Klassendefinition Mbc84Com ----------------------------------------------------------

#endif  /* _MBC84COM_H_ */

//###### Ende Quellcode Mbc84Com.h ################################################################
