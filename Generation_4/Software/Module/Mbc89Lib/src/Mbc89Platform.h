//*************************************************************************************************
//* HEADER - Plattformabstraktion ATmega644P/RP2040/ESP32-C3 (MBCANLib)                           *
//*************************************************************************************************
//* Buendelt alle Stellen, an denen sich die drei Zielprozessoren in einem WERT oder VERHALTEN    *
//* unterscheiden (EEPROM-Emulation, ADC-Aufloesung, Software-Reset, Watchdog-Timer), hinter      *
//* einer einzigen, plattformneutralen Aufrufschnittstelle - vergleichbar mit einem BIOS.         *
//* MbcUtils, MbcCom & Co. rufen nur noch Mbc89Platform::xxx() auf; das jeweilige #if             *
//* defined(__AVR__/ESP32/ARDUINO_ARCH_RP2040) steckt ausschliesslich noch in Mbc89Platform.cpp.  *
//*                                                                                               *
//* Nicht hierueber abgebildet sind STRUKTURELLE Unterschiede (z.B. der TWAI- statt               *
//* MCP2515-CAN-Konstruktor von MbcCan auf ESP32, oder der zusaetzliche                           *
//* pinReset-Konstruktorparameter von MbcCom auf AVR) - eine zur Laufzeit befragte Klasse kann    *
//* keine unterschiedlichen Konstruktorsignaturen oder Klassenmember erzeugen, das bleibt         *
//* weiterhin ein schlankes #ifdef an der jeweiligen Deklaration.                                 *
//*************************************************************************************************
//* Datei: Mbc89Platform.h                                                                        *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen:                                                                                  *
//* V1.0.0 - 05.08.2026 - Nullversion                                                             *
//*************************************************************************************************

#pragma once

//###### Start Quellcode Mbc89Platform.h ##########################################################

#ifndef _MBC89PLATFORM_H_
#define _MBC89PLATFORM_H_     "V1.0.0 - 05.08.2026"

//------ Einbindung Bibliotheken ------------------------------------------------------------------

#include <Arduino.h>

//------ Start Klassendefinition Mbc89Platform ----------------------------------------------------

    //* Klasse fuer <Mbc89Platform>
    //* Rein statische Klasse (kein Objekt anzulegen) - das "BIOS" der Bibliothek.

    class Mbc89Platform
    {
        public:

            Mbc89Platform() = delete;

            //*************************************************************************************
            //* Internes EEPROM vor dem ersten Zugriff reservieren                                *
            //*************************************************************************************
            //* Auf ESP32/RP2040 emuliert EEPROM.h den Speicher im Flash und muss vor dem ersten  *
            //* read()/write() mit begin(size) reserviert werden. Der ATmega644P hat echtes       *
            //* EEPROM und braucht das nicht (No-Op).                                             *
            //*************************************************************************************
            //* size: benoetigte Groesse in Byte                                                  *
            //*************************************************************************************

            static void eepromBegin(size_t size);

            //*************************************************************************************
            //* Internes EEPROM nach einem Schreibzugriff persistieren                            *
            //*************************************************************************************
            //* Auf ESP32/RP2040 wird EEPROM.write() zunaechst nur in einen RAM-Puffer            *
            //* geschrieben - erst commit() schreibt ihn in den Flash. Der ATmega644P schreibt    *
            //* direkt (No-Op).                                                                   *
            //*************************************************************************************

            static void eepromCommit();

            //*************************************************************************************
            //* Analog-Digital-Wandler auf die Aufloesung des ATmega644P (10 Bit) einstellen      *
            //*************************************************************************************
            //* Haelt die 10-Bit-Kalibrierung von MbcUtils::adcReadVspg()/adcReadPlugged() auf    *
            //* allen drei Zielen gueltig. Klassische AVR-Arduino-Cores kennen                    *
            //* analogReadResolution() nicht und liefern ohnehin 10 Bit (No-Op dort).             *
            //*************************************************************************************

            static void adcConfigure();

            //*************************************************************************************
            //* ADC-Vollausschlag-Referenzspannung des jeweiligen Prozessors liefern              *
            //*************************************************************************************
            //* Haelt Mbc89Utils::adcReadVspg()s Spannungsteiler-Umrechnung (fester Teilerfaktor  *
            //* 10.2/1.1, unveraendert auf allen drei Zielen) korrekt, unabhaengig von der        *
            //* tatsaechlichen ADC-Bezugsspannung: ATmega644P wandelt gegen die 5V-AVcc-Referenz  *
            //* (kein analogReference()-Aufruf im Projekt), ESP32-C3/RP2040 wandeln gegen ihre    *
            //* 3.3V-Versorgung. Fuer mbc-80 ESP32-C3 HW260805 hardwarebestaetigt (baugleicher    *
            //* Fix in der gemeinsamen MbcPlatform, siehe dort) - fuer mbc-89 noch nicht.         *
            //*************************************************************************************
            //* Return: ADC-Vollausschlagsspannung in Volt                                        *
            //*************************************************************************************

            static float adcRefVolts();

            //*************************************************************************************
            //* Sofortneustart ueber den Software-Reset des Controllers                           *
            //*************************************************************************************
            //* NICHT auf AVR verfuegbar (dort ohne Wirkung) - der ATmega644P besitzt keinen per  *
            //* Software auszuloesenden Chip-Reset, siehe resetViaPin().                          *
            //*************************************************************************************

            static void resetDevice();

            //*************************************************************************************
            //* Neustart ueber einen extern beschalteten Reset-Pin (NUR AVR)                      *
            //*************************************************************************************
            //* Watchdog-Trick: Pin kurz als Eingang mit Pull-up "vorladen", dann als Ausgang LOW *
            //* ziehen, um ueber eine externe Reset-Beschaltung einen sauberen Reset-Puls         *
            //* auszuloesen. Auf RP2040/ESP32-C3 ohne Wirkung (No-Op) - dort resetDevice()        *
            //* verwenden.                                                                        *
            //*************************************************************************************
            //* pin: Reset-Pin                                                                    *
            //*************************************************************************************

            static void resetViaPin(uint8_t pin);

            //*************************************************************************************
            //* Watchdog-Timer starten (Timeout ca. 8 s). No-Op auf AVR - der                     *
            //* ATmega644P-Hardware-Watchdog bleibt nach einem Watchdog-Reset scharf,             *
            //* C/C++-Laufzeitinitialisierung ist dafuer zu langsam (bekanntes Bootschleifen-     *
            //* Phaenomen), siehe resetViaPin().                                                  *
            //*************************************************************************************

            static void watchdogBegin();

            //*************************************************************************************
            //* Watchdog-Timer fuettern - muss regelmaessig aufgerufen werden (siehe              *
            //* MbcUtils::update()), sonst folgt der automatische Neustart aus watchdogBegin().   *
            //* No-Op auf AVR, siehe dort.                                                        *
            //*************************************************************************************

            static void watchdogReset();
    };

//------ Ende Klassendefinition Mbc89Platform -----------------------------------------------------

#endif  /* _MBC89PLATFORM_H_ */

//###### Ende Quellcode Mbc89Platform.h ###########################################################
