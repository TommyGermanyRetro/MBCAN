//*************************************************************************************************
//* Ein digitaler Ausgang: natives GPIO oder Mbc89Mcp23x17-Bit (Mbc89Lib, ESP32-C3)                *
//*************************************************************************************************
//* Datei: Mbc89DigitalOut.cpp                                                                     *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen:                                                                                  *
//* V1.0.0 - 27.08.2026 - Nullversion                                                             *
//*************************************************************************************************

//###### Start Quellcode Mbc89DigitalOut.cpp ######################################################

#include "Mbc89DigitalOut.h"

//------ Start Klasse Mbc89DigitalOut ---------------------------------------------------------------

    //*********************************************************************************************
    //* Konstruktor fuer ein natives GPIO                                                         *
    //*********************************************************************************************

    Mbc89DigitalOut::Mbc89DigitalOut(uint8_t pin, bool activeLow)
        : _backend(NATIVE), _activeLow(activeLow), _pin(pin),
          _expander(nullptr), _portB(false), _bit(0)
    {
    }

    //*********************************************************************************************
    //* Konstruktor fuer ein einzelnes GPIOA/GPIOB-Bit eines Mbc89Mcp23x17                        *
    //*********************************************************************************************

    Mbc89DigitalOut::Mbc89DigitalOut(Mbc89Mcp23x17 &expander, bool portB, uint8_t bit, bool activeLow)
        : _backend(EXPANDER), _activeLow(activeLow), _pin(0),
          _expander(&expander), _portB(portB), _bit(bit)
    {
    }

    //*********************************************************************************************
    //* Pin auf OUTPUT setzen und in den (inaktiven) Startzustand bringen - nur beim nativen      *
    //* Backend noetig, beim Mbc89Mcp23x17-Backend no-op                                          *
    //*********************************************************************************************

    void Mbc89DigitalOut::begin()
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

    void Mbc89DigitalOut::write(bool on)
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

//------ Ende Klasse Mbc89DigitalOut ----------------------------------------------------------------

//###### Ende Quellcode Mbc89DigitalOut.cpp #########################################################
