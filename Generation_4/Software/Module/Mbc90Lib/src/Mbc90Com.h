//*************************************************************************************************
//* HEADER - Modul mbc-90: S88-Rueckmeldedecoder, Vollmodul 16 Kontakte (Mbc90Lib)                *
//*************************************************************************************************
//* Modulspezifische MbcCom-Unterklasse fuer mbc-90: 16 Kontakteingaenge (aktiv-LOW, gepulste     *
//* 0-Leitung, je Pin ein millis()-basierter Integrator als Entpreller/Haltezeit-Ersatz) sowie    *
//* eine 2x8-Bit-Stellungsanzeige. Backend der Anzeige ist plattformabhaengig: auf AVR            *
//* (ATmega644P) zwei PCF8574 (I2C-Portexpander, Bausteinnummer 0 = Kontakt 1-8, Bausteinnummer 1 *
//* = Kontakt 9-16), auf allen anderen Zielplattformen (RP2040/ESP32-C3) ein einzelner MCP23S17   *
//* (SPI-Portexpander, GPIOA = Kontakt 1-8, GPIOB = Kontakt 9-16) - beide Konstruktor-/Member-    *
//* Varianten sind per #if defined(__AVR__) getrennt, siehe Konstruktor und private Member.       *
//* mbc-90 ist selbst schon ein volles 16er-Modul - es gibt daher KEIN Halbmodul-Konzept, und der *
//* 2-Byte-Rueckmeldestatus (Kontakt 1-8/9-16) wird NICHT invertiert und Bit fuer Bit direkt (Bit *
//* i = Kontakt i+1) im Systemarray abgelegt - verifiziert gegen das Parametriercenter fuer den   *
//* exakten Gegenvertrag. Eingaenge sind ueber MbcDigitalIn angebunden - je nach Sketch als       *
//* natives GPIO oder als MbcMcp23x17-Bit konstruiert (I2C oder SPI, je nach Sketch). can         *
//* (MbcCom-Basisklasse) = der einzige CAN-Kanal dieses Moduls. 17 CS2-Konfigkanaele              *
//* (S88-Modulnummer + 16x Integrationszeit K1..K16). Der rohe PC_ARRAY_DATA-Weg bleibt davon     *
//* unabhaengig weiterhin nutzbar.                                                                *
//*************************************************************************************************
//* Datei: Mbc90Com.h                                                                             *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen:                                                                                  *
//* V1.0.0 - 14.08.2026 - Nullversion                                                             *
//*************************************************************************************************

#pragma once

//###### Start Quellcode Mbc90Com.h ###############################################################

#ifndef _MBC90COM_H_
#define _MBC90COM_H_     "V1.0.0 - 14.08.2026"

//------ Einbindung Bibliotheken ------------------------------------------------------------------

#include <Arduino.h>
#include <MbcCom.h>
#include <MbcDigitalIn.h>

#if defined(__AVR__)
    #include <MbcPcf8574.h>
#else
    #include <MbcMcp23x17.h>
#endif

//------ Start Klassendefinition Mbc90Com ---------------------------------------------------------

    //* Klasse fuer <Mbc90Com>
    //* Modulspezifische MbcCom-Unterklasse fuer mbc-90. inputs: Index 0..15 = S88-Kontakt 1..16
    //* (physisches Pin/Expander-Bit waehlt allein der Sketch, siehe Beispiel-Sketches).
    //* displayLow/displayHigh (nur AVR) bzw. display (sonst): Stellungsanzeige-Baustein(e),
    //* siehe Konstruktor-Kommentar fuer die plattformabhaengige Signatur.

    class Mbc90Com : public MbcCom
    {
        public:

            //*************************************************************************************
            //* Konstruktor zur Deklaration von Objektvariablen (initialisiert ausserdem die      *
            //* modulspezifische CS2-Konfigurationstabelle _cs2IndexEx[] und die                  *
            //* Konfig-Bestaetigungsnachricht _msgKonfigOk[])                                     *
            //*************************************************************************************
            //* utils:       zugehoerige MbcUtils-Instanz                                         *
            //* can:         zugehoeriger CAN-Kanal dieses Moduls (Basisklassen-Parameter can)    *
            //* bootStorage: zugehoerige MbcBootStorage-Instanz (Firmware-Update-Staging)         *
            //* inputs:      16 Kontakteingaenge, Index-Konvention siehe Klassenkommentar         *
            //* displayLow:  NUR AVR - PCF8574 Stellungsanzeige Kontakt 1-8 (Bausteinnummer 0)    *
            //* displayHigh: NUR AVR - PCF8574 Stellungsanzeige Kontakt 9-16 (Bausteinnummer 1)   *
            //* display:     NUR NICHT-AVR - MCP23S17 Stellungsanzeige, GPIOA=Kontakt 1-8,        *
            //*              GPIOB=Kontakt 9-16 (ein einzelner Baustein deckt beide Byte ab)      *
            //* pinReset:    NUR AVR (ATmega644P) vorhanden, siehe MbcCom-Konstruktor             *
            //*************************************************************************************

            #if defined(__AVR__)
                Mbc90Com(MbcUtils &utils, MbcCan &can, MbcBootStorage &bootStorage,
                         MbcDigitalIn *(&inputs)[16], MbcPcf8574 &displayLow, MbcPcf8574 &displayHigh,
                         uint8_t pinReset);
            #else
                Mbc90Com(MbcUtils &utils, MbcCan &can, MbcBootStorage &bootStorage,
                         MbcDigitalIn *(&inputs)[16], MbcMcp23x17 &display);
            #endif

            //*************************************************************************************
            //* Einmalig nach dem begin() der Basisklasse aufrufen: gespeicherte Parameter aus    *
            //* der EEPROM-Zusatzregion laden, Integratoren zuruecksetzen und beide PCF8574 auf   *
            //* "alles aus" vorbelegen.                                                           *
            //*************************************************************************************

            void beginModule();

            //*************************************************************************************
            //* Je loop()-Durchlauf aufrufen: Kontakte einlesen/entprellen, Rueckmeldestatus und  *
            //* Stellungsanzeige aktualisieren sowie CAN-Nachrichtenempfang/-Auswertung (ruft     *
            //* intern auch die geerbte check() auf).                                             *
            //*************************************************************************************

            void update();

        protected:

            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //+ Die 7 Erweiterungspunkte der Basisklasse (siehe MbcCom.h fuer die volle           +
            //+ Beschreibung je Hook) - bei mbc-90 haben eepromReadParaExtern(),                  +
            //+ eepromWriteParaExtern() und cs2IndexParameterExtern() echte Logik (17 modulspez.  +
            //+ CS2-Konfigkanaele: S88-Modulnummer + 16x Integrationszeit K1..K16), der Rest      +
            //+ bleibt ungenutzt.                                                                 +
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

                //*********************************************************************************
                //* Laedt S88-Adresse/Integrationszeiten aus der EEPROM-Zusatzregion (mit         *
                //* Defaultwerten fuer den unbeschriebenen Zustand, 0xff-Sentinel).               *
                //*********************************************************************************

                void eepromReadParaExtern() override;

                //*********************************************************************************
                //* Schreibt den betroffenen Parameterblock in die EEPROM-Zusatzregion (kein      *
                //* RAM-Resync noetig, die Arbeitswerte lesen ohnehin direkt aus dem              *
                //* Systemarray).                                                                 *
                //*********************************************************************************
                //* block: Identifier des zu schreibenden Blocks (0x0A/0x0B)                      *
                //*********************************************************************************

                void eepromWriteParaExtern(uint8_t block) override;

                //*********************************************************************************
                //* mbc-90 hat nichts zu loeschen. Default: nichts zu tun.                        *
                //*********************************************************************************

                void eepromEraseParaExtern() override;

                //*********************************************************************************
                //* Traegt GUiD/HASH in die modulspezifische Konfig-Bestaetigungsnachricht        *
                //* _msgKonfigOk ein.                                                             *
                //*********************************************************************************

                void hashInitExtern() override;

                //*********************************************************************************
                //* mbc-90 simuliert keine fremde Softwareversion. Default: nichts zu tun.        *
                //*********************************************************************************

                void swInitExtern() override;

                //*********************************************************************************
                //* mbc-90 hat keine modulspezifischen Messwerte (ANZMESSWERTEMODUL=0). Default:  *
                //* nichts zu tun.                                                                *
                //*********************************************************************************
                //* kanal: Messkanalnummer                                                        *
                //*********************************************************************************

                void cs2IndexMesswerteExtern(uint8_t kanal) override;

                //*********************************************************************************
                //* Uebernimmt einen neuen Wert (S88-Modulnummer, Rohwert 0...93, oder eine der   *
                //* 16 Integrationszeiten, Rohwert 1...250) von der CS2, persistiert ihn und      *
                //* bestaetigt der CSx die Uebernahme. Ein ungueltiger Wert bleibt ohne Wirkung   *
                //* und wird mit FALSE quittiert.                                                 *
                //*********************************************************************************
                //* msg: CAN-Nachricht (msg[D5]=Kanalnummer, msg[D6..D7]=neuer Wert)              *
                //*********************************************************************************

                void cs2IndexParameterExtern(const uint8_t msg[MBC_COM_MSG_LAENGE]) override;

            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //+ Zusaetzliche Erweiterungspunkte (siehe Kopfkommentar von MbcCom.h)                +
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

                //*********************************************************************************
                //* mbc-90 hat keine feste Kennung - sie kommt vom Parametriercenter der CS2      *
                //* (Standardverhalten).                                                          *
                //*********************************************************************************
                //* knH/knL: unveraendert                                                         *
                //*********************************************************************************
                //* Return: false (keine feste Kennung)                                           *
                //*********************************************************************************

                bool kennerFixedExtern(uint8_t &knH, uint8_t &knL) override;

                //*********************************************************************************
                //* mbc-90 hat keinen Zusatz-Versandweg. Default: nichts zu tun.                  *
                //*********************************************************************************
                //* msg: Nachrichtenarray                                                         *
                //*********************************************************************************

                void sendMessageExtern(const uint8_t msg[MBC_COM_MSG_LAENGE]) override;

                //*********************************************************************************
                //* Liefert eine Zeile aus _cs2IndexEx[] (S88-Modulnummer oder eine der 16        *
                //* Integrationszeiten) oder nullptr, wenn laufendeNr keinen Kanal mehr           *
                //* bezeichnet.                                                                   *
                //*********************************************************************************
                //* laufendeNr: 0..MBC90_ANZMESSWERTEMODUL+MBC90_ANZKONFIGMODUL-1                 *
                //*********************************************************************************
                //* Return: Zeiger auf die 40-Byte-Zeile oder nullptr                             *
                //*********************************************************************************

                const uint8_t* cs2IndexExtern(uint8_t laufendeNr) override;

        private:

            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //+ Modulspezifische CSx-GUI-Parameter: 17 Konfigkanaele (S88-Modulnummer, zuerst     +
            //+ einzeln hardwareverifiziert, danach die 16 Integrationszeiten K1..K16             +
            //+ nachgeruestet).                                                                   +
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

            #define MBC90_ANZMESSWERTEMODUL 0    // Anzahl der modulspez. Messwerte
            #define MBC90_ANZKONFIGMODUL    17   // Anzahl der modulspez. Konfigurationskanaele

            uint8_t _cs2IndexEx[MBC90_ANZMESSWERTEMODUL + MBC90_ANZKONFIGMODUL][MBC_COM_CS2PAKETE * 8];

            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //+ Erweiterung der CAN-Nachrichten                                                   +
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

            uint8_t _msgKonfigOk[MBC_COM_MSG_LAENGE];   // Bestaetigung "Konfigwert uebernommen"

            MbcDigitalIn *_inputs[16];

            #if defined(__AVR__)
                MbcPcf8574 *_displayLow;    // Kontakt 1-8 (Bausteinnummer 0)
                MbcPcf8574 *_displayHigh;   // Kontakt 9-16 (Bausteinnummer 1)
            #else
                MbcMcp23x17 *_display;      // GPIOA=Kontakt 1-8, GPIOB=Kontakt 9-16
            #endif

            bool          _pinActive[16];        // Kontaktstatus aktiv/inaktiv (nach Entprellung)
            unsigned long _pinStartMs[16];        // Integrator-Startzeitpunkt
            unsigned long _pinLastChangeMs[16];   // Zeitpunkt der letzten Statusaenderung

            uint8_t _displayLowPrev;    // letztes tatsaechlich geschriebenes Byte je Anzeigehaelfte
            uint8_t _displayHighPrev;   // - nur bei Aenderung erneut schreiben

            uint16_t busNrFromS88AddrLow(uint8_t s88AddrLow);
            void     sendS88Event(uint8_t index, uint8_t eventActive);
            void     s88PinsCheck();
            void     handleS88Request(const uint8_t msg[MBC_CAN_MSG_LEN]);
    };

//------ Ende Klassendefinition Mbc90Com ----------------------------------------------------------

#endif  /* _MBC90COM_H_ */

//###### Ende Quellcode Mbc90Com.h ################################################################
