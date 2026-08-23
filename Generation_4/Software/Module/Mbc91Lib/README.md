# Mbc91Lib

Modulspezifische Erweiterung von [MBCANLib](../MBCANLib) für **mbc-91**, einen
Magnetartikel-Schaltdecoder mit lokalem Stellpult — ein Zubehörmodul mit eigenen Schalt-I/Os an
nur einem CAN-Kanal (identisch zur CS2-/MBCAN-Anbindung anderer Zubehörmodule).

## Was mbc-91 tut

- **8 Relaisausgänge** (4 Doppelspulenantriebe) steuern Magnetartikel (Weichen/Signale) gepulst an
  — Impulszeit pro Antrieb konfigurierbar, mit Zwangsabschaltung als Sicherheitsnetz gegen nicht
  endabgeschaltete Magnetartikel.
- **8 Stellpult-Eingänge** — lokale Taster, die direkt am Modul denselben Schaltvorgang wie ein
  CS2-Kommando auslösen.
- **Positionsanzeige** — 8 Stellungs-LEDs plus ein Bestückungs-Konfigurationsnibble über einen
  `MbcMcp23x17`-Expander (SPI/MCP23S17).
- Zusätzlich ein Rückmeldezyklus (Zwangsrückmeldung auf Anforderung eines vorgelagerten LinkS88)
  und die übliche `MbcCom`-Modulidentitäts-/Parametrierkommunikation — mbc-91 braucht dafür keinen
  der modulspezifischen `MbcCom`-Hooks (dynamische CS2-Kennungsvergabe wie ein generisches Modul).

## Installation

1. [MBCANLib](../MBCANLib) installieren (Abhängigkeit, siehe `depends=MBCANLib` in
   `library.properties`).
2. Diesen Ordner (`Mbc91Lib`) nach `<Arduino-Sketchbook>/libraries/Mbc91Lib` kopieren.
3. Arduino-IDE neu starten — die fünf Beispiel-Sketches erscheinen unter
   *Datei → Beispiele → Mbc91Lib*.

## Beispiel-Sketches

Ein vollständiger, direkt flashbarer Sketch je Zielboard. Die Kern-Pins (SPI, CAN, Boot-Speicher,
1-Wire, ADC-Busspannung) sind je Board konsistent gewählt; LED grün/rot und die
Steckernetzteil-Erkennung können je nach Board abweichen (eigene Pinwahl bzw. eigener Expander,
siehe Kopfkommentar des jeweiligen Sketches). Wo mbc-91 mehr I/O braucht, als nativ frei ist,
kommt ein zusätzlicher MCP23S17 dazu:

| Sketch | Board | Relais/Stellpult | Positionsanzeige |
|---|---|---|---|
| `Mbc91_ATmega644P_HW161002` | ATmega644P (MightyCore) | nativ, kein Stellpult (siehe unten) | 8 native GPIO-LEDs (kein Expander) |
| `Mbc91_ATmega644P_HW240401` | ATmega644P (MightyCore) | nativ | eigener MCP23S17 |
| `Mbc91_RP2040_Pico` | Raspberry Pi Pico | nativ (genug freie GPIOs) | eigener MCP23S17 |
| `Mbc91_RP2040_Zero` | Waveshare RP2040-Zero | zusätzlicher MCP23S17 | teilt sich den ohnehin vorhandenen LED-/Plug-MCP23S17 |
| `Mbc91_ESP32C3` | ESP32-C3 | zusätzlicher MCP23S17 | teilt sich den ohnehin vorhandenen LED-/Plug-MCP23S17 |

Bei RP2040 Zero und ESP32-C3 spart das gemeinsame Nutzen des LED-/Plug-Expanders für die
Positionsanzeige einen sonst nötigen dritten Baustein samt CS-Pin.

**`Mbc91_ATmega644P_HW161002` ist eine ÄLTERE Hardware-Generation** (vor der Einführung von
I2C/Stellpult) und nutzt einen eigenen zweiten `Mbc91Com`-Konstruktor ohne
`indicator`-/`stellpult`-Parameter: 8 direkte GPIO-Positions-LEDs statt Expander, und **kein
Stellpult** — stattdessen liest das Modul einmalig beim Start 4 direkte GPIO-„Bestückungs“-Jumper,
um pro Doppelspulenantrieb die Betriebsart (Impuls/Wechsel) automatisch festzulegen (gleiches
Bestückungs-Konzept wie bei der neueren Hardware, nur über native Pins statt über den
Indicator-Expander bezogen). Siehe `Mbc91Com.h`-Klassenkommentar für Details zu beiden
Konstruktoren.

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
#include <Mbc91Com.h>

// 8 Relais-/8 Stellpult-Objekte (MbcDigitalOut/In - natives GPIO oder MCP23x17-Bit je nach Board)
// Index-Konvention: 0..7 = REL_1_L,REL_1_R,REL_2_L,REL_2_R,... bzw. IN_1_G,IN_1_R,IN_2_G,...
MbcDigitalOut *relais[8]    = { ... };
MbcDigitalIn  *stellpult[8] = { ... };

MbcUtils      utils(10, pinAdcVspg, pinAdcPlug, pinLedGruen, pinLedRot, pinOneWire, 9);
MbcCan        can(pinCsMbcan, utils);
// backend-Default = EEPROM_24LC1024 (ATmega644P); RP2040/ESP32-C3 uebergeben explizit
// MbcBootStorageBackend::FLASH_W25Q64 als zweiten Parameter (siehe Sketch)
MbcBootStorage bootStorage(pinCsBootStorage);
MbcMcp23x17   indicator(pinCsIndicator);   // oder derselbe Expander wie LED/Plug, siehe Sketch
Mbc91Com      com(utils, can, bootStorage, indicator, relais, stellpult /*, pinReset auf AVR */);

void setup() {
    SPI.begin();
    indicator.begin(0xfc, 0x00, 0x00, 0x00);   // GPIOA Bit 3-6 Bestueckung, GPIOB 8 Stellungs-LEDs
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
    com.update();   // Puls-Abschaltung, Zwangsrueckmeldezyklus, Stellpult-Abfrage, CAN-Auswertung
}
```

Zwei Konstruktoraufrufe genügen dem Sketch: `MbcUtils` bekommt bei mbc-91 zusätzlich
`genPurpSize=10` und `extraEepromSize=9` (Impulszeiten, Aktorstellung, Basisadresse, Auto-Aus,
Zwangsrückmeldung-Flag — siehe `Mbc91Com.cpp`), `com.beginModule()` erledigt den kompletten
Startablauf, `com.update()` je `loop()`-Durchlauf die komplette Laufzeitlogik.

## Abhängigkeiten

- [MBCANLib](../MBCANLib)

## Copyright

MBCAN (c) 2007–2026 by Dr.-Ing. Thomas Wiesner
