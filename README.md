
![MBCAN](https://github.com/TommyGermanyRetro/MBCAN/blob/main/Bilder/mbc_header_960.png)

# MBCAN
<i>Fernsteuerung einer Märklin-Modelleisenbahn über den CAN-Bus</i>

Herzlich willkommen auf meinen GITHUB-Seiten zum Projekt "MBCAN". Bis Ende 2025 hatte ich noch eine Website mit allen Infos rund um das Projekt. Die Pflege war aber mehr als aufwändig, und so habe ich sie geschlossen. Außerdem war das Projekt zu diesem Zeitpunkt bereits 18 Jahre alt. Also Zeit, es abzuschließen und Neues auszuprobieren.

Damit der letzte Stand des Unterlagen nicht verloren gehen, habe ich:

1) Alle Unterlagen inkl. Quellcodes an Jörg Seitz übergeben (https://www.jss-elektronik.de)

2) Ein eigenes GIt erstellt, auf dem die Downloads der ehemaligen Website abgelegt sind (<a href="https://github.com/TommyGermanyRetro/MBCAN/tree/main/Generation_3">Generation_3 hier im Git</a>)

Durch die Beschäftigung mit moderneren Prozessoren wie ESP32 und RP2040 habe ich neue Motivation für das Projekt gefunden und portiere gerade die 3. Generation Arduino IDE tauglich auf die Prozessoren ATMEGA644P (wie bei der 3. Generation), den ESP32-C3, den Waveshare RP2040 Zero und den RP RP2040 Pi Pico. Diese Versionen bezeichne ich als 4. Geneartion.

Passend dazu wird es ein erweitertes Parametriercenter geben, welches auch das Firmwareupdate der neuen Prozessoren über den CAN-Bus ermöglicht. Dazu wird die OTA-Funktionalität genutzt. Die Sketche werden hier im Git Stück für Stück veröffentlicht. Reale Umsetzungen werde ich für den ESP32-C3 in Eagle erstellen und gleichfalls hier posten. Nur die Bedienungsanleitungen und Warenkörbe, die ich früher mal geschrieben habe, wird es nicht mehr geben.

Solltet ihr Spaß an der Weiterentwicklung auf Basis dieser Sketche haben seid ihr gerne eingeladen, den Sourcecode fleißig zu nutzen und zu experimentieren.

Bleibt gesund,
Thomas
