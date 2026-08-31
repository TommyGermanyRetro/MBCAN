//*************************************************************************************************
//* Modul mbc-84: 4-fach Servo-/Weichendecoder mit Stellpult (Mbc84Lib)                           *
//*************************************************************************************************
//* Datei: Mbc84Com.cpp                                                                           *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen:                                                                                  *
//* V1.0.0 - 18.08.2026 - Nullversion                                                             *
//* V1.0.1 - 29.08.2026 - RAM-Optimierung                                                         *
//*************************************************************************************************

//###### Start Quellcode Mbc84Com.cpp #############################################################

#include "Mbc84Com.h"
#include <string.h>

#if defined(__AVR__)
    #include <avr/interrupt.h>
    #include <util/atomic.h>
#endif

//------ Start interne Konstanten -----------------------------------------------------------------

    // F_CPU=16000000UL, Timer2-Prescaler=128 -> Basiswert der Servo-Pulsbreite in Timerticks
    // (MILLISEC_BASE/CENTER). Hier in physikalische Mikrosekunden umgerechnet (1 Tick = 8us,
    // siehe raw2fine()), damit dieselbe Rohwert-Semantik (0...60-Byte-Rohwert je Endanschlag)
    // unabhaengig vom Zielprozessor nachgebildet werden kann.

    static const uint8_t  MILLISEC_BASE = 124;   // 16000000UL / 128UL / 1000UL - 1UL
    static const uint8_t  CENTER        = 62;    // MILLISEC_BASE / 2
    static const uint16_t TICK_US       = 8;     // 128UL * 1000000UL / 16000000UL

    // Physikalische PWM-Endwerte fuer ein SG90-typisches Hobbyservo (konservativ 1000...2000us
    // statt der oft beworbenen, nicht immer mechanisch nutzbaren 500...2500us). UNABHAENGIG von
    // RT_GN_MAX/SERVO_MAX_STEPS: die Sv1..Sv4-RT/GN-max-Konfigwerte (0...RT_GN_MAX) sind nur noch
    // Stuetzstellen (Aufloesung der CS2-Eingabe), die eepromWriteParaExtern() proportional auf
    // diese physikalische Spanne skaliert (siehe dort) - der Konfigwert RT_GN_MAX bedeutet also
    // "voller Ausschlag bis RAW_MIN/RAW_MAX", nicht mehr woertlich "RT_GN_MAX Rohwert-Einheiten
    // von CENTER entfernt".

    static const uint16_t PULSE_MIN_US = 1000;
    static const uint16_t PULSE_MAX_US = 2000;
    static const uint8_t  RAW_MIN = (uint8_t) (PULSE_MIN_US / TICK_US - MILLISEC_BASE);   // 1
    static const uint8_t  RAW_MAX = (uint8_t) (PULSE_MAX_US / TICK_US - MILLISEC_BASE);   // 126

    // Feinerer Ausgaberaum fuer den Live-Interpolations-/Schreibpfad
    // (raw2fine()/writeServoRawFine()) - unabhaengig von Plattform/Timer, deshalb hier UNBEDINGT
    // (nicht AVR-exklusiv) angelegt. Die gespeicherte Servoweg-Tabelle bleibt grob (8-Bit,
    // RAW_MIN...RAW_MAX); RAW_SCALE ist das Verhaeltnis alter/neuer Tick-Aufloesung
    // (Timer1-Vorteiler 1 statt Timer2-Vorteiler 128: TICK_US_FINE=0,0625us statt 8us ->
    // RAW_SCALE=128). MILLISEC_BASE_FINE/RAW_MIN_FINE/RAW_MAX_FINE nach demselben Muster wie oben
    // aus PULSE_MIN_US/PULSE_MAX_US hergeleitet, NICHT einfach die groben Werte mal RAW_SCALE (das
    // wuerde die Endpunkte verschieben) - siehe raw2fine(). RAW_MIN_FINE/RAW_MAX_FINE rechnen
    // bewusst ueber (unsigned long), da PULSE_MAX_US*RAW_SCALE (2000*128=256000) einen 16-Bit-
    // Zwischenwert ueberlaufen wuerde.

    static const uint16_t RAW_SCALE          = 128;
    static const uint16_t MILLISEC_BASE_FINE = 15999;   // 16000000UL / 1UL / 1000UL - 1UL
    static const uint16_t RAW_MIN_FINE =    // 1
        (uint16_t) ((unsigned long) PULSE_MIN_US * RAW_SCALE / TICK_US - MILLISEC_BASE_FINE);
    static const uint16_t RAW_MAX_FINE =    // 16001
        (uint16_t) ((unsigned long) PULSE_MAX_US * RAW_SCALE / TICK_US - MILLISEC_BASE_FINE);

    static const uint8_t SERVO_MAX_STEPS = 60;

    // Basisgeschwindigkeit der Bewegung (siehe Klassenkommentar in Mbc84Com.h): bei Verzoegerung
    // 0 soll eine volle Rot-Gruen-Bewegung (SERVO_MAX_STEPS Schritte) ca. 1000 ms dauern
    // (Verzoegerung 0 bedeutet also NICHT "sofort"). Eine echte Verzoegerung (1...10) verlangsamt
    // die Bewegung um das (Verzoegerung+1)-fache, siehe servoStep(). Werte >10 werden von
    // eepromWriteParaExtern() verworfen (siehe dort), passend zum Maximum der CSx-GUI-
    // Eingabefelder.

    static const uint8_t VZ_MAX = 10;

    // Maximal zulaessiger Sv1..Sv4-RT/GN-max-Konfigwert - reine Stuetzstellen-Aufloesung der
    // CS2-Eingabe (siehe PULSE_MIN_US/RAW_MIN oben), KEIN Rohwert-Offset mehr. BEWUSST von
    // SERVO_MAX_STEPS abgeleitet statt als eigene Konstante gepflegt, damit eine kuenftige
    // SERVO_MAX_STEPS-Reduktion die Stuetzstellenzahl automatisch mitzieht - sonst genau der
    // Fehler, der bei der letzten Reduktion auf 60 Schritte uebersehen wurde (RT/GN-max blieb
    // faelschlich bei 60 stehen).

    static const uint8_t RT_GN_MAX = SERVO_MAX_STEPS / 2;

    static const unsigned long BASE_STEP_MS = 1000UL / SERVO_MAX_STEPS;   // ~16 ms (60 Schritte)

    // Interpolationstakt in servoStep() (siehe dort) - bewusst FEST und unabhaengig von
    // SERVO_MAX_STEPS/BASE_STEP_MS, damit eine kleinere Schrittanzahl (weniger Stuetzpunkte,
    // siehe Klassenkommentar in Mbc84Com.h) nicht automatisch den Update-Takt zum Servo
    // verlangsamt und die Bewegung wieder ruckelig macht.

    static const unsigned long INTERP_STEP_MS = 8;

    // Sentinel fuer _activeServo (siehe Klassenkommentar in Mbc84Com.h/servoStep()): kein Servo
    // faehrt gerade

    static const uint8_t NO_SERVO_ACTIVE = 0xff;

    #if defined(__AVR__)

        // Rohe Timer1-CTC-Servoansteuerung (siehe Klassenkommentar in Mbc84Com.h) - von Timer2
        // (8-Bit) auf Timer1 (16-Bit) umgezogen, um bei hohem VZ eine feinere Pulsbreitenaufloesung
        // zu bekommen (siehe RAW_SCALE/raw2fine() oben).
        // PRESCALER_BITS/SERVO_ISR_SLOTS sowie die ISR selbst sind bewusst dateiweite statische
        // Variablen statt Klassenmember - eine AVR-ISR kennt kein Objekt (kein this), sie kann nur
        // auf global sichtbare Variablen zugreifen.

        static const uint16_t PRESCALER_BITS = (1 << CS10);   // Timer1-Vorteiler 1

        // 10 Runden a je SERVO_ISR_SLOT_TICKS_FINE Ticks (~2 ms bei Vorteiler 1 und
        // TICK_US_FINE=0,0625us) ergeben den ueblichen 20-ms-Servo-Refreshzyklus; nur die ersten 4
        // Runden haben echte Pins, die restlichen 6 sind reine Fuellrunden fuer die Zykluszeit.

        static const uint8_t SERVO_ISR_SLOTS = 10;

        // Sicherheitsabstand in Ticks, wie er vor dem Timer1-Umzug (Timer2, grober Raum) direkt
        // verwendet wurde - hier nur noch Basis fuer die Fine-Ableitung darunter, nicht mehr
        // selbst als Rundenlaenge-Bestandteil genutzt.

        static const uint8_t SERVO_ISR_MARGIN = 4;

        // Sicherheitsabstand in Fine-Ticks zwischen dem hoechsten je vorkommenden servoIsrValue
        // (MILLISEC_BASE_FINE+RAW_MAX_FINE) und der Rundenlaenge SERVO_ISR_SLOT_TICKS_FINE - siehe
        // dort. Bewusst RAW_SCALE-fach hochskaliert (nicht der grobe Literalwert SERVO_ISR_MARGIN
        // wiederverwendet), damit dasselbe physikalische Zeitpolster wie vor dem Timer1-Umzug
        // erhalten bleibt (32us statt sonst nur 2us).

        static const uint16_t SERVO_ISR_MARGIN_FINE = SERVO_ISR_MARGIN * RAW_SCALE;

        // Rundenlaenge in Fine-Ticks - bewusst mit SERVO_ISR_MARGIN_FINE Sicherheitsabstand ueber
        // MILLISEC_BASE_FINE+RAW_MAX_FINE definiert (nicht exakt gleich): faellt Phase 2 der ISR
        // (SLOT_TICKS_FINE-servoIsrValue) exakt auf 0, kann ein durch ISR-Latenz verspaeteter
        // OCR1A-Schreibzugriff den Compare-Match verpassen - der Timer muss dann einmal komplett
        // durchlaufen (bis zu 65536 Ticks) bevor der naechste Match kommt. Das trifft strukturell
        // IMMER die Fuellrunden (servoIsrValue dort bewusst MILLISEC_BASE_FINE+RAW_MAX_FINE, NICHT
        // SLOT_TICKS_FINE, aus demselben Grund) und in der Folge IMMER zuerst Servo 0 (Sv1), da
        // dessen Slot als einziger echter Pin-Slot unmittelbar auf die Fuellrundenkette folgt
        // (Symptom: vereinzeltes Zucken nur bei Servo1, hardwarebestaetigt vor dem Timer1-Umzug).

        static const uint16_t SERVO_ISR_SLOT_TICKS_FINE =
            (uint16_t) (MILLISEC_BASE_FINE + RAW_MAX_FINE + SERVO_ISR_MARGIN_FINE);

        static volatile uint8_t  *servoIsrPort;                    // gemeinsames PORTx-Register
        static volatile uint8_t   servoIsrBit[4];                  // Bitmaske je Servo im Port
        static volatile uint16_t  servoIsrValue[SERVO_ISR_SLOTS];  // Rohwert Fine-Ticks je Runde
        static volatile uint8_t   servoIsrId;                      // aktuelle Runde

        //*****************************************************************************************
        //* Timer1-Compare-Match-ISR - erzeugt den Servo-Pulszug fuer alle 4 Servos ohne Zutun    *
        //* der Hauptschleife: pro Aufruf entweder Pulsbeginn (Pin high, OCR1A=Pulsbreite in      *
        //* Fine-Ticks) oder Pulsende (Pin low, OCR1A=Rest bis SERVO_ISR_SLOT_TICKS_FINE Ticks),  *
        //* danach bei Pulsende Rundenwechsel. servoIsrValue[] wird ausschliesslich von aussen    *
        //* (writeServoRawFine(), unter ATOMIC_BLOCK) beschrieben, hier nur gelesen.              *
        //*****************************************************************************************

        ISR(TIMER1_COMPA_vect)
        {
            static uint8_t puls = 0;

            if (puls == 1)
            {
                if (servoIsrId < 4) *servoIsrPort &= (uint8_t) ~servoIsrBit[servoIsrId];

                OCR1A = (uint16_t) (SERVO_ISR_SLOT_TICKS_FINE - servoIsrValue[servoIsrId]);

                puls = 0;

                if (++servoIsrId == SERVO_ISR_SLOTS) servoIsrId = 0;
            }
            else
            {
                if (servoIsrId < 4) *servoIsrPort |= servoIsrBit[servoIsrId];

                OCR1A = servoIsrValue[servoIsrId];

                puls = 1;
            }
        }

    #endif

    // CAN-Konstanten (CAN-ID/Sub-Code gemaess Maerklin-Definition), unabhaengig von der
    // MbcCom-Basisklasse fuer die eigene Schaltbefehl-/Rueckmeldezyklus-Auswertung dieses Moduls

    static const uint8_t CAN_ID_ZUBEHOER_SCHALTEN = 0x16;
    static const uint8_t CAN_LOCID_MBCAN          = 0x18;
    static const uint8_t MD_ZYKLUS                = 0x15;   // Anstupsen Rueckmeldung Schaltdecoder

    // Modulspezifische EEPROM-Blockindizes (siehe PC_ARRAY_DATA/MBC_COM_EEPROM in MbcCom.cpp)

    static const uint8_t EE_84_MMAddr    = 0x0A;
    static const uint8_t EE_84_AKTOR     = 0x0B;
    static const uint8_t EE_84_SERVOSMAX = 0x0C;
    static const uint8_t EE_84_SERVOSWEG = 0x0D;
    static const uint8_t EE_84_RUECK     = 0x0E;

    // CS2-Kanalnummer/Zeilenindex des MM-Addr-Konfigkanals (siehe Klassenkommentar in
    // Mbc84Com.h). Die CSx-GUI kann keine Hex-Eingabe, daher wird der CS2 nicht die
    // Basisadresse selbst, sondern eine einfache Modulnummer 0...255 gemeldet/uebergeben - die
    // Basisadresse ergibt sich daraus als MMADDR_BASE + 4*Modulnummer (0x3000...0x33FC)

    static const uint8_t  KANAL_MODNR = MBC_COM_ANZMESSWERTE + MBC_COM_ANZKONFIG + 1;
    static const uint8_t  ROW_MODNR   = 0;
    static const uint16_t MMADDR_BASE = 0x3000;
    static const uint8_t  MODNR_MAX   = 255;

    // CS2-Kanalnummer/Zeilenindex des Meldezyklus-Konfigkanals (Auswahlliste, Kenner 0x01, 2
    // Punkte "disabled"/"enabled") - Offset 3 innerhalb der Zeile ist bei diesem Kenner die
    // "Jetzige Einstellung" (Index in die Liste)

    static const uint8_t KANAL_RUECK  = KANAL_MODNR + 1;
    static const uint8_t ROW_RUECK    = 1;
    static const uint8_t RUECK_OFFSET = 3;

    // CS2-Kanalnummern-/Zeilenindex-Basis der 12 Servo-Endanschlag-/Verzoegerung-Konfigkanaele
    // (Kenner 0x02, numerisch, GUI-Label "SvN" fuer Servo(N-1)): 3 Kanaele je Servo, interleaved
    // RT max/GN max/Delay - Kanal KANAL_SVBASE+3*servo+sub (servo=0..3, sub=0:RT max/1:GN
    // max/2:Delay). Die "Jetzige Einstellung" liegt bei diesem Kenner direkt auf Offset 7 der
    // Zeile (kein eigener OFFSET-Konstante noetig, gleiches Muster wie KANAL_MODNR), siehe
    // cs2IndexParameterExtern()

    static const uint8_t KANAL_SVBASE = KANAL_RUECK + 1;   // 12 Kanaele
    static const uint8_t ROW_SVBASE   = 2;                  // Zeilen 2..13

//------ Ende interne Konstanten ------------------------------------------------------------------

//------ Start Systemarray-Feldoffsets (aus MbcUtils.h abgeleitet) --------------------------------

    static const uint16_t O_MM_H  = MBC_S_GENPURP;
    static const uint16_t O_MM_L  = MBC_S_GENPURP + 1;
    static const uint16_t O_MM    = MBC_S_GENPURP + 2;   // Aktorstellung, Bit(2n)=Gruen,(2n+1)=Rot
    static const uint16_t O_RUECK = MBC_S_GENPURP + 3;
    static const uint16_t O_SVMAX = MBC_S_GENPURP + 4;   // 13 Byte: 4x(Rot,Gruen), Center, 4x VZ
    static const uint16_t O_SVWEG = MBC_S_GENPURP + 17;  // 480 Byte

//------ Ende Systemarray-Feldoffsets -------------------------------------------------------------

//------ Start Klasse Mbc84Com --------------------------------------------------------------------

    // PROGMEM: nur einmal hier deklariert, in beiden Konstruktor-Zweigen per memcpy_P gelesen

    static const uint8_t initCs2Ex[MBC84_ANZMESSWERTEMODUL + MBC84_ANZKONFIGMODUL]
                                   [MBC_COM_CS2PAKETE * 8] PROGMEM =
    {
        // INDEX_CTRL 1.EX.1 - Modulnummer (0...255), Basisadresse = MMADDR_BASE + 4*Modulnummer

        {   KANAL_MODNR,0x02,0x00,0x00,0x00,0xff,0x00,0x00,              // 1...8
            'M','M','-','A','d','d','r',':',                           // 9...16
            0x00,'0',0x00,'2','5','5',0x00,'#',                        // 17...24
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,                    // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                     // 33...40
        },

        // INDEX_CTRL 1.EX.2 - Meldezyklus (Auswahlliste: 0=disabled, 1=enabled)

        {   KANAL_RUECK,0x01,0x02,0x00,0x00,0x00,0x00,0x00,              // 1...8
            'M','e','l','d','e','z','y','k',                            // 9...16
            'l','u','s',':',0x00,'d','i','s',                           // 17...24
            'a','b','l','e','d',0x00,'e','n',                           // 25...32
            'a','b','l','e','d',0x00,0x00,0x00                          // 33...40
        },

        // INDEX_CTRL 1.EX.3 - Sv1 RT max (Rot-Endanschlag, Rohwert 0...30)

        {   KANAL_SVBASE,0x02,0x00,0x00,0x00,RT_GN_MAX,0x00,0x00,       // 1...8
            'S','v','1',' ','R','T',' ','m',                            // 9...16
            'a','x',':',0x00,'0',0x00,'3','0',                          // 17...24
            0x00,'#',0x00,0x00,0x00,0x00,0x00,0x00,                     // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                     // 33...40
        },

        // INDEX_CTRL 1.EX.4 - Sv1 GN max (Gruen-Endanschlag, Rohwert 0...30)

        {   KANAL_SVBASE + 1,0x02,0x00,0x00,0x00,RT_GN_MAX,0x00,0x00,   // 1...8
            'S','v','1',' ','G','N',' ','m',                            // 9...16
            'a','x',':',0x00,'0',0x00,'3','0',                          // 17...24
            0x00,'#',0x00,0x00,0x00,0x00,0x00,0x00,                     // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                     // 33...40
        },

        // INDEX_CTRL 1.EX.5 - Sv1 Delay (Verzoegerung, Rohwert 0...10)

        {   KANAL_SVBASE + 2,0x02,0x00,0x00,0x00,0x0a,0x00,0x00,        // 1...8
            'S','v','1',' ','D','e','l','a',                            // 9...16
            'y',':',0x00,'0',0x00,'1','0',0x00,                         // 17...24
            '#',0x00,0x00,0x00,0x00,0x00,0x00,0x00,                     // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                     // 33...40
        },

        // INDEX_CTRL 1.EX.6 - Sv2 RT max (Rot-Endanschlag, Rohwert 0...30)

        {   KANAL_SVBASE + 3,0x02,0x00,0x00,0x00,RT_GN_MAX,0x00,0x00,   // 1...8
            'S','v','2',' ','R','T',' ','m',                            // 9...16
            'a','x',':',0x00,'0',0x00,'3','0',                          // 17...24
            0x00,'#',0x00,0x00,0x00,0x00,0x00,0x00,                     // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                     // 33...40
        },

        // INDEX_CTRL 1.EX.7 - Sv2 GN max (Gruen-Endanschlag, Rohwert 0...30)

        {   KANAL_SVBASE + 4,0x02,0x00,0x00,0x00,RT_GN_MAX,0x00,0x00,   // 1...8
            'S','v','2',' ','G','N',' ','m',                            // 9...16
            'a','x',':',0x00,'0',0x00,'3','0',                          // 17...24
            0x00,'#',0x00,0x00,0x00,0x00,0x00,0x00,                     // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                     // 33...40
        },

        // INDEX_CTRL 1.EX.8 - Sv2 Delay (Verzoegerung, Rohwert 0...10)

        {   KANAL_SVBASE + 5,0x02,0x00,0x00,0x00,0x0a,0x00,0x00,        // 1...8
            'S','v','2',' ','D','e','l','a',                            // 9...16
            'y',':',0x00,'0',0x00,'1','0',0x00,                         // 17...24
            '#',0x00,0x00,0x00,0x00,0x00,0x00,0x00,                     // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                     // 33...40
        },

        // INDEX_CTRL 1.EX.9 - Sv3 RT max (Rot-Endanschlag, Rohwert 0...30)

        {   KANAL_SVBASE + 6,0x02,0x00,0x00,0x00,RT_GN_MAX,0x00,0x00,   // 1...8
            'S','v','3',' ','R','T',' ','m',                            // 9...16
            'a','x',':',0x00,'0',0x00,'3','0',                          // 17...24
            0x00,'#',0x00,0x00,0x00,0x00,0x00,0x00,                     // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                     // 33...40
        },

        // INDEX_CTRL 1.EX.10 - Sv3 GN max (Gruen-Endanschlag, Rohwert 0...30)

        {   KANAL_SVBASE + 7,0x02,0x00,0x00,0x00,RT_GN_MAX,0x00,0x00,   // 1...8
            'S','v','3',' ','G','N',' ','m',                            // 9...16
            'a','x',':',0x00,'0',0x00,'3','0',                          // 17...24
            0x00,'#',0x00,0x00,0x00,0x00,0x00,0x00,                     // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                     // 33...40
        },

        // INDEX_CTRL 1.EX.11 - Sv3 Delay (Verzoegerung, Rohwert 0...10)

        {   KANAL_SVBASE + 8,0x02,0x00,0x00,0x00,0x0a,0x00,0x00,        // 1...8
            'S','v','3',' ','D','e','l','a',                            // 9...16
            'y',':',0x00,'0',0x00,'1','0',0x00,                         // 17...24
            '#',0x00,0x00,0x00,0x00,0x00,0x00,0x00,                     // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                     // 33...40
        },

        // INDEX_CTRL 1.EX.12 - Sv4 RT max (Rot-Endanschlag, Rohwert 0...30)

        {   KANAL_SVBASE + 9,0x02,0x00,0x00,0x00,RT_GN_MAX,0x00,0x00,   // 1...8
            'S','v','4',' ','R','T',' ','m',                            // 9...16
            'a','x',':',0x00,'0',0x00,'3','0',                          // 17...24
            0x00,'#',0x00,0x00,0x00,0x00,0x00,0x00,                     // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                     // 33...40
        },

        // INDEX_CTRL 1.EX.13 - Sv4 GN max (Gruen-Endanschlag, Rohwert 0...30)

        {   KANAL_SVBASE + 10,0x02,0x00,0x00,0x00,RT_GN_MAX,0x00,0x00,  // 1...8
            'S','v','4',' ','G','N',' ','m',                            // 9...16
            'a','x',':',0x00,'0',0x00,'3','0',                          // 17...24
            0x00,'#',0x00,0x00,0x00,0x00,0x00,0x00,                     // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                     // 33...40
        },

        // INDEX_CTRL 1.EX.14 - Sv4 Delay (Verzoegerung, Rohwert 0...10)

        {   KANAL_SVBASE + 11,0x02,0x00,0x00,0x00,0x0a,0x00,0x00,       // 1...8
            'S','v','4',' ','D','e','l','a',                            // 9...16
            'y',':',0x00,'0',0x00,'1','0',0x00,                         // 17...24
            '#',0x00,0x00,0x00,0x00,0x00,0x00,0x00,                     // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                     // 33...40
        }
    };

    static const uint8_t initMsgKonfigOk[MBC_COM_MSG_LAENGE] PROGMEM =
        {0x00,0x01,0x00,0x00,0x07,0x00,0x00,0x00,0x00,0x0b,0x00,0x01,0x00};

    //*********************************************************************************************
    //* Konstruktor zur Deklaration von Objektvariablen                                           *
    //*********************************************************************************************

    #if defined(__AVR__)

        Mbc84Com::Mbc84Com(MbcUtils &utils, MbcCan &can, MbcBootStorage &bootStorage,
                            const uint8_t servoPins[4], MbcDigitalIn *(&switchboard)[8],
                            MbcDigitalOut *(&led)[8], uint8_t pinReset)
            : MbcCom(utils, can, bootStorage, pinReset,
                     MBC84_ANZMESSWERTEMODUL, MBC84_ANZKONFIGMODUL),
              _pinReset(pinReset)
        {

    #else

        Mbc84Com::Mbc84Com(MbcUtils &utils, MbcCan &can, MbcBootStorage &bootStorage,
                            const uint8_t servoPins[4], MbcDigitalIn *(&switchboard)[8],
                            MbcDigitalOut *(&led)[8])
            : MbcCom(utils, can, bootStorage,
                     MBC84_ANZMESSWERTEMODUL, MBC84_ANZKONFIGMODUL)
        {

    #endif

        memcpy_P(_msgKonfigOk, initMsgKonfigOk, MBC_COM_MSG_LAENGE);

        for (uint8_t i = 0; i < 4; i++) _servoPin[i] = servoPins[i];

        for (uint8_t i = 0; i < 8; i++)
        {
            _switchboard[i]  = switchboard[i];
            _led[i]          = led[i];
            _switchState[i]  = 1;   // Ruhezustand (Taste losgelassen)
        }

        for (uint8_t i = 0; i < 4; i++)
        {
            _servoState[i].step         = 0;
            _servoState[i].dir          = 0;
            _servoState[i].maxRot       = CENTER;
            _servoState[i].maxGruen     = CENTER;
            _servoState[i].vz           = 0;
            _servoState[i].lastStepMs   = 0;
            _servoState[i].lastWriteMs  = 0;
        }

        _zyklusstep  = 0;
        _baseAddr    = 0;
        _activeServo = NO_SERVO_ACTIVE;
    }

    //*********************************************************************************************
    //* Einmalig nach dem begin() der Basisklasse aufrufen: Stellpult-/LED-Pins konfigurieren,    *
    //* gespeicherte Parameter aus der EEPROM-Zusatzregion laden, Servos an ihre Pins binden und  *
    //* in die gespeicherte Stellung fahren. Auf AVR wird dabei zusaetzlich Timer1 fuer die rohe  *
    //* Servo-ISR konfiguriert (siehe Klassenkommentar in Mbc84Com.h) - TCCR1A/B werden dafuer    *
    //* bewusst zurueckgesetzt statt nur ergaenzt, da der Arduino-Core Timer1 beim Start bereits  *
    //* fuer analogWrite()-PWM vorbelegt haben kann.                                              *
    //*********************************************************************************************

    void Mbc84Com::beginModule()
    {
        for (uint8_t i = 0; i < 8; i++)
        {
            _switchboard[i]->begin();
            _led[i]->begin();
        }

        eepromReadParaExtern();

        servoCurveInit();

        #if defined(__AVR__)

            // Gemeinsames PORTx-Register + Bitmasken aus den 4 Servopins ableiten (siehe
            // Klassenkommentar in Mbc84Com.h - alle 4 MUESSEN auf demselben Port liegen)

            servoIsrPort = portOutputRegister(digitalPinToPort(_servoPin[0]));

            for (uint8_t i = 0; i < 4; i++)
            {
                pinMode(_servoPin[i], OUTPUT);
                servoIsrBit[i] = digitalPinToBitMask(_servoPin[i]);
            }

            // Fuellrunden 4..9 fest auf MILLISEC_BASE_FINE+RAW_MAX_FINE - bewusst NICHT
            // SERVO_ISR_SLOT_TICKS_FINE selbst (siehe SERVO_ISR_MARGIN_FINE oben, sonst waere
            // Phase 2 hier immer exakt 0); kein echter Pin betroffen, nur Zykluszeit

            for (uint8_t i = 4; i < SERVO_ISR_SLOTS; i++)
            {
                servoIsrValue[i] = (uint16_t) (MILLISEC_BASE_FINE + RAW_MAX_FINE);
            }

            for (uint8_t i = 0; i < 4; i++) servoValueDirect(i);

            OCR1A      = servoIsrValue[0];
            servoIsrId = 0;

            TCCR1A = 0;
            TCCR1B = 0;

            TCCR1B |= (1 << WGM12);
            TCCR1B |= PRESCALER_BITS;

            TCNT1  = 0;
            TIMSK1 = (1 << OCIE1A);

        #else

            for (uint8_t i = 0; i < 4; i++)
            {
                _servo[i].attach(_servoPin[i]);
                servoValueDirect(i);
            }

        #endif

        ledShow();
    }

    //*********************************************************************************************
    //* Je loop()-Durchlauf aufrufen: Servobewegung nachfuehren, faelligen Rueckmeldezyklus-      *
    //* Schritt versenden, Stellpult auswerten und CAN-Nachrichtenempfang/-Auswertung - ruft      *
    //* intern auch die geerbte check() auf. Schaltbefehl-/Rueckmeldezyklus-Auswertung laeuft     *
    //* direkt hier inline, unabhaengig von der PC-/CS2-Dispatch-Logik der Basisklasse.           *
    //*********************************************************************************************

    void Mbc84Com::update()
    {
        poll();

        servoStep();

        // Rueckmeldezyklus: alle 100 ms einen Schritt weiter (1...4 = Servo 0...3, siehe
        // Klassenkommentar zu _zyklusstep in Mbc84Com.h) - sendet abwechselnd Strom-aus (fuer den
        // vorigen Schritt) und Strom-an (fuer den aktuellen Schritt)

        if (_utils->event100ms() && (_zyklusstep > 0))
        {
            if (_zyklusstep > 1)
            {
                can_msg[MBC_COM_MSG_CS2_0x16][MBC_CAN_D5] = 0x00;

                sendMessage(can_msg[MBC_COM_MSG_CS2_0x16]);
            }

            if (_zyklusstep <= 4)
            {
                uint8_t   servo = (uint8_t) (_zyklusstep - 1);
                uint8_t  *arr   = _utils->array();
                uint8_t   dir   = (uint8_t) ((arr[O_MM] >> (2 * servo)) & 0x03);
                uint16_t  addr  = (uint16_t) (_baseAddr + servo);

                can_msg[MBC_COM_MSG_CS2_0x16][MBC_CAN_D2] = (uint8_t) ((addr & 0xff00) >> 8);
                can_msg[MBC_COM_MSG_CS2_0x16][MBC_CAN_D3] = (uint8_t) (addr & 0x00ff);
                can_msg[MBC_COM_MSG_CS2_0x16][MBC_CAN_D5] = 0x01;

                if (dir == 0x01) can_msg[MBC_COM_MSG_CS2_0x16][MBC_CAN_D4] = 0x01;   // Gruen
                if (dir == 0x02) can_msg[MBC_COM_MSG_CS2_0x16][MBC_CAN_D4] = 0x00;   // Rot

                sendMessage(can_msg[MBC_COM_MSG_CS2_0x16]);

                _zyklusstep++;
            }
            else
            {
                _zyklusstep = 0;
            }
        }

        // Stellpult nur auswerten, wenn kein Rueckmeldezyklus laeuft (vermeidet ueberlappende
        // CAN_ID_ZUBEHOER_SCHALTEN-Telegramme)

        if (_zyklusstep == 0) switchboardCheck();

        // CAN-Nachricht empfangen?

        uint8_t msg[MBC_CAN_MSG_LEN];

        if (_can->receive(msg))
        {
            check(msg);

            bool isSchaltbefehl = (msg[MBC_CAN_ID] == CAN_ID_ZUBEHOER_SCHALTEN)
                               && (msg[MBC_CAN_D2] != CAN_LOCID_MBCAN);

            if (isSchaltbefehl) handleSwitchCommand(msg);

            // Rueckmeldeaufforderung vom vorgelagerten LinkS88: Rueckmeldezyklus starten, wenn
            // gerade keiner laeuft, Rueckmeldung freigegeben und die Basisadresse passt

            bool isZyklusAnstoss = (msg[MBC_CAN_D2] == CAN_LOCID_MBCAN)
                                && (msg[MBC_CAN_D3] == MD_ZYKLUS);

            if (isZyklusAnstoss && (_zyklusstep == 0))
            {
                uint8_t *arr = _utils->array();

                if (arr[O_RUECK])
                {
                    uint16_t addr = (uint16_t) (((uint16_t) msg[MBC_CAN_D6] << 8)
                                               | (uint16_t) msg[MBC_CAN_D7]);

                    if (addr == (uint16_t) (_baseAddr - 0x3000)) _zyklusstep = 1;
                }
            }
        }
    }

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //+ Erweiterungspunkte der Basisklasse (siehe MbcCom.h)                                       +
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    //*********************************************************************************************
    //* Laedt MM-Adresse, Aktorstellung, Servo-Endlagen/-Verzoegerung, Servoweg und               *
    //* Zwangsrueckmeldezyklus aus der EEPROM-Zusatzregion in das Systemarray - unbeschriebene    *
    //* Bytes (0xff) werden dabei auf 0 defaultet. Ist nach dem Laden weder das Rot- noch das     *
    //* Gruen-Bit einer Aktorstellung gesetzt (Erststart), wird das Gruen-Bit angenommen und      *
    //* sofort zurueckgeschrieben.                                                                *
    //*********************************************************************************************

    void Mbc84Com::eepromReadParaExtern()
    {
        uint8_t *arr = _utils->array();

        // Servo-Endlagen/-Verzoegerung (13 Byte)

        _utils->eepromReadExtra(EEX_84_SVMAX, &arr[O_SVMAX], 13);

        for (uint8_t i = 0; i < 13; i++) if (arr[O_SVMAX + i] == 0xff) arr[O_SVMAX + i] = 0;

        // Byte 8 des SVMAX-Blocks ("Center") ist bei uns keine EEPROM-persistierte, sondern eine
        // feste Firmware-Konstante (CENTER) - hier fuer das Parametriercenter (Servoweg-
        // Diagrammskalierung) ins Systemarray eintragen; eepromWriteParaExtern(EE_84_SERVOSMAX)
        // persistiert den ganzen 13-Byte-Block inkl. dieses Bytes automatisch mit

        arr[O_SVMAX + 8] = CENTER;

        // Aktorstellung - unbeschrieben (0xff) auf 0, danach je Servo normalisieren

        uint8_t aktor;

        _utils->eepromReadExtra(EEX_84_AKTOR, &aktor, 1);

        if (aktor == 0xff) aktor = 0;

        for (uint8_t i = 0; i < 4; i++)
        {
            if (((aktor >> (2 * i)) & 0x03) == 0x00) aktor |= (uint8_t) (0x01 << (2 * i));
        }

        arr[O_MM] = aktor;

        eepromWriteParaExtern(EE_84_AKTOR);

        // MM-Adresse - Default 0x30/0x00, wenn das High-Byte unbeschrieben (0xff) ist

        uint8_t addr[2];

        _utils->eepromReadExtra(EEX_84_MMAddr, addr, 2);

        if (addr[0] == 0xff) { addr[0] = 0x30; addr[1] = 0x00; }

        arr[O_MM_H] = addr[0];
        arr[O_MM_L] = addr[1];

        _baseAddr = (uint16_t) (((uint16_t) arr[O_MM_H] << 8) | (uint16_t) arr[O_MM_L]);

        // Zwangsrueckmeldezyklus - unbeschrieben (0xff) auf 0

        uint8_t rueck;

        _utils->eepromReadExtra(EEX_84_RUECK, &rueck, 1);

        arr[O_RUECK] = (rueck == 0xff) ? 0 : rueck;

        // Servoweg

        _utils->eepromReadExtra(EEX_84_SVWEG, &arr[O_SVWEG], 480);

        for (uint16_t i = 0; i < 480; i++) if (arr[O_SVWEG + i] == 0xff) arr[O_SVWEG + i] = 0;
    }

    //*********************************************************************************************
    //* Persistiert je nach block den passenden Ausschnitt des Systemarrays in der EEPROM-        *
    //* Zusatzregion und uebernimmt die Rot-/Gruen-Endanschlaege sowie die Verzoegerung           *
    //* blockunabhaengig neu in die Bewegungszustaende, unabhaengig davon, welcher Block gerade   *
    //* geschrieben wurde. Bei EE_84_MMAddr/_RUECK wird zusaetzlich, wenn passend, sofort ein     *
    //* Rueckmeldezyklus angestossen (siehe unten) - unabhaengig davon, ob die Aenderung ueber    *
    //* CS2 (cs2IndexParameterExtern()) oder ParaCenter (PC_ARRAY_DATA/MBC_COM_EEPROM in          *
    //* MbcCom::check()) kam, da beide Wege hier zusammenlaufen. Ein bereits am Ziel stehender    *
    //* Servo wird dabei sofort auf den neu berechneten Endanschlag nachgezogen, ein gerade       *
    //* fahrender bleibt von servoStep() unangetastet.                                            *
    //*********************************************************************************************

    void Mbc84Com::eepromWriteParaExtern(uint8_t block)
    {
        uint8_t *arr = _utils->array();

        if (block == EE_84_MMAddr)
        {
            _utils->eepromWriteExtra(EEX_84_MMAddr, &arr[O_MM_H], 2);

            _baseAddr = (uint16_t) (((uint16_t) arr[O_MM_H] << 8) | (uint16_t) arr[O_MM_L]);

            // MM-Addr geaendert: Rueckmeldeprozess sofort anstossen, wenn der Meldezyklus
            // aktiviert ist und gerade keiner laeuft

            if (arr[O_RUECK] && (_zyklusstep == 0)) _zyklusstep = 1;
        }
        else if (block == EE_84_AKTOR)
        {
            _utils->eepromWriteExtra(EEX_84_AKTOR, &arr[O_MM], 1);
        }
        else if (block == EE_84_SERVOSMAX)
        {
            _utils->eepromWriteExtra(EEX_84_SVMAX, &arr[O_SVMAX], 13);
        }
        else if (block == EE_84_SERVOSWEG)
        {
            _utils->eepromWriteExtra(EEX_84_SVWEG, &arr[O_SVWEG], 480);
        }
        else if (block == EE_84_RUECK)
        {
            // Alten, noch persistierten Zustand vor dem Ueberschreiben lesen, um den
            // disabled->enabled-Uebergang zu erkennen (siehe unten) - arr[O_RUECK] traegt hier
            // bereits den NEUEN Wert (vom Aufrufer vorher gesetzt)

            uint8_t alt;
            _utils->eepromReadExtra(EEX_84_RUECK, &alt, 1);
            bool warAktiviert = (alt != 0);

            _utils->eepromWriteExtra(EEX_84_RUECK, &arr[O_RUECK], 1);

            // Meldezyklus wurde soeben aktiviert: Rueckmeldeprozess sofort anstossen, wenn
            // gerade keiner laeuft

            if (!warAktiviert && arr[O_RUECK] && (_zyklusstep == 0)) _zyklusstep = 1;
        }

        // Konfigwert (0...RT_GN_MAX, reine Stuetzstelle) proportional auf die physikalische
        // RAW_MIN...RAW_MAX-Spanne skalieren (siehe PULSE_MIN_US/RT_GN_MAX oben) - Konfigwert 0
        // ergibt CENTER, Konfigwert RT_GN_MAX ergibt exakt RAW_MIN bzw. RAW_MAX

        for (uint8_t i = 0; i < 4; i++)
        {
            if (arr[O_SVMAX + 2 * i] <= RT_GN_MAX)
            {
                uint16_t wert = arr[O_SVMAX + 2 * i];

                _servoState[i].maxRot =
                    (uint8_t) (CENTER - (wert * (CENTER - RAW_MIN)) / RT_GN_MAX);
            }

            if (arr[O_SVMAX + 2 * i + 1] <= RT_GN_MAX)
            {
                uint16_t wert = arr[O_SVMAX + 2 * i + 1];

                _servoState[i].maxGruen =
                    (uint8_t) (CENTER + (wert * (RAW_MAX - CENTER)) / RT_GN_MAX);
            }

            if (arr[O_SVMAX + 9 + i] <= VZ_MAX) _servoState[i].vz = arr[O_SVMAX + 9 + i];

            // Ein bereits am Ziel stehender (nicht fahrender) Servo wird von servoStep() NICHT neu
            // angefahren, wenn sich hier nur maxRot/maxGruen aendern - arr[O_MM] und step bleiben
            // ja gleich, servoNeedsStep() sieht also keinen Anlass. Ohne diesen Nachzug wirkt eine
            // geaenderte Grenze bei einem ruhenden Servo daher erst ab dem naechsten echten
            // Schaltbefehl, nicht sofort nach dem Parametrieren.

            if (!servoNeedsStep(i))
            {
                bool gruenCmd = (arr[O_MM] & (0x01 << (2 * i))) != 0;

                writeServoRaw(i, gruenCmd ? _servoState[i].maxGruen : _servoState[i].maxRot);
            }
        }
    }

    //*********************************************************************************************
    //* Traegt GUiD/HASH in die Konfig-Bestaetigungsnachricht _msgKonfigOk ein.                   *
    //*********************************************************************************************

    void Mbc84Com::hashInitExtern()
    {
        uint8_t *arr = _utils->array();

        _msgKonfigOk[MBC_CAN_D0] = arr[MBC_S_KN + 4];
        _msgKonfigOk[MBC_CAN_D1] = arr[MBC_S_GUID + 1];
        _msgKonfigOk[MBC_CAN_D2] = arr[MBC_S_GUID + 2];
        _msgKonfigOk[MBC_CAN_D3] = arr[MBC_S_GUID + 3];

        _msgKonfigOk[MBC_CAN_HASH_H] = can_msg[0][MBC_CAN_HASH_H];
        _msgKonfigOk[MBC_CAN_HASH_L] = can_msg[0][MBC_CAN_HASH_L];
    }

    //*********************************************************************************************
    //* Uebernimmt Konfigkanaele von der CS2: msg[D5]==KANAL_MODNR - neue Modulnummer (0...255)   *
    //* in MMADDR_BASE + 4*Modulnummer umrechnen; msg[D5]==KANAL_RUECK - neue Meldezyklus-Auswahl *
    //* (0=disabled/1=enabled) uebernehmen; msg[D5] im Bereich KANAL_SVBASE..+11 - neuen          *
    //* Rot-/Gruen-Endanschlag (0...RT_GN_MAX) bzw. neue Verzoegerung (0...VZ_MAX) fuer den per   *
    //* Offset kodierten Servo uebernehmen, siehe Klassenkommentar in Mbc84Com.h. Alle 14 Kanaele *
    //* persistieren und werden mit TRUE quittiert - ein ungueltiger Wert bleibt ohne Wirkung und *
    //* wird mit FALSE quittiert.                                                                 *
    //*********************************************************************************************
    //* msg: CAN-Nachricht (msg[D5]==KANAL_MODNR/KANAL_RUECK, msg[D6..D7]=neuer Wert)             *
    //*********************************************************************************************

    void Mbc84Com::cs2IndexParameterExtern(const uint8_t msg[MBC_COM_MSG_LAENGE])
    {
        uint8_t  kanal = msg[MBC_CAN_D5];
        uint16_t wert  = (uint16_t) (((uint16_t) msg[MBC_CAN_D6] << 8) | (uint16_t) msg[MBC_CAN_D7]);

        if (kanal == KANAL_MODNR)
        {
            bool gueltig = (wert <= MODNR_MAX);

            if (gueltig)
            {
                uint16_t addr    = (uint16_t) (MMADDR_BASE + 4 * wert);
                uint8_t  *arr    = _utils->array();
                uint16_t oldAddr = (uint16_t) (((uint16_t) arr[O_MM_H] << 8) | arr[O_MM_L]);

                if (addr != oldAddr)
                {
                    arr[O_MM_H] = (uint8_t) (addr >> 8);
                    arr[O_MM_L] = (uint8_t) (addr & 0x00ff);

                    eepromWriteParaExtern(EE_84_MMAddr);   // persistiert + aktualisiert _baseAddr
                }
            }

            _msgKonfigOk[MBC_CAN_D5] = KANAL_MODNR;
            _msgKonfigOk[MBC_CAN_D6] = gueltig ? 0x01 : 0x00;
        }
        else if (kanal == KANAL_RUECK)
        {
            bool gueltig = (wert <= 1);

            if (gueltig)
            {
                uint8_t *arr = _utils->array();

                if (arr[O_RUECK] != (uint8_t) wert)
                {
                    arr[O_RUECK] = (uint8_t) wert;

                    eepromWriteParaExtern(EE_84_RUECK);   // persistiert + stoesst ggf. Zyklus an
                }
            }

            _msgKonfigOk[MBC_CAN_D5] = KANAL_RUECK;
            _msgKonfigOk[MBC_CAN_D6] = gueltig ? 0x01 : 0x00;
        }
        else if ((kanal >= KANAL_SVBASE) && (kanal < (uint8_t) (KANAL_SVBASE + 12)))
        {
            uint8_t offset = (uint8_t) (kanal - KANAL_SVBASE);
            uint8_t servo  = (uint8_t) (offset / 3);
            uint8_t sub    = (uint8_t) (offset % 3);   // 0=RT max, 1=GN max, 2=Delay

            bool gueltig = (wert <= ((sub == 2) ? VZ_MAX : RT_GN_MAX));

            if (gueltig)
            {
                uint8_t *arr = _utils->array();
                uint8_t *feld = (sub == 0) ? (arr + O_SVMAX + 2 * servo)
                              : (sub == 1) ? (arr + O_SVMAX + 2 * servo + 1)
                                           : (arr + O_SVMAX + 9 + servo);

                if (*feld != (uint8_t) wert)
                {
                    *feld = (uint8_t) wert;

                    eepromWriteParaExtern(EE_84_SERVOSMAX);   // persistiert + aktualisiert
                                                               // _servoState
                }
            }

            _msgKonfigOk[MBC_CAN_D5] = kanal;
            _msgKonfigOk[MBC_CAN_D6] = gueltig ? 0x01 : 0x00;
        }
        else
        {
            return;
        }

        sendMessage(_msgKonfigOk);
    }

    //*********************************************************************************************
    //* Baut die angeforderte Kanalzeile frisch in _cs2ScratchRow zusammen - mbc-84 hat 14        *
    //* Kanaele (ROW_MODNR = MM-Addr, ROW_RUECK = Meldezyklus, ROW_SVBASE+0..11 = 3               *
    //* Endanschlag-/Verzoegerung-Kanaele je Servo), sonst nullptr. Vorlage (initCs2Ex[],         *
    //* PROGMEM) bleibt statisch, nur der aktuelle Wert wird bei jedem Aufruf frisch aus dem      *
    //* Systemarray/_baseAddr gepatcht.                                                            *
    //*********************************************************************************************
    //* laufendeNr: 0..ANZMESSWERTEMODUL+ANZKONFIGMODUL-1                                         *
    //*********************************************************************************************
    //* Return: Zeiger auf _cs2ScratchRow (nur bis zum naechsten Aufruf gueltig) oder nullptr     *
    //*********************************************************************************************

    const uint8_t* Mbc84Com::cs2IndexExtern(uint8_t laufendeNr)
    {
        if (laufendeNr >= (MBC84_ANZMESSWERTEMODUL + MBC84_ANZKONFIGMODUL)) return nullptr;

        memcpy_P(_cs2ScratchRow, initCs2Ex[laufendeNr], sizeof(_cs2ScratchRow));

        uint8_t *arr = _utils->array();

        if (laufendeNr == ROW_MODNR)
        {
            _cs2ScratchRow[7] = (uint8_t) ((_baseAddr - MMADDR_BASE) / 4);
        }
        else if (laufendeNr == ROW_RUECK)
        {
            _cs2ScratchRow[RUECK_OFFSET] = arr[O_RUECK];
        }
        else if ((laufendeNr >= ROW_SVBASE) && (laufendeNr < (uint8_t) (ROW_SVBASE + 12)))
        {
            uint8_t offset = (uint8_t) (laufendeNr - ROW_SVBASE);
            uint8_t servo  = (uint8_t) (offset / 3);
            uint8_t sub    = (uint8_t) (offset % 3);   // 0=RT max, 1=GN max, 2=Delay

            _cs2ScratchRow[7] = (sub == 0) ? arr[O_SVMAX + 2 * servo]
                              : (sub == 1) ? arr[O_SVMAX + 2 * servo + 1]
                                           : arr[O_SVMAX + 9 + servo];
        }

        return _cs2ScratchRow;
    }

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //+ Interne Funktionen                                                                        +
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    //*********************************************************************************************
    //* Eingehenden Schaltbefehl (CAN_ID 0x16, kein S88-Anstupsen) auswerten: bei gueltiger, zu   *
    //* diesem Decoder gehoerender Adresse den Servo schalten und die Bestaetigung an die CSx/MS2 *
    //* zuruecksenden.                                                                            *
    //*********************************************************************************************
    //* msg: empfangene CAN-Nachricht                                                             *
    //*********************************************************************************************

    void Mbc84Com::handleSwitchCommand(const uint8_t msg[MBC_COM_MSG_LAENGE])
    {
        uint16_t addr = (uint16_t) (((uint16_t) msg[MBC_CAN_D2] << 8)
                                     | (uint16_t) msg[MBC_CAN_D3]);

        if ((addr < _baseAddr) || (addr > (uint16_t) (_baseAddr + 3))) return;

        if (msg[MBC_CAN_D5]) setAktorstellung((uint8_t) (addr - _baseAddr), msg[MBC_CAN_D4] == 1);

        // Bestaetigung an die CSx/MS2 senden

        uint8_t ack[MBC_COM_MSG_LAENGE];

        for (uint8_t i = 0; i < MBC_COM_MSG_LAENGE; i++) ack[i] = msg[i];

        ack[MBC_CAN_HASH_H] = can_msg[MBC_COM_MSG_CS2_0x16][MBC_CAN_HASH_H];
        ack[MBC_CAN_HASH_L] = can_msg[MBC_COM_MSG_CS2_0x16][MBC_CAN_HASH_L];
        ack[MBC_CAN_ID]     = 0x17;

        sendMessage(ack);
    }

    //*********************************************************************************************
    //* Rechnet einen groben Tabellenraum-Rohwert (RAW_MIN...RAW_MAX) exakt in den feineren       *
    //* Ausgaberaum um (RAW_MIN_FINE...RAW_MAX_FINE) - affine Umrechnung aus Pulsbreiten-         *
    //* Gleichheit hergeleitet (KEIN einfaches "mal RAW_SCALE", das wuerde die Endpunkte          *
    //* verschieben). Fuer eine Differenz zweier Tabellenwerte kuerzt sich der Offset heraus      *
    //* (diffFine = RAW_SCALE*diff_table), siehe servoStep(). Auf beiden Plattformen unbedingt    *
    //* kompiliert - reine Ganzzahl-Mathematik, kein Hardwarebezug.                               *
    //*********************************************************************************************

    uint16_t Mbc84Com::raw2fine(uint8_t raw)
    {
        return (uint16_t) (RAW_SCALE * ((uint16_t) MILLISEC_BASE + raw) - MILLISEC_BASE_FINE);
    }

    //*********************************************************************************************
    //* Rechnet einen Fine-Rohwert in eine physikalische Pulsbreite in Mikrosekunden um - NUR     *
    //* fuer RP2040/ESP32-C3 (Servo::writeMicroseconds(), siehe writeServoRawFine()), auf AVR     *
    //* arbeitet die rohe Timer1-ISR direkt in Fine-Ticks (kein Mikrosekunden-Umweg noetig).      *
    //* Divisor 16 folgt direkt aus TICK_US_FINE=0,0625us (16 Fine-Ticks = 1us).                  *
    //*********************************************************************************************

    uint16_t Mbc84Com::pulseUsFine(uint16_t rawFine)
    {
        return (uint16_t) ((MILLISEC_BASE_FINE + rawFine) / 16);
    }

    //*********************************************************************************************
    //* Faehrt ein Servo direkt (ohne Bewegungsrampe) in die aktuell kommandierte Endlage und     *
    //* setzt dessen Bewegungszustand auf "am Ziel" - NUR fuer die Start-Synchronisation beim     *
    //* Boot verwendet (siehe beginModule()), damit das Modul nicht bei jedem Einschalten langsam *
    //* in die zuletzt gespeicherte Stellung faehrt. Jede spaetere Stellungsaenderung laeuft      *
    //* ueber die Bewegungsrampe in servoStep().                                                  *
    //*********************************************************************************************

    void Mbc84Com::servoValueDirect(uint8_t servoIdx)
    {
        uint8_t *arr = _utils->array();

        bool gruen = (arr[O_MM] & (0x01 << (2 * servoIdx))) != 0;

        _servoState[servoIdx].step        = gruen ? SERVO_MAX_STEPS : 0;
        _servoState[servoIdx].dir         = gruen ? 1 : 0;
        _servoState[servoIdx].lastStepMs  = millis();
        _servoState[servoIdx].lastWriteMs = millis();

        uint8_t raw = gruen ? _servoState[servoIdx].maxGruen : _servoState[servoIdx].maxRot;

        writeServoRaw(servoIdx, raw);
    }

    //*********************************************************************************************
    //* Liefert den Rohwert der Bewegungsrampe an einem Schrittindex (0...2*SERVO_MAX_STEPS-1)    *
    //* aus dem frei programmierbaren Servoweg-Tabellenspeicher.                                  *
    //*********************************************************************************************
    //* servo: 0...3                                                                              *
    //* step:  0...2*SERVO_MAX_STEPS-1                                                            *
    //*********************************************************************************************
    //* Return: Rohwert (siehe raw2fine())                                                        *
    //*********************************************************************************************

    uint8_t Mbc84Com::rawAtStep(uint8_t servo, uint8_t step)
    {
        return _utils->array()[O_SVWEG + servo * 2 * SERVO_MAX_STEPS + step];
    }

    //*********************************************************************************************
    //* Liefert den Rohwert der Bewegungsrampe an einem Schrittindex, proportional auf den        *
    //* aktuell konfigurierten Rot-/Gruen-Stellbereich (maxRot/maxGruen) skaliert UND direkt in   *
    //* den feinen Ausgaberaum umgerechnet (kombiniert die Stuetzstellen-Transformation und       *
    //* raw2fine() in EINER Rechnung, siehe Konstruktion unten). Die gespeicherte Tabelle         *
    //* (rawAtStep()) ist immer auf die volle physikalische Spanne RAW_MIN...RAW_MAX ausgelegt    *
    //* (siehe servoCurveInit()) - stellt der Nutzer z.B. GN-max kleiner                          *
    //* als das Maximum ein, wuerde ein reines Klemmen (writeServoRawFine()s Clamp) die           *
    //* Kurvenform am neuen, kleineren Anschlag flach abschneiden statt sie zu skalieren.         *
    //* WICHTIG: die Multiplikation mit RAW_SCALE muss VOR der Division durch die Design-Spanne   *
    //* erfolgen, nicht danach (nicht raw2fine(grobes-Zwischenergebnis) rechnen!) - sonst rundet  *
    //* ein Zwischenschritt schon auf grobe 8-Bit-Rohwert-Aufloesung, bevor die Fine-Umrechnung   *
    //* ueberhaupt ansetzt. Bei einem kleinen konfigurierten Stellbereich (z.B. RT-max=GN-max=7,  *
    //* Spanne nur 28 statt 125 Ticks) faellt diese Rundung besonders ins Gewicht - mehrere       *
    //* Design-Kurvenpunkte wuerden auf denselben groben Rohwert fallen und die Interpolation     *
    //* wieder ruckelig machen (hardwarebestaetigt), genau das Problem, das der Timer1-Umzug      *
    //* eigentlich beheben sollte.                                                                *
    //*********************************************************************************************
    //* servo: 0...3                                                                              *
    //* step:  0...2*SERVO_MAX_STEPS-1                                                            *
    //*********************************************************************************************
    //* Return: Fine-Rohwert, skaliert auf [maxRot...maxGruen] (siehe raw2fine())                 *
    //*********************************************************************************************

    long Mbc84Com::rawAtStepScaledFine(uint8_t servo, uint8_t step)
    {
        uint8_t     r          = rawAtStep(servo, step);
        ServoState &s          = _servoState[servo];
        long        centerFine = raw2fine(CENTER);

        if (r <= CENTER)
        {
            long span = (long) (CENTER - RAW_MIN);

            return centerFine
                   - ((long) (CENTER - r) * RAW_SCALE * (CENTER - s.maxRot)) / span;
        }
        else
        {
            long span = (long) (RAW_MAX - CENTER);

            return centerFine
                   + ((long) (r - CENTER) * RAW_SCALE * (s.maxGruen - CENTER)) / span;
        }
    }

    //*********************************************************************************************
    //* Legt bei Erststart (Servoweg-Bereich noch komplett 0) einen linearen Standardweg zwischen *
    //* den beiden physikalischen PWM-Endlagen (RAW_MIN...RAW_MAX, siehe interne Konstanten) an,  *
    //* identisch fuer alle 4 Servos. Der Rohwert-Zuwachs je Schritt ist proportional zu          *
    //* SERVO_MAX_STEPS skaliert (nicht fest +-1), damit die Default-Kurve bei JEDER              *
    //* Schrittanzahl weiterhin die volle physikalische Spanne abdeckt.                           *
    //*********************************************************************************************

    void Mbc84Com::servoCurveInit()
    {
        uint8_t *step = &_utils->array()[O_SVWEG];

        if (step[0] != 0) return;

        step[0] = RAW_MIN;
        step[1 * (2 * SERVO_MAX_STEPS)] = step[0];
        step[2 * (2 * SERVO_MAX_STEPS)] = step[0];
        step[3 * (2 * SERVO_MAX_STEPS)] = step[0];

        // Rohwert-Zuwachs je Schritt proportional zur Schrittanzahl skaliert (nicht fest +1) -
        // deckt bei JEDER SERVO_MAX_STEPS-Groesse weiterhin die volle RAW_MIN...RAW_MAX-Spanne
        // ab, siehe Klassenkommentar

        for (uint16_t i = 1; i < SERVO_MAX_STEPS; i++)
        {
            step[i] = (uint8_t) (step[0] + (i * (RAW_MAX - RAW_MIN)) / SERVO_MAX_STEPS);
            step[i + 1 * (2 * SERVO_MAX_STEPS)] = step[i];
            step[i + 2 * (2 * SERVO_MAX_STEPS)] = step[i];
            step[i + 3 * (2 * SERVO_MAX_STEPS)] = step[i];
        }

        // Peak EXAKT auf RAW_MAX setzen (kein Duplikat des ggf. abgerundeten letzten
        // Schleifenwerts) - das ist zugleich der finale Ruhewert bei Gruen (siehe servoStep()),
        // muss also die physikalische Obergrenze wirklich exakt erreichen koennen

        step[SERVO_MAX_STEPS] = RAW_MAX;
        step[SERVO_MAX_STEPS + 1 * (2 * SERVO_MAX_STEPS)] = step[SERVO_MAX_STEPS];
        step[SERVO_MAX_STEPS + 2 * (2 * SERVO_MAX_STEPS)] = step[SERVO_MAX_STEPS];
        step[SERVO_MAX_STEPS + 3 * (2 * SERVO_MAX_STEPS)] = step[SERVO_MAX_STEPS];

        for (uint16_t i = SERVO_MAX_STEPS + 1; i < (2 * SERVO_MAX_STEPS); i++)
        {
            uint16_t back = i - SERVO_MAX_STEPS;

            step[i] =
                (uint8_t) (step[SERVO_MAX_STEPS] - (back * (RAW_MAX - RAW_MIN)) / SERVO_MAX_STEPS);
            step[i + 1 * (2 * SERVO_MAX_STEPS)] = step[i];
            step[i + 2 * (2 * SERVO_MAX_STEPS)] = step[i];
            step[i + 3 * (2 * SERVO_MAX_STEPS)] = step[i];
        }
    }

    //*********************************************************************************************
    //* Ermittelt, ob ein Servo noch eine ausstehende Bewegung hat: die kommandierte Endlage      *
    //* (Gruen/Rot laut Systemarray) ist genau dann erreicht, wenn step exakt SERVO_MAX_STEPS     *
    //* (Gruen) bzw. 0 (Rot) betraegt - beide Pfade in servoStep() (normale Vorwaertsbewegung UND *
    //* Sofort-Sprung bei Richtungswechsel) laufen auf denselben Endwert zusammen. Reine Abfrage  *
    //* ohne Seiteneffekt, siehe servoStep().                                                     *
    //*********************************************************************************************
    //* servo: 0...3                                                                              *
    //*********************************************************************************************
    //* Return: true, wenn dieser Servo noch fahren muesste                                       *
    //*********************************************************************************************

    bool Mbc84Com::servoNeedsStep(uint8_t servo)
    {
        bool gruenCmd = (_utils->array()[O_MM] & (0x01 << (2 * servo))) != 0;

        return gruenCmd ? (_servoState[servo].step != SERVO_MAX_STEPS)
                         : (_servoState[servo].step != 0);
    }

    //*********************************************************************************************
    //* Rechnet einen groben Tabellenraum-Rohwert in den Fine-Raum um und delegiert an            *
    //* writeServoRawFine() - gemeinsamer Einstiegspunkt fuer den Sofort-Sprung bei               *
    //* Richtungswechsel (servoStep()) und servoValueDirect() (Boot-Sync), beide bewusst OHNE     *
    //* Interpolation.                                                                            *
    //*********************************************************************************************
    //* servo: 0...3                                                                              *
    //* raw:   unbeschraenkter Rohwert (siehe rawAtStep())                                        *
    //*********************************************************************************************

    void Mbc84Com::writeServoRaw(uint8_t servo, uint8_t raw)
    {
        writeServoRawFine(servo, raw2fine(raw));
    }

    //*********************************************************************************************
    //* Klemmt rawFine auf den (in den Fine-Raum umgerechneten) konfigurierten Rot-/Gruen-        *
    //* Endanschlag dieses Servos und sendet ihn als Pulsbreite - gemeinsamer Schreibpfad fuer    *
    //* writeServoRaw() UND die normale, interpolierte Bewegung in servoStep() (dort direkt       *
    //* aufgerufen, um die volle Fine-Aufloesung zu nutzen). Auf AVR wird servoIsrValue[] unter   *
    //* ATOMIC_BLOCK beschrieben (ein 16-Bit-Zugriff ist NICHT atomar wie beim vorherigen 8-Bit-  *
    //* Timer2), auf RP2040/ESP32-C3 ueber die Arduino-Bibliothek Servo.                          *
    //*********************************************************************************************
    //* servo:   0...3                                                                            *
    //* rawFine: unbeschraenkter Fine-Rohwert (siehe raw2fine())                                  *
    //*********************************************************************************************

    void Mbc84Com::writeServoRawFine(uint8_t servo, uint16_t rawFine)
    {
        ServoState &s = _servoState[servo];

        uint16_t minFine = raw2fine(s.maxRot);
        uint16_t maxFine = raw2fine(s.maxGruen);

        if (rawFine < minFine)      rawFine = minFine;
        else if (rawFine > maxFine) rawFine = maxFine;

        #if defined(__AVR__)
            ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
            {
                servoIsrValue[servo] = (uint16_t) (MILLISEC_BASE_FINE + rawFine);
            }
        #else
            _servo[servo].writeMicroseconds(pulseUsFine(rawFine));
        #endif
    }

    //*********************************************************************************************
    //* Bewegungs-Zustandsmaschine (millis()-getaktet und entkoppelt von der eigentlichen         *
    //* Pulserzeugung, siehe ISR(TIMER1_COMPA_vect) weiter oben): faehrt pro Aufruf hoechstens    *
    //* EINEN Servo weiter - damit ist zu jedem Zeitpunkt hoechstens ein Servo in der Fahrphase   *
    //* (siehe Klassenkommentar in Mbc84Com.h). Bei einem Richtungswechsel waehrend der Fahrt     *
    //* springt der Servo sofort (naechster INTERP_STEP_MS-Takt) zur Gegenendlage. Bei normaler   *
    //* Bewegung wird dagegen IMMER alle INTERP_STEP_MS ein linear zwischen dem aktuellen und dem *
    //* naechsten Tabellenschritt interpolierter Zwischenwert gesendet, unabhaengig von vz - der  *
    //* Tabellenschritt selbst wechselt weiterhin erst nach dem vollen, vz-gestreckten Intervall. *
    //* Das haelt die Update-Rate zum Servo immer hoch genug fuer eine ruckelfreie Bewegung, auch *
    //* bei hoher Verzoegerung (siehe Klassenkommentar in Mbc84Com.h). Damit dabei trotzdem alle  *
    //* Servos gleichzeitig vorankommen (nicht erst der naechste, wenn der vorherige fertig ist), *
    //* rueckt der Rundlaufzeiger _activeServo nach jedem tatsaechlich ausgefuehrten Schritt zum  *
    //* naechsten Servo weiter; die Suche nach einem faelligen Servo beginnt dabei stets ab       *
    //* diesem Zeiger.                                                                            *
    //*********************************************************************************************

    void Mbc84Com::servoStep()
    {
        unsigned long now   = millis();
        uint8_t      *arr   = _utils->array();
        uint8_t       servo = (_activeServo == NO_SERVO_ACTIVE) ? 0 : _activeServo;

        for (uint8_t tries = 0; tries < 4; tries++)
        {
            ServoState &s = _servoState[servo];

            if (servoNeedsStep(servo) && ((now - s.lastWriteMs) >= INTERP_STEP_MS))
            {
                s.lastWriteMs = now;

                bool gruenCmd = (arr[O_MM] & (0x01 << (2 * servo))) != 0;
                bool rotCmd   = (arr[O_MM] & (0x02 << (2 * servo))) != 0;

                // Sofort-Sprung bei Richtungswechsel waehrend der Fahrt (siehe Klassenkommentar
                // zu servoNeedsStep())

                if ((gruenCmd && (s.step < (2 * SERVO_MAX_STEPS)) && (s.dir == 1)) ||
                    (rotCmd   && (s.step < SERVO_MAX_STEPS)        && (s.dir == 0)))
                {
                    s.step       = gruenCmd ? SERVO_MAX_STEPS : 0;
                    s.dir        = gruenCmd ? 1 : 0;
                    s.lastStepMs = now;

                    writeServoRawFine(servo, (uint16_t) rawAtStepScaledFine(servo, s.step));

                    _activeServo = (uint8_t) ((servo + 1) % 4);
                    return;
                }

                // Normale Bewegung: Tabellenschritt nach dem vollen, vz-gestreckten Intervall
                // fortschalten (unveraendert), dazwischen alle INTERP_STEP_MS interpolieren

                unsigned long interval = BASE_STEP_MS * ((unsigned long) s.vz + 1);
                unsigned long elapsed  = now - s.lastStepMs;

                if (elapsed >= interval)
                {
                    elapsed      = 0;
                    s.lastStepMs = now;

                    if (gruenCmd && (s.step < SERVO_MAX_STEPS) && (s.dir == 0))
                    {
                        s.step++;

                        if (s.step == SERVO_MAX_STEPS) s.dir = 1;
                    }

                    if (rotCmd && (s.step < (2 * SERVO_MAX_STEPS)) && (s.dir == 1))
                    {
                        s.step++;

                        if (s.step == (2 * SERVO_MAX_STEPS)) { s.dir = 0; s.step = 0; }
                    }
                }

                uint8_t stepNext = (uint8_t) (s.step + 1);

                if (stepNext == (2 * SERVO_MAX_STEPS)) stepNext = 0;

                long rawFineCur = rawAtStepScaledFine(servo, s.step);
                long diffFine   = rawAtStepScaledFine(servo, stepNext) - rawFineCur;
                long rawFine    = rawFineCur + (diffFine * (long) elapsed) / (long) interval;

                writeServoRawFine(servo, (uint16_t) rawFine);

                _activeServo = (uint8_t) ((servo + 1) % 4);
                return;
            }

            servo = (uint8_t) ((servo + 1) % 4);
        }

        _activeServo = servo;
    }

    //*********************************************************************************************
    //* Wertet die 8 Stellpulteingaenge aus (steigende Flanke pro Taste, hoechster Index gewinnt  *
    //* bei mehreren gleichzeitig neu gedrueckten Tasten) und sendet bei einer Betaetigung sofort *
    //* die neue Stellung auf den MBCAN-Bus.                                                      *
    //*********************************************************************************************

    void Mbc84Com::switchboardCheck()
    {
        uint8_t servoIdx  = 0;
        uint8_t gruenFlag = 0;
        bool    pressed   = false;

        for (uint8_t i = 0; i < 8; i++)
        {
            bool active = _switchboard[i]->read();

            if (active && (_switchState[i] == 1))
            {
                servoIdx  = (uint8_t) (i / 2);
                gruenFlag = (uint8_t) (i % 2);   // gerade i = Rot, ungerade i = Gruen (siehe
                                                  // Klassenkommentar in Mbc84Com.h)
                pressed = true;
                _switchState[i] = 0;
            }

            if (!active && (_switchState[i] == 0)) _switchState[i] = 1;
        }

        if (!pressed) return;

        setAktorstellung(servoIdx, gruenFlag == 1);

        uint16_t addr = (uint16_t) (_baseAddr + servoIdx);

        can_msg[MBC_COM_MSG_CS2_0x16][MBC_CAN_D2] = (uint8_t) ((addr & 0xff00) >> 8);
        can_msg[MBC_COM_MSG_CS2_0x16][MBC_CAN_D3] = (uint8_t) (addr & 0x00ff);
        can_msg[MBC_COM_MSG_CS2_0x16][MBC_CAN_D4] = gruenFlag;
        can_msg[MBC_COM_MSG_CS2_0x16][MBC_CAN_D5] = 0x01;

        sendMessage(can_msg[MBC_COM_MSG_CS2_0x16]);
    }

    //*********************************************************************************************
    //* Gibt die aktuelle Aktorstellung auf den 8 Positions-LEDs aus - led[i] entspricht dabei    *
    //* direkt Bit i der Aktorstellung (siehe Klassenkommentar in Mbc84Com.h: die physische Pin-  *
    //* Zuordnung je Board legt der Sketch fest).                                                 *
    //*********************************************************************************************

    void Mbc84Com::ledShow()
    {
        uint8_t mm = _utils->array()[O_MM];

        for (uint8_t i = 0; i < 8; i++) _led[i]->write((mm & (0x01 << i)) != 0);
    }

    //*********************************************************************************************
    //* Setzt die Aktorstellung eines Servos (Bit(2n)=Gruen, Bit(2n+1)=Rot), persistiert sie und  *
    //* aktualisiert die Positions-LEDs. Die eigentliche Servobewegung entlang der Rampe          *
    //* uebernimmt servoStep() im naechsten Bewegungstakt - siehe Klassenkommentar in             *
    //* Mbc84Com.h zur Geschwindigkeit.                                                           *
    //*********************************************************************************************
    //* servoIdx: 0...3                                                                           *
    //* gruen:    true = Gruen kommandieren, false = Rot kommandieren                             *
    //*********************************************************************************************

    void Mbc84Com::setAktorstellung(uint8_t servoIdx, bool gruen)
    {
        uint8_t *arr   = _utils->array();
        uint8_t  shift = (uint8_t) (2 * servoIdx);

        arr[O_MM] = (uint8_t) ((arr[O_MM] & ~(0x03 << shift)) | ((gruen ? 0x01 : 0x02) << shift));

        eepromWriteParaExtern(EE_84_AKTOR);

        ledShow();
    }

//------ Ende Klasse Mbc84Com ---------------------------------------------------------------------

//###### Ende Quellcode Mbc84Com.cpp ##############################################################
