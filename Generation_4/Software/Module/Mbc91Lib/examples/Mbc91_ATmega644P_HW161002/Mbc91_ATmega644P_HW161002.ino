//*************************************************************************************************
//* mbc-91 Magnetartikel-Schaltdecoder - ATmega644P (MightyCore, AELTERE Hardware-Generation)     *
//*************************************************************************************************
//* Pinbelegung entspricht der realen Hardware (feste Platinenleiterbahnen, Hardware 16.10.02) -  *
//* NICHT identisch zur neueren Hardware (24.04.01, siehe Mbc91_ATmega644P_HW240401):             *
//* Boot-EEPROM-CS liegt auf PA1 statt PA2, ADC-Steckererkennung auf ADC2/PA2 statt ADC1/PA1,     *
//* REL_4_R auf PC1 statt PA4. Vor allem aber KEIN I2C an Bord: die                               *
//* Positionsanzeige sind 8 DIREKTE GPIO-LEDs (kein MCP23017-Expander), und es gibt KEIN          *
//* Stellpult - stattdessen werden einmalig beim Start 4 direkte GPIO-"Bestueckungs"-Jumper       *
//* gelesen, um pro Doppelspulenantrieb die Betriebsart (Impuls/Wechsel) automatisch festzulegen  *
//* (siehe Mbc91Com-Klassenkommentar, zweiter Konstruktor). Arduino-Pin-Nummern anhand der        *
//* tatsaechlichen MightyCore-"standard"-Variante ermittelt (pins_arduino.h, lokal installiert    *
//* unter Arduino15/packages/MightyCore/hardware/avr/.../variants/standard/) - NICHT aus dem      *
//* Gedaechtnis oder aus anderen Sketches uebernehmen: PB0...PB7 = D0...D7, PD0...PD7 =           *
//* D8...D15, PC0...PC7 = D16...D23, PA0...PA7 = D24...D31 (alle 4 Ports aufsteigend).            *
//*                                                                                               *
//* MCP_INT_CH0 wird NICHT verdrahtet: MbcCan empfaengt per Polling.                              *
//*                                                                                               *
//* | Funktion                            | Port,Pin | Arduino-Pin |                              *
//* |-------------------------------------|----------|-------------|                              *
//* | MBCAN-CS (MCP2515)                  | PB4      | D4          |                              *
//* | SPI MOSI/MISO/SCK                   | PB5/6/7  | D5/D6/D7 (Standard-SPI)   |                *
//* | OneWire                             | PB0      | D0          |                              *
//* | Reset-Pin                           | PA3      | D27         |                              *
//* | Boot-EEPROM-CS                      | PA1      | D25         |                              *
//* | LED gruen                           | PB1      | D1          |                              *
//* | LED rot                             | PB3      | D3          |                              *
//* | ADC Busspannung                     | ADC0/PA0 | A0          |                              *
//* | ADC Steckernetzteil                 | ADC2/PA2 | A2          |                              *
//* | REL_1_L / REL_1_R                   | PD0/PD2  | D8/D10      |                              *
//* | REL_2_L / REL_2_R                   | PD4/PD6  | D12/D14     |                              *
//* | REL_3_L / REL_3_R                   | PC7/PC5  | D23/D21     |                              *
//* | REL_4_L / REL_4_R                   | PC3/PC1  | D19/D17     |                              *
//* | LED_1_L / LED_1_R (Positionsanzeige)| PD1/PD3  | D9/D11      |                              *
//* | LED_2_L / LED_2_R (Positionsanzeige)| PD5/PD7  | D13/D15     |                              *
//* | LED_3_L / LED_3_R (Positionsanzeige)| PC6/PC4  | D22/D20     |                              *
//* | LED_4_L / LED_4_R (Positionsanzeige)| PC2/PC0  | D18/D16     |                              *
//* | Bestueckung Sp1 (BEST_1)            | PA7      | D31         |                              *
//* | Bestueckung Sp2 (BEST_2)            | PA6      | D30         |                              *
//* | Bestueckung Sp3 (BEST_3)            | PA5      | D29         |                              *
//* | Bestueckung Sp4 (BEST_4)            | PA4      | D28         |                              *
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
//* Datei: Mbc91_ATmega644P_HW161002.ino                                                          *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen:                                                                                  *
//* V3.0.0 - 15.08.2026 - Nullversion                                                             *
//*************************************************************************************************

#include <SPI.h>
#include <Mbc91Com.h>
#include <MbcDigitalOut.h>
#include <MbcDigitalIn.h>

//------ Pinbelegung ------------------------------------------------------------------------------

    const uint8_t PIN_CS_CAN_MBCAN       = 4;
    const uint8_t PIN_ONEWIRE            = 0;
    const uint8_t PIN_RESET              = 27;
    const uint8_t PIN_CS_BOOT_EEPROM     = 25;
    const uint8_t PIN_LED_GRUEN          = 1;
    const uint8_t PIN_LED_ROT            = 3;
    const uint8_t PIN_ADC_VSPG           = A0;
    const uint8_t PIN_ADC_PLUG           = A2;

    const uint8_t PIN_REL_1_L = 8,  PIN_REL_1_R = 10;
    const uint8_t PIN_REL_2_L = 12, PIN_REL_2_R = 14;
    const uint8_t PIN_REL_3_L = 23, PIN_REL_3_R = 21;
    const uint8_t PIN_REL_4_L = 19, PIN_REL_4_R = 17;

    const uint8_t PIN_LED_1_L = 9,  PIN_LED_1_R = 11;
    const uint8_t PIN_LED_2_L = 13, PIN_LED_2_R = 15;
    const uint8_t PIN_LED_3_L = 22, PIN_LED_3_R = 20;
    const uint8_t PIN_LED_4_L = 18, PIN_LED_4_R = 16;

    const uint8_t PIN_BEST_SP1 = 31, PIN_BEST_SP2 = 30, PIN_BEST_SP3 = 29, PIN_BEST_SP4 = 28;

//------ Modulidentitaet (ASCII-Ziffern) ----------------------------------------------------------

    // ArtNum4=0 fuer diese aeltere Hardware (nicht '1' wie beim neueren Sketch HW240401)

    const uint8_t ART_NR[4] = { '9', '1', '0', '0' };   // [2]=Prozessor: 0=ATmega644P/1=ESP32-C3/2=RP2040-Pico/3=RP2040-Zero
    const uint8_t SW_VER[3] = { '3', '0', '0' };
    const uint8_t HW_VER[6] = { '1', '6', '1', '0', '0', '2' };   // Hardware 16.10.02

//------ Objekte ----------------------------------------------------------------------------------

    // Relaisausgaenge/Positions-LEDs/Bestueckungs-Jumper: native Pins (feste Hardware-
    // Pinbelegung). Index-Konvention siehe Mbc91Com-Klassenkommentar (0..7 = _1_L,_1_R,_2_L,_2_R,
    // _3_L,_3_R,_4_L,_4_R; Bestueckung 0..3 = Sp1..Sp4)

    MbcDigitalOut relL1(PIN_REL_1_L), relR1(PIN_REL_1_R);
    MbcDigitalOut relL2(PIN_REL_2_L), relR2(PIN_REL_2_R);
    MbcDigitalOut relL3(PIN_REL_3_L), relR3(PIN_REL_3_R);
    MbcDigitalOut relL4(PIN_REL_4_L), relR4(PIN_REL_4_R);

    MbcDigitalOut ledL1(PIN_LED_1_L), ledR1(PIN_LED_1_R);
    MbcDigitalOut ledL2(PIN_LED_2_L), ledR2(PIN_LED_2_R);
    MbcDigitalOut ledL3(PIN_LED_3_L), ledR3(PIN_LED_3_R);
    MbcDigitalOut ledL4(PIN_LED_4_L), ledR4(PIN_LED_4_R);

    // activeLow=false: read()==true bei HIGH (kein Jumper) - siehe Mbc91Com::beginModule()

    MbcDigitalIn bestSp1(PIN_BEST_SP1, false), bestSp2(PIN_BEST_SP2, false);
    MbcDigitalIn bestSp3(PIN_BEST_SP3, false), bestSp4(PIN_BEST_SP4, false);

    MbcDigitalOut *relais[8] = { &relL1, &relR1, &relL2, &relR2,
                                  &relL3, &relR3, &relL4, &relR4 };
    MbcDigitalOut *posLed[8] = { &ledL1, &ledR1, &ledL2, &ledR2,
                                  &ledL3, &ledR3, &ledL4, &ledR4 };
    MbcDigitalIn  *bestueckung[4] = { &bestSp1, &bestSp2, &bestSp3, &bestSp4 };

    // genPurpSize=10 (MBC_91_MM_H..RUECK + 4 Byte Impulszeiten), extraEepromSize=9 (siehe
    // Mbc91Com.cpp EEX_*-Konstanten) - unveraendert gegenueber der neueren Hardware, da rein
    // strukturell und unabhaengig von der Pin-/Backend-Wahl

    static uint8_t sysArray[MBC_SYSARRAY_SIZE(10)];

    MbcUtils       utils(sysArray, 10, PIN_ADC_VSPG, PIN_ADC_PLUG, PIN_LED_GRUEN, PIN_LED_ROT,
                          PIN_ONEWIRE, 9);
    MbcCan         can(PIN_CS_CAN_MBCAN, utils);
    MbcBootStorage bootStorage(PIN_CS_BOOT_EEPROM);
    Mbc91Com       com(utils, can, bootStorage, relais, posLed, bestueckung, PIN_RESET);

//------ setup()/loop() ---------------------------------------------------------------------------

void setup()
{
    SPI.begin();

    relL1.begin(); relR1.begin(); relL2.begin(); relR2.begin();
    relL3.begin(); relR3.begin(); relL4.begin(); relR4.begin();

    ledL1.begin(); ledR1.begin(); ledL2.begin(); ledR2.begin();
    ledL3.begin(); ledR3.begin(); ledL4.begin(); ledR4.begin();

    bestSp1.begin(); bestSp2.begin(); bestSp3.begin(); bestSp4.begin();

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
