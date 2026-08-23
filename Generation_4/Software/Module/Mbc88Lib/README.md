# Mbc88Lib

Modulspezifische Erweiterung von [MBCANLib](../MBCANLib) für **mbc-88**, einen
S88-Rückmeldedecoder: ein "echtes" Zubehörmodul mit eigenen I/Os an nur einem CAN-Kanal (identisch
zur CS2-/MBCAN-Anbindung anderer Zubehörmodule), kein Gateway.

## Was mbc-88 tut

- **8 Kontakteingänge** — aktiv-LOW mit PullUp (gepulste 0-Leitung), je Pin ein Integrator als
  Entpreller/Haltezeit-Ersatz: ein Kontaktwechsel wird erst nach Ablauf einer konfigurierbaren,
  pro Kontakt einstellbaren Integrationszeit als endgültiges Ereignis gemeldet.
- **8 Status-LED-Ausgänge** — spiegeln pro Kontakt, ob aktuell ein Ereignis ansteht (Belegtmelder-
  Anzeige: an, wenn der Kontakt ausgelöst ist, aus im Ruhezustand; reine Anzeige, keine steuernde
  Funktion).
- **Halbmodul-Konzept** — ein Modul bedient wahlweise die Kontakte 1-8 oder 9-16 einer 16er-
  Blockadresse (Parameter "Halbmodul", identisch zur Terminologie im Parametriercenter).
- **CS2-S88-Anfragebeantwortung** (CAN-ID `0x22`/`0x23`) sowie ein Broadcast an das
  mbc80-PC-Interface bei jedem Kontaktwechsel — beide Nachrichtentemplates sind bereits in der
  `MbcCom`-Basisklasse vorgefüllt.
- Die übliche `MbcCom`-Modulidentitäts-/Parametrierkommunikation — mbc-88 braucht dafür keinen der
  modulspezifischen `MbcCom`-Hooks (dynamische CS2-Kennungsvergabe wie ein generisches Modul).

## Installation

1. [MBCANLib](../MBCANLib) installieren (Abhängigkeit, siehe `depends=MBCANLib` in
   `library.properties`).
2. Diesen Ordner (`Mbc88Lib`) nach `<Arduino-Sketchbook>/libraries/Mbc88Lib` kopieren.
3. Arduino-IDE neu starten — die fünf Beispiel-Sketches erscheinen unter
   *Datei → Beispiele → Mbc88Lib*.

## Beispiel-Sketches

Ein vollständiger, direkt flashbarer Sketch je Zielboard. Die Kern-Pins (SPI, CAN, Boot-Speicher,
1-Wire, ADC-Busspannung) sind je Board einheitlich gewählt; LED grün/rot und die
Steckernetzteil-Erkennung können je nach Board abweichen (eigene Pinwahl bzw. eigener Expander,
siehe Kopfkommentar des jeweiligen Sketches). Wo mbc-88 mehr I/O braucht, als nativ frei ist, kommt
ein zusätzlicher MCP23S17 dazu:

| Sketch | Board | Kontakte/Status-LEDs |
|---|---|---|
| `Mbc88_ATmega644P_HW180329` | ATmega644P (MightyCore) | nativ (genug freie Pins) |
| `Mbc88_ATmega644P_HW240106` | ATmega644P (MightyCore) | nativ (genug freie Pins) |
| `Mbc88_RP2040_Pico` | Raspberry Pi Pico | nativ (genug freie Pins) |
| `Mbc88_RP2040_Zero` | Waveshare RP2040-Zero | zusätzlicher MCP23S17 |
| `Mbc88_ESP32C3` | ESP32-C3 | zusätzlicher MCP23S17 |

`Mbc88_ATmega644P_HW180329` ist eine ältere Hardware-Generation. Pinbelegung UND Ablauflogik sind
dabei Byte-für-Byte identisch zu `Mbc88_ATmega644P_HW240106` — beide Revisionen teilen dieselbe
Platinenbelegung, nur Hardware-Datum/ArtNum4/HW_VER unterscheiden sich (kein Klassen-Unterschied
nötig).

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
#include <Mbc88Com.h>

// 8 Eingangs-/8 Ausgangs-Objekte (MbcDigitalIn/Out - natives GPIO oder MCP23x17-Bit je nach Board)
// Index-Konvention: 0..7 = S88-Kontakt 1..8 des per Halbmodul-Parameter gewaehlten 8er-Bereichs
MbcDigitalIn  *inputs[8]  = { ... };
MbcDigitalOut *outputs[8] = { ... };

MbcUtils      utils(12, pinAdcVspg, pinAdcPlug, pinLedGruen, pinLedRot, pinOneWire, 11);
MbcCan        can(pinCsMbcan, utils);
// backend-Default = EEPROM_24LC1024 (ATmega644P); RP2040/ESP32-C3 uebergeben explizit
// MbcBootStorageBackend::FLASH_W25Q64 als zweiten Parameter (siehe Sketch)
MbcBootStorage bootStorage(pinCsBootStorage);
Mbc88Com      com(utils, can, bootStorage, inputs, outputs /*, pinReset auf AVR */);

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
    com.update();   // Kontakte einlesen/entprellen, Status-LEDs, CAN-Auswertung
}
```

Zwei Konstruktoraufrufe genügen dem Sketch: `MbcUtils` bekommt bei mbc-88 zusätzlich
`genPurpSize=12` und `extraEepromSize=11` (S88Addr, 8×Integrationszeit, Halbmodul — siehe
`Mbc88Com.cpp`), `com.beginModule()` erledigt den kompletten Startablauf, `com.update()` je
`loop()`-Durchlauf die komplette Laufzeitlogik.

## Abhängigkeiten

- [MBCANLib](../MBCANLib)

## Copyright

MBCAN (c) 2007–2026 by Dr.-Ing. Thomas Wiesner
