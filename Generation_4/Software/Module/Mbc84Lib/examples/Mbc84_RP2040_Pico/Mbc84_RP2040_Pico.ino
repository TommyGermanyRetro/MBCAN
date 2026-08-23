//*************************************************************************************************
//* mbc-84 4-fach Servo-/Weichendecoder - Raspberry Pi Pico (RP2040), Pinvorschlag                *
//*************************************************************************************************
//* WICHTIG - OTA-Partitionsschema: Dieser Sketch benoetigt eine reservierte zweite (OTA-) Flash- *
//* Partition, siehe MbcOta.h. Anders als beim ESP32 gibt es dafuer KEINE partitions.csv - in der *
//* Arduino-IDE unter Tools/Werkzeuge -> Flash Size eine Variante mit "OTA" im Namen waehlen -    *
//* ohne das hat ein OTA-Update keine Zielpartition zum Beschreiben und schlaegt fehl.  Freie     *
//* Pinwahl fuer dieses Board - ANNAHME, vor dem Verdrahten gegen das Pico-Pinout pruefen. Die 4  *
//* Servosignale MUESSEN native Pins sein (Servo-Bibliothek), liegen deshalb NICHT auf dem        *
//* Expander. Die 8 Stellpulteingaenge und 8 Positions-LEDs (16 Signale, exakt ein MCP23017)      *
//* haengen an einem I2C-Expander statt an nativen Pins - genug native Pins waeren zwar           *
//* vorhanden, I2C spart hier aber 14 Pins gegenueber nativer Verdrahtung.  Der Boot-Speicherchip *
//* fuer das Firmware-Update-Staging ist ein W25Q64-SPI-NOR-Flash (8 MByte) statt eines           *
//* 24LC1024-EEPROMs, siehe MbcBootStorage-Kopfkommentar.  Kein Reset-Pin: RP2040 nutzt den       *
//* Software-Reset des Cores (MbcPlatform).                                                       *
//* | Funktion                       | GPIO                          |                            *
//* |----------------------------------|-------------------------------|                          *
//* | SPI MOSI/MISO/SCK                | Standard-SPI (SPI0: 19/16/18) |                          *
//* | MBCAN-CS (MCP2515)               | GPIO20                        |                          *
//* | Boot-Speicher-CS (W25Q64)        | GPIO22                        |                          *
//* | OneWire                          | GPIO13                        |                          *
//* | LED gruen                        | GPIO14                        |                          *
//* | LED rot                          | GPIO15                        |                          *
//* | ADC Busspannung                  | GPIO26 (ADC0)                 |                          *
//* | ADC Steckernetzteil              | GPIO27 (ADC1)                 |                          *
//* | I2C SDA/SCL (MCP23017)           | GPIO4 / GPIO5                 |                          *
//* | Servo 0/1/2/3                    | GPIO6/GPIO7/GPIO8/GPIO9       |                          *
//* | Stellpult (MCP23017 GPIOA 0-7)   | -                             |                          *
//* | Positions-LEDs (MCP23017 GPIOB)  | -                             |                          *
//*************************************************************************************************
//* Datei: Mbc84_RP2040_Pico.ino                                                                  *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen:                                                                                  *
//* V1.0.0 - 18.08.2026 - Nullversion                                                             *
//*************************************************************************************************

#include <SPI.h>
#include <Wire.h>
#include <Mbc84Com.h>
#include <MbcI2cBus.h>
#include <MbcMcp23x17.h>
#include <MbcDigitalIn.h>
#include <MbcDigitalOut.h>

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

    const uint8_t SERVO_PINS[4] = { 6, 7, 8, 9 };

//------ Modulidentitaet (ASCII-Ziffern) ----------------------------------------------------------

    // [2]=Prozessor: 0=ATmega644P/1=ESP32-C3/2=RP2040-Pico/3=RP2040-Zero

    const uint8_t ART_NR[4] = { '8', '4', '2', '1' };
    const uint8_t SW_VER[3] = { '1', '0', '0' };
    const uint8_t HW_VER[6] = { '2', '4', '0', '1', '0', '6' };   // Hardware 24.01.06

//------ Objekte ----------------------------------------------------------------------------------

    MbcI2cBus   i2c(Wire);
    MbcMcp23x17 ioExpander(i2c, 0x00);

    // GPIOA Bit 0-7: Stellpult Rot0,Gruen0,Rot1,Gruen1,Rot2,Gruen2,Rot3,Gruen3 (Eingang,
    // aktiv-LOW mit PullUp - siehe Mbc84Com.h-Klassenkommentar). GPIOB Bit 0-7:
    // Positions-LEDs, gleiche Reihenfolge (Ausgang, aktiv-LOW)

    MbcDigitalIn r0(ioExpander, false, 0, true), g0(ioExpander, false, 1, true);
    MbcDigitalIn r1(ioExpander, false, 2, true), g1(ioExpander, false, 3, true);
    MbcDigitalIn r2(ioExpander, false, 4, true), g2(ioExpander, false, 5, true);
    MbcDigitalIn r3(ioExpander, false, 6, true), g3(ioExpander, false, 7, true);

    MbcDigitalIn *switchboard[8] = { &r0, &g0, &r1, &g1, &r2, &g2, &r3, &g3 };

    MbcDigitalOut led0(ioExpander, true, 0, true), led1(ioExpander, true, 1, true);
    MbcDigitalOut led2(ioExpander, true, 2, true), led3(ioExpander, true, 3, true);
    MbcDigitalOut led4(ioExpander, true, 4, true), led5(ioExpander, true, 5, true);
    MbcDigitalOut led6(ioExpander, true, 6, true), led7(ioExpander, true, 7, true);

    MbcDigitalOut *led[8] = { &led0, &led1, &led2, &led3, &led4, &led5, &led6, &led7 };

    // genPurpSize=497 (voller Servoweg, siehe Mbc84Com.h), extraEepromSize=497 (deckt sich damit)

    static uint8_t sysArray[MBC_SYSARRAY_SIZE(497)];

    MbcUtils       utils(sysArray, 497, PIN_ADC_VSPG, PIN_ADC_PLUG, PIN_LED_GRUEN, PIN_LED_ROT,
                          PIN_ONEWIRE, 497);
    MbcCan         can(PIN_CS_CAN_MBCAN, utils);
    MbcBootStorage bootStorage(PIN_CS_BOOT_STORAGE, MbcBootStorageBackend::FLASH_W25Q64);
    Mbc84Com       com(utils, can, bootStorage, SERVO_PINS, switchboard, led);

//------ Status der CAN-Initialisierung (fuer die wiederkehrende Fehlermeldung in loop()) ---------

    bool canOk = false;

//------ setup()/loop() ---------------------------------------------------------------------------

void setup()
{
    SPI.begin();

    Wire.setSDA(PIN_I2C_SDA);
    Wire.setSCL(PIN_I2C_SCL);
    Wire.begin();

    bool ioExpanderOk = ioExpander.begin(0xff, 0x00, 0x00, 0x00);   // GPIOA Eingang, GPIOB Ausgang

    utils.begin(ART_NR, SW_VER, HW_VER);

    Serial.begin(115200);

    // USB-CDC braucht nach dem Verbindungsaufbau etwas Zeit, bis der Host das Geraet
    // erkannt hat - sonst gehen die ersten Serial-Ausgaben verloren.

    unsigned long usbStart = millis();
    while ((unsigned long) (millis() - usbStart) < 2000) utils.update();

    Serial.println("\n[Mbc84] Start");

    Serial.print("[Mbc84] MCP23017 (Stellpult/LEDs): ");
    Serial.println(ioExpanderOk ? "OK"
                                 : "FEHLER - Baustein an SDA=GPIO4/SCL=GPIO5 nicht gefunden");

    canOk = can.begin();
    Serial.print("[Mbc84] MBCAN-Bus (MCP2515): ");
    Serial.println(canOk ? "OK" : "FEHLER - MCP2515 an GPIO20 nicht gefunden");

    bool bootStorageOk = bootStorage.begin();
    Serial.print("[Mbc84] W25Q64 (Boot-Speicher): ");
    Serial.println(bootStorageOk ? "OK"
                                  : "FEHLER - Baustein an GPIO22 nicht gefunden/falsche JEDEC-ID");

    com.begin();
    com.beginModule();

    // Warte, bis alle Statuswerte (Vspg/Plug/Temperatur) einmal ermittelt wurden - blinkt
    // waehrenddessen ca. 10 s durchgehend ORANGE (siehe MbcUtils::waitFilled())

    utils.waitFilled();

    Serial.println("[Mbc84] Bereit");
}

void loop()
{
    utils.update();
    utils.check();

    if (utils.eventSekunde() && !canOk)
    {
        Serial.println("[Mbc84] FEHLER: MBCAN-Bus (MCP2515) weiterhin nicht gefunden!");
    }

    com.update();
}
