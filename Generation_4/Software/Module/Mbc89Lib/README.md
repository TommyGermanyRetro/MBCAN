# Mbc89Lib

**Eigenständige** Bibliothek (KEINE Abhängigkeit von [MBCANLib](../MBCANLib), siehe Abschnitt
"Warum eigenständig" unten) für **mbc-89**, ein **Connect6021-Interface**: bindet die analoge
Märklin-6021-Systemwelt (Control Unit 6021, Keyboard 6040/6041, Memory 6043, Control 80f) sowie
ein direkt am Gleis anliegendes analoges MM2-Signal an die CS2/MS2 über CAN an. mbc-89 hat 69
CS2-Konfigkanäle: 5 "klassische" (SW Major/Minor der simulierten Connect6021-Softwareversion,
Zentralentyp-Mapping, Keyboard-Basisadressblock, I2C-Bus-Takt) sowie 64 Kanäle "KeyBx GroupY"
(x=1...16 Keyboard, y=1...4 Stellpultblock/Gruppe) für das Zubehörprotokoll je Stellpult
(Auswahlliste MM/DCC, 1:1 aus dem ParaCenter-Array `Protokoll(keyboard*4+gruppe)` übernommen) -
weiterhin zusätzlich über `PC_ARRAY_DATA`/ParaCenter setzbar.

## Was mbc-89 tut

- **CU-Bus (I2C)**: mbc-89 hängt sich als aktiver Teilnehmer an den proprietären Märklin-6021-
  Systembus (physisch I2C, aber ein eigenes, nicht-Standard-Protokoll mit Laufzeit-
  Adressumschaltung). Es agiert **gleichzeitig als I2C-SLAVE** (empfängt Tasten-/Schaltbefehle von
  echter Control-80f-/Keyboard-6040-/Memory-6043-Hardware) **und als I2C-MASTER** (spielt Befehle
  auf den Bus zurück: Keyboard-Echo wenn keine CU vorhanden ist, STOPP/GO-Simulation, kompletter
  "Alles-Null"-Reset).
- **STOPP/GO-Tastensimulation**: simuliert einen 100-ms-Tastendruck (bzw. 2×1,5 s für RESET) auf
  den physischen STOPP/GO-Tastern der CU per Timer2-CTC-Interrupt - bidirektional mit CS2-System-
  Stopp/Go-CAN-Kommandos verknüpft.
- **Gleis-Signaldecoder**: liest das rohe MM2-Gleissignal direkt über INT0 + Timer1 mit
  Mikrosekunden-Genauigkeit (Trinär-"TRIT"-Dekodierautomat für Lok- UND Magnetartikelbefehle) -
  für den Fall, dass eine alte analoge Zentrale direkt am Gleis hängt statt über CAN zu sprechen
  ("INFRA"-Betrieb).
- **80-Lok-Adressdatenbank** (810 Byte) bildet MM-Adressen (0...79, von CU-Bus/Keyboard/Gleis nur
  so bekannt) auf CS2-GUiDs ab (CONNECT6021-Protokoll, CAN-ID 0x44/0x45).
- **Lok-Kommando-Sperre**: wenn eine CU oder das Gleissignal (INFRA) erkannt wurde, werden
  Richtungs-/Funktions-/Geschwindigkeitsabfragen der CS2 aus der eigenen Lokdatenbank beantwortet
  statt durchgereicht - verhindert widersprüchliche Zustände zwischen CU/Gleis und CS2.
- **Eine feste CS2-Gerätekennung 0x0020** (Märklin-Vorgabe für Connect6021-Geräte,
  `kennerFixedExtern()`) - anders als bei den Booster-Modulen mbc-80/82/93 ist diese Zuweisung im
  Original-Disassembly widerspruchsfrei (kein Kompilier-Bug).
- **System-Stopp/Go direkt verarbeiten UND selbst auslösen**: mbc-89 reagiert auf eingehende
  System-Stopp-/Go-Telegramme (CAN-ID 0x00) über die simulierten CU-Tasten und sendet bei einem
  physischen CU-Tastendruck selbst ein spontanes System-Stopp/Go-Telegramm.

## Unterstützte Plattformen: ATmega644P und ESP32-C3 - kein RP2040-Support

Anders als bei Mbc82Lib/Mbc93Lib (dort ist `MbcCom` bis auf `attachInterrupt()`/`analogWrite()`
plattformneutral) sind bei mbc-89 **beide** Kernsubsysteme auf ATmega644P roher, zeitkritischer
AVR-Registercode:

- Der CU-Bus braucht die AVR-Hardware-TWI-Peripherie direkt (feste Pins PC0/SCL, PC1/SDA) mit
  Verhalten, das die Arduino-`Wire`-Bibliothek nicht abbilden kann (Slave-UND-Master gleichzeitig,
  Laufzeit-`TWAR`-Wechsel mitten in der Übertragung).
- Der Gleisdecoder braucht Mikrosekunden-Timing (Pulsbreiten ab ca. 7 µs) - das ist mit
  `attachInterrupt()`+`micros()` durch den Overhead der Arduino-Interrupt-Indirektion nicht
  zuverlässig erreichbar und bleibt eine rohe `ISR(INT0_vect)`/`ISR(TIMER1_OVF_vect)`, die direkt
  auf `TCNT1` zugreift.

Für **ESP32-C3** wurden beide Subsysteme durch eigene, plattformspezifische Implementierungen
ersetzt (nicht durch die Standard-Arduino-`Wire`-Bibliothek oder den ESP-IDF-`i2c_slave`-Treiber -
beide unterstützen den benötigten Laufzeit-Adresswechsel mitten in einer Übertragung nicht):

- **CU-Bus**: eine komplett selbstgeschriebene, GPIO-interrupt-basierte Software-I2C-Engine
  (`Mbc89I2cSoft` in `Mbc89Com.cpp`) - bildet dieselbe `twiRxd[]`/`twiTxd[]`-Ringpuffer-
  Schnittstelle wie die AVR-ISR nach, wodurch die komplette Protokolllogik (`cuI2cCheckFn()` mit
  allen Lok-/Keyboard-/Wake-Up-Fällen) **unverändert plattformneutral** bleibt. Die eigene
  Empfangsadresse ist dort nur eine gewöhnliche Variable statt eines Hardwareregisters.
- **Gleisdecoder**: die RMT-Peripherie erfasst Pulslängen in Hardware (immun gegen Interrupt-
  Jitter, Clock-Divider so gewählt, dass ein RMT-Tick exakt derselben 0,5-µs-Rate wie ein
  Timer1-Tick auf AVR entspricht) - die komplette Trit-Auswertelogik (`automatMagnetFn()`/
  `automatLokFn()`/`trackCheckLokFn()`/`trackCheckMagnetFn()`) ist ebenfalls **unverändert
  plattformneutral**.

Beides ist **neue, in diesem Projekt bisher ungetestete Technik** - anders als bei den anderen
ESP32-C3-Ports dieses Projekts, die nur einfache I2C-Master-/GPIO-Nutzung brauchten. Vor
Dauerbetrieb unbedingt mit Logic-Analyzer/Oszilloskop verifizieren (siehe Abschnitt
"Verifikation" unten).

Auf ESP32-C3 gibt es außerdem einen **OTA-Firmware-Update-Pfad** (`Mbc89Ota`, 1:1 nach `MbcOta`
aus `MBCANLib` portiert): `Mbc89ComBase::triggerReset()` prüft vor dem eigentlichen Neustart, ob im
Boot-Speicher (W25Q64) ein gültiges Firmware-Image hinterlegt wurde, und spielt es über
`<Update.h>` ein. Dafür braucht der Sketch ein Partitionsschema mit zwei App-Partitionen
(`ota_0`/`ota_1`, siehe die mitgelieferte `partitions.csv`) - die liegt im Sketch-Ordner und wird
vom Build-Prozess automatisch übernommen (`recipe.hooks.prebuild` in `platform.txt`), unabhängig
davon, welche Option unter Tools/Werkzeuge -> Partition Scheme gerade ausgewählt ist (verifiziert
mit Arduino-ESP32-Core 2.0.17: es gibt dort gar keinen "Custom"-Menüeintrag). Auf ATmega644P gibt
es das nicht - dort löst `triggerReset()` weiterhin den Watchdog-Reset über `pinReset` aus.

Die GPIO-Knappheit eines typischen ESP32-C3-Boards (mbc-89 braucht mehr Signale, als ein simples
DevKit an nativen Pins hergibt) wird wie bei den anderen ESP32-C3-Ports dieses Projekts über einen
SPI-MCP23S17-Port-Expander gelöst: alle unkritischen, nicht zeitkritischen Signale (LEDs,
Steckernetzteil-Erkennung, CU-STOP/-GO/-EXT) hängen am Expander, nur CU-Bus SCL/SDA
(Software-I2C) und das Gleisdecoder-Signal (RMT) bleiben - da zeitkritisch - auf nativen GPIOs
(siehe Kopfkommentar von `Mbc89_ESP32C3.ino` für die konkrete Pinbelegung/Bit-Zuordnung).

`Mbc89Com.h` erzwingt die Plattformbeschränkung über einen `#error`-Guard bei
`!defined(__AVR__) && !defined(ESP32)`. CU-Bus und Gleisdecoder sind bewusst **nicht** als
Klassenmethoden umgesetzt, sondern als freistehende, dateibegrenzte Funktionen mit eigenem
statischem Zustand (genau wie im Original, wo `mbc_cu.c`/`mbc_gleis.c` eigene C-Dateien mit
globalen Variablen waren) - auf AVR müssen die vier rohen Interrupt-Vektoren
(`ISR(TWI_vect)`/`ISR(TIMER2_COMPA_vect)`/`ISR(INT0_vect)`/`ISR(TIMER1_OVF_vect)`) aus
Linker-Gründen freie Funktionen sein.

## Warum eigenständig - kein `depends=MBCANLib`

`MBCANLib` selbst zieht über `MbcI2cBus.h` (fest `#include <Wire.h>`, gebraucht von
`MbcMcp23x17`/`MbcPcf8574`/`MbcSsd1306`/... für andere Module) unvermeidlich die Arduino-
`Wire`-Bibliothek in JEDEN Sketch, der `MBCANLib` verwendet - Arduino kompiliert beim Einbinden
einer Bibliothek immer deren komplette `src/`-Quelldateien, nicht nur die tatsächlich benötigten.
`Wire`s eigene `utility/twi.c` definiert dabei bereits `ISR(TWI_vect)` - das kollidiert hart
("multiple definition of `__vector_26`") mit dem eigenen `ISR(TWI_vect)` dieser Bibliothek (siehe
oben, mbc-89 muss die TWI-Hardware fuer den CU-Bus exklusiv besitzen). Ein Opt-in-Makro hilft
hier NICHT, da Arduino jede Bibliotheksdatei als eigene, vom Sketch unabhängige Übersetzungseinheit
kompiliert.

Deshalb enthält `Mbc89Lib/src/` eigene, umbenannte Kopien der benötigten `MBCANLib`-Klassen
(`Mbc89Utils`, `Mbc89Can`, `Mbc89ComBase`, `Mbc89BootStorage`, `Mbc89SpiDevice`, `Mbc89Platform`,
`Mbc89Ota` - nur ESP32-C3) - ohne jede Wire-/I2C-Abhängigkeit. Die optionale LED-/
Steckerkennung-Umleitung auf einen Port-Expander gibt es dennoch (für die GPIO-Knappheit auf dem
ESP32-C3, siehe unten): `Mbc89Mcp23x17`/`Mbc89DigitalOut`/`Mbc89DigitalIn` sind ebenfalls eigene
Kopien von `MbcMcp23x17`/`MbcDigitalOut`/`MbcDigitalIn`, aber **nur mit dem SPI-Backend**
(MCP23S17) - das I2C-Backend (MCP23017) wurde komplett entfernt, nicht nur ungenutzt gelassen, da
es über `MbcI2cBus.h` genau denselben `Wire.h`-Konflikt auslösen würde. Die Umbenennung (statt
gleicher Dateinamen wie in `MBCANLib`) verhindert
eine Namenskollision, falls beide Bibliotheken im selben Arduino-Sketchbook installiert sind -
sonst wäre nicht mehr eindeutig, welche `MbcCom.h`/`MbcUtils.h` ein anderer Sketch (z.B. mbc-93)
tatsächlich bekommt. Diese Kopien werden **nicht** separat gepflegt - Bugfixes an der gemeinsamen
Basis (`MbcCom`/`MbcUtils`/... in `MBCANLib`) müssen bei Bedarf manuell hierher übernommen werden.

## Wichtig: 1:1-Portierung, keine eigenen Korrekturen an der CAN-Logik

`handleSystemCommand()` ist bewusst 1:1 aus der Original-Firmware (`main.c`) übernommen -
inklusive eines auffälligen Vergleichs in der modulbezogenen GUiD-Prüfung (D2 wird gegen dieselbe
GUiD-Stelle wie D1 verglichen statt gegen die dritte - identisches Muster wie bei mbc-93). Gegen
die offizielle CS2-Protokolldokumentation geprüft: die universelle Broadcast-Form (D0...D3 alle
0), über die CS2 System-Stopp/Go in der Praxis überwiegend sendet, ist davon nicht betroffen.
Dieses Projekt ändert Sicherheitslogik in gefertigter Original-Firmware nicht ungefragt.

## Korrektur gegenüber dem Original: Gleisdecoder-Impulslängenfenster

`mbc_defs.h` definiert `F_CPU 18432000UL`, die reale mbc-89-Hardware hat aber 16 MHz. Die
CU-Bus-TWI-Baudrate löst sich davon im Port automatisch (die Arduino-Boarddefinition liefert das
korrekte `F_CPU`), aber die Gleisdecoder-Impulslängenfenster (rohe Timer1-Tick-Literale ohne
F_CPU-Bezug im Code selbst) waren erkennbar auf Basis des falschen 18,432-MHz-Takts ausgelegt.
Für diesen Port wurden sie **frisch aus der MM2-Spezifikation für den echten 16-MHz-Takt
abgeleitet** (Kreuzprüfung über die im Original-Kopfkommentar genannten Paketlängen: Magnetartikel
1,87 ms / Lok 3,74 ms für je 18 übertragene Trits → Magnetartikel-Trit=104 µs, Lok-Trit=208 µs,
Motorola-Kurz/Lang-Verhältnis 1:7 - siehe `MAGNET_*`/`LOK_*`-Konstanten in `Mbc89Com.cpp`). Dies
ist eine bewusste Korrektur, keine 1:1-Übernahme der alten (auf falschem Takt basierenden)
Literale - **die exakten Fenstergrenzen sind beim Hardwaretest mit echtem MM-Signal/Oszilloskop zu
verifizieren und ggf. nachzujustieren**, bevor auf Dauerbetrieb vertraut wird.

## Installation

1. Diesen Ordner (`Mbc89Lib`) nach `<Arduino-Sketchbook>/libraries/Mbc89Lib` kopieren -
   **`MBCANLib` wird NICHT benötigt** (siehe "Warum eigenständig" oben).
2. Arduino-IDE neu starten - der Beispiel-Sketch erscheint unter
   *Datei → Beispiele → Mbc89Lib*.

## Beispiel-Sketch

| Sketch | Board | Anmerkung |
|---|---|---|
| `Mbc89_ATmega644P_HW220301` | ATmega644P (MightyCore) | native Pins, feste Pinbelegung (Platinenleiterbahnen) |
| `Mbc89_ESP32C3` | ESP32-C3 SuperMini (Espressif-esp32-Boardpaket) | Software-I2C-Engine + RMT (neue, ungetestete Technik - siehe Abschnitt oben), MCP23S17-Expander für unkritische Signale, OTA |

**Wichtig:** Die exakte Pin-Zuordnung steht im Kopfkommentar des jeweiligen Sketches - vor dem
Verdrahten/Flashen dort nachsehen, nicht hier. Der ESP32-C3-Sketch ist auf ein "ESP32-C3
SuperMini"-Board zugeschnitten (nur 13 GPIOs: 0...10, 20, 21) - bei einem anderen Board zuerst
dessen tatsächlich herausgeführte GPIOs prüfen.

**Wichtig (Bootloader, nur ATmega644P):** Vor dem ersten Einsatz muss der MBCAN-Bootloader einmalig
über ISP geflasht werden (`Bootloaderinfos_MightyCore/` im Sketch-Ordner) - beim Boardverwalter-
Schritt „Variante" **644P/644PA** wählen (siehe Kopfkommentar des Sketches). Auf ESP32-C3 entfällt
das - USB-Flashen über die Arduino-IDE genügt.

## Minimalbeispiel (Kern-API)

```cpp
static uint8_t sysArray[MBC_SYSARRAY_SIZE(880)];

Mbc89Utils       utils(sysArray, 880, pinAdcVspg, pinAdcPlug, pinLedGruen, pinLedRot, pinOneWire, 70);
Mbc89Can         can(pinCsMbcan, utils);
Mbc89BootStorage bootStorage(pinCsBootStorage);

// ATmega644P: keine eigenen CU-Bus-/Gleisdecoder-Pins, feste AVR-Hardware
Mbc89Com com(utils, can, bootStorage, pinReset);

// ESP32-C3: freie GPIO-Wahl fuer CU-Bus (Software-I2C, zeitkritisch) und Gleisdecoder (RMT,
// zeitkritisch); STOP/GO/EXT als Mbc89DigitalIn&/Mbc89DigitalOut&-Referenzen (nativ ODER ueber
// einen Mbc89Mcp23x17-Expander, siehe Mbc89_ESP32C3.ino), kein pinReset
// Mbc89Com com(utils, can, bootStorage, pinCuScl, pinCuSda, cuStopIn, cuStopOut,
//              cuGoIn, cuGoOut, cuExt, pinTrack);

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
    com.update();   // CU-Bus, Gleisdecoder, System-Stopp/Go, CONNECT6021, CAN-Auswertung
}
```

`genPurpSize` ist `880`, `extraEepromSize` ist `70` - siehe `EEX_89_*`-Konstanten in
`Mbc89Com.cpp`. Auf ATmega644P hat `Mbc89Com` **keine** eigenen Ausgangs-/Konfigpins im
Konstruktor - CU-Bus und Gleisdecoder hängen an fest durch die Hardware vorgegebenen AVR-
Registern (siehe Klassenkommentar in `Mbc89Com.h`). Auf ESP32-C3 sind CU-Bus-/Gleisdecoder-Pins
dagegen echte Konstruktorparameter (siehe Tabelle im Kopfkommentar von `Mbc89_ESP32C3.ino`).
`com.beginModule()` erledigt den kompletten Startablauf (CU-Bus/Gleisdecoder scharfschalten,
EEPROM laden, Startsynchronisation), `com.update()` je `loop()`-Durchlauf die komplette
Laufzeitlogik - auf beiden Plattformen identisch.

## Verifikation

Deutlich anspruchsvoller als bei jedem bisherigen Modul, da CANMON allein nicht ausreicht (CU-Bus
und Gleissignal laufen außerhalb des CAN-Busses):

- **CS2-Ebene** wie gewohnt: Systemstart, GUiD/Kenner 0x0020, simulierte SW-Version, System-
  Stopp/Go über CAN, CONNECT6021-Mapping-Zuweisung/-Reset per CANMON prüfbar.
- **CU-Bus**: braucht echte Control-Unit-6021-/Keyboard-6040-/Memory-6043-Hardware am I2C-Bus zum
  Testen von Tastendruck-Erkennung, Keyboard-Weiterleitung an die CS2 und Rück-Synchronisierung
  (Speed/Dir/Fx) nach CONNECT6021-Übernahme.
- **Gleisdecoder**: braucht eine echte alte analoge MM-Zentrale (oder einen Signalgenerator) am
  TRACK-Eingang. Da die Impulslängenfenster für diesen Port frisch abgeleitet wurden (s.o.), sind
  hier insbesondere die Fenstergrenzen selbst mit Oszilloskop/Logic-Analyzer zu prüfen und ggf.
  nachzujustieren.
- Wegen der zwei parallel aktiven Interrupt-Subsysteme (TWI+Timer2, INT0+Timer1 auf AVR bzw.
  Mbc89I2cSoft+RMT auf ESP32-C3) empfiehlt sich ein vorsichtiger erster Hardwaretest (z.B. erst
  CU-Bus, dann erst Gleisdecoder aktiv verkabeln) statt beides gleichzeitig in Betrieb zu nehmen.

**Zusätzlich nur für ESP32-C3** (neue, in diesem Projekt bisher ungetestete Technik - deutlich
vorsichtiger vorgehen als bei den übrigen Punkten oben):

- **Software-I2C-Engine (`Mbc89I2cSoft`)**: vor dem Anschluss echter CU-Bus-Hardware unbedingt mit
  einem Logic-Analyzer verifizieren (Bit-Timing, ACK-Verhalten bei Adressübereinstimmung, START-/
  STOP-Erkennung, Master-Sendevorgang inkl. ACK-Auswertung). Die Engine unterstützt kein Clock-
  Stretching durch andere Busteilnehmer und keine echte Multi-Master-Arbitrierung während eigener
  Sendevorgänge - beides bei Bedarf gezielt nachbilden/prüfen.
- **RMT-Gleisdecoder**: nutzt die neue `driver/rmt_rx.h`-API (Arduino-ESP32-Core 3.x/IDF5,
  Callback-/Job-basiert statt des treiberinternen Ringbuffers der alten Core-2.x/IDF4-API). Die
  Aufloesung (2 MHz/0,5 µs pro Tick) ist auf dieselbe Tick-Rate wie Timer1 auf AVR ausgelegt, damit
  die bestehenden `MAGNET_*`/`LOK_*`-Schwellwerte unveraendert gelten - trotzdem mit echtem
  MM-Signal/Oszilloskop gegenpruefen, da die RMT-eigenen Glitch-Filter-/Idle-Schwellwerte
  (`signal_range_min_ns`/`signal_range_max_ns` in `trackInitFn()`) das Timing zusaetzlich
  beeinflussen koennen. Der `on_recv_done`-Callback laeuft in ISR-Kontext und schreibt deshalb in
  einen selbst angelegten FreeRTOS-Ringbuffer statt direkt in `cmdBLevel[]`/`cmdBLength[]` (siehe
  Kommentar in `trackInitFn()`), noch nicht auf echter Hardware verifiziert.
- **MCP23S17-Expander**: `expander.begin()` liefert beim Start eine Praesenzpruefung (Ruecklesen
  von IODIRA) - wird über Serial ausgegeben (`Mbc89_ESP32C3.ino`, "MCP23S17 (LEDs/Plug/CU-STOP/
  GO/EXT)"). Meldet der Sketch hier FEHLER, sind alle darüber geführten CU-Bus-Tasten-/LED-/
  Plug-Signale ohne Wirkung, bevor überhaupt eine echte CU angeschlossen wird - CS-Pin/Verdrahtung
  zuerst prüfen.

## Abhängigkeiten

Keine (siehe "Warum eigenständig" oben) - nur Arduino-Core-Bibliotheken (`SPI`, `EEPROM` auf AVR;
zusätzlich `esp_timer`/`driver/rmt.h` aus dem ESP32-Arduino-Core auf ESP32-C3).

## Copyright

MBCAN (c) 2007–2026 by Dr.-Ing. Thomas Wiesner
