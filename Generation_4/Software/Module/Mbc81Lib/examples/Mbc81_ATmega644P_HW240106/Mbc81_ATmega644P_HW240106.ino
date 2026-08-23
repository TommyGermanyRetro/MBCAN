//*************************************************************************************************
//* mbc-81 Stellpult mit 32 Tastern, 16 Stellungs-LEDs - ATmega644P (MightyCore)                  *
//*************************************************************************************************
//* Pinbelegung entspricht der realen Hardware (feste Platinenleiterbahnen, keine freie Pinwahl   *
//* moeglich). Arduino-Pin-Nummern anhand der tatsaechlichen MightyCore-"standard"-Variante       *
//* ermittelt (pins_arduino.h, lokal installiert unter                                            *
//* Arduino15/packages/MightyCore/hardware/avr/.../variants/standard/) - NICHT verlaesslich aus   *
//* dem Gedaechtnis oder aus anderen Sketches uebernehmen: PB0...PB7 = D0...D7, PD0...PD7 =       *
//* D8...D15, PC0...PC7 = D16...D23, PA0...PA7 = D24...D31 (alle 4 Ports aufsteigend).            *
//*                                                                                               *
//* | Funktion                                    | Port,Pin | Arduino-Pin              |         *
//* |-----------------------------------------------|----------|-------------------------|        *
//* | MBCAN-CS (MCP2515)                            | PB4      | D4                       |       *
//* | SPI MOSI/MISO/SCK                             | PB5/6/7  | D5/D6/D7 (Standard-SPI)  |       *
//* | OneWire                                       | PB0      | D0                       |       *
//* | Reset-Pin                                     | PA3      | D27                      |       *
//* | Boot-EEPROM-CS                                | PA2      | D26                      |       *
//* | LED gruen                                     | PB1      | D1                       |       *
//* | LED rot                                       | PB3      | D3                       |       *
//* | ADC Busspannung                               | ADC0/PA0 | A0                       |       *
//* | ADC Steckernetzteil                           | ADC1/PA1 | A1                       |       *
//* | I2C SDA/SCL (Taster-PCF8574, natives TWI)     | PC0/PC1  | D16/D17 (Standard-Wire)  |       *
//* | L01 / L02                                     | PA6/PA7  | D30/D31                  |       *
//* | L03..L08                                      | PC7...PC2| D23...D18                |       *
//* | L09..L16                                      | PD0...PD7| D8...D15                 |       *
//*                                                                                               *
//* Spare/unbenutzt: D2, D28, D29 - beim Testen zu verifizieren, ob diese Pins tatsaechlich frei  *
//* liegen (Board moeglicherweise dicht geroutet, nicht zwingend 100% belegt).                    *
//*************************************************************************************************
//* Bootloader-Vorbereitung (MightyCore, ATmega644P via ISP) - Dateien/Volltext siehe Ordner      *
//* Bootloaderinfos_MightyCore/ (README.txt, boards.txt, bootloaders/mbcan/mbcanboot.hex),        *
//* identisch fuer alle MBCAN-ATmega644P-Sketches:                                                *
//*                                                                                               *
//* 1) Arduino IDE - neuen Boardverwalter ergaenzen:                                              *
//* https://mcudude.github.io/MightyCore/package_MCUdude_MightyCore_index.json                    *
//* 2) Bei der Board-Verwaltung "MightyCore" installieren, IDE schliessen.                        *
//* 3) Im Pfad                                                                                    *
//* "C:\Users\<DeinName>\AppData\Local\Arduino15\packages\MightyCore\hardware\avr\y.y.y" die      *
//* Datei "boards.txt" und den Ordner "bootloaders" aus Bootloaderinfos_MightyCore/               *
//* hineinkopieren.                                                                               *
//* 4) Cache loeschen: "C:\Users\<DeinName>\AppData\Roaming\arduino-ide\". IDE neu starten.       *
//* 5) Arduino IDE starten, den Sketch auswaehlen und oeffnen.                                    *
//* 6) Unter "Werkzeuge" auf Prozessortyp "ATmega644" und Variante "644P/644PA" achten. Jetzt     *
//* sollte der Bootloader "MBCAN" angezeigt werden. Diesen auswaehlen. Zusaetzlich unter          *
//* "Werkzeuge -> BOD" den Wert "2,7V" auswaehlen (Brown-Out-Detection - verhindert, dass der     *
//* Prozessor bei kurzen Spannungseinbruechen mit korruptem SRAM-Inhalt weiterlaeuft, statt       *
//* sauber neu zu starten).                                                                       *
//* 7) Sketch zum Modul kompilieren.                                                              *
//* 8) "Binaerdateien exportieren" unter "Sketch" anstossen.                                      *
//* 9) Im Sketchordner liegt unter "<DeinSketch>\build\MightyCore.avr.644" die Datei              *
//* "xxx.with_bootloader.hex".                                                                    *
//* 10) Mit AVRStudio o.ae. per ISP diese Datei auf den ATmega644P aufspielen. FUSES: HIGH.BOOTSZ *
//* auf 2048 Byte, HIGH.BOOTRST enabled, EXTENDED auf 0xFD (BOD 2,7V) - dann startet der          *
//* Prozessor zuerst mit dem Bootloader und dann mit dem Sketch.                                  *
//* 11) Damit ist der originale Bootloader mit dem kompilierten Sketch aufgespielt. Ab da nur     *
//* noch die Upgrade-Funktion des Parametriercenters nutzen und die Datei "xxx.ino.hex".          *
//* 12) Gilt nur fuer das Parametriercenter ab Version 3.0.0.0 - fruehere Versionen unterstuetzen *
//* die Arduino-IDE-HEX-Dateien nicht.                                                            *
//*************************************************************************************************
//* Datei: Mbc81_ATmega644P_HW240106.ino                                                          *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen:                                                                                  *
//* V2.0.0 - 22.08.2026 - Nullversion                                                             *
//*************************************************************************************************

#include <SPI.h>
#include <Wire.h>
#include <Mbc81Com.h>
#include <MbcDigitalOut.h>
#include <MbcI2cBus.h>
#include <MbcPcf8574.h>

//------ Pinbelegung ------------------------------------------------------------------------------

    const uint8_t PIN_CS_CAN_MBCAN   = 4;
    const uint8_t PIN_ONEWIRE        = 0;
    const uint8_t PIN_RESET          = 27;
    const uint8_t PIN_CS_BOOT_EEPROM = 26;
    const uint8_t PIN_LED_GRUEN      = 1;
    const uint8_t PIN_LED_ROT        = 3;
    const uint8_t PIN_ADC_VSPG       = A0;
    const uint8_t PIN_ADC_PLUG       = A1;

    // L01..L16 (Index 0..15 = MM-Adressoffset, siehe Mbc81Com.h)

    const uint8_t PIN_LED[16] =
        { 30, 31, 23, 22, 21, 20, 19, 18, 8, 9, 10, 11, 12, 13, 14, 15 };

//------ Modulidentitaet (ASCII-Ziffern) ----------------------------------------------------------

    // [2]=Prozessor: 0=ATmega644P/1=ESP32-C3/2=RP2040-Pico/3=RP2040-Zero
    // ArtNum2=1 ist ein Hilfswert, der durch die Bestueckung des Moduls bestimmt wird

    const uint8_t ART_NR[4] = { '8', '1', '0', '2' };
    const uint8_t SW_VER[3] = { '2', '0', '0' };
    const uint8_t HW_VER[6] = { '2', '4', '0', '1', '0', '6' };   // Hardware 24.01.06

//------ Objekte ----------------------------------------------------------------------------------

    MbcI2cBus  i2cBus;
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
    MbcBootStorage bootStorage(PIN_CS_BOOT_EEPROM);
    Mbc81Com       com(utils, can, bootStorage, tasterChips, leds, PIN_RESET);

//------ setup()/loop() ---------------------------------------------------------------------------

void setup()
{
    SPI.begin();
    Wire.begin();
    i2cBus.begin();

    utils.begin(ART_NR, SW_VER, HW_VER);

    can.begin();
    bootStorage.begin();

    com.begin();
    com.beginModule();

    // Warte, bis alle Statuswerte (Vspg/Plug/Temperatur) einmal ermittelt wurden - blinkt
    // waehrenddessen ca. 10 s durchgehend ORANGE (siehe MbcUtils::waitFilled())

    utils.waitFilled();
}

void loop()
{
    utils.update();
    utils.check();

    com.update();
}
