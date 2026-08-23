//*************************************************************************************************
//* mbc-83 4-fach Relais-/Magnetartikeldecoder mit Dimmer - Waveshare RP2040-Zero                 *
//*************************************************************************************************
//* WICHTIG - OTA-Partitionsschema: Dieser Sketch benoetigt eine reservierte zweite (OTA-)        *
//* Flash-Partition, siehe MbcOta.h. Anders als beim ESP32 gibt es dafuer KEINE partitions.csv -  *
//* in der Arduino-IDE unter Tools/Werkzeuge -> Flash Size eine Variante mit "OTA" im Namen       *
//* waehlen (beim 2-MByte-Standard-Flash dieses Boards sinngemaess "2MB (Sketch: ..., OTA: ...)", *
//* genauer Wortlaut je nach arduino-pico-Version) - ohne das hat ein OTA-Update keine            *
//* Zielpartition zum Beschreiben und schlaegt fehl.                                              *
//*                                                                                               *
//* LED gruen/rot, die Steckernetzteil-Erkennung UND die 4 Rueckmelde-Eingaenge haengen an einem  *
//* gemeinsamen MCP23S17 statt an nativen Pins (siehe MbcMcp23x17-Kopfkommentar) - GPIOA Bit 0-2  *
//* fuer LEDs/Plug, GPIOA Bit 3-6 zusaetzlich die 4 Rueckmelde-Eingaenge (mbc-83 hat keine        *
//* Positionsanzeige, GPIOB bleibt daher komplett frei). Ein ZWEITER MCP23S17 bedient die 8       *
//* Relaisausgaenge (GPIOA) + 8 Stellpult-Eingaenge (GPIOB, PullUp intern) - auf diesem           *
//* pinknappen Board nicht mehr nativ unterzubringen.                                             *
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
//* MByte) statt des bisherigen 24LC1024-EEPROMs (128 KByte) - dessen Kapazitaet fuer groessere   *
//* Firmware-Images zu klein war. MbcBootStorage waehlt das Backend ueber den zweiten             *
//* Konstruktor-Parameter (FLASH_W25Q64), siehe dessen Kopfkommentar fuer Details zur             *
//* Sektor-Loesch-Strategie.                                                                      *
//*                                                                                               *
//* | Funktion                                    | GPIO                  |                       *
//* |-------------------------------------------------|-----------------------|                   *
//* | SPI0 SCK / MOSI / MISO                          | GPIO2 / GPIO3 / GPIO4 |                   *
//* | SPI0 CS (Standard, ungenutzt)                   | GPIO5                 |                   *
//* | MBCAN-CS (MCP2515)                              | GPIO10                |                   *
//* | Boot-Speicher-CS (W25Q64)                       | GPIO12                |                   *
//* | MCP23S17-CS (LEDs/Plug/Rueckmeldung)            | GPIO7                 |                   *
//* | MCP23S17-CS (Relais+Stellpult)                  | GPIO6                 |                   *
//* | OneWire                                         | GPIO9                 |                   *
//* | ADC Busspannung                                 | GPIO26 (ADC0)         |                   *
//* | Dimmer (PWM)                                    | GPIO0                 |                   *
//* | LED gruen (MCP23S17 GPA0)                       | -                     |                   *
//* | LED rot (MCP23S17 GPA1)                         | -                     |                   *
//* | Steckernetzteil (MCP23S17 GPA2)                 | -                     |                   *
//* | Rueckmeldung Relais 1..4 (MCP23S17 GPA3...6)    | -                     |                   *
//*************************************************************************************************
//* Datei: Mbc83_RP2040_Zero.ino                                                                  *
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

    const uint8_t PIN_SPI_SCK         = 2;
    const uint8_t PIN_SPI_MOSI        = 3;
    const uint8_t PIN_SPI_MISO        = 4;
    const uint8_t PIN_SPI_CS          = 5;   // SPI0-Standard-CS, ungenutzt
    const uint8_t PIN_CS_CAN_MBCAN    = 10;
    const uint8_t PIN_CS_BOOT_STORAGE = 12;
    const uint8_t PIN_CS_MCP23S17_LED = 7;   // LEDs/Plug-Erkennung/Rueckmeldung
    const uint8_t PIN_CS_MCP23S17_REL = 6;   // Relais+Stellpult
    const uint8_t PIN_ONEWIRE         = 9;
    const uint8_t PIN_ADC_VSPG        = 26;
    const uint8_t PIN_DIMMER          = 0;

//------ Modulidentitaet (ASCII-Ziffern) ----------------------------------------------------------

    const uint8_t ART_NR[4] = { '8', '3', '3', '2' };   // [2]=Prozessor: 0=ATmega644P/1=ESP32-C3/2=RP2040-Pico/3=RP2040-Zero
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
    MbcCan         can(PIN_CS_CAN_MBCAN, utils);
    MbcBootStorage bootStorage(PIN_CS_BOOT_STORAGE, MbcBootStorageBackend::FLASH_W25Q64);
    Mbc83Com       com(utils, can, bootStorage, relayOut, relayIn, switchboard, PIN_DIMMER);

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

    // USB-CDC braucht nach dem Verbindungsaufbau etwas Zeit, bis der Host das Geraet
    // erkannt hat - sonst gehen die ersten Serial-Ausgaben verloren.

    unsigned long usbStart = millis();
    while ((unsigned long) (millis() - usbStart) < 2000) utils.update();

    Serial.println("\n[Mbc83] Start");

    Serial.print("[Mbc83] MCP23S17 (LEDs/Plug/Rueckmeldung): ");
    Serial.println(ledExpanderOk ? "OK" : "FEHLER - Baustein an GPIO7 nicht gefunden");

    Serial.print("[Mbc83] MCP23S17 (Relais/Stellpult): ");
    Serial.println(ioExpanderOk ? "OK" : "FEHLER - Baustein an GPIO6 nicht gefunden");

    canOk = can.begin();
    Serial.print("[Mbc83] MBCAN-Bus (MCP2515): ");
    Serial.println(canOk ? "OK" : "FEHLER - MCP2515 an GPIO10 nicht gefunden");

    bool bootStorageOk = bootStorage.begin();
    Serial.print("[Mbc83] W25Q64 (Boot-Speicher): ");
    Serial.println(bootStorageOk ? "OK" : "FEHLER - Baustein an GPIO12 nicht gefunden/falsche JEDEC-ID");

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
