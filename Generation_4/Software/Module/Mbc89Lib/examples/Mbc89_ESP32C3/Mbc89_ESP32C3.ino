//*************************************************************************************************
//* mbc-89 Connect6021-Interface - ESP32-C3                                                       *
//*************************************************************************************************
//* Board: "ESP32-C3 SuperMini" (China-Modul, Espressif-esp32-Boardpaket "ESP32C3 Dev Module") -  *
//* dieses Board fuehrt nur 13 GPIOs heraus (GPIO0...10, GPIO20, GPIO21). mbc-89 braucht deutlich *
//* mehr Signale (CU-Bus allein 5, dazu Track/SPI/TWAI/LED/ADC/OneWire) - die unkritischen,       *
//* nicht zeitkritischen Signale (LEDs, Steckernetzteil-Erkennung, CU-STOP/-GO/-EXT) haengen daher*
//* an einem MCP23S17-SPI-Port-Expander, genau wie bei den anderen ESP32-C3-Sketches dieses       *
//* Projekts (siehe z.B. Mbc93_ESP32C3.ino). Nur CU-Bus SCL/SDA (Software-I2C) und das            *
//* Gleisdecoder-Signal (RMT) bleiben auf nativen GPIOs, da zeitkritisch.                         *
//* WICHTIG - OTA-Partitionsschema: Dieser Sketch benoetigt zwei App-Partitionen (ota_0/ota_1),   *
//* siehe Mbc89Ota.h. Kein Menue-Eintrag noetig: die mitgelieferte partitions.csv liegt im        *
//* Sketch-Ordner und wird vom Build-Prozess automatisch uebernommen, unabhaengig von der unter   *
//* Tools/Werkzeuge -> Partition Scheme gewaehlten Option (es gibt keinen "Custom"-Menueeintrag,  *
//* verifiziert mit Arduino-ESP32-Core 2.0.17). Wird die partitions.csv-Datei entfernt/verschoben,*
//* hat ein OTA-Update keine Zielpartition zum Beschreiben und schlaegt fehl.                     *
//* WICHTIG - richtige Datei fuer OTA: Im ParaCenter nur die reine <Sketchname>.ino.bin verwenden,*
//* NICHT .merged.bin/.bootloader.bin/*_flashed.bin (Boardpaket-Ausgaben fuer den anfaenglichen   *
//* USB-Flash bzw. veraltete Zwischenstaende) - nur die reine App-Datei passt zum OTA-Mechanismus *
//* (Mbc89Ota.h).                                                                                 *
//* WICHTIG - USB-Flash nach einem OTA-Test: Arduino/esptool schreiben beim USB-Upload IMMER an   *
//* die feste Adresse von ota_0 (0x10000) und aendern dabei die otadata-Partition (Boot-Auswahl)  *
//* NICHT. Zeigt otadata nach einem vorherigen OTA-Update auf ota_1, laeuft nach einem erneuten   *
//* USB-Flash weiterhin die ALTE, zuletzt per OTA in ota_1 geschriebene Firmware - der neue       *
//* USB-Stand in ota_0 wird schlicht nicht gebootet. Abhilfe: vor dem naechsten USB-Flash unter   *
//* Tools/Werkzeuge "Erase All Flash Before Sketch Upload" aktivieren (oder gezielt nur otadata   *
//* loeschen: esptool.py erase_region 0xE000 0x2000), damit der Bootloader auf ota_0 zurueckfaellt.*
//* WICHTIG: Tools/Werkzeuge -> "USB CDC On Boot" auf "Enabled" stellen, sonst bleiben alle       *
//* Serial-Ausgaben unsichtbar (dadurch werden GPIO20/21 als normale GPIOs frei, siehe unten).    *
//* SPI wird mit EXPLIZITEN Pins gestartet (SPI.begin(sck,miso,mosi,-1)) statt Boardstandard-Pins.*
//* GPIO9 (BOOT-Taste/Strapping-Pin) bleibt bewusst frei/unbenutzt (Reserve).                     *
//* Kein Reset-Pin: ESP32 nutzt den Software-Reset des Cores (Mbc89Platform::resetDevice()), samt *
//* OTA-Uebernahme aus dem Boot-Speicher (Mbc89Ota, siehe Mbc89ComBase::triggerReset()).          *
//* Boot-Speicherchip: W25Q64-SPI-NOR-Flash (8 MByte), siehe Mbc89BootStorage-Kopfkommentar.      *
//* CU-Bus/Gleisdecoder sind hier ueber eine selbstgeschriebene Software-I2C-Engine               *
//* (Mbc89I2cSoft) bzw. die RMT-Peripherie umgesetzt (siehe Klassenkommentar in Mbc89Com.h/.cpp) -*
//* NEUE, in diesem Projekt bisher ungetestete Technik. Vor Dauerbetrieb ausgiebig mit            *
//* Logic-Analyzer/Oszilloskop verifizieren (siehe README, Abschnitt Verifikation), inkl. Pruefung*
//* des MCP23S17-Praesenzergebnisses beim Start (siehe Serial-Ausgabe unten).                     *
//* | Funktion                                | GPIO    | Anmerkung                               *
//* |------------------------------------------|---------|----------------------------------------*
//* | SPI SCK / MISO / MOSI                   | 4/5/6   | fest, wie in allen ESP32-C3-Sketches    *
//* | MBCAN TWAI-TX / TWAI-RX                 | 2 / 3   | fest, wie in allen ESP32-C3-Sketches    *
//* | Boot-Speicher-CS (W25Q64)               | 8       |                                         *
//* | MCP23S17-CS (LEDs/Plug/CU-STOP/GO/EXT)  | 1       |                                         *
//* | OneWire (optionaler Temperatursensor)   | 10      |                                         *
//* | ADC Busspannung                         | 0       | ADC1_CH0                                *
//* | CU-Bus SCL / SDA (Software-I2C)         | 7 / 20  | zeitkritisch - natives GPIO             *
//* | Gleisdecoder-Signal (RMT)               | 21      | zeitkritisch - natives GPIO             *
//* | (Reserve, unbenutzt)                    | 9       | BOOT-Taste/Strapping-Pin, frei gelassen *
//* | LED gruen (MCP23S17 GPA0)               | -       |                                         *
//* | LED rot (MCP23S17 GPA1)                 | -       |                                         *
//* | Steckernetzteil-Erkennung (MCP23S17 GPA2)| -      |                                         *
//* | CU-STOP-IN (MCP23S17 GPA3)               | -      | Eingang, Pull-Up aktiv                  *
//* | CU-STOP-OUT (MCP23S17 GPA4)              | -      | Ausgang, activeLow (Idle=HIGH)          *
//* | CU-GO-IN (MCP23S17 GPA5)                 | -      | Eingang, Pull-Up aktiv                  *
//* | CU-GO-OUT (MCP23S17 GPA6)                | -      | Ausgang, activeLow (Idle=HIGH)          *
//* | CU-EXT (MCP23S17 GPA7)                   | -      | Eingang, ohne Pull-Up                   *
//*************************************************************************************************
//* Datei: Mbc89_ESP32C3.ino                                                                      *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen: V1.0.0 - 27.08.2026 - Nullversion                                                *
//*************************************************************************************************

#include <SPI.h>
#include <Mbc89Com.h>
#include <Mbc89Mcp23x17.h>
#include <Mbc89DigitalIn.h>
#include <Mbc89DigitalOut.h>

//------ Pinbelegung ------------------------------------------------------------------------------

    const uint8_t PIN_SPI_SCK         = 4;
    const uint8_t PIN_SPI_MISO        = 5;
    const uint8_t PIN_SPI_MOSI        = 6;
    const uint8_t PIN_TWAI_TX         = 2;   // fest, einheitlich ueber alle ESP32-C3-Sketches
    const uint8_t PIN_TWAI_RX         = 3;   // fest, einheitlich ueber alle ESP32-C3-Sketches
    const uint8_t PIN_CS_BOOT_STORAGE = 8;
    const uint8_t PIN_CS_EXPANDER     = 1;   // LEDs/Plug/CU-STOP/GO/EXT
    const uint8_t PIN_ONEWIRE         = 10;
    const uint8_t PIN_ADC_VSPG        = 0;
    const uint8_t PIN_CU_SCL          = 7;
    const uint8_t PIN_CU_SDA          = 20;
    const uint8_t PIN_TRACK           = 21;

//------ Modulidentitaet (ASCII-Ziffern) ----------------------------------------------------------

    // [2]=Prozessor: 0=ATmega644P/1=ESP32-C3

    const uint8_t ART_NR[4] = { '8', '9', '1', '2' };
    const uint8_t SW_VER[3] = { '1', '0', '0' };
    const uint8_t HW_VER[6] = { '2', '2', '0', '3', '0', '1' };   // Hardware 22.03.01

//------ Objekte ----------------------------------------------------------------------------------

    Mbc89Mcp23x17 expander(PIN_CS_EXPANDER);   // LEDs/Plug/CU-STOP/GO/EXT

    // GPIOA-Bits: 0=LED gruen(Ausg.), 1=LED rot(Ausg.), 2=Steckernetzteil(Eing.),
    // 3=CU-STOP-IN(Eing., Pull-Up), 4=CU-STOP-OUT(Ausg., activeLow), 5=CU-GO-IN(Eing., Pull-Up),
    // 6=CU-GO-OUT(Ausg., activeLow), 7=CU-EXT(Eing., ohne Pull-Up), GPIOB unbenutzt

    Mbc89DigitalOut ledGruenOut(expander, false, 0, false);
    Mbc89DigitalOut ledRotOut(expander, false, 1, false);
    Mbc89DigitalIn  plugDetectorIn(expander, false, 2, false);
    Mbc89DigitalIn  cuStopIn(expander, false, 3, true);
    Mbc89DigitalOut cuStopOut(expander, false, 4, true);
    Mbc89DigitalIn  cuGoIn(expander, false, 5, true);
    Mbc89DigitalOut cuGoOut(expander, false, 6, true);
    Mbc89DigitalIn  cuExt(expander, false, 7, false);

    // genPurpSize=880 (SW_1/SW_2, 810 Byte Lok-Datenbank, 65 Byte Zubehoerprotokoll, Zentralentyp-
    // Mapping, Basisadressblock, I2C-Frequenz), extraEepromSize=70 (siehe EEX_89_*-Konstanten in
    // Mbc89Com.cpp) - Plug-/LED-Pins ungenutzt, siehe setPlugDetector()/setLedDrivers()

    static uint8_t sysArray[MBC_SYSARRAY_SIZE(880)];

    Mbc89Utils       utils(sysArray, 880, PIN_ADC_VSPG, 0, 0, 0, PIN_ONEWIRE, 70);
    Mbc89Can         can(PIN_TWAI_TX, PIN_TWAI_RX, utils);
    Mbc89BootStorage bootStorage(PIN_CS_BOOT_STORAGE, Mbc89BootStorageBackend::FLASH_W25Q64);
    Mbc89Com         com(utils, can, bootStorage, PIN_CU_SCL, PIN_CU_SDA, cuStopIn, cuStopOut,
                         cuGoIn, cuGoOut, cuExt, PIN_TRACK);

//------ Status der CAN-Initialisierung (fuer die wiederkehrende Fehlermeldung in loop()) ---------

    bool canOk = false;

//------ setup()/loop() ---------------------------------------------------------------------------

void setup()
{
    SPI.begin(PIN_SPI_SCK, PIN_SPI_MISO, PIN_SPI_MOSI, -1);

    // GPIOA: Bit 0/1/4/6 Ausgang (LEDs, CU-STOP-OUT, CU-GO-OUT), Bit 2/3/5/7 Eingang (Plug,
    // CU-STOP-IN, CU-GO-IN, CU-EXT); CU-STOP-OUT/CU-GO-OUT idle=HIGH (activeLow); Pull-Up auf
    // CU-STOP-IN/CU-GO-IN (Taster gegen Masse), bewusst NICHT auf CU-EXT (siehe Mbc89Com.h)

    bool expanderOk = expander.begin(0xac, 0x50, 0xff, 0x00);
    expander.setPullUpA(0x28);

    utils.setLedDrivers(&ledGruenOut, &ledRotOut);
    utils.setPlugDetector(&plugDetectorIn);

    utils.begin(ART_NR, SW_VER, HW_VER);

    Serial.begin(115200);

    unsigned long usbStart = millis();
    while ((unsigned long) (millis() - usbStart) < 2000) utils.update();

    Serial.println("\n[Mbc89] Start");

    Serial.print("[Mbc89] MCP23S17 (LEDs/Plug/CU-STOP/GO/EXT): ");
    Serial.println(expanderOk ? "OK" : "FEHLER - Baustein an GPIO1 nicht gefunden");

    canOk = can.begin();
    Serial.print("[Mbc89] MBCAN-Bus (TWAI): ");
    Serial.println(canOk ? "OK" : "FEHLER - Transceiver/Verdrahtung an GPIO2/GPIO3 pruefen");

    bool bootStorageOk = bootStorage.begin();
    Serial.print("[Mbc89] W25Q64 (Boot-Speicher): ");
    Serial.println(bootStorageOk ? "OK" : "FEHLER - Baustein an GPIO8 nicht gefunden/falsche JEDEC-ID");

    com.begin();
    com.beginModule();

    // Warte, bis alle Statuswerte (Vspg/Plug/Temperatur) einmal ermittelt wurden - blinkt
    // waehrenddessen ca. 10 s durchgehend ORANGE (siehe Mbc89Utils::waitFilled())

    utils.waitFilled();

    Serial.println("[Mbc89] Bereit");
}

void loop()
{
    utils.update();
    utils.check();

    if (utils.eventSekunde() && !canOk)
    {
        Serial.println("[Mbc89] FEHLER: MBCAN-Bus (TWAI) weiterhin nicht initialisiert!");
    }

    com.update();
}
