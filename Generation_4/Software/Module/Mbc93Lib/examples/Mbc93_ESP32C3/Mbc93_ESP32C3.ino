//*************************************************************************************************
//* mbc-93 mfx-Booster-Monitor - ESP32-C3                                                         *
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
//* LED gruen/rot UND die Steckernetzteil-Erkennung haengen an einem MCP23S17 (GPIOA Bit 0-2) -   *
//* analog zum etablierten Muster in den anderen ESP32-C3-Sketches dieses Projekts (siehe z.B.    *
//* Mbc91_ESP32C3). mbc-93 hat nur einen CAN-Kanal, hier ueber die interne TWAI (externer CAN-    *
//* Transceiver noetig, z.B. SN65HVD230) statt eines externen MCP2515.                            *
//* WICHTIG: Tools/Werkzeuge -> "USB CDC On Boot" auf "Enabled" stellen, sonst bleiben alle       *
//* Serial-Ausgaben unsichtbar. SPI wird bewusst mit EXPLIZITEN Pins gestartet                    *
//* (SPI.begin(sck,miso,mosi,-1)) statt Boardstandard-Pins, da deren uebliche Default-Belegung    *
//* (oft GPIO4/5/6/7) sich sonst mit hier genutzten GPIOs ueberschneiden wuerde.                  *
//* Board: "ESP32C3 Dev Module" (Espressif-esp32-Boardpaket). ANNAHME: generisches                *
//* ESP32-C3-DevKit-Pinout - vor dem Verdrahten gegen das jeweilige Board pruefen (GPIO9 = BOOT-  *
//* Taste, daher nicht belegt; GPIO2 ist Boot-Strapping-Pin, hier trotzdem fuer TWAI-TX genutzt - *
//* Transceiver darf beim Reset keinen falschen Pegel ziehen). GPIO18 (Luefter-PWM) ist eine      *
//* ungeprueft uebernommene ANNAHME, da auf manchen ESP32-C3-Boards ueber die native              *
//* USB-Serial-JTAG-Funktion belegt - vor dem Verdrahten unbedingt gegen das jeweilige Board      *
//* pruefen.                                                                                      *
//* Kein Reset-Pin: ESP32 nutzt den Software-Reset des Cores (MbcPlatform).                       *
//* Boot-Speicherchip: W25Q64-SPI-NOR-Flash (8 MByte), siehe MbcBootStorage-Kopfkommentar.        *
//* | Funktion                                    | GPIO   |                                      *
//* |-------------------------------------------------|--------|                                  *
//* | SPI SCK / MISO / MOSI                           | GPIO4 / GPIO5 / GPIO6 |                   *
//* | MBCAN TWAI-TX                                   | GPIO2  |                                  *
//* | MBCAN TWAI-RX                                   | GPIO3  |                                  *
//* | Boot-Speicher-CS (W25Q64)                       | GPIO8  |                                  *
//* | MCP23S17-CS (LEDs/Plug)                         | GPIO1  |                                  *
//* | OneWire                                         | GPIO10 |                                  *
//* | ADC Busspannung                                 | GPIO0  |                                  *
//* | Booster ON-Bit                                  | GPIO20 |                                  *
//* | Booster SHORT-Bit                               | GPIO21 |                                  *
//* | Kurzschluss-Erkennung                           | GPIO7  |                                  *
//* | Luefter-PWM (ANNAHME, pruefen)                  | GPIO18 |                                  *
//* | LED gruen (MCP23S17 GPA0)                       | -      |                                  *
//* | LED rot (MCP23S17 GPA1)                         | -      |                                  *
//* | Steckernetzteil (MCP23S17 GPA2)                 | -      |                                  *
//*************************************************************************************************
//* Datei: Mbc93_ESP32C3.ino                                                                      *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen: V1.0.0 - 26.08.2026 - Nullversion                                                *
//*************************************************************************************************

#include <SPI.h>
#include <Mbc93Com.h>
#include <MbcMcp23x17.h>
#include <MbcDigitalIn.h>
#include <MbcDigitalOut.h>

//------ Pinbelegung ------------------------------------------------------------------------------

    const uint8_t PIN_SPI_SCK         = 4;
    const uint8_t PIN_SPI_MISO        = 5;
    const uint8_t PIN_SPI_MOSI        = 6;
    const uint8_t PIN_TWAI_TX         = 2;   // fest, einheitlich ueber alle ESP32-C3-Sketches
    const uint8_t PIN_TWAI_RX         = 3;   // fest, einheitlich ueber alle ESP32-C3-Sketches
    const uint8_t PIN_CS_BOOT_STORAGE = 8;
    const uint8_t PIN_CS_MCP23S17_LED = 1;   // LEDs/Plug-Erkennung
    const uint8_t PIN_ONEWIRE         = 10;
    const uint8_t PIN_ADC_VSPG        = 0;
    const uint8_t PIN_BOOSTER_ON      = 20;
    const uint8_t PIN_BOOSTER_SHORT   = 21;
    const uint8_t PIN_SHORT_DETECT    = 7;
    const uint8_t PIN_FAN             = 18;   // ANNAHME, siehe Kopfkommentar

//------ Modulidentitaet (ASCII-Ziffern) ----------------------------------------------------------

    // [2]=Prozessor: 0=ATmega644P/1=ESP32-C3/2=RP2040-Pico/3=RP2040-Zero

    const uint8_t ART_NR[4] = { '9', '3', '1', '1' };
    const uint8_t SW_VER[3] = { '1', '0', '0' };
    const uint8_t HW_VER[6] = { '2', '4', '0', '5', '0', '1' };   // Hardware 24.05.01

//------ Objekte ----------------------------------------------------------------------------------

    MbcMcp23x17 ledExpander(PIN_CS_MCP23S17_LED);   // auch Steckernetzteil-Erkennung

    // GPIOA-Bits: 0=LED gruen, 1=LED rot (beide HIGH-aktiv), 2=Steckernetzteil-Erkennung
    // (Eingang, ANNAHME HIGH-aktiv), GPIOB unbenutzt

    MbcDigitalOut ledGruenOut(ledExpander, false, 0, false);
    MbcDigitalOut ledRotOut(ledExpander, false, 1, false);
    MbcDigitalIn  plugDetectorIn(ledExpander, false, 2, false);

    MbcDigitalOut pinBoosterOn(PIN_BOOSTER_ON), pinBoosterShort(PIN_BOOSTER_SHORT);

    // genPurpSize=4 (STATUS, SW_1, SW_2, SH), extraEepromSize=3 (siehe EEX_93_*-Konstanten
    // in Mbc93Com.cpp)

    static uint8_t sysArray[MBC_SYSARRAY_SIZE(4)];

    MbcUtils       utils(sysArray, 4, PIN_ADC_VSPG, 0, 0, 0, PIN_ONEWIRE, 3);   // Plug-/LED-
                                                                                // Pins ungenutzt, siehe
                                                                                // setPlugDetector()/setLedDrivers()
    MbcCan         can(PIN_TWAI_TX, PIN_TWAI_RX, utils);
    MbcBootStorage bootStorage(PIN_CS_BOOT_STORAGE, MbcBootStorageBackend::FLASH_W25Q64);
    Mbc93Com       com(utils, can, bootStorage, pinBoosterOn, pinBoosterShort,
                       PIN_SHORT_DETECT, PIN_FAN);

//------ Status der CAN-Initialisierung (fuer die wiederkehrende Fehlermeldung in loop()) ---------

    bool canOk = false;

//------ setup()/loop() ---------------------------------------------------------------------------

void setup()
{
    SPI.begin(PIN_SPI_SCK, PIN_SPI_MISO, PIN_SPI_MOSI, -1);

    // GPIOA: Bit 0/1 Ausgang (LEDs), Bit 2 Eingang (Steckernetzteil), Rest unbenutzt

    bool ledExpanderOk = ledExpander.begin(0xf8, 0x00, 0x00, 0x00);

    utils.setLedDrivers(&ledGruenOut, &ledRotOut);
    utils.setPlugDetector(&plugDetectorIn);

    utils.begin(ART_NR, SW_VER, HW_VER);

    Serial.begin(115200);

    unsigned long usbStart = millis();
    while ((unsigned long) (millis() - usbStart) < 2000) utils.update();

    Serial.println("\n[Mbc93] Start");

    Serial.print("[Mbc93] MCP23S17 (LEDs/Plug): ");
    Serial.println(ledExpanderOk ? "OK" : "FEHLER - Baustein an GPIO1 nicht gefunden");

    canOk = can.begin();
    Serial.print("[Mbc93] MBCAN-Bus (TWAI): ");
    Serial.println(canOk ? "OK" : "FEHLER - Transceiver/Verdrahtung an GPIO2/GPIO3 pruefen");

    bool bootStorageOk = bootStorage.begin();
    Serial.print("[Mbc93] W25Q64 (Boot-Speicher): ");
    Serial.println(bootStorageOk ? "OK" : "FEHLER - Baustein an GPIO8 nicht gefunden/falsche JEDEC-ID");

    com.begin();
    com.beginModule();

    // Warte, bis alle Statuswerte (Vspg/Plug/Temperatur) einmal ermittelt wurden - blinkt
    // waehrenddessen ca. 10 s durchgehend ORANGE (siehe MbcUtils::waitFilled())

    utils.waitFilled();

    Serial.println("[Mbc93] Bereit");
}

void loop()
{
    utils.update();
    utils.check();

    if (utils.eventSekunde() && !canOk)
    {
        Serial.println("[Mbc93] FEHLER: MBCAN-Bus (TWAI) weiterhin nicht initialisiert!");
    }

    com.update();
}

