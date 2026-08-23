//*************************************************************************************************
//* mbc-92 Magnetartikel-Schaltdecoder - ESP32-C3                                                 *
//*************************************************************************************************
//* WICHTIG - OTA-Partitionsschema: Dieser Sketch benoetigt zwei App-Partitionen (ota_0/ota_1),   *
//* siehe MbcOta.h. In der Arduino-IDE unter Tools/Werkzeuge -> Partition Scheme "Custom"         *
//* waehlen, damit die mitgelieferte partitions.csv (liegt im selben Sketch-Ordner) verwendet     *
//* wird - ohne das hat ein OTA-Update keine Zielpartition zum Beschreiben und schlaegt fehl.     *
//*                                                                                               *
//* LED gruen/rot und Steckernetzteil-Erkennung haengen an einem gemeinsamen MCP23S17 (GPIOA Bit  *
//* 0-2 LEDs/Plug, GPIOB Bit 0-3 fuer die 4 Stellungsanzeige-LEDs - mbc-92 hat keine              *
//* Bestueckungserkennung). Ein zweiter MCP23S17 bedient die 8 Relaisausgaenge + 8 Stellpult-     *
//* Eingaenge (auf diesem pinknappen Board nicht nativ unterzubringen). mbc-92 hat nur einen CAN- *
//* Kanal, hier ueber die interne TWAI (externer CAN-Transceiver noetig, z.B. SN65HVD230) statt   *
//* eines externen MCP2515.                                                                       *
//*                                                                                               *
//* WICHTIG: Tools/Werkzeuge -> "USB CDC On Boot" auf "Enabled" stellen, sonst bleiben alle       *
//* Serial-Ausgaben unsichtbar. SPI wird bewusst mit EXPLIZITEN Pins gestartet                    *
//* (SPI.begin(sck,miso,mosi,-1)) statt Boardstandard-Pins, da deren uebliche Default-Belegung    *
//* (oft GPIO4/5/6/7) sich sonst mit hier genutzten GPIOs ueberschneiden wuerde.                  *
//*                                                                                               *
//* Board: "ESP32C3 Dev Module" (Espressif-esp32-Boardpaket). ANNAHME: generisches                *
//* ESP32-C3-DevKit-Pinout - vor dem Verdrahten gegen das jeweilige Board pruefen (GPIO9 =        *
//* BOOT-Taste, daher nicht belegt; GPIO2 ist Boot-Strapping-Pin, hier trotzdem fuer TWAI-TX      *
//* genutzt - Transceiver darf beim Reset keinen falschen Pegel ziehen). GPIO7/GPIO21 bleiben     *
//* frei fuer Erweiterungen.                                                                      *
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
//* | MCP23S17-CS (LEDs/Plug/Stellungsanzeige)        | GPIO1  |                                  *
//* | MCP23S17-CS (Relais+Stellpult)                  | GPIO20 |                                  *
//* | OneWire                                         | GPIO10 |                                  *
//* | ADC Busspannung                                 | GPIO0  |                                  *
//* | LED gruen (MCP23S17 GPA0)                       | -      |                                  *
//* | LED rot (MCP23S17 GPA1)                         | -      |                                  *
//* | Steckernetzteil (MCP23S17 GPA2)                 | -      |                                  *
//* | Stellungsanzeige 1..4 (MCP23S17 GPB0-3)         | -      |                                  *
//*************************************************************************************************
//* Datei: Mbc92_ESP32C3.ino                                                                      *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen:                                                                                  *
//* V1.0.0 - 14.08.2026 - Nullversion                                                             *
//*************************************************************************************************

#include <SPI.h>
#include <Mbc92Com.h>
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
    const uint8_t PIN_CS_MCP23S17_LED = 1;   // LEDs/Plug-Erkennung/Stellungsanzeige
    const uint8_t PIN_CS_MCP23S17_REL = 20;  // Relais+Stellpult
    const uint8_t PIN_ONEWIRE         = 10;
    const uint8_t PIN_ADC_VSPG        = 0;

//------ Modulidentitaet (ASCII-Ziffern) ----------------------------------------------------------

    const uint8_t ART_NR[4] = { '9', '2', '1', '1' };   // [2]=Prozessor: 0=ATmega644P/1=ESP32-C3/2=RP2040-Pico/3=RP2040-Zero
    const uint8_t SW_VER[3] = { '1', '0', '0' };
    const uint8_t HW_VER[6] = { '2', '4', '0', '7', '2', '0' };   // Hardware 24.07.20

//------ Objekte ----------------------------------------------------------------------------------

    MbcMcp23x17 ledExpander(PIN_CS_MCP23S17_LED);   // auch Stellungsanzeige, siehe Kopfkommentar
    MbcMcp23x17 ioExpander(PIN_CS_MCP23S17_REL);

    // GPIOA-Bits des LED-Expanders: 0=LED gruen, 1=LED rot (beide HIGH-aktiv), 2=
    // Steckernetzteil-Erkennung (Eingang, ANNAHME HIGH-aktiv). GPIOB Bit 0-3 = 4
    // Stellungsanzeige-LEDs (siehe Mbc92Com::stateDisplay())

    MbcDigitalOut ledGruenOut(ledExpander, false, 0, false);
    MbcDigitalOut ledRotOut(ledExpander, false, 1, false);
    MbcDigitalIn  plugDetectorIn(ledExpander, false, 2, false);

    MbcDigitalOut anz1(ledExpander, true, 0, false);
    MbcDigitalOut anz2(ledExpander, true, 1, false);
    MbcDigitalOut anz3(ledExpander, true, 2, false);
    MbcDigitalOut anz4(ledExpander, true, 3, false);

    // Relais/Stellpult: GPIOA = 8 Ausgaenge, GPIOB = 8 Eingaenge des ioExpander

    MbcDigitalOut relL1(ioExpander, false, 0, false), relR1(ioExpander, false, 1, false);
    MbcDigitalOut relL2(ioExpander, false, 2, false), relR2(ioExpander, false, 3, false);
    MbcDigitalOut relL3(ioExpander, false, 4, false), relR3(ioExpander, false, 5, false);
    MbcDigitalOut relL4(ioExpander, false, 6, false), relR4(ioExpander, false, 7, false);

    MbcDigitalIn inG1(ioExpander, true, 0, true), inR1(ioExpander, true, 1, true);
    MbcDigitalIn inG2(ioExpander, true, 2, true), inR2(ioExpander, true, 3, true);
    MbcDigitalIn inG3(ioExpander, true, 4, true), inR3(ioExpander, true, 5, true);
    MbcDigitalIn inG4(ioExpander, true, 6, true), inR4(ioExpander, true, 7, true);

    // Index-Konvention siehe Mbc92Com-Klassenkommentar (0..3 = Doppelspulenantrieb 1..4 bzw.
    // 0..7 = _1_L,_1_R,_2_L,_2_R,...)

    MbcDigitalOut *anzeige[4]   = { &anz1, &anz2, &anz3, &anz4 };
    MbcDigitalOut *relais[8]    = { &relL1, &relR1, &relL2, &relR2,
                                     &relL3, &relR3, &relL4, &relR4 };
    MbcDigitalIn  *stellpult[8] = { &inG1, &inR1, &inG2, &inR2, &inG3, &inR3, &inG4, &inR4 };

    static uint8_t sysArray[MBC_SYSARRAY_SIZE(10)];

    MbcUtils       utils(sysArray, 10, PIN_ADC_VSPG, 0, 0, 0, PIN_ONEWIRE, 10);   // Plug-/LED-
                                                                           // Pins ungenutzt, siehe
                                                                           // setPlugDetector()/
                                                                           // setLedDrivers()
    MbcCan         can(PIN_TWAI_TX, PIN_TWAI_RX, utils);
    MbcBootStorage bootStorage(PIN_CS_BOOT_STORAGE, MbcBootStorageBackend::FLASH_W25Q64);
    Mbc92Com       com(utils, can, bootStorage, anzeige, relais, stellpult);

//------ Status der CAN-Initialisierung (fuer die wiederkehrende Fehlermeldung in loop()) ---------

    bool canOk = false;

//------ setup()/loop() ---------------------------------------------------------------------------

void setup()
{
    SPI.begin(PIN_SPI_SCK, PIN_SPI_MISO, PIN_SPI_MOSI, -1);

    // GPIOA: Bit 0/1 Ausgang (LEDs), Bit 2 Eingang (Steckernetzteil-Erkennung), Bit 3-7
    // unbenutzt (Eingang). GPIOB: Bit 0-3 Ausgang (4 Stellungsanzeige-LEDs), Bit 4-7 unbenutzt

    bool ledExpanderOk = ledExpander.begin(0xfc, 0x00, 0xf0, 0x00);

    // Relais/Stellpult: GPIOA Ausgang (8 Relais), GPIOB Eingang mit PullUp (8 Taster)

    bool ioExpanderOk = ioExpander.begin(0x00, 0x00, 0xff, 0xff);
    ioExpander.setPullUpB(0xff);   // GPIOB (Stellpult-Taster): interne Pull-Up aktivieren

    utils.setLedDrivers(&ledGruenOut, &ledRotOut);
    utils.setPlugDetector(&plugDetectorIn);

    utils.begin(ART_NR, SW_VER, HW_VER);

    Serial.begin(115200);

    unsigned long usbStart = millis();
    while ((unsigned long) (millis() - usbStart) < 2000) utils.update();

    Serial.println("\n[Mbc92] Start");

    Serial.print("[Mbc92] MCP23S17 (LEDs/Plug/Stellungsanzeige): ");
    Serial.println(ledExpanderOk ? "OK" : "FEHLER - Baustein an GPIO1 nicht gefunden");

    Serial.print("[Mbc92] MCP23S17 (Relais/Stellpult): ");
    Serial.println(ioExpanderOk ? "OK" : "FEHLER - Baustein an GPIO20 nicht gefunden");

    canOk = can.begin();
    Serial.print("[Mbc92] MBCAN-Bus (TWAI): ");
    Serial.println(canOk ? "OK" : "FEHLER - Transceiver/Verdrahtung an GPIO2/GPIO3 pruefen");

    bool bootStorageOk = bootStorage.begin();
    Serial.print("[Mbc92] W25Q64 (Boot-Speicher): ");
    Serial.println(bootStorageOk ? "OK" : "FEHLER - Baustein an GPIO8 nicht gefunden/falsche JEDEC-ID");

    com.begin();
    com.beginModule();

    // Warte, bis alle Statuswerte (Vspg/Plug/Temperatur) einmal ermittelt wurden - blinkt
    // waehrenddessen ca. 10 s durchgehend ORANGE (siehe MbcUtils::waitFilled())

    utils.waitFilled();

    Serial.println("[Mbc92] Bereit");
}

void loop()
{
    utils.update();
    utils.check();

    if (utils.eventSekunde() && !canOk)
    {
        Serial.println("[Mbc92] FEHLER: MBCAN-Bus (TWAI) weiterhin nicht initialisiert!");
    }

    com.update();
}
