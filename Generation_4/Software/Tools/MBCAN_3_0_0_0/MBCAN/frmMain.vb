'**************************************************************************************************
'*** Parametriersoftware fuer die mbc-Decoder ***                                               ***
'*** (c) 2012 by Dr.-Ing. Thomas Wiesner                                                        ***
'**************************************************************************************************

Imports System.ComponentModel
Imports System.IO
Imports System.Net.Sockets
Imports System.Reflection
Imports System.Threading

Public Class frmMain

    '++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    '+++ Hauptprogramm                                                                          +++
    '+++ Allgemeine Funktionen zum Programm                                                     +++
    '++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    '**********************************************************************************************
    '*** Hauptprogramm oeffnen                                                                  ***
    '**********************************************************************************************

    Private Sub frmMain_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load

        frmLoks.Timer_Refresh.Stop()

        'Timer fuer Neuanmeldung initialisieren

        Timer_Neu_Init()

        'Timer fuer Alive-Meldung initialisieren

        Timer_Alive_Init()

        ' Decoder initialisieren

        Modul_Init()

        ' Menuebaum initialisieren

        Strukturbaum_Init()

        ' Registerkarten initialsisieren

        Register_Init()

        ' Statusleiste initialisieren

        Statusleiste_Init()

        ' Menue initialisieren

        Menue_Init()

        ' Einstellungen laden falls vorhanden

        DB_KomPara_Laden()

        ' Datenbank laden falls vorhanden

        DB_Modul_Laden()

        ' Setze Sprache

        SetSprache()

        ' Leeres Modul anzeigen

        Register_Modul_Bilder_Show(0)






    End Sub

    '**********************************************************************************************
    '*** Hauptprogramm schliessen                                                               ***
    '**********************************************************************************************

    Private Sub frmMain_FormClosing(ByVal sender As Object, ByVal e As System.Windows.Forms.FormClosingEventArgs) Handles Me.FormClosing


        ' Schließe alle Fenster die ggf. noch offen sind

        Try
            frmServo.Close()
        Catch ex As Exception

        End Try

        Try
            frmLoks.Close()
        Catch ex As Exception

        End Try

        Try
            frmInfo.Close()
        Catch ex As Exception

        End Try

        Try
            frmInit.Close()
        Catch ex As Exception

        End Try

        ' Timer fuer Alivemeldung stoppen

        tm_Alive.Stop()
        tm_Neu.Stop()
        frmLoks.Timer_Refresh.Stop()

        ' Kommunikationsparameter speichern

        DB_KomPara_Speichern()

        ' Datenbank speichern

        DB_Modul_Speichern(0)

        ' RS232-Receive-Thread schliessen wenn noch nicht erfolgt

        If RS232ThreadIsActive = True Then
            CommunicationThread.StopReceiveThread(RS232ThreadIsActive, RS232ThreadReceive, 2000)
            RS232Client.Close()
        End If

        ' BT-Receive-Thread schliessen wenn noch nicht erfolgt

        If BTThreadIsActive = True Then
            CommunicationThread.StopReceiveThread(BTThreadIsActive, BTThreadReceive, 2000)
            BTClient.Close()
        End If

        ' TCP-Thread schliessen wenn aktiv

        If TCPThreadIsActive = True Then

            stp_TCP.Text = "TCP: " & Sprachen(i_MAIN, i_stp_TCP_Disconnect)

            CommunicationThread.StopReceiveThread(TCPThreadIsActive, TCPThreadReceive, 4000)

            Try
                TCPclient.Close()
            Catch ex As Exception

            End Try

        End If

    End Sub

    '**********************************************************************************************
    '*** Modularray initialisieren                                                              ***
    '**********************************************************************************************

    Public Sub Modul_Init()

        ' Setze das aktive Modul auf 0,0

        AktNr = 0
        AktTyp = 0

    End Sub

    '**********************************************************************************************
    '*** Modul - Alle Module auf "nicht anwesend" setzen                                        ***
    '*** Wird beim Trennen der Verbindung (RS232/BT/TCP) aufgerufen: ohne aktive Verbindung ist ***
    '*** kein Modul mehr erreichbar, das wuerde der Alive-Automat (tm_Alive) sonst erst nach    ***
    '*** mehreren Zyklen je Modul einzeln erkennen. Geloeschte Module bleiben unangetastet.     ***
    '**********************************************************************************************

    Public Sub Modul_AlleMissing()

        Dim IndexT As Integer
        Dim IndexM As Integer

        tm_Alive.Stop()
        tm_Neu.Stop()

        For IndexT = 1 To MaxTyp
            For IndexM = 1 To MBC.ModulTyp(IndexT).Count
                If MBC.ModulTyp(IndexT).Modul(IndexM).Status <> IsDeleted And
                   MBC.ModulTyp(IndexT).Modul(IndexM).Status <> IsMissing Then
                    MBC.ModulTyp(IndexT).Modul(IndexM).Status = IsMissing
                    Strukturbaum_MenueIconChange(IndexT, IndexM, False, True)
                End If
            Next
        Next

        ' Registerkarten/Anzeige deaktivieren, da ohne Verbindung keine Interaktion moeglich ist

        Register_Modul_Werte_Show(False)

    End Sub

    '**********************************************************************************************
    '*** Timer für Neuanmeldung                                                                 ***
    '**********************************************************************************************

    Private Sub Timer_Neu_Init()

        Me.tm_Neu.Stop()

        Me.tm_Neu.Interval = 10000

    End Sub

    Private Sub Timer_Neu_Tick(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles tm_Neu.Tick
        Dim TxMsgReg As String

        ' Flag zuruecksetzen

        FLAG_Neu_Busy = False

        ' Sende Datenbanknummer

        COMMsgSend(MsgDatenbankHIGH)
        COMMsgSend(MsgDatenbankMID)
        COMMsgSend(MsgDatenbankLOW)
        COMMsgSend(MsgKenner & CS2_Identifier & " " & Microsoft.VisualBasic.Left(CS2_Kenner, 2) & " " & Microsoft.VisualBasic.Right(CS2_Kenner, 2))

        ' Sende Nachricht

        TxMsgReg = MsgNeuanmeldungsping

        COMMsgSend(TxMsgReg)

    End Sub

    '**********************************************************************************************
    '*** Timer für Reset nach Update                                                            ***
    '**********************************************************************************************

    Private Sub Timer_Update_Tick(sender As Object, e As EventArgs) Handles tm_Update.Tick

        ' Stoppe Updatetimer

        tm_Update.Stop()

        ' Sende das RESET-Signal an das Modul

        COMMsgSend(MsgReset & MBC.ModulTyp(AktTyp).Modul(AktNr).MsgGUiD)

        ' Deaktiviere das Modul im Strukturbaum und nehme die Selection des Moduls zurueck

        MBC.ModulTyp(AktTyp).Modul(AktNr).Status = IsMissing

        Register_Modul_Aktorstellung_Show()

        Strukturbaum_MenueIconChange(AktTyp, AktNr, False, True)

    End Sub


    '**********************************************************************************************
    '*** Timer für Aliveanmeldung                                                               ***
    '**********************************************************************************************

    Private Sub Timer_Alive_Init()

        Me.tm_Alive.Stop()

        Me.tm_Alive.Interval = 2000

        stp_Alive.Text = Sprachen(i_MAIN, i_stp_Alive_None)

    End Sub

    Private Sub Timer_Alive_Tick(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles tm_Alive.Tick

        Static ModulAliveTyp = 1
        Static ModulAliveNr = 1
        Static ModulPrevTyp = 1
        Static ModulPrevNr = 1
        Static ModulTyp = 1
        Static ModulNr = 1

        ' CSx-Anwesenheit pruefen: laenger als CSX_PING_TIMEOUT_SEC kein Ping mehr erhalten?
        ' Dann CSx als nicht mehr am Bus werten und tab_Module wieder freigeben (siehe
        ' Register_Modul_CSxLock_Apply()/definitionen.vb)

        If FLAG_CSxPingEmpfangen AndAlso
           (DateTime.Now - CSxLastPingUtc).TotalSeconds >= CSX_PING_TIMEOUT_SEC Then

            FLAG_CSxPingEmpfangen = False
            Register_Modul_CSxLock_Apply()

        End If

        tm_Alive.Stop()

        ' Sichere vorangegangenes Modul um die Symbolsteuerung zu ermöglichen

        ModulTyp = ModulPrevTyp
        ModulNr = ModulPrevNr

        ' Werte die Flags aus
        ' Start ist nur gesetzt, wenn ein im System bereits bekanntes Modul angesprochen wurde
        ' Busy ist nur gesetzt, wenn das Modul auf die ALIVE-Aufforderung die GUiD gesendet hat

        If (FLAG_Alive_Start And (FLAG_Alive_Ack = False)) Then

            Try
                MBC.ModulTyp(ModulPrevTyp).Modul(ModulPrevNr).IsAlive += 1
            Catch ex As Exception
                ModulAliveTyp = 1
                ModulAliveNr = 1
                ModulPrevTyp = 1
                ModulPrevNr = 1
                ModulTyp = 1
                ModulNr = 1
                FLAG_Alive_Start = False
                Exit Sub
            End Try

            ' Wenn das Modul 4x nicht gefunden wurde dann setze es auf "nicht anwesend"

            If MBC.ModulTyp(ModulPrevTyp).Modul(ModulPrevNr).IsAlive = 4 Then
                MBC.ModulTyp(ModulPrevTyp).Modul(ModulPrevNr).Status = IsMissing
                Strukturbaum_MenueIconChange(ModulPrevTyp, ModulPrevNr, False, True)
                MBC.ModulTyp(ModulPrevTyp).Modul(ModulPrevNr).IsAlive = 0
            End If

        End If

        If (FLAG_Alive_Start And FLAG_Alive_Ack) Then

            Select Case MBC.ModulTyp(ModulPrevTyp).Modul(ModulPrevNr).Status
                Case IsMissing
                    MBC.ModulTyp(ModulPrevTyp).Modul(ModulPrevNr).Status = IsAlive
                    Strukturbaum_MenueIconChange(ModulPrevTyp, ModulPrevNr, False, True)
                Case IsAlive
                    MBC.ModulTyp(ModulPrevTyp).Modul(ModulPrevNr).Status = IsAlive
                    Strukturbaum_MenueIconChange(ModulPrevTyp, ModulPrevNr, False, False)
                Case IsComplete
                    MBC.ModulTyp(ModulPrevTyp).Modul(ModulPrevNr).Status = IsComplete
                    Strukturbaum_MenueIconChange(ModulPrevTyp, ModulPrevNr, False, False)
            End Select

        End If

        ' Flag zum Alivestart zuruecksetzen

        FLAG_Alive_Start = False

        ' Sichere Modulindizes

        ModulPrevTyp = ModulAliveTyp
        ModulPrevNr = ModulAliveNr

        ' Wenn vom Modultyp mindestens 1 Modul angemeldet ist
        ' dann sende ALIVE-Aufforderungen ueber die GUiD

        If (MBC.ModulTyp(ModulAliveTyp).Count >= ModulAliveNr) Then

            ' Sichere das rueckzumeldende Modul fuer den Vergleich nach der Latenzzeit
            ' Latenzzeit ist der Timerzyklus

            ModulAliveGUiD = MBC.ModulTyp(ModulAliveTyp).Modul(ModulAliveNr).MsgGUiD
            ModulTyp = ModulAliveTyp
            ModulNr = ModulAliveNr

            ' Wenn das Modul nicht geloescht wurde dann sende ALIVE-Aufforderung

            If (MBC.ModulTyp(ModulAliveTyp).Modul(ModulAliveNr).Status <> IsDeleted) Then

                ' Setze Flags

                FLAG_Alive_Ack = False
                FLAG_Alive_Start = True
                FLAG_Alive_Request = True

            End If

            ' Erhoehe Modulnummer

            ModulAliveNr += 1

        End If

        ' Erhoehe Modultypzaehler wenn Anzahl der installierten Module
        ' eines Modultyps erreicht wurde

        If (ModulAliveNr > MBC.ModulTyp(ModulAliveTyp).Count) Then

            ModulAliveNr = 1
            ModulAliveTyp += 1

            ' Wenn Anzahl der installierten Modultypen erreicht ist
            ' dann setze Index zurueck auf Anfang

            If (ModulAliveTyp > MaxTyp) Then
                ModulAliveTyp = 1
            End If

        End If


        If FLAG_Alive_Start Then

            ' Timer neu setzen

            tm_Alive.Interval = 5000

            tm_Alive.Start()

            ' Sende ALIVE-Anforderung

            COMMsgSend(MsgAlive & MBC.ModulTyp(ModulTyp).Modul(ModulNr).MsgGUiD)

            ' Alive: Modultyp mbcxx Modulnummer: 0

            stp_Alive.Text = Sprachen(i_MAIN, i_stp_Alive_Active) & " " & MBC.ModulTyp(ModulTyp).Bezeichner & "/" & MBC.ModulTyp(ModulTyp).Modul(ModulNr).Name

        Else

            tm_Alive.Interval = 1000

            tm_Alive.Start()

            stp_Alive.Text = Sprachen(i_MAIN, i_stp_Alive_None)

        End If

    End Sub

    '**********************************************************************************************
    '*** Watchdog fuer den Lese-/Schreibautomaten (COMRefreshStart()/COMUpdateStart() und die    ***
    '*** Zustaende WR.1.1-WR.3/RD.1.1-RD.2 in frmMain.ComProtocol.vb, alle ueber                 ***
    '*** COMMsgSendAndArm() scharfgeschaltet) - der Automat ist rein ereignisgetrieben (wartet   ***
    '*** auf die naechste CAN-Antwort) und haette ohne diesen Timer bei einer im Bus-Traffic      ***
    '*** verlorenen Anfrage/Antwort dauerhaft haengen bleiben koennen.                            ***
    '**********************************************************************************************

    Private Sub Timer_ComTimeout_Tick(sender As Object, e As EventArgs) Handles tm_ComTimeout.Tick

        tm_ComTimeout.Stop()

        ' Nur relevant, wenn tatsaechlich noch ein Automatenschritt aussteht - der Timer kann
        ' durch normales Timing auch nach einem laengst abgeschlossenen Zyklus noch feuern

        If Not (FLAG_Read_Request Or FLAG_Read_IO Or FLAG_Write_Request Or
                FLAG_Write_IO Or FLAG_Write_Program Or FLAG_Write_Refresh) Then Exit Sub

        IOTimeoutRetryCount += 1

        If IOTimeoutRetryCount > IOTIMEOUT_RETRIES_MAX Then

            ' Endgueltig aufgeben - sauber abbrechen statt haengen bleiben

            If FLAG_Write_Request Or FLAG_Write_IO Or FLAG_Write_Program Or FLAG_Write_Refresh Then
                COMUpdateStop()
            Else
                COMRefreshStop()
            End If

        Else

            ' Letzte Anfrage erneut senden - bewusst kein COMMsgSendAndArm(), das wuerde den
            ' Retry-Zaehler zuruecksetzen; Watchdog wird trotzdem erneut scharfgeschaltet

            COMMsgSend(LastIOMsg)
            tm_ComTimeout.Start()

        End If

    End Sub

    '++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    '+++ Modul-Registerkarten                                                                   +++
    '+++ Aktionen fuer die Steuerelemente auf den Registerkarten der Module                     +++
    '++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    '**********************************************************************************************
    '*** Register - Modul - mbc-80-Aktionen                                                     ***
    '**********************************************************************************************

    Private Sub Register_mbc80_txt_BTPWD_TextChanged(sender As Object, e As EventArgs) Handles txt_mbc80_Passwort.TextChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-80").Modul(AktNr).Tabpage_PWDSet()
    End Sub

    Private Sub Register_mbc80_txt_BTPWD_KeyPress(sender As Object, e As KeyPressEventArgs) Handles txt_mbc80_Passwort.KeyPress
        ' Nur Zahlen zulassen
        Select Case Asc(e.KeyChar)
            Case 48 To 57, 8
            Case Else
                e.Handled = True
        End Select
    End Sub

    Private Sub Register_mbc80_txt_BTName_TextChanged(sender As Object, e As EventArgs) Handles txt_mbc80_Name.TextChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-80").Modul(AktNr).Tabpage_NameSet()
    End Sub

    Private Sub Register_mbc80_txt_BTName_KeyPress(sender As Object, e As KeyPressEventArgs) Handles txt_mbc80_Name.KeyPress
        ' Nur Grossbuchstaben zulassen
        Select Case Asc(e.KeyChar)
            Case 65 To 90, 8
            Case Else
                e.Handled = True
        End Select
    End Sub

    Private Sub Register_mbc80_nud_SW_1_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc80_SW1.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-80").Modul(AktNr).Tabpage_GFPSWSet()
    End Sub

    Private Sub Register_mbc80_nud_SW_2_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc80_SW2.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-80").Modul(AktNr).Tabpage_GFPSWSet()
    End Sub

    '**********************************************************************************************
    '*** Register - Modul - mbc-81-Aktionen                                                     ***
    '**********************************************************************************************

    Private Sub Register_mbc81_nud_MMAddr_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc81_LocID.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-81").Modul(AktNr).Tabpage_AddrSet()
    End Sub
    Private Sub mbc81_cmb_Nutzung_SelectedIndexChanged(sender As Object, e As EventArgs) Handles cmb_mbc81_Nutzung.SelectedIndexChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-81").Modul(AktNr).Tabpage_Bus_0()
    End Sub

    '**********************************************************************************************
    '*** Register - Modul - mbc-82-Aktionen                                                     ***
    '**********************************************************************************************

    Private Sub Register_mbc82_nud_SW_1_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc82_GFPSW1.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-82").Modul(AktNr).Tabpage_GFPSWSet()
    End Sub

    Private Sub Register_mbc82_nud_SW_2_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc82_GFPSW2.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-82").Modul(AktNr).Tabpage_GFPSWSet()
    End Sub

    Private Sub Register_mbc82_nud_latenz_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc82_Latenz.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-82").Modul(AktNr).Tabpage_KurzLatenz()
    End Sub

    '**********************************************************************************************
    '*** Register - Modul - mbc-83-Aktionen                                                     ***
    '**********************************************************************************************

    Private Sub Register_nud_mbc83_MMAddr_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc83_MMAddr.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-83").Modul(AktNr).Tabpage_AddrSet()
    End Sub

    Private Sub Register_nud_mbc83_Dimmer_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc83_Dimmer.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-83").Modul(AktNr).Tabpage_DimmerSet()
    End Sub

    Private Sub Register_nud_mbc83_Aus_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc83_Aus.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-83").Modul(AktNr).Tabpage_AusSet()
    End Sub

    Private Sub Register_nud_mbc83_Rueck_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc83_Rueck.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-83").Modul(AktNr).Tabpage_RueckSet()
    End Sub

    Private Sub Register_nud_mbc83_TIO_CH1_ValueChanged(sender As Object, e As EventArgs) Handles gb_mbc83_CH1_nud_TIO.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-83").Modul(AktNr).Tabpage_TimeSet(gb_mbc83_CH1_nud_TIO, MBC.ModulTyp(AktTyp).Modul(AktNr).MMTiming_1)
    End Sub

    Private Sub Register_nud_mbc83_TIO_CH2_ValueChanged(sender As Object, e As EventArgs) Handles gb_mbc83_CH2_nud_TIO.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-83").Modul(AktNr).Tabpage_TimeSet(gb_mbc83_CH2_nud_TIO, MBC.ModulTyp(AktTyp).Modul(AktNr).MMTiming_2)
    End Sub

    Private Sub Register_nud_mbc83_TIO_CH3_ValueChanged(sender As Object, e As EventArgs) Handles gb_mbc83_CH3_nud_TIO.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-83").Modul(AktNr).Tabpage_TimeSet(gb_mbc83_CH3_nud_TIO, MBC.ModulTyp(AktTyp).Modul(AktNr).MMTiming_3)
    End Sub

    Private Sub Register_nud_mbc83_TIO_CH4_ValueChanged(sender As Object, e As EventArgs) Handles gb_mbc83_CH4_nud_TIO.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-83").Modul(AktNr).Tabpage_TimeSet(gb_mbc83_CH4_nud_TIO, MBC.ModulTyp(AktTyp).Modul(AktNr).MMTiming_4)
    End Sub

    Private Sub Register_mbc83_clb_CH1_SelectedIndexChanged(sender As Object, e As EventArgs) Handles gb_mbc83_CH1_clb_Typ.SelectedIndexChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-83").Modul(AktNr).Tabpage_NutzungSet(gb_mbc83_CH1_clb_Typ, 1)
    End Sub

    Private Sub Register_mbc83_clb_CH2_SelectedIndexChanged(sender As Object, e As EventArgs) Handles gb_mbc83_CH2_clb_Typ.SelectedIndexChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-83").Modul(AktNr).Tabpage_NutzungSet(gb_mbc83_CH2_clb_Typ, 2)
    End Sub

    Private Sub Register_mbc83_clb_CH3_SelectedIndexChanged(sender As Object, e As EventArgs) Handles gb_mbc83_CH3_clb_Typ.SelectedIndexChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-83").Modul(AktNr).Tabpage_NutzungSet(gb_mbc83_CH3_clb_Typ, 3)
    End Sub

    Private Sub Register_mbc83_clb_CH4_SelectedIndexChanged(sender As Object, e As EventArgs) Handles gb_mbc83_CH4_clb_Typ.SelectedIndexChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-83").Modul(AktNr).Tabpage_NutzungSet(gb_mbc83_CH4_clb_Typ, 4)
    End Sub

    '**********************************************************************************************
    '*** Register - Modul - mbc-84-Aktionen                                                     ***
    '**********************************************************************************************

    Private Sub Register_nud_mbc84_MMAddr_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc84_MMAddr.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-84").Modul(AktNr).Tabpage_AddrSet()
    End Sub

    Private Sub Register_nud_mbc84_Links_CH1_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc84_Links_CH1.ValueChanged, nud_mbc84_Links_CH1.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-84").Modul(AktNr).Tabpage_ServoSet(nud_mbc84_Links_CH1, MBC.ModulTyp(AktTyp).Modul(AktNr).MMServo_L1)
    End Sub

    Private Sub Register_nud_mbc84_Rechts_CH1_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc84_Rechts_CH1.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-84").Modul(AktNr).Tabpage_ServoSet(nud_mbc84_Rechts_CH1, MBC.ModulTyp(AktTyp).Modul(AktNr).MMServo_R1)
    End Sub

    Private Sub Register_nud_mbc84_Links_CH2_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc84_Links_CH2.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-84").Modul(AktNr).Tabpage_ServoSet(nud_mbc84_Links_CH2, MBC.ModulTyp(AktTyp).Modul(AktNr).MMServo_L2)
    End Sub

    Private Sub Register_nud_mbc84_Rechts_CH2_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc84_Rechts_CH2.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-84").Modul(AktNr).Tabpage_ServoSet(nud_mbc84_Rechts_CH2, MBC.ModulTyp(AktTyp).Modul(AktNr).MMServo_R2)
    End Sub

    Private Sub Register_nud_mbc84_Links_CH3_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc84_Links_CH3.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-84").Modul(AktNr).Tabpage_ServoSet(nud_mbc84_Links_CH3, MBC.ModulTyp(AktTyp).Modul(AktNr).MMServo_L3)
    End Sub

    Private Sub Register_nud_mbc84_Rechts_CH3_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc84_Rechts_CH3.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-84").Modul(AktNr).Tabpage_ServoSet(nud_mbc84_Rechts_CH3, MBC.ModulTyp(AktTyp).Modul(AktNr).MMServo_R3)
    End Sub

    Private Sub Register_nud_mbc84_Links_CH4_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc84_Links_CH4.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-84").Modul(AktNr).Tabpage_ServoSet(nud_mbc84_Links_CH4, MBC.ModulTyp(AktTyp).Modul(AktNr).MMServo_L4)
    End Sub

    Private Sub Register_nud_mbc84_Rechts_CH4_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc84_Rechts_CH4.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-84").Modul(AktNr).Tabpage_ServoSet(nud_mbc84_Rechts_CH4, MBC.ModulTyp(AktTyp).Modul(AktNr).MMServo_R4)
    End Sub

    Private Sub Register_nud_mbc84_Step_CH1_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc84_Step_CH1.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-84").Modul(AktNr).Tabpage_ServoSet(nud_mbc84_Step_CH1, MBC.ModulTyp(AktTyp).Modul(AktNr).MMServo_1VZ)
    End Sub

    Private Sub Register_nud_mbc84_Step_CH2_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc84_Step_CH2.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-84").Modul(AktNr).Tabpage_ServoSet(nud_mbc84_Step_CH2, MBC.ModulTyp(AktTyp).Modul(AktNr).MMServo_2VZ)
    End Sub

    Private Sub Register_nud_mbc84_Step_CH3_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc84_Step_CH3.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-84").Modul(AktNr).Tabpage_ServoSet(nud_mbc84_Step_CH3, MBC.ModulTyp(AktTyp).Modul(AktNr).MMServo_3VZ)
    End Sub

    Private Sub Register_nud_mbc84_Step_CH4_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc84_Step_CH4.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-84").Modul(AktNr).Tabpage_ServoSet(nud_mbc84_Step_CH4, MBC.ModulTyp(AktTyp).Modul(AktNr).MMServo_4VZ)
    End Sub

    Private Sub Register_btn_mbc84_Weg_CH1_Click(sender As Object, e As EventArgs) Handles btn_mbc84_Weg_CH1.Click
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-84").Modul(AktNr).Tabpage_Servos_Show(0)
    End Sub

    Private Sub Register_btn_mbc84_Weg_CH2_Click(sender As Object, e As EventArgs) Handles btn_mbc84_Weg_CH2.Click
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-84").Modul(AktNr).Tabpage_Servos_Show(1)
    End Sub

    Private Sub Register_btn_mbc84_Weg_CH3_Click(sender As Object, e As EventArgs) Handles btn_mbc84_Weg_CH3.Click
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-84").Modul(AktNr).Tabpage_Servos_Show(2)
    End Sub

    Private Sub Register_btn_mbc84_Weg_CH4_Click(sender As Object, e As EventArgs) Handles btn_mbc84_Weg_CH4.Click
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-84").Modul(AktNr).Tabpage_Servos_Show(3)
    End Sub

    Private Sub Register_nud_mbc84_Rueck_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc84_Rueck.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-84").Modul(AktNr).Tabpage_RueckSet()
    End Sub

    '**********************************************************************************************
    '*** Register - Modul - mbc-85-Aktionen                                                     ***
    '**********************************************************************************************

    Private Sub Register_nud_mbc85_T_P01_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc85_T_P01.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-85").Modul(AktNr).Tabpage_TimeSet(nud_mbc85_T_P01, MBC.ModulTyp(AktTyp).Modul(AktNr).S88VZ_01)
    End Sub

    Private Sub Register_nud_mbc85_T_P02_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc85_T_P02.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-85").Modul(AktNr).Tabpage_TimeSet(nud_mbc85_T_P02, MBC.ModulTyp(AktTyp).Modul(AktNr).S88VZ_02)
    End Sub

    Private Sub Register_nud_mbc85_T_P03_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc85_T_P03.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-85").Modul(AktNr).Tabpage_TimeSet(nud_mbc85_T_P03, MBC.ModulTyp(AktTyp).Modul(AktNr).S88VZ_03)
    End Sub

    Private Sub Register_nud_mbc85_T_P04_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc85_T_P04.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-85").Modul(AktNr).Tabpage_TimeSet(nud_mbc85_T_P04, MBC.ModulTyp(AktTyp).Modul(AktNr).S88VZ_04)
    End Sub

    Private Sub Register_nud_mbc85_T_P05_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc85_T_P05.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-85").Modul(AktNr).Tabpage_TimeSet(nud_mbc85_T_P05, MBC.ModulTyp(AktTyp).Modul(AktNr).S88VZ_05)
    End Sub

    Private Sub Register_nud_mbc85_T_P06_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc85_T_P06.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-85").Modul(AktNr).Tabpage_TimeSet(nud_mbc85_T_P06, MBC.ModulTyp(AktTyp).Modul(AktNr).S88VZ_06)
    End Sub

    Private Sub Register_nud_mbc85_T_P07_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc85_T_P07.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-85").Modul(AktNr).Tabpage_TimeSet(nud_mbc85_T_P07, MBC.ModulTyp(AktTyp).Modul(AktNr).S88VZ_07)
    End Sub

    Private Sub Register_nud_mbc85_T_P08_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc85_T_P08.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-85").Modul(AktNr).Tabpage_TimeSet(nud_mbc85_T_P08, MBC.ModulTyp(AktTyp).Modul(AktNr).S88VZ_08)
    End Sub

    Private Sub Register_nud_mbc85_T_P09_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc85_T_P09.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-85").Modul(AktNr).Tabpage_TimeSet(nud_mbc85_T_P09, MBC.ModulTyp(AktTyp).Modul(AktNr).S88VZ_09)
    End Sub

    Private Sub Register_nud_mbc85_T_P10_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc85_T_P10.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-85").Modul(AktNr).Tabpage_TimeSet(nud_mbc85_T_P10, MBC.ModulTyp(AktTyp).Modul(AktNr).S88VZ_10)
    End Sub

    Private Sub Register_nud_mbc85_T_P11_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc85_T_P11.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-85").Modul(AktNr).Tabpage_TimeSet(nud_mbc85_T_P11, MBC.ModulTyp(AktTyp).Modul(AktNr).S88VZ_11)
    End Sub

    Private Sub Register_nud_mbc85_S88ModNr_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc85_S88ModNr.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-85").Modul(AktNr).Tabpage_AddrSet()
    End Sub

    Private Sub Register_nud_mbc85_mmlok_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc85_MMLok.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-85").Modul(AktNr).Tabpage_MM_Set()
    End Sub

    Private Sub Register_nud_mbc85_MMSpeed_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc85_MMSpeed.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-85").Modul(AktNr).Tabpage_MMSpeed_Set()
    End Sub

    Private Sub Register_btn_mbc85_Winkel_Click(sender As Object, e As EventArgs) Handles btn_mbc85_Winkel.Click
        frmWinkel.Show(Me)
    End Sub

    '**********************************************************************************************
    '*** Register - Modul - mbc-86-Aktionen                                                     ***
    '**********************************************************************************************

    Private Sub nud_mbc86_S88ModNr_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc86_S88ModNr.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-86").Modul(AktNr).Tabpage_AddrSet()
    End Sub

    Private Sub nud_mbc86_T_P01_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc86_T_P01.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-86").Modul(AktNr).Tabpage_TimeSet(nud_mbc86_T_P01, MBC.ModulTyp(AktTyp).Modul(AktNr).S88VZ_01)
    End Sub

    Private Sub nud_mbc86_T_P02_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc86_T_P02.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-86").Modul(AktNr).Tabpage_TimeSet(nud_mbc86_T_P02, MBC.ModulTyp(AktTyp).Modul(AktNr).S88VZ_02)
    End Sub

    Private Sub nud_mbc86_T_P03_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc86_T_P03.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-86").Modul(AktNr).Tabpage_TimeSet(nud_mbc86_T_P03, MBC.ModulTyp(AktTyp).Modul(AktNr).S88VZ_03)
    End Sub

    Private Sub nud_mbc86_T_P04_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc86_T_P04.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-86").Modul(AktNr).Tabpage_TimeSet(nud_mbc86_T_P04, MBC.ModulTyp(AktTyp).Modul(AktNr).S88VZ_04)
    End Sub

    Private Sub nud_mbc86_T_P05_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc86_T_P05.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-86").Modul(AktNr).Tabpage_TimeSet(nud_mbc86_T_P05, MBC.ModulTyp(AktTyp).Modul(AktNr).S88VZ_05)
    End Sub

    Private Sub nud_mbc86_T_P06_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc86_T_P06.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-86").Modul(AktNr).Tabpage_TimeSet(nud_mbc86_T_P06, MBC.ModulTyp(AktTyp).Modul(AktNr).S88VZ_06)
    End Sub

    Private Sub nud_mbc86_T_P07_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc86_T_P07.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-86").Modul(AktNr).Tabpage_TimeSet(nud_mbc86_T_P07, MBC.ModulTyp(AktTyp).Modul(AktNr).S88VZ_07)
    End Sub

    Private Sub nud_mbc86_T_P08_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc86_T_P08.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-86").Modul(AktNr).Tabpage_TimeSet(nud_mbc86_T_P08, MBC.ModulTyp(AktTyp).Modul(AktNr).S88VZ_08)
    End Sub

    Private Sub nud_mbc86_MMLok_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc86_MMLok.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-86").Modul(AktNr).Tabpage_MM_Set()
    End Sub

    '**********************************************************************************************
    '*** Register - Modul - mbc-87-Aktionen                                                     ***
    '**********************************************************************************************
    Private Sub Register_mbc87_nud_MMAddr_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc87_MMAddr.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-87").Modul(AktNr).Tabpage_AddrSet()
    End Sub


    '**********************************************************************************************
    '*** Register - Modul - mbc-88-Aktionen                                                     ***
    '**********************************************************************************************

    Private Sub Register_nud_mbc88_s88Addr_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc88_S88ModNr.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-88").Modul(AktNr).Tabpage_AddrSet()
    End Sub

    Private Sub cmb_mbc88_Halbmodul_SelectedIndexChanged(sender As Object, e As EventArgs) Handles cmb_mbc88_Halbmodul.SelectedIndexChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-88").Modul(AktNr).Tabpage_HalbmodulSet()
    End Sub

    Private Sub Register_nud_mbc88_T_P01_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc88_T_P01.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-88").Modul(AktNr).Tabpage_TimeSet(nud_mbc88_T_P01, MBC.ModulTyp(AktTyp).Modul(AktNr).S88VZ_01)
    End Sub

    Private Sub Register_nud_mbc88_T_P02_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc88_T_P02.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-88").Modul(AktNr).Tabpage_TimeSet(nud_mbc88_T_P02, MBC.ModulTyp(AktTyp).Modul(AktNr).S88VZ_02)
    End Sub

    Private Sub Register_nud_mbc88_T_P03_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc88_T_P03.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-88").Modul(AktNr).Tabpage_TimeSet(nud_mbc88_T_P03, MBC.ModulTyp(AktTyp).Modul(AktNr).S88VZ_03)
    End Sub

    Private Sub Register_nud_mbc88_T_P04_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc88_T_P04.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-88").Modul(AktNr).Tabpage_TimeSet(nud_mbc88_T_P04, MBC.ModulTyp(AktTyp).Modul(AktNr).S88VZ_04)
    End Sub

    Private Sub Register_nud_mbc88_T_P05_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc88_T_P05.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-88").Modul(AktNr).Tabpage_TimeSet(nud_mbc88_T_P05, MBC.ModulTyp(AktTyp).Modul(AktNr).S88VZ_05)
    End Sub

    Private Sub Register_nud_mbc88_T_P06_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc88_T_P06.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-88").Modul(AktNr).Tabpage_TimeSet(nud_mbc88_T_P06, MBC.ModulTyp(AktTyp).Modul(AktNr).S88VZ_06)
    End Sub

    Private Sub Register_nud_mbc88_T_P07_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc88_T_P07.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-88").Modul(AktNr).Tabpage_TimeSet(nud_mbc88_T_P07, MBC.ModulTyp(AktTyp).Modul(AktNr).S88VZ_07)
    End Sub

    Private Sub Register_nud_mbc88_T_P08_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc88_T_P08.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-88").Modul(AktNr).Tabpage_TimeSet(nud_mbc88_T_P08, MBC.ModulTyp(AktTyp).Modul(AktNr).S88VZ_08)
    End Sub

    Private Sub Register_nud_mbc88_T_P09_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc88_T_P09.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-88").Modul(AktNr).Tabpage_TimeSet(nud_mbc88_T_P09, MBC.ModulTyp(AktTyp).Modul(AktNr).S88VZ_01)
    End Sub

    Private Sub Register_nud_mbc88_T_P10_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc88_T_P10.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-88").Modul(AktNr).Tabpage_TimeSet(nud_mbc88_T_P10, MBC.ModulTyp(AktTyp).Modul(AktNr).S88VZ_02)
    End Sub

    Private Sub Register_nud_mbc88_T_P11_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc88_T_P11.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-88").Modul(AktNr).Tabpage_TimeSet(nud_mbc88_T_P11, MBC.ModulTyp(AktTyp).Modul(AktNr).S88VZ_03)
    End Sub

    Private Sub Register_nud_mbc88_T_P12_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc88_T_P12.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-88").Modul(AktNr).Tabpage_TimeSet(nud_mbc88_T_P12, MBC.ModulTyp(AktTyp).Modul(AktNr).S88VZ_04)
    End Sub

    Private Sub Register_nud_mbc88_T_P13_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc88_T_P13.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-88").Modul(AktNr).Tabpage_TimeSet(nud_mbc88_T_P13, MBC.ModulTyp(AktTyp).Modul(AktNr).S88VZ_05)
    End Sub

    Private Sub Register_nud_mbc88_T_P14_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc88_T_P14.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-88").Modul(AktNr).Tabpage_TimeSet(nud_mbc88_T_P14, MBC.ModulTyp(AktTyp).Modul(AktNr).S88VZ_06)
    End Sub

    Private Sub Register_nud_mbc88_T_P15_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc88_T_P15.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-88").Modul(AktNr).Tabpage_TimeSet(nud_mbc88_T_P15, MBC.ModulTyp(AktTyp).Modul(AktNr).S88VZ_07)
    End Sub

    Private Sub Register_nud_mbc88_T_P16_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc88_T_P16.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-88").Modul(AktNr).Tabpage_TimeSet(nud_mbc88_T_P16, MBC.ModulTyp(AktTyp).Modul(AktNr).S88VZ_08)
    End Sub

    '**********************************************************************************************
    '*** Register - Modul - mbc-89-Aktionen                                                     ***
    '**********************************************************************************************

    Private Sub Register_clb_mbc89_A_SelectedIndexChanged(sender As Object, e As EventArgs) Handles clb_mbc89_A.SelectedIndexChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-89").Modul(AktNr).Tabpage_Protokoll_Set(clb_mbc89_A, AktTyp, AktNr, 1)
    End Sub

    Private Sub Register_clb_mbc89_B_SelectedIndexChanged(sender As Object, e As EventArgs) Handles clb_mbc89_B.SelectedIndexChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-89").Modul(AktNr).Tabpage_Protokoll_Set(clb_mbc89_B, AktTyp, AktNr, 2)
    End Sub

    Private Sub Register_clb_mbc89_C_SelectedIndexChanged(sender As Object, e As EventArgs) Handles clb_mbc89_C.SelectedIndexChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-89").Modul(AktNr).Tabpage_Protokoll_Set(clb_mbc89_C, AktTyp, AktNr, 3)
    End Sub

    Private Sub Register_clb_mbc89_D_SelectedIndexChanged(sender As Object, e As EventArgs) Handles clb_mbc89_D.SelectedIndexChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-89").Modul(AktNr).Tabpage_Protokoll_Set(clb_mbc89_D, AktTyp, AktNr, 4)
    End Sub

    Private Sub Register_clb_mbc89_Zentrale_SelectedIndexChanged(sender As Object, e As EventArgs) Handles clb_mbc89_Mapping.SelectedIndexChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-89").Modul(AktNr).Tabpage_Mapping_Set(clb_mbc89_Mapping, AktTyp, AktNr)
    End Sub

    Private Sub Register_mbc89_nud_keyboard_nr_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc89_keyboard_nr.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-89").Modul(AktNr).Tabpage_Keyboard_Set()
    End Sub

    Private Sub Register_mbc89_nud_SW_1_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc89_SW_1.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-89").Modul(AktNr).Tabpage_SW_Set()
    End Sub

    Private Sub Register_mbc89_nud_SW_2_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc89_SW_2.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-89").Modul(AktNr).Tabpage_SW_Set()
    End Sub

    Private Sub Register_mbc89_nud_basis_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc89_basis.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-89").Modul(AktNr).Tabpage_Basis_Set()
    End Sub

    Private Sub Register_nud_mbc89_i2c_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc89_i2c.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-89").Modul(AktNr).Tabpage_I2C_Set()
    End Sub

    Private Sub Register_mbc89_btnLoks_Click(sender As Object, e As EventArgs) Handles btn_mbc89_Loks.Click
        frmLoks.Show(Me)
    End Sub

    '**********************************************************************************************
    '*** Register - Modul - mbc-90-Aktionen                                                     ***
    '**********************************************************************************************

    Private Sub Register_nud_mbc90_S88ModNr_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc90_S88ModNr.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-90").Modul(AktNr).Tabpage_AddrSet()
    End Sub

    Private Sub Register_nud_mbc90_T_P01_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc90_T_P01.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-90").Modul(AktNr).Tabpage_TimeSet(nud_mbc90_T_P01, MBC.ModulTyp(AktTyp).Modul(AktNr).S88VZ_01)
    End Sub

    Private Sub Register_nud_mbc90_T_P02_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc90_T_P02.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-90").Modul(AktNr).Tabpage_TimeSet(nud_mbc90_T_P02, MBC.ModulTyp(AktTyp).Modul(AktNr).S88VZ_02)
    End Sub

    Private Sub Register_nud_mbc90_T_P03_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc90_T_P03.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-90").Modul(AktNr).Tabpage_TimeSet(nud_mbc90_T_P03, MBC.ModulTyp(AktTyp).Modul(AktNr).S88VZ_03)
    End Sub

    Private Sub Register_nud_mbc90_T_P04_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc90_T_P04.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-90").Modul(AktNr).Tabpage_TimeSet(nud_mbc90_T_P04, MBC.ModulTyp(AktTyp).Modul(AktNr).S88VZ_04)
    End Sub

    Private Sub Register_nud_mbc90_T_P05_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc90_T_P05.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-90").Modul(AktNr).Tabpage_TimeSet(nud_mbc90_T_P05, MBC.ModulTyp(AktTyp).Modul(AktNr).S88VZ_05)
    End Sub

    Private Sub Register_nud_mbc90_T_P06_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc90_T_P06.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-90").Modul(AktNr).Tabpage_TimeSet(nud_mbc90_T_P06, MBC.ModulTyp(AktTyp).Modul(AktNr).S88VZ_06)
    End Sub

    Private Sub Register_nud_mbc90_T_P07_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc90_T_P07.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-90").Modul(AktNr).Tabpage_TimeSet(nud_mbc90_T_P07, MBC.ModulTyp(AktTyp).Modul(AktNr).S88VZ_07)
    End Sub

    Private Sub Register_nud_mbc90_T_P08_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc90_T_P08.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-90").Modul(AktNr).Tabpage_TimeSet(nud_mbc90_T_P08, MBC.ModulTyp(AktTyp).Modul(AktNr).S88VZ_08)
    End Sub

    Private Sub Register_nud_mbc90_T_P09_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc90_T_P09.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-90").Modul(AktNr).Tabpage_TimeSet(nud_mbc90_T_P09, MBC.ModulTyp(AktTyp).Modul(AktNr).S88VZ_09)
    End Sub

    Private Sub Register_nud_mbc90_T_P10_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc90_T_P10.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-90").Modul(AktNr).Tabpage_TimeSet(nud_mbc90_T_P10, MBC.ModulTyp(AktTyp).Modul(AktNr).S88VZ_10)
    End Sub

    Private Sub Register_nud_mbc90_T_P11_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc90_T_P11.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-90").Modul(AktNr).Tabpage_TimeSet(nud_mbc90_T_P11, MBC.ModulTyp(AktTyp).Modul(AktNr).S88VZ_11)
    End Sub

    Private Sub Register_nud_mbc90_T_P12_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc90_T_P12.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-90").Modul(AktNr).Tabpage_TimeSet(nud_mbc90_T_P12, MBC.ModulTyp(AktTyp).Modul(AktNr).S88VZ_12)
    End Sub

    Private Sub Register_nud_mbc90_T_P13_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc90_T_P13.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-90").Modul(AktNr).Tabpage_TimeSet(nud_mbc90_T_P13, MBC.ModulTyp(AktTyp).Modul(AktNr).S88VZ_13)
    End Sub

    Private Sub Register_nud_mbc90_T_P14_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc90_T_P14.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-90").Modul(AktNr).Tabpage_TimeSet(nud_mbc90_T_P14, MBC.ModulTyp(AktTyp).Modul(AktNr).S88VZ_14)
    End Sub

    Private Sub Register_nud_mbc90_T_P15_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc90_T_P15.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-90").Modul(AktNr).Tabpage_TimeSet(nud_mbc90_T_P15, MBC.ModulTyp(AktTyp).Modul(AktNr).S88VZ_15)
    End Sub

    Private Sub Register_nud_mbc90_T_P16_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc90_T_P16.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-90").Modul(AktNr).Tabpage_TimeSet(nud_mbc90_T_P16, MBC.ModulTyp(AktTyp).Modul(AktNr).S88VZ_16)
    End Sub



    '**********************************************************************************************
    '*** Register - Modul - mbc-91-Aktionen                                                     ***
    '**********************************************************************************************

    Private Sub Register_nud_mbc91_MMAddr_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc91_MMAddr.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-91").Modul(AktNr).Tabpage_AddrSet()
    End Sub

    Private Sub Register_nud_mbc91_Aus_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc91_Aus.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-91").Modul(AktNr).Tabpage_AusSet()
    End Sub

    Private Sub Register_nud_mbc91_Rueck_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc91_Rueck.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-91").Modul(AktNr).Tabpage_RueckSet()
    End Sub

    Private Sub Register_nud_mbc91_TIO_CH1_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc91_CH1_TIO.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-91").Modul(AktNr).Tabpage_TimeSet(nud_mbc91_CH1_TIO, MBC.ModulTyp(AktTyp).Modul(AktNr).MMTiming_1)
    End Sub

    Private Sub Register_nud_mbc91_TIO_CH2_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc91_CH2_TIO.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-91").Modul(AktNr).Tabpage_TimeSet(nud_mbc91_CH2_TIO, MBC.ModulTyp(AktTyp).Modul(AktNr).MMTiming_2)
    End Sub

    Private Sub Register_nud_mbc91_TIO_CH3_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc91_CH3_TIO.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-91").Modul(AktNr).Tabpage_TimeSet(nud_mbc91_CH3_TIO, MBC.ModulTyp(AktTyp).Modul(AktNr).MMTiming_3)
    End Sub

    Private Sub Register_nud_mbc91_TIO_CH4_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc91_CH4_TIO.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-91").Modul(AktNr).Tabpage_TimeSet(nud_mbc91_CH4_TIO, MBC.ModulTyp(AktTyp).Modul(AktNr).MMTiming_4)
    End Sub

    '**********************************************************************************************
    '*** Register - Modul - mbc-92-Aktionen                                                     ***
    '**********************************************************************************************

    Private Sub Register_nud_mbc92_MMAddr_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc92_MMAddr.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-92").Modul(AktNr).Tabpage_AddrSet()
    End Sub

    Private Sub Register_nud_mbc92_Aus_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc92_Aus.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-92").Modul(AktNr).Tabpage_AusSet()
    End Sub

    Private Sub Register_nud_mbc92_Rueck_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc92_Rueck.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-92").Modul(AktNr).Tabpage_RueckSet()
    End Sub

    Private Sub Register_nud_mbc92_TIO_CH1_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc92_CH1_TIO.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-92").Modul(AktNr).Tabpage_TimeSet(nud_mbc92_CH1_TIO, MBC.ModulTyp(AktTyp).Modul(AktNr).MMTiming_1)
    End Sub

    Private Sub Register_nud_mbc92_TIO_CH2_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc92_CH2_TIO.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-92").Modul(AktNr).Tabpage_TimeSet(nud_mbc92_CH2_TIO, MBC.ModulTyp(AktTyp).Modul(AktNr).MMTiming_2)
    End Sub

    Private Sub Register_nud_mbc92_TIO_CH3_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc92_CH3_TIO.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-92").Modul(AktNr).Tabpage_TimeSet(nud_mbc92_CH3_TIO, MBC.ModulTyp(AktTyp).Modul(AktNr).MMTiming_3)
    End Sub

    Private Sub Register_nud_mbc92_TIO_CH4_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc92_CH4_TIO.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-92").Modul(AktNr).Tabpage_TimeSet(nud_mbc92_CH4_TIO, MBC.ModulTyp(AktTyp).Modul(AktNr).MMTiming_4)
    End Sub

    Private Sub clb_mbc92_CH1_SelectedIndexChanged(sender As Object, e As EventArgs) Handles clb_mbc92_CH1.SelectedIndexChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-92").Modul(AktNr).Tabpage_NutzungSet(clb_mbc92_CH1, 1)
    End Sub

    Private Sub clb_mbc92_CH2_SelectedIndexChanged(sender As Object, e As EventArgs) Handles clb_mbc92_CH2.SelectedIndexChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-92").Modul(AktNr).Tabpage_NutzungSet(clb_mbc92_CH2, 2)
    End Sub

    Private Sub clb_mbc92_CH3_SelectedIndexChanged(sender As Object, e As EventArgs) Handles clb_mbc92_CH3.SelectedIndexChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-92").Modul(AktNr).Tabpage_NutzungSet(clb_mbc92_CH3, 3)
    End Sub

    Private Sub clb_mbc92_CH4_SelectedIndexChanged(sender As Object, e As EventArgs) Handles clb_mbc92_CH4.SelectedIndexChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-92").Modul(AktNr).Tabpage_NutzungSet(clb_mbc92_CH4, 4)
    End Sub


    '**********************************************************************************************
    '*** Register - Modul - mbc-82-Aktionen                                                     ***
    '**********************************************************************************************

    Private Sub Register_nud_mbc93_GFPSW1_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc93_GFPSW1.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-93").Modul(AktNr).Tabpage_GFPSWSet()
    End Sub

    Private Sub Register_nud_mbc93_GFPSW2_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc93_GFPSW2.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-93").Modul(AktNr).Tabpage_GFPSWSet()
    End Sub

    Private Sub Register_nud_mbc93_Latenz_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc93_Latenz.ValueChanged
        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp("mbc-93").Modul(AktNr).Tabpage_KurzLatenz()
    End Sub

    '**********************************************************************************************
    '*** Register - Modul - Button-Aktionen                                                     ***
    '**********************************************************************************************

    Private Sub Register_btnRefresh_Click(sender As Object, e As EventArgs) Handles btn_ControlRefresh.Click

        MBC.ModulTyp(AktTyp).Modul(AktNr).Tabpage_Aktivieren()

        ' Starte Auslesen des Moduls

        COMRefreshStart(MBC_ALLG_START, MBC.ModulTyp(AktTyp).Modul(AktNr).IOEnde)

    End Sub

    Private Sub Register_btnVerwerfen_Click(sender As Object, e As EventArgs) Handles btn_ControlVerwerfen.Click

        If FLAG_Prog_Index = PROG_INDEX_Upgrade Then

            COMUpgradeStop()

        Else

            MBC.ModulTyp(AktTyp).Modul(AktNr).Tabpage_Aktivieren()

            ' Starte Auslesen des Moduls wenn der Abbruch nicht durch

            COMRefreshStart(MBC.ModulTyp(AktTyp).Modul(AktNr).IOStart, MBC.ModulTyp(AktTyp).Modul(AktNr).IOEnde)

        End If

    End Sub

    Private Sub Register_btnUpdate_Click(sender As Object, e As EventArgs) Handles btn_ControlUpdate.Click

        MBC.ModulTyp(AktTyp).Modul(AktNr).Update()

    End Sub

    Private Sub Register_btnUpgrade_Click(sender As Object, e As EventArgs) Handles btn_ControlUpgrade.Click

        ' Zielprozessor abfragen, bevor der eigentliche Upgrade-Ablauf gestartet wird
        ' (Default-Instance wie bei frmWinkel/frmServo/frmLoks, damit SetSprache() die
        ' tatsaechlich angezeigte Instanz beschriftet)

        ' Vorauswahl anhand des dritten Zeichens der Artikelnummer (ART_NR[2] im Sketch: 0=ATmega644P/
        ' 1=ESP32-C3/2=RP2040-Pico/3=RP2040-Zero) - bei zu kurzer/fehlender Artikelnummer (z.B. noch
        ' nicht angemeldetes Modul) bleibt es beim Default ATmega644P.

        Dim artNum As String = MBC.ModulTyp(AktTyp).Modul(AktNr).ArtNum

        If Len(artNum) >= 3 Then
            Select Case Mid(artNum, 3, 1)
                Case "1" : frmProzessorAuswahl.VorschlagProzessor = frmProzessorAuswahl.ProzessorTyp.ESP32C3
                Case "2" : frmProzessorAuswahl.VorschlagProzessor = frmProzessorAuswahl.ProzessorTyp.RP2040Pico
                Case "3" : frmProzessorAuswahl.VorschlagProzessor = frmProzessorAuswahl.ProzessorTyp.RP2040Zero
                Case Else : frmProzessorAuswahl.VorschlagProzessor = frmProzessorAuswahl.ProzessorTyp.ATmega644P
            End Select
        Else
            frmProzessorAuswahl.VorschlagProzessor = frmProzessorAuswahl.ProzessorTyp.ATmega644P
        End If

        If frmProzessorAuswahl.ShowDialog() = DialogResult.OK Then

            Select Case frmProzessorAuswahl.SelectedProzessor

                Case frmProzessorAuswahl.ProzessorTyp.ATmega644P
                    COMUpgradeButton()

                Case frmProzessorAuswahl.ProzessorTyp.RP2040Pico
                    COMUpgradeButtonRP2040Pico()

                Case frmProzessorAuswahl.ProzessorTyp.RP2040Zero
                    COMUpgradeButtonRP2040Zero()

                Case frmProzessorAuswahl.ProzessorTyp.ESP32C3
                    COMUpgradeButtonESP32C3()

            End Select

        End If

    End Sub

    Private Sub Register_btnReset_Click(sender As Object, e As EventArgs) Handles btn_ControlReset.Click

        ' Sende das RESET-Signal an das Modul

        COMMsgSend(MsgReset & MBC.ModulTyp(AktTyp).Modul(AktNr).MsgGUiD)

        ' Deaktiviere das Modul im Strukturbaum und nehme die Selection des Moduls zurueck

        MBC.ModulTyp(AktTyp).Modul(AktNr).Status = IsMissing

        Register_Modul_Aktorstellung_Show()

        Strukturbaum_MenueIconChange(AktTyp, AktNr, False, True)

    End Sub

    Private Sub Register_btnLoeschen_Click(sender As Object, e As EventArgs) Handles btn_ControlLoeschen.Click

        ' Deaktiviere das Modul im Strukturbaum

        btn_ControlLoeschen.Enabled = False

        MBC.ModulTyp(AktTyp).Modul(AktNr).Status = IsDeleted

        Register_Modul_Aktorstellung_Show()

        Strukturbaum_MenueIconChange(AktTyp, AktNr, True, True)

    End Sub

    '**********************************************************************************************
    '*** Register - Modul - Stellpult                                                           ***
    '**********************************************************************************************

    Private Sub Register_btnTest_send(Nummer As Integer, Strom_ein As String, Strom_aus As String)

        Dim h As String

        h = FormHEX("0", 4, Nummer)

        COMMsgSend(MsgSwitchCS2 & Microsoft.VisualBasic.Mid(h, 1, 2) & " " & Microsoft.VisualBasic.Mid(h, 3, 2) & Strom_ein)
        COMMsgSend(MsgSwitchCS2 & Microsoft.VisualBasic.Mid(h, 1, 2) & " " & Microsoft.VisualBasic.Mid(h, 3, 2) & Strom_aus)

    End Sub

    ' Tastengruppe 1
    Private Sub Register_btnTestR1_MouseDown(sender As Object, e As MouseEventArgs) Handles btn_KeyboardR1.MouseDown
        Dim h As String
        h = Register_Get_MMAddr(0, AktTyp, AktNr)
        COMMsgSend(MsgSwitchCS2 & Microsoft.VisualBasic.Mid(h, 3, 2) & " " & Microsoft.VisualBasic.Mid(h, 5, 2) & " 00 01 00 00")
        tv_Strukturbaum.SelectedNode = SelNode
        tv_Strukturbaum.Select()
    End Sub

    Private Sub Register_btnTestR1_MouseUp(sender As Object, e As MouseEventArgs) Handles btn_KeyboardR1.MouseUp
        Dim h As String
        h = Register_Get_MMAddr(0, AktTyp, AktNr)
        COMMsgSend(MsgSwitchCS2 & Microsoft.VisualBasic.Mid(h, 3, 2) & " " & Microsoft.VisualBasic.Mid(h, 5, 2) & " 00 00 00 00")
        tv_Strukturbaum.SelectedNode = SelNode
        tv_Strukturbaum.Select()
    End Sub

    Private Sub Register_btnTestG1_MouseDown(sender As Object, e As MouseEventArgs) Handles btn_KeyboardG1.MouseDown
        Dim h As String
        h = Register_Get_MMAddr(0, AktTyp, AktNr)
        COMMsgSend(MsgSwitchCS2 & Microsoft.VisualBasic.Mid(h, 3, 2) & " " & Microsoft.VisualBasic.Mid(h, 5, 2) & " 01 01 00 00")
        tv_Strukturbaum.SelectedNode = SelNode
        tv_Strukturbaum.Select()
    End Sub

    Private Sub Register_btnTestG1_MouseUp(sender As Object, e As MouseEventArgs) Handles btn_KeyboardG1.MouseUp
        Dim h As String
        h = Register_Get_MMAddr(0, AktTyp, AktNr)
        COMMsgSend(MsgSwitchCS2 & Microsoft.VisualBasic.Mid(h, 3, 2) & " " & Microsoft.VisualBasic.Mid(h, 5, 2) & " 01 00 00 00")
        tv_Strukturbaum.SelectedNode = SelNode
        tv_Strukturbaum.Select()
    End Sub

    ' Tastengruppe 2
    Private Sub Register_btnTestR2_MouseDown(sender As Object, e As MouseEventArgs) Handles btn_KeyboardR2.MouseDown
        Dim h As String
        h = Register_Get_MMAddr(1, AktTyp, AktNr)
        COMMsgSend(MsgSwitchCS2 & Microsoft.VisualBasic.Mid(h, 3, 2) & " " & Microsoft.VisualBasic.Mid(h, 5, 2) & " 00 01 00 00")
        tv_Strukturbaum.SelectedNode = SelNode
        tv_Strukturbaum.Select()
    End Sub

    Private Sub Register_btnTestR2_MouseUp(sender As Object, e As MouseEventArgs) Handles btn_KeyboardR2.MouseUp
        Dim h As String
        h = Register_Get_MMAddr(1, AktTyp, AktNr)
        COMMsgSend(MsgSwitchCS2 & Microsoft.VisualBasic.Mid(h, 3, 2) & " " & Microsoft.VisualBasic.Mid(h, 5, 2) & " 00 00 00 00")
        tv_Strukturbaum.SelectedNode = SelNode
        tv_Strukturbaum.Select()
    End Sub

    Private Sub Register_btnTestG2_MouseDown(sender As Object, e As MouseEventArgs) Handles btn_KeyboardG2.MouseDown
        Dim h As String
        h = Register_Get_MMAddr(1, AktTyp, AktNr)
        COMMsgSend(MsgSwitchCS2 & Microsoft.VisualBasic.Mid(h, 3, 2) & " " & Microsoft.VisualBasic.Mid(h, 5, 2) & " 01 01 00 00")
        tv_Strukturbaum.SelectedNode = SelNode
        tv_Strukturbaum.Select()
    End Sub

    Private Sub Register_btnTestG2_MouseUp(sender As Object, e As MouseEventArgs) Handles btn_KeyboardG2.MouseUp
        Dim h As String
        h = Register_Get_MMAddr(1, AktTyp, AktNr)
        COMMsgSend(MsgSwitchCS2 & Microsoft.VisualBasic.Mid(h, 3, 2) & " " & Microsoft.VisualBasic.Mid(h, 5, 2) & " 01 00 00 00")
        tv_Strukturbaum.SelectedNode = SelNode
        tv_Strukturbaum.Select()
    End Sub

    ' Tastengruppe 3
    Private Sub Register_btnTestR3_MouseDown(sender As Object, e As MouseEventArgs) Handles btn_KeyboardR3.MouseDown
        Dim h As String
        h = Register_Get_MMAddr(2, AktTyp, AktNr)
        COMMsgSend(MsgSwitchCS2 & Microsoft.VisualBasic.Mid(h, 3, 2) & " " & Microsoft.VisualBasic.Mid(h, 5, 2) & " 00 01 00 00")
        tv_Strukturbaum.SelectedNode = SelNode
        tv_Strukturbaum.Select()
    End Sub

    Private Sub Register_btnTestR3_MouseUp(sender As Object, e As MouseEventArgs) Handles btn_KeyboardR3.MouseUp
        Dim h As String
        h = Register_Get_MMAddr(2, AktTyp, AktNr)
        COMMsgSend(MsgSwitchCS2 & Microsoft.VisualBasic.Mid(h, 3, 2) & " " & Microsoft.VisualBasic.Mid(h, 5, 2) & " 00 00 00 00")
        tv_Strukturbaum.SelectedNode = SelNode
        tv_Strukturbaum.Select()
    End Sub

    Private Sub Register_btnTestG3_MouseDown(sender As Object, e As MouseEventArgs) Handles btn_KeyboardG3.MouseDown
        Dim h As String
        h = Register_Get_MMAddr(2, AktTyp, AktNr)
        COMMsgSend(MsgSwitchCS2 & Microsoft.VisualBasic.Mid(h, 3, 2) & " " & Microsoft.VisualBasic.Mid(h, 5, 2) & " 01 01 00 00")
        tv_Strukturbaum.SelectedNode = SelNode
        tv_Strukturbaum.Select()
    End Sub

    Private Sub Register_btnTestG3_MouseUp(sender As Object, e As MouseEventArgs) Handles btn_KeyboardG3.MouseUp
        Dim h As String
        h = Register_Get_MMAddr(2, AktTyp, AktNr)
        COMMsgSend(MsgSwitchCS2 & Microsoft.VisualBasic.Mid(h, 3, 2) & " " & Microsoft.VisualBasic.Mid(h, 5, 2) & " 01 00 00 00")
        tv_Strukturbaum.SelectedNode = SelNode
        tv_Strukturbaum.Select()
    End Sub

    ' Tastengruppe 4
    Private Sub Register_btnTestR4_MouseDown(sender As Object, e As MouseEventArgs) Handles btn_KeyboardR4.MouseDown
        Dim h As String
        h = Register_Get_MMAddr(3, AktTyp, AktNr)
        COMMsgSend(MsgSwitchCS2 & Microsoft.VisualBasic.Mid(h, 3, 2) & " " & Microsoft.VisualBasic.Mid(h, 5, 2) & " 00 01 00 00")
        tv_Strukturbaum.SelectedNode = SelNode
        tv_Strukturbaum.Select()
    End Sub

    Private Sub Register_btnTestR4_MouseUp(sender As Object, e As MouseEventArgs) Handles btn_KeyboardR4.MouseUp
        Dim h As String
        h = Register_Get_MMAddr(3, AktTyp, AktNr)
        COMMsgSend(MsgSwitchCS2 & Microsoft.VisualBasic.Mid(h, 3, 2) & " " & Microsoft.VisualBasic.Mid(h, 5, 2) & " 00 00 00 00")
        tv_Strukturbaum.SelectedNode = SelNode
        tv_Strukturbaum.Select()
    End Sub

    Private Sub Register_btnTestG4_MouseDown(sender As Object, e As MouseEventArgs) Handles btn_KeyboardG4.MouseDown
        Dim h As String
        h = Register_Get_MMAddr(3, AktTyp, AktNr)
        COMMsgSend(MsgSwitchCS2 & Microsoft.VisualBasic.Mid(h, 3, 2) & " " & Microsoft.VisualBasic.Mid(h, 5, 2) & " 01 01 00 00")
        tv_Strukturbaum.SelectedNode = SelNode
        tv_Strukturbaum.Select()
    End Sub

    Private Sub Register_btnTestG4_MouseUp(sender As Object, e As MouseEventArgs) Handles btn_KeyboardG4.MouseUp
        Dim h As String
        h = Register_Get_MMAddr(3, AktTyp, AktNr)
        COMMsgSend(MsgSwitchCS2 & Microsoft.VisualBasic.Mid(h, 3, 2) & " " & Microsoft.VisualBasic.Mid(h, 5, 2) & " 01 00 00 00")
        tv_Strukturbaum.SelectedNode = SelNode
        tv_Strukturbaum.Select()
    End Sub

    '**********************************************************************************************
    '*** Register - Modul - Hilfsfunktion fuer Moduleigenschaften                               ***
    '**********************************************************************************************

    ' Hole die MM-Adresse des Moduls sowie Folgeadressen als HEX-String
    Public Function Register_Get_MMAddr(index As Integer, AktTyp As Integer, AktNr As Integer)

        Register_Get_MMAddr = ""

        If (AktTyp > 0) And (AktNr > 0) Then Register_Get_MMAddr = MBC.ModulTyp(AktTyp).Modul(AktNr).Get_MMAddr(index)

    End Function

    ' Weise MM-Adresse des Moduls zu
    Public Sub Register_Set_MMAddr(addr As Integer, AktTyp As Integer, AktNr As Integer)

        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp(AktTyp).Modul(AktNr).Set_MMAddr(addr)

    End Sub

    ' Hole den MM-Schaltzustand des Moduls
    Public Function Register_Get_MMValue(AktTyp As Integer, AktNr As Integer)

        Register_Get_MMValue = 0

        If (AktTyp > 0) And (AktNr > 0) Then Register_Get_MMValue = MBC.ModulTyp(AktTyp).Modul(AktNr).Get_MMValue()

    End Function

    ' Setze den MM-Schaltzustand des Moduls
    Private Sub Register_Set_MMValue(index As Integer, AktTyp As Integer, AktNr As Integer)

        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp(AktTyp).Modul(AktNr).Set_MMValue(index)

    End Sub

    ' Hole die S88-Modulnummer des Moduls
    Public Function Register_Get_S88ModNr(AktTyp As Integer, AktNr As Integer)

        Register_Get_S88ModNr = 0

        If (AktTyp > 0) And (AktNr > 0) Then Register_Get_S88ModNr = MBC.ModulTyp(AktTyp).Modul(AktNr).Get_S88ModNr()

    End Function

    ' Setze die S88-Modulnummer des Moduls

    Public Sub Register_Set_S88ModNr(nr As Integer, AktTyp As Integer, AktNr As Integer)

        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp(AktTyp).Modul(AktNr).Set_S88ModNr(nr)

    End Sub

    ' Hole den S88-Schaltzustand des Moduls
    Public Function Register_Get_S88Value(AktTyp As Integer, AktNr As Integer)

        Register_Get_S88Value = 0

        If (AktTyp > 0) And (AktNr > 0) Then Register_Get_S88Value = MBC.ModulTyp(AktTyp).Modul(AktNr).Get_S88Value()

    End Function

    ' Setze den S88-Schaltzustand des Moduls
    Private Sub Register_Set_S88Value(index As Integer, AktTyp As Integer, AktNr As Integer)

        If (AktTyp > 0) And (AktNr > 0) Then MBC.ModulTyp(AktTyp).Modul(AktNr).Set_S88Value(index)

    End Sub

    '**********************************************************************************************
    '*** Register - Modul - Buttons enablen/disablen                                            ***
    '**********************************************************************************************

    Public Sub Register_Modul_Buttons_Enable(Refresh As Boolean, Verwerfen As Boolean, Update As Boolean, Upgrade As Boolean, Reset As Boolean, Loeschen As Boolean)
        Dim Datum_Modul As Integer
        Dim Datum_Modul_Upgrade As Integer
        Dim Software_Modul As Integer
        Dim Software_Modul_Upgrade As Integer

        btn_ControlRefresh.Enabled = Refresh
        btn_ControlVerwerfen.Enabled = Verwerfen
        btn_ControlUpdate.Enabled = Update
        btn_ControlUpgrade.Enabled = Upgrade
        btn_ControlReset.Enabled = Reset
        btn_ControlLoeschen.Enabled = Loeschen

        If (AktTyp > 0) And (AktNr > 0) Then

            ' Hardwareversion

            Datum_Modul_Upgrade = Val(Mid(MBC.ModulTyp(AktTyp).UpgradeDatum, 1, 2)) * 10000 + Val(Mid(MBC.ModulTyp(AktTyp).UpgradeDatum, 4, 2)) * 100 + Val(Mid(MBC.ModulTyp(AktTyp).UpgradeDatum, 7, 2))
            Datum_Modul = Val(Mid(MBC.ModulTyp(AktTyp).Modul(AktNr).HW, 1, 2)) * 10000 + Val(Mid(MBC.ModulTyp(AktTyp).Modul(AktNr).HW, 4, 2)) * 100 + Val(Mid(MBC.ModulTyp(AktTyp).Modul(AktNr).HW, 7, 2))

            ' Softewareversion

            Software_Modul_Upgrade = Val(Strings.Left(MBC.ModulTyp(AktTyp).UpgradeSoftware, 1)) * 100 + Val(Strings.Right(MBC.ModulTyp(AktTyp).UpgradeSoftware, 2))
            Software_Modul = Val(Strings.Left(MBC.ModulTyp(AktTyp).Modul(AktNr).SW, 1)) * 100 + Val(Strings.Right(MBC.ModulTyp(AktTyp).Modul(AktNr).SW, 2))

            If (Datum_Modul_Upgrade < Datum_Modul) Then
                btn_ControlUpgrade.Enabled = Upgrade
            ElseIf (Software_Modul_Upgrade <= Software_Modul) Then
                btn_ControlUpgrade.Enabled = Upgrade
            Else
                btn_ControlUpgrade.Enabled = False
            End If

        End If

    End Sub

    '**********************************************************************************************
    '*** Register - Adressen - Loc-ID Tabelle darstellen                                        ***
    '**********************************************************************************************
    Private Sub Register_DataGridViewLocID_btnLocID_Click(sender As System.Object, e As System.EventArgs) Handles btn_AdressenLocID.Click

        Register_DataGridViewLocID_btnLocID()

    End Sub

    '**********************************************************************************************
    '*** Register - Adressen - Loc-ID Auswahl als Modul anzeigen                                ***
    '**********************************************************************************************

    Private Sub Register_DataGridViewLocID_CellContentClick(sender As System.Object, e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgv_AdressenLocID.CellContentClick

        Register_DataGridViewLocID_CellContent()

    End Sub

    '**********************************************************************************************
    '*** Register - Adressen - Rueckmelderbasis Tabelle darstellen                              ***
    '**********************************************************************************************

    Private Sub Register_DataGridViewS88_btnS88Melder_Click(sender As System.Object, e As System.EventArgs) Handles btn_AdressenS88Base.Click

        Register_DataGridViewS88_btnS88Melder()

    End Sub

    '**********************************************************************************************
    '*** Register - Adressen - Rueckmelderbasis Auswahl als Modul anzeigen                      ***
    '**********************************************************************************************

    Private Sub Register_DataGridViewS88_CellContentClick(sender As System.Object, e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgv_AdressenS88Base.CellContentClick

        Register_DataGridViewS88_CellContent()

    End Sub


    '**********************************************************************************************
    '*** Register - Allgemein - Registerkarten initialisieren                                   ***
    '**********************************************************************************************

    Public Sub Register_Init()


        ' Registerkarte Module initialisieren

        Register_Modul_Werte_Show(False)
        Register_Modul_Buttons_Enable(False, False, False, False, False, False)

        ' Registerkarte Monitor

        rtb_MonitorReceived.Enabled = False
        lbl_MonitorNachrichtenherkunft.Text = Sprachen(i_MAIN, i_None)

        ' Bilder initialisieren

        ' Aktorenstellungen

        ImROT = My.Resources.img_rot
        ImGRUEN = My.Resources.img_gruen
        ImGRAU = My.Resources.img_grau
        ImGELB = My.Resources.img_gelb

        ' Menueeintraege

        ImGLEIS = My.Resources.img_gleis
        ImMODUL = My.Resources.img_modul
        ImHAKEN = My.Resources.img_haken
        ImPUNKT = My.Resources.img_punkt
        ImKREUZ = My.Resources.img_kreuz
        ImDELETE = My.Resources.img_delete

        MenueImage.Images.Clear()

        MenueImage.Images.Add(ImGLEIS)
        MenueImage.Images.Add(ImMODUL)
        MenueImage.Images.Add(ImHAKEN)
        MenueImage.Images.Add(ImPUNKT)
        MenueImage.Images.Add(ImKREUZ)
        MenueImage.Images.Add(ImDELETE)

        ' Moduldetailbilder mbc-91, mbc-92 und mbc-83

        ImImpuls = My.Resources.img_impuls
        ImWechsel = My.Resources.img_wechsel
        ImStartUp = My.Resources.img_startup

        pic_mbc83_Impuls.Image = ImImpuls
        pic_mbc83_Wechsel.Image = ImWechsel
        pic_mbc83_Startup.Image = ImStartUp

        pic_mbc91_Impuls.Image = ImImpuls
        pic_mbc91_Wechsel.Image = ImWechsel

        pic_mbc92_Impuls.Image = ImImpuls
        pic_mbc92_Wechsel.Image = ImWechsel

        ImX = My.Resources.img_x_black

        pic_Status_ASM01.Image = ImX
        pic_Status_ASM02.Image = ImX
        pic_Status_ASM03.Image = ImX
        pic_Status_ASM04.Image = ImX
        pic_Status_ASM05.Image = ImX
        pic_Status_ASM06.Image = ImX
        pic_Status_ASM07.Image = ImX
        pic_Status_ASM08.Image = ImX

    End Sub

    '**********************************************************************************************
    '*** Register - Modul - TabParameter zeigen/verbergen                                       ***
    '**********************************************************************************************

    Public Sub Register_Modul_TabParameter_Show(show As Boolean)

        ' tab_Control deaktivieren

        tab_Module.Enabled = False

        ' TabPages entfernen

        tab_Module.TabPages.Remove(tab_mbc80)
        tab_Module.TabPages.Remove(tab_mbc81)
        tab_Module.TabPages.Remove(tab_mbc82)
        tab_Module.TabPages.Remove(tab_mbc83)
        tab_Module.TabPages.Remove(tab_mbc84)
        tab_Module.TabPages.Remove(tab_mbc85)
        tab_Module.TabPages.Remove(tab_mbc86)
        tab_Module.TabPages.Remove(tab_mbc87)
        tab_Module.TabPages.Remove(tab_mbc88)
        tab_Module.TabPages.Remove(tab_mbc89)
        tab_Module.TabPages.Remove(tab_mbc90)
        tab_Module.TabPages.Remove(tab_mbc91)
        tab_Module.TabPages.Remove(tab_mbc92)
        tab_Module.TabPages.Remove(tab_mbc93)

        If show Then

            ' tab_Control aktivieren

            tab_Module.Enabled = True

            ' Je nach aktuellem Modul das entsprechende TabPage aktivieren

            If AktTyp = 1 Then tab_Module.TabPages.Add(tab_mbc80)
            If AktTyp = 2 Then tab_Module.TabPages.Add(tab_mbc81)
            If AktTyp = 3 Then tab_Module.TabPages.Add(tab_mbc82)
            If AktTyp = 4 Then tab_Module.TabPages.Add(tab_mbc83)
            If AktTyp = 5 Then tab_Module.TabPages.Add(tab_mbc84)
            If AktTyp = 6 Then tab_Module.TabPages.Add(tab_mbc85)
            If AktTyp = 7 Then tab_Module.TabPages.Add(tab_mbc86)
            If AktTyp = 8 Then tab_Module.TabPages.Add(tab_mbc87)
            If AktTyp = 9 Then tab_Module.TabPages.Add(tab_mbc88)
            If AktTyp = 10 Then tab_Module.TabPages.Add(tab_mbc89)
            If AktTyp = 11 Then tab_Module.TabPages.Add(tab_mbc90)
            If AktTyp = 12 Then tab_Module.TabPages.Add(tab_mbc91)
            If AktTyp = 13 Then tab_Module.TabPages.Add(tab_mbc92)
            If AktTyp = 14 Then tab_Module.TabPages.Add(tab_mbc93)

        End If

    End Sub

    '**********************************************************************************************
    '*** Register - Modul - TabPage-Aktionen - Eingabe freigeben/sperren                        ***
    '*** Enable: TRUE = Eingaben sind moeglich, FALSE = Eingaben sind gesperrt                  ***
    '**********************************************************************************************

    Public Sub Register_Modul_Edit_Enable(Enable As Boolean)

        If Enable Then

            ' Groupbox Parameter aktivieren

            gb_Parameter.Enabled = True

            ' Groupbox Statusbild aktivieren

            gb_Status.Enabled = True

            ' Gebe Schaltpult frei

            If (MBC.ModulTyp(AktTyp).Keyboard) Then gb_Keyboard.Enabled = True

            ' Gebe Eingabemoeglichkeiten frei

            FLAG_Edit_Enable = True

            ' Gebe tab_Control frei - ausser eine CSx ist gerade am Bus (siehe
            ' Register_Modul_CSxLock_Apply()); Stellpult/Buttons bleiben davon unberuehrt und
            ' werden hier unbedingt freigegeben

            Register_Modul_CSxLock_Apply()

            ' Gebe Buttons frei

            Register_Modul_Buttons_Enable(True, False, False, True, True, False)

        Else

            ' Groupbox Parameter deaktivieren

            gb_Parameter.Enabled = False

            ' Groupbox Statusbild deaktivieren

            gb_Status.Enabled = False

            ' Sperre Schaltpult

            gb_Keyboard.Enabled = False

            ' Sperre tab_Control

            tab_Module.Enabled = False

            ' Sperre alle Buttons

            Register_Modul_Buttons_Enable(False, False, False, False, False, False)

            ' Sperre Eingabemoeglichkeiten

            FLAG_Edit_Enable = False

        End If

    End Sub

    '**********************************************************************************************
    '*** Register - Modul - tab_Module gemaess CSx-Anwesenheit UND laufender Uebertragung       ***
    '*** sperren/freigeben, sowie die Fussleisten-Anzeige "CSx: present/absent" (stp_CSx)       ***
    '*** aktualisieren. Wird bei jedem CSx-Ping (COMMsgReceived), bei jedem Timeout-Ablauf      ***
    '*** (Timer_Alive_Tick) sowie beim Freigeben nach einer Uebertragung                        ***
    '*** (Register_Modul_Edit_Enable) aufgerufen - Stellpult/Buttons sind bewusst NICHT         ***
    '*** betroffen, die folgen ausschliesslich FLAG_Edit_Enable. Ausnahme mbc84 (AktTyp=5):     ***
    '*** tab_Module bleibt von der CSx-Sperre ausgenommen, damit die Wegdiagramm-Buttons        ***
    '*** erreichbar bleiben - stattdessen werden dort die Eingabefelder einzeln gesperrt.       ***
    '*** Ausnahme mbc89 (AktTyp=10): tab_Module bleibt ebenfalls ausgenommen, damit der         ***
    '*** Lokeigenschaften-Button (btn_mbc89_Loks) bei CSx-Anwesenheit erreichbar bleibt -        ***
    '*** stattdessen werden nur die 5 Konfig-Gruppenboxen gesperrt.                             ***
    '**********************************************************************************************

    Public Sub Register_Modul_CSxLock_Apply()

        If AktTyp = 5 Then

            ' mbc84: tab_Module bleibt von der CSx-Sperre unberuehrt, damit die Wegdiagramm-Buttons
            ' erreichbar bleiben - stattdessen werden nur die Eingabefelder einzeln gesperrt

            Dim locked As Boolean = FLAG_Edit_Enable AndAlso Not FLAG_CSxPingEmpfangen

            tab_Module.Enabled = FLAG_Edit_Enable

            nud_mbc84_MMAddr.Enabled = locked
            nud_mbc84_Rueck.Enabled = locked
            nud_mbc84_Links_CH1.Enabled = locked
            nud_mbc84_Rechts_CH1.Enabled = locked
            nud_mbc84_Step_CH1.Enabled = locked
            nud_mbc84_Links_CH2.Enabled = locked
            nud_mbc84_Rechts_CH2.Enabled = locked
            nud_mbc84_Step_CH2.Enabled = locked
            nud_mbc84_Links_CH3.Enabled = locked
            nud_mbc84_Rechts_CH3.Enabled = locked
            nud_mbc84_Step_CH3.Enabled = locked
            nud_mbc84_Links_CH4.Enabled = locked
            nud_mbc84_Rechts_CH4.Enabled = locked
            nud_mbc84_Step_CH4.Enabled = locked

        ElseIf AktTyp = 10 Then

            ' mbc89: tab_Module bleibt von der CSx-Sperre unberuehrt, damit der
            ' Lokeigenschaften-Button (btn_mbc89_Loks, liegt direkt auf dem Tab, nicht in einer
            ' der Gruppenboxen) erreichbar bleibt - stattdessen werden nur die 5 Konfig-
            ' Gruppenboxen gesperrt (deaktiviert automatisch alle enthaltenen Steuerelemente)

            Dim locked As Boolean = FLAG_Edit_Enable AndAlso Not FLAG_CSxPingEmpfangen

            tab_Module.Enabled = FLAG_Edit_Enable

            gb_mbc89_Keyboard.Enabled = locked
            gb_mbc89_Basisadresse.Enabled = locked
            gb_mbc89_Version.Enabled = locked
            gb_mbc89_Mapping.Enabled = locked
            gb_mbc89_I2C.Enabled = locked

        Else

            tab_Module.Enabled = FLAG_Edit_Enable AndAlso Not FLAG_CSxPingEmpfangen

        End If

        stp_CSx.Text = If(FLAG_CSxPingEmpfangen, "CSx: present", "CSx: absent")

    End Sub

    '**********************************************************************************************
    '*** Register - Modul - Daten anzeigen entsprechend des ausgewaehlten Moduls                ***
    '*** Enable: TRUE = Werte werden angezeigt, FALSE = EIn leeres Modul wird angezeigt         ***
    '**********************************************************************************************

    Public Sub Register_Modul_Werte_Show(Enable As Boolean)

        If Enable Then

            ' Wenn der Status "Complete" ist dann zeige die Moduldaten entsprechend an

            If (MBC.ModulTyp(AktTyp).Modul(AktNr).Status = IsComplete) Then

                Register_Modul_Edit_Enable(True)

                ' Parameterregister aktivieren und Inhalte der TabPages anzeigen

                Register_Modul_TabParameter_Show(True)

                ' Sperre die ValueChanged-Eigenschaft und trage Werte ein

                FLAG_Edit_Enable = False

                ' Fuelle die Konfigurationsfelder zum Modul aus

                MBC.ModulTyp(AktTyp).Modul(AktNr).Tabpage_Aktivieren()
                MBC.ModulTyp(AktTyp).Modul(AktNr).Tabpage_Werte_Show()


                ' Gebe die ValueChanged-Eigenschaft wieder frei

                FLAG_Edit_Enable = True

            Else

                ' Alle Eingaben sperren

                Register_Modul_Edit_Enable(False)

                ' Parameterregister deaktiviert

                Register_Modul_TabParameter_Show(False)

            End If

            ' Zeige Infos zum ausgewaehlten Modul an
            ' Falls das Modul nicht aktiviert ist dann setze alle Werte auf Standard

            txt_ParameterName.Text = MBC.ModulTyp(AktTyp).Modul(AktNr).Name
            txt_ParameterDecTyp.Text = MBC.ModulTyp(AktTyp).Bezeichner
            txt_ParameterArtNum.Text = MBC.ModulTyp(AktTyp).Modul(AktNr).ArtNum
            txt_ParameterSerNr.Text = MBC.ModulTyp(AktTyp).Modul(AktNr).SerNr
            txt_ParameterGUID.Text = MBC.ModulTyp(AktTyp).Modul(AktNr).GUiD
            txt_ParameterSW.Text = MBC.ModulTyp(AktTyp).Modul(AktNr).SW
            txt_ParameterHW.Text = MBC.ModulTyp(AktTyp).Modul(AktNr).HW
            txt_ParameterPlugged.Text = MBC.ModulTyp(AktTyp).Modul(AktNr).Plugged
            txt_ParameterSpannung.Text = MBC.ModulTyp(AktTyp).Modul(AktNr).Spannung
            txt_ParameterTemperatur.Text = MBC.ModulTyp(AktTyp).Modul(AktNr).Temperatur

        Else

            ' Anzeige im Statusblock zuruecksetzen

            txt_ParameterName.Text = "-/-"
            txt_ParameterDecTyp.Text = "-/-"
            txt_ParameterArtNum.Text = "-/-"
            txt_ParameterGUID.Text = "-/-"
            txt_ParameterSW.Text = "-/-"
            txt_ParameterHW.Text = "-/-"
            txt_ParameterSerNr.Text = "-/-"
            txt_ParameterPlugged.Text = "-/-"
            txt_ParameterSpannung.Text = "0.00"
            txt_ParameterTemperatur.Text = "0.00"

            ' Alle Eingaben sperren

            Register_Modul_Edit_Enable(False)

            ' LED-Anzeige deaktivieren

            Register_Modul_Aktorstellung_Show()

            ' Parameterregister deaktiviert

            Register_Modul_TabParameter_Show(False)

        End If

    End Sub


    '**********************************************************************************************
    '*** Register - Modul - Stellungsbilder MM und s88 anzeigen                                 ***
    '**********************************************************************************************

    Public Sub Register_Modul_Aktorstellung_Show()

        If (AktTyp > 0) And (AktNr > 0) Then

            If MBC.ModulTyp(AktTyp).Modul(AktNr).Status = IsComplete Then MBC.ModulTyp(AktTyp).Modul(AktNr).Aktorstellung_Show()

        End If

    End Sub

    '**********************************************************************************************
    '*** Register - Modul - Modulbild anzeigen entsprechend des ausgewaehlten Moduls            ***
    '*** ModTyp: Modultyp                                                                       ***
    '*** LED-PIC01 = ImageOn(1)                                                                 ***
    '*** LED-PIC02 = ImageOn(9)                                                                 ***
    '*** LED-PIC03 = ImageOn(2)                                                                 ***
    '*** LED-PIC04 = ImageOn(10)                                                                ***
    '*** LED-PIC05 = ImageOn(11)                                                                ***
    '*** LED-PIC06 = ImageOn(3)                                                                 ***
    '*** LED-PIC07 = ImageOn(12)                                                                ***
    '*** LED-PIC08 = ImageOn(4)                                                                 ***
    '*** LED-PIC09 = ImageOn(17)                                                                ***
    '*** LED-PIC10 = ImageOn(18)                                                                ***
    '*** LED-PIC11 = ImageOn(19)                                                                ***
    '*** LED-PIC12 = ImageOn(5)                                                                 ***
    '*** LED-PIC13 = ImageOn(13)                                                                ***
    '*** LED-PIC14 = ImageOn(6)                                                                 ***
    '*** LED-PIC15 = ImageOn(14)                                                                ***
    '*** LED-PIC16 = ImageOn(15)                                                                ***
    '*** LED-PIC17 = ImageOn(7)                                                                 ***
    '*** LED-PIC18 = ImageOn(16)                                                                ***
    '*** LED-PIC19 = ImageOn(8)                                                                 ***
    '**********************************************************************************************

    Public Sub Register_Modul_Bilder_Show(ModTyp As Integer)

        If (ModTyp <> 0) And (ModTyp <= MaxTyp) Then

            ' Decoderbild anzeigen

            pic_Status_Decoder.Visible = True
            pic_Status_Decoder.Image = MBC.ModulTyp(ModTyp).Image

            ' LED anzeigen fuer Rueckmeldung

            If MBC.ModulTyp(ModTyp).RMImageOn(1) = True Then
                pic_Status_LED01.Visible = True
                pic_Status_LED01.Image = MBC.ModulTyp(ModTyp).RMImage(1)
            Else
                pic_Status_LED01.Visible = False
            End If

            If MBC.ModulTyp(ModTyp).RMImageOn(2) = True Then
                pic_Status_LED03.Visible = True
                pic_Status_LED03.Image = MBC.ModulTyp(ModTyp).RMImage(2)
            Else
                pic_Status_LED03.Visible = False
            End If

            If MBC.ModulTyp(ModTyp).RMImageOn(3) = True Then
                pic_Status_LED06.Visible = True
                pic_Status_LED06.Image = MBC.ModulTyp(ModTyp).RMImage(3)
            Else
                pic_Status_LED06.Visible = False
            End If

            If MBC.ModulTyp(ModTyp).RMImageOn(4) = True Then
                pic_Status_LED08.Visible = True
                pic_Status_LED08.Image = MBC.ModulTyp(ModTyp).RMImage(4)
            Else
                pic_Status_LED08.Visible = False
            End If

            If MBC.ModulTyp(ModTyp).RMImageOn(5) = True Then
                pic_Status_LED12.Visible = True
                pic_Status_LED12.Image = MBC.ModulTyp(ModTyp).RMImage(5)
            Else
                pic_Status_LED12.Visible = False
            End If

            If MBC.ModulTyp(ModTyp).RMImageOn(6) = True Then
                pic_Status_LED14.Visible = True
                pic_Status_LED14.Image = MBC.ModulTyp(ModTyp).RMImage(6)
            Else
                pic_Status_LED14.Visible = False
            End If

            If MBC.ModulTyp(ModTyp).RMImageOn(7) = True Then
                pic_Status_LED17.Visible = True
                pic_Status_LED17.Image = MBC.ModulTyp(ModTyp).RMImage(7)
            Else
                pic_Status_LED17.Visible = False
            End If

            If MBC.ModulTyp(ModTyp).RMImageOn(8) = True Then
                pic_Status_LED19.Visible = True
                pic_Status_LED19.Image = MBC.ModulTyp(ModTyp).RMImage(8)
            Else
                pic_Status_LED19.Visible = False
            End If

            If MBC.ModulTyp(ModTyp).RMImageOn(9) = True Then
                pic_Status_LED02.Visible = True
                pic_Status_LED02.Image = MBC.ModulTyp(ModTyp).RMImage(9)
            Else
                pic_Status_LED02.Visible = False
            End If

            If MBC.ModulTyp(ModTyp).RMImageOn(10) = True Then
                pic_Status_LED04.Visible = True
                pic_Status_LED04.Image = MBC.ModulTyp(ModTyp).RMImage(10)
            Else
                pic_Status_LED04.Visible = False
            End If

            If MBC.ModulTyp(ModTyp).RMImageOn(11) = True Then
                pic_Status_LED05.Visible = True
                pic_Status_LED05.Image = MBC.ModulTyp(ModTyp).RMImage(11)
            Else
                pic_Status_LED05.Visible = False
            End If

            If MBC.ModulTyp(ModTyp).RMImageOn(12) = True Then
                pic_Status_LED07.Visible = True
                pic_Status_LED07.Image = MBC.ModulTyp(ModTyp).RMImage(12)
            Else
                pic_Status_LED07.Visible = False
            End If

            If MBC.ModulTyp(ModTyp).RMImageOn(13) = True Then
                pic_Status_LED13.Visible = True
                pic_Status_LED13.Image = MBC.ModulTyp(ModTyp).RMImage(13)
            Else
                pic_Status_LED13.Visible = False
            End If

            If MBC.ModulTyp(ModTyp).RMImageOn(14) = True Then
                pic_Status_LED15.Visible = True
                pic_Status_LED15.Image = MBC.ModulTyp(ModTyp).RMImage(14)
            Else
                pic_Status_LED15.Visible = False
            End If

            If MBC.ModulTyp(ModTyp).RMImageOn(15) = True Then
                pic_Status_LED16.Visible = True
                pic_Status_LED16.Image = MBC.ModulTyp(ModTyp).RMImage(15)
            Else
                pic_Status_LED16.Visible = False
            End If

            If MBC.ModulTyp(ModTyp).RMImageOn(16) = True Then
                pic_Status_LED18.Visible = True
                pic_Status_LED18.Image = MBC.ModulTyp(ModTyp).RMImage(16)
            Else
                pic_Status_LED18.Visible = False
            End If

            If MBC.ModulTyp(ModTyp).RMImageOn(17) = True Then
                pic_Status_LED09.Visible = True
                pic_Status_LED09.Image = MBC.ModulTyp(ModTyp).RMImage(17)
            Else
                pic_Status_LED09.Visible = False
            End If

            If MBC.ModulTyp(ModTyp).RMImageOn(18) = True Then
                pic_Status_LED10.Visible = True
                pic_Status_LED10.Image = MBC.ModulTyp(ModTyp).RMImage(18)
            Else
                pic_Status_LED10.Visible = False
            End If

            If MBC.ModulTyp(ModTyp).RMImageOn(19) = True Then
                pic_Status_LED11.Visible = True
                pic_Status_LED11.Image = MBC.ModulTyp(ModTyp).RMImage(19)
            Else
                pic_Status_LED11.Visible = False
            End If

            ' Feld anzeigen fuer Bestueckung mbc-83

            If MBC.ModulTyp(ModTyp).TImageOn(1) = True Then
                pic_Status_ASM01.Visible = True
                pic_Status_ASM01.Image = MBC.ModulTyp(ModTyp).TImage(1)
            Else
                pic_Status_ASM01.Visible = False
            End If

            If MBC.ModulTyp(ModTyp).TImageOn(2) = True Then
                pic_Status_ASM02.Visible = True
                pic_Status_ASM02.Image = MBC.ModulTyp(ModTyp).TImage(2)
            Else
                pic_Status_ASM02.Visible = False
            End If

            If MBC.ModulTyp(ModTyp).TImageOn(3) = True Then
                pic_Status_ASM03.Visible = True
                pic_Status_ASM03.Image = MBC.ModulTyp(ModTyp).TImage(3)
            Else
                pic_Status_ASM03.Visible = False
            End If

            If MBC.ModulTyp(ModTyp).TImageOn(4) = True Then
                pic_Status_ASM04.Visible = True
                pic_Status_ASM04.Image = MBC.ModulTyp(ModTyp).TImage(4)
            Else
                pic_Status_ASM04.Visible = False
            End If

            If MBC.ModulTyp(ModTyp).TImageOn(5) = True Then
                pic_Status_ASM05.Visible = True
                pic_Status_ASM05.Image = MBC.ModulTyp(ModTyp).TImage(5)
            Else
                pic_Status_ASM05.Visible = False
            End If

            If MBC.ModulTyp(ModTyp).TImageOn(6) = True Then
                pic_Status_ASM06.Visible = True
                pic_Status_ASM06.Image = MBC.ModulTyp(ModTyp).TImage(6)
            Else
                pic_Status_ASM06.Visible = False
            End If

            If MBC.ModulTyp(ModTyp).TImageOn(7) = True Then
                pic_Status_ASM07.Visible = True
                pic_Status_ASM07.Image = MBC.ModulTyp(ModTyp).TImage(7)
            Else
                pic_Status_ASM07.Visible = False
            End If

            If MBC.ModulTyp(ModTyp).TImageOn(8) = True Then
                pic_Status_ASM08.Visible = True
                pic_Status_ASM08.Image = MBC.ModulTyp(ModTyp).TImage(8)
            Else
                pic_Status_ASM08.Visible = False
            End If

        Else

            pic_Status_Decoder.Visible = False
            pic_Status_LED01.Visible = False
            pic_Status_LED02.Visible = False
            pic_Status_LED03.Visible = False
            pic_Status_LED04.Visible = False
            pic_Status_LED05.Visible = False
            pic_Status_LED06.Visible = False
            pic_Status_LED07.Visible = False
            pic_Status_LED08.Visible = False
            pic_Status_LED09.Visible = False
            pic_Status_LED10.Visible = False
            pic_Status_LED11.Visible = False
            pic_Status_LED12.Visible = False
            pic_Status_LED13.Visible = False
            pic_Status_LED14.Visible = False
            pic_Status_LED15.Visible = False
            pic_Status_LED16.Visible = False
            pic_Status_LED17.Visible = False
            pic_Status_LED18.Visible = False
            pic_Status_LED19.Visible = False
            pic_Status_ASM01.Visible = False
            pic_Status_ASM02.Visible = False
            pic_Status_ASM03.Visible = False
            pic_Status_ASM04.Visible = False
            pic_Status_ASM05.Visible = False
            pic_Status_ASM06.Visible = False
            pic_Status_ASM07.Visible = False
            pic_Status_ASM08.Visible = False

        End If

    End Sub

    '**********************************************************************************************
    '*** Dashboard - Rueckmelderbasis Auswahl als Modul anzeigen                                ***
    '**********************************************************************************************

    Public Sub Register_DataGridViewS88_CellContent()
        Dim Name As String
        Dim Typ As String
        Dim Index As Integer
        Dim Row As Integer
        Dim FullPath As String

        Dim selectedCellCount As Integer = dgv_AdressenS88Base.GetCellCount(DataGridViewElementStates.Selected)

        If selectedCellCount > 0 Then

            ' Bestimme die ausgewaehlte Zeile

            Row = dgv_AdressenS88Base.SelectedCells(0).RowIndex

            ' Typ, Namen und Index des ausgewaehlten Moduls bestimmen

            Name = dgv_AdressenS88Base.Rows(Row).Cells(2).Value
            Typ = dgv_AdressenS88Base.Rows(Row).Cells(1).Value
            Index = dgv_AdressenS88Base.Rows(Row).Cells(3).Value

            ' Fullpath zusammensetzen

            FullPath = "MBCAN\" & Typ & "\" & Name

            ' Node im Treeview suchen
            ' Schleife notwendig, da Name bei Fremdeingabe auch doppelt vorkommen kann

            Do

                tv_Strukturbaum.SelectedNode = FindNode(tv_Strukturbaum.Nodes, FullPath)

                ' Node auswaehlen um aktives Modul festzulegen

                tv_Strukturbaum.Select()

            Loop Until (MBC.ModulTyp(AktTyp).Modul(AktNr).Name = Name) And (AktNr = Index)

            ' Registerkarte aktivieren

            tab_Control.SelectedTab = tab_Control.TabPages(0)

            tab_Control.TabPages(0).Show()

        End If

    End Sub

    '**********************************************************************************************
    '*** Register - Rueckmelderbasis Tabelle darstellen                                         ***
    '**********************************************************************************************

    Public Sub Register_DataGridViewS88_btnS88Melder()
        Dim ModNr As Integer

        Dim HalbMod As String

        ' DataGridView loeschen

        dgv_AdressenS88Base.SelectAll()
        While dgv_AdressenS88Base.SelectedRows.Count > 0
            dgv_AdressenS88Base.Rows.RemoveAt(dgv_AdressenS88Base.SelectedRows(0).Index)
        End While

        ' DataGridView mit Aktorendaten zur Loc-ID fuellen

        For ModNr = 1 To MBC.ModulTyp("mbc-88").Count
            If MBC.ModulTyp("mbc-88").Modul(ModNr).S88ModHalbNr = 0 Then
                HalbMod = ": 1-8"
            Else
                HalbMod = ": 9-16"
            End If
            dgv_AdressenS88Base.Rows.Add({Val(MBC.ModulTyp("mbc-88").Modul(ModNr).S88ModNr) & HalbMod, MBC.ModulTyp("mbc-88").Bezeichner, MBC.ModulTyp("mbc-88").Modul(ModNr).Name, ModNr})
        Next

        For ModNr = 1 To MBC.ModulTyp("mbc-90").Count
            dgv_AdressenS88Base.Rows.Add({Val(MBC.ModulTyp("mbc-90").Modul(ModNr).S88ModNr) & ": 1-16", MBC.ModulTyp("mbc-90").Bezeichner, MBC.ModulTyp("mbc-90").Modul(ModNr).Name, ModNr})
        Next

        For ModNr = 1 To MBC.ModulTyp("mbc-85").Count
            dgv_AdressenS88Base.Rows.Add({Val(MBC.ModulTyp("mbc-85").Modul(ModNr).S88ModNr) & ": 1-12", MBC.ModulTyp("mbc-85").Bezeichner, MBC.ModulTyp("mbc-85").Modul(ModNr).Name, ModNr})
        Next

        For ModNr = 1 To MBC.ModulTyp("mbc-86").Count
            dgv_AdressenS88Base.Rows.Add({Val(MBC.ModulTyp("mbc-86").Modul(ModNr).S88ModNr) & ": 1-8", MBC.ModulTyp("mbc-86").Bezeichner, MBC.ModulTyp("mbc-86").Modul(ModNr).Name, ModNr})
        Next

        dgv_AdressenS88Base.Sort(dgv_AdressenS88Base.Columns(0), ListSortDirection.Ascending)

    End Sub

    '**********************************************************************************************
    '*** Register - Loc-ID Auswahl als Modul anzeigen                                           ***
    '**********************************************************************************************

    Public Sub Register_DataGridViewLocID_CellContent()
        Dim Name As String
        Dim Typ As String
        Dim Index As Integer
        Dim Row As Integer
        Dim FullPath As String

        Dim selectedCellCount As Integer = dgv_AdressenLocID.GetCellCount(DataGridViewElementStates.Selected)

        If selectedCellCount > 0 Then

            COMRefreshStop()
            COMUpdateStop()

            ' Bestimme die ausgewaehlte Zeile

            Row = dgv_AdressenLocID.SelectedCells(0).RowIndex

            ' Typ, Namen und Index des ausgewaehlten Moduls bestimmen

            Name = dgv_AdressenLocID.Rows(Row).Cells(2).Value
            Typ = dgv_AdressenLocID.Rows(Row).Cells(1).Value
            Index = dgv_AdressenLocID.Rows(Row).Cells(3).Value

            ' Fullpath zusammensetzen

            FullPath = "MBCAN\" & Typ & "\" & Name

            ' Node im Treeview suchen
            ' Schleife notwendig, da Name bei Fremdeingabe auch doppelt vorkommen kann

            Do

                tv_Strukturbaum.SelectedNode = FindNode(tv_Strukturbaum.Nodes, FullPath)

                ' Node auswaehlen um aktives Modul festzulegen

                tv_Strukturbaum.Select()

            Loop Until AktNr = Index

            ' Registerkarte aktivieren

            tab_Control.SelectedTab = tab_Control.TabPages(0)

            tab_Control.TabPages(0).Show()

        End If

    End Sub

    '**********************************************************************************************
    '*** Register - Loc-ID Tabelle darstellen                                                   ***
    '**********************************************************************************************

    Public Sub Register_DataGridViewLocID_btnLocID()

        ' DataGridView loeschen

        dgv_AdressenLocID.SelectAll()
        While dgv_AdressenLocID.SelectedRows.Count > 0
            dgv_AdressenLocID.Rows.RemoveAt(dgv_AdressenLocID.SelectedRows(0).Index)
        End While

        ' DataGridView mit Aktorendaten zur Loc-ID fuellen

        ' Modul mbc-83

        Register_DataGridViewLocID_btnLocID_Add(4)

        ' Modul mbc-84

        Register_DataGridViewLocID_btnLocID_Add(5)

        ' Modul mbc-91

        Register_DataGridViewLocID_btnLocID_Add(12)

        ' Modul mbc-92

        Register_DataGridViewLocID_btnLocID_Add(13)

        dgv_AdressenLocID.Sort(dgv_AdressenLocID.Columns(0), ListSortDirection.Ascending)

    End Sub

    Private Sub Register_DataGridViewLocID_btnLocID_Add(ModTyp As Integer)
        Dim ModNr As Integer

        For ModNr = 1 To MBC.ModulTyp(ModTyp).Count
            If MBC.ModulTyp(ModTyp).Modul(ModNr).Status = IsComplete Then
                dgv_AdressenLocID.Rows.Add({Register_Get_MMAddr(0, ModTyp, ModNr), MBC.ModulTyp(ModTyp).Bezeichner, MBC.ModulTyp(ModTyp).Modul(ModNr).Name, ModNr})
            End If
        Next
    End Sub

    '++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    '+++ Strukturbaum                                                                           +++
    '+++ Allgemeine Funktionen zum Umgang mit dem Strukturbaum                                  +++
    '++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    '**********************************************************************************************
    '*** Strukturbaum - Rechte Maustaste zur Namensaenderung des Moduls                         ***
    '**********************************************************************************************

    Private Sub Strukturbaum_NodeMouseClick(sender As System.Object, e As System.Windows.Forms.TreeNodeMouseClickEventArgs) Handles tv_Strukturbaum.NodeMouseClick

        Dim IndexM As Integer
        Dim IndexT As Integer
        Dim Index As Integer
        Dim mySelectedNode As TreeNode

        ' Fokusiere auf den asugewaehlten Knoten

        mySelectedNode = tv_Strukturbaum.GetNodeAt(e.X, e.Y)

        ' Sollte der Editiermodus zugelassen sein dann sperre ihn wieder

        If tv_Strukturbaum.LabelEdit Then tv_Strukturbaum.LabelEdit = False

        ' Wenn rechte Maustaste gedrueckt dann .... 

        If e.Button = Windows.Forms.MouseButtons.Right Then

            ' ... starte Namensaenderung, aber nur, wenn die Auswahl nicht den ROOT-Knoten "MBCAN" (Level 0)
            ' oder einen Typ-Knoten "mbc-XX" (Level 1) betrifft und das Modul schon komplett ausgelesen
            ' wurde. Modul-Blattknoten liegen immer auf Level 2 (siehe Strukturbaum_MenueAddItem:
            ' Nodes(0).Nodes(TypIndex).Nodes(ModulIndex)). FRUEHER wurde stattdessen der Knotentext
            ' selbst geprueft (Left(Text,3)<>"mbc") - das brach dauerhaft, sobald ein Modul auf einen mit
            ' "mbc" beginnenden Namen umbenannt wurde (im "mbc"-Produktumfeld ein naheliegender Name),
            ' da der Text-Check danach fuer GENAU dieses Modul permanent fehlschlug und BeginEdit() nie
            ' wieder erreicht wurde - reproduzierbares "einmal geht's, danach nie wieder"-Muster.

            If Not (mySelectedNode Is Nothing) And Not (mySelectedNode.Parent Is Nothing) And (mySelectedNode.Level = 2) Then

                tv_Strukturbaum.SelectedNode = mySelectedNode

                IndexM = mySelectedNode.Index + 1
                IndexT = Strukturbaum_GetTypIndex(mySelectedNode.Parent.Text)

                If (Not mySelectedNode.IsEditing) And (MBC.ModulTyp(IndexT).Modul(IndexM).Status = IsComplete) Then
                    tv_Strukturbaum.LabelEdit = True
                    mySelectedNode.BeginEdit()
                End If

            End If

            ' ... sende Neuanmeldungsnachricht an die Module des ausgewaehlte Knoten ein "mbc"-Knoten 

            If Not (mySelectedNode Is Nothing) And
               Not (mySelectedNode.Parent Is Nothing) And
               (Len(mySelectedNode.Text) = 6) And
               (Microsoft.VisualBasic.Left(mySelectedNode.Text, 3) = "mbc") And
               (BTThreadIsActive Or RS232ThreadIsActive) Then

                If MsgBox(Sprachen(i_MSGBOX_MAIN_Neu, i_MSGBOX_MAIN_Neu_1) & " '" & mySelectedNode.Text & "' " & Sprachen(i_MSGBOX_MAIN_Neu, i_MSGBOX_MAIN_Neu_2) & Chr(13) & Chr(13) &
                        Sprachen(i_MSGBOX_MAIN_Neu, i_MSGBOX_MAIN_Neu_3) & Chr(13) & Chr(13) &
                        Sprachen(i_MSGBOX_MAIN_Neu, i_MSGBOX_MAIN_Neu_4) & " '" & mySelectedNode.Text & "' " & Sprachen(i_MSGBOX_MAIN_Neu, i_MSGBOX_MAIN_Neu_5) & Chr(13) & Chr(13) &
                        Sprachen(i_MSGBOX_MAIN_Neu, i_MSGBOX_MAIN_Neu_6), vbYesNo, Sprachen(i_MSGBOX_MAIN_Neu, i_MSGBOX_MAIN_Neu_7)) = vbYes Then

                    mySelectedNode.Nodes.Clear()

                    IndexT = Strukturbaum_GetTypIndex(mySelectedNode.Text)
                    IndexM = MBC.ModulTyp(IndexT).Count

                    ' Sende AbmeldeMsg an die betroffenen Module

                    For Index = 1 To IndexM
                        COMMsgSend(MsgMDDel & MBC.ModulTyp(IndexT).Modul(Index).MsgGUiD)
                    Next

                    MBC.ModulTyp(IndexT).Clear()

                End If

            End If

        End If

    End Sub

    Private Sub Strukturbaum_AfterLabelEdit(sender As Object, e As System.Windows.Forms.NodeLabelEditEventArgs) Handles tv_Strukturbaum.AfterLabelEdit
        Dim IndexM As Integer
        Dim IndexT As Integer

        If Not (e.Label Is Nothing) Then
            If e.Label.Length > 0 Then
                If e.Label.IndexOfAny(New Char() {"@"c, "."c, ","c, "!"c}) = -1 Then

                    If e.Label.Length > 18 Then

                        ' Starte Namensvergabe neu, da mehr als 18 Zeichen verwendet wurden

                        e.CancelEdit = True
                        MessageBox.Show(Sprachen(i_MSGBOX_MAIN_NAME, i_MSGBOX_MAIN_NAME_2) &
                          Microsoft.VisualBasic.ControlChars.Cr &
                          Sprachen(i_MSGBOX_MAIN_NAME, i_MSGBOX_MAIN_NAME_3),
                          Sprachen(i_MSGBOX_MAIN_NAME, i_MSGBOX_MAIN_NAME_1))
                        e.Node.BeginEdit()

                    Else

                        ' Beende Namensvergabe und sende den Namen an das Modul

                        IndexM = e.Node.Index + 1
                        IndexT = Strukturbaum_GetTypIndex(e.Node.Parent.Text)

                        ' Refreshtimer stoppen

                        COMRefreshStop()

                        For Index = 0 To Len(e.Label) - 1
                            MBC.ModulTyp(AktTyp).Modul(AktNr).SYS_ARRAY(MBC_S_NAMEBLOCK + Index) = FormHEX("0", 2, Asc(Microsoft.VisualBasic.Mid(e.Label, Index + 1, 1)))
                        Next

                        For Index = Len(e.Label) To MBC_L_NAMEBLOCK - 1
                            MBC.ModulTyp(AktTyp).Modul(AktNr).SYS_ARRAY(MBC_S_NAMEBLOCK + Index) = "81"
                        Next

                        ' Name geaendert dann speichere diese im Modul

                        COMUpdateStart(MBC_S_NAMEBLOCK, MBC_S_NAMEBLOCK + MBC_L_NAMEBLOCK - 1, EE_NAME)

                        tv_Strukturbaum.LabelEdit = False

                    End If

                Else

                    ' Starte Namensvergabe neu, da unerlaubte Zeichen verwendet wurden

                    e.CancelEdit = True
                    MessageBox.Show(Sprachen(i_MSGBOX_MAIN_NAME, i_MSGBOX_MAIN_NAME_2) &
                      Microsoft.VisualBasic.ControlChars.Cr &
                      Sprachen(i_MSGBOX_MAIN_NAME, i_MSGBOX_MAIN_NAME_4),
                      Sprachen(i_MSGBOX_MAIN_NAME, i_MSGBOX_MAIN_NAME_1))
                    e.Node.BeginEdit()

                End If
            Else

                ' Starte Namensvergabe neu, da der Name leer ist

                e.CancelEdit = True
                MessageBox.Show(Sprachen(i_MSGBOX_MAIN_NAME, i_MSGBOX_MAIN_NAME_2) &
                  Microsoft.VisualBasic.ControlChars.Cr &
                  Sprachen(i_MSGBOX_MAIN_NAME, i_MSGBOX_MAIN_NAME_5),
                  Sprachen(i_MSGBOX_MAIN_NAME, i_MSGBOX_MAIN_NAME_1))
                e.Node.BeginEdit()

            End If
        End If
    End Sub

    '**********************************************************************************************
    '*** Strukturbaum - Strukturbaum initialisieren                                             ***
    '**********************************************************************************************

    Public Sub Strukturbaum_Init()

        tv_Strukturbaum.ImageList = MenueImage

        tv_Strukturbaum.Nodes.Add("MBCAN")
        tv_Strukturbaum.Nodes(0).ImageIndex = 0
        tv_Strukturbaum.Nodes(0).SelectedImageIndex = 0

        tv_Strukturbaum.Nodes(0).Nodes.Add("mbc-80")
        tv_Strukturbaum.Nodes(0).Nodes(0).ImageIndex = 1
        tv_Strukturbaum.Nodes(0).Nodes(0).SelectedImageIndex = 1

        tv_Strukturbaum.Nodes(0).Nodes.Add("mbc-81")
        tv_Strukturbaum.Nodes(0).Nodes(1).ImageIndex = 1
        tv_Strukturbaum.Nodes(0).Nodes(1).SelectedImageIndex = 1

        tv_Strukturbaum.Nodes(0).Nodes.Add("mbc-82")
        tv_Strukturbaum.Nodes(0).Nodes(2).ImageIndex = 1
        tv_Strukturbaum.Nodes(0).Nodes(2).SelectedImageIndex = 1

        tv_Strukturbaum.Nodes(0).Nodes.Add("mbc-83")
        tv_Strukturbaum.Nodes(0).Nodes(3).ImageIndex = 1
        tv_Strukturbaum.Nodes(0).Nodes(3).SelectedImageIndex = 1

        tv_Strukturbaum.Nodes(0).Nodes.Add("mbc-84")
        tv_Strukturbaum.Nodes(0).Nodes(4).ImageIndex = 1
        tv_Strukturbaum.Nodes(0).Nodes(4).SelectedImageIndex = 1

        tv_Strukturbaum.Nodes(0).Nodes.Add("mbc-85")
        tv_Strukturbaum.Nodes(0).Nodes(5).ImageIndex = 1
        tv_Strukturbaum.Nodes(0).Nodes(5).SelectedImageIndex = 1

        tv_Strukturbaum.Nodes(0).Nodes.Add("mbc-86")
        tv_Strukturbaum.Nodes(0).Nodes(6).ImageIndex = 1
        tv_Strukturbaum.Nodes(0).Nodes(6).SelectedImageIndex = 1

        tv_Strukturbaum.Nodes(0).Nodes.Add("mbc-87")
        tv_Strukturbaum.Nodes(0).Nodes(7).ImageIndex = 1
        tv_Strukturbaum.Nodes(0).Nodes(7).SelectedImageIndex = 1

        tv_Strukturbaum.Nodes(0).Nodes.Add("mbc-88")
        tv_Strukturbaum.Nodes(0).Nodes(8).ImageIndex = 1
        tv_Strukturbaum.Nodes(0).Nodes(8).SelectedImageIndex = 1

        tv_Strukturbaum.Nodes(0).Nodes.Add("mbc-89")
        tv_Strukturbaum.Nodes(0).Nodes(9).ImageIndex = 1
        tv_Strukturbaum.Nodes(0).Nodes(9).SelectedImageIndex = 1

        tv_Strukturbaum.Nodes(0).Nodes.Add("mbc-90")
        tv_Strukturbaum.Nodes(0).Nodes(10).ImageIndex = 1
        tv_Strukturbaum.Nodes(0).Nodes(10).SelectedImageIndex = 1

        tv_Strukturbaum.Nodes(0).Nodes.Add("mbc-91")
        tv_Strukturbaum.Nodes(0).Nodes(11).ImageIndex = 1
        tv_Strukturbaum.Nodes(0).Nodes(11).SelectedImageIndex = 1

        tv_Strukturbaum.Nodes(0).Nodes.Add("mbc-92")
        tv_Strukturbaum.Nodes(0).Nodes(12).ImageIndex = 1
        tv_Strukturbaum.Nodes(0).Nodes(12).SelectedImageIndex = 1

        tv_Strukturbaum.Nodes(0).Nodes.Add("mbc-93")
        tv_Strukturbaum.Nodes(0).Nodes(13).ImageIndex = 1
        tv_Strukturbaum.Nodes(0).Nodes(13).SelectedImageIndex = 1

    End Sub

    '**********************************************************************************************
    '*** Strukturbaum - Anzeige des ausgewaehlten Strukturbaumeintrags                          ***
    '**********************************************************************************************

    Private Sub Strukturbaum_AfterSelect(ByVal sender As System.Object, ByVal e As System.Windows.Forms.TreeViewEventArgs) Handles tv_Strukturbaum.AfterSelect

        ' Baumwurzel ausgewaehlt?

        If e.Node.Text = "MBCAN" Then
            Register_Modul_Bilder_Show(0)
        End If

        ' Modultyp ausgewaehlt?

        If Microsoft.VisualBasic.Left(e.Node.Text, 4) = "mbc-" Then

            Register_Modul_Bilder_Show(Strukturbaum_GetTypIndex(e.Node.Text))
            Register_Modul_Werte_Show(False)
            COMRefreshStop()

        End If

        ' Modul ausgewaehlt?

        If Not (e.Node.Parent Is Nothing) Then

            If (Microsoft.VisualBasic.Left(e.Node.Parent.Text, 4) = "mbc-") Then

                SelNode = tv_Strukturbaum.SelectedNode

                ' Stoppe ggf. laufenden Refreshzyklus

                COMRefreshStop()

                ' Setze die globalen Variablen fuer das ausgewaehlte Modul

                AktTyp = Strukturbaum_GetTypIndex(e.Node.Parent.Text)
                AktNr = e.Node.Index + 1

                ' Zeige Bilder des ausgewaehlten Moduls an

                Register_Modul_Bilder_Show(AktTyp)
                Register_Modul_Werte_Show(False)

                ' Waehle Refreshaktion je nach Modulstatus aus oder lasse das Loeschen
                ' zu

                Select Case MBC.ModulTyp(AktTyp).Modul(AktNr).Status

                    Case IsMissing

                        Register_Modul_Werte_Show(True)

                        ' Wenn nicht aktiv dann zeige den Loeschen-Button an
                        btn_ControlLoeschen.Enabled = True

                    Case IsAlive

                        ' Wenn Alive dann starte Auslesen des Moduls
                        COMRefreshStart(MBC_ALLG_START, MBC.ModulTyp(AktTyp).Modul(AktNr).IOEnde)

                    Case IsComplete

                        ' Werte aktualisieren
                        Register_Modul_Werte_Show(True)

                        ' Gebe Eingabemoeglichkeiten frei
                        Register_Modul_Edit_Enable(True)

                End Select

            Else

                ' Wenn nein, dann setze die aktuellen Indices des Moduls
                ' auf das ROOT-Modul. Somit werden die Statusanzeigen nur dann 
                ' angezeigt, wenn auch wirklich ein Modul ausgewählt und aktiv ist

                AktTyp = Strukturbaum_GetTypIndex(e.Node.Text)
                AktNr = 0

                ' Zeige Bilder des ausgewaehlten Moduls an

                Register_Modul_Bilder_Show(AktTyp)
                Register_Modul_Werte_Show(False)


            End If

        End If

    End Sub

    '**********************************************************************************************
    '*** Strukturbaum - Bestimme den Modultyp                                                   ***
    '**********************************************************************************************

    Public Function Strukturbaum_GetTypIndex(ByVal Typ As String) As Integer

        ' Stelle fest welcher TypIndex aufgerufen wird
        ' Der Index = 1 gehoert dem Modul mbc-80
        ' Alle weiteren Module sind aufsteigend zum mbc-80, also mbc-81, mbc-82 usw.

        Dim Index As Integer

        Index = Val(Microsoft.VisualBasic.Right(Typ, 2))

        Index -= 79

        If Index <= 0 Then
            Strukturbaum_GetTypIndex = 0
        Else
            Strukturbaum_GetTypIndex = Index
        End If

    End Function

    '**********************************************************************************************
    '*** Strukturbaum - Trage neues Modul entsprechend des Modultyps in den Menuebaum ein       ***
    '*** ModulTyp: Modultyp                                                                     ***
    '*** ModulNr: Modulnummer                                                                   ***
    '*** Icon: Status des Moduls                                                                ***
    '*** Italic: Modulnummer                                                                    ***
    '*** Change: Modulnummr                                                                     ***
    '**********************************************************************************************

    Public Sub Strukturbaum_MenueIconChange(ModTyp As Integer, ModNr As Integer, Italic As Boolean, Change As Boolean)

        Dim Status As Integer
        Dim NodeNr As Integer
        Dim NodeItem As Integer

        ' Bestimme die Node-Indices

        Status = MBC.ModulTyp(ModTyp).Modul(ModNr).Status
        NodeNr = ModTyp - 1
        NodeItem = ModNr - 1

        ' Modul auf welches die Aenderungen wirken sollen
        ' Da es sowohl aktive als auch inaktive Module betreffen kann wird der Status 
        ' im globalen Modul-Array zugewiesen

        MBC.ModulTyp(ModTyp).Modul(ModNr).Status = Status

        ' Wenn der Modultyp geloescht war aber die ALIVE-Meldung noch kam dann stoppe Prozedur
        ' Das verhindert eine Zugriffsverletzung

        If tv_Strukturbaum.Nodes(0).Nodes(NodeNr).Nodes.Count = 0 Then Exit Sub

        ' Weise Icon im Strukturbaum den Modulen zu

        tv_Strukturbaum.Nodes(0).Nodes(NodeNr).Nodes(NodeItem).ImageIndex = Status

        tv_Strukturbaum.Nodes(0).Nodes(NodeNr).Nodes(NodeItem).SelectedImageIndex = Status

        ' Weise Italic als Schriftart des Modulnamens zu wenn gefordert

        If Italic Then
            tv_Strukturbaum.Nodes(0).Nodes(NodeNr).Nodes(NodeItem).NodeFont = New Font(tv_Strukturbaum.Font, FontStyle.Italic)
        Else
            tv_Strukturbaum.Nodes(0).Nodes(NodeNr).Nodes(NodeItem).NodeFont = New Font(tv_Strukturbaum.Font, FontStyle.Regular)
        End If

        ' Wenn das angefragte ALIVE-Modul das aktuell ausgewählte ist
        ' dann zeige die Modulwerte an wenn sich der Status geaendert hat

        If ((AktTyp = ModTyp) And (AktNr = ModNr) And Change) Then

            MBC.ModulTyp(AktTyp).Modul(AktNr).Status = Status

            ' Nehme Selection zurueck wenn es sich nicht um ein komplett ausgelesenes Modul handelt

            If Status <> IsComplete Then
                tv_Strukturbaum.SelectedNode = Nothing
                tab_Control.Select()
            End If

            ' Modulwerte anzeigen

            If Status <> IsDeleted Then
                Register_Modul_Werte_Show(True)
            Else
                Register_Modul_Werte_Show(False)
            End If

        End If

    End Sub

    '**********************************************************************************************
    '*** Strukturbaum - Trage neues Modul entsprechend des Modultyps in den Menuebaum ein       ***
    '**********************************************************************************************
    Public Sub Strukturbaum_MenueAddItem(TypIndex As Integer)
        Dim Index As Integer
        Dim NodeNr As Integer

        NodeNr = TypIndex - 1

        tv_Strukturbaum.Nodes(0).Nodes(NodeNr).Nodes.Add("")

        Index = tv_Strukturbaum.Nodes(0).Nodes(NodeNr).Nodes.Count - 1

        tv_Strukturbaum.Nodes(0).Nodes(NodeNr).Nodes(Index).Text = MBC.ModulTyp(TypIndex).Modul(Index + 1).Name

        If (MBC.ModulTyp(TypIndex).Modul(Index + 1).Status = IsComplete) Then
            tv_Strukturbaum.Nodes(0).Nodes(NodeNr).Nodes(Index).ImageIndex = IsComplete
            tv_Strukturbaum.Nodes(0).Nodes(NodeNr).Nodes(Index).SelectedImageIndex = IsComplete
            tv_Strukturbaum.Nodes(0).Nodes(NodeNr).Nodes(Index).NodeFont = New Font(tv_Strukturbaum.Font, FontStyle.Italic)
        End If

        If (MBC.ModulTyp(TypIndex).Modul(Index + 1).Status = IsAlive) Then
            tv_Strukturbaum.Nodes(0).Nodes(NodeNr).Nodes(Index).ImageIndex = IsAlive
            tv_Strukturbaum.Nodes(0).Nodes(NodeNr).Nodes(Index).SelectedImageIndex = IsAlive
        End If

        If (MBC.ModulTyp(TypIndex).Modul(Index + 1).Status = IsMissing) Then
            tv_Strukturbaum.Nodes(0).Nodes(NodeNr).Nodes(Index).ImageIndex = IsMissing
            tv_Strukturbaum.Nodes(0).Nodes(NodeNr).Nodes(Index).SelectedImageIndex = IsMissing
        End If

        If (MBC.ModulTyp(TypIndex).Modul(Index + 1).Status = IsDeleted) Then
            tv_Strukturbaum.Nodes(0).Nodes(NodeNr).Nodes(Index).ImageIndex = IsDeleted
            tv_Strukturbaum.Nodes(0).Nodes(NodeNr).Nodes(Index).SelectedImageIndex = IsDeleted
            tv_Strukturbaum.Nodes(0).Nodes(NodeNr).Nodes(Index).NodeFont = New Font(tv_Strukturbaum.Font, FontStyle.Italic)
        End If

    End Sub

    '**********************************************************************************************
    '*** Strukturbaum - Aendere Name im Strukturbaum                                            ***
    '**********************************************************************************************

    Public Sub Strukturbaum_MenueChangeLabel()

        tv_Strukturbaum.Nodes(0).Nodes(AktTyp - 1).Nodes(AktNr - 1).Text = MBC.ModulTyp(AktTyp).Modul(AktNr).Name

    End Sub

    '++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    '+++ Menue und Statusleiste                                                                 +++
    '+++ Allgemeine Funktionen zum Umgang mit den Menueeintraegen                               +++
    '++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    '**********************************************************************************************
    '*** Menue - Statusleiste initialisieren                                                    ***
    '**********************************************************************************************

    Private Sub Statusleiste_Init()

        stp_RS232.Text = "RS232: " & i_None
        stp_BT.Text = "BT: " & Sprachen(i_MAIN, i_None)
        stp_Refresh.Text = Sprachen(i_MAIN, i_stp_Refresh)
        stp_Upgrade.Text = Sprachen(i_MAIN, i_stp_Upgrade_None)
        spb_Refresh.Value = 0
        spb_Refresh.Enabled = False
        stp_Alive.Text = Sprachen(i_MAIN, i_stp_Alive_None)

    End Sub

    '**********************************************************************************************
    '*** Menue - Eintraege initialisieren                                                       ***
    '**********************************************************************************************

    Private Sub Menue_Init()

        ts_MenuVerbindungRS232.Enabled = False
        ts_MenuVerbindungRS232Trennen.Enabled = False
        ts_MenuVerbindungRS232Verbinden.Enabled = False
        ts_MenuVerbindungBluetooth.Enabled = False
        ts_MenuVerbindungBluetoothTrennen.Enabled = False
        ts_MenuVerbindungBluetoothVerbinden.Enabled = False
        ts_MenuVerbindungTCP.Enabled = False
        ts_MenuVerbindungTCPTrennen.Enabled = False
        ts_MenuVerbindungTCPVerbinden.Enabled = False

    End Sub

    '**********************************************************************************************
    '*** Menue - Eintrag Beenden                                                                ***
    '**********************************************************************************************

    Private Sub Menue_System_Beenden_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles ts_MenuSystemSchliessen.Click

        Me.Close()

    End Sub

    '**********************************************************************************************
    '*** Menue - Moduldatenbank speichern                                                       ***
    '**********************************************************************************************
    Private Sub Menue_System_Speichern_Click(sender As Object, e As EventArgs) Handles ts_MenuSystemSpeichern.Click

        If MsgBox(Sprachen(i_MSGBOX_MAIN_Db, i_MSGBOX_MAIN_Db_1) & DBVersion &
                 Sprachen(i_MSGBOX_MAIN_Db, i_MSGBOX_MAIN_Db_2), vbYesNo, Sprachen(i_MSGBOX_MAIN_Db, i_MSGBOX_MAIN_Db_3)) = vbYes Then
            DB_Modul_Speichern(1)
        End If

    End Sub

    '**********************************************************************************************
    '*** Menue - Datenbank schreibgeschuetzt anzeigen                                           ***
    '**********************************************************************************************
    Private Sub Menue_System_Anzeigen_Click(sender As Object, e As EventArgs) Handles ts_MenuSystemAnzeigen.Click
        frmDatenbank.ShowDialog()
    End Sub

    '**********************************************************************************************
    '*** Menue - Neue Datenbank anlegen                                                         ***
    '**********************************************************************************************
    Private Sub Menue_System_Neu_Click(sender As Object, e As EventArgs) Handles ts_MenuSystemNeu.Click
        DB_Modul_Neu()
    End Sub

    '**********************************************************************************************
    '*** Menue - Sprache einstellen - Deutsch                                                   ***
    '**********************************************************************************************

    Private Sub Menu_System_Sprache_Deutsch_Click(sender As Object, e As EventArgs) Handles ts_MenuSystemSprache_Deutsch.Click
        SprachIndex = 1
        SetSprache()
    End Sub

    '**********************************************************************************************
    '*** Menue - Sprache einstellen - Deutsch                                                   ***
    '**********************************************************************************************

    Private Sub Menu_System_Sprache_Französisch_Click(sender As Object, e As EventArgs) Handles ts_MenuSystemSprache_Französisch.Click
        SprachIndex = 2
        SetSprache()
    End Sub

    '**********************************************************************************************
    '*** Menue - Sprache einstellen - Deutsch                                                   ***
    '**********************************************************************************************

    Private Sub Menu_System_Sprache_Englisch_Click(sender As Object, e As EventArgs) Handles ts_MenuSystemSprache_Englisch.Click
        SprachIndex = 3
        SetSprache()
    End Sub

    '**********************************************************************************************
    '*** Menue - Eintrag Einstellen                                                             ***
    '**********************************************************************************************

    Private Sub Menue_Verbindung_Einstellen_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles ts_MenuVerbindungEinstellen.Click

        frmInit.ShowDialog()

    End Sub

    '**********************************************************************************************
    '*** Menue - RS232 - Menueeintrag Verbinden                                                 ***
    '**********************************************************************************************
    Private Sub Menue_Verbindung_RS232_Verbinden_Click(sender As Object, e As EventArgs) Handles ts_MenuVerbindungRS232Verbinden.Click
        ts_MenuSystemSprache.Enabled = False
        RS232Oeffnen()
    End Sub

    '**********************************************************************************************
    '*** Menue - RS232 - Menueeintrag Trennen                                                   ***
    '**********************************************************************************************
    Private Sub Menue_Verbindung_RS232_Trennen_Click(sender As Object, e As EventArgs) Handles ts_MenuVerbindungRS232Trennen.Click
        ts_MenuSystemSprache.Enabled = True
        RS232Schliessen()
    End Sub

    '**********************************************************************************************
    '*** Menue - BT - Menueeintrag Verbinden                                                    ***
    '**********************************************************************************************

    Private Sub Menue_Verbindung_Bluetooth_Verbinden_Click(sender As Object, e As EventArgs) Handles ts_MenuVerbindungBluetoothVerbinden.Click
        ts_MenuSystemSprache.Enabled = False
        BTOeffnen()
    End Sub

    '**********************************************************************************************
    '*** Menue - BT - Menueeintrag Trennen                                                      ***
    '**********************************************************************************************

    Private Sub Menue_Verbindung_Bluetooth_Trennen_Click(sender As Object, e As EventArgs) Handles ts_MenuVerbindungBluetoothTrennen.Click
        ts_MenuSystemSprache.Enabled = True
        BTSchliessen()
    End Sub

    '**********************************************************************************************
    '*** Menue - TCP - Menueeintrag Verbinden                                                   ***
    '**********************************************************************************************

    Private Sub Menue_Verbindung_TCP_Verbinden_Click(sender As Object, e As EventArgs) Handles ts_MenuVerbindungTCPVerbinden.Click
        ts_MenuSystemSprache.Enabled = False
        TCPOeffnen()
    End Sub

    '**********************************************************************************************
    '*** Menue - TCP - Menueeintrag Trennen                                                     ***
    '**********************************************************************************************

    Private Sub Menue_Verbindung_TCP_Trennen_Click(sender As Object, e As EventArgs) Handles ts_MenuVerbindungTCPTrennen.Click
        ts_MenuSystemSprache.Enabled = True
        TCPSchliessen()
    End Sub

    '**********************************************************************************************
    '*** Menue - Hilfedatei anzeigen                                                            ***
    '**********************************************************************************************

    Private Sub Menue_Info_Hilfe_Click(sender As Object, e As EventArgs) Handles ts_MenuInfoHilfe.Click

        ' Anzeige via ShellExecute über das für html registrierte Programm

        System.Diagnostics.Process.Start("https://github.com/TommyGermanyRetro/MBCAN")

    End Sub

    '**********************************************************************************************
    '*** Menue - Bedienungsanleitung anzeigen                                                   ***
    '**********************************************************************************************

    Private Sub Menue_Info_Bedienungsanleitung_Click(sender As Object, e As EventArgs) Handles ts_MenuInfoBedienungsanleitung.Click

        Dim pdfFilePath As String

        pdfFilePath = "Inifiles\Bedienungsanleitung.pdf"

        ' Bedienungsanleitung vorhanden? Wenn ja, dann loesche sie
        ' so wird auf einfache Weise festegestellt ob sie ggf. woanders genutzt wird

        If File.Exists("Inifiles\Bedienungsanleitung.pdf") Then
            Try
                File.Delete("Inifiles\Bedienungsanleitung.pdf")
            Catch ex As Exception
                MsgBox(Sprachen(i_MSGBOX_MAIN_Bedien, i_MSGBOX_MAIN_Bedien_1), vbOKOnly, Sprachen(i_MSGBOX_MAIN_Bedien, i_MSGBOX_MAIN_Bedien_2))
                Exit Sub
            End Try
        End If

        ' Erzeuge File neu

        System.IO.File.WriteAllBytes(pdfFilePath, My.Resources.mbc_verwalten_bd_parametriercenter)

        ' Anzeige via ShellExecute über das für PDF registrierte Programm

        System.Diagnostics.Process.Start(pdfFilePath)

    End Sub

    '**********************************************************************************************
    '*** Menue - Info zum Prgramm anzeigen                                                      ***
    '**********************************************************************************************

    Private Sub Menue_Info_Copyright_Click(sender As Object, e As EventArgs) Handles ts_MenuInfoCopyright.Click

        frmInfo.ShowDialog()

    End Sub

    '++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    '+++ RS232                                                                                  +++
    '+++ Funktionen zum Umgang mit der RS232-Schnittstelle                                      +++
    '++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    '**********************************************************************************************
    '*** RS232 - Schnittstelle oeffnen                                                          ***
    '**********************************************************************************************

    Public Sub RS232Oeffnen()

        'RS232-Schnittstelle oeffnen

        Try

            RS232Client.Open()

        Catch ex As Exception

            MsgBox(Sprachen(i_MSGBOX_MAIN_RS232, i_MSGBOX_MAIN_RS232_2) & " (" & RS232Client.PortName & ") " &
                   Sprachen(i_MSGBOX_MAIN_RS232, i_MSGBOX_MAIN_RS232_4), vbOKOnly, Sprachen(i_MSGBOX_MAIN_RS232, i_MSGBOX_MAIN_RS232_1))
            Exit Sub

        End Try

        'RS232-Thread starten

        CommunicationThread.StartReceiveThread(RS232ThreadReceive, RS232ThreadIsActive, AddressOf ReceiveMessagesRS232)

        'Logfile oeffnen

        COMLogFileOeffnen(RS232Log)

        'Buttons und Menueeintraege formatieren

        ts_MenuVerbindungRS232Verbinden.Enabled = False
        ts_MenuVerbindungRS232Trennen.Enabled = True

        ts_MenuVerbindungBluetooth.Enabled = False
        ts_MenuVerbindungBluetoothVerbinden.Enabled = False
        ts_MenuVerbindungBluetoothTrennen.Enabled = False

        ts_MenuVerbindungTCP.Enabled = False
        ts_MenuVerbindungTCPVerbinden.Enabled = False
        ts_MenuVerbindungTCPTrennen.Enabled = False

        ts_MenuVerbindungEinstellen.Enabled = False

        rtb_MonitorReceived.Enabled = True

        stp_RS232.Text = "RS232: " & RS232Client.PortName & " - 8N1 -" & Str(RS232BaudRate)
        lbl_MonitorNachrichtenherkunft.Text = Sprachen(i_MAIN, i_lbl_MonitorNachrichtenherkunftRS)

        'Timer starten

        tm_Neu.Start()
        tm_Alive.Start()

    End Sub

    '**********************************************************************************************
    '*** RS232 - Schnittstelle schliessen                                                       ***
    '**********************************************************************************************

    Public Sub RS232Schliessen()

        ' RS232-Receive-Thread schliessen

        CommunicationThread.StopReceiveThread(RS232ThreadIsActive, RS232ThreadReceive, 2000)

        'RS232-Port schliessen

        Try
            RS232Client.Close()
        Catch ex As Exception

        End Try

        'Logfile schliessen

        COMLogFileSchliessen()

        ' Module als nicht anwesend markieren, Registerkarten deaktivieren

        Modul_AlleMissing()

        'Buttons und Menueeintraege formatieren

        If BTIsInit = True Then
            ts_MenuVerbindungBluetooth.Enabled = True
            ts_MenuVerbindungBluetoothVerbinden.Enabled = True
            ts_MenuVerbindungBluetoothTrennen.Enabled = False
        End If

        If TCPIsInit = True Then
            ts_MenuVerbindungTCP.Enabled = True
            ts_MenuVerbindungTCPVerbinden.Enabled = True
            ts_MenuVerbindungTCPTrennen.Enabled = False
        End If

        ts_MenuVerbindungRS232Verbinden.Enabled = True
        ts_MenuVerbindungRS232Trennen.Enabled = False

        ts_MenuVerbindungEinstellen.Enabled = True

        rtb_MonitorReceived.Enabled = False

        lbl_MonitorNachrichtenherkunft.Text = Sprachen(i_MAIN, i_None)
        stp_RS232.Text = "RS232: " & Sprachen(i_MAIN, i_None)

    End Sub

    '**********************************************************************************************
    '*** RS232 - Nachricht verarbeiten                                                          ***
    '**********************************************************************************************

    Private Sub ReceiveMessagesRS232()

        Dim RS232RxBuffer As String
        Dim index As Integer
        Dim RS232BufferCount As Integer

        ' Thread ausfuehren bis Trennung angefordert

        While RS232ThreadIsActive = True

            ' Hole Anzahl der Bytes im Stream

            Try
                RS232BufferCount = RS232Client.BytesToRead
            Catch ex As Exception
                RS232BufferCount = 0
            End Try

            ' Wenn Daten vorliegen dann werte Stream aus

            If RS232BufferCount > 0 Then

                ' Lese Zeile aus dem COM-Port-Buffer

                Try

                    RS232RxBuffer = RS232Client.ReadLine()

                    ' Zerlege Nachricht, falls mehrere vorhanden sind

                    For index = 1 To Len(RS232RxBuffer) Step 40

                        If ((Len(RS232RxBuffer) > 37) And (Microsoft.VisualBasic.Mid(RS232RxBuffer, (index * 40) - 4, 1) = " ")) Then

                            COMMsgReceived(Microsoft.VisualBasic.Mid(RS232RxBuffer, index, 38))

                        End If
                    Next

                Catch ex As Exception

                End Try

            End If

        End While

    End Sub

    '++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    '+++ Bluetooth                                                                              +++
    '+++ Funktionen zum Umgang mit der Bluetooth-Schnittstelle                                  +++
    '++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    '**********************************************************************************************
    '*** BT - Schnittstelle oeffnen                                                             ***
    '**********************************************************************************************

    Public Sub BTOeffnen()

        ' Teste ob die Schnittstelle verfuegb_ar ist

        Try

            BTClient.Open()

        Catch ex As Exception

            MsgBox(Sprachen(i_MSGBOX_MAIN_BT, i_MSGBOX_MAIN_BT_2) & " (" & BTClient.PortName & ") " & Sprachen(i_MSGBOX_MAIN_BT, i_MSGBOX_MAIN_BT_4) &
                   Chr(13) & Chr(13) & String.Format("Error: {0}", ex.Message), vbOKOnly, Sprachen(i_MSGBOX_MAIN_BT, i_MSGBOX_MAIN_BT_1))

            Exit Sub

        End Try

        'BT-Thread starten

        CommunicationThread.StartReceiveThread(BTThreadReceive, BTThreadIsActive, AddressOf ReceiveMessagesBT)

        'Logfile oeffnen

        COMLogFileOeffnen(BTLog)


        'Buttons und Menueeintraege formatieren

        ts_MenuVerbindungRS232.Enabled = False
        ts_MenuVerbindungRS232Verbinden.Enabled = False
        ts_MenuVerbindungRS232Trennen.Enabled = False

        ts_MenuVerbindungTCP.Enabled = False
        ts_MenuVerbindungTCPVerbinden.Enabled = False
        ts_MenuVerbindungTCPTrennen.Enabled = False

        ts_MenuVerbindungBluetoothVerbinden.Enabled = False
        ts_MenuVerbindungBluetoothTrennen.Enabled = True

        ts_MenuVerbindungEinstellen.Enabled = False

        rtb_MonitorReceived.Enabled = True

        stp_BT.Text = "BT: " & BTClient.PortName & " - 8N1 -" & " " & BTBaudRate
        lbl_MonitorNachrichtenherkunft.Text = Sprachen(i_MAIN, i_lbl_MonitorNachrichtenherkunftBT)

        ' Timer starten

        tm_Neu.Start()
        tm_Alive.Start()

    End Sub

    '**********************************************************************************************
    '*** BT - Schnittstelle schliessen                                                          ***
    '**********************************************************************************************

    Public Sub BTSchliessen()

        ' BT-Receive-Thread schliessen

        CommunicationThread.StopReceiveThread(BTThreadIsActive, BTThreadReceive, 2000)

        'BT-Port schliessen

        Try
            BTClient.Close()
        Catch ex As Exception

        End Try

        'Logfile schliessen

        COMLogFileSchliessen()

        ' Module als nicht anwesend markieren, Registerkarten deaktivieren

        Modul_AlleMissing()

        'Buttons und Menueeintraege formatieren

        If RS232IsInit = True Then
            ts_MenuVerbindungRS232.Enabled = True
            ts_MenuVerbindungRS232Verbinden.Enabled = True
            ts_MenuVerbindungRS232Trennen.Enabled = False
        End If

        If TCPIsInit = True Then
            ts_MenuVerbindungTCP.Enabled = True
            ts_MenuVerbindungTCPVerbinden.Enabled = True
            ts_MenuVerbindungTCPTrennen.Enabled = False
        End If

        ts_MenuVerbindungBluetoothVerbinden.Enabled = True
        ts_MenuVerbindungBluetoothTrennen.Enabled = False

        ts_MenuVerbindungEinstellen.Enabled = True

        rtb_MonitorReceived.Enabled = False

        lbl_MonitorNachrichtenherkunft.Text = Sprachen(i_MAIN, i_lbl_MonitorNachrichtenherkunft)
        stp_BT.Text = "BT: " & Sprachen(i_MAIN, i_None)


    End Sub


    '**********************************************************************************************
    '*** BT - Nachricht verarbeiten                                                             ***
    '**********************************************************************************************

    Private Sub ReceiveMessagesBT()

        Dim BTRxBuffer As String
        Dim index As Integer
        Dim BTBufferCount As Integer

        ' Thread ausfuehren bis Trennung angefordert

        While BTThreadIsActive = True

            ' Schleife, solange Daten im Lesebuffer des COM-Ports abgeholt werden können

            Try
                BTBufferCount = BTClient.BytesToRead
            Catch ex As Exception
                BTBufferCount = 0
            End Try

            ' Wenn Daten vorliegen dann werte Stream aus

            If BTBufferCount > 0 Then

                ' Lese Zeile aus dem COM-Port-Buffer

                Try

                    BTRxBuffer = BTClient.ReadLine()

                    ' Zerlege Nachricht, falls mehrere vorhanden sind

                    For index = 1 To Len(BTRxBuffer) Step 40

                        If Len(BTRxBuffer) > 37 Then

                            COMMsgReceived(Microsoft.VisualBasic.Mid(BTRxBuffer, index, 38))

                        End If
                    Next

                Catch ex As Exception

                End Try

            End If

        End While

    End Sub

    '++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    '+++ TCP                                                                                    +++
    '+++ Funktionen zum Umgang mit der TCP-Schnittstelle                                        +++
    '++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    '**********************************************************************************************
    '*** TCP - Schnittstelle oeffnen                                                            ***
    '**********************************************************************************************

    Public Sub TCPOeffnen()

        stp_TCP.Text = "TCP: " & Sprachen(i_MAIN, i_stp_TCP_Connect)
        Refresh()

        Try
            'TCP-Kanal oeffnen

            TCPclient = New System.Net.Sockets.TcpClient()
            TCPclient.Connect(TCPIP, TCPSocketNo)
            TCPstream = TCPclient.GetStream

            If Not TCPstream.CanRead Then
                MsgBox(Sprachen(i_MSGBOX_MAIN_TCP, i_MSGBOX_MAIN_TCP_3), vbOKOnly, Sprachen(i_MSGBOX_MAIN_TCP, i_MSGBOX_MAIN_TCP_1))
                TCPclient.Close()
                Exit Sub
            Else
                If Not TCPstream.CanWrite Then
                    MsgBox(Sprachen(i_MSGBOX_MAIN_TCP, i_MSGBOX_MAIN_TCP_4), vbOKOnly, Sprachen(i_MSGBOX_MAIN_TCP, i_MSGBOX_MAIN_TCP_1))
                    TCPclient.Close()
                    Exit Sub
                End If
            End If

        Catch ex As Exception

            MsgBox(Sprachen(i_MSGBOX_MAIN_TCP, i_MSGBOX_MAIN_TCP_5) & Chr(13) & Chr(13) &
                   String.Format("Error: {0}", ex.Message), vbOKOnly, Sprachen(i_MSGBOX_MAIN_TCP, i_MSGBOX_MAIN_TCP_1))

            stp_TCP.Text = "TCP: " & Sprachen(i_MAIN, i_None)

            Exit Sub

        End Try

        'TCP-Thread starten

        CommunicationThread.StartReceiveThread(TCPThreadReceive, TCPThreadIsActive, AddressOf TCPReceiveMessages)

        'Logfile oeffnen

        COMLogFileOeffnen(TCPLog)

        'Buttons und Menueeintraege formatieren

        ts_MenuVerbindungRS232.Enabled = False
        ts_MenuVerbindungRS232Verbinden.Enabled = False
        ts_MenuVerbindungRS232Trennen.Enabled = False

        ts_MenuVerbindungBluetooth.Enabled = False
        ts_MenuVerbindungBluetoothVerbinden.Enabled = False
        ts_MenuVerbindungBluetoothTrennen.Enabled = False

        ts_MenuVerbindungTCPVerbinden.Enabled = False
        ts_MenuVerbindungTCPTrennen.Enabled = True

        ts_MenuVerbindungEinstellen.Enabled = False

        rtb_MonitorReceived.Enabled = True

        lbl_MonitorNachrichtenherkunft.Text = Sprachen(i_MAIN, i_lbl_MonitorNachrichtenherkunftTCP)
        stp_TCP.Text = "TCP: IP-" & TCPIP & " RX-/TX-" & Format("#####", TCPSocketNo)

        'Timer starten

        tm_Neu.Start()
        tm_Alive.Start()

    End Sub

    '**********************************************************************************************
    '*** TCP - Schnittstelle schliessen                                                         ***
    '**********************************************************************************************

    Public Sub TCPSchliessen()

        'TCP-Thread schliessen

        stp_TCP.Text = "TCP: " & Sprachen(i_MAIN, i_stp_TCP_Disconnect)

        CommunicationThread.StopReceiveThread(TCPThreadIsActive, TCPThreadReceive, 4000)

        'TCP-Ports schließen

        Try
            TCPclient.Close()
        Catch ex As Exception

        End Try

        'Logfile schliessen

        COMLogFileSchliessen()

        ' Module als nicht anwesend markieren, Registerkarten deaktivieren

        Modul_AlleMissing()

        'Buttons und Menueeintraege formatieren

        If RS232IsInit = True Then
            ts_MenuVerbindungRS232.Enabled = True
            ts_MenuVerbindungRS232Verbinden.Enabled = True
            ts_MenuVerbindungRS232Trennen.Enabled = False
        End If

        If BTIsInit = True Then
            ts_MenuVerbindungBluetooth.Enabled = True
            ts_MenuVerbindungBluetoothVerbinden.Enabled = True
            ts_MenuVerbindungBluetoothTrennen.Enabled = False
        End If

        ts_MenuVerbindungTCPVerbinden.Enabled = True
        ts_MenuVerbindungTCPTrennen.Enabled = False

        ts_MenuVerbindungEinstellen.Enabled = True

        stp_TCP.Text = "TCP: " & Sprachen(i_MAIN, i_None)

        rtb_MonitorReceived.Enabled = False

        lbl_MonitorNachrichtenherkunft.Text = Sprachen(i_MAIN, i_lbl_MonitorNachrichtenherkunft)

    End Sub

    '**********************************************************************************************
    '*** TCP - Nachricht verarbeiten                                                            ***
    '**********************************************************************************************

    Private Sub TCPReceiveMessages()

        Dim index As Integer
        Dim TCPState As New Object
        Dim TCPBuffer As String
        Dim bytes(13) As Byte

        While TCPThreadIsActive = True

            Try

                ' Lese Nachricht aus dem TCP-Buffer

                If TCPstream.CanRead Then

                    TCPstream.Read(bytes, 0, 13)

                    ' Datenbytes der Nachricht formatieren

                    TCPBuffer = ""

                    For index = 0 To 12
                        TCPBuffer += FormHEX("0", 2, CInt(bytes(index)))
                        If index <> 12 Then TCPBuffer += " "
                    Next

                    ' Daten des Threads an den Hauptthread uebergeben

                    ' If TCPBuffer <> "00 00 00 00 00 00 00 00 00 00 00 00 00" Then

                    COMMsgReceived(TCPBuffer)

                    ' End If

                End If

            Catch ex As Exception

            End Try

        End While

    End Sub


End Class