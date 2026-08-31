//*************************************************************************************************
//* mbc-93 mfx-Booster-Monitor auf Basis Maerklin 6017 - ATmega644 (MightyCore)                   *
//*************************************************************************************************
//* Pinbelegung entspricht der realen Hardware (feste Platinenleiterbahnen, keine freie Pinwahl   *
//* moeglich), SPI/CAN/Boot-EEPROM/LED/OneWire/Reset/ADC identisch zur Standard-Pinbelegung fuer  *
//* dieses Board. Arduino-Pin-Nummern anhand der tatsaechlichen MightyCore-standard-Variante      *
//* ermittelt (pins_arduino.h, lokal installiert unter                                            *
//* Arduino15/packages/MightyCore/hardware/avr/.../variants/standard/) - NICHT verlaesslich aus   *
//* dem Gedaechtnis oder aus anderen Sketches uebernehmen: PB0...PB7 = D0...D7, PD0...PD7 =       *
//* D8...D15, PC0...PC7 = D16...D23, PA0...PA7 = D24...D31 (alle 4 Ports aufsteigend).            *
//* | Funktion                            | Port,Pin | Arduino-Pin |                              *
//* |--------------------------------------|----------|-------------|                             *
//* | MBCAN-CS (MCP2515)                  | PB4      | D4          |                              *
//* | SPI MOSI/MISO/SCK                   | PB5/6/7  | D5/D6/D7 (Standard-SPI)         |          *
//* | OneWire (Temperatursensor)          | PB0      | D0          |                              *
//* | Reset-Pin                           | PA3      | D27         |                              *
//* | Boot-EEPROM-CS                       | PA2      | D26         |                             *
//* | LED gruen                           | PB1      | D1          |                              *
//* | LED rot                             | PB3      | D3          |                              *
//* | ADC Busspannung                     | ADC0/PA0 | A0          |                              *
//* | ADC Steckernetzteil                 | ADC1/PA1 | A1          |                              *
//* | Booster ON-Bit (ehem. PORTD Bit0)   | PD0      | D8          |                              *
//* | Booster SHORT-Bit (ehem. PORTD Bit1)| PD1      | D9          |                              *
//* | Kurzschluss-Erkennung (ehem. INT1)  | PD3      | D11         |                              *
//* | Luefter-PWM (ehem. OC2A)            | PD7      | D15         |                              *
//*************************************************************************************************
//* Deutlich weniger Pins belegt als bei den bisherigen Schalt-/Stellpult-Modulen dieses Projekts *
//* (kein Schaltdecoder). Frei: D2, D10, D12, D13, D14, D16...D25, D28...D31.                     *
//*************************************************************************************************
//* Zu verifizieren beim Testen: dass D11 (PD3/INT1) unter dem Arduino-Core per attachInterrupt() *
//* ansprechbar ist (INT1 ist auf ATmega644(P) hardwareseitig fest an PD3 gebunden), und ob       *
//* PD7/OC2A als PWM-Pin (analogWrite()) nutzbar ist (physisch ja, MightyCore sollte das          *
//* unterstuetzen).                                                                               *
//*************************************************************************************************
//* Bootloader-Vorbereitung (MightyCore, ATmega644 via ISP) - Dateien/Volltext siehe Ordner       *
//* Bootloaderinfos_MightyCore/ (README.txt, boards.txt, bootloaders/mbcan/mbcanboot.hex),        *
//* identisch fuer alle MBCAN-ATmega644(P)-Sketches:                                              *
//* 1) Arduino IDE - neuen Boardverwalter ergaenzen:                                              *
//* https://mcudude.github.io/MightyCore/package_MCUdude_MightyCore_index.json                    *
//* 2) Bei der Board-Verwaltung "MightyCore" installieren, IDE schliessen.                        *
//* 3) Im Pfad                                                                                    *
//* "C:\Users\<DeinName>\AppData\Local\Arduino15\packages\MightyCore\hardware\avr\y.y.y" die      *
//* Datei "boards.txt" und den Ordner "bootloaders" aus Bootloaderinfos_MightyCore/               *
//* hineinkopieren.                                                                               *
//* 4) Cache loeschen: "C:\Users\<DeinName>\AppData\Roaming\arduino-ide\". IDE neu starten.       *
//* 5) Arduino IDE starten, den Sketch auswaehlen und oeffnen.                                    *
//* 6) Unter "Werkzeuge" auf Prozessortyp "ATmega644" und Variante "644/644A" achten (NICHT       *
//* "644P/644PA" - diese Hardware bestueckt den originalen ATmega644, siehe Kopfkommentar). Jetzt *
//* sollte der Bootloader "MBCAN" angezeigt werden. Diesen auswaehlen. Zusaetzlich unter          *
//* "Werkzeuge -> BOD" den Wert "2,7V" auswaehlen (Brown-Out-Detection - verhindert, dass der     *
//* Prozessor bei kurzen Spannungseinbruechen mit korruptem SRAM-Inhalt weiterlaeuft, statt       *
//* sauber neu zu starten).                                                                       *
//* 7) Sketch zum Modul kompilieren.                                                              *
//* 8) "Binaerdateien exportieren" unter "Sketch" anstossen.                                      *
//* 9) Im Sketchordner liegt unter "<DeinSketch>\build\MightyCore.avr.644" die Datei              *
//* "xxx.with_bootloader.hex".                                                                    *
//* 10) Mit AVRStudio o.ae. per ISP diese Datei auf den ATmega644 aufspielen. FUSES: HIGH.BOOTSZ  *
//* auf 2048 Byte, HIGH.BOOTRST enabled, EXTENDED auf 0xFD (BOD 2,7V) - dann startet der          *
//* Prozessor zuerst mit dem Bootloader und dann mit dem Sketch.                                  *
//* 11) Damit ist der originale Bootloader mit dem kompilierten Sketch aufgespielt. Ab da nur     *
//* noch die Upgrade-Funktion des Parametriercenters nutzen und die Datei "xxx.ino.hex".          *
//* 12) Gilt nur fuer das Parametriercenter ab Version 3.0.0.0 - fruehere Versionen unterstuetzen *
//* die Arduino-IDE-HEX-Dateien nicht.                                                            *
//*************************************************************************************************
//* Datei: Mbc93_ATmega644_HW240501.ino                                                           *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen: V1.0.0 - 23.08.2026 - Nullversion                                                *
//*************************************************************************************************

#include <SPI.h>
#include <Mbc93Com.h>
#include <MbcDigitalOut.h>

//------ Pinbelegung --------------------------------------------------------------------------------

    const uint8_t PIN_CS_CAN_MBCAN   = 4;
    const uint8_t PIN_ONEWIRE        = 0;
    const uint8_t PIN_RESET          = 27;
    const uint8_t PIN_CS_BOOT_EEPROM = 26;
    const uint8_t PIN_LED_GRUEN      = 1;
    const uint8_t PIN_LED_ROT        = 3;
    const uint8_t PIN_ADC_VSPG       = A0;
    const uint8_t PIN_ADC_PLUG       = A1;
    const uint8_t PIN_BOOSTER_ON     = 8;
    const uint8_t PIN_BOOSTER_SHORT  = 9;
    const uint8_t PIN_SHORT_DETECT   = 11;
    const uint8_t PIN_FAN            = 15;

//------ Modulidentitaet (ASCII-Ziffern) -----------------------------------------------------------

    // [2]=Prozessor: 0=ATmega644P/1=ESP32-C3/2=RP2040-Pico/3=RP2040-Zero

    const uint8_t ART_NR[4] = { '9', '3', '0', '1' };
    const uint8_t SW_VER[3] = { '2', '0', '0' };
    const uint8_t HW_VER[6] = { '2', '4', '0', '5', '0', '1' };   // Hardware 24.05.01

//------ Objekte ------------------------------------------------------------------------------------

    MbcDigitalOut pinBoosterOn(PIN_BOOSTER_ON), pinBoosterShort(PIN_BOOSTER_SHORT);

    // genPurpSize=4 (STATUS, SW_1, SW_2, SH), extraEepromSize=3 (siehe EEX_93_*-Konstanten
    // in Mbc93Com.cpp)

    static uint8_t sysArray[MBC_SYSARRAY_SIZE(4)];

    MbcUtils       utils(sysArray, 4, PIN_ADC_VSPG, PIN_ADC_PLUG, PIN_LED_GRUEN, PIN_LED_ROT,
                          PIN_ONEWIRE, 3);
    MbcCan         can(PIN_CS_CAN_MBCAN, utils);
    MbcBootStorage bootStorage(PIN_CS_BOOT_EEPROM);
    Mbc93Com       com(utils, can, bootStorage, pinBoosterOn, pinBoosterShort,
                       PIN_SHORT_DETECT, PIN_FAN, PIN_RESET);

//------ setup()/loop() -----------------------------------------------------------------------------

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

