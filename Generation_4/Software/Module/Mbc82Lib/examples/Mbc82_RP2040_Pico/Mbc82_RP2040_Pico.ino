//*************************************************************************************************
//* mbc-82 mfx-Boosterlink - Raspberry Pi Pico (RP2040), Pinvorschlag                             *
//*************************************************************************************************
//* WICHTIG - OTA-Partitionsschema: Dieser Sketch benoetigt eine reservierte zweite (OTA-) Flash- *
//* Partition, siehe MbcOta.h. Anders als beim ESP32 gibt es dafuer KEINE partitions.csv - in der *
//* Arduino-IDE unter Tools/Werkzeuge -> Flash Size eine Variante mit "OTA" im Namen waehlen -    *
//* ohne das hat ein OTA-Update keine Zielpartition zum Beschreiben und schlaegt fehl.            *
//* Freie Pinwahl fuer dieses Board - ANNAHME, vor dem Verdrahten gegen das Pico-Pinout pruefen.  *
//* Kein Expander noetig: mbc-82s wenige zusaetzliche Signale (2 Ausgangsbits, 1 Kurzschluss-     *
//* Erkennungseingang) passen problemlos auf native GPIO, der Pico hat davon reichlich.           *
//* Der Boot-Speicherchip fuer das Firmware-Update-Staging ist hier ein W25Q64-SPI-NOR-Flash (8   *
//* MByte) statt des bisherigen 24LC1024-EEPROMs (128 KByte) - dessen Kapazitaet fuer groessere   *
//* Firmware-Images zu klein war. MbcBootStorage waehlt das Backend ueber den zweiten             *
//* Konstruktor-Parameter (FLASH_W25Q64), siehe dessen Kopfkommentar fuer Details zur Sektor-     *
//* Loesch-Strategie.                                                                             *
//* Kein Reset-Pin: RP2040 nutzt den Software-Reset des Cores (MbcPlatform).                      *
//* | Funktion                      | GPIO                                          |             *
//* |-------------------------------|-----------------------------------------------|             *
//* | SPI MOSI/MISO/SCK             | Standard-SPI (SPI0: 19/16/18)                 |             *
//* | MBCAN-CS (MCP2515)            | GPIO20                                        |             *
//* | Boot-Speicher-CS (W25Q64)     | GPIO22                                        |             *
//* | OneWire                       | GPIO13                                        |             *
//* | LED gruen                     | GPIO14                                        |             *
//* | LED rot                       | GPIO15                                        |             *
//* | ADC Busspannung               | GPIO26 (ADC0)                                 |             *
//* | ADC Steckernetzteil           | GPIO27 (ADC1)                                 |             *
//* | Booster ON-Bit                | GPIO6                                         |             *
//* | Booster SHORT-Bit             | GPIO7                                         |             *
//* | Kurzschluss-Erkennung         | GPIO8                                         |             *
//*************************************************************************************************
//* Datei: Mbc82_RP2040_Pico.ino                                                                  *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen: V1.0.0 - 26.08.2026 - Nullversion                                                *
//*************************************************************************************************

#include <SPI.h>
#include <Mbc82Com.h>
#include <MbcDigitalOut.h>

//------ Pinbelegung --------------------------------------------------------------------------------

    const uint8_t PIN_CS_CAN_MBCAN    = 20;
    const uint8_t PIN_CS_BOOT_STORAGE = 22;
    const uint8_t PIN_ONEWIRE         = 13;
    const uint8_t PIN_LED_GRUEN       = 14;
    const uint8_t PIN_LED_ROT         = 15;
    const uint8_t PIN_ADC_VSPG        = 26;
    const uint8_t PIN_ADC_PLUG        = 27;
    const uint8_t PIN_BOOSTER_ON      = 6;
    const uint8_t PIN_BOOSTER_SHORT   = 7;
    const uint8_t PIN_SHORT_DETECT    = 8;

//------ Modulidentitaet (ASCII-Ziffern) -----------------------------------------------------------

    // [2]=Prozessor: 0=ATmega644P/1=ESP32-C3/2=RP2040-Pico/3=RP2040-Zero

    const uint8_t ART_NR[4] = { '8', '2', '2', '1' };
    const uint8_t SW_VER[3] = { '1', '0', '0' };
    const uint8_t HW_VER[6] = { '2', '1', '0', '3', '0', '1' };   // Hardware 21.03.01

//------ Objekte ------------------------------------------------------------------------------------

    MbcDigitalOut pinBoosterOn(PIN_BOOSTER_ON), pinBoosterShort(PIN_BOOSTER_SHORT);

    // genPurpSize=4 (STATUS, SW_1, SW_2, SH), extraEepromSize=3 (siehe EEX_82_*-Konstanten
    // in Mbc82Com.cpp)

    static uint8_t sysArray[MBC_SYSARRAY_SIZE(4)];

    MbcUtils       utils(sysArray, 4, PIN_ADC_VSPG, PIN_ADC_PLUG, PIN_LED_GRUEN, PIN_LED_ROT,
                          PIN_ONEWIRE, 3);
    MbcCan         can(PIN_CS_CAN_MBCAN, utils);
    MbcBootStorage bootStorage(PIN_CS_BOOT_STORAGE, MbcBootStorageBackend::FLASH_W25Q64);
    Mbc82Com       com(utils, can, bootStorage, pinBoosterOn, pinBoosterShort,
                       PIN_SHORT_DETECT);

//------ Status der CAN-Initialisierung (fuer die wiederkehrende Fehlermeldung in loop()) ---------

    bool canOk = false;

//------ setup()/loop() -----------------------------------------------------------------------------

void setup()
{
    SPI.begin();

    utils.begin(ART_NR, SW_VER, HW_VER);

    Serial.begin(115200);

    // USB-CDC braucht nach dem Verbindungsaufbau etwas Zeit, bis der Host das Geraet
    // erkannt hat - sonst gehen die ersten Serial-Ausgaben verloren.

    unsigned long usbStart = millis();
    while ((unsigned long) (millis() - usbStart) < 2000) utils.update();

    Serial.println("\n[Mbc82] Start");

    canOk = can.begin();
    Serial.print("[Mbc82] MBCAN-Bus (MCP2515): ");
    Serial.println(canOk ? "OK" : "FEHLER - MCP2515 an GPIO20 nicht gefunden");

    bool bootStorageOk = bootStorage.begin();
    Serial.print("[Mbc82] W25Q64 (Boot-Speicher): ");
    Serial.println(bootStorageOk ? "OK" : "FEHLER - Baustein an GPIO22 nicht gefunden/falsche JEDEC-ID");

    com.begin();
    com.beginModule();

    // Warte, bis alle Statuswerte (Vspg/Plug/Temperatur) einmal ermittelt wurden - blinkt
    // waehrenddessen ca. 10 s durchgehend ORANGE (siehe MbcUtils::waitFilled())

    utils.waitFilled();

    Serial.println("[Mbc82] Bereit");
}

void loop()
{
    utils.update();
    utils.check();

    if (utils.eventSekunde() && !canOk)
    {
        Serial.println("[Mbc82] FEHLER: MBCAN-Bus (MCP2515) weiterhin nicht gefunden!");
    }

    com.update();
}

