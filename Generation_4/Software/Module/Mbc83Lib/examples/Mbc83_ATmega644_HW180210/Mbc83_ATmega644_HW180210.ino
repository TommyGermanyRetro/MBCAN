//*************************************************************************************************
//* mbc-83 4-fach Relais-/Magnetartikeldecoder mit Dimmer - ATmega644 (MightyCore)                *
//*************************************************************************************************
//* Pinbelegung entspricht der aelteren Hardware-Revision (Hardware-Datum 10.02.2018) - identisch *
//* zur neueren Hardware (siehe Mbc83_ATmega644_HW240323), gleiche Platinenbelegung, nur das      *
//* Hardware-Datum unterscheidet sich (ArtNum4 bleibt bei 1).                                     *
//*                                                                                               *
//* BEKANNTER, BEWUSST NICHT NACHGEBILDETER Unterschied dieser aelteren Hardware-Generation: die  *
//* Firmware dieser Revision setzte die interne Aktorstellung (MBC_83_MM) beim Stellpultauswerten *
//* anhand des zuletzt empfangenen CAN-Nachrichtenfelds statt anhand des lokal ermittelten        *
//* Ausgangszustands - ein Bezugsfehler ohne Zusammenhang zum tatsaechlich gedrueckten Taster,    *
//* der bei der neueren Hardware-Revision bereits korrigiert war (dort wird korrekt der lokale    *
//* Ausgangszustand verwendet, siehe Klassenkommentar in Mbc83Com.h). Betrifft nur die interne    *
//* Stellungs-Bitablage bei einem Stellpultdruck, NICHT die tatsaechliche Relaisschaltung (die    *
//* war bereits korrekt) und NICHT das an die CSx gesendete Telegramm. Mbc83Com wird von allen    *
//* Hardware-Revisionen gemeinsam genutzt (siehe Mbc83Com::switchboardCheck()) und bildet         *
//* durchgehend die bereits korrigierte, neuere Verhaltensweise nach - dieser Sketch repliziert   *
//* den alten Fehler daher NICHT.                                                                 *
//*                                                                                               *
//* SPI/CAN/Boot-EEPROM/LED/OneWire/Reset/ADC identisch zur Standard-Pinbelegung fuer dieses      *
//* Board. Arduino-Pin-Nummern anhand der tatsaechlichen MightyCore-"standard"-Variante ermittelt *
//* (pins_arduino.h, lokal installiert unter                                                      *
//* Arduino15/packages/MightyCore/hardware/avr/.../variants/standard/) - NICHT verlaesslich aus   *
//* dem Gedaechtnis oder aus anderen Sketches uebernehmen: PB0...PB7 = D0...D7, PD0...PD7 =       *
//* D8...D15, PC0...PC7 = D16...D23, PA0...PA7 = D24...D31 (alle 4 Ports aufsteigend). Alle 32    *
//* Pins sind belegt (dicht geroutetes Board).                                                    *
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
//* | Dimmer (PWM, OC2A)                  | PD7      | D15         |                              *
//* | Relais 1 Rot(L)/Gruen(R)            | PD0/PD2  | D8/D10      |                              *
//* | Relais 2 Rot(L)/Gruen(R)            | PD4/PD6  | D12/D14     |                              *
//* | Relais 3 Rot(L)/Gruen(R)            | PC7/PC5  | D23/D21     |                              *
//* | Relais 4 Rot(L)/Gruen(R)            | PC3/PC1  | D19/D17     |                              *
//* | Rueckmeldung Relais 1/2/3/4         | PA4/5/6/7| D28/D29/D30/D31                            *
//* | Stellpult 1 Rot/Gruen                | PD3/PD1  | D11/D9      |                             *
//* | Stellpult 2 Rot/Gruen                | PB2/PD5  | D2/D13      |                             *
//* | Stellpult 3 Rot/Gruen                | PC4/PC6  | D20/D22     |                             *
//* | Stellpult 4 Rot/Gruen                | PC0/PC2  | D16/D18     |                             *
//*************************************************************************************************
//* Zu verifizieren beim Testen: ob die Relais tatsaechlich aktiv-HIGH sind (keine Hinweise auf   *
//* eine Inversion) und ob PD7/OC2A unter dem Arduino-Core als PWM-Pin (analogWrite()) nutzbar    *
//* ist (physisch ja, MightyCore sollte das unterstuetzen).                                       *
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
//* Datei: Mbc83_ATmega644_HW180210.ino                                                           *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen:                                                                                  *
//* V3.0.0 - 21.08.2026 - Nullversion                                                             *
//*************************************************************************************************

#include <SPI.h>
#include <Mbc83Com.h>
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
    const uint8_t PIN_DIMMER         = 15;

    // Reihenfolge Ausgang1-Rot(L),Ausgang1-Gruen(R),...,Ausgang4-Gruen (siehe Mbc83Com.h)

    const uint8_t PIN_REL_1_L = 8,  PIN_REL_1_R = 10;
    const uint8_t PIN_REL_2_L = 12, PIN_REL_2_R = 14;
    const uint8_t PIN_REL_3_L = 23, PIN_REL_3_R = 21;
    const uint8_t PIN_REL_4_L = 19, PIN_REL_4_R = 17;

    const uint8_t PIN_REL_IN[4] = { 28, 29, 30, 31 };

    // Reihenfolge Ausgang1-Rot,Ausgang1-Gruen,...,Ausgang4-Gruen (gleiche Konvention wie die
    // Relaisausgaenge oben)

    const uint8_t PIN_IN_1_R = 11, PIN_IN_1_G = 9;
    const uint8_t PIN_IN_2_R = 2,  PIN_IN_2_G = 13;
    const uint8_t PIN_IN_3_R = 20, PIN_IN_3_G = 22;
    const uint8_t PIN_IN_4_R = 16, PIN_IN_4_G = 18;

//------ Modulidentitaet (ASCII-Ziffern) ----------------------------------------------------------

    // [2]=Prozessor: 0=ATmega644P/1=ESP32-C3/2=RP2040-Pico/3=RP2040-Zero

    const uint8_t ART_NR[4] = { '8', '3', '0', '1' };
    const uint8_t SW_VER[3] = { '3', '0', '0' };
    const uint8_t HW_VER[6] = { '1', '8', '0', '2', '1', '0' };   // Hardware 18.02.10

//------ Objekte ----------------------------------------------------------------------------------

    MbcDigitalOut rel1L(PIN_REL_1_L), rel1R(PIN_REL_1_R);
    MbcDigitalOut rel2L(PIN_REL_2_L), rel2R(PIN_REL_2_R);
    MbcDigitalOut rel3L(PIN_REL_3_L), rel3R(PIN_REL_3_R);
    MbcDigitalOut rel4L(PIN_REL_4_L), rel4R(PIN_REL_4_R);

    MbcDigitalOut *relayOut[8] =
        { &rel1L, &rel1R, &rel2L, &rel2R, &rel3L, &rel3R, &rel4L, &rel4R };

    MbcDigitalIn relIn1(PIN_REL_IN[0]), relIn2(PIN_REL_IN[1]);
    MbcDigitalIn relIn3(PIN_REL_IN[2]), relIn4(PIN_REL_IN[3]);

    MbcDigitalIn *relayIn[4] = { &relIn1, &relIn2, &relIn3, &relIn4 };

    MbcDigitalIn in1R(PIN_IN_1_R), in1G(PIN_IN_1_G), in2R(PIN_IN_2_R), in2G(PIN_IN_2_G);
    MbcDigitalIn in3R(PIN_IN_3_R), in3G(PIN_IN_3_G), in4R(PIN_IN_4_R), in4G(PIN_IN_4_G);

    MbcDigitalIn *switchboard[8] = { &in1R, &in1G, &in2R, &in2G, &in3R, &in3G, &in4R, &in4G };

    // genPurpSize=11 (MM-Addr H/L, Aktorstellung, Art, Auto-Aus, Meldezyklus, Dimmer,
    // 4x Spulentiming), extraEepromSize=10 (siehe EEX_83_*-Konstanten in Mbc83Com.cpp)

    static uint8_t sysArray[MBC_SYSARRAY_SIZE(11)];

    MbcUtils       utils(sysArray, 11, PIN_ADC_VSPG, PIN_ADC_PLUG, PIN_LED_GRUEN, PIN_LED_ROT,
                          PIN_ONEWIRE, 10);
    MbcCan         can(PIN_CS_CAN_MBCAN, utils);
    MbcBootStorage bootStorage(PIN_CS_BOOT_EEPROM);
    Mbc83Com       com(utils, can, bootStorage, relayOut, relayIn, switchboard, PIN_DIMMER,
                       PIN_RESET);

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
