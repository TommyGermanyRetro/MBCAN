//*************************************************************************************************
//* HEADER - CAN-Anbindung, Maerklin-CS2-29-Bit-Header (MBCANLib)                                 *
//*************************************************************************************************
//* Ein Objekt = ein physischer CAN-Anschluss dieses Moduls. Zwei Backends stehen zur Wahl: ein   *
//* externer MCP2515 an SPI, oder (nur ESP32-C3) der INTERNE CAN-Controller (TWAI,                *
//* ESP-IDF-Treiber driver/twai.h) - dafuer bietet diese Klasse einen zweiten Konstruktor an, der *
//* auf ESP32 den externen MCP2515 ueberfluessig macht. Welches Backend ein Objekt nutzt, wird    *
//* ueber den verwendeten Konstruktor entschieden: - MbcCan(pinCs, utils, spi) -> MCP2515 an SPI  *
//* (ATmega644P, RP2040, auch ESP32 moeglich) - MbcCan(pinTx, pinRx, utils) -> interner           *
//* TWAI-Controller (NUR ESP32/ESP32-C3) Beide Backends bilden nach aussen dieselbe               *
//* MBC_CAN_MSG_LEN-Byte-Nachrichtenstruktur und das Maerklin-29-Bit-Header-Mapping ab (siehe     *
//* ausfuehrliche Herleitung in MbcCan.cpp) - fuer MbcCom und darueber liegenden Code ist das     *
//* Backend transparent. Jedes Objekt = eine Instanz (mehrere CAN-Kanaele werden durch mehrere    *
//* Instanzen abgebildet). Das MCP2515-Backend nutzt die gemeinsame Arduino-SPIClass. Fuer die    *
//* Traffic-LED-Anzeige haelt die Klasse eine Referenz auf MbcUtils.                              *
//*************************************************************************************************
//* Datei: MbcCan.h                                                                               *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen:                                                                                  *
//* V1.0.0 - 02.08.2026 - Nullversion                                                             *
//*************************************************************************************************

#pragma once

//###### Start Quellcode MbcCan.h #################################################################

#ifndef _MBCCAN_H_
#define _MBCCAN_H_     "V1.0.0 - 02.08.2026"

//------ Einbindung Bibliotheken ------------------------------------------------------------------

#include <Arduino.h>
#include <SPI.h>
#include "MbcUtils.h"
#include "MbcSpiDevice.h"

#if defined(ESP32)
    #include "freertos/FreeRTOS.h"
    #include "driver/twai.h"
#endif

//------ Start globale Deklarationen --------------------------------------------------------------

    // Layout einer CAN-Nachricht als flaches Byte-Array (13 Byte) - wird unveraendert an
    // MbcCom weitergereicht.

    #define MBC_CAN_MSG_LEN   13

    #define MBC_CAN_PRIO      0
    #define MBC_CAN_ID        1
    #define MBC_CAN_HASH_H    2
    #define MBC_CAN_HASH_L    3
    #define MBC_CAN_DLC       4
    #define MBC_CAN_D0        5
    #define MBC_CAN_D1        6
    #define MBC_CAN_D2        7
    #define MBC_CAN_D3        8
    #define MBC_CAN_D4        9
    #define MBC_CAN_D5        10
    #define MBC_CAN_D6        11
    #define MBC_CAN_D7        12

    // Maximale Wartezeit in begin() auf den Normalmodus des MCP2515, bevor mit Fehler
    // abgebrochen wird (verhindert einen endlosen Blockiervorgang bei fehlender Antwort).

    #define MBC_CAN_INIT_TIMEOUT_MS   500

//------ Ende globale Deklarationen ---------------------------------------------------------------

//------ Start Klassendefinition MbcCan -----------------------------------------------------------

    //* Klasse fuer <MbcCan>
    //* Ein Objekt = ein physischer CAN-Anschluss dieses Moduls - entweder ein externer MCP2515
    //* an SPI oder (nur ESP32-C3) der interne TWAI-Controller. Implementiert das Maerklin-CS2-
    //* 29-Bit-Extended-Frame-Header-Mapping im Polling-Betrieb (kein Interrupt-Pin genutzt).

    class MbcCan
    {
        public:

            //*************************************************************************************
            //* Konstruktor fuer das MCP2515-Backend (externer Baustein an SPI)                   *
            //*************************************************************************************
            //* pinCs:      /CS-Pin dieses MCP2515                                                *
            //* utils:      MbcUtils-Instanz fuer die Traffic-LED-Anzeige                         *
            //* spi:        zu nutzende SPI-Schnittstelle (Standard: SPI)                         *
            //* spiClockHz: SPI-Takt in Hz (Standard: 8 MHz)                                      *
            //*************************************************************************************

            MbcCan(uint8_t pinCs, MbcUtils &utils, SPIClass &spi = SPI, uint32_t spiClockHz = 8000000);

            #if defined(ESP32)

                //*********************************************************************************
                //* Konstruktor fuer den INTERNEN TWAI-Controller (NUR ESP32/ESP32-C3) - hierfuer *
                //* wird kein externer MCP2515 benoetigt.                                         *
                //*********************************************************************************
                //* pinTx: TX-Pin zum externen CAN-Transceiver (z.B. SN65HVD230)                  *
                //* pinRx: RX-Pin vom externen CAN-Transceiver                                    *
                //* utils: MbcUtils-Instanz fuer die Traffic-LED-Anzeige                          *
                //*********************************************************************************

                MbcCan(uint8_t pinTx, uint8_t pinRx, MbcUtils &utils);

            #endif

            //*************************************************************************************
            //* CAN-Controller initialisieren (250 kBit/s; Empfangsfilter aus). Beim              *
            //* MCP2515-Backend zusaetzlich: Software-Reset, Registerpruefung, bei Nichtantworten *
            //* Anzeige ueber MBC_LED_MCP_ERROR an der Status-LED. Das Warten auf den Normalmodus *
            //* ist auf MBC_CAN_INIT_TIMEOUT_MS begrenzt, damit ein nicht antwortender MCP2515    *
            //* begin() nicht endlos blockiert.                                                   *
            //*************************************************************************************
            //* Return: false bei Initialisierungsfehler (auch bei Timeout)                       *
            //*************************************************************************************

            bool begin();

            //*************************************************************************************
            //* CAN-Nachricht senden (beim MCP2515-Backend wird nur TX-Puffer 0 genutzt)          *
            //*************************************************************************************
            //* msg: MBC_CAN_MSG_LEN Byte, siehe MBC_CAN_*-Indexkonstanten                        *
            //*************************************************************************************
            //* Return: false bei Sendefehler                                                     *
            //*************************************************************************************

            bool send(const uint8_t msg[MBC_CAN_MSG_LEN]);

            //*************************************************************************************
            //* CAN-Nachricht abholen (Polling-Betrieb)                                           *
            //*************************************************************************************
            //* msg: MBC_CAN_MSG_LEN Byte, siehe MBC_CAN_*-Indexkonstanten                        *
            //*************************************************************************************
            //* Return: false, wenn keine Nachricht vorliegt                                      *
            //*************************************************************************************

            bool receive(uint8_t msg[MBC_CAN_MSG_LEN]);

        private:

            enum MbcCanBackend { MBC_CAN_BACKEND_MCP2515 = 0, MBC_CAN_BACKEND_TWAI = 1 };

            MbcCanBackend _backend;
            MbcUtils     *_utils;
            bool          _initialized;

            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //+ MCP2515-Backend (SPI)                                                             +
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

                MbcSpiDevice _spiDevice;
                bool         _bothRx;    // Quasi-FIFO-Umschaltung zwischen RX-Puffer 0 und 1

                bool    beginMcp2515();
                bool    sendMcp2515(const uint8_t msg[MBC_CAN_MSG_LEN]);
                bool    receiveMcp2515(uint8_t msg[MBC_CAN_MSG_LEN]);

                void    writeReg(uint8_t addr, uint8_t data);
                uint8_t readReg(uint8_t addr);
                uint8_t readStatus(uint8_t type);

            #if defined(ESP32)

                //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                //+ TWAI-Backend (interner Controller, nur ESP32/ESP32-C3)                        +
                //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

                    uint8_t _pinTx;
                    uint8_t _pinRx;

                    bool beginTwai();
                    bool sendTwai(const uint8_t msg[MBC_CAN_MSG_LEN]);
                    bool receiveTwai(uint8_t msg[MBC_CAN_MSG_LEN]);

            #endif
    };

//------ Ende Klassendefinition MbcCan ------------------------------------------------------------

#endif  /* _MBCCAN_H_ */

//###### Ende Quellcode MbcCan.h ##################################################################
