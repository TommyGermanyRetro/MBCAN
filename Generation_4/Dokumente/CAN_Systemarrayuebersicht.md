# Systemarray - Aufbau und PC_ARRAY_DATA-Protokoll

Diese Dokumentation beschreibt das Systemarray der Module `mbc-80`, `mbc-81`,
`mbc-83`, `mbc-84`, `mbc-88`, `mbc-90`, `mbc-91`, `mbc-92` sowie den
CAN-Nachrichtenaustausch, mit dem das ParaCenter dieses Systemarray liest,
beschreibt und im internen EEPROM persistiert (`PC_ARRAY`/`PC_ARRAY_DATA`).
Sie ergaenzt `CAN_Konfigurationsuebersicht.md`, die die davon unabhaengige
CS2-Konfigurationskanal-Kommunikation beschreibt.

## 1. Allgemeiner Aufbau des Systemarrays

Das Systemarray ist ein einziger, zusammenhaengender Byte-Puffer, den der
Sketch als global/statisch angelegten Speicher an `MbcUtils` uebergibt
(`MBC_SYSARRAY_SIZE(genPurpSize)`, `MbcUtils.h`). Es gliedert sich in einen
fuer alle Module identischen Kopfblock, einen Boot-Staging-Bereich und einen
modulspezifischen General-Purpose-Bereich (`genPurp`):

| Offset | Feld | Bytes | Bedeutung |
|---|---|---:|---|
| 1..8 | Seriennummer | 8 | `MBC_S_SNR` |
| 9..12 | Artikelnummer | 4 | `MBC_S_ART` |
| 13..15 | Softwareversion | 3 | `MBC_S_SW` |
| 16..21 | Hardwareversion | 6 | `MBC_S_HW` |
| 22..41 | Namensblock | 20 | `MBC_S_NAMEBLOCK`, Ende des Infoblocks (`MBC_INFO_ENDE`) |
| 42..45 | GUiD | 4 | `MBC_S_GUID` |
| 46..57 | Datenbankeintrag | 12 | `MBC_S_DB` |
| 58..62 | Kennung (CS2-Anmeldung) | 5 | `MBC_S_KN` |
| 63..69 | Parametrierstatus | 7 | `MBC_S_PARA`, Ende des Allgemeinblocks (`MBC_ALLG_ENDE`) |
| 70..327 | Boot-Staging-Bereich (Firmware-Upgrade) | 258 | `MBC_S_BOOT`, `MBC_BOOT_PGSZ=256` + 2 Steuerbyte |
| ab 328 | genPurp - modulspezifischer Bereich | `genPurpSize` | `MBC_S_GENPURP`, siehe Abschnitt 3 |

Die genauen Offsets ergeben sich additiv aus den `MBC_L_*`/`MBC_S_*`-Makros in
`MBCANLib/src/MbcUtils.h` und sind fuer alle Module identisch; nur die Groesse
des genPurp-Bereichs (`genPurpSize`) und dessen Inhalt sind modulspezifisch.
Der Infoblock (Seriennummer...Namensblock...GUiD...Kennung...Parametrierstatus)
wird ueber `MbcUtils::eepromRead()`/`eepromWrite(block)` mit den Blocknummern
`MBC_EE_NAME=0x01`, `MBC_EE_GUID=0x02`, `MBC_EE_KENNUNG=0x03`, `MBC_EE_DB=0x04`
persistiert - unabhaengig vom modulspezifischen genPurp-Bereich.

## 2. PC_ARRAY_DATA-Protokoll (Lesen/Schreiben des Systemarrays)

Das ParaCenter greift auf das Systemarray ueber denselben Loc-ID-Rahmen wie
die CS2-Konfigurationskanaele zu (`CAN-ID=16`, `D2=18` = `CAN_LOCID_MBCAN`),
aber mit eigenen Subcodes in `D3`:

| Subcode | Wert | Richtung | Bedeutung |
|---|---:|---|---|
| `PC_ARRAY` | `0B` | CSx/PC->Modul | Transfersitzung eroeffnen |
| `MD_ARRAY` | `0C` | Modul->PC | Sitzung bestaetigt |
| `PC_ARRAY_DATA` | `0D` | PC->Modul | Byteweiser Lese-/Schreibzugriff bzw. Sitzungssteuerung |
| `MD_ARRAY_DATA` | `0E` | Modul->PC | Quittung je Byte |
| `PC_UPGRADE`/`MD_UPGRADE` | `0F`/`10` | beidseitig | Firmware-Upgrade-Sitzung (eigener Rahmen, siehe unten) |
| `PC_UPGRADE_DATA`/`MD_UPGRADE_DATA` | `11`/`12` | beidseitig | Firmware-Upgrade-Nutzdaten |
| `PC_BOOT` | `13` | PC->Modul | Bootloader scharfschalten, Modul startet neu |

### 2.1 Sitzung eroeffnen

Das ParaCenter sendet `PC_ARRAY` (`D3=0B`); das Modul quittiert mit
`MD_ARRAY` (`D3=0C`) und setzt intern `transfer=true`. Ein erneutes
`PC_ARRAY`/`PC_UPGRADE` waehrend einer laufenden Sitzung bricht diese
stillschweigend ab (keine Quittung).

### 2.2 Richtung ankuendigen

Anschliessend sendet das ParaCenter genau eine `PC_ARRAY_DATA`-Nachricht mit
einem reservierten Sonderindex in `D5:D6` (16-Bit big-endian) statt eines
echten Array-Offsets:

| Sonderindex | Wert | Bedeutung |
|---|---:|---|
| `MBC_COM_READ` | `FFFE` | Lesesitzung starten (`D7=1`) bzw. beenden (`D7=0`) |
| `MBC_COM_WRITE` | `FFFF` | Schreibsitzung starten (`D7=1`) bzw. beenden (`D7=0`) |
| `MBC_COM_EEPROM` | `FFFD` | Persistiere EEPROM-Block Nr. `D7` (siehe 2.4) |

### 2.3 Byteweiser Zugriff

Je Systemarray-Byte laeuft ein eigener CAN-Umlauf, immer mit `D3=0D`
(Anfrage) und `D3=0E` (Quittung), `DLC=08`:

```text
D4: 1 = Byte schreiben, sonst = Byte lesen
D5,D6: 16-Bit Systemarray-Index (big-endian)
D7: zu schreibender Wert (Schreiben) bzw. gelesener/gespiegelter Wert (Quittung)
```

Das Modul quittiert `D4`/`D5`/`D6` immer unveraendert und legt bei einem
Lesezugriff den aktuellen Arraywert in `D7`. Ein Index ausserhalb von
`0...arrayMax()-1` wird zwar weiterhin quittiert, das eigentliche
Lesen/Schreiben des Arrays entfaellt dann aber stillschweigend - `arrayMax()`
ist `MBC_S_GENPURP + genPurpSize` (siehe Abschnitt 3, insbesondere die
mbc-93-Abweichung am Ende).

Das ParaCenter durchlaeuft fuer einen Block so den kompletten adressierten
Bereich (typischerweise `MBC_S_GENPURP...MBC_S_GENPURP+genPurpSize-1` fuer
modulspezifische Parameter, der Boot-Staging-Bereich wird beim allgemeinen
Lesen explizit uebersprungen).

### 2.4 EEPROM-Persistierung

Byteweises Lesen/Schreiben aendert nur den RAM-Spiegel (das Systemarray
selbst). Persistiert wird erst durch eine weitere `PC_ARRAY_DATA`-Nachricht
mit Sonderindex `MBC_COM_EEPROM` (`FFFD`) und der Blocknummer in `D7`, direkt
im Anschluss an die zugehoerigen Byteschreibzugriffe:

- Blocknummern `< 10` (`0x01...0x04`) betreffen den gemeinsamen Infoblock und
  werden direkt von `MbcUtils::eepromWrite(block)` verarbeitet.
- Blocknummern `>= 10` (ueblicherweise ab `0x0A`) betreffen den
  modulspezifischen genPurp-Bereich und werden an die virtuelle Funktion
  `eepromWriteParaExtern(block)` der jeweiligen `MbcXXCom`-Unterklasse
  weitergereicht. Diese kopiert die zugehoerigen genPurp-Bytes in eine
  eigene, kleinere EEPROM-Zusatzregion (`MbcUtils::eepromWriteExtra()`,
  Groesse `extraEepromSize` aus dem `MbcUtils`-Konstruktor) - deren interne
  Byte-Offsets sind reine Implementierungsdetails und stimmen nicht mit den
  genPurp-Offsets aus Abschnitt 3 ueberein.

Es gibt also keine eigene "Schreiben mit EEPROM-Block"-Nachrichtenart -
das Persistieren eines Blocks ist dieselbe `PC_ARRAY_DATA`-Nachricht wie ein
normaler Schreibzugriff, nur mit dem reservierten Index `FFFD` und der
Blocknummer statt eines Datenbyte in `D7`.

### 2.5 Sitzung schliessen

Die Sitzung wird durch dieselbe Lese-/Schreib-Sonderindex-Nachricht mit
`D7=0` beendet; das Modul setzt seine internen Sitzungsflags zurueck.

### 2.6 Abgrenzung zum Firmware-Upgrade

Das Firmware-Upgrade laeuft ueber einen eigenen Sitzungsrahmen
(`PC_UPGRADE`/`MD_UPGRADE`, `PC_UPGRADE_DATA`/`MD_UPGRADE_DATA`) und adressiert
den Boot-Staging-Bereich (`MBC_S_BOOT...MBC_BOOT_ENDE`) ueber `HASH_H`/`HASH_L`
statt `D5:D6`, mit 4 Nutzdatenbyte je Nachricht (`D4...D7`) statt einem
einzelnen Byte. `PC_BOOT` schaltet abschliessend den Bootloader scharf und
startet das Modul neu. Das vollstaendige Upgrade-Chunk-Protokoll ist nicht
Gegenstand dieser Dokumentation.

## 3. Belegung des genPurp-Bereichs je Modul

Die folgenden Tabellen beginnen jeweils bei Offset `0` = erstes Byte ab
`MBC_S_GENPURP`. Verbindliche Quelle ist die jeweilige `mbc_modul_XX.vb` im
ParaCenter (dort werden dieselben Offsets ueber `PC_ARRAY_DATA` adressiert);
die Firmware-Klassen `MbcXXCom` bilden dasselbe Layout nach.

### mbc-80

| Offset | Feld | Bytes |
|---|---|---:|
| 0..3 | BT-Passwort (Bluetooth-Altlast, ungenutzt) | 4 |
| 4..8 | BT-Name (Bluetooth-Altlast, ungenutzt) | 5 |
| 9 | Simulierte L88-SW-Version, Major | 1 |
| 10 | Simulierte L88-SW-Version, Minor | 1 |

`genPurpSize=11`. EEPROM-Blockkonstanten: `EE_L88_SW=0x0C` (einzig
implementierter Block; die BT-Altlastblocks `EE_BT_Passwort=0x0A`/
`EE_BT_Name=0x0B` sind in der VB definiert, aber in `eepromWriteParaExtern()`
nicht umgesetzt).

### mbc-81

| Offset | Feld | Bytes |
|---|---|---:|
| 0 | MM-Addr High | 1 |
| 1 | MM-Addr Low | 1 |
| 2 | Nutzung (`0`=Switchboard, `1`/`2`=Bus-0-Matrix-Slot) | 1 |

`genPurpSize=3`. EEPROM-Blockkonstanten: `EE_81_MMAddr=0x0A`, `EE_81_Nutzung=0x0B`.
Zusaetzlich per CS2-Konfigkanal setzbar (Label `KeyB:` fuer MM-Addr, Faktor 16 statt
der bei mbc-83/91/92 ueblichen 4, da mbc-81 16 statt 4 MM-Adressen je Modul belegt;
Label `Nutz:` fuer die Nutzung-Auswahl) - siehe `CAN_Konfigurationsuebersicht.md`.

### mbc-83

| Offset | Feld | Bytes |
|---|---|---:|
| 0 | MM-Addr High | 1 |
| 1 | MM-Addr Low | 1 |
| 2 | Aktorstellung (MM) | 1 |
| 3 | Art der Ausgaenge (MM_ART) | 1 |
| 4 | Auto-Aus (AUS) | 1 |
| 5 | Zwangsrueckmeldung (RUECK) | 1 |
| 6 | Dimmerstellung (DIMM) | 1 |
| 7..10 | Spulentiming TIO[0..3] | 4 |

`genPurpSize=11`. EEPROM-Blockkonstanten: `EE_83_MMAddr=0x0A`,
`EE_83_MM_TIMING=0x0B`, `EE_83_MM_ART=0x0C`, `EE_83_AUS=0x0D`,
`EE_83_RUECK=0x0E`, `EE_83_DIMM=0x0F`.

### mbc-84

| Offset | Feld | Bytes |
|---|---|---:|
| 0 | MM-Addr High | 1 |
| 1 | MM-Addr Low | 1 |
| 2 | Aktorstellung (Bit `2n`=Gruen, `2n+1`=Rot, je Servo `n=0..3`) | 1 |
| 3 | Zwangsrueckmeldung (RUECK) | 1 |
| 4,5 | Sv1 Rot-Endanschlag, Sv1 Gruen-Endanschlag | 2 |
| 6,7 | Sv2 Rot-Endanschlag, Sv2 Gruen-Endanschlag | 2 |
| 8,9 | Sv3 Rot-Endanschlag, Sv3 Gruen-Endanschlag | 2 |
| 10,11 | Sv4 Rot-Endanschlag, Sv4 Gruen-Endanschlag | 2 |
| 12 | Center (feste Firmware-Konstante, nicht editierbar/persistiert, nur fuers ParaCenter-Diagramm) | 1 |
| 13 | Sv1 Verzoegerung (VZ) | 1 |
| 14 | Sv2 Verzoegerung (VZ) | 1 |
| 15 | Sv3 Verzoegerung (VZ) | 1 |
| 16 | Sv4 Verzoegerung (VZ) | 1 |
| 17..496 | Servoweg-Tabelle (4 Servos x 60 Schritte x 2 [Rot-Weg + Gruen-Weg]) | 480 |

`genPurpSize=497`. EEPROM-Blockkonstanten: `EE_84_MMAddr=0x0A`,
`EE_84_AKTOR=0x0B`, `EE_84_SERVOSMAX=0x0C`, `EE_84_SERVOSWEG=0x0D`,
`EE_84_RUECK=0x0E`.

### mbc-88

| Offset | Feld | Bytes |
|---|---|---:|
| 0 | S88-Adresse High | 1 |
| 1 | S88-Adresse Low | 1 |
| 2 | Halbmodul (`0`=Kontakte 1...8, `1`=Kontakte 9...16) | 1 |
| 3 | Rueckmeldestatus (RM, invertiert, live Wert, nicht EEPROM-persistiert) | 1 |
| 4..11 | Integrationszeit je Kontakt K1..K8 (Rohwert x10ms) | 8 |

`genPurpSize=12`. EEPROM-Blockkonstanten: `EE_88_S88ADDR=0x0A` (Offset 0-1),
`EE_88_S88TIMING=0x0B` (Offset 4-11), `EE_88_S88MOD=0x0C` (Offset 2). Fuer
Offset 3 (RM) existiert kein EEPROM-Block.

### mbc-90

| Offset | Feld | Bytes |
|---|---|---:|
| 0 | S88-Adresse High | 1 |
| 1 | S88-Adresse Low | 1 |
| 2 | Rueckmeldestatus High (RM_H, live Wert, nicht EEPROM-persistiert) | 1 |
| 3 | Rueckmeldestatus Low (RM_L, live Wert, nicht EEPROM-persistiert) | 1 |
| 4..19 | Integrationszeit je Kontakt K1..K16 (Rohwert x10ms) | 16 |

`genPurpSize=20`. EEPROM-Blockkonstanten: `EE_90_S88ADDR=0x0A` (Offset 0-1),
`EE_90_S88TIMING=0x0B` (Offset 4-19). Fuer Offset 2-3 (RM_H/RM_L) existiert
kein EEPROM-Block. Anders als mbc-88 gibt es kein Halbmodul-Feld, mbc-90 ist
immer ein vollstaendiges 16-Kontakt-Modul.

### mbc-91

| Offset | Feld | Bytes |
|---|---|---:|
| 0 | MM-Addr High | 1 |
| 1 | MM-Addr Low | 1 |
| 2 | Aktorstellung (MM) | 1 |
| 3 | Art der Aktoren (MM_ART) - rein informativ/ReadOnly, einmalig hardwareerkannt | 1 |
| 4 | Auto-Aus (AUS) | 1 |
| 5 | Zwangsrueckmeldung (RUECK) | 1 |
| 6..9 | Impulszeiten TIO[0..3] je Doppelspulenantrieb (x10ms) | 4 |

`genPurpSize=10`. EEPROM-Blockkonstanten: `EE_91_MMAddr=0x0A`,
`EE_91_MM_TIMING=0x0B`, `EE_91_AUS=0x0D`, `EE_91_RUECK=0x0E`,
`EE_91_AKTOR=0x0F`. Die Luecke bei `0x0C` (`EE_91_MM_ART`) ist beabsichtigt:
Offset 3 ist auf beiden Seiten nur lesbar, wird also nie als Schreibblock
gesendet.

### mbc-92

| Offset | Feld | Bytes |
|---|---|---:|
| 0 | MM-Addr High | 1 |
| 1 | MM-Addr Low | 1 |
| 2 | Aktorstellung (MM) | 1 |
| 3 | Art der Aktoren (MM_ART) - editierbar (`0`=Impuls/`1`=Wechsel), anders als mbc-91 | 1 |
| 4 | Auto-Aus (AUS) | 1 |
| 5 | Zwangsrueckmeldung (RUECK) | 1 |
| 6..9 | Impulszeiten TIO[0..3] je Doppelspulenantrieb (x10ms) | 4 |

`genPurpSize=10`. EEPROM-Blockkonstanten: `EE_92_MMAddr=0x0A`,
`EE_92_MM_TIMING=0x0B`, `EE_92_MM_ART=0x0C`, `EE_92_AUS=0x0D`,
`EE_92_RUECK=0x0E`, `EE_92_AKTOR=0x0F`.
