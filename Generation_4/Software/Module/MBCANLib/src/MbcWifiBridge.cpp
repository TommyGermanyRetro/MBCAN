//*************************************************************************************************
//* CAN-Nachrichten-Bruecke ueber natives WLAN/TCP (MBCANLib, NUR ESP32)                          *
//*************************************************************************************************
//* Datei: MbcWifiBridge.cpp                                                                      *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen:                                                                                  *
//* V1.0.0 - 05.08.2026 - Nullversion                                                             *
//* V1.1.0 - 03.11.2026 - Bis zu 3 gleichzeitige TCP-Clients, Setup-Webportal zentriert           *
//*************************************************************************************************

//###### Start Quellcode MbcWifiBridge.cpp ########################################################

#include "MbcWifiBridge.h"

#if defined(ESP32)

#include <string.h>

//------ Start interne Konstanten -----------------------------------------------------------------

    static const char *AP_SSID     = "MBCAN Setup Portal";
    static const char *AP_PASSWORD = "1234567890";
    static const char *PREFS_NS    = "mbcwifi";

    static const char *PAGE_STYLE =
        "<style>body{max-width:420px;margin:0 auto;padding:1rem;font-family:'Segoe UI',Roboto,'Helvetica Neue',Arial,"
        "'Noto Sans','Liberation Sans';font-size:1rem;font-weight:400;line-height:1.5;"
        "color:#212529;background-color:#f5f5f5}h1{text-align:center;font-size:1.5rem}"
        "input,button{width:100%;box-sizing:border-box;margin-bottom:.5rem;padding:.4rem;font-size:1rem}</style>";

    // Wartezeit auf die Verbindung zum gespeicherten Netz, bevor auf den Setup-Access-Point
    // zurueckgefallen wird

    static const unsigned long CONNECT_TIMEOUT_MS = 30000;

//------ Ende interne Konstanten ------------------------------------------------------------------

//------ Start Klasse MbcWifiBridge ---------------------------------------------------------------

    //*********************************************************************************************
    //* Konstruktor zur Deklaration von Objektvariablen                                           *
    //*********************************************************************************************
    //* port:          TCP-Port, auf dem auf Client-Verbindungen gewartet wird                    *
    //* pinConnLed:    optional - LOW-aktiver nativer Pin, der anzeigt, ob aktuell ein TCP-Client *
    //* verbunden ist. MBC_WIFI_NO_LED: keine Verbindungs-LED verbaut. Wird von connLedDriver     *
    //* ueberstimmt, falls dieser angegeben ist.                                                  *
    //* connLedDriver: optional - Verbindungs-LED stattdessen ueber einen MbcDigitalOut ansteuern *
    //*********************************************************************************************

    MbcWifiBridge::MbcWifiBridge(uint16_t port, uint8_t pinConnLed, MbcDigitalOut *connLedDriver)
        : _port(port), _server(port), _nextSlot(0), _webServer(80), _isStation(false), _pinConnLed(pinConnLed),
          _connLedDriver(connLedDriver)
    {
    }

    //*********************************************************************************************
    //* Gespeicherte Zugangsdaten laden und verbinden, sonst Setup-Access-Point                   *
    //*********************************************************************************************
    //* utils: optional - waehrend des Verbindungsaufbaus mit update() weiterticken               *
    //*********************************************************************************************

    void MbcWifiBridge::begin(MbcUtils *utils)
    {
        if (_connLedDriver != nullptr)
        {
            _connLedDriver->write(false);   // LOW-aktiv: false = keine Verbindung
        }
        else if (_pinConnLed != MBC_WIFI_NO_LED)
        {
            pinMode(_pinConnLed, OUTPUT);
            digitalWrite(_pinConnLed, HIGH);   // LOW-aktiv: HIGH = keine Verbindung
        }

        Settings settings;
        loadSettings(settings);

        _isStation = false;

        if (settings.ssid.length() > 0)
        {
            Serial.print("[MbcWifiBridge] Verbinde mit \"");
            Serial.print(settings.ssid);
            Serial.println("\" ...");

            WiFi.mode(WIFI_STA);

            IPAddress ip, gateway, dns, subnet(255, 255, 255, 0);

            if (ip.fromString(settings.ip) && gateway.fromString(settings.gateway) && dns.fromString(settings.dns))
            {
                WiFi.config(ip, gateway, subnet, dns);
            }

            WiFi.begin(settings.ssid.c_str(), settings.password.c_str());

            // Modem-Sleep verzoegert eingehende Frames bis zum naechsten Beacon (~100 ms) und
            // bremst damit den CS3-Konfigwalk
            WiFi.setSleep(false);
            unsigned long start = millis();

            while ((WiFi.status() != WL_CONNECTED) && ((unsigned long) (millis() - start) < CONNECT_TIMEOUT_MS))
            {
                if (utils != nullptr) utils->update();
                delay(20);
            }

            _isStation = (WiFi.status() == WL_CONNECTED);

            if (_isStation)
            {
                Serial.print("[MbcWifiBridge] Verbunden, IP ");
                Serial.println(WiFi.localIP());
            }
            else
            {
                Serial.println("[MbcWifiBridge] Verbindung fehlgeschlagen (Timeout).");
            }
        }
        else
        {
            Serial.println("[MbcWifiBridge] Keine Zugangsdaten hinterlegt.");
        }

        if (_isStation)
        {
            _server.begin();

            Serial.print("[MbcWifiBridge] TCP-Server auf Port ");
            Serial.println(_port);
        }
        else
        {
            startAccessPoint();

            Serial.print("[MbcWifiBridge] Setup-Access-Point \"");
            Serial.print(AP_SSID);
            Serial.print("\" gestartet, IP ");
            Serial.println(WiFi.softAPIP());
        }

        _webServer.on("/", [this]() { handlePortal(); });
        _webServer.begin();

        Serial.println("[MbcWifiBridge] Setup-Webportal erreichbar unter \"/\".");
    }

    //*********************************************************************************************
    //* Setup-Access-Point aufspannen (keine/keine funktionierenden Zugangsdaten)                 *
    //*********************************************************************************************

    void MbcWifiBridge::startAccessPoint()
    {
        WiFi.mode(WIFI_AP);
        WiFi.softAP(AP_SSID, AP_PASSWORD);
    }

    //*********************************************************************************************
    //* Nachricht an alle verbundenen Clients senden (kein Client verbunden: verworfen)           *
    //*********************************************************************************************
    //* msg: MBC_CAN_MSG_LEN Byte (siehe MbcCan MBC_CAN_*-Indexkonstanten)                      *
    //*********************************************************************************************

    void MbcWifiBridge::send(const uint8_t msg[MBC_CAN_MSG_LEN])
    {
        sendToAll(msg, -1);
    }

    //*********************************************************************************************
    //* Nachricht an alle verbundenen Clients ausser einem Slot senden                            *
    //*********************************************************************************************
    //* msg:        MBC_CAN_MSG_LEN Byte                                                          *
    //* exceptSlot: Slot, der uebersprungen wird (Absender); -1: keiner                           *
    //*********************************************************************************************

    void MbcWifiBridge::sendToAll(const uint8_t msg[MBC_CAN_MSG_LEN], int8_t exceptSlot)
    {
        for (int8_t i = 0; i < MBC_WIFI_MAX_CLIENTS; i++)
        {
            if ((i != exceptSlot) && _slots[i].client.connected())
            {
                _slots[i].client.write(msg, MBC_CAN_MSG_LEN);
            }
        }
    }

    //*********************************************************************************************
    //* Wartende TCP-Verbindungen annehmen: freien Slot nutzen, sonst den am laengsten            *
    //* verbundenen Client ersetzen                                                               *
    //*********************************************************************************************

    void MbcWifiBridge::acceptClients()
    {
        while (_server.hasClient())
        {
            WiFiClient incoming = _server.available();

            if (!incoming) break;

            int8_t target = -1;

            for (int8_t i = 0; i < MBC_WIFI_MAX_CLIENTS; i++)
            {
                if (!_slots[i].client.connected()) { target = i; break; }
            }

            if (target < 0)
            {
                target = 0;

                for (int8_t i = 1; i < MBC_WIFI_MAX_CLIENTS; i++)
                {
                    if ((long) (_slots[target].connectedAt - _slots[i].connectedAt) < 0) continue;
                    target = i;
                }

                _slots[target].client.stop();
            }

            _slots[target].client      = incoming;
            _slots[target].client.setNoDelay(true);
            _slots[target].rxIndex     = 0;
            _slots[target].connectedAt = millis();

            Serial.print("[MbcWifiBridge] TCP-Client ");
            Serial.print(target);
            Serial.print(" verbunden: ");
            Serial.println(_slots[target].client.remoteIP());
        }
    }

    //*********************************************************************************************
    //* Webportal-Anfragen bedienen, neue Client-Verbindungen annehmen und eingehende Bytes       *
    //* aller Clients nicht-blockierend einlesen; bei vollstaendiger Nachricht dekodieren und an  *
    //* die uebrigen Clients weiterreichen                                                        *
    //*********************************************************************************************
    //* msg: MBC_CAN_MSG_LEN Byte, bei Return true mit der empfangenen Nachricht gefuellt         *
    //*********************************************************************************************
    //* Return: true, wenn eine vollstaendige Nachricht empfangen wurde                           *
    //*********************************************************************************************

    bool MbcWifiBridge::check(uint8_t msg[MBC_CAN_MSG_LEN])
    {
        _webServer.handleClient();

        if (!_isStation) return false;

        acceptClients();

        bool anyConnected = false;

        for (int8_t i = 0; i < MBC_WIFI_MAX_CLIENTS; i++)
        {
            bool connected = _slots[i].client.connected();

            if (_slots[i].wasConnected && !connected)
            {
                Serial.print("[MbcWifiBridge] TCP-Client ");
                Serial.print(i);
                Serial.println(" getrennt.");
            }

            _slots[i].wasConnected = connected;
            anyConnected = anyConnected || connected;
        }

        if (_connLedDriver != nullptr)
        {
            _connLedDriver->write(anyConnected);
        }
        else if (_pinConnLed != MBC_WIFI_NO_LED)
        {
            digitalWrite(_pinConnLed, anyConnected ? LOW : HIGH);
        }

        // Reihum pro Aufruf hoechstens eine Nachricht liefern; Restbytes bleiben im Socket-Puffer

        for (uint8_t n = 0; n < MBC_WIFI_MAX_CLIENTS; n++)
        {
            int8_t i = (int8_t) ((_nextSlot + n) % MBC_WIFI_MAX_CLIENTS);
            ClientSlot &s = _slots[i];

            if (!s.client.connected()) continue;

            while (s.client.available() > 0)
            {
                s.rxFrame[s.rxIndex] = (uint8_t) s.client.read();
                s.rxIndex++;

                if (s.rxIndex == MBC_CAN_MSG_LEN)
                {
                    memcpy(msg, s.rxFrame, MBC_CAN_MSG_LEN);
                    s.rxIndex = 0;
                    _nextSlot = (uint8_t) ((i + 1) % MBC_WIFI_MAX_CLIENTS);
                    sendToAll(msg, i);
                    return true;
                }
            }
        }

        return false;
    }

    //*********************************************************************************************
    //* Zugangsdaten aus dem NVS-Speicher laden (leer, wenn noch nichts hinterlegt)               *
    //*********************************************************************************************
    //* settings: Zielvariable                                                                    *
    //*********************************************************************************************

    void MbcWifiBridge::loadSettings(Settings &settings)
    {
        Preferences prefs;
        prefs.begin(PREFS_NS, true);

        settings.ssid     = prefs.getString("ssid", "");
        settings.password = prefs.getString("password", "");
        settings.ip       = prefs.getString("ip", "");
        settings.gateway  = prefs.getString("gateway", "");
        settings.dns      = prefs.getString("dns", "");

        prefs.end();
    }

    //*********************************************************************************************
    //* Zugangsdaten in den NVS-Speicher schreiben                                                *
    //*********************************************************************************************
    //* settings: zu schreibende Zugangsdaten                                                     *
    //*********************************************************************************************

    void MbcWifiBridge::saveSettings(const Settings &settings)
    {
        Preferences prefs;
        prefs.begin(PREFS_NS, false);

        prefs.putString("ssid", settings.ssid);
        prefs.putString("password", settings.password);
        prefs.putString("ip", settings.ip);
        prefs.putString("gateway", settings.gateway);
        prefs.putString("dns", settings.dns);

        prefs.end();
    }

    //*********************************************************************************************
    //* Setup-Webseite ausliefern (GET) bzw. neue Zugangsdaten uebernehmen (POST)                 *
    //*********************************************************************************************

    void MbcWifiBridge::handlePortal()
    {
        if (_webServer.method() == HTTP_POST)
        {
            Settings settings;

            settings.ssid     = _webServer.arg("ssid");
            settings.password = _webServer.arg("password");
            settings.ip       = _webServer.arg("ipv4");
            settings.gateway  = _webServer.arg("gwv4");
            settings.dns      = _webServer.arg("dnsv4");

            saveSettings(settings);

            Serial.print("[MbcWifiBridge] Neue Zugangsdaten gespeichert, SSID \"");
            Serial.print(settings.ssid);
            Serial.println("\" - Neustart in 5s.");

            String page = "<!doctype html><html lang='de'><head><meta charset='utf-8'>"
                          "<meta name='viewport' content='width=device-width, initial-scale=1'>"
                          "<title>Wifi Setup</title>" + String(PAGE_STYLE) + "</head><body>"
                          "<h1>MBCAN mbc-80 Wifi-Setup</h1>"
                          "<p>Die Einstellungen wurden gespeichert.<br>"
                          "Die Verbindung wird jetzt neu aufgebaut - dieses Fenster kann geschlossen werden.</p>"
                          "</body></html>";

            _webServer.send(200, "text/html", page);

            delay(5000);
            ESP.restart();
        }
        else
        {
            Settings settings;
            loadSettings(settings);

            _webServer.send(200, "text/html", setupPage(settings));
        }
    }

    //*********************************************************************************************
    //* HTML der Setup-Webseite mit den aktuell hinterlegten Werten vorbelegen                    *
    //*********************************************************************************************
    //* settings: aktuell hinterlegte Zugangsdaten                                                *
    //*********************************************************************************************
    //* Return: fertiges HTML-Dokument                                                            *
    //*********************************************************************************************

    String MbcWifiBridge::setupPage(const Settings &settings)
    {
        String page = "<!doctype html><html lang='de'><head><meta charset='utf-8'>"
                      "<meta name='viewport' content='width=device-width, initial-scale=1'>"
                      "<title>Wifi Setup</title>" + String(PAGE_STYLE) + "</head><body>"
                      "<h1>MBCAN mbc-80 Wifi-Setup</h1>"
                      "<p>SSID aktuell: <i>" + settings.ssid + "</i><br>"
                      "IPv4 aktuell: <i>" + settings.ip + "</i></p>"
                      "<form action='/' method='post'>"
                      "SSID neu:<br><input type='text' name='ssid' required><br>"
                      "Passwort neu:<br><input type='password' name='password' required><br>"
                      "IPv4 neu:<br><input type='text' name='ipv4' required><br>"
                      "Gateway neu:<br><input type='text' name='gwv4' required><br>"
                      "DNS neu:<br><input type='text' name='dnsv4' required><br><br>"
                      "<button type='submit'>Sichern</button>"
                      "</form>"
                      "<p><i>Subnet wird automatisch auf 255.255.255.0 gesetzt.</i></p>"
                      "</body></html>";

        return page;
    }

//------ Ende Klasse MbcWifiBridge ----------------------------------------------------------------

#endif  /* defined(ESP32) */

//###### Ende Quellcode MbcWifiBridge.cpp #########################################################
