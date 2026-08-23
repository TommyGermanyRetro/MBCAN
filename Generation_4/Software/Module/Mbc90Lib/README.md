# Mbc90Lib

Modulspezifische Erweiterung von [MBCANLib](../MBCANLib) für **mbc-90**, einen S88-Rückmeldedecoder
als **Vollmodul mit 16 Kontakten** (deckt selbst einen kompletten 16er-CS2-Adressblock ab) mit
einer Stellungsanzeige über einen Portexpander statt direkter LED-Ausgänge (Backend
plattformabhängig: PCF8574/I2C auf ATmega644P, MCP23S17/SPI auf RP2040/ESP32-C3). Ein "echtes"
Zubehörmodul mit eigenen I/Os an nur einem CAN-Kanal.

## Was mbc-90 tut

- **16 Kontakteingänge** — aktiv-LOW mit PullUp (gepulste 0-Leitung), je Pin ein Integrator als
  Entpreller/Haltezeit-Ersatz: ein Kontaktwechsel wird erst nach Ablauf einer konfigurierbaren,
  pro Kontakt einstellbaren Integrationszeit als endgültiges Ereignis gemeldet.
- **Stellungsanzeige über einen Portexpander** statt direkter GPIO-LEDs — reine Anzeige, keine
  steuernde Funktion, aktiv-LOW-Ausgang, nur bei Änderung tatsächlich geschrieben. Backend
  plattformabhängig: auf ATmega644P 2× PCF8574 (I2C, Bausteinnummer 0 = Kontakt 1-8,
  Bausteinnummer 1 = Kontakt 9-16), auf RP2040/ESP32-C3 ein einzelner MCP23S17 (SPI, GPIOA =
  Kontakt 1-8, GPIOB = Kontakt 9-16) — beide Varianten sind in `Mbc90Com` per `#if
  defined(__AVR__)` gekapselt, der Sketch übergibt je nach Plattform die passenden Objekte.
- **Kein Halbmodul-Konzept** — mbc-90 ist selbst ein volles 16er-Modul, alle Adress-/
  Pin-Berechnungen verwenden den Kontaktindex 0-15 direkt.
- **Rückmeldestatus** (2 Byte im Systemarray) bewusst **nicht invertiert**, Bit *i* = Kontakt *i*+1
  — verifiziert gegen das Parametriercenter.
- **CS2-S88-Anfragebeantwortung** (CAN-ID `0x22`/`0x23`) sowie ein Broadcast an ein vorgelagertes
  LinkS88-Gateway bei jedem Kontaktwechsel — beide Nachrichtentemplates sind bereits in der
  `MbcCom`-Basisklasse vorgefüllt.
- Die übliche `MbcCom`-Modulidentitäts-/Parametrierkommunikation, ergänzt um **17 CS2-GUI-
  Konfigkanäle**: 1× S88-Modulnummer sowie 16× Integrationszeit (K1…K16, je Kontakt einzeln
  einstellbar). Die CS2-Anmeldung selbst (Fetch + Set-Value für S88-Modulnummer, K1 und K2) ist
  hardwareverifiziert; K3…K16 nutzen dasselbe Muster, stehen aber noch aus. Der rohe
  PC_ARRAY_DATA-Weg bleibt davon unabhängig weiterhin nutzbar.

## Installation

1. [MBCANLib](../MBCANLib) installieren (Abhängigkeit, siehe `depends=MBCANLib` in
   `library.properties`).
2. Diesen Ordner (`Mbc90Lib`) nach `<Arduino-Sketchbook>/libraries/Mbc90Lib` kopieren.
3. Arduino-IDE neu starten — die fünf Beispiel-Sketches erscheinen unter
   *Datei → Beispiele → Mbc90Lib*.

## Beispiel-Sketches

Ein vollständiger, direkt flashbarer Sketch je Zielboard. Die Kern-Pins (SPI, CAN, Boot-Speicher,
1-Wire, ADC-Busspannung) folgen überall der üblichen Standard-Pinbelegung für die jeweilige
Plattform. mbc-90 braucht keine direkten LED-Ausgangspins:

| Sketch | Board | 16 Kontakte | Stellungsanzeige |
|---|---|---|---|
| `Mbc90_ATmega644P_HW180304` | ATmega644P (MightyCore) | nativ | 2x PCF8574, natives I2C |
| `Mbc90_ATmega644P_HW231125` | ATmega644P (MightyCore) | nativ | 2x PCF8574, natives I2C |
| `Mbc90_RP2040_Pico` | Raspberry Pi Pico | nativ | MCP23S17, eigener CS-Pin auf SPI0 |
| `Mbc90_RP2040_Zero` | Waveshare RP2040-Zero | MCP23S17, eigener CS-Pin auf SPI0 | MCP23S17, eigener CS-Pin auf SPI0 |
| `Mbc90_ESP32C3` | ESP32-C3 | MCP23S17, eigener CS-Pin | MCP23S17, eigener CS-Pin auf demselben SPI-Bus |

`Mbc90_ATmega644P_HW180304` ist eine ältere Hardware-Generation (Hardware-Datum 04.03.2018).
Pinbelegung UND Ablauflogik sind dabei identisch zu `Mbc90_ATmega644P_HW231125` — beide
Revisionen teilen dieselbe Platinenbelegung und Firmware-Klasse, nur Hardware-Datum/ArtNum4/
HW_VER unterscheiden sich.

Auf RP2040-Pico bleiben die 16 Kontakteingänge nativ (genug freie GPIO) — nur die
Stellungsanzeige läuft über einen MCP23S17 (SPI, deckt mit GPIOA/GPIOB beide Byte ab), der sich
den ohnehin vorhandenen SPI-Bus mit MBCAN-CS/Boot-Speicher-CS teilt (nur ein zusätzlicher CS-Pin
nötig). Auf RP2040-Zero und ESP32-C3 laufen **beide** Expander (Kontakte UND Anzeige) über SPI,
jeweils mit eigenem CS-Pin auf demselben Bus — kein I2C mehr in diesen beiden Sketches. Auf dem
extrem pinknappen ESP32-C3 ist das zugleich pin-neutral (2 CS-Pins statt 2 I2C-Pins) und vermeidet
einen I2C-SCL-auf-GPIO9(BOOT-Taste)-Kompromiss zugunsten eines CS-auf-GPIO9-Kompromisses (dazu
Details im Kopfkommentar des Sketches).

**Wichtig:** Die exakten Pin-Zuordnungen und alle ANNAHME-Vermerke stehen im Kopfkommentar jedes
Sketches — vor dem Verdrahten/Flashen dort nachsehen, nicht hier.

**Wichtig (Bootloader, nur ATmega644P):** Vor dem ersten Einsatz muss der MBCAN-Bootloader einmalig
per ISP auf den ATmega644P aufgespielt werden (reserviert 2048 Byte Flash für spätere
Firmware-Updates über das Parametriercenter). Board-Definition (`boards.txt`), Bootloader-Hex und
die komplette Schritt-für-Schritt-Anleitung liegen im Ordner `Bootloaderinfos_MightyCore/` neben
diesem Sketch (`README.txt`) — dieselbe Anleitung steht auch im Kopfkommentar des Sketches.

**Wichtig (OTA, nur RP2040/ESP32-C3):** Beide Cores benötigen eine reservierte zweite Flash-
Partition für Firmware-Updates (siehe `MbcOta.h`-Kopfkommentar). Beim ESP32-C3 über die
mitgelieferte `partitions.csv` im jeweiligen Sketch-Ordner (*Tools/Werkzeuge → Partition Scheme →
Custom*); beim RP2040 dagegen **ohne** CSV-Datei ausschließlich über *Tools/Werkzeuge → Flash
Size* (eine Variante mit „OTA" im Namen wählen) — ohne diese Einstellung findet ein OTA-Update
keine Zielpartition und schlägt fehl.

## Verwendung

```cpp
#include <SPI.h>
#include <Mbc90Com.h>

// 16 Eingangs-Objekte (MbcDigitalIn - natives GPIO oder MCP23x17-Bit je nach Board)
// Index-Konvention: 0..15 = S88-Kontakt 1..16
MbcDigitalIn *inputs[16] = { ... };

// Stellungsanzeige, Backend plattformabhaengig (siehe Mbc90Com-Konstruktor):
#if defined(__AVR__)
    // ATmega644P: 2x PCF8574 auf dem nativen I2C-Bus
    MbcI2cBus  i2cBus;
    MbcPcf8574 displayLow(i2cBus, 0);    // Kontakt 1-8
    MbcPcf8574 displayHigh(i2cBus, 1);   // Kontakt 9-16
#else
    // RP2040/ESP32-C3: ein MCP23S17 auf SPI deckt beide Byte ab
    MbcMcp23x17 display(pinCsMcp23s17Anzeige);
#endif

MbcUtils      utils(20, pinAdcVspg, pinAdcPlug, pinLedGruen, pinLedRot, pinOneWire, 18);
MbcCan        can(pinCsMbcan, utils);
// backend-Default = EEPROM_24LC1024 (ATmega644P); RP2040/ESP32-C3 uebergeben explizit
// MbcBootStorageBackend::FLASH_W25Q64 als zweiten Parameter (siehe Sketch)
MbcBootStorage bootStorage(pinCsBootStorage);
#if defined(__AVR__)
    Mbc90Com  com(utils, can, bootStorage, inputs, displayLow, displayHigh, pinReset);
#else
    Mbc90Com  com(utils, can, bootStorage, inputs, display);
#endif

void setup() {
    SPI.begin();
    utils.begin(artikelnummer, swVersion, hwVersion);
    can.begin();
    bootStorage.begin();
    com.begin();
    com.beginModule();
    utils.waitFilled();   // blinkt ca. 10 s ORANGE, bis Vspg/Plug/Temperatur einmal ermittelt sind
}

void loop() {
    utils.update();
    utils.check();
    com.update();   // Kontakte einlesen/entprellen, Stellungsanzeige, CAN-Auswertung
}
```

Zwei Konstruktoraufrufe genügen dem Sketch: `MbcUtils` bekommt bei mbc-90 zusätzlich
`genPurpSize=20` und `extraEepromSize=18` (S88Addr, Rückmeldestatus, 16×Integrationszeit — siehe
`Mbc90Com.cpp`), `com.beginModule()` erledigt den kompletten Startablauf, `com.update()` je
`loop()`-Durchlauf die komplette Laufzeitlogik.

## Abhängigkeiten

- [MBCANLib](../MBCANLib)

## Copyright

MBCAN (c) 2007–2026 by Dr.-Ing. Thomas Wiesner
