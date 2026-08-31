//*************************************************************************************************
//* HEADER - Systemarray, Konvertierung, ADC, internes EEPROM, Ereignisse/LED, 1-Wire (MBCANLib)  *
//*************************************************************************************************
//* Fasst Systemarray/Modulidentitaet, Hex-/String-Konvertierung, internen ADC                    *
//* (Versorgungs-spannung/Steckererkennung), internes EEPROM (Modulidentitaet),                   *
//* Ereignis-/Status-LED-Zeitsteuerung und den 1-Wire-Temperatursensor (DS18S20/DS18B20) in einer *
//* Klasse zusammen. Der externe SPI-Boot-Speicherchip ist als eigene Klasse MbcBootStorage       *
//* ausgelagert - hier verbleibt nur dessen Staging-Flaeche im Systemarray                        *
//* (MBC_S_BOOT/MBC_BOOT_PGSZ/MBC_BOOT_PGMAX), die MbcCom direkt befuellt, bevor sie an           *
//* MbcBootStorage::write() uebergeben wird.                                                      *
//*                                                                                               *
//* Zielprozessoren: ATmega644P (ueber einen Arduino-kompatiblen AVR-Core, z.B. MightyCore),      *
//* RP2040 (Raspberry Pi Pico, RP2040 Zero, ...), ESP32-C3 - einheitlich ueber Arduino-Core-APIs  *
//* (SPI, analogRead, EEPROM.h, digitalWrite, millis). update() muss regelmaessig aufgerufen      *
//* werden (z.B. bei jedem loop()-Durchlauf) und tickt intern im 10-ms-Raster nach (Traffic-/     *
//* Blinkanzeige, Sekunden-/500-ms-/100-ms-Ereigniszaehler).                                      *
//*                                                                                               *
//* RAM-Hinweis: Der ATmega644P hat nur 4 KB SRAM, das Systemarray macht dort je nach Modul einen *
//* erheblichen Teil des Budgets aus. Das Systemarray wird deshalb NICHT von Mbc89Utils selbst    *
//* gehalten, sondern vom Sketch als exakt bemessener globaler/statischer Puffer angelegt und dem *
//* Konstruktor als Zeiger uebergeben (siehe MBC_SYSARRAY_SIZE()/Mbc89Utils::Mbc89Utils()) - jedes*
//* Modul zahlt so nur fuer die Groesse, die es tatsaechlich braucht, keine gemeinsame, im        *
//* Einzelfall ueberdimensionierte Puffergroesse. Keine Heap-Allokation, keine dynamischen Puffer *
//* in den Konvertierungsfunktionen (statische Puffer, nicht rekursions-/threadsicher, aber auf   *
//* einem Single-Thread-Polling-System unproblematisch).                                          *
//*                                                                                               *
//* Die Formel/Schwellwerte in adcReadVspg()/adcReadPlugged() sind auf einen 10-Bit-ADC mit       *
//* 5-V-Referenz (ATmega644P) kalibriert. Auf RP2040/ESP32-C3 (12-Bit-ADC, 3,3-V-Referenz)        *
//* muessen sowohl die Aufloesung (siehe Mbc89Platform::adcConfigure(), in begin() aufgerufen) als*
//* auch die Spannungsteiler-Formel noch gegen die reale Platine verifiziert werden.              *
//*************************************************************************************************
//* Datei: Mbc89Utils.h                                                                           *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen:                                                                                  *
//* V1.0.0 - 02.08.2026 - Nullversion                                                             *
//* V1.1.0 - 06.08.2026 - eepromReadExtra()/eepromWriteExtra() fuer Unterklassen-Parameter        *
//*************************************************************************************************

#pragma once

//###### Start Quellcode Mbc89Utils.h #############################################################

#ifndef _MBC89UTILS_H_
#define _MBC89UTILS_H_     "V1.1.0 - 06.08.2026"

//------ Einbindung Bibliotheken ------------------------------------------------------------------

#include <Arduino.h>
#include "Mbc89DigitalOut.h"
#include "Mbc89DigitalIn.h"

// Abgespeckte, private Kopie von MBCANLib/MbcUtils.h fuer Mbc89Lib (siehe Klassenkommentar
// in Mbc89Com.h fuer den Grund: mbc-89 benoetigt die AVR-TWI-Hardware exklusiv fuer den
// eigenen CU-Bus). setLedDrivers()/setPlugDetector() nutzen dafuer bewusst die SPI-basierten
// Mbc89DigitalOut/Mbc89DigitalIn/Mbc89Mcp23x17 (KEIN MbcI2cBus/Wire-Pfad) - siehe deren
// Klassenkommentare. Default bleibt in jedem Fall der native LED-/Plug-Pin aus dem Konstruktor.

//------ Start globale Deklarationen --------------------------------------------------------------

    //=============================================================================================
    //= Systemarray-Layout (Bytelayout des Modul-Kopfblocks)                                      =
    //=============================================================================================

        #define MBC_ARRAY_START       1
        #define MBC_ALLG_START        MBC_ARRAY_START
        #define MBC_INFO_START        MBC_ARRAY_START

        #define MBC_L_SNR             8
        #define MBC_S_SNR             MBC_ARRAY_START

        #define MBC_L_ART             4
        #define MBC_S_ART             (MBC_L_SNR + MBC_S_SNR)

        #define MBC_L_SW              3
        #define MBC_S_SW              (MBC_L_ART + MBC_S_ART)

        #define MBC_L_HW              6
        #define MBC_S_HW              (MBC_L_SW + MBC_S_SW)

        #define MBC_L_NAMEBLOCK       20
        #define MBC_S_NAMEBLOCK       (MBC_L_HW + MBC_S_HW)

        #define MBC_INFO_ENDE         (MBC_L_NAMEBLOCK + MBC_S_NAMEBLOCK - 1)

        #define MBC_L_GUID            4
        #define MBC_S_GUID            (MBC_L_NAMEBLOCK + MBC_S_NAMEBLOCK)

        #define MBC_L_DB              12
        #define MBC_S_DB              (MBC_L_GUID + MBC_S_GUID)

        #define MBC_L_KN              5
        #define MBC_S_KN              (MBC_L_DB + MBC_S_DB)

        #define MBC_L_PARA            7
        #define MBC_S_PARA            (MBC_L_KN + MBC_S_KN)

        #define MBC_ALLG_ENDE         (MBC_L_PARA + MBC_S_PARA - 1)

        #define MBC_BOOT_START        (MBC_L_PARA + MBC_S_PARA)
        #define MBC_BOOT_PGSZ         256                          // EEPROM-Page-Groesse (Firmware-Update)
        #define MBC_BOOT_PGMAX        (MBC_BOOT_PGSZ / 4)           // Pakete je Page-Uebertragung
        #define MBC_L_BOOT            (2 + MBC_BOOT_PGSZ)
        #define MBC_S_BOOT            MBC_BOOT_START
        #define MBC_BOOT_ENDE         (MBC_L_BOOT + MBC_S_BOOT - 1)

        #define MBC_S_GENPURP         (MBC_BOOT_ENDE + 1)

        // Das Systemarray liegt NICHT mehr fest in Mbc89Utils eingebettet, sondern wird dem
        // Konstruktor als Zeiger auf einen vom Sketch angelegten Puffer uebergeben (siehe
        // Mbc89Utils::Mbc89Utils()) - jedes Modul zahlt damit exakt fuer die Groesse, die es
        // braucht (genPurpSize), statt sich eine fuer alle Module gemeinsame, im unguenstigen
        // Fall ueberdimensionierte Groesse zu teilen. Historie: eine gemeinsame Konstante
        // MBC_ARRAY_MAX_SIZE (zuletzt AVR=1050/RP2040+ESP32-C3=2000 Byte) hatte bei RAM-armen
        // Modulen mit kleinem genPurpSize (z.B. mbc-90, genPurpSize=20, aber 17 eigene
        // CS2-Konfigkanaele mit hohem sonstigen RAM-Bedarf) real gemessen nur noch 738 Byte freien
        // RAM uebriggelassen (Arduino-IDE-Warnung "Wenig Arbeitsspeicher verfuegbar") - mit
        // exakter Puffergroesse pro Modul faellt dieser Verschnitt weg. Sketches legen den Puffer
        // ueber das Makro MBC_SYSARRAY_SIZE(genPurpSize) korrekt bemessen an, siehe
        // Mbc89Utils::Mbc89Utils()-Kommentar und die Beispiel-Sketches.

        #define MBC_SYSARRAY_SIZE(genPurpSize)   ((uint16_t) (MBC_S_GENPURP + (genPurpSize)))

    //=============================================================================================
    //= EEPROM-Blocknummern (intern)                                                              =
    //=============================================================================================

        #define MBC_EE_NAME           0x01
        #define MBC_EE_GUID           0x02
        #define MBC_EE_KENNUNG        0x03
        #define MBC_EE_DB             0x04

    //=============================================================================================
    //= LED-/Blinkmuster-Kennungen - jeweils Zeile,Dauerblinken-Flag                              =
    //=============================================================================================

        #define MBC_LED_PC_OK          1, 0    // Neuanmeldung PC erfolgreich
        #define MBC_LED_CS2_OK         2, 0    // Neuanmeldung CS2 erfolgreich
        #define MBC_LED_IO_OK          3, 0    // Modulzugriff erfolgreich
        #define MBC_LED_BT_OK          4, 0    // BT-Schreiben erfolgreich
        #define MBC_LED_UP_OK          5, 0    // Modulupgrade erfolgreich

        #define MBC_LED_MCP_ERROR      7, 1    // MCP defekt
        #define MBC_LED_SPG_ERROR      8, 1    // Spannung zu niedrig
        #define MBC_LED_FW_ERROR       11, 0   // Firmwarefehler
        #define MBC_LED_BOOT_ERROR     12, 1   // BOOT-Fehler

        #define MBC_LED_NORMAL_START   13, 0   // Modul startet mit interner Firmware
        #define MBC_LED_FW_UPGRADE_P   14, 0   // Firmware-Upgrade - Probelauf
        #define MBC_LED_FW_UPGRADE_S   15, 0   // Firmware-Upgrade - Scharf
        #define MBC_LED_MODKONFIG      18, 1   // Modul konfiguriert sich

    //=============================================================================================
    //= 1-Wire Sensortypen                                                                        =
    //=============================================================================================

        #define MBC_OWI_DS18S20        0x10    // 85 GradC-Typ (Faktor 1 bei Temperaturberechnung)
        #define MBC_OWI_DS18B20        0x28    // 125 GradC-Typ (Faktor 1/8 bei Temperaturberechnung)

//------ Ende globale Deklarationen ---------------------------------------------------------------

//------ Start Klassendefinition Mbc89Utils -------------------------------------------------------

    //* Klasse fuer <Mbc89Utils>
    //* Buendelt Systemarray/Modulidentitaet, Hex-/ASCII-/Dezimal-Konvertierung, internen ADC
    //* (Versorgungsspannung/Steckererkennung), internes EEPROM (Modulidentitaet), Ereignis-/
    //* Status-LED-Zeitsteuerung sowie 1-Wire-Temperatursensor (DS18S20/DS18B20). Das externe
    //* SPI-Boot-Speicherchip ist als eigene Klasse MbcBootStorage ausgelagert.

    class Mbc89Utils
    {
        public:

            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //+ KONSTRUKTOREN                                                                     +
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

                //*********************************************************************************
                //* Konstruktor zur Deklaration von Objektvariablen                               *
                //*********************************************************************************
                //* systemArray:    Zeiger auf den vom Sketch als globales/statisches Array       *
                //* angelegten Systemarray-Puffer - MUSS mindestens                               *
                //* MBC_SYSARRAY_SIZE(genPurpSize) Byte gross sein, z.B.                          *
                //* static uint8_t sysArray[MBC_SYSARRAY_SIZE(977)];                              *
                //* Mbc89Utils utils(sysArray, 977, ...);                                         *
                //* Mbc89Utils uebernimmt KEINEN Besitz (kein delete[] im Destruktor) - der Puffer*
                //* muss die gesamte Laufzeit des Sketches bestehen bleiben (globales/statisches  *
                //* Array, kein lokales/temporaeres).                                             *
                //* genPurpSize:    Groesse des modulspezifischen General-Purpose-Bereichs (Byte) *
                //* pinAdcVspg:     Analogpin fuer die MBCAN-Bus-Versorgungsspannung              *
                //* pinAdcPlug:     Analogpin fuer die Steckernetzteil-Erkennung                  *
                //* pinLedGruen:    Digitalpin der gruenen Status-LED                             *
                //* pinLedRot:      Digitalpin der roten Status-LED                               *
                //* pinOneWire:     Digitalpin des 1-Wire-Bus (DS18S20/DS18B20)                   *
                //* extraEepromSize: Reservierte EEPROM-Bytes fuer eine Unterklasse (Default 0)   *
                //*********************************************************************************

                Mbc89Utils(uint8_t *systemArray, uint16_t genPurpSize, uint8_t pinAdcVspg,
                         uint8_t pinAdcPlug, uint8_t pinLedGruen, uint8_t pinLedRot,
                         uint8_t pinOneWire, uint16_t extraEepromSize = 0);

                //*********************************************************************************
                //* Destruktor - das Systemarray gehoert dem Sketch (siehe Konstruktor), hier     *
                //* nichts freizugeben.                                                           *
                //*********************************************************************************

                ~Mbc89Utils();

            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //+ SYSTEMARRAY / MODULIDENTITAET                                                     +
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

                //*********************************************************************************
                //* Initialisiert das Systemarray: loescht es, setzt die GUiD auf 0xff            *
                //* (unangemeldet), laedt Name/GUiD/Kennung/Datenbank aus dem internen EEPROM     *
                //* (mit Defaultwerten fuer den unbeschriebenen Zustand), initialisiert           *
                //* Ereignis-/LED- und ADC-Subsystem, sucht den 1-Wire-Sensor und traegt          *
                //* Artikel-/Software-/Hardwareversion ein.                                       *
                //*********************************************************************************
                //* artikelnummer: 4 Byte Artikelnummer                                           *
                //* swVersion:     3 Byte Softwareversion                                         *
                //* hwVersion:     6 Byte Hardwareversion                                         *
                //*********************************************************************************

                void begin(const uint8_t artikelnummer[4], const uint8_t swVersion[3],
                           const uint8_t hwVersion[6]);

                //*********************************************************************************
                //* Muss regelmaessig aufgerufen werden (z.B. bei jedem loop()-Durchlauf) und     *
                //* tickt intern im 10-ms-Raster nach (Traffic-/Blinkanzeige, Sekunden-/          *
                //* 500-ms-/100-ms-Ereigniszaehler).                                              *
                //*********************************************************************************

                void update();

            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //+ ZYKLISCHER SERVICE                                                                +
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

                //*********************************************************************************
                //* Zyklischer Service: wertet die Sekunden-Ereignisnummer aus und stoesst        *
                //* Spannungs-/Temperaturmessungen an. In loop() nach update() aufrufen.          *
                //*********************************************************************************

                void check();

                //*********************************************************************************
                //* Blockiert, bis alle Ereignisse einmal durchlaufen wurden (10 s) - z.B. um     *
                //* nach dem Start sicherzustellen, dass alle Statuswerte einmal ermittelt        *
                //* wurden. Tickt update()/check() waehrenddessen selbst weiter.                  *
                //*********************************************************************************

                void waitFilled();

                //*********************************************************************************
                //* Status-LED-Ausgaenge optional auf einen Mbc89DigitalOut umleiten (z.B. ein     *
                //* Mbc89Mcp23x17-Bit, siehe ESP32-C3-Beispiel) statt der nativen pinLedGruen/      *
                //* pinLedRot-Pins aus dem Konstruktor. nullptr (Default) belaesst es beim          *
                //* jeweiligen nativen Pin.                                                         *
                //*********************************************************************************
                //* gruen/rot: Zielausgang oder nullptr fuer den nativen Pin                        *
                //*********************************************************************************

                void setLedDrivers(Mbc89DigitalOut *gruen, Mbc89DigitalOut *rot);

                //*********************************************************************************
                //* Steckernetzteil-Erkennung optional auf einen Mbc89DigitalIn umleiten (z.B. ein  *
                //* Mbc89Mcp23x17-Bit) statt der nativen analogen Auswertung ueber pinAdcPlug aus   *
                //* dem Konstruktor. nullptr (Default) belaesst es bei der nativen Auswertung.      *
                //*********************************************************************************
                //* detector: Zielquelle oder nullptr fuer die native analoge Auswertung            *
                //*********************************************************************************

                void setPlugDetector(Mbc89DigitalIn *detector);

                //*********************************************************************************
                //* Direkter Zugriff auf das Systemarray                                          *
                //*********************************************************************************
                //* Return: Zeiger auf das Systemarray                                            *
                //*********************************************************************************

                uint8_t* array();

                //*********************************************************************************
                //* Groesse des Systemarrays in Byte                                              *
                //*********************************************************************************
                //* Return: Groesse des Systemarrays in Byte                                      *
                //*********************************************************************************

                uint16_t arrayMax() const;

            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //+ KONVERTIERUNG                                                                     +
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

                //*********************************************************************************
                //* Wandle Hex-ASCII-Ziffernfolge in eine Dezimalnummer um                        *
                //*********************************************************************************
                //* ascii: Buffer mit ASCII-Hex-Ziffern                                           *
                //* num:   Anzahl der auszuwertenden Ziffern                                      *
                //*********************************************************************************
                //* Return: Umgewandelter Wert                                                    *
                //*********************************************************************************

                static uint16_t hex2num(const uint8_t *ascii, uint8_t num);

                //*********************************************************************************
                //* Wandle eine Zahl (0...999) in einen rechtsbuendigen 3-stelligen String um     *
                //*********************************************************************************
                //* num: umzuwandelnder Wert                                                      *
                //*********************************************************************************
                //* Return: Zeiger auf internen statischen Buffer (nicht                          *
                //* rekursions-/threadsicher)                                                     *
                //*********************************************************************************

                static char* num2string(uint16_t num);

                //*********************************************************************************
                //* Wandle eine Zahl in einen HEX-String mit fester Stellenzahl um                *
                //*********************************************************************************
                //* num:     umzuwandelnder Wert                                                  *
                //* stellen: Anzahl der HEX-Nibble (2, 3 oder 4)                                  *
                //*********************************************************************************
                //* Return: Zeiger auf internen statischen Buffer (nicht                          *
                //* rekursions-/threadsicher)                                                     *
                //*********************************************************************************

                static char* num2hexstring(uint16_t num, uint8_t stellen);

                //*********************************************************************************
                //* Wandle 4-Bit-HEX-Wert in ASCII um                                             *
                //*********************************************************************************
                //* digit: 4-Bit-Wert auf HEX-Basis 0x0...0xf                                     *
                //*********************************************************************************
                //* Return: ASCII-Zeichen (0...9 und A...F)                                       *
                //*********************************************************************************

                static uint8_t hex2ascii(uint8_t digit);

                //*********************************************************************************
                //* Wandle ASCII in 4-Bit-HEX-Wert um                                             *
                //*********************************************************************************
                //* digit: ASCII-Zeichen (0...9 und A...F)                                        *
                //*********************************************************************************
                //* Return: Wert auf HEX-Basis 0x0...0xf                                          *
                //*********************************************************************************

                static uint8_t ascii2hex(uint8_t digit);

            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //+ INTERNES EEPROM (Modulidentitaet)                                                 +
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

                //*********************************************************************************
                //* Liest Name/GUiD/Kennung/Datenbank aus dem internen EEPROM in das Systemarray  *
                //* (mit Defaultwerten fuer den unbeschriebenen Zustand)                          *
                //*********************************************************************************

                void eepromRead();

                //*********************************************************************************
                //* Schreibt einen Block des Systemarrays in das interne EEPROM                   *
                //*********************************************************************************
                //* block: MBC_EE_NAME / MBC_EE_GUID / MBC_EE_KENNUNG / MBC_EE_DB                 *
                //*********************************************************************************

                void eepromWrite(uint8_t block);

                //*********************************************************************************
                //* Liest Bytes aus der Zusatzregion des internen EEPROM (siehe extraEepromSize   *
                //* im Konstruktor) - fuer beliebige Unterklassen-Parameter, die einen            *
                //* Stromausfall ueberleben muessen (z.B. Aktorstellung, Basisadresse).           *
                //*********************************************************************************
                //* offset: Byte-Offset innerhalb der Zusatzregion (0...extraEepromSize-1)        *
                //* buffer: Zielpuffer (mind. len Byte)                                           *
                //* len:    Anzahl zu lesender Byte                                               *
                //*********************************************************************************
                //* Return: false, wenn offset+len ausserhalb der reservierten Zusatzregion       *
                //* liegt                                                                         *
                //*********************************************************************************

                bool eepromReadExtra(uint16_t offset, uint8_t *buffer, uint16_t len);

                //*********************************************************************************
                //* Schreibt Bytes in die Zusatzregion des internen EEPROM (siehe                 *
                //* eepromReadExtra())                                                            *
                //*********************************************************************************
                //* offset: Byte-Offset innerhalb der Zusatzregion (0...extraEepromSize-1)        *
                //* buffer: Quellpuffer (mind. len Byte)                                          *
                //* len:    Anzahl zu schreibender Byte                                           *
                //*********************************************************************************
                //* Return: false, wenn offset+len ausserhalb der reservierten Zusatzregion       *
                //* liegt                                                                         *
                //*********************************************************************************

                bool eepromWriteExtra(uint16_t offset, const uint8_t *buffer, uint16_t len);

            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //+ EREIGNISSE / STATUS-LED                                                           +
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

                //*********************************************************************************
                //* LED-Blinken einschalten                                                       *
                //*********************************************************************************
                //* muster: Zeile in der LED-Blinkmustertabelle (siehe MBC_LED_*-Konstanten)      *
                //* block:  0 = einmalig, 1 = kontinuierlich                                      *
                //*********************************************************************************

                void ledToggleOn(uint8_t muster, uint8_t block);

                //*********************************************************************************
                //* LED-Blinken ausschalten, LED zeigt danach GRUEN                               *
                //*********************************************************************************

                void ledToggleOff();

                //*********************************************************************************
                //* Traffic-Anzeige anfordern (kurzes ORANGE-Aufblitzen, wenn kein Blinkmuster    *
                //* aktiv ist)                                                                    *
                //*********************************************************************************

                void ledTraffic();

                //*********************************************************************************
                //* Sekunden-Ereignis abfragen (liest und loescht das Flag)                       *
                //*********************************************************************************
                //* Return: true, wenn seit dem letzten Abfragen eine Sekunde vergangen ist       *
                //*********************************************************************************

                bool eventSekunde();

                //*********************************************************************************
                //* 500-ms-Ereignis abfragen (liest und loescht das Flag)                         *
                //*********************************************************************************
                //* Return: true, wenn seit dem letzten Abfragen 500 ms vergangen sind            *
                //*********************************************************************************

                bool event500ms();

                //*********************************************************************************
                //* 100-ms-Ereignis abfragen (liest und loescht das Flag)                         *
                //*********************************************************************************
                //* Return: true, wenn seit dem letzten Abfragen 100 ms vergangen sind            *
                //*********************************************************************************

                bool event100ms();

                //*********************************************************************************
                //* Aktuelle Ereignisnummer (0...9) abfragen, wird jede Sekunde hochgezaehlt      *
                //*********************************************************************************
                //* Return: Ereignisnummer 0...9                                                  *
                //*********************************************************************************

                uint8_t eventNummer();

            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //+ 1-WIRE-TEMPERATURSENSOR                                                           +
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

                //*********************************************************************************
                //* Ist ein 1-Wire-Sensor gefunden worden? (Ergebnis von begin())                 *
                //*********************************************************************************
                //* Return: true, wenn beim begin() ein 1-Wire-Sensor gefunden wurde              *
                //*********************************************************************************

                bool owiPresent() const;

                //*********************************************************************************
                //* Zuletzt gemessene Temperatur in GradC (nur gueltig, wenn owiPresent())        *
                //*********************************************************************************
                //* Return: Temperatur in GradC                                                   *
                //*********************************************************************************

                float owiTemperature() const;

        private:

            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //+ VARIABLEN                                                                         +
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

                uint8_t      *_array;      // vom Sketch uebergebener Puffer, siehe Konstruktor
                uint16_t     _arrayMax;
                uint16_t     _extraEepromSize;

                uint8_t      _pinAdcVspg;
                uint8_t      _pinAdcPlug;
                uint8_t      _pinLedGruen;
                uint8_t      _pinLedRot;
                uint8_t      _pinOneWire;

                // Optionale Umleitung auf Mbc89DigitalOut/Mbc89DigitalIn statt der nativen Pins
                // oben (siehe setLedDrivers()/setPlugDetector()) - nullptr = nativer Pin

                Mbc89DigitalOut *_ledDriverGruen;
                Mbc89DigitalOut *_ledDriverRot;
                Mbc89DigitalIn  *_plugDetector;

                // Ereignisse/LED

                unsigned long _lastTickMs;
                uint8_t       _eventCnt10ms;     // 0..9, ein Schritt je tick10ms() (=100 ms)
                uint16_t      _trafficCnt;
                uint16_t      _blinktaktCnt;
                uint16_t      _blinkbildCnt;
                uint16_t      _eventCnt100ms;

                bool          _fReady;
                bool          _fSekunde;
                bool          _f500ms;
                bool          _f100ms;
                uint8_t       _eventNummer;

                bool          _ledTraffic;
                bool          _ledBlinken;
                bool          _ledBlinkenContinuous;
                bool          _ledBlinkenNeustart;
                uint8_t       _ledBlinkenMuster;

                static const uint8_t PATTERN[20][13] PROGMEM;   // LED-Blinkmustertabelle

                // ADC (ohne Busy-Flags, da analogRead() synchron ist)

                bool          _spgHurdle;        // Hysterese fuer Unterspannungs-LED-Meldung

                bool          _owiPresent;
                uint8_t       _owiType;
                float         _owiValue;

                #if defined(__AVR__)
                    // Rohe Portregister fuer den 1-Wire-Pin (statt digitalWrite()/digitalRead()/
                    // pinMode()), einmalig im Konstruktor aufgeloest - siehe owiReset()/owiBitIo()
                    // in Mbc89Utils.cpp fuer den Hintergrund

                    volatile uint8_t *_owiDdr;
                    volatile uint8_t *_owiPort;
                    volatile uint8_t *_owiPinReg;
                    uint8_t            _owiMask;
                #endif

            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //+ INTERNE FUNKTIONEN                                                                +
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

                void tick10ms();
                void setLed(uint8_t colorBits);

                void adcReadVspg();
                void adcReadPlugged();

                // 1-Wire-Grundfunktionen

                bool owiReset();
                uint8_t owiBitIo(uint8_t b);
                uint8_t owiByteWr(uint8_t b);
                uint8_t owiByteRd();
                uint8_t owiRomSearch(uint8_t diff, uint8_t *id);
                bool owiInit();
                void owiStartTempMeasurement();
                void owiReadTemperature();
    };

//------ Ende Klassendefinition Mbc89Utils --------------------------------------------------------

#endif  /* _MBC89UTILS_H_ */

//###### Ende Quellcode Mbc89Utils.h ##############################################################
