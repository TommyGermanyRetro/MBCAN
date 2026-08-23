//*************************************************************************************************
//* mbc-90 S88-Rueckmeldedecoder, 16 Kontakte - ATmega644P (MightyCore, AELTERE Hardware-Revision)*
//*************************************************************************************************
//* Pinbelegung entspricht der aelteren Hardware-Revision (Hardware-Datum 04.03.2018) - feste     *
//* Platinenleiterbahnen, keine freie Pinwahl moeglich. Pin- UND Ablauflogik sind dabei identisch *
//* zur neueren Hardware (siehe Mbc90_ATmega644P_HW231125) - beide Revisionen teilen dieselbe     *
//* Platinenbelegung, nur Hardware-Datum/ArtNum4/HW_VER unterscheiden sich                        *
//* (Bus0/500/1500/2500-Grenzwerte, RM-Bitpolaritaet und PCF8574-Remap identisch). Arduino-Pin-   *
//* Nummern anhand der tatsaechlichen MightyCore-"standard"-Variante ermittelt (pins_arduino.h,   *
//* lokal installiert unter Arduino15/packages/MightyCore/hardware/avr/.../variants/standard/) -  *
//* NICHT verlaesslich aus dem Gedaechtnis oder aus anderen Sketches uebernehmen: PB0...PB7 =     *
//* D0...D7, PD0...PD7 = D8...D15, PC0...PC7 = D16...D23, PA0...PA7 = D24...D31 (alle 4 Ports     *
//* aufsteigend). SPI/CAN/Boot-EEPROM/LED/OneWire/Reset/ADC identisch zur Standard-Pinbelegung    *
//* fuer dieses Board. Wie bei der neueren Hardware haengt die Stellungsanzeige an 2 PCF8574 auf  *
//* dem nativen I2C-Bus (TWI, PC0=SDA/PC1=SCL), die bei mbc-90                                    *
//* frei sind (IN08-15 belegen nur C2/C3/C4/C6/C7).                                               *
//*                                                                                               *
//* MCP_INT_CH0 wird NICHT verdrahtet: MbcCan empfaengt per Polling.                              *
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
//* | I2C SDA/SCL (2x PCF8574 Anzeige)    | PC0/PC1  | D16/D17 (Standard-Wire)   |                *
//* | IN00/IN01/IN02                      | PA5/6/7  | D29/D30/D31 |                              *
//* | IN03/IN04/IN05/IN06/IN07            | PD0/1/2/3/4 | D8/D9/D10/D11/D12      |                *
//* | IN08/IN09/IN10/IN11/IN12            | PC7/6/4/3/2 | D23/D22/D20/D19/D18   |                 *
//* | IN13/IN14/IN15                      | PD7/5/6  | D15/D13/D14              |                 *
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
//* Datei: Mbc90_ATmega644P_HW180304.ino                                                          *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen:                                                                                  *
//* V3.0.0 - 15.08.2026 - Nullversion                                                             *
//*************************************************************************************************

#include <SPI.h>
#include <Wire.h>
#include <Mbc90Com.h>
#include <MbcDigitalIn.h>
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

    // IN00...IN15 = PA5,PA6,PA7,PD0,PD1,PD2,PD3,PD4,PC7,PC6,PC4,PC3,PC2,PD7,PD5,PD6

    const uint8_t PIN_IN[16] = { 29, 30, 31, 8, 9, 10, 11, 12, 23, 22, 20, 19, 18, 15, 13, 14 };

//------ Modulidentitaet (ASCII-Ziffern) ----------------------------------------------------------

    // ArtNum4=1 (nicht '2' wie beim neueren Sketch HW231125) - kennzeichnet diese aeltere
    // Hardware-Revision

    const uint8_t ART_NR[4] = { '9', '0', '0', '1' };   // [2]=Prozessor: 0=ATmega644P/1=ESP32-C3/2=RP2040-Pico/3=RP2040-Zero
    const uint8_t SW_VER[3] = { '3', '0', '0' };
    const uint8_t HW_VER[6] = { '1', '8', '0', '3', '0', '4' };   // Hardware 18.03.04

//------ Objekte ----------------------------------------------------------------------------------

    // Kontakteingaenge: native Pins, aktiv-LOW mit PullUp (Default des MbcDigitalIn-Konstruktors).
    // Stellungsanzeige: 2 PCF8574 auf dem nativen I2C-Bus (Bausteinnummer 0 = Kontakt 1-8,
    // Bausteinnummer 1 = Kontakt 9-16, siehe Mbc90Com-Klassenkommentar)

    MbcDigitalIn in00(PIN_IN[0]),  in01(PIN_IN[1]),  in02(PIN_IN[2]),  in03(PIN_IN[3]);
    MbcDigitalIn in04(PIN_IN[4]),  in05(PIN_IN[5]),  in06(PIN_IN[6]),  in07(PIN_IN[7]);
    MbcDigitalIn in08(PIN_IN[8]),  in09(PIN_IN[9]),  in10(PIN_IN[10]), in11(PIN_IN[11]);
    MbcDigitalIn in12(PIN_IN[12]), in13(PIN_IN[13]), in14(PIN_IN[14]), in15(PIN_IN[15]);

    MbcDigitalIn *inputs[16] = { &in00, &in01, &in02, &in03, &in04, &in05, &in06, &in07,
                                  &in08, &in09, &in10, &in11, &in12, &in13, &in14, &in15 };

    MbcI2cBus  i2cBus;
    MbcPcf8574 displayLow(i2cBus, 0);
    MbcPcf8574 displayHigh(i2cBus, 1);

    // genPurpSize=20 (S88Addr-H/-L, Rueckmeldestatus-H/-L + 16 Byte Integrationszeit),
    // extraEepromSize=18 (siehe Mbc90Com.cpp EEX_*-Konstanten) - unveraendert gegenueber der
    // neueren Hardware, da rein strukturell und unabhaengig von der Pin-/Backend-Wahl

    static uint8_t sysArray[MBC_SYSARRAY_SIZE(20)];

    MbcUtils       utils(sysArray, 20, PIN_ADC_VSPG, PIN_ADC_PLUG, PIN_LED_GRUEN, PIN_LED_ROT,
                          PIN_ONEWIRE, 18);
    MbcCan         can(PIN_CS_CAN_MBCAN, utils);
    MbcBootStorage bootStorage(PIN_CS_BOOT_EEPROM);
    Mbc90Com       com(utils, can, bootStorage, inputs, displayLow, displayHigh, PIN_RESET);

//------ setup()/loop() ---------------------------------------------------------------------------

void setup()
{
    SPI.begin();
    Wire.begin();
    i2cBus.begin();

    for (uint8_t index = 0; index < 16; index++) inputs[index]->begin();

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
