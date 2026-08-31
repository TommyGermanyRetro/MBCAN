//*************************************************************************************************
//* HEADER - Modul mbc-93: mfx-Booster-Monitor auf Basis Maerklin 6017 (Mbc93Lib)                 *
//*************************************************************************************************
//* mbc-93 ist kein eigener Schaltdecoder, sondern ein Monitor-/Steuermodul fuer einen externen,  *
//* analogen Booster (mfx-faehig, Maerklin-6017-Basis): steuert ihn ueber 2 Ausgangsbits (Ein/Aus *
//* + Kurzschluss-Signalisierung, 3 gueltige Zustaende), erkennt einen Kurzschluss ueber einen    *
//* latenzgemessenen Eingang (steigende Flanke startet die Latenzzeit, eine fallende Flanke vor   *
//* Ablauf bricht sie ab - der kurze mfx-Quasi-Kurzschluss beim Rueckmeldezyklus wird so nicht    *
//* als echter Kurzschluss gewertet) und meldet einen echten Kurzschluss per spontanem System-    *
//* Stopp-Telegramm (universelle Broadcast-GUiD) an die CSx. Regelt zusaetzlich einen Luefter     *
//* temperaturabhaengig ueber den vorhandenen 1-Wire-Sensor, simuliert eine GFP/Booster-          *
//* Softwareversion (per CS2-Konfigkanal Major/Minor einstellbar, analog Mbc80Com fuer L88), um   *
//* eine Updateanforderung der CSx zu vermeiden, und meldet sich mit der Maerklin-fest-           *
//* vorgegebenen Booster-Kennung 0x0000 an (kennerFixedExtern()). Verarbeitet ausserdem           *
//* eingehende System-Stopp-/Go-Telegramme (CAN-ID 0x00) direkt in update(), unabhaengig von der  *
//* PC-/CS2-Dispatch-Logik der Basisklasse - dieser Teil (inkl. eines auffaelligen, aber          *
//* unveraendert uebernommenen GUiD-Vergleichs) ist 1:1 aus der Original-Firmware portiert, siehe *
//* Mbc93Com.cpp.                                                                                 *
//*************************************************************************************************
//* Datei: Mbc93Com.h                                                                             *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen:                                                                                  *
//* V1.0.0 - 23.08.2026 - Nullversion                                                             *
//* V1.0.1 - 29.08.2026 - RAM-Optimierung                                                         *
//*************************************************************************************************

#pragma once

//###### Start Quellcode Mbc93Com.h ################################################################

#ifndef _MBC93COM_H_
#define _MBC93COM_H_     "V1.0.1 - 29.08.2026"

//------ Einbindung Bibliotheken -------------------------------------------------------------------

#include <Arduino.h>
#include <MbcCom.h>
#include <MbcDigitalOut.h>

//------ Start Klassendefinition Mbc93Com ----------------------------------------------------------

    //* Klasse fuer <Mbc93Com>
    //* Modulspezifische MbcCom-Unterklasse fuer mbc-93. pinOn/pinShort sind die 2 Ausgangsbits zum
    //* Booster (Original: PORTD Bit0/Bit1, 3 gueltige Kombinationen OFF_OK/ON_OK/OFF_SHORT),
    //* pinShortDetect ist der Kurzschluss-Erkennungseingang (ehem. INT1, hier per
    //* attachInterrupt()+millis()-Latenzmessung statt roher INT1+TIMER1-Register - siehe
    //* shortIsr()/shortCheck()), pinFan der native PWM-Pin des Luefters (ehem. OC2A, hier per
    //* analogWrite() statt roher Timer2-PWM).

    class Mbc93Com : public MbcCom
    {
        public:

            //*************************************************************************************
            //* Konstruktor zur Deklaration von Objektvariablen                                   *
            //*************************************************************************************
            //* utils:         zugehoerige MbcUtils-Instanz                                       *
            //* can:           zugehoeriger CAN-Kanal dieses Moduls (Basisklassen-Parameter can)  *
            //* bootStorage:   zugehoerige MbcBootStorage-Instanz (Firmware-Update-Staging)       *
            //* pinOn:         Ausgangsbit Booster an (Original PORTD Bit0)                       *
            //* pinShort:      Ausgangsbit Kurzschluss signalisieren (Original PORTD Bit1)        *
            //* pinShortDetect: nativer, interruptfaehiger Digitalpin der Kurzschlusserkennung    *
            //* pinFan:        nativer PWM-Pin des Luefters                                       *
            //* pinReset:      NUR auf AVR (ATmega644P) vorhanden, siehe MbcCom-Konstruktor       *
            //*************************************************************************************

            #if defined(__AVR__)
                Mbc93Com(MbcUtils &utils, MbcCan &can, MbcBootStorage &bootStorage,
                         MbcDigitalOut &pinOn, MbcDigitalOut &pinShort, uint8_t pinShortDetect,
                         uint8_t pinFan, uint8_t pinReset);
            #else
                Mbc93Com(MbcUtils &utils, MbcCan &can, MbcBootStorage &bootStorage,
                         MbcDigitalOut &pinOn, MbcDigitalOut &pinShort, uint8_t pinShortDetect,
                         uint8_t pinFan);
            #endif

            //*************************************************************************************
            //* Einmalig nach dem begin() der Basisklasse aufrufen: Ausgangsbit-Pins/Luefter-Pin  *
            //* konfigurieren, Kurzschluss-Interrupt scharfschalten, gespeicherte Parameter aus   *
            //* der EEPROM-Zusatzregion laden.                                                    *
            //*************************************************************************************

            void beginModule();

            //*************************************************************************************
            //* Je loop()-Durchlauf aufrufen: Kurzschluss-Latenzzeit pruefen, Luefter             *
            //* nachfuehren, CAN-Nachrichtenempfang/-Auswertung (ruft intern auch die geerbte     *
            //* check() auf).                                                                     *
            //*************************************************************************************

            void update();

        protected:

            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //+ Die 7 Erweiterungspunkte der Basisklasse (siehe MbcCom.h) - bei mbc-93 haben      +
            //+ eepromReadParaExtern()/eepromWriteParaExtern()/hashInitExtern()/swInitExtern()/   +
            //+ cs2IndexMesswerteExtern()/cs2IndexParameterExtern() echte Logik (Softwareversion, +
            //+ Kurzschluss-Latenzzeit, ON-Track-/Kurzschluss-Messwerte), eepromEraseParaExtern() +
            //+ bleibt leer (Original hat ebenfalls nichts zu loeschen)                           +
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

                //*********************************************************************************
                //* Laedt Softwareversion (Default 3/85) und Kurzschluss-Latenzzeit (Default 0,   *
                //* d.h. 48 ms) aus der EEPROM-Zusatzregion, 0xff-Sentinel fuer den               *
                //* unbeschriebenen Zustand.                                                      *
                //*********************************************************************************

                void eepromReadParaExtern() override;

                //*********************************************************************************
                //* Persistiert Softwareversion bzw. Kurzschluss-Latenzzeit in der EEPROM-        *
                //* Zusatzregion, je nach block getrennt (Original: EE_93_SW/EE_93_SH).           *
                //*********************************************************************************
                //* block: Identifier des zu schreibenden Blocks (0x0A/0x0B)                      *
                //*********************************************************************************

                void eepromWriteParaExtern(uint8_t block) override;

                //*********************************************************************************
                //* mbc-93 hat nichts zu loeschen. Default: nichts zu tun.                        *
                //*********************************************************************************

                void eepromEraseParaExtern() override;

                //*********************************************************************************
                //* Traegt GUiD/HASH in die 3 modulspezifischen Antwort-/Bestaetigungsnachrichten *
                //* ein (_msgBoosterStopResponse/_msgBoosterGoResponse/_msgKonfigOk). Das         *
                //* spontane Kurzschluss-Stopp-Telegramm _msgBoosterStop bekommt bewusst NUR den  *
                //* HASH - seine GUiD bleibt die universelle Broadcast-GUiD 0x00000000            *
                //* (Originalverhalten, 1:1 uebernommen).                                         *
                //*********************************************************************************

                void hashInitExtern() override;

                //*********************************************************************************
                //* Setzt die simulierte GFP/Booster-Softwareversion in die CS2-Ping-Antwort.     *
                //*********************************************************************************

                void swInitExtern() override;

                //*********************************************************************************
                //* Modulspezifischen Messwert an die CS2 senden (ON Track oder Kurzschluss,      *
                //* jeweils 0/1).                                                                 *
                //*********************************************************************************
                //* kanal: Messkanalnummer                                                        *
                //*********************************************************************************

                void cs2IndexMesswerteExtern(uint8_t kanal) override;

                //*********************************************************************************
                //* Modulspezifischen Parameterwert von der CS2 uebernehmen: msg[D5]==Kanal der   *
                //* simulierten SW-Version (Major/Minor, je Slider 1...255) oder der Kurzschluss- *
                //* Latenzzeit (Slider 1...20, msg[D6..D7]=16-Bit-Rohwert) - gegen die jeweils    *
                //* gueltigen Grenzen pruefen (Latenzzeit gegen dieselben wie im ParaCenter       *
                //* nud_mbc93_Latenz, SW-Version analog Mbc80Com), persistieren und der CSx       *
                //* bestaetigen. Ein Wert ausserhalb bleibt ohne Wirkung und wird per FALSE       *
                //* quittiert.                                                                    *
                //*********************************************************************************
                //* msg: CAN-Nachricht (msg[D5]==Kanal, msg[D6..D7]=neuer Rohwert)                *
                //*********************************************************************************

                void cs2IndexParameterExtern(const uint8_t msg[MBC_COM_MSG_LAENGE]) override;

            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //+ Zusaetzliche, mbc-93-spezifische Erweiterungspunkte (siehe Kopfkommentar von      +
            //+ MbcCom.h)                                                                         +
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

                //*********************************************************************************
                //* Liefert die feste CS2-Geraetekennung 0x0000 (Maerklin-Vorgabe fuer Booster-   *
                //* Geraete wie mbc-93 - im Original-Quellcode als Kommentar dokumentiert, aber   *
                //* durch eine ueberschriebene Zuweisung real auf 0x0020 kompiliert; hier bewusst *
                //* der dokumentierte, beabsichtigte Wert 0x0000 verwendet, nicht der fehlerhaft  *
                //* kompilierte).                                                                 *
                //*********************************************************************************
                //* knH/knL: gesetzt auf die feste Kennung, wenn Return true                      *
                //*********************************************************************************
                //* Return: true (mbc-93 hat immer eine feste Kennung)                            *
                //*********************************************************************************

                bool kennerFixedExtern(uint8_t &knH, uint8_t &knL) override;

                //*********************************************************************************
                //* Baut die angeforderte Kanalzeile (2 Messwertkanaele + 3 Konfigkanaele) frisch *
                //* in _cs2ScratchRow zusammen (PROGMEM-Vorlage + aktueller Wert aus dem          *
                //* Systemarray, Messwertkanaele unveraendert - deren Live-Wert geht ueber        *
                //* cs2IndexMesswerteExtern() als eigene Nachricht raus) und gibt sie zurueck,     *
                //* sonst nullptr, wenn laufendeNr keinen Kanal mehr bezeichnet.                  *
                //*********************************************************************************
                //* laufendeNr: 0..ANZMESSWERTEMODUL+ANZKONFIGMODUL-1                             *
                //*********************************************************************************
                //* Return: Zeiger auf _cs2ScratchRow (nur bis zum naechsten Aufruf gueltig)      *
                //*         oder nullptr                                                          *
                //*********************************************************************************

                const uint8_t* cs2IndexExtern(uint8_t laufendeNr) override;

        private:

            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //+ Modulspezifische CSx-GUI-Parameter (ANZMESSWERTEMODUL=2, ANZKONFIGMODUL=3,        +
            //+ cs2_index_ex[]): Zeile 0=ON-Track-Messwert, Zeile 1=Kurzschluss-Messwert, Zeile   +
            //+ 2=SW-Major-Konfigkanal, Zeile 3=SW-Minor-Konfigkanal (beide Slider 1...255,       +
            //+ simulierte GFP/Booster-Softwareversion, analog Mbc80Com fuer L88), Zeile          +
            //+ 4=KS-x-50-ms-Konfigkanal (Slider 1...20) - siehe cs2IndexParameterExtern(). Die   +
            //+ Kanaldefinition (initCs2Ex[], PROGMEM) bleibt komplett im Flash -                 +
            //+ cs2IndexExtern() baut die angeforderte Zeile bei jedem Aufruf frisch in           +
            //+ _cs2ScratchRow zusammen statt 5 dauerhafte 40-Byte-Zeilen im RAM zu halten.        +
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

            #define MBC93_ANZMESSWERTEMODUL 2   // Anzahl der modulspez. Messwerte
            #define MBC93_ANZKONFIGMODUL    3   // Anzahl der modulspez. Konfigurationskanaele

            uint8_t _cs2ScratchRow[MBC_COM_CS2PAKETE * 8];

            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //+ Erweiterung der CAN-Nachrichten                                                   +
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

            uint8_t _msgBoosterStop[MBC_COM_MSG_LAENGE];         // spontanes Kurzschluss-Stopp
            uint8_t _msgBoosterStopResponse[MBC_COM_MSG_LAENGE]; // Antwort auf System-Stopp
            uint8_t _msgBoosterGoResponse[MBC_COM_MSG_LAENGE];   // Antwort auf System-Go
            uint8_t _msgKonfigOk[MBC_COM_MSG_LAENGE];            // Bestaetigung Konfigwert uebernommen

            MbcDigitalOut *_pinOn;
            MbcDigitalOut *_pinShort;
            uint8_t        _pinShortDetect;
            uint8_t        _pinFan;

            bool _boosterOn;   // Original MBC_93_ON

            // Kurzschluss-Latenzmessung (Ersatz fuer INT1+TIMER1, siehe Klassenkommentar) -
            // volatile, da von shortIsr() (Interruptkontext) gesetzt und von shortCheck()
            // (Hauptschleife) gelesen

            static Mbc93Com        *_instance;   // fuer die statische ISR-Trampolinfunktion
            volatile bool           _shortArmed;
            volatile unsigned long  _shortStartMs;

            static void shortIsr();

            void shortCheck();
            void fanUpdate();
            void handleSystemCommand(const uint8_t msg[MBC_CAN_MSG_LEN]);
    };

//------ Ende Klassendefinition Mbc93Com -----------------------------------------------------------

#endif  /* _MBC93COM_H_ */

//###### Ende Quellcode Mbc93Com.h #################################################################

