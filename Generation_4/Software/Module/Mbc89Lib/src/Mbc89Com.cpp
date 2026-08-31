//*************************************************************************************************
//* Modul mbc-89: Connect6021-Interface (Mbc89Lib)                                                *
//*************************************************************************************************
//*************************************************************************************************
//* Datei: Mbc89Com.cpp                                                                           *
//*************************************************************************************************
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//*************************************************************************************************
//* Aenderungen:                                                                                  *
//* V1.0.0 - 26.08.2026 - Nullversion                                                             *
//* V1.0.1 - 29.08.2026 - KeyBx/GroupY-Konfigkanaele, AVR-/ESP32-Kompilierfixes                   *
//*************************************************************************************************

//###### Start Quellcode Mbc89Com.cpp #############################################################

#include "Mbc89Com.h"

#if defined(__AVR__)
    #include <avr/interrupt.h>
    #include <util/atomic.h>
#else
    #include <esp_timer.h>
    #include <driver/rmt_rx.h>
    #include <freertos/ringbuf.h>
#endif

//------ Start interne Konstanten -----------------------------------------------------------------

    // Rohe CAN-IDs (Sub-CMD in D4 bzw. eigenstaendige IDs) - mbc-89 wertet System-Stopp/-Go, Lok-
    // Abfragen und CONNECT6021 direkt in update() aus, unabhaengig von der PC-/CS2-Dispatch-Logik
    // der Basisklasse (siehe handleSystemCommand()/handleLokAbfrage()/handleConnect6021()).
    // CAN_ID_CONNECT6021/_R (0x44/0x45) sind bereits aus der CANMON-Portierung dieses Projekts
    // bekannt.

    static const uint8_t CAN_ID_SYSTEM_BEFEHLE      = 0x00;
    static const uint8_t CAN_ID_LOK_GESCHWINDIGKEIT = 0x08;
    static const uint8_t CAN_ID_LOK_RICHTUNG        = 0x0A;
    static const uint8_t CAN_ID_LOK_FUNKTION        = 0x0C;
    static const uint8_t CAN_ID_KEYBOARD            = 0x16;
    static const uint8_t CAN_ID_CONNECT6021         = 0x44;
    static const uint8_t CAN_ID_CONNECT6021_R       = 0x45;

    // Modulspezifische EEPROM-Blockindizes (kommen 1:1 aus mbc_modul_89.vb/mbc_defs.h -
    // PC_ARRAY_DATA sendet genau diese Werte als Schreib-Identifier)

    static const uint8_t EE_89_SW        = 0x0A;
    static const uint8_t EE_89_PROTOKOLL = 0x0B;
    static const uint8_t EE_89_MAPPING   = 0x0C;
    static const uint8_t EE_89_BASIS     = 0x0D;
    static const uint8_t EE_89_I2C       = 0x0E;

    // Byteoffsets in der EEPROM-Zusatzregion (extraEepromSize=70, siehe Beispiel-Sketch) - 5
    // Bloecke direkt hintereinander, Reihenfolge wie im Original

    static const uint16_t EEX_89_SW        = 0;    // 2 Byte: SW_1, SW_2
    static const uint16_t EEX_89_PROTOKOLL = 2;    // 65 Byte: Protokoll je Stellpult (MM/DCC)
    static const uint16_t EEX_89_MAPPING   = 67;   // 1 Byte: Zentralentyp (0=CSx, 1=MS2)
    static const uint16_t EEX_89_BASIS     = 68;   // 1 Byte: Basisadressblock 1...4
    static const uint16_t EEX_89_I2C       = 69;   // 1 Byte: I2C-Frequenz in kHz

    // CS2-Kanalnummern/Zeilenindizes der modulspezifischen Konfigkanaele "SW Major"/"SW Minor" -
    // beide teilen sich denselben EEPROM-Block EE_89_SW (arr[MBC_89_S_SW]/[+1] werden immer
    // gemeinsam persistiert) - 1:1 aus Mbc80Lib uebernommenes Muster (siehe Projekt-Memory
    // project_mbcan_cs2_konfigkanal_pattern)

    static const uint8_t KANAL_SW_MAJ = MBC_COM_ANZMESSWERTE + MBC_COM_ANZKONFIG + 1;  // 7
    static const uint8_t KANAL_SW_MIN = KANAL_SW_MAJ + 1;                              // 8
    static const uint8_t ROW_SW_MAJ   = 0;
    static const uint8_t ROW_SW_MIN   = 1;
    static const uint8_t SW_OFFSET_H  = 6;   // "Aktueller Wert High" innerhalb der Zeile
    static const uint8_t SW_OFFSET_L  = 7;   // "Aktueller Wert Low" innerhalb der Zeile

    // CS2-Kanalnummer/Zeilenindex des modulspezifischen Konfigkanals "Mapping" (Zentralentyp) -
    // Auswahlliste (Kenner 0x01) mit 2 Optionen "Mapping"/"Mapping&Direkt", 1:1 aus dem
    // ParaCenter-CheckedListBox clb_mbc89_Mapping uebernommen (Index 0/1). WICHTIG: bei Kenner 0x01
    // sitzt die "Jetzige Einstellung" auf Offset 3, NICHT auf Offset 7 wie bei Kenner 0x02/Slider
    // (siehe Projekt-Memory project_mbcan_cs2_konfigkanal_pattern, 3. dokumentierter Hardwarebug).

    static const uint8_t KANAL_MAPPING    = KANAL_SW_MIN + 1;  // 9
    static const uint8_t ROW_MAPPING      = 2;
    static const uint8_t MAPPING_OFFSET   = 3;

    // CS2-Kanalnummern/Zeilenindizes der modulspezifischen Konfigkanaele "Basis"/"I2C" - beide
    // Slider (Kenner 0x02, gleiches Zeilenformat wie SW Major/Minor). Grenzen 1:1 aus
    // nud_mbc89_basis (Basisadressblock 1...4, umgerechnet aus dem raw NUD-Wert) bzw.
    // nud_mbc89_i2c (I2C-Bus-Takt 40...100 kHz) im ParaCenter uebernommen. Jeweils eigener
    // EEPROM-Block (EE_89_BASIS/EE_89_I2C), kein gemeinsamer Block wie bei SW Major/Minor.

    static const uint8_t KANAL_BASIS   = KANAL_MAPPING + 1;  // 10
    static const uint8_t ROW_BASIS     = 3;
    static const uint8_t BASIS_OFFSET_H = 6;
    static const uint8_t BASIS_OFFSET_L = 7;

    static const uint8_t KANAL_I2C     = KANAL_BASIS + 1;  // 11
    static const uint8_t ROW_I2C       = 4;
    static const uint8_t I2C_OFFSET_H  = 6;
    static const uint8_t I2C_OFFSET_L  = 7;

    // CS2-Kanalnummern/Zeilenindizes der 64 modulspezifischen Konfigkanaele "KeyBx GroupY:" - je
    // Keyboard (x=1...16) und Stellpultblock/Gruppe (y=1...4) ein Auswahlliste-Kanal (Kenner
    // 0x01, 2 Optionen MM/DCC), 1:1 aus dem ParaCenter-Array Protokoll(keyboard*4+gruppe)
    // uebernommen (siehe mbc_modul_89.vb, Optionsreihenfolge aus clb_mbc89_A/B/C/D: Index 0="MM",
    // Index 1="DCC" - passt zum Firmware-Bit in cuI2cCheckFn(), 0=0x3000/MM, 1=0x3800/DCC).
    // War bisher nur ueber PC_ARRAY_DATA/ParaCenter setzbar. Alle 64 Zeilen liegen wie die
    // fuenf klassischen Kanaele als Literal-PROGMEM-Zeilen in initCs2Ex[] (siehe dort) - der
    // Byte-Inhalt wurde geskriptet erzeugt und gegengeprueft statt von Hand ausgezaehlt
    // (Tippfehlerrisiko bei 64 Eintraegen).

    static const uint8_t KANAL_PROTOKOLL_BASE   = KANAL_I2C + 1;  // 12...75
    static const uint8_t ROW_PROTOKOLL_BASE     = ROW_I2C + 1;    // 5...68
    static const uint8_t PROTOKOLL_ANZ          = 64;             // 16 Keyboards x 4 Gruppen
    static const uint8_t PROTOKOLL_VALUE_OFFSET = 3;              // "Jetzige Einstellung", wie MAPPING_OFFSET

    // Systemarray-Layout ab MBC_S_GENPURP (verbindlich aus mbc_modul_89.vb/mbc_defs.h) -
    // genPurpSize (880 Byte) ergibt sich vollstaendig aus diesen Offsets, kein CS2-Kanal noetig

    static const uint16_t MBC_89_S_SW      = MBC_S_GENPURP;
    static const uint16_t MBC_89_S_LOKS    = MBC_89_S_SW + 2;
    static const uint16_t MBC_89_S_MAGNET  = MBC_89_S_LOKS + 810;
    static const uint16_t MBC_89_S_MAPPING = MBC_89_S_MAGNET + 65;
    static const uint16_t MBC_89_S_BASIS   = MBC_89_S_MAPPING + 1;
    static const uint16_t MBC_89_S_I2C     = MBC_89_S_BASIS + 1;

    // Lok-Datenbank (810 Byte, 10 Byte je der 80 MM-Adressen 0...79 - MM-Adresse 0 auf dem
    // I2C-/Gleis-Bus entspricht CS2-Slot 80, siehe cuInitFn()). Feldlayout 1:1 aus
    // MBC_89_LOK_*-Makros von mbc_defs.h uebernommen.

    static const uint8_t LOKS_ANZAHL = 80;
    static const uint8_t LOKS_ITEMS  = 10;

    static const uint8_t LOK_SPEED  = 0;   // aktuelle MM-Fahrstufe (0...14)
    static const uint8_t LOK_DIR    = 1;   // 1=vorwaerts, 2=rueckwaerts
    static const uint8_t LOK_F0     = 2;
    static const uint8_t LOK_FX     = 3;   // Bit0=F1...Bit3=F4
    static const uint8_t LOK_ADDR4  = 4;   // GUiD Byte 4 (hoechstwertig)
    static const uint8_t LOK_ADDR3  = 5;
    static const uint8_t LOK_ADDR2  = 6;
    static const uint8_t LOK_ADDR1  = 7;   // GUiD Byte 1 (niedrigstwertig)
    static const uint8_t LOK_MAPPED = 8;   // Bit0=aktuell gemappt, Bit1=LOK_MAPPED_ECHT (siehe unten)
    static const uint8_t LOK_INFRA  = 9;   // wird gerade vom Gleisdecoder gesteuert (0/1)

    // lok[LOK_MAPPED] traegt zwei Bits statt eines: Bit0 ist der eigentliche Mapping-Zustand (wie
    // ueberall im Code per Wahrheitswert abgefragt), Bit1 (LOK_MAPPED_ECHT) markiert dauerhaft
    // "diese Lok wurde bereits per CONNECT6021 44/45 echt zugewiesen" - gesetzt einzig in
    // handleConnect6021(), geloescht einzig durch einen echten Reboot in cuInitFn(). Ein
    // KANAL_MAPPING-Wechsel darf eine so markierte Lok nie anfassen, unabhaengig davon, wie oft der
    // Zentralentyp seitdem hin- und hergeschaltet wurde (siehe cs2IndexParameterExtern()).

    static const uint8_t LOK_MAPPED_ECHT = 0x02;

    static const uint8_t  DIR_VORWAERTS   = 1;
    static const uint8_t  DIR_RUECKWAERTS = 2;
    static const uint8_t  DIR_UMSCHALTEN  = 3;
    static const uint16_t MM_SPEED_STEP   = 68;   // CS2-Fahrstufenschrittweite je MM-Stufe

    #if defined(__AVR__)

        // CU-Bus-Pinbelegung (PORTC, fest durch die ATmega644P-Hardware vorgegeben - siehe
        // Pintabelle im Bibliotheks-README, NICHT als Konstruktor-Parameter waehlbar). PC0/PC1
        // (SCL/SDA) sind zusaetzlich durch die AVR-TWI-Hardwareperipherie fest belegt.

        static const uint8_t BIT_CU_SCL      = 0;   // PC0 (TWI-Hardware)
        static const uint8_t BIT_CU_SDA      = 1;   // PC1 (TWI-Hardware)
        static const uint8_t BIT_CU_STOP_IN  = 3;   // PC3
        static const uint8_t BIT_CU_STOP_OUT = 4;   // PC4
        static const uint8_t BIT_CU_EXT      = 5;   // PC5 (CU-Anwesenheit, ohne Pull-Up)
        static const uint8_t BIT_CU_GO_IN    = 6;   // PC6
        static const uint8_t BIT_CU_GO_OUT   = 7;   // PC7
        static const uint8_t BIT_TRACK       = 2;   // PD2 (INT0, Gleissignal)

    #else

        // ESP32-C3: freie GPIO-Wahl, aus dem Konstruktor uebernommen (siehe Mbc89Com.h) - Bruecke
        // fuer die freistehenden CU-Bus-/Gleisdecoder-Funktionen analog zu cuUtils/cuOwner.

        static uint8_t cuPinScl, cuPinSda, cuPinTrack;

        // STOP/GO/EXT sind unkritische Digitalsignale und koennen ueber Mbc89DigitalIn/
        // Mbc89DigitalOut wahlweise nativ oder ueber einen Expander laufen (siehe Konstruktor)

        static Mbc89DigitalIn  *cuStopInPtr  = nullptr;
        static Mbc89DigitalOut *cuStopOutPtr = nullptr;
        static Mbc89DigitalIn  *cuGoInPtr    = nullptr;
        static Mbc89DigitalOut *cuGoOutPtr   = nullptr;
        static Mbc89DigitalIn  *cuExtPtr     = nullptr;

    #endif

    static const uint8_t CU_GO   = 0;
    static const uint8_t CU_STOP = 1;
    static const uint8_t CU_RESET = 2;
    static const uint8_t RESET_CU_STEP_1 = 3;
    static const uint8_t RESET_CU_STEP_2 = 4;

    // CU-Bus-I2C-Protokollkonstanten - 1:1 aus mbc_cu.c uebernommen (nur die tatsaechlich im
    // switch(status) benutzten TWI-Statuscodes, unbenutzte wie TWI_STX_*/TWI_MRX_*/TWI_SRX_GEN_*
    // wurden als toter Code nicht mitportiert)

    static const uint8_t TWI_RX_MAX   = 30;
    static const uint8_t TWI_RX_FRAME = 6;
    static const uint8_t TWI_TX_FRAME = 6;
    static const uint8_t TWI_MASTER   = 1;
    static const uint8_t TWI_SLAVE    = 0;

    static const uint8_t TWI_SEND_STARTSTOP        = 0;
    static const uint8_t TWI_SEND_KB               = 1;
    static const uint8_t TWI_SEND_STARTSTOPCONTROL = 2;
    static const uint8_t TWI_SEND_ALLESNULL        = 3;

    static const uint8_t SWITCH_ROT   = 0;
    static const uint8_t SWITCH_GRUEN = 1;

    #if defined(__AVR__)

        static const uint8_t TWI_START               = 0x08;
        static const uint8_t TWI_REP_START            = 0x10;
        static const uint8_t TWI_ARB_LOST             = 0x38;
        static const uint8_t TWI_MTX_ADR_ACK          = 0x18;
        static const uint8_t TWI_MTX_ADR_NACK         = 0x20;
        static const uint8_t TWI_MTX_DATA_ACK         = 0x28;
        static const uint8_t TWI_MTX_DATA_NACK        = 0x30;
        static const uint8_t TWI_SRX_ADR_ACK          = 0x60;
        static const uint8_t TWI_SRX_ADR_ACK_M_ARB_LOST = 0x68;
        static const uint8_t TWI_SRX_ADR_DATA_ACK     = 0x80;
        static const uint8_t TWI_SRX_ADR_DATA_NACK    = 0x88;
        static const uint8_t TWI_SRX_STOP_RESTART     = 0xA0;
        static const uint8_t TWI_BUS_ERROR            = 0x00;

        // Wiederkehrende TWCR-Bitmuster der TWI-ISR (nur zur Lesbarkeit/Zeilenlaenge benannt,
        // Bitwerte 1:1 aus mbc_cu.c uebernommen)

        #define TWCR_RESTART_MASTER \
            ((1<<TWEN)|(1<<TWIE)|(1<<TWINT)|(0<<TWEA)|(1<<TWSTA)|(0<<TWSTO)|(0<<TWWC))
        #define TWCR_RELEASE_SLAVE \
            ((1<<TWEN)|(1<<TWIE)|(1<<TWINT)|(1<<TWEA)|(0<<TWSTA)|(1<<TWSTO)|(0<<TWWC))
        #define TWCR_CONTINUE_NOACK \
            ((1<<TWEN)|(1<<TWIE)|(1<<TWINT)|(0<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|(0<<TWWC))
        #define TWCR_CONTINUE_ACK \
            ((1<<TWEN)|(1<<TWIE)|(1<<TWINT)|(1<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|(0<<TWWC))

    #else

        // ESP32-C3: keine TWI-Hardware mit dieser Adressierungslogik - CU-Bus laeuft ueber eine
        // selbstgeschriebene Software-I2C-Engine (siehe Mbc89I2cSoft-Abschnitt weiter unten). Diese
        // Zeitkonstanten sind grobe Richtwerte fuer 25...50 kHz (Original-I2C-Frequenzbereich, siehe
        // arr[MBC_89_S_I2C]) und werden von der Engine bei Bedarf aus arr[MBC_89_S_I2C] abgeleitet.

        static const uint32_t I2C_SOFT_MIN_HALF_PERIOD_US = 10UL;   // entspricht max. 50 kHz

    #endif

    // Gleisdecoder-Signalklassen (Trit-Automat) - Bezeichner 1:1 aus mbc_gleis.c uebernommen

    static const uint8_t TRI0PART1 = 1;
    static const uint8_t TRI0PART2 = 2;
    static const uint8_t TRI1PART1 = 3;
    static const uint8_t TRI1PART2 = 4;
    static const uint8_t TRIEND    = 5;
    static const uint8_t TRI0      = 1;
    static const uint8_t TRI1      = 2;
    static const uint8_t SIGNALPUFFER = 200;   // Original B_CMD

    // Gleisdecoder-Impulslaengenfenster (Timer1-Ticks, Prescaler /8, 500 ns/Tick bei echten 16
    // MHz) - FRISCH aus der MM2-Spezifikation abgeleitet (siehe Portierungsplan), NICHT die alten,
    // auf einen falschen 18,432-MHz-Takt ausgelegten Literale des Originals. Kreuzpruefung ueber
    // die im Original-Kopfkommentar genannten Paketlaengen (Magnetartikel 1,87 ms / Lok 3,74 ms
    // fuer je 18 uebertragene Trits): Magnetartikel-Trit=104 us (kurz 13 us=26 Ticks, lang 91
    // us=182 Ticks), Lok-Trit=208 us (kurz 26 us=52 Ticks, lang 182 us=364 Ticks) - Motorola-
    // Kurz/Lang-Verhaeltnis 1:7. Toleranzfenster grosszuegig um diese Nominalwerte gelegt
    // (Original-Stil), exakte Grenzen sind beim Hardwaretest mit echtem MM-Signal/Oszilloskop zu
    // verifizieren/nachzujustieren (siehe README).

    static const uint16_t MAGNET_KURZ_MIN = 15;    // ~7,5 us  (Nominal 13 us / 26 Ticks)
    static const uint16_t MAGNET_KURZ_MAX = 40;    // ~20 us
    static const uint16_t MAGNET_LANG_MIN = 145;   // ~72,5 us (Nominal 91 us / 182 Ticks)
    static const uint16_t MAGNET_LANG_MAX = 220;   // ~110 us
    static const uint16_t LOK_KURZ_MIN    = 30;    // ~15 us   (Nominal 26 us / 52 Ticks)
    static const uint16_t LOK_KURZ_MAX    = 80;    // ~40 us
    static const uint16_t LOK_LANG_MIN    = 290;   // ~145 us  (Nominal 182 us / 364 Ticks)
    static const uint16_t LOK_LANG_MAX    = 440;   // ~220 us

//------ Ende interne Konstanten ------------------------------------------------------------------

//------ Start Klasse Mbc89Com --------------------------------------------------------------------

    // CU-Bus und Gleisdecoder sind bewusst NICHT als Klassenmethoden umgesetzt, sondern als
    // freistehende, dateibegrenzte Funktionen mit eigenem statischem Zustand - genau wie im
    // Original (mbc_cu.c/mbc_gleis.c waren eigene C-Dateien mit globalen Variablen). Grund: die
    // beiden rohen Interrupt-Vektoren
    // ISR(TWI_vect)/ISR(TIMER2_COMPA_vect)/ISR(INT0_vect)/ISR(TIMER1_OVF_vect) muessen aus Linker-
    // Gruenden freie Funktionen sein (feste Vektortabellenadresse, keine Klassenmethode moeglich)
    // - sie haetten sonst keinen Zugriff auf private Klassenmembers. cuUtils/cuOwner sind die
    // einzigen Bruecken zurueck in die Mbc89ComBase-Basisklasse (Systemarray-Zugriff bzw.
    // sendCan()/sendZubehoerSchalten() mit Sende-Wiederholung) - siehe beginModule().

    static Mbc89Utils *cuUtils = nullptr;
    static Mbc89Com *cuOwner = nullptr;

    // CU-Bus-Zustand (Original: Struct cu_t)

    static bool    cuIsPresent   = false;
    static bool    cuIsInit      = false;
    static bool    cuIsConnected = false;

    static volatile bool    cuIsReset = false;   // von ISR(TIMER2_COMPA_vect) gesetzt
    static volatile bool    cuTasteOn = false;   // von ISR(TIMER2_COMPA_vect) gesetzt
    static volatile uint8_t cuGo      = 0;       // von ISR(TIMER2_COMPA_vect) gelesen

    // Gleisdecoder-/INFRA-Zustand (Original: Struct infra_t)

    static bool infraIsPresent = false;

    #if !defined(__AVR__)
        // ESP32-C3: One-Shot-Timer-Handle fuer die Tastensimulation (ersetzt
        // ISR(TIMER2_COMPA_vect), siehe cuTastenOnFn()/cuTastenReleaseCallback())
        static esp_timer_handle_t cuTastenTimer = nullptr;
    #endif

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //+ TWI-Kommunikationspuffer (volatile, werden aus ISR(TWI_vect) heraus beschrieben) - 1:1 aus+
    //+ mbc_cu.c uebernommen                                                                      +
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    static volatile uint8_t twiRxd[TWI_RX_MAX][TWI_RX_FRAME];
    static volatile uint8_t twiRxdInterrupt = 0;
    static volatile uint8_t twiRxdAktuell   = 0;
    static volatile uint8_t twiTxd[TWI_TX_FRAME];
    static volatile uint8_t twiMode = TWI_SLAVE;

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //+ CAN-Nachrichten fuer Lok-Speed/-Richtung/-Funktion (Original: can_msg_speed/_dir/_fx aus  +
    //+ mbc_cu.c) - HASH_H/L bleiben bewusst auf dem festen Vorlagenwert (0x03/0x00), werden NIE  +
    //+ aktualisiert (1:1 Originalverhalten - Lok-Steuernachrichten werden von der CS2 ueber die  +
    //+ GUiD in D0...D3 adressiert, nicht ueber den modulbezogenen HASH)                          +
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    static uint8_t msgLokSpeed[MBC_CAN_MSG_LEN] =
        {0x00,0x08,0x03,0x00,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    static uint8_t msgLokDir[MBC_CAN_MSG_LEN] =
        {0x00,0x0a,0x03,0x00,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    static uint8_t msgLokFx[MBC_CAN_MSG_LEN] =
        {0x00,0x0c,0x03,0x00,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //+ Gleisdecoder-Signalpuffer (volatile, werden aus ISR(INT0_vect) heraus beschrieben) - 1:1  +
    //+ aus mbc_gleis.c uebernommen                                                               +
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    static volatile uint8_t  cmdBLevel[SIGNALPUFFER + 1];
    static volatile uint16_t cmdBLength[SIGNALPUFFER + 1];
    static volatile uint8_t  cmdNr = 0;

    // PROGMEM: nur einmal im Konstruktor gelesen (siehe hashInitExtern() fuer die 4 modulspez.
    // Antwort-/Broadcast-Nachrichten). _msgStop/_msgGo (ID=0x00, spontaner Broadcast) vs.
    // _msgStopResponse/_msgGoResponse (ID=0x01, Antwort auf eine empfangene Anfrage) -
    // PRIO,CAN_ID,HASH_H,HASH_L,DLC,D0,D1,D2,D3,D4,D5,D6,D7

    static const uint8_t initMsgStop[MBC_COM_MSG_LAENGE] PROGMEM =
        {0x00,0x00,0x03,0x00,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

    static const uint8_t initMsgGo[MBC_COM_MSG_LAENGE] PROGMEM =
        {0x00,0x00,0x03,0x00,0x05,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00};

    static const uint8_t initMsgStopResponse[MBC_COM_MSG_LAENGE] PROGMEM =
        {0x00,0x01,0x03,0x00,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

    static const uint8_t initMsgGoResponse[MBC_COM_MSG_LAENGE] PROGMEM =
        {0x00,0x01,0x03,0x00,0x05,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00};

    // PROGMEM: bleibt dauerhaft im Flash, wird bei JEDEM cs2IndexExtern()-Aufruf frisch nach
    // _cs2ScratchRow kopiert (siehe dort) statt einmalig in eine dauerhafte RAM-Kopie - spart
    // RAM. CS2-Konfigkanaele "SW Major"/"SW Minor" der simulierten Connect6021-Softwareversion.
    // Byte-fuer-Byte identisch mit dem hardwarebestaetigten Mbc80Lib-Muster (siehe Projekt-Memory
    // project_mbcan_cs2_konfigkanal_pattern) uebernommen - NICHT von Hand neu auszaehlen, die
    // NUL-Terminierung von Label/Start/Ende/Einheit ist bereits verifiziert korrekt.
    //
    static const uint8_t initCs2Ex[MBC89_ANZMESSWERTEMODUL + MBC89_ANZKONFIGMODUL]
                                   [MBC_COM_CS2PAKETE * 8] PROGMEM =
    {
        // INDEX_CTRL 1.EX.1 - vorgetaeuschte Connect6021-SW-Version, Major-Byte (Slider, 1...255)

        {   KANAL_SW_MAJ,0x02,0x00,0x01,0x00,0xff,0x00,0x00,             // 1...8
            'S','W',' ','M','a','j','o','r',                             // 9...16
            ':',0x00,'1',0x00,'2','5','5',0x00,                          // 17...24
            '#',0x00,0x00,0x00,0x00,0x00,0x00,0x00,                      // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                      // 33...40
        },

        // INDEX_CTRL 1.EX.2 - vorgetaeuschte Connect6021-SW-Version, Minor-Byte (Slider, 1...255)

        {   KANAL_SW_MIN,0x02,0x00,0x01,0x00,0xff,0x00,0x00,             // 1...8
            'S','W',' ','M','i','n','o','r',                             // 9...16
            ':',0x00,'1',0x00,'2','5','5',0x00,                          // 17...24
            '#',0x00,0x00,0x00,0x00,0x00,0x00,0x00,                      // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                      // 33...40
        },

        // INDEX_CTRL 1.EX.3 - Zentralentyp-Mapping (Auswahlliste mit 2 Punkten: 0=Mapping,
        // 1=Mapping&Direkt), 1:1 aus der ParaCenter-CheckedListBox clb_mbc89_Mapping uebernommen

        {   KANAL_MAPPING,0x01,0x02,0x00,0x00,0x00,0x00,0x00,            // 1...8
            'M','a','p','p','i','n','g',':',                             // 9...16
            0x00,'M','a','p','p','i','n','g',                            // 17...24
            0x00,'M','a','p','p','i','n','g',                            // 25...32
            '&','D','i','r','e','k','t',0x00                             // 33...40
        },

        // INDEX_CTRL 1.EX.4 - Keyboard-Basisadressblock (Slider, 1...4), Grenzen 1:1 aus
        // nud_mbc89_basis (ParaCenter) uebernommen

        {   KANAL_BASIS,0x02,0x00,0x01,0x00,0x04,0x00,0x00,              // 1...8
            'B','a','s','i','s',':',0x00,'1',                            // 9...16
            0x00,'4',0x00,'#',0x00,0x00,0x00,0x00,                       // 17...24
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,                     // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                      // 33...40
        },

        // INDEX_CTRL 1.EX.5 - I2C-Bus-Takt (Slider, 40...100 kHz), Grenzen 1:1 aus nud_mbc89_i2c
        // (ParaCenter) uebernommen

        {   KANAL_I2C,0x02,0x00,0x28,0x00,0x64,0x00,0x00,                // 1...8
            'I','2','C',':',0x00,'4','0',0x00,                           // 9...16
            '1','0','0',0x00,'k','H','z',0x00,                           // 17...24
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,                     // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                      // 33...40
        },

        // 64 Konfigkanaele "KeyBx GroupY:" (x=1...16 Keyboard, y=1...4 Gruppe/Stellpultblock),
        // je Auswahlliste MM/DCC - 1:1 aus dem ParaCenter-Array Protokoll(keyboard*4+gruppe)
        // uebernommen (siehe mbc_modul_89.vb). Optionsreihenfolge aus clb_mbc89_A/B/C/D:
        // Index 0="MM", Index 1="DCC" (passt zum Firmware-Bit in cuI2cCheckFn(), 0=0x3000/MM,
        // 1=0x3800/DCC). Byte-Inhalt geskriptet erzeugt und gegengeprueft, nicht von Hand
        // ausgezaehlt (Tippfehlerrisiko bei 64 Zeilen).

        // INDEX_CTRL 1.EX.6 - KeyB1 Group1: (Auswahlliste: 0=MM, 1=DCC)

        {   KANAL_PROTOKOLL_BASE+0,0x01,0x02,0x00,0x00,0x00,0x00,0x00,                     // 1...8
            'K','e','y','B','1',' ','G','r',                             // 9...16
            'o','u','p','1',':',0x00,'M','M',                             // 17...24
            0x00,'D','C','C',0x00,0x00,0x00,0x00,                             // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                              // 33...40
        },

        // INDEX_CTRL 1.EX.7 - KeyB1 Group2: (Auswahlliste: 0=MM, 1=DCC)

        {   KANAL_PROTOKOLL_BASE+1,0x01,0x02,0x00,0x00,0x00,0x00,0x00,                     // 1...8
            'K','e','y','B','1',' ','G','r',                             // 9...16
            'o','u','p','2',':',0x00,'M','M',                             // 17...24
            0x00,'D','C','C',0x00,0x00,0x00,0x00,                             // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                              // 33...40
        },

        // INDEX_CTRL 1.EX.8 - KeyB1 Group3: (Auswahlliste: 0=MM, 1=DCC)

        {   KANAL_PROTOKOLL_BASE+2,0x01,0x02,0x00,0x00,0x00,0x00,0x00,                     // 1...8
            'K','e','y','B','1',' ','G','r',                             // 9...16
            'o','u','p','3',':',0x00,'M','M',                             // 17...24
            0x00,'D','C','C',0x00,0x00,0x00,0x00,                             // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                              // 33...40
        },

        // INDEX_CTRL 1.EX.9 - KeyB1 Group4: (Auswahlliste: 0=MM, 1=DCC)

        {   KANAL_PROTOKOLL_BASE+3,0x01,0x02,0x00,0x00,0x00,0x00,0x00,                     // 1...8
            'K','e','y','B','1',' ','G','r',                             // 9...16
            'o','u','p','4',':',0x00,'M','M',                             // 17...24
            0x00,'D','C','C',0x00,0x00,0x00,0x00,                             // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                              // 33...40
        },

        // INDEX_CTRL 1.EX.10 - KeyB2 Group1: (Auswahlliste: 0=MM, 1=DCC)

        {   KANAL_PROTOKOLL_BASE+4,0x01,0x02,0x00,0x00,0x00,0x00,0x00,                     // 1...8
            'K','e','y','B','2',' ','G','r',                             // 9...16
            'o','u','p','1',':',0x00,'M','M',                             // 17...24
            0x00,'D','C','C',0x00,0x00,0x00,0x00,                             // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                              // 33...40
        },

        // INDEX_CTRL 1.EX.11 - KeyB2 Group2: (Auswahlliste: 0=MM, 1=DCC)

        {   KANAL_PROTOKOLL_BASE+5,0x01,0x02,0x00,0x00,0x00,0x00,0x00,                     // 1...8
            'K','e','y','B','2',' ','G','r',                             // 9...16
            'o','u','p','2',':',0x00,'M','M',                             // 17...24
            0x00,'D','C','C',0x00,0x00,0x00,0x00,                             // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                              // 33...40
        },

        // INDEX_CTRL 1.EX.12 - KeyB2 Group3: (Auswahlliste: 0=MM, 1=DCC)

        {   KANAL_PROTOKOLL_BASE+6,0x01,0x02,0x00,0x00,0x00,0x00,0x00,                     // 1...8
            'K','e','y','B','2',' ','G','r',                             // 9...16
            'o','u','p','3',':',0x00,'M','M',                             // 17...24
            0x00,'D','C','C',0x00,0x00,0x00,0x00,                             // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                              // 33...40
        },

        // INDEX_CTRL 1.EX.13 - KeyB2 Group4: (Auswahlliste: 0=MM, 1=DCC)

        {   KANAL_PROTOKOLL_BASE+7,0x01,0x02,0x00,0x00,0x00,0x00,0x00,                     // 1...8
            'K','e','y','B','2',' ','G','r',                             // 9...16
            'o','u','p','4',':',0x00,'M','M',                             // 17...24
            0x00,'D','C','C',0x00,0x00,0x00,0x00,                             // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                              // 33...40
        },

        // INDEX_CTRL 1.EX.14 - KeyB3 Group1: (Auswahlliste: 0=MM, 1=DCC)

        {   KANAL_PROTOKOLL_BASE+8,0x01,0x02,0x00,0x00,0x00,0x00,0x00,                     // 1...8
            'K','e','y','B','3',' ','G','r',                             // 9...16
            'o','u','p','1',':',0x00,'M','M',                             // 17...24
            0x00,'D','C','C',0x00,0x00,0x00,0x00,                             // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                              // 33...40
        },

        // INDEX_CTRL 1.EX.15 - KeyB3 Group2: (Auswahlliste: 0=MM, 1=DCC)

        {   KANAL_PROTOKOLL_BASE+9,0x01,0x02,0x00,0x00,0x00,0x00,0x00,                     // 1...8
            'K','e','y','B','3',' ','G','r',                             // 9...16
            'o','u','p','2',':',0x00,'M','M',                             // 17...24
            0x00,'D','C','C',0x00,0x00,0x00,0x00,                             // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                              // 33...40
        },

        // INDEX_CTRL 1.EX.16 - KeyB3 Group3: (Auswahlliste: 0=MM, 1=DCC)

        {   KANAL_PROTOKOLL_BASE+10,0x01,0x02,0x00,0x00,0x00,0x00,0x00,                     // 1...8
            'K','e','y','B','3',' ','G','r',                             // 9...16
            'o','u','p','3',':',0x00,'M','M',                             // 17...24
            0x00,'D','C','C',0x00,0x00,0x00,0x00,                             // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                              // 33...40
        },

        // INDEX_CTRL 1.EX.17 - KeyB3 Group4: (Auswahlliste: 0=MM, 1=DCC)

        {   KANAL_PROTOKOLL_BASE+11,0x01,0x02,0x00,0x00,0x00,0x00,0x00,                     // 1...8
            'K','e','y','B','3',' ','G','r',                             // 9...16
            'o','u','p','4',':',0x00,'M','M',                             // 17...24
            0x00,'D','C','C',0x00,0x00,0x00,0x00,                             // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                              // 33...40
        },

        // INDEX_CTRL 1.EX.18 - KeyB4 Group1: (Auswahlliste: 0=MM, 1=DCC)

        {   KANAL_PROTOKOLL_BASE+12,0x01,0x02,0x00,0x00,0x00,0x00,0x00,                     // 1...8
            'K','e','y','B','4',' ','G','r',                             // 9...16
            'o','u','p','1',':',0x00,'M','M',                             // 17...24
            0x00,'D','C','C',0x00,0x00,0x00,0x00,                             // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                              // 33...40
        },

        // INDEX_CTRL 1.EX.19 - KeyB4 Group2: (Auswahlliste: 0=MM, 1=DCC)

        {   KANAL_PROTOKOLL_BASE+13,0x01,0x02,0x00,0x00,0x00,0x00,0x00,                     // 1...8
            'K','e','y','B','4',' ','G','r',                             // 9...16
            'o','u','p','2',':',0x00,'M','M',                             // 17...24
            0x00,'D','C','C',0x00,0x00,0x00,0x00,                             // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                              // 33...40
        },

        // INDEX_CTRL 1.EX.20 - KeyB4 Group3: (Auswahlliste: 0=MM, 1=DCC)

        {   KANAL_PROTOKOLL_BASE+14,0x01,0x02,0x00,0x00,0x00,0x00,0x00,                     // 1...8
            'K','e','y','B','4',' ','G','r',                             // 9...16
            'o','u','p','3',':',0x00,'M','M',                             // 17...24
            0x00,'D','C','C',0x00,0x00,0x00,0x00,                             // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                              // 33...40
        },

        // INDEX_CTRL 1.EX.21 - KeyB4 Group4: (Auswahlliste: 0=MM, 1=DCC)

        {   KANAL_PROTOKOLL_BASE+15,0x01,0x02,0x00,0x00,0x00,0x00,0x00,                     // 1...8
            'K','e','y','B','4',' ','G','r',                             // 9...16
            'o','u','p','4',':',0x00,'M','M',                             // 17...24
            0x00,'D','C','C',0x00,0x00,0x00,0x00,                             // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                              // 33...40
        },

        // INDEX_CTRL 1.EX.22 - KeyB5 Group1: (Auswahlliste: 0=MM, 1=DCC)

        {   KANAL_PROTOKOLL_BASE+16,0x01,0x02,0x00,0x00,0x00,0x00,0x00,                     // 1...8
            'K','e','y','B','5',' ','G','r',                             // 9...16
            'o','u','p','1',':',0x00,'M','M',                             // 17...24
            0x00,'D','C','C',0x00,0x00,0x00,0x00,                             // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                              // 33...40
        },

        // INDEX_CTRL 1.EX.23 - KeyB5 Group2: (Auswahlliste: 0=MM, 1=DCC)

        {   KANAL_PROTOKOLL_BASE+17,0x01,0x02,0x00,0x00,0x00,0x00,0x00,                     // 1...8
            'K','e','y','B','5',' ','G','r',                             // 9...16
            'o','u','p','2',':',0x00,'M','M',                             // 17...24
            0x00,'D','C','C',0x00,0x00,0x00,0x00,                             // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                              // 33...40
        },

        // INDEX_CTRL 1.EX.24 - KeyB5 Group3: (Auswahlliste: 0=MM, 1=DCC)

        {   KANAL_PROTOKOLL_BASE+18,0x01,0x02,0x00,0x00,0x00,0x00,0x00,                     // 1...8
            'K','e','y','B','5',' ','G','r',                             // 9...16
            'o','u','p','3',':',0x00,'M','M',                             // 17...24
            0x00,'D','C','C',0x00,0x00,0x00,0x00,                             // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                              // 33...40
        },

        // INDEX_CTRL 1.EX.25 - KeyB5 Group4: (Auswahlliste: 0=MM, 1=DCC)

        {   KANAL_PROTOKOLL_BASE+19,0x01,0x02,0x00,0x00,0x00,0x00,0x00,                     // 1...8
            'K','e','y','B','5',' ','G','r',                             // 9...16
            'o','u','p','4',':',0x00,'M','M',                             // 17...24
            0x00,'D','C','C',0x00,0x00,0x00,0x00,                             // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                              // 33...40
        },

        // INDEX_CTRL 1.EX.26 - KeyB6 Group1: (Auswahlliste: 0=MM, 1=DCC)

        {   KANAL_PROTOKOLL_BASE+20,0x01,0x02,0x00,0x00,0x00,0x00,0x00,                     // 1...8
            'K','e','y','B','6',' ','G','r',                             // 9...16
            'o','u','p','1',':',0x00,'M','M',                             // 17...24
            0x00,'D','C','C',0x00,0x00,0x00,0x00,                             // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                              // 33...40
        },

        // INDEX_CTRL 1.EX.27 - KeyB6 Group2: (Auswahlliste: 0=MM, 1=DCC)

        {   KANAL_PROTOKOLL_BASE+21,0x01,0x02,0x00,0x00,0x00,0x00,0x00,                     // 1...8
            'K','e','y','B','6',' ','G','r',                             // 9...16
            'o','u','p','2',':',0x00,'M','M',                             // 17...24
            0x00,'D','C','C',0x00,0x00,0x00,0x00,                             // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                              // 33...40
        },

        // INDEX_CTRL 1.EX.28 - KeyB6 Group3: (Auswahlliste: 0=MM, 1=DCC)

        {   KANAL_PROTOKOLL_BASE+22,0x01,0x02,0x00,0x00,0x00,0x00,0x00,                     // 1...8
            'K','e','y','B','6',' ','G','r',                             // 9...16
            'o','u','p','3',':',0x00,'M','M',                             // 17...24
            0x00,'D','C','C',0x00,0x00,0x00,0x00,                             // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                              // 33...40
        },

        // INDEX_CTRL 1.EX.29 - KeyB6 Group4: (Auswahlliste: 0=MM, 1=DCC)

        {   KANAL_PROTOKOLL_BASE+23,0x01,0x02,0x00,0x00,0x00,0x00,0x00,                     // 1...8
            'K','e','y','B','6',' ','G','r',                             // 9...16
            'o','u','p','4',':',0x00,'M','M',                             // 17...24
            0x00,'D','C','C',0x00,0x00,0x00,0x00,                             // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                              // 33...40
        },

        // INDEX_CTRL 1.EX.30 - KeyB7 Group1: (Auswahlliste: 0=MM, 1=DCC)

        {   KANAL_PROTOKOLL_BASE+24,0x01,0x02,0x00,0x00,0x00,0x00,0x00,                     // 1...8
            'K','e','y','B','7',' ','G','r',                             // 9...16
            'o','u','p','1',':',0x00,'M','M',                             // 17...24
            0x00,'D','C','C',0x00,0x00,0x00,0x00,                             // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                              // 33...40
        },

        // INDEX_CTRL 1.EX.31 - KeyB7 Group2: (Auswahlliste: 0=MM, 1=DCC)

        {   KANAL_PROTOKOLL_BASE+25,0x01,0x02,0x00,0x00,0x00,0x00,0x00,                     // 1...8
            'K','e','y','B','7',' ','G','r',                             // 9...16
            'o','u','p','2',':',0x00,'M','M',                             // 17...24
            0x00,'D','C','C',0x00,0x00,0x00,0x00,                             // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                              // 33...40
        },

        // INDEX_CTRL 1.EX.32 - KeyB7 Group3: (Auswahlliste: 0=MM, 1=DCC)

        {   KANAL_PROTOKOLL_BASE+26,0x01,0x02,0x00,0x00,0x00,0x00,0x00,                     // 1...8
            'K','e','y','B','7',' ','G','r',                             // 9...16
            'o','u','p','3',':',0x00,'M','M',                             // 17...24
            0x00,'D','C','C',0x00,0x00,0x00,0x00,                             // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                              // 33...40
        },

        // INDEX_CTRL 1.EX.33 - KeyB7 Group4: (Auswahlliste: 0=MM, 1=DCC)

        {   KANAL_PROTOKOLL_BASE+27,0x01,0x02,0x00,0x00,0x00,0x00,0x00,                     // 1...8
            'K','e','y','B','7',' ','G','r',                             // 9...16
            'o','u','p','4',':',0x00,'M','M',                             // 17...24
            0x00,'D','C','C',0x00,0x00,0x00,0x00,                             // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                              // 33...40
        },

        // INDEX_CTRL 1.EX.34 - KeyB8 Group1: (Auswahlliste: 0=MM, 1=DCC)

        {   KANAL_PROTOKOLL_BASE+28,0x01,0x02,0x00,0x00,0x00,0x00,0x00,                     // 1...8
            'K','e','y','B','8',' ','G','r',                             // 9...16
            'o','u','p','1',':',0x00,'M','M',                             // 17...24
            0x00,'D','C','C',0x00,0x00,0x00,0x00,                             // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                              // 33...40
        },

        // INDEX_CTRL 1.EX.35 - KeyB8 Group2: (Auswahlliste: 0=MM, 1=DCC)

        {   KANAL_PROTOKOLL_BASE+29,0x01,0x02,0x00,0x00,0x00,0x00,0x00,                     // 1...8
            'K','e','y','B','8',' ','G','r',                             // 9...16
            'o','u','p','2',':',0x00,'M','M',                             // 17...24
            0x00,'D','C','C',0x00,0x00,0x00,0x00,                             // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                              // 33...40
        },

        // INDEX_CTRL 1.EX.36 - KeyB8 Group3: (Auswahlliste: 0=MM, 1=DCC)

        {   KANAL_PROTOKOLL_BASE+30,0x01,0x02,0x00,0x00,0x00,0x00,0x00,                     // 1...8
            'K','e','y','B','8',' ','G','r',                             // 9...16
            'o','u','p','3',':',0x00,'M','M',                             // 17...24
            0x00,'D','C','C',0x00,0x00,0x00,0x00,                             // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                              // 33...40
        },

        // INDEX_CTRL 1.EX.37 - KeyB8 Group4: (Auswahlliste: 0=MM, 1=DCC)

        {   KANAL_PROTOKOLL_BASE+31,0x01,0x02,0x00,0x00,0x00,0x00,0x00,                     // 1...8
            'K','e','y','B','8',' ','G','r',                             // 9...16
            'o','u','p','4',':',0x00,'M','M',                             // 17...24
            0x00,'D','C','C',0x00,0x00,0x00,0x00,                             // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                              // 33...40
        },

        // INDEX_CTRL 1.EX.38 - KeyB9 Group1: (Auswahlliste: 0=MM, 1=DCC)

        {   KANAL_PROTOKOLL_BASE+32,0x01,0x02,0x00,0x00,0x00,0x00,0x00,                     // 1...8
            'K','e','y','B','9',' ','G','r',                             // 9...16
            'o','u','p','1',':',0x00,'M','M',                             // 17...24
            0x00,'D','C','C',0x00,0x00,0x00,0x00,                             // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                              // 33...40
        },

        // INDEX_CTRL 1.EX.39 - KeyB9 Group2: (Auswahlliste: 0=MM, 1=DCC)

        {   KANAL_PROTOKOLL_BASE+33,0x01,0x02,0x00,0x00,0x00,0x00,0x00,                     // 1...8
            'K','e','y','B','9',' ','G','r',                             // 9...16
            'o','u','p','2',':',0x00,'M','M',                             // 17...24
            0x00,'D','C','C',0x00,0x00,0x00,0x00,                             // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                              // 33...40
        },

        // INDEX_CTRL 1.EX.40 - KeyB9 Group3: (Auswahlliste: 0=MM, 1=DCC)

        {   KANAL_PROTOKOLL_BASE+34,0x01,0x02,0x00,0x00,0x00,0x00,0x00,                     // 1...8
            'K','e','y','B','9',' ','G','r',                             // 9...16
            'o','u','p','3',':',0x00,'M','M',                             // 17...24
            0x00,'D','C','C',0x00,0x00,0x00,0x00,                             // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                              // 33...40
        },

        // INDEX_CTRL 1.EX.41 - KeyB9 Group4: (Auswahlliste: 0=MM, 1=DCC)

        {   KANAL_PROTOKOLL_BASE+35,0x01,0x02,0x00,0x00,0x00,0x00,0x00,                     // 1...8
            'K','e','y','B','9',' ','G','r',                             // 9...16
            'o','u','p','4',':',0x00,'M','M',                             // 17...24
            0x00,'D','C','C',0x00,0x00,0x00,0x00,                             // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                              // 33...40
        },

        // INDEX_CTRL 1.EX.42 - KeyB10 Group1: (Auswahlliste: 0=MM, 1=DCC)

        {   KANAL_PROTOKOLL_BASE+36,0x01,0x02,0x00,0x00,0x00,0x00,0x00,                     // 1...8
            'K','e','y','B','1','0',' ','G',                             // 9...16
            'r','o','u','p','1',':',0x00,'M',                             // 17...24
            'M',0x00,'D','C','C',0x00,0x00,0x00,                             // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                              // 33...40
        },

        // INDEX_CTRL 1.EX.43 - KeyB10 Group2: (Auswahlliste: 0=MM, 1=DCC)

        {   KANAL_PROTOKOLL_BASE+37,0x01,0x02,0x00,0x00,0x00,0x00,0x00,                     // 1...8
            'K','e','y','B','1','0',' ','G',                             // 9...16
            'r','o','u','p','2',':',0x00,'M',                             // 17...24
            'M',0x00,'D','C','C',0x00,0x00,0x00,                             // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                              // 33...40
        },

        // INDEX_CTRL 1.EX.44 - KeyB10 Group3: (Auswahlliste: 0=MM, 1=DCC)

        {   KANAL_PROTOKOLL_BASE+38,0x01,0x02,0x00,0x00,0x00,0x00,0x00,                     // 1...8
            'K','e','y','B','1','0',' ','G',                             // 9...16
            'r','o','u','p','3',':',0x00,'M',                             // 17...24
            'M',0x00,'D','C','C',0x00,0x00,0x00,                             // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                              // 33...40
        },

        // INDEX_CTRL 1.EX.45 - KeyB10 Group4: (Auswahlliste: 0=MM, 1=DCC)

        {   KANAL_PROTOKOLL_BASE+39,0x01,0x02,0x00,0x00,0x00,0x00,0x00,                     // 1...8
            'K','e','y','B','1','0',' ','G',                             // 9...16
            'r','o','u','p','4',':',0x00,'M',                             // 17...24
            'M',0x00,'D','C','C',0x00,0x00,0x00,                             // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                              // 33...40
        },

        // INDEX_CTRL 1.EX.46 - KeyB11 Group1: (Auswahlliste: 0=MM, 1=DCC)

        {   KANAL_PROTOKOLL_BASE+40,0x01,0x02,0x00,0x00,0x00,0x00,0x00,                     // 1...8
            'K','e','y','B','1','1',' ','G',                             // 9...16
            'r','o','u','p','1',':',0x00,'M',                             // 17...24
            'M',0x00,'D','C','C',0x00,0x00,0x00,                             // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                              // 33...40
        },

        // INDEX_CTRL 1.EX.47 - KeyB11 Group2: (Auswahlliste: 0=MM, 1=DCC)

        {   KANAL_PROTOKOLL_BASE+41,0x01,0x02,0x00,0x00,0x00,0x00,0x00,                     // 1...8
            'K','e','y','B','1','1',' ','G',                             // 9...16
            'r','o','u','p','2',':',0x00,'M',                             // 17...24
            'M',0x00,'D','C','C',0x00,0x00,0x00,                             // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                              // 33...40
        },

        // INDEX_CTRL 1.EX.48 - KeyB11 Group3: (Auswahlliste: 0=MM, 1=DCC)

        {   KANAL_PROTOKOLL_BASE+42,0x01,0x02,0x00,0x00,0x00,0x00,0x00,                     // 1...8
            'K','e','y','B','1','1',' ','G',                             // 9...16
            'r','o','u','p','3',':',0x00,'M',                             // 17...24
            'M',0x00,'D','C','C',0x00,0x00,0x00,                             // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                              // 33...40
        },

        // INDEX_CTRL 1.EX.49 - KeyB11 Group4: (Auswahlliste: 0=MM, 1=DCC)

        {   KANAL_PROTOKOLL_BASE+43,0x01,0x02,0x00,0x00,0x00,0x00,0x00,                     // 1...8
            'K','e','y','B','1','1',' ','G',                             // 9...16
            'r','o','u','p','4',':',0x00,'M',                             // 17...24
            'M',0x00,'D','C','C',0x00,0x00,0x00,                             // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                              // 33...40
        },

        // INDEX_CTRL 1.EX.50 - KeyB12 Group1: (Auswahlliste: 0=MM, 1=DCC)

        {   KANAL_PROTOKOLL_BASE+44,0x01,0x02,0x00,0x00,0x00,0x00,0x00,                     // 1...8
            'K','e','y','B','1','2',' ','G',                             // 9...16
            'r','o','u','p','1',':',0x00,'M',                             // 17...24
            'M',0x00,'D','C','C',0x00,0x00,0x00,                             // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                              // 33...40
        },

        // INDEX_CTRL 1.EX.51 - KeyB12 Group2: (Auswahlliste: 0=MM, 1=DCC)

        {   KANAL_PROTOKOLL_BASE+45,0x01,0x02,0x00,0x00,0x00,0x00,0x00,                     // 1...8
            'K','e','y','B','1','2',' ','G',                             // 9...16
            'r','o','u','p','2',':',0x00,'M',                             // 17...24
            'M',0x00,'D','C','C',0x00,0x00,0x00,                             // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                              // 33...40
        },

        // INDEX_CTRL 1.EX.52 - KeyB12 Group3: (Auswahlliste: 0=MM, 1=DCC)

        {   KANAL_PROTOKOLL_BASE+46,0x01,0x02,0x00,0x00,0x00,0x00,0x00,                     // 1...8
            'K','e','y','B','1','2',' ','G',                             // 9...16
            'r','o','u','p','3',':',0x00,'M',                             // 17...24
            'M',0x00,'D','C','C',0x00,0x00,0x00,                             // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                              // 33...40
        },

        // INDEX_CTRL 1.EX.53 - KeyB12 Group4: (Auswahlliste: 0=MM, 1=DCC)

        {   KANAL_PROTOKOLL_BASE+47,0x01,0x02,0x00,0x00,0x00,0x00,0x00,                     // 1...8
            'K','e','y','B','1','2',' ','G',                             // 9...16
            'r','o','u','p','4',':',0x00,'M',                             // 17...24
            'M',0x00,'D','C','C',0x00,0x00,0x00,                             // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                              // 33...40
        },

        // INDEX_CTRL 1.EX.54 - KeyB13 Group1: (Auswahlliste: 0=MM, 1=DCC)

        {   KANAL_PROTOKOLL_BASE+48,0x01,0x02,0x00,0x00,0x00,0x00,0x00,                     // 1...8
            'K','e','y','B','1','3',' ','G',                             // 9...16
            'r','o','u','p','1',':',0x00,'M',                             // 17...24
            'M',0x00,'D','C','C',0x00,0x00,0x00,                             // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                              // 33...40
        },

        // INDEX_CTRL 1.EX.55 - KeyB13 Group2: (Auswahlliste: 0=MM, 1=DCC)

        {   KANAL_PROTOKOLL_BASE+49,0x01,0x02,0x00,0x00,0x00,0x00,0x00,                     // 1...8
            'K','e','y','B','1','3',' ','G',                             // 9...16
            'r','o','u','p','2',':',0x00,'M',                             // 17...24
            'M',0x00,'D','C','C',0x00,0x00,0x00,                             // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                              // 33...40
        },

        // INDEX_CTRL 1.EX.56 - KeyB13 Group3: (Auswahlliste: 0=MM, 1=DCC)

        {   KANAL_PROTOKOLL_BASE+50,0x01,0x02,0x00,0x00,0x00,0x00,0x00,                     // 1...8
            'K','e','y','B','1','3',' ','G',                             // 9...16
            'r','o','u','p','3',':',0x00,'M',                             // 17...24
            'M',0x00,'D','C','C',0x00,0x00,0x00,                             // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                              // 33...40
        },

        // INDEX_CTRL 1.EX.57 - KeyB13 Group4: (Auswahlliste: 0=MM, 1=DCC)

        {   KANAL_PROTOKOLL_BASE+51,0x01,0x02,0x00,0x00,0x00,0x00,0x00,                     // 1...8
            'K','e','y','B','1','3',' ','G',                             // 9...16
            'r','o','u','p','4',':',0x00,'M',                             // 17...24
            'M',0x00,'D','C','C',0x00,0x00,0x00,                             // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                              // 33...40
        },

        // INDEX_CTRL 1.EX.58 - KeyB14 Group1: (Auswahlliste: 0=MM, 1=DCC)

        {   KANAL_PROTOKOLL_BASE+52,0x01,0x02,0x00,0x00,0x00,0x00,0x00,                     // 1...8
            'K','e','y','B','1','4',' ','G',                             // 9...16
            'r','o','u','p','1',':',0x00,'M',                             // 17...24
            'M',0x00,'D','C','C',0x00,0x00,0x00,                             // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                              // 33...40
        },

        // INDEX_CTRL 1.EX.59 - KeyB14 Group2: (Auswahlliste: 0=MM, 1=DCC)

        {   KANAL_PROTOKOLL_BASE+53,0x01,0x02,0x00,0x00,0x00,0x00,0x00,                     // 1...8
            'K','e','y','B','1','4',' ','G',                             // 9...16
            'r','o','u','p','2',':',0x00,'M',                             // 17...24
            'M',0x00,'D','C','C',0x00,0x00,0x00,                             // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                              // 33...40
        },

        // INDEX_CTRL 1.EX.60 - KeyB14 Group3: (Auswahlliste: 0=MM, 1=DCC)

        {   KANAL_PROTOKOLL_BASE+54,0x01,0x02,0x00,0x00,0x00,0x00,0x00,                     // 1...8
            'K','e','y','B','1','4',' ','G',                             // 9...16
            'r','o','u','p','3',':',0x00,'M',                             // 17...24
            'M',0x00,'D','C','C',0x00,0x00,0x00,                             // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                              // 33...40
        },

        // INDEX_CTRL 1.EX.61 - KeyB14 Group4: (Auswahlliste: 0=MM, 1=DCC)

        {   KANAL_PROTOKOLL_BASE+55,0x01,0x02,0x00,0x00,0x00,0x00,0x00,                     // 1...8
            'K','e','y','B','1','4',' ','G',                             // 9...16
            'r','o','u','p','4',':',0x00,'M',                             // 17...24
            'M',0x00,'D','C','C',0x00,0x00,0x00,                             // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                              // 33...40
        },

        // INDEX_CTRL 1.EX.62 - KeyB15 Group1: (Auswahlliste: 0=MM, 1=DCC)

        {   KANAL_PROTOKOLL_BASE+56,0x01,0x02,0x00,0x00,0x00,0x00,0x00,                     // 1...8
            'K','e','y','B','1','5',' ','G',                             // 9...16
            'r','o','u','p','1',':',0x00,'M',                             // 17...24
            'M',0x00,'D','C','C',0x00,0x00,0x00,                             // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                              // 33...40
        },

        // INDEX_CTRL 1.EX.63 - KeyB15 Group2: (Auswahlliste: 0=MM, 1=DCC)

        {   KANAL_PROTOKOLL_BASE+57,0x01,0x02,0x00,0x00,0x00,0x00,0x00,                     // 1...8
            'K','e','y','B','1','5',' ','G',                             // 9...16
            'r','o','u','p','2',':',0x00,'M',                             // 17...24
            'M',0x00,'D','C','C',0x00,0x00,0x00,                             // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                              // 33...40
        },

        // INDEX_CTRL 1.EX.64 - KeyB15 Group3: (Auswahlliste: 0=MM, 1=DCC)

        {   KANAL_PROTOKOLL_BASE+58,0x01,0x02,0x00,0x00,0x00,0x00,0x00,                     // 1...8
            'K','e','y','B','1','5',' ','G',                             // 9...16
            'r','o','u','p','3',':',0x00,'M',                             // 17...24
            'M',0x00,'D','C','C',0x00,0x00,0x00,                             // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                              // 33...40
        },

        // INDEX_CTRL 1.EX.65 - KeyB15 Group4: (Auswahlliste: 0=MM, 1=DCC)

        {   KANAL_PROTOKOLL_BASE+59,0x01,0x02,0x00,0x00,0x00,0x00,0x00,                     // 1...8
            'K','e','y','B','1','5',' ','G',                             // 9...16
            'r','o','u','p','4',':',0x00,'M',                             // 17...24
            'M',0x00,'D','C','C',0x00,0x00,0x00,                             // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                              // 33...40
        },

        // INDEX_CTRL 1.EX.66 - KeyB16 Group1: (Auswahlliste: 0=MM, 1=DCC)

        {   KANAL_PROTOKOLL_BASE+60,0x01,0x02,0x00,0x00,0x00,0x00,0x00,                     // 1...8
            'K','e','y','B','1','6',' ','G',                             // 9...16
            'r','o','u','p','1',':',0x00,'M',                             // 17...24
            'M',0x00,'D','C','C',0x00,0x00,0x00,                             // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                              // 33...40
        },

        // INDEX_CTRL 1.EX.67 - KeyB16 Group2: (Auswahlliste: 0=MM, 1=DCC)

        {   KANAL_PROTOKOLL_BASE+61,0x01,0x02,0x00,0x00,0x00,0x00,0x00,                     // 1...8
            'K','e','y','B','1','6',' ','G',                             // 9...16
            'r','o','u','p','2',':',0x00,'M',                             // 17...24
            'M',0x00,'D','C','C',0x00,0x00,0x00,                             // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                              // 33...40
        },

        // INDEX_CTRL 1.EX.68 - KeyB16 Group3: (Auswahlliste: 0=MM, 1=DCC)

        {   KANAL_PROTOKOLL_BASE+62,0x01,0x02,0x00,0x00,0x00,0x00,0x00,                     // 1...8
            'K','e','y','B','1','6',' ','G',                             // 9...16
            'r','o','u','p','3',':',0x00,'M',                             // 17...24
            'M',0x00,'D','C','C',0x00,0x00,0x00,                             // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                              // 33...40
        },

        // INDEX_CTRL 1.EX.69 - KeyB16 Group4: (Auswahlliste: 0=MM, 1=DCC)

        {   KANAL_PROTOKOLL_BASE+63,0x01,0x02,0x00,0x00,0x00,0x00,0x00,                     // 1...8
            'K','e','y','B','1','6',' ','G',                             // 9...16
            'r','o','u','p','4',':',0x00,'M',                             // 17...24
            'M',0x00,'D','C','C',0x00,0x00,0x00,                             // 25...32
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                              // 33...40
        }
    };

    static const uint8_t initMsgKonfigOk[MBC_COM_MSG_LAENGE] PROGMEM =
        {0x00,0x01,0x00,0x00,0x07,0x00,0x00,0x00,0x00,0x0b,0x00,0x01,0x00};

    // Vorwaertsdeklaration der freistehenden CU-Bus-/Gleisdecoder-Funktionen (Definition weiter
    // unten in dieser Datei) - werden aus den Klassenmethoden beginModule()/update() heraus
    // aufgerufen

    static void    cuInitFn();
    static uint8_t cuTastenOnFn(uint8_t taste);
    static void    cuTastenCheckFn();
    static void    i2cSendCuFn(uint8_t index, uint8_t addr, uint8_t funktion);
    static uint8_t cuI2cCheckFn();
    static uint8_t cuSendKbFn(const uint8_t msg[MBC_CAN_MSG_LEN]);
    static void    cuResetFn();
    static void    cuPresentCheckFn();

    static void    trackInitFn();
    static void    trackCheckFn();
    static uint8_t automatMagnetFn(uint8_t cmdLevel, uint8_t cmdValue, uint32_t *cmdBefehl);
    static uint8_t automatLokFn(uint8_t cmdLevel, uint8_t cmdValue, uint32_t *cmdBefehl);
    static void    trackCheckMagnetFn(uint32_t trackCmd);
    static void    trackCheckLokFn(uint32_t trackCmd);
    static uint8_t findLokByAlias(uint8_t gleisAddr);

    #if !defined(__AVR__)
        // ESP32-C3-spezifische Vorwaertsdeklarationen (Software-I2C-Engine/RMT-Gleisdecoder/
        // Tastensimulations-Timer) - siehe jeweilige Abschnitte weiter unten
        static void cuTastenReleaseCallback(void *arg);
        static void i2cSoftInit(uint8_t pinScl, uint8_t pinSda, uint8_t i2cKhz);
        static void i2cSoftSetOwnAddr(uint8_t addr);
        static void i2cSoftMasterSend();
        static void trackRmtDrain();
    #endif

    //*********************************************************************************************
    //* Konstruktor zur Deklaration von Objektvariablen                                           *
    //*********************************************************************************************
    //*********************************************************************************************
    //* utils:       zugehoerige Mbc89Utils-Instanz                                               *
    //* can:         zugehoeriger CAN-Kanal dieses Moduls (Basisklassen-Parameter can)            *
    //* bootStorage: zugehoerige Mbc89BootStorage-Instanz (Firmware-Update-Staging)               *
    //* pinReset:    Pin, ueber den sich das Modul per Watchdog-Trick selbst neu startet          *
    //*********************************************************************************************

    #if defined(__AVR__)

        Mbc89Com::Mbc89Com(Mbc89Utils &utils, Mbc89Can &can, Mbc89BootStorage &bootStorage,
                            uint8_t pinReset)
            : Mbc89ComBase(utils, can, bootStorage, pinReset,
                            MBC89_ANZMESSWERTEMODUL, MBC89_ANZKONFIGMODUL),
              _pinReset(pinReset)
        {
            memcpy_P(_msgStop, initMsgStop, MBC_COM_MSG_LAENGE);
            memcpy_P(_msgGo, initMsgGo, MBC_COM_MSG_LAENGE);
            memcpy_P(_msgStopResponse, initMsgStopResponse, MBC_COM_MSG_LAENGE);
            memcpy_P(_msgGoResponse, initMsgGoResponse, MBC_COM_MSG_LAENGE);
            memcpy_P(_msgKonfigOk, initMsgKonfigOk, MBC_COM_MSG_LAENGE);
        }

    #else

        Mbc89Com::Mbc89Com(Mbc89Utils &utils, Mbc89Can &can, Mbc89BootStorage &bootStorage,
                            uint8_t pinCuScl, uint8_t pinCuSda, Mbc89DigitalIn &cuStopIn,
                            Mbc89DigitalOut &cuStopOut, Mbc89DigitalIn &cuGoIn,
                            Mbc89DigitalOut &cuGoOut, Mbc89DigitalIn &cuExt, uint8_t pinTrack)
            : Mbc89ComBase(utils, can, bootStorage,
                            MBC89_ANZMESSWERTEMODUL, MBC89_ANZKONFIGMODUL),
              _pinCuScl(pinCuScl), _pinCuSda(pinCuSda), _pinTrack(pinTrack),
              _cuStopIn(&cuStopIn), _cuStopOut(&cuStopOut), _cuGoIn(&cuGoIn),
              _cuGoOut(&cuGoOut), _cuExt(&cuExt)
        {
            cuPinScl = pinCuScl; cuPinSda = pinCuSda; cuPinTrack = pinTrack;

            cuStopInPtr  = _cuStopIn;
            cuStopOutPtr = _cuStopOut;
            cuGoInPtr    = _cuGoIn;
            cuGoOutPtr   = _cuGoOut;
            cuExtPtr     = _cuExt;

            memcpy_P(_msgStop, initMsgStop, MBC_COM_MSG_LAENGE);
            memcpy_P(_msgGo, initMsgGo, MBC_COM_MSG_LAENGE);
            memcpy_P(_msgStopResponse, initMsgStopResponse, MBC_COM_MSG_LAENGE);
            memcpy_P(_msgGoResponse, initMsgGoResponse, MBC_COM_MSG_LAENGE);
            memcpy_P(_msgKonfigOk, initMsgKonfigOk, MBC_COM_MSG_LAENGE);
        }

    #endif

    //*********************************************************************************************
    //* Siehe Klassenkommentar in Mbc89Com.h - reicht mit Sende-Wiederholung an den CAN-Bus       *
    //* weiter, NUR fuer die freistehenden CU-Bus-/Gleisdecoder-Funktionen gedacht.               *
    //*********************************************************************************************

    bool Mbc89Com::sendCan(const uint8_t msg[MBC_COM_MSG_LAENGE])
    {
        return sendMessage(msg);
    }

    //*********************************************************************************************
    //* Siehe Klassenkommentar in Mbc89Com.h - baut aus dem CS2-0x16-Vorlagentemplate der         *
    //* Basisklasse eine Zubehoer-Schalten-Nachricht und sendet sie mit Sende-Wiederholung.       *
    //*********************************************************************************************

    bool Mbc89Com::sendZubehoerSchalten(uint16_t addr, uint8_t richtung, bool strom,
                                         uint8_t *outMsg)
    {
        can_msg[MBC_COM_MSG_CS2_0x16][MBC_CAN_D2] = (uint8_t) (addr >> 8);
        can_msg[MBC_COM_MSG_CS2_0x16][MBC_CAN_D3] = (uint8_t) (addr & 0xff);
        can_msg[MBC_COM_MSG_CS2_0x16][MBC_CAN_D4] = richtung;
        can_msg[MBC_COM_MSG_CS2_0x16][MBC_CAN_D5] = strom ? 0x01 : 0x00;

        bool ok = sendMessage(can_msg[MBC_COM_MSG_CS2_0x16]);

        if (outMsg != nullptr) memcpy(outMsg, can_msg[MBC_COM_MSG_CS2_0x16], MBC_CAN_MSG_LEN);

        return ok;
    }

    //*********************************************************************************************
    //* Einmalig nach dem begin() der Basisklasse aufrufen: CU-Bus (TWI/Timer2) und Gleisdecoder  *
    //* (INT0/Timer1) scharfschalten, gespeicherte Parameter aus der EEPROM-Zusatzregion laden.   *
    //* Setzt cuUtils/cuOwner, BEVOR cuInitFn()/trackInitFn() aufgerufen werden, da diese bereits *
    //* auf die Lok-Datenbank im Systemarray zugreifen.                                           *
    //*********************************************************************************************

    void Mbc89Com::beginModule()
    {
        cuUtils = _utils;
        cuOwner = this;

        // Startsynchronisation - 1:1 aus main() uebernommen: der allererste cuTastenOnFn(CU_STOP)
        // ist im Original ein No-Op (cuIsInit erst nach cuInitFn() wahr) und MUSS daher vor
        // cuInitFn() aufgerufen werden - sonst wuerde er die von cuInitFn() intern ausgeloeste
        // CU_RESET-Tastensimulation sofort wieder abbrechen (cuIsInit waere zu diesem Zeitpunkt
        // schon wahr).

        cuTastenOnFn(CU_STOP);
        sendCan(_msgStop);

        // eepromReadParaExtern() MUSS vor cuInitFn() laufen (1:1 wie im Original, wo
        // mbc_eeprom_read_para() lange vor mbc_cu_init() steht) - cuInitFn() liest beim
        // Zuruecksetzen der Lok-Datenbank bereits arr[MBC_89_S_MAPPING] (Zentralentyp CSx/MS2) und
        // arr[MBC_89_S_I2C] (I2C-Frequenz). Lief das hier erst danach, wurde jede Lok unabhaengig
        // vom tatsaechlich gespeicherten Zentralentyp immer mit LOK_MAPPED=0 vorbelegt - bei MS2
        // (wo alle Loks ohne CONNECT6021 automatisch gemappt sein muessen) blieb dadurch jede
        // CU-Bus-/Gleis-Lok-Steuerung wirkungslos.

        eepromReadParaExtern();

        cuInitFn();
        trackInitFn();

        while (cuTasteOn) {}
        cuTastenOnFn(CU_GO);
        sendCan(_msgGo);
    }

    //*********************************************************************************************
    //* Je loop()-Durchlauf aufrufen: CU-Anwesenheit/I2C-Nachrichten pruefen, Gleissignal         *
    //* auswerten (beide nur wenn initDone(), wie Original com.init.f_init), CAN-                 *
    //* Nachrichtenempfang/-Auswertung. Die eigentliche Dispatch-Logik (System-Stopp/-Go,         *
    //* CONNECT6021, Lok-Abfragen, Keyboard-Weiterleitung, Resync nach Wiederverbindung) laeuft - *
    //* 1:1 wie im Original - NUR, wenn in diesem Zyklus tatsaechlich eine CAN-Nachricht empfangen*
    //* wurde.                                                                                    *
    //*********************************************************************************************

    void Mbc89Com::update()
    {
        poll();

        if (initDone())
        {
            cuPresentCheckFn();
            trackCheckFn();
        }

        uint8_t msg[MBC_CAN_MSG_LEN];

        if (_can->receive(msg))
        {
            check(msg);

            uint8_t canId = msg[MBC_CAN_ID];

            if (canId == CAN_ID_SYSTEM_BEFEHLE) handleSystemCommand(msg);
            if (canId == CAN_ID_CONNECT6021)    handleConnect6021(msg);

            handleLokAbfrage(msg);

            // Sende Aenderung des Keyboards auf dem I2C-Bus (nur wenn initDone(), wie Original
            // com.init.f_init) - D2==0x18 wird ausgeschlossen (Originalbedingung, 1:1 uebernommen
            // ohne weitere Interpretation)

            if (initDone() && (canId == CAN_ID_KEYBOARD) && (msg[MBC_CAN_D2] != 0x18))
            {
                cuSendKbFn(msg);
            }

            resyncAfterConnect();
        }
    }

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //+ Die 7 Erweiterungspunkte der Basisklasse (siehe Mbc89ComBase.h) - hier mbc-89-spezifisches+
    //+ EEPROM-/CS2-Verhalten ergaenzen                                                           +
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    //*********************************************************************************************
    //* Modulspezifische Parameterdaten aus dem EEPROM lesen - 5 Bloecke (SW-Version,             *
    //* Zubehoerprotokoll je Stellpult, Zentralentyp-Mapping, Basisadressblock, I2C-Frequenz),    *
    //* 0xff-Sentinel fuer den unbeschriebenen Zustand wie im Original.                           *
    //*********************************************************************************************

    void Mbc89Com::eepromReadParaExtern()
    {
        uint8_t *arr = _utils->array();
        uint8_t  buf[2];

        _utils->eepromReadExtra(EEX_89_SW, buf, 2);

        if (buf[0] == 0xff)
        {
            buf[0] = 1;
            buf[1] = 0;

            _utils->eepromWriteExtra(EEX_89_SW, buf, 2);
        }

        arr[MBC_89_S_SW]     = buf[0];
        arr[MBC_89_S_SW + 1] = buf[1];

        _utils->eepromReadExtra(EEX_89_PROTOKOLL, arr + MBC_89_S_MAGNET, 65);

        if (arr[MBC_89_S_MAGNET] == 0xff)
        {
            for (uint8_t index = 0; index < 65; index++) arr[MBC_89_S_MAGNET + index] = 0;

            _utils->eepromWriteExtra(EEX_89_PROTOKOLL, arr + MBC_89_S_MAGNET, 65);
        }

        uint8_t mapping;

        _utils->eepromReadExtra(EEX_89_MAPPING, &mapping, 1);

        if (mapping == 0xff)
        {
            mapping = 0;
            _utils->eepromWriteExtra(EEX_89_MAPPING, &mapping, 1);
        }

        arr[MBC_89_S_MAPPING] = mapping;

        uint8_t basis;

        _utils->eepromReadExtra(EEX_89_BASIS, &basis, 1);

        if (basis == 0xff)
        {
            basis = 1;
            _utils->eepromWriteExtra(EEX_89_BASIS, &basis, 1);
        }

        arr[MBC_89_S_BASIS] = basis;

        uint8_t i2c;

        _utils->eepromReadExtra(EEX_89_I2C, &i2c, 1);

        if (i2c == 0xff)
        {
            i2c = 40;
            _utils->eepromWriteExtra(EEX_89_I2C, &i2c, 1);
        }

        arr[MBC_89_S_I2C] = i2c;

        swInitExtern();
    }

    //*********************************************************************************************
    //* Persistiert einen der 5 Parameterbloecke, je nach block getrennt (Original:               *
    //* EE_89_SW/_PROTOKOLL/_MAPPING/_BASIS/_I2C).                                                *
    //*********************************************************************************************
    //*********************************************************************************************
    //* block: Identifier des zu schreibenden Blocks (0x0A...0x0E)                                *
    //*********************************************************************************************

    void Mbc89Com::eepromWriteParaExtern(uint8_t block)
    {
        uint8_t *arr = _utils->array();

        if (block == EE_89_SW)
        {
            uint8_t buf[2] = {arr[MBC_89_S_SW], arr[MBC_89_S_SW + 1]};

            _utils->eepromWriteExtra(EEX_89_SW, buf, 2);
            swInitExtern();
        }
        else if (block == EE_89_PROTOKOLL)
        {
            _utils->eepromWriteExtra(EEX_89_PROTOKOLL, arr + MBC_89_S_MAGNET, 65);
        }
        else if (block == EE_89_MAPPING)
        {
            _utils->eepromWriteExtra(EEX_89_MAPPING, arr + MBC_89_S_MAPPING, 1);
        }
        else if (block == EE_89_BASIS)
        {
            _utils->eepromWriteExtra(EEX_89_BASIS, arr + MBC_89_S_BASIS, 1);
        }
        else if (block == EE_89_I2C)
        {
            _utils->eepromWriteExtra(EEX_89_I2C, arr + MBC_89_S_I2C, 1);
        }
    }

    //*********************************************************************************************
    //* Traegt GUiD/HASH in _msgStopResponse/_msgGoResponse ein (Antworten auf einen empfangenen  *
    //* System-Stopp/-Go-Wunsch). _msgStop/_msgGo (spontane Broadcasts) bekommen bewusst NUR den  *
    //* HASH - ihre GUiD bleibt die universelle Broadcast-GUiD 0x00000000 (Originalverhalten, 1:1 *
    //* uebernommen).                                                                             *
    //*********************************************************************************************

    void Mbc89Com::hashInitExtern()
    {
        uint8_t *arr = _utils->array();

        _msgStopResponse[MBC_CAN_D0] = arr[MBC_S_GUID];
        _msgStopResponse[MBC_CAN_D1] = arr[MBC_S_GUID + 1];
        _msgStopResponse[MBC_CAN_D2] = arr[MBC_S_GUID + 2];
        _msgStopResponse[MBC_CAN_D3] = arr[MBC_S_GUID + 3];

        _msgGoResponse[MBC_CAN_D0] = arr[MBC_S_GUID];
        _msgGoResponse[MBC_CAN_D1] = arr[MBC_S_GUID + 1];
        _msgGoResponse[MBC_CAN_D2] = arr[MBC_S_GUID + 2];
        _msgGoResponse[MBC_CAN_D3] = arr[MBC_S_GUID + 3];

        _msgStopResponse[MBC_CAN_HASH_H] = can_msg[0][MBC_CAN_HASH_H];
        _msgStopResponse[MBC_CAN_HASH_L] = can_msg[0][MBC_CAN_HASH_L];

        _msgGoResponse[MBC_CAN_HASH_H] = can_msg[0][MBC_CAN_HASH_H];
        _msgGoResponse[MBC_CAN_HASH_L] = can_msg[0][MBC_CAN_HASH_L];

        _msgStop[MBC_CAN_HASH_H] = can_msg[0][MBC_CAN_HASH_H];
        _msgStop[MBC_CAN_HASH_L] = can_msg[0][MBC_CAN_HASH_L];

        _msgGo[MBC_CAN_HASH_H] = can_msg[0][MBC_CAN_HASH_H];
        _msgGo[MBC_CAN_HASH_L] = can_msg[0][MBC_CAN_HASH_L];

        // Bestaetigungsnachricht "Konfigwert uebernommen" fuer die SW-Major/-Minor-Konfigkanaele -
        // GUiD-Byte-Mischung (D0 aus Kennung, D1..D3 aus GUiD) 1:1 aus Mbc80Lib uebernommen

        _msgKonfigOk[MBC_CAN_D0] = arr[MBC_S_KN + 4];
        _msgKonfigOk[MBC_CAN_D1] = arr[MBC_S_GUID + 1];
        _msgKonfigOk[MBC_CAN_D2] = arr[MBC_S_GUID + 2];
        _msgKonfigOk[MBC_CAN_D3] = arr[MBC_S_GUID + 3];

        _msgKonfigOk[MBC_CAN_HASH_H] = can_msg[0][MBC_CAN_HASH_H];
        _msgKonfigOk[MBC_CAN_HASH_L] = can_msg[0][MBC_CAN_HASH_L];
    }

    //*********************************************************************************************
    //* Setzt die simulierte Connect6021-Softwareversion in die CS2-Ping-Antwort.                 *
    //*********************************************************************************************

    void Mbc89Com::swInitExtern()
    {
        uint8_t *arr = _utils->array();

        can_msg[MBC_COM_MSG_CS2_0x30_R][MBC_CAN_D4] = arr[MBC_89_S_SW];
        can_msg[MBC_COM_MSG_CS2_0x30_R][MBC_CAN_D5] = arr[MBC_89_S_SW + 1];
    }

    //*********************************************************************************************
    //* Modulspezifischen Parameterwert von der CS2 uebernehmen:                                  *
    //* msg[D5]==KANAL_SW_MAJ/KANAL_SW_MIN setzt das Major-/Minor-Byte der vorgetaeuschten        *
    //* Connect6021-Softwareversion (Rohwert 1...255), msg[D5]==KANAL_MAPPING setzt den           *
    //* Zentralentyp (Auswahlliste, 0=Mapping/1=Mapping&Direkt) und hebt/senkt dabei live jedes   *
    //* lok[LOK_MAPPED] ohne gesetztes LOK_MAPPED_ECHT-Bit auf den neuen Wert - eine bereits per  *
    //* CONNECT6021 44/45 echt zugewiesene Lok bleibt dauerhaft unangetastet, kein Reset noetig   *
    //* (siehe LOK_MAPPED_ECHT oben), msg[D5]==KANAL_BASIS setzt den Keyboard-Basisadressblock    *
    //* (1...4), msg[D5]==KANAL_I2C setzt den I2C-Bus-Takt (40...100 kHz), msg[D5] in             *
    //* KANAL_PROTOKOLL_BASE...+63 setzt einen der 64 "KeyBx GroupY"-Kanaele (Auswahlliste,       *
    //* 0=MM/1=DCC) im Zubehoerprotokoll-Array (MBC_89_S_MAGNET). Persistiert ueber               *
    //* eepromWriteParaExtern() (EE_89_SW/EE_89_MAPPING/EE_89_BASIS/EE_89_I2C/EE_89_PROTOKOLL,    *
    //* aktualisiert bei SW zusaetzlich can_msg[] via swInitExtern()) und bestaetigt der CSx. Ein *
    //* Wert ausserhalb des jeweiligen Bereichs bleibt ohne Wirkung und wird per FALSE quittiert. *
    //* Der Konfigurationswert ist laut CS2-Protokoll ein vollstaendiges Word ueber D6 (High) +   *
    //* D7 (Low), nicht nur D7.                                                                   *
    //*********************************************************************************************
    //* msg: CAN-Nachricht (msg[D5]==Kanalnummer, msg[D6..D7]=neuer Wert)                         *
    //*********************************************************************************************

    void Mbc89Com::cs2IndexParameterExtern(const uint8_t msg[MBC_COM_MSG_LAENGE])
    {
        uint8_t *arr      = _utils->array();
        uint8_t  kanal    = msg[MBC_CAN_D5];
        uint16_t wert     = (uint16_t) (((uint16_t) msg[MBC_CAN_D6] << 8) | msg[MBC_CAN_D7]);
        bool     gueltig;
        bool     geaendert = false;
        uint8_t  eeBlock;

        if (kanal == KANAL_SW_MAJ)
        {
            gueltig = (wert >= 1) && (wert <= 255);
            eeBlock = EE_89_SW;

            if (gueltig && (arr[MBC_89_S_SW] != (uint8_t) wert))
            {
                arr[MBC_89_S_SW] = (uint8_t) wert;
                geaendert = true;
            }
        }
        else if (kanal == KANAL_SW_MIN)
        {
            gueltig = (wert >= 1) && (wert <= 255);
            eeBlock = EE_89_SW;

            if (gueltig && (arr[MBC_89_S_SW + 1] != (uint8_t) wert))
            {
                arr[MBC_89_S_SW + 1] = (uint8_t) wert;
                geaendert = true;
            }
        }
        else if (kanal == KANAL_MAPPING)
        {
            // Auswahlliste, 0=Mapping/1=Mapping&Direkt - Index direkt uebernommen
            gueltig = (wert <= 1);
            eeBlock = EE_89_MAPPING;

            if (gueltig && (arr[MBC_89_S_MAPPING] != (uint8_t) wert))
            {
                arr[MBC_89_S_MAPPING] = (uint8_t) wert;
                geaendert = true;

                // Kein Modul-Reset noetig: lok[LOK_MAPPED] wird live auf den neuen Default
                // gehoben/gesenkt, aber nur bei Loks ohne LOK_MAPPED_ECHT-Bit. Eine Lok mit
                // bereits gesetztem Echt-Bit bleibt fuer immer unangetastet (bis zum naechsten
                // echten Reboot) - unabhaengig davon, wie oft der Zentralentyp seitdem
                // hin- und hergeschaltet wird.

                for (uint8_t index = 0; index < LOKS_ANZAHL; index++)
                {
                    uint8_t *lok = lokAt(index);

                    if (!(lok[LOK_MAPPED] & LOK_MAPPED_ECHT)) lok[LOK_MAPPED] = (uint8_t) wert;
                }
            }
        }
        else if (kanal == KANAL_BASIS)
        {
            gueltig = (wert >= 1) && (wert <= 4);
            eeBlock = EE_89_BASIS;

            if (gueltig && (arr[MBC_89_S_BASIS] != (uint8_t) wert))
            {
                arr[MBC_89_S_BASIS] = (uint8_t) wert;
                geaendert = true;
            }
        }
        else if (kanal == KANAL_I2C)
        {
            gueltig = (wert >= 40) && (wert <= 100);
            eeBlock = EE_89_I2C;

            if (gueltig && (arr[MBC_89_S_I2C] != (uint8_t) wert))
            {
                arr[MBC_89_S_I2C] = (uint8_t) wert;
                geaendert = true;
            }
        }
        else if ((kanal >= KANAL_PROTOKOLL_BASE)
                 && (kanal < (uint8_t) (KANAL_PROTOKOLL_BASE + PROTOKOLL_ANZ)))
        {
            // Auswahlliste, 0=MM/1=DCC - Index direkt uebernommen, wie KANAL_MAPPING
            gueltig = (wert <= 1);
            eeBlock = EE_89_PROTOKOLL;

            if (gueltig)
            {
                uint8_t protoIndex = (uint8_t) (kanal - KANAL_PROTOKOLL_BASE);

                if (arr[MBC_89_S_MAGNET + protoIndex] != (uint8_t) wert)
                {
                    arr[MBC_89_S_MAGNET + protoIndex] = (uint8_t) wert;
                    geaendert = true;
                }
            }
        }
        else
        {
            return;
        }

        if (geaendert) eepromWriteParaExtern(eeBlock);

        _msgKonfigOk[MBC_CAN_D5] = kanal;
        _msgKonfigOk[MBC_CAN_D6] = gueltig ? 0x01 : 0x00;

        sendMessage(_msgKonfigOk);
    }

    //*********************************************************************************************
    //* Baut die angeforderte Kanalzeile frisch in _cs2ScratchRow zusammen - mbc-89 hat 69        *
    //* Kanaele: 5 "klassische" (SW-Major/SW-Minor der vorgetaeuschten Connect6021-               *
    //* Softwareversion, Mapping, Basis, I2C) sowie 64 "KeyBx GroupY"-Kanaele (Zubehoerprotokoll  *
    //* MM/DCC). Alle 69 Zeilen kommen aus der PROGMEM-Vorlage initCs2Ex[], nur der aktuelle Wert *
    //* wird bei jedem Aufruf frisch aus dem Systemarray gepatcht - spart gegenueber einer        *
    //* klassischen RAM-Kopie 69x40 Byte RAM. Sonst nullptr.                                      *
    //*********************************************************************************************
    //*********************************************************************************************
    //* laufendeNr: 0..ANZMESSWERTEMODUL+ANZKONFIGMODUL-1                                         *
    //*********************************************************************************************
    //*********************************************************************************************
    //* Return: Zeiger auf _cs2ScratchRow (nur bis zum naechsten Aufruf gueltig) oder nullptr     *
    //*********************************************************************************************

    const uint8_t* Mbc89Com::cs2IndexExtern(uint8_t laufendeNr)
    {
        if (laufendeNr >= (MBC89_ANZMESSWERTEMODUL + MBC89_ANZKONFIGMODUL)) return nullptr;

        memcpy_P(_cs2ScratchRow, initCs2Ex[laufendeNr], sizeof(_cs2ScratchRow));

        uint8_t *arr = _utils->array();

        if (laufendeNr == ROW_SW_MAJ)
        {
            _cs2ScratchRow[SW_OFFSET_H] = 0;
            _cs2ScratchRow[SW_OFFSET_L] = arr[MBC_89_S_SW];
        }
        else if (laufendeNr == ROW_SW_MIN)
        {
            _cs2ScratchRow[SW_OFFSET_H] = 0;
            _cs2ScratchRow[SW_OFFSET_L] = arr[MBC_89_S_SW + 1];
        }
        else if (laufendeNr == ROW_MAPPING)
        {
            _cs2ScratchRow[MAPPING_OFFSET] = arr[MBC_89_S_MAPPING];
        }
        else if (laufendeNr == ROW_BASIS)
        {
            _cs2ScratchRow[BASIS_OFFSET_H] = 0;
            _cs2ScratchRow[BASIS_OFFSET_L] = arr[MBC_89_S_BASIS];
        }
        else if (laufendeNr == ROW_I2C)
        {
            _cs2ScratchRow[I2C_OFFSET_H] = 0;
            _cs2ScratchRow[I2C_OFFSET_L] = arr[MBC_89_S_I2C];
        }
        else if ((laufendeNr >= ROW_PROTOKOLL_BASE)
                 && (laufendeNr < (uint8_t) (ROW_PROTOKOLL_BASE + PROTOKOLL_ANZ)))
        {
            uint8_t protoIndex = (uint8_t) (laufendeNr - ROW_PROTOKOLL_BASE);

            _cs2ScratchRow[PROTOKOLL_VALUE_OFFSET] = arr[MBC_89_S_MAGNET + protoIndex];
        }

        return _cs2ScratchRow;
    }

    //*********************************************************************************************
    //* Feste CS2-Geraetekennung 0x0020 (Maerklin-Vorgabe fuer Connect6021-Geraete wie mbc-89 -   *
    //* anders als bei den Booster-Modulen mbc-80/82/93 ist diese Zuweisung im Original-          *
    //* Disassembly widerspruchsfrei, kein Kompilier-Bug).                                        *
    //*********************************************************************************************
    //*********************************************************************************************
    //* knH/knL: gesetzt auf 0x00/0x20                                                            *
    //*********************************************************************************************
    //*********************************************************************************************
    //* Return: true (mbc-89 hat immer eine feste Kennung)                                        *
    //*********************************************************************************************

    bool Mbc89Com::kennerFixedExtern(uint8_t &knH, uint8_t &knL)
    {
        knH = 0x00;
        knL = 0x20;
        return true;
    }

    //*********************************************************************************************
    //* Liefert den Zeiger auf den Datensatz der MM-Adresse index (0...79) innerhalb des          *
    //* Systemarrays.                                                                             *
    //*********************************************************************************************
    //*********************************************************************************************
    //* index: MM-Adresse 0...79                                                                  *
    //*********************************************************************************************
    //*********************************************************************************************
    //* Return: Zeiger auf den 10-Byte-Datensatz (siehe LOK_*-Feldoffsets)                        *
    //*********************************************************************************************

    uint8_t* Mbc89Com::lokAt(uint8_t index)
    {
        return _utils->array() + MBC_89_S_LOKS + ((uint16_t) index * LOKS_ITEMS);
    }

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //+ System-Stopp/Go (CAN-ID 0x00), CONNECT6021-Protokoll (CAN-ID 0x44/0x45) und Lok-Kommando- +
    //+ Weiterleitung - 1:1 aus main()s Empfangsblock uebernommen                                 +
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    //*********************************************************************************************
    //* System-Stopp/Go auswerten - 1:1 aus main()s Empfangsblock uebernommen, inklusive eines    *
    //* auffaelligen Vergleichs: die modulbezogene GUiD-Pruefung vergleicht D2 gegen dieselbe     *
    //* GUiD-Stelle wie D1 statt gegen arr[MBC_S_GUID+2] - ein Copy-Paste-Fehler der Original-    *
    //* Firmware (identisches Muster wie bei mbc-93), bewusst NICHT korrigiert. Reagiert nur, wenn*
    //* die GUiD universell (D0...D3 alle 0) oder modulbezogen ist UND initDone() (entspricht     *
    //* Original com.init.f_init). Loest die simulierten CU-Tasten aus statt Ausgangsbits zu      *
    //* schalten (mbc-89 hat keinen eigenen Booster-Ausgang).                                     *
    //*********************************************************************************************
    //*********************************************************************************************
    //* msg: empfangene CAN-Nachricht                                                             *
    //*********************************************************************************************

    void Mbc89Com::handleSystemCommand(const uint8_t msg[MBC_CAN_MSG_LEN])
    {
        if (!initDone()) return;

        uint8_t *arr = _utils->array();

        bool istUniversell = (msg[MBC_CAN_D0] == 0x00) && (msg[MBC_CAN_D1] == 0x00)
                          && (msg[MBC_CAN_D2] == 0x00) && (msg[MBC_CAN_D3] == 0x00);

        bool istModul = (msg[MBC_CAN_D0] == arr[MBC_S_GUID])
                     && (msg[MBC_CAN_D1] == arr[MBC_S_GUID + 1])
                     && (msg[MBC_CAN_D2] == arr[MBC_S_GUID + 1])
                     && (msg[MBC_CAN_D3] == arr[MBC_S_GUID + 3]);

        if (!istUniversell && !istModul) return;

        if (msg[MBC_CAN_D4] == 0x00)
        {
            while (cuTasteOn) {}
            cuTastenOnFn(CU_STOP);

            cuGo = 0;

            sendCan(_msgStopResponse);
        }
        else if (msg[MBC_CAN_D4] == 0x01)
        {
            while (cuTasteOn) {}
            cuTastenOnFn(CU_GO);

            cuGo = 1;

            sendCan(_msgGoResponse);
        }
    }

    //*********************************************************************************************
    //* CONNECT6021-Mapping-Zuweisung/-Reset auswerten - 1:1 aus main() uebernommen. DLC=6:       *
    //* Stream-Start (D4=D5=0x00, setzt alle 80 Lok-Adressfelder auf unverbundene GUiD-Defaults   *
    //* zurueck, MBC_89_LOK_MAPPED bleibt dabei UNVERAENDERT) oder Stream-Ende (D4=D5=0xff, setzt *
    //* cuIsConnected). DLC=5: weist einem einzelnen Slot (D4, MM-Adresse 0 entspricht Slot 80)   *
    //* eine GUiD (D0...D3) zu und markiert ihn als gemappt.                                      *
    //*********************************************************************************************
    //*********************************************************************************************
    //* msg: empfangene CAN-Nachricht                                                             *
    //*********************************************************************************************

    void Mbc89Com::handleConnect6021(const uint8_t msg[MBC_CAN_MSG_LEN])
    {
        uint8_t response[MBC_CAN_MSG_LEN];

        memcpy(response, msg, MBC_CAN_MSG_LEN);

        if (msg[MBC_CAN_DLC] == 6)
        {
            if ((msg[MBC_CAN_D4] == 0x00) && (msg[MBC_CAN_D5] == 0x00))
            {
                for (uint8_t index = 0; index < LOKS_ANZAHL; index++)
                {
                    uint8_t *lok = lokAt(index);

                    lok[LOK_ADDR4] = 0x00;
                    lok[LOK_ADDR3] = 0x00;
                    lok[LOK_ADDR2] = 0x00;
                    lok[LOK_ADDR1] = index;
                }

                lokAt(0)[LOK_ADDR1] = 80;
            }
            else if ((msg[MBC_CAN_D4] == 0xff) && (msg[MBC_CAN_D5] == 0xff))
            {
                cuIsConnected = true;
            }

            response[MBC_CAN_ID] = CAN_ID_CONNECT6021_R;
            sendCan(response);
        }
        else if (msg[MBC_CAN_DLC] == 5)
        {
            uint8_t index = msg[MBC_CAN_D4];

            if (index > 80) return;

            uint8_t *lok = lokAt((index == 80) ? 0 : index);

            lok[LOK_ADDR4]  = msg[MBC_CAN_D0];
            lok[LOK_ADDR3]  = msg[MBC_CAN_D1];
            lok[LOK_ADDR2]  = msg[MBC_CAN_D2];
            lok[LOK_ADDR1]  = msg[MBC_CAN_D3];
            lok[LOK_MAPPED] = 0x01 | LOK_MAPPED_ECHT;

            response[MBC_CAN_ID] = CAN_ID_CONNECT6021_R;
            sendCan(response);
        }
    }

    //*********************************************************************************************
    //* Nach einem Modul-Reset (cuIsReset) UND einer bereits abgeschlossenen                      *
    //* CONNECT6021-Wiederverbindung (cuIsConnected) UND anwesender CU (cuIsPresent) einmalig alle*
    //* gemappten Loks auf Stopp, Vorwaerts und keine Funktion zuruecksetzen - 1:1 aus main()     *
    //* uebernommen (inkl. der 50-ms-Pausen zwischen den Sendungen, damit ist ein vollstaendiger  *
    //* Durchlauf ueber alle 80 Slots in ungeplanten Ausnahmefaellen mehrere Sekunden blockierend,*
    //* wie im Original - bewusst NICHT entschaerzt, das CONNECT6021-Streaming braucht diese      *
    //* Reihenfolge/dieses Timing). Blinkt waehrenddessen als Traffic-Anzeige durchgehend ORANGE  *
    //* (wie Mbc89Utils::waitFilled()), damit die Blockierung nicht wie ein haengendes Modul      *
    //* aussieht, und ruft dafuer _utils->update() zwischen den delay(50)-Pausen auf - das         *
    //* fuettert nebenbei auch den ESP32-Task-Watchdog (8 s Timeout, siehe                        *
    //* Mbc89Platform::watchdogBegin()), der sonst waehrend dieser Blockierung auslaufen wuerde.  *
    //*********************************************************************************************

    void Mbc89Com::resyncAfterConnect()
    {
        if (!(cuIsReset && cuIsConnected && cuIsPresent)) return;

        cuIsReset     = false;
        cuIsConnected = false;

        _utils->ledToggleOn(MBC_LED_MODKONFIG);

        cuResetFn();

        for (uint8_t index = 0; index < LOKS_ANZAHL; index++)
        {
            uint8_t *lok = lokAt(index);

            if (!lok[LOK_MAPPED]) continue;

            uint8_t msg[MBC_CAN_MSG_LEN];

            memcpy(msg, msgLokSpeed, MBC_CAN_MSG_LEN);
            msg[MBC_CAN_D0] = lok[LOK_ADDR4];
            msg[MBC_CAN_D1] = lok[LOK_ADDR3];
            msg[MBC_CAN_D2] = lok[LOK_ADDR2];
            msg[MBC_CAN_D3] = lok[LOK_ADDR1];
            msg[MBC_CAN_D4] = 0;
            msg[MBC_CAN_D5] = 0;
            sendCan(msg);
            delay(50);
            _utils->update();

            memcpy(msg, msgLokDir, MBC_CAN_MSG_LEN);
            msg[MBC_CAN_D0] = lok[LOK_ADDR4];
            msg[MBC_CAN_D1] = lok[LOK_ADDR3];
            msg[MBC_CAN_D2] = lok[LOK_ADDR2];
            msg[MBC_CAN_D3] = lok[LOK_ADDR1];
            msg[MBC_CAN_D4] = DIR_VORWAERTS;
            sendCan(msg);
            delay(50);
            _utils->update();

            memcpy(msg, msgLokFx, MBC_CAN_MSG_LEN);
            msg[MBC_CAN_D0] = lok[LOK_ADDR4];
            msg[MBC_CAN_D1] = lok[LOK_ADDR3];
            msg[MBC_CAN_D2] = lok[LOK_ADDR2];
            msg[MBC_CAN_D3] = lok[LOK_ADDR1];

            for (uint8_t fx = 0; fx < 5; fx++)
            {
                msg[MBC_CAN_D4] = fx;
                msg[MBC_CAN_D5] = 0;
                sendCan(msg);
                delay(50);
                _utils->update();
            }
        }

        _utils->ledToggleOff();
    }

    //*********************************************************************************************
    //* Richtungs-/Funktions-/Geschwindigkeitsabfragen der CS2 (CAN-ID 0x0A/0x0C/0x08) aus der    *
    //* eigenen Lokdatenbank beantworten, wenn eine CU oder ein Gleissignal (INFRA) erkannt wurde *
    //* - sperrt damit die direkte Aenderung durch die CS2 fuer diese Loks (Original: 3 separate, *
    //* fast identische Schleifen in main(), hier zu einer zusammengefasst). Bricht die Lok-      *
    //* Schleife nach einem Treffer NICHT ab (1:1 Originalverhalten).                             *
    //*********************************************************************************************
    //*********************************************************************************************
    //* msg: empfangene CAN-Nachricht                                                             *
    //*********************************************************************************************

    void Mbc89Com::handleLokAbfrage(const uint8_t msg[MBC_CAN_MSG_LEN])
    {
        if (!(cuIsPresent || infraIsPresent)) return;

        uint8_t canId = msg[MBC_CAN_ID];

        if ((canId != CAN_ID_LOK_RICHTUNG) && (canId != CAN_ID_LOK_FUNKTION)
         && (canId != CAN_ID_LOK_GESCHWINDIGKEIT)) return;

        for (uint8_t index = 0; index < LOKS_ANZAHL; index++)
        {
            uint8_t *lok = lokAt(index);

            bool addrMatch = (lok[LOK_ADDR4] == msg[MBC_CAN_D0])
                          && (lok[LOK_ADDR3] == msg[MBC_CAN_D1])
                          && (lok[LOK_ADDR2] == msg[MBC_CAN_D2])
                          && (lok[LOK_ADDR1] == msg[MBC_CAN_D3])
                          && lok[LOK_MAPPED];

            if (!addrMatch) continue;

            uint8_t response[MBC_CAN_MSG_LEN];

            memcpy(response, msg, MBC_CAN_MSG_LEN);

            if (canId == CAN_ID_LOK_RICHTUNG)
            {
                response[MBC_CAN_D4] = lok[LOK_DIR];
                sendCan(response);

                uint8_t speed = lok[LOK_SPEED];

                if (speed == 0) speed = 1;

                uint16_t cs2Speed = (uint16_t) (speed - 1) * MM_SPEED_STEP;

                response[MBC_CAN_ID]  = CAN_ID_LOK_GESCHWINDIGKEIT;
                response[MBC_CAN_DLC] = 6;
                response[MBC_CAN_D4]  = (uint8_t) ((cs2Speed & 0xff00) >> 8);
                response[MBC_CAN_D5]  = (uint8_t) (cs2Speed & 0xff);
                sendCan(response);
            }
            else if (canId == CAN_ID_LOK_FUNKTION)
            {
                switch (msg[MBC_CAN_D4])
                {
                    case 0: response[MBC_CAN_D5] = lok[LOK_F0]; break;
                    case 1: response[MBC_CAN_D5] = lok[LOK_FX] & 0x01; break;
                    case 2: response[MBC_CAN_D5] = (lok[LOK_FX] & 0x02) >> 1; break;
                    case 3: response[MBC_CAN_D5] = (lok[LOK_FX] & 0x04) >> 2; break;
                    case 4: response[MBC_CAN_D5] = (lok[LOK_FX] & 0x08) >> 3; break;
                }

                sendCan(response);
            }
            else if (canId == CAN_ID_LOK_GESCHWINDIGKEIT)
            {
                uint8_t speed = lok[LOK_SPEED];

                if (speed == 0) speed = 1;

                uint16_t cs2Speed = (uint16_t) (speed - 1) * MM_SPEED_STEP;

                response[MBC_CAN_D4] = (uint8_t) ((cs2Speed & 0xff00) >> 8);
                response[MBC_CAN_D5] = (uint8_t) (cs2Speed & 0xff);
                sendCan(response);
            }
        }
    }

//------ Ende Klasse Mbc89Com ---------------------------------------------------------------------

//------ Start CU-Bus (I2C) Subsystem -------------------------------------------------------------

//*************************************************************************************************
//* Initialisiere CU-Bus-Hardware (GPIO/TWI/Timer2) und Lok-Speicher - 1:1 aus mbc_cu_init()      *
//* uebernommen. Die Lok-Adresse als GUiD entspricht dabei der MM-Adresse, die Fahrtrichtung wird *
//* auf VORWAERTS gestellt, das Mappingflag auf den Zentralentyp vorbelegt (0 bei CSx, dann wird  *
//* das Mapping ueber CAN gesetzt; 1 bei MS2, dann werden alle Loks als MM-Adresse an die MS2     *
//* geroutet, da sie keine Mapping-GUI besitzt).                                                  *
//*************************************************************************************************

    #if defined(__AVR__)

        static void cuInitFn()
        {
            // STOP-Taste (Eingang mit Pull-Up) / STOP-Ausgang (idle HIGH)
            PORTC |= (uint8_t)  (1 << BIT_CU_STOP_IN);
            DDRC  &= (uint8_t) ~(1 << BIT_CU_STOP_IN);
            PORTC &= (uint8_t) ~(1 << BIT_CU_STOP_OUT);
            DDRC  |= (uint8_t)  (1 << BIT_CU_STOP_OUT);

            // CU-Anwesenheit - OHNE Pull-Up, da Grundzustand LOW sein soll und erst eine echte CU
            // den Pegel auf HIGH zieht. Mit Pull-Up entstuende ein Spannungsteiler mit einem
            // unzulaessigen Pegel (Originalkommentar 1:1 uebernommen).
            DDRC &= (uint8_t) ~(1 << BIT_CU_EXT);

            // GO-Taste (Eingang mit Pull-Up) / GO-Ausgang (idle HIGH)
            PORTC |= (uint8_t)  (1 << BIT_CU_GO_IN);
            DDRC  &= (uint8_t) ~(1 << BIT_CU_GO_IN);
            PORTC &= (uint8_t) ~(1 << BIT_CU_GO_OUT);
            DDRC  |= (uint8_t)  (1 << BIT_CU_GO_OUT);

            // I2C-Bus mit Terminierung (Pull-Up auf SCL/SDA)
            PORTC |= (uint8_t)  (1 << BIT_CU_SCL);
            DDRC  &= (uint8_t) ~(1 << BIT_CU_SCL);
            PORTC |= (uint8_t)  (1 << BIT_CU_SDA);
            DDRC  &= (uint8_t) ~(1 << BIT_CU_SDA);

            // Timer2 auf CTC-Modus, Vorteiler 1024, Comparematch bei 1,024 ms (Tasterperiode)
            OCR2A  = 15;
            TCCR2A = 0;
            TCCR2B = (uint8_t) ((1 << WGM22) | (1 << CS22) | (1 << CS20));
            TCNT2  = 0;

            // Lok-Speicher zuruecksetzen
            uint8_t *arr = cuUtils->array();

            for (uint8_t index = 0; index < LOKS_ANZAHL; index++)
            {
                uint8_t *lok = arr + MBC_89_S_LOKS + ((uint16_t) index * LOKS_ITEMS);

                lok[LOK_SPEED]  = 0x00;
                lok[LOK_DIR]    = DIR_VORWAERTS;
                lok[LOK_F0]     = 0x00;
                lok[LOK_FX]     = 0x00;
                lok[LOK_ADDR4]  = 0x00;
                lok[LOK_ADDR3]  = 0x00;
                lok[LOK_ADDR2]  = 0x00;
                lok[LOK_ADDR1]  = index;
                lok[LOK_MAPPED] = arr[MBC_89_S_MAPPING];
                lok[LOK_INFRA]  = 0x00;
            }

            // Lok-Adresse 0 auf dem I2C-/Gleis-Bus entspricht Lok-Adresse 80 bei CSx/MS2
            arr[MBC_89_S_LOKS + LOK_ADDR1] = 80;

            cuGo          = 0;
            cuIsInit      = true;
            cuIsPresent   = (PINC & (uint8_t) (1 << BIT_CU_EXT)) != 0;
            cuTasteOn     = false;
            cuIsConnected = false;
            cuIsReset     = false;

            // I2C-Bus initialisieren (I2C-Frequenz aus dem Systemarray), Slave-Adresse als CU
            // setzen
            TWBR = (uint8_t) (((F_CPU / ((uint32_t) arr[MBC_89_S_I2C] * 1000UL)) - 16UL) / 8UL);

            TWAR = 0xfe;
            TWDR = 0xff;
            TWCR = (uint8_t) ((1 << TWEN) | (1 << TWEA) | (1 << TWIE));

            // Resette CU
            while (cuTasteOn) {}

            cuTastenOnFn(CU_RESET);
        }

    #else

        static void cuInitFn()
        {
            // STOP/GO-OUT sind als activeLow=true angelegt (siehe Sketch) - write(false)=idle HIGH,
            // write(true)=Taste gedrueckt/LOW (siehe cuTastenOnFn()). STOP/GO-IN ebenso
            // activeLow=true - read()==true bedeutet physisch gedrueckt (LOW).

            cuStopInPtr->begin();
            cuStopOutPtr->begin();
            cuStopOutPtr->write(false);   // idle

            // CU-Anwesenheit - activeLow=false (siehe AVR-Originalkommentar oben: Grundzustand
            // LOW=nicht anwesend, echte CU zieht HIGH=anwesend) - read() liefert cuIsPresent direkt

            cuExtPtr->begin();

            cuGoInPtr->begin();
            cuGoOutPtr->begin();
            cuGoOutPtr->write(false);   // idle

            // Lok-Speicher zuruecksetzen (identisch zur AVR-Fassung)
            uint8_t *arr = cuUtils->array();

            for (uint8_t index = 0; index < LOKS_ANZAHL; index++)
            {
                uint8_t *lok = arr + MBC_89_S_LOKS + ((uint16_t) index * LOKS_ITEMS);

                lok[LOK_SPEED]  = 0x00;
                lok[LOK_DIR]    = DIR_VORWAERTS;
                lok[LOK_F0]     = 0x00;
                lok[LOK_FX]     = 0x00;
                lok[LOK_ADDR4]  = 0x00;
                lok[LOK_ADDR3]  = 0x00;
                lok[LOK_ADDR2]  = 0x00;
                lok[LOK_ADDR1]  = index;
                lok[LOK_MAPPED] = arr[MBC_89_S_MAPPING];
                lok[LOK_INFRA]  = 0x00;
            }

            arr[MBC_89_S_LOKS + LOK_ADDR1] = 80;

            cuGo          = 0;
            cuIsInit      = true;
            cuIsPresent   = cuExtPtr->read();
            cuTasteOn     = false;
            cuIsConnected = false;
            cuIsReset     = false;

            // Software-I2C-Engine starten (siehe Mbc89I2cSoft-Abschnitt) - I2C-Frequenz aus dem
            // Systemarray, Slave-Adresse als Zentrale (0xfe)
            i2cSoftInit(cuPinScl, cuPinSda, arr[MBC_89_S_I2C]);
            i2cSoftSetOwnAddr(0xfe);

            // Einmaliger One-Shot-Timer fuer die STOP/GO/RESET-Tastensimulation (ersetzt
            // ISR(TIMER2_COMPA_vect) - siehe cuTastenOnFn()/cuTastenReleaseCallback())
            esp_timer_create_args_t timerArgs = {};
            timerArgs.callback = &cuTastenReleaseCallback;
            timerArgs.name     = "cuTaste";
            esp_timer_create(&timerArgs, &cuTastenTimer);

            while (cuTasteOn) {}

            cuTastenOnFn(CU_RESET);
        }

    #endif

//*************************************************************************************************
//* STOP/GO/RESET-Ausgang einschalten (Original: mbc_cu_tasten_on()). Wird nur ausgefuehrt, wenn  *
//* die CU-Bus-Hardware bereits initialisiert wurde.                                              *
//*************************************************************************************************
//*************************************************************************************************
//* taste: CU_GO -> GO-Taste, CU_STOP -> STOPP-Taste, CU_RESET -> GO und STOPP-Taste              *
//*************************************************************************************************
//*************************************************************************************************
//* Return: 0, wenn cuIsInit noch nicht gesetzt ist, sonst 1                                      *
//*************************************************************************************************

    #if defined(__AVR__)

        static uint8_t cuTastenOnFn(uint8_t taste)
        {
            if (!cuIsInit) return 0;

            switch (taste)
            {
                case CU_GO:
                    PORTC &= (uint8_t) ~(1 << BIT_CU_GO_OUT);
                    cuGo = 1;
                break;

                case CU_STOP:
                    PORTC &= (uint8_t) ~(1 << BIT_CU_STOP_OUT);
                    cuGo = 0;
                break;

                case CU_RESET:
                    PORTC &= (uint8_t) ~(1 << BIT_CU_STOP_OUT);
                    PORTC &= (uint8_t) ~(1 << BIT_CU_GO_OUT);
                    cuGo = RESET_CU_STEP_1;
                break;
            }

            TCNT2  = 0;
            TIMSK2 = (uint8_t) (1 << OCIE2A);
            cuTasteOn = true;

            return 1;
        }

    #else

        //*****************************************************************************************
        //* Ersetzt ISR(TIMER2_COMPA_vect) - statt periodischem 1,024-ms-Tick (100x=~100ms fuer     *
        //* GO/STOPP, 1500x=~1,5s fuer RESET) wird hier direkt ein einmaliger esp_timer fuer die    *
        //* jeweils benoetigte Gesamtdauer gestartet (siehe cuTastenOnFn()). Zeiten 1:1 aus den      *
        //* Original-Tickzahlen umgerechnet (100*1,024ms/1500*1,024ms).                              *
        //*****************************************************************************************

        static void cuTastenReleaseCallback(void *arg)
        {
            (void) arg;

            cuGoOutPtr->write(false);
            cuStopOutPtr->write(false);

            if (cuGo == RESET_CU_STEP_1)
            {
                cuStopOutPtr->write(true);
                cuGo = RESET_CU_STEP_2;

                esp_timer_start_once(cuTastenTimer, 1536000ULL);
            }
            else
            {
                if (cuGo == RESET_CU_STEP_2)
                {
                    cuGo      = 0;
                    cuIsReset = true;
                }

                cuTasteOn = false;
            }
        }

        static uint8_t cuTastenOnFn(uint8_t taste)
        {
            if (!cuIsInit) return 0;

            switch (taste)
            {
                case CU_GO:
                    cuGoOutPtr->write(true);
                    cuGo = 1;
                break;

                case CU_STOP:
                    cuStopOutPtr->write(true);
                    cuGo = 0;
                break;

                case CU_RESET:
                    cuStopOutPtr->write(true);
                    cuGoOutPtr->write(true);
                    cuGo = RESET_CU_STEP_1;
                break;
            }

            cuTasteOn = true;

            esp_timer_start_once(cuTastenTimer, (taste == CU_RESET) ? 1536000ULL : 102400ULL);

            return 1;
        }

    #endif

//*************************************************************************************************
//* Teste, ob STOP/GO-Taste an der CU physisch gedrueckt wurde - 1:1 aus mbc_cu_tasten_check()    *
//* uebernommen, sendet bei einer erkannten Tastenbetaetigung das spontane System-Stopp-/Go-      *
//* Telegramm.                                                                                    *
//*************************************************************************************************

    static void cuTastenCheckFn()
    {
        #if defined(__AVR__)
            bool stopGedrueckt = (PINC & (uint8_t) (1 << BIT_CU_STOP_IN)) == 0;
            bool goGedrueckt   = (PINC & (uint8_t) (1 << BIT_CU_GO_IN)) == 0;
        #else
            bool stopGedrueckt = cuStopInPtr->read();
            bool goGedrueckt   = cuGoInPtr->read();
        #endif

        if (stopGedrueckt && cuGo && !cuTasteOn)
        {
            cuTastenOnFn(CU_STOP);
            cuOwner->sendCan(cuOwner->stopBroadcast());
            cuGo = 0;
        }

        if (goGedrueckt && !cuGo && !cuTasteOn)
        {
            cuTastenOnFn(CU_GO);
            cuOwner->sendCan(cuOwner->goBroadcast());
            cuGo = 1;
        }
    }

//*************************************************************************************************
//* Sende anliegende CSx/MS2-Nachrichten an die CU via I2C-Bus - 1:1 aus mbc_i2c_send_cu()        *
//* uebernommen. Setzt den TWI-Sendepuffer entsprechend index/addr/funktion, loest per START-     *
//* Kondition den MASTER-Sendevorgang aus und wartet (busy) bis ISR(TWI_vect) ihn abgeschlossen   *
//* und den Bus wieder in den SLAVE-Modus zurueckgesetzt hat.                                     *
//*************************************************************************************************
//*************************************************************************************************
//* index:    I2C-Botschaftsindex (TWI_SEND_KB/_STARTSTOP/_STARTSTOPCONTROL/_ALLESNULL)           *
//* addr:     Kennung Keyboard/Decoder                                                            *
//* funktion: Kennung Taste/Funktion                                                              *
//*************************************************************************************************

    static void i2cSendCuFn(uint8_t index, uint8_t addr, uint8_t funktion)
    {
        switch (index)
        {
            case TWI_SEND_KB:
                twiTxd[1] = addr;
                twiTxd[2] = 0xfe;
                twiTxd[3] = funktion;
                twiTxd[0] = 3;
            break;

            case TWI_SEND_STARTSTOP:
                twiTxd[0] = 0;
            break;

            case TWI_SEND_STARTSTOPCONTROL:
                twiTxd[1] = addr;
                twiTxd[0] = 1;
            break;

            case TWI_SEND_ALLESNULL:
                twiTxd[1] = 0xfe;
                twiTxd[2] = 0x02;
                twiTxd[3] = addr;
                twiTxd[4] = 0x20;
                twiTxd[0] = 4;
            break;
        }

        #if defined(__AVR__)
            TWCR = TWCR_RESTART_MASTER;
            twiMode = TWI_MASTER;

            while (twiMode == TWI_MASTER) {}
        #else
            i2cSoftMasterSend();
        #endif
    }

//*************************************************************************************************
//* Pruefe anliegende CU-Bus-Nachrichten (Control 80f Lok-Speed/F0, Zentrale-Bestaetigung Lok-    *
//* Richtung, Control 80f F1...F4, Memory 6043 Fahrstrassen-Abschluss, Keyboard 6040) - 1:1 aus   *
//* mbc_cu_i2c_check() uebernommen. Wertet den TWI-Ringpuffer aus und leitet Aenderungen als      *
//* CS2-CAN-Nachrichten weiter bzw. aktualisiert die Lok-Datenbank.                               *
//*************************************************************************************************
//*************************************************************************************************
//* Return: immer 1 (Originalverhalten, Rueckgabewert ungenutzt)                                  *
//*************************************************************************************************

    static uint8_t cuI2cCheckFn()
    {
        if (twiRxdInterrupt == twiRxdAktuell) return 1;

        uint8_t *arr = cuUtils->array();

        // Control 80f: Lok-Speed/F0-Nachricht (gueltiger Befehl bei frame[2] in 0x02...0x14) -
        // Format z.B. sFEa02a18aA0ap: Zentrale(FE), Control-80f-Nr(02), Lokadresse(18=24), F0-Info
        // und Fahrstufe im letzten Byte (2/3=F0 aus/an, 0=Fahrstufe 0, 1=Umschalten, 2...F=Stufe)

        if ((twiRxd[twiRxdAktuell][0] == 3) && (twiRxd[twiRxdAktuell][2] > 0x01)
                                          && (twiRxd[twiRxdAktuell][2] < 0x15))
        {
            uint8_t  mmLokAddr = twiRxd[twiRxdAktuell][3];
            uint8_t *lok = arr + MBC_89_S_LOKS + ((uint16_t) mmLokAddr * LOKS_ITEMS);

            // nur MM-Adressen 0(80)...79 zulaessig; frame[4]<0xC0 schliesst einen
            // Steuerpult-Fehler aus.

            if ((mmLokAddr < 80) && (twiRxd[twiRxdAktuell][4] < 0xC0))
            {
                // Solange der Gleisdecoder diese Adresse fuehrt (LOK_INFRA), darf die CU-Seite die
                // gemeinsamen Datenbankfelder nicht beschreiben - sonst verliert der Gleisdecoder
                // seine eigene Aenderungserkennung. Der CU-Bus-Ablauf/Wake-Up bleibt unabhaengig
                // davon immer bestehen.

                if (!lok[LOK_INFRA])
                {
                    uint8_t funktionH = (twiRxd[twiRxdAktuell][4] & 0xf0) >> 4;
                    uint8_t funktionL =  twiRxd[twiRxdAktuell][4] & 0x0f;

                    uint8_t  mmSpeed  = funktionL;
                    uint16_t cs2Speed = (mmSpeed > 1)
                                      ? ((uint16_t) (mmSpeed - 1) * MM_SPEED_STEP) : 0;

                    if (mmSpeed != lok[LOK_SPEED])
                    {
                        msgLokSpeed[MBC_CAN_D0] = lok[LOK_ADDR4];
                        msgLokSpeed[MBC_CAN_D1] = lok[LOK_ADDR3];
                        msgLokSpeed[MBC_CAN_D2] = lok[LOK_ADDR2];
                        msgLokSpeed[MBC_CAN_D3] = lok[LOK_ADDR1];
                        msgLokSpeed[MBC_CAN_D4] = (uint8_t) ((cs2Speed & 0xff00) >> 8);
                        msgLokSpeed[MBC_CAN_D5] = (uint8_t) (cs2Speed & 0xff);
                        lok[LOK_SPEED] = mmSpeed;

                        if (lok[LOK_MAPPED]) cuOwner->sendCan(msgLokSpeed);
                    }

                    uint8_t mmF0 = lok[LOK_F0];

                    if (funktionH == 0x03) mmF0 = 1;
                    if (funktionH == 0x02) mmF0 = 0;

                    if (mmF0 != lok[LOK_F0])
                    {
                        msgLokFx[MBC_CAN_D0] = lok[LOK_ADDR4];
                        msgLokFx[MBC_CAN_D1] = lok[LOK_ADDR3];
                        msgLokFx[MBC_CAN_D2] = lok[LOK_ADDR2];
                        msgLokFx[MBC_CAN_D3] = lok[LOK_ADDR1];
                        msgLokFx[MBC_CAN_D4] = 0;
                        msgLokFx[MBC_CAN_D5] = mmF0;
                        lok[LOK_F0] = mmF0;

                        if (lok[LOK_MAPPED]) cuOwner->sendCan(msgLokFx);
                    }
                }

                // Wake-Up setzen zur Freigabe des I2C-Busses
                i2cSendCuFn(TWI_SEND_STARTSTOPCONTROL, twiRxd[twiRxdAktuell][2], 0);
            }
        }

        // Zentrale: Bestaetigung Lok-Richtung (frame[2]==0xfe, (frame[4]&0x0f)==0x01) - nur die
        // Zentrale sendet die tatsaechliche Richtungsinfo, das Control 80f fordert nur den Wechsel
        // an

        if ((twiRxd[twiRxdAktuell][0] == 3) && (twiRxd[twiRxdAktuell][2] == 0xfe)
                                          && ((twiRxd[twiRxdAktuell][4] & 0x0f) == 0x01))
        {
            uint8_t  mmLokAddr = twiRxd[twiRxdAktuell][3];
            uint8_t *lok = arr + MBC_89_S_LOKS + ((uint16_t) mmLokAddr * LOKS_ITEMS);

            // Bei INFRA-gefuehrter Adresse darf die CU-Seite die gemeinsamen Datenbankfelder
            // nicht beschreiben - siehe Lok-Speed/F0-Block oben. Diese Nachricht hat kein Wake-Up.

            if ((mmLokAddr < 80) && !lok[LOK_INFRA])
            {
                uint8_t funktionH = (twiRxd[twiRxdAktuell][4] & 0xf0) >> 4;
                uint8_t funktionL =  twiRxd[twiRxdAktuell][4] & 0x0f;
                uint8_t mmDir     = lok[LOK_DIR];

                if (((funktionH == 0x08) || (funktionH == 0x09)) && (funktionL == 0x01))
                    mmDir = DIR_RUECKWAERTS;

                if (((funktionH == 0x0a) || (funktionH == 0x0b)) && (funktionL == 0x01))
                    mmDir = DIR_VORWAERTS;

                if (mmDir != lok[LOK_DIR])
                {
                    msgLokDir[MBC_CAN_D0] = lok[LOK_ADDR4];
                    msgLokDir[MBC_CAN_D1] = lok[LOK_ADDR3];
                    msgLokDir[MBC_CAN_D2] = lok[LOK_ADDR2];
                    msgLokDir[MBC_CAN_D3] = lok[LOK_ADDR1];
                    msgLokDir[MBC_CAN_D4] = mmDir;
                    lok[LOK_DIR] = mmDir;

                    if (lok[LOK_MAPPED]) cuOwner->sendCan(msgLokDir);
                }
            }
        }

        // Control 80f: F1...F4-Nachricht (frame[2] in 0x40...0x54, frame[4]<0x10) - Format z.B.
        // sFEa42a18a00ap: Zentrale(FE), Control 80f F1...F4(42), Lokadresse(18), F-Taste/an-aus im
        // letzten Byte

        if ((twiRxd[twiRxdAktuell][0] == 3) && (twiRxd[twiRxdAktuell][2] > 0x3f)
                                          && (twiRxd[twiRxdAktuell][2] < 0x55)
                                          && (twiRxd[twiRxdAktuell][4] < 0x10))
        {
            uint8_t  mmLokAddr = twiRxd[twiRxdAktuell][3];
            uint8_t *lok = arr + MBC_89_S_LOKS + ((uint16_t) mmLokAddr * LOKS_ITEMS);

            msgLokFx[MBC_CAN_D0] = lok[LOK_ADDR4];
            msgLokFx[MBC_CAN_D1] = lok[LOK_ADDR3];
            msgLokFx[MBC_CAN_D2] = lok[LOK_ADDR2];
            msgLokFx[MBC_CAN_D3] = lok[LOK_ADDR1];

            // Bei INFRA-gefuehrter Adresse darf die CU-Seite die gemeinsamen Datenbankfelder
            // nicht beschreiben - siehe Lok-Speed/F0-Block oben. Wake-Up bleibt unabhaengig davon.

            if (mmLokAddr < 80)
            {
                if (!lok[LOK_INFRA])
                {
                    uint8_t mmFx = twiRxd[twiRxdAktuell][4] & 0x0f;
                    uint8_t f1 = (mmFx & 0x01) ? 1 : 0;
                    uint8_t f2 = (mmFx & 0x02) ? 1 : 0;
                    uint8_t f3 = (mmFx & 0x04) ? 1 : 0;
                    uint8_t f4 = (mmFx & 0x08) ? 1 : 0;

                    if (f1 != (lok[LOK_FX] & 0x01))
                    {
                        msgLokFx[MBC_CAN_D4] = 0x01;
                        msgLokFx[MBC_CAN_D5] = f1;
                        lok[LOK_FX] = (uint8_t) ((lok[LOK_FX] & 0xfe) | (f1 * 0x01));

                        if (lok[LOK_MAPPED]) cuOwner->sendCan(msgLokFx);
                    }

                    if (f2 != ((lok[LOK_FX] & 0x02) >> 1))
                    {
                        msgLokFx[MBC_CAN_D4] = 0x02;
                        msgLokFx[MBC_CAN_D5] = f2;
                        lok[LOK_FX] = (uint8_t) ((lok[LOK_FX] & 0xfd) | (f2 * 0x02));

                        if (lok[LOK_MAPPED]) cuOwner->sendCan(msgLokFx);
                    }

                    if (f3 != ((lok[LOK_FX] & 0x04) >> 2))
                    {
                        msgLokFx[MBC_CAN_D4] = 0x03;
                        msgLokFx[MBC_CAN_D5] = f3;
                        lok[LOK_FX] = (uint8_t) ((lok[LOK_FX] & 0xfb) | (f3 * 0x04));

                        if (lok[LOK_MAPPED]) cuOwner->sendCan(msgLokFx);
                    }

                    if (f4 != ((lok[LOK_FX] & 0x08) >> 3))
                    {
                        msgLokFx[MBC_CAN_D4] = 0x04;
                        msgLokFx[MBC_CAN_D5] = f4;
                        lok[LOK_FX] = (uint8_t) ((lok[LOK_FX] & 0xf7) | (f4 * 0x08));

                        if (lok[LOK_MAPPED]) cuOwner->sendCan(msgLokFx);
                    }
                }

                // Wake-Up setzen zur Freigabe des I2C-Busses
                i2cSendCuFn(TWI_SEND_STARTSTOPCONTROL, twiRxd[twiRxdAktuell][2] & 0xbf, 0);
            }
        }

        // Memory 6043: Fahrstrassen-Abschluss (frame[2]==frame[3]==0xfe, frame[4]==0x00, Format
        // sFEaFEaFEa00p) - warte bis der Bus frei ist, dann START/STOP zur Freigabe senden

        if ((twiRxd[twiRxdAktuell][0] == 3) && (twiRxd[twiRxdAktuell][2] == 0xfe)
                                          && (twiRxd[twiRxdAktuell][3] == 0xfe)
                                          && (twiRxd[twiRxdAktuell][4] == 0x00))
        {
            #if defined(__AVR__)
                // Erzwingt die Freigabe der TWI-Hardware-Pins (AVR-Eigenheit) - auf ESP32 ist
                // dieser Schritt ohne Entsprechung noetig, da die Software-I2C-Engine SCL/SDA im
                // Ruhezustand ohnehin als Eingang haelt (siehe i2cSoftMasterSend()-Kommentar zu
                // TWI_SEND_STARTSTOP)
                PORTC |= (uint8_t) (1 << BIT_CU_SCL);
                PORTC |= (uint8_t) (1 << BIT_CU_SDA);

                while ((PINC & (uint8_t) (1 << BIT_CU_SCL)) == 0
                    || (PINC & (uint8_t) (1 << BIT_CU_SDA)) == 0) {}
            #endif

            i2cSendCuFn(TWI_SEND_STARTSTOP, 0, 0);
        }

        // Keyboard 6040: Schaltbefehl (frame[0]==2, frame[2] in 0x20...0x3f) - Format z.B.
        // sFEa20a09ap: Zentrale(FE), Keyboard 6040(20), Tastenpaare(09) - Stellpultblock 1 von 4,
        // Taste GRUEN der ersten Taste des Stellpultblocks.

        static uint16_t mmKeyboardOld = 0;

        if ((twiRxd[twiRxdAktuell][0] == 2) && (twiRxd[twiRxdAktuell][2] > 0x1f)
                                          && (twiRxd[twiRxdAktuell][2] < 0x40))
        {
            // Antworte auf Anfrage wenn CU nicht da ist (Solobetrieb Keyboard/Memory)
            if (!cuIsPresent)
            {
                #if defined(__AVR__)
                    // Siehe Kommentar oben beim Memory-6043-Fahrstrassen-Abschluss - gleiche
                    // AVR-TWI-Pinfreigabe, auf ESP32 ohne Entsprechung noetig
                    PORTC |= (uint8_t) (1 << BIT_CU_SCL);
                    PORTC |= (uint8_t) (1 << BIT_CU_SDA);

                    while ((PINC & (uint8_t) (1 << BIT_CU_SCL)) == 0
                        || (PINC & (uint8_t) (1 << BIT_CU_SDA)) == 0) {}
                #endif

                i2cSendCuFn(TWI_SEND_KB, twiRxd[twiRxdAktuell][2], twiRxd[twiRxdAktuell][3]);
            }

            // Keyboardnummer, MM/DCC-Protokoll und CS2-Zubehoeradresse bestimmen

            uint16_t mmKeyboard = (uint16_t) (twiRxd[twiRxdAktuell][2] - 0x20) * 8UL;
            uint8_t  protoIndex = (uint8_t) (((twiRxd[twiRxdAktuell][2] - 0x20) * 2)
                                + ((twiRxd[twiRxdAktuell][3] & 0xf0) >> 4));

            mmKeyboard += (arr[MBC_89_S_MAGNET + protoIndex] == 1) ? 0x3800 : 0x3000;

            // Stellpultnummer/-taste bestimmen, Richtung (Rot/Gruen) zuweisen

            uint16_t mmBlock = (uint16_t) (twiRxd[twiRxdAktuell][3] & 0xf0) / 4UL;
            uint8_t  mmTaste = twiRxd[twiRxdAktuell][3] & 0x0f;

            // static: behaelt bei mmTaste 0...7 (Taste losgelassen) bewusst den Wert des zuletzt
            // gedrueckten Tasters (Originalverhalten - can_msg[CAN_MSG_CS2_0x16][D4] war dort ein
            // persistentes globales Array-Element ohne default-Fall im switch)
            static uint8_t richtung = SWITCH_ROT;

            switch (mmTaste)
            {
                case  8: richtung = SWITCH_ROT;   break;
                case  9: richtung = SWITCH_GRUEN; break;
                case 10: mmBlock += 1; richtung = SWITCH_ROT;   break;
                case 11: mmBlock += 1; richtung = SWITCH_GRUEN; break;
                case 12: mmBlock += 2; richtung = SWITCH_ROT;   break;
                case 13: mmBlock += 2; richtung = SWITCH_GRUEN; break;
                case 14: mmBlock += 3; richtung = SWITCH_ROT;   break;
                case 15: mmBlock += 3; richtung = SWITCH_GRUEN; break;
            }

            // Bei Taste (>7): CS2-Zubehoeradresse berechnen (inkl. Basis 1 aus 4, da 4 mbc-89 fuer
            // alle 1024 Adressen moeglich sind), Strom an, Adresse merken. Sonst (Taste
            // losgelassen): zuletzt gemerkte Adresse mit Strom aus senden.

            if (mmTaste > 7)
            {
                mmKeyboard += mmBlock + ((uint16_t) (arr[MBC_89_S_BASIS] - 1) * 256UL);
                mmKeyboardOld = mmKeyboard;

                cuOwner->sendZubehoerSchalten(mmKeyboard, richtung, true);
            }
            else
            {
                cuOwner->sendZubehoerSchalten(mmKeyboardOld, richtung, false);
            }
        }

        // Ringspeicher-Nachricht bereinigen und Lesezeiger weiterschalten

        for (uint8_t index = 0; index < TWI_RX_FRAME; index++)
        {
            twiRxd[twiRxdAktuell][index] = 0;
        }

        if (++twiRxdAktuell == TWI_RX_MAX) twiRxdAktuell = 0;

        return 1;
    }

//*************************************************************************************************
//* Sende anliegende CSx/MS2-Nachrichten zum Keyboard an die CU - 1:1 aus mbc_cu_send_kb()        *
//* uebernommen. Wird sowohl aus update() (CAN-ID 0x16, Original-Weiterleitung) als auch aus dem  *
//* Gleisdecoder (Magnetartikel-Spiegelung auf den I2C-Bus) aufgerufen.                           *
//*************************************************************************************************
//*************************************************************************************************
//* msg: CAN-Nachricht an Keyboard (D2/D3=CS2-Adresse, D4=Spulennummer, D5=Strom an/aus)          *
//*************************************************************************************************
//*************************************************************************************************
//* Return: 0, wenn cuIsInit noch nicht gesetzt ist, sonst 1                                      *
//*************************************************************************************************

    static uint8_t cuSendKbFn(const uint8_t msg[MBC_CAN_MSG_LEN])
    {
        if (!cuIsInit) return 0;

        uint16_t mmKeyboard = ((uint16_t) msg[MBC_CAN_D2] << 8) | (uint16_t) msg[MBC_CAN_D3];

        // Bei MM Basisadresse 0x3000 abziehen, bei DCC 0x3800
        mmKeyboard -= (mmKeyboard > 0x37ff) ? 0x3800 : 0x3000;

        uint8_t mmBasis = (uint8_t) (((mmKeyboard & 0xf00) >> 8) + 1);

        // Pruefe, ob die Adresse des Keyboards zur eingestellten Basis 1 aus 4 passt
        if (mmBasis == cuUtils->array()[MBC_89_S_BASIS])
        {
            uint8_t mmKb = (uint8_t) (((mmKeyboard & 0xf0) >> 3) + 0x20);
            uint8_t mmTaste;

            if (msg[MBC_CAN_D5])
            {
                uint8_t mmBlock    = (uint8_t) ((mmKeyboard & 0x000C) << 2);
                uint8_t mmTasteBit = (uint8_t) ((mmKeyboard & 0x0003) << 1);

                mmTaste = (uint8_t) (((mmBlock | mmTasteBit) + 8) | msg[MBC_CAN_D4]);
            }
            else
            {
                mmTaste = 0;
            }

            if (mmKb < 0x3f) i2cSendCuFn(TWI_SEND_KB, mmKb, mmTaste);
        }

        return 1;
    }

//*************************************************************************************************
//* Loesche alle Daten der CU ("Alles Null") - 1:1 aus mbc_cu_reset() uebernommen, inklusive der  *
//* blockierenden 500-ms-Pause zwischen den beiden Telegrammen.                                   *
//*************************************************************************************************

    static void cuResetFn()
    {
        i2cSendCuFn(TWI_SEND_ALLESNULL, 91, 0);
        delay(500);
        i2cSendCuFn(TWI_SEND_ALLESNULL, 93, 0);
    }

//*************************************************************************************************
//* CU-Anwesenheit/I2C-Nachrichten pruefen - 1:1 aus mbc_cu_present() uebernommen.                *
//*************************************************************************************************

    static void cuPresentCheckFn()
    {
        cuI2cCheckFn();

        if (cuIsPresent) cuTastenCheckFn();

        #if defined(__AVR__)
            cuIsPresent = (PINC & (uint8_t) (1 << BIT_CU_EXT)) != 0;
        #else
            cuIsPresent = cuExtPtr->read();
        #endif
    }

#if defined(__AVR__)

//*************************************************************************************************
//* Interruptroutine fuer den I2C-Bus im SLAVE- UND MASTER-Modus - 1:1 aus SIGNAL(TWI_vect) in    *
//* mbc_cu.c uebernommen (nur Bezeichner umbenannt). Rein registerbasierte AVR-TWI-               *
//* Zustandsmaschine, siehe Klassenkommentar in Mbc89Com.h.                                       *
//*************************************************************************************************

    ISR(TWI_vect)
    {
        static uint8_t frameByteIndexRx = 0x01;
        static uint8_t frameByteIndexTx = 0x01;
        static uint8_t resStart = 0;

        uint8_t status = TWSR & 0xF8;

        switch (status)
        {
            case TWI_BUS_ERROR:

                if (twiMode == TWI_MASTER)
                {
                    TWCR = 0;
                    resStart = 0;
                    TWCR = TWCR_RESTART_MASTER;
                }
                else
                {
                    resStart = 0;
                    TWCR = TWCR_RELEASE_SLAVE;
                }
            break;

            case TWI_ARB_LOST:
                TWCR = 0;
                resStart = 0;
                TWCR = TWCR_RESTART_MASTER;
            break;

            case TWI_START:
            case TWI_REP_START:

                if (resStart == 0) frameByteIndexTx = 1;

                if (twiTxd[0] == 0)
                {
                    TWCR = TWCR_RELEASE_SLAVE;
                    resStart = 0;
                    twiMode = TWI_SLAVE;
                    break;
                }

                // fall through - 1:1 aus mbc_cu.c uebernommen: ohne Break geht es direkt in die
                // Sendelogik von TWI_MTX_ADR_ACK/TWI_MTX_DATA_ACK weiter

            case TWI_MTX_ADR_ACK:
            case TWI_MTX_DATA_ACK:

                if (frameByteIndexTx < (twiTxd[0] + 1))
                {
                    TWDR = twiTxd[frameByteIndexTx++];
                    TWCR = TWCR_CONTINUE_NOACK;
                }
                else
                {
                    if (resStart == 0)
                    {
                        TWCR = 0;
                        TWCR = TWCR_RESTART_MASTER;
                        resStart = 1;
                    }
                    else
                    {
                        TWCR = TWCR_RELEASE_SLAVE;
                        resStart = 0;
                        twiMode = TWI_SLAVE;
                    }
                }
            break;

            case TWI_MTX_ADR_NACK:
            case TWI_MTX_DATA_NACK:

                frameByteIndexTx = (uint8_t) (twiTxd[0] + 1);
                TWCR = 0;
                TWCR = TWCR_RESTART_MASTER;
                resStart = 1;
            break;

            case TWI_SRX_ADR_ACK:
            case TWI_SRX_ADR_ACK_M_ARB_LOST:

                frameByteIndexRx = 2;
                twiRxd[twiRxdInterrupt][1] = TWAR;
                TWCR = TWCR_CONTINUE_ACK;
            break;

            case TWI_SRX_ADR_DATA_ACK:
            case TWI_SRX_ADR_DATA_NACK:

                twiRxd[twiRxdInterrupt][frameByteIndexRx] = TWDR;

                if (frameByteIndexRx == 4)
                {
                    if ((twiRxd[twiRxdInterrupt][2] < 0x20)
                        && ((twiRxd[twiRxdInterrupt][4] & 0x0f) == 0x01))
                    {
                        TWAR = twiRxd[twiRxdInterrupt][2];
                    }
                    else
                    {
                        TWAR = 0xfe;
                    }

                    twiRxd[twiRxdInterrupt][0] = 3;

                    if (++twiRxdInterrupt == TWI_RX_MAX) twiRxdInterrupt = 0;

                    frameByteIndexRx = 2;
                }

                if ((frameByteIndexRx == 3) && (twiRxd[twiRxdInterrupt][2] > 0x1f)
                                            && (twiRxd[twiRxdInterrupt][2] < 0x40))
                {
                    twiRxd[twiRxdInterrupt][0] = 2;
                    TWAR = 0xfe;

                    if (++twiRxdInterrupt == TWI_RX_MAX) twiRxdInterrupt = 0;

                    frameByteIndexRx = 2;
                }

                if (++frameByteIndexRx == TWI_RX_FRAME) frameByteIndexRx = TWI_RX_FRAME - 1;

                TWCR = TWCR_CONTINUE_ACK;
            break;

            case TWI_SRX_STOP_RESTART:
                TWCR = TWCR_CONTINUE_ACK;
            break;

            default:
                TWCR = TWCR_CONTINUE_ACK;
        }
    }

    #else

//------ Start Mbc89I2cSoft (Software-I2C-Engine, ESP32-C3) ---------------------------------------

    //*************************************************************************************************
    //* Mbc89I2cSoft ersetzt die AVR-TWI-Hardware (TWCR/TWAR/TWBR/TWDR, ISR(TWI_vect)) komplett durch *
    //* eine selbstgeschriebene, GPIO-interrupt-basierte I2C-SLAVE- und MASTER-Engine. Grund: weder   *
    //* die Arduino-Wire-Bibliothek noch der ESP-IDF-i2c_slave-Treiber unterstuetzen den              *
    //* Laufzeit-Adresswechsel mitten in einer Uebertragung, den das Original-Protokoll braucht (die  *
    //* Adresse ist hier nur eine simple volatile Variable, kein Hardwareregister). Alle Frames dieses*
    //* Maerklin-CU-Bus-Protokolls sind reine I2C-WRITE-Transaktionen (nie ein Lesevorgang/Repeated-  *
    //* Start-Read) - dadurch muss die Engine nur SLAVE-Empfang und MASTER-Senden beherrschen, kein   *
    //* Slave-Senden. Die Engine bildet exakt dieselbe twiRxd[]/twiTxd[]/twiRxdInterrupt-Schnittstelle*
    //* wie die AVR-ISR nach - cuI2cCheckFn() und die gesamte Protokolllogik bleiben dadurch          *
    //* unveraendert wiederverwendbar. WICHTIG: reine Software-Loesung ohne Clock-Stretching-         *
    //* Unterstuetzung fuer andere Bus-Teilnehmer und ohne Multi-Master-Arbitrierung waehrend eigener *
    //* Master-Sendungen - beides in der Praxis mit einem Logic-Analyzer verifizieren (siehe Plan/    *
    //* README).                                                                                      *
    //*************************************************************************************************

        static uint8_t           softPinScl, softPinSda;
        static volatile uint32_t softHalfPeriodUs   = 20;   // Bit-Halbperiode fuer Master-TX
        static volatile uint8_t  softOwnAddr        = 0xfe;
        static volatile bool     softFrameActive    = false;
        static volatile bool     softAddrPhase      = true;
        static volatile uint8_t  softBitCount       = 0;    // 0...7 Datenbits, 8/9 = ACK-Takt
        static volatile uint8_t  softShiftIn         = 0;
        static volatile uint8_t  softFrameIdx        = 2;   // Position in twiRxd[cur][]

        //*********************************************************************************************
        //* Ein vollstaendig eingelesenes Byte auswerten - Adressbyte (Vergleich gegen softOwnAddr,   *
        //* R/W-Bit wird ignoriert, dieses Protokoll kennt nur WRITE-Transaktionen) oder Datenbyte     *
        //* (Framelaengen-Logik 1:1 aus ISR(TWI_vect) uebernommen: 2-Byte-Nutzlast fuer Keyboard/      *
        //* Magnetartikel-Absender 0x20...0x3f, sonst 3-Byte-Nutzlast).                                *
        //*********************************************************************************************

        static void IRAM_ATTR softProcessByte()
        {
            if (softAddrPhase)
            {
                uint8_t addr7 = (uint8_t) (softShiftIn >> 1);

                if (addr7 == softOwnAddr)
                {
                    twiRxd[twiRxdInterrupt][1] = softOwnAddr;
                    softFrameIdx  = 2;
                    softAddrPhase = false;

                    pinMode(softPinSda, OUTPUT);
                    digitalWrite(softPinSda, LOW);   // ACK
                }

                // Adresse passt nicht: kein ACK, restliche Bytes dieser Transaktion werden bis zum
                // naechsten START/STOP ignoriert (softAddrPhase bleibt true)

                return;
            }

            twiRxd[twiRxdInterrupt][softFrameIdx] = softShiftIn;

            pinMode(softPinSda, OUTPUT);
            digitalWrite(softPinSda, LOW);   // dieses Protokoll erwartet immer ACK auf Datenbytes

            if (softFrameIdx == 3)
            {
                bool kurzesFrame = (twiRxd[twiRxdInterrupt][2] > 0x1f)
                                 && (twiRxd[twiRxdInterrupt][2] < 0x40);

                if (kurzesFrame)
                {
                    twiRxd[twiRxdInterrupt][0] = 2;
                    if (++twiRxdInterrupt == TWI_RX_MAX) twiRxdInterrupt = 0;
                    softFrameIdx = 2;
                    return;
                }
            }

            if (softFrameIdx == 4)
            {
                twiRxd[twiRxdInterrupt][0] = 3;
                if (++twiRxdInterrupt == TWI_RX_MAX) twiRxdInterrupt = 0;
                softFrameIdx = 2;
                return;
            }

            softFrameIdx++;
        }

        //*********************************************************************************************
        //* GPIO-ISR auf SCL (CHANGE) - steigende Flanke: Bit einlesen; fallende Flanke: nach dem 8.   *
        //* Bit das Byte auswerten/ACK vorbereiten (softProcessByte()), nach dem 9. Takt (ACK-Slot)    *
        //* SDA wieder freigeben.                                                                      *
        //*********************************************************************************************

        static void IRAM_ATTR softSclIsr()
        {
            if (!softFrameActive) return;

            if (digitalRead(softPinScl))
            {
                if (softBitCount < 8)
                {
                    softShiftIn = (uint8_t) ((softShiftIn << 1) | (digitalRead(softPinSda) ? 1 : 0));
                    softBitCount++;
                }
            }
            else
            {
                if (softBitCount == 8)
                {
                    softProcessByte();
                    softBitCount = 9;
                }
                else if (softBitCount == 9)
                {
                    pinMode(softPinSda, INPUT);
                    softBitCount = 0;
                }
            }
        }

        //*********************************************************************************************
        //* GPIO-ISR auf SDA (CHANGE) - nur relevant, wenn SCL gerade High ist (sonst normaler         *
        //* Bitwechsel waehrend SCL=Low): SDA faellt = START, SDA steigt = STOP.                        *
        //*********************************************************************************************

        static void IRAM_ATTR softSdaIsr()
        {
            if (!digitalRead(softPinScl)) return;

            if (!digitalRead(softPinSda))
            {
                softFrameActive = true;
                softAddrPhase   = true;
                softBitCount    = 0;
                softShiftIn     = 0;
            }
            else
            {
                softFrameActive = false;
            }

            pinMode(softPinSda, INPUT);
        }

        //*********************************************************************************************
        //* Software-I2C-Engine initialisieren (Ersatz fuer TWBR/TWAR/TWDR/TWCR-Initialisierung in    *
        //* cuInitFn()).                                                                               *
        //*********************************************************************************************
        //* pinScl/pinSda: GPIOs des CU-Bus (externe Pull-Ups vorausgesetzt, wie beim AVR-Original)    *
        //* i2cKhz:        I2C-Frequenz aus dem Systemarray (arr[MBC_89_S_I2C])                        *
        //*********************************************************************************************

        static void i2cSoftInit(uint8_t pinScl, uint8_t pinSda, uint8_t i2cKhz)
        {
            softPinScl = pinScl;
            softPinSda = pinSda;

            softHalfPeriodUs = (i2cKhz > 0) ? (500UL / i2cKhz) : 20UL;
            if (softHalfPeriodUs < I2C_SOFT_MIN_HALF_PERIOD_US)
                softHalfPeriodUs = I2C_SOFT_MIN_HALF_PERIOD_US;

            pinMode(pinScl, INPUT);
            pinMode(pinSda, INPUT);

            attachInterrupt(digitalPinToInterrupt(pinScl), softSclIsr, CHANGE);
            attachInterrupt(digitalPinToInterrupt(pinSda), softSdaIsr, CHANGE);
        }

        //*********************************************************************************************
        //* Eigene Empfangsadresse aendern - Ersatz fuer "TWAR = x" (hier nur eine Variable, kein      *
        //* Hardwareregister, daher ohne Zeitstrafe zur Laufzeit beliebig oft aenderbar).              *
        //*********************************************************************************************

        static void i2cSoftSetOwnAddr(uint8_t addr)
        {
            softOwnAddr = addr;
        }

        //*********************************************************************************************
        //* Sende den Inhalt von twiTxd[] als I2C-MASTER-Schreibtransaktion (Empfaenger=twiTxd[1],    *
        //* restliche Bytes=Datenbytes) - reines Bit-Banging mit blockierendem Timing                 *
        //* (delayMicroseconds()), analog zum blockierenden "while(twiMode==TWI_MASTER){}" auf AVR.   *
        //* Deaktiviert waehrend der Sendung die eigenen Slave-ISRs (sonst wuerden die selbst          *
        //* erzeugten SCL/SDA-Flanken faelschlich als fremder Bus-Traffic interpretiert).              *
        //*********************************************************************************************

        static void i2cSoftMasterSend()
        {
            uint8_t len = twiTxd[0];

            if (len == 0) return;   // TWI_SEND_STARTSTOP - auf ESP32 ohne Wirkung, siehe
                                     // Klassenkommentar zu i2cSendCuFn()

            uint8_t bytes[TWI_TX_FRAME];

            bytes[0] = (uint8_t) (twiTxd[1] << 1);   // Empfaenger-Adresse, R/W=0 (Write)

            for (uint8_t i = 2; i <= len; i++) bytes[i - 1] = twiTxd[i];

            uint8_t byteCount = len;

            detachInterrupt(digitalPinToInterrupt(softPinScl));
            detachInterrupt(digitalPinToInterrupt(softPinSda));

            pinMode(softPinSda, OUTPUT);
            digitalWrite(softPinSda, LOW);          // START
            delayMicroseconds(softHalfPeriodUs);
            pinMode(softPinScl, OUTPUT);
            digitalWrite(softPinScl, LOW);

            bool ackOk = true;

            for (uint8_t b = 0; (b < byteCount) && ackOk; b++)
            {
                for (int8_t bit = 7; bit >= 0; bit--)
                {
                    digitalWrite(softPinSda, (bytes[b] >> bit) & 0x01);
                    delayMicroseconds(softHalfPeriodUs);
                    digitalWrite(softPinScl, HIGH);
                    delayMicroseconds(softHalfPeriodUs);
                    digitalWrite(softPinScl, LOW);
                }

                pinMode(softPinSda, INPUT);
                delayMicroseconds(softHalfPeriodUs);
                digitalWrite(softPinScl, HIGH);
                ackOk = !digitalRead(softPinSda);
                delayMicroseconds(softHalfPeriodUs);
                digitalWrite(softPinScl, LOW);
                pinMode(softPinSda, OUTPUT);
            }

            // STOP
            digitalWrite(softPinSda, LOW);
            delayMicroseconds(softHalfPeriodUs);
            digitalWrite(softPinScl, HIGH);
            delayMicroseconds(softHalfPeriodUs);
            digitalWrite(softPinSda, HIGH);

            pinMode(softPinSda, INPUT);
            pinMode(softPinScl, INPUT);

            attachInterrupt(digitalPinToInterrupt(softPinScl), softSclIsr, CHANGE);
            attachInterrupt(digitalPinToInterrupt(softPinSda), softSdaIsr, CHANGE);
        }

//------ Ende Mbc89I2cSoft -------------------------------------------------------------------------

    #endif

//*************************************************************************************************
//* Interruptroutine fuer das Tastentiming (STOPP/GO/RESET-Simulation, NUR AVR) - 1:1 aus         *
//* SIGNAL(TIMER2_COMPA_vect) in mbc_cu.c uebernommen. ESP32-Aequivalent siehe                    *
//* cuTastenReleaseCallback() weiter oben.                                                        *
//*************************************************************************************************

    #if defined(__AVR__)

    ISR(TIMER2_COMPA_vect)
    {
        static uint16_t outputPulseCount = 0;
        uint8_t release = 0;

        outputPulseCount++;

        // Pruefe, ob 100 ms bei GO/STOPP-Taste bzw. 1,5 s bei RESET abgelaufen sind

        if ((outputPulseCount == 100)
            && (cuGo != RESET_CU_STEP_1) && (cuGo != RESET_CU_STEP_2)) release = 1;

        if ((outputPulseCount == 1500)
            && ((cuGo == RESET_CU_STEP_1) || (cuGo == RESET_CU_STEP_2))) release = 1;

        if (release)
        {
            PORTC |= (uint8_t) (1 << BIT_CU_GO_OUT);
            PORTC |= (uint8_t) (1 << BIT_CU_STOP_OUT);

            if (cuGo == RESET_CU_STEP_1)
            {
                outputPulseCount = 0;
                PORTC &= (uint8_t) ~(1 << BIT_CU_STOP_OUT);
                cuGo = RESET_CU_STEP_2;
            }
            else if (cuGo == RESET_CU_STEP_2)
            {
                TIMSK2 &= (uint8_t) ~(1 << OCIE2A);
                outputPulseCount = 0;
                PORTC |= (uint8_t) (1 << BIT_CU_STOP_OUT);
                cuGo = 0;
                cuTasteOn = false;
                cuIsReset = true;
            }
            else
            {
                TIMSK2 &= (uint8_t) ~(1 << OCIE2A);
                outputPulseCount = 0;
                cuTasteOn = false;
            }
        }
    }

    #endif

//------ Ende CU-Bus (I2C) Subsystem --------------------------------------------------------------

//------ Start Gleisdecoder-Subsystem -------------------------------------------------------------

//*************************************************************************************************
//* Initialisiere INT0/Timer1 zur Kommunikation mit dem Gleis - 1:1 aus mbc_track_init()          *
//* uebernommen. Timer1 auf 0,5 us/Tick bei echten 16 MHz (Prescaler /8) fuer die Pegel-          *
//* Zeitdauermessung ueber INT0, Interrupt zunaechst auf fallende Flanke, PULL-UP am TRACK-Eingang*
//* bewusst ausgeschaltet.                                                                        *
//*************************************************************************************************

    #if defined(__AVR__)

        static void trackInitFn()
        {
            TCCR1A  = 0x00;
            TCCR1B  = (1 << CS11);
            TCNT1   = 0;
            TIMSK1 |= (uint8_t) (1 << TOIE1);

            PORTD &= (uint8_t) ~(1 << BIT_TRACK);
            DDRD  &= (uint8_t) ~(1 << BIT_TRACK);
            EICRA  = (1 << ISC01);
            EIMSK  = (1 << INT0);

            infraIsPresent = false;
        }

    #else

        //*****************************************************************************************
        //* Ersetzt trackInitFn()/ISR(INT0_vect)/ISR(TIMER1_OVF_vect) - die RMT-Peripherie erfasst *
        //* Pulslaengen in Hardware (immun gegen Interrupt-Jitter) statt per Register-ISR.          *
        //* Aufloesung 2 MHz/0,5 us pro RMT-Tick - exakt dieselbe Tick-Rate wie Timer1 auf AVR, die  *
        //* bestehenden MAGNET_*/LOK_*-Schwellwerte gelten daher unveraendert weiter. Nutzt die neue *
        //* driver/rmt_rx.h-API (Arduino-ESP32-Core 3.x/IDF5) - Empfang laeuft dort Callback-/       *
        //* Job-basiert statt ueber den treiberinternen Ringbuffer der alten Core-2.x/IDF4-API.      *
        //* Der Callback laeuft in ISR-Kontext und schreibt deshalb NICHT direkt in                  *
        //* cmdBLevel[]/cmdBLength[] (die in trackCheckFn() unsynchronisiert aus Hauptschleifen-     *
        //* kontext gelesen werden), sondern in einen selbst angelegten FreeRTOS-Ringbuffer - genau  *
        //* das Verhalten, das der alte Treiber vorher intern selbst geliefert hat. Empfangene Items *
        //* werden weiterhin gepollt in trackRmtDrain() (aufgerufen aus trackCheckFn()) in           *
        //* cmdBLevel[]/cmdBLength[] uebertragen.                                                    *
        //*****************************************************************************************

        static rmt_channel_handle_t trackRmtChan    = nullptr;
        static RingbufHandle_t      trackRmtRingbuf = nullptr;
        static rmt_symbol_word_t    trackRmtBuf[64];

        static const rmt_receive_config_t trackRmtRxConfig =
        {
            5000,       // signal_range_min_ns - Glitches < 5 us unterdruecken (wie alt: 10 Ticks x 0,5 us)
            30000000    // signal_range_max_ns - Idle-Schwelle 30 ms (wie alt: 60000 Ticks x 0,5 us)
        };

        //*****************************************************************************************
        //* on_recv_done-Callback (ISR-Kontext) - kopiert die empfangenen Symbole in den eigenen    *
        //* Ringbuffer und stoesst sofort den naechsten Empfangsjob an (laut IDF-Dokumentation aus   *
        //* ISR-Kontext zulaessig), da die neue API - anders als die alte - nach jeder Idle-Pause    *
        //* neu bewaffnet werden muss.                                                               *
        //*****************************************************************************************

        static bool trackRmtRxDone(rmt_channel_handle_t chan, const rmt_rx_done_event_data_t *edata,
                                    void *ctx)
        {
            (void) chan;
            (void) ctx;

            BaseType_t woken = pdFALSE;

            xRingbufferSendFromISR(trackRmtRingbuf, edata->received_symbols,
                                    edata->num_symbols * sizeof(rmt_symbol_word_t), &woken);

            rmt_receive(trackRmtChan, trackRmtBuf, sizeof(trackRmtBuf), &trackRmtRxConfig);

            return woken == pdTRUE;
        }

        static void trackInitFn()
        {
            infraIsPresent = false;

            trackRmtRingbuf = xRingbufferCreate(2000, RINGBUF_TYPE_NOSPLIT);
            if (trackRmtRingbuf == nullptr) return;

            rmt_rx_channel_config_t chanConfig = {};

            chanConfig.gpio_num          = (gpio_num_t) cuPinTrack;
            chanConfig.clk_src           = RMT_CLK_SRC_DEFAULT;
            chanConfig.resolution_hz     = 2000000;   // 2 MHz -> 0,5 us/Tick, wie Timer1 auf AVR
            chanConfig.mem_block_symbols = 64;

            if (rmt_new_rx_channel(&chanConfig, &trackRmtChan) != ESP_OK) { trackRmtChan = nullptr; return; }

            rmt_rx_event_callbacks_t cbs = {};
            cbs.on_recv_done = trackRmtRxDone;

            if (rmt_rx_register_event_callbacks(trackRmtChan, &cbs, nullptr) != ESP_OK) return;
            if (rmt_enable(trackRmtChan) != ESP_OK) return;

            rmt_receive(trackRmtChan, trackRmtBuf, sizeof(trackRmtBuf), &trackRmtRxConfig);
        }

        //*****************************************************************************************
        //* Aus dem (eigenen, siehe oben) RMT-Ringbuffer verfuegbare Flankenwechsel in             *
        //* cmdBLevel[]/cmdBLength[] uebertragen (nicht-blockierend, xRingbufferReceive() mit       *
        //* Timeout=0) - muss regelmaessig aufgerufen werden (siehe trackCheckFn()).                *
        //*****************************************************************************************

        static void trackRmtDrain()
        {
            if (trackRmtRingbuf == nullptr) return;

            size_t rxSize = 0;
            rmt_symbol_word_t *items = (rmt_symbol_word_t *) xRingbufferReceive(trackRmtRingbuf, &rxSize, 0);

            if (items == nullptr) return;

            static uint8_t cmdPWrite = 0;
            size_t itemCount = rxSize / sizeof(rmt_symbol_word_t);

            for (size_t i = 0; (i < itemCount) && (cmdNr < SIGNALPUFFER); i++)
            {
                cmdBLevel[cmdPWrite]  = items[i].level0;
                cmdBLength[cmdPWrite] = items[i].duration0;
                if (++cmdPWrite == SIGNALPUFFER) cmdPWrite = 0;
                cmdNr++;

                if ((items[i].duration1 > 0) && (cmdNr < SIGNALPUFFER))
                {
                    cmdBLevel[cmdPWrite]  = items[i].level1;
                    cmdBLength[cmdPWrite] = items[i].duration1;
                    if (++cmdPWrite == SIGNALPUFFER) cmdPWrite = 0;
                    cmdNr++;
                }
            }

            vRingbufferReturnItem(trackRmtRingbuf, (void *) items);
        }

    #endif

//*************************************************************************************************
//* Automat fuer die Magnetartikelbefehlserkennung - 1:1 aus mbc_automat_magnet() uebernommen.    *
//* Nach 9 Trits (cmd_bit==0x40000) liegt eine gueltige Kette vor; da immer ein Doppelkommando    *
//* gesendet wird, muss der erste und zweite Teil des Befehls identisch sein. TRIT5 ist bei       *
//* Weichenbefehlen immer Null (Filter cmd&0x300==0).                                             *
//*************************************************************************************************
//*************************************************************************************************
//* cmdLevel:  PIN-Level HIGH/LOW (im Original wie hier ungenutzt, nur Signaturkompatibilitaet)   *
//* cmdValue:  Signallaengentyp (TRI0PART1/TRI0PART2/TRI1PART1/TRI1PART2/TRIEND/0)                *
//* cmdBefehl: Befehlsspeicher fuer die Magnetartikeldaten                                        *
//*************************************************************************************************
//*************************************************************************************************
//* Return: 1 bei vollstaendigem, verifiziertem Befehl, sonst 0                                   *
//*************************************************************************************************

    static uint8_t automatMagnetFn(uint8_t cmdLevel, uint8_t cmdValue, uint32_t *cmdBefehl)
    {
        (void) cmdLevel;

        static uint8_t  cmdStep    = 0;
        static uint8_t  cmdTrinaer = 0;
        static uint32_t cmd        = 0UL;
        static uint32_t cmdBit     = 1UL;
        static uint8_t  cmdCount   = 0;
        static uint32_t cmdPrev    = 0UL;
        uint8_t         cmdFlag    = 0;
        uint8_t         cmdError   = 0;

        if ((cmdBit == 0x20000UL) && (cmdStep == 3)) cmdValue = TRIEND;

        if (cmdValue > 0)
        {
            switch (cmdStep)
            {
                case 0:
                    if (cmdValue == TRI0PART1) { cmdTrinaer = TRI0; cmdStep++; }
                    else if (cmdValue == TRI1PART1) { cmdTrinaer = TRI1; cmdStep++; }
                    else cmdError = 1;
                break;

                case 1:
                    if ((cmdValue == TRI0PART2) && (cmdTrinaer == TRI0))
                    {
                        cmdStep++;
                        cmdBit *= 2UL;
                    }
                    else if ((cmdValue == TRI1PART2) && (cmdTrinaer == TRI1))
                    {
                        cmdStep++;
                        cmd |= cmdBit;
                        cmdBit *= 2UL;
                    }
                    else cmdError = 1;
                break;

                    // Zweiter Teil Part 1 - Trinaer-Art kann sich aendern (Z-Zustand), dann auf
                    // TRINAER0 setzen
                case 2:
                    if ((cmdValue == TRI0PART1) && (cmdTrinaer == TRI0)) cmdStep++;
                    else if ((cmdValue == TRI1PART1) && (cmdTrinaer == TRI1)) cmdStep++;
                    else if ((cmdValue == TRI0PART1) && (cmdTrinaer == TRI1))
                    {
                        cmdStep++;
                        cmdTrinaer = TRI0;
                    }
                    else cmdError = 1;
                break;

                case 3:
                    if (cmdValue == TRIEND)
                    {
                        if (cmdTrinaer == TRI1) cmd |= cmdBit;
                        cmdStep = 0;
                        cmdTrinaer = 0;
                        cmdBit *= 2UL;
                    }
                    else if ((cmdValue == TRI0PART2) && (cmdTrinaer == TRI0))
                    {
                        cmdStep = 0;
                        cmdTrinaer = 0;
                        cmdBit *= 2UL;
                    }
                    else if ((cmdValue == TRI1PART2) && (cmdTrinaer == TRI1))
                    {
                        cmdStep = 0;
                        cmdTrinaer = 0;
                        cmd |= cmdBit;
                        cmdBit *= 2UL;
                    }
                    else cmdError = 1;
                break;
            }
        }
        else
        {
            cmdError = 1;
        }

        if (cmdError)
        {
            cmdStep = 0;
            cmdTrinaer = 0;
            cmdBit = 1UL;
            cmdCount = 0;
            cmd = 0UL;
            cmdPrev = 0UL;
        }

        if (cmdBit == 0x40000UL)
        {
            if ((cmd & 0x300UL) == 0x0UL)
            {
                if (cmdCount == 0)
                {
                    cmdPrev = cmd;
                    cmdCount++;
                }
                else
                {
                    if (cmd == cmdPrev)
                    {
                        *cmdBefehl = cmd;
                        cmdFlag = 1;
                    }

                    cmdCount = 0;
                    cmdPrev = 0UL;
                }
            }

            cmdBit = 1UL;
            cmd = 0UL;
        }

        return cmdFlag;
    }

//*************************************************************************************************
//* Automat fuer die Lokomotivbefehlserkennung - 1:1 aus mbc_automat_lok() uebernommen.           *
//* Strukturell identisch zu automatMagnetFn(), aber ohne den TRIT5-Filter (jede verifizierte     *
//* Doppelkette ist ein gueltiger Lokbefehl) und mit einer zusaetzlichen Trinaer-Wechsel-Variante *
//* in Schritt 2 (TRI1PART1 bei TRI0 -> TRINAERZ1, gilt fuer den erweiterten MM2-Modus).          *
//*************************************************************************************************
//*************************************************************************************************
//* cmdLevel:  PIN-Level HIGH/LOW (im Original wie hier ungenutzt, nur Signaturkompatibilitaet)   *
//* cmdValue:  Signallaengentyp (TRI0PART1/TRI0PART2/TRI1PART1/TRI1PART2/TRIEND/0)                *
//* cmdBefehl: Befehlsspeicher fuer die Lokomotivdaten                                            *
//*************************************************************************************************
//*************************************************************************************************
//* Return: 1 bei vollstaendigem, verifiziertem Befehl, sonst 0                                   *
//*************************************************************************************************

    static uint8_t automatLokFn(uint8_t cmdLevel, uint8_t cmdValue, uint32_t *cmdBefehl)
    {
        (void) cmdLevel;

        static uint8_t  cmdStep    = 0;
        static uint8_t  cmdTrinaer = 0;
        static uint32_t cmd        = 0UL;
        static uint32_t cmdBit     = 1UL;
        static uint8_t  cmdCount   = 0;
        static uint32_t cmdPrev    = 0UL;
        uint8_t         cmdFlag    = 0;
        uint8_t         cmdError   = 0;

        if ((cmdBit == 0x20000UL) && (cmdStep == 3)) cmdValue = TRIEND;

        if (cmdValue > 0)
        {
            switch (cmdStep)
            {
                case 0:
                    if (cmdValue == TRI0PART1) { cmdTrinaer = TRI0; cmdStep++; }
                    else if (cmdValue == TRI1PART1) { cmdTrinaer = TRI1; cmdStep++; }
                    else cmdError = 1;
                break;

                case 1:
                    if ((cmdValue == TRI0PART2) && (cmdTrinaer == TRI0))
                    {
                        cmdStep++;
                        cmdBit *= 2UL;
                    }
                    else if ((cmdValue == TRI1PART2) && (cmdTrinaer == TRI1))
                    {
                        cmdStep++;
                        cmd |= cmdBit;
                        cmdBit *= 2UL;
                    }
                    else cmdError = 1;
                break;

                    // Zweiter Teil Part 1 - Trinaer-Art kann sich in beide Richtungen aendern
                    // (Z-Zustand, erweiterter MM2-Modus)
                case 2:
                    if ((cmdValue == TRI0PART1) && (cmdTrinaer == TRI0)) cmdStep++;
                    else if ((cmdValue == TRI1PART1) && (cmdTrinaer == TRI1)) cmdStep++;
                    else if ((cmdValue == TRI0PART1) && (cmdTrinaer == TRI1))
                    {
                        cmdStep++;
                        cmdTrinaer = TRI0;
                    }
                    else if ((cmdValue == TRI1PART1) && (cmdTrinaer == TRI0))
                    {
                        cmdStep++;
                        cmdTrinaer = TRI1;
                    }
                    else cmdError = 1;
                break;

                case 3:
                    if (cmdValue == TRIEND)
                    {
                        if (cmdTrinaer == TRI1) cmd |= cmdBit;
                        cmdStep = 0;
                        cmdTrinaer = 0;
                        cmdBit *= 2UL;
                    }
                    else if ((cmdValue == TRI0PART2) && (cmdTrinaer == TRI0))
                    {
                        cmdStep = 0;
                        cmdTrinaer = 0;
                        cmdBit *= 2UL;
                    }
                    else if ((cmdValue == TRI1PART2) && (cmdTrinaer == TRI1))
                    {
                        cmdStep = 0;
                        cmdTrinaer = 0;
                        cmd |= cmdBit;
                        cmdBit *= 2UL;
                    }
                    else cmdError = 1;
                break;
            }
        }
        else
        {
            cmdError = 1;
        }

        if (cmdError)
        {
            cmdStep = 0;
            cmdTrinaer = 0;
            cmdBit = 1UL;
            cmdCount = 0;
            cmd = 0UL;
            cmdPrev = 0UL;
        }

        if (cmdBit == 0x40000UL)
        {
            if (cmdCount == 0)
            {
                cmdPrev = cmd;
                cmdCount++;
            }
            else
            {
                if (cmd == cmdPrev)
                {
                    *cmdBefehl = cmd;
                    cmdFlag = 1;
                }

                cmdCount = 0;
                cmdPrev = 0UL;
            }

            cmdBit = 1UL;
            cmd = 0UL;
        }

        return cmdFlag;
    }

//*************************************************************************************************
//* Auswerten eines ueber INT0 empfangenen, verifizierten Magnetartikelbefehls und Versenden als  *
//* CS2-Zubehoer-Schalten-Nachricht (inkl. I2C-Spiegelung) - 1:1 aus mbc_track_check_magnet()     *
//* uebernommen. Bei Strom aus wird die zuletzt gemerkte Adresse/Spulennummer wiederverwendet, da *
//* das Motorola-Ausschaltsignal selbst keine Adressinfo mehr traegt.                             *
//*************************************************************************************************
//*************************************************************************************************
//* trackCmd: verifizierter 18-Bit-Rohbefehl (siehe automatMagnetFn())                            *
//*************************************************************************************************

    static void trackCheckMagnetFn(uint32_t trackCmd)
    {
        static uint16_t addrPrev     = 0;
        static uint8_t  richtungPrev = 0;

        uint8_t trackDirection = (uint8_t) ((trackCmd & 0x0fc00UL) >> 10);
        uint8_t trackCurrent   = (uint8_t) ((trackCmd & 0x30000UL) >> 16);

        if (!trackCurrent)
        {
            uint8_t msg[MBC_CAN_MSG_LEN];

            cuOwner->sendZubehoerSchalten(addrPrev, richtungPrev, false, msg);
            cuSendKbFn(msg);
            return;
        }

        // Address-TRITs auswerten, Basisadresse des angesprochenen Decoders bestimmen

        uint8_t trit0 = (uint8_t) (trackCmd & 0x00003UL);
        uint8_t trit1 = (uint8_t) ((trackCmd & 0x0000cUL) >> 2);
        uint8_t trit2 = (uint8_t) ((trackCmd & 0x00030UL) >> 4);
        uint8_t trit3 = (uint8_t) ((trackCmd & 0x000c0UL) >> 6);

        uint16_t trackAddr = 0;

        switch (trit0) { case 1: trackAddr += 2;  break; case 3: trackAddr += 1;  break; }
        switch (trit1) { case 1: trackAddr += 6;  break; case 3: trackAddr += 3;  break; }
        switch (trit2) { case 1: trackAddr += 18; break; case 3: trackAddr += 9;  break; }
        switch (trit3) { case 1: trackAddr += 54; break; case 3: trackAddr += 27; break; }

        trackAddr = (uint16_t) ((trackAddr - 1) << 2);
        trackAddr += 0x3000;

        // Spulennummer/Richtung aus der Richtungs-TRIT-Kombination bestimmen

        uint8_t richtung = 0;
        uint8_t monoid   = 0;
        uint8_t error    = 0;

        switch (trackDirection)
        {
            case 0x00: richtung = 0; monoid = 0; break;
            case 0x03: richtung = 1; monoid = 0; break;
            case 0x0c: richtung = 0; monoid = 1; break;
            case 0x0f: richtung = 1; monoid = 1; break;
            case 0x30: richtung = 0; monoid = 2; break;
            case 0x33: richtung = 1; monoid = 2; break;
            case 0x3c: richtung = 0; monoid = 3; break;
            case 0x3f: richtung = 1; monoid = 3; break;
            default:   error = 1;
        }

        if (error) return;

        trackAddr = (uint16_t) (trackAddr + monoid);

        addrPrev     = trackAddr;
        richtungPrev = richtung;

        uint8_t msg[MBC_CAN_MSG_LEN];

        cuOwner->sendZubehoerSchalten(trackAddr, richtung, true, msg);
        cuSendKbFn(msg);
    }

//*************************************************************************************************
//* Sucht den Lokliste-Index, dessen GUiD der "klassischen" MM-Alias-GUiD einer vom Gleisdecoder    *
//* gelesenen Adresse entspricht ({0x00,0x00,0x00,Adresse}, Adresse 0 zaehlt dabei als 80 - gleiche *
//* Regel wie beim Default-Init in cuInitFn()). Noetig, weil der Gleisdecoder echte MM-Gleisadressen*
//* liest, waehrend CU-Bus/CSx dieselbe Lok ueber ihren CONNECT6021-Slot-Index anspricht - beides   *
//* ist nur ohne CONNECT6021-Remapping identisch.                                                   *
//*************************************************************************************************
//*************************************************************************************************
//* gleisAddr: vom Gleisdecoder gelesene MM-Adresse (0...79, 0 entspricht Adresse 80)               *
//*************************************************************************************************
//*************************************************************************************************
//* Return: Lokliste-Index (0...79) der gemappten Lok mit dieser Alias-GUiD, sonst 0xFF             *
//*************************************************************************************************

    static uint8_t findLokByAlias(uint8_t gleisAddr)
    {
        uint8_t realAddr = (gleisAddr == 0) ? 80 : gleisAddr;

        for (uint8_t index = 0; index < LOKS_ANZAHL; index++)
        {
            uint8_t *lok = cuUtils->array() + MBC_89_S_LOKS + ((uint16_t) index * LOKS_ITEMS);

            if (lok[LOK_MAPPED] && (lok[LOK_ADDR4] == 0) && (lok[LOK_ADDR3] == 0)
                                 && (lok[LOK_ADDR2] == 0) && (lok[LOK_ADDR1] == realAddr))
            {
                return index;
            }
        }

        return 0xff;
    }

//*************************************************************************************************
//* Auswerten eines ueber INT0 empfangenen, verifizierten Lokbefehls und Versenden der Aenderungen*
//* (Richtung/Geschwindigkeit/F0.../F4) als CS2-CAN-Nachrichten - 1:1 aus mbc_track_check_lok()   *
//* uebernommen. Nur wenn die adressierte Lok bereits per CONNECT6021 gemappt ist (sonst ist keine*
//* GUiD bekannt, an die gesendet werden koennte). Der Lokliste-Index wird ueber findLokByAlias() *
//* aufgeloest, da der Gleisdecoder echte MM-Gleisadressen liest, nicht CONNECT6021-Slot-Indizes. *
//*************************************************************************************************
//*************************************************************************************************
//* trackCmd: verifizierter 18-Bit-Rohbefehl (siehe automatLokFn())                               *
//*************************************************************************************************

    static void trackCheckLokFn(uint32_t trackCmd)
    {
        // TRITs auswerten und invertieren (vereinfacht die weitere Auswertung)

        uint8_t trit[9];

        trit[0] = (uint8_t) (trackCmd & 0x00003UL);
        trit[1] = (uint8_t) ((trackCmd & 0x0000cUL) >> 2);
        trit[2] = (uint8_t) ((trackCmd & 0x00030UL) >> 4);
        trit[3] = (uint8_t) ((trackCmd & 0x000c0UL) >> 6);
        trit[4] = (uint8_t) ((trackCmd & 0x00300UL) >> 8);
        trit[5] = (uint8_t) ((trackCmd & 0x00c00UL) >> 10);
        trit[6] = (uint8_t) ((trackCmd & 0x03000UL) >> 12);
        trit[7] = (uint8_t) ((trackCmd & 0x0c000UL) >> 14);
        trit[8] = (uint8_t) ((trackCmd & 0x30000UL) >> 16);

        for (uint8_t i = 0; i < 9; i++)
        {
            if ((trit[i] != 0) && (trit[i] != 3)) trit[i] = (uint8_t) ((~trit[i]) & 0x03);
        }

        // Lok-Adresse (dezimal) aus den ersten 4 TRITs bestimmen

        uint8_t lokAddrDez = 0;

        if (trit[0] == 3) lokAddrDez = 1;
        if (trit[0] == 2) lokAddrDez = 2;
        if (trit[1] == 3) lokAddrDez += 3;
        if (trit[1] == 2) lokAddrDez += 6;
        if (trit[2] == 3) lokAddrDez += 9;
        if (trit[2] == 2) lokAddrDez += 18;
        if (trit[3] == 3) lokAddrDez += 27;
        if (trit[3] == 2) lokAddrDez += 54;

        infraIsPresent = true;

        uint8_t index = findLokByAlias(lokAddrDez);

        if (index == 0xff) return;

        uint8_t *lok = cuUtils->array() + MBC_89_S_LOKS + ((uint16_t) index * LOKS_ITEMS);

        if (!lok[LOK_INFRA]) lok[LOK_INFRA] = 1;

        // Lok-Werte aus dem Signalstream bzw. aus dem Lokspeicher vorbelegen

        uint8_t lokSpeed = (uint8_t) (((trit[8] & 0x02) << 2) | ((trit[7] & 0x02) << 1)
                                    | (trit[6] & 0x02) | ((trit[5] & 0x02) >> 1));

        uint8_t lokDirCs2 = lok[LOK_DIR];
        uint8_t lokF0     = trit[4];
        uint8_t f1 = lok[LOK_FX] & 0x01;
        uint8_t f2 = (lok[LOK_FX] & 0x02) >> 1;
        uint8_t f3 = (lok[LOK_FX] & 0x04) >> 2;
        uint8_t f4 = (lok[LOK_FX] & 0x08) >> 3;

        // Richtung, Fahrstufenbereich und Funktionen F1...F4 anhand des Filterwerts festlegen. Bei
        // den Filter-Ergebnissen 0x05/0x0a gibt es aus Kompatibilitaetsgruenden je nach erkannter
        // Geschwindigkeit Ausnahmen (Doppelbelegung Fahrstufe/Funktion).

        uint8_t lokFilter = (uint8_t) (((trit[5] & 0x01) << 3) | ((trit[6] & 0x01) << 2)
                                     | ((trit[7] & 0x01) << 1) | (trit[8] & 0x01));
        uint8_t lokStep = 0;

        switch (lokFilter)
        {
            case 0x04:
                lokDirCs2 = 1;
                lokStep = 1;
            break;

            case 0x05:
                if ((lokSpeed != 0x0b) && (lokSpeed != 0x0c)
                                       && (lokSpeed != 0x0e) && (lokSpeed != 0x0f))
                {
                    lokDirCs2 = 1;
                    lokStep = 1;
                }

                if (lokSpeed == 0x0b) { f1 = 1; lokStep = 2; }
                if (lokSpeed == 0x0c) { f2 = 1; lokStep = 2; }
                if (lokSpeed == 0x0e) { f3 = 1; lokStep = 2; }
                if (lokSpeed == 0x0f) { f4 = 1; lokStep = 2; }
            break;

            case 0x0a:
                if ((lokSpeed != 0x03) && (lokSpeed != 0x04)
                                       && (lokSpeed != 0x06) && (lokSpeed != 0x07))
                {
                    lokDirCs2 = 2;
                    lokStep = 1;
                }

                if (lokSpeed == 0x03) { f1 = 0; lokStep = 2; }
                if (lokSpeed == 0x04) { f2 = 0; lokStep = 2; }
                if (lokSpeed == 0x06) { f3 = 0; lokStep = 2; }
                if (lokSpeed == 0x07) { f4 = 0; lokStep = 2; }
            break;

            case 0x0b: lokDirCs2 = 2; lokStep = 1; break;
            case 0x0c: f1 = 0; lokStep = 2; break;
            case 0x0d: f1 = 1; lokStep = 2; break;
            case 0x02: f2 = 0; lokStep = 2; break;
            case 0x03: f2 = 1; lokStep = 2; break;
            case 0x06: f3 = 0; lokStep = 2; break;
            case 0x07: f3 = 1; lokStep = 2; break;
            case 0x0e: f4 = 0; lokStep = 2; break;
            case 0x0f: f4 = 1; lokStep = 2; break;
        }

        // lokStep 1: Fahrtrichtungsumschaltung, Geschwindigkeit und Funktion F0 bei Aenderung an
        // die CS2 senden und in der Lok-Datenbank sichern

        if (lokStep == 1)
        {
            if (lokSpeed == 1) lokDirCs2 = DIR_UMSCHALTEN;

            if (lokDirCs2 != lok[LOK_DIR])
            {
                msgLokDir[MBC_CAN_D0] = lok[LOK_ADDR4];
                msgLokDir[MBC_CAN_D1] = lok[LOK_ADDR3];
                msgLokDir[MBC_CAN_D2] = lok[LOK_ADDR2];
                msgLokDir[MBC_CAN_D3] = lok[LOK_ADDR1];
                msgLokDir[MBC_CAN_D4] = lokDirCs2;
                lok[LOK_DIR] = lokDirCs2;

                cuOwner->sendCan(msgLokDir);
            }

            uint16_t lokSpeedCs2 = (lokSpeed > 1)
                                 ? ((uint16_t) (lokSpeed - 1) * MM_SPEED_STEP) : 0;

            if (lokSpeed != lok[LOK_SPEED])
            {
                msgLokSpeed[MBC_CAN_D0] = lok[LOK_ADDR4];
                msgLokSpeed[MBC_CAN_D1] = lok[LOK_ADDR3];
                msgLokSpeed[MBC_CAN_D2] = lok[LOK_ADDR2];
                msgLokSpeed[MBC_CAN_D3] = lok[LOK_ADDR1];
                msgLokSpeed[MBC_CAN_D4] = (uint8_t) ((lokSpeedCs2 & 0xff00) >> 8);
                msgLokSpeed[MBC_CAN_D5] = (uint8_t) (lokSpeedCs2 & 0xff);
                lok[LOK_SPEED] = lokSpeed;

                cuOwner->sendCan(msgLokSpeed);
            }

            if (lokF0 > 0) lokF0 = 1;

            if (lokF0 != lok[LOK_F0])
            {
                msgLokFx[MBC_CAN_D0] = lok[LOK_ADDR4];
                msgLokFx[MBC_CAN_D1] = lok[LOK_ADDR3];
                msgLokFx[MBC_CAN_D2] = lok[LOK_ADDR2];
                msgLokFx[MBC_CAN_D3] = lok[LOK_ADDR1];
                msgLokFx[MBC_CAN_D4] = 0;
                msgLokFx[MBC_CAN_D5] = lokF0;
                lok[LOK_F0] = lokF0;

                cuOwner->sendCan(msgLokFx);
            }
        }

        // lokStep 2: Aenderungen bei den Funktionen F1...F4 bestimmen und senden

        if (lokStep == 2)
        {
            msgLokFx[MBC_CAN_D0] = lok[LOK_ADDR4];
            msgLokFx[MBC_CAN_D1] = lok[LOK_ADDR3];
            msgLokFx[MBC_CAN_D2] = lok[LOK_ADDR2];
            msgLokFx[MBC_CAN_D3] = lok[LOK_ADDR1];

            if (f1 != (lok[LOK_FX] & 0x01))
            {
                msgLokFx[MBC_CAN_D4] = 0x01;
                msgLokFx[MBC_CAN_D5] = f1;
                lok[LOK_FX] = (uint8_t) ((lok[LOK_FX] & 0xfe) | (f1 * 0x01));

                cuOwner->sendCan(msgLokFx);
            }

            if (f2 != ((lok[LOK_FX] & 0x02) >> 1))
            {
                msgLokFx[MBC_CAN_D4] = 0x02;
                msgLokFx[MBC_CAN_D5] = f2;
                lok[LOK_FX] = (uint8_t) ((lok[LOK_FX] & 0xfd) | (f2 * 0x02));

                cuOwner->sendCan(msgLokFx);
            }

            if (f3 != ((lok[LOK_FX] & 0x04) >> 2))
            {
                msgLokFx[MBC_CAN_D4] = 0x03;
                msgLokFx[MBC_CAN_D5] = f3;
                lok[LOK_FX] = (uint8_t) ((lok[LOK_FX] & 0xfb) | (f3 * 0x04));

                cuOwner->sendCan(msgLokFx);
            }

            if (f4 != ((lok[LOK_FX] & 0x08) >> 3))
            {
                msgLokFx[MBC_CAN_D4] = 0x04;
                msgLokFx[MBC_CAN_D5] = f4;
                lok[LOK_FX] = (uint8_t) ((lok[LOK_FX] & 0xf7) | (f4 * 0x08));

                cuOwner->sendCan(msgLokFx);
            }
        }
    }

//*************************************************************************************************
//* Auswerten des Signalpuffers - 1:1 aus mbc_track_check() uebernommen: signalisiert bei jedem   *
//* neuen Signal die INFRA-Anwesenheit, klassifiziert die Impulslaenge getrennt fuer              *
//* Magnetartikel- und Lok-Timing (siehe die frisch abgeleiteten MAGNET_*/LOK_*-Fenster oben) und *
//* speist beide Trit-Automaten parallel.                                                         *
//*************************************************************************************************

    static void trackCheckFn()
    {
        static uint8_t  cmdPRead = 0;
        static uint32_t cmdLok    = 0UL;
        static uint32_t cmdMagnet = 0UL;

        #if !defined(__AVR__)
            trackRmtDrain();
        #endif

        if (cmdNr == 0) return;

        infraIsPresent = true;

        uint8_t  cmdLevel;
        uint16_t cmdLength;

        #if defined(__AVR__)
            ATOMIC_BLOCK(ATOMIC_FORCEON)
            {
                cmdLevel  = cmdBLevel[cmdPRead];
                cmdLength = cmdBLength[cmdPRead];
            }
        #else
            // Keine ISR-Konkurrenz auf ESP32 - trackRmtDrain() laeuft im selben (Hauptschleifen-)
            // Kontext wie trackCheckFn(), kein atomarer Schutz noetig.
            cmdLevel  = cmdBLevel[cmdPRead];
            cmdLength = cmdBLength[cmdPRead];
        #endif

        // Magnetartikel: TRIT-Zuordnung nach Impulslaenge

        uint8_t cmdValue = 0;

        if (!cmdLevel && (cmdLength > MAGNET_LANG_MIN) && (cmdLength < MAGNET_LANG_MAX))
            cmdValue = TRI1PART1;

        if (cmdLevel && (cmdLength > MAGNET_KURZ_MIN) && (cmdLength < MAGNET_KURZ_MAX))
            cmdValue = TRI1PART2;

        if (!cmdLevel && (cmdLength > MAGNET_KURZ_MIN) && (cmdLength < MAGNET_KURZ_MAX))
            cmdValue = TRI0PART1;

        if (cmdLevel && (cmdLength > MAGNET_LANG_MIN) && (cmdLength < MAGNET_LANG_MAX))
            cmdValue = TRI0PART2;

        if (automatMagnetFn(cmdLevel, cmdValue, &cmdMagnet)) trackCheckMagnetFn(cmdMagnet);

        // Lokomotive: TRIT-Zuordnung nach Impulslaenge (jeweils doppelt so lang wie Magnetartikel)

        cmdValue = 0;

        if (!cmdLevel && (cmdLength > LOK_LANG_MIN) && (cmdLength < LOK_LANG_MAX))
            cmdValue = TRI1PART1;

        if (cmdLevel && (cmdLength > LOK_KURZ_MIN) && (cmdLength < LOK_KURZ_MAX))
            cmdValue = TRI1PART2;

        if (!cmdLevel && (cmdLength > LOK_KURZ_MIN) && (cmdLength < LOK_KURZ_MAX))
            cmdValue = TRI0PART1;

        if (cmdLevel && (cmdLength > LOK_LANG_MIN) && (cmdLength < LOK_LANG_MAX))
            cmdValue = TRI0PART2;

        if (automatLokFn(cmdLevel, cmdValue, &cmdLok)) trackCheckLokFn(cmdLok);

        cmdNr--;

        if (++cmdPRead == SIGNALPUFFER) cmdPRead = 0;
    }

//*************************************************************************************************
//* Interruptroutine INT0 (NUR AVR) - wertet das Tracksignal aus, wenn eine MM-Zentrale ueber den *
//* TRACK-Eingang angeschlossen ist. 1:1 aus SIGNAL(INT0_vect) in mbc_gleis.c uebernommen.        *
//* ESP32-Aequivalent: trackRmtDrain() (siehe oben), da die RMT-Hardware pollend statt per         *
//* Pro-Flanke-ISR ausgelesen wird.                                                                *
//*************************************************************************************************

    #if defined(__AVR__)

    ISR(INT0_vect)
    {
        static uint8_t cmdPWrite = 0;
        static uint8_t level     = 0xff;

        uint16_t length = TCNT1;

        TCNT1 = 0;

        if (cmdNr < SIGNALPUFFER)
        {
            cmdBLevel[cmdPWrite]  = level;
            cmdBLength[cmdPWrite] = length;

            if (++cmdPWrite == SIGNALPUFFER) cmdPWrite = 0;

            cmdNr++;
        }

        if (level)
        {
            EICRA = (1 << ISC01) | (1 << ISC00);
            level = 0;
        }
        else
        {
            EICRA = (1 << ISC01);
            level = 0xff;
        }
    }

//*************************************************************************************************
//* Interruptroutine TIMER1-Overflow - bei Zeitueberschreitung waehrend der Befehlserkennung liegt*
//* ein ungueltiger Befehlsframe vor (entsteht durch das ueberlagerte Lok-/mfx-/DCC-Signal) - 1:1 *
//* aus SIGNAL(TIMER1_OVF_vect) uebernommen.                                                      *
//*************************************************************************************************

    ISR(TIMER1_OVF_vect)
    {
        TCNT1 = 1000;
    }

    #endif

//------ Ende Gleisdecoder-Subsystem --------------------------------------------------------------

//###### Ende Quellcode Mbc89Com.cpp ##############################################################

