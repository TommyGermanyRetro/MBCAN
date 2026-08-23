//*************************************************************************************************
//* Ein digitaler Eingang: natives GPIO oder MbcMcp23x17-Bit (MBCANLib)                           *
//*************************************************************************************************
//* Datei: MbcDigitalIn.cpp                                                                       *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen:                                                                                  *
//* V1.0.0 - 05.08.2026 - Nullversion                                                             *
//* V1.1.0 - 07.08.2026 - Natives GPIO und MbcMcp23x17-Bit in einer Klasse zusammengefuehrt       *
//*************************************************************************************************

//###### Start Quellcode MbcDigitalIn.cpp #########################################################

#include "MbcDigitalIn.h"

//------ Start Klasse MbcDigitalIn ----------------------------------------------------------------

    //*********************************************************************************************
    //* Konstruktor fuer ein natives GPIO                                                         *
    //*********************************************************************************************
    //* pin:       natives Digitalpin                                                             *
    //* activeLow: true, wenn LOW-Pegel read()==true liefern soll (Standard - typisch fuer        *
    //*            Taster gegen Masse mit PullUp)                                                 *
    //* pullup:    true, um den internen PullUp zu aktivieren (INPUT_PULLUP statt INPUT)          *
    //*********************************************************************************************

    MbcDigitalIn::MbcDigitalIn(uint8_t pin, bool activeLow, bool pullup)
        : _backend(NATIVE), _activeLow(activeLow), _pin(pin), _pullup(pullup),
          _expander(nullptr), _portB(false), _bit(0)
    {
    }

    //*********************************************************************************************
    //* Konstruktor fuer ein einzelnes GPIOA/GPIOB-Bit eines MbcMcp23x17                          *
    //*********************************************************************************************
    //* expander:  zugehoeriger MbcMcp23x17 (muss vor Nutzung bereits begin()t sein)              *
    //* portB:     false = GPIOA, true = GPIOB                                                    *
    //* bit:       0...7                                                                          *
    //* activeLow: true, wenn LOW-Pegel read()==true liefern soll                                 *
    //*********************************************************************************************

    MbcDigitalIn::MbcDigitalIn(MbcMcp23x17 &expander, bool portB, uint8_t bit, bool activeLow)
        : _backend(EXPANDER), _activeLow(activeLow), _pin(0), _pullup(false),
          _expander(&expander), _portB(portB), _bit(bit)
    {
    }

    //*********************************************************************************************
    //* Pin auf INPUT bzw. INPUT_PULLUP setzen - nur beim nativen Backend noetig, beim            *
    //* MbcMcp23x17-Backend no-op                                                                 *
    //*********************************************************************************************

    void MbcDigitalIn::begin()
    {
        if (_backend != NATIVE) return;

        pinMode(_pin, _pullup ? INPUT_PULLUP : INPUT);
    }

    //*********************************************************************************************
    //* Eingang lesen                                                                             *
    //*********************************************************************************************
    //* Return: Zustand des Eingangs (bereits gegen activeLow aufgeloest)                         *
    //*********************************************************************************************

    bool MbcDigitalIn::read()
    {
        bool level;

        if (_backend == NATIVE)
        {
            level = (digitalRead(_pin) == HIGH);
        }
        else
        {
            uint8_t gpio = _portB ? _expander->readGpioB() : _expander->readGpioA();

            level = ((gpio & (1 << _bit)) != 0);
        }

        return _activeLow ? !level : level;
    }

//------ Ende Klasse MbcDigitalIn -----------------------------------------------------------------

//###### Ende Quellcode MbcDigitalIn.cpp ##########################################################
