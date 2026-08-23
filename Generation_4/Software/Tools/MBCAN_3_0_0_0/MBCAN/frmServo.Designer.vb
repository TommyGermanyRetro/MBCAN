<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()>
Partial Class frmServo
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
        Dim ChartArea1 As System.Windows.Forms.DataVisualization.Charting.ChartArea = New System.Windows.Forms.DataVisualization.Charting.ChartArea()
        Dim Legend1 As System.Windows.Forms.DataVisualization.Charting.Legend = New System.Windows.Forms.DataVisualization.Charting.Legend()
        Dim Series1 As System.Windows.Forms.DataVisualization.Charting.Series = New System.Windows.Forms.DataVisualization.Charting.Series()
        Dim resources As System.ComponentModel.ComponentResourceManager = New System.ComponentModel.ComponentResourceManager(GetType(frmServo))
        Me.chart_servo = New System.Windows.Forms.DataVisualization.Charting.Chart()
        Me.btn_servo_ok = New System.Windows.Forms.Button()
        Me.nud_servo_step = New System.Windows.Forms.NumericUpDown()
        Me.nud_servo_wert = New System.Windows.Forms.NumericUpDown()
        Me.lbl_servo_step = New System.Windows.Forms.Label()
        Me.lbl_servo_wert = New System.Windows.Forms.Label()
        Me.lbl_servo_linksmax = New System.Windows.Forms.Label()
        Me.lbl_servo_rechtsmax = New System.Windows.Forms.Label()
        Me.txt_servo_linksmax = New System.Windows.Forms.TextBox()
        Me.txt_servo_rechtsmax = New System.Windows.Forms.TextBox()
        Me.btn_servo_verwerfen = New System.Windows.Forms.Button()
        Me.lbl_servo_anschlaglinks = New System.Windows.Forms.Label()
        Me.lbl_servo_anschlagmitte = New System.Windows.Forms.Label()
        Me.lbl_servo_anschlagrechts = New System.Windows.Forms.Label()
        Me.lbl_servo_dirlinksrechts = New System.Windows.Forms.Label()
        Me.lbl_servo_dirrechts = New System.Windows.Forms.Label()
        Me.lbl_servo_dirrechtslinks = New System.Windows.Forms.Label()
        Me.lbl_servo_drehrichtung_lr = New System.Windows.Forms.Label()
        Me.lbl_servo_drehrichtung_rl = New System.Windows.Forms.Label()
        Me.rtb_servo_r = New System.Windows.Forms.RichTextBox()
        Me.rtb_servo_l = New System.Windows.Forms.RichTextBox()
        Me.btn_servo_laden = New System.Windows.Forms.Button()
        Me.btn_servo_speichern = New System.Windows.Forms.Button()
        Me.OpenFileDialogServo = New System.Windows.Forms.OpenFileDialog()
        Me.SaveFileDialogServo = New System.Windows.Forms.SaveFileDialog()
        Me.mbc84_txt_Hinweis = New System.Windows.Forms.Label()
        CType(Me.chart_servo, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_servo_step, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_servo_wert, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'chart_servo
        '
        ChartArea1.Name = "ChartArea1"
        Me.chart_servo.ChartAreas.Add(ChartArea1)
        Legend1.Enabled = False
        Legend1.Name = "Legend1"
        Me.chart_servo.Legends.Add(Legend1)
        Me.chart_servo.Location = New System.Drawing.Point(12, 42)
        Me.chart_servo.Name = "chart_servo"
        Series1.ChartArea = "ChartArea1"
        Series1.IsXValueIndexed = True
        Series1.Legend = "Legend1"
        Series1.Name = "Servo"
        Me.chart_servo.Series.Add(Series1)
        Me.chart_servo.Size = New System.Drawing.Size(591, 343)
        Me.chart_servo.TabIndex = 0
        Me.chart_servo.Text = "Servodiagramm"
        '
        'btn_servo_ok
        '
        Me.btn_servo_ok.Location = New System.Drawing.Point(517, 485)
        Me.btn_servo_ok.Name = "btn_servo_ok"
        Me.btn_servo_ok.Size = New System.Drawing.Size(131, 28)
        Me.btn_servo_ok.TabIndex = 1
        Me.btn_servo_ok.Text = "Ok"
        Me.btn_servo_ok.UseVisualStyleBackColor = True
        '
        'nud_servo_step
        '
        Me.nud_servo_step.Location = New System.Drawing.Point(91, 449)
        Me.nud_servo_step.Maximum = New Decimal(New Integer() {240, 0, 0, 0})
        Me.nud_servo_step.Minimum = New Decimal(New Integer() {1, 0, 0, 0})
        Me.nud_servo_step.Name = "nud_servo_step"
        Me.nud_servo_step.Size = New System.Drawing.Size(67, 20)
        Me.nud_servo_step.TabIndex = 2
        Me.nud_servo_step.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_servo_step.Value = New Decimal(New Integer() {1, 0, 0, 0})
        '
        'nud_servo_wert
        '
        Me.nud_servo_wert.Location = New System.Drawing.Point(252, 449)
        Me.nud_servo_wert.Maximum = New Decimal(New Integer() {120, 0, 0, 0})
        Me.nud_servo_wert.Name = "nud_servo_wert"
        Me.nud_servo_wert.Size = New System.Drawing.Size(69, 20)
        Me.nud_servo_wert.TabIndex = 3
        Me.nud_servo_wert.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        '
        'lbl_servo_step
        '
        Me.lbl_servo_step.AutoSize = True
        Me.lbl_servo_step.Location = New System.Drawing.Point(39, 452)
        Me.lbl_servo_step.Name = "lbl_servo_step"
        Me.lbl_servo_step.Size = New System.Drawing.Size(29, 13)
        Me.lbl_servo_step.TabIndex = 4
        Me.lbl_servo_step.Text = "Step"
        '
        'lbl_servo_wert
        '
        Me.lbl_servo_wert.AutoSize = True
        Me.lbl_servo_wert.Location = New System.Drawing.Point(198, 452)
        Me.lbl_servo_wert.Name = "lbl_servo_wert"
        Me.lbl_servo_wert.Size = New System.Drawing.Size(30, 13)
        Me.lbl_servo_wert.TabIndex = 5
        Me.lbl_servo_wert.Text = "Wert"
        '
        'lbl_servo_linksmax
        '
        Me.lbl_servo_linksmax.AutoSize = True
        Me.lbl_servo_linksmax.Location = New System.Drawing.Point(348, 452)
        Me.lbl_servo_linksmax.Name = "lbl_servo_linksmax"
        Me.lbl_servo_linksmax.Size = New System.Drawing.Size(68, 13)
        Me.lbl_servo_linksmax.TabIndex = 6
        Me.lbl_servo_linksmax.Text = "<ROT>-MAX"
        '
        'lbl_servo_rechtsmax
        '
        Me.lbl_servo_rechtsmax.AutoSize = True
        Me.lbl_servo_rechtsmax.Location = New System.Drawing.Point(497, 450)
        Me.lbl_servo_rechtsmax.Name = "lbl_servo_rechtsmax"
        Me.lbl_servo_rechtsmax.Size = New System.Drawing.Size(77, 13)
        Me.lbl_servo_rechtsmax.TabIndex = 7
        Me.lbl_servo_rechtsmax.Text = "<GRÜN>-MAX"
        '
        'txt_servo_linksmax
        '
        Me.txt_servo_linksmax.Location = New System.Drawing.Point(421, 449)
        Me.txt_servo_linksmax.Name = "txt_servo_linksmax"
        Me.txt_servo_linksmax.ReadOnly = True
        Me.txt_servo_linksmax.Size = New System.Drawing.Size(46, 20)
        Me.txt_servo_linksmax.TabIndex = 8
        Me.txt_servo_linksmax.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        '
        'txt_servo_rechtsmax
        '
        Me.txt_servo_rechtsmax.Location = New System.Drawing.Point(585, 449)
        Me.txt_servo_rechtsmax.Name = "txt_servo_rechtsmax"
        Me.txt_servo_rechtsmax.ReadOnly = True
        Me.txt_servo_rechtsmax.Size = New System.Drawing.Size(46, 20)
        Me.txt_servo_rechtsmax.TabIndex = 9
        Me.txt_servo_rechtsmax.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        '
        'btn_servo_verwerfen
        '
        Me.btn_servo_verwerfen.Location = New System.Drawing.Point(356, 485)
        Me.btn_servo_verwerfen.Name = "btn_servo_verwerfen"
        Me.btn_servo_verwerfen.Size = New System.Drawing.Size(131, 28)
        Me.btn_servo_verwerfen.TabIndex = 10
        Me.btn_servo_verwerfen.Text = "Verwerfen"
        Me.btn_servo_verwerfen.UseVisualStyleBackColor = True
        '
        'lbl_servo_anschlaglinks
        '
        Me.lbl_servo_anschlaglinks.AutoSize = True
        Me.lbl_servo_anschlaglinks.Location = New System.Drawing.Point(614, 329)
        Me.lbl_servo_anschlaglinks.Name = "lbl_servo_anschlaglinks"
        Me.lbl_servo_anschlaglinks.Size = New System.Drawing.Size(42, 13)
        Me.lbl_servo_anschlaglinks.TabIndex = 11
        Me.lbl_servo_anschlaglinks.Text = "<ROT>"
        '
        'lbl_servo_anschlagmitte
        '
        Me.lbl_servo_anschlagmitte.AutoSize = True
        Me.lbl_servo_anschlagmitte.Location = New System.Drawing.Point(619, 191)
        Me.lbl_servo_anschlagmitte.Name = "lbl_servo_anschlagmitte"
        Me.lbl_servo_anschlagmitte.Size = New System.Drawing.Size(30, 13)
        Me.lbl_servo_anschlagmitte.TabIndex = 12
        Me.lbl_servo_anschlagmitte.Text = "Mitte"
        '
        'lbl_servo_anschlagrechts
        '
        Me.lbl_servo_anschlagrechts.AutoSize = True
        Me.lbl_servo_anschlagrechts.Location = New System.Drawing.Point(609, 57)
        Me.lbl_servo_anschlagrechts.Name = "lbl_servo_anschlagrechts"
        Me.lbl_servo_anschlagrechts.Size = New System.Drawing.Size(51, 13)
        Me.lbl_servo_anschlagrechts.TabIndex = 13
        Me.lbl_servo_anschlagrechts.Text = "<GRÜN>"
        '
        'lbl_servo_dirlinksrechts
        '
        Me.lbl_servo_dirlinksrechts.AutoSize = True
        Me.lbl_servo_dirlinksrechts.Location = New System.Drawing.Point(53, 26)
        Me.lbl_servo_dirlinksrechts.Name = "lbl_servo_dirlinksrechts"
        Me.lbl_servo_dirlinksrechts.Size = New System.Drawing.Size(42, 13)
        Me.lbl_servo_dirlinksrechts.TabIndex = 14
        Me.lbl_servo_dirlinksrechts.Text = "<ROT>"
        '
        'lbl_servo_dirrechts
        '
        Me.lbl_servo_dirrechts.AutoSize = True
        Me.lbl_servo_dirrechts.Location = New System.Drawing.Point(305, 26)
        Me.lbl_servo_dirrechts.Name = "lbl_servo_dirrechts"
        Me.lbl_servo_dirrechts.Size = New System.Drawing.Size(51, 13)
        Me.lbl_servo_dirrechts.TabIndex = 15
        Me.lbl_servo_dirrechts.Text = "<GRÜN>"
        '
        'lbl_servo_dirrechtslinks
        '
        Me.lbl_servo_dirrechtslinks.AutoSize = True
        Me.lbl_servo_dirrechtslinks.Location = New System.Drawing.Point(560, 26)
        Me.lbl_servo_dirrechtslinks.Name = "lbl_servo_dirrechtslinks"
        Me.lbl_servo_dirrechtslinks.Size = New System.Drawing.Size(42, 13)
        Me.lbl_servo_dirrechtslinks.TabIndex = 16
        Me.lbl_servo_dirrechtslinks.Text = "<ROT>"
        '
        'lbl_servo_drehrichtung_lr
        '
        Me.lbl_servo_drehrichtung_lr.AutoSize = True
        Me.lbl_servo_drehrichtung_lr.Location = New System.Drawing.Point(156, 26)
        Me.lbl_servo_drehrichtung_lr.Name = "lbl_servo_drehrichtung_lr"
        Me.lbl_servo_drehrichtung_lr.Size = New System.Drawing.Size(89, 13)
        Me.lbl_servo_drehrichtung_lr.TabIndex = 17
        Me.lbl_servo_drehrichtung_lr.Text = "Drehrichtung ---->"
        '
        'lbl_servo_drehrichtung_rl
        '
        Me.lbl_servo_drehrichtung_rl.AutoSize = True
        Me.lbl_servo_drehrichtung_rl.Location = New System.Drawing.Point(418, 26)
        Me.lbl_servo_drehrichtung_rl.Name = "lbl_servo_drehrichtung_rl"
        Me.lbl_servo_drehrichtung_rl.Size = New System.Drawing.Size(89, 13)
        Me.lbl_servo_drehrichtung_rl.TabIndex = 18
        Me.lbl_servo_drehrichtung_rl.Text = "Drehrichtung ---->"
        '
        'rtb_servo_r
        '
        Me.rtb_servo_r.BackColor = System.Drawing.SystemColors.Control
        Me.rtb_servo_r.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.rtb_servo_r.Location = New System.Drawing.Point(619, 83)
        Me.rtb_servo_r.Name = "rtb_servo_r"
        Me.rtb_servo_r.Size = New System.Drawing.Size(22, 95)
        Me.rtb_servo_r.TabIndex = 21
        Me.rtb_servo_r.Text = "/\" & Global.Microsoft.VisualBasic.ChrW(10) & " |" & Global.Microsoft.VisualBasic.ChrW(10) & " |" & Global.Microsoft.VisualBasic.ChrW(10) & " |" & Global.Microsoft.VisualBasic.ChrW(10) & " |" & Global.Microsoft.VisualBasic.ChrW(10) & " |"
        '
        'rtb_servo_l
        '
        Me.rtb_servo_l.BackColor = System.Drawing.SystemColors.Control
        Me.rtb_servo_l.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.rtb_servo_l.Location = New System.Drawing.Point(619, 231)
        Me.rtb_servo_l.Name = "rtb_servo_l"
        Me.rtb_servo_l.Size = New System.Drawing.Size(22, 95)
        Me.rtb_servo_l.TabIndex = 22
        Me.rtb_servo_l.Text = " |" & Global.Microsoft.VisualBasic.ChrW(10) & " |" & Global.Microsoft.VisualBasic.ChrW(10) & " |" & Global.Microsoft.VisualBasic.ChrW(10) & " |" & Global.Microsoft.VisualBasic.ChrW(10) & " |" & Global.Microsoft.VisualBasic.ChrW(10) & "\/"
        '
        'btn_servo_laden
        '
        Me.btn_servo_laden.Location = New System.Drawing.Point(34, 485)
        Me.btn_servo_laden.Name = "btn_servo_laden"
        Me.btn_servo_laden.Size = New System.Drawing.Size(131, 28)
        Me.btn_servo_laden.TabIndex = 23
        Me.btn_servo_laden.Text = "Servoweg laden ..."
        Me.btn_servo_laden.UseVisualStyleBackColor = True
        '
        'btn_servo_speichern
        '
        Me.btn_servo_speichern.Location = New System.Drawing.Point(195, 485)
        Me.btn_servo_speichern.Name = "btn_servo_speichern"
        Me.btn_servo_speichern.Size = New System.Drawing.Size(131, 28)
        Me.btn_servo_speichern.TabIndex = 24
        Me.btn_servo_speichern.Text = "Servoweg speichern ..."
        Me.btn_servo_speichern.UseVisualStyleBackColor = True
        '
        'OpenFileDialogServo
        '
        Me.OpenFileDialogServo.FileName = "*.*"
        '
        'mbc84_txt_Hinweis
        '
        Me.mbc84_txt_Hinweis.AutoSize = True
        Me.mbc84_txt_Hinweis.Location = New System.Drawing.Point(9, 395)
        Me.mbc84_txt_Hinweis.Name = "mbc84_txt_Hinweis"
        Me.mbc84_txt_Hinweis.Size = New System.Drawing.Size(585, 39)
        Me.mbc84_txt_Hinweis.TabIndex = 25
        Me.mbc84_txt_Hinweis.Text = "Hinweis: " & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "Die Drehrichtung ist abhängig vom Servotyp. " & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "Deshalb sind hier die En" &
    "dlagen und auch der entsprechende Servoweg auf die Stellungen <ROT> und <GRÜN> b" &
    "ezogen."
        '
        'frmServo
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(684, 561)
        Me.Controls.Add(Me.mbc84_txt_Hinweis)
        Me.Controls.Add(Me.btn_servo_speichern)
        Me.Controls.Add(Me.btn_servo_laden)
        Me.Controls.Add(Me.rtb_servo_l)
        Me.Controls.Add(Me.rtb_servo_r)
        Me.Controls.Add(Me.lbl_servo_drehrichtung_rl)
        Me.Controls.Add(Me.lbl_servo_drehrichtung_lr)
        Me.Controls.Add(Me.lbl_servo_dirrechtslinks)
        Me.Controls.Add(Me.lbl_servo_dirrechts)
        Me.Controls.Add(Me.lbl_servo_dirlinksrechts)
        Me.Controls.Add(Me.lbl_servo_anschlagrechts)
        Me.Controls.Add(Me.lbl_servo_anschlagmitte)
        Me.Controls.Add(Me.lbl_servo_anschlaglinks)
        Me.Controls.Add(Me.btn_servo_verwerfen)
        Me.Controls.Add(Me.txt_servo_rechtsmax)
        Me.Controls.Add(Me.txt_servo_linksmax)
        Me.Controls.Add(Me.lbl_servo_rechtsmax)
        Me.Controls.Add(Me.lbl_servo_linksmax)
        Me.Controls.Add(Me.lbl_servo_wert)
        Me.Controls.Add(Me.lbl_servo_step)
        Me.Controls.Add(Me.nud_servo_wert)
        Me.Controls.Add(Me.nud_servo_step)
        Me.Controls.Add(Me.btn_servo_ok)
        Me.Controls.Add(Me.chart_servo)
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle
        Me.Icon = CType(resources.GetObject("$this.Icon"), System.Drawing.Icon)
        Me.MaximizeBox = False
        Me.MinimizeBox = False
        Me.Name = "frmServo"
        Me.ShowInTaskbar = False
        Me.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent
        Me.Text = "MBCAN - Wegdiagramm des Servos"
        CType(Me.chart_servo, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_servo_step, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_servo_wert, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub

    Friend WithEvents chart_servo As DataVisualization.Charting.Chart
    Friend WithEvents btn_servo_ok As Button
    Friend WithEvents nud_servo_step As NumericUpDown
    Friend WithEvents nud_servo_wert As NumericUpDown
    Friend WithEvents lbl_servo_step As Label
    Friend WithEvents lbl_servo_wert As Label
    Friend WithEvents lbl_servo_linksmax As Label
    Friend WithEvents lbl_servo_rechtsmax As Label
    Friend WithEvents txt_servo_linksmax As TextBox
    Friend WithEvents txt_servo_rechtsmax As TextBox
    Friend WithEvents btn_servo_verwerfen As Button
    Friend WithEvents lbl_servo_anschlaglinks As Label
    Friend WithEvents lbl_servo_anschlagmitte As Label
    Friend WithEvents lbl_servo_anschlagrechts As Label
    Friend WithEvents lbl_servo_dirlinksrechts As Label
    Friend WithEvents lbl_servo_dirrechts As Label
    Friend WithEvents lbl_servo_dirrechtslinks As Label
    Friend WithEvents lbl_servo_drehrichtung_lr As Label
    Friend WithEvents lbl_servo_drehrichtung_rl As Label
    Friend WithEvents rtb_servo_r As RichTextBox
    Friend WithEvents rtb_servo_l As RichTextBox
    Friend WithEvents btn_servo_laden As Button
    Friend WithEvents btn_servo_speichern As Button
    Friend WithEvents OpenFileDialogServo As OpenFileDialog
    Friend WithEvents SaveFileDialogServo As SaveFileDialog
    Friend WithEvents mbc84_txt_Hinweis As Label
End Class
