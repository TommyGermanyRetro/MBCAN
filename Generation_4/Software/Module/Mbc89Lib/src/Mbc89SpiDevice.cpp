//*************************************************************************************************
//* Duenner SPI-Geraete-Wrapper (MBCANLib)                                                        *
//*************************************************************************************************
//* Datei: Mbc89SpiDevice.cpp                                                                     *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen:                                                                                  *
//* V1.0.0 - 05.08.2026 - Nullversion                                                             *
//*************************************************************************************************

//###### Start Quellcode Mbc89SpiDevice.cpp #######################################################

#include "Mbc89SpiDevice.h"

//------ Start Klasse Mbc89SpiDevice --------------------------------------------------------------

    //*********************************************************************************************
    //* Konstruktor zur Deklaration von Objektvariablen                                           *
    //*********************************************************************************************
    //* pinCs:   /CS-Pin dieses SPI-Teilnehmers                                                   *
    //* clockHz: SPI-Takt in Hz fuer diesen Teilnehmer                                            *
    //* spi:     zu nutzende SPI-Schnittstelle                                                    *
    //*********************************************************************************************

    Mbc89SpiDevice::Mbc89SpiDevice(uint8_t pinCs, uint32_t clockHz, SPIClass &spi)
        : _pinCs(pinCs), _clockHz(clockHz), _spi(&spi)
    {
    }

    //*********************************************************************************************
    //* /CS-Pin initialisieren (Ausgang, HIGH = inaktiv)                                          *
    //*********************************************************************************************

    void Mbc89SpiDevice::begin()
    {
        pinMode(_pinCs, OUTPUT);
        digitalWrite(_pinCs, HIGH);

        #if defined(__AVR__)
            _csPort    = portOutputRegister(digitalPinToPort(_pinCs));
            _csBitMask = digitalPinToBitMask(_pinCs);
        #endif
    }

    //*********************************************************************************************
    //* Transaktion beginnen: SPI-Takt/-Modus setzen, /CS auf LOW                                 *
    //*********************************************************************************************

    void Mbc89SpiDevice::beginTransaction()
    {
        _spi->beginTransaction(SPISettings(_clockHz, MSBFIRST, SPI_MODE0));

        #if defined(__AVR__)
            *_csPort &= (uint8_t) ~_csBitMask;
        #else
            digitalWrite(_pinCs, LOW);
        #endif
    }

    //*********************************************************************************************
    //* Transaktion beenden: /CS auf HIGH, SPI-Bus wieder freigeben                               *
    //*********************************************************************************************

    void Mbc89SpiDevice::endTransaction()
    {
        #if defined(__AVR__)
            *_csPort |= _csBitMask;
        #else
            digitalWrite(_pinCs, HIGH);
        #endif

        _spi->endTransaction();
    }

    //*********************************************************************************************
    //* Ein Byte senden/empfangen (nur innerhalb einer Transaktion gueltig)                       *
    //*********************************************************************************************
    //* data: zu sendendes Byte                                                                   *
    //*********************************************************************************************
    //* Return: empfangenes Byte                                                                  *
    //*********************************************************************************************

    uint8_t Mbc89SpiDevice::transfer(uint8_t data)
    {
        return _spi->transfer(data);
    }

//------ Ende Klasse Mbc89SpiDevice ---------------------------------------------------------------

//###### Ende Quellcode Mbc89SpiDevice.cpp ########################################################
