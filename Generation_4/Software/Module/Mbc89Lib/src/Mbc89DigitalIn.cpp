//*************************************************************************************************
//* Ein digitaler Eingang: natives GPIO oder Mbc89Mcp23x17-Bit (Mbc89Lib, ESP32-C3)                *
//*************************************************************************************************
//* Datei: Mbc89DigitalIn.cpp                                                                      *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen:                                                                                  *
//* V1.0.0 - 27.08.2026 - Nullversion                                                             *
//*************************************************************************************************

//###### Start Quellcode Mbc89DigitalIn.cpp #######################################################

#include "Mbc89DigitalIn.h"

//------ Start Klasse Mbc89DigitalIn ----------------------------------------------------------------

    //*********************************************************************************************
    //* Konstruktor fuer ein natives GPIO                                                         *
    //*********************************************************************************************

    Mbc89DigitalIn::Mbc89DigitalIn(uint8_t pin, bool activeLow, bool pullup)
        : _backend(NATIVE), _activeLow(activeLow), _pin(pin), _pullup(pullup),
          _expander(nullptr), _portB(false), _bit(0)
    {
    }

    //*********************************************************************************************
    //* Konstruktor fuer ein einzelnes GPIOA/GPIOB-Bit eines Mbc89Mcp23x17                        *
    //*********************************************************************************************

    Mbc89DigitalIn::Mbc89DigitalIn(Mbc89Mcp23x17 &expander, bool portB, uint8_t bit, bool activeLow)
        : _backend(EXPANDER), _activeLow(activeLow), _pin(0), _pullup(false),
          _expander(&expander), _portB(portB), _bit(bit)
    {
    }

    //*********************************************************************************************
    //* Pin auf INPUT bzw. INPUT_PULLUP setzen - nur beim nativen Backend noetig, beim            *
    //* Mbc89Mcp23x17-Backend no-op                                                                *
    //*********************************************************************************************

    void Mbc89DigitalIn::begin()
    {
        if (_backend != NATIVE) return;

        pinMode(_pin, _pullup ? INPUT_PULLUP : INPUT);
    }

    //*********************************************************************************************
    //* Eingang lesen                                                                             *
    //*********************************************************************************************
    //* Return: Zustand des Eingangs (bereits gegen activeLow aufgeloest)                         *
    //*********************************************************************************************

    bool Mbc89DigitalIn::read()
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

//------ Ende Klasse Mbc89DigitalIn -----------------------------------------------------------------

//###### Ende Quellcode Mbc89DigitalIn.cpp ##########################################################
