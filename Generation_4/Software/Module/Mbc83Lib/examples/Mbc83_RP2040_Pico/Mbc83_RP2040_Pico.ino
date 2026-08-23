//*************************************************************************************************
//* mbc-83 4-fach Relais-/Magnetartikeldecoder mit Dimmer - Raspberry Pi Pico (RP2040)            *
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
//* Genug native Pins fuer die 8 Relaisausgaenge, 8 Stellpult-Eingaenge UND den Dimmer - NUR die  *
//* 4 Rueckmelde-Eingaenge passen nicht mehr mit dazu und haengen deshalb an einem zusaetzlichen  *
//* SPI-MCP23S17 (nur 4 von 16 Bit genutzt - ein voller Expander nur fuer 4 Eingaenge ist         *
//* unschoen, aber die einfachste Loesung, die den restlichen Sketch sauber "alles nativ"         *
//* laesst).                                                                                      *
//*                                                                                               *
//* Der Boot-Speicherchip fuer das Firmware-Update-Staging ist hier ein W25Q64-SPI-NOR-Flash (8   *
//* MByte) statt des bisherigen 24LC1024-EEPROMs (128 KByte) - dessen Kapazitaet fuer groessere   *
//* Firmware-Images zu klein war. MbcBootStorage waehlt das Backend ueber den zweiten             *
//* Konstruktor-Parameter (FLASH_W25Q64), siehe dessen Kopfkommentar fuer Details zur             *
//* Sektor-Loesch-Strategie.                                                                      *
//*                                                                                               *
//* Kein Reset-Pin: RP2040 nutzt den Software-Reset des Cores (MbcPlatform).                      *
//*                                                                                               *
//* | Funktion                       | GPIO                          |                            *
//* |---------------------------------|--------------------------------|                          *
//* | SPI MOSI/MISO/SCK               | Standard-SPI (SPI0: 19/16/18) |                           *
//* | MBCAN-CS (MCP2515)              | GPIO20                        |                           *
//* | Boot-Speicher-CS (W25Q64)       | GPIO22                        |                           *
//* | MCP23S17-CS (Rueckmeldung)      | GPIO2                         |                           *
//* | OneWire                         | GPIO13                        |                           *
//* | LED gruen                       | GPIO14                        |                           *
//* | LED rot                         | GPIO15                        |                           *
//* | ADC Busspannung                 | GPIO26 (ADC0)                 |                           *
//* | ADC Steckernetzteil             | GPIO27 (ADC1)                 |                           *
//* | Dimmer (PWM)                    | GPIO3                         |                           *
//* | Relais 1 Rot(L)/Gruen(R)        | GPIO4 / GPIO5                 |                           *
//* | Relais 2 Rot(L)/Gruen(R)        | GPIO6 / GPIO7                 |                           *
//* | Relais 3 Rot(L)/Gruen(R)        | GPIO8 / GPIO9                 |                           *
//* | Relais 4 Rot(L)/Gruen(R)        | GPIO10 / GPIO11               |                           *
//* | Rueckmeldung Relais 1..4        | MCP23S17 GPIOA Bit 0...3      |                           *
//* | Stellpult 1 Rot/Gruen            | GPIO1 / GPIO0                 |                          *
//* | Stellpult 2 Rot/Gruen            | GPIO17 / GPIO12               |                          *
//* | Stellpult 3 Rot/Gruen            | GPIO23 / GPIO21               |                          *
//* | Stellpult 4 Rot/Gruen            | GPIO25 / GPIO24               |                          *
//*************************************************************************************************
//* Datei: Mbc83_RP2040_Pico.ino                                                                  *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen:                                                                                  *
//* V1.0.0 - 21.08.2026 - Nullversion                                                             *
//*************************************************************************************************

#include <SPI.h>
#include <Mbc83Com.h>
#include <MbcMcp23x17.h>
#include <MbcDigitalOut.h>
#include <MbcDigitalIn.h>

//------ Pinbelegung ------------------------------------------------------------------------------

    const uint8_t PIN_CS_CAN_MBCAN     = 20;
    const uint8_t PIN_CS_BOOT_STORAGE  = 22;
    const uint8_t PIN_CS_MCP23S17_REIN = 2;   // Rueckmeldung
    const uint8_t PIN_ONEWIRE          = 13;
    const uint8_t PIN_LED_GRUEN        = 14;
    const uint8_t PIN_LED_ROT          = 15;
    const uint8_t PIN_ADC_VSPG         = 26;
    const uint8_t PIN_ADC_PLUG         = 27;
    const uint8_t PIN_DIMMER           = 3;

    const uint8_t PIN_REL_1_L = 4,  PIN_REL_1_R = 5;
    const uint8_t PIN_REL_2_L = 6,  PIN_REL_2_R = 7;
    const uint8_t PIN_REL_3_L = 8,  PIN_REL_3_R = 9;
    const uint8_t PIN_REL_4_L = 10, PIN_REL_4_R = 11;

    const uint8_t PIN_IN_1_R = 1,  PIN_IN_1_G = 0;
    const uint8_t PIN_IN_2_R = 17, PIN_IN_2_G = 12;
    const uint8_t PIN_IN_3_R = 23, PIN_IN_3_G = 21;
    const uint8_t PIN_IN_4_R = 25, PIN_IN_4_G = 24;

//------ Modulidentitaet (ASCII-Ziffern) ----------------------------------------------------------

    const uint8_t ART_NR[4] = { '8', '3', '2', '2' };   // [2]=Prozessor: 0=ATmega644P/1=ESP32-C3/2=RP2040-Pico/3=RP2040-Zero
    const uint8_t SW_VER[3] = { '1', '0', '0' };
    const uint8_t HW_VER[6] = { '2', '4', '0', '3', '2', '3' };   // Hardware 24.03.23

//------ Objekte ----------------------------------------------------------------------------------

    MbcMcp23x17 reinExpander(PIN_CS_MCP23S17_REIN);   // 4 Rueckmelde-Eingaenge, GPIOA Bit 0...3

    MbcDigitalOut rel1L(PIN_REL_1_L), rel1R(PIN_REL_1_R);
    MbcDigitalOut rel2L(PIN_REL_2_L), rel2R(PIN_REL_2_R);
    MbcDigitalOut rel3L(PIN_REL_3_L), rel3R(PIN_REL_3_R);
    MbcDigitalOut rel4L(PIN_REL_4_L), rel4R(PIN_REL_4_R);

    MbcDigitalOut *relayOut[8] =
        { &rel1L, &rel1R, &rel2L, &rel2R, &rel3L, &rel3R, &rel4L, &rel4R };

    MbcDigitalIn relIn1(reinExpander, false, 0, true), relIn2(reinExpander, false, 1, true);
    MbcDigitalIn relIn3(reinExpander, false, 2, true), relIn4(reinExpander, false, 3, true);

    MbcDigitalIn *relayIn[4] = { &relIn1, &relIn2, &relIn3, &relIn4 };

    MbcDigitalIn in1R(PIN_IN_1_R), in1G(PIN_IN_1_G), in2R(PIN_IN_2_R), in2G(PIN_IN_2_G);
    MbcDigitalIn in3R(PIN_IN_3_R), in3G(PIN_IN_3_G), in4R(PIN_IN_4_R), in4G(PIN_IN_4_G);

    MbcDigitalIn *switchboard[8] = { &in1R, &in1G, &in2R, &in2G, &in3R, &in3G, &in4R, &in4G };

    // genPurpSize=11, extraEepromSize=10 (siehe Mbc83Com.cpp EEX_83_*-Konstanten)

    static uint8_t sysArray[MBC_SYSARRAY_SIZE(11)];

    MbcUtils       utils(sysArray, 11, PIN_ADC_VSPG, PIN_ADC_PLUG, PIN_LED_GRUEN, PIN_LED_ROT,
                          PIN_ONEWIRE, 10);
    MbcCan         can(PIN_CS_CAN_MBCAN, utils);
    MbcBootStorage bootStorage(PIN_CS_BOOT_STORAGE, MbcBootStorageBackend::FLASH_W25Q64);
    Mbc83Com       com(utils, can, bootStorage, relayOut, relayIn, switchboard, PIN_DIMMER);

//------ Status der CAN-Initialisierung (fuer die wiederkehrende Fehlermeldung in loop()) ---------

    bool canOk = false;

//------ setup()/loop() ---------------------------------------------------------------------------

void setup()
{
    SPI.begin();

    // GPIOA Bit 0...3 Eingang mit PullUp (Rueckmeldung), Rest unbenutzt, GPIOB komplett unbenutzt

    bool reinExpanderOk = reinExpander.begin(0x0f, 0x00, 0xff, 0x00);
    reinExpander.setPullUpA(0x0f);

    utils.begin(ART_NR, SW_VER, HW_VER);

    Serial.begin(115200);

    // USB-CDC braucht nach dem Verbindungsaufbau etwas Zeit, bis der Host das Geraet
    // erkannt hat - sonst gehen die ersten Serial-Ausgaben verloren.

    unsigned long usbStart = millis();
    while ((unsigned long) (millis() - usbStart) < 2000) utils.update();

    Serial.println("\n[Mbc83] Start");

    Serial.print("[Mbc83] MCP23S17 (Rueckmeldung): ");
    Serial.println(reinExpanderOk ? "OK" : "FEHLER - Baustein an GPIO2 nicht gefunden");

    canOk = can.begin();
    Serial.print("[Mbc83] MBCAN-Bus (MCP2515): ");
    Serial.println(canOk ? "OK" : "FEHLER - MCP2515 an GPIO20 nicht gefunden");

    bool bootStorageOk = bootStorage.begin();
    Serial.print("[Mbc83] W25Q64 (Boot-Speicher): ");
    Serial.println(bootStorageOk ? "OK" : "FEHLER - Baustein an GPIO22 nicht gefunden/falsche JEDEC-ID");

    com.begin();
    com.beginModule();

    // Warte, bis alle Statuswerte (Vspg/Plug/Temperatur) einmal ermittelt wurden - blinkt
    // waehrenddessen ca. 10 s durchgehend ORANGE (siehe MbcUtils::waitFilled())

    utils.waitFilled();

    Serial.println("[Mbc83] Bereit");
}

void loop()
{
    utils.update();
    utils.check();

    if (utils.eventSekunde() && !canOk)
    {
        Serial.println("[Mbc83] FEHLER: MBCAN-Bus (MCP2515) weiterhin nicht gefunden!");
    }

    com.update();
}
