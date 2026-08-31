//*************************************************************************************************
//* HEADER - Modul mbc-81: Stellpult mit 32 Tastern, 16 Stellungs-LEDs (Mbc81Lib)                 *
//*************************************************************************************************
//* Modulspezifische MbcCom-Unterklasse fuer mbc-81: 32 Taster (16 Adresspaare Rot/Gruen) ueber   *
//* 4x PCF8574 (I2C-Portexpander, Bausteinadressen 0...3, TASTER-Pin-Remap {0,1,2,3,7,6,5,4} je   *
//* Baustein - reale Verdrahtung, siehe Mbc81Com.cpp) und 16 native Stellungs-LEDs (je Adresse    *
//* eine LED, zeigt die zuletzt kommandierte Richtung). can (MbcCom-Basisklasse) = der einzige    *
//* CAN-Kanal dieses Moduls. Umschaltbar ueber das Feld "Nutzung" (Systemarray-Offset 2) zwischen *
//* zwei grundverschiedenen Betriebsarten: bei Nutzung=0 ("Switchboard") ist das Modul selbst die *
//* Schaltquelle - ein Tasterdruck sendet einen echten Motorola-Schaltbefehl                      *
//* (CAN_ID_ZUBEHOER_SCHALTEN) fuer MM-Addr...MM-Addr+15 und schaltet gleichzeitig die eigene LED *
//* (Echo, keine externe Rueckmeldung). Bei Nutzung=1/2 ("Bus 0 Matrix 1...32"/"33...64") ist das *
//* Modul dagegen ein passiver Rueckmelder/Statusspiegel - ein Tasterdruck sendet KEINEN          *
//* Schaltbefehl, sondern eine S88-Ereignismeldung (CS2-0x22-Meldung, Pinnummer                   *
//* 101+(Nutzung-1)*32+Index, D6/D7 = vergangene Zeit seit letzter Aenderung) unabhaengig von MM- *
//* Addr; eingehende Schaltbefehle im konfigurierten Adressbereich werden dagegen auf den LEDs    *
//* gespiegelt (passive Statusanzeige), siehe handleSwitchCommand(). Eine explizite               *
//* CS2-Statusabfrage antwortet mit dem echten, zuletzt erkannten Tasterzustand, siehe            *
//* handleS88Request(). 2 CS2-Konfigkanaele (Label "KeyB:", Modulnummer 0...63, Basisadresse =    *
//* MMADDR_MIN + 16*Modulnummer; Label "Nutz:", Auswahlliste 0=Switch/1=B0 1..32/2=B0 33..63 -    *
//* beide zusaetzliche Konfigkanaele, siehe cs2IndexParameterExtern()) erlauben das Setzen von    *
//* Basisadresse und Nutzung direkt aus der CSx-GUI - der rohe PC_ARRAY_DATA-Zugriff auf das      *
//* Systemarray bleibt daneben weiterhin moeglich.                                                *
//*************************************************************************************************
//* Datei: Mbc81Com.h                                                                             *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen:                                                                                  *
//* V1.0.0 - 22.08.2026 - Nullversion                                                             *
//* V1.0.1 - 29.08.2026 - RAM-Optimierung                                                         *
//*************************************************************************************************

#pragma once

//###### Start Quellcode Mbc81Com.h ###############################################################

#ifndef _MBC81COM_H_
#define _MBC81COM_H_     "V1.0.1 - 29.08.2026"

//------ Einbindung Bibliotheken ------------------------------------------------------------------

#include <Arduino.h>
#include <MbcCom.h>
#include <MbcDigitalOut.h>
#include <MbcPcf8574.h>

//------ Start Klassendefinition Mbc81Com ---------------------------------------------------------

    //* Klasse fuer <Mbc81Com>
    //* Modulspezifische MbcCom-Unterklasse fuer mbc-81. tasterChips[0..3] sind die 4 PCF8574
    //* (Bausteinadressen 0...3, je 8 Bit = 4 Adresspaare Rot/Gruen). leds[0..15] sind die 16
    //* Stellungs-LEDs (Index = MM-Adressoffset 0...15, natives GPIO).

    class Mbc81Com : public MbcCom
    {
        public:

            //*************************************************************************************
            //* Konstruktor zur Deklaration von Objektvariablen                                   *
            //* utils:       zugehoerige MbcUtils-Instanz                                         *
            //* can:         zugehoeriger CAN-Kanal dieses Moduls (Basisklassen-Parameter can)    *
            //* bootStorage: zugehoerige MbcBootStorage-Instanz (Firmware-Update-Staging)         *
            //* tasterChips: 4 PCF8574 der Tastermatrix, siehe Klassenkommentar                   *
            //* leds:        16 Stellungs-LEDs, siehe Klassenkommentar                            *
            //* pinReset:    NUR auf AVR (ATmega644P) vorhanden, siehe MbcCom-Konstruktor         *
            //*************************************************************************************

            #if defined(__AVR__)
                Mbc81Com(MbcUtils &utils, MbcCan &can, MbcBootStorage &bootStorage,
                         MbcPcf8574 *(&tasterChips)[4], MbcDigitalOut *(&leds)[16],
                         uint8_t pinReset);
            #else
                Mbc81Com(MbcUtils &utils, MbcCan &can, MbcBootStorage &bootStorage,
                         MbcPcf8574 *(&tasterChips)[4], MbcDigitalOut *(&leds)[16]);
            #endif

            //*************************************************************************************
            //* Einmalig nach dem begin() der Basisklasse aufrufen: LEDs auf OUTPUT setzen und in *
            //* den Startzustand bringen (alle "gesetzt", siehe Klassenkommentar in Mbc81Com.cpp), *
            //* gespeicherte Parameter aus der EEPROM-Zusatzregion laden und die Entprellzustaende *
            //* zuruecksetzen.                                                                     *
            //*************************************************************************************

            void beginModule();

            //*************************************************************************************
            //* Je loop()-Durchlauf aufrufen: alle 32 Taster einlesen/entprellen (loest je nach    *
            //* Nutzung einen Schaltbefehl oder eine S88-Meldung aus) sowie CAN-Nachrichtenempfang/ *
            //* -Auswertung (ruft intern auch die geerbte check() auf).                            *
            //*************************************************************************************

            void update();

        protected:

            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //+ Erweiterungspunkte der Basisklasse (siehe MbcCom.h) - mbc-81 hat 2 CS2-Konfigkanaele +
            //+ (Label "KeyB:", Modulnummer 0...63; Label "Nutz:", Auswahlliste Switch/B0 1..32/    +
            //+ B0 33..63), daher echte Logik in hashInitExtern()/cs2IndexParameterExtern()/         +
            //+ cs2IndexExtern() zusaetzlich zu eepromReadParaExtern()/eepromWriteParaExtern()      +
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

                //*********************************************************************************
                //* Laedt MM-Adresse und Nutzung aus der EEPROM-Zusatzregion (0xff-Sentinel-       *
                //* Defaults: MM-Addr 0x3000, Nutzung 0 = Switchboard).                            *
                //*********************************************************************************

                void eepromReadParaExtern() override;

                //*********************************************************************************
                //* Persistiert den betroffenen Block (MM-Addr oder Nutzung) in der EEPROM-        *
                //* Zusatzregion und haelt die intern gecachte Basisadresse/Nutzung synchron.      *
                //*********************************************************************************
                //* block: Identifier des zu schreibenden Blocks (EE_81_MMAddr/EE_81_Nutzung)      *
                //*********************************************************************************

                void eepromWriteParaExtern(uint8_t block) override;

                //*********************************************************************************
                //* Traegt GUiD/HASH in die Konfig-Bestaetigungsnachricht _msgKonfigOk ein -      *
                //* gemeinsam von beiden Konfigkanaelen gebraucht.                                *
                //*********************************************************************************

                void hashInitExtern() override;

                //*********************************************************************************
                //* Uebernimmt eine neue Modulnummer (Label "KeyB:", 0...63, intern MMADDR_MIN +   *
                //* 16*Modulnummer, 0x3000...0x33F0 - die CSx-GUI kann keine Hex-Eingabe) oder     *
                //* eine neue Nutzung-Auswahl (Label "Nutz:", 0=Switch/1=B0 1..32/2=B0 33..63) von  *
                //* der CS2. Ein ungueltiger Wert bleibt bei BEIDEN Kanaelen ohne Wirkung und wird  *
                //* mit FALSE quittiert.                                                            *
                //*********************************************************************************
                //* msg: CAN-Nachricht (msg[D5]=Kanal, msg[D6..D7]=neuer Wert)                     *
                //*********************************************************************************

                void cs2IndexParameterExtern(const uint8_t msg[MBC_COM_MSG_LAENGE]) override;

                //*********************************************************************************
                //* Baut die angeforderte Kanalzeile (KeyB- oder Nutz-Konfigkanal) frisch in       *
                //* _cs2ScratchRow zusammen (PROGMEM-Vorlage + aktueller Wert aus dem              *
                //* Systemarray/_baseAddr) und gibt sie zurueck, sonst nullptr.                    *
                //*********************************************************************************
                //* laufendeNr: 0..ANZMESSWERTEMODUL+ANZKONFIGMODUL-1                              *
                //*********************************************************************************
                //* Return: Zeiger auf _cs2ScratchRow (nur bis zum naechsten Aufruf gueltig)       *
                //*         oder nullptr                                                           *
                //*********************************************************************************

                const uint8_t* cs2IndexExtern(uint8_t laufendeNr) override;

        private:

            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //+ Modulspezifische CSx-GUI-Parameter: 2 Konfigkanaele (Label "KeyB:", Modulnummer    +
            //+ 0...63; Label "Nutz:", Auswahlliste Switch/B0 1..32/B0 33..63), siehe              +
            //+ cs2IndexParameterExtern() in Mbc81Com.cpp. Die Kanaldefinition (initCs2Ex[],       +
            //+ PROGMEM) bleibt komplett im Flash - cs2IndexExtern() baut die angeforderte Zeile   +
            //+ bei jedem Aufruf frisch in _cs2ScratchRow zusammen.                                 +
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

            #define MBC81_ANZMESSWERTEMODUL 0   // Anzahl der modulspez. Messwerte
            #define MBC81_ANZKONFIGMODUL    2   // Anzahl der modulspez. Konfigurationskanaele

            uint8_t _cs2ScratchRow[MBC_COM_CS2PAKETE * 8];

            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //+ Erweiterung der CAN-Nachrichten                                                   +
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

            uint8_t _msgKonfigOk[MBC_COM_MSG_LAENGE];   // Bestaetigung "Konfigwert uebernommen"

            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //+ Taster-/LED-Hardware                                                              +
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

            MbcPcf8574    *_tasterChips[4];
            MbcDigitalOut *_leds[16];

            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //+ Entprellung, ein Eintrag je Taster-Bit (Index 0..31 = Bausteinnummer*8+Paar*2       +
            //+ [+1 fuer Gruen], siehe Mbc81Com.cpp)                                                +
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

            bool          _pinActive[32];        // zuletzt bestaetigter (entprellter) Zustand
            bool          _pinRaw[32];           // zuletzt gelesener (unbestaetigter) Rohzustand
            unsigned long _pinStartMs[32];       // Beginn der aktuellen Rohzustands-Phase
            unsigned long _pinLastChangeMs[32];  // Zeitpunkt der letzten S88-Zustandsaenderung

            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //+ Aus dem Systemarray gecachte Parameter (siehe eepromReadParaExtern()/              +
            //+ eepromWriteParaExtern())                                                           +
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

            uint16_t _baseAddr;   // MM-Basisadresse
            uint8_t  _nutzung;    // 0 = Switchboard, 1/2 = Bus-0-Matrix-Slot

            #if defined(__AVR__)
                uint8_t _pinReset;
            #endif

            void tasterPinsCheck();
            void tasterEdge(uint8_t index, bool raw);
            void sendSwitchCommand(uint8_t index, bool active);
            void sendS88Event(uint8_t index, bool active);
            void handleS88Request(const uint8_t msg[MBC_COM_MSG_LAENGE]);
            void handleSwitchCommand(const uint8_t msg[MBC_COM_MSG_LAENGE]);
            void ledSet(uint8_t pairIndex, uint8_t direction);
    };

//------ Ende Klassendefinition Mbc81Com ----------------------------------------------------------

#endif  /* _MBC81COM_H_ */

//###### Ende Quellcode Mbc81Com.h ################################################################
