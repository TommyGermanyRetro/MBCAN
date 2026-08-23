# Mbc84Lib

Modulspezifische Erweiterung von [MBCANLib](../MBCANLib) für **mbc-84**, einen 4-fach
Servo-/Weichendecoder mit Stellpult. mbc-84 hat einen ersten CS2-Konfigkanal (Label "MM-Addr",
Modulnummer 0...255) zum Setzen der Basisadresse direkt aus der CSx-GUI; die übrige
Parametrierung läuft weiterhin über `PC_ARRAY_DATA` (direkter Systemarray-Zugriff) und die
Ansteuerung über rohe Motorola-Zubehör-Schaltbefehle statt über weitere CS2-Konfigkanäle.

## Was mbc-84 tut

- **4 Servoausgänge** über die Arduino-Standardbibliothek `Servo` (interruptgetriebener Pulszug
  auf beliebigen Digitalpins, unabhängig von Hardware-PWM-Kanälen, portabel über alle
  Zielplattformen). Jeder Servo hat einstellbare Rot-/Grün-Endlagen und
  optional einen 120-Schritt-Servoweg je Richtung mit konfigurierbarer Verzögerung für weiche
  Bewegungen.
- **4x2 Stellpulteingänge** (Rot/Grün je Servo, native GPIO mit internem PullUp) schalten die
  Aktorstellung direkt am Modul, unabhängig vom Bus.
- **8 Positions-LEDs** zeigen die aktuelle Aktorstellung (Bit `i` der Aktorstellung ↔ `led[i]` —
  die physische Pin-Zuordnung je Board legt der Sketch fest).
- **Rohe Motorola-Zubehör-Schaltbefehle** (`CAN_ID_ZUBEHOER_SCHALTEN`, Adresse per D2/D3) statt
  eines CS2-Konfigkanals — mbc-84 hört wie ein echter Märklin-Motorola-Decoder auf einen
  4er-Adressblock ab der parametrierten MM-Basisadresse. Zusätzlich wertet es eine
  Rückmeldeanforderung eines vorgelagerten LinkS88-Gateways aus (Loc-ID-Subkommando
  `MD_ZYKLUS`). Beides wird direkt in `update()` ausgewertet (`handleSwitchCommand()`,
  Rückmeldezyklus-Zähler `_zyklusstep`),
  unabhängig von der PC-/CS2-Dispatch-Logik der `MbcCom`-Basisklasse. Der MM-Addr-Konfigkanal ist
  davon unabhängig — er ändert dieselbe, im Systemarray gehaltene Basisadresse, nur über einen
  anderen Zugriffsweg (siehe `cs2IndexParameterExtern()`).

## RAM: voller Servoweg auch auf AVR

Der Servoweg-Bereich ist mit 480 Byte (4 Servos × 60 Schritte × 2 Richtungen) weiterhin mit
Abstand der größte General-Purpose-Bereich aller bisher portierten Module (ursprünglich 960 Byte
bei 120 Schritten je Richtung — seit servoStep() bei jedem Bewegungsschritt zwischen den
Tabellenwerten interpoliert (siehe `INTERP_STEP_MS`), bestimmt die Schrittzahl nur noch die Anzahl
der Stützpunkte für eine frei programmierbare Kurvenform, nicht mehr die gelieferte Laufruhe -
120 Schritte waren fürs Standard-Hobbyservo weit feiner als mechanisch nutzbar, 60 wurden deshalb
als RAM-sparenderer Wert gewählt). Das Systemarray (siehe `MbcUtils.h`) liegt nicht mehr fest in
`MbcUtils` eingebettet, sondern wird dem Konstruktor als Zeiger auf einen vom Sketch selbst
angelegten, exakt bemessenen Puffer übergeben (Makro `MBC_SYSARRAY_SIZE(genPurpSize)`, siehe
Minimalbeispiel unten) — jedes Modul zahlt damit genau für die Größe, die es braucht, statt sich
eine für alle Module gemeinsame, im ungünstigen Fall überdimensionierte Größe zu teilen. Die
tatsächliche RAM-Auslastung hängt inzwischen auch stark von der Anzahl der CS2-Konfigkanäle
(`_cs2IndexEx`, siehe Klassenkommentar in `Mbc84Com.h`) ab und sollte je nach Ausbaustufe über die
Arduino-IDE-Kompilierausgabe am realen Sketch nachgemessen werden, statt hier veraltete Fixwerte
fortzuschreiben.

## Installation

1. [MBCANLib](../MBCANLib) installieren (Abhängigkeit, siehe `depends=MBCANLib` in
   `library.properties`).
2. Diesen Ordner (`Mbc84Lib`) nach `<Arduino-Sketchbook>/libraries/Mbc84Lib` kopieren.
3. Arduino-IDE neu starten — die Beispiel-Sketches erscheinen unter
   *Datei → Beispiele → Mbc84Lib*.

## Beispiel-Sketches

| Sketch | Board |
|---|---|
| `Mbc84_ATmega644P_HW240106` | ATmega644P (MightyCore) |
| `Mbc84_ATmega644P_HW180203` | ATmega644P (MightyCore), ältere Hardware-Revision — Pin-/Ablauflogik identisch zu HW240106, nur ein Facelift (ArtNum4 bleibt bei 1) |
| `Mbc84_RP2040_Pico` | Raspberry Pi Pico |
| `Mbc84_RP2040_Zero` | Waveshare RP2040-Zero |
| `Mbc84_ESP32C3` | ESP32-C3 |

**Wichtig:** Die exakten Pin-Zuordnungen stehen im Kopfkommentar jedes Sketches — vor dem
Verdrahten/Flashen dort nachsehen, nicht hier.

**Wichtig (Bootloader, nur ATmega644P):** Vor dem ersten Einsatz muss der MBCAN-Bootloader einmalig
über ISP geflasht werden (`Bootloaderinfos_MightyCore/` im Sketch-Ordner).

## Beispiel-Fahrwege (`Servos/`)

Fertige Servoweg-Dateien zum Laden über den Button *Servoweg laden...* im Parametriercenter —
Format: 120 Zeilen `idx;wert` (Hex, Index 0x00...0x77, Wert 0...0x3C).

| Datei | Beschreibung |
|---|---|
| `linear.txt` | Linearer Standardweg (Rot→Grün→Rot mit konstanter Geschwindigkeit) — entspricht der Default-Kurve, die `servoCurveInit()` bei Erststart selbst anlegt. |
| `schranke.txt` | Nachgebildete Bahnschranken-Bewegung: erreicht die Endlage etwas vor dem letzten Schritt, federt leicht zurück und pendelt sich dann exakt ein — simuliert das Auftreffen auf einen mechanischen Anschlag. Beide Enden schwingen gleich oft (1x). **Empfohlen: VZ=4** (≈4,8 s Fahrzeit je Richtung, kommt der realen Schließzeit eines Bahnübergangs nahe). |
| `schranke_links.txt` / `schranke_rechts.txt` | Realistischere Variante für zwei separate Schrankenbäume (links/rechts vom Gleis) — jeder schwingt unterschiedlich oft nach, damit sie nicht wie Zwillinge wirken: links 1x bei Grün / 3x bei Rot, rechts 2x bei Grün / 2x bei Rot. **Empfohlen: VZ=4.** |
| `schuppentor.txt` | Nachgebildete 90°-Drehtor-Bewegung (Lokschuppentor): kein Überschwingen, dafür spürbares Losbrechmoment am Anfang (kurze Verharrphase) und sanftes Abbremsen zum Anschlag — symmetrisch für Auf/Zu. **Empfohlen: VZ=3...5** (≈3,8...5,8 s Fahrzeit je Richtung). |
| `fluegelsignal.txt` | Nachgebildete Flügelsignal-Bewegung: zügige Fahrt mit kleinem mechanischem "Klack"-Nachschwingen am Ende (leichter Flügel, kein schweres Auftreffen wie bei der Schranke), symmetrisch für Halt/Fahrt. **Wichtig:** ein echtes Flügelsignal schwenkt nur ca. 45° statt der vollen 90° — RT-max/GN-max daher nicht auf 30/30, sondern eher auf ca. 15/15 stellen (die Kurve selbst bleibt unverändert, `rawAtStepScaled()` skaliert sie automatisch auf den kleineren Stellbereich). **Empfohlen: VZ=1** (≈1,9 s Fahrzeit je Richtung, deutlich zügiger als Schranke/Schuppentor). |
| `weichenantrieb.txt` | Nachgebildete Weichenstellbewegung: sehr zügig, kein Losbrechmoment, knapper/harter Einrastvorgang am Ende statt weichem Nachfedern — erreicht die Endlage bereits deutlich vor dem letzten Schritt und haelt dort. **Wichtig:** eine echte Weiche braucht am Servo nur ca. 15-25° Drehwinkel — RT-max/GN-max daher deutlich kleiner als bei den anderen Beispielen wählen, ca. 7-10 statt 30 (Kurve bleibt unverändert, wird automatisch skaliert). **Empfohlen: VZ=0** (schnellstmögliche Einstellung, ≈1 s Fahrzeit — kommt einer echten Weichenstellzeit am nächsten). |

## Minimalbeispiel (Kern-API)

```cpp
static uint8_t sysArray[MBC_SYSARRAY_SIZE(genPurpSize)];

MbcUtils       utils(sysArray, genPurpSize, pinAdcVspg, pinAdcPlug, pinLedGruen, pinLedRot,
                      pinOneWire);
MbcCan         can(pinCsMbcan, utils);
MbcBootStorage bootStorage(pinCsBootStorage);

MbcDigitalIn  *switchboard[8] = { &r0, &g0, &r1, &g1, &r2, &g2, &r3, &g3 };
MbcDigitalOut *led[8]         = { &led0, &led1, &led2, &led3, &led4, &led5, &led6, &led7 };
const uint8_t servoPins[4]    = { pinServo0, pinServo1, pinServo2, pinServo3 };

Mbc84Com com(utils, can, bootStorage, servoPins, switchboard, led /*, pinReset auf AVR */);

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
    com.update();   // Stellpult, Servobewegung, Rueckmeldungen, CAN-Auswertung
}
```

`genPurpSize` ist auf allen 4 Zielprozessoren `497` (MM-Adresse+Aktorstellung+Servo-Endlagen+voller
480-Byte-Servoweg) — siehe `EEX_84_*`-Konstanten in `Mbc84Com.cpp`. `com.beginModule()` erledigt
den kompletten Startablauf (EEPROM laden, Servos binden, Startstellung anfahren), `com.update()`
je `loop()`-Durchlauf die komplette Laufzeitlogik.

## Abhängigkeiten

- [MBCANLib](../MBCANLib)
- `Servo` (bei AVR/RP2040-Cores bereits enthalten; auf ESP32 ggf. `ESP32Servo` nötig, falls der
  verwendete Core keine eigene kompatible `Servo`-Bibliothek mitbringt)

## Copyright

MBCAN (c) 2007–2026 by Dr.-Ing. Thomas Wiesner
