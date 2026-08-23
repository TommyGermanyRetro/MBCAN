//*************************************************************************************************
//* mbc-90 S88-Rueckmeldedecoder, 16 Kontakte - Raspberry Pi Pico (RP2040), Pinvorschlag          *
//*************************************************************************************************
//* WICHTIG - OTA-Partitionsschema: Dieser Sketch benoetigt eine reservierte zweite (OTA-)        *
//* Flash-Partition, siehe MbcOta.h. Anders als beim ESP32 gibt es dafuer KEINE partitions.csv -  *
//* in der Arduino-IDE unter Tools/Werkzeuge -> Flash Size eine Variante mit "OTA" im Namen       *
//* waehlen (beim 2-MByte-Standard-Pico sinngemaess "2MB (Sketch: ..., OTA: ...)", genauer        *
//* Wortlaut je nach arduino-pico-Version) - ohne das hat ein OTA-Update keine Zielpartition zum  *
//* Beschreiben und schlaegt fehl.                                                                *
//*                                                                                               *
//* Freie Pinwahl - ANNAHME, vor dem Verdrahten gegen das Pico-Pinout pruefen. mbc-90 hat KEINE   *
//* direkten LED-Ausgangspins - die                                                               *
//* Stellungsanzeige haengt an einem MCP23S17 (SPI-Portexpander, GPIOA=Kontakt 1-8/GPIOB=Kontakt  *
//* 9-16), der sich den ohnehin vorhandenen SPI0-Bus mit MBCAN-CS/Boot-Speicher-CS teilt (nur ein *
//* zusaetzlicher CS-Pin noetig) - daher bleiben genug native GPIO fuer alle 16 Kontakteingaenge  *
//* frei, kein weiterer Expander noetig.                                                          *
//*                                                                                               *
//* Der Boot-Speicherchip fuer das Firmware-Update-Staging ist hier ein W25Q64-SPI-NOR-Flash (8   *
//* MByte) statt des bisherigen 24LC1024-EEPROMs (128 KByte) - dessen Kapazitaet fuer groessere   *
//* Firmware-Images zu klein war. MbcBootStorage waehlt das Backend ueber den zweiten             *
//* Konstruktor-Parameter (FLASH_W25Q64), siehe dessen Kopfkommentar fuer Details zur             *
//* Sektor-Loesch-Strategie.                                                                      *
//*                                                                                               *
//* Kein Reset-Pin: RP2040 nutzt den Software-Reset des Cores (MbcPlatform).                      *
//*                                                                                               *
//* | Funktion                      | GPIO                                          |             *
//* |-------------------------------|-----------------------------------------------|             *
//* | SPI MOSI/MISO/SCK             | Standard-SPI (SPI0: 19/16/18)                 |             *
//* | MBCAN-CS (MCP2515)            | GPIO20                                        |             *
//* | Boot-Speicher-CS (W25Q64)     | GPIO22                                        |             *
//* | MCP23S17-CS (Stellungsanzeige)| GPIO4                                         |             *
//* | OneWire                       | GPIO13                                        |             *
//* | LED gruen                     | GPIO14                                        |             *
//* | LED rot                       | GPIO15                                        |             *
//* | ADC Busspannung               | GPIO26 (ADC0)                                 |             *
//* | ADC Steckernetzteil           | GPIO27 (ADC1)                                 |             *
//* | IN00...IN07 (S88-Kontakte)    | GPIO0, 1, 2, 3, 6, 7, 8, 9                    |             *
//* | IN08...IN15 (S88-Kontakte)    | GPIO10, 11, 12, 17, 21, 23, 24, 25            |             *
//*                                                                                               *
//* GPIO5, GPIO28 bleiben frei fuer zukuenftige Erweiterungen.                                    *
//*************************************************************************************************
//* Datei: Mbc90_RP2040_Pico.ino                                                                  *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen:                                                                                  *
//* V1.0.0 - 14.08.2026 - Nullversion                                                             *
//*************************************************************************************************

#include <SPI.h>
#include <Mbc90Com.h>
#include <MbcDigitalIn.h>
#include <MbcMcp23x17.h>

//------ Pinbelegung ------------------------------------------------------------------------------

    const uint8_t PIN_CS_CAN_MBCAN    = 20;
    const uint8_t PIN_CS_BOOT_STORAGE = 22;
    const uint8_t PIN_CS_MCP23S17_ANZ = 4;   // Stellungsanzeige
    const uint8_t PIN_ONEWIRE         = 13;
    const uint8_t PIN_LED_GRUEN       = 14;
    const uint8_t PIN_LED_ROT         = 15;
    const uint8_t PIN_ADC_VSPG        = 26;
    const uint8_t PIN_ADC_PLUG        = 27;

    const uint8_t PIN_IN[16] = { 0, 1, 2, 3, 6, 7, 8, 9, 10, 11, 12, 17, 21, 23, 24, 25 };

//------ Modulidentitaet (ASCII-Ziffern) ----------------------------------------------------------

    const uint8_t ART_NR[4] = { '9', '0', '2', '2' };   // [2]=Prozessor: 0=ATmega644P/1=ESP32-C3/2=RP2040-Pico/3=RP2040-Zero
    const uint8_t SW_VER[3] = { '1', '0', '0' };
    const uint8_t HW_VER[6] = { '2', '3', '1', '1', '2', '5' };   // Hardware 23.11.25

//------ Objekte ----------------------------------------------------------------------------------

    MbcDigitalIn in00(PIN_IN[0]),  in01(PIN_IN[1]),  in02(PIN_IN[2]),  in03(PIN_IN[3]);
    MbcDigitalIn in04(PIN_IN[4]),  in05(PIN_IN[5]),  in06(PIN_IN[6]),  in07(PIN_IN[7]);
    MbcDigitalIn in08(PIN_IN[8]),  in09(PIN_IN[9]),  in10(PIN_IN[10]), in11(PIN_IN[11]);
    MbcDigitalIn in12(PIN_IN[12]), in13(PIN_IN[13]), in14(PIN_IN[14]), in15(PIN_IN[15]);

    MbcDigitalIn *inputs[16] = { &in00, &in01, &in02, &in03, &in04, &in05, &in06, &in07,
                                  &in08, &in09, &in10, &in11, &in12, &in13, &in14, &in15 };

    // Stellungsanzeige: ein MCP23S17 deckt beide Byte ab (GPIOA=Kontakt 1-8, GPIOB=Kontakt 9-16)

    MbcMcp23x17 display(PIN_CS_MCP23S17_ANZ);

    // genPurpSize=20, extraEepromSize=18 (siehe Mbc90Com.cpp EEX_*-Konstanten)

    static uint8_t sysArray[MBC_SYSARRAY_SIZE(20)];

    MbcUtils       utils(sysArray, 20, PIN_ADC_VSPG, PIN_ADC_PLUG, PIN_LED_GRUEN, PIN_LED_ROT,
                          PIN_ONEWIRE, 18);
    MbcCan         can(PIN_CS_CAN_MBCAN, utils);
    MbcBootStorage bootStorage(PIN_CS_BOOT_STORAGE, MbcBootStorageBackend::FLASH_W25Q64);
    Mbc90Com       com(utils, can, bootStorage, inputs, display);

//------ Status der CAN-Initialisierung (fuer die wiederkehrende Fehlermeldung in loop()) ---------

    bool canOk = false;

//------ setup()/loop() ---------------------------------------------------------------------------

void setup()
{
    // MbcUtils schon vor der ersten Serial-Ausgabe begin()en, damit beim anschliessenden Warten
    // auf die USB-CDC-Verbindung tick-aware (utils.update()) statt mit einem blockierenden
    // delay() gewartet werden kann.

    SPI.begin();

    // Stellungsanzeige: GPIOA/GPIOB komplett auf Ausgang, initial "alles aus" (aktiv-LOW, 0xff)

    bool displayOk = display.begin(0x00, 0xff, 0x00, 0xff);

    for (uint8_t index = 0; index < 16; index++) inputs[index]->begin();

    utils.begin(ART_NR, SW_VER, HW_VER);

    Serial.begin(115200);

    // USB-CDC braucht nach dem Verbindungsaufbau etwas Zeit, bis der Host das Geraet
    // erkannt hat - sonst gehen die ersten Serial-Ausgaben verloren.

    unsigned long usbStart = millis();
    while ((unsigned long) (millis() - usbStart) < 2000) utils.update();

    Serial.println("\n[Mbc90] Start");

    Serial.print("[Mbc90] MCP23S17 (Stellungsanzeige): ");
    Serial.println(displayOk ? "OK" : "FEHLER - Baustein an GPIO4 nicht gefunden");

    canOk = can.begin();
    Serial.print("[Mbc90] MBCAN-Bus (MCP2515): ");
    Serial.println(canOk ? "OK" : "FEHLER - MCP2515 an GPIO20 nicht gefunden");

    bool bootStorageOk = bootStorage.begin();
    Serial.print("[Mbc90] W25Q64 (Boot-Speicher): ");
    Serial.println(bootStorageOk ? "OK" : "FEHLER - Baustein an GPIO22 nicht gefunden/falsche JEDEC-ID");

    com.begin();
    com.beginModule();

    // Warte, bis alle Statuswerte (Vspg/Plug/Temperatur) einmal ermittelt wurden - blinkt
    // waehrenddessen ca. 10 s durchgehend ORANGE (siehe MbcUtils::waitFilled())

    utils.waitFilled();

    Serial.println("[Mbc90] Bereit");
}

void loop()
{
    utils.update();
    utils.check();

    if (utils.eventSekunde() && !canOk)
    {
        Serial.println("[Mbc90] FEHLER: MBCAN-Bus (MCP2515) weiterhin nicht gefunden!");
    }

    com.update();
}
