//*************************************************************************************************
//* HEADER - Ein digitaler Eingang: natives GPIO oder Mbc89Mcp23x17-Bit (Mbc89Lib, ESP32-C3)       *
//*************************************************************************************************
//* Umbenannte Kopie von MBCANLib/MbcDigitalIn.h (siehe Mbc89Lib/README.md, "Warum eigenstaendig") *
//* - inhaltlich unveraendert, da diese Klasse ohnehin keine I2C-Abhaengigkeit hat. Ein einzelner   *
//* boolscher Eingang, wahlweise ein natives GPIO oder ein einzelnes GPIOA/GPIOB-Bit eines         *
//* Mbc89Mcp23x17-Port-Expanders - der Konstruktor waehlt das Backend, analog zum Gegenstueck       *
//* Mbc89DigitalOut. Ermoeglicht Mbc89Utils (Steckernetzteil-Erkennung) oder Mbc89Com               *
//* (CU-STOP/CU-GO/CU-EXT), ihre Eingaenge optional auf einen Expander umzuleiten.                  *
//*************************************************************************************************
//* Datei: Mbc89DigitalIn.h                                                                        *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen:                                                                                  *
//* V1.0.0 - 27.08.2026 - Nullversion                                                             *
//*************************************************************************************************

#pragma once

//###### Start Quellcode Mbc89DigitalIn.h #########################################################

#ifndef _MBC89DIGITALIN_H_
#define _MBC89DIGITALIN_H_     "V1.0.0 - 27.08.2026"

//------ Einbindung Bibliotheken ------------------------------------------------------------------

#include <Arduino.h>
#include "Mbc89Mcp23x17.h"

//------ Start Klassendefinition Mbc89DigitalIn ---------------------------------------------------

    //* Klasse fuer <Mbc89DigitalIn>
    //* Ein Objekt = ein einzelner digitaler Eingang, natives GPIO oder Mbc89Mcp23x17-Bit.

    class Mbc89DigitalIn
    {
        public:

            //*************************************************************************************
            //* Konstruktor fuer ein natives GPIO                                                 *
            //*************************************************************************************
            //* pin:       natives Digitalpin                                                     *
            //* activeLow: true, wenn LOW-Pegel read()==true liefern soll (Standard - typisch     *
            //*            fuer Taster gegen Masse mit PullUp)                                    *
            //* pullup:    true, um den internen PullUp zu aktivieren (INPUT_PULLUP statt INPUT)  *
            //*************************************************************************************

            explicit Mbc89DigitalIn(uint8_t pin, bool activeLow = true, bool pullup = true);

            //*************************************************************************************
            //* Konstruktor fuer ein einzelnes GPIOA/GPIOB-Bit eines Mbc89Mcp23x17                *
            //*************************************************************************************
            //* expander:  zugehoeriger Mbc89Mcp23x17 (muss vor Nutzung bereits begin()t sein)    *
            //* portB:     false = GPIOA, true = GPIOB                                            *
            //* bit:       0...7                                                                  *
            //* activeLow: true, wenn LOW-Pegel read()==true liefern soll                         *
            //*************************************************************************************

            Mbc89DigitalIn(Mbc89Mcp23x17 &expander, bool portB, uint8_t bit, bool activeLow = false);

            //*************************************************************************************
            //* Pin auf INPUT bzw. INPUT_PULLUP setzen - nur beim nativen Backend noetig, beim    *
            //* Mbc89Mcp23x17-Backend no-op                                                       *
            //*************************************************************************************

            void begin();

            //*************************************************************************************
            //* Eingang lesen                                                                     *
            //*************************************************************************************
            //* Return: Zustand des Eingangs (bereits gegen activeLow aufgeloest)                 *
            //*************************************************************************************

            bool read();

        private:

            enum Backend { NATIVE, EXPANDER } _backend;
            bool _activeLow;

            uint8_t _pin;             // nur natives Backend
            bool    _pullup;          // nur natives Backend

            Mbc89Mcp23x17 *_expander; // nur Mbc89Mcp23x17-Backend
            bool           _portB;
            uint8_t        _bit;
    };

//------ Ende Klassendefinition Mbc89DigitalIn ----------------------------------------------------

#endif  /* _MBC89DIGITALIN_H_ */

//###### Ende Quellcode Mbc89DigitalIn.h ###########################################################
