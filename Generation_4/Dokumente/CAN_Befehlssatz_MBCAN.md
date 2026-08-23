# CAN-Befehlssatz MBCAN

Diese Dokumentation beschreibt den vollstaendigen internen Loc-ID-Befehlssatz
(`CAN-ID=16`, `D2=18` = `CAN_LOCID_MBCAN`), mit dem die MBCAN-Module
untereinander und mit dem Parametriercenter kommunizieren - erzeugt aus
`9999_CAN_Dokumentation/2024-10-20 MBCAN - Befehlssatz.xlsx`. Sie ergaenzt
`CAN_Konfigurationsuebersicht.md` (CS2-Konfigurationskanal-Protokoll) und
`CAN_Systemarrayuebersicht.md` (PC_ARRAY_DATA-Sitzungsprotokoll im Detail -
die Befehle `PC_ARRAY`/`MD_ARRAY`/`PC_ARRAY_DATA`/`MD_ARRAY_DATA` sind dort
bereits ausfuehrlich mit Byteformat erklaert, hier nur als Referenzeintrag
gelistet).

Jede Nachricht folgt dem gemeinsamen 13-Byte-Format `PRIO CAN-ID HASH_H HASH_L
DLC D0 D1 D2 D3 D4 D5 D6 D7`. D0...D3 tragen bei den meisten Befehlen entweder
die Loc-ID (`0x000018xx`, an ALLE MBCAN-Module gerichtet) oder die GUiD eines
konkreten Zielmoduls, siehe jeweilige Spalte "Genutzte Datenbytes".

## Uebersicht

| Befehl | Sender | Loc-ID | Funktion | Antwort |
|---|---|---|---|---|
| `PC_DB_H` | PC | `0x00001800` | Uebertragung des Datenbanknamens mit 12 Bytes | -/- |
| `PC_DB_M` | PC | `0x00001801` | Uebertragung des Datenbanknamens mit 12 Bytes | -/- |
| `PC_DB_L` | PC | `0x00001802` | Uebertragung des Datenbanknamens mit 12 Bytes | -/- |
| `PC_KENNER` | PC | `0x00001803` | Kenner und Identifier fuer die Module | -/- |
| `PC_NEU` | PC | `0x00001804` | Neuanmeldeaufforderung | `MD_NEU_DATA` |
| `PC_NEU_DATA` | PC | `0x00001805` | Rueckmeldung des PC an das Modul waehrend des Neuanmeldeprozesses | `MD_NEU_DATA` |
| `MD_NEU_DATA` | Modul | `0x00001806` | Meldung des Moduls waehrend des Neuanmeldeprozesses | `PC_NEU_DATA` |
| `PC_RESET` | PC | `0x00001807` | Durchfuehren eines Hardware-Resets auf dem Modul | -/- |
| `PC_MD_DEL` | PC | `0x00001808` | Modul aus Datenbank entfernen | -/- |
| `PC_ALIVE` | PC | `0x00001809` | ALIVE-Abfrage | `MD_ALIVE` |
| `MD_ALIVE` | Modul | `0x0000180A` | ALIVE-Abfrage | -/- |
| `PC_ARRAY` | PC | `0x0000180B` | Zugriff Systemarray anfragen | `MD_ARRAY` |
| `MD_ARRAY` | Modul | `0x0000180C` | Zugriff Systemarray freigegeben | -/- |
| `PC_ARRAY_DATA` | PC | `0x0000180D` | Zugriff Systemarray freigegeben | `MD_ARRAY_DATA` |
| `MD_ARRAY_DATA` | Modul | `0x0000180E` | Antwort des Moduls auf Systemarray-Zugriff | -/- |
| `PC_UPGRADE` | PC | `0x0000180F` | Firmware-Upgrade | `MD_UPGRADE` |
| `MD_UPGRADE` | Modul | `0x00001810` | Firmware-Upgrade freigeben | -/- |
| `PC_UPGRADE_DATA` | PC | `0x00001811` | Schreibe Firmware | `MD_UPGRADE_DATA` |
| `MD_UPGRADE_DATA` | Modul | `0x00001812` | Antwort des Moduls auf Schreibe Firmware | -/- |
| `PC_BOOT` | PC | `0x00001813` | Modul neu Booten | -/- |
| `MD_S88` | Modul | `0x00001814` | Stellungsmeldung mbc-88 / mbc-90 | -/- |
| `MD_ZYKLUS` | Modul | `0x00001815` | Anstoss eines Rueckmeldezyklus bei anderen MBCAN-Modulen (nur interner Sub-Bus) | -/- |

## Befehle im Detail

Sortiert nach Loc-ID (aufsteigend).

### PC_DB_H

- **Sender:** PC
- **Loc-ID:** `0x00001800`
- **Funktion:** Uebertragung des Datenbanknamens mit 12 Bytes
- **Antwort:** -/-

Bytes 1 bis 6 stellen das Datum, Bytes 7 bis 12 die Uhrzeit dar.

Beispielstring:

"070917235340" = am 07.09.2017 um 23:53:40 wurde die Datenbank erstellt. Die einzelnen Bytes werden in ASCII-Werte uebersetzt und dann uebertragen.

Dieser String findet sich auch im Dateinamen der exportierten Datenbank aus dem Parametriercenter.

**Genutzte Datenbytes:**

| Bytes | Bedeutung |
|---|---|
| `D0...D3` | Loc-ID 0x00001800 |
| `D4...D7` | Bytes 1-4 des Datum/Uhrzeit-Strings |

**Beispielnachricht:**

```text
00 16 5F 38 08 00 00 18 00 30 37 30 39
```

### PC_DB_M

- **Sender:** PC
- **Loc-ID:** `0x00001801`
- **Funktion:** Uebertragung des Datenbanknamens mit 12 Bytes
- **Antwort:** -/-

Bytes 1 bis 6 stellen das Datum, Bytes 7 bis 12 die Uhrzeit dar.

Beispielstring:

"070917235340" = am 07.09.2017 um 23:53:40 wurde die Datenbank erstellt. Die einzelnen Bytes werden in ASCII-Werte uebersetzt und dann uebertragen.

Dieser String findet sich auch im Dateinamen der exportierten Datenbank aus dem Parametriercenter.

**Genutzte Datenbytes:**

| Bytes | Bedeutung |
|---|---|
| `D0...D3` | Loc-ID 0x00001801 |
| `D4...D7` | Bytes 5-8 des Datum/Uhrzeit-Strings |

**Beispielnachricht:**

```text
00 16 5F 38 08 00 00 18 01 31 37 32 33
```

### PC_DB_L

- **Sender:** PC
- **Loc-ID:** `0x00001802`
- **Funktion:** Uebertragung des Datenbanknamens mit 12 Bytes
- **Antwort:** -/-

Bytes 1 bis 6 stellen das Datum, Bytes 7 bis 12 die Uhrzeit dar.

Beispielstring:

"070917235340" = am 07.09.2017 um 23:53:40 wurde die Datenbank erstellt. Die einzelnen Bytes werden in ASCII-Werte uebersetzt und dann uebertragen.

Dieser String findet sich auch im Dateinamen der exportierten Datenbank aus dem Parametriercenter.

**Genutzte Datenbytes:**

| Bytes | Bedeutung |
|---|---|
| `D0...D3` | Loc-ID 0x00001802 |
| `D4...D7` | Bytes 5-8 des Datum/Uhrzeit-Strings |

**Beispielnachricht:**

```text
00 16 5F 38 08 00 00 18 02 35 33 34 30
```

### PC_KENNER

- **Sender:** PC
- **Loc-ID:** `0x00001803`
- **Funktion:** Kenner und Identifier fuer die Module
- **Antwort:** -/-

Die Identifizierung der MBCAN-Module folgt strikt dem Format der Geraete-UiD von Maerklin. In der GUiD stellt die erste Stelle den Identifier dar. Zur Zeit verwendet MBCAN die Kennung "m" (0x6D). Im Parametriercenter kann dieser angepasst werden, falls Maerklin den Identifier "m" fuer seine eigene Module reklamiert.

Darueber hinaus bekommt jedes Modul noch einen Kenner, mit dem es sich an der GUiD der CS2/3 als "Sonstige Geraete" anmelden kann. Zur Zeit ist dies "AAAA" (0xAAAA). Im Parametriercenter kann dieser angepasst werden, falls Maerklin den Kenner "AAAA" fuer seine eigene Module reklamiert.

Ausgenommen sind die Module mbc-80 (Kenner 0x0040 - LinkS88), mbc-82/93 (Kenner 0x0000 - Booster) und mbc-89 (Kenner 0x0020 - Connect6021) die von Maerklin fest vorgegeben sind. Diese Kenner sind in der Firmware der Module bereits fest integriert.

**Genutzte Datenbytes:**

| Bytes | Bedeutung |
|---|---|
| `D0...D3` | Loc-ID 0x00001803 |
| `D4` | 0x00 |
| `D5` | Identifier |
| `D6...D7` | Kenner |

**Beispielnachricht:**

```text
00 16 5F 38 08 00 00 18 03 00 6D AA AA
```

### PC_NEU

- **Sender:** PC
- **Loc-ID:** `0x00001804`
- **Funktion:** Neuanmeldeaufforderung
- **Antwort:** `MD_NEU_DATA`

Zyklische Aufforderung an neu am MBCAN-Bus angeschlossene und noch nicht angemeldete Module, sich am Parametriercenter anzumelden. Dies gilt auch fuer Module, die ueber das Parametriercenter zurueckgesetzt wurden.

**Genutzte Datenbytes:**

| Bytes | Bedeutung |
|---|---|
| `D0...D3` | Loc-ID 0x00001804 |
| `D4...D7` | 0x00 |

**Beispielnachricht:**

```text
00 16 5F 38 08 00 00 18 04 00 00 00 00
```

### PC_NEU_DATA

- **Sender:** PC
- **Loc-ID:** `0x00001805`
- **Funktion:** Rueckmeldung des PC an das Modul waehrend des Neuanmeldeprozesses
- **Antwort:** `MD_NEU_DATA`

Der PC spiegelt Schritt-Zaehler (D6) UND Byte-Wert (D7) der zuletzt empfangenen MD_NEU_DATA-Nachricht exakt zurueck als Quittierung. Nur wenn BEIDE Werte exakt uebereinstimmen, sendet das entsprechende Modul das naechste Byte des Info-Blocks (Schritt-Zaehler+1); bei einer Abweichung bricht das Modul seinen Neuanmeldeprozess ab (zurueck in den Listen-Modus). Alle anderen, noch nicht erfolgreich angemeldeten Module schalten ebenfalls in den Listen-Modus und reagieren erst nach einer weiteren PC_NEU-Nachricht.

WICHTIG: Der uebertragene Info-Block umfasst den GESAMTEN Bereich von MBC_INFO_START bis MBC_INFO_ENDE (Seriennummer, Artikelnummer, SW-/HW-Version UND Modulname)

**Genutzte Datenbytes:**

| Bytes | Bedeutung |
|---|---|
| `D0...D3` | Loc-ID 0x00001805 |
| `D4...D5` | 0x00 |
| `D6` | Schritt-Zaehler (muss exakt der zuletzt vom Modul in MD_NEU_DATA gesendete Wert sein - Echo-Handshake) |
| `D7` | n-tes Byte xx des Info-Blocks (Seriennummer/Artikelnummer/SW-/HW-Version/Modulname) |

**Beispielnachricht:**

```text
00 16 5F 38 08 00 00 18 05 00 01 00 xx
```

### MD_NEU_DATA

- **Sender:** Modul
- **Loc-ID:** `0x00001806`
- **Funktion:** Meldung des Moduls waehrend des Neuanmeldeprozesses
- **Antwort:** `PC_NEU_DATA`

Wenn das Modul noch nicht am Parametriercenter angemeldet war, reagiert es mit dieser Nachricht an den PC: es sendet Schritt-Zaehler (D6, beginnt bei 1) und das n-te Byte xx des Info-Blocks (D7) an den PC. Reagiert der PC mit der Nachricht PC_NEU_DATA mit exakt demselben Schritt-Zaehler UND demselben Byte-Wert, sendet es das naechste Byte (Schritt-Zaehler+1), bis entweder der gesamte Info-Block uebertragen wurde (erfolgreiche Anmeldung) oder der PC gerade ein anderes Modul initiiert. Stimmt einer der beiden Werte nicht ueberein, geht es in den Listen-Modus und wartet auf eine weitere PC_NEU-Nachricht.

WICHTIG: Der uebertragene Info-Block umfasst den GESAMTEN Bereich von MBC_INFO_START bis MBC_INFO_ENDE (Seriennummer, Artikelnummer, SW-/HW-Version UND Modulname)

**Genutzte Datenbytes:**

| Bytes | Bedeutung |
|---|---|
| `D0...D3` | Loc-ID 0x00001806 |
| `D4...D5` | 0x00 |
| `D6` | Schritt-Zaehler (beginnt bei 1, erhoeht sich je erfolgreich bestaetigtem Byte) |
| `D7` | n-tes Byte xx des Info-Blocks (Seriennummer/Artikelnummer/SW-/HW-Version/Modulname) |

**Beispielnachricht:**

```text
00 16 5F 38 08 00 00 18 06 00 01 00 xx
```

### PC_RESET

- **Sender:** PC
- **Loc-ID:** `0x00001807`
- **Funktion:** Durchfuehren eines Hardware-Resets auf dem Modul
- **Antwort:** -/-

Ueber das Parametriercenter koennen Module gezielt einem RESET unterzogen werden. Die Identifizierung der Module geschieht ueber ihre GUiD.

**Genutzte Datenbytes:**

| Bytes | Bedeutung |
|---|---|
| `D0...D3` | Loc-ID 0x00001807 |
| `D4...D7` | GUiD des Moduls |

**Beispielnachricht:**

GUiD mbc-84 #1 = 6D 38 34 01

```text
00 16 5F 38 08 00 00 18 07 6D 38 34 01
```

### PC_MD_DEL

- **Sender:** PC
- **Loc-ID:** `0x00001808`
- **Funktion:** Modul aus Datenbank entfernen
- **Antwort:** -/-

Das Modul wurde aus der Datenbank entfernt und kann sich an dieser Datenbank auch nicht mehr neu anmelden. Wird in der Regel nur bei Modulen verwendet, die sich in der Datenbank befinden aber nicht mehr am Bus angeschlossen werden sollen. Wird nur einmal gesendet, wenn das Modul im Parametriercenter geloescht wird. Ist das Modul nicht am Bus und wird nach einem Neustart der Software wieder am Bus angeschlossen, meldet es sich nicht mehr neu an, es sei denn, die Datenbank wird neu erstellt.

**Genutzte Datenbytes:**

| Bytes | Bedeutung |
|---|---|
| `D0...D3` | Loc-ID 0x00001808 |
| `D4...D7` | GUiD des Moduls |

**Beispielnachricht:**

GUiD mbc-84 #1 = 6D 38 34 01

```text
00 16 5F 38 08 00 00 18 08 6D 38 34 01
```

### PC_ALIVE

- **Sender:** PC
- **Loc-ID:** `0x00001809`
- **Funktion:** ALIVE-Abfrage
- **Antwort:** `MD_ALIVE`

Zyklische Abfrage ueber die GUiD, ob das betreffende Modul sich noch am MBCAN-Bus befindet. Es antwortet mit der Nachricht MD_ALIVE.

**Genutzte Datenbytes:**

| Bytes | Bedeutung |
|---|---|
| `D0...D3` | Loc-ID 0x00001809 |
| `D4...D7` | GUiD des Moduls |

**Beispielnachricht:**

GUiD mbc-84 #1 = 6D 38 34 01

```text
00 16 5F 38 08 00 00 18 09 6D 38 34 01
```

### MD_ALIVE

- **Sender:** Modul
- **Loc-ID:** `0x0000180A`
- **Funktion:** ALIVE-Abfrage
- **Antwort:** -/-

Zyklische Abfrage ueber die GUiD, ob das betreffende Modul sich noch am MBCAN-Bus befindet. Es antwortet mit der Nachricht MD_ALIVE.

**Genutzte Datenbytes:**

| Bytes | Bedeutung |
|---|---|
| `D0...D3` | Loc-ID 0x0000180A |
| `D4...D7` | GUiD des Moduls |

**Beispielnachricht:**

GUiD mbc-84 #1 = 6D 38 34 01

```text
00 16 3B 59 08 00 00 18 0A 6D 38 34 01
```

### PC_ARRAY

- **Sender:** PC
- **Loc-ID:** `0x0000180B`
- **Funktion:** Zugriff Systemarray anfragen
- **Antwort:** `MD_ARRAY`

Der PC fragt ueber die GUiD an, ob er auf das Systemarray des Moduls zugreifen darf.

**Genutzte Datenbytes:**

| Bytes | Bedeutung |
|---|---|
| `D0...D3` | Loc-ID 0x0000180B |
| `D4...D7` | GUiD des Moduls |

**Beispielnachricht:**

GUiD mbc-84 #1 = 6D 38 34 01

```text
00 16 5F 38 08 00 00 18 0B 6D 38 34 01
```

### MD_ARRAY

- **Sender:** Modul
- **Loc-ID:** `0x0000180C`
- **Funktion:** Zugriff Systemarray freigegeben
- **Antwort:** -/-

Antwort des durch die GUiD im Befehl PC_ARRAY adressierten Moduls mit Freigabe des Zugriffs. Das Modul geht dann in die Wartestellung, alle anderen Module werten die folgenden Anfragen des PC nicht mehr aus. Ausgenommen sind Anfragen des PC ausserhalb des Befehls PC_ARRAY_DATA.

**Genutzte Datenbytes:**

| Bytes | Bedeutung |
|---|---|
| `D0...D3` | Loc-ID 0x0000180C |
| `D4...D7` | GUiD des Moduls |

**Beispielnachricht:**

GUiD mbc-84 #1 = 6D 38 34 01

```text
00 16 3B 59 08 00 00 18 0C 6D 38 34 01
```

### PC_ARRAY_DATA

- **Sender:** PC
- **Loc-ID:** `0x0000180D`
- **Funktion:** Zugriff Systemarray freigegeben
- **Antwort:** `MD_ARRAY_DATA`

Der PC stellt die Zugriffsanfrage. Dies kann entweder ein Lese- oder ein Schreibzugriff sein. Ausserdem ist der Systemarray-Index enthalten, der gelesen oder beschrieben werden soll.

Beispiel:

D4 = 0 -> Lesen, 1 -> Schreiben

D5 + D6 = Systemarray-Index

D7 = zu schreibender Wert, bei lesendem Zugriff irrelevant

Ueber den Index des Systemarrays wird ausserdem das Ende einer Datenuebertragung angezeigt. Liegt der Index ueber der Maximallaenge des Systemarrays und entspricht es einem bestimmten Wert, wird die Wartestellung des Modus fuer weitere Datenuebertragungen aufgehoben und alle anderen Module koennen wieder auf einen PC_ARRAY-Zugriff angesprochen werden.

**Genutzte Datenbytes:**

| Bytes | Bedeutung |
|---|---|
| `D0...D3` | Loc-ID 0x0000180D |
| `D4` | 0 -> Lesen, 1 -> Schreiben |
| `D5...D6` | Systemarray-Index |
| `D7` | zu schreibender Wert, beim Lesen n.c. |

**Beispielnachricht:**

Wert 0x0A an die Stelle 0x0001 im Systemarray

```text
00 16 5F 38 08 00 00 18 0D 01 00 01 0A
```

### MD_ARRAY_DATA

- **Sender:** Modul
- **Loc-ID:** `0x0000180E`
- **Funktion:** Antwort des Moduls auf Systemarray-Zugriff
- **Antwort:** -/-

Bei einem lesenden Zugriff uebergibt das Modul auf D7 den Inhalt des Systemarrays, bei einem schreibenden Zugriff ist D7 irrelevant. Die anderen Datenbytes der Nachricht (D4 ... D6) sind identisch mit der Nachricht des PC.

**Genutzte Datenbytes:**

| Bytes | Bedeutung |
|---|---|
| `D0...D3` | Loc-ID 0x0000180E |
| `D4` | 0 -> Lesen, 1 -> Schreiben |
| `D5...D6` | Systemarray-Index |
| `D7` | gelesener Inhalt des Systemarrays, beim Schreiben n.c. |

**Beispielnachricht:**

Gelesener Wert 0x07 aus der Stelle 0x0108 im Systemarray

```text
00 16 2B 17 08 00 00 18 0E 00 01 08 07
```

### PC_UPGRADE

- **Sender:** PC
- **Loc-ID:** `0x0000180F`
- **Funktion:** Firmware-Upgrade
- **Antwort:** `MD_UPGRADE`

Der PC fragt ueber die GUiD an, ob er die Firmware des Moduls upgraden darf. Ist nur aktiv bei Modulen der 3. Generation und nicht gueltig fuer die Module des Typs mbc-91.

**Genutzte Datenbytes:**

| Bytes | Bedeutung |
|---|---|
| `D0...D3` | Loc-ID 0x0000180F |
| `D4...D7` | GUiD des Moduls |

**Beispielnachricht:**

GUiD mbc-84 #1 = 6D 38 34 01

```text
00 16 5F 38 08 00 00 18 0F 6D 38 34 01
```

### MD_UPGRADE

- **Sender:** Modul
- **Loc-ID:** `0x00001810`
- **Funktion:** Firmware-Upgrade freigeben
- **Antwort:** -/-

Antwort des durch die GUiD im Befehl PC_UPGRADE adressierten Moduls mit Freigabe des Zugriffs. Das Modul geht dann in die Wartestellung, alle anderen Module werten die folgenden Anfragen des PC nicht mehr aus. Ausgenommen sind Anfragen des PC ausserhalb des Befehls PC_UPGRADE_DATA.

**Genutzte Datenbytes:**

| Bytes | Bedeutung |
|---|---|
| `D0...D3` | Loc-ID 0x00001810 |
| `D4...D7` | GUiD des Moduls |

**Beispielnachricht:**

GUiD mbc-84 #1 = 6D 38 34 01

```text
00 16 3B 59 08 00 00 18 10 6D 38 34 01
```

### PC_UPGRADE_DATA

- **Sender:** PC
- **Loc-ID:** `0x00001811`
- **Funktion:** Schreibe Firmware
- **Antwort:** `MD_UPGRADE_DATA`

Der PC uebermittelt die Upgrade-Daten. Das Modul speichert diese in das externe EEPROM zur Vorbereitung der Neuprogrammierung. Die Daten werden PAGE-weise (je 64 Byte) vom Parametriercenter uebertragen, so dass der BOOTLOADER hinterher die Daten aus dem externen EEPROM auch korrekt auslesen kann.

**Genutzte Datenbytes:**

| Bytes | Bedeutung |
|---|---|
| `HASH` | Laufende Nummer in der jeweiligen PAGE |
| `D0...D3` | Loc-ID 0x00001811 |
| `D4...D7` | 4 zu schreibende Bytes |

**Beispielnachricht:**

Schreibe im laufenden Index 2 die Werte 0x01, 0x00, 0x01 und 0x0A fortlaufend in das externe EEPROM

```text
00 16 03 02 08 00 00 18 11 01 00 01 0A
```

### MD_UPGRADE_DATA

- **Sender:** Modul
- **Loc-ID:** `0x00001812`
- **Funktion:** Antwort des Moduls auf Schreibe Firmware
- **Antwort:** -/-

Das Modul antwortet mit der exakten Datenstruktur der gesendeten Nachricht und signalisiert damit, dass es die Upgrade-Daten im externen EEPROM gespeichert hat.

**Genutzte Datenbytes:**

| Bytes | Bedeutung |
|---|---|
| `HASH` | Laufende Nummer in der jeweiligen PAGE |
| `D0...D3` | Loc-ID 0x00001812 |
| `D4...D7` | 4 zu schreibende Bytes |

**Beispielnachricht:**

Schreibe im laufenden Index 2 die Werte 0x01, 0x00, 0x01 und 0x0A fortlaufend in das externe EEPROM

```text
00 16 03 02 08 00 00 18 12 01 00 01 0A
```

### PC_BOOT

- **Sender:** PC
- **Loc-ID:** `0x00001813`
- **Funktion:** Modul neu Booten
- **Antwort:** -/-

Nach erfolgreicher Uebertragung der neuen Firmware signalisiert der PC einen Hardwarereset des Moduls. Dies geschieht nicht ueber den Befehl PC_RESET, da vorher noch Identifier in das externe EEPROM gespeichert werden muessen die anzeigen, dass eine neue Firmware vorliegt.

**Genutzte Datenbytes:**

| Bytes | Bedeutung |
|---|---|
| `D0...D3` | Loc-ID 0x00001813 |
| `D4...D7` | GUiD des Moduls |

**Beispielnachricht:**

GUiD mbc-84 #1 = 6D 38 34 01

```text
00 16 5F 38 08 00 00 18 13 6D 38 34 01
```

### MD_S88

- **Sender:** Modul
- **Loc-ID:** `0x00001814`
- **Funktion:** Stellungsmeldung mbc-88 / mbc-90
- **Antwort:** -/-

Sendet bei Statusaenderung eines PINs die Stellung auf den MBCAN-Bus, so dass sowohl des Parametriercenter als auch andere Module diese ggf. weiterverarbeiten koennen. Ist ein Relikt aus den ersten beiden Generationen der MBCAN-Modulreihe und sollte bei Eigenentwicklungen durch Auswertung der 0x22/23-CAN-Kommandos von Maerklin ersetzt werden.

**Genutzte Datenbytes:**

| Bytes | Bedeutung |
|---|---|
| `D0...D3` | Loc-ID 0x00001814 |
| `D4...D5` | Modulnummer (BUS 1 1...31, BUS 2 32...62, BUS 3 63...93) |
| `D6` | Kontaktnummer (1...16) |
| `D7` | Stellung |

**Beispielnachricht:**

Modul 16, Kontakt 2 hat Stellung 1

```text
00 16 2B 17 08 00 00 18 14 00 10 02 01
```

### MD_ZYKLUS

- **Sender:** Modul
- **Loc-ID:** `0x00001815`
- **Funktion:** Anstoss eines Rueckmeldezyklus bei anderen MBCAN-Modulen (nur interner Sub-Bus)
- **Antwort:** -/-

Sekuendlich wird die naechste Modul-Basisadresse (durchlaeuft zyklisch 0x3000, 0x3004, 0x3008, ... bis 0x0400, danach wieder 0x0000) versendet - UND ZWAR NICHT ueber den Haupt-PC-/CS2-Bus, sondern ausschliesslich ueber den INTERNEN MBCAN-Sub-Bus sowie zusaetzlich ueber die austauschbare Nachrichten-Bruecke. Andere Module am internen Sub-Bus werten diese Adresse aus, um bei Zustaendigkeit ihren eigenen Rueckmeldezyklus anzustossen, ohne dass CS2/Parametriercenter dafuer aktiv werden muessen.

**Genutzte Datenbytes:**

| Bytes | Bedeutung |
|---|---|
| `D0...D3` | Loc-ID 0x00001815 |
| `D4...D5` | 0x00 |
| `D6` | Basisadresse High-Byte |
| `D7` | Basisadresse Low-Byte |

**Beispielnachricht:**

```text
00 16 xx xx 08 00 00 18 15 00 00 30 00
```
