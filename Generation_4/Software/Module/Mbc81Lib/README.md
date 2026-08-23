# Mbc81Lib

Modulspezifische Erweiterung von [MBCANLib](../MBCANLib) für **mbc-81**, ein dediziertes
Stellpult mit 32 Tastern (16 Adresspaare Rot/Grün) und 16 Stellungs-LEDs. mbc-81 hat 1
CS2-Konfigkanal (Label „KeyB:", Modulnummer 0...63, siehe unten) - die übrige Parametrierung
(Nutzung) läuft weiterhin über den rohen `PC_ARRAY_DATA`-Zugriff auf das Systemarray.

## Was mbc-81 tut

Umschaltbar über das Feld „Nutzung" (`cmb_mbc81_Nutzung` im Parametriercenter, 3 Einträge:
„Switchboard", „Bus 0 Matrix 1...32", „Bus 0 Matrix 33...64") zwischen zwei grundverschiedenen
Betriebsarten:

- **Nutzung=0 ("Switchboard"):** das Modul ist selbst die Schaltquelle. Ein Tasterdruck sendet
  einen echten Motorola-Schaltbefehl für die konfigurierte MM-Basisadresse (16 Adressen im Bereich
  `MM-Addr...MM-Addr+15`) und schaltet gleichzeitig die eigene LED des betroffenen Ausgangs (Echo
  dessen, was gerade gesendet wurde - keine externe Rückmeldung). Eingehende Schaltbefehle werden
  in diesem Modus NICHT auf den LEDs gespiegelt.
- **Nutzung=1/2 ("Bus 0 Matrix 1...32"/"33...64"):** das Modul ist ein passiver
  Rückmelder/Statusspiegel. Ein Tasterdruck sendet KEINEN Schaltbefehl, sondern eine
  S88-Ereignismeldung (Pinnummer `101 + (Nutzung-1)*32 + Index`, unabhängig von der MM-Adresse).
  Gleichzeitig werden eingehende Schaltbefehle im konfigurierten Adressbereich auf den LEDs
  gespiegelt (passive Statusanzeige neben einem "echten" Stellpult/einer Zentrale). Eine explizite
  CS2-Statusabfrage antwortet mit dem echten, zuletzt erkannten Tasterzustand statt pauschal
  "Kontakt frei" zu melden.

## CS2-Konfigkanal „KeyB"

Die MM-Basisadresse ist zusätzlich zum `PC_ARRAY_DATA`-Weg direkt aus der CSx-GUI setzbar (Label
„KeyB:", Rohwert 0...63). Die CSx-GUI kann keine Hex-Eingabe, daher wird nicht die Basisadresse
selbst übertragen, sondern eine einfache Modulnummer, die intern in `0x3000 + 16*Modulnummer`
(0x3000...0x33F0) umgerechnet wird - mbc-81 belegt 16 MM-Adressen je Modul
(`MM-Addr...MM-Addr+15`), daher Faktor 16 statt eines kleineren Werts.

## Taster-Einlesen und Entprellung

32 Taster über 4x PCF8574 (I2C-Portexpander, Bausteinadressen 0...3, je 8 Bit = 4 Adresspaare
Rot/Grün). Der PCB-Pin-Remap (Bit-Reihenfolge innerhalb eines PCF8574-Byte auf die 4 Adresspaare)
ist reale Verdrahtung. Die Entprellung erfolgt über einen millis()-basierten, nicht-blockierenden
Ersatz mit festem Wert, da mbc-81 kein CS2-Konfigkanal für eine einstellbare Zeit hat.

## Installation

1. [MBCANLib](../MBCANLib) installieren (Abhängigkeit, siehe `depends=MBCANLib` in
   `library.properties`).
2. Diesen Ordner (`Mbc81Lib`) nach `<Arduino-Sketchbook>/libraries/Mbc81Lib` kopieren.
3. Arduino-IDE neu starten - der Beispiel-Sketch erscheint unter
   *Datei → Beispiele → Mbc81Lib*.

## Beispiel-Sketches

| Sketch | Board | Taster/LEDs |
|---|---|---|
| `Mbc81_ATmega644P_HW240106` | ATmega644P (MightyCore) | native LED-Pins, 4x PCF8574 (I2C) für die Tastermatrix, feste Pinbelegung (Platinenleiterbahnen) |
| `Mbc81_ATmega644P_HW180212` | ATmega644P (MightyCore) | ältere Hardware-Revision, Pinbelegung/Logik identisch zu HW240106 (nur ArtNum4 abweichend) |
| `Mbc81_RP2040_Pico` | Raspberry Pi Pico | native LED-Pins (genug Pins frei), 4x PCF8574 (I2C) für die Tastermatrix |
| `Mbc81_RP2040_Zero` | Waveshare RP2040-Zero | 16 LEDs an einem MCP23017 (GPIOA/GPIOB), LED grün/rot + Steckernetzteil-Erkennung an einem zweiten MCP23017, 4x PCF8574 - alle 3 Bausteine am selben I2C-Bus |
| `Mbc81_ESP32C3` | ESP32-C3 | wie RP2040-Zero (pinknapp, interne TWAI statt MCP2515) |

`Mbc81Com` selbst ist plattformneutral (I2C-PCF8574 und natives GPIO funktionieren unverändert auf
allen 3 Zielplattformen) - die Unterschiede zwischen den Sketches stecken ausschließlich in der
Pin-/Expander-Zuordnung, nicht in der Bibliothek.

**Wichtig:** Die exakten Pin-Zuordnungen und alle ANNAHME-Vermerke stehen im Kopfkommentar jedes
Sketches - vor dem Verdrahten/Flashen dort nachsehen, nicht hier.

**Wichtig (Bootloader, nur ATmega644P):** Vor dem ersten Einsatz muss der MBCAN-Bootloader einmalig
über ISP geflasht werden (`Bootloaderinfos_MightyCore/` im Sketch-Ordner), siehe Kopfkommentar des
Sketches.

**Wichtig (OTA, nur RP2040/ESP32-C3):** Beide Cores benötigen eine reservierte zweite Flash-
Partition für Firmware-Updates (siehe `MbcOta.h`-Kopfkommentar). Beim ESP32-C3 über die
mitgelieferte `partitions.csv` im Sketch-Ordner (*Tools/Werkzeuge → Partition Scheme → Custom*);
beim RP2040 dagegen **ohne** CSV-Datei ausschließlich über *Tools/Werkzeuge → Flash Size* (eine
Variante mit „OTA" im Namen wählen).

## Minimalbeispiel (Kern-API)

```cpp
static uint8_t sysArray[MBC_SYSARRAY_SIZE(3)];

MbcUtils       utils(sysArray, 3, pinAdcVspg, pinAdcPlug, pinLedGruen, pinLedRot, pinOneWire, 3);
MbcCan         can(pinCsMbcan, utils);
MbcBootStorage bootStorage(pinCsBootStorage);

MbcI2cBus   i2cBus;
MbcPcf8574  *tasterChips[4] = { &chip0, &chip1, &chip2, &chip3 };
MbcDigitalOut *leds[16]     = { &led00, /* ... */ &led15 };

Mbc81Com com(utils, can, bootStorage, tasterChips, leds /*, pinReset auf AVR */);

void setup() {
    SPI.begin();
    Wire.begin();
    i2cBus.begin();
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
    com.update();   // Taster einlesen/entprellen, Schaltbefehle/S88-Meldungen, CAN-Auswertung
}
```

`genPurpSize` ist `3` (MM-Adresse H/L, Nutzung), `extraEepromSize` ist `3` - siehe `EEX_81_*`-
Konstanten in `Mbc81Com.cpp`. `com.beginModule()` erledigt den kompletten Startablauf (LEDs
initialisieren, EEPROM laden), `com.update()` je `loop()`-Durchlauf die komplette Laufzeitlogik.

## Abhängigkeiten

- [MBCANLib](../MBCANLib)

## Copyright

MBCAN (c) 2007–2026 by Dr.-Ing. Thomas Wiesner
