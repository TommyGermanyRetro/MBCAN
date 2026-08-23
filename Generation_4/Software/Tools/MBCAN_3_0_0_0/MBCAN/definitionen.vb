'**************************************************************************************************
'*** Globale Variablen und Strukturen                                                           ***
'**************************************************************************************************

Imports System.IO
Imports System.Net.Sockets

Module definitionen
    '**********************************************************************************************
    '*** Definitionen zur Belegung des Systemarrays aus den Modulen Zum Vergleich sind die      ***
    '*** Definitionen aus den C-Programmen fuer die AVR eingefuegt                              ***
    '**********************************************************************************************

    '**********************************************************************************************
    '*** Indizes fuer Standardparameter der Module                                              ***
    '**********************************************************************************************

    ' // Je nach Modultyp ist das sys_array unterschiedlich lang. Alle Module haben einen gemeinsamen 
    ' // Teil mit einer Laenge von 69 Bytes. Bei den anderen Modulen kommt noch ein General-Purpose-Array
    ' // mit einer Laenge von 1024 Bytes hinzu.

    ' #ifdef _mbc_80_
    '   #define MBC_ARRAY_MAX	(MBC_L_PARA + MBC_S_PARA + 1)		    // Laenge des Systemarrays
    ' #Else
    '   #define MBC_L_GENPURP	1024								    // Laenge des General Purpose Arrays
    '   #define MBC_S_GENPURP	(MBC_L_PARA + MBC_S_PARA)			    // Index Start des General-Purpose-Arrays
    '   #define MBC_ARRAY_MAX	(MBC_L_GENPURP + MBC_S_GENPURP + 1)	    // Laenge des Systemarrays
    ' #End If

    ' #define MBC_ARRAY_START       1									// Start des belegten Systemarrays
    ' #define MBC_INFO_START	    MBC_ARRAY_START						// Start Neuanmeldeblock
    ' #define MBC_ALLG_START		(MBC_L_NAMEBLOCK + MBC_S_NAMEBLOCK)	// Start Allgemeinblock

    Public MBC_ARRAY_START = 1                                          ' Start Systemarray
    Public MBC_INFO_START = 1                                           ' Start Infoblock
    Public MBC_ALLG_START = 1                                           ' Start Allgemeinblock

    ' #define MBC_L_SNR			    8	   								// Laenge der Seriennummer
    ' #define MBC_S_SNR			    MBC_ARRAY_START						// Index Start Seriennummer
    ' #define MBC_SNR				sys_array[MBC_S_SNR]				// Seriennummer

    Public MBC_L_SNR = 8                                                ' Laenge der Seriennummer
    Public MBC_S_SNR = 1                                                ' Index Start Seriennummer

    ' #define MBC_L_ART			    4									// Laenge der Artikelnummer
    ' #define MBC_S_ART			    (MBC_L_SNR + MBC_S_SNR)				// Index Start Artikelnummer
    ' #define MBC_ART_1			    sys_array[MBC_S_ART]				// Artikelnummer Byte 4
    ' #define MBC_ART_2			    sys_array[MBC_S_ART + 1]			// Artikelnummer Byte 3
    ' #define MBC_ART_3			    sys_array[MBC_S_ART + 2]			// Artikelnummer Byte 2
    ' #define MBC_ART_4			    sys_array[MBC_S_ART + 3]			// Artikelnummer Byte 1

    Public MBC_L_ART = 4                                                ' Laenge der Artikelnummer
    Public MBC_S_ART = (MBC_S_SNR + MBC_L_SNR)                          ' Index Start Artikelnummer

    ' #define MBC_L_SW			    3									// Laenge Softwareversion
    ' #define MBC_S_SW			    (MBC_L_ART + MBC_S_ART)				// Index Start Softwareversion
    ' #define MBC_SW_1			    sys_array[MBC_S_SW]					// Softwareversion Byte 3
    ' #define MBC_SW_2			    sys_array[MBC_S_SW + 1]				// Softwareversion Byte 2
    ' #define MBC_SW_3			    sys_array[MBC_S_SW + 2]				// Softwareversion Byte 1

    Public MBC_L_SW = 3                                                 ' Laenge der Softwareversion
    Public MBC_S_SW = (MBC_S_ART + MBC_L_ART)                           ' Index Start Softwareversion

    ' #define MBC_L_HW			    6									// Laenge Hardwareversion
    ' #define MBC_S_HW			    (MBC_L_SW + MBC_S_SW)				// Index Start Hardwareversion
    ' #define MBC_HW_1			    sys_array[MBC_S_HW]					// Hardwareversion Byte 6
    ' #define MBC_HW_2			    sys_array[MBC_S_HW + 1]				// Hardwareversion Byte 5
    ' #define MBC_HW_3			    sys_array[MBC_S_HW + 2]				// Hardwareversion Byte 4
    ' #define MBC_HW_4			    sys_array[MBC_S_HW + 3]				// Hardwareversion Byte 3
    ' #define MBC_HW_5			    sys_array[MBC_S_HW + 4]				// Hardwareversion Byte 2
    ' #define MBC_HW_6			    sys_array[MBC_S_HW + 5]				// Hardwareversion Byte 1

    Public MBC_L_HW = 6                                                 ' Laenge der Hardwareversion
    Public MBC_S_HW = (MBC_S_SW + MBC_L_SW)                             ' Index Start Hardwareversion

    ' #define MBC_L_NAMEBLOCK		20									// Laenge des Modulnamens
    ' #define MBC_S_NAMEBLOCK		(MBC_L_HW + MBC_S_HW)				// Index Start Modulname
    ' #define MBC_NAME			    sys_array[MBC_S_NAMEBLOCK]			// Name des Moduls

    Public MBC_L_NAMEBLOCK = 20                                         ' Laenge des Modulnamens
    Public MBC_S_NAMEBLOCK = (MBC_L_HW + MBC_S_HW)                      ' Index Start Modulname  

    ' #define MBC_INFO_ENDE		    (MBC_L_NAMEBLOCK + MBC_S_NAMEBLOCK)	// Ende Neuanmeldeblock

    Public MBC_INFO_ENDE = (MBC_L_NAMEBLOCK + MBC_S_NAMEBLOCK - 1)      ' Ende Neuanmeldeblock

    ' #define MBC_L_GUID			4									// Laenge der GuiD
    ' #define MBC_S_GUID			(MBC_L_NAMEBLOCK + MBC_S_NAMEBLOCK)	// Index Start GUiD
    ' #define MBC_UiD_1			    sys_array[MBC_S_GUID]				// GUiD Byte 4
    ' #define MBC_UiD_2			    sys_array[MBC_S_GUID + 1]			// GUiD Byte 3
    ' #define MBC_UiD_3			    sys_array[MBC_S_GUID + 2]			// GUiD Byte 2
    ' #define MBC_UiD_4			    sys_array[MBC_S_GUID + 3]			// GUiD Byte 1

    Public MBC_L_GUID = 4                                               ' Laenge der GUiD
    Public MBC_S_GUID = (MBC_L_NAMEBLOCK + MBC_S_NAMEBLOCK)             ' Index Start GUiD

    ' #define MBC_L_DB			    12									// Laenge der Datenbankversion
    ' #define MBC_S_DB			    (MBC_L_GUID + MBC_S_GUID)			// Index Start Datenbankversion
    ' #define MBC_DB				sys_array[MBC_S_DB]					// Datenbanknummer des PC

    Public MBC_L_DB = 12                                                ' Laenge der Datenbankversion
    Public MBC_S_DB = (MBC_L_GUID + MBC_S_GUID)                         ' Index Start Datenbankversion

    ' #define MBC_L_KN			    5									// Laenge CS2-Geraetekennung
    ' #define MBC_S_KN			    (MBC_L_DB + MBC_S_DB)				// Index Start CS2-Geraetekennung
    ' #define MBC_KN_H			    sys_array[MBC_S_KN]					// CS2-Geraetekennung HIGH
    ' #define MBC_KN_L			    sys_array[MBC_S_KN + 1]				// CS2-Geraetekennung LOW
    ' #define MBC_AK_H			    sys_array[MBC_S_KN + 2]				// CS2-Autokennung HIGH
    ' #define MBC_AK_L			    sys_array[MBC_S_KN + 3]				// CS2-Autokennung LOW
    ' #define MBC_CS2_GER			sys_array[MBC_S_KN + 4]				// CS2-Geraetegruppe

    Public MBC_L_KN = 5                                                 ' Laenge CS2-Geraetekennung
    Public MBC_S_KN = (MBC_L_DB + MBC_S_DB)                             ' Index Start CS2-Geraetekennung
    Public MBC_KN_H = MBC_S_KN                                          ' CS2-Geraetekennung HIGH
    Public MBC_KN_L = MBC_S_KN + 1                                      ' CS2-Geraetekennung HIGH
    Public MBC_AK_H = MBC_S_KN + 2                                      ' CS2-Autokennung HIGH
    Public MBC_AK_L = MBC_S_KN + 3                                      ' CS2-Autokennung LOW
    Public MBC_CS2_GER = MBC_S_KN + 4                                   ' CS2-Geraetegruppe

    ' #define MBC_L_PARA			7									// Laenge Parametersatz
    ' #define MBC_S_PARA			(MBC_L_KN + MBC_S_KN)				// Index Start Parametersatz
    ' #define MBC_PLUGGED			sys_array[MBC_S_PARA]				// Steckernetzteil gesteckt
    ' #define MBC_SPG_1			    sys_array[MBC_S_PARA + 1]			// Spannung 10er Digit
    ' #define MBC_SPG_2			    sys_array[MBC_S_PARA + 2]			// Spannung 1er Digit
    ' #define MBC_SPG_3			    sys_array[MBC_S_PARA + 3]			// Spannung 0.1er Digit
    ' #define MBC_TMP_1			    sys_array[MBC_S_PARA + 4]			// Temperatur 10er Digit
    ' #define MBC_TMP_2			    sys_array[MBC_S_PARA + 5]			// Temperatur 1er Digit
    ' #define MBC_TMP_3			    sys_array[MBC_S_PARA + 6]			// Temperatur 0.1er Digit

    Public MBC_L_PARA = 7                                               ' Laenge des Parametersatzes
    Public MBC_S_PARA = (MBC_L_KN + MBC_S_KN)                           ' Index Start Parametersatz
    Public MBC_PLUGGED = MBC_S_PARA                                     ' Index Steckernetzteil gesteckt ja/nein
    Public MBC_SPG_1 = MBC_S_PARA + 1                                   ' Index Spannung 10er Digit
    Public MBC_SPG_2 = MBC_S_PARA + 2                                   ' Index Spannung 1er Digit
    Public MBC_SPG_3 = MBC_S_PARA + 3                                   ' Index Spannung 0.1er Digit
    Public MBC_TMP_1 = MBC_S_PARA + 4                                   ' Index Temperatur 10er Digit
    Public MBC_TMP_2 = MBC_S_PARA + 5                                   ' Index Temperatur 1er Digit
    Public MBC_TMP_3 = MBC_S_PARA + 6                                   ' Index Temperatur 0.1er Digit

    ' #define MBC_ALLG_ENDE		(MBC_L_PARA + MBC_S_PARA)			    // Ende Allgemeinblock

    Public MBC_ALLG_ENDE = (MBC_L_PARA + MBC_S_PARA - 1)                ' Ende Allgemeinblock 

    ' #define MBC_BOOT_START    (MBC_L_PARA + MBC_S_PARA)               // Anfang des BOOT-Bereiches
    ' #define MBC_BOOT_PGSZ     256                                     // Groesse der EEPROM-PAGE
    ' #define MBC_BOOT_PGMAX    (MBC_BOOT_PGSZ / 4)                     // Anzahl der Pakete fuer die BOOT-Array-Uebertragung
    ' #define MBC_L_BOOT		(2 + MBC_BOOT_PGSZ)                     // Laenge BOOT-Array
    ' #define MBC_S_BOOT		MBC_BOOT_START                          // Index Start BOOT-Array
    ' #define MBC_BOOT_ARRAY	sys_array[MBC_BOOT_START]               // BOOT-Array
    ' #define MBC_BOOT_ENDE     (MBC_L_BOOT + MBC_S_BOOT - 1)           // Ende des BOOT-Bereiches

    Public MBC_BOOT_START = MBC_L_PARA + MBC_S_PARA                     ' Index Start BOOT-Array

    Public MBC_BOOT_PGSZ = 256                                          ' BOOT-EEPROM PAGE-Size
    Public MBC_BOOT_PGMAX = (MBC_BOOT_PGSZ / 4)                         ' BOOT-EEPROM Anzahl der Msg-Pakete
    Public MBC_L_BOOT = (2 + MBC_BOOT_PGSZ)                             ' Laenge BOOT-EEPROM (2x PAGE + PAGE-Size)
    Public MBC_S_BOOT = MBC_L_PARA + MBC_S_PARA                         ' Index Start BOOT-EEPROM
    Public MBC_BOOT_ARRAY = MBC_S_BOOT                                  ' Index Start BOOT-Array

    Public MBC_BOOT_ENDE = (MBC_L_BOOT + MBC_S_BOOT - 1)                ' Index Ende BOOT-Array

    '**********************************************************************************************
    '*** OTA-Header fuer RP2040/ESP32 (siehe MbcOta.h in MBCANLib - Gegenstueck auf dem Modul)  ***
    '*** Wird nach der letzten Firmware-Page zusaetzlich auf MBC_OTA_HEADER_PAGE geschrieben,   ***
    '*** damit MbcOta::apply() beim naechsten Reset ein vollstaendiges Image erkennt.           ***
    '**********************************************************************************************

    Public MBC_OTA_MAGIC As UInteger = &H544F424DUI                     ' "MBOT" als little-endian uint32
    Public MBC_OTA_HEADER_PAGE As Integer = 32767                       ' letzte Page eines W25Q64

    '**********************************************************************************************
    '*** ESP32-C3-OTA-App-Partitionsgroesse (siehe partitions.csv in den ESP32-C3-Beispiel-     ***
    '*** Sketches, ota_0/ota_1) - kleiner als die W25Q64-Kapazitaet oben und damit die          ***
    '*** tatsaechlich bindende Obergrenze fuer ein .bin-Firmware-Image auf diesem Board. RP2040 ***
    '*** hat KEIN Gegenstueck (Groesse haengt vom Tools -> Flash Size-Menue beim Kompilieren ab,***
    '*** dem PC-Tool zur Laufzeit unbekannt) - dort bleibt die W25Q64-Kapazitaet oben die einzig***
    '*** pruefbare Grenze.                                                                      ***
    '**********************************************************************************************

    Public MBC_OTA_ESP32C3_PARTITION_MAX As Integer = &H1F0000          ' ota_0/ota_1 je 1.966.080 Byte

    '**********************************************************************************************
    '*** Sonderindizes fuer allgemeime Steuerung der EEPROM-Programmierung                      ***
    '**********************************************************************************************

    '#define MBC_EEPROM		        0xfffd								// Sonderindex EEPROM-Block schreiben
    '#define MBC_READ			    0xfffe	                            // Sonderindex Lesen
    '#define MBC_WRITE			    0xffff	                            // Sonderindex Schreiben

    Public MBC_EEPROM = &HFFFD                                          ' Sonderindex EEPROM-Block schreiben
    Public MBC_EEPROM_Msg = "FD"                                        ' Sonderindex EEPROM-Block schreiben
    Public MBC_READ = &HFFFE                                            ' Sonderindex Lesen
    Public MBC_READ_Msg = "FE"                                          ' Sonderindex Lesen
    Public MBC_WRITE = &HFFFF                                           ' Sonderindex Schreiben
    Public MBC_WRITE_Msg = "FF"                                         ' Sonderindex Schreiben

    '**********************************************************************************************
    '*** Sonderindizes fuer allgemeime Steuerung der EEPROM-Programmierung                      ***
    '**********************************************************************************************

    ' #define EE_NAME			    0x01                                // ID Block Name
    ' #define EE_GUID			    0x02                                // ID Block GUiD
    ' #define EE_KENNUNG		    0x03                                // ID Block Geraetekennung
    ' #define EE_DB			        0x04                                // ID Block Datenbank
    ' #define EE_UPGRADE            0x05                                // ID Block Upgrade

    Public EE_NAME = "01"                                               ' ID Block Name
    Public EE_GUID = "02"                                               ' ID Block GUiD
    Public EE_KENNUNG = "03"                                            ' ID Block Geraetekennung
    Public EE_DB = "04"                                                 ' ID Block Datenbank
    Public EE_UPGRADE = "05"                                            ' ID Block Datenbank

    '**********************************************************************************************
    '*** Sonderspeicherbereich fuer Funktionen der Module Die Sonderbelegungen sind in den      ***
    '*** VB-Modulen definiert. Hier ist nur der gesamte Speicher indiziert.                     ***
    '**********************************************************************************************

    ' #define MBC_L_GENPURP		    2048								// Laenge des General Purpose Blocks
    ' #define MBC_S_GENPURP		    (MBC_L_BOOT + MBC_S_BOOT)	        // Index Start General Purpose Blocks
    ' #define MBC_GENPURP			sys_array[MBC_S_GENPURP]			// General Purpose Blocks

    Public MBC_L_GENPURP = 2048                                         ' Laenge des General Purpose Blocks
    Public MBC_S_GENPURP = (MBC_L_BOOT + MBC_S_BOOT)                    ' Index Start General Purpose Blocks

    '**********************************************************************************************
    '*** Abschluss des Systemarrays                                                             ***
    '**********************************************************************************************

    ' #define MBC_ARRAY_ENDE        (MBC_L_GENPURP + MBC_S_GENPURP)	    // Ende des belegten Systemarrays

    Public MBC_ARRAY_ENDE = (MBC_L_GENPURP + MBC_S_GENPURP)             ' Ende des belegten Systemarrays

    '**********************************************************************************************
    '*** CAN-Befehlsnummern PC-Kommunikation initialisieren                                     ***
    '**********************************************************************************************

    ' #define PC_DB_H					0x00	// PC - Datenbanknummer HIGH
    ' #define PC_DB_M					0x01	// PC - Datenbanknummer MID
    ' #define PC_DB_L					0x02	// PC - Datenbanknummer LOW
    ' #define PC_KENNER					0x03	// PC - Geraetekenner und Identifier
    ' #define PC_NEU					0x04	// PC - Neuanmeldungsanforderung des PC
    ' #define PC_NEU_DATA				0x05	// PC - Neuanmeldungskanal des PC
    ' #define MD_NEU_DATA				0x06	// MD - Neuanmeldungskanal des Moduls
    ' #define PC_RESET					0x07	// PC - Reset durch PC
    ' #define PC_MD_DEL					0x08	// PC - Modul wurde aus Datenbank geloescht
    ' #define PC_ALIVE					0x09	// PC - Alivemeldung durch PC angefordert
    ' #define MD_ALIVE					0x0A	// MD - Acknowledge des Moduls auf PC_ALIVE
    ' #define PC_ARRAY					0x0B	// PC - Anfordern, auf das Systemarray des Moduls zuzugreifen
    ' #define MD_ARRAY					0x0C	// MD - Acknowledge des Moduls auf PC_ARRAY
    ' #define PC_ARRAY_DATA				0x0D	// PC - Schreiben/Lesen und ggf. Wert und Systemarrayindex uebergeben
    ' #define MD_ARRAY_DATA				0x0E	// MD - Ack des Moduls auf PC_ARRAY_DATA und Wert aus dem Systemarray uebergeben
    ' #define PC_UPGRADE				0x0F	// PC - Anfordern, auf das Systemarray des Moduls zuzugreifen
    ' #define MD_UPGRADE				0x10	// MD - Acknowledge des Moduls auf PC_UPGRADE
    ' #define PC_UPGRADE_DATA			0x11	// PC - Schreiben/Lesen und ggf. Wert und Systemarrayindex uebergeben
    ' #define MD_UPGRADE_DATA			0x12	// MD - Ack des Moduls auf PC_UPGRADE_DATA und Wert aus dem Systemarray uebergeben
    ' #define PC_BOOT					0x13	// PC - Anfordern, auf das Systemarray des Moduls zuzugreifen
    ' #define MD_S88					0x14	// MD - S88-Stellungsmeldung

    Public PC_DB_H = "00"         ' PC - Datenbanknummer HIGH
    Public PC_DB_M = "01"         ' PC - Datenbanknummer MID
    Public PC_DB_L = "02"         ' PC - Datenbanknummer LOW
    Public PC_KENNER = "03"       ' PC - Geraetekenner und Identifier
    Public PC_NEU = "04"          ' PC - Neuanmeldungsanforderung des PC
    Public PC_NEU_DATA = "05"     ' PC - Neuanmeldungskanal des PC
    Public MD_NEU_DATA = "06"     ' MD - Neuanmeldungskanal des Moduls
    Public PC_RESET = "07"        ' PC - Reset durch PC
    Public PC_MD_DEL = "08"       ' PC - Modul aus Datenbank geloescht
    Public PC_ALIVE = "09"        ' PC - Alivemeldung durch PC angefordert
    Public MD_ALIVE = "0A"        ' MD - Acknowledge des Moduls auf PC_ALIVE
    Public PC_ARRAY = "0B"        ' PC - Anfordern, auf das Systemarray des Moduls zuzugreifen
    Public MD_ARRAY = "0C"        ' MD - Acknowledge des Moduls auf PC_ARRAY
    Public PC_ARRAY_DATA = "0D"   ' PC - Schreiben/Lesen und ggf. Wert und Systemarrayindex uebergeben
    Public MD_ARRAY_DATA = "0E"   ' MD - Ack des Moduls auf PC_ARRAY_DATA und Wert aus dem Systemarray uebergeben
    Public PC_UPGRADE = "0F"      ' PC - Anfordern, auf das Systemarray des Moduls zuzugreifen
    Public MD_UPGRADE = "10"      ' MD - Acknowledge des Moduls auf PC_UPGRADE
    Public PC_UPGRADE_DATA = "11" ' PC - Schreiben/Lesen und ggf. Wert und Systemarrayindex uebergeben
    Public MD_UPGRADE_DATA = "12" ' MD - Ack des Moduls auf PC_UPGRADE_DATA und Wert aus dem Systemarray uebergeben
    Public PC_BOOT = "13"         ' PC - Anfordern, auf das Systemarray des Moduls zuzugreifen
    Public MD_S88 = "14"          ' MD - S88-Stellungsmeldung

    '**********************************************************************************************
    '*** Versionsinfos initialisieren                                                           ***
    '**********************************************************************************************

    '**********************************************************************************************
    '*** CAN-Nachrichten fuer die PC-Kommunikation initialisieren                               ***
    '**********************************************************************************************

    Public MsgDatenbankHIGH = "00 16 5F 38 08 00 00 18 " & PC_DB_H & " "
    Public MsgDatenbankMID = "00 16 5F 38 08 00 00 18 " & PC_DB_M & " "
    Public MsgDatenbankLOW = "00 16 5F 38 08 00 00 18 " & PC_DB_L & " "
    Public MsgKenner = "00 16 5F 38 08 00 00 18 " & PC_KENNER & " 00 "
    Public MsgNeuanmeldungsping = "00 16 5F 38 08 00 00 18 " & PC_NEU & " 00 00 00 00"
    Public MsgNeuanmeldekanal = "00 16 5F 38 08 00 00 18 " & PC_NEU_DATA & " 00 00 "
    Public MsgReset = "00 16 5F 38 08 00 00 18 " & PC_RESET & " "
    Public MsgMDDel = "00 16 5F 38 08 00 00 18 " & PC_MD_DEL & " "
    Public MsgAlive = "00 16 5F 38 08 00 00 18 " & PC_ALIVE & " "
    Public MsgIORequest = "00 16 5F 38 08 00 00 18 " & PC_ARRAY & " "
    Public MsgIOAct = "00 16 5F 38 08 00 00 18 " & PC_ARRAY_DATA & " "
    Public MsgIOEEPROM = "00 16 5F 38 08 00 00 18 " & PC_ARRAY_DATA & " 01 FF " & MBC_EEPROM_Msg & " "
    Public MsgUPRequest = "00 16 5F 38 08 00 00 18 " & PC_UPGRADE & " "
    Public MsgUPAct = "00 16 5F 38 08 00 00 18 " & PC_UPGRADE_DATA & " "
    Public MsgBOOT = "00 16 5F 38 08 00 00 18 " & PC_BOOT & " "
    Public MsgSend As String

    '**********************************************************************************************
    '*** CAN-Befehlsadressen (LocID) fuer PC-Kommunikation initialisieren                       ***
    '**********************************************************************************************

    Public B_PC_NEU = Val("&H18" & PC_NEU)
    Public B_PC_NEU_DATA = Val("&H18" & PC_NEU_DATA)
    Public B_MD_NEU_DATA = Val("&H18" & MD_NEU_DATA)
    Public B_PC_RESET = Val("&H18" & PC_RESET)
    Public B_PC_MD_DEL = Val("&H18" & PC_MD_DEL)
    Public B_PC_ALIVE = Val("&H18" & PC_ALIVE)
    Public B_MD_ALIVE = Val("&H18" & MD_ALIVE)
    Public B_PC_ARRAY = Val("&H18" & PC_ARRAY)
    Public B_MD_ARRAY = Val("&H18" & MD_ARRAY)
    Public B_PC_ARRAY_DATA = Val("&H18" & PC_ARRAY_DATA)
    Public B_MD_ARRAY_DATA = Val("&H18" & MD_ARRAY_DATA)
    Public B_PC_UPGRADE = Val("&H18" & PC_UPGRADE)
    Public B_MD_UPGRADE = Val("&H18" & MD_UPGRADE)
    Public B_PC_UPGRADE_DATA = Val("&H18" & PC_UPGRADE_DATA)
    Public B_MD_UPGRADE_DATA = Val("&H18" & MD_UPGRADE_DATA)
    Public B_MD_S88 = Val("&H18" & MD_S88)

    '**********************************************************************************************
    '*** CAN-Befehle, Nachrichten und Indices fuer CS2-Kommunikation initialisieren             ***
    '**********************************************************************************************

    ' CS2-Schaltbefehl
    Public MsgSwitchCS2 = "00 16 5F 38 06 00 00 "

    ' CS2-Geraetekennung
    Public CS2_Kenner As String = "AAAA"
    Public CS2_S88_Kenner As String = "FFFF"
    Public CS2_Identifier As String = "6D"

    '**********************************************************************************************
    '*** CAN-Befehlsstruktur                                                                    ***
    '**********************************************************************************************

    ' Variablen Befehlauswertung
    Structure Befehle
        Dim ID As Integer
        Dim Count As Integer
        Dim CanID As String
        Dim Response As Integer
        Dim Command As String
        Dim D0 As String
        Dim D1 As String
        Dim D2 As String
        Dim D3 As String
        Dim D4 As String
        Dim D5 As String
        Dim D6 As String
        Dim D7 As String
        Dim Msg As String
    End Structure

    Public Befehl As Befehle

    Public NachrichtenMax As Integer = &H44

    Public Nachrichten As String() = {"Befehl:  System-Befehl Loc-ID: ",
                                      "Antwort: System-Befehl Loc-ID: ",
                                      "Befehl:  Lok Discovery ",
                                      "Antwort: Lok Discovery ",
                                      "Befehl:  MFX Bind ",
                                      "Antwort: MFX Bind ",
                                      "Befehl:  MFX Verify ",
                                      "Antwort: MFX Verify ",
                                      "Befehl:  Lok Speed ",
                                      "Antwort: Lok Speed ",
                                      "Befehl:  Lok Richtung ",
                                      "Antwort: Lok Richtung ",
                                      "Befehl:  Lok Funktion ",
                                      "Antwort: Lok Funktion ",
                                      "Befehl:  Read Config ",
                                      "Antwort: Read Config ",
                                      "Befehl:  Write Config ",
                                      "Antwort: Write Config ",
                                      "Befehl:  unknown          ",
                                      "Antwort: unknown          ",
                                      "Befehl:  unknown          ",
                                      "Antwort: unknown          ",
                                      "Befehl:  ",
                                      "Antwort: ",
                                      "Befehl:  Zubehör Konfig ",
                                      "Antwort: Zubehör Konfig ",
                                      "Befehl:  unknown          ",
                                      "Antwort: unknown          ",
                                      "Befehl:  unknown          ",
                                      "Antwort: unknown          ",
                                      "Befehl:  unknown          ",
                                      "Antwort: unknown          ",
                                      "Befehl:  s88 Polling ",
                                      "Antwort: s88 Polling ",
                                      "Befehl:  s88 Event ",
                                      "Antwort: s88 Event ",
                                      "Befehl:  SX1 Event ",
                                      "Antwort: SX1 Event ",
                                      "Befehl:  unknown          ",
                                      "Antwort: unknown          ",
                                      "Befehl:  unknown          ",
                                      "Antwort: unknown          ",
                                      "Befehl:  unknown          ",
                                      "Antwort: unknown          ",
                                      "Befehl:  unknown          ",
                                      "Antwort: unknown          ",
                                      "Befehl:  unknown          ",
                                      "Antwort: unknown          ",
                                      "Befehl:  Ping-Anfrage ",
                                      "Antwort: Ping-Anfrage ",
                                      "Befehl:  Uodate-Anfrage ",
                                      "Antwort: Update-Anfrage ",
                                      "Befehl:  Read Config Data ",
                                      "Antwort: Read Config Data ",
                                      "Befehl:  Bootloader CAN ",
                                      "Antwort: Bootloader CAN ",
                                      "Befehl:  Bootloader GLEIS ",
                                      "Antwort: Bootloader GLEIS ",
                                      "Befehl:  Statusdaten Konf ",
                                      "Antwort: Statusdaten Konf ",
                                      "Befehl:  Konf Daten ",
                                      "Antwort: Konf Daten ",
                                      "Befehl:  Konf Datastream ",
                                      "Antwort: Konf Datastream "}

    Public NachrichtenSystemBefehleMax As Integer = &HD

    Public NachrichtenSystemBefehle As String() = {"Cmd: System Stopp",
                                                   "Cmd: System Go",
                                                   "Cmd: System Halt",
                                                   "Cmd: Lok Nothalt",
                                                   "Cmd: Lok Zyklus beenden",
                                                   "Cmd: Lok Datenprotokoll",
                                                   "Cmd: Schaltzeit Zubehör",
                                                   "Cmd: Fast Read SID",
                                                   "Cmd: Gleisprotokoll",
                                                   "Cmd: MFX Neuanmeldezähler",
                                                   "Cmd: System Überlast",
                                                   "Cmd: System Status",
                                                   "Cmd: Gerätekennung"}

    Public NachrichtenSystemBefehle0x30 As String = "Cmd: MFX Seek"
    Public NachrichtenSystemBefehle0x80 As String = "Cmd: System Reset"

    Public MsgPRIO = 1
    Public MsgID = 4
    Public MsgHashH = 7
    Public MsgHashL = 10
    Public MsgDLC = 13
    Public MsgD0 = 16
    Public MsgD1 = 19
    Public MsgD2 = 22
    Public MsgD3 = 25
    Public MsgD4 = 28
    Public MsgD5 = 31
    Public MsgD6 = 34
    Public MsgD7 = 37

    Public NachrichtenMBCANMax = &H15

    Public NachrichtenMBCAN As String() = {"PC_DB_H", "PC_DB_M", "PC_DB_L", "PC_KENNER", "PC_NEU", "PC_NEU_DATA",
                                            "MD_NEU_DATA", "PC_RESET", "PC_MD_DEL", "PC_ALIVE", "MD_ALIVE", "PC_ARRAY",
                                            "MD_ARRAY", "PC_ARRAY_DATA", "MD_ARRAY_DATA", "PC_UPGRADE", "MD_UPGRADE",
                                            "PC_UPGRADE_DATA", "MD_UPGRADE_DATA", "PC_BOOT", "MD_S88", "MD_ZYKLUS"}

    '**********************************************************************************************
    '*** Modul- und Systemarray                                                                 ***
    '**********************************************************************************************

    '**********************************************************************************************
    '*** Klassen-Modell                                                                         ***
    '**********************************************************************************************

    Public MBC As New mbc_system_cl

    ' Modulvariablen

    Public MaxNr As Integer = 256
    Public MaxTyp As Integer = 14
    Public AktNr As Integer
    Public AktTyp As Integer
    Public ModulAliveGUiD As String
    Public DBVersion As String
    Public SelNode As TreeNode
    'Public Datum_Upgrade = 180101


    ' Bilderlisten

    Public MenueImage As New ImageList
    Public ImROT As Image
    Public ImGRUEN As Image
    Public ImGRAU As Image
    Public ImGELB As Image
    Public ImGLEIS As Image
    Public ImMODUL As Image
    Public ImHAKEN As Image
    Public ImPUNKT As Image
    Public ImKREUZ As Image
    Public ImDELETE As Image
    Public ImImpuls As Image
    Public ImWechsel As Image
    Public ImStartUp As Image
    Public ImX As Image

    ' Upgrade

    Public UpgradeBuffer() As String
    Public UpgradePageMax As Integer    ' Maximale Anzahl der Pages des Upgrades
    Public UpgradePage As Integer       ' Aktuelle Page die geschrieben wird
    Public UpgradeHash As Integer       ' Paketindex als HASH

    '**********************************************************************************************
    '*** Automatenindizes                                                                       ***
    '**********************************************************************************************

    ' Read/Write-Indizes

    Public IOStart = 0              ' Startindex fuer den Datentransfer
    Public IOIndex = 0              ' Aktueller Index fuer den Datentransfer
    Public IOMax = 0                ' Maximalindex fuer den Datentransfer
    Public IOEEPROM = "00"

    ' Watchdog fuer den Lese-/Schreibautomaten (COMMsgSendAndArm()/Timer_ComTimeout_Tick in
    ' frmMain.ComProtocol.vb/frmMain.vb) - ohne Antwort im Bus-Traffic verlorene Anfragen werden
    ' sonst nie wiederholt, der Automat haengt dann dauerhaft

    Public LastIOMsg As String = ""             ' zuletzt gesendete Automaten-Anfrage, fuer Retry
    Public IOTimeoutRetryCount As Integer = 0   ' Retry-Zaehler fuer den aktuellen Schritt
    Public Const IOTIMEOUT_RETRIES_MAX As Integer = 5

    ' Modulstatus

    Public Const IsComplete = 2
    Public Const IsAlive = 3
    Public Const IsMissing = 4
    Public Const IsDeleted = 5

    ' Flags und Indizes zur Steuerung des Prozessablaufs

    Public FLAG_Neu_Busy As Boolean = False
    Public FLAG_Alive_Start As Boolean = False
    Public FLAG_Alive_Request As Boolean = False
    Public FLAG_Alive_Ack As Boolean = False
    Public FLAG_Read_Request As Boolean = False
    Public FLAG_Read_Request_Active As Boolean = False
    Public FLAG_Read_IO As Boolean = False
    Public FLAG_Write_Request As Boolean = False
    Public FLAG_Write_Request_Active As Boolean = False
    Public FLAG_Write_IO As Boolean = False
    Public FLAG_Write_Program As Boolean = False
    Public FLAG_Write_Complete As Boolean = False
    Public FLAG_Write_Refresh As Boolean = False
    Public FLAG_Edit_Enable As Boolean = False
    Public FLAG_Prog_Index As Integer
    Public FLAG_Upgrade_Request As Boolean = False
    Public FLAG_Upgrade_IO As Boolean = False
    Public FLAG_Upgrade_NeedsOtaHeader As Boolean = False   ' RP2040/ESP32: nach der letzten
                                                             ' Firmware-Page zusaetzlich die
                                                             ' MbcOta-Header-Page senden
    Public FLAG_Upgrade_SendingOtaHeader As Boolean = False ' True waehrend die Header-Page
                                                             ' selbst uebertragen wird
    Public FLAG_Refresh_Acomplished As Boolean = False

    ' Erkennung "eine CSx ist am Bus": eine Ping-ANFRAGE (CAN_ID = 0x30) wird nur von einer
    ' Zentrale gesendet, nie von einem Zubehoermodul - ihr Empfang genuegt als Nachweis. Wird in
    ' COMMsgReceived() bei jedem Ping auf True gesetzt UND per CSxLastPingUtc mit einem
    ' Zeitstempel versehen. Bleibt eine Zeit lang (CSX_PING_TIMEOUT_SEC) ohne weiteren Ping, wertet
    ' Timer_Alive_Tick() das als "CSx nicht mehr am Bus" und setzt das Flag automatisch wieder
    ' zurueck (siehe Register_Modul_CSxLock_Apply()). Sperrt darueber tab_Module (alle
    ' modulspezifischen Eingabefelder) - Stellpult/Buttons bleiben davon unberuehrt (folgen
    ' weiterhin nur FLAG_Edit_Enable, siehe Register_Modul_Edit_Enable()).

    Public FLAG_CSxPingEmpfangen As Boolean = False
    Public CSxLastPingUtc As DateTime = DateTime.MinValue

    ' Zeitspanne ohne weiteren CSx-Ping, nach der die Sperre automatisch wieder aufgehoben wird

    Public Const CSX_PING_TIMEOUT_SEC As Double = 60

    ' Konstanten fuer die Zuordung der Updateanfrage Programm-Button/Verwerfen-Button

    Public PROG_INDEX_Upgrade = 21

    '**********************************************************************************************
    '*** Schnittstellenvariablen fuer RS232, Bluetooth und UDP                                  ***
    '**********************************************************************************************

    ' Variablen RS232
    Public RS232Client As New System.IO.Ports.SerialPort
    Public RS232ThreadReceive As System.Threading.Thread
    Public RS232Port As String = "-"
    Public RS232BaudRate As String = "250000"
    Public RS232IsInit As Boolean = False
    Public RS232ThreadIsActive As Boolean = False
    Public RS232Log As Integer = 1

    ' Variablen Bluetooth
    Public BTClient As New System.IO.Ports.SerialPort
    Public BTPort As String = "-"
    Public BTVirtualBaudRate As String = "2400"
    Public BTBaudRate As String = "230400"
    Public BTThreadReceive As System.Threading.Thread
    Public BTIsInit As Boolean = False
    Public BTThreadIsActive As Boolean = False
    Public BT_Name_TXT As String
    Public BTLog As Integer = 2

    ' Variablen MBC-TCP

    Public TCPRemoteIpEndPoint As New System.Net.IPEndPoint(System.Net.IPAddress.Any, 0)
    Public TCPThreadReceive As System.Threading.Thread
    Public TCPSocketNo As Integer = 15731
    Public TCPIP As String = "0.0.0.0"
    Public TCPThreadIsActive As Boolean = False
    Public TCPIsInit As Boolean = False
    Public TCPLog As Integer = 4
    Public TCPstream As NetworkStream
    Public TCPclient As New TcpClient

    ' Variablen Logfile
    Public COMRecFile As FileStream
    Public COMRecFileWr As StreamWriter
    Public COMState As New Object
    Public COMPfad As String
    Public COMMsgCount As Integer


End Module
