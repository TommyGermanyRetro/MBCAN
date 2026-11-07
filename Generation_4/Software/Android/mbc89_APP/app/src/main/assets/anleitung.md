# mbc89 für Android – Anleitung

Die App bildet die Märklin-Stellpulte Keyboard 6040, Control Unit 6021, Control 80f und Memory 6043 auf einem Android-Tablet oder Smartphone nach. Gegenüber der Märklin-Zentrale (CS2/CS3) tritt sie als Connect6021 auf, gegenüber dem ParaCenter als MBCAN-Modul mbc89.

## 1. Voraussetzungen

- Eine mbc80 mit ESP32 als WLAN-Bridge zum CAN-Bus, empfohlen ab Firmware MBCANLib 1.1.2.
- Tablet oder Smartphone im selben WLAN wie die mbc80, Android 8 oder neuer.
- Die App läuft immer im Querformat.

## 2. Erster Start und Verbindung

1. App starten. Nach dem Startbildschirm (5 s) erscheint das Panel.
2. Reiter **Verbindung** öffnen, die IP-Adresse der mbc80 und den Port (Standard 15731) eintragen.
3. **Verbinden** tippen. Beim ersten Mal fragt Android nach der Erlaubnis für Benachrichtigungen – bitte erlauben, die Verbindung läuft als Dienst mit sichtbarer Benachrichtigung.
4. Die CS3 erkennt die App als Connect6021 und liest ihre Konfiguration. Im ParaCenter kann das Modul wie ein echtes mbc89 angemeldet werden. Es erscheint mit dem Namen „0x…“ (Seriennummer des Geräts).

IP-Adresse und Port werden gespeichert. Danach genügt der Schalter **Verbindung** rechts oben im Panel.

**Statusanzeige unter dem Schalter:**

- Grün „Verbunden“ – alles in Ordnung.
- Gelb blinkend „Verbinde mit …“ – Verbindungsaufbau läuft.
- Orange blinkend „Verbindung verloren – neuer Versuch …“ – die Verbindung ist abgebrochen, die App versucht es alle 5 s erneut, bis sie wieder steht oder getrennt wird.
- Rot „Nicht erreichbar“ – falsche Adresse oder mbc80 aus. Ein Tipp auf die Statuszeile öffnet den Reiter Verbindung.
- Grau „Getrennt“.

Ohne Verbindung sind alle Panels gesperrt, die Anzeigen der Control Units bleiben dunkel. Befehle, die wegen eines Abbruchs nicht gesendet wurden, werden nicht nachgeholt, sondern im Reiter Meldungen genannt.

Solange die App verbunden ist, hält sie das Gerät auch bei ausgeschaltetem Display wach. Das kostet etwas Akku; nach **Trennen** ist das wieder aus.

## 3. Die Reiter

- **Panel** – die Stellpulte (Abschnitte 4 bis 7).
- **Fahrstraßen** – Liste aller aufgenommenen Memory-Fahrstraßen (Abschnitt 8).
- **Verbindung** – Adresse, Status und Anzahl der Geräte (Abschnitt 9).
- **Systemarray** – alle Werte, die ParaCenter und CS3 in die App schreiben; geänderte Werte gelb, noch nicht gespeicherte rot. Nur zur Kontrolle.
- **Meldungen** – Verbindungsereignisse und Fehler dieser Sitzung mit Uhrzeit. Die Zahl im Reiter nennt ungelesene Meldungen.
- **Anleitung** – diese Seite.

## 4. Panel bedienen

Die Pulte liegen nebeneinander wie auf einer Anlage: Memory 4 … 1, Keyboard 16 … 1, Control Unit, Control 80f 1 … 8. Angezeigt werden nur die Geräte, die im Reiter Verbindung eingestellt sind. Die App startet immer auf der Control Unit.

- **Blättern:** Tipp auf die Miniatur links oder rechts neben dem Pult.
- **Übersicht:** Die Fußzeile zeigt alle 29 Plätze in derselben Reihenfolge. Die Control Unit steht immer an derselben Stelle, das angezeigte Pult ist gelb umrahmt, freie Plätze sind blasse Rahmen. Ein Tipp springt direkt zum Pult.
- **Tasten:** Jede Taste reagiert auf ihre ganze Zelle (Beschriftung, LED und Taste). Eine gedrückte Taste wird kurz dunkler und kleiner.
- **Smartphone:** Das Pult wird auf die Bildschirmhöhe verkleinert und läuft im Vollbild. Die Systemleisten kommen mit einer Wischgeste vom Rand zurück. Die Fußzeile blendet der Knopf **Übersicht einblenden** unten links ein und aus.

## 5. Keyboard 6040

Jedes Keyboard schaltet 16 Magnetartikel. Oben steht die rote Taste (rund/Halt), unten die grüne (gerade/Fahrt), dazwischen Nummer und LED. Die LED zeigt die zuletzt geschaltete Stellung – auch wenn der Artikel an der CS3, einer MS2 oder einem anderen Gerät geschaltet wurde.

- Die Adressen ergeben sich aus der Keyboard-Nummer und dem Basis-Adressblock (Konfiguration „Basis“ 1–4 im ParaCenter oder an der CS3): Keyboard 1 schaltet im Block 1 die Artikel 1–16, Keyboard 2 die Artikel 17–32 usw.
- **MM oder DCC:** Oben links zeigt die Matrix „Protokoll“, ob die Tastengruppen 1–4, 5–8, 9–12 und 13–16 Märklin-Motorola (MM) oder DCC schalten. Eingestellt wird das im ParaCenter oder an der CS3 (Konfigurationskanäle „KeyB n Group m“).
- Bei Anlagenstopp schaltet das Keyboard nichts.

## 6. Control Unit 6021 und Control 80f

Die Control Unit und bis zu acht Control 80f haben dasselbe Pult. Mit ihnen werden die Loks gefahren, die der Connect6021 in der CS3 zugeordnet sind (6021-Adressen 01–80).

**Lok aufrufen:** Zwei Ziffern eintippen, z. B. 0 und 8 für Lok 08. Die App übernimmt dabei den aktuellen Zustand der Lok. Blinkt die Adresse, ist die Lok nicht steuerbar: Sie ist in der CS3 nicht zugeordnet oder läuft bereits auf einem anderen Pult der App.

**Fahren:**

- **Fahrstufe:** Den Drehknopf ziehen oder auf die Skala tippen, 15 Stufen von 0 bis 250.
- **Richtung:** Tipp auf das obere (vorwärts) oder untere (rückwärts) rote Dreieck neben der Anzeige. Das leuchtende Dreieck zeigt die aktuelle Richtung.
- **Funktionen:** function (Licht ein), off (Licht aus), f1–f4 schalten um. Die LEDs zeigen den Zustand.
- **stop / go** schalten die ganze Anlage aus und wieder ein. Die go-LED leuchtet, wenn die Anlage läuft.

**L und F – getrennte Funktionsdecoder-Adresse** (wie in der 6021-Anleitung, Abschnitt 5.5):

- Nach dem Start leuchten die LEDs über L und F gemeinsam: Lok und Funktionen gehören zur selben Adresse.
- **F** tippen und eine Adresse eingeben, z. B. F, 1, 0: f1–f4 schalten jetzt den Funktionsdecoder 10. Die bisherige Lok fährt weiter, Fahrstufe, Richtung und function/off gehören weiter zu ihr. Die F-LED leuchtet.
- **L** tippen und eine Adresse eingeben, z. B. L, 0, 8: Lok 08 wird gefahren, der Funktionsdecoder 10 bleibt aktiv. Die L-LED leuchtet.
- Wird für L und F dieselbe Adresse eingegeben, leuchten wieder beide LEDs, und neue Adressen gelten für beide.

**Abweichungen vom Original:**

- Die Richtung wird nicht durch Drehen des Knopfs über den Nullanschlag gewechselt, sondern nur über die beiden Dreiecke. Der Knopf stellt nur die Fahrstufe.
- Der Knopf wird gezogen oder angetippt statt gedreht.
- Eine Lok kann nur auf einem Pult der App gleichzeitig gefahren werden. Wer die Adresse zuerst vollständig eingibt, behält sie, das andere Pult blinkt und sendet nichts.
- Ohne Verbindung bleibt die Anzeige dunkel.
- Fahrbefehle anderer Geräte (CS3, MS2, andere Apps) übernimmt die App nicht in ihre Anzeige – wie beim Original.

## 7. Memory 6043

Ein Memory speichert 24 Fahrstraßen (A1–C8) mit je bis zu 20 Einträgen. Bis zu vier Memory sind möglich, jedes mit eigenen Fahrstraßen.

**Tasten:** In der oberen Reihe extern, off, input, end und clear, darunter die Fahrstraßentasten A1–A8, B1–B8 und C1–C8. Die LED einer Fahrstraße leuchtet, solange die Fahrstraße gestellt ist.

Schaltbefehle für eine Fahrstraße können vom Keyboard der App kommen, aber auch von jedem anderen Gerät am Bus (CS3, MS2, anderes Keyboard).

### 7.1 Beispiel: Einfahrt Gleis A aufnehmen

Weiche 1 rund, Weiche 2 gerade, Signal 6 rot, Signal 5 grün:

1. **input** tippen – input und alle belegten Fahrstraßen blinken.
2. **A1** tippen – input und A1 leuchten.
3. Am Keyboard Weiche 1 rot, Weiche 2 grün, Signal 6 rot und Signal 5 grün tippen.
4. **end** tippen – die Aufnahme ist beendet, end leuchtet.

Ab jetzt stellt ein Tipp auf **A1** die ganze Fahrstraße. Die LED von A1 blinkt bei jedem gesendeten Befehl kurz auf und leuchtet danach dauerhaft.

### 7.2 Schaltdauer verlängern

Schwergängige Weichen oder Entkupplungsgleise brauchen einen längeren Impuls. Beim Aufnehmen dieselbe Keyboard-Taste mehrmals tippen: jeder weitere Druck verlängert den Impuls um etwa 0,3 s, höchstens 7 Drücke. Das geht nur mit dem Keyboard der App. Befehle von CS3, MS2 oder anderen Geräten werden immer mit Standardschaltzeit aufgenommen, weil deren Tasten umschalten.

### 7.3 Fahrstraße korrigieren und erweitern

- **Richtung falsch:** Während der Aufnahme einfach die andere Taste desselben Artikels tippen – der neue Befehl ersetzt den alten.
- **Erweitern:** input, Fahrstraße, neue Befehle am Keyboard, end. Die neuen Befehle werden hinten angehängt.
- **Letzte Befehle löschen:** input, Fahrstraße, dann clear – jeder Druck löscht den jeweils letzten Eintrag, clear leuchtet dabei kurz auf. Danach end.
- **Ganze Fahrstraße löschen:** input, clear (clear und alle belegten Fahrstraßen blinken), dann die Fahrstraße tippen. input und die Fahrstraße leuchten kurz, dann leuchtet end – die Fahrstraße ist leer.
- **Abbrechen:** end beendet jeden Schritt vorzeitig.

### 7.4 Folgefahrstraßen

Eine Fahrstraße kann eine andere Fahrstraße desselben Memory mitstellen. Dazu während der Aufnahme die Taste der Folgefahrstraße tippen – sie wird als Eintrag gespeichert und belegt einen der 20 Plätze.

Beispiel mehr als 20 Befehle: In A1 nach 19 Befehlen (die LED von A1 blinkt) die Taste A2 tippen – die Aufnahme endet automatisch, end leuchtet. Danach input, A2 und die restlichen Befehle eingeben. Ein Tipp auf A1 stellt jetzt A1 und A2 nacheinander.

### 7.5 Kreuzende Fahrstraßen und Verriegelung

Schaltet eine Fahrstraße einen Artikel in die andere Richtung als eine gestellte Fahrstraße, erlischt die gestellte Fahrstraße. Dasselbe gilt, wenn der Artikel von Hand am Keyboard, an der CS3 oder an einem anderen Gerät umgeschaltet wird.

Mit **Verriegelung** (im Reiter Verbindung je Memory einstellbar) wird eine kreuzende Fahrstraße desselben Memory nicht gestellt:

- Beide Fahrstraßen blinken etwa 5 s, nichts wird geschaltet.
- Jede Taste dieses Memory beendet das Blinken.
- Ein zweiter Tipp auf die blinkende Fahrstraße stellt sie trotzdem.
- Eine kreuzende Folgefahrstraße wird ohne Meldung übersprungen.

**Fahrstraße freigeben:** clear, dann die Fahrstraße tippen – ihre LED erlischt, der Inhalt bleibt erhalten. So kann eine kreuzende Fahrstraße gestellt werden, ohne dass etwas blinkt.

### 7.6 Automatik mit s88-Rückmeldern

Mit **extern** schaltet das Memory auf Automatik, mit **off** zurück auf Handbetrieb. Im Automatikbetrieb stellen Rückmeldekontakte die Fahrstraßen.

**Anschluss:** Je Memory drei s88-Module, eines je Tastenreihe A, B und C. Im Reiter Verbindung bei „s88 Reihe A“ die Moduladresse für Reihe A eintragen; B und C liegen auf den beiden folgenden Modulen. Überschneidet sich die Eingabe mit einem anderen Memory, wird das Feld rot und nicht gespeichert.

- Kontakte 1–8 eines Moduls fordern die Fahrstraßen 1–8 seiner Reihe an.
- Kontakte 9–16 sind die Freigabe-Eingänge dieser Fahrstraßen.

**Ohne Verriegelung** stellt jeder belegte Anforderungskontakt seine Fahrstraße sofort.

**Mit Verriegelung:**

- Eine Anforderung, die wegen einer kreuzenden Fahrstraße nicht gestellt werden kann, wird gespeichert und gestellt, sobald der Weg frei ist – bei mehreren in der Reihenfolge A1, A2 … C8.
- Ein belegter Freigabe-Eingang gibt seine gestellte Fahrstraße frei.
- Solange ein Freigabe-Eingang belegt ist, wird seine Fahrstraße weder gestellt noch angefordert.
- off verwirft alle gespeicherten Anforderungen.

**Beispiel Blockstrecke** (Ringstrecke mit vier Blöcken, ohne Verriegelung): Je Block eine Fahrstraße, die das Einfahrsignal des eigenen Blocks auf Rot und das des vorherigen Blocks auf Grün stellt. Die Schaltgleise am Blockanfang an die Kontakte 1–4 anschließen, extern tippen – jeder einfahrende Zug sichert seinen Block und gibt den vorherigen frei.

Bei Anlagenstopp ignoriert das Memory neue Anforderungen. Belegungen und Freigaben laufen weiter, nach go passiert nichts Besonderes.

### 7.7 Nach dem Neustart

Fahrstraßen, gestellte Fahrstraßen und die Betriebsart (extern/off) bleiben gespeichert. Nach dem Start leuchtet zusätzlich end. Gespeicherte Automatik-Anforderungen verfallen, und es wird nichts geschaltet.

### 7.8 Memory leeren

Im Reiter Verbindung löscht der Knopf **M1** … **M4** hinter „Leeren“ nach einer Rückfrage alle Fahrstraßen dieses Memory. Ein gefüllter Knopf zeigt, dass das Memory Fahrstraßen enthält. Wird die Anzahl der Memory verringert, werden die entfallenden Memory geleert.

## 8. Reiter Fahrstraßen

Eine Tabelle aller aufgenommenen Fahrstraßen: Memory, Fahrstraße, ob sie gestellt ist, Nummer des Eintrags, Artikel (z. B. „MM 12“ oder „Folgefahrstraße A2“), Stellung (rot/grün) und Schaltdauer. Leere Fahrstraßen erscheinen nicht. Die Tabelle dient nur zur Übersicht, geändert wird am Memory.

## 9. Reiter Verbindung

- IP-Adresse und Port der mbc80, **Verbinden** und **Trennen**.
- Status mit ParaCenter-Anmeldung, CS3-Erkennung und Anlagenzustand.
- Anzahl der Keyboards (0–16), Control 80f (0–8) und Memory (0–4).
- Je Memory: Verriegelung, s88-Moduladresse Reihe A und Leeren, jeweils mit den Knöpfen M1 … M4.

Die Geräteeinstellungen lassen sich nur bei bestehender Verbindung ändern. Ohne Verbindung zeigt der Reiter den gespeicherten Stand.

## 10. Mehrere Geräte

Mehrere Tablets oder Smartphones können gleichzeitig an derselben mbc80 arbeiten. Jedes Gerät ist ein eigenes Connect6021-Modul mit eigener Seriennummer und eigener Lok-Zuordnung in der CS3. Fahrzustände werden zwischen den Geräten nicht abgeglichen – wie bei mehreren echten Steuerpulten.

## 11. Hilfe bei Problemen

- **„Nicht erreichbar“:** IP-Adresse und Port im Reiter Verbindung prüfen, mbc80 eingeschaltet und im selben WLAN?
- **Verbindung bricht immer wieder ab:** Firmware der mbc80 prüfen (ab MBCANLib 1.1.2 trennt sie ein nicht lesendes Gerät, statt neu zu starten).
- **Adresse blinkt an der Control Unit:** Lok in der CS3 der Connect6021 zuordnen oder prüfen, ob sie auf einem anderen Pult läuft.
- **Keyboard schaltet falsche Artikel:** Basis-Adressblock und MM/DCC im ParaCenter oder an der CS3 prüfen.
- **Memory-Automatik reagiert nicht:** extern gedrückt? s88-Moduladresse Reihe A eingetragen? Anlage auf go?
- **Fehler und Abbrüche:** Reiter Meldungen ansehen.
