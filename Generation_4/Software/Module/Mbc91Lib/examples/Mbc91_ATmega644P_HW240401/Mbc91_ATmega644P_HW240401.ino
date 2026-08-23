//*************************************************************************************************
//* mbc-91 Magnetartikel-Schaltdecoder - ATmega644P (MightyCore, feste Pinbelegung)               *
//*************************************************************************************************
//* Pinbelegung entspricht der realen Hardware (feste Platinenleiterbahnen, Hardware 24.04.01),   *
//* SPI/CAN/Boot-EEPROM/LED/OneWire/Reset/ADC identisch zur Standard-Pinbelegung fuer dieses      *
//* Board.                                                                                        *
//* Arduino-Pin-Nummern anhand der tatsaechlichen MightyCore-"standard"-Variante ermittelt        *
//* (pins_arduino.h, lokal installiert unter Arduino15/packages/MightyCore/hardware/avr/.../      *
//* variants/standard/) - NICHT verlaesslich aus dem Gedaechtnis oder aus anderen Sketches        *
//* uebernehmen: PB0...PB7 = D0...D7, PD0...PD7 = D8...D15, PC0...PC7 = D16...D23, PA0...PA7 =    *
//* D24...D31 (alle 4 Ports aufsteigend).                                                         *
//*                                                                                               *
//* MCP_INT_CH0 wird NICHT verdrahtet: MbcCan empfaengt per Polling.                              *
//*                                                                                               *
//* Die Positionsanzeige laeuft hier - anders als bei den pinknappen Boards RP2040-Zero/ESP32-C3  *
//* dieser Bibliothek, die dafuer einen zusaetzlichen SPI-MCP23S17 nutzen - ueber die native      *
//* I2C-Verdrahtung: MCP23017, natives TWI (PC0/PC1 = D16/D17), Basisadresse 0. GPIOA/GPIOB-      *
//* Rollen entsprechend der Hardware-Belegung (siehe Mbc91Com-Klassenkommentar).                  *
//*                                                                                               *
//* | Funktion                            | Port,Pin | Arduino-Pin |                              *
//* |-------------------------------------|----------|-------------|                              *
//* | MBCAN-CS (MCP2515)                  | PB4      | D4          |                              *
//* | SPI MOSI/MISO/SCK                   | PB5/6/7  | D5/D6/D7 (Standard-SPI)   |                *
//* | OneWire                             | PB0      | D0          |                              *
//* | Reset-Pin                           | PA3      | D27         |                              *
//* | Boot-EEPROM-CS                      | PA2      | D26         |                              *
//* | LED gruen                           | PB1      | D1          |                              *
//* | LED rot                             | PB3      | D3          |                              *
//* | ADC Busspannung                     | ADC0/PA0 | A0          |                              *
//* | ADC Steckernetzteil                 | ADC1/PA1 | A1          |                              *
//* | I2C SCL/SDA (Positionsanzeige, MCP23017, natives TWI) | PC0/PC1 | D16/D17 (Standard-Wire)  |*
//* | REL_1_L / REL_1_R                   | PD0/PD2  | D8/D10      |                              *
//* | REL_2_L / REL_2_R                   | PD4/PD6  | D12/D14     |                              *
//* | REL_3_L / REL_3_R                   | PC7/PC5  | D23/D21     |                              *
//* | REL_4_L / REL_4_R                   | PC3/PA4  | D19/D28     |                              *
//* | IN_1_G / IN_1_R                     | PD1/PD3  | D9/D11      |                              *
//* | IN_2_G / IN_2_R                     | PD5/PD7  | D13/D15     |                              *
//* | IN_3_G / IN_3_R                     | PC6/PC4  | D22/D20     |                              *
//* | IN_4_G / IN_4_R                     | PC2/PA5  | D18/D29     |                              *
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
//* Datei: Mbc91_ATmega644P_HW240401.ino                                                          *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen:                                                                                  *
//* V2.0.0 - 06.08.2026 - Nullversion                                                             *
//*************************************************************************************************

#include <SPI.h>
#include <Wire.h>
#include <Mbc91Com.h>
#include <MbcI2cBus.h>
#include <MbcMcp23x17.h>
#include <MbcDigitalOut.h>
#include <MbcDigitalIn.h>

//------ Pinbelegung ------------------------------------------------------------------------------

    const uint8_t PIN_CS_CAN_MBCAN       = 4;
    const uint8_t PIN_ONEWIRE            = 0;
    const uint8_t PIN_RESET              = 27;
    const uint8_t PIN_CS_BOOT_EEPROM     = 26;
    const uint8_t PIN_LED_GRUEN          = 1;
    const uint8_t PIN_LED_ROT            = 3;
    const uint8_t PIN_ADC_VSPG           = A0;
    const uint8_t PIN_ADC_PLUG           = A1;

    // Positionsanzeige: natives TWI (PC0/PC1 = D16/D17), kein eigener Pin noetig - siehe
    // Kopfkommentar

    const uint8_t PIN_REL_1_L = 8,  PIN_REL_1_R = 10;
    const uint8_t PIN_REL_2_L = 12, PIN_REL_2_R = 14;
    const uint8_t PIN_REL_3_L = 23, PIN_REL_3_R = 21;
    const uint8_t PIN_REL_4_L = 19, PIN_REL_4_R = 28;

    const uint8_t PIN_IN_1_G = 9,  PIN_IN_1_R = 11;
    const uint8_t PIN_IN_2_G = 13, PIN_IN_2_R = 15;
    const uint8_t PIN_IN_3_G = 22, PIN_IN_3_R = 20;
    const uint8_t PIN_IN_4_G = 18, PIN_IN_4_R = 29;

//------ Modulidentitaet (ASCII-Ziffern) ----------------------------------------------------------

    const uint8_t ART_NR[4] = { '9', '1', '0', '1' };   // [2]=Prozessor: 0=ATmega644P/1=ESP32-C3/2=RP2040-Pico/3=RP2040-Zero
    const uint8_t SW_VER[3] = { '2', '0', '0' };
    const uint8_t HW_VER[6] = { '2', '4', '0', '4', '0', '1' };   // Hardware 24.04.01

//------ Objekte ----------------------------------------------------------------------------------

    // Positionsanzeige: I2C-MCP23017 (siehe Kopfkommentar)

    MbcI2cBus   i2cBus;
    MbcMcp23x17 indicator(i2cBus, 0);   // Basisadresse 0

    // Relaisausgaenge/Stellpult-Eingaenge: native Pins (feste Hardware-Pinbelegung)

    MbcDigitalOut relL1(PIN_REL_1_L), relR1(PIN_REL_1_R);
    MbcDigitalOut relL2(PIN_REL_2_L), relR2(PIN_REL_2_R);
    MbcDigitalOut relL3(PIN_REL_3_L), relR3(PIN_REL_3_R);
    MbcDigitalOut relL4(PIN_REL_4_L), relR4(PIN_REL_4_R);

    MbcDigitalIn inG1(PIN_IN_1_G), inR1(PIN_IN_1_R);
    MbcDigitalIn inG2(PIN_IN_2_G), inR2(PIN_IN_2_R);
    MbcDigitalIn inG3(PIN_IN_3_G), inR3(PIN_IN_3_R);
    MbcDigitalIn inG4(PIN_IN_4_G), inR4(PIN_IN_4_R);

    // Index-Konvention siehe Mbc91Com-Klassenkommentar (0..7 = _1_L,_1_R,_2_L,_2_R,...)

    MbcDigitalOut *relais[8]    = { &relL1, &relR1, &relL2, &relR2,
                                     &relL3, &relR3, &relL4, &relR4 };
    MbcDigitalIn  *stellpult[8] = { &inG1, &inR1, &inG2, &inR2, &inG3, &inR3, &inG4, &inR4 };

    // genPurpSize=10 (MBC_91_MM_H..RUECK + 4 Byte Impulszeiten), extraEepromSize=9 (siehe
    // Mbc91Com.cpp EEX_*-Konstanten)

    static uint8_t sysArray[MBC_SYSARRAY_SIZE(10)];

    MbcUtils       utils(sysArray, 10, PIN_ADC_VSPG, PIN_ADC_PLUG, PIN_LED_GRUEN, PIN_LED_ROT,
                          PIN_ONEWIRE, 9);
    MbcCan         can(PIN_CS_CAN_MBCAN, utils);
    MbcBootStorage bootStorage(PIN_CS_BOOT_EEPROM);
    Mbc91Com       com(utils, can, bootStorage, indicator, relais, stellpult, PIN_RESET);

//------ setup()/loop() ---------------------------------------------------------------------------

void setup()
{
    SPI.begin();
    i2cBus.begin();

    // GPIOA: Ausgang (8 Stellungs-LEDs), GPIOB: Bit 0-3 Eingang (Bestueckungsnibble, PullUp
    // extern, Rest unbenutzt) - siehe Mbc91Com::stateDisplay()/beginModule()

    indicator.begin(0x00, 0x00, 0xff, 0x00);

    relL1.begin(); relR1.begin(); relL2.begin(); relR2.begin();
    relL3.begin(); relR3.begin(); relL4.begin(); relR4.begin();

    inG1.begin(); inR1.begin(); inG2.begin(); inR2.begin();
    inG3.begin(); inR3.begin(); inG4.begin(); inR4.begin();

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
