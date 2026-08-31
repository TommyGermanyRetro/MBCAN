# Mbc93Lib

Modulspezifische Erweiterung von [MBCANLib](../MBCANLib) für **mbc-93**, einen Monitor-/
Steuermodul für einen externen, analogen Booster (mfx-fähig, Märklin-6017-Basis). mbc-93 hat
keinen eigenen Schaltdecoder - es hat 2 modulspezifische CS2-Messwertkanäle (ON Track,
Kurzschluss) und 1 CS2-Konfigkanal (Kurzschluss-Latenzzeit); Softwareversion und
Kurzschluss-Latenzzeit sind zusätzlich über `PC_ARRAY_DATA` erreichbar.

## Was mbc-93 tut

- **2 Ausgangsbits** steuern den Booster an (Ein/Aus + Kurzschluss-Signalisierung, 3 gültige
  Zustände: aus, an, aus-mit-Kurzschluss).
- **1 Kurzschluss-Erkennungseingang**, latenzgemessen: eine steigende Flanke startet die
  Latenzzeit, eine fallende Flanke vor Ablauf bricht sie ab - der kurze mfx-Quasi-Kurzschluss,
  der beim normalen Rückmeldezyklus durchfährt, wird so nicht fälschlich als echter Kurzschluss
  gewertet. Mindestlatenz 48 ms, einstellbar in 48-ms-Schritten über den CS2-Konfigkanal
  „KS x 50 ms" (GUI-Anzeige 1...20, real 48...960 ms).
- **Ein temperaturabhängiger Lüfter** (PWM über den vorhandenen 1-Wire-Sensor): < 20 °C = PWM
  55/255, 20...60 °C linear 55→255, > 60 °C = 255/255.
- **Eine simulierte GFP/Booster-Softwareversion** (Default 3.85) verhindert eine
  Updateanforderung der CSx, analog zu [Mbc80Lib](../Mbc80Lib) für L88 - einstellbar über die
  CS2-Konfigkanäle „SW Major:"/„SW Minor:" (je 1...255).
- **Eine feste CS2-Gerätekennung 0x0000** (Märklin-Vorgabe für Booster-Geräte,
  `kennerFixedExtern()`) - im Original-Quellcode als Kommentar dokumentiert, aber durch eine
  überschriebene Zuweisung real auf 0x0020 kompiliert; der Port verwendet bewusst den
  dokumentierten, beabsichtigten Wert 0x0000.
- **System-Stopp/Go direkt verarbeiten UND selbst auslösen:** mbc-93 reagiert auf eingehende
  System-Stopp-/Go-Telegramme (CAN-ID 0x00) und sendet bei einem echten, physischen Kurzschluss
  selbst ein spontanes System-Stopp-Telegramm mit universeller Broadcast-GUiD - hält damit bei
  einem Kurzschluss die gesamte Anlage an, nicht nur den eigenen Booster.

## Wichtig: 1:1-Portierung, keine eigenen Korrekturen an der CAN-Logik

`handleSystemCommand()` ist bewusst 1:1 aus der Original-Firmware (`main.c`) übernommen -
inklusive eines auffälligen Vergleichs in der modulbezogenen GUiD-Prüfung (D2 wird gegen
dieselbe GUiD-Stelle wie D1 verglichen statt gegen die dritte). Gegen die offizielle
CS2-Protokolldokumentation geprüft: die universelle Broadcast-Form (D0-D3 alle 0), über die CS2
System-Stopp/Go in der Praxis überwiegend sendet, ist davon nicht betroffen. Dieses Projekt
ändert Sicherheitslogik in gefertigter Original-Firmware nicht ungefragt - siehe
Klassenkommentar in `Mbc93Com.cpp`.

## Kurzschluss-Latenzmessung: `attachInterrupt()` statt INT1+TIMER1

Die Original-Firmware misst die Latenzzeit über eine rohe INT1-Interruptroutine plus einen
TIMER1-Overflow. Der Port ersetzt das durch `attachInterrupt(CHANGE)` plus eine
`millis()`-Zeitstempelmessung (siehe `shortIsr()`/`shortCheck()` in `Mbc93Com.cpp`) - funktional
identisch, ohne Timer1 zu belegen.

## Installation

1. [MBCANLib](../MBCANLib) installieren (Abhängigkeit, siehe `depends=MBCANLib` in
   `library.properties`).
2. Diesen Ordner (`Mbc93Lib`) nach `<Arduino-Sketchbook>/libraries/Mbc93Lib` kopieren.
3. Arduino-IDE neu starten - der Beispiel-Sketch erscheint unter
   *Datei → Beispiele → Mbc93Lib*.

## Beispiel-Sketch

| Sketch | Board | Anmerkung |
|---|---|---|
| `Mbc93_ATmega644_HW240501` | ATmega644 (MightyCore) - **kein** 644P, siehe Kopfkommentar | native Pins, feste Pinbelegung (Platinenleiterbahnen) |

`Mbc93Com` selbst ist bis auf die Kurzschluss-Latenzmessung (`attachInterrupt()`) plattformneutral
(sonst nur natives `millis()`/`analogWrite()`/`digitalWrite()`) - für weitere Plattformen wäre
lediglich ein zusätzlicher Sketch nötig, kein Eingriff in die Bibliothek.

**Wichtig:** Die exakte Pin-Zuordnung und alle ANNAHME-Vermerke stehen im Kopfkommentar des
Sketches - vor dem Verdrahten/Flashen dort nachsehen, nicht hier.

**Wichtig (Bootloader):** Vor dem ersten Einsatz muss der MBCAN-Bootloader einmalig über ISP
geflasht werden (`Bootloaderinfos_MightyCore/` im Sketch-Ordner) - beim Boardverwalter-Schritt
„Variante" **644/644A** wählen, nicht 644P/644PA (siehe Kopfkommentar des Sketches).

## Minimalbeispiel (Kern-API)

```cpp
static uint8_t sysArray[MBC_SYSARRAY_SIZE(4)];

MbcUtils       utils(sysArray, 4, pinAdcVspg, pinAdcPlug, pinLedGruen, pinLedRot, pinOneWire, 3);
MbcCan         can(pinCsMbcan, utils);
MbcBootStorage bootStorage(pinCsBootStorage);

MbcDigitalOut pinBoosterOn(pinOn), pinBoosterShort(pinShort);

Mbc93Com com(utils, can, bootStorage, pinBoosterOn, pinBoosterShort,
             pinShortDetect, pinFan /*, pinReset auf AVR */);

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
    com.update();   // Kurzschluss-Latenzzeit, Luefter, System-Stopp/Go, CAN-Auswertung
}
```

`genPurpSize` ist `4` (STATUS, SW_1, SW_2, SH), `extraEepromSize` ist `3` - siehe `EEX_93_*`-
Konstanten in `Mbc93Com.cpp`. `com.beginModule()` erledigt den kompletten Startablauf
(Ausgangsbit-/Lüfter-Pins konfigurieren, Kurzschluss-Interrupt scharfschalten, EEPROM laden),
`com.update()` je `loop()`-Durchlauf die komplette Laufzeitlogik.

## Abhängigkeiten

- [MBCANLib](../MBCANLib)

## Copyright

MBCAN (c) 2007–2026 by Dr.-Ing. Thomas Wiesner
