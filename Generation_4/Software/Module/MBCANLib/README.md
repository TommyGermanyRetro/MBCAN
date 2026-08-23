# MBCANLib

Klassenbasierte Arduino-Bibliothek für MBCAN-Zubehörmodule (Märklin-CS2-kompatibles CAN-Bus-System
für Modellbahn-Zubehör). Bildet die gemeinsame Basis, auf der modulspezifische Bibliotheken wie
[Mbc80Lib](../Mbc80Lib), [Mbc81Lib](../Mbc81Lib), [Mbc88Lib](../Mbc88Lib), [Mbc90Lib](../Mbc90Lib),
[Mbc91Lib](../Mbc91Lib) und [Mbc92Lib](../Mbc92Lib) aufbauen — sie selbst enthält keine
fertigen Beispiel-Sketches, sondern nur die Bausteine.

## Unterstützte Zielprozessoren

- **ATmega644P** über einen Arduino-kompatiblen AVR-Core (z. B. MightyCore)
- **RP2040** (Raspberry Pi Pico, Waveshare RP2040-Zero, ...)
- **ESP32-C3**

Ein und dieselbe Klasse funktioniert unverändert auf allen drei Plattformen; plattformspezifische
Details (Reset, EEPROM-Emulation, ADC-Auflösung, CAN-Backend) sind in `MbcPlatform` bzw. hinter
bedingter Kompilierung (`#if defined(__AVR__)` / `#if defined(ESP32)`) gekapselt.

## Installation

1. Diesen Ordner (`MBCANLib`) nach `<Arduino-Sketchbook>/libraries/MBCANLib` kopieren.
2. Arduino-IDE neu starten.
3. Für ein konkretes Modul zusätzlich die passende Modulbibliothek installieren (z. B. `Mbc80Lib`
   oder `Mbc91Lib`) — diese hängen per `depends=MBCANLib` in `library.properties` von dieser
   Bibliothek ab.

## Architektur eines MBCAN-Moduls

Ein typisches Modul verdrahtet folgende Objekte im Sketch (siehe die Beispiel-Sketches in
`Mbc80Lib`/`Mbc91Lib` für vollständige, lauffähige Vorlagen je Zielboard):

```cpp
static uint8_t sysArray[MBC_SYSARRAY_SIZE(genPurpSize)];   // vom Sketch angelegter Puffer

MbcUtils      utils(sysArray, genPurpSize, pinAdcVspg, pinAdcPlug, pinLedGruen, pinLedRot,
                     pinOneWire);
MbcCan        can(pinCsMbcan, utils);           // oder: MbcCan(pinTx, pinRx, utils) auf ESP32
MbcBootStorage bootStorage(pinCsBootStorage);   // Backend waehlbar, siehe MbcBootStorage-Kopfkommentar
MeinModulCom  com(utils, can, bootStorage, ...);  // Unterklasse von MbcCom, siehe unten

void setup() {
    SPI.begin();
    utils.begin(artikelnummer, swVersion, hwVersion);
    can.begin();
    bootStorage.begin();
    com.begin();
}

void loop() {
    utils.update();
    utils.check();
    // CAN-Nachricht abrufen, an com weiterreichen (modulabhaengig)
}
```

`MbcUtils` hält das Systemarray (Modulidentität, EEPROM-Spiegel, Ereignis-/LED-Zeitsteuerung, ADC,
1-Wire), `MbcCan` die CAN-Anbindung, `MbcCom` den PC-/CS2-Protokoll-Zustandsautomaten. Ein eigenes
Modul leitet **nicht** direkt von `MbcCan`/`MbcUtils` ab, sondern schreibt eine Unterklasse von
`MbcCom`, die die `protected virtual` Erweiterungspunkte überschreibt, die es tatsächlich braucht:

| Hook | Zweck |
|---|---|
| `swInitExtern()` | Softwareversion für die CS2-Ping-Antwort überschreiben |
| `hashInitExtern()` | Zusätzliche `can_msg[]`/`cs2_index[]`-Einträge nach HASH-Neuberechnung nachziehen |
| `kennerFixedExtern()` | Feste CS2-Gerätekennung liefern (statt der von der CS2 vergebenen) |
| `cs2IndexParameterExtern()` / `cs2IndexMesswerteExtern()` | Modulspezifische CS2-GUI-Parameter/Messwerte |
| `eepromWriteParaExtern()` | Zusätzliches EEPROM-Schreiben für modulspezifische Blöcke (≥ 0x0A) |
| `sendMessageExtern()` | Zusätzlicher Versandweg für ausgehende Nachrichten (z. B. eine WLAN-Brücke) |

Ohne Überschreibung verhält sich `MbcCom` wie ein generisches Zubehörmodul. `Mbc80Com` (Gateway,
kein Hook außer den kommunikationsbezogenen) und `Mbc91Com` (Schaltdecoder, keiner der Hooks
gebraucht) zeigen beide Enden dieses Spektrums.

## Klassenübersicht

**Kern / System**
- `MbcUtils` — Systemarray, Modulidentität, internes EEPROM, Ereignis-/Status-LED-Zeitsteuerung, ADC
  (Busspannung/Steckernetzteil-Erkennung), 1-Wire-Temperatursensor (DS18S20/DS18B20)
- `MbcPlatform` — Plattformabstraktion (Reset, EEPROM-Emulation, ADC-Konfiguration je Zielprozessor)

**CAN**
- `MbcCan` — CAN-Anbindung, wahlweise MCP2515 (SPI) oder ESP32-interne TWAI, mit dem
  Märklin-CS2-29-Bit-Header-Mapping
- `MbcCom` — PC-/CS2-Protokoll-Zustandsautomat (Anmeldung, Datentransfer, Parametrierung),
  Basisklasse für modulspezifische Unterklassen

**Persistenz**
- `MbcBootStorage` — externer SPI-Boot-Speicherchip (Firmware-Update-Staging), zwei austauschbare
  Backends: 24LC1024-EEPROM (ATmega644P) oder W25Q64-Flash (RP2040/ESP32-C3, per Konstruktor-Parameter)
- `MbcOta` — OTA-Firmware-Update aus dem Boot-Speicherchip heraus (RP2040/ESP32); benötigt eine
  reservierte zweite Flash-Partition (ESP32: `partitions.csv` im Sketch-Ordner, Partition Scheme
  "Custom"; RP2040: keine CSV, sondern Tools → Flash Size mit einer "OTA"-Variante), siehe
  `MbcOta.h`-Kopfkommentar

**Nachrichten-Brücken** (austauschbar über das `MbcMsgBridge`-Interface)
- `MbcWlanBridge` — ASCII-Hex-Rahmenprotokoll über UART zu einem externen WLAN-Modul
- `MbcWifiBridge` — natives WLAN/TCP auf ESP32 inkl. Setup-Webportal mit Access-Point-Fallback

**Digitale Ein-/Ausgänge**
- `MbcDigitalOut` / `MbcDigitalIn` — ein einzelner digitaler Aus-/Eingang, wahlweise natives GPIO
  oder ein einzelnes Bit eines `MbcMcp23x17`-Expanders (Backend wird über den Konstruktor gewählt).
  Damit können z. B. `MbcUtils::setLedDrivers()`/`setPlugDetector()` oder die
  `MbcWifiBridge`-Verbindungs-LED auf einen Expander umgeleitet werden, ohne dass die jeweilige
  Klasse selbst etwas davon wissen muss.

**SPI**
- `MbcSpiDevice` — gemeinsamer SPI-Geräte-Wrapper (CS-Pin-Handling, Transaktionen)

**I2C und Peripherie**
- `MbcI2cBus` — dünner I2C-Wrapper
- `MbcMcp23x17` — MCP23017 (I2C) und MCP23S17 (SPI) in einer Klasse vereint (Backend über
  Konstruktor gewählt)
- `MbcPcf8574`, `MbcSsd1306`, `MbcAs5600`, `MbcPcf8583`, `MbcI2cLcd` — weitere I2C-Peripherie

## Abhängigkeiten

Nur Arduino-Core-Bibliotheken: `SPI`, `Wire`, `EEPROM`.

## Verwandte Bibliotheken

- [Mbc80Lib](../Mbc80Lib) — Gateway-Modul mbc-80 (MBCAN-Bus ↔ CS2-Bus ↔ WLAN)
- [Mbc81Lib](../Mbc81Lib) — Stellpult mbc-81 (32 Taster, 16 Stellungs-LEDs, Switchboard/Bus-0-S88-
  Doppelmodus)
- [Mbc88Lib](../Mbc88Lib) — S88-Rückmeldedecoder mbc-88 (8 Kontakte, Halbmodul-Konzept)
- [Mbc90Lib](../Mbc90Lib) — S88-Rückmeldedecoder mbc-90, Vollmodul (16 Kontakte)
- [Mbc91Lib](../Mbc91Lib) — Magnetartikel-Schaltdecoder mbc-91 (4 Doppelspulenantriebe, Stellpult)
- [Mbc92Lib](../Mbc92Lib) — Magnetartikel-Schaltdecoder mbc-92 (4 Doppelspulenantriebe, Stellpult,
  konfigurierbare Ausgangsart)

## Copyright

MBCAN (c) 2007–2026 by Dr.-Ing. Thomas Wiesner
