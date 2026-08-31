'**************************************************************************************************
'*** Nachrichtenauswertung / COM-Protokoll                                                      ***
'*** Ausgelagerter Teil von frmMain (Partial Class): Wertet eingehende CAN-Nachrichten aus      ***
'*** (Neuanmeldung/Alive/Read/Write/Upgrade-Automat) und formatiert/versendet ausgehende. Reiner***
'*** Quelltext-Umzug ohne Verhaltensaenderung - siehe frmMain.vb fuer UI und Verbindungsschicht.***
'**************************************************************************************************

Imports System.ComponentModel
Imports System.IO
Imports System.Net.Sockets
Imports System.Reflection
Imports System.Threading

Partial Class frmMain

    '++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    '+++ Nachrichtenauswertung                                                                  +++
    '+++ Funktionen zum Umgang mit den Daten der UDP-/RS232-/BT-Schnittstelle                   +++
    '++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    ' Hilfsfunktionen fuer Thread-Shared-Datenaustausch

    Delegate Sub SetTextCallback(ByVal [text] As String)
    Delegate Sub SetTextCallback2(ByVal [text1] As String, ByVal [text2] As String)

    '**********************************************************************************************
    '*** COM - Threadshared - Trage Nachricht von BT, UDP und RS232 in Register "Modul" ein     ***
    '**********************************************************************************************

    Public Sub COMMsgReceived(ByVal [text] As String)

        Dim MsgLine1 As String = ""
        Dim MsgLine2 As String = ""

        If tv_Strukturbaum.InvokeRequired Then

            Dim x As New SetTextCallback(AddressOf COMMsgReceived)
            Invoke(x, New Object() {(text)})

        Else

            ' Erhoehe Nachrichtenzaehler

            COMMsgCount += 1

            ' Nachricht im Monitorfenster ausgeben

            COMBuildMsgMonitor([text], MsgLine1, MsgLine2)
            COMMonitorMsgReceived(Microsoft.VisualBasic.TimeString & Format(Now, ".fff") & " R " & MsgLine1, MsgLine2)

            ' CSx-Erkennung: eine Ping-ANFRAGE (CAN_ID = 0x30) wird nur von einer Zentrale
            ' gesendet, nie von einem Zubehoermodul - ihr Empfang allein genuegt als Nachweis.
            ' Unabhaengig vom nachfolgenden Schalten/S88-Filter, da eine Ping-Anfrage nicht in
            ' dessen CAN_ID-Menge (16/17/23) faellt.

            If Microsoft.VisualBasic.Mid([text], 4, 2) = "30" Then
                FLAG_CSxPingEmpfangen = True
                CSxLastPingUtc = DateTime.Now
                Register_Modul_CSxLock_Apply()
            End If

            ' Nachricht auf Befehl "Schalten" oder "S88" testen
            ' Wenn wahr, Nachricht im Befehlsbuffer ablegen

            If ((Microsoft.VisualBasic.Mid([text], 4, 2) = "16") Or
                (Microsoft.VisualBasic.Mid([text], 4, 2) = "17") Or
                (Microsoft.VisualBasic.Mid([text], 4, 2) = "23")) Then
                Befehl.CanID = (Microsoft.VisualBasic.Mid([text], 4, 2))
                Befehl.Response = (Convert.ToInt32((Microsoft.VisualBasic.Mid([text], 4, 2)), 16) And 1)
                Befehl.Command = "&H" & Hex((Convert.ToInt32((Microsoft.VisualBasic.Mid([text], 4, 2)), 16) >> 1))
                Befehl.ID = Convert.ToInt32((Microsoft.VisualBasic.Mid([text], 22, 2)), 16) * 256
                Befehl.ID += Convert.ToInt32((Microsoft.VisualBasic.Mid([text], 25, 2)), 16)
                Befehl.Count = Convert.ToInt32((Microsoft.VisualBasic.Mid([text], 34, 2)), 16)
                Befehl.D0 = (Microsoft.VisualBasic.Mid([text], 16, 2))
                Befehl.D1 = (Microsoft.VisualBasic.Mid([text], 19, 2))
                Befehl.D2 = (Microsoft.VisualBasic.Mid([text], 22, 2))
                Befehl.D3 = (Microsoft.VisualBasic.Mid([text], 25, 2))
                Befehl.D4 = (Microsoft.VisualBasic.Mid([text], 28, 2))
                Befehl.D5 = (Microsoft.VisualBasic.Mid([text], 31, 2))
                Befehl.D6 = (Microsoft.VisualBasic.Mid([text], 34, 2))
                Befehl.D7 = (Microsoft.VisualBasic.Mid([text], 37, 2))
                Befehl.Msg = [text]

                COMCmdReceive()

            End If

        End If

    End Sub

    '**********************************************************************************************
    '*** Register - Trage Nachricht von BT, UDP und RS232 in Register "Monitor" ein             ***
    '**********************************************************************************************

    Public Sub COMMonitorMsgReceived(ByVal [text1] As String, ByVal [text2] As String)

        If rtb_MonitorReceived.InvokeRequired Then
            Dim x As New SetTextCallback2(AddressOf COMMonitorMsgReceived)
            Invoke(x, New Object() {(text1), (text2)})

        Else

            Dim Zeile() As String
            Dim ZeileNeu(34) As String
            Dim Index As Integer
            Dim currentFont = rtb_MonitorReceived.SelectionFont

            ' Hole Inhalt der RTB als String-Array

            Zeile = rtb_MonitorReceived.Lines

            ' Wenn mehr als 34 Strings vorhanden sind dann lösche die ersten beiden Strings
            ' und sortiere die anderen zwei Stellen vorher im Array ein

            If Zeile.LongLength > 34 Then
                rtb_MonitorReceived.Clear()
                For Index = 0 To 31 Step 2
                    Zeile(Index) = Zeile(Index + 2)
                    Zeile(Index + 1) = Zeile(Index + 3)
                    rtb_MonitorReceived.SelectionFont = New Font(currentFont.FontFamily, currentFont.Size, FontStyle.Regular)
                    rtb_MonitorReceived.SelectionColor = Color.Black
                    rtb_MonitorReceived.AppendText(Zeile(Index) & vbCrLf)

                    rtb_MonitorReceived.SelectionFont = New Font(currentFont.FontFamily, currentFont.Size, FontStyle.Italic)
                    rtb_MonitorReceived.SelectionColor = Color.Blue
                    rtb_MonitorReceived.AppendText(Zeile(Index + 1) & vbCrLf)
                Next
            End If

            ' Fuege die beiden neuen Nachrichtenzeilen formatiert in die RTB ein

            rtb_MonitorReceived.SelectionFont = New Font(currentFont.FontFamily, currentFont.Size, FontStyle.Regular)
            rtb_MonitorReceived.SelectionColor = Color.Black
            rtb_MonitorReceived.AppendText([text1])

            rtb_MonitorReceived.SelectionFont = New Font(currentFont.FontFamily, currentFont.Size, FontStyle.Italic)
            rtb_MonitorReceived.SelectionColor = Color.Blue
            rtb_MonitorReceived.AppendText([text2])

            ' Daten in Logfile schreiben

            If COMRecFile IsNot Nothing Then
                Try
                    COMRecFileWr.Write([text1] & [text2])
                Catch ex As Exception

                End Try

            End If

        End If

    End Sub

    '**********************************************************************************************
    '*** COM - Threadshared - Allgemeine Sendefunktion je nach ausgewaehlter Schnittstelle      ***
    '**********************************************************************************************

    Public Sub COMMsgSend(ByVal Msg As String)
        Dim Index As Integer
        Dim sendBytes As Byte() = New Byte(12) {}
        Dim isSend As Boolean = False
        Dim MsgLine1 As String = ""
        Dim MsgLine2 As String = ""

        ' Ueber RS232 senden wenn aktiv

        If RS232Client.IsOpen Then

            Try
                ' Nachricht senden

                RS232Client.WriteLine(Msg)

                ' Nachricht efolgreich ´gesendet

                isSend = True

            Catch ex As Exception

                RS232Schliessen()

                MsgBox(Sprachen(i_MSGBOX_MAIN_RS232, i_MSGBOX_MAIN_RS232_2) & " (" & RS232Client.PortName & ") " &
                       Sprachen(i_MSGBOX_MAIN_RS232, i_MSGBOX_MAIN_RS232_3), vbOKOnly, Sprachen(i_MSGBOX_MAIN_RS232, i_MSGBOX_MAIN_RS232_1))

                Exit Sub

            End Try

        End If

        ' Ueber Bluetooth senden wenn aktiv

        If BTClient.IsOpen Then

            Try
                ' Nachricht senden

                BTClient.WriteLine(Msg)

                ' Nachricht efolgreich ´gesendet

                isSend = True

            Catch ex As Exception

                BTSchliessen()

                MsgBox(Sprachen(i_MSGBOX_MAIN_BT, i_MSGBOX_MAIN_BT_2) & " (" & BTClient.PortName & ") " &
                       Sprachen(i_MSGBOX_MAIN_BT, i_MSGBOX_MAIN_BT_3), vbOKOnly, Sprachen(i_MSGBOX_MAIN_BT, i_MSGBOX_MAIN_BT_1))

                Exit Sub

            End Try

        End If

        ' Ueber TCP senden wenn aktiv

        If TCPThreadIsActive = True Then

            Try
                ' Nachricht senden

                For Index = 0 To 12
                    sendBytes(Index) = CByte("&H" & Microsoft.VisualBasic.Mid(Msg, Index * 3 + 1, 2))
                Next

                TCPstream.Write(sendBytes, 0, sendBytes.Length)

                ' Nachricht efolgreich ´gesendet

                isSend = True

            Catch ex As Exception

                TCPSchliessen()

                MsgBox(Sprachen(i_MSGBOX_MAIN_TCP, i_MSGBOX_MAIN_TCP_2), vbOKOnly, Sprachen(i_MSGBOX_MAIN_TCP, i_MSGBOX_MAIN_TCP_1))

                Exit Sub

            End Try

        End If

        ' Nachricht erfolgreich gesendet?
        ' Dann aktualisiere Monitor

        If isSend Then

            ' Nachrichtenzaehler erhoehen

            COMMsgCount += 1

            ' Nachricht im Monitorfenster ausgeben

            COMBuildMsgMonitor(Msg, MsgLine1, MsgLine2)
            COMMonitorMsgReceived(Microsoft.VisualBasic.TimeString & Format(Now, ".fff") & " S " & MsgLine1, MsgLine2)

            MsgSend = Msg

        End If

    End Sub

    '**********************************************************************************************
    '*** COM - Sende eine Anfrage des Lese-/Schreibautomaten und scharfschalte den Watchdog     ***
    '*** (tm_ComTimeout) - ersetzt COMMsgSend() an allen Sendestellen von COMRefreshStart()/     ***
    '*** COMUpdateStart() und den Automatenzustaenden WR.1.1-WR.3/RD.1.1-RD.2 in                 ***
    '*** COMCmdReceive(). Merkt sich die Nachricht (LastIOMsg) fuer einen moeglichen Retry und    ***
    '*** setzt den Retry-Zaehler zurueck (ein neuer Automatenschritt bekommt sein eigenes         ***
    '*** volles Retry-Kontingent) - siehe Timer_ComTimeout_Tick in frmMain.vb.                    ***
    '**********************************************************************************************

    Public Sub COMMsgSendAndArm(Msg As String)

        COMMsgSend(Msg)

        LastIOMsg = Msg
        IOTimeoutRetryCount = 0

        tm_ComTimeout.Stop()
        tm_ComTimeout.Start()

    End Sub

    '**********************************************************************************************
    '*** COM - Formatiere die gesendete Nachricht fuer die Anzeige im Minitor                   ***
    '**********************************************************************************************
    Private Sub COMBuildMsgMonitor(ByVal Msg As String, ByRef MsgLine1 As String, ByRef MsgLine2 As String)
        Dim ID As String
        Dim PRIO As String
        Dim HASH As String
        Dim DLC As String
        Dim Dx As String
        Dim Speed As Double
        Dim D0, D1, D2, D3, D4, D5, D6, D7 As String

        PRIO = Microsoft.VisualBasic.Mid(Msg, MsgPRIO, 2)
        ID = Microsoft.VisualBasic.Mid(Msg, MsgID, 2)
        HASH = Microsoft.VisualBasic.Mid(Msg, MsgHashH, 2) & Microsoft.VisualBasic.Mid(Msg, MsgHashL, 2)
        DLC = Microsoft.VisualBasic.Mid(Msg, MsgDLC + 1, 1)
        D0 = Microsoft.VisualBasic.Mid(Msg, MsgD0, 2)
        D1 = Microsoft.VisualBasic.Mid(Msg, MsgD1, 2)
        D2 = Microsoft.VisualBasic.Mid(Msg, MsgD2, 2)
        D3 = Microsoft.VisualBasic.Mid(Msg, MsgD3, 2)
        D4 = Microsoft.VisualBasic.Mid(Msg, MsgD4, 2)
        D5 = Microsoft.VisualBasic.Mid(Msg, MsgD5, 2)
        D6 = Microsoft.VisualBasic.Mid(Msg, MsgD6, 2)
        D7 = Microsoft.VisualBasic.Mid(Msg, MsgD7, 2)

        If Val("&H" & DLC) = 0 Then
            Dx = " -"
        Else
            Dx = Microsoft.VisualBasic.Mid(Msg, 15, 3 + 3 * (Val("&H" & DLC) - 1))
        End If

        MsgLine1 = "PRIO: " & PRIO & " ID: " & ID & " HASH: " & HASH & " DLC: " & DLC & " DATEN:" & Dx & vbCrLf

        If Val("&H" & ID) < NachrichtenMax Then

            Try
                MsgLine2 &= "             > " & Nachrichten(Val("&H" & ID))
            Catch ex As Exception
                MsgLine2 &= "             > Unknown Command"
            End Try

            ' Systembefehle

            If Val("&H" & ID) = 0 Or Val("&H" & ID) = 1 Then

                ' Loc-ID

                MsgLine2 &= "0x" & D0 & D1 & D2 & D3 & " "

                ' Sub-Cmd

                If Val("&H" & D4) < NachrichtenSystemBefehleMax Then
                    MsgLine2 &= NachrichtenSystemBefehle(Val("&H" & D4))
                Else
                    If Val("&H" & D4) = &H30 Then MsgLine2 &= NachrichtenSystemBefehle0x30
                    If Val("&H" & D4) = &H80 Then MsgLine2 &= NachrichtenSystemBefehle0x80
                End If

                ' Sub-Cmd-Werte

                If Val("&H" & D4) = &H5 Or
                   Val("&H" & D4) = &H8 Or
                   Val("&H" & D4) = &HA Then
                    MsgLine2 &= " W: 0x" & D5
                End If

                If Val("&H" & D4) = &H6 Or
                   Val("&H" & D4) = &H7 Or
                   Val("&H" & D4) = &H9 Then
                    MsgLine2 &= " W: " & "0x" & D5 & D6
                End If

                If Val("&H" & D4) = &HB Then
                    If DLC = "07" Then MsgLine2 &= " W: " & D5
                    If DLC = "08" Then MsgLine2 &= " W: " & "0x" & D5 & D6
                End If

                If Val("&H" & D4) = &HC Then
                    If DLC = "07" Then MsgLine2 &= " W: " & "0x" & D5 & D6
                End If

            End If

            ' Lok Discovery

            If Val("&H" & ID) = 2 Or Val("&H" & ID) = 3 Then

                If DLC = "1" Then MsgLine2 &= "Protokoll-Kennung: 0x" & D0

                If DLC = "5" Then
                    MsgLine2 &= "Loc-ID: 0x" & D0 & D1 & D2 & D3 & " "
                    MsgLine2 &= "Protokoll-Kennung: 0x" & D4
                End If

                If DLC = "6" Then
                    MsgLine2 &= "Loc-ID: 0x" & D0 & D1 & D2 & D3 & " "
                    MsgLine2 &= "Range: 0x" & D4 & " "
                    MsgLine2 &= "ASK: 0x" & D5
                End If

            End If

            ' MFX Bind

            If Val("&H" & ID) = 4 Or Val("&H" & ID) = 5 Then
                MsgLine2 &= "MFX-UID: 0x" & D0 & D1 & D2 & D3 & " "
                MsgLine2 &= "MFX-SID: 0x" & D4 & D5

            End If

            ' MFX Verify

            If Val("&H" & ID) = 6 Or Val("&H" & ID) = 7 Then
                MsgLine2 &= "MFX-UID: 0x" & D0 & D1 & D2 & D3 & " "
                MsgLine2 &= "MFX-SID: 0x" & D4 & D5
                If DLC = "07" Then
                    MsgLine2 &= " ASK: " & D6
                End If
            End If

            ' Lok Geschwindigkeit

            If Val("&H" & ID) = 8 Or Val("&H" & ID) = 9 Then
                If D2 = "00" Then
                    MsgLine2 &= "MM-Lok Adresse: " & Val("&H" & D3)
                End If
                If D2 = "08" Then
                    MsgLine2 &= "SX-Lok Adresse: " & Val("&H" & D3)
                End If
                If D2 = "40" Then
                    MsgLine2 &= "MFX-Lok Adresse: " & Val("&H" & D3)
                End If
                If D2 = "C0" Then
                    MsgLine2 &= "DCC-Lok Adresse: " & Val("&H" & D3)
                End If
                If DLC = "6" Then
                    Speed = Math.Round((Val("&H" & D4) * 256 + Val("&H" & D5)) / 10, 1)
                    MsgLine2 &= " Geschwindigkeit: " & CStr(Speed) & "%"
                End If
            End If

            ' Lok Richtung

            If Val("&H" & ID) = 10 Or Val("&H" & ID) = 11 Then
                If D2 = "00" Then
                    MsgLine2 &= "MM-Lok Adresse: " & Val("&H" & D3)
                End If
                If D2 = "08" Then
                    MsgLine2 &= "SX-Lok Adresse: " & Val("&H" & D3)
                End If
                If D2 = "40" Then
                    MsgLine2 &= "MFX-Lok Adresse: " & Val("&H" & D3)
                End If
                If D2 = "C0" Then
                    MsgLine2 &= "DCC-Lok Adresse: " & Val("&H" & D3)
                End If
                If DLC = "5" Then
                    MsgLine2 &= " Richtung: "
                    If D4 = "01" Then
                        MsgLine2 &= "vorwärts"
                    Else
                        If D4 = "02" Then
                            MsgLine2 &= "rückwärts"
                        Else
                            If D4 = "03" Then
                                MsgLine2 &= "umschalten"
                            Else
                                MsgLine2 &= "bleibt"
                            End If
                        End If
                    End If
                End If
            End If

            ' Lok Funktion

            If Val("&H" & ID) = 12 Or Val("&H" & ID) = 13 Then
                If D2 = "00" Then
                    MsgLine2 &= "MM-Lok Adresse: " & Val("&H" & D3)
                End If
                If D2 = "08" Then
                    MsgLine2 &= "SX-Lok Adresse: " & Val("&H" & D3)
                End If
                If D2 = "40" Then
                    MsgLine2 &= "MFX-Lok Adresse: " & Val("&H" & D3)
                End If
                If D2 = "C0" Then
                    MsgLine2 &= "DCC-Lok Adresse: " & Val("&H" & D3)
                End If
                MsgLine2 &= " Funktion: " & Val("&H" & D4)
                If DLC = "6" Or DLC = "8" Then MsgLine2 &= " W: 0x" & D5
                If DLC = "8" Then MsgLine2 &= " SW: 0x" & D6 & D7
            End If

            ' Read Config

            If Val("&H" & ID) = 14 Or Val("&H" & ID) = 15 Then
                MsgLine2 &= "Loc-ID: 0x" & D0 & D1 & D2 & D3 & " CV: 0x" & D4 & D5
                If DLC = "7" Then MsgLine2 &= " W: 0x" & D6
            End If

            ' Write Config

            If Val("&H" & ID) = 16 Or Val("&H" & ID) = 17 Then
                MsgLine2 &= "Loc-ID: 0x" & D0 & D1 & D2 & D3 & " CV: 0x" & D4 & D5 & " W1: 0x" & D6 & " W1: 0x" & D7
            End If

            ' Zubehoer schalten

            If Val("&H" & ID) = &H16 Or Val("&H" & ID) = &H17 Then
                If D2 <> "18" Then
                    MsgLine2 &= "Zubehör Schalten Adr: 0x" & D0 & D1 & D2 & D3 & " Pos: " & Val("&H" & D4) & " Str: " & Val("&H" & D5)
                    If DLC = "8" Then MsgLine2 &= " SW: " & Val("&H" & D6 & D7)
                Else
                    MsgLine2 &= "MBCAN Sub-Cmd: "
                    If Val("&H" & D3) <= NachrichtenMBCANMax Then MsgLine2 &= NachrichtenMBCAN(Val("&H" & D3))
                End If

            End If

            ' S88 Polling

            If Val("&H" & ID) = &H20 Or Val("&H" & ID) = &H21 Then
                MsgLine2 &= "GUiD: 0x" & D0 & D1 & D2 & D3 & " "
                If DLC = "5" Then MsgLine2 &= "Mod-Anzahl: " & Val("&H" & D4)
                If DLC = "7" Then MsgLine2 &= "Mod-Nr: " & Val("&H" & D4) & " Zustand: " & Val("&H" & D5)

            End If

            ' Rueckmelde Event

            If Val("&H" & ID) = &H22 Or Val("&H" & ID) = &H23 Then
                MsgLine2 &= "GK: 0x" & D0 & D1 & " KK: 0x" & D2 & D3 & " "
                If DLC = "5" Then MsgLine2 &= "Parameter: 0x" & D4
                If DLC = "8" Then MsgLine2 &= "Z(a): " & Val("&H" & D4) & " Z(n): " & Val("&H" & D5) & " Zeit: " & Val("&H" & D6 & D7) & " ms"
            End If

            ' Ping

            If Val("&H" & ID) = &H30 Or Val("&H" & ID) = &H31 Then
                If DLC <> "0" Then MsgLine2 &= "GUiD: 0x" & D0 & D1 & D2 & D3 & " SW: " & Val("&H" & D4) & "." & Val("&H" & D5) & " GK: 0x" & D6 & D7
            End If


            MsgLine2 &= vbCrLf

        Else

            MsgLine2 &= "               Nicht implementiert" & vbCrLf

        End If



    End Sub

    '**********************************************************************************************
    '*** COM - Bestimme den Spannungsversorgungstyp                                             ***
    '**********************************************************************************************
    Public Function COMGetPlugged(ByVal Plugged As String) As String

        COMGetPlugged = ""

        If Plugged = "00" Then COMGetPlugged = Sprachen(i_MAIN, i_COMGetPlugged_1) '"MBC-Bus-Versorgung"
        If Plugged = "01" Then COMGetPlugged = Sprachen(i_MAIN, i_COMGetPlugged_2) '"Steckernetzteil"

    End Function


    '**********************************************************************************************
    '*** COM - Auswertung s88-Stellung                                                          ***
    '**********************************************************************************************

    Private Sub COMs88Receive()

        Dim S88Modul As Integer
        Dim S88 As Integer
        Dim S88Stellung As Integer
        Dim S88Pin As Integer

        ' Bestimme neue Stellung des S88-Pin

        S88Stellung = Val("&H" + Befehl.D7)

        ' Bestimme betroffenes Modul

        S88Modul = Val("&H" + Befehl.D4) << 8

        S88Modul += Val("&H" + Befehl.D5)

        ' Wenn betroffenes Modul gleich dem aktiven Modul ist dann aendere die Anzeige im Modulbild

        If S88Modul = Val(Register_Get_S88ModNr(AktTyp, AktNr)) Then

            If (AktTyp > 0) And (AktNr > 0) Then

                If MBC.ModulTyp(AktTyp).Bezeichner = "mbc-88" Then
                    S88Pin = Val("&H" + Befehl.D6) - 8 * MBC.ModulTyp(AktTyp).Modul(AktNr).S88ModHalbNr
                Else
                    S88Pin = Val("&H" + Befehl.D6)
                End If

                S88 = Val(Register_Get_S88Value(AktTyp, AktNr))

                If S88Stellung = 0 Then
                    S88 = (S88 And Not (2 ^ S88Pin)) And &HFFFF
                Else
                    S88 = (S88 Or 2 ^ S88Pin) And &HFFFF
                End If

                Register_Set_S88Value(S88, AktTyp, AktNr)

                Register_Modul_Aktorstellung_Show()

            End If

        End If

    End Sub

    '**********************************************************************************************
    '*** COM - Auswertung Aktor-Stellung                                                        ***
    '**********************************************************************************************

    Private Sub COMMMReceive(AND1 As Integer, OR1 As Integer, OR2 As Integer)

        Dim MMInt As Integer

        MMInt = Val(Register_Get_MMValue(AktTyp, AktNr))

        If (Befehl.D4 = "01") Then
            Register_Set_MMValue((MMInt And AND1) Or OR1, AktTyp, AktNr)
        Else
            Register_Set_MMValue((MMInt And AND1) Or OR2, AktTyp, AktNr)
        End If

    End Sub


    '**********************************************************************************************
    '*** COM - Werte Befehlsnachricht von den Modulen aus                                       ***
    '**********************************************************************************************

    Public Sub COMCmdReceive()

        Dim TxMsgReg As String
        Static SerNr As String
        Static Typ As String
        Static GUiD As String
        Static ArtNum As String
        Static SW As String
        Static HW As String
        Static Name As String
        Dim h As String
        Dim Index_High As String
        Dim Index_Low As String
        Dim Index As Integer
        Dim TypIndex As Integer
        Dim KnownIndex As Integer



        '******************************************************************************************

        ' Der empfangene Befehl ist ein Schaltkommando (0x0B / &HB)
        ' Nur dann werden die weiteren Daten ausgewertet. Die gesamte Kommunikation zwischen dem PC und
        ' den modulen erfolgt verpackt in einem Schaltkommande ab der Adresse &H1800, dem durch Maerklin freigegebenen Adressbereich
        ' der Loc-ID fuer Privatanwender und Clubs

        If (Befehl.Command = "&HB") Then

            '**************************************************************************************
            '*** MELDEN START                                                                   ***
            '**************************************************************************************

            ' Der empfangene Befehl ist eine S88-Stellungsmeldung
            ' Dann werte diese aus und stelle die aktuelle Rueckmeldung im Modulbild dar wenn es sich um das betroffene Modul handelt
            ' Ansonsten speichere die Rueckmeldung in der Moduldatenbank beim betroffenen Modul ab
            ' Danach verlassse die Funktion da Befehl verarbeitet wurde

            If (Befehl.ID = B_MD_S88) Then

                If (AktTyp > 0) And (AktNr > 0) Then

                    If MBC.ModulTyp(AktTyp).Modul(AktNr).Status = IsComplete Then COMs88Receive()

                End If
                Exit Sub
            End If ' If (Befehl.ID = B_MD_S88)

            '**************************************************************************************
            '*** MELDEN ENDE                                                                    ***
            '**************************************************************************************

            '------------------------------------------------------------------------------

            '**************************************************************************************
            '*** SCHALTEN START                                                                 ***
            '**************************************************************************************

            ' Stellungsmeldung Aktoren im Bereich 0x3000 bis 0x33ff
            ' Dann werte diese aus und stelle die aktuelle Rueckmeldung im Modulbild dar wenn es sich um das betroffene Modul handelt
            ' Ansonsten speichere die Rueckmeldung in der Moduldatenbank beim betroffenen Modul ab
            ' Danach verlassse die Funktion da Befehl verarbeitet wurde

            If (Befehl.ID = Val(Register_Get_MMAddr(0, AktTyp, AktNr))) Then
                If (AktTyp > 0) And (AktNr > 0) Then
                    If MBC.ModulTyp(AktTyp).Modul(AktNr).Status < IsMissing Then
                        COMMMReceive(&HFC, &H1, &H2)
                        Register_Modul_Aktorstellung_Show()
                    End If
                End If

                Exit Sub

            End If '  If (Befehl.ID = ModulGlobal(AktTyp,AktNr).MMAddrInt)

            If (Befehl.ID = Val(Register_Get_MMAddr(1, AktTyp, AktNr))) Then

                If (AktTyp > 0) And (AktNr > 0) Then
                    If MBC.ModulTyp(AktTyp).Modul(AktNr).Status < IsMissing Then
                        COMMMReceive(&HF3, &H4, &H8)
                        Register_Modul_Aktorstellung_Show()
                    End If
                End If

                Exit Sub

            End If ' If (Befehl.ID = ModulGlobal(AktTyp,AktNr).MMAddrInt + 1)

            If (Befehl.ID = Val(Register_Get_MMAddr(2, AktTyp, AktNr))) Then

                If (AktTyp > 0) And (AktNr > 0) Then
                    If MBC.ModulTyp(AktTyp).Modul(AktNr).Status < IsMissing Then
                        COMMMReceive(&HCF, &H10, &H20)
                        Register_Modul_Aktorstellung_Show()
                    End If
                End If

                Exit Sub

            End If ' If (Befehl.ID = ModulGlobal(AktTyp,AktNr).MMAddrInt + 2)

            If (Befehl.ID = Val(Register_Get_MMAddr(3, AktTyp, AktNr))) Then

                If (AktTyp > 0) And (AktNr > 0) Then
                    If MBC.ModulTyp(AktTyp).Modul(AktNr).Status < IsMissing Then
                        COMMMReceive(&H3F, &H40, &H80)
                        Register_Modul_Aktorstellung_Show()
                    End If
                End If

                Exit Sub

            End If ' If (Befehl.ID = ModulGlobal(AktTyp,AktNr).MMAddrInt + 3)

            '**************************************************************************************
            '*** SCHALTEN ENDE                                                                  ***
            '**************************************************************************************

            '------------------------------------------------------------------------------

            '**************************************************************************************
            '*** ANMELDUNG START                                                                ***
            '**************************************************************************************

            ' Neuanmeldenachricht empfangen
            ' Dann starte den Neuanmeldeautomaten im PC und im Modul und hole den Infoblock vom meldenden Modul ab

            If (Befehl.ID = B_MD_NEU_DATA) Then

                ' Bereite Antwort an das Modul vor

                TxMsgReg = MsgNeuanmeldekanal

                ' Antworte jeweils auf die Modulmeldungen mit den empfangenen Daten
                ' Damit wird sichergestellt, dass auch nur das Modul die Daten weiter uebertraegt die 
                ' der PC auch zuruecksendet, also das Modul und der PC synchron im Neuanmeldeautomaten laufen

                ' Seriennummer empfangen und formatieren - insgesamt 8 Bytes

                If ((Befehl.Count = 1) And (FLAG_Neu_Busy = False)) Then
                    h = FormHEX("0", 2, Befehl.Count)
                    TxMsgReg = MsgNeuanmeldekanal & h & " " & Befehl.D7
                    SerNr = "0x" & Befehl.D7
                    COMMsgSend(TxMsgReg)
                    FLAG_Neu_Busy = True

                    ' Verlassse die Funktion da Befehl verarbeitet wurde

                    Exit Sub

                End If

                If ((Befehl.Count > 1) And (Befehl.Count < 9) And (FLAG_Neu_Busy = True)) Then
                    h = FormHEX("0", 2, Befehl.Count)
                    TxMsgReg = MsgNeuanmeldekanal & h & " " & Befehl.D7
                    SerNr &= Befehl.D7
                    COMMsgSend(TxMsgReg)

                    ' Verlassse die Funktion da Befehl verarbeitet wurde

                    Exit Sub

                End If

                ' Artikelnummer empfangen und formatieren - insgesamt 4 Bytes

                If ((Befehl.Count = 9) And (FLAG_Neu_Busy = True)) Then
                    h = FormHEX("0", 2, Befehl.Count)
                    TxMsgReg = MsgNeuanmeldekanal & h & " " & Befehl.D7
                    ArtNum = Microsoft.VisualBasic.Right(Befehl.D7, 1)
                    GUiD = Befehl.D7
                    COMMsgSend(TxMsgReg)

                    ' Verlassse die Funktion da Befehl verarbeitet wurde

                    Exit Sub

                End If

                If ((Befehl.Count > 9) And (Befehl.Count < 13) And (FLAG_Neu_Busy = True)) Then
                    h = FormHEX("0", 2, Befehl.Count)
                    TxMsgReg = MsgNeuanmeldekanal & h & " " & Befehl.D7
                    ArtNum &= Microsoft.VisualBasic.Right(Befehl.D7, 1)
                    GUiD &= Befehl.D7
                    COMMsgSend(TxMsgReg)

                    ' Verlassse die Funktion da Befehl verarbeitet wurde

                    Exit Sub

                End If

                ' Softwareversion empfangen und formatieren - insgesamt 3 Bytes

                If ((Befehl.Count = 13) And (FLAG_Neu_Busy = True)) Then
                    h = FormHEX("0", 2, Befehl.Count)
                    TxMsgReg = MsgNeuanmeldekanal & h & " " & Befehl.D7
                    SW = Chr("&h" & Befehl.D7) & "."
                    COMMsgSend(TxMsgReg)

                    ' Verlassse die Funktion da Befehl verarbeitet wurde

                    Exit Sub

                End If

                If ((Befehl.Count > 13) And (Befehl.Count < 16) And (FLAG_Neu_Busy = True)) Then
                    h = FormHEX("0", 2, Befehl.Count)
                    TxMsgReg = MsgNeuanmeldekanal & h & " " & Befehl.D7
                    SW &= Chr("&h" & Befehl.D7)
                    COMMsgSend(TxMsgReg)

                    ' Verlassse die Funktion da Befehl verarbeitet wurde

                    Exit Sub

                End If

                ' Hardwareversion empfangen und formatieren - insgesamt 6 Bytes

                If ((Befehl.Count = 16) And (FLAG_Neu_Busy = True)) Then
                    h = FormHEX("0", 2, Befehl.Count)
                    TxMsgReg = MsgNeuanmeldekanal & h & " " & Befehl.D7
                    HW = Chr("&h" & Befehl.D7)
                    COMMsgSend(TxMsgReg)

                    ' Verlassse die Funktion da Befehl verarbeitet wurde

                    Exit Sub

                End If

                If ((Befehl.Count = 17) And (FLAG_Neu_Busy = True)) Then
                    h = FormHEX("0", 2, Befehl.Count)
                    TxMsgReg = MsgNeuanmeldekanal & h & " " & Befehl.D7
                    HW = HW & Chr("&h" & Befehl.D7) & "."
                    COMMsgSend(TxMsgReg)

                    ' Verlassse die Funktion da Befehl verarbeitet wurde

                    Exit Sub

                End If

                If ((Befehl.Count = 18) And (FLAG_Neu_Busy = True)) Then
                    h = FormHEX("0", 2, Befehl.Count)
                    TxMsgReg = MsgNeuanmeldekanal & h & " " & Befehl.D7
                    HW &= Chr("&h" & Befehl.D7)
                    COMMsgSend(TxMsgReg)

                    ' Verlassse die Funktion da Befehl verarbeitet wurde

                    Exit Sub

                End If

                If ((Befehl.Count = 19) And (FLAG_Neu_Busy = True)) Then
                    h = FormHEX("0", 2, Befehl.Count)
                    TxMsgReg = MsgNeuanmeldekanal & h & " " & Befehl.D7
                    HW = HW & Chr("&h" & Befehl.D7) & "."
                    COMMsgSend(TxMsgReg)

                    ' Verlassse die Funktion da Befehl verarbeitet wurde

                    Exit Sub

                End If

                If ((Befehl.Count > 19) And (Befehl.Count < 22) And (FLAG_Neu_Busy = True)) Then
                    h = FormHEX("0", 2, Befehl.Count)
                    TxMsgReg = MsgNeuanmeldekanal & h & " " & Befehl.D7
                    HW &= Chr("&h" & Befehl.D7)
                    COMMsgSend(TxMsgReg)

                    ' Verlassse die Funktion da Befehl verarbeitet wurde

                    Exit Sub

                End If

                ' Name empfangen und formatieren - insgesamt 20 Bytes

                If ((Befehl.Count = 22) And (FLAG_Neu_Busy = True)) Then
                    h = FormHEX("0", 2, Befehl.Count)
                    TxMsgReg = MsgNeuanmeldekanal & h & " " & Befehl.D7
                    If Befehl.D7 <> "81" Then Name = Chr("&h" & Befehl.D7)
                    COMMsgSend(TxMsgReg)

                    ' Verlassse die Funktion da Befehl verarbeitet wurde

                    Exit Sub

                End If

                If ((Befehl.Count > 22) And (Befehl.Count < 42) And (FLAG_Neu_Busy = True)) Then
                    h = FormHEX("0", 2, Befehl.Count)
                    TxMsgReg = MsgNeuanmeldekanal & h & " " & Befehl.D7
                    If Befehl.D7 <> "81" Then Name &= Chr("&h" & Befehl.D7)
                    COMMsgSend(TxMsgReg)

                    ' Verlassse die Funktion da Befehl verarbeitet wurde

                    Exit Sub

                End If

                ' Neuanmeldungsnachricht komplett empfangen da das meldende Modul alle angefragten Daten des Infoblocks
                ' gesendet hat. Der Neuanmeldeblock ist 1 Byte laenger als der Datenblock. Jetzt wird dem Modul eine 
                ' gueltige GUiD zugewiesen und die Daten werden in der Datenbank gespeichert

                If ((Befehl.Count = 42) And (FLAG_Neu_Busy = True)) Then

                    ' Vervollstaendige GUiD fuer die Anzeige
                    ' Neben dem CS2-Identifier, der fuer die Kommunikation mit der CS2
                    ' notwendig ist wird auch noch der Modultyp eingetragen
                    ' Der letzte Teil der GUiD besteht aus der lfd. Nr. und wird weiter unten
                    ' vergeben

                    GUiD = "0x" & CS2_Identifier & GUiD

                    ' Bestimme Modultyp

                    Typ = "mbc-" & Microsoft.VisualBasic.Left(ArtNum, 2)

                    TypIndex = Strukturbaum_GetTypIndex(Typ)

                    ' Falls der Modultyp nicht implementiert dann beende Prozedur

                    If (MBC.ModulTyp(TypIndex).TypIsActive = False) Then
                        FLAG_Neu_Busy = False
                        Exit Sub
                    End If

                    ' Modul schon in Liste?
                    ' Wenn ja dann Neuanmeldeflag zuruecksetzen

                    For Index = 1 To MBC.ModulTyp(TypIndex).Count
                        If (SerNr = MBC.ModulTyp(TypIndex).Modul(Index).SerNr) Then
                            FLAG_Neu_Busy = False
                            KnownIndex = Index
                            Name = MBC.ModulTyp(TypIndex).Modul(Index).Name
                            GUiD = MBC.ModulTyp(TypIndex).Modul(Index).GUiD
                            Exit For
                        End If
                    Next

                    ' Wenn neu und kein Interface, dann hole neuen Index und stelle GUiD zusammen
                    ' Interface ist es dann, wenn TypIndex = 1 ist

                    If (FLAG_Neu_Busy = True) And (MBC.ModulTyp(TypIndex).Count <= 256) Then 'And (TypIndex <> 1) Then

                        Index = MBC.ModulTyp(TypIndex).Count

                        ' Schaue nach, ob die maximale Anzahl der Module schon erreicht ist
                        ' Wenn nicht, dann weise neue GUiD zu
                        ' Und setze Modulplatz als gesetzt

                        If (Index <= MaxNr) Then

                            ' GUiD komplett vergeben mit lfd. Nr. Diese ist immer um den Wert 1 kleiner als die 
                            ' aktuelle Anzahl der Module eines Typen. Daher wird dieser erst spaeter erhoeht

                            GUiD = Microsoft.VisualBasic.Left(GUiD, 8)

                            ' Lasse mbc-80-Module nicht bei 0 sondern bei 1 starten

                            If (TypIndex = 1) Then
                                h = FormHEX("0", 2, Index + 1)
                            Else
                                h = FormHEX("0", 2, Index)
                            End If

                            GUiD &= h

                            ' Erhoehe den Index fuer die Anzahl der Module

                            MBC.ModulTyp(Typ).Add(Name)
                            KnownIndex = MBC.ModulTyp(Typ).Count
                            MBC.ModulTyp(Typ).Modul(KnownIndex).GUiD = GUiD

                        Else

                            MsgBox(Sprachen(i_MSGBOX_MAIN_MAX, i_MSGBOX_MAIN_MAX_1) & " " & Str(MaxNr) & " " &
                                   Sprachen(i_MSGBOX_MAIN_MAX, i_MSGBOX_MAIN_MAX_2), MsgBoxStyle.OkOnly, Sprachen(i_MSGBOX_MAIN_MAX, i_MSGBOX_MAIN_MAX_3))
                            FLAG_Neu_Busy = False
                            Exit Sub

                        End If

                    End If

                    ' Daten im Modularray speichern

                    MBC.ModulTyp(TypIndex).Modul(KnownIndex).Status = IsAlive
                    MBC.ModulTyp(TypIndex).Modul(KnownIndex).SerNr = SerNr
                    MBC.ModulTyp(TypIndex).Modul(KnownIndex).Name = Name
                    MBC.ModulTyp(TypIndex).Modul(KnownIndex).ArtNum = ArtNum
                    MBC.ModulTyp(TypIndex).Modul(KnownIndex).SW = SW
                    MBC.ModulTyp(TypIndex).Modul(KnownIndex).HW = HW
                    MBC.ModulTyp(TypIndex).Modul(KnownIndex).Plugged = "-"
                    MBC.ModulTyp(TypIndex).Modul(KnownIndex).Spannung = "0.0"
                    MBC.ModulTyp(TypIndex).Modul(KnownIndex).Temperatur = "0.0"

                    ' Fuer spaetere einfache Vergleiche sichere die GUiD auch als MsgText so wie sie auch in der RS232- oder UDP-
                    ' Nachricht am Ende auftaucht

                    MBC.ModulTyp(TypIndex).Modul(KnownIndex).MsgGUiD = Microsoft.VisualBasic.Mid(MBC.ModulTyp(TypIndex).Modul(KnownIndex).GUiD, 3, 2) & " " _
                                                        & Microsoft.VisualBasic.Mid(MBC.ModulTyp(TypIndex).Modul(KnownIndex).GUiD, 5, 2) & " " _
                                                        & Microsoft.VisualBasic.Mid(MBC.ModulTyp(TypIndex).Modul(KnownIndex).GUiD, 7, 2) & " " _
                                                        & Microsoft.VisualBasic.Mid(MBC.ModulTyp(TypIndex).Modul(KnownIndex).GUiD, 9, 2)


                    ' Trage Modul ein wenn es eine Neuanmedung ist und der Modultyp zulaessig

                    If (FLAG_Neu_Busy) Then Strukturbaum_MenueAddItem(TypIndex)

                    ' Damit ist der Neuanmeldeprozess beendet

                    FLAG_Neu_Busy = False

                    ' Die Modulnummer aus der GUiD muss noch an das Modul ubergeben werden
                    ' Dies wird in die letzte Rueckmeldung in der Message an der Stelle D5 an das Modul mitgegeben

                    TxMsgReg = Microsoft.VisualBasic.Left(MsgNeuanmeldekanal, 30)

                    If (TypIndex = 1) Then
                        h = FormHEX("0", 2, KnownIndex)
                    Else
                        h = FormHEX("0", 2, KnownIndex - 1)
                    End If


                    ' Baue Rueckmeldung an das Modul zusammen 

                    TxMsgReg = TxMsgReg & h & " "

                    h = FormHEX("0", 2, Befehl.Count)

                    TxMsgReg = TxMsgReg & h & " " & Befehl.D7

                    ' Jetzt wird mit der letzten Meldung der Neuanmeldeautomat im Modul beendet

                    COMMsgSend(TxMsgReg)

                End If

                ' Verlassse die Funktion da Befehl verarbeitet wurde

                Exit Sub

            End If ' If (Befehl.ID = B_MD_NEU_DATA)

            '**************************************************************************************
            '*** ANMELDUNG ENDE                                                                 ***
            '**************************************************************************************

            '------------------------------------------------------------------------------

            '**************************************************************************************
            '*** ALIVE START                                                                    ***
            '**************************************************************************************

            ' Achknowledge vom Modul empfangen zu Alive-Request

            If (Befehl.ID = B_MD_ALIVE) Then

                COMCmdReceiveAlive()

                ' Verlassse die Funktion da Befehl verarbeitet wurde

                Exit Sub

            End If

            '**************************************************************************************
            '*** ALIVE ENDE                                                                     ***
            '**************************************************************************************

            '------------------------------------------------------------------------------

            '**************************************************************************************
            '*** UPDATE START                                                                   ***
            '**************************************************************************************

            ' Achknowledge vom Modul empfangen zu Datenaustausch
            ' Hier startet ein Automat, der verschiedene Zustaende kennt

            ' Schreibzugriff: 
            '    1) FLAG_Write_Request    WR.1.1 und WR 1.2
            '    2) FLAG_Write_IO         WR.2
            '    3) FLAG_Write_Program    WR.3
            '    4) FLAG_Write_Refresh    WR.4

            ' Lesezugriff: 
            '    1) FLAG_Read_Request     RD.1.1 und RD 1.2
            '    2) FLAG_Read_IO          RD.2

            ' Achknowledge vom Modul empfangen zu Read/Write-Request

            If (Befehl.ID = B_MD_ARRAY) Then

                '+++ Update-Automat - Schritt WR.1.1 ++++++++++++++++++++++++++++++++++++++++++++++

                ' Modul hat auf Zugriffsanforderung geantwortet
                ' Ausloeser war eine Aenderung?
                ' Dann sende Modul die Info dass es sich um ein Update handelt 

                If (FLAG_Write_Request) Then

                    ' Signalisiere an das Modul den Beginn eines Updates

                    TxMsgReg = MsgIOAct & "01 FF " & MBC_WRITE_Msg & " 01"

                    COMMsgSendAndArm(TxMsgReg)

                    ' Verlassse die Funktion da Befehl verarbeitet wurde

                    Exit Sub

                End If ' If FLAG_Write_Request

                '+++ Update-Automat - Schritt RD.1.1 ++++++++++++++++++++++++++++++++++++++++++++++

                ' Modul hat auf Zugriffsanforderung geantwortet
                ' Ausloeser war eine Refreshanfrage?
                ' Dann sende Abfrage 

                If (FLAG_Read_Request) Then

                    ' Signalisiere an das Modul den Beginn eines Lesezyklus

                    TxMsgReg = MsgIOAct & "01 FF " & MBC_READ_Msg & " 01"

                    COMMsgSendAndArm(TxMsgReg)

                    ' Verlassse die Funktion da Befehl verarbeitet wurde

                    Exit Sub

                End If ' If FLAG_Read_Request

                ' Verlassse die Funktion da Befehl verarbeitet wurde

                Exit Sub

            End If ' If (Befehl.ID = B_MD_ARRAY)

            ' Antwort vom Modul empfangen zum Datenaustausch

            If (Befehl.ID = B_MD_ARRAY_DATA) Then

                '+++ Update-Automat - Schritt WR.1.2 ++++++++++++++++++++++++++++++++++++++++++++++

                ' Modul hat die Updateanfrage quittiert
                ' Dann fange an die Daten zu senden 

                If (FLAG_Write_Request) Then

                    ' Setze den Laufindex auf den Anfang des Uebergabeblocks

                    IOIndex = IOStart

                    ' Signalisiere, dass ein Update stattfindet

                    FLAG_Write_IO = True

                    ' Nehme den Request und alle anderen FLAGS zurueck

                    FLAG_Write_Request = False
                    FLAG_Write_Program = False
                    FLAG_Write_Refresh = False
                    FLAG_Read_Request = False
                    FLAG_Read_IO = False

                    ' Schreibe ersten Wert an das Modul

                    h = FormHEX("0", 4, IOIndex)

                    Index_High = Microsoft.VisualBasic.Left(h, 2)
                    Index_Low = Microsoft.VisualBasic.Right(h, 2)

                    ' Bestimme zu schreibenden Wert

                    TxMsgReg = MsgIOAct & "01 " & Index_High & " " & Index_Low & " " & MBC.ModulTyp(AktTyp).Modul(AktNr).SYS_ARRAY(IOIndex)

                    COMMsgSendAndArm(TxMsgReg)

                    ' Verlassse die Funktion da Befehl verarbeitet wurde

                    Exit Sub

                End If ' If (FLAG_Write_Request)

                '+++ Update-Automat - Schritt WR.2 ++++++++++++++++++++++++++++++++++++++++++++++++

                ' Modul hat die gesendete Daten quittiert und der Automat steht auf Update
                ' Dann sende naechste Daten oder beende die Uebertragung wenn die Blocklaenge erreicht ist 
                ' und schreibe die Daten in das EEPROM falls erforderlich

                If (FLAG_Write_IO) Then

                    ' Erhoehe den Laufindex

                    IOIndex += 1

                    ' Sende naechsten Wert an das Modul, es sei denn
                    ' das Blockende ist erreicht. Dann sende Abschlussnachricht an das Modul

                    If (IOIndex > IOMax) Then

                        ' Updatezyklus abschließen

                        FLAG_Write_IO = False

                        ' Schreibe Parameter in das interne / Boot-EEPROM

                        TxMsgReg = MsgIOEEPROM & IOEEPROM

                        COMMsgSendAndArm(TxMsgReg)

                        ' Flag setzen, dass das EEPROM programmiert wird

                        FLAG_Write_Program = True

                        ' Statusanzeige aktualisieren

                        stp_Refresh.Text = Sprachen(i_MAIN, i_stp_Refresh_update_ready)
                        spb_Refresh.Value = 0
                        spb_Refresh.Enabled = False

                        If AktTyp = 1 Then
                            If IOStart = MBC.ModulTyp("mbc-80").Modul(AktNr).BTNameSpeicher Then COMUpdateStop()
                            If IOStart = MBC.ModulTyp("mbc-80").Modul(AktNr).BTPWSpeicher Then COMUpdateStop()
                        End If

                    Else

                        'Bestimme den naechsten zu sendenen Wert

                        h = FormHEX("0", 4, IOIndex)

                        Index_High = Microsoft.VisualBasic.Left(h, 2)
                        Index_Low = Microsoft.VisualBasic.Right(h, 2)

                        ' Bestimme zu schreibenden Wert

                        TxMsgReg = MsgIOAct & "01 " & Index_High & " " & Index_Low & " " & MBC.ModulTyp(AktTyp).Modul(AktNr).SYS_ARRAY(IOIndex)

                        COMMsgSendAndArm(TxMsgReg)

                        ' Stripangabe aktualisieren

                        spb_Refresh.Increment(1)

                    End If ' If (IOIndex > MsgIOMax)

                    ' Verlassse die Funktion da Befehl verarbeitet wurde

                    Exit Sub

                End If ' If (FLAG_Write_IO)

                '+++ Update-Automat - Schritt WR.3 ++++++++++++++++++++++++++++++++++++++++++++++++

                ' Modul hat die EEPROM-Programmierung quittiert und der Updateprozess ist abgeschlossen
                ' Dann beende den Datentransfer und fordere einen Refresh ein wenn das Modul
                ' geantwortet hat, es sei denn das die Bluetooth-Eigenschaften geändert wurden.
                ' Dann stoppe den Update-Prozess

                If (FLAG_Write_Program) Then

                    ' Flag zuruecksetzen, dass das EEPROM programmiert wird

                    FLAG_Write_Program = False

                    ' Flag setzen, dass der Prozess abgeschlossen ist

                    FLAG_Write_Refresh = True

                    ' Signalisiere an das Modul das Ende eines Updates

                    TxMsgReg = MsgIOAct & "01 FF " & MBC_WRITE_Msg & " 00"

                    COMMsgSendAndArm(TxMsgReg)

                    ' Verlassse die Funktion da Befehl verarbeitet wurde

                    Exit Sub

                End If ' If (FLAG_Write_Program)

                '+++ Update-Automat - Schritt WR.4 ++++++++++++++++++++++++++++++++++++++++++++++++

                ' Modul hat die Programmierung erfolgreich abgeschlossen und den Updatezyklus beendet
                ' Dann lese geschriebene Werte zurueck und zeige sie an

                If (FLAG_Write_Refresh) Then

                    FLAG_Write_Refresh = False

                    COMRefreshStart(IOStart, IOMax)

                    ' Verlassse die Funktion da Befehl verarbeitet wurde

                    Exit Sub

                End If ' If (FLAG_Write_Refresh)

                '+++ Update-Automat - Schritt RD 1.2 ++++++++++++++++++++++++++++++++++++++++++++++

                ' Modul hat die Refreshanfrage quittiert
                ' Dann fange an die Daten einzusammeln 

                If (FLAG_Read_Request) Then

                    ' Setze den Laufindex auf den Anfang des Uebergabeblocks

                    IOIndex = IOStart

                    ' Signalisiere, dass ein Refreshzugriff stattfindet

                    FLAG_Read_IO = True

                    ' Nehme den Request und alle anderen FLAGS zurueck

                    FLAG_Read_Request = False
                    FLAG_Write_Request = False
                    FLAG_Write_IO = False
                    FLAG_Write_Program = False
                    FLAG_Write_Refresh = False

                    ' Fordere Modul auf den ersten Wert aus dem
                    ' Parameterblock zu uebertragen

                    h = FormHEX("0", 4, IOIndex)

                    Index_High = Microsoft.VisualBasic.Left(h, 2)
                    Index_Low = Microsoft.VisualBasic.Right(h, 2)

                    TxMsgReg = MsgIOAct & "00 " & Index_High & " " & Index_Low & " 00"

                    COMMsgSendAndArm(TxMsgReg)

                    ' Verlassse die Funktion da Befehl verarbeitet wurde

                    Exit Sub

                End If ' If (FLAG_Read_Request)

                '+++ Update-Automat - Schritt RD.2 ++++++++++++++++++++++++++++++++++++++++++++++++

                ' Modul hat die Refreshanfrage quittiert
                ' Dann uebernehme Daten und fordere naechste Daten an oder beende
                ' den Refreshzyklus wenn die Blocklaenge erreicht ist

                If (FLAG_Read_IO) Then

                    ' Pruefe, ob die Antwort tatsaechlich zum aktuell angefragten Index gehoert.
                    ' Eine verspaetete/doppelte Antwort (z.B. weil die urspruengliche Antwort erst
                    ' nach einem Watchdog-Retry eintrifft) darf NICHT uebernommen werden, sonst
                    ' landet der Wert im falschen Slot und das Systemarray wird ab dort
                    ' verschoben/verfaelscht dargestellt.

                    h = FormHEX("0", 4, IOIndex)

                    If (Befehl.D5 <> Microsoft.VisualBasic.Left(h, 2)) OrElse
                       (Befehl.D6 <> Microsoft.VisualBasic.Right(h, 2)) Then

                        ' Nicht die erwartete Antwort - verwerfen. Der Watchdog fuer den aktuell
                        ' offenen Index laeuft unveraendert weiter (wurde beim Senden der Anfrage
                        ' bzw. beim letzten Retry scharfgeschaltet).

                        Exit Sub

                    End If

                    ' Uebernehme Datum aus der Nachricht in das Systemarray

                    MBC.ModulTyp(AktTyp).Modul(AktNr).SYS_ARRAY(IOIndex) = Befehl.D7

                    ' Erhoehe den Laufindex

                    IOIndex += 1

                    ' Ueberspringe den BOOTLOADER - Sektor

                    If IOIndex = MBC_BOOT_START Then IOIndex = MBC_BOOT_ENDE + 1

                    ' Pruefe auf Ende des angeforderten Lesebereiches

                    If (IOIndex > IOMax) Then

                        ' Beende den Refreshzyklus

                        FLAG_Read_IO = False

                        ' Signalisiere an das Modul das Ende eines Lesezyklus

                        TxMsgReg = MsgIOAct & "01 FF " & MBC_READ_Msg & " 00"

                        COMMsgSendAndArm(TxMsgReg)

                        ' Statusanzeige aktualisieren

                        stp_Refresh.Text = Sprachen(i_MAIN, i_stp_Refresh_ready)
                        spb_Refresh.Value = 0
                        spb_Refresh.Enabled = False

                        ' Uebernehme die neuen Moduldaten

                        COMRefreshComplete()

                    Else

                        ' Fordere das Modul auf, den naechsten Wert zu senden

                        h = FormHEX("0", 4, IOIndex)

                        Index_High = Microsoft.VisualBasic.Left(h, 2)
                        Index_Low = Microsoft.VisualBasic.Right(h, 2)

                        TxMsgReg = MsgIOAct & "00 " & Index_High & " " & Index_Low & " 00"

                        COMMsgSendAndArm(TxMsgReg)

                        ' Stripangabe aktualisieren

                        spb_Refresh.Increment(1)

                    End If ' If (IOIndex = RefreshMaxIndexAllg)

                    ' Verlassse die Funktion da Befehl verarbeitet wurde

                    Exit Sub

                End If ' If (FLAG_Read_IO)

            End If ' If (Befehl.ID = B_MD_ARRAY_DATA)

            '**************************************************************************************
            '*** UPDATE ENDE                                                                    ***
            '**************************************************************************************

            '------------------------------------------------------------------------------

            '**************************************************************************************
            '*** UPGRADE START                                                                  ***
            '**************************************************************************************

            ' Achknowledge vom Modul empfangen zu Upgrade
            ' Hier startet ein Automat, der verschiedene Zustaende kennt
            '    1) FLAG_Upgrade_Request
            '    2) FLAG_Upgrade_IO

            '+++ Upgrade-Automat - Schritt 1 ++++++++++++++++++++++++++++++++++++++++++++++++++++++

            If (Befehl.ID = B_MD_UPGRADE) Then

                ' Modul hat die Updateanfrage quittiert
                ' Dann fange an die Daten zu senden 

                If (FLAG_Upgrade_Request) Then

                    ' Setze den Laufindex auf den Anfang des Uebergabeblocks

                    IOIndex = IOStart

                    ' Setze den Paketindex auf Start
                    ' Der Paketindex ersetzt den HASH in der CAN-Nachricht

                    UpgradeHash = &H301

                    ' Signalisiere, dass ein Update stattfindet

                    FLAG_Upgrade_IO = True

                    ' Nehme den Request und alle anderen FLAGS zurueck

                    FLAG_Upgrade_Request = False

                    ' Schreibe ersten Wert an das Modul

                    h = FormHEX("0", 4, UpgradeHash)

                    Index_High = Microsoft.VisualBasic.Left(h, 2)
                    Index_Low = Microsoft.VisualBasic.Right(h, 2)

                    ' Bestimme zu schreibenden Wert

                    TxMsgReg = "00 16 " & Index_High & " " & Index_Low & " 08 00 00 18 " & PC_UPGRADE_DATA & " "

                    TxMsgReg &= MBC.ModulTyp(AktTyp).Modul(AktNr).SYS_ARRAY(IOIndex) & " " &
                               MBC.ModulTyp(AktTyp).Modul(AktNr).SYS_ARRAY(IOIndex + 1) & " " &
                               MBC.ModulTyp(AktTyp).Modul(AktNr).SYS_ARRAY(IOIndex + 2) & " " &
                               MBC.ModulTyp(AktTyp).Modul(AktNr).SYS_ARRAY(IOIndex + 3)

                    COMMsgSend(TxMsgReg)

                    ' Verlassse die Funktion da Befehl verarbeitet wurde

                    Exit Sub

                End If ' If (FLAG_Write_Request)

            End If ' Befehl.ID = B_MD_UPGRADE

            '+++ Upgrade-Automat - Schritt 2 ++++++++++++++++++++++++++++++++++++++++++++++++++++++

            If (Befehl.ID = B_MD_UPGRADE_DATA) Then

                ' Modul hat die gesendete Daten quittiert und der Automat steht auf Update
                ' Dann sende naechste Daten oder beende die Uebertragung wenn die Blocklaenge erreicht ist 
                ' und schreibe die Daten in das EEPROM falls erforderlich

                If (FLAG_Upgrade_IO) Then

                    ' Erhoehe den Laufindex

                    IOIndex += 4

                    ' Da im HASH resp.Paketindex der Wert 0x0300 fest gesetzt ist
                    ' muss bei einem Ueberlauf des Index der Wert auf 0x1300 gesetzt werden
                    ' Damit bleibt die Konvention von Maerklin erhalten.

                    If UpgradeHash = &H3FF Then

                        UpgradeHash = &H1300

                    Else

                        UpgradeHash += 1

                    End If

                    ' Sende naechsten Wert an das Modul, es sei denn
                    ' das Blockende ist erreicht. Dann sende Abschlussnachricht an das Modul

                    If (UpgradeHash - &H300 = MBC_BOOT_PGMAX + 2) Then

                        ' Updatezyklus abschließen

                        FLAG_Upgrade_IO = False

                        If FLAG_Upgrade_SendingOtaHeader Then

                            ' Die MbcOta-Header-Page (RP2040/ESP32) wurde soeben quittiert -
                            ' der Upgrade-Vorgang ist jetzt wirklich vollstaendig abgeschlossen

                            FLAG_Upgrade_SendingOtaHeader = False

                            COMUpgradeComplete()

                        Else

                            UpgradePage += 1

                            ' Starte neue Uebertragung es sei denn, das Ende des
                            ' Firmwareupgrades ist erreicht.

                            If (UpgradePage <= UpgradePageMax) Then

                                COMUpgradeStart(IOStart, IOMax, UpgradePage)

                            ElseIf FLAG_Upgrade_NeedsOtaHeader Then

                                ' Alle Firmware-Pages sind uebertragen - fuer RP2040/ESP32
                                ' zusaetzlich die MbcOta-Header-Page (Magic/Laenge/CRC32) auf
                                ' die letzte Boot-Speicherchip-Page schreiben (siehe MbcOta.h)

                                FLAG_Upgrade_SendingOtaHeader = True

                                COMUpgradeStart(IOStart, IOMax, MBC_OTA_HEADER_PAGE)

                            Else

                                COMUpgradeComplete()

                            End If

                        End If

                    Else

                        ' Schreibe naechsten Wert an das Modul

                        h = FormHEX("0", 4, UpgradeHash)

                        Index_High = Microsoft.VisualBasic.Left(h, 2)
                        Index_Low = Microsoft.VisualBasic.Right(h, 2)

                        ' Bestimme zu schreibenden Wert

                        TxMsgReg = "00 16 " & Index_High & " " & Index_Low & " 08 00 00 18 " & PC_UPGRADE_DATA & " "

                        ' Wenn 64 Bytes uebertragen wurden dann sind nur noch 2 Bytes uebrig.
                        ' Diese werden dann um Dummys ergaenzt
                        ' Ansonsten sende die 4 Bytes aus dem System-Array

                        If (UpgradeHash - &H300 <= MBC_BOOT_PGMAX) Then

                            TxMsgReg &= MBC.ModulTyp(AktTyp).Modul(AktNr).SYS_ARRAY(IOIndex) & " " &
                                       MBC.ModulTyp(AktTyp).Modul(AktNr).SYS_ARRAY(IOIndex + 1) & " " &
                                       MBC.ModulTyp(AktTyp).Modul(AktNr).SYS_ARRAY(IOIndex + 2) & " " &
                                       MBC.ModulTyp(AktTyp).Modul(AktNr).SYS_ARRAY(IOIndex + 3)

                            COMMsgSend(TxMsgReg)

                            ' Stripangabe aktualisieren

                            spb_Refresh.Increment(4)

                        Else

                            TxMsgReg &= MBC.ModulTyp(AktTyp).Modul(AktNr).SYS_ARRAY(IOIndex) & " " &
                                       MBC.ModulTyp(AktTyp).Modul(AktNr).SYS_ARRAY(IOIndex + 1) &
                                        " FF FF"

                            COMMsgSend(TxMsgReg)

                            ' Stripangabe aktualisieren

                            spb_Refresh.Increment(4)

                        End If

                    End If ' If (IOIndex > MsgIOMax)

                    ' Verlassse die Funktion da Befehl verarbeitet wurde

                    Exit Sub

                End If ' If (FLAG_UPGRADE_IO)

            End If ' If (Befehl.ID = B_MD_UPGRADE_DATA)

            '**************************************************************************************
            '*** UPGRADE ENDE                                                                   ***
            '**************************************************************************************

        End If ' If (Befehl.Command = "&HB")

    End Sub

    '**********************************************************************************************
    '*** COM - Alive-Meldung des aktuelle Modul erhalten?                                       ***
    '**********************************************************************************************

    Private Sub COMCmdReceiveAlive()

        If FLAG_Alive_Request = True Then

            ' Entspricht die GUiD des meldenden Moduls der des angeforderten?
            ' Wenn ja, dann setze das Ack-Flag

            If ModulAliveGUiD = Microsoft.VisualBasic.Right(Befehl.Msg, 11) Then

                ' Modul mit der gesendeten GUiD ist noch da
                ' Mit Ack = True bleibt das Icon auf Haken

                FLAG_Alive_Ack = True

            End If

            ' Setze den Alive-Request zurueck

            FLAG_Alive_Request = False

            tm_Alive.Interval = 1


        End If

    End Sub

    '**********************************************************************************************
    '*** Funktionen fuer das Update von Modulenparametern                                       ***
    '**********************************************************************************************

    '**********************************************************************************************
    '*** COM - Update-Anfrage an das aktuelle Modul senden                                      ***
    '**********************************************************************************************

    Public Sub COMUpdateStart(Startindex As Integer, Stopindex As Integer, EEPROM As String)

        ' Sperre Eingabemoeglichkeiten

        Register_Modul_Edit_Enable(False)

        'Stoppe Alive-Timer

        tm_Alive.Stop()
        tm_Neu.Stop()

        ' Setze alle Flags und Indizes zurueck

        FLAG_Read_IO = False
        FLAG_Read_Request = False
        FLAG_Write_IO = False
        FLAG_Write_Request = False
        FLAG_Write_Program = False
        FLAG_Write_Refresh = False

        ' Signalisiere den Start des Updatezyklus

        stp_Refresh.Text = Sprachen(i_MAIN, i_stp_Refresh_update_start)
        stp_Alive.Text = Sprachen(i_MAIN, i_stp_Alive_None)
        spb_Refresh.Enabled = True
        spb_Refresh.Value = 0
        spb_Refresh.Maximum = Stopindex - Startindex + 10

        ' Sende initiale Zugriffsanfrage

        IOStart = Startindex
        IOMax = Stopindex
        IOEEPROM = EEPROM

        ' Sende Zugriffsanfrage

        COMMsgSendAndArm(MsgIORequest & MBC.ModulTyp(AktTyp).Modul(AktNr).MsgGUiD)

        ' Setze Update-Flag

        FLAG_Write_Request = True

    End Sub

    '**********************************************************************************************
    '*** COM - Update-Anfrage an das aktuelle Modul stoppen                                     ***
    '**********************************************************************************************

    Public Sub COMUpdateStop()

        ' Stripanzeige aktualisieren

        stp_Refresh.Text = Sprachen(i_MAIN, i_stp_Refresh_update_stop)
        spb_Refresh.Enabled = False
        spb_Refresh.Value = 0

        ' Flags zuruecksetzen

        FLAG_Read_IO = False
        FLAG_Read_Request = False
        FLAG_Write_IO = False
        FLAG_Write_Request = False
        FLAG_Write_Program = False
        FLAG_Write_Refresh = False

        ' Watchdog stoppen

        tm_ComTimeout.Stop()

        ' Gebe Eingabemoeglichkeiten frei

        Register_Modul_Edit_Enable(True)

        ' Alive-Timer wieder starten

        tm_Alive.Start()
        tm_Neu.Start()

    End Sub

    '**********************************************************************************************
    '*** Funktionen fuer das Upgrade von Modulen                                                ***
    '**********************************************************************************************

    '**********************************************************************************************
    '*** COM - Upgrade vorbereiten nach Anfrage auf dem Modul-Register                          ***
    '**********************************************************************************************
    Public Sub COMUpgradeButton()

        Dim UserPfad As String

        Dim FileRead As FileStream
        Dim FileReadStream As StreamReader
        Dim Read As String

        Dim FileWrite As FileStream
        Dim FileWriteStream As StreamWriter

        Dim PageIndex As Integer
        Dim BufferIndex As Integer

        ' Oeffne einen Dateidialog

        If ofd_Oeffnen.ShowDialog() = System.Windows.Forms.DialogResult.OK Then

            ' Setze die BOOT-EEPROM-Indices

            PageIndex = 1
            BufferIndex = 1

            ' Oeffne die asugewaehlte Datei

            FileRead = New FileStream(ofd_Oeffnen.FileName, FileMode.Open)
            FileReadStream = New StreamReader(FileRead)

            ' Bestimme die Anzahl der PAGEs des BOOT-EEPROMs

            While Not (FileReadStream.EndOfStream)

                Read = FileReadStream.ReadLine

                For index = 1 To Len(Read)

                    BufferIndex += 1

                    If BufferIndex > MBC_BOOT_PGSZ Then
                        BufferIndex = 1
                        PageIndex += 1
                    End If
                Next

            End While

            FileReadStream.Close()
            FileRead.Close()

            ' Bestimme unberuecksichtigte PAGES

            If BufferIndex > MBC_BOOT_PGSZ Then
                PageIndex -= 1
            End If

            ' Lege Anzahl der BOOT-Pages fest

            UpgradePageMax = PageIndex

            ' Dimensioniere den UpgradeBuffer neu

            ReDim UpgradeBuffer(UpgradePageMax)

            ' Dateipfad bestimmen fuer internes Speichern

            UserPfad = "Inifiles/"

            ' Ordner erstellen wenn nicht existent

            If System.IO.Directory.Exists("Inifiles") = False Then

                System.IO.Directory.CreateDirectory("Inifiles")

            End If

            ' Loesche Datei falls schon vorhanden

            If File.Exists(UserPfad & "test.hex") Then
                File.Delete(UserPfad & "test.hex")
            End If

            ' Oeffne Datei und uebersetze das HEX-File in eine PAGE-Format

            FileRead = New FileStream(ofd_Oeffnen.FileName, FileMode.Open)
            FileReadStream = New StreamReader(FileRead)

            ' Oeffne eine neue Datei zur Kontrolle des Datenflusses im PAGE-FORMAT

            FileWrite = New FileStream(UserPfad & "test.hex", FileMode.Create)
            FileWriteStream = New StreamWriter(FileWrite)

            ' Setze die BOOT-EEPROM-Indices

            PageIndex = 1
            BufferIndex = 1

            ' Loesche ggf. vorhandene BOOT-Bufferdaten

            For index = 0 To UpgradePageMax
                UpgradeBuffer(index) = ""
            Next

            ' Gehe in eine Schleife und uebersetze das HEX-File in eine PAGE-Format fuer
            ' das BOOT-EEPROM

            While Not (FileReadStream.EndOfStream)

                Read = FileReadStream.ReadLine

                For index = 1 To Len(Read)

                    ' Speichere Daten im BOOT-Array ab

                    If BufferIndex <= MBC_BOOT_PGSZ Then
                        UpgradeBuffer(PageIndex) &= Mid(Read, index, 1)
                        BufferIndex += 1
                    End If

                    ' Ist die PAGE-Groesse erreicht, dann schreibe Daten in die BOOT-Datei

                    If BufferIndex > MBC_BOOT_PGSZ Then
                        FileWriteStream.WriteLine(UpgradeBuffer(PageIndex))
                        BufferIndex = 1
                        PageIndex += 1
                    End If
                Next

            End While

            ' Schreibe Restdaten in die Datei

            If Len(UpgradeBuffer(PageIndex)) < MBC_BOOT_PGSZ Then
                For BufferIndex = Len(UpgradeBuffer(PageIndex)) + 1 To MBC_BOOT_PGSZ
                    UpgradeBuffer(PageIndex) &= "F"
                Next
                FileWriteStream.WriteLine(UpgradeBuffer(PageIndex))
            End If

            FileReadStream.Close()
            FileRead.Close()

            FileWriteStream.Close()
            FileWrite.Close()

            ' Setze eine neutrale Nachricht im BOOT-Buffer PAGE 0 fuer den Bootloader

            UpgradeBuffer(0) = ""

            For BufferIndex = 1 To MBC_BOOT_PGSZ
                UpgradeBuffer(0) &= Chr(&HFF)
            Next

            ' Sperre Eingabemoeglichkeiten

            Register_Modul_Edit_Enable(False)
            tv_Strukturbaum.Enabled = False

            'Stoppe Alive-Timer

            tm_Alive.Stop()
            tm_Neu.Stop()

            ' Setze das ProgrammierFLAG, damit beim Betaetigen des <Stopp>-Buttons ein gesteuerter Abbruch moeglich ist

            FLAG_Prog_Index = PROG_INDEX_Upgrade

            ' Gebe <Stopp>-Button frei

            Register_Modul_Buttons_Enable(False, True, False, False, False, False)

            ' <Verwerfen>-Button umbenennen

            btn_ControlVerwerfen.Text = Sprachen(i_MAIN, i_btn_ControlVerwerfen_Stop)

            ' Zeige MsgBox

            MsgBox(Sprachen(i_MSGBOX_MAIN_UPGSEND, i_MSGBOX_MAIN_UPGSEND_1) & ":  '" & MBC.ModulTyp(AktTyp).Bezeichner & "' " &
                   Sprachen(i_MSGBOX_MAIN_UPGSEND, i_MSGBOX_MAIN_UPGSEND_2) & "." & Chr(13) & Chr(13) _
                   & Sprachen(i_MSGBOX_MAIN_UPGSEND, i_MSGBOX_MAIN_UPGSEND_3) & ".",
                   vbOKOnly, Sprachen(i_MSGBOX_MAIN_UPGSEND, i_MSGBOX_MAIN_UPGSEND_4))

            ' Starte Datenuebertragung

            COMUpgradeStart(MBC_BOOT_ARRAY, MBC_BOOT_ARRAY + MBC_BOOT_PGSZ + 1, 0)

        End If
    End Sub

    '**********************************************************************************************
    '*** COM - Upgrade vorbereiten fuer RP2040 (Raspberry Pi Pico) - Einsprungmethode           ***
    '*** RP2040 nutzt denselben MbcOta-Mechanismus wie ESP32-C3 (siehe MbcOta.h in MBCANLib -   ***
    '*** identischer #if-Zweig fuer beide Cores) - daher gemeinsame Implementierung ueber       ***
    '*** COMUpgradeButtonBinary().                                                              ***
    '**********************************************************************************************

    Public Sub COMUpgradeButtonRP2040Pico()

        COMUpgradeButtonBinary("RP2040 (Raspberry Pi Pico)",
            Sprachen(i_MSGBOX_MAIN_UPGBIN, i_MSGBOX_MAIN_UPGBIN_6) & Chr(13) &
            Sprachen(i_MSGBOX_MAIN_UPGBIN, i_MSGBOX_MAIN_UPGBIN_7))

    End Sub

    '**********************************************************************************************
    '*** COM - Upgrade vorbereiten fuer RP2040 Zero - Einsprungmethode                          ***
    '*** Siehe COMUpgradeButtonRP2040Pico() - gleicher Core, gleicher Mechanismus.              ***
    '**********************************************************************************************

    Public Sub COMUpgradeButtonRP2040Zero()

        COMUpgradeButtonBinary("RP2040 Zero",
            Sprachen(i_MSGBOX_MAIN_UPGBIN, i_MSGBOX_MAIN_UPGBIN_6) & Chr(13) &
            Sprachen(i_MSGBOX_MAIN_UPGBIN, i_MSGBOX_MAIN_UPGBIN_7))

    End Sub

    '**********************************************************************************************
    '*** COM - Upgrade vorbereiten fuer ESP32-C3 - Einsprungmethode                             ***
    '*** Siehe COMUpgradeButtonBinary() fuer den eigentlichen Ablauf. Nutzt zusaetzlich die     ***
    '*** engere, konkret bekannte OTA-Partitionsgroesse (MBC_OTA_ESP32C3_PARTITION_MAX, siehe   ***
    '*** definitionen.vb) statt nur der generischen Boot-Speicherchip-Kapazitaet - RP2040 kann  ***
    '*** das mangels bekannter Flash-Size-Auswahl nicht (siehe COMUpgradeButtonRP2040Pico()).   ***
    '**********************************************************************************************

    Public Sub COMUpgradeButtonESP32C3()

        COMUpgradeButtonBinary("ESP32-C3", maxFwBytes:=MBC_OTA_ESP32C3_PARTITION_MAX)

    End Sub

    '**********************************************************************************************
    '*** COM - Upgrade vorbereiten fuer RP2040/ESP32-C3 (gemeinsame Implementierung)            ***
    '*** Liest eine von der Arduino-IDE erzeugte .bin-Datei binaer ein (kein Text-/Hex-Format   ***
    '*** wie beim ATmega644P), verteilt sie auf 256-Byte-Boot-Speicherchip-Pages und haengt     ***
    '*** zusaetzlich eine MbcOta-Header-Page (Magic/Laenge/CRC32, siehe MbcOta.h in MBCANLib)   ***
    '*** an, die vom Schritt-2-Automaten in COMCmdReceive nach der letzten Firmware-Page        ***
    '*** mitgesendet wird.                                                                      ***
    '**********************************************************************************************
    '*** boardName: Anzeigename in der Bestaetigungsmeldung (z.B. "ESP32-C3") *********************
    '*** hinweis: optional - zusaetzliche Hinweiszeile(n) in der Bestaetigungsmeldung *************
    '*** maxFwBytes: optional - board-spezifische Obergrenze in Byte (z.B. ESP32-C3-OTA- **********
    '*** Partitionsgroesse), enger als die Boot-Speicherchip-Kapazitaet unten; -1 (Default) = *****
    '*** nur die generische Kapazitaetsgrenze pruefen (RP2040, siehe definitionen.vb) *************
    '**********************************************************************************************

    Private Sub COMUpgradeButtonBinary(boardName As String, Optional hinweis As String = "", Optional maxFwBytes As Integer = -1)

        Dim fileBytes() As Byte
        Dim maxPageIndex As Integer
        Dim pageIndex As Integer
        Dim byteIndex As Integer
        Dim srcOffset As Integer
        Dim pageText As String
        Dim headerText As String
        Dim crc As UInteger
        Dim fwLength As UInteger

        If ofd_OeffnenBin.ShowDialog() = System.Windows.Forms.DialogResult.OK Then

            ' Firmwaredatei komplett binaer einlesen (keine Zeilenstruktur wie beim ATmega644P)

            fileBytes = File.ReadAllBytes(ofd_OeffnenBin.FileName)

            ' Pruefen, ob die Datei in den Boot-Speicherchip-Zwischenspeicher passt
            ' (Pages 0..MBC_OTA_HEADER_PAGE-1 fuer Firmware, MBC_OTA_HEADER_PAGE selbst = Header)

            maxPageIndex = CInt(Math.Ceiling(fileBytes.Length / CDbl(MBC_BOOT_PGSZ))) - 1

            If maxPageIndex >= MBC_OTA_HEADER_PAGE Then
                MsgBox(Sprachen(i_MSGBOX_MAIN_UPGBIN, i_MSGBOX_MAIN_UPGBIN_1) & " " &
                       CStr(MBC_OTA_HEADER_PAGE * MBC_BOOT_PGSZ) & " " & Sprachen(i_MSGBOX_MAIN_UPGBIN, i_MSGBOX_MAIN_UPGBIN_2),
                       vbOKOnly, Sprachen(i_MSGBOX_MAIN_UPGSEND, i_MSGBOX_MAIN_UPGSEND_4))
                Exit Sub
            End If

            ' Zusaetzliche, board-spezifisch engere Grenze (z.B. ESP32-C3-OTA-Partitionsgroesse aus
            ' partitions.csv) - kleiner als die Boot-Speicherchip-Kapazitaet oben, wird VOR der
            ' (langsamen) CAN-Uebertragung geprueft. RP2040 hat kein Gegenstueck (maxFwBytes bleibt
            ' -1) - dort greift nur die generische Pruefung oben, siehe definitionen.vb.

            If maxFwBytes > 0 AndAlso fileBytes.Length > maxFwBytes Then
                MsgBox(Sprachen(i_MSGBOX_MAIN_UPGBIN, i_MSGBOX_MAIN_UPGBIN_1) & " " &
                       CStr(maxFwBytes) & " " & Sprachen(i_MSGBOX_MAIN_UPGBIN, i_MSGBOX_MAIN_UPGBIN_2),
                       vbOKOnly, Sprachen(i_MSGBOX_MAIN_UPGSEND, i_MSGBOX_MAIN_UPGSEND_4))
                Exit Sub
            End If

            ' Eine kompilierte .bin-Datei enthaelt anders als beim ATmega644P keine Signaturzeile
            ' mit dem Modultyp - stattdessen hier eine manuelle Bestaetigung einholen

            Dim confirmText As String = Sprachen(i_MSGBOX_MAIN_UPGBIN, i_MSGBOX_MAIN_UPGBIN_3) & ": '" & ofd_OeffnenBin.FileName & "'" & Chr(13) &
                      Sprachen(i_MSGBOX_MAIN_UPGBIN, i_MSGBOX_MAIN_UPGBIN_4) & ": '" & MBC.ModulTyp(AktTyp).Bezeichner & "'" & Chr(13) &
                      Sprachen(i_PROZESSORAUSWAHL, i_PROZESSORAUSWAHL_gb) & ": '" & boardName & "'" & Chr(13)

            If hinweis <> "" Then confirmText &= Chr(13) & hinweis & Chr(13)

            confirmText &= Chr(13) & Sprachen(i_MSGBOX_MAIN_UPGBIN, i_MSGBOX_MAIN_UPGBIN_5)

            If MsgBox(confirmText, vbYesNo, Sprachen(i_MSGBOX_MAIN_UPGSEND, i_MSGBOX_MAIN_UPGSEND_4)) = vbNo Then
                Exit Sub
            End If

            UpgradePageMax = maxPageIndex

            ' Boot-Pages (0..UpgradePageMax) plus reservierter Platz fuer die OTA-Header-Page

            ReDim UpgradeBuffer(MBC_OTA_HEADER_PAGE)

            ' Firmware auf 256-Byte-Pages verteilen, letzte Page mit Fuellbyte auffuellen
            ' (MbcOta liest ohnehin nur die per Header angegebenen Nutzbytes, keine Fuellbytes)

            For pageIndex = 0 To UpgradePageMax
                pageText = ""
                For byteIndex = 0 To MBC_BOOT_PGSZ - 1
                    srcOffset = pageIndex * MBC_BOOT_PGSZ + byteIndex
                    If srcOffset < fileBytes.Length Then
                        pageText &= Chr(fileBytes(srcOffset))
                    Else
                        pageText &= Chr(&HFF)
                    End If
                Next
                UpgradeBuffer(pageIndex) = pageText
            Next

            ' OTA-Header-Page vorbereiten: Magic(4)+Laenge(4)+CRC32(4), alle little-endian,
            ' Rest der Page mit 0x00 aufgefuellt (siehe MbcOta::Header in MbcOta.h)

            crc = Crc32(fileBytes, fileBytes.Length)
            fwLength = CUInt(fileBytes.Length)

            headerText = Chr(CByte(MBC_OTA_MAGIC And &HFFUI)) &
                         Chr(CByte((MBC_OTA_MAGIC >> 8) And &HFFUI)) &
                         Chr(CByte((MBC_OTA_MAGIC >> 16) And &HFFUI)) &
                         Chr(CByte((MBC_OTA_MAGIC >> 24) And &HFFUI)) &
                         Chr(CByte(fwLength And &HFFUI)) &
                         Chr(CByte((fwLength >> 8) And &HFFUI)) &
                         Chr(CByte((fwLength >> 16) And &HFFUI)) &
                         Chr(CByte((fwLength >> 24) And &HFFUI)) &
                         Chr(CByte(crc And &HFFUI)) &
                         Chr(CByte((crc >> 8) And &HFFUI)) &
                         Chr(CByte((crc >> 16) And &HFFUI)) &
                         Chr(CByte((crc >> 24) And &HFFUI))

            For byteIndex = 12 To MBC_BOOT_PGSZ - 1
                headerText &= Chr(0)
            Next

            UpgradeBuffer(MBC_OTA_HEADER_PAGE) = headerText

            ' Sperre Eingabemoeglichkeiten

            Register_Modul_Edit_Enable(False)
            tv_Strukturbaum.Enabled = False

            ' Stoppe Alive-Timer

            tm_Alive.Stop()
            tm_Neu.Stop()

            ' Setze das ProgrammierFLAG, damit beim Betaetigen des <Stopp>-Buttons ein
            ' gesteuerter Abbruch moeglich ist

            FLAG_Prog_Index = PROG_INDEX_Upgrade

            ' Gebe <Stopp>-Button frei

            Register_Modul_Buttons_Enable(False, True, False, False, False, False)

            ' <Verwerfen>-Button umbenennen

            btn_ControlVerwerfen.Text = Sprachen(i_MAIN, i_btn_ControlVerwerfen_Stop)

            ' Nach der letzten Firmware-Page zusaetzlich die OTA-Header-Page senden

            FLAG_Upgrade_NeedsOtaHeader = True

            ' Zeige MsgBox

            MsgBox(Sprachen(i_MSGBOX_MAIN_UPGSEND, i_MSGBOX_MAIN_UPGSEND_1) & ":  '" & MBC.ModulTyp(AktTyp).Bezeichner & "' " &
                   Sprachen(i_MSGBOX_MAIN_UPGSEND, i_MSGBOX_MAIN_UPGSEND_2) & "." & Chr(13) & Chr(13) _
                   & Sprachen(i_MSGBOX_MAIN_UPGSEND, i_MSGBOX_MAIN_UPGSEND_3) & ".",
                   vbOKOnly, Sprachen(i_MSGBOX_MAIN_UPGSEND, i_MSGBOX_MAIN_UPGSEND_4))

            ' Starte Datenuebertragung

            COMUpgradeStart(MBC_BOOT_ARRAY, MBC_BOOT_ARRAY + MBC_BOOT_PGSZ + 1, 0)

        End If

    End Sub

    '**********************************************************************************************
    '*** COM - Upgrade-Anfrage an das aktuelle Modul senden                                     ***
    '**********************************************************************************************

    Public Sub COMUpgradeStart(Startindex As Integer, Stopindex As Integer, Page As Integer)

        Dim sPage As String

        ' Bestimme zu schreiben PAGE des BOOT-Arrays

        UpgradePage = Page

        sPage = FormHEX("0", 4, Page)

        MBC.ModulTyp(AktTyp).Modul(AktNr).SYS_ARRAY(MBC_BOOT_ARRAY) = Mid(sPage, 1, 2)
        MBC.ModulTyp(AktTyp).Modul(AktNr).SYS_ARRAY(MBC_BOOT_ARRAY + 1) = Mid(sPage, 3, 2)

        ' Schreibe auf der angegebenen PAGE den Inhalt der Firmware
        ' Nutze dazu den Modulspeicher im SYS_ARRAY

        For index = 1 To MBC_BOOT_PGSZ
            MBC.ModulTyp(AktTyp).Modul(AktNr).SYS_ARRAY(MBC_BOOT_ARRAY + 1 + index) = FormHEX("0", 2, Asc(Mid(UpgradeBuffer(Page), index, 1)))
        Next

        ' Setze alle Flags und Indizes zurueck

        FLAG_Upgrade_IO = False
        FLAG_Upgrade_Request = False

        ' Signalisiere den Start des Updatezyklus

        If Page = 0 Then
            spb_Refresh.Value = 0
            spb_Refresh.Maximum = (2 + MBC_BOOT_PGSZ) * (UpgradePageMax + 1)
            stp_Refresh.Text = Sprachen(i_MAIN, i_stp_Refresh_upgrade_start)
            stp_Alive.Text = Sprachen(i_MAIN, i_stp_Alive_None)
            spb_Refresh.Enabled = True
        End If

        stp_Upgrade.Text = Sprachen(i_MAIN, i_stp_Upgrade_Active) & " " & CStr(UpgradePage + 1) & " / " & CStr(UpgradePageMax + 1)

        ' Sende Zugriffsanfrage zum Upgrade an das Modul

        COMMsgSend(MsgUPRequest & MBC.ModulTyp(AktTyp).Modul(AktNr).MsgGUiD)

        ' Setze Update-Flag und Laufindices auf Anfang und Ende

        IOStart = Startindex
        IOMax = Stopindex
        FLAG_Upgrade_Request = True

    End Sub

    '**********************************************************************************************
    '*** COM - Upgrade-Ende an das aktuelle Modul senden                                        ***
    '**********************************************************************************************

    Public Sub COMUpgradeComplete()

        Dim Button As Integer

        Button = MsgBox(Sprachen(i_MSGBOX_MAIN_UPGCOMPLETE, i_MSGBOX_MAIN_UPGCOMPLETE_1) & Chr(13) & Chr(13) _
                        & Sprachen(i_MSGBOX_MAIN_UPGCOMPLETE, i_MSGBOX_MAIN_UPGCOMPLETE_2) & ":   '" & MBC.ModulTyp(AktTyp).Bezeichner & "' " _
                        & Sprachen(i_MSGBOX_MAIN_UPGCOMPLETE, i_MSGBOX_MAIN_UPGCOMPLETE_3) & Chr(13) & Chr(13) _
                        & Sprachen(i_MSGBOX_MAIN_UPGCOMPLETE, i_MSGBOX_MAIN_UPGCOMPLETE_4) & Chr(13) & Chr(13) _
                        & Sprachen(i_MSGBOX_MAIN_UPGCOMPLETE, i_MSGBOX_MAIN_UPGCOMPLETE_5) & Chr(13) & Chr(13) _
                        & Sprachen(i_MSGBOX_MAIN_UPGCOMPLETE, i_MSGBOX_MAIN_UPGCOMPLETE_6) & ":" & Chr(13) & Chr(13) _
                        & Sprachen(i_MSGBOX_MAIN_UPGCOMPLETE, i_MSGBOX_MAIN_UPGCOMPLETE_7), vbYesNo, Sprachen(i_MSGBOX_MAIN_UPGCOMPLETE, i_MSGBOX_MAIN_UPGCOMPLETE_8))

        If Button = vbYes Then

            ' Sage dem Modul dass es neu BOOTEN soll

            COMMsgSend(MsgBOOT & MBC.ModulTyp(AktTyp).Modul(AktNr).MsgGUiD)

            ' Das Modul ist nach dem Reset fuer die Dauer des Bootvorgangs nicht erreichbar -
            ' im Strukturbaum sofort auf "nicht anwesend" setzen, statt auf die naechsten 4
            ' erfolglosen Alive-Ticks zu warten (siehe Timer_Alive_Tick())

            MBC.ModulTyp(AktTyp).Modul(AktNr).Status = IsMissing
            MBC.ModulTyp(AktTyp).Modul(AktNr).IsAlive = 0
            Strukturbaum_MenueIconChange(AktTyp, AktNr, False, True)

            ' Modulbild/LEDs auf Typ-Standard zuruecksetzen - gleicher Zustand wie ein
            ' noch nie registriertes Modul (Stellpult/Buttons werden ueber den
            ' Modulstatus-Check in COMUpgradeStop() gesperrt gehalten)

            Register_Modul_Bilder_Show(AktTyp)

        End If

        COMUpgradeStop()

    End Sub

    '**********************************************************************************************
    '*** COM - Upgrade-Ende an das aktuelle Modul senden                                        ***
    '**********************************************************************************************

    Public Sub COMUpgradeStop()

        ' Stripanzeige aktualisieren

        stp_Refresh.Text = Sprachen(i_MAIN, i_stp_Refresh_upgrade_ready)
        spb_Refresh.Enabled = False
        spb_Refresh.Value = 0
        stp_Upgrade.Text = Sprachen(i_MAIN, i_stp_Upgrade_None)

        ' Flags zuruecksetzen

        FLAG_Upgrade_IO = False
        FLAG_Upgrade_Request = False
        FLAG_Upgrade_NeedsOtaHeader = False
        FLAG_Upgrade_SendingOtaHeader = False

        ' Gebe Eingabemoeglichkeiten frei - nur wenn das Modul (z.B. bei Abbruch per
        ' <Stopp>-Button) weiterhin vollstaendig angemeldet ist. Nach einem erfolgreich
        ' abgeschlossenen Upgrade wurde der Status bereits auf IsMissing gesetzt - hier
        ' NICHT blind wieder freigeben, sonst bleiben Buttons/Stellpult/Modulbild-
        ' Container faelschlich aktiv, obwohl der Baum das Modul als inaktiv zeigt.

        If MBC.ModulTyp(AktTyp).Modul(AktNr).Status = IsComplete Then
            Register_Modul_Edit_Enable(True)
        End If
        tv_Strukturbaum.Enabled = True

        ' <Verwerfen>-Button rueckbenennen

        btn_ControlVerwerfen.Text = Sprachen(i_MAIN, i_btn_ControlVerwerfen)

        ' Alive-Timer wieder starten

        tm_Alive.Start()
        tm_Neu.Start()

    End Sub

    '**********************************************************************************************
    '*** Funktionen fuer den Refresh von Modulenparametern                                      ***
    '**********************************************************************************************


    '**********************************************************************************************
    '*** COM - Refresh-Anfrage an das aktuelle Modul senden                                     ***
    '**********************************************************************************************

    Public Sub COMRefreshStart(Startindex As Integer, Stopindex As Integer)

        ' Sperre Eingabemoeglichkeiten

        Register_Modul_Edit_Enable(False)

        'Stoppe Alive-Timer

        tm_Alive.Stop()
        tm_Neu.Stop()

        ' Setze alle Flags und Indizes zurueck

        FLAG_Read_IO = False
        FLAG_Read_Request = False
        FLAG_Write_IO = False
        FLAG_Write_Request = False
        FLAG_Write_Program = False
        FLAG_Write_Refresh = False
        FLAG_Refresh_Acomplished = False

        ' Signalisiere den Start des Refreshzyklus

        stp_Refresh.Text = Sprachen(i_MAIN, i_stp_Refresh_start)
        stp_Alive.Text = Sprachen(i_MAIN, i_stp_Alive_None)
        spb_Refresh.Enabled = True
        spb_Refresh.Value = 0
        spb_Refresh.Maximum = Stopindex - Startindex + 10

        ' Sende initiale Zugriffsanfrage

        IOStart = Startindex
        IOMax = Stopindex


        COMMsgSendAndArm(MsgIORequest & MBC.ModulTyp(AktTyp).Modul(AktNr).MsgGUiD)


        ' Zeige an, dass eine Refreshanfrage gesendet wurde

        FLAG_Read_Request = True

    End Sub

    '**********************************************************************************************
    '*** COM - Refresh-Anfrage an das aktuelle Modul stoppen                                    ***
    '**********************************************************************************************

    Public Sub COMRefreshStop()

        ' Stripanzeige aktualisieren

        stp_Refresh.Text = Sprachen(i_MAIN, i_stp_Refresh_stop)
        spb_Refresh.Enabled = False
        spb_Refresh.Value = 0

        ' Flags zuruecksetzen

        FLAG_Read_IO = False
        FLAG_Read_Request = False
        FLAG_Write_IO = False
        FLAG_Write_Request = False
        FLAG_Write_Program = False
        FLAG_Write_Refresh = False

        ' Watchdog stoppen

        tm_ComTimeout.Stop()

        ' Alive-Timer wieder starten

        tm_Alive.Start()
        tm_Neu.Start()

    End Sub

    '**********************************************************************************************
    '*** COM - Neue Werte aus der Refresh-Anfrage in das aktuelle Modul uebernehmen und anzeigen***
    '**********************************************************************************************
    Public Sub COMRefreshComplete()

        Dim Name As String

        '++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

        ' Allgemeine Modulparameter setzen

        ' Namen zusammensetzen

        Name = ""

        For Index = MBC_S_NAMEBLOCK To MBC_S_NAMEBLOCK + MBC_L_NAMEBLOCK - 1
            If (MBC.ModulTyp(AktTyp).Modul(AktNr).SYS_ARRAY(Index) <> "81") And (MBC.ModulTyp(AktTyp).Modul(AktNr).SYS_ARRAY(Index) <> "") Then Name &= Chr("&H" & MBC.ModulTyp(AktTyp).Modul(AktNr).SYS_ARRAY(Index))
        Next

        If Len(Name) > 18 Then Name = Microsoft.VisualBasic.Left(Name, 18)

        If (Name <> MBC.ModulTyp(AktTyp).Modul(AktNr).Name) Then
            txt_ParameterName.Text = Name
            MBC.ModulTyp(AktTyp).Modul(AktNr).Name = Name
            Strukturbaum_MenueChangeLabel()
        End If

        ' Plugged

        MBC.ModulTyp(AktTyp).Modul(AktNr).Plugged = COMGetPlugged(MBC.ModulTyp(AktTyp).Modul(AktNr).SYS_ARRAY(MBC_PLUGGED))

        ' Spannung

        MBC.ModulTyp(AktTyp).Modul(AktNr).Spannung = Chr("&H" & MBC.ModulTyp(AktTyp).Modul(AktNr).SYS_ARRAY(MBC_SPG_1)) &
                           Chr("&H" & MBC.ModulTyp(AktTyp).Modul(AktNr).SYS_ARRAY(MBC_SPG_2)) & "." &
                           Chr("&H" & MBC.ModulTyp(AktTyp).Modul(AktNr).SYS_ARRAY(MBC_SPG_3))

        ' Temperatur

        MBC.ModulTyp(AktTyp).Modul(AktNr).Temperatur = Chr("&H" & MBC.ModulTyp(AktTyp).Modul(AktNr).SYS_ARRAY(MBC_TMP_1)) &
                             Chr("&H" & MBC.ModulTyp(AktTyp).Modul(AktNr).SYS_ARRAY(MBC_TMP_2)) & "." &
                             Chr("&H" & MBC.ModulTyp(AktTyp).Modul(AktNr).SYS_ARRAY(MBC_TMP_3))

        '++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

        ' Modulspezifische Parameter setzen

        MBC.ModulTyp(AktTyp).Modul(AktNr).Parameter_Fill()

        '++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

        ' Status auf IsComplete setzen

        MBC.ModulTyp(AktTyp).Modul(AktNr).Status = IsComplete
        Strukturbaum_MenueIconChange(AktTyp, AktNr, False, True)

        ' Werte aktualisieren

        Register_Modul_Werte_Show(True)

        ' Gebe Eingabemoeglichkeiten frei

        Register_Modul_Edit_Enable(True)

        ' Refresh erfolgreich abgeschlossen

        FLAG_Refresh_Acomplished = True

        ' Watchdog stoppen (regulaeres Zyklusende, laeuft nicht ueber COMRefreshStop())

        tm_ComTimeout.Stop()

        ' Alive-Timer wieder starten

        tm_Alive.Start()
        tm_Neu.Start()

    End Sub

    '**********************************************************************************************
    '*** COM - Logfile oeffnen                                                                  ***
    '**********************************************************************************************

    Public Sub COMLogFileOeffnen(LogIndex As Integer)

        Dim Tag As String
        Dim Monat As String
        Dim Jahr As String
        Dim UserPfad As String

        UserPfad = Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments)

        If LogIndex = RS232Log Then UserPfad &= "\MBCAN\LogfilesRS232"
        If LogIndex = BTLog Then UserPfad &= "\MBCAN\LogfilesBT"
        If LogIndex = TCPLog Then UserPfad &= "\MBCAN\LogfilesTCP"

        ' Ordner erstellen wenn nicht existent

        If System.IO.Directory.Exists(UserPfad) = False Then

            System.IO.Directory.CreateDirectory(UserPfad)

        End If

        ' Logfile generieren

        Monat = Microsoft.VisualBasic.Left(Microsoft.VisualBasic.DateString, 2)
        Tag = Microsoft.VisualBasic.Mid(Microsoft.VisualBasic.DateString, 4, 2)
        Jahr = Microsoft.VisualBasic.Right(Microsoft.VisualBasic.DateString, 4)

        COMPfad = UserPfad & "\" & Jahr & "-" & Monat & "-" & Tag & " - " & Microsoft.VisualBasic.TimeOfDay

        If LogIndex = RS232Log Then COMPfad &= " - CAN over RS232.txt"
        If LogIndex = BTLog Then COMPfad &= " - CAN over BT.txt"
        If LogIndex = TCPLog Then COMPfad &= " - CAN over TCP.txt"

        COMPfad = "C:" & Microsoft.VisualBasic.Replace(COMPfad, ":", "-", 3)

        COMRecFile = New FileStream(COMPfad, FileMode.Create)
        COMRecFileWr = New StreamWriter(COMRecFile)

        COMRecFileWr.WriteLine("CAN-Telegramme auf dem Märklin-Bus")
        COMRecFileWr.WriteLine("(c) 2007-" & Jahr & " by Dr.-Ing. Thomas Wiesner")
        COMRecFileWr.WriteLine("https://github.com/TommyGermanyRetro/MBCAN")

        If LogIndex = RS232Log Then COMRecFileWr.WriteLine("Daten gelesen über die RS232-Schnittstelle")
        If LogIndex = BTLog Then COMRecFileWr.WriteLine("Daten gelesen über die BT-Schnittstelle")
        If LogIndex = TCPLog Then COMRecFileWr.WriteLine("Daten gelesen über die TCP-Schnittstelle")

        COMRecFileWr.WriteLine("Logfile ohne Gewähr")

        COMMonitorMsgReceived("--------------------------------------------------------------------------------------------" & vbCrLf, vbCrLf)

        ' Setze Nachrichtenzaehler zurueck

        COMMsgCount = 0

    End Sub

    '**********************************************************************************************
    '*** COM - Logfile schließen                                                                ***
    '**********************************************************************************************
    Public Sub COMLogFileSchliessen()

        ' Logfile schliessen

        COMRecFileWr.Close()
        COMRecFile.Close()

    End Sub

End Class
