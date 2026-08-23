# Mbc92Lib

Modulspezifische Erweiterung von [MBCANLib](../MBCANLib) für **mbc-92**, einen
Magnetartikel-Schaltdecoder mit lokalem Stellpult und einer pro Doppelspulenantrieb
konfigurierbaren Ausgangsart.

## Was mbc-92 tut

- **8 Relaisausgänge** (4 Doppelspulenantriebe) steuern Magnetartikel (Weichen/Signale) an —
  Impulszeit pro Antrieb konfigurierbar, mit Zwangsabschaltung als Sicherheitsnetz gegen nicht
  endabgeschaltete Magnetartikel. Beim Schalten werden **immer beide** Ausgänge eines Paars
  gleichzeitig angesteuert (Zielseite aktiv, Gegenseite gehalten) — passend zur pro Paar
  wählbaren **Ausgangsart** (`Art der Aktoren`, EEPROM-persistiert, per CS2-Konfigkanal
  einstellbar): *Impulsausgang* (nach Ablauf der Impulszeit werden beide Seiten wieder
  deaktiviert) oder *Wechselschalter* (die zuletzt geschaltete Seite bleibt dauerhaft aktiv).
- **8 Stellpult-Eingänge** — lokale Taster, die direkt am Modul denselben Schaltvorgang wie ein
  CS2-Kommando auslösen.
- **4 Stellungsanzeige-LEDs** — mbc-92 hat keine Positionsanzeige-Hardware und keine
  Bestückungserkennung; stattdessen eine LED je Doppelspulenantrieb, direkt am jeweiligen Board.
- Zusätzlich ein Rückmeldezyklus (Zwangsrückmeldung auf Anforderung eines vorgelagerten LinkS88)
  und die übliche `MbcCom`-Modulidentitäts-/Parametrierkommunikation — mbc-92 braucht dafür keinen
  der modulspezifischen `MbcCom`-Hooks (dynamische CS2-Kennungsvergabe wie ein generisches Modul).

## Installation

1. [MBCANLib](../MBCANLib) installieren (Abhängigkeit, siehe `depends=MBCANLib` in
   `library.properties`).
2. Diesen Ordner (`Mbc92Lib`) nach `<Arduino-Sketchbook>/libraries/Mbc92Lib` kopieren.
3. Arduino-IDE neu starten — die vier Beispiel-Sketches erscheinen unter
   *Datei → Beispiele → Mbc92Lib*.

## Beispiel-Sketches

Ein vollständiger, direkt flashbarer Sketch je Zielboard. Die Kern-Pins (SPI, CAN, Boot-Speicher,
1-Wire, ADC-Busspannung, Status-LEDs) folgen überall der üblichen Standard-Pinbelegung für die
jeweilige Plattform. Ohne Bestückungserkennung und mit nur 4 Stellungs-LEDs fällt bei mbc-92 pro
Board mindestens ein Expander weg bzw. wird kleiner besetzt:

| Sketch | Board | Relais/Stellpult | LED grün/rot, Plug, 4 Stellungsanzeige-LEDs |
|---|---|---|---|
| `Mbc92_ATmega644P_HW240720` | ATmega644P (MightyCore) | nativ, feste Pinbelegung (Platinenleiterbahnen) | nativ |
| `Mbc92_RP2040_Pico` | Raspberry Pi Pico | nativ (genug freie GPIOs) | LED grün/rot + Plug nativ, 4 Stellungsanzeige-LEDs über eigenen MCP23S17 |
| `Mbc92_RP2040_Zero` | Waveshare RP2040-Zero | eigener MCP23S17 | eigener MCP23S17 (nur noch 7 statt 15 Signale) |
| `Mbc92_ESP32C3` | ESP32-C3 | eigener MCP23S17 | eigener MCP23S17 (nur noch 7 statt 15 Signale) |

**Wichtig:** Die exakten Pin-Zuordnungen und alle ANNAHME-Vermerke stehen im Kopfkommentar jedes
Sketches — vor dem Verdrahten/Flashen dort nachsehen, nicht hier.

**Wichtig (LED-Polarität, alle Boards):** Ob eine gesetzte Stellungsanzeige-LED elektrisch HIGH
oder LOW aktiv ist, ist hardwareseitig noch nicht verifiziert — `stateDisplay()` nimmt keine
Polarität vorweg. Nach dem ersten Hardwaretest ggf. Rückmeldung geben, ob die Anzeige invertiert
wirkt.

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
#include <Mbc92Com.h>

// 4 Stellungsanzeige-LEDs, 8 Relais-/8 Stellpult-Objekte (natives GPIO oder MCP23x17-Bit je nach
// Board). Index-Konvention: anzeige 0..3 = Doppelspulenantrieb 1..4, relais/stellpult 0..7 =
// REL_1_L,REL_1_R,REL_2_L,REL_2_R,... bzw. IN_1_G,IN_1_R,IN_2_G,...
MbcDigitalOut *anzeige[4]   = { ... };
MbcDigitalOut *relais[8]    = { ... };
MbcDigitalIn  *stellpult[8] = { ... };

MbcUtils      utils(10, pinAdcVspg, pinAdcPlug, pinLedGruen, pinLedRot, pinOneWire, 10);
MbcCan        can(pinCsMbcan, utils);
// backend-Default = EEPROM_24LC1024 (ATmega644P); RP2040/ESP32-C3 uebergeben explizit
// MbcBootStorageBackend::FLASH_W25Q64 als zweiten Parameter (siehe Sketch)
MbcBootStorage bootStorage(pinCsBootStorage);
Mbc92Com      com(utils, can, bootStorage, anzeige, relais, stellpult /*, pinReset auf AVR */);

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
    com.update();   // Puls-Abschaltung, Zwangsrueckmeldezyklus, Stellpult-Abfrage, CAN-Auswertung
}
```

`MbcUtils` bekommt bei mbc-92 zusätzlich `genPurpSize=10` und `extraEepromSize=10` (Impulszeiten,
Aktorstellung, Basisadresse, Auto-Aus, Zwangsrückmeldung-Flag, Art der Aktoren — siehe
`Mbc92Com.cpp`), `com.beginModule()` erledigt den kompletten Startablauf, `com.update()` je
`loop()`-Durchlauf die komplette Laufzeitlogik.

## Abhängigkeiten

- [MBCANLib](../MBCANLib)

## Copyright

MBCAN (c) 2007–2026 by Dr.-Ing. Thomas Wiesner
