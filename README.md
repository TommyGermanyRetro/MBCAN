
![MBCAN](https://github.com/TommyGermanyRetro/MBCAN/blob/main/Bilder/mbc_header_960.png)

# MBCAN

Herzlich willkommen auf meinen GITHUB-Seiten zum Projekt "MBCAN". Bis Ende 2025 hatte ich noch eine Website mit allen Infos rund um das Projekt. Die Pflege war aber mehr als aufwändig, und so habe ich sie geschlossen. Außerdem war das Projekt zu diesem Zeitpunkt bereits 18 Jahre alt. Also Zeit, es abzuschließen und Neues auszuprobieren.

Damit der letzte Stand der Unterlagen nicht verloren geht, habe ich:

1) Alle Unterlagen inkl. Quellcodes an Jörg Seitz übergeben (https://www.jss-elektronik.de)

2) Ein eigenes Git erstellt, auf dem die Downloads der ehemaligen Website abgelegt sind (<a href="https://github.com/TommyGermanyRetro/MBCAN/tree/main/Generation_3">Generation_3 hier im Git</a>)

Durch die Beschäftigung mit moderneren Prozessoren wie ESP32 und RP2040 habe ich neue Motivation für das Projekt gefunden und portiere gerade die 3. Generation Arduino IDE tauglich auf die Prozessoren ATMEGA644P (wie bei der 3. Generation), den ESP32-C3, den Waveshare RP2040 Zero und den RP RP2040 Pi Pico. Diese Versionen bezeichne ich als 4. Generation.

Passend dazu wird es ein erweitertes Parametriercenter geben, welches auch das Firmwareupdate der neuen Prozessoren über den CAN-Bus ermöglicht. Dazu wird die OTA-Funktionalität genutzt. Die Sketche werden hier im Git Stück für Stück veröffentlicht. Reale Umsetzungen werde ich für den ESP32-C3 in Eagle erstellen und gleichfalls hier posten. Nur die Bedienungsanleitungen und Warenkörbe, die ich früher mal geschrieben habe, wird es nicht mehr geben.

Solltet ihr Spaß an der Weiterentwicklung auf Basis dieser Sketche haben seid ihr gerne eingeladen, den Sourcecode fleißig zu nutzen und zu experimentieren.

Bleibt gesund,<br>
Thomas

# Systemarchitekur

Im folgenden Bild ist die Systemarchitektur der Module der 3. Generation dargestellt. Nach und nach wird es dazu auch die passende 4. Generation geben.

![MBCAN](https://github.com/TommyGermanyRetro/MBCAN/blob/main/Bilder/img_mbc_systemarchitektur.png)

# Portierungshistorie

	<table text-align="center" style="width:100%" >
  <tr>
    <th>Modultyp</th>
    <th>ATMEGA644P</th>
    <th>ESP32C3</th>	
		<th>RP2040 PI PICO</th>	
		<th>RP2040 ZERO</th>	
  </tr>
  <tr>
    <td>mbc-80</td>
    <td>x</td>
    <th></td>	
		<td></td>	
		<td></td>	
  </tr>
	</table>

# Tagebuch

-----------------------------------------------------

# 13.08.2026

Systemarchitekur ergänzt, Parametriercenter-ZIP unter 3. Generation ergänzt

-----------------------------------------------------

# 09.08.2026

Neuanlage des Gits zum MBCAN-Projekts

-----------------------------------------------------
