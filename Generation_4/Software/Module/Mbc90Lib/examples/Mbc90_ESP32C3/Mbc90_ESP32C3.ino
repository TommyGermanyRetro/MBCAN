//*************************************************************************************************
//* mbc-90 S88-Rueckmeldedecoder, 16 Kontakte - ESP32-C3                                          *
//*************************************************************************************************
//* WICHTIG - OTA-Partitionsschema: Dieser Sketch benoetigt zwei App-Partitionen (ota_0/ota_1),   *
//* siehe MbcOta.h. In der Arduino-IDE unter Tools/Werkzeuge -> Partition Scheme "Custom"         *
//* waehlen, damit die mitgelieferte partitions.csv (liegt im selben Sketch-Ordner) verwendet     *
//* wird - ohne das hat ein OTA-Update keine Zielpartition zum Beschreiben und schlaegt fehl.     *
//*                                                                                               *
//* 16 Kontakteingaenge UND Stellungsanzeige haengen an je einem eigenen MCP23S17 (SPI,           *
//* GPIOA=Kontakt 1-8/GPIOB=Kontakt 9-16), beide teilen sich den ohnehin vorhandenen SPI-Bus -    *
//* nur die 2 CS-Pins kommen dazu. LED gruen/rot und die Steckernetzteil-Erkennung sind NATIV     *
//* verkabelt. Dieses Board ist extrem pinknapp - von den 12 sinnvoll nutzbaren GPIO              *
//* (0,1,2,3,4,5,6,7,8,9,10,20,21 minus GPIO9=BOOT-Taste) sind hier 11 belegt. Die 2 CS-Pins fuer *
//* die MCP23S17 belegen GPIO20 (Kontakte) und bewusst GPIO9 (Anzeige, BOOT-Taste) - ein CS-Pin   *
//* liegt als MCU-getriebener Push-Pull-Ausgang im Ruhezustand HIGH (per SPI-Bibliothek           *
//* deterministisch gesetzt, sobald der Sketch laeuft), das Restrisiko beschraenkt sich auf das   *
//* kurze Zeitfenster zwischen Power-On und Sketch-Start, in dem der Pin floatet - trotzdem vor   *
//* dem Verdrahten gegen das jeweilige Devkit pruefen, ob GPIO9 dort zusaetzlich hart mit dem     *
//* BOOT-Taster verdrahtet ist.                                                                   *
//*                                                                                               *
//* WICHTIG: Tools/Werkzeuge -> "USB CDC On Boot" auf "Enabled" stellen, sonst bleiben alle       *
//* Serial-Ausgaben unsichtbar. SPI wird bewusst mit EXPLIZITEN Pins gestartet                    *
//* (SPI.begin(sck,miso,mosi,-1)) statt Boardstandard-Pins, da deren uebliche Default-Belegung    *
//* (oft GPIO4/5/6/7) sich sonst mit hier genutzten GPIOs ueberschneiden wuerde.                  *
//*                                                                                               *
//* Board: "ESP32C3 Dev Module" (Espressif-esp32-Boardpaket). ANNAHME: generisches                *
//* ESP32-C3-DevKit-Pinout - vor dem Verdrahten gegen das jeweilige Board pruefen (GPIO2 ist      *
//* Boot-Strapping-Pin, hier trotzdem fuer TWAI-TX genutzt - Transceiver darf beim Reset keinen   *
//* falschen Pegel ziehen). Keine weiteren GPIOs frei.                                            *
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
//* | MCP23S17-CS (16 Kontakteingaenge)               | GPIO20                |                   *
//* | MCP23S17-CS (Stellungsanzeige)                  | GPIO9 (BOOT-Taste)    |                   *
//* | OneWire                                         | GPIO10                |                   *
//* | ADC Busspannung                                 | GPIO0                 |                   *
//* | ADC Steckernetzteil                             | GPIO1                 |                   *
//* | LED gruen                                       | GPIO7                 |                   *
//* | LED rot                                         | GPIO21                |                   *
//*************************************************************************************************
//* Datei: Mbc90_ESP32C3.ino                                                                      *
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

    const uint8_t PIN_SPI_SCK         = 4;
    const uint8_t PIN_SPI_MISO        = 5;
    const uint8_t PIN_SPI_MOSI        = 6;
    const uint8_t PIN_TWAI_TX         = 2;   // fest, einheitlich ueber alle ESP32-C3-Sketches
    const uint8_t PIN_TWAI_RX         = 3;   // fest, einheitlich ueber alle ESP32-C3-Sketches
    const uint8_t PIN_CS_BOOT_STORAGE = 8;
    const uint8_t PIN_CS_MCP23S17_IN  = 20;   // 16 Kontakteingaenge
    const uint8_t PIN_CS_MCP23S17_ANZ = 9;    // Stellungsanzeige, BOOT-Taste, siehe Kopfkommentar
    const uint8_t PIN_ONEWIRE         = 10;
    const uint8_t PIN_ADC_VSPG        = 0;
    const uint8_t PIN_ADC_PLUG        = 1;
    const uint8_t PIN_LED_GRUEN       = 7;
    const uint8_t PIN_LED_ROT         = 21;

//------ Modulidentitaet (ASCII-Ziffern) ----------------------------------------------------------

    const uint8_t ART_NR[4] = { '9', '0', '1', '2' };   // [2]=Prozessor: 0=ATmega644P/1=ESP32-C3/2=RP2040-Pico/3=RP2040-Zero
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
    MbcCan         can(PIN_TWAI_TX, PIN_TWAI_RX, utils);
    MbcBootStorage bootStorage(PIN_CS_BOOT_STORAGE, MbcBootStorageBackend::FLASH_W25Q64);
    Mbc90Com       com(utils, can, bootStorage, inputs, display);

//------ Status der CAN-Initialisierung (fuer die wiederkehrende Fehlermeldung in loop()) ---------

    bool canOk = false;

//------ setup()/loop() ---------------------------------------------------------------------------

void setup()
{
    SPI.begin(PIN_SPI_SCK, PIN_SPI_MISO, PIN_SPI_MOSI, -1);

    // 16 Kontakteingaenge: GPIOA/GPIOB komplett auf Eingang mit internem Pull-Up

    bool ioExpanderOk = ioExpander.begin(0xff, 0x00, 0xff, 0x00);
    ioExpander.setPullUpA(0xff);
    ioExpander.setPullUpB(0xff);

    // Stellungsanzeige: GPIOA/GPIOB komplett auf Ausgang, initial "alles aus" (aktiv-LOW, 0xff)

    bool displayOk = display.begin(0x00, 0xff, 0x00, 0xff);

    for (uint8_t index = 0; index < 16; index++) inputs[index]->begin();

    utils.begin(ART_NR, SW_VER, HW_VER);

    Serial.begin(115200);

    unsigned long usbStart = millis();
    while ((unsigned long) (millis() - usbStart) < 2000) utils.update();

    Serial.println("\n[Mbc90] Start");

    Serial.print("[Mbc90] MCP23S17 (Kontakte): ");
    Serial.println(ioExpanderOk ? "OK" : "FEHLER - Baustein an GPIO20 nicht gefunden");

    Serial.print("[Mbc90] MCP23S17 (Stellungsanzeige): ");
    Serial.println(displayOk ? "OK" : "FEHLER - Baustein an GPIO9 nicht gefunden");

    canOk = can.begin();
    Serial.print("[Mbc90] MBCAN-Bus (TWAI): ");
    Serial.println(canOk ? "OK" : "FEHLER - Transceiver/Verdrahtung an GPIO2/GPIO3 pruefen");

    bool bootStorageOk = bootStorage.begin();
    Serial.print("[Mbc90] W25Q64 (Boot-Speicher): ");
    Serial.println(bootStorageOk ? "OK" : "FEHLER - Baustein an GPIO8 nicht gefunden/falsche JEDEC-ID");

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
        Serial.println("[Mbc90] FEHLER: MBCAN-Bus (TWAI) weiterhin nicht initialisiert!");
    }

    com.update();
}
