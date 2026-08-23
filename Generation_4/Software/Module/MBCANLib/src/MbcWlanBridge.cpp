//*************************************************************************************************
//* ASCII-Hex-Rahmenprotokoll ueber UART zu einem WLAN-Bruecken-Modul (MBCANLib)                  *
//*************************************************************************************************
//* Datei: MbcWlanBridge.cpp                                                                      *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen:                                                                                  *
//* V1.0.0 - 05.08.2026 - Nullversion                                                             *
//*************************************************************************************************

//###### Start Quellcode MbcWlanBridge.cpp ########################################################

#include "MbcWlanBridge.h"
#include "MbcUtils.h"

//------ Start Klasse MbcWlanBridge ---------------------------------------------------------------

    //*********************************************************************************************
    //* Konstruktor zur Deklaration von Objektvariablen                                           *
    //*********************************************************************************************
    //* serial: bereits initialisierte Stream-Schnittstelle (z.B. Serial1, nach einem vorherigen  *
    //* Serial1.begin(125000) im Sketch)                                                          *
    //*********************************************************************************************

    MbcWlanBridge::MbcWlanBridge(Stream &serial)
        : _serial(&serial), _rxIndex(0)
    {
    }

    //*********************************************************************************************
    //* Nachricht als ASCII-Hex-Rahmen kodieren und senden                                        *
    //*********************************************************************************************
    //* msg: MBC_CAN_MSG_LEN Byte (siehe MbcCan MBC_CAN_*-Indexkonstanten)                        *
    //*********************************************************************************************

    void MbcWlanBridge::send(const uint8_t msg[MBC_CAN_MSG_LEN])
    {
        uint8_t frame[FRAME_LEN];

        for (uint8_t index = 0; index < MBC_CAN_MSG_LEN; index++)
        {
            frame[2 * index]     = MbcUtils::hex2ascii((uint8_t) ((msg[index] & 0xf0) >> 4));
            frame[2 * index + 1] = MbcUtils::hex2ascii((uint8_t) (msg[index] & 0x0f));
        }

        frame[FRAME_LEN - 2] = 0x0d;   // \r
        frame[FRAME_LEN - 1] = 0x0a;   // \n

        _serial->write(frame, FRAME_LEN);
    }

    //*********************************************************************************************
    //* Eingehende Bytes nicht-blockierend einlesen und bei vollstaendigem Rahmen dekodieren      *
    //*********************************************************************************************
    //* msg: MBC_CAN_MSG_LEN Byte, bei Return true mit der dekodierten Nachricht gefuellt         *
    //*********************************************************************************************
    //* Return: true, wenn ein vollstaendiger Rahmen dekodiert wurde                              *
    //*********************************************************************************************

    bool MbcWlanBridge::check(uint8_t msg[MBC_CAN_MSG_LEN])
    {
        while (_serial->available() > 0)
        {
            uint8_t incoming = (uint8_t) _serial->read();

            _rxFrame[_rxIndex] = incoming;
            _rxIndex++;

            if (incoming == 0x0a)   // \n: Rahmenende
            {
                bool complete = (_rxIndex == FRAME_LEN);

                _rxIndex = 0;

                if (!complete) continue;   // unvollstaendiger/fehlerhafter Rahmen verworfen

                for (uint8_t index = 0; index < MBC_CAN_MSG_LEN; index++)
                {
                    msg[index] = (uint8_t) ((MbcUtils::ascii2hex(_rxFrame[2 * index]) << 4)
                                            | MbcUtils::ascii2hex(_rxFrame[2 * index + 1]));
                }

                return true;
            }
            else if (_rxIndex >= FRAME_LEN)
            {
                _rxIndex = 0;   // Ueberlaufschutz: Rahmen ohne LineFeed verworfen
            }
        }

        return false;
    }

//------ Ende Klasse MbcWlanBridge ----------------------------------------------------------------

//###### Ende Quellcode MbcWlanBridge.cpp #########################################################
