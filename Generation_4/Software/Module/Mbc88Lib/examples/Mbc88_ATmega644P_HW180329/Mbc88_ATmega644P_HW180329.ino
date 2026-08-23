//*************************************************************************************************
//* mbc-88 S88-Rueckmeldedecoder - ATmega644P (MightyCore, aeltere Hardware-Generation)           *
//*************************************************************************************************
//* Pinbelegung entspricht der realen Hardware dieser aelteren Revision (feste                    *
//* Platinenleiterbahnen, keine freie Pinwahl moeglich). Pin- UND Ablauflogik sind Byte-fuer-Byte *
//* identisch zur neueren Hardware (siehe Mbc88_ATmega644P_HW240106) - beide Revisionen teilen    *
//* dieselbe Platinenbelegung, nur Hardware-Datum/ArtNum4/HW_VER unterscheiden sich. Arduino-Pin- *
//* Nummern anhand der tatsaechlichen MightyCore-"standard"-Variante ermittelt (pins_arduino.h,   *
//* lokal installiert unter Arduino15/packages/MightyCore/hardware/avr/.../variants/standard/) -  *
//* NICHT verlaesslich aus dem Gedaechtnis oder aus anderen Sketches uebernehmen: PB0...PB7 =     *
//* D0...D7, PD0...PD7 = D8...D15, PC0...PC7 = D16...D23, PA0...PA7 = D24...D31 (alle 4 Ports     *
//* aufsteigend). SPI/CAN/Boot-EEPROM/LED/OneWire/Reset/ADC identisch zur Standard-Pinbelegung    *
//* fuer dieses Board.                                                                            *
//*                                                                                               *
//* MCP_INT_CH0 wird NICHT verdrahtet: MbcCan empfaengt per Polling.                              *
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
//* | IN00/IN01/IN02                      | PA5/6/7  | D29/D30/D31 |                              *
//* | IN03/IN04/IN05/IN06/IN07            | PD0/1/2/3/4 | D8/D9/D10/D11/D12       |               *
//* | OUT00/OUT01/OUT02/OUT03             | PC7/6/5/4 | D23/D22/D21/D20           |               *
//* | OUT04/OUT05/OUT06/OUT07             | PC3/2/1/0 | D19/D18/D17/D16           |               *
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
//* Datei: Mbc88_ATmega644P_HW180329.ino                                                          *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen:                                                                                  *
//* V3.0.0 - 15.08.2026 - Nullversion                                                             *
//*************************************************************************************************

#include <SPI.h>
#include <Mbc88Com.h>
#include <MbcDigitalOut.h>
#include <MbcDigitalIn.h>

//------ Pinbelegung ------------------------------------------------------------------------------

    const uint8_t PIN_CS_CAN_MBCAN   = 4;
    const uint8_t PIN_ONEWIRE        = 0;
    const uint8_t PIN_RESET          = 27;
    const uint8_t PIN_CS_BOOT_EEPROM = 26;
    const uint8_t PIN_LED_GRUEN      = 1;
    const uint8_t PIN_LED_ROT        = 3;
    const uint8_t PIN_ADC_VSPG       = A0;
    const uint8_t PIN_ADC_PLUG       = A1;

    // IN00...IN07 = PA5,PA6,PA7,PD0,PD1,PD2,PD3,PD4

    const uint8_t PIN_IN[8]  = { 29, 30, 31, 8, 9, 10, 11, 12 };

    // OUT00...OUT07 = PC7,PC6,PC5,PC4,PC3,PC2,PC1,PC0

    const uint8_t PIN_OUT[8] = { 23, 22, 21, 20, 19, 18, 17, 16 };

//------ Modulidentitaet (ASCII-Ziffern) ----------------------------------------------------------

    // ArtNum4=1 fuer diese aeltere Hardware-Revision (nicht '2' wie beim neueren Sketch HW240106)

    const uint8_t ART_NR[4] = { '8', '8', '0', '1' };   // [2]=Prozessor: 0=ATmega644P/1=ESP32-C3/2=RP2040-Pico/3=RP2040-Zero
    const uint8_t SW_VER[3] = { '3', '0', '0' };
    const uint8_t HW_VER[6] = { '1', '8', '0', '3', '2', '9' };   // Hardware 18.03.29

//------ Objekte ----------------------------------------------------------------------------------

    // Kontakteingaenge: native Pins, aktiv-LOW mit PullUp (Default des MbcDigitalIn-Konstruktors).
    // Status-LED-Ausgaenge: ebenfalls aktiv-LOW auf dieser Platine (LOW = LED an), daher
    // activeLow=true - Index-Konvention siehe Mbc88Com-Klassenkommentar

    MbcDigitalIn  in0(PIN_IN[0]),  in1(PIN_IN[1]),  in2(PIN_IN[2]),  in3(PIN_IN[3]);
    MbcDigitalIn  in4(PIN_IN[4]),  in5(PIN_IN[5]),  in6(PIN_IN[6]),  in7(PIN_IN[7]);

    MbcDigitalOut out0(PIN_OUT[0], true), out1(PIN_OUT[1], true);
    MbcDigitalOut out2(PIN_OUT[2], true), out3(PIN_OUT[3], true);
    MbcDigitalOut out4(PIN_OUT[4], true), out5(PIN_OUT[5], true);
    MbcDigitalOut out6(PIN_OUT[6], true), out7(PIN_OUT[7], true);

    MbcDigitalIn  *inputs[8]  = { &in0, &in1, &in2, &in3, &in4, &in5, &in6, &in7 };
    MbcDigitalOut *outputs[8] = { &out0, &out1, &out2, &out3, &out4, &out5, &out6, &out7 };

    // genPurpSize=12 (S88Addr-H/-L, Halbmodul, Rueckmeldestatus + 8 Byte Integrationszeit),
    // extraEepromSize=11 (siehe Mbc88Com.cpp EEX_*-Konstanten) - unveraendert gegenueber der
    // neueren Hardware, da rein strukturell und unabhaengig von der Pin-/Backend-Wahl

    static uint8_t sysArray[MBC_SYSARRAY_SIZE(12)];

    MbcUtils       utils(sysArray, 12, PIN_ADC_VSPG, PIN_ADC_PLUG, PIN_LED_GRUEN, PIN_LED_ROT,
                          PIN_ONEWIRE, 11);
    MbcCan         can(PIN_CS_CAN_MBCAN, utils);
    MbcBootStorage bootStorage(PIN_CS_BOOT_EEPROM);
    Mbc88Com       com(utils, can, bootStorage, inputs, outputs, PIN_RESET);

//------ setup()/loop() ---------------------------------------------------------------------------

void setup()
{
    SPI.begin();

    for (uint8_t index = 0; index < 8; index++) inputs[index]->begin();
    for (uint8_t index = 0; index < 8; index++) outputs[index]->begin();

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
