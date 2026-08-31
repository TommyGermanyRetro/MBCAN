//*************************************************************************************************
//* HEADER - Modul mbc-89: Connect6021-Interface (Mbc89Lib)                                       *
//*************************************************************************************************
//*************************************************************************************************
//* mbc-89 bindet die analoge Maerklin-6021-Systemwelt (Control Unit 6021, Keyboard 6040/6041,    *
//* Memory 6043, Control 80f) sowie ein direkt am Gleis anliegendes analoges MM2-Signal an die    *
//* CS2/MS2 ueber CAN an. Dazu vereint es zwei voneinander unabhaengige, hardwarezeitkritische    *
//* Subsysteme: einen CU-Bus (proprietaerer I2C-Bus, gleichzeitig SLAVE und MASTER, rohe          *
//* AVR-TWI-Registerzustandsmaschine, siehe cuI2cCheck()/i2cSendCu() in Mbc89Com.cpp) und einen   *
//* Gleis-Signaldecoder (INT0+Timer1, Trinaer-Dekodierautomat fuer Lok- und Magnetartikelbefehle, *
//* siehe trackCheck()/automatMagnet()/automatLok()). Beide bilden MM-Adressen (0...79) ueber eine*
//* 80-Lok-Adressdatenbank (CONNECT6021-Protokoll) auf CS2-GUiDs ab. mbc-89 hat 69                 *
//* CS2-Konfigkanaele: 5 "klassische" (SW Major/Minor der simulierten Connect6021-                *
//* Softwareversion, Zentralentyp-Mapping, Keyboard-Basisadressblock, I2C-Bus-Frequenz) sowie 64   *
//* Kanaele "KeyBx GroupY" fuer das Zubehoerprotokoll je Stellpult (weiterhin zusaetzlich ueber    *
//* PC_ARRAY_DATA/ParaCenter setzbar). Meldet sich mit der                                         *
//* Maerklin-fest-vorgegebenen Connect6021-Kennung 0x0020 an (kennerFixedExtern()). Verarbeitet   *
//* eingehende System-Stopp-/Go-Telegramme (CAN-ID 0x00) direkt in update() ueber die simulierten *
//* CU-Tasten (inkl. eines auffaelligen, aber unveraendert uebernommenen GUiD-Vergleichs, siehe   *
//* Mbc89Com.cpp). Beide Subsysteme sind sowohl fuer ATmega644P (rohe AVR-TWI-/Timer1-/           *
//* INT0-Register, siehe cuI2cCheck()/i2cSendCu()/ISR(INT0_vect)) als auch fuer ESP32-C3           *
//* implementiert (selbstgeschriebene Software-I2C-Engine fuer den CU-Bus, RMT-Peripherie fuer den*
//* Gleisdecoder, siehe die jeweiligen #else-Zweige) - kein RP2040-Support (siehe                 *
//* #error-Guard unten). CU-Bus-/Gleisdecoder-Zustand liegt in dateibegrenzten statischen         *
//* Variablen (siehe Klassenkommentar in Mbc89Com.cpp) - Mbc89Com ist daher NUR fuer GENAU EIN    *
//* Objekt je Sketch ausgelegt, ein zweites Mbc89Com wuerde sich denselben Zustand teilen.         *
//*************************************************************************************************
//*************************************************************************************************
//* Datei: Mbc89Com.h                                                                             *
//*************************************************************************************************
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//*************************************************************************************************
//* Aenderungen:                                                                                  *
//* V1.0.0 - 26.08.2026 - Nullversion                                                             *
//* V1.0.1 - 29.08.2026 - KeyBx/GroupY-Konfigkanaele, AVR-/ESP32-Kompilierfixes                   *
//*************************************************************************************************

#pragma once

//###### Start Quellcode Mbc89Com.h ###############################################################

#ifndef _MBC89COM_H_
#define _MBC89COM_H_     "V1.1.0 - 29.08.2026"

#if !defined(__AVR__) && !defined(ESP32)
    #error "Mbc89Lib benoetigt entweder rohe AVR-TWI-/Timer1-/Timer2-/INT0-Register "\
           "(ATmega644P/MightyCore) oder die ESP32-RMT-Peripherie plus eine selbst "\
           "geschriebene Software-I2C-Engine (ESP32-C3) fuer CU-Bus und Gleisdecoder - "\
           "kein RP2040-Support."
#endif

//------ Einbindung Bibliotheken ------------------------------------------------------------------

#include <Arduino.h>
#include "Mbc89ComBase.h"

#if !defined(__AVR__)
    #include "Mbc89DigitalIn.h"
    #include "Mbc89DigitalOut.h"
#endif

//------ Start Klassendefinition Mbc89Com ---------------------------------------------------------

    //* Klasse fuer <Mbc89Com>
    //* Modulspezifische Mbc89ComBase-Unterklasse fuer mbc-89. Auf ATmega644P hat mbc-89 keine
    //* eigenen Ausgangs-/Konfigpins im Konstruktor - CU-Bus (TWI, PC0/PC1) und Gleisdecoder (INT0,
    //* PD2) haengen an fest durch die Hardware vorgegebenen AVR-Registern (siehe Mbc89Com.cpp),
    //* nicht an frei waehlbaren Pins. Auf ESP32-C3 gibt es keine vergleichbare feste TWI-Hardware
    //* (die Software-I2C-Engine kann jeden GPIO nutzen) - dort sind CU-Bus-/Gleisdecoder-Pins daher
    //* echte Konstruktorparameter (siehe #if defined(__AVR__) unten). anzMesswerteModul=0,
    //* anzKonfigModul=5 - mbc-89 hat fuenf CS2-GUI-Kanaele (SW Major/Minor, Mapping, Basis, I2C).

    class Mbc89Com : public Mbc89ComBase
    {
        public:

            #if defined(__AVR__)

                //*********************************************************************************
                //* Konstruktor zur Deklaration von Objektvariablen (ATmega644P) - CU-Bus-/        *
                //* Gleisdecoder-Pins liegen fest an der AVR-TWI-/INT0-Hardware, keine eigenen     *
                //* Konstruktorparameter dafuer.                                                   *
                //*********************************************************************************
                //* utils:       zugehoerige Mbc89Utils-Instanz                                    *
                //* can:         zugehoeriger CAN-Kanal dieses Moduls (Basisklassen-Parameter can) *
                //* bootStorage: zugehoerige Mbc89BootStorage-Instanz (Firmware-Update-Staging)    *
                //* pinReset:    Pin, ueber den sich das Modul per Watchdog-Trick selbst neu       *
                //* startet                                                                        *
                //*********************************************************************************

                Mbc89Com(Mbc89Utils &utils, Mbc89Can &can, Mbc89BootStorage &bootStorage,
                         uint8_t pinReset);

            #else

                //*********************************************************************************
                //* Konstruktor zur Deklaration von Objektvariablen (ESP32-C3) - CU-Bus            *
                //* (Software-I2C) und Gleisdecoder (RMT) sind zeitkritisch und brauchen daher      *
                //* echte native GPIOs; STOP/GO/EXT sind unkritische, einfache Digitalsignale und   *
                //* koennen wahlweise nativ ODER ueber einen Expander laufen (siehe                 *
                //* Mbc89DigitalIn/Mbc89DigitalOut/Mbc89Mcp23x17, z.B. bei GPIO-Knappheit wie auf   *
                //* dem ESP32-C3 SuperMini) - deshalb hier als Referenz auf die bereits vom Sketch  *
                //* konfigurierten Digitalobjekte statt als rohe Pinnummer. Kein pinReset - ESP32   *
                //* nutzt den Software-Reset des Cores (siehe Mbc89Platform::resetDevice()).        *
                //*********************************************************************************
                //* utils:        zugehoerige Mbc89Utils-Instanz                                   *
                //* can:          zugehoeriger CAN-Kanal dieses Moduls (Basisklassen-Parameter can)*
                //* bootStorage:  zugehoerige Mbc89BootStorage-Instanz (Firmware-Update-Staging)   *
                //* pinCuScl:     CU-Bus SCL (Software-I2C, natives GPIO, zeitkritisch)             *
                //* pinCuSda:     CU-Bus SDA (Software-I2C, natives GPIO, zeitkritisch)             *
                //* cuStopIn:     STOPP-Taste an der CU (Eingang)                                  *
                //* cuStopOut:    STOPP-Taste an der CU simulieren (Ausgang)                       *
                //* cuGoIn:       GO-Taste an der CU (Eingang)                                     *
                //* cuGoOut:      GO-Taste an der CU simulieren (Ausgang)                          *
                //* cuExt:        CU-Anwesenheitserkennung (Eingang, ohne Pull-Up)                 *
                //* pinTrack:     Gleisdecoder-Signaleingang (natives GPIO, RMT-Kanal, zeitkritisch)*
                //*********************************************************************************

                Mbc89Com(Mbc89Utils &utils, Mbc89Can &can, Mbc89BootStorage &bootStorage,
                         uint8_t pinCuScl, uint8_t pinCuSda, Mbc89DigitalIn &cuStopIn,
                         Mbc89DigitalOut &cuStopOut, Mbc89DigitalIn &cuGoIn,
                         Mbc89DigitalOut &cuGoOut, Mbc89DigitalIn &cuExt, uint8_t pinTrack);

            #endif

            //*************************************************************************************
            //* Einmalig nach dem begin() der Basisklasse aufrufen: CU-Bus (TWI/Timer2) und       *
            //* Gleisdecoder (INT0/Timer1) scharfschalten, gespeicherte Parameter aus der EEPROM- *
            //* Zusatzregion laden.                                                               *
            //*************************************************************************************

            void beginModule();

            //*************************************************************************************
            //* Je loop()-Durchlauf aufrufen: CU-Anwesenheit/I2C-Nachrichten pruefen, Gleissignal *
            //* auswerten, CAN-Nachrichtenempfang/-Auswertung (ruft intern auch die geerbte       *
            //* check() auf).                                                                     *
            //*************************************************************************************

            void update();

            //*************************************************************************************
            //* NUR fuer die freistehenden ISR-Hilfsfunktionen in Mbc89Com.cpp gedacht (CU-Bus/   *
            //* Gleisdecoder sind aus Registerzugriffsgruenden freie Funktionen, keine            *
            //* Klassenmethoden - siehe Klassenkommentar in Mbc89Com.cpp) - kein Bestandteil der  *
            //* oeffentlichen Sketch-API. Reicht mit Sende-Wiederholung (siehe                    *
            //* Mbc89ComBase::sendMessage()) an den CAN-Bus weiter.                               *
            //*************************************************************************************
            //*************************************************************************************
            //* msg: MBC_COM_MSG_LAENGE Byte                                                      *
            //*************************************************************************************
            //*************************************************************************************
            //* Return: false bei Sendefehler                                                     *
            //*************************************************************************************

            bool sendCan(const uint8_t msg[MBC_COM_MSG_LAENGE]);

            //*************************************************************************************
            //* NUR fuer die freistehenden ISR-Hilfsfunktionen in Mbc89Com.cpp gedacht (siehe     *
            //* sendCan()) - baut aus dem CS2-0x16-Vorlagentemplate der Basisklasse (can_msg[],   *
            //* PRIO/HASH/GUiD bereits vorbelegt) eine Zubehoer-Schalten-Nachricht und sendet sie.*
            //* Wird sowohl vom CU-Bus (Keyboard-Weiterleitung) als auch vom Gleisdecoder         *
            //* (Magnetartikel-Dekodierung, dort zusaetzlich auf den I2C-Bus gespiegelt)          *
            //* benoetigt.                                                                        *
            //*************************************************************************************
            //*************************************************************************************
            //* addr:     CS2-Zubehoeradresse                                                     *
            //* richtung: 0=Rot/Aus, 1=Gruen/Ein (Spulennummer bereits eincodiert)                *
            //* strom:    true=Strom an, false=Strom aus                                          *
            //* outMsg:   optional - erhaelt eine Kopie der gesendeten MBC_CAN_MSG_LEN-Byte-      *
            //* Nachricht (fuer die I2C-Spiegelung im Gleisdecoder), nullptr wenn nicht benoetigt *
            //*************************************************************************************
            //*************************************************************************************
            //* Return: false bei Sendefehler                                                     *
            //*************************************************************************************

            bool sendZubehoerSchalten(uint16_t addr, uint8_t richtung, bool strom,
                                      uint8_t *outMsg = nullptr);

            //*************************************************************************************
            //* NUR fuer die freistehende cuTastenCheckFn() in Mbc89Com.cpp gedacht - liefert die *
            //* spontanen System-Stopp-/Go-Broadcast-Vorlagen (_msgStop/_msgGo) mit bereits       *
            //* aktuellem HASH (siehe hashInitExtern()), damit ein physischer CU-Tastendruck      *
            //* korrekt an die CS2 gemeldet werden kann.                                          *
            //*************************************************************************************

            const uint8_t* stopBroadcast() const { return _msgStop; }
            const uint8_t* goBroadcast()   const { return _msgGo; }

        protected:

            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //+ Die 7 Erweiterungspunkte der Basisklasse (siehe Mbc89ComBase.h) - bei mbc-89 haben+
            //+ eepromReadParaExtern()/eepromWriteParaExtern()/hashInitExtern()/swInitExtern()/   +
            //+ kennerFixedExtern()/cs2IndexExtern()/cs2IndexParameterExtern() echte Logik.       +
            //+ cs2IndexMesswerteExtern() bleibt auf dem leeren Default der Basisklasse           +
            //+ (ANZMESSWERTEMODUL=0), eepromEraseParaExtern() ebenso (Original hat nichts zu     +
            //+ loeschen).                                                                        +
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

                //*********************************************************************************
                //* Laedt SW-Version, Zubehoerprotokoll je Stellpult (65 Byte), Zentralentyp-     *
                //* Mapping, Basisadressblock und I2C-Frequenz aus der EEPROM-Zusatzregion (5     *
                //* Bloecke, siehe EE_89_*-Konstanten in Mbc89Com.cpp), 0xff-Sentinel fuer den    *
                //* unbeschriebenen Zustand.                                                      *
                //*********************************************************************************

                void eepromReadParaExtern() override;

                //*********************************************************************************
                //* Persistiert einen der 5 Parameterbloecke, je nach block getrennt.             *
                //*********************************************************************************
                //*********************************************************************************
                //* block: Identifier des zu schreibenden Blocks (0x0A...0x0E)                    *
                //*********************************************************************************

                void eepromWriteParaExtern(uint8_t block) override;

                //*********************************************************************************
                //* Traegt GUiD/HASH in _msgStopResponse/_msgGoResponse ein (Antworten auf einen  *
                //* empfangenen System-Stopp/-Go-Wunsch). _msgStop/_msgGo (spontane Broadcasts)   *
                //* bekommen bewusst NUR den HASH - ihre GUiD bleibt die universelle              *
                //* Broadcast-GUiD 0x00000000 (Originalverhalten, 1:1 uebernommen). Traegt        *
                //* ausserdem GUiD/HASH in die Konfigkanal-Bestaetigung _msgKonfigOk ein (siehe   *
                //* cs2IndexParameterExtern()).                                                   *
                //*********************************************************************************

                void hashInitExtern() override;

                //*********************************************************************************
                //* Setzt die simulierte Connect6021-Softwareversion in die CS2-Ping-Antwort.     *
                //*********************************************************************************

                void swInitExtern() override;

                //*********************************************************************************
                //* Modulspezifischen Parameterwert von der CS2 uebernehmen: SW-Major-/           *
                //* SW-Minor-Konfigkanaele der simulierten Connect6021-Softwareversion (Slider,   *
                //* 1...255 je Byte, gemeinsam ueber EE_89_SW persistiert), Mapping-Konfigkanal   *
                //* (Auswahlliste, Zentralentyp, ueber EE_89_MAPPING persistiert), Basis-/         *
                //* I2C-Konfigkanaele (Slider, ueber EE_89_BASIS/EE_89_I2C persistiert). Ein       *
                //* EEPROM-Schreibzugriff erfolgt NUR, wenn sich der Wert gegenueber dem           *
                //* bisherigen Systemarray-Inhalt tatsaechlich aendert (die CSx sendet             *
                //* Konfigwerte auch als Echo/Polling erneut) - bei einer echten Aenderung des     *
                //* Mapping-Kanals loest das zusaetzlich einen Neustart aus (siehe                 *
                //* triggerReset()).                                                               *
                //*********************************************************************************
                //*********************************************************************************
                //* msg: CAN-Nachricht (msg[D5]=Kanalnummer, msg[D6..D7]=neuer Wert)              *
                //*********************************************************************************

                void cs2IndexParameterExtern(const uint8_t msg[MBC_COM_MSG_LAENGE]) override;

                //*********************************************************************************
                //* Baut die angeforderte Kanalzeile (mbc-89 hat 69: SW-Major/SW-Minor/Mapping/    *
                //* Basis/I2C sowie 64 "KeyBx GroupY"-Kanaele fuer das Zubehoerprotokoll) frisch    *
                //* in _cs2ScratchRow zusammen und gibt sie zurueck, sonst nullptr. Alle 69 Zeilen  *
                //* kommen aus der PROGMEM-Vorlage initCs2Ex[] + aktuellem Wert aus dem             *
                //* Systemarray. Der zurueckgegebene Zeiger ist NUR bis zum naechsten Aufruf        *
                //* gueltig (gemeinsamer Puffer, kein Halten/Vergleichen zweier Zeilen              *
                //* gleichzeitig moeglich).                                                         *
                //*********************************************************************************
                //*********************************************************************************
                //* laufendeNr: 0..ANZMESSWERTEMODUL+ANZKONFIGMODUL-1                             *
                //*********************************************************************************
                //*********************************************************************************
                //* Return: Zeiger auf die 40-Byte-Zeile (_cs2ScratchRow) oder nullptr             *
                //*********************************************************************************

                const uint8_t* cs2IndexExtern(uint8_t laufendeNr) override;

            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //+ Zusaetzliche, mbc-89-spezifische Erweiterungspunkte (siehe Kopfkommentar von      +
            //+ Mbc89ComBase.h)                                                                   +
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

                //*********************************************************************************
                //* Liefert die feste CS2-Geraetekennung 0x0020 (Maerklin-Vorgabe fuer            *
                //* Connect6021-Geraete wie mbc-89 - anders als bei den Booster-Modulen           *
                //* mbc-80/82/93 ist diese Zuweisung im Original-Disassembly widerspruchsfrei,    *
                //* kein Kompilier-Bug).                                                          *
                //*********************************************************************************
                //*********************************************************************************
                //* knH/knL: gesetzt auf die feste Kennung, wenn Return true                      *
                //*********************************************************************************
                //*********************************************************************************
                //* Return: true (mbc-89 hat immer eine feste Kennung)                            *
                //*********************************************************************************

                bool kennerFixedExtern(uint8_t &knH, uint8_t &knL) override;

        private:

            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //+ Erweiterung der CAN-Nachrichten: _msgStop/_msgGo sind die SPONTANEN Broadcasts    +
            //+ (universelle GUiD 0x00000000, nur HASH gesetzt - bei Start und bei physischem CU- +
            //+ Tastendruck gesendet), _msgStopResponse/_msgGoResponse die Antworten auf einen von+
            //+ der CS2 EMPFANGENEN Stopp-/Go-Wunsch (echte GUiD, siehe hashInitExtern()/         +
            //+ handleSystemCommand())                                                            +
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

            uint8_t _msgStop[MBC_COM_MSG_LAENGE];
            uint8_t _msgGo[MBC_COM_MSG_LAENGE];
            uint8_t _msgStopResponse[MBC_COM_MSG_LAENGE];
            uint8_t _msgGoResponse[MBC_COM_MSG_LAENGE];
            uint8_t _msgKonfigOk[MBC_COM_MSG_LAENGE];   // Bestaetigung "Konfigwert uebernommen"

            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //+ Modulspezifische CSx-GUI-Parameter (ANZMESSWERTEMODUL=0, ANZKONFIGMODUL=69): 5     +
            //+ "klassische" Konfigkanaele - Zeile 0 = "SW Major:", Zeile 1 = "SW Minor:", je      +
            //+ Slider 1...255; Zeile 2 = "Mapping:", Auswahlliste (Zentralentyp, "Mapping"/       +
            //+ "Mapping&Direkt"); Zeile 3 = "Basis:", Slider 1...4 (Keyboard-Basisadressblock);   +
            //+ Zeile 4 = "I2C:", Slider 40...100 (I2C-Bus-Takt in kHz) - Grenzen/Optionen jeweils +
            //+ 1:1 aus dem zugehoerigen ParaCenter-Steuerelement uebernommen. Zeilen 5...68: 64   +
            //+ Kanaele "KeyBx GroupY:" (x=1...16 Keyboard, y=1...4 Gruppe/Stellpultblock),         +
            //+ Auswahlliste MM/DCC je Protokoll(keyboard*4+gruppe) aus mbc_modul_89.vb - bisher    +
            //+ nur ueber PC_ARRAY_DATA/ParaCenter setzbar, jetzt zusaetzlich per CS2-GUI. Siehe    +
            //+ cs2IndexParameterExtern()/cs2IndexExtern() in Mbc89Com.cpp. Alle 69 Zeilen liegen   +
            //+ als Literal-PROGMEM-Zeilen in initCs2Ex[] (Byte-Inhalt der 64 KeyB/Group-Zeilen     +
            //+ geskriptet erzeugt und gegengeprueft, nicht von Hand ausgezaehlt). cs2IndexExtern() +
            //+ baut die angeforderte Zeile bei jedem Aufruf frisch in _cs2ScratchRow zusammen      +
            //+ statt 69 dauerhafte 40-Byte-Zeilen im RAM zu halten - spart RAM. Der Puffer ist nur +
            //+ bis zum naechsten cs2IndexExtern()-Aufruf gueltig (siehe dort).                     +
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

            #define MBC89_ANZMESSWERTEMODUL 0   // Anzahl der modulspez. Messwerte
            #define MBC89_ANZKONFIGMODUL    69  // Anzahl der modulspez. Konfigurationskanaele
                                                 // (5 klassische + 64 KeyBx-GroupY-Kanaele)

            uint8_t _cs2ScratchRow[MBC_COM_CS2PAKETE * 8];

            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //+ Lok-Datenbank-Zugriff (810 Byte, 10 Byte je der 80 MM-Adressen - siehe            +
            //+ Klassenkommentar in Mbc89Com.cpp fuer das Feldlayout)                             +
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

            //*************************************************************************************
            //* Liefert den Zeiger auf den Datensatz der MM-Adresse index (0...79) innerhalb des  *
            //* Systemarrays.                                                                     *
            //*************************************************************************************
            //*************************************************************************************
            //* index: MM-Adresse 0...79                                                          *
            //*************************************************************************************
            //*************************************************************************************
            //* Return: Zeiger auf den 10-Byte-Datensatz (siehe LOK_*-Feldoffsets in Mbc89Com.cpp)*
            //*************************************************************************************

            uint8_t* lokAt(uint8_t index);

            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //+ System-Stopp/Go (CAN-ID 0x00) und CONNECT6021-Protokoll (CAN-ID 0x44/0x45) - 1:1  +
            //+ aus main()s Empfangsblock uebernommen, siehe Klassenkommentar in Mbc89Com.cpp     +
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

            //*************************************************************************************
            //* System-Stopp/Go auswerten - loest die simulierten CU-Tasten aus (cuTastenOn()),   *
            //* inklusive eines auffaelligen, unveraendert uebernommenen GUiD-Vergleichs (siehe   *
            //* Mbc89Com.cpp).                                                                    *
            //*************************************************************************************
            //*************************************************************************************
            //* msg: empfangene CAN-Nachricht                                                     *
            //*************************************************************************************

            void handleSystemCommand(const uint8_t msg[MBC_CAN_MSG_LEN]);

            //*************************************************************************************
            //* CONNECT6021-Mapping-Zuweisung/-Reset auswerten (DLC=5 Einzelzuweisung, DLC=6      *
            //* Stream-Reset).                                                                    *
            //*************************************************************************************
            //*************************************************************************************
            //* msg: empfangene CAN-Nachricht                                                     *
            //*************************************************************************************

            void handleConnect6021(const uint8_t msg[MBC_CAN_MSG_LEN]);

            //*************************************************************************************
            //* Nach einem CONNECT6021-Wiederverbinden (cuIsReset && cuIsConnected && cuIsPresent,*
            //* 1:1 aus main() uebernommen) alle gemappten Loks einmalig auf Stopp/Vorwaerts/F0-F4*
            //* aus zuruecksetzen - synchronisiert damit die CU nach einem Modul-Reset.           *
            //*************************************************************************************

            void resyncAfterConnect();

            //*************************************************************************************
            //* Richtungs-/Funktions-/Geschwindigkeitsabfragen der CS2 (CAN-ID 0x0A/0x0C/0x08) aus*
            //* der eigenen Lokdatenbank beantworten, wenn eine CU oder ein Gleis-Signal (INFRA)  *
            //* erkannt wurde - sperrt damit die direkte Aenderung durch die CS2 fuer diese Loks. *
            //*************************************************************************************
            //*************************************************************************************
            //* msg: empfangene CAN-Nachricht                                                     *
            //*************************************************************************************

            void handleLokAbfrage(const uint8_t msg[MBC_CAN_MSG_LEN]);

            #if defined(__AVR__)
                uint8_t _pinReset;
            #else
                uint8_t _pinCuScl, _pinCuSda, _pinTrack;

                Mbc89DigitalIn  *_cuStopIn;
                Mbc89DigitalOut *_cuStopOut;
                Mbc89DigitalIn  *_cuGoIn;
                Mbc89DigitalOut *_cuGoOut;
                Mbc89DigitalIn  *_cuExt;
            #endif
    };

//------ Ende Klassendefinition Mbc89Com ----------------------------------------------------------

#endif  /* _MBC89COM_H_ */

//###### Ende Quellcode Mbc89Com.h ################################################################

