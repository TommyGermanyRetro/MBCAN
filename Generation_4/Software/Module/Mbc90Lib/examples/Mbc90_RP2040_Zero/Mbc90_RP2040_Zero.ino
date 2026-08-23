//*************************************************************************************************
//* mbc-90 S88-Rueckmeldedecoder, 16 Kontakte - Waveshare RP2040-Zero                             *
//*************************************************************************************************
//* WICHTIG - OTA-Partitionsschema: Dieser Sketch benoetigt eine reservierte zweite (OTA-)        *
//* Flash-Partition, siehe MbcOta.h. Anders als beim ESP32 gibt es dafuer KEINE partitions.csv -  *
//* in der Arduino-IDE unter Tools/Werkzeuge -> Flash Size eine Variante mit "OTA" im Namen       *
//* waehlen (beim 2-MByte-Standard-Flash dieses Boards sinngemaess "2MB (Sketch: ..., OTA: ...)", *
//* genauer Wortlaut je nach arduino-pico-Version) - ohne das hat ein OTA-Update keine            *
//* Zielpartition zum Beschreiben und schlaegt fehl.                                              *
//*                                                                                               *
//* 16 Kontakteingaenge UND Stellungsanzeige haengen an je einem eigenen MCP23S17 (SPI,           *
//* GPIOA=Kontakt 1-8/GPIOB=Kontakt 9-16), beide teilen sich den ohnehin vorhandenen SPI0-Bus -   *
//* nur die 2 CS-Pins kommen dazu, kein I2C mehr im Sketch. LED gruen/rot und die                 *
//* Steckernetzteil-Erkennung sind NATIV verkabelt (kein Expander noetig, da dieser hier nur fuer *
//* Kontakte/Anzeige gebraucht wird).                                                             *
//*                                                                                               *
//* ANNAHME: generischer GPIO-Vorschlag fuer die Waveshare RP2040-Zero - vor dem Verdrahten gegen *
//* deren Pinout-Diagramm pruefen (insb. GPIO16, dort sitzt bei diesem Board das                  *
//* Onboard-WS2812-RGB-LED und wird deshalb gemieden). SPI0 wird bewusst mit EXPLIZITEN Pins      *
//* gestartet (SPI.setSCK()/setTX()/setRX()/setCS() vor SPI.begin()), nicht mit den               *
//* Boardstandard-Pins - siehe SPI-Hinweis beim ESP32-Sketch fuer die Art des Risikos bei         *
//* impliziten SPI-Standardpins.                                                                  *
//*                                                                                               *
//* Kein Reset-Pin: RP2040 nutzt den Software-Reset des Cores (MbcPlatform).                      *
//*                                                                                               *
//* Der Boot-Speicherchip fuer das Firmware-Update-Staging ist hier ein W25Q64-SPI-NOR-Flash (8   *
//* MByte). MbcBootStorage waehlt das Backend ueber den zweiten Konstruktor-Parameter             *
//* (FLASH_W25Q64), siehe dessen Kopfkommentar fuer Details zur Sektor-Loesch-Strategie.          *
//*                                                                                               *
//* | Funktion                                        | GPIO                  |                   *
//* |-------------------------------------------------|-----------------------|                   *
//* | SPI0 SCK / MOSI / MISO                          | GPIO2 / GPIO3 / GPIO4 |                   *
//* | SPI0 CS (Standard, ungenutzt)                   | GPIO5                 |                   *
//* | MCP23S17-CS (16 Kontakteingaenge)               | GPIO6                 |                   *
//* | MBCAN-CS (MCP2515)                              | GPIO10                |                   *
//* | Boot-Speicher-CS (W25Q64)                       | GPIO12                |                   *
//* | MCP23S17-CS (Stellungsanzeige)                  | GPIO8                 |                   *
//* | OneWire                                         | GPIO9                 |                   *
//* | ADC Busspannung                                 | GPIO26 (ADC0)         |                   *
//* | LED gruen                                       | GPIO0                 |                   *
//* | LED rot                                         | GPIO1                 |                   *
//* | ADC Steckernetzteil                             | GPIO27 (ADC1)         |                   *
//*                                                                                               *
//* Frei fuer kuenftige Erweiterungen: GPIO7, GPIO11, GPIO13, GPIO14, GPIO15, GPIO28, GPIO29.     *
//*************************************************************************************************
//* Datei: Mbc90_RP2040_Zero.ino                                                                  *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen:                                                                                  *
//* V1.0.0 - 14.08.2026 - Nullversion                                                             *
//*************************************************************************************************

#include <SPI.h>
#include <Mbc90Com.h>
#include <MbcMcp23x17.h>
#include <MbcDigitalIn.h>

//------ Pinbelegung ------------------------------------------------------------------------------

    const uint8_t PIN_SPI_SCK         = 2;
    const uint8_t PIN_SPI_MOSI        = 3;
    const uint8_t PIN_SPI_MISO        = 4;
    const uint8_t PIN_SPI_CS          = 5;   // SPI0-Standard-CS, ungenutzt
    const uint8_t PIN_CS_MCP23S17_IN  = 6;   // 16 Kontakteingaenge
    const uint8_t PIN_CS_CAN_MBCAN    = 10;
    const uint8_t PIN_CS_BOOT_STORAGE = 12;
    const uint8_t PIN_CS_MCP23S17_ANZ = 8;   // Stellungsanzeige
    const uint8_t PIN_ONEWIRE         = 9;
    const uint8_t PIN_ADC_VSPG        = 26;
    const uint8_t PIN_LED_GRUEN       = 0;
    const uint8_t PIN_LED_ROT         = 1;
    const uint8_t PIN_ADC_PLUG        = 27;

//------ Modulidentitaet (ASCII-Ziffern) ----------------------------------------------------------

    const uint8_t ART_NR[4] = { '9', '0', '3', '2' };   // [2]=Prozessor: 0=ATmega644P/1=ESP32-C3/2=RP2040-Pico/3=RP2040-Zero
    const uint8_t SW_VER[3] = { '1', '0', '0' };
    const uint8_t HW_VER[6] = { '2', '3', '1', '1', '2', '5' };   // Hardware 23.11.25

//------ Objekte ----------------------------------------------------------------------------------

    MbcMcp23x17 ioExpander(PIN_CS_MCP23S17_IN);
    MbcMcp23x17 display(PIN_CS_MCP23S17_ANZ);

    // 16 Kontakteingaenge: GPIOA = Kontakt 1-8, GPIOB = Kontakt 9-16 (aktiv-LOW mit PullUp) des
    // ioExpander - Index-Konvention siehe Mbc90Com-Klassenkommentar

    MbcDigitalIn in00(ioExpander, false, 0, true), in01(ioExpander, false, 1, true);
    MbcDigitalIn in02(ioExpander, false, 2, true), in03(ioExpander, false, 3, true);
    MbcDigitalIn in04(ioExpander, false, 4, true), in05(ioExpander, false, 5, true);
    MbcDigitalIn in06(ioExpander, false, 6, true), in07(ioExpander, false, 7, true);
    MbcDigitalIn in08(ioExpander, true,  0, true), in09(ioExpander, true,  1, true);
    MbcDigitalIn in10(ioExpander, true,  2, true), in11(ioExpander, true,  3, true);
    MbcDigitalIn in12(ioExpander, true,  4, true), in13(ioExpander, true,  5, true);
    MbcDigitalIn in14(ioExpander, true,  6, true), in15(ioExpander, true,  7, true);

    MbcDigitalIn *inputs[16] = { &in00, &in01, &in02, &in03, &in04, &in05, &in06, &in07,
                                  &in08, &in09, &in10, &in11, &in12, &in13, &in14, &in15 };

    // genPurpSize=20, extraEepromSize=18 (siehe Mbc90Com.cpp EEX_*-Konstanten). LED/Plug nativ,
    // keine setLedDrivers()/setPlugDetector()-Umleitung noetig

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
    SPI.setSCK(PIN_SPI_SCK);
    SPI.setTX(PIN_SPI_MOSI);
    SPI.setRX(PIN_SPI_MISO);
    SPI.setCS(PIN_SPI_CS);
    SPI.begin();

    // 16 Kontakteingaenge: GPIOA/GPIOB komplett auf Eingang mit internem Pull-Up

    bool ioExpanderOk = ioExpander.begin(0xff, 0x00, 0xff, 0x00);
    ioExpander.setPullUpA(0xff);
    ioExpander.setPullUpB(0xff);

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

    Serial.print("[Mbc90] MCP23S17 (Kontakte): ");
    Serial.println(ioExpanderOk ? "OK" : "FEHLER - Baustein an GPIO6 nicht gefunden");

    Serial.print("[Mbc90] MCP23S17 (Stellungsanzeige): ");
    Serial.println(displayOk ? "OK" : "FEHLER - Baustein an GPIO8 nicht gefunden");

    canOk = can.begin();
    Serial.print("[Mbc90] MBCAN-Bus (MCP2515): ");
    Serial.println(canOk ? "OK" : "FEHLER - MCP2515 an GPIO10 nicht gefunden");

    bool bootStorageOk = bootStorage.begin();
    Serial.print("[Mbc90] W25Q64 (Boot-Speicher): ");
    Serial.println(bootStorageOk ? "OK" : "FEHLER - Baustein an GPIO12 nicht gefunden/falsche JEDEC-ID");

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
