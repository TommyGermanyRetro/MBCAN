//*************************************************************************************************
//* mbc-80 Gateway-Modul - Raspberry Pi Pico (RP2040), Pinvorschlag                               *
//*************************************************************************************************
//* WICHTIG - OTA-Partitionsschema: Dieser Sketch benoetigt eine reservierte zweite (OTA-)        *
//* Flash-Partition, siehe MbcOta.h. Anders als beim ESP32 gibt es dafuer KEINE partitions.csv -  *
//* in der Arduino-IDE unter Tools/Werkzeuge -> Flash Size eine Variante mit "OTA" im Namen       *
//* waehlen (beim 2-MByte-Standard-Pico sinngemaess "2MB (Sketch: ..., OTA: ...)", genauer        *
//* Wortlaut je nach arduino-pico-Version) - ohne das hat ein OTA-Update keine Zielpartition zum  *
//* Beschreiben und schlaegt fehl.                                                                *
//*                                                                                               *
//* Freie Pinwahl fuer dieses Board - ANNAHME, vor dem Verdrahten gegen das Pico-Pinout pruefen.  *
//*                                                                                               *
//* Der Boot-Speicherchip fuer das Firmware-Update-Staging ist hier ein W25Q64-SPI-NOR-Flash (8   *
//* MByte) statt des bisherigen 24LC1024-EEPROMs (128 KByte) - dessen Kapazitaet fuer groessere   *
//* Firmware-Images zu klein war. MbcBootStorage waehlt das Backend ueber den zweiten             *
//* Konstruktor-Parameter (FLASH_W25Q64), siehe dessen Kopfkommentar fuer Details zur             *
//* Sektor-Loesch-Strategie.                                                                      *
//*                                                                                               *
//* | Funktion              | GPIO                        |                                       *
//* |------------------------|-----------------------------|                                      *
//* | SPI MOSI/MISO/SCK      | Standard-SPI (SPI0: 19/16/18)|                                     *
//* | MBCAN-CS (MCP2515)     | GPIO20                      |                                      *
//* | CS2-CS (MCP2515)       | GPIO21                      |                                      *
//* | Boot-Speicher-CS (W25Q64) | GPIO22                   |                                      *
//* | OneWire                | GPIO13                      |                                      *
//* | LED gruen               | GPIO14                      |                                     *
//* | LED rot                 | GPIO15                      |                                     *
//* | ADC Busspannung        | GPIO26 (ADC0)               |                                      *
//* | ADC Steckernetzteil    | GPIO27 (ADC1)               |                                      *
//* | WLAN-UART (125 kBit/s) | Serial1 (Standard: UART0, GPIO0/1)                                 *
//*                                                                                               *
//* Serial (natives USB, 115200 Baud) ist ausschliesslich fuer Debug-Ausgaben reserviert          *
//* (Setup-Fortschritt) - unabhaengig von Serial1, das die WLAN-Bruecke belegt, daher kein        *
//* Konflikt.                                                                                     *
//*                                                                                               *
//* Kein Reset-Pin: RP2040 nutzt den Software-Reset des Cores (MbcPlatform).                      *
//*************************************************************************************************
//* Datei: Mbc80_RP2040_Pico.ino                                                                  *
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

    const uint8_t PIN_CS_CAN_MBCAN    = 20;
    const uint8_t PIN_CS_CAN_CS2      = 21;
    const uint8_t PIN_CS_BOOT_STORAGE = 22;
    const uint8_t PIN_ONEWIRE         = 13;
    const uint8_t PIN_LED_GRUEN       = 14;
    const uint8_t PIN_LED_ROT         = 15;
    const uint8_t PIN_ADC_VSPG        = 26;
    const uint8_t PIN_ADC_PLUG        = 27;

//------ Modulidentitaet (ASCII-Ziffern) ----------------------------------------------------------

    const uint8_t ART_NR[4] = { '8', '0', '2', '1' };   // [2]=Prozessor: 0=ATmega644P/1=ESP32-C3/2=RP2040-Pico/3=RP2040-Zero
    const uint8_t SW_VER[3] = { '1', '0', '0' };
    const uint8_t HW_VER[6] = { '2', '1', '1', '0', '0', '1' };   // Hardware 21.10.01

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
    // MbcUtils schon vor der ersten Serial-Ausgabe begin()en, damit beim anschliessenden
    // Warten auf die USB-CDC-Verbindung tick-aware (utils.update()) statt mit einem blockierenden
    // delay() gewartet werden kann - die Status-LED blinkt so auch waehrend dieser Wartezeit
    // normal.

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
    Serial.println(canCs2Ok ? "OK" : "FEHLER - MCP2515 an GPIO21 nicht gefunden/antwortet nicht");

    canMbcanOk = canMbcan.begin();
    Serial.print("[Mbc80] MBCAN-Bus (MCP2515): ");
    Serial.println(canMbcanOk ? "OK" : "FEHLER - MCP2515 an GPIO20 nicht gefunden/antwortet nicht");

    bool bootStorageOk = bootStorage.begin();
    Serial.print("[Mbc80] W25Q64 (Boot-Speicher): ");
    Serial.println(bootStorageOk ? "OK" : "FEHLER - Baustein an GPIO22 nicht gefunden/falsche JEDEC-ID");

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

    if (utils.eventSekunde())
    {
        if (!canCs2Ok)   Serial.println("[Mbc80] FEHLER: CS2-Bus (MCP2515) weiterhin nicht gefunden!");
        if (!canMbcanOk) Serial.println("[Mbc80] FEHLER: MBCAN-Bus (MCP2515) weiterhin nicht gefunden!");
    }

    com.bridge();
}
