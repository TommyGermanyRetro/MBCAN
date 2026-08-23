'**************************************************************************************************
'*** Register - Modul - Einstellung Sprache                                                     ***
'**************************************************************************************************

Module Sprache

    '**********************************************************************************************
    '*** Variablen                                                                              ***
    '**********************************************************************************************

    Public SprachIndex As Integer                           ' gewaehlte Sprache

    ' ResourceManager je Sprache (1=Deutsch, 2=Franzoesisch, 3=Englisch), lazy angelegt

    Private _rmDE As System.Resources.ResourceManager
    Private _rmFR As System.Resources.ResourceManager
    Private _rmEN As System.Resources.ResourceManager

    '**********************************************************************************************
    '*** Sprachtext lesen (ersetzt das frueher CSV-befuellte Sprachen(50,100)-Array)            ***
    '*** Aufrufsyntax bleibt unveraendert Sprachen(Hauptindex, Detailindex) - VB unterscheidet  ***
    '*** nicht zwischen Array- und Funktionsaufruf, daher keine Aenderung an den ~377           ***
    '*** Aufrufstellen                                                                          ***
    '**********************************************************************************************
    Public Function Sprachen(Hauptindex As Integer, Detailindex As Integer) As String
        Dim rm As System.Resources.ResourceManager

        Select Case SprachIndex
            Case 2
                If _rmFR Is Nothing Then _rmFR = New System.Resources.ResourceManager("MBCAN.SprachenFR", System.Reflection.Assembly.GetExecutingAssembly())
                rm = _rmFR
            Case 3
                If _rmEN Is Nothing Then _rmEN = New System.Resources.ResourceManager("MBCAN.SprachenEN", System.Reflection.Assembly.GetExecutingAssembly())
                rm = _rmEN
            Case Else
                If _rmDE Is Nothing Then _rmDE = New System.Resources.ResourceManager("MBCAN.SprachenDE", System.Reflection.Assembly.GetExecutingAssembly())
                rm = _rmDE
        End Select

        Dim value As String = rm.GetString("R" & Hauptindex & "C" & Detailindex)
        If value Is Nothing Then Return ""
        Return value
    End Function

    ' Zuordnung Text/label zu Haupt- und Nebenindex der Sprachdatei

    Public Const i_MAIN = 1

    Public Const i_Mainfrm = 1                              ' "MBCAN - Parametriercenter für mbc-Module"
    Public Const i_None = 2                                 ' "None"
    Public Const i_stp_TCP_Connect = 3                      ' "Connecting ..."
    Public Const i_stp_TCP_Disconnect = 4                   ' "Disconnecting ..."
    Public Const i_stp_Refresh = 5                          ' "Refresh: -/-"
    Public Const i_stp_Refresh_update_ready = 6             ' "Update: Ready"
    Public Const i_stp_Refresh_ready = 7                    ' "Refresh: Ready"
    Public Const i_stp_Refresh_update_start = 8             ' "Update: Start"
    Public Const i_stp_Refresh_update_stop = 9              ' "Update: Stop"
    Public Const i_stp_Refresh_upgrade_start = 10           ' "Upgrade: Start"
    Public Const i_stp_Refresh_upgrade_ready = 11           ' "Ugrade: Ready"
    Public Const i_stp_Refresh_start = 12                   ' "Refresh: Start"
    Public Const i_stp_Refresh_stop = 13                    ' "Refresh: Stop"
    Public Const i_stp_Alive_None = 14                      ' "Searching: -/-"
    Public Const i_stp_Alive_Active = 15                    ' "Searching: "
    Public Const i_stp_Upgrade_None = 16                    ' "-/-"
    Public Const i_stp_Upgrade_Active = 17                  ' "BOOT-Pages: "
    Public Const i_MenuSystem = 18                          ' "System"
    Public Const i_MenuSystemNeu = 19                       ' "Datenbank erneuern ..."
    Public Const i_MenuSystemSpeichern = 20                 ' "Datenbank speichern ..."
    Public Const i_MenuSystemSchliessen = 21                ' "Beenden"
    Public Const i_MenuVerbindung = 22                      ' "Verbindung"
    Public Const i_MenuVerbindungEinstellen = 23            ' "Einstellen"
    Public Const i_MenuVerbindungVerbinden = 24             ' "Verbinden"
    Public Const i_MenuVerbindungTrennen = 25               ' "Trennen"
    Public Const i_MenuInfo = 26                            ' "Info"
    Public Const i_MenuInfoHilfe = 27                       ' "Über MBCAN ..."
    Public Const i_MenuInfoBedienungsanleitung = 28         ' "Bedienungsanleitung"
    Public Const i_MenuInfoCopyright = 29                   ' "Copyright"
    Public Const i_tab_Modul = 30                           ' "Modul"
    Public Const i_btn_ControlRefresh = 31                  ' "Refresh"
    Public Const i_btn_ControlVerwerfen = 32                ' "Verwerfen"
    Public Const i_btn_ControlVerwerfen_Stop = 33           ' "Stopp"
    Public Const i_btn_ControlUpdate = 34                   ' "Update"
    Public Const i_btn_ControlUpgrade = 35                  ' "Upgrade"
    Public Const i_btn_ControlReset = 36                    ' "Reset"
    Public Const i_btn_ControlLoeschen = 37                 ' "Löschen"
    Public Const i_gb_Parameter = 38                        ' "Parameter"
    Public Const i_lbl_ParameterName = 39                   ' "Name"
    Public Const i_lbl_ParameterDecTyp = 40                 ' "Typ"
    Public Const i_lbl_ParameterArtNum = 41                 ' "Artikelnummer"
    Public Const i_lbl_ParameterSerNr = 42                  ' "Seriennummer"
    Public Const i_lbl_ParameterGUID = 43                   ' "Geräte-UiD"
    Public Const i_lbl_ParameterSW = 44                     ' "SW-Version"
    Public Const i_lbl_ParameterHW = 45                     ' "HW-Version"
    Public Const i_lbl_ParameterPlugged = 46                ' "Plugged"
    Public Const i_lbl_ParameterTemperatur = 47             ' "Temperatur"
    Public Const i_lbl_ParameterSpannung = 48               ' "Spannung"
    Public Const i_gb_Status = 49                           ' "Status"
    Public Const i_gb_Keyboard = 50                         ' "Stellpult"
    Public Const i_btn_KeyboardR = 51                       ' "R"
    Public Const i_btn_KeyboardG = 52                       ' "G"
    Public Const i_tab_Monitor = 53                         ' "Monitor"
    Public Const i_lbl_MonitorNachrichtenherkunft = 54      ' "None"
    Public Const i_lbl_MonitorNachrichtenherkunftRS = 55    ' "CAN-Telegramme auf dem Märklin- und MBCAN-Bus via RS232"
    Public Const i_lbl_MonitorNachrichtenherkunftBT = 56    ' "CAN-Telegramme auf dem Märklin- und MBCAN-Bus via Bluetooth"
    Public Const i_lbl_MonitorNachrichtenherkunftTCP = 57   ' "CAN-Telegramme auf dem Märklin- und MBCAN-Bus via TCP"
    Public Const i_lbl_MonitorInhalt = 58                   ' "Nachrichteninhalt"
    Public Const i_tab_Adressen = 59                        ' "Adressen"
    Public Const i_lbl_AdressenLocID = 60                   ' "Zuordnung Module zu Loc-ID"
    Public Const i_btn_AdressenUebersicht = 61              ' "Übersicht generieren"
    Public Const i_dgv_AdressenColumnLocID = 62             ' "Loc-ID"
    Public Const i_dgv_AdressenColumnLocIDTyp = 63          ' "Typ"
    Public Const i_dgv_AdressenColumnLocIDName = 64         ' "Name"
    Public Const i_dgv_AdressenColumnLocIDIndex = 65        ' "Index"
    Public Const i_lbl_AdressenS88Base = 66                 ' "Zuordnung Module zu Rückmelder"
    Public Const i_dgv_AdressenColumnModNr = 67             ' "Mod-Nr"
    Public Const i_COMGetPlugged_1 = 68                     ' "MBC-Bus-Versorgung"
    Public Const i_COMGetPlugged_2 = 69                     ' "Steckernetzteil"
    Public Const i_ts_MenuSystemSprache = 70                ' "Sprache"
    Public Const i_ts_MenuSystemSprache_Deutsch = 71        ' "Deutsch"
    Public Const i_ts_MenuSystemSprache_Französisch = 72    ' "Französisch"
    Public Const i_ts_MenuSystemSprache_Englisch = 73       ' "Englisch"

    Public Const i_MBC80 = 2

    Public Const i_lbl_mbc80_BTNameBeschreibung = 1         ' "Die Länge des Modulnamens ist mit fünf Buchstaben fest vorgegeben. "
    Public Const i_lbl_mbc80_Name = 2                       ' "Name"
    Public Const i_lbl_mbc80_PasswortBeschreibung = 3       ' "Das Passwort ist numerisch im Bereich 0000 ... 9999 einstellbar."
    Public Const i_lbl_mbc80_Passwort = 4                   ' "Passwort"
    Public Const i_gb_mbc80_AK = 5                          ' "s88-Automatikgerät"
    Public Const i_lbl_mbc80_AK = 6                         ' "Durch CSx zugewiesener Automatik-Kenner"
    Public Const i_gb_mbc80_SW = 7                          ' "L88-Softwareversion"
    Public Const i_lbl_mbc80_SWBeschreibung = 8             ' "Die aktuelle Softwareversion des L88 von Märklin, um die Update-Aufforderung von der CSx zu umgehen."
    Public Const i_lbl_mbc80_SWBezeichnung = 9              ' "L88-SW-Version"

    Public Const i_MBC81 = 3

    Public Const i_lbl_mbc81_LocID = 1                      ' "Loc-ID in HEX"
    Public Const i_lbl_mbc81_Nutzung = 2                    ' "Nutzung des Moduls"
    Public Const i_lbl_mbc81_Adresse2 = 3                   ' "bis"
    Public Const i_lbl_mbc81_Adresse1 = 4                   ' "Adresse"
    Public Const i_lbl_mbc81_Nutzung_Erlaeuterung_1 = 5     ' "Über die Combo-Box kann ausgewählt werden, ob das Modul als MM2-Switchboard genutzt wird (Ersatz für 6040) oder als Halbmodul für den BUS 0 des mbc-80 (L88-Bus-0). Bei den Halbmodulen können von den 64 möglichen Tastern der Matrix entweder die Tasten 1 bis 32 oder 33 bis 64 ausgewählt werden."
    Public Const i_lbl_mbc81_Nutzung_Erlaeuterung_2 = 6     ' "CS2: Die Tasten 1-32 entsprechen den Kontakten 101-132, die Tasten 33-64 den Kontakte 133-164.  "
    Public Const i_lbl_mbc81_Nutzung_Erlaeuterung_3 = 7     ' "CS3: Bei der Einstellung <Tastenmatrix> entsprechen die Kontakte 1-32 und 33-64 den jeweiligen Tasten. "
    Public Const i_lbl_mbc81_Nutzung_1 = 8                  ' "Switchboard"
    Public Const i_lbl_mbc81_Nutzung_2 = 9                  ' "Bus 0 Matrix  1...32"
    Public Const i_lbl_mbc81_Nutzung_3 = 10                 ' "Bus 0 Matrix 33...64"

    Public Const i_MBC82 = 4

    Public Const i_gb_mbc82_GFP = 1                         ' "GFP-Softwareversion"
    Public Const i_lbl_mbc82_GFPBeschreibung_1 = 2          ' "Die aktuelle Softwareversion des GFP/Booster von Märklin, um die Update-Aufforderung von der CSx zu umgehen."
    Public Const i_lbl_mbc82_GFPBeschreibung_2 = 3          ' "Nach dem <Update> das Modul bitte per <Reset> neu starten. Die neue Softwareversion wird dann in der CSx angezeigt."
    Public Const i_lbl_mbc82_GFPBezeichnung = 4             ' "GFP-SW-Version"
    Public Const i_gb_mbc82_Latenz = 5                      ' "Latenzzeit Kurzschlusserkennung"
    Public Const i_lbl_mbc82_LatenzBeschreibung_1 = 6       ' "Die Latenzzeit bis zum Ansprechen der Kurzschlusserkennung kann hier in Schritten von 50 ms eingestellt werden."
    Public Const i_lbl_mbc82_LatenzBeschreibung_2 = 7       ' "Nach dem <Update> des Moduls bitte dieses aus der GUI der CS3 löschen. Der neue Latenzwert wird dann in der CS3 angezeigt."
    Public Const i_lbl_mbc82_LatenzBeschreibung_3 = 8       ' "Bei einer CS2 bitte diesen Wert ausschließlich in der CS2-GUI ändern und über den <Refresh>-Button aktualisieren. "
    Public Const i_lbl_mbc82_LatenzBezeichnung = 9          ' "Latenz [ms]"

    Public Const i_MBC83 = 5

    Public Const i_lbl_mbc83_LocID = 1                      ' "Loc-ID in HEX"
    Public Const i_lbl_mbc83_Dimmer = 2                     ' "Dimmer StartUp"
    Public Const i_lbl_mbc83_AutoAus = 3                    ' "Auto-AUS"
    Public Const i_lbl_mbc83_Meldezyklus = 4                ' "Meldezyklus"
    Public Const i_lbl_mbc83_Impuls = 5                     ' "Impulsausgang"
    Public Const i_lbl_mbc83_Wechsel = 6                    ' "Wechselschalter"
    Public Const i_lbl_mbc83_Startup = 7                    ' "StartUp-Signal"
    Public Const i_gb_mbc83_CH_lbl_Adresse = 8              ' "Adresse"
    Public Const i_gb_mbc83_CH_lbl_typ = 9                  ' "Typ"
    Public Const i_gb_mbc83_CH_clb_Typ_A = 10               ' "Impuls"
    Public Const i_gb_mbc83_CH_clb_Typ_B = 11               ' "Wechsel"
    Public Const i_gb_mbc83_CH_clb_Typ_C = 12               ' "StartUp"
    Public Const i_gb_mbc83_CH_lbl_ON = 13                  ' "ON-Time"
    Public Const i_gb_mbc83_CH1 = 14                        ' "Ausgang 1"
    Public Const i_gb_mbc83_CH2 = 15                        ' "Ausgang 2"
    Public Const i_gb_mbc83_CH3 = 16                        ' "Ausgang 3"
    Public Const i_gb_mbc83_CH4 = 17                        ' "Ausgang 4"

    Public Const i_MBC84 = 6

    Public Const i_lbl_mbc84_LocID = 1                      ' "Loc-ID in HEX"
    Public Const i_lbl_mbc84_Hinweis = 2                    ' "Meldezyklus"
    Public Const i_gb_mbc84_CH1 = 3                         ' "Servo 1"
    Public Const i_gb_mbc84_CH2 = 4                         ' "Servo 2"
    Public Const i_gb_mbc84_CH3 = 5                         ' "Servo 3"
    Public Const i_gb_mbc84_CH4 = 6                         ' "Servo 4"
    Public Const i_lbl_mbc84_adr = 7                        ' "Adresse:"
    Public Const i_lbl_mbc84_Links = 8                      ' "Grün:"
    Public Const i_lbl_mbc84_Rechts = 9                     ' "Rot:"
    Public Const i_btn_mbc84_Weg = 10                       ' "Wegdiagramm"
    Public Const i_lbl_mbc84_Step = 11                      ' "Step:"

    Public Const i_MBC85 = 7

    Public Const i_lbl_mbc85_ModulNr = 1                    ' "ModulNr"
    Public Const i_lbl_mbc85_BusNr = 2                      ' "BusNr"
    Public Const i_lbl_mbc85_Kontakte = 3                   ' "Kontakte CS2"
    Public Const i_gb_mbc85_MMLok = 4                       ' "MM-Lok-Adresse"
    Public Const i_lbl_mbc85_MMLok = 5                      ' "LokAddr"
    Public Const i_lbl_mbc85_MMSpeed = 6                    ' "Speed"
    Public Const i_gb_mbc85_Integrator = 7                  ' "Integrator"
    Public Const i_gb_mbc85_DS_Integrator = 8               ' "Drehscheibe Integrator"
    Public Const i_gb_mbc85_Winkel = 9                      ' "Gleiswinkel"

    Public Const i_MBC86 = 8

    Public Const i_lbl_mbc86_ModulNr = 1                    ' "ModulNr"
    Public Const i_lbl_mbc86_BusNr = 2                      ' "BusNr"
    Public Const i_lbl_mbc86_Kontakte = 3                   ' "Kontakte CS2"
    Public Const i_gb_mbc86_MMLok = 4                       ' "MM-Lok-Adresse"
    Public Const i_lbl_mbc86_MMLok = 5                      ' "LokAddr"

    Public Const i_MBC87 = 9

    Public Const i_lbl_mbc87_1 = 1                          ' "Loc-ID in HEX"
    Public Const i_gb_mbc87_drehen = 2                      ' "Drehen"
    Public Const i_lbl_mbc87_adr_d = 3                      ' "Adresse"
    Public Const i_rb_mbc87_d_rechts = 4                    ' "Rechts"
    Public Const i_rb_mbc87_d_stopp = 5                     ' "Stopp"
    Public Const i_rb_mbc87_d_links = 6                     ' "Links"
    Public Const i_gb_mbc87_hebensenken = 7                 ' "Heben/Senken"
    Public Const i_gb_mbc87_heben = 8                       ' "Heben"
    Public Const i_rb_mbc87_senken = 9                      ' "Senken"
    Public Const i_gb_mbc87_magnet = 10                     ' "Magnet"
    Public Const i_rb_mbc87_m_aus = 11                      ' "Aus"
    Public Const i_rb_mbc87_m_an = 12                       ' "An"

    Public Const i_MBC88 = 10

    Public Const i_lbl_mbc88_modulnr = 1                    ' "ModulNr"
    Public Const i_lbl_mbc88_halbmodul = 2                  ' "Halbmodul"
    Public Const i_lbl_mbc88_busnr = 3                      ' "BusNr"
    Public Const i_lbl_mbc88_kontaktecs2 = 4                ' "Kontakte CS2"
    Public Const i_gb_mbc88 = 5                             ' "Integrator"
    Public Const i_cmb_mbc88_Halbmodul_1 = 6                ' "Integrator"
    Public Const i_cmb_mbc88_Halbmodul_2 = 7                ' "Integrator"

    Public Const i_MBC89 = 11

    Public Const i_gb_mbc89_I2C = 1                         ' "I2C-Bus-Clock"
    Public Const i_lbl_mbc89_takt = 2                       ' "Takt"
    Public Const i_gb_mbc89_Basisadresse = 3                ' "Basisadresse"
    Public Const i_lbl_mbc89_Basisadresse = 4               ' "Keyboard Nr. 1"
    Public Const i_btn_mbc89_Loks = 5                       ' "Lokeigenschaften"
    Public Const i_gb_mbc89_Mapping = 6                     ' "Mapping"
    Public Const i_clb_mbc89_Mapping = 7                    ' "Mapping"
    Public Const i_clb_mbc89_MappingDirect = 8              ' "Mapping & Direkt"
    Public Const i_gb_mbc89_Keyboard = 9                    ' "Keyboard"
    Public Const i_gb_mbc89_Version = 10                    ' "Connect6021-Softwareversion"
    Public Const i_txt_mbc89_alt = 11                       ' "Das Modul mbc-89 bietet derzeit keine Parametrierungsmöglichkeiten!"

    Public Const i_MBC90 = 12

    Public Const i_lbl_mbc90_modulnr = 1                    ' "ModulNr"
    Public Const i_lbl_mbc90_busnr = 2                      ' "BusNr"
    Public Const i_lbl_mbc90_kontaktecs2 = 3                ' "Kontakte CS2"
    Public Const i_gb_mbc90 = 4                             ' "Integrator"

    Public Const i_MBC91 = 13

    Public Const i_lbl_mbc91_MMAddr = 1                     ' "Loc-ID in HEX"
    Public Const i_lbl_mbc91_Rueck = 2                      ' "Meldezyklus"
    Public Const i_lbl_mbc91_Aus = 3                        ' "Auto-AUS"
    Public Const i_lbl_mbc91_CH_Addr = 4                    ' "Adresse"
    Public Const i_lbl_mbc91_CH_Typ = 5                     ' "Typ"
    Public Const i_lbl_mbc91_CH_TIO = 6                     ' "ON-Time"
    Public Const i_clb_mbc91_CH_Impuls = 7                  ' "Impuls"
    Public Const i_clb_mbc91_CH_Wechsel = 8                 ' "Wechsel"
    Public Const i_gb_mbc91_CH1 = 9                         ' "Ausgang 1"
    Public Const i_gb_mbc91_CH2 = 10                        ' "Ausgang 2"
    Public Const i_gb_mbc91_CH3 = 11                        ' "Ausgang 3"
    Public Const i_gb_mbc91_CH4 = 12                        ' "Ausgang 4"
    Public Const i_lbl_mbc91_Wechsel = 13                   ' "Wechselschalter"
    Public Const i_lbl_mbc91_Impuls = 14                    ' "Impulsausgang"

    Public Const i_MBC92 = 14

    Public Const i_lbl_mbc92_MMAddr = 1                     ' "Loc-ID in HEX"
    Public Const i_lbl_mbc92_Rueck = 2                      ' "Meldezyklus"
    Public Const i_lbl_mbc92_Aus = 3                        ' "Auto-AUS"
    Public Const i_lbl_mbc92_CH_Addr = 4                    ' "Adresse"
    Public Const i_lbl_mbc92_CH_Typ = 5                     ' "Typ"
    Public Const i_lbl_mbc92_CH_TIO = 6                     ' "ON-Time"
    Public Const i_clb_mbc92_CH_Impuls = 7                  ' "Impuls"
    Public Const i_clb_mbc92_CH_Wechsel = 8                 ' "Wechsel"
    Public Const i_gb_mbc92_CH1 = 9                         ' "Ausgang 1"
    Public Const i_gb_mbc92_CH2 = 10                        ' "Ausgang 2"
    Public Const i_gb_mbc92_CH3 = 11                        ' "Ausgang 3"
    Public Const i_gb_mbc92_CH4 = 12                        ' "Ausgang 4"
    Public Const i_lbl_mbc92_Wechsel = 13                   ' "Wechselschalter"
    Public Const i_lbl_mbc92_Impuls = 14                    ' "Impulsausgang"

    Public Const i_MBC93 = 15

    Public Const i_gb_mbc93_GFP = 1                         ' "GFP-Softwareversion"
    Public Const i_lbl_mbc93_GFPBeschreibung_1 = 2          ' "Die aktuelle Softwareversion des GFP/Booster von Märklin, um die Update-Aufforderung von der CSx zu umgehen."
    Public Const i_lbl_mbc93_GFPBeschreibung_2 = 3          ' "Nach dem <Update> das Modul bitte per <Reset> neu starten. Die neue Softwareversion wird dann in der CSx angezeigt."
    Public Const i_lbl_mbc93_GFPBezeichnung = 4             ' "GFP-SW-Version"
    Public Const i_gb_mbc93_Latenz = 5                      ' "Latenzzeit Kurzschlusserkennung"
    Public Const i_lbl_mbc93_LatenzBeschreibung_1 = 6       ' "Die Latenzzeit bis zum Ansprechen der Kurzschlusserkennung kann hier in Schritten von 50 ms eingestellt werden."
    Public Const i_lbl_mbc93_LatenzBeschreibung_2 = 7       ' "Nach dem <Update> des Moduls bitte dieses aus der GUI der CS3 löschen. Der neue Latenzwert wird dann in der CS3 angezeigt."
    Public Const i_lbl_mbc93_LatenzBeschreibung_3 = 8       ' "Bei einer CS2 bitte diesen Wert ausschließlich in der CS2-GUI ändern und über den <Refresh>-Button aktualisieren. "
    Public Const i_lbl_mbc93_LatenzBezeichnung = 9          ' "Latenz [ms]"

    Public Const i_MSGBOX_MAIN_Neu = 16                     ' MsgBox Neuanmeldung frmMain

    Public Const i_MSGBOX_MAIN_Neu_1 = 1                    ' "Sollen die Module des Typs"
    Public Const i_MSGBOX_MAIN_Neu_2 = 2                    ' "neu initialisiert werden?" 
    Public Const i_MSGBOX_MAIN_Neu_3 = 3                    ' "Alle Daten der betroffenen Module werden gelöscht."
    Public Const i_MSGBOX_MAIN_Neu_4 = 4                    ' "Danach melden sich die am MBCAN-Bus befindlichen Module des Typs"
    Public Const i_MSGBOX_MAIN_Neu_5 = 5                    ' "wieder an."
    Public Const i_MSGBOX_MAIN_Neu_6 = 6                    ' "Versehentlich gelöschte, aber weiterhin aktive Module erscheinen so wieder In der Datenbank."
    Public Const i_MSGBOX_MAIN_Neu_7 = 7                    ' "MBCAN - Modulsteuerung"

    Public Const i_MSGBOX_MAIN_Db = 17                      ' MsgBox Neuanmeldung frmMain

    Public Const i_MSGBOX_MAIN_Db_1 = 1                     ' "Die Datenbank wird im Ordner 'Dokumente/MBCAN/Datenbank/' unter der Datei 'mbc_db_"
    Public Const i_MSGBOX_MAIN_Db_2 = 2                     ' ".txt' abgelegt. Bitte mit 'Ja' bestätigen oder mit 'Nein' verwerfen." 
    Public Const i_MSGBOX_MAIN_Db_3 = 3                     ' "MBCAN - Datenbanken"

    Public Const i_MSGBOX_MAIN_Bedien = 18                  ' MsgBox Bedienungsanleitung frmMain

    Public Const i_MSGBOX_MAIN_Bedien_1 = 1                 ' "Die Bedienungsanleitung ist noch geöffnet oder wird von einem anderen Prozess verwendet!"
    Public Const i_MSGBOX_MAIN_Bedien_2 = 2                 ' "MBCAN - System"

    Public Const i_MSGBOX_MAIN_RS232 = 19                   ' MsgBox RS232

    Public Const i_MSGBOX_MAIN_RS232_1 = 1                  ' "MBCAN - RS232"
    Public Const i_MSGBOX_MAIN_RS232_2 = 2                  ' "Der COM-Port für die RS232-Kommunikation"
    Public Const i_MSGBOX_MAIN_RS232_3 = 3                  ' "wird geschlossen da das Interface nicht erreichbar ist! Bitte die Einstellungen prüfen!" 
    Public Const i_MSGBOX_MAIN_RS232_4 = 4                  ' "konnte nicht geöffnet werden!"

    Public Const i_MSGBOX_MAIN_BT = 20                      ' MsgBox BT

    Public Const i_MSGBOX_MAIN_BT_1 = 1                     ' "MBCAN - Bluetooth"
    Public Const i_MSGBOX_MAIN_BT_2 = 2                     ' "Der COM-Port für die Bluetooth-Kommunikation"
    Public Const i_MSGBOX_MAIN_BT_3 = 3                     ' "wird geschlossen da das Interface nicht erreichbar ist! Bitte die Einstellungen prüfen!" 
    Public Const i_MSGBOX_MAIN_BT_4 = 4                     ' "konnte nicht geöffnet werden!"

    Public Const i_MSGBOX_MAIN_TCP = 21                     ' MsgBox TCP

    Public Const i_MSGBOX_MAIN_TCP_1 = 1                    ' "MBCAN - TCP"
    Public Const i_MSGBOX_MAIN_TCP_2 = 2                    ' "Der Port für die TCP-Kommunikation wird geschlossen da das Interface nicht erreichbar ist! Bitte die Einstellungen prüfen!"
    Public Const i_MSGBOX_MAIN_TCP_3 = 3                    ' "Vom TCP-Port kann nicht gelesen werden!"
    Public Const i_MSGBOX_MAIN_TCP_4 = 4                    ' "Auf den TCP-Port kann nicht geschrieben werden!"
    Public Const i_MSGBOX_MAIN_TCP_5 = 5                    ' "Der TCP-Port für die Kommunikation konnte nicht geöffnet werden!"

    Public Const i_MSGBOX_MAIN_MAX = 22                     ' MsgBox Maximale Anzahl Module

    Public Const i_MSGBOX_MAIN_MAX_1 = 1                    ' "Die maximale Anzahl von"
    Public Const i_MSGBOX_MAIN_MAX_2 = 2                    ' "Modulen ist registriert!" 
    Public Const i_MSGBOX_MAIN_MAX_3 = 3                    ' "MBCAN - Modulsteuerung"

    Public Const i_MSGBOX_MAIN_UPG = 23                     ' MsgBox Falsche Datei fuer Uprade

    Public Const i_MSGBOX_MAIN_UPG_1 = 1                    ' "Diese Datei ist kein gültiges HEX-File für den Modultyp"
    Public Const i_MSGBOX_MAIN_UPG_2 = 2                    ' "MBCAN - Firmwareupgrade"

    Public Const i_MSGBOX_MAIN_UPGSEND = 24                 ' MsgBox Upgrade

    Public Const i_MSGBOX_MAIN_UPGSEND_1 = 1                ' "Die neue Firmware wird nun an das Modul vom Typ"
    Public Const i_MSGBOX_MAIN_UPGSEND_2 = 2                ' "übertragen" 
    Public Const i_MSGBOX_MAIN_UPGSEND_3 = 3                ' "Dieser Vorgang kann jederzeit über den <Stopp>-Button unterbrochen werden"
    Public Const i_MSGBOX_MAIN_UPGSEND_4 = 4                ' "MBCAN - Firmwareupgrade"

    Public Const i_MSGBOX_MAIN_UPGCOMPLETE = 25             ' MsgBox Upgrade fertig

    Public Const i_MSGBOX_MAIN_UPGCOMPLETE_1 = 1            ' "Die Übertragung der neuen Firmware ist abgeschlossen."
    Public Const i_MSGBOX_MAIN_UPGCOMPLETE_2 = 2            ' "Das Modul vom Typ"
    Public Const i_MSGBOX_MAIN_UPGCOMPLETE_3 = 3            ' "wird jetzt neu gestartet und die Firmware in den Programmspeicher geschrieben."
    Public Const i_MSGBOX_MAIN_UPGCOMPLETE_4 = 4            ' "Währenddessen bitte das Modul nicht von der Spannungsversorgung trennen."
    Public Const i_MSGBOX_MAIN_UPGCOMPLETE_5 = 5            ' "Soll weiter fortgefahren werden?"
    Public Const i_MSGBOX_MAIN_UPGCOMPLETE_6 = 6            ' "ACHTUNG"
    Public Const i_MSGBOX_MAIN_UPGCOMPLETE_7 = 7            ' "Sollte sich das Modul nach ca. 2 min. nicht zurückgemeldet haben, dann muss es ausgebaut und von Grund auf neu über die ISP-Schnittstelle programmiert werden!!!"
    Public Const i_MSGBOX_MAIN_UPGCOMPLETE_8 = 8            ' "MBCAN - Firmwareupgrade"

    Public Const i_MSGBOX_MAIN_NAME = 26                    ' MsgBox Unerlaubter Modulname

    Public Const i_MSGBOX_MAIN_NAME_1 = 1                   ' "MBCAN - System"
    Public Const i_MSGBOX_MAIN_NAME_2 = 2                   ' "Unerlaubte Namensvergabe."
    Public Const i_MSGBOX_MAIN_NAME_3 = 3                   ' "Bitte nicht mehr als 18 Zeichen verwenden!"
    Public Const i_MSGBOX_MAIN_NAME_4 = 4                   ' "Bitte verwenden Sie keine: '@','.', ',', '!'"
    Public Const i_MSGBOX_MAIN_NAME_5 = 5                   ' "Der Name des Moduls darf nicht leer sein!"

    Public Const i_MSGBOX_MAIN_SYSDB = 27                   ' MsgBox Neue Systemdatenbank

    Public Const i_MSGBOX_MAIN_SYSDB_1 = 1                  ' "MBCAN - Datenbanken"
    Public Const i_MSGBOX_MAIN_SYSDB_2 = 2                  ' "Soll eine neue Systemdatenbank erstellt werden? Alle Daten werden gelöscht und die Module müssen sich neu anmelden. Dies kann einige Zeit in Anspruch nehmen."
    Public Const i_MSGBOX_MAIN_SYSDB_3 = 3                  ' "Die ausgewählte Datei hat nicht das richtige Format. Es wird eine Dateilänge von 240 Zeilen erwartet. Gelesen werden konnten nur"
    Public Const i_MSGBOX_MAIN_SYSDB_4 = 4                  ' "Zeilen!"
    Public Const i_MSGBOX_MAIN_SYSDB_5 = 5                  ' "Die ausgewählte Datei hat nicht das richtige Format. Die Nummerierung der Steps ist fehlerhaft ab Zeile"
    Public Const i_MSGBOX_MAIN_SYSDB_6 = 6                  ' "Die ausgewählte Datei hat nicht das richtige Format. Die maximale Aussteuerung der Servos beträgt 120. In Zeile"
    Public Const i_MSGBOX_MAIN_SYSDB_7 = 7                  ' "ist ein Wert von"
    Public Const i_MSGBOX_MAIN_SYSDB_8 = 8                  ' "angegeben!"

    Public Const i_MSGBOX_MAIN_INIT = 28                   ' MsgBox Neue Systemdatenbank

    Public Const i_MSGBOX_MAIN_INIT_1 = 1                  ' "MBCAN - System"
    Public Const i_MSGBOX_MAIN_INIT_2 = 2                  ' "IP-Adresse nicht korrekt eingegeben. Bitte anpassen!"

    Public Const i_MSGBOX_MAIN_SPRACHE = 29                ' MsgBox Neue Systemdatenbank

    Public Const i_MSGBOX_MAIN_SPRACHE_1 = 1               ' "MBCAN - System"
    Public Const i_MSGBOX_MAIN_SPRACHE_2 = 2               ' "Die Sprachen-Datei ist noch geöffnet oder wird von einem anderen Prozess verwendet!"

    Public Const i_SERVO = 30                               ' frmServo

    Public Const i_chart_servo = 1                          ' "Servodiagramm"
    Public Const i_btn_servo_ok = 2                         ' "Ok"
    Public Const i_lbl_servo_step = 3                       ' "Step"
    Public Const i_lbl_servo_wert = 4                       ' "Wert"
    Public Const i_btn_servo_verwerfen = 5                  ' "Verwerfen"
    Public Const i_lbl_servo_anschlaglinks = 6              ' "ROT"
    Public Const i_lbl_servo_anschlagmitte = 7              ' "Mitte"
    Public Const i_lbl_servo_anschlagrechts = 8             ' "GRÜN"
    Public Const i_lbl_servo_drehrichtung = 9               ' "Drehrichtung"
    Public Const i_btn_servo_laden = 10                     ' "Servoweg laden"
    Public Const i_btn_servo_speichern = 11                 ' "Servoweg speichern"
    Public Const i_mbc84_txt_Hinweis_1 = 12                 ' "Hinweis"
    Public Const i_mbc84_txt_Hinweis_2 = 13                 ' "Die Drehrichtung ist abhängig vom Servotyp."
    Public Const i_mbc84_txt_Hinweis_3 = 14                 ' "Deshalb sind hier die Endlagen und auch der entsprechende Servoweg auf die Stellungen <ROT> und <GRÜN> bezogen."
    Public Const i_servo_Text = 15                          ' "MBCAN - Wegdiagramm des Servos"

    Public Const i_INIT = 31                                ' frmInit

    Public Const i_INIT_Label17 = 1                         ' "Modulindex"
    Public Const i_INIT_Label18 = 2                         ' "Dieser Index symbolisiert die erste Stelle der GUiD. Default-Wert 'm'. Erlaubt ist jeder Kleinbuchstabe."
    Public Const i_INIT_Label15 = 3                         ' "Dieser Index symbolisiert die Gerätekennung bei der Anmeldung an die CSx. Default-Wert 'AAAA'. Vergleiche CAN-Protokoll-Defintion von Märklin."
    Public Const i_INIT_Text = 4                            ' "MBCAN - Kommunikationsparameter"
    Public Const i_INIT_Label13 = 5                         ' "Gerätekennung"

    Public Const i_LOKS = 32                                ' frmLoks

    Public Const i_LOKS_lbl_alias = 1                       ' "Alias-Adr"
    Public Const i_LOKS_lbl_speed = 2                       ' "Speed"
    Public Const i_LOKS_btn_ok = 3                          ' "Ok"
    Public Const i_LOKS_btn_Refresh = 4                     ' "Refresh"
    Public Const i_LOKS_cb_Dir_V = 5                        ' "Vorwärts"
    Public Const i_LOKS_cb_Dir_R = 6                        ' "Rückwärts"
    Public Const i_LOKS_cb_Fx = 7                           ' "Funktion"
    Public Const i_LOKS_Text = 8                            ' "Übersicht Lokomotiven"

    Public Const i_WINKEL = 33                              ' frmWinkel

    Public Const i_WINKEL_Text = 1                          ' Titel des Fensters
    Public Const i_WINKEL_Hinweis_1 = 2                     ' Hinweiszeile Teil 1
    Public Const i_WINKEL_Hinweis_2 = 3                     ' Hinweiszeile Teil 2
    Public Const i_WINKEL_btn_Save = 4                      ' SAVE-Button
    Public Const i_WINKEL_btn_Verwerfen = 5                 ' VERWERFEN-Button
    Public Const i_WINKEL_btn_Ok = 6                        ' OK-Button

    Public Const i_PROZESSORAUSWAHL = 34                    ' frmProzessorAuswahl

    Public Const i_PROZESSORAUSWAHL_Text = 1                ' "MBCAN - Zielprozessor auswählen"
    Public Const i_PROZESSORAUSWAHL_gb = 2                  ' "Zielprozessor" (GroupBox-Beschriftung)
    Public Const i_PROZESSORAUSWAHL_btn_OK = 3              ' "OK"-Button
    Public Const i_PROZESSORAUSWAHL_btn_Abbrechen = 4       ' "Abbrechen"-Button
    Public Const i_PROZESSORAUSWAHL_lbl_Hinweis = 5         ' "Der aktuelle Prozessortyp gemäß Artikelnummer lautet:"

    Public Const i_MSGBOX_MAIN_UPGBIN = 35                  ' MsgBox Firmwareupgrade RP2040/ESP32-C3 (.bin)

    Public Const i_MSGBOX_MAIN_UPGBIN_1 = 1                 ' "Diese Datei ist zu groß für den Boot-Speicherchip-Zwischenspeicher (max." (Praefix vor Bytezahl)
    Public Const i_MSGBOX_MAIN_UPGBIN_2 = 2                 ' "Byte)." (Suffix nach Bytezahl)
    Public Const i_MSGBOX_MAIN_UPGBIN_3 = 3                 ' "Datei" (Label)
    Public Const i_MSGBOX_MAIN_UPGBIN_4 = 4                 ' "Modultyp" (Label)
    Public Const i_MSGBOX_MAIN_UPGBIN_5 = 5                 ' "Passt diese Datei zum ausgewählten Modultyp und Zielprozessor? Firmware jetzt übertragen?"
    Public Const i_MSGBOX_MAIN_UPGBIN_6 = 6                 ' "Bitte die .bin-Datei wählen (nicht die .uf2-Datei für den Bootloader-Drag&Drop)."
    Public Const i_MSGBOX_MAIN_UPGBIN_7 = 7                 ' RP2040-Hinweis: Groesse haengt vom Flash-Size-Menue ab, PC-Tool kennt es nicht

    Public Const i_MenuSystemAnzeigen = 74                  ' "Datenbank zeigen ..." (Detailindex unter i_MAIN)

    Public Const i_DATENBANK = 36                           ' frmDatenbank

    Public Const i_DATENBANK_Text = 1                       ' "MBCAN - Datenbank"
    Public Const i_DATENBANK_col_Status = 2                 ' Spalte "Status"
    Public Const i_DATENBANK_col_Typ = 3                    ' Spalte "Typ"
    Public Const i_DATENBANK_col_Name = 4                   ' Spalte "Name"
    Public Const i_DATENBANK_col_ArtNum = 5                 ' Spalte "Artikelnummer"
    Public Const i_DATENBANK_col_Prozessor = 22             ' Spalte "Prozessor"
    Public Const i_DATENBANK_col_SerNr = 6                  ' Spalte "Seriennummer"
    Public Const i_DATENBANK_col_GUiD = 7                   ' Spalte "GUiD"
    Public Const i_DATENBANK_col_SW = 8                     ' Spalte "SW"
    Public Const i_DATENBANK_col_HW = 9                     ' Spalte "HW"
    Public Const i_DATENBANK_col_MMAddr = 10                ' Spalte "MM-Adresse"
    Public Const i_DATENBANK_col_S88ModNr = 11              ' Spalte "s88-Modulnr."
    Public Const i_DATENBANK_col_Plugged = 12               ' Spalte "Steckernetzteil"
    Public Const i_DATENBANK_col_Spannung = 13              ' Spalte "Spannung"
    Public Const i_DATENBANK_col_Temperatur = 14            ' Spalte "Temperatur"
    Public Const i_DATENBANK_btn_Aktualisieren = 15         ' "Aktualisieren"-Button
    Public Const i_DATENBANK_btn_Schliessen = 16            ' "Schließen"-Button
    Public Const i_DATENBANK_Status_Angemeldet = 17         ' Statuswert IsComplete
    Public Const i_DATENBANK_Status_Aktiv = 18               ' Statuswert IsAlive
    Public Const i_DATENBANK_Status_Fehlt = 19               ' Statuswert IsMissing
    Public Const i_DATENBANK_Status_Geloescht = 20           ' Statuswert IsDeleted
    Public Const i_DATENBANK_Status_Unbekannt = 21           ' sonstiger/unbekannter Status

    '**********************************************************************************************
    '*** Aktualisiere Sprachinhalte                                                             ***
    '**********************************************************************************************
    Public Sub SetSprache()

        ' Wenn noch keine Sprache ausgewaehlt wurde waehle DEUTSCH (=1)

        If SprachIndex = 0 Then SprachIndex = 1

        ' Initiale Benennung der Label gemaess ausgewaehlter Sprache

        '******************************************************************************************
        '*** Start MAIN                                                                         ***
        '******************************************************************************************

        frmMain.Text = Sprachen(i_MAIN, i_Mainfrm)   ' "MBCAN - Parametriercenter für mbc-Module"
        frmMain.stp_RS232.Text = Sprachen(i_MAIN, i_None)   ' "None"
        frmMain.stp_BT.Text = Sprachen(i_MAIN, i_None)   ' "None"
        frmMain.stp_TCP.Text = Sprachen(i_MAIN, i_None)   ' "None"
        frmMain.stp_Refresh.Text = Sprachen(i_MAIN, i_stp_Refresh)   ' "Refresh: -/-"
        frmMain.stp_Alive.Text = Sprachen(i_MAIN, i_stp_Alive_None)   ' "Searching: -/-"
        frmMain.stp_Upgrade.Text = Sprachen(i_MAIN, i_stp_Upgrade_None)   ' "-/-"
        frmMain.ts_MenuSystem.Text = Sprachen(i_MAIN, i_MenuSystem)   ' "System"
        frmMain.ts_MenuSystemNeu.Text = Sprachen(i_MAIN, i_MenuSystemNeu)   ' "Datenbank erneuern ..."
        frmMain.ts_MenuSystemSpeichern.Text = Sprachen(i_MAIN, i_MenuSystemSpeichern)   ' "Datenbank speichern ..."
        frmMain.ts_MenuSystemAnzeigen.Text = Sprachen(i_MAIN, i_MenuSystemAnzeigen)   ' "Datenbank zeigen ..."
        frmMain.ts_MenuSystemSchliessen.Text = Sprachen(i_MAIN, i_MenuSystemSchliessen)   ' "Beenden"
        frmMain.ts_MenuVerbindung.Text = Sprachen(i_MAIN, i_MenuVerbindung)   ' "Verbindung"
        frmMain.ts_MenuVerbindungEinstellen.Text = Sprachen(i_MAIN, i_MenuVerbindungEinstellen)   ' "Einstellen"
        frmMain.ts_MenuVerbindungRS232Verbinden.Text = Sprachen(i_MAIN, i_MenuVerbindungVerbinden)   ' "Verbinden"
        frmMain.ts_MenuVerbindungRS232Trennen.Text = Sprachen(i_MAIN, i_MenuVerbindungTrennen)   ' "Trennen"
        frmMain.ts_MenuVerbindungBluetoothVerbinden.Text = Sprachen(i_MAIN, i_MenuVerbindungVerbinden)   ' "Verbinden"
        frmMain.ts_MenuVerbindungBluetoothTrennen.Text = Sprachen(i_MAIN, i_MenuVerbindungTrennen)   ' "Trennen"
        frmMain.ts_MenuVerbindungTCPVerbinden.Text = Sprachen(i_MAIN, i_MenuVerbindungVerbinden)   ' "Verbinden"
        frmMain.ts_MenuVerbindungTCPTrennen.Text = Sprachen(i_MAIN, i_MenuVerbindungTrennen)   ' "Trennen"
        frmMain.ts_MenuInfo.Text = Sprachen(i_MAIN, i_MenuInfo)   ' "Info"
        frmMain.ts_MenuInfoHilfe.Text = Sprachen(i_MAIN, i_MenuInfoHilfe)   ' "Über MBCAN ..."
        frmMain.ts_MenuInfoBedienungsanleitung.Text = Sprachen(i_MAIN, i_MenuInfoBedienungsanleitung)   ' "Bedienungsanleitung"
        frmMain.ts_MenuInfoCopyright.Text = Sprachen(i_MAIN, i_MenuInfoCopyright)   ' "Copyright"
        frmMain.tab_Modul.Text = Sprachen(i_MAIN, i_tab_Modul)   ' "Modul"
        frmMain.btn_ControlRefresh.Text = Sprachen(i_MAIN, i_btn_ControlRefresh)   ' "Refresh"
        frmMain.btn_ControlVerwerfen.Text = Sprachen(i_MAIN, i_btn_ControlVerwerfen)   ' "Verwerfen"
        frmMain.btn_ControlUpdate.Text = Sprachen(i_MAIN, i_btn_ControlUpdate)   ' "Update"
        frmMain.btn_ControlUpgrade.Text = Sprachen(i_MAIN, i_btn_ControlUpgrade)   ' "Upgrade"
        frmMain.btn_ControlReset.Text = Sprachen(i_MAIN, i_btn_ControlReset)   ' "Reset"
        frmMain.btn_ControlLoeschen.Text = Sprachen(i_MAIN, i_btn_ControlLoeschen)   ' "Löschen"
        frmMain.gb_Parameter.Text = Sprachen(i_MAIN, i_gb_Parameter)   ' "Parameter"
        frmMain.lbl_ParameterName.Text = Sprachen(i_MAIN, i_lbl_ParameterName)   ' "Name:"
        frmMain.lbl_ParameterDecTyp.Text = Sprachen(i_MAIN, i_lbl_ParameterDecTyp)   ' "Typ:"
        frmMain.lbl_ParameterArtNum.Text = Sprachen(i_MAIN, i_lbl_ParameterArtNum)   ' "Artikelnummer:"
        frmMain.lbl_ParameterSerNr.Text = Sprachen(i_MAIN, i_lbl_ParameterSerNr)   ' "Seriennummer:"
        frmMain.lbl_ParameterGUID.Text = Sprachen(i_MAIN, i_lbl_ParameterGUID)   ' "Geräte-UiD:"
        frmMain.lbl_ParameterSW.Text = Sprachen(i_MAIN, i_lbl_ParameterSW)   ' "SW-Version:"
        frmMain.lbl_ParameterHW.Text = Sprachen(i_MAIN, i_lbl_ParameterHW)   ' "HW-Version:"
        frmMain.lbl_ParameterPlugged.Text = Sprachen(i_MAIN, i_lbl_ParameterPlugged)   ' "Plugged:"
        frmMain.lbl_ParameterTemperatur.Text = Sprachen(i_MAIN, i_lbl_ParameterTemperatur)   ' "Temperatur:"
        frmMain.lbl_ParameterSpannung.Text = Sprachen(i_MAIN, i_lbl_ParameterSpannung)   ' "Spannung:"
        frmMain.gb_Status.Text = Sprachen(i_MAIN, i_gb_Status)   ' "Status"
        frmMain.gb_Keyboard.Text = Sprachen(i_MAIN, i_gb_Keyboard)   ' "Stellpult"
        frmMain.btn_KeyboardR1.Text = Sprachen(i_MAIN, i_btn_KeyboardR)   ' "R"
        frmMain.btn_KeyboardG1.Text = Sprachen(i_MAIN, i_btn_KeyboardG)   ' "G"
        frmMain.btn_KeyboardR2.Text = Sprachen(i_MAIN, i_btn_KeyboardR)   ' "R"
        frmMain.btn_KeyboardG2.Text = Sprachen(i_MAIN, i_btn_KeyboardG)   ' "G"
        frmMain.btn_KeyboardR3.Text = Sprachen(i_MAIN, i_btn_KeyboardR)   ' "R"
        frmMain.btn_KeyboardG3.Text = Sprachen(i_MAIN, i_btn_KeyboardG)   ' "G"
        frmMain.btn_KeyboardR4.Text = Sprachen(i_MAIN, i_btn_KeyboardR)   ' "R"
        frmMain.btn_KeyboardG4.Text = Sprachen(i_MAIN, i_btn_KeyboardG)   ' "G"
        frmMain.tab_Monitor.Text = Sprachen(i_MAIN, i_tab_Monitor)   ' "Monitor"
        frmMain.lbl_MonitorNachrichtenherkunft.Text = Sprachen(i_MAIN, i_lbl_MonitorNachrichtenherkunft)   ' "None"
        frmMain.lbl_MonitorInhalt.Text = Sprachen(i_MAIN, i_lbl_MonitorInhalt)   ' "Nachrichteninhalt:"
        frmMain.tab_Adressen.Text = Sprachen(i_MAIN, i_tab_Adressen)   ' "Adressen"
        frmMain.lbl_AdressenLocID.Text = Sprachen(i_MAIN, i_lbl_AdressenLocID)   ' "Zuordnung Module zu Loc-ID"
        frmMain.btn_AdressenLocID.Text = Sprachen(i_MAIN, i_btn_AdressenUebersicht)   ' "Übersicht generieren"
        frmMain.MMColumnLocID.HeaderText = Sprachen(i_MAIN, i_dgv_AdressenColumnLocID)   ' "Loc-ID"
        frmMain.MMColumnLocIDTyp.HeaderText = Sprachen(i_MAIN, i_dgv_AdressenColumnLocIDTyp)   ' "Typ"
        frmMain.MMColumnLocIDName.HeaderText = Sprachen(i_MAIN, i_dgv_AdressenColumnLocIDName)   ' "Name"
        frmMain.MMColumnLocIDIndex.HeaderText = Sprachen(i_MAIN, i_dgv_AdressenColumnLocIDIndex)   ' "Index"
        frmMain.lbl_AdressenS88Base.Text = Sprachen(i_MAIN, i_lbl_AdressenS88Base)   ' "Zuordnung Module zu Rückmelder"
        frmMain.btn_AdressenS88Base.Text = Sprachen(i_MAIN, i_btn_AdressenUebersicht)   ' "Übersicht generieren"
        frmMain.S88ColumnModNr.HeaderText = Sprachen(i_MAIN, i_dgv_AdressenColumnModNr)   ' "Mod-Nr"
        frmMain.S88ColumnS88Typ.HeaderText = Sprachen(i_MAIN, i_dgv_AdressenColumnLocIDTyp)   ' "Typ"
        frmMain.S88ColumnS88Name.HeaderText = Sprachen(i_MAIN, i_dgv_AdressenColumnLocIDName)   ' "Name"
        frmMain.S88ColumnS88Index.HeaderText = Sprachen(i_MAIN, i_dgv_AdressenColumnLocIDIndex)   ' "Index"
        frmMain.ts_MenuSystemSprache.Text = Sprachen(i_MAIN, i_ts_MenuSystemSprache) ' "Sprache"
        frmMain.ts_MenuSystemSprache_Deutsch.Text = Sprachen(i_MAIN, i_ts_MenuSystemSprache_Deutsch) ' "Deutsch"
        frmMain.ts_MenuSystemSprache_Französisch.Text = Sprachen(i_MAIN, i_ts_MenuSystemSprache_Französisch) ' "Französisch"
        frmMain.ts_MenuSystemSprache_Englisch.Text = Sprachen(i_MAIN, i_ts_MenuSystemSprache_Englisch) ' "Englisch"

        '******************************************************************************************
        '*** Ende Main                                                                          ***
        '******************************************************************************************

        '------------------------------------------------------------------------------------------

        '******************************************************************************************
        '*** Start mbc-80                                                                       ***
        '******************************************************************************************

        frmMain.lbl_mbc80_BTNameBeschreibung.Text = Sprachen(i_MBC80, i_lbl_mbc80_BTNameBeschreibung)   ' "Die Länge des Modulnamens ist mit fünf Buchstaben fest vorgegeben. "
        frmMain.lbl_mbc80_Name.Text = Sprachen(i_MBC80, i_lbl_mbc80_Name)   ' "Name"
        frmMain.lbl_mbc80_PasswortBeschreibung.Text = Sprachen(i_MBC80, i_lbl_mbc80_PasswortBeschreibung)   ' "Das Passwort ist numerisch im Bereich 0000 ... 9999 einstellbar."
        frmMain.lbl_mbc80_Passwort.Text = Sprachen(i_MBC80, i_lbl_mbc80_Passwort)   ' "Passwort"
        frmMain.gb_mbc80_AK.Text = Sprachen(i_MBC80, i_gb_mbc80_AK)   ' "s88-Automatikgerät"
        frmMain.lbl_mbc80_AK.Text = Sprachen(i_MBC80, i_lbl_mbc80_AK)   ' "Durch CSx zugewiesener Automatik-Kenner:"
        frmMain.gb_mbc80_SW.Text = Sprachen(i_MBC80, i_gb_mbc80_SW)   ' "L88-Softwareversion"
        frmMain.lbl_mbc80_SWBeschreibung.Text = Sprachen(i_MBC80, i_lbl_mbc80_SWBeschreibung)   ' "Die aktuelle Softwareversion des L88 von Märklin, um die Update-Aufforderung von der CSx zu umgehen."
        frmMain.lbl_mbc80_SWBezeichnung.Text = Sprachen(i_MBC80, i_lbl_mbc80_SWBezeichnung)   ' "L88-SW-Version"

        '******************************************************************************************
        '*** Ende mbc-80                                                                        ***
        '******************************************************************************************

        '------------------------------------------------------------------------------------------

        '******************************************************************************************
        '*** Start mbc-81                                                                       ***
        '******************************************************************************************

        frmMain.lbl_mbc81_LocID.Text = Sprachen(i_MBC81, i_lbl_mbc81_LocID)   ' "Loc-ID in HEX"
        frmMain.lbl_mbc81_Nutzung.Text = Sprachen(i_MBC81, i_lbl_mbc81_Nutzung)   ' "Nutzung des Moduls"
        frmMain.lbl_mbc81_Adresse2.Text = Sprachen(i_MBC81, i_lbl_mbc81_Adresse2)   ' "bis"
        frmMain.lbl_mbc81_Adresse1.Text = Sprachen(i_MBC81, i_lbl_mbc81_Adresse1)   ' "Adresse:"
        frmMain.lbl_mbc81_Nutzung_Erlaeuterung_1.Text = Sprachen(i_MBC81, i_lbl_mbc81_Nutzung_Erlaeuterung_1)   ' "Über die Combo-Box kann ausgewählt werden, ob das Modul als MM2-Switchboard genutzt wird (Ersatz für 6040) oder als Halbmodul für den BUS 0 des mbc-80 (L88-Bus-0). Bei den Halbmodulen können von den 64 möglichen Tastern der Matrix entweder die Tasten 1 bis 32 oder 33 bis 64 ausgewählt werden."
        frmMain.lbl_mbc81_Nutzung_Erlaeuterung_2.Text = Sprachen(i_MBC81, i_lbl_mbc81_Nutzung_Erlaeuterung_2)   ' "CS2: Die Tasten 1-32 entsprechen den Kontakten 101-132, die Tasten 33-64 den Kontakte 133-164.  "
        frmMain.lbl_mbc81_Nutzung_Erlaeuterung_3.Text = Sprachen(i_MBC81, i_lbl_mbc81_Nutzung_Erlaeuterung_3)   ' "CS3: Bei der Einstellung <Tastenmatrix> entsprechen die Kontakte 1-32 und 33-64 den jeweiligen Tasten. "
        frmMain.cmb_mbc81_Nutzung.Items.Clear()
        frmMain.cmb_mbc81_Nutzung.Items.AddRange(New Object() {Sprachen(i_MBC81, i_lbl_mbc81_Nutzung_1), Sprachen(i_MBC81, i_lbl_mbc81_Nutzung_2), Sprachen(i_MBC81, i_lbl_mbc81_Nutzung_3)}) ' {"Switchboard", "Bus 0 Matrix  1...32", "Bus 0 Matrix  33...64"})

        '******************************************************************************************
        '*** Ende mbc-81                                                                        ***
        '******************************************************************************************

        '------------------------------------------------------------------------------------------

        '******************************************************************************************
        '*** Start mbc-82                                                                       ***
        '******************************************************************************************

        frmMain.gb_mbc82_GFP.Text = Sprachen(i_MBC82, i_gb_mbc82_GFP)   ' "GFP-Softwareversion"
        frmMain.lbl_mbc82_GFPBeschreibung_1.Text = Sprachen(i_MBC82, i_lbl_mbc82_GFPBeschreibung_1)   ' "Die aktuelle Softwareversion des GFP/Booster von Märklin, um die Update-Aufforderung von der CSx zu umgehen."
        frmMain.lbl_mbc82_GFPBeschreibung_2.Text = Sprachen(i_MBC82, i_lbl_mbc82_GFPBeschreibung_2)   ' "Nach dem <Update> das Modul bitte per <Reset> neu starten. Die neue Softwareversion wird dann in der CSx angezeigt."
        frmMain.lbl_mbc82_GFPBezeichnung.Text = Sprachen(i_MBC82, i_lbl_mbc82_GFPBezeichnung)   ' "GFP-SW-Version"
        frmMain.gb_mbc82_Latenz.Text = Sprachen(i_MBC82, i_gb_mbc82_Latenz)   ' "Latenzzeit Kurzschlusserkennung"
        frmMain.lbl_mbc82_LatenzBeschreibung_1.Text = Sprachen(i_MBC82, i_lbl_mbc82_LatenzBeschreibung_1)   ' "Die Latenzzeit bis zum Ansprechen der Kurzschlusserkennung kann hier in Schritten von 50 ms eingestellt werden."
        frmMain.lbl_mbc82_LatenzBeschreibung_2.Text = Sprachen(i_MBC82, i_lbl_mbc82_LatenzBeschreibung_2)   ' "Nach dem <Update> des Moduls bitte dieses aus der GUI der CS3 löschen. Der neue Latenzwert wird dann in der CS3 angezeigt."
        frmMain.lbl_mbc82_LatenzBeschreibung_3.Text = Sprachen(i_MBC82, i_lbl_mbc82_LatenzBeschreibung_3)   ' "Bei einer CS2 bitte diesen Wert ausschließlich in der CS2-GUI ändern und über den <Refresh>-Button aktualisieren. "
        frmMain.lbl_mbc82_LatenzBezeichnung.Text = Sprachen(i_MBC82, i_lbl_mbc82_LatenzBezeichnung)   ' "Latenz [ms]"

        '******************************************************************************************
        '*** Ende mbc-82                                                                        ***
        '******************************************************************************************

        '------------------------------------------------------------------------------------------

        '******************************************************************************************
        '*** Start mbc-83                                                                       ***
        '******************************************************************************************

        frmMain.lbl_mbc83_LocID.Text = Sprachen(i_MBC83, i_lbl_mbc83_LocID)   ' "Loc-ID in HEX"
        frmMain.lbl_mbc83_Dimmer.Text = Sprachen(i_MBC83, i_lbl_mbc83_Dimmer)   ' "Dimmer StartUp"
        frmMain.lbl_mbc83_AutoAus.Text = Sprachen(i_MBC83, i_lbl_mbc83_AutoAus)   ' "Auto-AUS"
        frmMain.lbl_mbc83_Meldezyklus.Text = Sprachen(i_MBC83, i_lbl_mbc83_Meldezyklus)   ' "Meldezyklus"
        frmMain.lbl_mbc83_Impuls.Text = Sprachen(i_MBC83, i_lbl_mbc83_Impuls)   ' "Impulsausgang"
        frmMain.lbl_mbc83_Wechsel.Text = Sprachen(i_MBC83, i_lbl_mbc83_Wechsel)   ' "Wechselschalter"
        frmMain.lbl_mbc83_Startup.Text = Sprachen(i_MBC83, i_lbl_mbc83_Startup)   ' "StartUp-Signal"
        frmMain.gb_mbc83_CH1.Text = Sprachen(i_MBC83, i_gb_mbc83_CH1)   ' "Ausgang 1"
        frmMain.gb_mbc83_CH1_lbl_Adresse.Text = Sprachen(i_MBC83, i_gb_mbc83_CH_lbl_Adresse)   ' "Adresse:"
        frmMain.gb_mbc83_CH1_lbl_typ.Text = Sprachen(i_MBC83, i_gb_mbc83_CH_lbl_typ)   ' "Typ:"
        frmMain.gb_mbc83_CH1_clb_Typ.Items.Clear()
        frmMain.gb_mbc83_CH1_clb_Typ.Items.AddRange(New Object() {Sprachen(i_MBC83, i_gb_mbc83_CH_clb_Typ_A), Sprachen(i_MBC83, i_gb_mbc83_CH_clb_Typ_B), Sprachen(i_MBC83, i_gb_mbc83_CH_clb_Typ_C)}) ' {"Impuls", "Wechsel", "StartUp"})
        frmMain.gb_mbc83_CH1_lbl_ON.Text = Sprachen(i_MBC83, i_gb_mbc83_CH_lbl_ON)   ' "ON-Time:"
        frmMain.gb_mbc83_CH2.Text = Sprachen(i_MBC83, i_gb_mbc83_CH2)   ' "Ausgang 2"
        frmMain.gb_mbc83_CH2_lbl_Adresse.Text = Sprachen(i_MBC83, i_gb_mbc83_CH_lbl_Adresse)   ' "Adresse:"
        frmMain.gb_mbc83_CH2_lbl_Typ.Text = Sprachen(i_MBC83, i_gb_mbc83_CH_lbl_typ)   ' "Typ:"
        frmMain.gb_mbc83_CH2_clb_Typ.Items.Clear()
        frmMain.gb_mbc83_CH2_clb_Typ.Items.AddRange(New Object() {Sprachen(i_MBC83, i_gb_mbc83_CH_clb_Typ_A), Sprachen(i_MBC83, i_gb_mbc83_CH_clb_Typ_B), Sprachen(i_MBC83, i_gb_mbc83_CH_clb_Typ_C)}) '{"Impuls", "Wechsel", "StartUp"})
        frmMain.gb_mbc83_CH2_lbl_ON.Text = Sprachen(i_MBC83, i_gb_mbc83_CH_lbl_ON)   ' "ON-Time:"
        frmMain.gb_mbc83_CH3.Text = Sprachen(i_MBC83, i_gb_mbc83_CH3)   ' "Ausgang 3"
        frmMain.gb_mbc83_CH3_lbl_Adresse.Text = Sprachen(i_MBC83, i_gb_mbc83_CH_lbl_Adresse)   ' "Adresse:"
        frmMain.gb_mbc83_CH3_lbl_Typ.Text = Sprachen(i_MBC83, i_gb_mbc83_CH_lbl_typ)   ' "Typ:"
        frmMain.gb_mbc83_CH3_clb_Typ.Items.Clear()
        frmMain.gb_mbc83_CH3_clb_Typ.Items.AddRange(New Object() {Sprachen(i_MBC83, i_gb_mbc83_CH_clb_Typ_A), Sprachen(i_MBC83, i_gb_mbc83_CH_clb_Typ_B), Sprachen(i_MBC83, i_gb_mbc83_CH_clb_Typ_C)}) '{"Impuls", "Wechsel", "StartUp"})
        frmMain.gb_mbc83_CH3_lbl_ON.Text = Sprachen(i_MBC83, i_gb_mbc83_CH_lbl_ON)   ' "ON-Time:"
        frmMain.gb_mbc83_CH4.Text = Sprachen(i_MBC83, i_gb_mbc83_CH4)   ' "Ausgang 4"
        frmMain.gb_mbc83_CH4_lbl_Adresse.Text = Sprachen(i_MBC83, i_gb_mbc83_CH_lbl_Adresse)   ' "Adresse:"
        frmMain.gb_mbc83_CH4_lbl_Typ.Text = Sprachen(i_MBC83, i_gb_mbc83_CH_lbl_typ)   ' "Typ:"
        frmMain.gb_mbc83_CH4_clb_Typ.Items.Clear()
        frmMain.gb_mbc83_CH4_clb_Typ.Items.AddRange(New Object() {Sprachen(i_MBC83, i_gb_mbc83_CH_clb_Typ_A), Sprachen(i_MBC83, i_gb_mbc83_CH_clb_Typ_B), Sprachen(i_MBC83, i_gb_mbc83_CH_clb_Typ_C)})  '{"Impuls", "Wechsel", "StartUp"})
        frmMain.gb_mbc83_CH4_lbl_ON.Text = Sprachen(i_MBC83, i_gb_mbc83_CH_lbl_ON)   ' "ON-Time:"

        '******************************************************************************************
        '*** Ende mbc-83                                                                        ***
        '******************************************************************************************

        '------------------------------------------------------------------------------------------

        '******************************************************************************************
        '*** Start mbc-84                                                                       ***
        '******************************************************************************************

        frmMain.lbl_mbc84_LocID.Text = Sprachen(i_MBC84, i_lbl_mbc84_LocID)   ' "Loc-ID in HEX"
        frmMain.lbl_mbc84_Hinweis.Text = Sprachen(i_MBC84, i_lbl_mbc84_Hinweis)   ' "Meldezyklus"
        frmMain.gb_mbc84_CH1.Text = Sprachen(i_MBC84, i_gb_mbc84_CH1)   ' "Servo 1"
        frmMain.lbl_mbc84_adr_CH1.Text = Sprachen(i_MBC84, i_lbl_mbc84_adr)   ' "Adresse:"
        frmMain.lbl_mbc84_Links_CH1.Text = Sprachen(i_MBC84, i_lbl_mbc84_Links)   ' "Grün:"
        frmMain.lbl_mbc84_Rechts_CH1.Text = Sprachen(i_MBC84, i_lbl_mbc84_Rechts)   ' "Rot:"
        frmMain.btn_mbc84_Weg_CH1.Text = Sprachen(i_MBC84, i_btn_mbc84_Weg)   ' "Wegdiagramm"
        frmMain.lbl_mbc84_Step_CH1.Text = Sprachen(i_MBC84, i_lbl_mbc84_Step)   ' "Step:"
        frmMain.gb_mbc84_CH2.Text = Sprachen(i_MBC84, i_gb_mbc84_CH2)   ' "Servo 2"
        frmMain.lbl_mbc84_adr_CH2.Text = Sprachen(i_MBC84, i_lbl_mbc84_adr)   ' "Adresse:"
        frmMain.lbl_mbc84_Links_CH2.Text = Sprachen(i_MBC84, i_lbl_mbc84_Links)   ' "Grün:"
        frmMain.lbl_mbc84_Rechts_CH2.Text = Sprachen(i_MBC84, i_lbl_mbc84_Rechts)   ' "Rot:"
        frmMain.btn_mbc84_Weg_CH2.Text = Sprachen(i_MBC84, i_btn_mbc84_Weg)   ' "Wegdiagramm"
        frmMain.lbl_mbc84_Step_CH2.Text = Sprachen(i_MBC84, i_lbl_mbc84_Step)   ' "Step:"
        frmMain.gb_mbc84_CH3.Text = Sprachen(i_MBC84, i_gb_mbc84_CH3)   ' "Servo 3"
        frmMain.lbl_mbc84_adr_CH3.Text = Sprachen(i_MBC84, i_lbl_mbc84_adr)   ' "Adresse:"
        frmMain.lbl_mbc84_Links_CH3.Text = Sprachen(i_MBC84, i_lbl_mbc84_Links)   ' "Grün:"
        frmMain.lbl_mbc84_Rechts_CH3.Text = Sprachen(i_MBC84, i_lbl_mbc84_Rechts)   ' "Rot:"
        frmMain.btn_mbc84_Weg_CH3.Text = Sprachen(i_MBC84, i_btn_mbc84_Weg)   ' "Wegdiagramm"
        frmMain.lbl_mbc84_Step_CH3.Text = Sprachen(i_MBC84, i_lbl_mbc84_Step)   ' "Step:"
        frmMain.gb_mbc84_CH4.Text = Sprachen(i_MBC84, i_gb_mbc84_CH4)   ' "Servo 4"
        frmMain.lbl_mbc84_adr_CH4.Text = Sprachen(i_MBC84, i_lbl_mbc84_adr)   ' "Adresse:"
        frmMain.lbl_mbc84_Links_CH4.Text = Sprachen(i_MBC84, i_lbl_mbc84_Links)   ' "Grün:"
        frmMain.lbl_mbc84_Rechts_CH4.Text = Sprachen(i_MBC84, i_lbl_mbc84_Rechts)   ' "Rot:"
        frmMain.btn_mbc84_Weg_CH4.Text = Sprachen(i_MBC84, i_btn_mbc84_Weg)   ' "Wegdiagramm"
        frmMain.lbl_mbc84_Step_CH4.Text = Sprachen(i_MBC84, i_lbl_mbc84_Step)   ' "Step:"

        '******************************************************************************************
        '*** Ende mbc-84                                                                        ***
        '******************************************************************************************

        '------------------------------------------------------------------------------------------

        '******************************************************************************************
        '*** Start mbc-85                                                                       ***
        '******************************************************************************************

        frmMain.lbl_mbc85_ModulNr.Text = Sprachen(i_MBC85, i_lbl_mbc85_ModulNr)   ' "ModulNr"
        frmMain.lbl_mbc85_BusNr.Text = Sprachen(i_MBC85, i_lbl_mbc85_BusNr)   ' "BusNr"
        frmMain.lbl_mbc85_Kontakte.Text = Sprachen(i_MBC85, i_lbl_mbc85_Kontakte)   ' "Kontakte CS2"
        frmMain.gb_mbc85_MMLok.Text = Sprachen(i_MBC85, i_gb_mbc85_MMLok)   ' "MM-Lok-Adresse"
        frmMain.lbl_mbc85_MMLok.Text = Sprachen(i_MBC85, i_lbl_mbc85_MMLok)   ' "LokAddr"
        frmMain.lbl_mbc85_MMSpeed.Text = Sprachen(i_MBC85, i_lbl_mbc85_MMSpeed)   ' "Speed"
        frmMain.gb_mbc85_Integrator.Text = Sprachen(i_MBC85, i_gb_mbc85_Integrator)   ' "Integrator"
        frmMain.gb_mbc85_DS_Integrator.Text = Sprachen(i_MBC85, i_gb_mbc85_DS_Integrator)   ' "Drehscheibe Integrator"
        frmMain.btn_mbc85_Winkel.Text = Sprachen(i_MBC85, i_gb_mbc85_Winkel)   ' "Gleiswinkel"

        '******************************************************************************************
        '*** Ende mbc-85                                                                        ***
        '******************************************************************************************

        '------------------------------------------------------------------------------------------

        '******************************************************************************************
        '*** Start mbc-86                                                                       ***
        '******************************************************************************************

        frmMain.lbl_mbc86_ModulNr.Text = Sprachen(i_MBC86, i_lbl_mbc86_ModulNr)   ' "ModulNr"
        frmMain.lbl_mbc86_BusNr.Text = Sprachen(i_MBC86, i_lbl_mbc86_BusNr)   ' "BusNr"
        frmMain.lbl_mbc86_Kontakte.Text = Sprachen(i_MBC86, i_lbl_mbc86_Kontakte)   ' "Kontakte CS2"
        frmMain.gb_mbc86_MMLok.Text = Sprachen(i_MBC86, i_gb_mbc86_MMLok)   ' "MM-Lok-Adresse"
        frmMain.lbl_mbc86_MMLok.Text = Sprachen(i_MBC86, i_lbl_mbc86_MMLok)   ' "LokAddr"

        '******************************************************************************************
        '*** Ende mbc-86                                                                        ***
        '******************************************************************************************

        '------------------------------------------------------------------------------------------

        '******************************************************************************************
        '*** Start mbc-87                                                                       ***
        '******************************************************************************************

        frmMain.lbl_mbc87_1.Text = Sprachen(i_MBC87, i_lbl_mbc87_1)   ' "Loc-ID in HEX"

        '******************************************************************************************
        '*** Ende mbc-87                                                                        ***
        '******************************************************************************************

        '------------------------------------------------------------------------------------------

        '******************************************************************************************
        '*** Start mbc-88                                                                       ***
        '******************************************************************************************

        frmMain.lbl_mbc88_modulnr.Text = Sprachen(i_MBC88, i_lbl_mbc88_modulnr)   ' "ModulNr"
        frmMain.lbl_mbc88_halbmodul.Text = Sprachen(i_MBC88, i_lbl_mbc88_halbmodul)   ' "Halbmodul"
        frmMain.lbl_mbc88_busnr.Text = Sprachen(i_MBC88, i_lbl_mbc88_busnr)   ' "BusNr"
        frmMain.lbl_mbc88_kontaktecs2.Text = Sprachen(i_MBC88, i_lbl_mbc88_kontaktecs2)   ' "Kontakte CS2"
        frmMain.gb_mbc88.Text = Sprachen(i_MBC88, i_gb_mbc88)   ' "Integrator"
        frmMain.cmb_mbc88_Halbmodul.Items.Clear()
        frmMain.cmb_mbc88_Halbmodul.Items.AddRange(New Object() {Sprachen(i_MBC88, i_cmb_mbc88_Halbmodul_1), Sprachen(i_MBC88, i_cmb_mbc88_Halbmodul_2)}) ' {"Eingänge 1 - 8", "Eingänge 9 - 16"})

        '******************************************************************************************
        '*** Ende mbc-88                                                                        ***
        '******************************************************************************************

        '------------------------------------------------------------------------------------------

        '******************************************************************************************
        '*** Start mbc-89                                                                       ***
        '******************************************************************************************

        frmMain.gb_mbc89_I2C.Text = Sprachen(i_MBC89, i_gb_mbc89_I2C)   ' "I2C-Bus-Clock"
        frmMain.lbl_mbc89_takt.Text = Sprachen(i_MBC89, i_lbl_mbc89_takt)   ' "Takt"
        frmMain.gb_mbc89_Basisadresse.Text = Sprachen(i_MBC89, i_gb_mbc89_Basisadresse)   ' "Basisadresse"
        frmMain.lbl_mbc89_Basisadresse.Text = Sprachen(i_MBC89, i_lbl_mbc89_Basisadresse)   ' "Keyboard Nr. 1"
        frmMain.btn_mbc89_Loks.Text = Sprachen(i_MBC89, i_btn_mbc89_Loks)   ' "Lokeigenschaften"
        frmMain.gb_mbc89_Mapping.Text = Sprachen(i_MBC89, i_gb_mbc89_Mapping)   ' "Mapping"
        frmMain.clb_mbc89_Mapping.Items.Clear()
        frmMain.clb_mbc89_Mapping.Items.AddRange(New Object() {Sprachen(i_MBC89, i_clb_mbc89_Mapping), Sprachen(i_MBC89, i_clb_mbc89_MappingDirect)})
        frmMain.gb_mbc89_Keyboard.Text = Sprachen(i_MBC89, i_gb_mbc89_Keyboard)   ' "Keyboard"
        frmMain.gb_mbc89_Version.Text = Sprachen(i_MBC89, i_gb_mbc89_Version)   ' "Connect6021-Softwareversion"
        frmMain.txt_mbc89_alt.Text = Sprachen(i_MBC89, i_txt_mbc89_alt)   ' "Das Modul mbc-89 bietet derzeit keine Parametrierungsmöglichkeiten!"

        '******************************************************************************************
        '*** Ende mbc-89                                                                        ***
        '******************************************************************************************

        '------------------------------------------------------------------------------------------

        '******************************************************************************************
        '*** Start mbc-90                                                                       ***
        '******************************************************************************************

        frmMain.lbl_mbc90_S88ModulNr.Text = Sprachen(i_MBC88, i_lbl_mbc88_modulnr)   ' "ModulNr"
        frmMain.lbl_mbc90_busnr.Text = Sprachen(i_MBC88, i_lbl_mbc88_busnr)   ' "BusNr"
        frmMain.lbl_mbc90_kontaktecs2.Text = Sprachen(i_MBC88, i_lbl_mbc88_kontaktecs2)   ' "Kontakte CS2"
        frmMain.gb_mbc90.Text = Sprachen(i_MBC88, i_gb_mbc88)   ' "Integrator"

        '******************************************************************************************
        '*** Ende mbc-90                                                                        ***
        '******************************************************************************************

        '------------------------------------------------------------------------------------------

        '******************************************************************************************
        '*** Start mbc-91                                                                       ***
        '******************************************************************************************

        frmMain.lbl_mbc91_MMAddr.Text = Sprachen(i_MBC91, i_lbl_mbc91_MMAddr)   '  "Loc-ID in HEX"
        frmMain.lbl_mbc91_Rueck.Text = Sprachen(i_MBC91, i_lbl_mbc91_Rueck)   '  "Meldezyklus"
        frmMain.lbl_mbc91_Aus.Text = Sprachen(i_MBC91, i_lbl_mbc91_Aus)   '  "Auto-AUS"
        frmMain.gb_mbc91_CH1.Text = Sprachen(i_MBC91, i_gb_mbc91_CH1)   '  "Ausgang 1"
        frmMain.lbl_mbc91_CH1_Addr.Text = Sprachen(i_MBC91, i_lbl_mbc91_CH_Addr)   '  "Adresse:"
        frmMain.lbl_mbc91_CH1_Typ.Text = Sprachen(i_MBC91, i_lbl_mbc91_CH_Typ)   '  "Typ:"
        frmMain.lbl_mbc91_CH1_TIO.Text = Sprachen(i_MBC91, i_lbl_mbc91_CH_TIO)   '  "ON-Time:"
        frmMain.clb_mbc91_CH1.Items.Clear()
        frmMain.clb_mbc91_CH1.Items.AddRange(New Object() {Sprachen(i_MBC91, i_clb_mbc91_CH_Impuls), Sprachen(i_MBC91, i_clb_mbc91_CH_Wechsel)})
        frmMain.gb_mbc91_CH2.Text = Sprachen(i_MBC91, i_gb_mbc91_CH2)   '  "Ausgang 2"
        frmMain.lbl_mbc91_CH2_Addr.Text = Sprachen(i_MBC91, i_lbl_mbc91_CH_Addr)   '  "Adresse:"
        frmMain.lbl_mbc91_CH2_Typ.Text = Sprachen(i_MBC91, i_lbl_mbc91_CH_Typ)   '  "Typ:"
        frmMain.lbl_mbc91_CH2_TIO.Text = Sprachen(i_MBC91, i_lbl_mbc91_CH_TIO)   '  "ON-Time:"
        frmMain.clb_mbc91_CH2.Items.Clear()
        frmMain.clb_mbc91_CH2.Items.AddRange(New Object() {Sprachen(i_MBC91, i_clb_mbc91_CH_Impuls), Sprachen(i_MBC91, i_clb_mbc91_CH_Wechsel)})
        frmMain.gb_mbc91_CH3.Text = Sprachen(i_MBC91, i_gb_mbc91_CH3)   '  "Ausgang 3"
        frmMain.lbl_mbc91_CH3_Addr.Text = Sprachen(i_MBC91, i_lbl_mbc91_CH_Addr)   '  "Adresse:"
        frmMain.lbl_mbc91_CH3_Typ.Text = Sprachen(i_MBC91, i_lbl_mbc91_CH_Typ)   '  "Typ:"
        frmMain.lbl_mbc91_CH3_TIO.Text = Sprachen(i_MBC91, i_lbl_mbc91_CH_TIO)   '  "ON-Time:"
        frmMain.clb_mbc91_CH3.Items.Clear()
        frmMain.clb_mbc91_CH3.Items.AddRange(New Object() {Sprachen(i_MBC91, i_clb_mbc91_CH_Impuls), Sprachen(i_MBC91, i_clb_mbc91_CH_Wechsel)})
        frmMain.gb_mbc91_CH4.Text = Sprachen(i_MBC91, i_gb_mbc91_CH4)   '  "Ausgang 4"
        frmMain.lbl_mbc91_CH4_Addr.Text = Sprachen(i_MBC91, i_lbl_mbc91_CH_Addr)   '  "Adresse:"
        frmMain.lbl_mbc91_CH4_Typ.Text = Sprachen(i_MBC91, i_lbl_mbc91_CH_Typ)   '  "Typ:"
        frmMain.lbl_mbc91_CH4_TIO.Text = Sprachen(i_MBC91, i_lbl_mbc91_CH_TIO)   '  "ON-Time:"
        frmMain.clb_mbc91_CH4.Items.Clear()
        frmMain.clb_mbc91_CH4.Items.AddRange(New Object() {Sprachen(i_MBC91, i_clb_mbc91_CH_Impuls), Sprachen(i_MBC91, i_clb_mbc91_CH_Wechsel)})
        frmMain.lbl_mbc91_Wechsel.Text = Sprachen(i_MBC91, i_lbl_mbc91_Wechsel)   '  "Wechselschalter"
        frmMain.lbl_mbc91_Impuls.Text = Sprachen(i_MBC91, i_lbl_mbc91_Impuls)   '  "Impulsausgang"

        '******************************************************************************************
        '*** Ende mbc-91                                                                        ***
        '******************************************************************************************


        '------------------------------------------------------------------------------------------

        '******************************************************************************************
        '*** Start mbc-92                                                                       ***
        '******************************************************************************************

        frmMain.lbl_mbc92_MMAddr.Text = Sprachen(i_MBC91, i_lbl_mbc92_MMAddr)   '  "Loc-ID in HEX"
        frmMain.lbl_mbc92_Rueck.Text = Sprachen(i_MBC91, i_lbl_mbc92_Rueck)   '  "Meldezyklus"
        frmMain.lbl_mbc92_Aus.Text = Sprachen(i_MBC91, i_lbl_mbc92_Aus)   '  "Auto-AUS"
        frmMain.gb_mbc92_CH1.Text = Sprachen(i_MBC91, i_gb_mbc92_CH1)   '  "Ausgang 1"
        frmMain.lbl_mbc92_CH1_Addr.Text = Sprachen(i_MBC91, i_lbl_mbc92_CH_Addr)   '  "Adresse:"
        frmMain.lbl_mbc92_CH1_Typ.Text = Sprachen(i_MBC91, i_lbl_mbc92_CH_Typ)   '  "Typ:"
        frmMain.lbl_mbc92_CH1_TIO.Text = Sprachen(i_MBC91, i_lbl_mbc92_CH_TIO)   '  "ON-Time:"
        frmMain.clb_mbc92_CH1.Items.Clear()
        frmMain.clb_mbc92_CH1.Items.AddRange(New Object() {Sprachen(i_MBC91, i_clb_mbc92_CH_Impuls), Sprachen(i_MBC91, i_clb_mbc92_CH_Wechsel)})
        frmMain.gb_mbc92_CH2.Text = Sprachen(i_MBC91, i_gb_mbc92_CH2)   '  "Ausgang 2"
        frmMain.lbl_mbc92_CH2_Addr.Text = Sprachen(i_MBC91, i_lbl_mbc92_CH_Addr)   '  "Adresse:"
        frmMain.lbl_mbc92_CH2_Typ.Text = Sprachen(i_MBC91, i_lbl_mbc92_CH_Typ)   '  "Typ:"
        frmMain.lbl_mbc92_CH2_TIO.Text = Sprachen(i_MBC91, i_lbl_mbc92_CH_TIO)   '  "ON-Time:"
        frmMain.clb_mbc92_CH2.Items.Clear()
        frmMain.clb_mbc92_CH2.Items.AddRange(New Object() {Sprachen(i_MBC91, i_clb_mbc92_CH_Impuls), Sprachen(i_MBC91, i_clb_mbc92_CH_Wechsel)})
        frmMain.gb_mbc92_CH3.Text = Sprachen(i_MBC91, i_gb_mbc92_CH3)   '  "Ausgang 3"
        frmMain.lbl_mbc92_CH3_Addr.Text = Sprachen(i_MBC91, i_lbl_mbc92_CH_Addr)   '  "Adresse:"
        frmMain.lbl_mbc92_CH3_Typ.Text = Sprachen(i_MBC91, i_lbl_mbc92_CH_Typ)   '  "Typ:"
        frmMain.lbl_mbc92_CH3_TIO.Text = Sprachen(i_MBC91, i_lbl_mbc92_CH_TIO)   '  "ON-Time:"
        frmMain.clb_mbc92_CH3.Items.Clear()
        frmMain.clb_mbc92_CH3.Items.AddRange(New Object() {Sprachen(i_MBC91, i_clb_mbc92_CH_Impuls), Sprachen(i_MBC91, i_clb_mbc92_CH_Wechsel)})
        frmMain.gb_mbc92_CH4.Text = Sprachen(i_MBC91, i_gb_mbc92_CH4)   '  "Ausgang 4"
        frmMain.lbl_mbc92_CH4_Addr.Text = Sprachen(i_MBC91, i_lbl_mbc92_CH_Addr)   '  "Adresse:"
        frmMain.lbl_mbc92_CH4_Typ.Text = Sprachen(i_MBC91, i_lbl_mbc92_CH_Typ)   '  "Typ:"
        frmMain.lbl_mbc92_CH4_TIO.Text = Sprachen(i_MBC91, i_lbl_mbc92_CH_TIO)   '  "ON-Time:"
        frmMain.clb_mbc92_CH4.Items.Clear()
        frmMain.clb_mbc92_CH4.Items.AddRange(New Object() {Sprachen(i_MBC91, i_clb_mbc92_CH_Impuls), Sprachen(i_MBC91, i_clb_mbc92_CH_Wechsel)})
        frmMain.lbl_mbc92_Wechsel.Text = Sprachen(i_MBC91, i_lbl_mbc92_Wechsel)   '  "Wechselschalter"
        frmMain.lbl_mbc92_Impuls.Text = Sprachen(i_MBC91, i_lbl_mbc92_Impuls)   '  "Impulsausgang"

        '******************************************************************************************
        '*** Ende mbc-91                                                                        ***
        '******************************************************************************************


        '------------------------------------------------------------------------------------------

        '******************************************************************************************
        '*** Start mbc-93                                                                       ***
        '******************************************************************************************

        frmMain.gb_mbc93_GFP.Text = Sprachen(i_MBC93, i_gb_mbc93_GFP)   ' "GFP-Softwareversion"
        frmMain.lbl_mbc93_GFPBeschreibung_1.Text = Sprachen(i_MBC93, i_lbl_mbc93_GFPBeschreibung_1)   ' "Die aktuelle Softwareversion des GFP/Booster von Märklin, um die Update-Aufforderung von der CSx zu umgehen."
        frmMain.lbl_mbc93_GFPBeschreibung_2.Text = Sprachen(i_MBC93, i_lbl_mbc93_GFPBeschreibung_2)   ' "Nach dem <Update> das Modul bitte per <Reset> neu starten. Die neue Softwareversion wird dann in der CSx angezeigt."
        frmMain.lbl_mbc93_GFPBezeichnung.Text = Sprachen(i_MBC93, i_lbl_mbc93_GFPBezeichnung)   ' "GFP-SW-Version"
        frmMain.gb_mbc93_Latenz.Text = Sprachen(i_MBC93, i_gb_mbc93_Latenz)   ' "Latenzzeit Kurzschlusserkennung"
        frmMain.lbl_mbc93_LatenzBeschreibung_1.Text = Sprachen(i_MBC93, i_lbl_mbc93_LatenzBeschreibung_1)   ' "Die Latenzzeit bis zum Ansprechen der Kurzschlusserkennung kann hier in Schritten von 50 ms eingestellt werden."
        frmMain.lbl_mbc93_LatenzBeschreibung_2.Text = Sprachen(i_MBC93, i_lbl_mbc93_LatenzBeschreibung_2)   ' "Nach dem <Update> des Moduls bitte dieses aus der GUI der CS3 löschen. Der neue Latenzwert wird dann in der CS3 angezeigt."
        frmMain.lbl_mbc93_LatenzBeschreibung_3.Text = Sprachen(i_MBC93, i_lbl_mbc93_LatenzBeschreibung_3)   ' "Bei einer CS2 bitte diesen Wert ausschließlich in der CS2-GUI ändern und über den <Refresh>-Button aktualisieren. "
        frmMain.lbl_mbc93_LatenzBezeichnung.Text = Sprachen(i_MBC93, i_lbl_mbc93_LatenzBezeichnung)   ' "Latenz [ms]"

        '******************************************************************************************
        '*** Ende mbc-93                                                                        ***
        '******************************************************************************************

        '------------------------------------------------------------------------------------------

        '******************************************************************************************
        '*** Start frmServo                                                                     ***
        '******************************************************************************************

        frmServo.chart_servo.Text = Sprachen(i_SERVO, i_chart_servo)   ' "Servodiagramm"
        frmServo.btn_servo_ok.Text = Sprachen(i_SERVO, i_btn_servo_ok)   ' "Ok"
        frmServo.lbl_servo_step.Text = Sprachen(i_SERVO, i_lbl_servo_step)   ' "Step"
        frmServo.lbl_servo_wert.Text = Sprachen(i_SERVO, i_lbl_servo_wert)   ' "Wert"
        frmServo.lbl_servo_linksmax.Text = "<" & Sprachen(i_SERVO, i_lbl_servo_anschlaglinks) & ">-MAX"   ' "<ROT>-MAX"
        frmServo.lbl_servo_rechtsmax.Text = "<" & Sprachen(i_SERVO, i_lbl_servo_anschlagrechts) & ">-MAX"  ' "<GRÜN>-MAX"
        frmServo.btn_servo_verwerfen.Text = Sprachen(i_SERVO, i_btn_servo_verwerfen)   ' "Verwerfen"
        frmServo.lbl_servo_anschlaglinks.Text = "<" & Sprachen(i_SERVO, i_lbl_servo_anschlaglinks) & ">"   ' "<ROT>"
        frmServo.lbl_servo_anschlagmitte.Text = Sprachen(i_SERVO, i_lbl_servo_anschlagmitte)   ' "Mitte"
        frmServo.lbl_servo_anschlagrechts.Text = "<" & Sprachen(i_SERVO, i_lbl_servo_anschlagrechts) & ">"  ' "<GRÜN>"
        frmServo.lbl_servo_dirlinksrechts.Text = "<" & Sprachen(i_SERVO, i_lbl_servo_anschlaglinks) & ">"    ' "<ROT>"
        frmServo.lbl_servo_dirrechts.Text = "<" & Sprachen(i_SERVO, i_lbl_servo_anschlagrechts) & ">"   ' "<GRÜN>"
        frmServo.lbl_servo_dirrechtslinks.Text = "<" & Sprachen(i_SERVO, i_lbl_servo_anschlaglinks) & ">"   ' "<ROT>"
        frmServo.lbl_servo_drehrichtung_lr.Text = Sprachen(i_SERVO, i_lbl_servo_drehrichtung) & " ---->" ' "Drehrichtung ---->"
        frmServo.lbl_servo_drehrichtung_rl.Text = Sprachen(i_SERVO, i_lbl_servo_drehrichtung) & " ---->" ' "Drehrichtung ---->"
        frmServo.btn_servo_laden.Text = Sprachen(i_SERVO, i_btn_servo_laden) & " ..."  ' "Servoweg laden ..."
        frmServo.btn_servo_speichern.Text = Sprachen(i_SERVO, i_btn_servo_speichern) & " ..."  ' "Servoweg speichern ..."
        frmServo.mbc84_txt_Hinweis.Text = Sprachen(i_SERVO, i_mbc84_txt_Hinweis_1) & ": " & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & Sprachen(i_SERVO, i_mbc84_txt_Hinweis_2) & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & Sprachen(i_SERVO, i_mbc84_txt_Hinweis_3)
        frmServo.Text = Sprachen(i_SERVO, i_servo_Text)   ' "MBCAN - Wegdiagramm des Servos"

        '******************************************************************************************
        '*** Ende frmServo                                                                      ***
        '******************************************************************************************

        '------------------------------------------------------------------------------------------

        '******************************************************************************************
        '*** Start frmInit                                                                      ***
        '******************************************************************************************

        frmInit.Label13.Text = Sprachen(i_INIT, i_INIT_Label13) & ":"  ' "Gerätekennung" & ":"
        frmInit.Label17.Text = Sprachen(i_INIT, i_INIT_Label17) & ":"  ' "Modulindex" & ":"
        frmInit.Label18.Text = Sprachen(i_INIT, i_INIT_Label18)   ' "Dieser Index symbolisiert die erste Stelle der GUiD. Default-Wert 'm'. Erlaubt ist jeder Kleinbuchstabe."
        frmInit.Label15.Text = Sprachen(i_INIT, i_INIT_Label15)   ' "Dieser Index symbolisiert die Gerätekennung bei der Anmeldung an die CSx. Default-Wert 'AAAA'. Vergleiche CAN-Protokoll-Defintion von Märklin."
        frmInit.Text = Sprachen(i_INIT, i_INIT_Text)   ' "MBCAN - Kommunikationsparameter"

        '******************************************************************************************
        '*** Ende frmInit                                                                       ***
        '******************************************************************************************

        '------------------------------------------------------------------------------------------

        '******************************************************************************************
        '*** Start frmLoks                                                                      ***
        '******************************************************************************************

        frmLoks.lbl_alias.Text = Sprachen(i_LOKS, i_LOKS_lbl_alias) & ": .........."
        frmLoks.lbl_speed.Text = Sprachen(i_LOKS, i_LOKS_lbl_speed)   ' "Speed"
        frmLoks.btn_ok.Text = Sprachen(i_LOKS, i_LOKS_btn_ok)   ' "Ok"
        frmLoks.btn_Refresh.Text = Sprachen(i_LOKS, i_LOKS_btn_Refresh)   ' "Refresh"
        frmLoks.cb_Dir_V.Text = Sprachen(i_LOKS, i_LOKS_cb_Dir_V)   ' "Vorwärts"
        frmLoks.cb_Dir_R.Text = Sprachen(i_LOKS, i_LOKS_cb_Dir_R)   ' "Rückwärts"
        frmLoks.cb_F0.Text = Sprachen(i_LOKS, i_LOKS_cb_Fx) & " F0"  ' "Funktion" & " F0"
        frmLoks.cb_F1.Text = Sprachen(i_LOKS, i_LOKS_cb_Fx) & " F1"  ' "Funktion" & " F1"
        frmLoks.cb_F2.Text = Sprachen(i_LOKS, i_LOKS_cb_Fx) & " F2"  ' "Funktion" & " F2"
        frmLoks.cb_F3.Text = Sprachen(i_LOKS, i_LOKS_cb_Fx) & " F3"  ' "Funktion" & " F3"
        frmLoks.cb_F4.Text = Sprachen(i_LOKS, i_LOKS_cb_Fx) & " F4"  ' "Funktion" & " F4"
        frmLoks.Text = Sprachen(i_LOKS, i_LOKS_Text)   ' "Übersicht Lokomotiven"

        '******************************************************************************************
        '*** Ende frmLoks                                                                       ***
        '******************************************************************************************

        '------------------------------------------------------------------------------------------

        '******************************************************************************************
        '*** Start frmWinkel                                                                    ***
        '******************************************************************************************

        frmWinkel.Text = Sprachen(i_WINKEL, i_WINKEL_Text) ' Titel des Fensters
        frmWinkel.lbl_mbc85_hinweis.Text = Sprachen(i_WINKEL, i_WINKEL_Hinweis_1) ' Hinweiszeile Teil 1
        frmWinkel.lbl_mbc85_hinweis.Text &= vbCr & vbLf & Sprachen(i_WINKEL, i_WINKEL_Hinweis_2) ' Hinweiszeile Teil 2
        frmWinkel.btn_winkel_speichern.Text = Sprachen(i_WINKEL, i_WINKEL_btn_Save) ' SAVE-Button
        frmWinkel.btn_winkel_verwerfen.Text = Sprachen(i_WINKEL, i_WINKEL_btn_Verwerfen) ' VERWERFEN-Button
        frmWinkel.btn_winkel_ok.Text = Sprachen(i_WINKEL, i_WINKEL_btn_Ok) ' OK-Button

        '******************************************************************************************
        '*** Ende frmWinkel                                                                     ***
        '******************************************************************************************

        '------------------------------------------------------------------------------------------

        '******************************************************************************************
        '*** Start frmProzessorAuswahl                                                          ***
        '******************************************************************************************

        frmProzessorAuswahl.Text = Sprachen(i_PROZESSORAUSWAHL, i_PROZESSORAUSWAHL_Text) ' Titel des Fensters
        frmProzessorAuswahl.gb_Prozessor.Text = Sprachen(i_PROZESSORAUSWAHL, i_PROZESSORAUSWAHL_gb) ' GroupBox
        frmProzessorAuswahl.btn_OK.Text = Sprachen(i_PROZESSORAUSWAHL, i_PROZESSORAUSWAHL_btn_OK) ' OK-Button
        frmProzessorAuswahl.btn_Abbrechen.Text = Sprachen(i_PROZESSORAUSWAHL, i_PROZESSORAUSWAHL_btn_Abbrechen) ' Abbrechen-Button
        frmProzessorAuswahl.lbl_Hinweis.Text = Sprachen(i_PROZESSORAUSWAHL, i_PROZESSORAUSWAHL_lbl_Hinweis) ' Hinweistext

        '******************************************************************************************
        '*** Ende frmProzessorAuswahl                                                           ***
        '******************************************************************************************

        '------------------------------------------------------------------------------------------

        '******************************************************************************************
        '*** Start frmDatenbank                                                                 ***
        '******************************************************************************************

        frmDatenbank.Text = Sprachen(i_DATENBANK, i_DATENBANK_Text) ' Titel des Fensters
        frmDatenbank.col_db_Status.HeaderText = Sprachen(i_DATENBANK, i_DATENBANK_col_Status)
        frmDatenbank.col_db_Typ.HeaderText = Sprachen(i_DATENBANK, i_DATENBANK_col_Typ)
        frmDatenbank.col_db_Name.HeaderText = Sprachen(i_DATENBANK, i_DATENBANK_col_Name)
        frmDatenbank.col_db_ArtNum.HeaderText = Sprachen(i_DATENBANK, i_DATENBANK_col_ArtNum)
        frmDatenbank.col_db_Prozessor.HeaderText = Sprachen(i_DATENBANK, i_DATENBANK_col_Prozessor)
        frmDatenbank.col_db_SerNr.HeaderText = Sprachen(i_DATENBANK, i_DATENBANK_col_SerNr)
        frmDatenbank.col_db_GUiD.HeaderText = Sprachen(i_DATENBANK, i_DATENBANK_col_GUiD)
        frmDatenbank.col_db_SW.HeaderText = Sprachen(i_DATENBANK, i_DATENBANK_col_SW)
        frmDatenbank.col_db_HW.HeaderText = Sprachen(i_DATENBANK, i_DATENBANK_col_HW)
        frmDatenbank.col_db_MMAddr.HeaderText = Sprachen(i_DATENBANK, i_DATENBANK_col_MMAddr)
        frmDatenbank.col_db_S88ModNr.HeaderText = Sprachen(i_DATENBANK, i_DATENBANK_col_S88ModNr)
        frmDatenbank.col_db_Plugged.HeaderText = Sprachen(i_DATENBANK, i_DATENBANK_col_Plugged)
        frmDatenbank.col_db_Spannung.HeaderText = Sprachen(i_DATENBANK, i_DATENBANK_col_Spannung)
        frmDatenbank.col_db_Temperatur.HeaderText = Sprachen(i_DATENBANK, i_DATENBANK_col_Temperatur)
        frmDatenbank.btn_db_Aktualisieren.Text = Sprachen(i_DATENBANK, i_DATENBANK_btn_Aktualisieren)
        frmDatenbank.btn_db_Schliessen.Text = Sprachen(i_DATENBANK, i_DATENBANK_btn_Schliessen)

        '******************************************************************************************
        '*** Ende frmDatenbank                                                                  ***
        '******************************************************************************************

        '------------------------------------------------------------------------------------------

    End Sub



End Module




