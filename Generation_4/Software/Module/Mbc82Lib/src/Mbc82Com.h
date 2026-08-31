//*************************************************************************************************
//* HEADER - Modul mbc-82: mfx-Boosterlink (Mbc82Lib)                                             *
//*************************************************************************************************
//* mbc-82 ist kein eigener Schaltdecoder, sondern ein Monitor-/Steuermodul fuer einen externen,  *
//* analogen Booster (mfx-faehig): steuert ihn ueber 2 Ausgangsbits (Ein/Aus + Kurzschluss-       *
//* Signalisierung, 3 gueltige Zustaende), erkennt einen Kurzschluss ueber einen latenzgemessenen *
//* Eingang (steigende Flanke startet die Latenzzeit, eine fallende Flanke vor Ablauf bricht sie  *
//* ab - der kurze mfx-Quasi-Kurzschluss beim Rueckmeldezyklus wird so nicht als echter           *
//* Kurzschluss gewertet) und meldet einen echten Kurzschluss per spontanem System-Stopp-         *
//* Telegramm (universelle Broadcast-GUiD) an die CSx. Simuliert eine GFP/Booster-Softwareversion *
//* (per CS2-Konfigkanal Major/Minor einstellbar, analog Mbc80Com fuer L88), um eine              *
//* Updateanforderung der CSx zu vermeiden, und meldet sich mit der Maerklin-fest-vorgegebenen    *
//* Booster-Kennung 0x0000 an (kennerFixedExtern()). Verarbeitet ausserdem eingehende System-     *
//* Stopp-/Go-Telegramme (CAN-ID 0x00) direkt in update(), unabhaengig von der PC-/CS2-Dispatch-  *
//* Logik der Basisklasse - dieser Teil (inkl. eines auffaelligen, aber unveraendert              *
//* uebernommenen GUiD-Vergleichs) ist 1:1 aus der Original-Firmware portiert, siehe              *
//* Mbc82Com.cpp. Im Unterschied zu Mbc93Lib (dem baugleichen zweiten Booster-Modul dieses        *
//* Projekts) hat mbc-82 KEINEN Luefter.                                                          *
//*************************************************************************************************
//* Datei: Mbc82Com.h                                                                             *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen:                                                                                  *
//* V1.0.0 - 26.08.2026 - Nullversion                                                             *
//* V1.0.1 - 29.08.2026 - RAM-Optimierung                                                         *
//*************************************************************************************************

#pragma once

//###### Start Quellcode Mbc82Com.h ################################################################

#ifndef _MBC82COM_H_
#define _MBC82COM_H_     "V1.0.1 - 29.08.2026"

//------ Einbindung Bibliotheken --------------------------------------------------------------------

#include <Arduino.h>
#include <MbcCom.h>
#include <MbcDigitalOut.h>

//------ Start Klassendefinition Mbc82Com ----------------------------------------------------------

    //* Klasse fuer <Mbc82Com>
    //* Modulspezifische MbcCom-Unterklasse fuer mbc-82. pinOn/pinShort sind die 2 Ausgangsbits zum
    //* Booster (Original: PORTD Bit0/Bit1, 3 gueltige Kombinationen OFF_OK/ON_OK/OFF_SHORT),
    //* pinShortDetect ist der Kurzschluss-Erkennungseingang (ehem. INT1, hier per
    //* attachInterrupt()+millis()-Latenzmessung statt roher INT1+TIMER1-Register - siehe
    //* shortIsr()/shortCheck()). Kein Luefter-Pin (im Unterschied zu Mbc93Com).

    class Mbc82Com : public MbcCom
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
            //* pinReset:      NUR auf AVR (ATmega644P) vorhanden, siehe MbcCom-Konstruktor       *
            //*************************************************************************************

            #if defined(__AVR__)
                Mbc82Com(MbcUtils &utils, MbcCan &can, MbcBootStorage &bootStorage,
                         MbcDigitalOut &pinOn, MbcDigitalOut &pinShort, uint8_t pinShortDetect,
                         uint8_t pinReset);
            #else
                Mbc82Com(MbcUtils &utils, MbcCan &can, MbcBootStorage &bootStorage,
                         MbcDigitalOut &pinOn, MbcDigitalOut &pinShort, uint8_t pinShortDetect);
            #endif

            //*************************************************************************************
            //* Einmalig nach dem begin() der Basisklasse aufrufen: Ausgangsbit-Pins              *
            //* konfigurieren, Kurzschluss-Interrupt scharfschalten, gespeicherte Parameter aus   *
            //* der EEPROM-Zusatzregion laden.                                                    *
            //*************************************************************************************

            void beginModule();

            //*************************************************************************************
            //* Je loop()-Durchlauf aufrufen: Kurzschluss-Latenzzeit pruefen, CAN-                *
            //* Nachrichtenempfang/-Auswertung (ruft intern auch die geerbte check() auf).        *
            //*************************************************************************************

            void update();

        protected:

            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //+ Die 7 Erweiterungspunkte der Basisklasse (siehe MbcCom.h) - bei mbc-82 haben      +
            //+ eepromReadParaExtern()/eepromWriteParaExtern()/hashInitExtern()/swInitExtern()/   +
            //+ cs2IndexMesswerteExtern()/cs2IndexParameterExtern() echte Logik (Softwareversion, +
            //+ Kurzschluss-Latenzzeit, ON-Track-/Kurzschluss-Messwerte), eepromEraseParaExtern() +
            //+ bleibt leer (Original hat ebenfalls nichts zu loeschen)                           +
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

                //*********************************************************************************
                //* Laedt Softwareversion (Default 3/81) und Kurzschluss-Latenzzeit (Default 0,   *
                //* d.h. 48 ms) aus der EEPROM-Zusatzregion, 0xff-Sentinel fuer den               *
                //* unbeschriebenen Zustand.                                                      *
                //*********************************************************************************

                void eepromReadParaExtern() override;

                //*********************************************************************************
                //* Persistiert Softwareversion bzw. Kurzschluss-Latenzzeit in der EEPROM-        *
                //* Zusatzregion, je nach block getrennt (Original: EE_82_SW/EE_82_SH).           *
                //*********************************************************************************
                //* block: Identifier des zu schreibenden Blocks (0x0A/0x0B)                      *
                //*********************************************************************************

                void eepromWriteParaExtern(uint8_t block) override;

                //*********************************************************************************
                //* mbc-82 hat nichts zu loeschen. Default: nichts zu tun.                        *
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
                //* nud_mbc82_Latenz, SW-Version analog Mbc80Com), persistieren und der CSx       *
                //* bestaetigen. Ein Wert ausserhalb bleibt ohne Wirkung und wird per FALSE       *
                //* quittiert.                                                                    *
                //*********************************************************************************
                //* msg: CAN-Nachricht (msg[D5]==Kanal, msg[D6..D7]=neuer Rohwert)                *
                //*********************************************************************************

                void cs2IndexParameterExtern(const uint8_t msg[MBC_COM_MSG_LAENGE]) override;

            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //+ Zusaetzliche, mbc-82-spezifische Erweiterungspunkte (siehe Kopfkommentar von      +
            //+ MbcCom.h)                                                                         +
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

                //*********************************************************************************
                //* Liefert die feste CS2-Geraetekennung 0x0000 (Maerklin-Vorgabe fuer Booster-   *
                //* Geraete wie mbc-82 - im Original-Quellcode als Kommentar dokumentiert, aber   *
                //* durch eine ueberschriebene Zuweisung real auf 0x0020 kompiliert; hier bewusst *
                //* der dokumentierte, beabsichtigte Wert 0x0000 verwendet, nicht der fehlerhaft  *
                //* kompilierte - dieselbe Entscheidung wie bereits fuer mbc-93 getroffen).       *
                //*********************************************************************************
                //* knH/knL: gesetzt auf die feste Kennung, wenn Return true                      *
                //*********************************************************************************
                //* Return: true (mbc-82 hat immer eine feste Kennung)                            *
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

            #define MBC82_ANZMESSWERTEMODUL 2   // Anzahl der modulspez. Messwerte
            #define MBC82_ANZKONFIGMODUL    3   // Anzahl der modulspez. Konfigurationskanaele

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

            bool _boosterOn;   // Original MBC_82_ON

            // Kurzschluss-Latenzmessung (Ersatz fuer INT1+TIMER1, siehe Klassenkommentar) -
            // volatile, da von shortIsr() (Interruptkontext) gesetzt und von shortCheck()
            // (Hauptschleife) gelesen

            static Mbc82Com        *_instance;   // fuer die statische ISR-Trampolinfunktion
            volatile bool           _shortArmed;
            volatile unsigned long  _shortStartMs;

            static void shortIsr();

            void shortCheck();
            void handleSystemCommand(const uint8_t msg[MBC_CAN_MSG_LEN]);
    };

//------ Ende Klassendefinition Mbc82Com -----------------------------------------------------------

#endif  /* _MBC82COM_H_ */

//###### Ende Quellcode Mbc82Com.h #################################################################

