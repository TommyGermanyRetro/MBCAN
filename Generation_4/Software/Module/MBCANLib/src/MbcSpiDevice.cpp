//*************************************************************************************************
//* Duenner SPI-Geraete-Wrapper (MBCANLib)                                                        *
//*************************************************************************************************
//* Datei: MbcSpiDevice.cpp                                                                       *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen:                                                                                  *
//* V1.0.0 - 05.08.2026 - Nullversion                                                             *
//* V1.2.0 - 13.11.2026 - RP2040: SPI-Transaktionen ueber beide Kerne gegeneinander gesperrt      *
//*************************************************************************************************

//###### Start Quellcode MbcSpiDevice.cpp #########################################################

#include "MbcSpiDevice.h"

#if defined(ARDUINO_ARCH_RP2040)
    #include "pico/mutex.h"

    // Gemeinsame Sperre aller SPI-Teilnehmer: Auf dem RP2040 kann Kern 1 die MCP2515 auslesen
    // (MbcCan::serviceRxQueue), waehrend Kern 0 auf denselben SPI-Bus zugreift. Initialisiert vor
    // setup()/setup1() durch einen statischen Konstruktor.

    static recursive_mutex_t mbcSpiMutex;

    static struct MbcSpiMutexInit
    {
        MbcSpiMutexInit() { recursive_mutex_init(&mbcSpiMutex); }
    } mbcSpiMutexInit;
#endif

//------ Start Klasse MbcSpiDevice ----------------------------------------------------------------

    //*********************************************************************************************
    //* Konstruktor zur Deklaration von Objektvariablen                                           *
    //*********************************************************************************************
    //* pinCs:   /CS-Pin dieses SPI-Teilnehmers                                                   *
    //* clockHz: SPI-Takt in Hz fuer diesen Teilnehmer                                            *
    //* spi:     zu nutzende SPI-Schnittstelle                                                    *
    //*********************************************************************************************

    MbcSpiDevice::MbcSpiDevice(uint8_t pinCs, uint32_t clockHz, SPIClass &spi)
        : _pinCs(pinCs), _clockHz(clockHz), _spi(&spi)
    {
    }

    //*********************************************************************************************
    //* /CS-Pin initialisieren (Ausgang, HIGH = inaktiv)                                          *
    //*********************************************************************************************

    void MbcSpiDevice::begin()
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

    void MbcSpiDevice::beginTransaction()
    {
        #if defined(ARDUINO_ARCH_RP2040)
            recursive_mutex_enter_blocking(&mbcSpiMutex);
        #endif

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

    void MbcSpiDevice::endTransaction()
    {
        #if defined(__AVR__)
            *_csPort |= _csBitMask;
        #else
            digitalWrite(_pinCs, HIGH);
        #endif

        _spi->endTransaction();

        #if defined(ARDUINO_ARCH_RP2040)
            recursive_mutex_exit(&mbcSpiMutex);
        #endif
    }

    //*********************************************************************************************
    //* Ein Byte senden/empfangen (nur innerhalb einer Transaktion gueltig)                       *
    //*********************************************************************************************
    //* data: zu sendendes Byte                                                                   *
    //*********************************************************************************************
    //* Return: empfangenes Byte                                                                  *
    //*********************************************************************************************

    uint8_t MbcSpiDevice::transfer(uint8_t data)
    {
        return _spi->transfer(data);
    }

//------ Ende Klasse MbcSpiDevice -----------------------------------------------------------------

//###### Ende Quellcode MbcSpiDevice.cpp ##########################################################
