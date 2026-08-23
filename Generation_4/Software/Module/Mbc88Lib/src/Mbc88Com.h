//*************************************************************************************************
//* HEADER - Modul mbc-88: S88-Rueckmeldedecoder (Mbc88Lib)                                       *
//*************************************************************************************************
//* Modulspezifische MbcCom-Unterklasse fuer mbc-88: 8 Kontakteingaenge (aktiv-LOW, gepulste      *
//* 0-Leitung, je Pin ein millis()-basierter Integrator als Entpreller/Haltezeit-Ersatz) sowie 8  *
//* Status-LED-Ausgaenge (spiegeln pro Kontakt, ob aktuell EIN Ereignis ansteht - Belegtmelder-   *
//* Anzeige, reine Anzeige, keine steuernde Funktion). Ein- und Ausgaenge sind ueber              *
//* MbcDigitalIn/MbcDigitalOut angebunden - je nach Sketch als natives GPIO oder als Bit eines    *
//* zusaetzlichen MCP23S17 konstruiert. can (MbcCom-Basisklasse) = der einzige CAN-Kanal dieses   *
//* Moduls (identisch zur CS2-/MBCAN-Anbindung anderer Zubehoermodule).                           *
//*************************************************************************************************
//* Datei: Mbc88Com.h                                                                             *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen:                                                                                  *
//* V1.0.0 - 08.08.2026 - Nullversion                                                             *
//*************************************************************************************************

#pragma once

//###### Start Quellcode Mbc88Com.h ###############################################################

#ifndef _MBC88COM_H_
#define _MBC88COM_H_     "V1.0.0 - 08.08.2026"

//------ Einbindung Bibliotheken ------------------------------------------------------------------

#include <Arduino.h>
#include <MbcCom.h>
#include <MbcDigitalOut.h>
#include <MbcDigitalIn.h>

//------ Start Klassendefinition Mbc88Com ---------------------------------------------------------

    //* Klasse fuer <Mbc88Com>
    //* Modulspezifische MbcCom-Unterklasse fuer mbc-88. inputs/outputs: Index 0..7 = S88-Kontakt
    //* 1..8 des per Halbmodul-Parameter gewaehlten 8er-Bereichs (welches physische Pin/Expander-
    //* Bit dahintersteckt, entscheidet allein der Sketch, siehe Beispiel-Sketches).

    class Mbc88Com : public MbcCom
    {
        public:

            //*************************************************************************************
            //* Konstruktor zur Deklaration von Objektvariablen (initialisiert ausserdem die      *
            //* modulspezifische CS2-Konfigurationstabelle _cs2IndexEx[] und die                  *
            //* Konfig-Bestaetigungsnachricht _msgKonfigOk[])                                     *
            //*************************************************************************************
            //* utils:      zugehoerige MbcUtils-Instanz                                          *
            //* can:        zugehoeriger CAN-Kanal dieses Moduls (Basisklassen-Parameter can)     *
            //* bootStorage: zugehoerige MbcBootStorage-Instanz (Firmware-Update-Staging)         *
            //* inputs:     8 Kontakteingaenge, Index-Konvention siehe Klassenkommentar           *
            //* outputs:    8 Status-LED-Ausgaenge, Index-Konvention siehe Klassenkommentar       *
            //* pinReset:   NUR auf AVR (ATmega644P) vorhanden, siehe MbcCom-Konstruktor          *
            //*************************************************************************************

            #if defined(__AVR__)
                Mbc88Com(MbcUtils &utils, MbcCan &can, MbcBootStorage &bootStorage,
                         MbcDigitalIn *(&inputs)[8], MbcDigitalOut *(&outputs)[8], uint8_t pinReset);
            #else
                Mbc88Com(MbcUtils &utils, MbcCan &can, MbcBootStorage &bootStorage,
                         MbcDigitalIn *(&inputs)[8], MbcDigitalOut *(&outputs)[8]);
            #endif

            //*************************************************************************************
            //* Einmalig nach dem begin() der Basisklasse aufrufen: gespeicherte Parameter aus    *
            //* der EEPROM-Zusatzregion laden. inputs[i]->begin()/outputs[i]->begin() vorher im   *
            //* Sketch aufrufen.                                                                  *
            //*************************************************************************************

            void beginModule();

            //*************************************************************************************
            //* Je loop()-Durchlauf aufrufen: Kontakte einlesen/entprellen, Status-LEDs           *
            //* aktualisieren und CAN-Nachrichtenempfang/-Auswertung (ruft intern auch die        *
            //* geerbte check() auf).                                                             *
            //*************************************************************************************

            void update();

        protected:

            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //+ Die 7 Erweiterungspunkte der Basisklasse (siehe MbcCom.h fuer die volle           +
            //+ Beschreibung je Hook) - bei mbc-88 haben eepromReadParaExtern(),                  +
            //+ eepromWriteParaExtern() und cs2IndexParameterExtern() echte Logik                 +
            //+ (S88-Modulnummer, Halbmodul und die 8 Integrationszeiten als CS2-Konfigkanaele),  +
            //+ der Rest bleibt leer                                                              +
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

                //*********************************************************************************
                //* Laedt S88-Adresse/Integrationszeiten/Halbmodul aus der EEPROM-Zusatzregion    *
                //* (mit Defaultwerten fuer den unbeschriebenen Zustand, 0xff-Sentinel).          *
                //*********************************************************************************

                void eepromReadParaExtern() override;

                //*********************************************************************************
                //* Schreibt den betroffenen Parameterblock in die EEPROM-Zusatzregion (kein      *
                //* RAM-Resync noetig, die Arbeitswerte lesen ohnehin direkt aus dem              *
                //* Systemarray).                                                                 *
                //*********************************************************************************
                //* block: Identifier des zu schreibenden Blocks (0x0A....)                       *
                //*********************************************************************************

                void eepromWriteParaExtern(uint8_t block) override;

                //*********************************************************************************
                //* mbc-88 hat nichts zu loeschen. Default: nichts zu tun.                        *
                //*********************************************************************************

                void eepromEraseParaExtern() override;

                //*********************************************************************************
                //* Traegt GUiD/HASH in die modulspezifische Konfig-Bestaetigungsnachricht        *
                //* _msgKonfigOk ein.                                                             *
                //*********************************************************************************

                void hashInitExtern() override;

                //*********************************************************************************
                //* mbc-88 simuliert keine fremde Softwareversion. Default: nichts zu tun.        *
                //*********************************************************************************

                void swInitExtern() override;

                //*********************************************************************************
                //* mbc-88 hat keine modulspezifischen Messwerte (ANZMESSWERTEMODUL=0). Default:  *
                //* nichts zu tun.                                                                *
                //*********************************************************************************
                //* kanal: Messkanalnummer                                                        *
                //*********************************************************************************

                void cs2IndexMesswerteExtern(uint8_t kanal) override;

                //*********************************************************************************
                //* Uebernimmt eine neue S88-Modulnummer (1...93), ein neues Halbmodul (0/1) oder *
                //* eine neue Integrationszeit (Kontakt 1...8, Rohwert 1...250) von der CS2,      *
                //* persistiert den Wert und bestaetigt der CS2 die Uebernahme. Ein ungueltiger   *
                //* Wert bleibt ohne Wirkung und wird mit FALSE quittiert.                        *
                //*********************************************************************************
                //* msg: CAN-Nachricht (msg[D5]==KANAL_S88MOD/KANAL_S88HALBMODUL/                 *
                //* KANAL_S88TIMING(0..7), msg[D6..D7]=neuer Wert)                                *
                //*********************************************************************************

                void cs2IndexParameterExtern(const uint8_t msg[MBC_COM_MSG_LAENGE]) override;

            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //+ Zusaetzliche Erweiterungspunkte (siehe Kopfkommentar von MbcCom.h)                +
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

                //*********************************************************************************
                //* mbc-88 hat keine feste Kennung - sie kommt vom Parametriercenter der CS2      *
                //* (Standardverhalten).                                                          *
                //*********************************************************************************
                //* knH/knL: unveraendert                                                         *
                //*********************************************************************************
                //* Return: false (keine feste Kennung)                                           *
                //*********************************************************************************

                bool kennerFixedExtern(uint8_t &knH, uint8_t &knL) override;

                //*********************************************************************************
                //* mbc-88 hat keinen Zusatz-Versandweg. Default: nichts zu tun.                  *
                //*********************************************************************************
                //* msg: Nachrichtenarray                                                         *
                //*********************************************************************************

                void sendMessageExtern(const uint8_t msg[MBC_COM_MSG_LAENGE]) override;

                //*********************************************************************************
                //* Liefert eine der zehn Zeilen aus _cs2IndexEx[] (S88-Modulnummer/Halbmodul/    *
                //* 8x Integrationszeit) oder nullptr, wenn laufendeNr keinen Kanal mehr          *
                //* bezeichnet.                                                                   *
                //*********************************************************************************
                //* laufendeNr: 0..MBC88_ANZMESSWERTEMODUL+MBC88_ANZKONFIGMODUL-1                 *
                //*********************************************************************************
                //* Return: Zeiger auf die 40-Byte-Zeile oder nullptr                             *
                //*********************************************************************************

                const uint8_t* cs2IndexExtern(uint8_t laufendeNr) override;

        private:

            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //+ Modulspezifische CSx-GUI-Parameter: zehn Konfigkanaele (S88-Modulnummer,          +
            //+ Halbmodul, 8x Integrationszeit)                                                   +
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

            #define MBC88_ANZMESSWERTEMODUL 0    // Anzahl der modulspez. Messwerte
            #define MBC88_ANZKONFIGMODUL    10   // Anzahl der modulspez. Konfigurationskanaele

            uint8_t _cs2IndexEx[MBC88_ANZMESSWERTEMODUL + MBC88_ANZKONFIGMODUL][MBC_COM_CS2PAKETE * 8];

            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //+ Erweiterung der CAN-Nachrichten                                                   +
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

            uint8_t _msgKonfigOk[MBC_COM_MSG_LAENGE];   // Bestaetigung "Konfigwert uebernommen"

            MbcDigitalIn  *_inputs[8];
            MbcDigitalOut *_outputs[8];

            bool          _pinActive[8];          // Kontakt aktuell ausgeloest?
            unsigned long _pinStartMs[8];         // Integrator-Start
            unsigned long _pinLastChangeMs[8];    // Zeit seit letztem Statuswechsel

            uint16_t busNrFromS88AddrLow(uint8_t s88AddrLow);
            void     sendS88Event(uint8_t index, uint8_t eventActive);
            void     s88PinsCheck();
            void     handleS88Request(const uint8_t msg[MBC_CAN_MSG_LEN]);
    };

//------ Ende Klassendefinition Mbc88Com ----------------------------------------------------------

#endif  /* _MBC88COM_H_ */

//###### Ende Quellcode Mbc88Com.h ################################################################
