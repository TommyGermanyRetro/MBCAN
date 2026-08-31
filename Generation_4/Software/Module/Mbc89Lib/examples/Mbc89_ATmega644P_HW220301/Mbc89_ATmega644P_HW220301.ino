//*************************************************************************************************
//* mbc-89 Connect6021-Interface - ATmega644P (MightyCore, feste Pinbelegung)                     *
//*************************************************************************************************
//*************************************************************************************************
//* Pinbelegung entspricht der realen Hardware (feste Platinenleiterbahnen, keine freie Pinwahl   *
//* moeglich). Arduino-Pin-Nummern anhand der tatsaechlichen MightyCore-"standard"-Variante       *
//* ermittelt (pins_arduino.h, lokal installiert unter                                            *
//* Arduino15/packages/MightyCore/hardware/avr/.../variants/standard/) - NICHT verlaesslich aus   *
//* dem Gedaechtnis oder aus anderen Sketches uebernehmen: PB0...PB7 = D0...D7, PD0...PD7 =       *
//* D8...D15, PC0...PC7 = D16...D23, PA0...PA7 = D24...D31 (alle 4 Ports aufsteigend).            *
//*************************************************************************************************
//* | Funktion                             | Port,Pin | Arduino-Pin |                             *
//* |---------------------------------------|----------|-------------|                            *
//* | MBCAN-CS (MCP2515)                    | PB4      | D4          |                            *
//* | SPI MOSI/MISO/SCK                     | PB5/6/7  | D5/D6/D7    |                            *
//* | OneWire (Temperatursensor)            | PB0      | D0          |                            *
//* | Reset-Pin                             | PA3      | D27         |                            *
//* | Boot-EEPROM-CS                        | PA2      | D26         |                            *
//* | LED gruen                             | PB1      | D1          |                            *
//* | LED rot                               | PB3      | D3          |                            *
//* | ADC Busspannung                       | ADC0/PA0 | A0          |                            *
//* | ADC Steckernetzteil                   | ADC1/PA1 | A1          |                            *
//* | TRACK-Signal (INT0, Gleisdecoder)     | PD2      | D10         |                            *
//* | CU-SCL (Hardware-TWI, fest)           | PC0      | D16         |                            *
//* | CU-SDA (Hardware-TWI, fest)           | PC1      | D17         |                            *
//* | CU-STOP-IN / CU-STOP-OUT              | PC3/PC4  | D19/D20     |                            *
//* | CU-GO-IN / CU-GO-OUT                  | PC6/PC7  | D22/D23     |                            *
//* | CU-EXT (CU-Anwesenheitserkennung)     | PC5      | D21         |                            *
//*************************************************************************************************
//* Die CU-Bus- (PC0/PC1/PC3...PC7) und TRACK-Pins (PD2) sind NICHT als Konstruktor-Parameter     *
//* waehlbar - Mbc89Com.cpp steuert sie direkt ueber AVR-Register (siehe Klassenkommentar in      *
//* Mbc89Com.h). Kein Wire.begin() im Sketch - der CU-Bus verwaltet die TWI-Hardware exklusiv     *
//* selbst. Frei: D2, D9, D11...D15, D18, D24/D25, D28...D31.                                     *
//*************************************************************************************************
//*************************************************************************************************
//* WICHTIG: Diese Bibliothek ist wegen roher AVR-TWI-/Timer1-/Timer2-/INT0-Register              *
//* ausschliesslich fuer ATmega644P (MightyCore) vorgesehen - kein RP2040-/ESP32-Support (siehe   *
//* #error-Guard in Mbc89Com.h). Der Gleisdecoder-Anteil ist ungetestete Neuware fuer diesen Port *
//* (frisch aus der MM2-Spezifikation abgeleitete Impulslaengenfenster, siehe README) - vor       *
//* Dauerbetrieb mit Oszilloskop/Logic-Analyzer am echten TRACK-Signal verifizieren.              *
//*************************************************************************************************
//*************************************************************************************************
//* Bootloader-Vorbereitung (MightyCore, ATmega644P via ISP) - Dateien/Volltext siehe Ordner      *
//* Bootloaderinfos_MightyCore/ (README.txt, boards.txt, bootloaders/mbcan/mbcanboot.hex),        *
//* identisch fuer alle MBCAN-ATmega644P-Sketches:                                                *
//*************************************************************************************************
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
//*************************************************************************************************
//* Datei: Mbc89_ATmega644P_HW220301.ino                                                          *
//*************************************************************************************************
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//*************************************************************************************************
//* Aenderungen: V1.0.0 - 26.08.2026 - Nullversion                                                *
//*************************************************************************************************

#include <SPI.h>
#include <Mbc89Com.h>

//------ Pinbelegung ------------------------------------------------------------------------------

    const uint8_t PIN_CS_CAN_MBCAN   = 4;
    const uint8_t PIN_ONEWIRE        = 0;
    const uint8_t PIN_RESET          = 27;
    const uint8_t PIN_CS_BOOT_EEPROM = 26;
    const uint8_t PIN_LED_GRUEN      = 1;
    const uint8_t PIN_LED_ROT        = 3;
    const uint8_t PIN_ADC_VSPG       = A0;
    const uint8_t PIN_ADC_PLUG       = A1;

//------ Modulidentitaet (ASCII-Ziffern) ----------------------------------------------------------

    // [2]=Prozessor: 0=ATmega644P (bislang einzige Plattform dieser Bibliothek)

    const uint8_t ART_NR[4] = { '8', '9', '0', '2' };
    const uint8_t SW_VER[3] = { '2', '0', '0' };
    const uint8_t HW_VER[6] = { '2', '2', '0', '3', '0', '1' };   // Hardware 22.03.01

//------ Objekte ----------------------------------------------------------------------------------

    // genPurpSize=880 (SW_1/SW_2, 810 Byte Lok-Datenbank, 65 Byte Zubehoerprotokoll, Zentralentyp-
    // Mapping, Basisadressblock, I2C-Frequenz), extraEepromSize=70 (siehe EEX_89_*-Konstanten in
    // Mbc89Com.cpp)

    static uint8_t sysArray[MBC_SYSARRAY_SIZE(880)];

    Mbc89Utils       utils(sysArray, 880, PIN_ADC_VSPG, PIN_ADC_PLUG, PIN_LED_GRUEN,
                            PIN_LED_ROT, PIN_ONEWIRE, 70);
    Mbc89Can         can(PIN_CS_CAN_MBCAN, utils);
    Mbc89BootStorage bootStorage(PIN_CS_BOOT_EEPROM);
    Mbc89Com         com(utils, can, bootStorage, PIN_RESET);

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
    // waehrenddessen ca. 10 s durchgehend ORANGE (siehe Mbc89Utils::waitFilled())

    utils.waitFilled();
}

void loop()
{
    utils.update();
    utils.check();

    com.update();
}

