# Mbc82Lib

Modulspezifische Erweiterung von [MBCANLib](../MBCANLib) für **mbc-82**, einen Monitor-/
Steuermodul für einen externen, analogen Booster (mfx-fähig). mbc-82 hat keinen eigenen
Schaltdecoder - es hat 2 modulspezifische CS2-Messwertkanäle (ON Track, Kurzschluss) und 3
CS2-Konfigkanäle (SW Major, SW Minor, Kurzschluss-Latenzzeit); Softwareversion und
Kurzschluss-Latenzzeit sind zusätzlich über `PC_ARRAY_DATA` erreichbar. Baugleiche Basis wie
[Mbc93Lib](../Mbc93Lib) (das zweite Booster-Modul dieses Projekts), aber **ohne Lüfter**.

## Was mbc-82 tut

- **2 Ausgangsbits** steuern den Booster an (Ein/Aus + Kurzschluss-Signalisierung, 3 gültige
  Zustände: aus, an, aus-mit-Kurzschluss).
- **1 Kurzschluss-Erkennungseingang**, latenzgemessen: eine steigende Flanke startet die
  Latenzzeit, eine fallende Flanke vor Ablauf bricht sie ab - der kurze mfx-Quasi-Kurzschluss,
  der beim normalen Rückmeldezyklus durchfährt, wird so nicht fälschlich als echter Kurzschluss
  gewertet. Mindestlatenz 48 ms, einstellbar in 48-ms-Schritten über den CS2-Konfigkanal
  „KS x 50 ms" (GUI-Anzeige 1...20, real 48...960 ms).
- **Kein Lüfter** - im Unterschied zu Mbc93Lib hat mbc-82 keinen Lüfterausgang.
- **Eine simulierte GFP/Booster-Softwareversion** (Default 3.81) verhindert eine
  Updateanforderung der CSx, analog zu [Mbc80Lib](../Mbc80Lib) für L88 - einstellbar über die
  CS2-Konfigkanäle „SW Major:"/„SW Minor:" (je 1...255).
- **Eine feste CS2-Gerätekennung 0x0000** (Märklin-Vorgabe für Booster-Geräte,
  `kennerFixedExtern()`) - im Original-Quellcode als Kommentar dokumentiert, aber durch eine
  überschriebene Zuweisung real auf 0x0020 kompiliert; der Port verwendet bewusst den
  dokumentierten, beabsichtigten Wert 0x0000 (dieselbe Entscheidung wie bei mbc-93).
- **System-Stopp/Go direkt verarbeiten UND selbst auslösen:** mbc-82 reagiert auf eingehende
  System-Stopp-/Go-Telegramme (CAN-ID 0x00) und sendet bei einem echten, physischen Kurzschluss
  selbst ein spontanes System-Stopp-Telegramm mit universeller Broadcast-GUiD - hält damit bei
  einem Kurzschluss die gesamte Anlage an, nicht nur den eigenen Booster.

## Wichtig: 1:1-Portierung, keine eigenen Korrekturen an der CAN-Logik

`handleSystemCommand()` ist bewusst 1:1 aus der Original-Firmware (`main.c`) übernommen -
inklusive eines auffälligen Vergleichs in der modulbezogenen GUiD-Prüfung (D2 wird gegen
dieselbe GUiD-Stelle wie D1 verglichen statt gegen die dritte, D3 gegen die dritte statt gegen
die vierte - die vierte GUiD-Stelle wird dadurch nie geprüft). Eine eigenständige Variante des
bei mbc-93 gefundenen, ähnlich falschen Vergleichs. Gegen die offizielle CS2-Protokolldokumentation
geprüft: die universelle Broadcast-Form (D0-D3 alle 0), über die CS2 System-Stopp/Go in der
Praxis überwiegend sendet, ist davon nicht betroffen. Dieses Projekt ändert Sicherheitslogik in
gefertigter Original-Firmware nicht ungefragt - siehe Klassenkommentar in `Mbc82Com.cpp`.

## Härtung gegenüber dem Original: KS-Latenz-Konfigkanal

Die Original-Firmware validiert den Kurzschluss-Latenz-Konfigkanal überhaupt nicht
(`MBC_82_SH = msg[D7] - 1;` ohne Bereichscheck, nur das untere Byte statt der vollen 16-Bit-
Kombination, Bestätigung meldet immer „gültig"). Analog zur bereits für mbc-93 vorgenommenen
Härtung prüft der Port den Rohwert gegen den auch im ParaCenter geltenden Bereich 1...20,
kombiniert `msg[D6]:msg[D7]` zu 16 Bit und quittiert einen Wert außerhalb per FALSE (siehe
`cs2IndexParameterExtern()` in `Mbc82Com.cpp`).

## Kurzschluss-Latenzmessung: `attachInterrupt()` statt INT1+TIMER1

Die Original-Firmware misst die Latenzzeit über eine rohe INT1-Interruptroutine plus einen
TIMER1-Overflow. Der Port ersetzt das durch `attachInterrupt(CHANGE)` plus eine
`millis()`-Zeitstempelmessung (siehe `shortIsr()`/`shortCheck()` in `Mbc82Com.cpp`) - funktional
identisch, ohne Timer1 zu belegen.

## Installation

1. [MBCANLib](../MBCANLib) installieren (Abhängigkeit, siehe `depends=MBCANLib` in
   `library.properties`).
2. Diesen Ordner (`Mbc82Lib`) nach `<Arduino-Sketchbook>/libraries/Mbc82Lib` kopieren.
3. Arduino-IDE neu starten - der Beispiel-Sketch erscheint unter
   *Datei → Beispiele → Mbc82Lib*.

## Beispiel-Sketches

| Sketch | Board | Anmerkung |
|---|---|---|
| `Mbc82_ATmega644P_HW210301` | ATmega644P (MightyCore) | native Pins, feste Pinbelegung (Platinenleiterbahnen) |
| `Mbc82_RP2040_Pico` | Raspberry Pi Pico | freie Pinwahl, alles native GPIO (kein Expander noetig) |
| `Mbc82_RP2040_Zero` | Waveshare RP2040-Zero | LED/Steckernetzteil-Erkennung ueber I2C-MCP23017, Rest native GPIO |
| `Mbc82_ESP32C3` | ESP32-C3 (native TWAI) | LED/Steckernetzteil-Erkennung ueber SPI-MCP23S17, eigene `partitions.csv` fuer OTA |

`Mbc82Com` selbst ist bis auf die Kurzschluss-Latenzmessung (`attachInterrupt()`) plattformneutral
(sonst nur natives `millis()`/`digitalWrite()`) - für weitere Plattformen wäre lediglich ein
zusätzlicher Sketch nötig, kein Eingriff in die Bibliothek.

**Wichtig:** Die exakte Pin-Zuordnung und alle Anmerkungen (inkl. ANNAHME-Vermerke) stehen im
Kopfkommentar des jeweiligen Sketches - vor dem Verdrahten/Flashen dort nachsehen, nicht hier.

**Wichtig (Bootloader, nur ATmega644P):** Vor dem ersten Einsatz muss der MBCAN-Bootloader einmalig
über ISP geflasht werden (`Bootloaderinfos_MightyCore/` im Sketch-Ordner) - beim
Boardverwalter-Schritt „Variante" **644P/644PA** wählen (siehe Kopfkommentar des Sketches). Die
RP2040-/ESP32-Sketches brauchen keinen separaten Bootloader, nutzen aber ein eigenes
OTA-Partitionsschema - siehe deren Kopfkommentare.

## Minimalbeispiel (Kern-API)

```cpp
static uint8_t sysArray[MBC_SYSARRAY_SIZE(4)];

MbcUtils       utils(sysArray, 4, pinAdcVspg, pinAdcPlug, pinLedGruen, pinLedRot, pinOneWire, 3);
MbcCan         can(pinCsMbcan, utils);
MbcBootStorage bootStorage(pinCsBootStorage);

MbcDigitalOut pinBoosterOn(pinOn), pinBoosterShort(pinShort);

Mbc82Com com(utils, can, bootStorage, pinBoosterOn, pinBoosterShort,
             pinShortDetect /*, pinReset auf AVR */);

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
    com.update();   // Kurzschluss-Latenzzeit, System-Stopp/Go, CAN-Auswertung
}
```

`genPurpSize` ist `4` (STATUS, SW_1, SW_2, SH), `extraEepromSize` ist `3` - siehe `EEX_82_*`-
Konstanten in `Mbc82Com.cpp`. `com.beginModule()` erledigt den kompletten Startablauf
(Ausgangsbit-Pins konfigurieren, Kurzschluss-Interrupt scharfschalten, EEPROM laden),
`com.update()` je `loop()`-Durchlauf die komplette Laufzeitlogik.

## Abhängigkeiten

- [MBCANLib](../MBCANLib)

## Copyright

MBCAN (c) 2007–2026 by Dr.-Ing. Thomas Wiesner
