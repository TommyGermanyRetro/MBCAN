//*************************************************************************************************
//* mbc-83 4-fach Relais-/Magnetartikeldecoder mit Dimmer - ESP32-C3                              *
//*************************************************************************************************
//* WICHTIG - OTA-Partitionsschema: Dieser Sketch benoetigt zwei App-Partitionen (ota_0/ota_1),   *
//* siehe MbcOta.h. In der Arduino-IDE unter Tools/Werkzeuge -> Partition Scheme "Custom"         *
//* waehlen, damit die mitgelieferte partitions.csv (liegt im selben Sketch-Ordner) verwendet     *
//* wird - ohne das hat ein OTA-Update keine Zielpartition zum Beschreiben und schlaegt fehl.     *
//*                                                                                               *
//* WICHTIG - richtige Datei fuer OTA: Im ParaCenter nur die reine <Sketchname>.ino.bin           *
//* verwenden, NICHT .merged.bin/.bootloader.bin/*_flashed.bin (das sind Boardpaket-Ausgaben      *
//* fuer den anfaenglichen USB-Flash bzw. veraltete Zwischenstaende) - nur die reine App-Datei    *
//* passt zum OTA-Mechanismus (MbcOta.h).                                                         *
//*                                                                                               *
//* WICHTIG - USB-Flash nach einem OTA-Test: Arduino/esptool schreiben beim USB-Upload IMMER an   *
//* die feste Adresse von ota_0 (0x10000) und aendern dabei die otadata-Partition (Boot-Auswahl)  *
//* NICHT. Zeigt otadata nach einem vorherigen OTA-Update auf ota_1, laeuft nach einem erneuten   *
//* USB-Flash weiterhin die ALTE, zuletzt per OTA in ota_1 geschriebene Firmware - der neue       *
//* USB-Stand in ota_0 wird schlicht nicht gebootet. Abhilfe: vor dem naechsten USB-Flash unter   *
//* Tools/Werkzeuge "Erase All Flash Before Sketch Upload" aktivieren (oder gezielt nur otadata   *
//* loeschen: esptool.py erase_region 0xE000 0x2000), damit der Bootloader auf ota_0 zurueckfaellt*
//*                                                                                               *
//* LED gruen/rot, Steckernetzteil-Erkennung UND die 4 Rueckmelde-Eingaenge haengen an einem      *
//* gemeinsamen MCP23S17 (GPIOA Bit 0-2 LEDs/Plug, Bit 3-6 Rueckmeldung Relais 1...4, GPIOB       *
//* unbenutzt - mbc-83 hat keine Positionsanzeige). Ein zweiter MCP23S17 bedient die 8            *
//* Relaisausgaenge + 8 Stellpult-Eingaenge (auf diesem pinknappen Board nicht nativ              *
//* unterzubringen). mbc-83 hat nur einen CAN-Kanal, hier ueber die interne TWAI (externer CAN-   *
//* Transceiver noetig, z.B. SN65HVD230) statt eines externen MCP2515.                            *
//*                                                                                               *
//* WICHTIG: Tools/Werkzeuge -> "USB CDC On Boot" auf "Enabled" stellen, sonst bleiben alle       *
//* Serial-Ausgaben unsichtbar. SPI wird bewusst mit EXPLIZITEN Pins gestartet                    *
//* (SPI.begin(sck,miso,mosi,-1)) statt Boardstandard-Pins, da deren uebliche Default-Belegung    *
//* (oft GPIO4/5/6/7) sich sonst mit hier genutzten GPIOs ueberschneiden wuerde.                  *
//*                                                                                               *
//* Board: "ESP32C3 Dev Module" (Espressif-esp32-Boardpaket). ANNAHME: generisches                *
//* ESP32-C3-DevKit-Pinout - vor dem Verdrahten gegen das jeweilige Board pruefen (GPIO9 =        *
//* BOOT-Taste, daher nicht belegt; GPIO2 ist Boot-Strapping-Pin, hier trotzdem fuer TWAI-TX      *
//* genutzt - Transceiver darf beim Reset keinen falschen Pegel ziehen). GPIO21 bleibt frei fuer  *
//* Erweiterungen.                                                                                *
//*                                                                                               *
//* Kein Reset-Pin: ESP32 nutzt den Software-Reset des Cores (MbcPlatform).                       *
//*                                                                                               *
//* Boot-Speicherchip: W25Q64-SPI-NOR-Flash (8 MByte), siehe MbcBootStorage-Kopfkommentar.        *
//*                                                                                               *
//* | Funktion                                    | GPIO   |                                      *
//* |-------------------------------------------------|--------|                                  *
//* | SPI SCK / MISO / MOSI                           | GPIO4 / GPIO5 / GPIO6 |                   *
//* | MBCAN TWAI-TX                                   | GPIO2  |                                  *
//* | MBCAN TWAI-RX                                   | GPIO3  |                                  *
//* | Boot-Speicher-CS (W25Q64)                       | GPIO8  |                                  *
//* | MCP23S17-CS (LEDs/Plug/Rueckmeldung)            | GPIO1  |                                  *
//* | MCP23S17-CS (Relais+Stellpult)                  | GPIO20 |                                  *
//* | OneWire                                         | GPIO10 |                                  *
//* | ADC Busspannung                                 | GPIO0  |                                  *
//* | Dimmer (PWM)                                    | GPIO7  |                                  *
//* | LED gruen (MCP23S17 GPA0)                       | -      |                                  *
//* | LED rot (MCP23S17 GPA1)                         | -      |                                  *
//* | Steckernetzteil (MCP23S17 GPA2)                 | -      |                                  *
//* | Rueckmeldung Relais 1..4 (MCP23S17 GPA3...6)    | -      |                                  *
//*************************************************************************************************
//* Datei: Mbc83_ESP32C3.ino                                                                      *
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

    const uint8_t PIN_SPI_SCK         = 4;
    const uint8_t PIN_SPI_MISO        = 5;
    const uint8_t PIN_SPI_MOSI        = 6;
    const uint8_t PIN_TWAI_TX         = 2;   // fest, einheitlich ueber alle ESP32-C3-Sketches
    const uint8_t PIN_TWAI_RX         = 3;   // fest, einheitlich ueber alle ESP32-C3-Sketches
    const uint8_t PIN_CS_BOOT_STORAGE = 8;
    const uint8_t PIN_CS_MCP23S17_LED = 1;   // LEDs/Plug-Erkennung/Rueckmeldung
    const uint8_t PIN_CS_MCP23S17_REL = 20;  // Relais+Stellpult
    const uint8_t PIN_ONEWIRE         = 10;
    const uint8_t PIN_ADC_VSPG        = 0;
    const uint8_t PIN_DIMMER          = 7;

//------ Modulidentitaet (ASCII-Ziffern) ----------------------------------------------------------

    const uint8_t ART_NR[4] = { '8', '3', '1', '2' };   // [2]=Prozessor: 0=ATmega644P/1=ESP32-C3/2=RP2040-Pico/3=RP2040-Zero
    const uint8_t SW_VER[3] = { '1', '0', '0' };
    const uint8_t HW_VER[6] = { '2', '4', '0', '3', '2', '3' };   // Hardware 24.03.23

//------ Objekte ----------------------------------------------------------------------------------

    MbcMcp23x17 ledExpander(PIN_CS_MCP23S17_LED);   // auch Rueckmeldung, siehe Kopfkommentar
    MbcMcp23x17 ioExpander(PIN_CS_MCP23S17_REL);

    // GPIOA-Bits des LED-Expanders: 0=LED gruen, 1=LED rot (beide HIGH-aktiv), 2=
    // Steckernetzteil-Erkennung (Eingang, ANNAHME HIGH-aktiv), 3-6=Rueckmeldung Relais 1...4
    // (Eingang, aktiv-LOW mit PullUp), GPIOB unbenutzt

    MbcDigitalOut ledGruenOut(ledExpander, false, 0, false);
    MbcDigitalOut ledRotOut(ledExpander, false, 1, false);
    MbcDigitalIn  plugDetectorIn(ledExpander, false, 2, false);

    MbcDigitalIn relIn1(ledExpander, false, 3, true), relIn2(ledExpander, false, 4, true);
    MbcDigitalIn relIn3(ledExpander, false, 5, true), relIn4(ledExpander, false, 6, true);

    MbcDigitalIn *relayIn[4] = { &relIn1, &relIn2, &relIn3, &relIn4 };

    // Relais/Stellpult: GPIOA = 8 Ausgaenge, GPIOB = 8 Eingaenge des ioExpander

    MbcDigitalOut rel1L(ioExpander, false, 0, false), rel1R(ioExpander, false, 1, false);
    MbcDigitalOut rel2L(ioExpander, false, 2, false), rel2R(ioExpander, false, 3, false);
    MbcDigitalOut rel3L(ioExpander, false, 4, false), rel3R(ioExpander, false, 5, false);
    MbcDigitalOut rel4L(ioExpander, false, 6, false), rel4R(ioExpander, false, 7, false);

    MbcDigitalOut *relayOut[8] =
        { &rel1L, &rel1R, &rel2L, &rel2R, &rel3L, &rel3R, &rel4L, &rel4R };

    MbcDigitalIn in1R(ioExpander, true, 0, true), in1G(ioExpander, true, 1, true);
    MbcDigitalIn in2R(ioExpander, true, 2, true), in2G(ioExpander, true, 3, true);
    MbcDigitalIn in3R(ioExpander, true, 4, true), in3G(ioExpander, true, 5, true);
    MbcDigitalIn in4R(ioExpander, true, 6, true), in4G(ioExpander, true, 7, true);

    MbcDigitalIn *switchboard[8] = { &in1R, &in1G, &in2R, &in2G, &in3R, &in3G, &in4R, &in4G };

    // genPurpSize=11, extraEepromSize=10 (siehe Mbc83Com.cpp EEX_83_*-Konstanten)

    static uint8_t sysArray[MBC_SYSARRAY_SIZE(11)];

    MbcUtils       utils(sysArray, 11, PIN_ADC_VSPG, 0, 0, 0, PIN_ONEWIRE, 10);   // Plug-/LED-
                                                                              // Pins ungenutzt,
                                                                              // siehe
                                                                              // setPlugDetector()/
                                                                              // setLedDrivers()
    MbcCan         can(PIN_TWAI_TX, PIN_TWAI_RX, utils);
    MbcBootStorage bootStorage(PIN_CS_BOOT_STORAGE, MbcBootStorageBackend::FLASH_W25Q64);
    Mbc83Com       com(utils, can, bootStorage, relayOut, relayIn, switchboard, PIN_DIMMER);

//------ Status der CAN-Initialisierung (fuer die wiederkehrende Fehlermeldung in loop()) ---------

    bool canOk = false;

//------ setup()/loop() ---------------------------------------------------------------------------

void setup()
{
    SPI.begin(PIN_SPI_SCK, PIN_SPI_MISO, PIN_SPI_MOSI, -1);

    // GPIOA: Bit 0/1 Ausgang (LEDs), Bit 2 Eingang (Steckernetzteil-Erkennung), Bit 3-6
    // Eingang mit PullUp (Rueckmeldung Relais 1...4), Bit 7 unbenutzt. GPIOB unbenutzt.

    bool ledExpanderOk = ledExpander.begin(0xfc, 0x00, 0xff, 0x00);
    ledExpander.setPullUpA(0x78);   // Bit 3...6

    // Relais/Stellpult: GPIOA Ausgang (8 Relais), GPIOB Eingang mit PullUp (8 Taster)

    bool ioExpanderOk = ioExpander.begin(0x00, 0x00, 0xff, 0xff);
    ioExpander.setPullUpB(0xff);   // GPIOB (Stellpult-Taster): interne Pull-Up aktivieren

    utils.setLedDrivers(&ledGruenOut, &ledRotOut);
    utils.setPlugDetector(&plugDetectorIn);

    utils.begin(ART_NR, SW_VER, HW_VER);

    Serial.begin(115200);

    unsigned long usbStart = millis();
    while ((unsigned long) (millis() - usbStart) < 2000) utils.update();

    Serial.println("\n[Mbc83] Start");

    Serial.print("[Mbc83] MCP23S17 (LEDs/Plug/Rueckmeldung): ");
    Serial.println(ledExpanderOk ? "OK" : "FEHLER - Baustein an GPIO1 nicht gefunden");

    Serial.print("[Mbc83] MCP23S17 (Relais/Stellpult): ");
    Serial.println(ioExpanderOk ? "OK" : "FEHLER - Baustein an GPIO20 nicht gefunden");

    canOk = can.begin();
    Serial.print("[Mbc83] MBCAN-Bus (TWAI): ");
    Serial.println(canOk ? "OK" : "FEHLER - Transceiver/Verdrahtung an GPIO2/GPIO3 pruefen");

    bool bootStorageOk = bootStorage.begin();
    Serial.print("[Mbc83] W25Q64 (Boot-Speicher): ");
    Serial.println(bootStorageOk ? "OK" : "FEHLER - Baustein an GPIO8 nicht gefunden/falsche JEDEC-ID");

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
        Serial.println("[Mbc83] FEHLER: MBCAN-Bus (TWAI) weiterhin nicht initialisiert!");
    }

    com.update();
}
