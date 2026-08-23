//*************************************************************************************************
//* mbc-80 Gateway-Modul - ESP32-C3                                                               *
//*************************************************************************************************
//* WICHTIG - OTA-Partitionsschema: Dieser Sketch benoetigt zwei App-Partitionen (ota_0/ota_1),   *
//* siehe MbcOta.h. In der Arduino-IDE unter Tools/Werkzeuge -> Partition Scheme "Custom"         *
//* waehlen, damit die mitgelieferte partitions.csv (liegt im selben Sketch-Ordner) verwendet     *
//* wird - ohne das hat ein OTA-Update keine Zielpartition zum Beschreiben und schlaegt fehl.     *
//*                                                                                               *
//* MBCAN-Bus laeuft ueber die interne TWAI, CS2-Bus ueber einen externen MCP2515 (nur ein        *
//* TWAI-Controller vorhanden). LED gruen/rot, TCP-Verbindungs-LED und Steckernetzteil-Erkennung  *
//* haengen an einem MCP23S17 (GPIO0-10 sind hier zu knapp fuer alles nativ) - siehe              *
//* MbcMcp23x17-Kopfkommentar. Die Steckernetzteil-Erkennung liefert dadurch nur ein binaeres     *
//* HIGH/LOW statt einer echten Analogspannung, Aufbereitung (Komparator/Spannungsteiler) muss    *
//* extern erfolgen. ANNAHME: GPA2 HIGH = Steckernetzteil gesteckt, gegen die externe             *
//* Beschaltung verifizieren.                                                                     *
//*                                                                                               *
//* Nachrichten-Bruecke: MbcWifiBridge (natives WLAN/TCP) statt MbcWlanBridge. WLAN-Zugangsdaten  *
//* werden ueber ein Setup-Webportal konfiguriert: ohne funktionierende Zugangsdaten spannt das   *
//* Modul den Access-Point "MBCAN Setup Portal" (Passwort "1234567890") auf, unter                *
//* http://192.168.4.1/ koennen SSID/Passwort/feste IP eingetragen werden.                        *
//*                                                                                               *
//* WICHTIG: Tools/Werkzeuge -> "USB CDC On Boot" auf "Enabled" stellen, sonst bleiben alle       *
//* Serial-Ausgaben unsichtbar. SPI wird bewusst mit EXPLIZITEN Pins gestartet                    *
//* (SPI.begin(sck,miso,mosi,-1)) statt Boardstandard-Pins, da deren uebliche Default-Belegung    *
//* (oft GPIO4/5/6/7) sich sonst mit hier genutzten GPIOs ueberschneiden wuerde (stiller Hang     *
//* der SPI-Bus-Arbitrierung, ohne Fehlermeldung).                                                *
//*                                                                                               *
//* Board: "ESP32C3 Dev Module" (Espressif-esp32-Boardpaket). ANNAHME: generisches                *
//* ESP32-C3-DevKit-Pinout - vor dem Verdrahten gegen das jeweilige Board pruefen (GPIO9 =        *
//* BOOT-Taste, daher nicht belegt; GPIO2/GPIO8 sind Boot-Strapping-Pins, hier trotzdem fuer      *
//* TWAI-TX/Boot-Speicher-CS genutzt - die daran haengende Hardware darf beim Reset keinen        *
//* falschen Pegel ziehen).                                                                       *
//*                                                                                               *
//* Boot-Speicherchip: W25Q64-SPI-NOR-Flash (8 MByte), siehe MbcBootStorage-Kopfkommentar.        *
//*                                                                                               *
//* | Funktion                     | GPIO                  |                                      *
//* |-------------------------------|-----------------------|                                     *
//* | SPI SCK                       | GPIO4                 |                                     *
//* | SPI MISO                      | GPIO5                 |                                     *
//* | SPI MOSI                      | GPIO6                 |                                     *
//* | CS2-CS (MCP2515)              | GPIO7                 |                                     *
//* | Boot-Speicher-CS (W25Q64)     | GPIO8                 |                                     *
//* | MCP23S17-CS (LEDs/Plug-Erk.)  | GPIO1                 |                                     *
//* | MBCAN TWAI-TX                 | GPIO2                 |                                     *
//* | MBCAN TWAI-RX                 | GPIO3                 |                                     *
//* | OneWire                       | GPIO10                |                                     *
//* | ADC Busspannung               | GPIO0                 |                                     *
//* | LED gruen (MCP23S17 GPA0)     | -                     |                                     *
//* | LED rot (MCP23S17 GPA1)       | -                     |                                     *
//* | Steckernetzteil (MCP23S17 GPA2)| -                    |                                     *
//* | TCP-LED (MCP23S17 GPA3)       | -                     |                                     *
//*                                                                                               *
//* GPIO9, GPIO20/21 bleiben frei fuer zukuenftige Erweiterungen.                                 *
//*                                                                                               *
//* Kein Reset-Pin: ESP32 nutzt den Software-Reset des Cores (MbcPlatform).                       *
//*************************************************************************************************
//* Datei: Mbc80_ESP32C3.ino                                                                      *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen:                                                                                  *
//* V1.0.0 - 05.08.2026 - Nullversion                                                             *
//*************************************************************************************************

#include <SPI.h>
#include <Mbc80Com.h>
#include <MbcWifiBridge.h>
#include <MbcMcp23x17.h>
#include <MbcDigitalOut.h>
#include <MbcDigitalIn.h>

//------ Pinbelegung ------------------------------------------------------------------------------

    const uint8_t PIN_SPI_SCK         = 4;
    const uint8_t PIN_SPI_MISO        = 5;
    const uint8_t PIN_SPI_MOSI        = 6;
    const uint8_t PIN_CS_CAN_CS2      = 7;
    const uint8_t PIN_CS_BOOT_STORAGE = 8;
    const uint8_t PIN_CS_MCP23S17     = 1;
    const uint8_t PIN_TWAI_TX         = 2;   // fest, einheitlich ueber alle ESP32-C3-Sketches
    const uint8_t PIN_TWAI_RX         = 3;   // fest, einheitlich ueber alle ESP32-C3-Sketches
    const uint8_t PIN_ONEWIRE         = 10;
    const uint8_t PIN_ADC_VSPG        = 0;

//------ WLAN -------------------------------------------------------------------------------------

    const uint16_t WLAN_TCP_PORT = 15731;   // TCP-Port fuer die WLAN-Bruecke (MbcWifiBridge)

//------ Modulidentitaet (ASCII-Ziffern) ----------------------------------------------------------

    const uint8_t ART_NR[4] = { '8', '0', '1', '1' };   // [2]=Prozessor: 0=ATmega644P/1=ESP32-C3/2=RP2040-Pico/3=RP2040-Zero
    const uint8_t SW_VER[3] = { '1', '0', '0' };
    const uint8_t HW_VER[6] = { '2', '6', '0', '8', '0', '5' };   // Hardware 26.08.05

//------ Objekte ----------------------------------------------------------------------------------

    MbcMcp23x17    ioExpander(PIN_CS_MCP23S17);   // SPI-Konstruktor -> MCP23S17-Backend

    // GPIOA-Bits des Expanders: 0=LED gruen, 1=LED rot (beide HIGH-aktiv, wie die native
    // MbcUtils-Status-LED, Ausgaenge), 2=Steckernetzteil-Erkennung (Eingang, ANNAHME HIGH-aktiv),
    // 3=TCP-Verbindungs-LED (LOW-aktiv, Ausgang)

    MbcDigitalOut ledGruenOut(ioExpander, false, 0, false);
    MbcDigitalOut ledRotOut(ioExpander, false, 1, false);
    MbcDigitalIn  plugDetectorIn(ioExpander, false, 2, false);
    MbcDigitalOut tcpLedOut(ioExpander, false, 3, true);

    // genPurpSize=11 (9 Byte ungenutzte Luecke fuer PC_ARRAY_DATA-Kompatibilitaet zum
    // Parametriercenter + 2 Byte L88-Softwareversion, siehe Mbc80Com.cpp)

    static uint8_t sysArray[MBC_SYSARRAY_SIZE(11)];

    MbcUtils       utils(sysArray, 11, PIN_ADC_VSPG, 0, 0, 0, PIN_ONEWIRE, 2);  // Plug-/LED-Pins
                                                                             // ungenutzt, siehe
                                                                             // setPlugDetector()/
                                                                             // setLedDrivers()
    MbcCan         canCs2(PIN_CS_CAN_CS2, utils);                // externer MCP2515
    MbcCan         canMbcan(PIN_TWAI_TX, PIN_TWAI_RX, utils);    // interner TWAI-Controller
    MbcBootStorage bootStorage(PIN_CS_BOOT_STORAGE, MbcBootStorageBackend::FLASH_W25Q64);
    MbcWifiBridge  wifiBridge(WLAN_TCP_PORT, MBC_WIFI_NO_LED, &tcpLedOut);
    Mbc80Com       com(utils, canCs2, canMbcan, bootStorage, wifiBridge);

//------ Status der CAN-Initialisierung (fuer die wiederkehrende Fehlermeldung in loop()) ---------

    bool canCs2Ok    = false;
    bool canMbcanOk  = false;

//------ setup()/loop() ---------------------------------------------------------------------------

void setup()
{
    // SPI/Expander/LED-Treiber/MbcUtils muessen vor der ersten Serial-Ausgabe fertig begin()t
    // sein, damit beim anschliessenden Warten auf die USB-CDC-Verbindung tick-aware
    // (utils.update()) statt mit einem blockierenden delay() gewartet werden kann. Vorher waere
    // utils.update() nicht sicher nutzbar: setLedDrivers()/setPlugDetector() sind noch nicht
    // gesetzt, ein setLed() wuerde also auf die (hier ungenutzten) Platzhalter-Pins der
    // MbcUtils-Konstruktion durchgreifen - bei diesem Sketch waere das GPIO0, der ADC-
    // Busspannungs-Pin.

    SPI.begin(PIN_SPI_SCK, PIN_SPI_MISO, PIN_SPI_MOSI, -1);   // explizite Pins statt Boardstandard, siehe Kopfkommentar

    // GPIOA: Bit 2 Eingang (Steckernetzteil-Erkennung), Bits 0/1/3 Ausgang (LEDs)

    bool expanderOk = ioExpander.begin(0x04, 0x00, 0xff, 0x00);

    utils.setLedDrivers(&ledGruenOut, &ledRotOut);
    utils.setPlugDetector(&plugDetectorIn);

    utils.begin(ART_NR, SW_VER, HW_VER);

    Serial.begin(115200);

    // USB-CDC braucht nach dem Verbindungsaufbau etwas Zeit, bis der Host das Geraet erkannt
    // hat - sonst gehen die ersten Serial-Ausgaben verloren.

    unsigned long usbStart = millis();
    while ((unsigned long) (millis() - usbStart) < 2000) utils.update();

    Serial.println("\n[Mbc80] Start");
    Serial.println("[Mbc80] SPI gestartet");

    Serial.print("[Mbc80] MCP23S17 (LEDs/Plug-Erkennung): ");
    Serial.println(expanderOk ? "OK" : "FEHLER - Baustein an GPIO1 nicht gefunden/antwortet nicht");

    Serial.println("[Mbc80] MbcUtils initialisiert");

    canCs2Ok = canCs2.begin();
    Serial.print("[Mbc80] CS2-Bus (MCP2515): ");
    Serial.println(canCs2Ok ? "OK" : "FEHLER - MCP2515 an GPIO7 nicht gefunden/antwortet nicht");

    canMbcanOk = canMbcan.begin();
    Serial.print("[Mbc80] MBCAN-Bus (TWAI): ");
    Serial.println(canMbcanOk ? "OK" : "FEHLER - Transceiver/Verdrahtung an GPIO2/GPIO3 pruefen");

    bool bootStorageOk = bootStorage.begin();
    Serial.print("[Mbc80] W25Q64 (Boot-Speicher): ");
    Serial.println(bootStorageOk ? "OK" : "FEHLER - Baustein an GPIO8 nicht gefunden/falsche JEDEC-ID");

    wifiBridge.begin(&utils);

    com.begin();
    com.beginModule();

    // Warte, bis alle Statuswerte (Vspg/Plug/Temperatur) einmal ermittelt wurden - blinkt
    // waehrenddessen ca. 10 s durchgehend ORANGE (siehe MbcUtils::waitFilled())

    utils.waitFilled();

    Serial.println("[Mbc80] Bereit");
}

void loop()
{
    utils.update();
    utils.check();

    // Solange ein CAN-Kanal nicht initialisiert werden konnte, bleibt der Sketch lauffaehig
    // (WLAN-Portal, LED, etc. funktionieren weiter) - aber ohne diese Erinnerung waere davon
    // ausserhalb der Status-LED (MBC_LED_MCP_ERROR) nichts mehr zu sehen. Einmal je Sekunde
    // wiederholen, damit der Fehler auch bemerkt wird, wenn das Serial-Monitor-Fenster erst
    // nach dem Start geoeffnet wurde.

    if (utils.eventSekunde())
    {
        if (!canCs2Ok)   Serial.println("[Mbc80] FEHLER: CS2-Bus (MCP2515) weiterhin nicht gefunden!");
        if (!canMbcanOk) Serial.println("[Mbc80] FEHLER: MBCAN-Bus (TWAI) weiterhin nicht initialisiert!");
    }

    com.bridge();
}
