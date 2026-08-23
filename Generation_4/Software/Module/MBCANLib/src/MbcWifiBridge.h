//*************************************************************************************************
//* HEADER - CAN-Nachrichten-Bruecke ueber natives WLAN/TCP (MBCANLib, NUR ESP32)                 *
//*************************************************************************************************
//* Ersetzt fuer Boards mit eigenem WLAN (ESP32) die serielle Bruecke zu einem externen           *
//* WLAN-Modul (MbcWlanBridge) durch einen direkt im Sketch laufenden TCP-Server: verbindet sich  *
//* mit dem konfigurierten Netz und nimmt auf einem TCP-Port Verbindungen von PC-Tools entgegen   *
//* (z.B. Rocrail). Anders als die serielle Bruecke ist keine ASCII-Hex-Kodierung noetig - es     *
//* werden die MBC_CAN_MSG_LEN Rohbytes je Nachricht direkt uebertragen (TCP liefert Bytes        *
//* bereits zuverlaessig und in Reihenfolge, eine Rahmenkennung ist daher nicht erforderlich).    *
//*                                                                                               *
//* Verwendet TCP-Port 15731.                                                                     *
//*                                                                                               *
//* Bringt ein eigenes WLAN-Setup-Webportal mit: Ohne hinterlegte Zugangsdaten (oder wenn die     *
//* Verbindung zum gespeicherten Netz fehlschlaegt) wird ein eigener Access-Point "MBCAN Setup    *
//* Portal" aufgespannt, ueber den SSID/Passwort/feste IP/Gateway/DNS per Webformular unter       *
//* http://192.168.4.1/ hinterlegt werden koennen. Die Zugangsdaten werden ueber die ESP32-eigene *
//* Preferences-Klasse (NVS-Speicher) abgelegt - das interne, von MbcUtils reservierte            *
//* EEPROM-Fenster (siehe MbcUtils::begin()) bleibt dadurch unberuehrt.                           *
//*                                                                                               *
//* Optional (pinConnLed, native Pin, oder connLedDriver, z.B. ein MbcMcp23x17-Bit) ein           *
//* LOW-aktiver LED-Ausgang, der anzeigt, ob aktuell eine TCP-Verbindung zu einem PC-Tool besteht.*
//*                                                                                               *
//* Gibt Diagnosemeldungen (Verbindungsaufbau, AP-Fallback, TCP-Client-Verbindung/-Trennung,      *
//* gespeicherte Portal-Einstellungen) unbedingt auf Serial aus - dafuer muss der Sketch vor      *
//* begin() bereits Serial.begin() aufgerufen haben. Unproblematisch, da diese Klasse             *
//* ESP32-spezifisch ist und Serial dort - anders als bei MbcWlanBridge - nicht durch die         *
//* eigentliche Nachrichten-Bruecke belegt wird (die laeuft hier ueber TCP, nicht UART).          *
//*************************************************************************************************
//* Datei: MbcWifiBridge.h                                                                        *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen:                                                                                  *
//* V1.0.0 - 05.08.2026 - Nullversion                                                             *
//*************************************************************************************************

#pragma once

//###### Start Quellcode MbcWifiBridge.h ##########################################################

#if defined(ESP32)

#ifndef _MBCWIFIBRIDGE_H_
#define _MBCWIFIBRIDGE_H_     "V1.0.0 - 05.08.2026"

//------ Einbindung Bibliotheken ------------------------------------------------------------------

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>
#include "MbcCan.h"
#include "MbcMsgBridge.h"
#include "MbcUtils.h"
#include "MbcDigitalOut.h"

//------ Start globale Deklarationen --------------------------------------------------------------

    // Sentinel fuer den optionalen pinConnLed-Konstruktorparameter: keine Verbindungs-LED verbaut

    #define MBC_WIFI_NO_LED   0xff

//------ Ende globale Deklarationen ---------------------------------------------------------------

//------ Start Klassendefinition MbcWifiBridge ----------------------------------------------------

    //* Klasse fuer <MbcWifiBridge>
    //* Ein Objekt = ein TCP-Server auf dem internen WLAN-Controller des ESP32, plus WLAN-Setup-
    //* Webportal (Access-Point-Fallback, wenn keine/keine funktionierenden Zugangsdaten
    //* hinterlegt sind).

    class MbcWifiBridge : public MbcMsgBridge
    {
        public:

            //*************************************************************************************
            //* Konstruktor zur Deklaration von Objektvariablen                                   *
            //*************************************************************************************
            //* port:       TCP-Port, auf dem auf Client-Verbindungen gewartet wird (Default      *
            //*             15731)                                                                *
            //* pinConnLed:    optional - LOW-aktiver nativer Pin, der anzeigt, ob aktuell        *
            //*                ein TCP-Client verbunden ist (LOW = verbunden). Default            *
            //*                MBC_WIFI_NO_LED: keine Verbindungs-LED verbaut. Wird von           *
            //*                connLedDriver ueberstimmt, falls dieser angegeben ist.             *
            //* connLedDriver: optional - Verbindungs-LED stattdessen ueber einen                 *
            //*                MbcDigitalOut ansteuern (z.B. ein MbcMcp23x17-Bit) statt eines     *
            //*                nativen Pins. Default nullptr.                                     *
            //*************************************************************************************

            explicit MbcWifiBridge(uint16_t port = 15731, uint8_t pinConnLed = MBC_WIFI_NO_LED,
                                    MbcDigitalOut *connLedDriver = nullptr);

            //*************************************************************************************
            //* Gespeicherte Zugangsdaten laden und verbinden. Schlaegt das fehl (keine           *
            //* hinterlegt oder Verbindung nicht moeglich, Timeout 30s), wird stattdessen der     *
            //* Setup-Access-Point "MBCAN Setup Portal" (Passwort "1234567890") aufgespannt - das *
            //* Webportal unter http://192.168.4.1/ ist in beiden Faellen erreichbar.             *
            //*************************************************************************************
            //* utils: optional - waehrend des Verbindungsaufbaus mit update() weiterticken       *
            //*************************************************************************************

            void begin(MbcUtils *utils = nullptr);

            //*************************************************************************************
            //* Nachricht an den verbundenen Client senden (kein Client verbunden: verworfen)     *
            //*************************************************************************************
            //* msg: MBC_CAN_MSG_LEN Byte (siehe MbcCan MBC_CAN_*-Indexkonstanten)                *
            //*************************************************************************************

            void send(const uint8_t msg[MBC_CAN_MSG_LEN]) override;

            //*************************************************************************************
            //* Webportal-Anfragen bedienen, neue Client-Verbindungen annehmen und eingehende     *
            //* Bytes nicht-blockierend einlesen; bei vollstaendiger Nachricht dekodieren. Wird   *
            //* jeden loop()-Durchlauf aufgerufen.                                                *
            //*************************************************************************************
            //* msg: MBC_CAN_MSG_LEN Byte, bei Return true mit der empfangenen Nachricht gefuellt *
            //*************************************************************************************
            //* Return: true, wenn eine vollstaendige Nachricht empfangen wurde                   *
            //*************************************************************************************

            bool check(uint8_t msg[MBC_CAN_MSG_LEN]) override;

        private:

            struct Settings
            {
                String ssid;
                String password;
                String ip;
                String gateway;
                String dns;
            };

            uint16_t   _port;
            WiFiServer _server;
            WiFiClient _client;
            WebServer  _webServer;
            bool       _isStation;
            uint8_t    _pinConnLed;
            MbcDigitalOut *_connLedDriver;
            bool       _clientWasConnected;

            uint8_t _rxFrame[MBC_CAN_MSG_LEN];
            uint8_t _rxIndex;

            void loadSettings(Settings &settings);
            void saveSettings(const Settings &settings);
            void startAccessPoint();
            void handlePortal();
            String setupPage(const Settings &settings);
    };

//------ Ende Klassendefinition MbcWifiBridge -----------------------------------------------------

#endif  /* _MBCWIFIBRIDGE_H_ */

#endif  /* defined(ESP32) */

//###### Ende Quellcode MbcWifiBridge.h ###########################################################
