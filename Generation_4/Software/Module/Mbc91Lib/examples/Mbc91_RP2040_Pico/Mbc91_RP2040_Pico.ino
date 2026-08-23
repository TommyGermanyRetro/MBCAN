//*************************************************************************************************
//* mbc-91 Magnetartikel-Schaltdecoder - Raspberry Pi Pico (RP2040), Pinvorschlag                 *
//*************************************************************************************************
//* WICHTIG - OTA-Partitionsschema: Dieser Sketch benoetigt eine reservierte zweite (OTA-)        *
//* Flash-Partition, siehe MbcOta.h. Anders als beim ESP32 gibt es dafuer KEINE partitions.csv -  *
//* in der Arduino-IDE unter Tools/Werkzeuge -> Flash Size eine Variante mit "OTA" im Namen       *
//* waehlen (beim 2-MByte-Standard-Pico sinngemaess "2MB (Sketch: ..., OTA: ...)", genauer        *
//* Wortlaut je nach arduino-pico-Version) - ohne das hat ein OTA-Update keine Zielpartition zum  *
//* Beschreiben und schlaegt fehl.                                                                *
//*                                                                                               *
//* Freie Pinwahl - ANNAHME, vor dem Verdrahten gegen das Pico-Pinout pruefen.                    *
//*                                                                                               *
//* Genug native Pins fuer alle 8 Relaisausgaenge und 8 Stellpult-Eingaenge sowie fuer die        *
//* Positionsanzeige einen zusaetzlichen SPI-MCP23S17 - kein weiterer Expander noetig.            *
//*                                                                                               *
//* Der Boot-Speicherchip fuer das Firmware-Update-Staging ist hier ein W25Q64-SPI-NOR-Flash (8   *
//* MByte) statt des bisherigen 24LC1024-EEPROMs (128 KByte) - dessen Kapazitaet fuer groessere   *
//* Firmware-Images zu klein war. MbcBootStorage waehlt das Backend ueber den zweiten             *
//* Konstruktor-Parameter (FLASH_W25Q64), siehe dessen Kopfkommentar fuer Details zur             *
//* Sektor-Loesch-Strategie.                                                                      *
//*                                                                                               *
//* Kein Reset-Pin: RP2040 nutzt den Software-Reset des Cores (MbcPlatform).                      *
//*                                                                                               *
//* | Funktion                     | GPIO                        |                                *
//* |-------------------------------|-----------------------------|                               *
//* | SPI MOSI/MISO/SCK             | Standard-SPI (SPI0: 19/16/18)|                              *
//* | MBCAN-CS (MCP2515)            | GPIO20                      |                               *
//* | Boot-Speicher-CS (W25Q64)     | GPIO22                      |                               *
//* | MCP23S17-CS (Positionsanzeige)| GPIO2                       |                               *
//* | OneWire                       | GPIO13                      |                               *
//* | LED gruen                     | GPIO14                      |                               *
//* | LED rot                       | GPIO15                      |                               *
//* | ADC Busspannung               | GPIO26 (ADC0)               |                               *
//* | ADC Steckernetzteil           | GPIO27 (ADC1)               |                               *
//* | REL_1_L / REL_1_R             | GPIO4 / GPIO5               |                               *
//* | REL_2_L / REL_2_R             | GPIO6 / GPIO7               |                               *
//* | REL_3_L / REL_3_R             | GPIO8 / GPIO9               |                               *
//* | REL_4_L / REL_4_R             | GPIO10 / GPIO11             |                               *
//* | IN_1_G / IN_1_R               | GPIO0 / GPIO1               |                               *
//* | IN_2_G / IN_2_R               | GPIO12 / GPIO17             |                               *
//* | IN_3_G / IN_3_R               | GPIO21 / GPIO23             |                               *
//* | IN_4_G / IN_4_R               | GPIO24 / GPIO25             |                               *
//*************************************************************************************************
//* Datei: Mbc91_RP2040_Pico.ino                                                                  *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen:                                                                                  *
//* V1.0.0 - 06.08.2026 - Nullversion                                                             *
//*************************************************************************************************

#include <SPI.h>
#include <Mbc91Com.h>
#include <MbcMcp23x17.h>
#include <MbcDigitalOut.h>
#include <MbcDigitalIn.h>

//------ Pinbelegung ------------------------------------------------------------------------------

    const uint8_t PIN_CS_CAN_MBCAN    = 20;
    const uint8_t PIN_CS_BOOT_STORAGE = 22;
    const uint8_t PIN_CS_MCP23S17_ANZ = 2;   // Positionsanzeige
    const uint8_t PIN_ONEWIRE        = 13;
    const uint8_t PIN_LED_GRUEN      = 14;
    const uint8_t PIN_LED_ROT        = 15;
    const uint8_t PIN_ADC_VSPG       = 26;
    const uint8_t PIN_ADC_PLUG       = 27;

    const uint8_t PIN_REL_1_L = 4,  PIN_REL_1_R = 5;
    const uint8_t PIN_REL_2_L = 6,  PIN_REL_2_R = 7;
    const uint8_t PIN_REL_3_L = 8,  PIN_REL_3_R = 9;
    const uint8_t PIN_REL_4_L = 10, PIN_REL_4_R = 11;

    const uint8_t PIN_IN_1_G = 0,  PIN_IN_1_R = 1;
    const uint8_t PIN_IN_2_G = 12, PIN_IN_2_R = 17;
    const uint8_t PIN_IN_3_G = 21, PIN_IN_3_R = 23;
    const uint8_t PIN_IN_4_G = 24, PIN_IN_4_R = 25;

//------ Modulidentitaet (ASCII-Ziffern) ----------------------------------------------------------

    const uint8_t ART_NR[4] = { '9', '1', '2', '1' };   // [2]=Prozessor: 0=ATmega644P/1=ESP32-C3/2=RP2040-Pico/3=RP2040-Zero
    const uint8_t SW_VER[3] = { '1', '0', '0' };
    const uint8_t HW_VER[6] = { '2', '4', '0', '4', '0', '1' };   // Hardware 24.04.01

//------ Objekte ----------------------------------------------------------------------------------

    MbcMcp23x17 indicator(PIN_CS_MCP23S17_ANZ);

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

    // genPurpSize=10, extraEepromSize=9 (siehe Mbc91Com.cpp EEX_*-Konstanten)

    static uint8_t sysArray[MBC_SYSARRAY_SIZE(10)];

    MbcUtils       utils(sysArray, 10, PIN_ADC_VSPG, PIN_ADC_PLUG, PIN_LED_GRUEN, PIN_LED_ROT,
                          PIN_ONEWIRE, 9);
    MbcCan         can(PIN_CS_CAN_MBCAN, utils);
    MbcBootStorage bootStorage(PIN_CS_BOOT_STORAGE, MbcBootStorageBackend::FLASH_W25Q64);
    Mbc91Com       com(utils, can, bootStorage, indicator, relais, stellpult);

//------ Status der CAN-Initialisierung (fuer die wiederkehrende Fehlermeldung in loop()) ---------

    bool canOk = false;

//------ setup()/loop() ---------------------------------------------------------------------------

void setup()
{
    // MbcUtils schon vor der ersten Serial-Ausgabe begin()en, damit beim
    // anschliessenden Warten auf die USB-CDC-Verbindung tick-aware (utils.update())
    // statt mit einem blockierenden delay() gewartet werden kann.

    SPI.begin();

    // GPIOA: Bit 3-6 Eingang (Bestueckungsnibble, PullUp extern, Rest unbenutzt), GPIOB: Ausgang
    // (8 Stellungs-LEDs) - siehe Mbc91Com::stateDisplay()/beginModule()

    bool indicatorOk = indicator.begin(0xfc, 0x00, 0x00, 0x00);

    relL1.begin(); relR1.begin(); relL2.begin(); relR2.begin();
    relL3.begin(); relR3.begin(); relL4.begin(); relR4.begin();

    inG1.begin(); inR1.begin(); inG2.begin(); inR2.begin();
    inG3.begin(); inR3.begin(); inG4.begin(); inR4.begin();

    utils.begin(ART_NR, SW_VER, HW_VER);

    Serial.begin(115200);

    // USB-CDC braucht nach dem Verbindungsaufbau etwas Zeit, bis der Host das Geraet
    // erkannt hat - sonst gehen die ersten Serial-Ausgaben verloren.

    unsigned long usbStart = millis();
    while ((unsigned long) (millis() - usbStart) < 2000) utils.update();

    Serial.println("\n[Mbc91] Start");

    Serial.print("[Mbc91] MCP23S17 (Positionsanzeige): ");
    Serial.println(indicatorOk ? "OK" : "FEHLER - Baustein an GPIO2 nicht gefunden");

    canOk = can.begin();
    Serial.print("[Mbc91] MBCAN-Bus (MCP2515): ");
    Serial.println(canOk ? "OK" : "FEHLER - MCP2515 an GPIO20 nicht gefunden");

    bool bootStorageOk = bootStorage.begin();
    Serial.print("[Mbc91] W25Q64 (Boot-Speicher): ");
    Serial.println(bootStorageOk ? "OK" : "FEHLER - Baustein an GPIO22 nicht gefunden/falsche JEDEC-ID");

    com.begin();
    com.beginModule();

    // Warte, bis alle Statuswerte (Vspg/Plug/Temperatur) einmal ermittelt wurden - blinkt
    // waehrenddessen ca. 10 s durchgehend ORANGE (siehe MbcUtils::waitFilled())

    utils.waitFilled();

    Serial.println("[Mbc91] Bereit");
}

void loop()
{
    utils.update();
    utils.check();

    if (utils.eventSekunde() && !canOk)
    {
        Serial.println("[Mbc91] FEHLER: MBCAN-Bus (MCP2515) weiterhin nicht gefunden!");
    }

    com.update();
}
