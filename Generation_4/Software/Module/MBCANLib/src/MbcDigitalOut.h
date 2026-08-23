//*************************************************************************************************
//* HEADER - Ein digitaler Ausgang: natives GPIO oder MbcMcp23x17-Bit (MBCANLib)                  *
//*************************************************************************************************
//* Ein einzelner boolscher Ausgang, wahlweise ein natives GPIO oder ein einzelnes                *
//* GPIOA/GPIOB-Bit eines MbcMcp23x17-Port-Expanders - der Konstruktor waehlt das Backend, analog *
//* zum Muster von MbcMcp23x17 selbst (I2C/SPI in einer Klasse statt getrennter Unterklassen).    *
//* Ermoeglicht Klassen wie MbcUtils (Status-LEDs) oder MbcWifiBridge (Verbindungs-LED), ihre     *
//* Ausgaenge optional auf einen Expander statt auf ein natives Pin umzuleiten, ohne selbst       *
//* plattform- oder hardwarespezifisch angepasst werden zu muessen.                               *
//*************************************************************************************************
//* Datei: MbcDigitalOut.h                                                                        *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen:                                                                                  *
//* V1.0.0 - 05.08.2026 - Nullversion                                                             *
//* V1.1.0 - 07.08.2026 - Natives GPIO und MbcMcp23x17-Bit in einer Klasse zusammengefuehrt       *
//*************************************************************************************************

#pragma once

//###### Start Quellcode MbcDigitalOut.h ##########################################################

#ifndef _MBCDIGITALOUT_H_
#define _MBCDIGITALOUT_H_     "V1.1.0 - 07.08.2026"

//------ Einbindung Bibliotheken ------------------------------------------------------------------

#include <Arduino.h>
#include "MbcMcp23x17.h"

//------ Start Klassendefinition MbcDigitalOut ----------------------------------------------------

    //* Klasse fuer <MbcDigitalOut>
    //* Ein Objekt = ein einzelner digitaler Ausgang, natives GPIO oder MbcMcp23x17-Bit.

    class MbcDigitalOut
    {
        public:

            //*************************************************************************************
            //* Konstruktor fuer ein natives GPIO                                                 *
            //*************************************************************************************
            //* pin:       natives Digitalpin                                                     *
            //* activeLow: true, wenn write(true) den Pin auf LOW statt HIGH legen soll           *
            //*************************************************************************************

            explicit MbcDigitalOut(uint8_t pin, bool activeLow = false);

            //*************************************************************************************
            //* Konstruktor fuer ein einzelnes GPIOA/GPIOB-Bit eines MbcMcp23x17                  *
            //*************************************************************************************
            //* expander:  zugehoeriger MbcMcp23x17 (muss vor Nutzung bereits begin()t sein)      *
            //* portB:     false = GPIOA, true = GPIOB                                            *
            //* bit:       0...7                                                                  *
            //* activeLow: true, wenn write(true) das Bit auf LOW statt HIGH legen soll           *
            //*************************************************************************************

            MbcDigitalOut(MbcMcp23x17 &expander, bool portB, uint8_t bit, bool activeLow = false);

            //*************************************************************************************
            //* Pin auf OUTPUT setzen und in den (inaktiven) Startzustand bringen - nur beim      *
            //* nativen Backend noetig, beim MbcMcp23x17-Backend no-op (der Expander wird         *
            //* eigenstaendig begin()t, siehe Konstruktor)                                        *
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

            uint8_t _pin;          // nur natives Backend

            MbcMcp23x17 *_expander; // nur MbcMcp23x17-Backend
            bool         _portB;
            uint8_t      _bit;
    };

//------ Ende Klassendefinition MbcDigitalOut -----------------------------------------------------

#endif  /* _MBCDIGITALOUT_H_ */

//###### Ende Quellcode MbcDigitalOut.h ###########################################################
