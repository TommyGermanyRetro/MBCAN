//*************************************************************************************************
//* mbc-92 Magnetartikel-Schaltdecoder - ATmega644P (MightyCore)                                  *
//*************************************************************************************************
//* Pinbelegung entspricht der realen Hardware (feste Platinenleiterbahnen, keine freie Pinwahl   *
//* moeglich), SPI/CAN/Boot-EEPROM/LED/OneWire/Reset/ADC identisch zur Standard-Pinbelegung fuer  *
//* dieses Board.                                                                                 *
//* Arduino-Pin-Nummern anhand der tatsaechlichen MightyCore-"standard"-Variante ermittelt        *
//* (pins_arduino.h, lokal installiert unter Arduino15/packages/MightyCore/hardware/avr/.../      *
//* variants/standard/) - NICHT verlaesslich aus dem Gedaechtnis oder aus anderen Sketches        *
//* uebernehmen: PB0...PB7 = D0...D7, PD0...PD7 = D8...D15, PC0...PC7 = D16...D23, PA0...PA7 =    *
//* D24...D31 (alle 4 Ports aufsteigend).                                                         *
//*                                                                                               *
//* MCP_INT_CH0 (PD2/PD3) wird NICHT verdrahtet: MbcCan empfaengt per Polling.                    *
//*                                                                                               *
//* mbc-92 hat KEIN I2C und KEINE Positionsanzeige-Hardware - stattdessen 4                       *
//* direkte Stellungsanzeige-LEDs (eine je Doppelspulenantrieb, PORTA Bit 4-7) und ein EEPROM-    *
//* persistiertes "Art der Aktoren"-Flag statt einer per Hardware ausgelesenen Bestueckung        *
//* (siehe Mbc92Com-Klassenkommentar). LED-Polaritaet dieser neuen Hardware ist noch nicht        *
//* verifiziert.                                                                                  *
//*                                                                                               *
//* | Funktion                           | Port,Pin | Arduino-Pin |                               *
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
//* | REL_1_L / REL_1_R                   | PC0/PC1  | D16/D17     |                              *
//* | REL_2_L / REL_2_R                   | PC2/PC3  | D18/D19     |                              *
//* | REL_3_L / REL_3_R                   | PC4/PC5  | D20/D21     |                              *
//* | REL_4_L / REL_4_R                   | PC6/PC7  | D22/D23     |                              *
//* | IN_1_G / IN_1_R                     | PD0/PD1  | D8/D9       |                              *
//* | IN_2_G / IN_2_R                     | PD2/PD3  | D10/D11     |                              *
//* | IN_3_G / IN_3_R                     | PD4/PD5  | D12/D13     |                              *
//* | IN_4_G / IN_4_R                     | PD6/PD7  | D14/D15     |                              *
//* | Stellungsanzeige 1..4               | PA4..PA7 | D28..D31    |                              *
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
//* Datei: Mbc92_ATmega644P_HW240720.ino                                                          *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen:                                                                                  *
//* V1.0.0 - 14.08.2026 - Nullversion                                                             *
//*************************************************************************************************

#include <SPI.h>
#include <Mbc92Com.h>
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

    const uint8_t PIN_REL_1_L = 16, PIN_REL_1_R = 17;
    const uint8_t PIN_REL_2_L = 18, PIN_REL_2_R = 19;
    const uint8_t PIN_REL_3_L = 20, PIN_REL_3_R = 21;
    const uint8_t PIN_REL_4_L = 22, PIN_REL_4_R = 23;

    const uint8_t PIN_IN_1_G = 8,  PIN_IN_1_R = 9;
    const uint8_t PIN_IN_2_G = 10, PIN_IN_2_R = 11;
    const uint8_t PIN_IN_3_G = 12, PIN_IN_3_R = 13;
    const uint8_t PIN_IN_4_G = 14, PIN_IN_4_R = 15;

    const uint8_t PIN_ANZEIGE_1 = 28, PIN_ANZEIGE_2 = 29;
    const uint8_t PIN_ANZEIGE_3 = 30, PIN_ANZEIGE_4 = 31;

//------ Modulidentitaet (ASCII-Ziffern) ----------------------------------------------------------

    const uint8_t ART_NR[4] = { '9', '2', '0', '1' };   // [2]=Prozessor: 0=ATmega644P/1=ESP32-C3/2=RP2040-Pico/3=RP2040-Zero
    const uint8_t SW_VER[3] = { '1', '0', '0' };
    const uint8_t HW_VER[6] = { '2', '4', '0', '7', '2', '0' };   // Hardware 24.07.20

//------ Objekte ----------------------------------------------------------------------------------

    // Stellungsanzeige-LEDs/Relaisausgaenge/Stellpult-Eingaenge: native Pins (reale Hardware,
    // feste Pinbelegung)

    MbcDigitalOut anz1(PIN_ANZEIGE_1), anz2(PIN_ANZEIGE_2);
    MbcDigitalOut anz3(PIN_ANZEIGE_3), anz4(PIN_ANZEIGE_4);

    MbcDigitalOut relL1(PIN_REL_1_L), relR1(PIN_REL_1_R);
    MbcDigitalOut relL2(PIN_REL_2_L), relR2(PIN_REL_2_R);
    MbcDigitalOut relL3(PIN_REL_3_L), relR3(PIN_REL_3_R);
    MbcDigitalOut relL4(PIN_REL_4_L), relR4(PIN_REL_4_R);

    MbcDigitalIn inG1(PIN_IN_1_G), inR1(PIN_IN_1_R);
    MbcDigitalIn inG2(PIN_IN_2_G), inR2(PIN_IN_2_R);
    MbcDigitalIn inG3(PIN_IN_3_G), inR3(PIN_IN_3_R);
    MbcDigitalIn inG4(PIN_IN_4_G), inR4(PIN_IN_4_R);

    // Index-Konvention siehe Mbc92Com-Klassenkommentar (0..3 = Doppelspulenantrieb 1..4 bzw.
    // 0..7 = _1_L,_1_R,_2_L,_2_R,...)

    MbcDigitalOut *anzeige[4]   = { &anz1, &anz2, &anz3, &anz4 };
    MbcDigitalOut *relais[8]    = { &relL1, &relR1, &relL2, &relR2,
                                     &relL3, &relR3, &relL4, &relR4 };
    MbcDigitalIn  *stellpult[8] = { &inG1, &inR1, &inG2, &inR2, &inG3, &inR3, &inG4, &inR4 };

    // genPurpSize=10 (MBC_92_MM_H..MM_ART + 4 Byte Impulszeiten), extraEepromSize=10 (siehe
    // Mbc92Com.cpp EEX_*-Konstanten)

    static uint8_t sysArray[MBC_SYSARRAY_SIZE(10)];

    MbcUtils       utils(sysArray, 10, PIN_ADC_VSPG, PIN_ADC_PLUG, PIN_LED_GRUEN, PIN_LED_ROT,
                          PIN_ONEWIRE, 10);
    MbcCan         can(PIN_CS_CAN_MBCAN, utils);
    MbcBootStorage bootStorage(PIN_CS_BOOT_EEPROM);
    Mbc92Com       com(utils, can, bootStorage, anzeige, relais, stellpult, PIN_RESET);

//------ setup()/loop() ---------------------------------------------------------------------------

void setup()
{
    SPI.begin();

    anz1.begin(); anz2.begin(); anz3.begin(); anz4.begin();

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
