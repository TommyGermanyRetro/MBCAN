//*************************************************************************************************
//* HEADER - ASCII-Hex-Rahmenprotokoll ueber UART zu einem WLAN-Bruecken-Modul (MBCANLib)         *
//*************************************************************************************************
//* Kodiert/dekodiert MBC_CAN_MSG_LEN-Byte-CAN-Nachrichten als 26-Zeichen-ASCII-Hex-Rahmen (je 2  *
//* Hex-Ziffern pro Byte in Nachrichtenreihenfolge, terminiert mit \r\n - 28 Byte gesamt) ueber   *
//* eine beliebige Stream-Schnittstelle, typischerweise eine Hardware-UART zu einem externen      *
//* WLAN-Bruecken-Modul (z.B. ESP8266 mit passender Gegenstelle-Firmware).                        *
//*                                                                                               *
//* Der Baustein ruft dabei bewusst kein eigenes begin() auf: wie bei SPI/Wire obliegt die        *
//* Bus-Initialisierung (z.B. Serial1.begin(125000)) dem Sketch, bevor dieser Baustein            *
//* konstruiert/genutzt wird. Die Gegenstelle muss mit 125000 Baud (8N1) laufen.                  *
//*************************************************************************************************
//* Datei: MbcWlanBridge.h                                                                        *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen:                                                                                  *
//* V1.0.0 - 05.08.2026 - Nullversion                                                             *
//*************************************************************************************************

#pragma once

//###### Start Quellcode MbcWlanBridge.h ##########################################################

#ifndef _MBCWLANBRIDGE_H_
#define _MBCWLANBRIDGE_H_     "V1.0.0 - 05.08.2026"

//------ Einbindung Bibliotheken ------------------------------------------------------------------

#include <Arduino.h>
#include "MbcCan.h"
#include "MbcMsgBridge.h"

//------ Start Klassendefinition MbcWlanBridge ----------------------------------------------------

    //* Klasse fuer <MbcWlanBridge>
    //* Ein Objekt = eine UART-Verbindung zu einem externen WLAN-Bruecken-Modul.

    class MbcWlanBridge : public MbcMsgBridge
    {
        public:

            //*************************************************************************************
            //* Konstruktor zur Deklaration von Objektvariablen                                   *
            //*************************************************************************************
            //* serial: bereits initialisierte Stream-Schnittstelle (z.B. Serial1, nach einem     *
            //* vorherigen Serial1.begin(125000) im Sketch)                                       *
            //*************************************************************************************

            explicit MbcWlanBridge(Stream &serial);

            //*************************************************************************************
            //* Nachricht als ASCII-Hex-Rahmen kodieren und senden                                *
            //*************************************************************************************
            //* msg: MBC_CAN_MSG_LEN Byte (siehe MbcCan MBC_CAN_*-Indexkonstanten)                *
            //*************************************************************************************

            void send(const uint8_t msg[MBC_CAN_MSG_LEN]) override;

            //*************************************************************************************
            //* Eingehende Bytes nicht-blockierend einlesen und bei vollstaendigem Rahmen         *
            //* dekodieren. Wird jeden loop()-Durchlauf aufgerufen.                               *
            //*************************************************************************************
            //* msg: MBC_CAN_MSG_LEN Byte, bei Return true mit der dekodierten Nachricht gefuellt *
            //*************************************************************************************
            //* Return: true, wenn ein vollstaendiger Rahmen dekodiert wurde                      *
            //*************************************************************************************

            bool check(uint8_t msg[MBC_CAN_MSG_LEN]) override;

        private:

            static const uint8_t FRAME_LEN = 2 * MBC_CAN_MSG_LEN + 2;   // 26 Hex-Zeichen + \r\n

            Stream *_serial;

            uint8_t _rxFrame[FRAME_LEN];
            uint8_t _rxIndex;
    };

//------ Ende Klassendefinition MbcWlanBridge -----------------------------------------------------

#endif  /* _MBCWLANBRIDGE_H_ */

//###### Ende Quellcode MbcWlanBridge.h ###########################################################
