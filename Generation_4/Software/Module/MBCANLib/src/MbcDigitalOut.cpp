//*************************************************************************************************
//* Ein digitaler Ausgang: natives GPIO oder MbcMcp23x17-Bit (MBCANLib)                           *
//*************************************************************************************************
//* Datei: MbcDigitalOut.cpp                                                                      *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen:                                                                                  *
//* V1.0.0 - 05.08.2026 - Nullversion                                                             *
//* V1.1.0 - 07.08.2026 - Natives GPIO und MbcMcp23x17-Bit in einer Klasse zusammengefuehrt       *
//*************************************************************************************************

//###### Start Quellcode MbcDigitalOut.cpp ########################################################

#include "MbcDigitalOut.h"

//------ Start Klasse MbcDigitalOut ---------------------------------------------------------------

    //*********************************************************************************************
    //* Konstruktor fuer ein natives GPIO                                                         *
    //*********************************************************************************************
    //* pin:       natives Digitalpin                                                             *
    //* activeLow: true, wenn write(true) den Pin auf LOW statt HIGH legen soll                   *
    //*********************************************************************************************

    MbcDigitalOut::MbcDigitalOut(uint8_t pin, bool activeLow)
        : _backend(NATIVE), _activeLow(activeLow), _pin(pin),
          _expander(nullptr), _portB(false), _bit(0)
    {
    }

    //*********************************************************************************************
    //* Konstruktor fuer ein einzelnes GPIOA/GPIOB-Bit eines MbcMcp23x17                          *
    //*********************************************************************************************
    //* expander:  zugehoeriger MbcMcp23x17 (muss vor Nutzung bereits begin()t sein)              *
    //* portB:     false = GPIOA, true = GPIOB                                                    *
    //* bit:       0...7                                                                          *
    //* activeLow: true, wenn write(true) das Bit auf LOW statt HIGH legen soll                   *
    //*********************************************************************************************

    MbcDigitalOut::MbcDigitalOut(MbcMcp23x17 &expander, bool portB, uint8_t bit, bool activeLow)
        : _backend(EXPANDER), _activeLow(activeLow), _pin(0),
          _expander(&expander), _portB(portB), _bit(bit)
    {
    }

    //*********************************************************************************************
    //* Pin auf OUTPUT setzen und in den (inaktiven) Startzustand bringen - nur beim nativen      *
    //* Backend noetig, beim MbcMcp23x17-Backend no-op                                            *
    //*********************************************************************************************

    void MbcDigitalOut::begin()
    {
        if (_backend != NATIVE) return;

        pinMode(_pin, OUTPUT);
        write(false);
    }

    //*********************************************************************************************
    //* Ausgang setzen                                                                            *
    //*********************************************************************************************
    //* on: true = aktiv (bereits gegen activeLow aufgeloest)                                     *
    //*********************************************************************************************

    void MbcDigitalOut::write(bool on)
    {
        bool level = _activeLow ? !on : on;

        if (_backend == NATIVE)
        {
            digitalWrite(_pin, level ? HIGH : LOW);
        }
        else
        {
            if (_portB) _expander->writeGpioBBit(_bit, level);
            else        _expander->writeGpioABit(_bit, level);
        }
    }

//------ Ende Klasse MbcDigitalOut ----------------------------------------------------------------

//###### Ende Quellcode MbcDigitalOut.cpp #########################################################
