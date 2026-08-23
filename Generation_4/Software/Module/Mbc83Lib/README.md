# Mbc83Lib

Modulspezifische Erweiterung von [MBCANLib](../MBCANLib) für **mbc-83**, einen 4-fach
Relais-/Magnetartikeldecoder mit Dimmerausgang. mbc-83 hat 4 modulspezifische CS2-Konfigkanäle
(MM-Addr, Auto-Aus, Meldezyklus, Dimmer) - die übrige Parametrierung (Spulentiming, Art der
Ausgänge) läuft weiterhin ausschließlich über den rohen `PC_ARRAY_DATA`-Zugriff auf das
Systemarray.

## Was mbc-83 tut

- **4x2 gepulste Relaisausgänge** (klassischer Märklin-K83/K84-Stil: Spulenstrom kurz an, dann
  automatisch aus) - je Ausgang Rot(L)/Grün(R). Zu jedem Zeitpunkt ist höchstens ein Ausgang aktiv
  (ein einzelner Pulszustand, kein Array je Ausgang).
- **4 Rückmelde-Eingänge** lesen die tatsächliche (bistabile) Relaisstellung zurück. **Wichtig:**
  die Aktorstellung wird NIE ins EEPROM geschrieben - sie wird bei jedem `beginModule()` frisch aus
  diesen 4 Eingängen abgeleitet, nicht aus einer gespeicherten Stellung.
- **Konfigurierbares Spulentiming je Ausgang** UND ein globaler **Auto-Aus**-Sicherheitswert - die
  tatsächliche Pulsdauer ist das Minimum aus beiden (zwei ODER-verknüpfte Abbruchbedingungen im
  selben Zeitraster).
- **Art der Ausgänge** (2 Bit je Ausgang): ist das Bitpaar eines Ausgangs ungleich 0, wird beim
  erzwungenen Rückmeldezyklus nur das CS2-Telegramm gesendet, ohne das Relais physisch zu pulsen
  (Ausgang dient dann nur als virtueller Status, z.B. zur Ansteuerung eines vorgelagerten
  Adapters). Bei direkten Schaltbefehlen/Stellpult gilt diese Sperre nicht.
- **Ein PWM-Dimmerausgang** (`analogWrite()` auf einem nativen Pin) - wird bei jeder Parameteränderung neu angewendet,
  nicht nur bei einer eigenen Dimmer-Änderung. Startwert per CS2-Konfigkanal
  (Label „Dimmer", Rohwert 0...200) oder `PC_ARRAY_DATA` einstellbar.
- **4x2 Stellpulteingänge** lösen direkt einen Pulsauftrag aus.
- **Rohe Motorola-Zubehör-Schaltbefehle** (`CAN_ID_ZUBEHOER_SCHALTEN`, Adresse per D2/D3) sowie
  eine Rückmeldeanforderung eines vorgelagerten LinkS88-Gateways (Loc-ID-Subkommando
  `MD_ZYKLUS`) werden direkt in `update()` ausgewertet, unabhängig von der PC-/CS2-Dispatch-Logik
  der `MbcCom`-Basisklasse.
- **4 CS2-Konfigkanäle** (Label „MM-Addr" 0...255, „Auto-Aus x10ms" 1...254, „Meldezyklus"
  disabled/enabled, „Dimmer" 0...200) erlauben das Setzen von Basisadresse, Auto-Aus-Zeit,
  Zwangsrückmeldung bzw. Dimmer-Startwert direkt aus der CSx-GUI, siehe
  `cs2IndexParameterExtern()`. Eine Änderung von MM-Addr oder eine Aktivierung von Meldezyklus
  löst dabei sofort einen Rückmeldezyklus aus, wenn gerade keiner läuft - unabhängig davon, ob
  die Änderung über CS2 oder ParaCenter (`PC_ARRAY_DATA`) kam, da beide Wege in
  `eepromWriteParaExtern()` zusammenlaufen.

## Wichtige Asymmetrie: wer unterbricht einen laufenden Puls?

Ein direkter Schaltbefehl (CAN) unterbricht/überschreibt sofort einen laufenden Puls (sendet dabei
noch das Aus-Telegramm des unterbrochenen Pulses). Stellpult UND Rückmeldezyklus wirken dagegen
NUR, wenn gerade kein Puls aktiv ist.

**Rückmeldezyklus-Timing:** jeder Rückmeldezyklus-Schritt startet bei mbc-83 einen ECHTEN
Timing-Puls und wartet auf dessen Ende, weil derselbe Pulstimer wie bei einem echten Schaltbefehl
verwendet wird - unabhängig davon, ob der Ausgang wegen "Art" physisch gepulst wird oder nicht.
Der Zyklus braucht also je Schritt so lange wie das konfigurierte Spulentiming/Auto-Aus.

## Installation

1. [MBCANLib](../MBCANLib) installieren (Abhängigkeit, siehe `depends=MBCANLib` in
   `library.properties`).
2. Diesen Ordner (`Mbc83Lib`) nach `<Arduino-Sketchbook>/libraries/Mbc83Lib` kopieren.
3. Arduino-IDE neu starten - die Beispiel-Sketches erscheinen unter
   *Datei → Beispiele → Mbc83Lib*.

## Beispiel-Sketches

| Sketch | Board | Relais/Rückmeldung/Stellpult |
|---|---|---|
| `Mbc83_ATmega644_HW240323` | ATmega644 (MightyCore) - **kein** 644P, siehe Kopfkommentar | native Pins, feste Pinbelegung (Platinenleiterbahnen) |
| `Mbc83_ATmega644_HW180210` | ATmega644 (MightyCore), ältere Hardware-Revision - Pin-/Ablauflogik identisch zu HW240323, nur ein Facelift (ArtNum4 bleibt bei 1) | native Pins, identisch zu HW240323 |
| `Mbc83_RP2040_Pico` | Raspberry Pi Pico | Relais+Stellpult native Pins, Rückmeldung (4 Eingänge) an einem eigenen `MbcMcp23x17` |
| `Mbc83_RP2040_Zero` | Waveshare RP2040-Zero | Relais+Stellpult an einem `MbcMcp23x17`, Rückmeldung teilt sich den LED-/Plug-Expander |
| `Mbc83_ESP32C3` | ESP32-C3 | wie RP2040-Zero (pinknapp, 2 Expander) |

`Mbc83Com` selbst ist plattformneutral (kein AVR-Timer-Hack nötig, nur natives
`millis()`/`analogWrite()`/`digitalWrite()`) - die Unterschiede zwischen den Sketches stecken
ausschließlich in der Pin-/Expander-Zuordnung, nicht in der Bibliothek.

**Wichtig:** Die exakten Pin-Zuordnungen und alle ANNAHME-Vermerke stehen im Kopfkommentar jedes
Sketches - vor dem Verdrahten/Flashen dort nachsehen, nicht hier. Ebenso die dort vermerkten, noch
nicht hardwarebestätigten Annahmen (Relais-Polarität, PWM-Pin-Eignung).

**Wichtig (Bootloader, nur ATmega644):** Vor dem ersten Einsatz muss der MBCAN-Bootloader einmalig
über ISP geflasht werden (`Bootloaderinfos_MightyCore/` im Sketch-Ordner) - beim
Boardverwalter-Schritt "Variante" **644/644A** wählen, nicht 644P/644PA (siehe Kopfkommentar des
Sketches).

**Wichtig (OTA, nur RP2040/ESP32-C3):** Beide Cores benötigen eine reservierte zweite Flash-
Partition für Firmware-Updates (siehe `MbcOta.h`-Kopfkommentar). Beim ESP32-C3 über die
mitgelieferte `partitions.csv` im jeweiligen Sketch-Ordner (*Tools/Werkzeuge → Partition Scheme →
Custom*); beim RP2040 dagegen **ohne** CSV-Datei ausschließlich über *Tools/Werkzeuge → Flash
Size* (eine Variante mit „OTA" im Namen wählen).

## Minimalbeispiel (Kern-API)

```cpp
static uint8_t sysArray[MBC_SYSARRAY_SIZE(11)];

MbcUtils       utils(sysArray, 11, pinAdcVspg, pinAdcPlug, pinLedGruen, pinLedRot, pinOneWire, 10);
MbcCan         can(pinCsMbcan, utils);
MbcBootStorage bootStorage(pinCsBootStorage);

MbcDigitalOut *relayOut[8] = { &rel1L, &rel1R, &rel2L, &rel2R, &rel3L, &rel3R, &rel4L, &rel4R };
MbcDigitalIn  *relayIn[4]  = { &relIn1, &relIn2, &relIn3, &relIn4 };
MbcDigitalIn  *switchboard[8] = { &in1R, &in1G, &in2R, &in2G, &in3R, &in3G, &in4R, &in4G };

Mbc83Com com(utils, can, bootStorage, relayOut, relayIn, switchboard,
             pinDimmer /*, pinReset auf AVR */);

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
    com.update();   // Stellpult, Relaispulse, Rueckmeldungen, Dimmer, CAN-Auswertung
}
```

`genPurpSize` ist `11` (MM-Adresse H/L, Aktorstellung, Art der Ausgänge, Auto-Aus, Meldezyklus,
Dimmerstellung, 4x Spulentiming), `extraEepromSize` ist `10` - siehe `EEX_83_*`-Konstanten in
`Mbc83Com.cpp`. `com.beginModule()` erledigt den kompletten Startablauf (EEPROM laden, Relais/
Rückmelde-/Stellpultpins konfigurieren, Aktorstellung aus den Rückmelde-Eingängen ableiten,
Dimmer-Startwert anwenden), `com.update()` je `loop()`-Durchlauf die komplette Laufzeitlogik.

## Abhängigkeiten

- [MBCANLib](../MBCANLib)

## Copyright

MBCAN (c) 2007–2026 by Dr.-Ing. Thomas Wiesner
