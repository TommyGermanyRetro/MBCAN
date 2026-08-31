//*************************************************************************************************
//* HEADER - Ein digitaler Ausgang: natives GPIO oder Mbc89Mcp23x17-Bit (Mbc89Lib, ESP32-C3)       *
//*************************************************************************************************
//* Umbenannte Kopie von MBCANLib/MbcDigitalOut.h (siehe Mbc89Lib/README.md, "Warum eigenstaendig")*
//* - inhaltlich unveraendert, da diese Klasse ohnehin keine I2C-Abhaengigkeit hat. Ein einzelner   *
//* boolscher Ausgang, wahlweise ein natives GPIO oder ein einzelnes GPIOA/GPIOB-Bit eines         *
//* Mbc89Mcp23x17-Port-Expanders - der Konstruktor waehlt das Backend. Ermoeglicht Mbc89Utils       *
//* (Status-LEDs), seine Ausgaenge optional auf einen Expander statt auf ein natives Pin            *
//* umzuleiten, ohne selbst hardwarespezifisch angepasst werden zu muessen.                         *
//*************************************************************************************************
//* Datei: Mbc89DigitalOut.h                                                                       *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen:                                                                                  *
//* V1.0.0 - 27.08.2026 - Nullversion                                                             *
//*************************************************************************************************

#pragma once

//###### Start Quellcode Mbc89DigitalOut.h ########################################################

#ifndef _MBC89DIGITALOUT_H_
#define _MBC89DIGITALOUT_H_     "V1.0.0 - 27.08.2026"

//------ Einbindung Bibliotheken ------------------------------------------------------------------

#include <Arduino.h>
#include "Mbc89Mcp23x17.h"

//------ Start Klassendefinition Mbc89DigitalOut --------------------------------------------------

    //* Klasse fuer <Mbc89DigitalOut>
    //* Ein Objekt = ein einzelner digitaler Ausgang, natives GPIO oder Mbc89Mcp23x17-Bit.

    class Mbc89DigitalOut
    {
        public:

            //*************************************************************************************
            //* Konstruktor fuer ein natives GPIO                                                 *
            //*************************************************************************************
            //* pin:       natives Digitalpin                                                     *
            //* activeLow: true, wenn write(true) den Pin auf LOW statt HIGH legen soll           *
            //*************************************************************************************

            explicit Mbc89DigitalOut(uint8_t pin, bool activeLow = false);

            //*************************************************************************************
            //* Konstruktor fuer ein einzelnes GPIOA/GPIOB-Bit eines Mbc89Mcp23x17                *
            //*************************************************************************************
            //* expander:  zugehoeriger Mbc89Mcp23x17 (muss vor Nutzung bereits begin()t sein)    *
            //* portB:     false = GPIOA, true = GPIOB                                            *
            //* bit:       0...7                                                                  *
            //* activeLow: true, wenn write(true) das Bit auf LOW statt HIGH legen soll           *
            //*************************************************************************************

            Mbc89DigitalOut(Mbc89Mcp23x17 &expander, bool portB, uint8_t bit, bool activeLow = false);

            //*************************************************************************************
            //* Pin auf OUTPUT setzen und in den (inaktiven) Startzustand bringen - nur beim      *
            //* nativen Backend noetig, beim Mbc89Mcp23x17-Backend no-op                          *
            //*************************************************************************************

            void begin();

            //*************************************************************************************
            //* Ausgang setzen                                                                    *
            //*************************************************************************************
            //* on: true = aktiv (bereits gegen activeLow aufgeloest)                             *
            //*************************************************************************************

            void write(bool on);

        private:

            enum Backend { NATIVE, EXPANDER } _backend;
            bool _activeLow;

            uint8_t _pin;             // nur natives Backend

            Mbc89Mcp23x17 *_expander; // nur Mbc89Mcp23x17-Backend
            bool           _portB;
            uint8_t        _bit;
    };

//------ Ende Klassendefinition Mbc89DigitalOut ---------------------------------------------------

#endif  /* _MBC89DIGITALOUT_H_ */

//###### Ende Quellcode Mbc89DigitalOut.h ##########################################################
