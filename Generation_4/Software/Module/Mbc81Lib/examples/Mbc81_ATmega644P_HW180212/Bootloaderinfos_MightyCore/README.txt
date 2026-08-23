Vorbereitung auf MBCAN für Arduino IDE beim Neuaufsetzen der Module auf Basis des ATmega644P via ISP und Vorbereitung eines Upgrades über das Paramentriercenter

1) Arduino IDE - neuen Boardverwalter ergänzen unter "Datei -> Einstellungen... -> Zusätzliche Boardverwalter-URLs"

https://mcudude.github.io/MightyCore/package_MCUdude_MightyCore_index.json

2) Bei der "Board-Verwaltung" das Board "MightyCore" installieren. Arduino IDE schließen.

3) Im Pfad "C:\Users\<DeinName>\AppData\Local\Arduino15\packages\MightyCore\hardware\avr\y.y.y" die Datei "boards.txt" und den Ordner "bootloaders" reinkopieren

4) CACHE von Arduini IDE löschen um sicherzugehen, dass die Änderungen auch übernommen werden: C:\Users\<DeinName>\AppData\Roaming\arduino-ide\. IDE neu starten. 

5) Arduino IDE starten, <DeinSketch> auswählen und öffnen

6) Unter "Werkzeuge" auf den Prozessortyp "ATmega644" und den Variant "644P/644PA" achten. Jetzt sollte der "Bootloader" MBCAN angezeigt oder ausgewählt werden. Zusätzlich unter "Werkzeuge -> BOD" den Wert "2,7V" auswählen (Brown-Out-Detection - verhindert, dass der Prozessor bei kurzen Spannungseinbrüchen mit korruptem SRAM-Inhalt weiterläuft, statt sauber neu zu starten).

7) Sketch zum Modul kompilieren. 

8) "Sketch -> Komilierte Binärdateien exportieren" anstoßen.

9) Im Sketchordner liegt nun unter "<DeinSketch>\build\MightyCore.avr.644" die Datei "xxx.with_bootloader.hex". 

10) Mit AVRSTudio o.ä. per ISP diese Datei auf den ATmega644P aufspielen. Dazu die FUSES so einstellen, das HIGH.BOOTSZ auf 2048 Bytes steht, HIGH.BOOTRST enabled ist und EXTENDED auf 0xFD (BOD 2,7V) steht. Dann startet der Prozessor zuerst mit dem Bootloader und dann mit dem Sketch.

11) Damit ist der originale Bootloader mit dem kompilierten Sketch aufgespielt. Ab da nur noch die Upgrade-Funktion des Parametriercenters nutzen und die HEX-Datei "xxx.ino.hex". 

12) Gilt nur für das Parametriercenter ab Version 3.0.0.0. Frühere Versionen unterstützen die Arduino IDE HEX-Dateien nicht.
 