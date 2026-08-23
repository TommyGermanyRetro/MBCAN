//*************************************************************************************************
//* mbc-84 4-fach Servo-/Weichendecoder - ESP32-C3                                                *
//*************************************************************************************************
//* WICHTIG - OTA-Partitionsschema: siehe Mbc93_ESP32C3-Kopfkommentar (identisch) - zwei App-     *
//* Partitionen (ota_0/ota_1), Tools/Werkzeuge -> Partition Scheme "Custom" waehlen.  LED         *
//* gruen/rot, Steckernetzteil-Erkennung, Stellpult (8) und Positions-LEDs (8) haengen an ZWEI    *
//* I2C-MCP23017 (Expander1: LED/Plug auf GPIOA Bit 0-2; Expander2: Stellpult GPIOA, Positions-   *
//* LEDs GPIOB, exakt 16 Signale) - beide teilen sich den I2C-Bus mit unterschiedlicher Adresse.  *
//* Die 4 Servosignale MUESSEN native Pins sein (Servo-Bibliothek).  ACHTUNG - Pinbudget sehr     *
//* knapp: dieses Board hat GPIO11-19 typischerweise fuer internes Flash reserviert (siehe        *
//* Mbc93_ESP32C3-Kopfkommentar) und GPIO9 als BOOT-Taste belegt. Fuer I2C wurden hier dennoch    *
//* GPIO18/GPIO19 gewaehlt (ANNAHME: bei vielen ESP32-C3-DevKit-Modulen frei, aber                *
//* MODULABHAENGIG) - vor dem Verdrahten UNBEDINGT gegen das jeweilige Modul-Datenblatt pruefen,  *
//* ggf. andere freie Pins waehlen.  mbc-84 hat nur einen CAN-Kanal, hier ueber die interne TWAI  *
//* (externer CAN-Transceiver noetig, z.B. SN65HVD230) statt eines externen MCP2515.  WICHTIG:    *
//* Tools/Werkzeuge -> "USB CDC On Boot" auf "Enabled" stellen, sonst bleiben alle Serial-        *
//* Ausgaben unsichtbar. SPI wird bewusst mit EXPLIZITEN Pins gestartet                           *
//* (SPI.begin(sck,miso,mosi,-1)) statt Boardstandard-Pins.  Kein Reset-Pin: ESP32 nutzt den      *
//* Software-Reset des Cores (MbcPlatform).  Boot-Speicherchip: W25Q64-SPI-NOR-Flash (8 MByte),   *
//* siehe MbcBootStorage-Kopfkommentar.                                                           *
//* | Funktion                              | GPIO                          |                     *
//* |------------------------------------------|-------------------------------|                  *
//* | SPI SCK / MISO / MOSI                     | GPIO4 / GPIO5 / GPIO6        |                  *
//* | MBCAN TWAI-TX                             | GPIO2                       |                  *
//* | MBCAN TWAI-RX                             | GPIO3                       |                  *
//* | Boot-Speicher-CS (W25Q64)                 | GPIO8                       |                  *
//* | OneWire                                   | GPIO10                      |                  *
//* | ADC Busspannung                           | GPIO0                       |                  *
//* | I2C SDA/SCL (2x MCP23017, siehe ACHTUNG)  | GPIO18 / GPIO19             |                  *
//* | Servo 0/1/2/3                             | GPIO1/GPIO7/GPIO20/GPIO21   |                  *
//* | LED gruen (Expander1 GPA0)                | -                           |                  *
//* | LED rot (Expander1 GPA1)                  | -                           |                  *
//* | Steckernetzteil (Expander1 GPA2)          | -                           |                  *
//* | Stellpult (Expander2 GPIOA 0-7)           | -                           |                  *
//* | Positions-LEDs (Expander2 GPIOB 0-7)      | -                           |                  *
//*************************************************************************************************
//* Datei: Mbc84_ESP32C3.ino                                                                      *
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

    const uint8_t PIN_SPI_SCK         = 4;
    const uint8_t PIN_SPI_MISO        = 5;
    const uint8_t PIN_SPI_MOSI        = 6;
    const uint8_t PIN_TWAI_TX         = 2;   // fest, einheitlich ueber alle ESP32-C3-Sketches
    const uint8_t PIN_TWAI_RX         = 3;   // fest, einheitlich ueber alle ESP32-C3-Sketches
    const uint8_t PIN_CS_BOOT_STORAGE = 8;
    const uint8_t PIN_ONEWIRE         = 10;
    const uint8_t PIN_ADC_VSPG        = 0;

    const uint8_t PIN_I2C_SDA         = 18;   // siehe ACHTUNG im Kopfkommentar
    const uint8_t PIN_I2C_SCL         = 19;

    const uint8_t SERVO_PINS[4] = { 1, 7, 20, 21 };

//------ Modulidentitaet (ASCII-Ziffern) ----------------------------------------------------------

    // [2]=Prozessor: 0=ATmega644P/1=ESP32-C3/2=RP2040-Pico/3=RP2040-Zero

    const uint8_t ART_NR[4] = { '8', '4', '1', '1' };
    const uint8_t SW_VER[3] = { '1', '0', '0' };
    const uint8_t HW_VER[6] = { '2', '4', '0', '1', '0', '6' };   // Hardware 24.01.06

//------ Objekte ----------------------------------------------------------------------------------

    MbcI2cBus   i2c(Wire);
    MbcMcp23x17 ledExpander(i2c, 0x00);
    MbcMcp23x17 ioExpander(i2c, 0x01);

    // ledExpander GPIOA-Bits: 0=LED gruen, 1=LED rot (beide HIGH-aktiv), 2=Steckernetzteil-
    // Erkennung (Eingang, ANNAHME HIGH-aktiv)

    MbcDigitalOut ledGruenOut(ledExpander, false, 0, false);
    MbcDigitalOut ledRotOut(ledExpander, false, 1, false);
    MbcDigitalIn  plugDetectorIn(ledExpander, false, 2, false);

    // ioExpander GPIOA Bit 0-7: Stellpult Rot0,Gruen0,Rot1,Gruen1,Rot2,Gruen2,Rot3,Gruen3
    // (Eingang, aktiv-LOW mit PullUp - siehe Mbc84Com.h-Klassenkommentar). GPIOB Bit 0-7:
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

    MbcUtils       utils(sysArray, 497, PIN_ADC_VSPG, 0, 0, 0, PIN_ONEWIRE, 497);   // Plug-/LED-
                                                                       // Pins ungenutzt, siehe
                                                                       // setPlugDetector()/
                                                                       // setLedDrivers()
    MbcCan         can(PIN_TWAI_TX, PIN_TWAI_RX, utils);
    MbcBootStorage bootStorage(PIN_CS_BOOT_STORAGE, MbcBootStorageBackend::FLASH_W25Q64);
    Mbc84Com       com(utils, can, bootStorage, SERVO_PINS, switchboard, led);

//------ Status der CAN-Initialisierung (fuer die wiederkehrende Fehlermeldung in loop()) ---------

    bool canOk = false;

//------ setup()/loop() ---------------------------------------------------------------------------

void setup()
{
    SPI.begin(PIN_SPI_SCK, PIN_SPI_MISO, PIN_SPI_MOSI, -1);

    Wire.setPins(PIN_I2C_SDA, PIN_I2C_SCL);
    Wire.begin();

    // ledExpander GPIOA: Bit 0/1 Ausgang (LEDs), Bit 2 Eingang (Steckernetzteil), Rest unbenutzt

    bool ledExpanderOk = ledExpander.begin(0xf8, 0x00, 0x00, 0x00);

    // GPIOA Eingang, GPIOB Ausgang

    bool ioExpanderOk = ioExpander.begin(0xff, 0x00, 0x00, 0x00);

    utils.setLedDrivers(&ledGruenOut, &ledRotOut);
    utils.setPlugDetector(&plugDetectorIn);

    utils.begin(ART_NR, SW_VER, HW_VER);

    Serial.begin(115200);

    unsigned long usbStart = millis();
    while ((unsigned long) (millis() - usbStart) < 2000) utils.update();

    Serial.println("\n[Mbc84] Start");

    Serial.print("[Mbc84] MCP23017 (LED/Plug): ");
    Serial.println(ledExpanderOk ? "OK" : "FEHLER - Baustein an I2C-Adresse 0x00 nicht gefunden");

    Serial.print("[Mbc84] MCP23017 (Stellpult/Positions-LEDs): ");
    Serial.println(ioExpanderOk ? "OK" : "FEHLER - Baustein an I2C-Adresse 0x01 nicht gefunden");

    canOk = can.begin();
    Serial.print("[Mbc84] MBCAN-Bus (TWAI): ");
    Serial.println(canOk ? "OK" : "FEHLER - Transceiver/Verdrahtung an GPIO2/GPIO3 pruefen");

    bool bootStorageOk = bootStorage.begin();
    Serial.print("[Mbc84] W25Q64 (Boot-Speicher): ");
    Serial.println(bootStorageOk ? "OK"
                                  : "FEHLER - Baustein an GPIO8 nicht gefunden/falsche JEDEC-ID");

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
        Serial.println("[Mbc84] FEHLER: MBCAN-Bus (TWAI) weiterhin nicht initialisiert!");
    }

    com.update();
}
