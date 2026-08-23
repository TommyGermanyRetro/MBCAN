Public Class frmInit

    Dim ValidIPMBC As Boolean

    ' Funktion zur Pruefung der IP-Adresse bei UDP-Betrieb

    Private Function IsValidAddress(ByVal Address As String) As Boolean

        ' Zaehle die Anzahl der Trennpunkte im IPv4-Format

        If (Address.Length - Address.Replace(".", "").Length) <> 3 Then
            Return False
        End If

        ' IP-Segemente testen

        Dim Segments() As String = Address.Split(".")

        For Each Segment As String In Segments
            If Not Byte.TryParse(Segment, Nothing) Then
                Return False
            End If
        Next

        Return True

    End Function

    Private Sub txtMBCIP_TextChanged(sender As Object, e As EventArgs) Handles txtMBCIP.TextChanged
        If IsValidAddress(txtMBCIP.Text) = False Then
            txtMBCIP.BackColor = Color.Wheat
            ValidIPMBC = False
        Else
            txtMBCIP.BackColor = Color.White
            ValidIPMBC = True
        End If
    End Sub

    ' Funktion um den Geraetekenner an die Freigaben von Maerklin anzupassen
    ' Diese sind in der Beschreibung der CS2 unter dem Menuepunkt zum Systembefehl 0x00 Sub-CMD 0x0C zu finden
    ' Derzeit funktioniert die Freigabe mit dem HEX-Wert 0xAAAA

    Private Sub txtKenner_TextChanged(sender As System.Object, e As System.EventArgs) Handles txtKenner.TextChanged

        If Len(txtKenner.Text) = 4 Then
            CS2_Kenner = txtKenner.Text
            txtKenner.BackColor = Color.White
        Else
            txtKenner.BackColor = Color.Wheat
        End If

    End Sub

    Private Sub txtKenner_KeyPress(sender As Object, e As KeyPressEventArgs) Handles txtKenner.KeyPress

        ' Nur HEX-Werte zulassen

        Select Case Asc(e.KeyChar)
            Case &H30 To &H39, 65 To 70, 8
            Case Else
                e.Handled = True
        End Select

    End Sub

    ' Funktion um den Modulindex anzupassen
    ' Derzeit funktioniert die Freigabe mit dem Wert m

    Private Sub txtIndex_TextChanged(sender As Object, e As EventArgs) Handles txtIndex.TextChanged

        If Len(txtIndex.Text) = 1 Then
            CS2_Identifier = FormHEX("0", 2, Asc(txtIndex.Text))
            txtIndex.BackColor = Color.White
        Else
            txtIndex.BackColor = Color.Wheat
        End If

    End Sub

    Private Sub txtIndex_KeyPress(sender As Object, e As KeyPressEventArgs) Handles txtIndex.KeyPress

        ' Nur Kleinbuchstaben zulassen

        Select Case Asc(e.KeyChar)
            Case Asc("a") To Asc("z"), 8
            Case Else
                e.Handled = True
        End Select

    End Sub

    ' Funktion wird ausgefuehrt wenn das Formular geschlossen wird

    Private Sub frmInit_FormClosing(ByVal sender As Object, ByVal e As System.Windows.Forms.FormClosingEventArgs) Handles Me.FormClosing

        ' Sichere Einstellungen für TCP-Verbindung
        ' Pruefe dazu auf valide IP-Adressen

        If txtMBCIP.Text <> "0.0.0.0" And IsValidAddress(txtMBCIP.Text) Then

            frmMain.ts_MenuVerbindungTCP.Enabled = True
            frmMain.ts_MenuVerbindungTCPVerbinden.Enabled = True
            frmMain.ts_MenuVerbindungTCPTrennen.Enabled = False
            TCPSocketNo = txtMBCTXPort.Text
            TCPIP = txtMBCIP.Text
            TCPIsInit = True

        Else

            frmMain.ts_MenuVerbindungTCP.Enabled = False
            frmMain.ts_MenuVerbindungTCPVerbinden.Enabled = False
            frmMain.ts_MenuVerbindungTCPTrennen.Enabled = False
            TCPIP = "0.0.0.0"
            TCPIsInit = False

        End If

        ' Sichere Einstellungen für RS232-Verbindung

        If cmbRS232Port.Text <> "-" Then

            With RS232Client
                .PortName = cmbRS232Port.Text
                .BaudRate = Val(cmbRS232BaudRate.Text)
                .Parity = IO.Ports.Parity.None
                .StopBits = IO.Ports.StopBits.One
                .DtrEnable = False
                .RtsEnable = False
                .DataBits = 8
            End With

            frmMain.ts_MenuVerbindungRS232.Enabled = True
            frmMain.ts_MenuVerbindungRS232Verbinden.Enabled = True
            frmMain.ts_MenuVerbindungRS232Trennen.Enabled = False
            RS232Port = cmbRS232Port.Text
            RS232BaudRate = Val(cmbRS232BaudRate.Text)
            RS232IsInit = True

        Else

            RS232Client.PortName = " "
            frmMain.ts_MenuVerbindungRS232.Enabled = False
            frmMain.ts_MenuVerbindungRS232Verbinden.Enabled = False
            frmMain.ts_MenuVerbindungRS232Trennen.Enabled = False
            RS232Port = "-"
            RS232IsInit = False

        End If

        ' Sichere Einstellungen für Bluetooth-Verbindung

        If cmbBTPort.Text <> "-" Then

            With BTClient
                .PortName = cmbBTPort.Text
                .BaudRate = Val(BTVirtualBaudRate)
            End With

            frmMain.ts_MenuVerbindungBluetooth.Enabled = True
            frmMain.ts_MenuVerbindungBluetoothVerbinden.Enabled = True
            frmMain.ts_MenuVerbindungBluetoothTrennen.Enabled = False
            BTPort = cmbBTPort.Text
            BTIsInit = True

        Else

            BTClient.PortName = " "
            frmMain.ts_MenuVerbindungBluetooth.Enabled = False
            frmMain.ts_MenuVerbindungBluetoothVerbinden.Enabled = False
            frmMain.ts_MenuVerbindungBluetoothTrennen.Enabled = False
            BTPort = "-"
            BTIsInit = False

        End If

        frmMain.Enabled = True

    End Sub

    ' Funktion wird ausgefuehrt beim Laden des Formulars

    Private Sub frmInit_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load

        frmMain.Enabled = False
        AddHandler Me.LocationChanged, AddressOf ClampInnerhalbEltern

        ' Lade TCP-Parameter

        txtMBCIP.Text = TCPIP
        txtMBCTXPort.Text = TCPSocketNo

        'Bereite RS232 und BT vor

        Dim myPort As Array

        ' Hole verfuegbare COM-Port-Nummern und fuelle die PullDown-Listen

        myPort = IO.Ports.SerialPort.GetPortNames()

        cmbRS232Port.Items.Clear()
        cmbBTPort.Items.Clear()

        For i = 0 To UBound(myPort)
            cmbRS232Port.Items.Add(myPort(i))
            cmbBTPort.Items.Add(myPort(i))
        Next

        cmbRS232Port.Text = RS232Port
        cmbRS232BaudRate.Text = RS232BaudRate

        cmbBTPort.Text = BTPort
        cmbBTBaudRate.Text = BTBaudRate

        ' CSx-Parameter einstellen fuer die Modulidentifizierung

        txtKenner.Text = CS2_Kenner
        txtIndex.Text = Chr("&H" & CS2_Identifier)

    End Sub

    Private Sub btnOk_Click(sender As Object, e As EventArgs) Handles btnOk.Click

        ' Pruefe auf valide IP-Adressen
        ' Wenn zulaessig, dann schliesse das Fenster

        If ValidIPMBC = False Then
            MsgBox("IP-Adresse nicht korrekt eingegeben. Bitte anpassen!", vbOKOnly)
        Else
            Me.Close()
        End If

    End Sub

    Private Sub cmbRS232Port_SelectedIndexChanged(sender As Object, e As EventArgs) Handles cmbRS232Port.SelectedIndexChanged

    End Sub
End Class