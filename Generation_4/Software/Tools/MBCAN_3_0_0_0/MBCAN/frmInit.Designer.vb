<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()>
Partial Class frmInit
    Inherits System.Windows.Forms.Form

    'Das Formular überschreibt den Löschvorgang, um die Komponentenliste zu bereinigen.
    <System.Diagnostics.DebuggerNonUserCode()>
    Protected Overrides Sub Dispose(ByVal disposing As Boolean)
        Try
            If disposing AndAlso components IsNot Nothing Then
                components.Dispose()
            End If
        Finally
            MyBase.Dispose(disposing)
        End Try
    End Sub

    'Wird vom Windows Form-Designer benötigt.
    Private components As System.ComponentModel.IContainer

    'Hinweis: Die folgende Prozedur ist für den Windows Form-Designer erforderlich.
    'Das Bearbeiten ist mit dem Windows Form-Designer möglich.  
    'Das Bearbeiten mit dem Code-Editor ist nicht möglich.
    <System.Diagnostics.DebuggerStepThrough()>
    Private Sub InitializeComponent()
        Dim resources As System.ComponentModel.ComponentResourceManager = New System.ComponentModel.ComponentResourceManager(GetType(frmInit))
        Me.btnOk = New System.Windows.Forms.Button()
        Me.GroupBoxRS232 = New System.Windows.Forms.GroupBox()
        Me.cmbRS232BaudRate = New System.Windows.Forms.Label()
        Me.cmbRS232Port = New System.Windows.Forms.ComboBox()
        Me.Label9 = New System.Windows.Forms.Label()
        Me.Label8 = New System.Windows.Forms.Label()
        Me.Label7 = New System.Windows.Forms.Label()
        Me.Label5 = New System.Windows.Forms.Label()
        Me.Label4 = New System.Windows.Forms.Label()
        Me.Label10 = New System.Windows.Forms.Label()
        Me.Label11 = New System.Windows.Forms.Label()
        Me.Label12 = New System.Windows.Forms.Label()
        Me.GroupBox1 = New System.Windows.Forms.GroupBox()
        Me.txtIndex = New System.Windows.Forms.TextBox()
        Me.Label17 = New System.Windows.Forms.Label()
        Me.Label18 = New System.Windows.Forms.Label()
        Me.Label15 = New System.Windows.Forms.Label()
        Me.Label14 = New System.Windows.Forms.Label()
        Me.txtKenner = New System.Windows.Forms.TextBox()
        Me.Label13 = New System.Windows.Forms.Label()
        Me.GroupBoxBT = New System.Windows.Forms.GroupBox()
        Me.cmbBTBaudRate = New System.Windows.Forms.Label()
        Me.cmbBTPort = New System.Windows.Forms.ComboBox()
        Me.Label24 = New System.Windows.Forms.Label()
        Me.Label19 = New System.Windows.Forms.Label()
        Me.Label25 = New System.Windows.Forms.Label()
        Me.Label23 = New System.Windows.Forms.Label()
        Me.Label26 = New System.Windows.Forms.Label()
        Me.Label20 = New System.Windows.Forms.Label()
        Me.Label22 = New System.Windows.Forms.Label()
        Me.Label21 = New System.Windows.Forms.Label()
        Me.GroupBoxTCP = New System.Windows.Forms.GroupBox()
        Me.txtMBCTXPort = New System.Windows.Forms.Label()
        Me.txtMBCIP = New System.Windows.Forms.TextBox()
        Me.Label27 = New System.Windows.Forms.Label()
        Me.Label29 = New System.Windows.Forms.Label()
        Me.GroupBoxRS232.SuspendLayout()
        Me.GroupBox1.SuspendLayout()
        Me.GroupBoxBT.SuspendLayout()
        Me.GroupBoxTCP.SuspendLayout()
        Me.SuspendLayout()
        '
        'btnOk
        '
        Me.btnOk.Location = New System.Drawing.Point(12, 346)
        Me.btnOk.Name = "btnOk"
        Me.btnOk.Size = New System.Drawing.Size(75, 23)
        Me.btnOk.TabIndex = 4
        Me.btnOk.Text = "Ok"
        Me.btnOk.UseVisualStyleBackColor = True
        '
        'GroupBoxRS232
        '
        Me.GroupBoxRS232.Controls.Add(Me.cmbRS232BaudRate)
        Me.GroupBoxRS232.Location = New System.Drawing.Point(12, 12)
        Me.GroupBoxRS232.Name = "GroupBoxRS232"
        Me.GroupBoxRS232.Size = New System.Drawing.Size(186, 177)
        Me.GroupBoxRS232.TabIndex = 5
        Me.GroupBoxRS232.TabStop = False
        Me.GroupBoxRS232.Text = "RS232"
        '
        'cmbRS232BaudRate
        '
        Me.cmbRS232BaudRate.AutoSize = True
        Me.cmbRS232BaudRate.Location = New System.Drawing.Point(80, 60)
        Me.cmbRS232BaudRate.Name = "cmbRS232BaudRate"
        Me.cmbRS232BaudRate.Size = New System.Drawing.Size(16, 13)
        Me.cmbRS232BaudRate.TabIndex = 22
        Me.cmbRS232BaudRate.Text = "..."
        '
        'cmbRS232Port
        '
        Me.cmbRS232Port.FormattingEnabled = True
        Me.cmbRS232Port.Location = New System.Drawing.Point(98, 39)
        Me.cmbRS232Port.Name = "cmbRS232Port"
        Me.cmbRS232Port.Size = New System.Drawing.Size(80, 21)
        Me.cmbRS232Port.TabIndex = 19
        '
        'Label9
        '
        Me.Label9.AutoSize = True
        Me.Label9.Location = New System.Drawing.Point(95, 164)
        Me.Label9.Name = "Label9"
        Me.Label9.Size = New System.Drawing.Size(13, 13)
        Me.Label9.TabIndex = 18
        Me.Label9.Text = "1"
        '
        'Label8
        '
        Me.Label8.AutoSize = True
        Me.Label8.Location = New System.Drawing.Point(95, 133)
        Me.Label8.Name = "Label8"
        Me.Label8.Size = New System.Drawing.Size(33, 13)
        Me.Label8.TabIndex = 17
        Me.Label8.Text = "None"
        '
        'Label7
        '
        Me.Label7.AutoSize = True
        Me.Label7.Location = New System.Drawing.Point(95, 102)
        Me.Label7.Name = "Label7"
        Me.Label7.Size = New System.Drawing.Size(13, 13)
        Me.Label7.TabIndex = 16
        Me.Label7.Text = "8"
        '
        'Label5
        '
        Me.Label5.AutoSize = True
        Me.Label5.Location = New System.Drawing.Point(18, 164)
        Me.Label5.Name = "Label5"
        Me.Label5.Size = New System.Drawing.Size(39, 13)
        Me.Label5.TabIndex = 14
        Me.Label5.Text = "#Stop:"
        '
        'Label4
        '
        Me.Label4.AutoSize = True
        Me.Label4.Location = New System.Drawing.Point(18, 133)
        Me.Label4.Name = "Label4"
        Me.Label4.Size = New System.Drawing.Size(43, 13)
        Me.Label4.TabIndex = 13
        Me.Label4.Text = "#Parity:"
        '
        'Label10
        '
        Me.Label10.AutoSize = True
        Me.Label10.Location = New System.Drawing.Point(18, 102)
        Me.Label10.Name = "Label10"
        Me.Label10.Size = New System.Drawing.Size(34, 13)
        Me.Label10.TabIndex = 12
        Me.Label10.Text = "#Bits:"
        '
        'Label11
        '
        Me.Label11.AutoSize = True
        Me.Label11.Location = New System.Drawing.Point(18, 72)
        Me.Label11.Name = "Label11"
        Me.Label11.Size = New System.Drawing.Size(53, 13)
        Me.Label11.TabIndex = 11
        Me.Label11.Text = "Baudrate:"
        '
        'Label12
        '
        Me.Label12.AutoSize = True
        Me.Label12.Location = New System.Drawing.Point(18, 42)
        Me.Label12.Name = "Label12"
        Me.Label12.Size = New System.Drawing.Size(56, 13)
        Me.Label12.TabIndex = 10
        Me.Label12.Text = "COM-Port:"
        '
        'GroupBox1
        '
        Me.GroupBox1.Controls.Add(Me.txtIndex)
        Me.GroupBox1.Controls.Add(Me.Label17)
        Me.GroupBox1.Controls.Add(Me.Label18)
        Me.GroupBox1.Controls.Add(Me.Label15)
        Me.GroupBox1.Controls.Add(Me.Label14)
        Me.GroupBox1.Controls.Add(Me.txtKenner)
        Me.GroupBox1.Controls.Add(Me.Label13)
        Me.GroupBox1.Location = New System.Drawing.Point(12, 195)
        Me.GroupBox1.Name = "GroupBox1"
        Me.GroupBox1.Size = New System.Drawing.Size(733, 132)
        Me.GroupBox1.TabIndex = 7
        Me.GroupBox1.TabStop = False
        Me.GroupBox1.Text = "CSx-Parameter"
        '
        'txtIndex
        '
        Me.txtIndex.Location = New System.Drawing.Point(147, 86)
        Me.txtIndex.MaxLength = 1
        Me.txtIndex.Name = "txtIndex"
        Me.txtIndex.Size = New System.Drawing.Size(54, 20)
        Me.txtIndex.TabIndex = 12
        Me.txtIndex.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        '
        'Label17
        '
        Me.Label17.AutoSize = True
        Me.Label17.Location = New System.Drawing.Point(6, 89)
        Me.Label17.Name = "Label17"
        Me.Label17.Size = New System.Drawing.Size(64, 13)
        Me.Label17.TabIndex = 11
        Me.Label17.Text = "Modulindex:"
        '
        'Label18
        '
        Me.Label18.Location = New System.Drawing.Point(262, 86)
        Me.Label18.MaximumSize = New System.Drawing.Size(340, 60)
        Me.Label18.Name = "Label18"
        Me.Label18.Size = New System.Drawing.Size(340, 29)
        Me.Label18.TabIndex = 10
        Me.Label18.Text = "Dieser Index symbolisiert die erste Stelle der GUiD. Default-Wert ""m"". Erlaubt is" &
    "t jeder Kleinbuchstabe."
        '
        'Label15
        '
        Me.Label15.Location = New System.Drawing.Point(262, 28)
        Me.Label15.MaximumSize = New System.Drawing.Size(340, 60)
        Me.Label15.Name = "Label15"
        Me.Label15.Size = New System.Drawing.Size(340, 60)
        Me.Label15.TabIndex = 7
        Me.Label15.Text = "Dieser Index symbolisiert die Gerätekennung bei der Anmeldung an die CSx. Default" &
    "-Wert ""AAAA"".  Vergleiche CAN-Protokoll-Defintion von Märklin."
        '
        'Label14
        '
        Me.Label14.AutoSize = True
        Me.Label14.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25!, System.Drawing.FontStyle.Italic, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label14.Location = New System.Drawing.Point(116, 31)
        Me.Label14.Name = "Label14"
        Me.Label14.Size = New System.Drawing.Size(29, 13)
        Me.Label14.TabIndex = 6
        Me.Label14.Text = "HEX"
        '
        'txtKenner
        '
        Me.txtKenner.Location = New System.Drawing.Point(147, 28)
        Me.txtKenner.MaxLength = 4
        Me.txtKenner.Name = "txtKenner"
        Me.txtKenner.Size = New System.Drawing.Size(54, 20)
        Me.txtKenner.TabIndex = 5
        Me.txtKenner.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        '
        'Label13
        '
        Me.Label13.AutoSize = True
        Me.Label13.Location = New System.Drawing.Point(6, 31)
        Me.Label13.Name = "Label13"
        Me.Label13.Size = New System.Drawing.Size(84, 13)
        Me.Label13.TabIndex = 4
        Me.Label13.Text = "Gerätekennung:"
        '
        'GroupBoxBT
        '
        Me.GroupBoxBT.Controls.Add(Me.cmbBTBaudRate)
        Me.GroupBoxBT.Controls.Add(Me.cmbBTPort)
        Me.GroupBoxBT.Controls.Add(Me.Label24)
        Me.GroupBoxBT.Controls.Add(Me.Label19)
        Me.GroupBoxBT.Controls.Add(Me.Label25)
        Me.GroupBoxBT.Controls.Add(Me.Label23)
        Me.GroupBoxBT.Controls.Add(Me.Label26)
        Me.GroupBoxBT.Controls.Add(Me.Label20)
        Me.GroupBoxBT.Controls.Add(Me.Label22)
        Me.GroupBoxBT.Controls.Add(Me.Label21)
        Me.GroupBoxBT.Location = New System.Drawing.Point(204, 12)
        Me.GroupBoxBT.Name = "GroupBoxBT"
        Me.GroupBoxBT.Size = New System.Drawing.Size(183, 177)
        Me.GroupBoxBT.TabIndex = 6
        Me.GroupBoxBT.TabStop = False
        Me.GroupBoxBT.Text = "Bluetooth"
        '
        'cmbBTBaudRate
        '
        Me.cmbBTBaudRate.AutoSize = True
        Me.cmbBTBaudRate.Location = New System.Drawing.Point(85, 60)
        Me.cmbBTBaudRate.Name = "cmbBTBaudRate"
        Me.cmbBTBaudRate.Size = New System.Drawing.Size(16, 13)
        Me.cmbBTBaudRate.TabIndex = 25
        Me.cmbBTBaudRate.Text = "..."
        '
        'cmbBTPort
        '
        Me.cmbBTPort.FormattingEnabled = True
        Me.cmbBTPort.Location = New System.Drawing.Point(88, 27)
        Me.cmbBTPort.Name = "cmbBTPort"
        Me.cmbBTPort.Size = New System.Drawing.Size(80, 21)
        Me.cmbBTPort.TabIndex = 20
        '
        'Label24
        '
        Me.Label24.AutoSize = True
        Me.Label24.Location = New System.Drawing.Point(87, 152)
        Me.Label24.Name = "Label24"
        Me.Label24.Size = New System.Drawing.Size(13, 13)
        Me.Label24.TabIndex = 23
        Me.Label24.Text = "1"
        '
        'Label19
        '
        Me.Label19.AutoSize = True
        Me.Label19.Location = New System.Drawing.Point(6, 152)
        Me.Label19.Name = "Label19"
        Me.Label19.Size = New System.Drawing.Size(39, 13)
        Me.Label19.TabIndex = 24
        Me.Label19.Text = "#Stop:"
        '
        'Label25
        '
        Me.Label25.AutoSize = True
        Me.Label25.Location = New System.Drawing.Point(87, 121)
        Me.Label25.Name = "Label25"
        Me.Label25.Size = New System.Drawing.Size(33, 13)
        Me.Label25.TabIndex = 22
        Me.Label25.Text = "None"
        '
        'Label23
        '
        Me.Label23.AutoSize = True
        Me.Label23.Location = New System.Drawing.Point(6, 30)
        Me.Label23.Name = "Label23"
        Me.Label23.Size = New System.Drawing.Size(56, 13)
        Me.Label23.TabIndex = 20
        Me.Label23.Text = "COM-Port:"
        '
        'Label26
        '
        Me.Label26.AutoSize = True
        Me.Label26.Location = New System.Drawing.Point(87, 90)
        Me.Label26.Name = "Label26"
        Me.Label26.Size = New System.Drawing.Size(13, 13)
        Me.Label26.TabIndex = 21
        Me.Label26.Text = "8"
        '
        'Label20
        '
        Me.Label20.AutoSize = True
        Me.Label20.Location = New System.Drawing.Point(6, 121)
        Me.Label20.Name = "Label20"
        Me.Label20.Size = New System.Drawing.Size(43, 13)
        Me.Label20.TabIndex = 23
        Me.Label20.Text = "#Parity:"
        '
        'Label22
        '
        Me.Label22.AutoSize = True
        Me.Label22.Location = New System.Drawing.Point(6, 60)
        Me.Label22.Name = "Label22"
        Me.Label22.Size = New System.Drawing.Size(53, 13)
        Me.Label22.TabIndex = 21
        Me.Label22.Text = "Baudrate:"
        '
        'Label21
        '
        Me.Label21.AutoSize = True
        Me.Label21.Location = New System.Drawing.Point(6, 90)
        Me.Label21.Name = "Label21"
        Me.Label21.Size = New System.Drawing.Size(34, 13)
        Me.Label21.TabIndex = 22
        Me.Label21.Text = "#Bits:"
        '
        'GroupBoxTCP
        '
        Me.GroupBoxTCP.Controls.Add(Me.txtMBCTXPort)
        Me.GroupBoxTCP.Controls.Add(Me.txtMBCIP)
        Me.GroupBoxTCP.Controls.Add(Me.Label27)
        Me.GroupBoxTCP.Controls.Add(Me.Label29)
        Me.GroupBoxTCP.Location = New System.Drawing.Point(393, 12)
        Me.GroupBoxTCP.Name = "GroupBoxTCP"
        Me.GroupBoxTCP.Size = New System.Drawing.Size(173, 177)
        Me.GroupBoxTCP.TabIndex = 21
        Me.GroupBoxTCP.TabStop = False
        Me.GroupBoxTCP.Text = "TCP"
        '
        'txtMBCTXPort
        '
        Me.txtMBCTXPort.AutoSize = True
        Me.txtMBCTXPort.Location = New System.Drawing.Point(60, 61)
        Me.txtMBCTXPort.Name = "txtMBCTXPort"
        Me.txtMBCTXPort.Size = New System.Drawing.Size(16, 13)
        Me.txtMBCTXPort.TabIndex = 7
        Me.txtMBCTXPort.Text = "..."
        '
        'txtMBCIP
        '
        Me.txtMBCIP.Location = New System.Drawing.Point(63, 28)
        Me.txtMBCIP.Name = "txtMBCIP"
        Me.txtMBCIP.Size = New System.Drawing.Size(100, 20)
        Me.txtMBCIP.TabIndex = 5
        '
        'Label27
        '
        Me.Label27.AutoSize = True
        Me.Label27.Location = New System.Drawing.Point(6, 31)
        Me.Label27.Name = "Label27"
        Me.Label27.Size = New System.Drawing.Size(20, 13)
        Me.Label27.TabIndex = 4
        Me.Label27.Text = "IP:"
        '
        'Label29
        '
        Me.Label29.AutoSize = True
        Me.Label29.Location = New System.Drawing.Point(7, 61)
        Me.Label29.Name = "Label29"
        Me.Label29.Size = New System.Drawing.Size(29, 13)
        Me.Label29.TabIndex = 1
        Me.Label29.Text = "Port:"
        '
        'frmInit
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.BackColor = System.Drawing.Color.White
        Me.ClientSize = New System.Drawing.Size(764, 381)
        Me.Controls.Add(Me.GroupBoxTCP)
        Me.Controls.Add(Me.GroupBoxBT)
        Me.Controls.Add(Me.GroupBox1)
        Me.Controls.Add(Me.cmbRS232Port)
        Me.Controls.Add(Me.Label9)
        Me.Controls.Add(Me.Label8)
        Me.Controls.Add(Me.Label7)
        Me.Controls.Add(Me.Label5)
        Me.Controls.Add(Me.Label4)
        Me.Controls.Add(Me.Label10)
        Me.Controls.Add(Me.Label11)
        Me.Controls.Add(Me.Label12)
        Me.Controls.Add(Me.GroupBoxRS232)
        Me.Controls.Add(Me.btnOk)
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle
        Me.Icon = CType(resources.GetObject("$this.Icon"), System.Drawing.Icon)
        Me.MaximizeBox = False
        Me.MinimizeBox = False
        Me.Name = "frmInit"
        Me.ShowInTaskbar = False
        Me.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent
        Me.Text = "MBCAN - Kommunikationsparameter"
        Me.GroupBoxRS232.ResumeLayout(False)
        Me.GroupBoxRS232.PerformLayout()
        Me.GroupBox1.ResumeLayout(False)
        Me.GroupBox1.PerformLayout()
        Me.GroupBoxBT.ResumeLayout(False)
        Me.GroupBoxBT.PerformLayout()
        Me.GroupBoxTCP.ResumeLayout(False)
        Me.GroupBoxTCP.PerformLayout()
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Friend WithEvents btnOk As System.Windows.Forms.Button
    Friend WithEvents GroupBoxRS232 As System.Windows.Forms.GroupBox
    Friend WithEvents cmbRS232Port As System.Windows.Forms.ComboBox
    Friend WithEvents Label9 As System.Windows.Forms.Label
    Friend WithEvents Label8 As System.Windows.Forms.Label
    Friend WithEvents Label7 As System.Windows.Forms.Label
    Friend WithEvents Label5 As System.Windows.Forms.Label
    Friend WithEvents Label4 As System.Windows.Forms.Label
    Friend WithEvents Label10 As System.Windows.Forms.Label
    Friend WithEvents Label11 As System.Windows.Forms.Label
    Friend WithEvents Label12 As System.Windows.Forms.Label
    Friend WithEvents GroupBox1 As System.Windows.Forms.GroupBox
    Friend WithEvents Label14 As System.Windows.Forms.Label
    Friend WithEvents txtKenner As System.Windows.Forms.TextBox
    Friend WithEvents Label13 As System.Windows.Forms.Label
    Friend WithEvents Label18 As System.Windows.Forms.Label
    Friend WithEvents Label15 As System.Windows.Forms.Label
    Friend WithEvents GroupBoxBT As GroupBox
    Friend WithEvents cmbBTPort As ComboBox
    Friend WithEvents Label24 As Label
    Friend WithEvents Label19 As Label
    Friend WithEvents Label25 As Label
    Friend WithEvents Label23 As Label
    Friend WithEvents Label26 As Label
    Friend WithEvents Label20 As Label
    Friend WithEvents Label22 As Label
    Friend WithEvents Label21 As Label
    Friend WithEvents txtIndex As TextBox
    Friend WithEvents Label17 As Label
    Friend WithEvents cmbRS232BaudRate As Label
    Friend WithEvents cmbBTBaudRate As Label
    Friend WithEvents GroupBoxTCP As GroupBox
    Friend WithEvents txtMBCTXPort As Label
    Friend WithEvents txtMBCIP As TextBox
    Friend WithEvents Label27 As Label
    Friend WithEvents Label29 As Label
End Class
