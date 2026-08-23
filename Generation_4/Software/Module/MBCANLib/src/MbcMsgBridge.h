//*************************************************************************************************
//* HEADER - Abstrakte Schnittstelle fuer CAN-Nachrichten-Bruecken (MBCANLib)                     *
//*************************************************************************************************
//* Gemeinsame Schnittstelle fuer austauschbare "externe Versandwege" einer CAN-Nachricht (z.B.   *
//* MbcWlanBridge - UART/ASCII-Hex zu einem externen WLAN-Modul, oder MbcWifiBridge - natives     *
//* WLAN via TCP auf ESP32). Ermoeglicht modulspezifischen Unterklassen, den tatsaechlich         *
//* genutzten Bruecken-Baustein austauschbar zu halten (welche Implementierung ein Sketch         *
//* konstruiert, entscheidet allein der Sketch).                                                  *
//*************************************************************************************************
//* Datei: MbcMsgBridge.h                                                                         *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen:                                                                                  *
//* V1.0.0 - 05.08.2026 - Nullversion                                                             *
//*************************************************************************************************

#pragma once

//###### Start Quellcode MbcMsgBridge.h ###########################################################

#ifndef _MBCMSGBRIDGE_H_
#define _MBCMSGBRIDGE_H_     "V1.0.0 - 05.08.2026"

//------ Einbindung Bibliotheken ------------------------------------------------------------------

#include <Arduino.h>
#include "MbcCan.h"

//------ Start Klassendefinition MbcMsgBridge -----------------------------------------------------

    //* Klasse fuer <MbcMsgBridge>
    //* Rein abstrakte Basisklasse (Interface) fuer austauschbare CAN-Nachrichten-Bruecken.

    class MbcMsgBridge
    {
        public:

            virtual ~MbcMsgBridge() {}

            //*************************************************************************************
            //* Nachricht ueber die Bruecke versenden                                             *
            //*************************************************************************************
            //* msg: MBC_CAN_MSG_LEN Byte (siehe MbcCan MBC_CAN_*-Indexkonstanten)                *
            //*************************************************************************************

            virtual void send(const uint8_t msg[MBC_CAN_MSG_LEN]) = 0;

            //*************************************************************************************
            //* Nicht-blockierend auf eine eingehende Nachricht pruefen. Wird jeden               *
            //* loop()-Durchlauf aufgerufen.                                                      *
            //*************************************************************************************
            //* msg: MBC_CAN_MSG_LEN Byte, bei Return true mit der empfangenen Nachricht gefuellt *
            //*************************************************************************************
            //* Return: true, wenn eine vollstaendige Nachricht empfangen wurde                   *
            //*************************************************************************************

            virtual bool check(uint8_t msg[MBC_CAN_MSG_LEN]) = 0;
    };

//------ Ende Klassendefinition MbcMsgBridge ------------------------------------------------------

#endif  /* _MBCMSGBRIDGE_H_ */

//###### Ende Quellcode MbcMsgBridge.h ############################################################
