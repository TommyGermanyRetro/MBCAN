
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

## 1.  Systemarchitekur

Im folgenden Bild ist die Systemarchitektur von MBCAN abgebildet. Der CAN-Bus erfordert eine strikte Linienstruktur. Für Verzweigungen ist ein Hub notwendig zur Sicherstellung der Terminierung des Busses zur Vermeidung von ungewolltem Verhalten.

![MBCAN](https://github.com/TommyGermanyRetro/MBCAN/blob/main/Bilder/img_mbc_systemarchitektur.png)

## 2.  Portierungshistorie

| Modultyp            | ATMEGA644P          | ESP32C3             | RP2040 PI PICO      | WS RP2040 ZERO      |
| :-----------------: | :-----------------: | :-----------------: | :-----------------: | :-----------------: |
| mbc-80              | HW:21-10-01         |                     |                     |                     |
| mbc-84              | HW:18-02-03<br>HW:24-01-06         |                     |                     |                     |
| mbc-88              | HW:18-03-29<br>HW:24-01-06         |                     |                     |                     | 
| mbc-90              | HW:18-03-04<br>HW:23-11-25         |                     |                     |                     | 
| mbc-91              | HW:16-10-02<br>HW:24-04-01         |                     |                     |                     | 
| mbc-92              | HW:24-07-20         |                     |                     |                     | 
	
# Tagebuch

## 16.08.2026

Erste Bedienungsanleitungen zum neuen Parametriercenter und der portierten Module hochgeladen unter Generation_4/Dokumente

## 13.08.2026

Systemarchitekur ergänzt, Parametriercenter-ZIP unter 3. Generation ergänzt

## 09.08.2026

Neuanlage des Gits zum MBCAN-Projekt
