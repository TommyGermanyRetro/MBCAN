'**************************************************************************************************
'*** CAN-Monitor fuer MBCAN/CS2 - TCP-Client                                                    ***
'*** (c) 2026 by Dr.-Ing. Thomas Wiesner                                                        ***
'*** Verbindet sich als TCP-Client mit einem CAN-Gateway (z. B. mbc-80/CS2), zeigt jedes        ***
'*** empfangene CAN-Telegramm dekodiert im Monitorfenster an und schreibt es mitlaufend in ein  ***
'*** Logfile. Dekodierung, Empfangs-Thread und Logfile-Textformat 1:1 aus dem                   ***
'*** MBCAN-Parametriercenter uebernommen (dort: frmMain.vb/frmMain.ComProtocol.vb).             ***
'**************************************************************************************************

Imports System.IO
Imports System.Net.Sockets
Imports System.Runtime.InteropServices

Public Class frmMain

    ' Fester TCP-Zielport (CAN-ueber-TCP-Gateway)

    Private Const TCPSocketNo As Integer = 15731

    ' Monitorfenster: erst ab MAX_LOG_LINES auf TRIM_TO_LINES kuerzen (statt bei jeder Nachricht),
    ' damit das Kuerzen selten genug passiert, um kein sichtbares Flackern zu verursachen

    Private Const MAX_LOG_LINES As Integer = 2000
    Private Const TRIM_TO_LINES As Integer = 1000

    <DllImport("user32.dll")>
    Private Shared Function SendMessage(hWnd As IntPtr, msg As Integer, wParam As IntPtr, lParam As IntPtr) As IntPtr
    End Function

    Private Const WM_SETREDRAW As Integer = &HB

    ' Neuzeichnen eines Controls waehrend eines groesseren Update-Batches unterdruecken (verhindert
    ' Flackern beim seltenen Kuerzen des Monitorpuffers)

    Private Sub SuspendDrawing(ctl As Control)
        SendMessage(ctl.Handle, WM_SETREDRAW, IntPtr.Zero, IntPtr.Zero)
    End Sub

    Private Sub ResumeDrawing(ctl As Control)
        SendMessage(ctl.Handle, WM_SETREDRAW, New IntPtr(1), IntPtr.Zero)
        ctl.Invalidate()
    End Sub

    Private TCPclient As TcpClient
    Private TCPstream As NetworkStream
    Private TCPThreadReceive As System.Threading.Thread
    Private TCPThreadIsActive As Boolean = False

    Private LogFile As FileStream
    Private LogFileWr As StreamWriter

    Delegate Sub SetTextCallback(ByVal [text] As String)
    Delegate Sub SetTextCallback2(ByVal [text1] As String, ByVal [text2] As String)

    '++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    '+++ Hauptfenster                                                                           +++
    '++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    Private Sub frmMain_Load(sender As Object, e As EventArgs) Handles MyBase.Load

        txt_IPAdresse_TextChanged(txt_IPAdresse, EventArgs.Empty)

    End Sub

    Private Sub frmMain_FormClosing(sender As Object, e As FormClosingEventArgs) Handles MyBase.FormClosing

        If TCPThreadIsActive Then TCPSchliessen()

    End Sub

    '**********************************************************************************************
    '*** IP-Adresse - Formatpruefung                                                            ***
    '*** Vier durch Punkt getrennte Byte-Werte (0...255), sonst ungueltig.                      ***
    '**********************************************************************************************

    Private Function IsValidAddress(ByVal Address As String) As Boolean

        If (Address.Length - Address.Replace(".", "").Length) <> 3 Then
            Return False
        End If

        Dim Segments() As String = Address.Split(".")

        For Each Segment As String In Segments
            If Not Byte.TryParse(Segment, Nothing) Then
                Return False
            End If
        Next

        Return True

    End Function

    Private Sub txt_IPAdresse_TextChanged(sender As Object, e As EventArgs) Handles txt_IPAdresse.TextChanged

        If IsValidAddress(txt_IPAdresse.Text) Then
            txt_IPAdresse.BackColor = Color.White
            btn_Verbinden.Enabled = Not TCPThreadIsActive
        Else
            txt_IPAdresse.BackColor = Color.Wheat
            btn_Verbinden.Enabled = False
        End If

    End Sub

    '**********************************************************************************************
    '*** Buttons - Verbinden / Trennen                                                          ***
    '**********************************************************************************************

    Private Sub btn_Verbinden_Click(sender As Object, e As EventArgs) Handles btn_Verbinden.Click

        TCPOeffnen()

    End Sub

    Private Sub btn_Trennen_Click(sender As Object, e As EventArgs) Handles btn_Trennen.Click

        TCPSchliessen()

    End Sub

    '++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    '+++ TCP                                                                                    +++
    '+++ Funktionen zum Umgang mit der TCP-Schnittstelle                                        +++
    '++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    '**********************************************************************************************
    '*** TCP - Schnittstelle oeffnen                                                            ***
    '**********************************************************************************************

    Private Sub TCPOeffnen()

        stp_Verbindung.Text = "Verbinde..."
        Refresh()

        Try
            TCPclient = New TcpClient()
            TCPclient.Connect(txt_IPAdresse.Text, TCPSocketNo)
            TCPstream = TCPclient.GetStream

            If Not TCPstream.CanRead Then
                MsgBox("Der Datenstrom kann nicht gelesen werden.", vbOKOnly, "TCP-Verbindung")
                TCPclient.Close()
                stp_Verbindung.Text = "Getrennt"
                Exit Sub
            End If

            If Not TCPstream.CanWrite Then
                MsgBox("Der Datenstrom kann nicht beschrieben werden.", vbOKOnly, "TCP-Verbindung")
                TCPclient.Close()
                stp_Verbindung.Text = "Getrennt"
                Exit Sub
            End If

        Catch ex As Exception

            MsgBox("Die Verbindung zum Server konnte nicht hergestellt werden." & Chr(13) & Chr(13) &
                   String.Format("Error: {0}", ex.Message), vbOKOnly, "TCP-Verbindung")

            stp_Verbindung.Text = "Getrennt"

            Exit Sub

        End Try

        ' Empfangs-Thread starten

        CommunicationThread.StartReceiveThread(TCPThreadReceive, TCPThreadIsActive, AddressOf TCPReceiveMessages)

        ' Logfile oeffnen

        LogFileOeffnen()

        ' Bedienelemente sperren/freigeben

        txt_IPAdresse.Enabled = False
        btn_Verbinden.Enabled = False
        btn_Trennen.Enabled = True

        stp_Verbindung.Text = "Verbunden: " & txt_IPAdresse.Text & " Port " & TCPSocketNo

    End Sub

    '**********************************************************************************************
    '*** TCP - Schnittstelle schliessen                                                         ***
    '**********************************************************************************************

    Private Sub TCPSchliessen()

        stp_Verbindung.Text = "Trenne..."

        CommunicationThread.StopReceiveThread(TCPThreadIsActive, TCPThreadReceive, 4000)

        Try
            TCPclient.Close()
        Catch ex As Exception

        End Try

        LogFileSchliessen()

        txt_IPAdresse.Enabled = True
        btn_Verbinden.Enabled = IsValidAddress(txt_IPAdresse.Text)
        btn_Trennen.Enabled = False

        stp_Verbindung.Text = "Getrennt"

    End Sub

    '**********************************************************************************************
    '*** TCP - Nachricht empfangen (laeuft im Empfangs-Thread)                                  ***
    '**********************************************************************************************

    Private Sub TCPReceiveMessages()

        Dim index As Integer
        Dim TCPBuffer As String
        Dim bytes(13) As Byte

        While TCPThreadIsActive = True

            Try

                If TCPstream.CanRead Then

                    TCPstream.Read(bytes, 0, 13)

                    TCPBuffer = ""

                    For index = 0 To 12
                        TCPBuffer += CanDecode.FormHEX("0", 2, CInt(bytes(index)))
                        If index <> 12 Then TCPBuffer += " "
                    Next

                    ' Daten des Threads an den Hauptthread uebergeben

                    MsgReceived(TCPBuffer)

                End If

            Catch ex As Exception

            End Try

        End While

    End Sub

    '++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    '+++ Nachrichtenanzeige und Logfile                                                         +++
    '++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    '**********************************************************************************************
    '*** Threadshared - Nachricht dekodieren und im Monitorfenster anzeigen                     ***
    '**********************************************************************************************

    Private Sub MsgReceived(ByVal [text] As String)

        If rtb_Monitor.InvokeRequired Then

            Dim x As New SetTextCallback(AddressOf MsgReceived)
            Invoke(x, New Object() {(text)})

        Else

            Dim MsgLine1 As String = ""
            Dim MsgLine2 As String = ""

            CanDecode.BuildMsgMonitor([text], MsgLine1, MsgLine2)

            MonitorMsgReceived(Microsoft.VisualBasic.TimeString & Format(Now, ".fff") & " R " & MsgLine1, MsgLine2)

        End If

    End Sub

    '**********************************************************************************************
    '*** Nachricht im Richtextfeld einfuegen und ins Logfile schreiben                          ***
    '**********************************************************************************************

    Private Sub MonitorMsgReceived(ByVal [text1] As String, ByVal [text2] As String)

        If rtb_Monitor.InvokeRequired Then
            Dim x As New SetTextCallback2(AddressOf MonitorMsgReceived)
            Invoke(x, New Object() {(text1), (text2)})

        Else

            Dim currentFont = rtb_Monitor.SelectionFont

            SuspendDrawing(rtb_Monitor)

            Try
                ' Puffer nur selten (alle MAX_LOG_LINES Zeilen) auf einen Schlag kuerzen, statt bei
                ' jeder Nachricht neu aufzubauen - das war die Ursache des Flackerns/Renderfehlers.
                ' Bis dahin wird laufend nur angehaengt, das Fenster fuellt sich also vollstaendig.

                If rtb_Monitor.Lines.LongLength > MAX_LOG_LINES Then
                    Dim Zeile() As String = rtb_Monitor.Lines
                    Dim Rest(TRIM_TO_LINES - 1) As String
                    Array.Copy(Zeile, Zeile.Length - TRIM_TO_LINES, Rest, 0, TRIM_TO_LINES)
                    rtb_Monitor.Lines = Rest
                End If

                rtb_Monitor.SelectionStart = rtb_Monitor.TextLength
                rtb_Monitor.SelectionLength = 0

                rtb_Monitor.SelectionFont = New Font(currentFont.FontFamily, currentFont.Size, FontStyle.Regular)
                rtb_Monitor.SelectionColor = Color.Black
                rtb_Monitor.AppendText([text1])

                rtb_Monitor.SelectionFont = New Font(currentFont.FontFamily, currentFont.Size, FontStyle.Italic)
                rtb_Monitor.SelectionColor = Color.Blue
                rtb_Monitor.AppendText([text2])

                rtb_Monitor.SelectionStart = rtb_Monitor.TextLength
                rtb_Monitor.ScrollToCaret()

            Finally
                ResumeDrawing(rtb_Monitor)
            End Try

            ' Daten in Logfile schreiben

            If LogFileWr IsNot Nothing Then
                Try
                    LogFileWr.Write([text1] & [text2])
                Catch ex As Exception

                End Try

            End If

        End If

    End Sub

    '**********************************************************************************************
    '*** Logfile oeffnen                                                                        ***
    '**********************************************************************************************

    Private Sub LogFileOeffnen()

        Dim Tag As String
        Dim Monat As String
        Dim Jahr As String
        Dim UserPfad As String
        Dim LogPfad As String

        UserPfad = Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments) & "\CANMON\LogfilesTCP"

        If System.IO.Directory.Exists(UserPfad) = False Then
            System.IO.Directory.CreateDirectory(UserPfad)
        End If

        Monat = Microsoft.VisualBasic.Left(Microsoft.VisualBasic.DateString, 2)
        Tag = Microsoft.VisualBasic.Mid(Microsoft.VisualBasic.DateString, 4, 2)
        Jahr = Microsoft.VisualBasic.Right(Microsoft.VisualBasic.DateString, 4)

        LogPfad = UserPfad & "\" & Jahr & "-" & Monat & "-" & Tag & " - " & Microsoft.VisualBasic.TimeOfDay & " - CAN over TCP.txt"

        LogPfad = "C:" & Microsoft.VisualBasic.Replace(LogPfad, ":", "-", 3)

        LogFile = New FileStream(LogPfad, FileMode.Create)
        LogFileWr = New StreamWriter(LogFile)

        LogFileWr.WriteLine("CAN-Telegramme auf dem Märklin-Bus")
        LogFileWr.WriteLine("(c) 2007-" & Jahr & " by Dr.-Ing. Thomas Wiesner")
        LogFileWr.WriteLine("https://github.com/TommyGermanyRetro/MBCAN")
        LogFileWr.WriteLine("Daten gelesen über die TCP-Schnittstelle")
        LogFileWr.WriteLine("Logfile ohne Gewähr")

        MonitorMsgReceived("--------------------------------------------------------------------------------------------" & vbCrLf, vbCrLf)

    End Sub

    '**********************************************************************************************
    '*** Logfile schliessen                                                                     ***
    '**********************************************************************************************

    Private Sub LogFileSchliessen()

        Try
            LogFileWr.Close()
            LogFile.Close()
        Catch ex As Exception

        End Try

        LogFileWr = Nothing
        LogFile = Nothing

    End Sub

End Class
