//*************************************************************************************************
//* HEADER - Ein digitaler Eingang: natives GPIO oder MbcMcp23x17-Bit (MBCANLib)                  *
//*************************************************************************************************
//* Ein einzelner boolscher Eingang, wahlweise ein natives GPIO oder ein einzelnes                *
//* GPIOA/GPIOB-Bit eines MbcMcp23x17-Port-Expanders - der Konstruktor waehlt das Backend, analog *
//* zum Muster von MbcMcp23x17 selbst (I2C/SPI in einer Klasse statt getrennter Unterklassen) und *
//* zum Gegenstueck MbcDigitalOut. Ermoeglicht Klassen wie MbcUtils (Steckernetzteil-Erkennung),  *
//* ihre Eingaenge optional auf einen Expander statt auf ein natives Pin umzuleiten, ohne selbst  *
//* plattform- oder hardwarespezifisch angepasst werden zu muessen.                               *
//*************************************************************************************************
//* Datei: MbcDigitalIn.h                                                                         *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen:                                                                                  *
//* V1.0.0 - 05.08.2026 - Nullversion                                                             *
//* V1.1.0 - 07.08.2026 - Natives GPIO und MbcMcp23x17-Bit in einer Klasse zusammengefuehrt       *
//*************************************************************************************************

#pragma once

//###### Start Quellcode MbcDigitalIn.h ###########################################################

#ifndef _MBCDIGITALIN_H_
#define _MBCDIGITALIN_H_     "V1.1.0 - 07.08.2026"

//------ Einbindung Bibliotheken ------------------------------------------------------------------

#include <Arduino.h>
#include "MbcMcp23x17.h"

//------ Start Klassendefinition MbcDigitalIn -----------------------------------------------------

    //* Klasse fuer <MbcDigitalIn>
    //* Ein Objekt = ein einzelner digitaler Eingang, natives GPIO oder MbcMcp23x17-Bit.

    class MbcDigitalIn
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

            explicit MbcDigitalIn(uint8_t pin, bool activeLow = true, bool pullup = true);

            //*************************************************************************************
            //* Konstruktor fuer ein einzelnes GPIOA/GPIOB-Bit eines MbcMcp23x17                  *
            //*************************************************************************************
            //* expander:  zugehoeriger MbcMcp23x17 (muss vor Nutzung bereits begin()t sein)      *
            //* portB:     false = GPIOA, true = GPIOB                                            *
            //* bit:       0...7                                                                  *
            //* activeLow: true, wenn LOW-Pegel read()==true liefern soll                         *
            //*************************************************************************************

            MbcDigitalIn(MbcMcp23x17 &expander, bool portB, uint8_t bit, bool activeLow = false);

            //*************************************************************************************
            //* Pin auf INPUT bzw. INPUT_PULLUP setzen - nur beim nativen Backend noetig, beim    *
            //* MbcMcp23x17-Backend no-op (der Expander wird eigenstaendig begin()t, siehe        *
            //* Konstruktor)                                                                      *
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

            uint8_t _pin;          // nur natives Backend
            bool    _pullup;       // nur natives Backend

            MbcMcp23x17 *_expander; // nur MbcMcp23x17-Backend
            bool         _portB;
            uint8_t      _bit;
    };

//------ Ende Klassendefinition MbcDigitalIn ------------------------------------------------------

#endif  /* _MBCDIGITALIN_H_ */

//###### Ende Quellcode MbcDigitalIn.h ############################################################
