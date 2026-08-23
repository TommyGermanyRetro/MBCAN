//*************************************************************************************************
//* mbc-84 4-fach Servo-/Weichendecoder - ATmega644P (MightyCore, feste Pinbelegung)              *
//*************************************************************************************************
//* Pinbelegung entspricht der realen Hardware (feste Platinenleiterbahnen, keine freie Pinwahl   *
//* moeglich), SPI/CAN/Boot-EEPROM/LED/OneWire/Reset/ADC identisch zur Standard-Pinbelegung fuer  *
//* dieses Board. Arduino-Pin-Nummern anhand der tatsaechlichen MightyCore-"standard"-Variante    *
//* ermittelt (pins_arduino.h, lokal installiert unter                                            *
//* Arduino15/packages/MightyCore/hardware/avr/.../variants/standard/) - NICHT verlaesslich aus   *
//* dem Gedaechtnis oder aus anderen Sketches uebernehmen: PB0...PB7 = D0...D7, PD0...PD7 =       *
//* D8...D15, PC0...PC7 = D16...D23, PA0...PA7 = D24...D31 (alle 4 Ports aufsteigend). Positions- *
//* LEDs auf PORTC (D16...D23), Bitreihenfolge led_stellung={7,6,5,4,3,2,1,0} (Aktorstellungsbit  *
//* i -> PORTC-Bit 7-i -> Arduino-Pin D23-i).                                                     *
//* | Funktion                            | Port,Pin | Arduino-Pin |                              *
//* |--------------------------------------|----------|-------------|                             *
//* | MBCAN-CS (MCP2515)                  | PB4      | D4          |                              *
//* | SPI MOSI/MISO/SCK                   | PB5/6/7  | D5/D6/D7 (Standard-SPI)                    *
//* | OneWire                             | PB0      | D0          |                              *
//* | Reset-Pin                           | PA3      | D27         |                              *
//* | Boot-EEPROM-CS                      | PA2      | D26         |                              *
//* | LED gruen                           | PB1      | D1          |                              *
//* | LED rot                             | PB3      | D3          |                              *
//* | ADC Busspannung                     | ADC0/PA0 | A0          |                              *
//* | ADC Steckernetzteil                 | ADC1/PA1 | A1          |                              *
//* | Servo 0                             | PA4      | D28         |                              *
//* | Servo 1                             | PA5      | D29         |                              *
//* | Servo 2                             | PA6      | D30         |                              *
//* | Servo 3                             | PA7      | D31         |                              *
//* | Stellpult Servo0 Rot/Gruen          | PD0/PD1  | D8/D9       |                              *
//* | Stellpult Servo1 Rot/Gruen          | PD2/PD3  | D10/D11     |                              *
//* | Stellpult Servo2 Rot/Gruen          | PD4/PD5  | D12/D13     |                              *
//* | Stellpult Servo3 Rot/Gruen          | PD6/PD7  | D14/D15     |                              *
//* | Positions-LEDs (PORTC, 8 Bit)       | PC0...PC7| D16...D23   |                              *
//*************************************************************************************************
//* Bootloader-Vorbereitung (MightyCore, ATmega644P via ISP) - Dateien/Volltext siehe Ordner      *
//* Bootloaderinfos_MightyCore/ (README.txt, boards.txt, bootloaders/mbcan/mbcanboot.hex),        *
//* identisch fuer alle MBCAN-ATmega644P-Sketches:                                                *
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
//* Datei: Mbc84_ATmega644P_HW240106.ino                                                          *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen: V2.0.0 - 18.08.2026 - Nullversion                                                *
//*************************************************************************************************

#include <SPI.h>
#include <Mbc84Com.h>
#include <MbcDigitalIn.h>
#include <MbcDigitalOut.h>

//------ Pinbelegung ------------------------------------------------------------------------------

    const uint8_t PIN_CS_CAN_MBCAN   = 4;
    const uint8_t PIN_ONEWIRE        = 0;
    const uint8_t PIN_RESET          = 27;
    const uint8_t PIN_CS_BOOT_EEPROM = 26;
    const uint8_t PIN_LED_GRUEN      = 1;
    const uint8_t PIN_LED_ROT        = 3;
    const uint8_t PIN_ADC_VSPG       = A0;
    const uint8_t PIN_ADC_PLUG       = A1;

    const uint8_t SERVO_PINS[4] = { 28, 29, 30, 31 };

    // Reihenfolge Rot0,Gruen0,Rot1,Gruen1,Rot2,Gruen2,Rot3,Gruen3 (siehe Mbc84Com.h-Kommentar)

    const uint8_t PIN_R0 = 8,  PIN_G0 = 9;
    const uint8_t PIN_R1 = 10, PIN_G1 = 11;
    const uint8_t PIN_R2 = 12, PIN_G2 = 13;
    const uint8_t PIN_R3 = 14, PIN_G3 = 15;

    // Positions-LEDs: Aktorstellungsbit i -> Arduino-Pin D23-i (Bit-Remap, siehe Kopfkommentar)

    const uint8_t PIN_LED[8] = { 23, 22, 21, 20, 19, 18, 17, 16 };

//------ Modulidentitaet (ASCII-Ziffern) ----------------------------------------------------------

    // [2]=Prozessor: 0=ATmega644P/1=ESP32-C3/2=RP2040-Pico/3=RP2040-Zero

    const uint8_t ART_NR[4] = { '8', '4', '0', '1' };
    const uint8_t SW_VER[3] = { '2', '0', '0' };
    const uint8_t HW_VER[6] = { '2', '4', '0', '1', '0', '6' };   // Hardware 24.01.06

//------ Objekte ----------------------------------------------------------------------------------

    MbcDigitalIn r0(PIN_R0), g0(PIN_G0), r1(PIN_R1), g1(PIN_G1);
    MbcDigitalIn r2(PIN_R2), g2(PIN_G2), r3(PIN_R3), g3(PIN_G3);

    MbcDigitalIn *switchboard[8] = { &r0, &g0, &r1, &g1, &r2, &g2, &r3, &g3 };

    MbcDigitalOut led0(PIN_LED[0], true), led1(PIN_LED[1], true);
    MbcDigitalOut led2(PIN_LED[2], true), led3(PIN_LED[3], true);
    MbcDigitalOut led4(PIN_LED[4], true), led5(PIN_LED[5], true);
    MbcDigitalOut led6(PIN_LED[6], true), led7(PIN_LED[7], true);

    MbcDigitalOut *led[8] = { &led0, &led1, &led2, &led3, &led4, &led5, &led6, &led7 };

    // genPurpSize=497 (voller Servoweg, siehe MbcUtils.h/Mbc84Com.h), extraEepromSize=497

    static uint8_t sysArray[MBC_SYSARRAY_SIZE(497)];

    MbcUtils       utils(sysArray, 497, PIN_ADC_VSPG, PIN_ADC_PLUG, PIN_LED_GRUEN, PIN_LED_ROT,
                          PIN_ONEWIRE, 497);
    MbcCan         can(PIN_CS_CAN_MBCAN, utils);
    MbcBootStorage bootStorage(PIN_CS_BOOT_EEPROM);
    Mbc84Com       com(utils, can, bootStorage, SERVO_PINS, switchboard, led, PIN_RESET);

//------ setup()/loop() ---------------------------------------------------------------------------

void setup()
{
    SPI.begin();

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
