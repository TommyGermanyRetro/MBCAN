//*************************************************************************************************
//* mbc-88 S88-Rueckmeldedecoder - Waveshare RP2040-Zero                                          *
//*************************************************************************************************
//* WICHTIG - OTA-Partitionsschema: Dieser Sketch benoetigt eine reservierte zweite (OTA-)        *
//* Flash-Partition, siehe MbcOta.h. Anders als beim ESP32 gibt es dafuer KEINE partitions.csv -  *
//* in der Arduino-IDE unter Tools/Werkzeuge -> Flash Size eine Variante mit "OTA" im Namen       *
//* waehlen (beim 2-MByte-Standard-Flash dieses Boards sinngemaess "2MB (Sketch: ..., OTA: ...)", *
//* genauer Wortlaut je nach arduino-pico-Version) - ohne das hat ein OTA-Update keine            *
//* Zielpartition zum Beschreiben und schlaegt fehl.                                              *
//*                                                                                               *
//* LED gruen/rot und die Steckernetzteil-Erkennung sind hier NATIV verkabelt: mbc-88 braucht nur *
//* einen einzigen MCP23S17 (fuer die 8 Kontakteingaenge/8 Status-LED-Ausgaenge, s.u.), wodurch   *
//* genug native Pins fuer LED/Plug frei bleiben und der sonst uebliche LED-/Plug-Expander        *
//* entfaellt. Die Steckernetzteil-Erkennung liest damit eine echte Analogspannung per            *
//* analogRead() aus (GPIO26-29 sind die ADC-Kanaele auf dem RP2040), nicht nur ein digitales     *
//* HIGH/LOW wie beim MCP23S17-Ansatz.                                                            *
//*                                                                                               *
//* ANNAHME: generischer GPIO-Vorschlag fuer die Waveshare RP2040-Zero - vor dem Verdrahten gegen *
//* deren Pinout-Diagramm pruefen (insb. GPIO16, dort sitzt bei diesem Board das                  *
//* Onboard-WS2812-RGB-LED und wird deshalb gemieden). SPI0 wird bewusst mit EXPLIZITEN Pins      *
//* gestartet (SPI.setSCK()/setTX()/setRX()/setCS() vor SPI.begin()), nicht mit den               *
//* Boardstandard-Pins - siehe SPI-Hinweis beim ESP32-Sketch fuer die Art des Risikos bei         *
//* impliziten SPI-Standardpins.                                                                  *
//*                                                                                               *
//* Kein Reset-Pin: RP2040 nutzt den Software-Reset des Cores (MbcPlatform). Frei fuer kuenftige  *
//* Erweiterungen: GPIO7, GPIO8, GPIO11, GPIO13, GPIO14, GPIO15, GPIO28, GPIO29.                  *
//*                                                                                               *
//* Der Boot-Speicherchip fuer das Firmware-Update-Staging ist hier ein W25Q64-SPI-NOR-Flash (8   *
//* MByte) statt des bisherigen 24LC1024-EEPROMs (128 KByte) - dessen Kapazitaet fuer groessere   *
//* Firmware-Images zu klein war. MbcBootStorage waehlt das Backend ueber den zweiten             *
//* Konstruktor-Parameter (FLASH_W25Q64), siehe dessen Kopfkommentar fuer Details zur             *
//* Sektor-Loesch-Strategie.                                                                      *
//*                                                                                               *
//* | Funktion                                        | GPIO                  |                   *
//* |-------------------------------------------------|-----------------------|                   *
//* | SPI0 SCK / MOSI / MISO                          | GPIO2 / GPIO3 / GPIO4 |                   *
//* | SPI0 CS (Standard, ungenutzt)                   | GPIO5                 |                   *
//* | MBCAN-CS (MCP2515)                              | GPIO10                |                   *
//* | Boot-Speicher-CS (W25Q64)                       | GPIO12                |                   *
//* | MCP23S17-CS (Kontakte/Status-LEDs)              | GPIO6                 |                   *
//* | OneWire                                         | GPIO9                 |                   *
//* | ADC Busspannung                                 | GPIO26 (ADC0)         |                   *
//* | LED gruen                                       | GPIO0                 |                   *
//* | LED rot                                         | GPIO1                 |                   *
//* | ADC Steckernetzteil                             | GPIO27 (ADC1)         |                   *
//*************************************************************************************************
//* Datei: Mbc88_RP2040_Zero.ino                                                                  *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen:                                                                                  *
//* V1.0.0 - 08.08.2026 - Nullversion                                                             *
//*************************************************************************************************

#include <SPI.h>
#include <Mbc88Com.h>
#include <MbcMcp23x17.h>
#include <MbcDigitalOut.h>
#include <MbcDigitalIn.h>

//------ Pinbelegung ------------------------------------------------------------------------------

    const uint8_t PIN_SPI_SCK        = 2;
    const uint8_t PIN_SPI_MOSI       = 3;
    const uint8_t PIN_SPI_MISO       = 4;
    const uint8_t PIN_SPI_CS         = 5;   // SPI0-Standard-CS, ungenutzt
    const uint8_t PIN_CS_CAN_MBCAN    = 10;
    const uint8_t PIN_CS_BOOT_STORAGE = 12;
    const uint8_t PIN_CS_MCP23S17_IO  = 6;   // Kontakte/Status-LEDs
    const uint8_t PIN_ONEWIRE         = 9;
    const uint8_t PIN_ADC_VSPG        = 26;
    const uint8_t PIN_LED_GRUEN       = 0;
    const uint8_t PIN_LED_ROT         = 1;
    const uint8_t PIN_ADC_PLUG        = 27;

//------ Modulidentitaet (ASCII-Ziffern) ----------------------------------------------------------

    const uint8_t ART_NR[4] = { '8', '8', '3', '1' };   // [2]=Prozessor: 0=ATmega644P/1=ESP32-C3/2=RP2040-Pico/3=RP2040-Zero
    const uint8_t SW_VER[3] = { '1', '0', '0' };
    const uint8_t HW_VER[6] = { '2', '4', '0', '1', '0', '6' };   // Hardware 24.01.06

//------ Objekte ----------------------------------------------------------------------------------

    MbcMcp23x17 ioExpander(PIN_CS_MCP23S17_IO);

    // Kontakte/Status-LEDs: GPIOA = 8 Ausgaenge, GPIOB = 8 Eingaenge (aktiv-LOW mit PullUp) des
    // ioExpander - Index-Konvention siehe Mbc88Com-Klassenkommentar. Status-LEDs aktiv-LOW
    // (LOW = LED an), daher activeLow=true

    MbcDigitalOut out0(ioExpander, false, 0, true), out1(ioExpander, false, 1, true);
    MbcDigitalOut out2(ioExpander, false, 2, true), out3(ioExpander, false, 3, true);
    MbcDigitalOut out4(ioExpander, false, 4, true), out5(ioExpander, false, 5, true);
    MbcDigitalOut out6(ioExpander, false, 6, true), out7(ioExpander, false, 7, true);

    MbcDigitalIn  in0(ioExpander, true, 0, true), in1(ioExpander, true, 1, true);
    MbcDigitalIn  in2(ioExpander, true, 2, true), in3(ioExpander, true, 3, true);
    MbcDigitalIn  in4(ioExpander, true, 4, true), in5(ioExpander, true, 5, true);
    MbcDigitalIn  in6(ioExpander, true, 6, true), in7(ioExpander, true, 7, true);

    MbcDigitalIn  *inputs[8]  = { &in0, &in1, &in2, &in3, &in4, &in5, &in6, &in7 };
    MbcDigitalOut *outputs[8] = { &out0, &out1, &out2, &out3, &out4, &out5, &out6, &out7 };

    // genPurpSize=12, extraEepromSize=11 (siehe Mbc88Com.cpp EEX_*-Konstanten). LED/Plug nativ,
    // keine setLedDrivers()/setPlugDetector()-Umleitung noetig

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
    SPI.setSCK(PIN_SPI_SCK);
    SPI.setTX(PIN_SPI_MOSI);
    SPI.setRX(PIN_SPI_MISO);
    SPI.setCS(PIN_SPI_CS);
    SPI.begin();

    // Kontakte/Status-LEDs: GPIOA Ausgang (8 Status-LEDs), GPIOB Eingang mit PullUp (8 Kontakte)

    bool ioExpanderOk = ioExpander.begin(0x00, 0x00, 0xff, 0xff);
    ioExpander.setPullUpB(0xff);   // GPIOB (S88-Kontakte): interne Pull-Up aktivieren

    for (uint8_t index = 0; index < 8; index++) inputs[index]->begin();
    for (uint8_t index = 0; index < 8; index++) outputs[index]->begin();

    utils.begin(ART_NR, SW_VER, HW_VER);

    Serial.begin(115200);

    // USB-CDC braucht nach dem Verbindungsaufbau etwas Zeit, bis der Host das Geraet
    // erkannt hat - sonst gehen die ersten Serial-Ausgaben verloren.

    unsigned long usbStart = millis();
    while ((unsigned long) (millis() - usbStart) < 2000) utils.update();

    Serial.println("\n[Mbc88] Start");

    Serial.print("[Mbc88] MCP23S17 (Kontakte/Status-LEDs): ");
    Serial.println(ioExpanderOk ? "OK" : "FEHLER - Baustein an GPIO6 nicht gefunden");

    canOk = can.begin();
    Serial.print("[Mbc88] MBCAN-Bus (MCP2515): ");
    Serial.println(canOk ? "OK" : "FEHLER - MCP2515 an GPIO10 nicht gefunden");

    bool bootStorageOk = bootStorage.begin();
    Serial.print("[Mbc88] W25Q64 (Boot-Speicher): ");
    Serial.println(bootStorageOk ? "OK" : "FEHLER - Baustein an GPIO12 nicht gefunden/falsche JEDEC-ID");

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
