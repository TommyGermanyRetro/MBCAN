//*************************************************************************************************
//* mbc-91 Magnetartikel-Schaltdecoder - Waveshare RP2040-Zero                                    *
//*************************************************************************************************
//* WICHTIG - OTA-Partitionsschema: Dieser Sketch benoetigt eine reservierte zweite (OTA-)        *
//* Flash-Partition, siehe MbcOta.h. Anders als beim ESP32 gibt es dafuer KEINE partitions.csv -  *
//* in der Arduino-IDE unter Tools/Werkzeuge -> Flash Size eine Variante mit "OTA" im Namen       *
//* waehlen (beim 2-MByte-Standard-Flash dieses Boards sinngemaess "2MB (Sketch: ..., OTA: ...)", *
//* genauer Wortlaut je nach arduino-pico-Version) - ohne das hat ein OTA-Update keine            *
//* Zielpartition zum Beschreiben und schlaegt fehl.                                              *
//*                                                                                               *
//* LED gruen/rot, die Steckernetzteil-Erkennung UND die Positionsanzeige haengen an einem        *
//* gemeinsamen MCP23S17 statt an nativen Pins (siehe MbcMcp23x17-Kopfkommentar) - GPIOA Bit 0-2  *
//* fuer LEDs/Plug, GPIOA Bit 3-6 zusaetzlich das Bestueckungsnibble der Positionsanzeige, GPIOB  *
//* komplett deren 8 Stellungs-LEDs. Fuer mbc-91 kommt EIN WEITERER MCP23S17 hinzu, da 8          *
//* Relaisausgaenge + 8 Stellpult-Eingaenge auf diesem pinknappen Board nicht mehr nativ          *
//* unterzubringen sind (GPIOA = 8 Relaisausgaenge, GPIOB = 8 Stellpult-Eingaenge, PullUp         *
//* intern).                                                                                      *
//*                                                                                               *
//* ANNAHME: generischer GPIO-Vorschlag fuer die Waveshare RP2040-Zero - vor dem Verdrahten gegen *
//* deren Pinout-Diagramm pruefen (insb. GPIO16, dort sitzt bei diesem Board das                  *
//* Onboard-WS2812-RGB-LED und wird deshalb gemieden). SPI0 wird bewusst mit EXPLIZITEN Pins      *
//* gestartet (SPI.setSCK()/setTX()/setRX()/setCS() vor SPI.begin()), nicht mit den               *
//* Boardstandard-Pins - siehe SPI-Hinweis beim ESP32-Sketch fuer die Art des Risikos bei         *
//* impliziten SPI-Standardpins.                                                                  *
//*                                                                                               *
//* Kein Reset-Pin: RP2040 nutzt den Software-Reset des Cores (MbcPlatform). Frei fuer kuenftige  *
//* Erweiterungen: GPIO0, GPIO1, GPIO8, GPIO11, GPIO13, GPIO14, GPIO15, GPIO27, GPIO28, GPIO29.   *
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
//* | MCP23S17-CS (LEDs/Plug/Positionsanzeige)        | GPIO7                 |                   *
//* | MCP23S17-CS (Relais+Stellpult, neu)             | GPIO6                 |                   *
//* | OneWire                                         | GPIO9                 |                   *
//* | ADC Busspannung                                 | GPIO26 (ADC0)         |                   *
//* | LED gruen (MCP23S17 GPA0)                       | -                     |                   *
//* | LED rot (MCP23S17 GPA1)                         | -                     |                   *
//* | Steckernetzteil (MCP23S17 GPA2)                 | -                     |                   *
//* | Positionsanzeige (MCP23S17 GPA3-6 Bestueckung,  | -                     |                   *
//* | GPIOB 8 Stellungs-LEDs)                         | -                     |                   *
//*************************************************************************************************
//* Datei: Mbc91_RP2040_Zero.ino                                                                  *
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

    const uint8_t PIN_SPI_SCK        = 2;
    const uint8_t PIN_SPI_MOSI       = 3;
    const uint8_t PIN_SPI_MISO       = 4;
    const uint8_t PIN_SPI_CS         = 5;   // SPI0-Standard-CS, ungenutzt
    const uint8_t PIN_CS_CAN_MBCAN    = 10;
    const uint8_t PIN_CS_BOOT_STORAGE = 12;
    const uint8_t PIN_CS_MCP23S17_LED = 7;   // LEDs/Plug-Erkennung/Positionsanzeige
    const uint8_t PIN_CS_MCP23S17_REL = 6;   // Relais+Stellpult (neu)
    const uint8_t PIN_ONEWIRE         = 9;
    const uint8_t PIN_ADC_VSPG        = 26;

//------ Modulidentitaet (ASCII-Ziffern) ----------------------------------------------------------

    const uint8_t ART_NR[4] = { '9', '1', '3', '1' };   // [2]=Prozessor: 0=ATmega644P/1=ESP32-C3/2=RP2040-Pico/3=RP2040-Zero
    const uint8_t SW_VER[3] = { '1', '0', '0' };
    const uint8_t HW_VER[6] = { '2', '4', '0', '4', '0', '1' };   // Hardware 24.04.01

//------ Objekte ----------------------------------------------------------------------------------

    MbcMcp23x17 ledExpander(PIN_CS_MCP23S17_LED);   // auch Positionsanzeige, siehe Kopfkommentar
    MbcMcp23x17 ioExpander(PIN_CS_MCP23S17_REL);

    // GPIOA-Bits des LED-Expanders: 0=LED gruen, 1=LED rot (beide HIGH-aktiv), 2=
    // Steckernetzteil-Erkennung (Eingang, ANNAHME HIGH-aktiv), 3-6=Bestueckungsnibble der
    // Positionsanzeige (Eingang), GPIOB traegt zusaetzlich deren 8 Stellungs-LEDs (siehe
    // Mbc91Com::stateDisplay())

    MbcDigitalOut ledGruenOut(ledExpander, false, 0, false);
    MbcDigitalOut ledRotOut(ledExpander, false, 1, false);
    MbcDigitalIn  plugDetectorIn(ledExpander, false, 2, false);

    // Relais/Stellpult: GPIOA = 8 Ausgaenge, GPIOB = 8 Eingaenge des ioExpander

    MbcDigitalOut relL1(ioExpander, false, 0, false), relR1(ioExpander, false, 1, false);
    MbcDigitalOut relL2(ioExpander, false, 2, false), relR2(ioExpander, false, 3, false);
    MbcDigitalOut relL3(ioExpander, false, 4, false), relR3(ioExpander, false, 5, false);
    MbcDigitalOut relL4(ioExpander, false, 6, false), relR4(ioExpander, false, 7, false);

    MbcDigitalIn inG1(ioExpander, true, 0, true), inR1(ioExpander, true, 1, true);
    MbcDigitalIn inG2(ioExpander, true, 2, true), inR2(ioExpander, true, 3, true);
    MbcDigitalIn inG3(ioExpander, true, 4, true), inR3(ioExpander, true, 5, true);
    MbcDigitalIn inG4(ioExpander, true, 6, true), inR4(ioExpander, true, 7, true);

    // Index-Konvention siehe Mbc91Com-Klassenkommentar (0..7 = _1_L,_1_R,_2_L,_2_R,...)

    MbcDigitalOut *relais[8]    = { &relL1, &relR1, &relL2, &relR2,
                                     &relL3, &relR3, &relL4, &relR4 };
    MbcDigitalIn  *stellpult[8] = { &inG1, &inR1, &inG2, &inR2, &inG3, &inR3, &inG4, &inR4 };

    static uint8_t sysArray[MBC_SYSARRAY_SIZE(10)];

    MbcUtils       utils(sysArray, 10, PIN_ADC_VSPG, 0, 0, 0, PIN_ONEWIRE, 9);   // Plug-/LED-
                                                                              // Pins ungenutzt,
                                                                              // siehe
                                                                              // setPlugDetector()/
                                                                              // setLedDrivers()
    MbcCan         can(PIN_CS_CAN_MBCAN, utils);
    MbcBootStorage bootStorage(PIN_CS_BOOT_STORAGE, MbcBootStorageBackend::FLASH_W25Q64);
    Mbc91Com       com(utils, can, bootStorage, ledExpander, relais, stellpult);

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
    // Eingang (Bestueckungsnibble Positionsanzeige), Bit 7 unbenutzt (Eingang).
    // GPIOB: Ausgang (8 Stellungs-LEDs Positionsanzeige)

    bool ledExpanderOk = ledExpander.begin(0xfc, 0x00, 0x00, 0x00);

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

    Serial.println("\n[Mbc91] Start");

    Serial.print("[Mbc91] MCP23S17 (LEDs/Plug/Positionsanzeige): ");
    Serial.println(ledExpanderOk ? "OK" : "FEHLER - Baustein an GPIO7 nicht gefunden");

    Serial.print("[Mbc91] MCP23S17 (Relais/Stellpult): ");
    Serial.println(ioExpanderOk ? "OK" : "FEHLER - Baustein an GPIO6 nicht gefunden");

    canOk = can.begin();
    Serial.print("[Mbc91] MBCAN-Bus (MCP2515): ");
    Serial.println(canOk ? "OK" : "FEHLER - MCP2515 an GPIO10 nicht gefunden");

    bool bootStorageOk = bootStorage.begin();
    Serial.print("[Mbc91] W25Q64 (Boot-Speicher): ");
    Serial.println(bootStorageOk ? "OK" : "FEHLER - Baustein an GPIO12 nicht gefunden/falsche JEDEC-ID");

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
