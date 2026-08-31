//*************************************************************************************************
//* CAN-Anbindung, Maerklin-CS2-29-Bit-Header (MBCANLib)                                          *
//*************************************************************************************************
//* CAN-Protokoll der CS2 von Maerklin - Herleitung des Bit-Layouts:                              *
//*                                                                                               *
//* Maerklin benutzt eine eigene Interpretation des CAN-Headers fuer die Uebertragung von         *
//* Befehlen und zur Unterscheidung von Prioritaeten und Units.                                   *
//*                                                                                               *
//* CAN-Header Maerklin (29-Bit Extended Frame) 2+2 Bit 8 Bit 1 Bit 16 Bit Prio Kommando Response *
//* Hash                                                                                          *
//*                                                                                               *
//* Der Hashwert weist auf den Bitpositionen 8 und 9 immer einen 1-Wert auf, und in Bitposition 7 *
//* immer einen 0-Wert, um eine Unterscheidung zum Protokoll der CS1 herbeizufuehren.             *
//*                                                                                               *
//* Der Extended-Header, den der MCP2515 benutzt, muss aus dem Maerklin-Header abgeleitet werden. *
//* Die Prio-Bits sind dabei immer 0 und sollen laut Maerklin nicht ausgewertet werden. Der       *
//* Hashwert wird defaultmaessig immer auf 0x0300 gesetzt.                                        *
//*                                                                                               *
//* Da die CAN-ID nur 29 Bit hat, muss diese zunaechst um drei Stellen nach links verschoben      *
//* werden, um die Stellen fuer SID0 bis SID10 zu erhalten. Danach werden die restlichen Bits aus *
//* dem Maerklin-Header auf den MCP-Header gemappt (N.C. = Not Care, EXTIDE = Kenner fuer den     *
//* MCP, dass ein 29-Bit-Header verwendet wird):                                                  *
//*                                                                                               *
//* CAN-Header des MCP2515 (29-Bit Extended Frame, in vier Bytes): SID10...SID0 + N.C. + EXTIDE + *
//* N.C. + EID17...EID0 = 32 Bit                                                                  *
//*                                                                                               *
//* Beispiel: Prio: 0x0 = 0b0000 Kommando Schalten: 0x0b = 0b00001011 Response: 0x0 = 0b0 Hash:   *
//* 0x0300 = 0b0000001100000000 Zusammengesetzt: 0x00160300 = 0b0000 00001011 0 0000001100000000  *
//*                                                                                               *
//* Maerklin-Header: 0x00160300 = 0bxxx00000000 101100000001100000000 3 Stellen nach links:       *
//* 0x00b01800 = 0b00000000101 100000001100000000 SID10...SID3: 0b00000000 SID2...SID0: 0b101     *
//* N.C.: 0b0 EXTIDE: 0b1 N.C.: 0b0 EID17...EID16: 0b10 EID15...EID8: 0b00000011 EID7...EID0:     *
//* 0b00000000                                                                                    *
//*                                                                                               *
//*   MCP-Header: Byte1=0x00  Byte2=0xaa  Byte3=0x03  Byte4=0x00                                  *
//*                                                                                               *
//* Das interne TWAI-Backend des ESP32-C3 (siehe unten) benoetigt keine MCP2515-Registerbytes,    *
//* sondern nimmt die 29-Bit-Extended-ID direkt als Zahlenwert entgegen/liefert sie so zurueck.   *
//* Um beide Backends auf denselben Header-Bytes (id1...id4 im MCP2515-Registerformat             *
//* SIDH/SIDL/EID8/EID0) aufzusetzen, rechnen buildExtendedId()/splitExtendedId() zwischen diesem *
//* Byteformat und der reinen 29-Bit-Zahl um - das ist eine allgemeine,                           *
//* MCP2515-Datenblatt-genormte Umrechnung, unabhaengig von den Maerklin-Spezifika, die bereits   *
//* oben in id1...id4 stecken.                                                                    *
//*************************************************************************************************
//* Datei: MbcCan.cpp                                                                             *
//*************************************************************************************************
//* MBCAN (c) 2007-2026 by Dr.-Ing. Thomas Wiesner                                                *
//*************************************************************************************************
//* Aenderungen:                                                                                  *
//* V1.0.0 - 02.08.2026 - Nullversion                                                             *
//*************************************************************************************************

//###### Start Quellcode MbcCan.cpp ###############################################################

#include "MbcCan.h"

//------ Start interne Konstanten -----------------------------------------------------------------

    static const uint8_t CAN_CNF1VALUE = 0x81;   // 250 kBit/s bei 16 MHz
    static const uint8_t CAN_CNF2VALUE = 0xac;
    static const uint8_t CAN_CNF3VALUE = 0x03;

    static const uint8_t CAN_CANINTEVALUE = (1 << 1) | (1 << 0);   // RX1IE, RX0IE

    static const uint8_t CAN_RESET        = 0xC0;
    static const uint8_t CAN_READ         = 0x03;
    static const uint8_t CAN_READ_RX_0    = 0x90;
    static const uint8_t CAN_READ_RX_1    = 0x94;
    static const uint8_t CAN_WRITE        = 0x02;
    static const uint8_t CAN_WRITE_TX_0   = 0x40;
    static const uint8_t CAN_RTS_0        = 0x81;
    static const uint8_t CAN_READ_STATUS  = 0xA0;
    static const uint8_t CAN_RX_STATUS    = 0xB0;
    static const uint8_t CAN_OPMOD_NORMAL = 0xE0;

    static const uint8_t CAN_SPI_DUMMY = 0xFF;

    static const uint8_t CAN_BFPCTRL    = 0x0C;
    static const uint8_t CAN_TXRTSCTRL  = 0x0D;
    static const uint8_t CAN_CANSTAT    = 0x0E;
    static const uint8_t CAN_CANCTRL    = 0x0F;
    static const uint8_t CAN_CNF3       = 0x28;
    static const uint8_t CAN_CNF2       = 0x29;
    static const uint8_t CAN_CNF1       = 0x2A;
    static const uint8_t CAN_CANINTE    = 0x2B;
    static const uint8_t CAN_RXB0CTRL   = 0x60;
    static const uint8_t CAN_RXB1CTRL   = 0x70;

    static const uint8_t CAN_RXM1 = 6;
    static const uint8_t CAN_RXM0 = 5;

    static const uint16_t NC_IDE_NC = 0x0008;   // Extended-Identifier-Kenner (29-Bit-Header)

//------ Ende interne Konstanten ------------------------------------------------------------------

//------ Start Umrechnung MCP2515-Registerbytes <-> 29-Bit-Extended-ID ----------------------------

    // Nur vom TWAI-Backend (ESP32/ESP32-C3) gebraucht, siehe Kopfkommentar - beim MCP2515-Backend
    // wuerde sonst "defined but not used" gewarnt, da dort nirgends aufgerufen

    #if defined(ESP32)

    //*********************************************************************************************
    //* SIDH(id1)/SIDL(id2)/EID8(id3)/EID0(id4) -> reine 29-Bit-Extended-ID (fuer TWAI)           *
    //*********************************************************************************************
    //* id1...id4: MCP2515-Registerbytes SIDH/SIDL/EID8/EID0                                      *
    //*********************************************************************************************
    //* Return: 29-Bit-Extended-ID                                                                *
    //*********************************************************************************************

    static uint32_t buildExtendedId(uint8_t id1, uint8_t id2, uint8_t id3, uint8_t id4)
    {
        uint32_t sid = ((uint32_t) id1 << 3) | (id2 >> 5);
        uint32_t eid = (((uint32_t) (id2 & 0x03)) << 16) | ((uint32_t) id3 << 8) | id4;

        return (sid << 18) | eid;
    }

    //*********************************************************************************************
    //* 29-Bit-Extended-ID (von TWAI) -> SIDH(id1)/SIDL(id2)/EID8(id3)/EID0(id4)                  *
    //*********************************************************************************************
    //* extId:      29-Bit-Extended-ID                                                            *
    //* id1...id4:  Zielvariablen fuer die MCP2515-Registerbytes                                  *
    //*********************************************************************************************

    static void splitExtendedId(uint32_t extId, uint8_t &id1, uint8_t &id2, uint8_t &id3, uint8_t &id4)
    {
        uint32_t sid = (extId >> 18) & 0x7FFUL;
        uint32_t eid = extId & 0x3FFFFUL;

        id1 = (uint8_t) ((sid >> 3) & 0xFF);
        id2 = (uint8_t) (((sid & 0x07) << 5) | ((eid >> 16) & 0x03));
        id3 = (uint8_t) ((eid >> 8) & 0xFF);
        id4 = (uint8_t) (eid & 0xFF);
    }

    #endif

//------ Ende Umrechnung --------------------------------------------------------------------------

//------ Start Klasse MbcCan ----------------------------------------------------------------------

    //*********************************************************************************************
    //* Konstruktor fuer das MCP2515-Backend (externer Baustein an SPI)                           *
    //*********************************************************************************************
    //* pinCs:      /CS-Pin dieses MCP2515                                                        *
    //* utils:      MbcUtils-Instanz fuer die Traffic-LED-Anzeige                                 *
    //* spi:        zu nutzende SPI-Schnittstelle                                                 *
    //* spiClockHz: SPI-Takt in Hz                                                                *
    //*********************************************************************************************

    MbcCan::MbcCan(uint8_t pinCs, MbcUtils &utils, SPIClass &spi, uint32_t spiClockHz)
        : _backend(MBC_CAN_BACKEND_MCP2515), _utils(&utils), _initialized(false),
          _spiDevice(pinCs, spiClockHz, spi), _bothRx(false)
          #if defined(ESP32)
              , _pinTx(0), _pinRx(0)
          #endif
    {
    }

    #if defined(ESP32)

        //*****************************************************************************************
        //* Konstruktor fuer den internen TWAI-Controller (NUR ESP32/ESP32-C3)                    *
        //*****************************************************************************************
        //* pinTx: TX-Pin zum externen CAN-Transceiver (z.B. SN65HVD230)                          *
        //* pinRx: RX-Pin vom externen CAN-Transceiver                                            *
        //* utils: MbcUtils-Instanz fuer die Traffic-LED-Anzeige                                  *
        //*****************************************************************************************

        MbcCan::MbcCan(uint8_t pinTx, uint8_t pinRx, MbcUtils &utils)
            : _backend(MBC_CAN_BACKEND_TWAI), _utils(&utils), _initialized(false),
              _spiDevice(255), _bothRx(false),
              _pinTx(pinTx), _pinRx(pinRx)
        {
        }

    #endif

    //*********************************************************************************************
    //* Wartet ms Millisekunden, ohne MbcUtils::update() (Traffic-/Blinkanzeige, Ereigniszaehler) *
    //* waehrenddessen anzuhalten - im Unterschied zu delay().                                    *
    //*********************************************************************************************
    //* ms:    Wartezeit in Millisekunden                                                         *
    //* utils: MbcUtils-Instanz, die waehrenddessen getickt wird                                  *
    //*********************************************************************************************

    static void waitMsTicking(unsigned long ms, MbcUtils &utils)
    {
        unsigned long start = millis();

        while ((unsigned long) (millis() - start) < ms)
        {
            utils.update();
        }
    }

    //*********************************************************************************************
    //* CAN-Controller initialisieren - verzweigt je nach Konstruktor auf das MCP2515- oder das   *
    //* TWAI-Backend                                                                              *
    //*********************************************************************************************
    //* Return: false bei Initialisierungsfehler (auch bei Timeout)                               *
    //*********************************************************************************************

    bool MbcCan::begin()
    {
        #if defined(ESP32)
            if (_backend == MBC_CAN_BACKEND_TWAI) return beginTwai();
        #endif

        return beginMcp2515();
    }

    //*********************************************************************************************
    //* CAN-Nachricht senden - verzweigt je nach Konstruktor auf das MCP2515- oder das            *
    //* TWAI-Backend                                                                              *
    //*********************************************************************************************
    //* msg: MBC_CAN_MSG_LEN Byte, siehe MBC_CAN_*-Indexkonstanten                                *
    //*********************************************************************************************
    //* Return: false bei Sendefehler                                                             *
    //*********************************************************************************************

    bool MbcCan::send(const uint8_t msg[MBC_CAN_MSG_LEN])
    {
        #if defined(ESP32)
            if (_backend == MBC_CAN_BACKEND_TWAI) return sendTwai(msg);
        #endif

        return sendMcp2515(msg);
    }

    //*********************************************************************************************
    //* CAN-Nachricht abholen - verzweigt je nach Konstruktor auf das MCP2515-oder das            *
    //* TWAI-Backend                                                                              *
    //*********************************************************************************************
    //* msg: MBC_CAN_MSG_LEN Byte, siehe MBC_CAN_*-Indexkonstanten                                *
    //*********************************************************************************************
    //* Return: false, wenn keine Nachricht vorliegt                                              *
    //*********************************************************************************************

    bool MbcCan::receive(uint8_t msg[MBC_CAN_MSG_LEN])
    {
        #if defined(ESP32)
            if (_backend == MBC_CAN_BACKEND_TWAI) return receiveTwai(msg);
        #endif

        return receiveMcp2515(msg);
    }

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //+ MCP2515-Backend (SPI)                                                                     +
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

        //*****************************************************************************************
        //* Beschreibe Register im MCP2515                                                        *
        //*****************************************************************************************
        //* addr: Registeradresse                                                                 *
        //* data: zu schreibendes Byte                                                            *
        //*****************************************************************************************

        void MbcCan::writeReg(uint8_t addr, uint8_t data)
        {
            _spiDevice.beginTransaction();

            _spiDevice.transfer(CAN_WRITE);
            _spiDevice.transfer(addr);
            _spiDevice.transfer(data);

            _spiDevice.endTransaction();
        }

        //*****************************************************************************************
        //* Lese Register vom MCP2515                                                             *
        //*****************************************************************************************
        //* addr: Registeradresse                                                                 *
        //*****************************************************************************************
        //* Return: Registerinhalt                                                                *
        //*****************************************************************************************

        uint8_t MbcCan::readReg(uint8_t addr)
        {
            _spiDevice.beginTransaction();

            _spiDevice.transfer(CAN_READ);
            _spiDevice.transfer(addr);
            uint8_t data = _spiDevice.transfer(CAN_SPI_DUMMY);

            _spiDevice.endTransaction();

            return data;
        }

        //*****************************************************************************************
        //* Lese Status vom MCP2515                                                               *
        //*****************************************************************************************
        //* type: Statustyp (CAN_READ_STATUS/CAN_RX_STATUS)                                       *
        //*****************************************************************************************
        //* Return: Status                                                                        *
        //*****************************************************************************************

        uint8_t MbcCan::readStatus(uint8_t type)
        {
            _spiDevice.beginTransaction();

            _spiDevice.transfer(type);
            uint8_t data = _spiDevice.transfer(CAN_SPI_DUMMY);

            _spiDevice.endTransaction();

            return data;
        }

        //*****************************************************************************************
        //* MCP2515 initialisieren (Software-Reset, Timing 250 kBit/s, Empfangsfilter aus, beide  *
        //* RX-Puffer aktiv). Bricht das Warten auf den Normalmodus nach MBC_CAN_INIT_TIMEOUT_MS  *
        //* ab, statt endlos zu blockieren.                                                       *
        //*****************************************************************************************
        //* Return: false bei Initialisierungsfehler (auch bei Timeout), dann zusaetzlich Anzeige *
        //* ueber MBC_LED_MCP_ERROR an der Status-LED                                             *
        //*****************************************************************************************

        bool MbcCan::beginMcp2515()
        {
            _spiDevice.begin();

            // MCP2515 per Software-Reset zuruecksetzen, danach automatisch im
            // Konfigurationsmodus.

            _spiDevice.beginTransaction();
            _spiDevice.transfer(CAN_RESET);
            _spiDevice.endTransaction();

            waitMsTicking(200, *_utils);

            // Timingwerte einstellen

            writeReg(CAN_CNF1, CAN_CNF1VALUE);
            writeReg(CAN_CNF2, CAN_CNF2VALUE);
            writeReg(CAN_CNF3, CAN_CNF3VALUE);

            // Interrupts fuer RX-Puffer 0 und 1 aktivieren (nur zur Statusanzeige im MCP,
            // hier im Polling-Betrieb ohne INT-Pin-Auswertung genutzt)

            writeReg(CAN_CANINTE, CAN_CANINTEVALUE);

            // TXnRTS-Pins als Eingang, PIN-Funktionen aus

            writeReg(CAN_TXRTSCTRL, 0);
            writeReg(CAN_BFPCTRL, 0);

            // Beide RX-Puffer: alle Nachrichten empfangen, kein Filter, kein Rollover

            writeReg(CAN_RXB0CTRL, (uint8_t) ((1 << CAN_RXM1) | (1 << CAN_RXM0)));
            writeReg(CAN_RXB1CTRL, (uint8_t) ((1 << CAN_RXM1) | (1 << CAN_RXM0)));

            bool ok = (readReg(CAN_CNF1) == CAN_CNF1VALUE);

            // Zurueck in den Normalmodus, Clkout-Pin deaktivieren, anstehende
            // Transaktionen abbrechen

            writeReg(CAN_CANCTRL, 0);

            unsigned long start = millis();

            while ((readReg(CAN_CANSTAT) & CAN_OPMOD_NORMAL) != 0)
            {
                if ((unsigned long) (millis() - start) >= MBC_CAN_INIT_TIMEOUT_MS)
                {
                    ok = false;
                    break;
                }
            }

            if (!ok)
            {
                _utils->ledToggleOn(MBC_LED_MCP_ERROR);
            }

            _bothRx = false;
            _initialized = ok;

            return ok;
        }

        //*****************************************************************************************
        //* CAN-Nachricht vom MCP2515 abholen (Polling-Betrieb, Quasi-FIFO ueber RX-Puffer 0/1)   *
        //*****************************************************************************************
        //* msg: MBC_CAN_MSG_LEN Byte, siehe MBC_CAN_*-Indexkonstanten                            *
        //*****************************************************************************************
        //* Return: false, wenn keine Nachricht vorliegt                                          *
        //*****************************************************************************************

        bool MbcCan::receiveMcp2515(uint8_t msg[MBC_CAN_MSG_LEN])
        {
            if (!_initialized) return false;

            uint8_t status = readStatus(CAN_RX_STATUS) & 0xc0;

            if (status == 0x00) return false;

            uint8_t addr;

            if (_bothRx)
            {
                addr = CAN_READ_RX_1;
                _bothRx = false;
            }
            else
            {
                if (status == 0xc0)      { addr = CAN_READ_RX_0; _bothRx = true; }
                else if (status == 0x80) { addr = CAN_READ_RX_1; }
                else                      { addr = CAN_READ_RX_0; }   // status == 0x40
            }

            _utils->ledTraffic();

            _spiDevice.beginTransaction();

            _spiDevice.transfer(addr);

            uint8_t id1 = _spiDevice.transfer(CAN_SPI_DUMMY);
            uint8_t id2 = _spiDevice.transfer(CAN_SPI_DUMMY);
            msg[MBC_CAN_HASH_H] = _spiDevice.transfer(CAN_SPI_DUMMY);
            msg[MBC_CAN_HASH_L] = _spiDevice.transfer(CAN_SPI_DUMMY);

            msg[MBC_CAN_DLC] = _spiDevice.transfer(CAN_SPI_DUMMY);

            for (uint8_t index = 0; index < 8; index++)
            {
                msg[MBC_CAN_D0 + index] = (index < msg[MBC_CAN_DLC]) ? _spiDevice.transfer(CAN_SPI_DUMMY) : 0;
            }

            _spiDevice.endTransaction();

            // CAN-Header gemaess Maerklin-Definition zerlegen

            msg[MBC_CAN_PRIO] = (uint8_t) (id1 >> 3);
            msg[MBC_CAN_ID]   = (uint8_t) (((id1 & 0x07) << 5) | ((id2 & 0xe0) >> 3) | (id2 & 0x03));

            return true;
        }

        //*****************************************************************************************
        //* CAN-Nachricht ueber den MCP2515 senden (nur TX-Puffer 0 wird genutzt)                 *
        //*****************************************************************************************
        //* msg: MBC_CAN_MSG_LEN Byte, siehe MBC_CAN_*-Indexkonstanten                            *
        //*****************************************************************************************
        //* Return: false bei Timeout (kein freier TX-Puffer)                                     *
        //*****************************************************************************************

        bool MbcCan::sendMcp2515(const uint8_t msg[MBC_CAN_MSG_LEN])
        {
            if (!_initialized) return false;

            // Warten bis TX0-Puffer frei ist (nur TX0 wird genutzt). Ohne
            // weiteren Busteilnehmer nach 300 Versuchen abbrechen.

            uint8_t  status = 0xff;
            uint16_t overflow = 0;

            while (((status & (1 << 2)) != 0) && (overflow < 300))
            {
                status = readStatus(CAN_READ_STATUS);
                overflow++;
            }

            if (overflow == 300) return false;

            // CAN-Header gemaess Maerklin-Definition zusammensetzen

            uint16_t id = (uint16_t) (((uint16_t) msg[MBC_CAN_PRIO]) << 11);
            id |= (uint16_t) ((((uint16_t) msg[MBC_CAN_ID]) & 0x00fc) << 3);
            id |= NC_IDE_NC;
            id |= (uint16_t) (((uint16_t) msg[MBC_CAN_ID]) & 0x0003);

            uint8_t id1 = (uint8_t) ((id & 0xff00) >> 8);
            uint8_t id2 = (uint8_t) (id & 0x00ff);
            uint8_t id3 = msg[MBC_CAN_HASH_H];
            uint8_t id4 = msg[MBC_CAN_HASH_L];

            _utils->ledTraffic();

            _spiDevice.beginTransaction();

            _spiDevice.transfer(CAN_WRITE_TX_0);

            _spiDevice.transfer(id1);
            _spiDevice.transfer(id2);
            _spiDevice.transfer(id3);
            _spiDevice.transfer(id4);

            _spiDevice.transfer(msg[MBC_CAN_DLC]);

            for (uint8_t index = 0; index < msg[MBC_CAN_DLC]; index++)
            {
                _spiDevice.transfer(msg[MBC_CAN_D0 + index]);
            }

            _spiDevice.endTransaction();

            delayMicroseconds(1);

            _spiDevice.beginTransaction();
            _spiDevice.transfer(CAN_RTS_0);
            _spiDevice.endTransaction();

            return true;
        }

    #if defined(ESP32)

        //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        //+ TWAI-Backend (interner Controller, nur ESP32/ESP32-C3)                                +
        //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

            //*************************************************************************************
            //* Internen TWAI-Controller initialisieren (250 kBit/s, kein Empfangsfilter)         *
            //*************************************************************************************
            //* Return: false bei Initialisierungsfehler                                          *
            //*************************************************************************************

            bool MbcCan::beginTwai()
            {
                twai_general_config_t gConfig = TWAI_GENERAL_CONFIG_DEFAULT(
                    (gpio_num_t) _pinTx, (gpio_num_t) _pinRx, TWAI_MODE_NORMAL);

                // 250 kBit/s (analog zur MCP2515-Konfiguration ueber CAN_CNF1/2/3VALUE)

                twai_timing_config_t tConfig = TWAI_TIMING_CONFIG_250KBITS();

                // Kein Empfangsfilter (analog zur MCP2515-Konfiguration ueber RXB0CTRL/RXB1CTRL)

                twai_filter_config_t fConfig = TWAI_FILTER_CONFIG_ACCEPT_ALL();

                if (twai_driver_install(&gConfig, &tConfig, &fConfig) != ESP_OK) return false;

                if (twai_start() != ESP_OK)
                {
                    twai_driver_uninstall();
                    return false;
                }

                _initialized = true;

                return true;
            }

            //*************************************************************************************
            //* CAN-Nachricht ueber den internen TWAI-Controller senden                           *
            //*************************************************************************************
            //* msg: MBC_CAN_MSG_LEN Byte, siehe MBC_CAN_*-Indexkonstanten                        *
            //*************************************************************************************
            //* Return: false bei Timeout                                                         *
            //*************************************************************************************

            bool MbcCan::sendTwai(const uint8_t msg[MBC_CAN_MSG_LEN])
            {
                if (!_initialized) return false;

                // CAN-Header gemaess Maerklin-Definition zusammensetzen (identisch zum
                // MCP2515-Backend) und in eine reine 29-Bit-Extended-ID umrechnen.

                uint16_t id = (uint16_t) (((uint16_t) msg[MBC_CAN_PRIO]) << 11);
                id |= (uint16_t) ((((uint16_t) msg[MBC_CAN_ID]) & 0x00fc) << 3);
                id |= NC_IDE_NC;
                id |= (uint16_t) (((uint16_t) msg[MBC_CAN_ID]) & 0x0003);

                uint8_t id1 = (uint8_t) ((id & 0xff00) >> 8);
                uint8_t id2 = (uint8_t) (id & 0x00ff);
                uint8_t id3 = msg[MBC_CAN_HASH_H];
                uint8_t id4 = msg[MBC_CAN_HASH_L];

                twai_message_t message = {};
                message.identifier = buildExtendedId(id1, id2, id3, id4);
                message.extd = 1;
                message.rtr = 0;
                message.data_length_code = msg[MBC_CAN_DLC];

                for (uint8_t index = 0; index < 8; index++)
                {
                    message.data[index] = (index < msg[MBC_CAN_DLC]) ? msg[MBC_CAN_D0 + index] : 0;
                }

                _utils->ledTraffic();

                // Wie beim MCP2515-Backend: nach kurzer Wartezeit ohne weiteren
                // Busteilnehmer abbrechen statt endlos zu blockieren.

                return (twai_transmit(&message, pdMS_TO_TICKS(10)) == ESP_OK);
            }

            //*************************************************************************************
            //* CAN-Nachricht vom internen TWAI-Controller abholen (Polling-Betrieb)              *
            //*************************************************************************************
            //* msg: MBC_CAN_MSG_LEN Byte, siehe MBC_CAN_*-Indexkonstanten                        *
            //*************************************************************************************
            //* Return: false, wenn keine Nachricht vorliegt                                      *
            //*************************************************************************************

            bool MbcCan::receiveTwai(uint8_t msg[MBC_CAN_MSG_LEN])
            {
                if (!_initialized) return false;

                twai_message_t message;

                // Nicht-blockierendes Polling: receive() wird zyklisch aufgerufen statt
                // interruptgesteuert zu arbeiten.

                if (twai_receive(&message, 0) != ESP_OK) return false;

                if (!message.extd) return false;   // Maerklin nutzt ausschliesslich 29-Bit-Frames

                _utils->ledTraffic();

                uint8_t id1, id2, id3, id4;
                splitExtendedId(message.identifier, id1, id2, id3, id4);

                msg[MBC_CAN_HASH_H] = id3;
                msg[MBC_CAN_HASH_L] = id4;

                msg[MBC_CAN_DLC] = message.data_length_code;

                for (uint8_t index = 0; index < 8; index++)
                {
                    msg[MBC_CAN_D0 + index] = (index < msg[MBC_CAN_DLC]) ? message.data[index] : 0;
                }

                // CAN-Header gemaess Maerklin-Definition zerlegen (identisch zum MCP2515-Backend)

                msg[MBC_CAN_PRIO] = (uint8_t) (id1 >> 3);
                msg[MBC_CAN_ID]   = (uint8_t) (((id1 & 0x07) << 5) | ((id2 & 0xe0) >> 3) | (id2 & 0x03));

                return true;
            }

    #endif

//------ Ende Klasse MbcCan -----------------------------------------------------------------------

//###### Ende Quellcode MbcCan.cpp ################################################################
