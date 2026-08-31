# CAN-Nachrichten zur Modulkonfiguration

Diese Dokumentation beschreibt die Konfigurationskommunikation zwischen den
Modulen `mbc-80`, `mbc-81`, `mbc-82`, `mbc-83`, `mbc-84`, `mbc-88`, `mbc-89`, `mbc-90`, `mbc-91`,
`mbc-92`, `mbc-93` und einer Maerklin-CS2/CSx.

## 1. Befehlsstruktur einer CAN-Nachricht

Jede CAN-Nachricht wird im beschriebenen Nachrichtenformat mit 13 Bytes
dargestellt. Das CAN-Datenfeld besteht aus `D0...D7`; Byte 0 bis 4 sind

Die gemeinsamen Zeilen sind:

| Zeile/Kanal | Inhalt |
|---:|---|
| `0` | Geraeteinformationen, Kennung `MBC`, Text `MBCAN mbc-...` |
| `1` | Versorgungsspannung, Anzeige `VOLT 10.00 27.00 V` |
| `2` | Temperatur, Anzeige `TEMP 0.0 80.0 C` |
| `3` | Seriennummer |
| `4` | GUiD |
| `5` | Firmware |
| `6` | Hardware |

Die Modulzeilen beginnen bei Kanal `7`. Kanal `0` enthaelt in den ersten
beiden Nutzbytes ausserdem die Gesamtzahl der Messwert- und
Konfigurationskanaele.

### Klartext der Initialbytes 1...8 nach Maerklin

Die Bytes 1...8 sind nicht acht unabhaengige Flags. Diese bilden den Kopf eines
Konfigurationsdatenblocks. Die Bedeutung richtet sich nach Byte 2:

| Byte | Typ `01 = Auswahlliste` | Typ `02 = Slider` |
|---:|---|---|
| 1 | Konfigurationskanalnummer | Konfigurationskanalnummer |
| 2 | Kennung `01`, Klartext `Auswahlliste` | Kennung `02`, Klartext `Slider` |
| 3 | Anzahl der Auswahlpunkte, `2...255` | Untergrenze Highbyte |
| 4 | aktueller Auswahlindex, `0`-basiert | Untergrenze Lowbyte |
| 5 | reserviert, normalerweise `00` | Obergrenze Highbyte |
| 6 | Beginn des folgenden Textbereichs bzw. reserviert | Obergrenze Lowbyte |
| 7 | Beginn des folgenden Textbereichs bzw. reserviert | aktuelle Einstellung Highbyte |
| 8 | Beginn des folgenden Textbereichs bzw. reserviert | aktuelle Einstellung Lowbyte |

Die in Byte 3...8 gespeicherten Sliderwerte sind 16-Bit-Werte in
Big-Endian-Reihenfolge. Hinter dem Kopf folgen die Klartextfelder aus dem
MÃ¤rklin-Protokoll: bei einer Auswahlliste zuerst die
`Auswahlbezeichnung`, danach ein String je Auswahlpunkt; bei einem Slider die
`Auswahlbezeichnung`, `Bezeichnung Start`, `Bezeichnung Ende` und `Einheit`.
Die C++-Arrays verwenden dafuer dieselbe 40-Byte-Zeile und fuellen die
unbenutzten Restbytes mit `00`.

Damit lassen sich die im Folgenden angegebenen Initialbytes direkt lesen:

```text
07 02 00 00 00 FF 00 00
|  |  |     |     +------ aktuelle Einstellung = 0
|  |  |     +------------ Obergrenze = 255
|  |  +------------------ Untergrenze = 0
|  +--------------------- Slider
+------------------------ Kanal 7
```

Bei einem Auswahlkanal wie `09 01 02 00 00 00 00 00` bedeutet die Folge:
Kanal `9`, Kennung `Auswahlliste`, zwei Auswahlpunkte, aktueller Index `0`,
danach reservierte bzw. durch Textfelder belegte Bytes.

### Klartext der Initialbytes 1...16 fuer Messwertkanaele (Typ `00`)

Neben `01`/`02` gibt es einen dritten Kopftyp fuer reine Messwertkanaele
(anzeigend, nicht editierbar) - bisher nur bei `mbc-93` genutzt, alle anderen
Module dieses Projekts haben `ANZMESSWERTEMODUL=0`:

| Byte | Typ `00 = Messwert` |
|---:|---|
| 1 | Konfigurationskanalnummer |
| 2 | Kennung `00`, Klartext `Messwert`; Byte gleichzeitig "Potenz" (Kommastellen-Exponent, bei mbc-93 immer `0`) |
| 3..6 | Farbcode je Anzeigebereich 1...4 (siehe `CAN_Befehlssatz_MBCAN.md`/CS2-Protokoll fuer die Farbcodierung) |
| 7,8 | Nullpunkt Highbyte, Nullpunkt Lowbyte |
| 9,10 | Bereich 1 Highbyte, Bereich 1 Lowbyte |
| 11,12 | Bereich 2 Highbyte, Bereich 2 Lowbyte |
| 13,14 | Bereich 3 Highbyte, Bereich 3 Lowbyte |
| 15,16 | Bereich 4 Highbyte, Bereich 4 Lowbyte |

Der aktuelle Messwert selbst steht NICHT im Initialblock, sondern wird erst
bei einer Statusabfrage im `D6:D7`-Wort der Antwort geliefert (siehe
Abschnitt 3, "Status-/Messwertrueckmeldung"). Hinter Byte 16 folgt wie bei den
Konfigurationstypen die `Messwertbezeichnung` als Text.

## 2. Konfigurationsdaten-Initialisierung je Modul

In den folgenden Tabellen sind die festen ersten acht Bytes der jeweiligen
40-Byte-Zeile angegeben. Die Bytes sind `Daten[1]...Daten[8]`; die aktuelle
Einstellung liegt normalerweise in `Daten[7]`/`Daten[8]`. Texte und Grenzwerte
folgen in der restlichen Zeile.

### mbc-80

Keine modulspezifischen Messwerte, zwei Konfigurationskanaele:

| Kanal | Initialbytes 1...8 | Inhalt und Bereich |
|---:|---|---|
| `7` | `07 02 00 01 00 FF 00 00` | Kanal 7, `Slider`, `SW Major:`, Untergrenze `1`, Obergrenze `255`, Highbyte `Daten[7] = 00`, aktuell `Daten[8]`, EEPROM `FF -> 1` |
| `8` | `08 02 00 01 00 FF 00 00` | Kanal 8, `Slider`, `SW Minor:`, Untergrenze `1`, Obergrenze `255`, Highbyte `Daten[7] = 00`, aktuell `Daten[8]`, EEPROM `FF -> 1` |

### mbc-81

| Kanal | Initialbytes 1...8 | Inhalt und Bereich |
|---:|---|---|
| `7` | `07 02 00 00 00 3F 00 00` | Kanal 7, `Slider`, `KeyB:`, `0...63`, MM-Adresse `0x3000 + 16*n`, aktuell `Daten[7:8]` |
| `8` | `08 01 03 00 00 00 00 00` | Kanal 8, `Auswahlliste`, `Nutz:`, 3 Punkte `Switch`/`B0 1..32`/`B0 33..63`, Index `Daten[4]` |

### mbc-82

**Sonderfall wie mbc-93:** mbc-82 hat ebenfalls modulspezifische Messwertkanaele
(`anzMesswerteModul=2`), siehe die Erklaerung im mbc-93-Abschnitt weiter unten - die
Kanaele 3/4 (Messwerte) muessen direkt hinter Kanal 1/2 einsortiert werden, die
Basiskanaele Seriennummer/GUiD/Firmware/Hardware verschieben sich dadurch von `3/4/5/6`
auf `5/6/7/8`:

| Kanal | Initialbytes | Inhalt und Bereich |
|---:|---|---|
| `3` | `03 00 30 30 30 30 00 00 00 00 00 00 00 00 00 01` | Kanal 3, `Messwert`, `ON Track`, Anzeigebereiche grau/gruen, aktueller Wert (0/1) per Statusabfrage |
| `4` | `04 00 C0 C0 C0 C0 00 00 00 00 00 00 00 00 00 01` | Kanal 4, `Messwert`, `Kurzschluss`, Anzeigebereiche grau/rot, aktueller Wert (0/1) per Statusabfrage |
| `9` | `09 02 00 01 00 FF 00 00` | Kanal 9, `Slider`, `SW Major:`, `1...255`, aktuell `Daten[7:8]`, simulierte GFP/Booster-Softwareversion (analog mbc-80/L88 bzw. mbc-93) |
| `10` | `0A 02 00 01 00 FF 00 00` | Kanal 10, `Slider`, `SW Minor:`, `1...255`, aktuell `Daten[7:8]` |
| `11` | `0B 02 00 01 00 14 00 00` | Kanal 11, `Slider`, `KS x 50 ms:`, `1...20`, aktuell `Daten[7:8]`, entspricht `50...1000` ms in 50-ms-Schritten (ParaCenter zeigt den realen ms-Wert, siehe `nud_mbc82_Latenz` in `frmMain.Designer.vb`) - Original-Firmware validiert diesen Kanal nicht, Port haertet analog mbc-93 |

### mbc-83

| Kanal | Initialbytes 1...8 | Inhalt und Bereich |
|---:|---|---|
| `7` | `07 02 00 00 00 FF 00 00` | Kanal 7, `Slider`, `MM-Addr:`, `0...255`, MM-Adresse `0x3000 + 4*n`, aktuell `Daten[7:8]` |
| `8` | `08 02 00 00 00 C8 00 00` | Kanal 8, `Slider`, `Auto-Aus x10ms:`, `0...200`, aktuell `Daten[7:8]` |
| `9` | `09 01 02 00 00 00 00 00` | Kanal 9, `Auswahlliste`, 2 Punkte `disabled/enabled`, Index `Daten[4]` |
| `10` | `0A 02 00 00 00 C8 00 00` | Kanal 10, `Slider`, `Dimmer:`, `0...200`, aktuell `Daten[7:8]` |
| `11` | `0B 01 03 00 00 00 00 00` | Kanal 11, `Auswahlliste`, 3 Punkte, Aktorart Sp1, Index `Daten[4]`, `0=Impuls`, `1=Wechsel`, `2=StartUp`, schreibbar |
| `12` | `0C 02 00 01 00 FA 00 00` | Kanal 12, `Slider`, Impulszeit Sp1, `1...250`, aktuell `Daten[7:8]` |
| `13` | `0D 01 03 00 00 00 00 00` | Kanal 13, `Auswahlliste`, 3 Punkte, Aktorart Sp2, Index `Daten[4]`, `0=Impuls`, `1=Wechsel`, `2=StartUp`, schreibbar |
| `14` | `0E 02 00 01 00 FA 00 00` | Kanal 14, `Slider`, Impulszeit Sp2, `1...250`, aktuell `Daten[7:8]` |
| `15` | `0F 01 03 00 00 00 00 00` | Kanal 15, `Auswahlliste`, 3 Punkte, Aktorart Sp3, Index `Daten[4]`, `0=Impuls`, `1=Wechsel`, `2=StartUp`, schreibbar |
| `16` | `10 02 00 01 00 FA 00 00` | Kanal 16, `Slider`, Impulszeit Sp3, `1...250`, aktuell `Daten[7:8]` |
| `17` | `11 01 03 00 00 00 00 00` | Kanal 17, `Auswahlliste`, 3 Punkte, Aktorart Sp4, Index `Daten[4]`, `0=Impuls`, `1=Wechsel`, `2=StartUp`, schreibbar |
| `18` | `12 02 00 01 00 FA 00 00` | Kanal 18, `Slider`, Impulszeit Sp4, `1...250`, aktuell `Daten[7:8]` |

### mbc-84

| Kanal | Initialbytes 1...8 | Inhalt und Bereich |
|---:|---|---|
| `7` | `07 02 00 00 00 FF 00 00` | Kanal 7, `Slider`, `MM-Addr:`, `0...255`, MM-Adresse `0x3000 + 4*n`, aktuell `Daten[7:8]` |
| `8` | `08 01 02 00 00 00 00 00` | Kanal 8, `Auswahlliste`, 2 Punkte `disabled/enabled`, Index `Daten[4]` |
| `9` | `09 02 00 00 00 1E 00 00` | Kanal 9, `Slider`, `Sv1 RT max`, `0...30`, aktuell `Daten[7:8]` |
| `10` | `0A 02 00 00 00 1E 00 00` | Kanal 10, `Slider`, `Sv1 GN max`, `0...30`, aktuell `Daten[7:8]` |
| `11` | `0B 02 00 00 00 0A 00 00` | Kanal 11, `Slider`, `Sv1 Delay`, `0...10`, aktuell `Daten[7:8]` |
| `12` | `0C 02 00 00 00 1E 00 00` | Kanal 12, `Slider`, `Sv2 RT max`, `0...30`, aktuell `Daten[7:8]` |
| `13` | `0D 02 00 00 00 1E 00 00` | Kanal 13, `Slider`, `Sv2 GN max`, `0...30`, aktuell `Daten[7:8]` |
| `14` | `0E 02 00 00 00 0A 00 00` | Kanal 14, `Slider`, `Sv2 Delay`, `0...10`, aktuell `Daten[7:8]` |
| `15` | `0F 02 00 00 00 1E 00 00` | Kanal 15, `Slider`, `Sv3 RT max`, `0...30`, aktuell `Daten[7:8]` |
| `16` | `10 02 00 00 00 1E 00 00` | Kanal 16, `Slider`, `Sv3 GN max`, `0...30`, aktuell `Daten[7:8]` |
| `17` | `11 02 00 00 00 0A 00 00` | Kanal 17, `Slider`, `Sv3 Delay`, `0...10`, aktuell `Daten[7:8]` |
| `18` | `12 02 00 00 00 1E 00 00` | Kanal 18, `Slider`, `Sv4 RT max`, `0...30`, aktuell `Daten[7:8]` |
| `19` | `13 02 00 00 00 1E 00 00` | Kanal 19, `Slider`, `Sv4 GN max`, `0...30`, aktuell `Daten[7:8]` |
| `20` | `14 02 00 00 00 0A 00 00` | Kanal 20, `Slider`, `Sv4 Delay`, `0...10`, aktuell `Daten[7:8]` |

### mbc-88

| Kanal | Initialbytes 1...8 | Inhalt und Bereich |
|---:|---|---|
| `7` | `07 02 00 01 00 5D 00 00` | Kanal 7, `Slider`, `S88 Modul:`, `1...93`, aktuell `Daten[7:8]` |
| `8` | `08 02 00 00 00 01 00 00` | Kanal 8, `Slider`, `Halbmodul:`, `0...1`, aktuell `Daten[7:8]` |
| `9` | `09 02 00 01 00 FA 00 00` | Kanal 9, `Slider`, `K1 x10ms:`, `1...250`, aktuell `Daten[7:8]` |
| `10` | `0A 02 00 01 00 FA 00 00` | Kanal 10, `Slider`, `K2 x10ms:`, `1...250`, aktuell `Daten[7:8]` |
| `11` | `0B 02 00 01 00 FA 00 00` | Kanal 11, `Slider`, `K3 x10ms:`, `1...250`, aktuell `Daten[7:8]` |
| `12` | `0C 02 00 01 00 FA 00 00` | Kanal 12, `Slider`, `K4 x10ms:`, `1...250`, aktuell `Daten[7:8]` |
| `13` | `0D 02 00 01 00 FA 00 00` | Kanal 13, `Slider`, `K5 x10ms:`, `1...250`, aktuell `Daten[7:8]` |
| `14` | `0E 02 00 01 00 FA 00 00` | Kanal 14, `Slider`, `K6 x10ms:`, `1...250`, aktuell `Daten[7:8]` |
| `15` | `0F 02 00 01 00 FA 00 00` | Kanal 15, `Slider`, `K7 x10ms:`, `1...250`, aktuell `Daten[7:8]` |
| `16` | `10 02 00 01 00 FA 00 00` | Kanal 16, `Slider`, `K8 x10ms:`, `1...250`, aktuell `Daten[7:8]` |

### mbc-89

**Sonderfall:** 69 Konfigkanaele, mit Abstand die meisten aller Module dieses Projekts - 5
"klassische" Kanaele (7...11) sowie 64 baugleiche "KeyBx GroupY:"-Kanaele (12...75, siehe unten).

| Kanal | Initialbytes 1...8 | Inhalt und Bereich |
|---:|---|---|
| `7` | `07 02 00 01 00 FF 00 00` | Kanal 7, `Slider`, `SW Major:`, `1...255`, aktuell `Daten[7:8]`, vorgetaeuschte Connect6021-Softwareversion (analog mbc-80/L88/mbc-82/93) |
| `8` | `08 02 00 01 00 FF 00 00` | Kanal 8, `Slider`, `SW Minor:`, `1...255`, aktuell `Daten[7:8]` |
| `9` | `09 01 02 00 00 00 00 00` | Kanal 9, `Auswahlliste`, `Mapping:`, 2 Punkte `Mapping`/`Mapping&Direkt`, Index `Daten[4]` (Zentralentyp: 0=CSx-Mapping per CONNECT6021, 1=MS2-Stil - alle Loks automatisch gemappt) |
| `10` | `0A 02 00 01 00 04 00 00` | Kanal 10, `Slider`, `Basis:`, `1...4`, aktuell `Daten[7:8]`, Keyboard-Basisadressblock |
| `11` | `0B 02 00 28 00 64 00 00` | Kanal 11, `Slider`, `I2C:`, `40...100`, aktuell `Daten[7:8]`, CU-Bus-I2C-Takt in kHz |
| `12...75` | `xx 01 02 00 00 00 00 00` | 64 gleichartige `Auswahlliste`-Kanaele, je 2 Punkte `MM`/`DCC`, Index `Daten[4]`. Kanal `12 + (keyboard-1)*4 + (gruppe-1)` fuer Keyboard `1...16` und Gruppe/Stellpultblock `1...4`, Label `KeyB<keyboard> Group<gruppe>:` (z.B. Kanal 12 = `KeyB1 Group1:`, Kanal 75 = `KeyB16 Group4:`) - legt das Zubehoerprotokoll (Motorola oder DCC) je Stellpultblock fest |

Alle 69 Kanaele liegen als PROGMEM-Vorlage in `initCs2Ex[]` (`Mbc89Com.cpp`), die aktuelle Zeile
wird bei jeder CSx-Anfrage frisch in einen einzigen `_cs2ScratchRow`-Puffer kopiert statt 69
dauerhafte 40-Byte-Zeilen im RAM zu halten.

### mbc-90

| Kanal | Initialbytes 1...8 | Inhalt und Bereich |
|---:|---|---|
| `7` | `07 02 00 00 00 5D 00 00` | Kanal 7, `Slider`, `S88 Modul:`, `0...93`, aktuell `Daten[7:8]` |
| `8` | `08 02 00 01 00 FA 00 00` | Kanal 8, `Slider`, `K1 x10ms:`, `1...250`, aktuell `Daten[7:8]` |
| `9` | `09 02 00 01 00 FA 00 00` | Kanal 9, `Slider`, `K2 x10ms:`, `1...250`, aktuell `Daten[7:8]` |
| `10` | `0A 02 00 01 00 FA 00 00` | Kanal 10, `Slider`, `K3 x10ms:`, `1...250`, aktuell `Daten[7:8]` |
| `11` | `0B 02 00 01 00 FA 00 00` | Kanal 11, `Slider`, `K4 x10ms:`, `1...250`, aktuell `Daten[7:8]` |
| `12` | `0C 02 00 01 00 FA 00 00` | Kanal 12, `Slider`, `K5 x10ms:`, `1...250`, aktuell `Daten[7:8]` |
| `13` | `0D 02 00 01 00 FA 00 00` | Kanal 13, `Slider`, `K6 x10ms:`, `1...250`, aktuell `Daten[7:8]` |
| `14` | `0E 02 00 01 00 FA 00 00` | Kanal 14, `Slider`, `K7 x10ms:`, `1...250`, aktuell `Daten[7:8]` |
| `15` | `0F 02 00 01 00 FA 00 00` | Kanal 15, `Slider`, `K8 x10ms:`, `1...250`, aktuell `Daten[7:8]` |
| `16` | `10 02 00 01 00 FA 00 00` | Kanal 16, `Slider`, `K9 x10ms:`, `1...250`, aktuell `Daten[7:8]` |
| `17` | `11 02 00 01 00 FA 00 00` | Kanal 17, `Slider`, `K10 x10ms:`, `1...250`, aktuell `Daten[7:8]` |
| `18` | `12 02 00 01 00 FA 00 00` | Kanal 18, `Slider`, `K11 x10ms:`, `1...250`, aktuell `Daten[7:8]` |
| `19` | `13 02 00 01 00 FA 00 00` | Kanal 19, `Slider`, `K12 x10ms:`, `1...250`, aktuell `Daten[7:8]` |
| `20` | `14 02 00 01 00 FA 00 00` | Kanal 20, `Slider`, `K13 x10ms:`, `1...250`, aktuell `Daten[7:8]` |
| `21` | `15 02 00 01 00 FA 00 00` | Kanal 21, `Slider`, `K14 x10ms:`, `1...250`, aktuell `Daten[7:8]` |
| `22` | `16 02 00 01 00 FA 00 00` | Kanal 22, `Slider`, `K15 x10ms:`, `1...250`, aktuell `Daten[7:8]` |
| `23` | `17 02 00 01 00 FA 00 00` | Kanal 23, `Slider`, `K16 x10ms:`, `1...250`, aktuell `Daten[7:8]` |

### mbc-91

| Kanal | Initialbytes 1...8 | Inhalt und Bereich |
|---:|---|---|
| `7` | `07 02 00 00 00 FF 00 00` | Kanal 7, `Slider`, `0...255`, MM-Adresse `0x3000 + 4*n`, aktuell `Daten[7:8]` |
| `8` | `08 02 00 00 00 C8 00 00` | Kanal 8, `Slider`, Auto-Aus, `0...200`, aktuell `Daten[7:8]` |
| `9` | `09 01 02 00 00 00 00 00` | Kanal 9, `Auswahlliste`, Meldezyklus, Index `Daten[4]` |
| `10` | `0A 01 01 00 00 00 00 00` | Kanal 10, `Auswahlliste`, 1 Punkt, Aktorart Spule 1 `Impuls/Wechsel`, Index `Daten[4]`, nur Anzeige |
| `11` | `0B 02 00 01 00 FA 00 00` | Kanal 11, `Slider`, Impulszeit Spule 1, `1...250`, aktuell `Daten[7:8]` |
| `12` | `0C 01 01 00 00 00 00 00` | Kanal 12, `Auswahlliste`, 1 Punkt, Aktorart Spule 2 `Impuls/Wechsel`, Index `Daten[4]`, nur Anzeige |
| `13` | `0D 02 00 01 00 FA 00 00` | Kanal 13, `Slider`, Impulszeit Spule 2, `1...250`, aktuell `Daten[7:8]` |
| `14` | `0E 01 01 00 00 00 00 00` | Kanal 14, `Auswahlliste`, 1 Punkt, Aktorart Spule 3 `Impuls/Wechsel`, Index `Daten[4]`, nur Anzeige |
| `15` | `0F 02 00 01 00 FA 00 00` | Kanal 15, `Slider`, Impulszeit Spule 3, `1...250`, aktuell `Daten[7:8]` |
| `16` | `10 01 01 00 00 00 00 00` | Kanal 16, `Auswahlliste`, 1 Punkt, Aktorart Spule 4 `Impuls/Wechsel`, Index `Daten[4]`, nur Anzeige |
| `17` | `11 02 00 01 00 FA 00 00` | Kanal 17, `Slider`, Impulszeit Spule 4, `1...250`, aktuell `Daten[7:8]` |

### mbc-92

| Kanal | Initialbytes 1...8 | Inhalt und Bereich |
|---:|---|---|
| `7` | `07 02 00 00 00 FF 00 00` | Kanal 7, `Slider`, `0...255`, MM-Adresse `0x3000 + 4*n`, aktuell `Daten[7:8]` |
| `8` | `08 02 00 00 00 C8 00 00` | Kanal 8, `Slider`, Auto-Aus, `0...200`, aktuell `Daten[7:8]` |
| `9` | `09 01 02 00 00 00 00 00` | Kanal 9, `Auswahlliste`, Meldezyklus, Index `Daten[4]` |
| `10` | `0A 01 02 00 00 00 00 00` | Kanal 10, `Auswahlliste`, 2 Punkte, Aktorart Spule 1, Index `Daten[4]`, `0=Impuls`, `1=Wechsel`, schreibbar |
| `11` | `0B 02 00 01 00 FA 00 00` | Kanal 11, `Slider`, Impulszeit Spule 1, `1...250`, aktuell `Daten[7:8]` |
| `12` | `0C 01 02 00 00 00 00 00` | Kanal 12, `Auswahlliste`, 2 Punkte, Aktorart Spule 2, Index `Daten[4]`, `0=Impuls`, `1=Wechsel`, schreibbar |
| `13` | `0D 02 00 01 00 FA 00 00` | Kanal 13, `Slider`, Impulszeit Spule 2, `1...250`, aktuell `Daten[7:8]` |
| `14` | `0E 01 02 00 00 00 00 00` | Kanal 14, `Auswahlliste`, 2 Punkte, Aktorart Spule 3, Index `Daten[4]`, `0=Impuls`, `1=Wechsel`, schreibbar |
| `15` | `0F 02 00 01 00 FA 00 00` | Kanal 15, `Slider`, Impulszeit Spule 3, `1...250`, aktuell `Daten[7:8]` |
| `16` | `10 01 02 00 00 00 00 00` | Kanal 16, `Auswahlliste`, 2 Punkte, Aktorart Spule 4, Index `Daten[4]`, `0=Impuls`, `1=Wechsel`, schreibbar |
| `17` | `11 02 00 01 00 FA 00 00` | Kanal 17, `Slider`, Impulszeit Spule 4, `1...250`, aktuell `Daten[7:8]` |

### mbc-93

**Sonderfall:** mbc-93 ist das einzige Modul dieses Projekts mit modulspezifischen
Messwertkanaelen (`anzMesswerteModul=2` statt sonst immer `0`). Die CSx zeigt die ersten
`Basis-Messwerte + anzMesswerteModul` Kanaele als Messwerte an, unabhaengig von deren eigener
Kennung - modulspezifische Messwertkanaele muessen deshalb direkt hinter die Basis-Messwerte
(Kanal 1/2, Versorgungsspannung/Temperatur) einsortiert werden, NICHT hinter Kanal 6 wie bei
reinen Konfigkanaelen. `MbcCom` erledigt das automatisch (siehe `checkCs2Connection()` in
`MbcCom.cpp`) und verschiebt dabei die Basiskanaele Seriennummer/GUiD/Firmware/Hardware von
`3/4/5/6` auf `5/6/7/8`:

| Kanal | Initialbytes | Inhalt und Bereich |
|---:|---|---|
| `3` | `03 00 30 30 30 30 00 00 00 00 00 00 00 00 00 01` | Kanal 3, `Messwert`, `ON Track`, Anzeigebereiche grau/gruen, aktueller Wert (0/1) per Statusabfrage |
| `4` | `04 00 C0 C0 C0 C0 00 00 00 00 00 00 00 00 00 01` | Kanal 4, `Messwert`, `Kurzschluss`, Anzeigebereiche grau/rot, aktueller Wert (0/1) per Statusabfrage |
| `9` | `09 02 00 01 00 FF 00 00` | Kanal 9, `Slider`, `SW Major:`, `1...255`, aktuell `Daten[7:8]`, simulierte GFP/Booster-Softwareversion (analog mbc-80/L88) |
| `10` | `0A 02 00 01 00 FF 00 00` | Kanal 10, `Slider`, `SW Minor:`, `1...255`, aktuell `Daten[7:8]` |
| `11` | `0B 02 00 01 00 14 00 00` | Kanal 11, `Slider`, `KS x 50 ms:`, `1...20`, aktuell `Daten[7:8]`, entspricht `50...1000` ms in 50-ms-Schritten (ParaCenter zeigt den realen ms-Wert, siehe `nud_mbc93_Latenz` in `frmMain.Designer.vb`) |

Die 3 Konfigkanaele (9/10/11) liegen hinter den auf 5/6/7/8 verschobenen Basis-Konfigkanaelen -
die Gesamtzahl aller Kanaele (2 Basis-Messwerte + 2 Modul-Messwerte + 4 Basis-Konfig + 3
Modul-Konfig = 11) bestimmt die letzte Kanalnummer.

## 3. Parameterrueckmeldungen

### Parameter schreiben: Anfrage

Die Parameter-Schreibnachricht hat folgendes Format:

```text
CSx->Modul: 00 00 hh hh 08 G0 G1 G2 G3 0B Kanal Wert_H Wert_L
```

Dabei gilt:

- `CAN-ID = 00`: Systembefehl.
- `D0...D3`: GUiD-/CS2-Kennung, die das Zielmodul identifiziert.
- `D4 = 0B`: Subcode fuer Messwert-/Parameterdaten.
- `D5`: CS2-Kanalnummer des Parameters.
- `D6:D7`: Parameterwert als 16-Bit-Wort, Highbyte zuerst.
- `DLC = 08` bei der Schreibanforderung.

Die jeweils zulaessigen Kanalbereiche und Werte stehen direkt in den
Modultabellen.

### Parameterquittung

Nach erfolgreicher oder abgelehnter Verarbeitung antwortet das Modul mit:

```text
Modul->CSx: 00 01 hh hh 07 G0 G1 G2 G3 0B Kanal Status 00
```

`G0...G3` sind die GUiD-Kennung des Moduls. `Status = 01` bedeutet gueltig
uebernommen, `Status = 00` bedeutet ungueltig oder ausserhalb des erlaubten
Bereichs. Die Antwort wird aus dem jeweiligen `initMsgKonfigOk`-Template
aufgebaut; Hash und GUiD werden zur Laufzeit eingesetzt.

### Status-/Messwertrueckmeldung

Eine Abfrage verwendet denselben Systembefehl, aber `DLC = 06` und den
angeforderten Kanal in `D5`:

```text
CSx->Modul: 00 00 hh hh 06 G0 G1 G2 G3 0B Kanal 00 00
```

Die Antwort hat `DLC = 08`:

```text
Modul->CSx: 00 01 hh hh 08 G0 G1 G2 G3 0B Kanal Wert_H Wert_L
```

Die Antwort enthaelt bei einer Statusabfrage den aktuellen 16-Bit-Wert in
`D6:D7`.
