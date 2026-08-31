//*************************************************************************************************
//* mbc-82 mfx-Boosterlink - Waveshare RP2040-Zero                                                *
//*************************************************************************************************
//* WICHTIG - OTA-Partitionsschema: siehe Mbc82_RP2040_Pico-Kopfkommentar (identisch).            *
//* LED gruen/rot UND die Steckernetzteil-Erkennung haengen an einem I2C-MCP23017 (GPIOA Bit      *
//* 0-2), da auf diesem pinknappen Board sonst kein zweiter analoger Kanal fuer die               *
//* Steckernetzteil-Erkennung frei waere - analog zum etablierten Muster in den anderen           *
//* RP2040-Zero-Sketches dieses Projekts (siehe z.B. Mbc91_RP2040_Zero). Die uebrigen             *
//* mbc-82-spezifischen Signale (2 Ausgangsbits, 1 Kurzschluss-Erkennungseingang) passen          *
//* problemlos auf native GPIO.                                                                   *
//* ANNAHME: generischer GPIO-Vorschlag fuer die Waveshare RP2040-Zero - vor dem Verdrahten gegen *
//* deren Pinout-Diagramm pruefen (insb. GPIO16, dort sitzt bei diesem Board das Onboard-         *
//* WS2812-RGB-LED und wird deshalb gemieden). SPI0 wird bewusst mit EXPLIZITEN Pins gestartet    *
//* (SPI.setSCK()/setTX()/setRX()/setCS() vor SPI.begin()), nicht mit den Boardstandard-Pins.     *
//* Kein Reset-Pin: RP2040 nutzt den Software-Reset des Cores (MbcPlatform).                      *
//* Der Boot-Speicherchip fuer das Firmware-Update-Staging ist ein W25Q64-SPI-NOR-Flash (8 MByte) *
//* statt eines 24LC1024-EEPROMs, siehe MbcBootStorage-Kopfkommentar.                             *
//* | Funktion                              | GPIO                  |                             *
//* |-------------------------------------------|-----------------------|                         *
//* | SPI0 SCK / MOSI / MISO                    | GPIO2 / GPIO3 / GPIO4 |                         *
//* | SPI0 CS (Standard, ungenutzt)             | GPIO5                 |                         *
//* | MBCAN-CS (MCP2515)                        | GPIO10                |                         *
//* | Boot-Speicher-CS (W25Q64)                 | GPIO12                |                         *
//* | I2C SDA/SCL (MCP23017 LED/Plug)           | GPIO6 / GPIO7         |                         *
//* | OneWire                                   | GPIO9                 |                         *
//* | ADC Busspannung                           | GPIO26 (ADC0)         |                         *
//* | Booster ON-Bit                            | GPIO0                 |                         *
//* | Booster SHORT-Bit                         | GPIO1                 |                         *
//* | Kurzschluss-Erkennung                     | GPIO8                 |                         *
//* | LED gruen (MCP23017 GPA0)                 | -                     |                         *
//* | LED rot (MCP23017 GPA1)                   | -                     |                         *
//* | Steckernetzteil (MCP23017 GPA2)           | -                     |                         *
//*************************************************************************************************
//* Datei: Mbc82_RP2040_Zero.ino                                                                  *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen: V1.0.0 - 26.08.2026 - Nullversion                                                *
//*************************************************************************************************

#include <SPI.h>
#include <Wire.h>
#include <Mbc82Com.h>
#include <MbcI2cBus.h>
#include <MbcMcp23x17.h>
#include <MbcDigitalIn.h>
#include <MbcDigitalOut.h>

//------ Pinbelegung --------------------------------------------------------------------------------

    const uint8_t PIN_SPI_SCK         = 2;
    const uint8_t PIN_SPI_MOSI        = 3;
    const uint8_t PIN_SPI_MISO        = 4;
    const uint8_t PIN_SPI_CS          = 5;   // SPI0-Standard-CS, ungenutzt
    const uint8_t PIN_CS_CAN_MBCAN    = 10;
    const uint8_t PIN_CS_BOOT_STORAGE = 12;
    const uint8_t PIN_I2C_SDA         = 6;
    const uint8_t PIN_I2C_SCL         = 7;
    const uint8_t PIN_ONEWIRE         = 9;
    const uint8_t PIN_ADC_VSPG        = 26;
    const uint8_t PIN_BOOSTER_ON      = 0;
    const uint8_t PIN_BOOSTER_SHORT   = 1;
    const uint8_t PIN_SHORT_DETECT    = 8;

//------ Modulidentitaet (ASCII-Ziffern) -----------------------------------------------------------

    // [2]=Prozessor: 0=ATmega644P/1=ESP32-C3/2=RP2040-Pico/3=RP2040-Zero

    const uint8_t ART_NR[4] = { '8', '2', '3', '1' };
    const uint8_t SW_VER[3] = { '1', '0', '0' };
    const uint8_t HW_VER[6] = { '2', '1', '0', '3', '0', '1' };   // Hardware 21.03.01

//------ Objekte ------------------------------------------------------------------------------------

    MbcI2cBus   i2c(Wire);
    MbcMcp23x17 ledExpander(i2c, 0x00);

    // GPIOA-Bits: 0=LED gruen, 1=LED rot (beide HIGH-aktiv), 2=Steckernetzteil-Erkennung
    // (Eingang, ANNAHME HIGH-aktiv), GPIOB unbenutzt

    MbcDigitalOut ledGruenOut(ledExpander, false, 0, false);
    MbcDigitalOut ledRotOut(ledExpander, false, 1, false);
    MbcDigitalIn  plugDetectorIn(ledExpander, false, 2, false);

    MbcDigitalOut pinBoosterOn(PIN_BOOSTER_ON), pinBoosterShort(PIN_BOOSTER_SHORT);

    // genPurpSize=4 (STATUS, SW_1, SW_2, SH), extraEepromSize=3 (siehe EEX_82_*-Konstanten
    // in Mbc82Com.cpp)

    static uint8_t sysArray[MBC_SYSARRAY_SIZE(4)];

    MbcUtils       utils(sysArray, 4, PIN_ADC_VSPG, 0, 0, 0, PIN_ONEWIRE, 3);   // Plug-/LED-
                                                                                // Pins ungenutzt, siehe
                                                                                // setPlugDetector()/setLedDrivers()
    MbcCan         can(PIN_CS_CAN_MBCAN, utils);
    MbcBootStorage bootStorage(PIN_CS_BOOT_STORAGE, MbcBootStorageBackend::FLASH_W25Q64);
    Mbc82Com       com(utils, can, bootStorage, pinBoosterOn, pinBoosterShort,
                       PIN_SHORT_DETECT);

//------ Status der CAN-Initialisierung (fuer die wiederkehrende Fehlermeldung in loop()) ---------

    bool canOk = false;

//------ setup()/loop() -----------------------------------------------------------------------------

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

    // GPIOA: Bit 0/1 Ausgang (LEDs), Bit 2 Eingang (Steckernetzteil), Rest unbenutzt

    bool ledExpanderOk = ledExpander.begin(0xf8, 0x00, 0x00, 0x00);

    utils.setLedDrivers(&ledGruenOut, &ledRotOut);
    utils.setPlugDetector(&plugDetectorIn);

    utils.begin(ART_NR, SW_VER, HW_VER);

    Serial.begin(115200);

    // USB-CDC braucht nach dem Verbindungsaufbau etwas Zeit, bis der Host das Geraet
    // erkannt hat - sonst gehen die ersten Serial-Ausgaben verloren.

    unsigned long usbStart = millis();
    while ((unsigned long) (millis() - usbStart) < 2000) utils.update();

    Serial.println("\n[Mbc82] Start");

    Serial.print("[Mbc82] MCP23017 (LED/Plug): ");
    Serial.println(ledExpanderOk ? "OK" : "FEHLER - Baustein an I2C-Adresse 0x00 nicht gefunden");

    canOk = can.begin();
    Serial.print("[Mbc82] MBCAN-Bus (MCP2515): ");
    Serial.println(canOk ? "OK" : "FEHLER - MCP2515 an GPIO10 nicht gefunden");

    bool bootStorageOk = bootStorage.begin();
    Serial.print("[Mbc82] W25Q64 (Boot-Speicher): ");
    Serial.println(bootStorageOk ? "OK"
                                  : "FEHLER - Baustein an GPIO12 nicht gefunden/falsche JEDEC-ID");

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

