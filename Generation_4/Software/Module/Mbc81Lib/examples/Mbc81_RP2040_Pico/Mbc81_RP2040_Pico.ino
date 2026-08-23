//*************************************************************************************************
//* mbc-81 Stellpult mit 32 Tastern, 16 Stellungs-LEDs - Raspberry Pi Pico (RP2040)               *
//*************************************************************************************************
//* WICHTIG - OTA-Partitionsschema: Dieser Sketch benoetigt eine reservierte zweite (OTA-) Flash- *
//* Partition, siehe MbcOta.h. Anders als beim ESP32 gibt es dafuer KEINE partitions.csv - in der *
//* Arduino-IDE unter Tools/Werkzeuge -> Flash Size eine Variante mit "OTA" im Namen waehlen -    *
//* ohne das hat ein OTA-Update keine Zielpartition zum Beschreiben und schlaegt fehl.            *
//*                                                                                               *
//* Freie Pinwahl - ANNAHME, vor dem Verdrahten gegen das Pico-Pinout pruefen.                    *
//*                                                                                               *
//* 4x PCF8574 (Tastermatrix) haengen am I2C-Bus, genau wie beim ATmega644P-Sketch. Fuer die 16   *
//* Stellungs-LEDs sind auf dem Pico genug native Pins frei - anders als bei den pinknappen       *
//* Boards RP2040-Zero/ESP32-C3 dieser Bibliothek, die dafuer einen zusaetzlichen I2C-MCP23017    *
//* nutzen, siehe deren Kopfkommentare.                                                           *
//*                                                                                               *
//* Der Boot-Speicherchip fuer das Firmware-Update-Staging ist hier ein W25Q64-SPI-NOR-Flash (8   *
//* MByte) statt des bisherigen 24LC1024-EEPROMs (128 KByte), siehe MbcBootStorage-Kopfkommentar. *
//*                                                                                               *
//* Kein Reset-Pin: RP2040 nutzt den Software-Reset des Cores (MbcPlatform).                      *
//*                                                                                               *
//* | Funktion                       | GPIO                          |                            *
//* |---------------------------------|--------------------------------|                          *
//* | SPI MOSI/MISO/SCK               | Standard-SPI (SPI0: 19/16/18) |                           *
//* | MBCAN-CS (MCP2515)              | GPIO20                        |                           *
//* | Boot-Speicher-CS (W25Q64)       | GPIO22                        |                           *
//* | OneWire                         | GPIO13                        |                           *
//* | LED gruen                       | GPIO14                        |                           *
//* | LED rot                         | GPIO15                        |                           *
//* | ADC Busspannung                 | GPIO26 (ADC0)                 |                           *
//* | ADC Steckernetzteil             | GPIO27 (ADC1)                 |                           *
//* | I2C SDA/SCL (4x PCF8574)        | GPIO4 / GPIO5                 |                           *
//* | L01..L16                        | siehe PIN_LED[] im Sketch     |                           *
//*************************************************************************************************
//* Datei: Mbc81_RP2040_Pico.ino                                                                  *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen:                                                                                  *
//* V1.0.0 - 22.08.2026 - Nullversion                                                             *
//*************************************************************************************************

#include <SPI.h>
#include <Wire.h>
#include <Mbc81Com.h>
#include <MbcDigitalOut.h>
#include <MbcI2cBus.h>
#include <MbcPcf8574.h>

//------ Pinbelegung ------------------------------------------------------------------------------

    const uint8_t PIN_CS_CAN_MBCAN    = 20;
    const uint8_t PIN_CS_BOOT_STORAGE = 22;
    const uint8_t PIN_ONEWIRE         = 13;
    const uint8_t PIN_LED_GRUEN       = 14;
    const uint8_t PIN_LED_ROT         = 15;
    const uint8_t PIN_ADC_VSPG        = 26;
    const uint8_t PIN_ADC_PLUG        = 27;

    const uint8_t PIN_I2C_SDA         = 4;
    const uint8_t PIN_I2C_SCL         = 5;

    // L01..L16 (Index 0..15 = MM-Adressoffset, siehe Mbc81Com.h)

    const uint8_t PIN_LED[16] =
        { 0, 1, 2, 3, 6, 7, 8, 9, 10, 11, 12, 17, 21, 23, 24, 25 };

//------ Modulidentitaet (ASCII-Ziffern) ----------------------------------------------------------

    // [2]=Prozessor: 0=ATmega644P/1=ESP32-C3/2=RP2040-Pico/3=RP2040-Zero

    const uint8_t ART_NR[4] = { '8', '1', '2', '2' };
    const uint8_t SW_VER[3] = { '1', '0', '0' };
    const uint8_t HW_VER[6] = { '2', '4', '0', '1', '0', '6' };   // Hardware 24.01.06

//------ Objekte ----------------------------------------------------------------------------------

    MbcI2cBus  i2cBus(Wire);
    MbcPcf8574 tasterChip0(i2cBus, 0);
    MbcPcf8574 tasterChip1(i2cBus, 1);
    MbcPcf8574 tasterChip2(i2cBus, 2);
    MbcPcf8574 tasterChip3(i2cBus, 3);

    MbcPcf8574 *tasterChips[4] = { &tasterChip0, &tasterChip1, &tasterChip2, &tasterChip3 };

    MbcDigitalOut led00(PIN_LED[0]),  led01(PIN_LED[1]),  led02(PIN_LED[2]),  led03(PIN_LED[3]);
    MbcDigitalOut led04(PIN_LED[4]),  led05(PIN_LED[5]),  led06(PIN_LED[6]),  led07(PIN_LED[7]);
    MbcDigitalOut led08(PIN_LED[8]),  led09(PIN_LED[9]),  led10(PIN_LED[10]), led11(PIN_LED[11]);
    MbcDigitalOut led12(PIN_LED[12]), led13(PIN_LED[13]), led14(PIN_LED[14]), led15(PIN_LED[15]);

    MbcDigitalOut *leds[16] =
        { &led00, &led01, &led02, &led03, &led04, &led05, &led06, &led07,
          &led08, &led09, &led10, &led11, &led12, &led13, &led14, &led15 };

    // genPurpSize=3 (MM-Addr H/L, Nutzung), extraEepromSize=3 (siehe EEX_81_*-Konstanten in
    // Mbc81Com.cpp)

    static uint8_t sysArray[MBC_SYSARRAY_SIZE(3)];

    MbcUtils       utils(sysArray, 3, PIN_ADC_VSPG, PIN_ADC_PLUG, PIN_LED_GRUEN, PIN_LED_ROT,
                          PIN_ONEWIRE, 3);
    MbcCan         can(PIN_CS_CAN_MBCAN, utils);
    MbcBootStorage bootStorage(PIN_CS_BOOT_STORAGE, MbcBootStorageBackend::FLASH_W25Q64);
    Mbc81Com       com(utils, can, bootStorage, tasterChips, leds);

//------ Status der CAN-Initialisierung (fuer die wiederkehrende Fehlermeldung in loop()) ---------

    bool canOk = false;

//------ setup()/loop() ---------------------------------------------------------------------------

void setup()
{
    SPI.begin();

    Wire.setSDA(PIN_I2C_SDA);
    Wire.setSCL(PIN_I2C_SCL);
    Wire.begin();

    utils.begin(ART_NR, SW_VER, HW_VER);

    Serial.begin(115200);

    // USB-CDC braucht nach dem Verbindungsaufbau etwas Zeit, bis der Host das Geraet
    // erkannt hat - sonst gehen die ersten Serial-Ausgaben verloren.

    unsigned long usbStart = millis();
    while ((unsigned long) (millis() - usbStart) < 2000) utils.update();

    Serial.println("\n[Mbc81] Start");

    canOk = can.begin();
    Serial.print("[Mbc81] MBCAN-Bus (MCP2515): ");
    Serial.println(canOk ? "OK" : "FEHLER - MCP2515 an GPIO20 nicht gefunden");

    bool bootStorageOk = bootStorage.begin();
    Serial.print("[Mbc81] W25Q64 (Boot-Speicher): ");
    Serial.println(bootStorageOk ? "OK" : "FEHLER - Baustein an GPIO22 nicht gefunden/falsche JEDEC-ID");

    com.begin();
    com.beginModule();

    // Warte, bis alle Statuswerte (Vspg/Plug/Temperatur) einmal ermittelt wurden - blinkt
    // waehrenddessen ca. 10 s durchgehend ORANGE (siehe MbcUtils::waitFilled())

    utils.waitFilled();

    Serial.println("[Mbc81] Bereit");
}

void loop()
{
    utils.update();
    utils.check();

    if (utils.eventSekunde() && !canOk)
    {
        Serial.println("[Mbc81] FEHLER: MBCAN-Bus (MCP2515) weiterhin nicht gefunden!");
    }

    com.update();
}
