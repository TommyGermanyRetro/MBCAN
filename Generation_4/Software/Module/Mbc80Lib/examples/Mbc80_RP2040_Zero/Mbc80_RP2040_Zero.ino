//*************************************************************************************************
//* mbc-80 Gateway-Modul - Waveshare RP2040-Zero                                                  *
//*************************************************************************************************
//* WICHTIG - OTA-Partitionsschema: Dieser Sketch benoetigt eine reservierte zweite (OTA-)        *
//* Flash-Partition, siehe MbcOta.h. Anders als beim ESP32 gibt es dafuer KEINE partitions.csv -  *
//* in der Arduino-IDE unter Tools/Werkzeuge -> Flash Size eine Variante mit "OTA" im Namen       *
//* waehlen (beim 2-MByte-Standard-Flash dieses Boards sinngemaess "2MB (Sketch: ..., OTA: ...)", *
//* genauer Wortlaut je nach arduino-pico-Version) - ohne das hat ein OTA-Update keine            *
//* Zielpartition zum Beschreiben und schlaegt fehl.                                              *
//*                                                                                               *
//* LED gruen/rot (sonst native MbcUtils-Status-LED) und die Steckernetzteil-Erkennung sind hier  *
//* NATIV verkabelt - anders als beim ESP32-C3-Sketch, wo ein MCP23S17-Expander noetig ist, weil  *
//* dieses Board genug freie GPIOs dafuer frei hat. Die Steckernetzteil-Erkennung liest damit     *
//* eine echte Analogspannung per analogRead() aus (GPIO26-29 sind die ADC-Kanaele auf dem        *
//* RP2040), wie beim RP-Pi-Pico-Sketch, nicht nur ein digitales HIGH/LOW wie beim                *
//* MCP23S17-Ansatz.                                                                              *
//*                                                                                               *
//* mbc-80 ist ein reines Gateway-Modul ohne eigene Schalt-/Tasten-I/Os - ausser                  *
//* LED/Steckernetzteil-Erkennung (jetzt nativ) und den CS-/OneWire-Pins (bleiben nativ) bleibt   *
//* nur die zwingend native UART fuer die WLAN-Bruecke uebrig.                                    *
//*                                                                                               *
//* ANNAHME: generischer GPIO-Vorschlag fuer die Waveshare RP2040-Zero - vor dem Verdrahten gegen *
//* deren Pinout-Diagramm pruefen (insb. GPIO16, dort sitzt bei diesem Board das                  *
//* Onboard-WS2812-RGB-LED und wird deshalb gemieden).                                            *
//*                                                                                               *
//* | Funktion                        | GPIO                  |                                   *
//* |---------------------------------|-----------------------|                                   *
//* | SPI0 SCK                        | GPIO2                 |                                   *
//* | SPI0 MOSI                       | GPIO3                 |                                   *
//* | SPI0 MISO                       | GPIO4                 |                                   *
//* | SPI0 CS (Standard, ungenutzt)   | GPIO5                 |                                   *
//* | MBCAN-CS (MCP2515)              | GPIO10                |                                   *
//* | CS2-CS (MCP2515)                | GPIO11                |                                   *
//* | Boot-Speicher-CS (W25Q64)       | GPIO12                |                                   *
//* | OneWire                         | GPIO9                 |                                   *
//* | ADC Busspannung                 | GPIO26 (ADC0)         |                                   *
//* | LED gruen                       | GPIO7                 |                                   *
//* | LED rot                         | GPIO8                 |                                   *
//* | ADC Steckernetzteil             | GPIO27 (ADC1)         |                                   *
//* | WLAN-UART (125 kBit/s)          | Serial1 (Standard: UART0, GPIO0/1)                        *
//*                                                                                               *
//* SPI0 wird bewusst mit EXPLIZITEN Pins gestartet (SPI.setSCK()/setTX()/setRX()/setCS() vor     *
//* SPI.begin()), nicht mit den Boardstandard-Pins - deren tatsaechliche Default-Belegung fuer    *
//* diese Boardvariante war nicht zuverlaessig zu ermitteln, siehe SPI-Hinweis beim ESP32-Sketch  *
//* fuer die Art des Risikos bei impliziten SPI-Standardpins.                                     *
//*                                                                                               *
//* GPIO6, GPIO13, GPIO14, GPIO15, GPIO28, GPIO29 bleiben frei fuer zukuenftige Erweiterungen.    *
//*                                                                                               *
//* Serial (natives USB, 115200 Baud) ist ausschliesslich fuer Debug-Ausgaben reserviert          *
//* (Setup-Fortschritt) - unabhaengig von Serial1, das die WLAN-Bruecke belegt, daher kein        *
//* Konflikt.                                                                                     *
//*                                                                                               *
//* Kein Reset-Pin: RP2040 nutzt den Software-Reset des Cores (MbcPlatform).                      *
//*                                                                                               *
//* Der Boot-Speicherchip fuer das Firmware-Update-Staging ist hier ein W25Q64-SPI-NOR-Flash (8   *
//* MByte) statt des bisherigen 24LC1024-EEPROMs (128 KByte) - dessen Kapazitaet fuer groessere   *
//* Firmware-Images zu klein war. MbcBootStorage waehlt das Backend ueber den zweiten             *
//* Konstruktor-Parameter (FLASH_W25Q64), siehe dessen Kopfkommentar fuer Details zur             *
//* Sektor-Loesch-Strategie.                                                                      *
//*************************************************************************************************
//* Datei: Mbc80_RP2040_Zero.ino                                                                  *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen:                                                                                  *
//* V1.0.0 - 05.08.2026 - Nullversion                                                             *
//*************************************************************************************************

#include <SPI.h>
#include <Mbc80Com.h>
#include <MbcWlanBridge.h>

//------ Pinbelegung ------------------------------------------------------------------------------

    const uint8_t PIN_SPI_SCK        = 2;
    const uint8_t PIN_SPI_MOSI       = 3;
    const uint8_t PIN_SPI_MISO       = 4;
    const uint8_t PIN_SPI_CS         = 5;   // SPI0-Standard-CS, ungenutzt - CS wird je Geraet manuell gesetzt
    const uint8_t PIN_CS_CAN_MBCAN    = 10;
    const uint8_t PIN_CS_CAN_CS2      = 11;
    const uint8_t PIN_CS_BOOT_STORAGE = 12;
    const uint8_t PIN_ONEWIRE         = 9;
    const uint8_t PIN_ADC_VSPG        = 26;
    const uint8_t PIN_LED_GRUEN       = 7;
    const uint8_t PIN_LED_ROT         = 8;
    const uint8_t PIN_ADC_PLUG        = 27;

//------ Modulidentitaet (ASCII-Ziffern) ----------------------------------------------------------

    const uint8_t ART_NR[4] = { '8', '0', '3', '1' };   // [2]=Prozessor: 0=ATmega644P/1=ESP32-C3/2=RP2040-Pico/3=RP2040-Zero
    const uint8_t SW_VER[3] = { '1', '0', '0' };
    const uint8_t HW_VER[6] = { '2', '6', '0', '8', '0', '5' };   // Hardware 26.08.05

//------ Objekte ----------------------------------------------------------------------------------

    // genPurpSize=11 (9 Byte ungenutzte Luecke fuer PC_ARRAY_DATA-Kompatibilitaet zum
    // Parametriercenter + 2 Byte L88-Softwareversion, siehe Mbc80Com.cpp), extraEepromSize=2

    static uint8_t sysArray[MBC_SYSARRAY_SIZE(11)];

    MbcUtils       utils(sysArray, 11, PIN_ADC_VSPG, PIN_ADC_PLUG, PIN_LED_GRUEN, PIN_LED_ROT,
                          PIN_ONEWIRE, 2);
    MbcCan         canCs2(PIN_CS_CAN_CS2, utils);
    MbcCan         canMbcan(PIN_CS_CAN_MBCAN, utils);
    MbcBootStorage bootStorage(PIN_CS_BOOT_STORAGE, MbcBootStorageBackend::FLASH_W25Q64);
    MbcWlanBridge  wlan(Serial1);
    Mbc80Com       com(utils, canCs2, canMbcan, bootStorage, wlan);

//------ Status der CAN-Initialisierung (fuer die wiederkehrende Fehlermeldung in loop()) ---------

    bool canCs2Ok   = false;
    bool canMbcanOk = false;

//------ setup()/loop() ---------------------------------------------------------------------------

void setup()
{
    // SPI/MbcUtils muessen vor der ersten Serial-Ausgabe fertig begin()t sein, damit beim
    // anschliessenden Warten auf die USB-CDC-Verbindung tick-aware (utils.update()) statt mit
    // einem blockierenden delay() gewartet werden kann.

    SPI.setSCK(PIN_SPI_SCK);
    SPI.setTX(PIN_SPI_MOSI);
    SPI.setRX(PIN_SPI_MISO);
    SPI.setCS(PIN_SPI_CS);
    SPI.begin();

    utils.begin(ART_NR, SW_VER, HW_VER);

    Serial.begin(115200);

    // USB-CDC braucht nach dem Verbindungsaufbau etwas Zeit, bis der Host das Geraet erkannt
    // hat - sonst gehen die ersten Serial-Ausgaben verloren.

    unsigned long usbStart = millis();
    while ((unsigned long) (millis() - usbStart) < 2000) utils.update();

    Serial.println("\n[Mbc80] Start");
    Serial.println("[Mbc80] SPI gestartet");

    Serial.println("[Mbc80] MbcUtils initialisiert");

    Serial1.begin(125000);   // 8N1 - Gegenstelle: externes WLAN-Modul mit mbc_80_esp8266_1_05
    Serial.println("[Mbc80] WLAN-UART (Serial1) gestartet");

    canCs2Ok = canCs2.begin();
    Serial.print("[Mbc80] CS2-Bus (MCP2515): ");
    Serial.println(canCs2Ok ? "OK" : "FEHLER - MCP2515 an GPIO11 nicht gefunden/antwortet nicht");

    canMbcanOk = canMbcan.begin();
    Serial.print("[Mbc80] MBCAN-Bus (MCP2515): ");
    Serial.println(canMbcanOk ? "OK" : "FEHLER - MCP2515 an GPIO10 nicht gefunden/antwortet nicht");

    bool bootStorageOk = bootStorage.begin();
    Serial.print("[Mbc80] W25Q64 (Boot-Speicher): ");
    Serial.println(bootStorageOk ? "OK" : "FEHLER - Baustein an GPIO12 nicht gefunden/falsche JEDEC-ID");

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

    // Solange ein CAN-Kanal nicht initialisiert werden konnte, bleibt der Sketch lauffaehig -
    // aber ohne diese Erinnerung waere davon ausserhalb der Status-LED (MBC_LED_MCP_ERROR)
    // nichts mehr zu sehen. Einmal je Sekunde wiederholen, damit der Fehler auch bemerkt wird,
    // wenn das Serial-Monitor-Fenster erst nach dem Start geoeffnet wurde.
    //
    // WICHTIG: eigene, unabhaengige millis()-Zeitbasis statt utils.eventSekunde() - letzteres
    // ist ein einmalig auslesendes Flag, das Mbc80Com::bridge() intern selbst fuer die
    // zyklische Rueckmeldeaufforderung auf dem MBCAN-Bus abfragt (1x/s, siehe dortigen
    // Kopfkommentar). Wuerde hier zusaetzlich utils.eventSekunde() abgefragt, wird das Flag
    // vor bridge() verbraucht und die Rueckmeldeaufforderung feuert nie mehr - genau dieser
    // Bug war hardwarebestaetigt vorhanden (siehe Mbc80_ESP32C3_HW260805.ino), bevor auf
    // millis() umgestellt wurde.

    static unsigned long lastErrorReminder = 0;

    if ((unsigned long) (millis() - lastErrorReminder) >= 1000)
    {
        lastErrorReminder = millis();

        if (!canCs2Ok)   Serial.println("[Mbc80] FEHLER: CS2-Bus (MCP2515) weiterhin nicht gefunden!");
        if (!canMbcanOk) Serial.println("[Mbc80] FEHLER: MBCAN-Bus (MCP2515) weiterhin nicht gefunden!");
    }

    com.bridge();
}
