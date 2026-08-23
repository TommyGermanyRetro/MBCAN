//*************************************************************************************************
//* mbc-81 Stellpult mit 32 Tastern, 16 Stellungs-LEDs - Waveshare RP2040-Zero                    *
//*************************************************************************************************
//* WICHTIG - OTA-Partitionsschema: siehe Mbc81_RP2040_Pico-Kopfkommentar (identisch).            *
//*                                                                                               *
//* LED gruen/rot UND die Steckernetzteil-Erkennung haengen an einem I2C-MCP23017 (GPIOA Bit      *
//* 0-2). Die 16 Stellungs-LEDs haengen an einem ZWEITEN I2C-MCP23017 (GPIOA/GPIOB, exakt 16      *
//* Signale). Die 4 PCF8574 der Tastermatrix teilen sich denselben I2C-Bus mit allen 3 Expandern  *
//* (unterschiedliche Adressen, siehe Tabelle).                                                   *
//*                                                                                               *
//* ANNAHME: generischer GPIO-Vorschlag fuer die Waveshare RP2040-Zero - vor dem Verdrahten gegen *
//* deren Pinout-Diagramm pruefen (insb. GPIO16, dort sitzt bei diesem Board das Onboard-         *
//* WS2812-RGB-LED und wird deshalb gemieden). SPI0 wird bewusst mit EXPLIZITEN Pins gestartet    *
//* (SPI.setSCK()/setTX()/setRX()/setCS() vor SPI.begin()), nicht mit den Boardstandard-Pins.     *
//*                                                                                               *
//* Kein Reset-Pin: RP2040 nutzt den Software-Reset des Cores (MbcPlatform). Der Boot-            *
//* Speicherchip fuer das Firmware-Update-Staging ist ein W25Q64-SPI-NOR-Flash (8 MByte) statt    *
//* eines 24LC1024-EEPROMs, siehe MbcBootStorage-Kopfkommentar.                                   *
//*                                                                                               *
//* | Funktion                              | GPIO                          |                     *
//* |-----------------------------------------|-------------------------------|                   *
//* | SPI0 SCK / MOSI / MISO                   | GPIO2 / GPIO3 / GPIO4         |                  *
//* | SPI0 CS (Standard, ungenutzt)             | GPIO5                        |                  *
//* | MBCAN-CS (MCP2515)                        | GPIO10                      |                   *
//* | Boot-Speicher-CS (W25Q64)                 | GPIO12                      |                   *
//* | OneWire                                   | GPIO9                       |                   *
//* | ADC Busspannung                           | GPIO26 (ADC0)               |                   *
//* | I2C SDA/SCL (3x Expander + 4x PCF8574)    | GPIO6 / GPIO7               |                   *
//* | LED gruen (Expander1 GPA0)                | -                           |                   *
//* | LED rot (Expander1 GPA1)                  | -                           |                   *
//* | Steckernetzteil (Expander1 GPA2)          | -                           |                   *
//* | L01..L08 (Expander2 GPIOA)                | -                           |                   *
//* | L09..L16 (Expander2 GPIOB)                | -                           |                   *
//* | Taster-Matrix (4x PCF8574)                | -                           |                   *
//*************************************************************************************************
//* Datei: Mbc81_RP2040_Zero.ino                                                                  *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen:                                                                                  *
//* V1.0.0 - 22.08.2026 - Nullversion                                                             *
//*************************************************************************************************

#include <SPI.h>
#include <Wire.h>
#include <Mbc81Com.h>
#include <MbcI2cBus.h>
#include <MbcMcp23x17.h>
#include <MbcDigitalIn.h>
#include <MbcDigitalOut.h>
#include <MbcPcf8574.h>

//------ Pinbelegung ------------------------------------------------------------------------------

    const uint8_t PIN_SPI_SCK         = 2;
    const uint8_t PIN_SPI_MOSI        = 3;
    const uint8_t PIN_SPI_MISO        = 4;
    const uint8_t PIN_SPI_CS          = 5;   // SPI0-Standard-CS, ungenutzt
    const uint8_t PIN_CS_CAN_MBCAN    = 10;
    const uint8_t PIN_CS_BOOT_STORAGE = 12;
    const uint8_t PIN_ONEWIRE         = 9;
    const uint8_t PIN_ADC_VSPG        = 26;   // ADC0 - Steckernetzteil-Erkennung laeuft ueber
                                               // den Expander, siehe setPlugDetector()

    const uint8_t PIN_I2C_SDA         = 6;
    const uint8_t PIN_I2C_SCL         = 7;

//------ Modulidentitaet (ASCII-Ziffern) ----------------------------------------------------------

    // [2]=Prozessor: 0=ATmega644P/1=ESP32-C3/2=RP2040-Pico/3=RP2040-Zero

    const uint8_t ART_NR[4] = { '8', '1', '3', '2' };
    const uint8_t SW_VER[3] = { '1', '0', '0' };
    const uint8_t HW_VER[6] = { '2', '4', '0', '1', '0', '6' };   // Hardware 24.01.06

//------ Objekte ----------------------------------------------------------------------------------

    MbcI2cBus  i2cBus(Wire);

    // I2C-Adressen: 0...3 = PCF8574 Tastermatrix, 4 = statusExpander (LED/Plug), 5 = ledExpander
    // (16 Stellungs-LEDs) - alle 6 Bausteine teilen sich denselben Bus

    MbcPcf8574 tasterChip0(i2cBus, 0);
    MbcPcf8574 tasterChip1(i2cBus, 1);
    MbcPcf8574 tasterChip2(i2cBus, 2);
    MbcPcf8574 tasterChip3(i2cBus, 3);

    MbcPcf8574 *tasterChips[4] = { &tasterChip0, &tasterChip1, &tasterChip2, &tasterChip3 };

    MbcMcp23x17 statusExpander(i2cBus, 4);
    MbcMcp23x17 ledExpander(i2cBus, 5);

    // statusExpander GPIOA-Bits: 0=LED gruen, 1=LED rot (beide HIGH-aktiv), 2=Steckernetzteil-
    // Erkennung (Eingang, ANNAHME HIGH-aktiv)

    MbcDigitalOut ledGruenOut(statusExpander, false, 0, false);
    MbcDigitalOut ledRotOut(statusExpander, false, 1, false);
    MbcDigitalIn  plugDetectorIn(statusExpander, false, 2, false);

    // ledExpander GPIOA Bit 0-7 = L01..L08, GPIOB Bit 0-7 = L09..L16 (alle HIGH-aktiv)

    MbcDigitalOut led00(ledExpander, false, 0, false), led01(ledExpander, false, 1, false);
    MbcDigitalOut led02(ledExpander, false, 2, false), led03(ledExpander, false, 3, false);
    MbcDigitalOut led04(ledExpander, false, 4, false), led05(ledExpander, false, 5, false);
    MbcDigitalOut led06(ledExpander, false, 6, false), led07(ledExpander, false, 7, false);
    MbcDigitalOut led08(ledExpander, true,  0, false), led09(ledExpander, true,  1, false);
    MbcDigitalOut led10(ledExpander, true,  2, false), led11(ledExpander, true,  3, false);
    MbcDigitalOut led12(ledExpander, true,  4, false), led13(ledExpander, true,  5, false);
    MbcDigitalOut led14(ledExpander, true,  6, false), led15(ledExpander, true,  7, false);

    MbcDigitalOut *leds[16] =
        { &led00, &led01, &led02, &led03, &led04, &led05, &led06, &led07,
          &led08, &led09, &led10, &led11, &led12, &led13, &led14, &led15 };

    // genPurpSize=3 (MM-Addr H/L, Nutzung), extraEepromSize=3 (siehe EEX_81_*-Konstanten in
    // Mbc81Com.cpp)

    static uint8_t sysArray[MBC_SYSARRAY_SIZE(3)];

    MbcUtils       utils(sysArray, 3, PIN_ADC_VSPG, 0, 0, 0, PIN_ONEWIRE, 3);   // Plug-/LED-Pins
                                                                     // ungenutzt, siehe
                                                                     // setPlugDetector()/
                                                                     // setLedDrivers()
    MbcCan         can(PIN_CS_CAN_MBCAN, utils);
    MbcBootStorage bootStorage(PIN_CS_BOOT_STORAGE, MbcBootStorageBackend::FLASH_W25Q64);
    Mbc81Com       com(utils, can, bootStorage, tasterChips, leds);

//------ Status der CAN-Initialisierung (fuer die wiederkehrende Fehlermeldung in loop()) ---------

    bool canOk = false;

//------ setup()/loop() ---------------------------------------------------------------------------

void setup()
{
    SPI.setSCK(PIN_SPI_SCK);
    SPI.setTX(PIN_SPI_MOSI);
    SPI.setRX(PIN_SPI_MISO);
    SPI.setCS(PIN_SPI_CS);
    SPI.begin();

    Wire.setSDA(PIN_I2C_SDA);
    Wire.setSCL(PIN_I2C_SCL);
    Wire.begin();

    // statusExpander GPIOA: Bit 0/1 Ausgang (LEDs), Bit 2 Eingang (Steckernetzteil), Rest
    // unbenutzt

    bool statusExpanderOk = statusExpander.begin(0xf8, 0x00, 0x00, 0x00);

    // ledExpander GPIOA/GPIOB komplett Ausgang

    bool ledExpanderOk = ledExpander.begin(0x00, 0x00, 0x00, 0x00);

    utils.setLedDrivers(&ledGruenOut, &ledRotOut);
    utils.setPlugDetector(&plugDetectorIn);

    utils.begin(ART_NR, SW_VER, HW_VER);

    Serial.begin(115200);

    // USB-CDC braucht nach dem Verbindungsaufbau etwas Zeit, bis der Host das Geraet
    // erkannt hat - sonst gehen die ersten Serial-Ausgaben verloren.

    unsigned long usbStart = millis();
    while ((unsigned long) (millis() - usbStart) < 2000) utils.update();

    Serial.println("\n[Mbc81] Start");

    Serial.print("[Mbc81] MCP23017 (LED/Plug): ");
    Serial.println(statusExpanderOk ? "OK" : "FEHLER - Baustein an I2C-Adresse 0x24 nicht gefunden");

    Serial.print("[Mbc81] MCP23017 (Stellungs-LEDs): ");
    Serial.println(ledExpanderOk ? "OK" : "FEHLER - Baustein an I2C-Adresse 0x25 nicht gefunden");

    canOk = can.begin();
    Serial.print("[Mbc81] MBCAN-Bus (MCP2515): ");
    Serial.println(canOk ? "OK" : "FEHLER - MCP2515 an GPIO10 nicht gefunden");

    bool bootStorageOk = bootStorage.begin();
    Serial.print("[Mbc81] W25Q64 (Boot-Speicher): ");
    Serial.println(bootStorageOk ? "OK" : "FEHLER - Baustein an GPIO12 nicht gefunden/falsche JEDEC-ID");

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
