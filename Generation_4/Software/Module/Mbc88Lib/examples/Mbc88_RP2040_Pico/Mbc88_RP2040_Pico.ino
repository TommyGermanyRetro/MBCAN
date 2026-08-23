//*************************************************************************************************
//* mbc-88 S88-Rueckmeldedecoder - Raspberry Pi Pico (RP2040), Pinvorschlag                       *
//*************************************************************************************************
//* WICHTIG - OTA-Partitionsschema: Dieser Sketch benoetigt eine reservierte zweite (OTA-)        *
//* Flash-Partition, siehe MbcOta.h. Anders als beim ESP32 gibt es dafuer KEINE partitions.csv -  *
//* in der Arduino-IDE unter Tools/Werkzeuge -> Flash Size eine Variante mit "OTA" im Namen       *
//* waehlen (beim 2-MByte-Standard-Pico sinngemaess "2MB (Sketch: ..., OTA: ...)", genauer        *
//* Wortlaut je nach arduino-pico-Version) - ohne das hat ein OTA-Update keine Zielpartition zum  *
//* Beschreiben und schlaegt fehl.                                                                *
//*                                                                                               *
//* Freie Pinwahl fuer dieses Board - ANNAHME, vor dem Verdrahten gegen das Pico-Pinout pruefen.  *
//* Die 8 Kontakteingaenge/8 Status-LED-Ausgaenge kommen mit genug nativen Pins aus, kein         *
//* Expander noetig.                                                                              *
//*                                                                                               *
//* Der Boot-Speicherchip fuer das Firmware-Update-Staging ist hier ein W25Q64-SPI-NOR-Flash (8   *
//* MByte) statt des bisherigen 24LC1024-EEPROMs (128 KByte) - dessen Kapazitaet fuer groessere   *
//* Firmware-Images zu klein war. MbcBootStorage waehlt das Backend ueber den zweiten             *
//* Konstruktor-Parameter (FLASH_W25Q64), siehe dessen Kopfkommentar fuer Details zur             *
//* Sektor-Loesch-Strategie.                                                                      *
//*                                                                                               *
//* Kein Reset-Pin: RP2040 nutzt den Software-Reset des Cores (MbcPlatform).                      *
//*                                                                                               *
//* | Funktion                      | GPIO                             |                          *
//* |-------------------------------|----------------------------------|                          *
//* | SPI MOSI/MISO/SCK             | Standard-SPI (SPI0: 19/16/18)    |                          *
//* | MBCAN-CS (MCP2515)            | GPIO20                           |                          *
//* | Boot-Speicher-CS (W25Q64)     | GPIO22                           |                          *
//* | OneWire                       | GPIO13                           |                          *
//* | LED gruen                     | GPIO14                           |                          *
//* | LED rot                       | GPIO15                           |                          *
//* | ADC Busspannung               | GPIO26 (ADC0)                    |                          *
//* | ADC Steckernetzteil           | GPIO27 (ADC1)                    |                          *
//* | OUT_1..OUT_8 (Status-LEDs)    | GPIO4, 5, 6, 7, 8, 9, 10, 11     |                          *
//* | IN_1..IN_8 (S88-Kontakte)     | GPIO0, 1, 12, 17, 21, 23, 24, 25 |                          *
//*                                                                                               *
//* GPIO2, GPIO3, GPIO28 bleiben frei fuer zukuenftige Erweiterungen.                             *
//*************************************************************************************************
//* Datei: Mbc88_RP2040_Pico.ino                                                                  *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen:                                                                                  *
//* V1.0.0 - 08.08.2026 - Nullversion                                                             *
//*************************************************************************************************

#include <SPI.h>
#include <Mbc88Com.h>
#include <MbcDigitalOut.h>
#include <MbcDigitalIn.h>

//------ Pinbelegung ------------------------------------------------------------------------------

    const uint8_t PIN_CS_CAN_MBCAN    = 20;
    const uint8_t PIN_CS_BOOT_STORAGE = 22;
    const uint8_t PIN_ONEWIRE         = 13;
    const uint8_t PIN_LED_GRUEN       = 14;
    const uint8_t PIN_LED_ROT         = 15;
    const uint8_t PIN_ADC_VSPG        = 26;
    const uint8_t PIN_ADC_PLUG        = 27;

    const uint8_t PIN_OUT[8] = { 4, 5, 6, 7, 8, 9, 10, 11 };
    const uint8_t PIN_IN[8]  = { 0, 1, 12, 17, 21, 23, 24, 25 };

//------ Modulidentitaet (ASCII-Ziffern) ----------------------------------------------------------

    const uint8_t ART_NR[4] = { '8', '8', '2', '1' };   // [2]=Prozessor: 0=ATmega644P/1=ESP32-C3/2=RP2040-Pico/3=RP2040-Zero
    const uint8_t SW_VER[3] = { '1', '0', '0' };
    const uint8_t HW_VER[6] = { '2', '4', '0', '1', '0', '6' };   // Hardware 24.01.06

//------ Objekte ----------------------------------------------------------------------------------

    // Status-LED-Ausgaenge: aktiv-LOW auf dieser Platine (LOW = LED an), daher activeLow=true

    MbcDigitalIn  in0(PIN_IN[0]),  in1(PIN_IN[1]),  in2(PIN_IN[2]),  in3(PIN_IN[3]);
    MbcDigitalIn  in4(PIN_IN[4]),  in5(PIN_IN[5]),  in6(PIN_IN[6]),  in7(PIN_IN[7]);

    MbcDigitalOut out0(PIN_OUT[0], true), out1(PIN_OUT[1], true);
    MbcDigitalOut out2(PIN_OUT[2], true), out3(PIN_OUT[3], true);
    MbcDigitalOut out4(PIN_OUT[4], true), out5(PIN_OUT[5], true);
    MbcDigitalOut out6(PIN_OUT[6], true), out7(PIN_OUT[7], true);

    MbcDigitalIn  *inputs[8]  = { &in0, &in1, &in2, &in3, &in4, &in5, &in6, &in7 };
    MbcDigitalOut *outputs[8] = { &out0, &out1, &out2, &out3, &out4, &out5, &out6, &out7 };

    // genPurpSize=12, extraEepromSize=11 (siehe Mbc88Com.cpp EEX_*-Konstanten)

    static uint8_t sysArray[MBC_SYSARRAY_SIZE(12)];

    MbcUtils       utils(sysArray, 12, PIN_ADC_VSPG, PIN_ADC_PLUG, PIN_LED_GRUEN, PIN_LED_ROT,
                          PIN_ONEWIRE, 11);
    MbcCan         can(PIN_CS_CAN_MBCAN, utils);
    MbcBootStorage bootStorage(PIN_CS_BOOT_STORAGE, MbcBootStorageBackend::FLASH_W25Q64);
    Mbc88Com       com(utils, can, bootStorage, inputs, outputs);

//------ Status der CAN-Initialisierung (fuer die wiederkehrende Fehlermeldung in loop()) ---------

    bool canOk = false;

//------ setup()/loop() ---------------------------------------------------------------------------

void setup()
{
    // MbcUtils schon vor der ersten Serial-Ausgabe begin()en, damit beim anschliessenden Warten
    // auf die USB-CDC-Verbindung tick-aware (utils.update()) statt mit einem blockierenden
    // delay() gewartet werden kann.

    SPI.begin();

    for (uint8_t index = 0; index < 8; index++) inputs[index]->begin();
    for (uint8_t index = 0; index < 8; index++) outputs[index]->begin();

    utils.begin(ART_NR, SW_VER, HW_VER);

    Serial.begin(115200);

    // USB-CDC braucht nach dem Verbindungsaufbau etwas Zeit, bis der Host das Geraet
    // erkannt hat - sonst gehen die ersten Serial-Ausgaben verloren.

    unsigned long usbStart = millis();
    while ((unsigned long) (millis() - usbStart) < 2000) utils.update();

    Serial.println("\n[Mbc88] Start");

    canOk = can.begin();
    Serial.print("[Mbc88] MBCAN-Bus (MCP2515): ");
    Serial.println(canOk ? "OK" : "FEHLER - MCP2515 an GPIO20 nicht gefunden");

    bool bootStorageOk = bootStorage.begin();
    Serial.print("[Mbc88] W25Q64 (Boot-Speicher): ");
    Serial.println(bootStorageOk ? "OK" : "FEHLER - Baustein an GPIO22 nicht gefunden/falsche JEDEC-ID");

    com.begin();
    com.beginModule();

    // Warte, bis alle Statuswerte (Vspg/Plug/Temperatur) einmal ermittelt wurden - blinkt
    // waehrenddessen ca. 10 s durchgehend ORANGE (siehe MbcUtils::waitFilled())

    utils.waitFilled();

    Serial.println("[Mbc88] Bereit");
}

void loop()
{
    utils.update();
    utils.check();

    if (utils.eventSekunde() && !canOk)
    {
        Serial.println("[Mbc88] FEHLER: MBCAN-Bus (MCP2515) weiterhin nicht gefunden!");
    }

    com.update();
}
