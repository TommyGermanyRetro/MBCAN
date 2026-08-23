//*************************************************************************************************
//* mbc-88 S88-Rueckmeldedecoder - ESP32-C3                                                       *
//*************************************************************************************************
//* WICHTIG - OTA-Partitionsschema: Dieser Sketch benoetigt zwei App-Partitionen (ota_0/ota_1),   *
//* siehe MbcOta.h. In der Arduino-IDE unter Tools/Werkzeuge -> Partition Scheme "Custom"         *
//* waehlen, damit die mitgelieferte partitions.csv (liegt im selben Sketch-Ordner) verwendet     *
//* wird - ohne das hat ein OTA-Update keine Zielpartition zum Beschreiben und schlaegt fehl.     *
//*                                                                                               *
//* LED gruen/rot und die Steckernetzteil-Erkennung sind NATIV verkabelt: mbc-88 braucht nur      *
//* einen einzigen MCP23S17 (8 Kontakteingaenge/8 Status-LED-Ausgaenge), wodurch genug native     *
//* Pins fuer LED/Plug frei bleiben. Die Steckernetzteil-Erkennung liest eine echte               *
//* Analogspannung per analogRead() aus (GPIO0-4 sind ADC1-Kanaele). mbc-88 hat nur einen         *
//* CAN-Kanal, hier ueber die interne TWAI (externer CAN-Transceiver noetig, z.B. SN65HVD230)     *
//* statt eines externen MCP2515 - TWAI-TX/RX liegen fest auf GPIO2/GPIO3 (einheitlich ueber alle *
//* ESP32-C3-Sketches).                                                                           *
//*                                                                                               *
//* WICHTIG: Tools/Werkzeuge -> "USB CDC On Boot" auf "Enabled" stellen, sonst bleiben alle       *
//* Serial-Ausgaben unsichtbar. SPI wird bewusst mit EXPLIZITEN Pins gestartet                    *
//* (SPI.begin(sck,miso,mosi,-1)) statt Boardstandard-Pins, da deren uebliche Default-Belegung    *
//* (oft GPIO4/5/6/7) sich sonst mit hier genutzten GPIOs ueberschneiden wuerde.                  *
//*                                                                                               *
//* Board: "ESP32C3 Dev Module" (Espressif-esp32-Boardpaket). ANNAHME: generisches                *
//* ESP32-C3-DevKit-Pinout - vor dem Verdrahten gegen das jeweilige Board pruefen (GPIO9 =        *
//* BOOT-Taste, daher nicht belegt; GPIO2 ist Boot-Strapping-Pin, hier trotzdem fuer TWAI-TX      *
//* genutzt - Transceiver darf beim Reset keinen falschen Pegel ziehen). Keine weiteren GPIOs     *
//* frei.                                                                                         *
//*                                                                                               *
//* Kein Reset-Pin: ESP32 nutzt den Software-Reset des Cores (MbcPlatform).                       *
//*                                                                                               *
//* Boot-Speicherchip: W25Q64-SPI-NOR-Flash (8 MByte), siehe MbcBootStorage-Kopfkommentar.        *
//*                                                                                               *
//* | Funktion                                        | GPIO                  |                   *
//* |-------------------------------------------------|-----------------------|                   *
//* | SPI SCK / MISO / MOSI                           | GPIO4 / GPIO5 / GPIO6 |                   *
//* | MBCAN TWAI-TX                                   | GPIO2                 |                   *
//* | MBCAN TWAI-RX                                   | GPIO3                 |                   *
//* | Boot-Speicher-CS (W25Q64)                       | GPIO8                 |                   *
//* | MCP23S17-CS (Kontakte/Status-LEDs)              | GPIO20                |                   *
//* | OneWire                                         | GPIO10                |                   *
//* | ADC Busspannung                                 | GPIO0                 |                   *
//* | ADC Steckernetzteil                             | GPIO1                 |                   *
//* | LED gruen                                       | GPIO7                 |                   *
//* | LED rot                                         | GPIO21                |                   *
//*************************************************************************************************
//* Datei: Mbc88_ESP32C3.ino                                                                      *
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

    const uint8_t PIN_SPI_SCK         = 4;
    const uint8_t PIN_SPI_MISO        = 5;
    const uint8_t PIN_SPI_MOSI        = 6;
    const uint8_t PIN_TWAI_TX         = 2;   // fest, einheitlich ueber alle ESP32-C3-Sketches
    const uint8_t PIN_TWAI_RX         = 3;   // fest, einheitlich ueber alle ESP32-C3-Sketches
    const uint8_t PIN_CS_BOOT_STORAGE = 8;
    const uint8_t PIN_CS_MCP23S17_IO  = 20;   // Kontakte/Status-LEDs
    const uint8_t PIN_ONEWIRE         = 10;
    const uint8_t PIN_ADC_VSPG        = 0;
    const uint8_t PIN_ADC_PLUG        = 1;
    const uint8_t PIN_LED_GRUEN       = 7;
    const uint8_t PIN_LED_ROT         = 21;

//------ Modulidentitaet (ASCII-Ziffern) ----------------------------------------------------------

    const uint8_t ART_NR[4] = { '8', '8', '1', '1' };   // [2]=Prozessor: 0=ATmega644P/1=ESP32-C3/2=RP2040-Pico/3=RP2040-Zero
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
    MbcCan         can(PIN_TWAI_TX, PIN_TWAI_RX, utils);
    MbcBootStorage bootStorage(PIN_CS_BOOT_STORAGE, MbcBootStorageBackend::FLASH_W25Q64);
    Mbc88Com       com(utils, can, bootStorage, inputs, outputs);

//------ Status der CAN-Initialisierung (fuer die wiederkehrende Fehlermeldung in loop()) ---------

    bool canOk = false;

//------ setup()/loop() ---------------------------------------------------------------------------

void setup()
{
    SPI.begin(PIN_SPI_SCK, PIN_SPI_MISO, PIN_SPI_MOSI, -1);

    // Kontakte/Status-LEDs: GPIOA Ausgang (8 Status-LEDs), GPIOB Eingang mit PullUp (8 Kontakte)

    bool ioExpanderOk = ioExpander.begin(0x00, 0x00, 0xff, 0xff);
    ioExpander.setPullUpB(0xff);   // GPIOB (S88-Kontakte): interne Pull-Up aktivieren

    for (uint8_t index = 0; index < 8; index++) inputs[index]->begin();
    for (uint8_t index = 0; index < 8; index++) outputs[index]->begin();

    utils.begin(ART_NR, SW_VER, HW_VER);

    Serial.begin(115200);

    unsigned long usbStart = millis();
    while ((unsigned long) (millis() - usbStart) < 2000) utils.update();

    Serial.println("\n[Mbc88] Start");

    Serial.print("[Mbc88] MCP23S17 (Kontakte/Status-LEDs): ");
    Serial.println(ioExpanderOk ? "OK" : "FEHLER - Baustein an GPIO20 nicht gefunden");

    canOk = can.begin();
    Serial.print("[Mbc88] MBCAN-Bus (TWAI): ");
    Serial.println(canOk ? "OK" : "FEHLER - Transceiver/Verdrahtung an GPIO2/GPIO3 pruefen");

    bool bootStorageOk = bootStorage.begin();
    Serial.print("[Mbc88] W25Q64 (Boot-Speicher): ");
    Serial.println(bootStorageOk ? "OK" : "FEHLER - Baustein an GPIO8 nicht gefunden/falsche JEDEC-ID");

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
        Serial.println("[Mbc88] FEHLER: MBCAN-Bus (TWAI) weiterhin nicht initialisiert!");
    }

    com.update();
}
