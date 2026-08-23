# Mbc80Lib

Modulspezifische Erweiterung von [MBCANLib](../MBCANLib) für **mbc-80**, ein reines
Gateway-Modul: es hat keine eigenen Schalt-/Rückmelde-I/Os, sondern hängt an drei
Kommunikationswegen und leitet CAN-Nachrichten zwischen ihnen weiter.

## Was mbc-80 tut

- **Modulinterner MBCAN-Bus** — eigener CAN-Kanal (MCP2515), das interne CAN-Netz der übrigen
  MBCAN-Zubehörmodule.
- **Echter Märklin-CS2-Bus** — zweiter CAN-Kanal, gleichzeitig auch der Kanal, über den die normale
  `MbcCom`-Modulidentitäts-/Parametrierkommunikation läuft.
- **Austauschbare Nachrichten-Brücke** (`MbcMsgBridge`-Interface) — `MbcWlanBridge` für ein externes
  WLAN-Modul per UART, oder `MbcWifiBridge` für natives WLAN auf dem ESP32; welche Implementierung
  verwendet wird, entscheidet allein der Sketch.

Dabei simuliert das Modul ein Märklin-L88-Gerät (feste CS2-Kennung `0x0040`) inklusive
S88-Automatik-Kenner-Anfrage/-Verwaltung.

## Installation

1. [MBCANLib](../MBCANLib) installieren (Abhängigkeit, siehe `depends=MBCANLib` in
   `library.properties`).
2. Diesen Ordner (`Mbc80Lib`) nach `<Arduino-Sketchbook>/libraries/Mbc80Lib` kopieren.
3. Arduino-IDE neu starten — die vier Beispiel-Sketches erscheinen unter
   *Datei → Beispiele → Mbc80Lib*.

## Beispiel-Sketches

Ein vollständiger, direkt flashbarer Sketch je Zielboard — jeder mit eigener Pin-Tabelle und den
board-spezifischen Annahmen im Kopfkommentar:

| Sketch | Board | Besonderheit |
|---|---|---|
| `Mbc80_ATmega644P_HW211001` | ATmega644P (MightyCore) | feste Pinbelegung (reale Hardware, keine freie Pinwahl) |
| `Mbc80_RP2040_Pico` | Raspberry Pi Pico | freie Pinwahl, alles nativ (genug GPIOs) |
| `Mbc80_RP2040_Zero` | Waveshare RP2040-Zero | freie Pinwahl, alles nativ (genug GPIOs) |
| `Mbc80_ESP32C3` | ESP32-C3 | CS2-Bus über die interne TWAI, MBCAN-Bus weiterhin über externen MCP2515; LED/Plug ebenfalls über MCP23S17 |

Alle vier nutzen `MbcWlanBridge` (UART zu einem externen WLAN-Modul) außer dem ESP32-C3-Sketch, der
stattdessen `MbcWifiBridge` (natives WLAN/TCP inkl. Setup-Webportal) einsetzt.

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
#include <Mbc80Com.h>

MbcUtils      utils(0, pinAdcVspg, pinAdcPlug, pinLedGruen, pinLedRot, pinOneWire);
MbcCan        canCs2(pinCsCs2, utils);
MbcCan        canMbcan(pinCsMbcan, utils);
// backend-Default = EEPROM_24LC1024 (ATmega644P); RP2040/ESP32-C3 uebergeben explizit
// MbcBootStorageBackend::FLASH_W25Q64 als zweiten Parameter (siehe Sketch)
MbcBootStorage bootStorage(pinCsBootStorage);
MbcWlanBridge wlan(Serial1);                 // oder MbcWifiBridge auf ESP32
Mbc80Com      com(utils, canCs2, canMbcan, bootStorage, wlan /*, pinReset auf AVR */);

void setup() {
    SPI.begin();
    utils.begin(artikelnummer, swVersion, hwVersion);
    canCs2.begin();
    canMbcan.begin();
    bootStorage.begin();
    com.begin();
    com.beginModule();
    utils.waitFilled();   // blinkt ca. 10 s ORANGE, bis Vspg/Plug/Temperatur einmal ermittelt sind
}

void loop() {
    utils.update();
    utils.check();
    com.bridge();   // uebernimmt die komplette MBCAN<->CS2<->WLAN-Weiterleitung
}
```

`Mbc80Com::bridge()` kapselt die gesamte Weiterleitungslogik (alle drei Kommunikationswege
abfragen, S88-Automatik-Kenner verwalten, zyklische Basisadress-Rückmeldung) — der Sketch muss sie
nur einmal je `loop()`-Durchlauf aufrufen.

## Abhängigkeiten

- [MBCANLib](../MBCANLib)

## Copyright

MBCAN (c) 2007–2026 by Dr.-Ing. Thomas Wiesner
