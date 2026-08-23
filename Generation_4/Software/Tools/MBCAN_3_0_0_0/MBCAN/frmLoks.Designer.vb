<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()>
Partial Class frmLoks
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
        Me.components = New System.ComponentModel.Container()
        Dim resources As System.ComponentModel.ComponentResourceManager = New System.ComponentModel.ComponentResourceManager(GetType(frmLoks))
        Me.lbl_alias = New System.Windows.Forms.Label()
        Me.nud_lok_nr = New System.Windows.Forms.NumericUpDown()
        Me.lbl_speed = New System.Windows.Forms.Label()
        Me.btn_ok = New System.Windows.Forms.Button()
        Me.btn_Refresh = New System.Windows.Forms.Button()
        Me.cb_Mapped = New System.Windows.Forms.CheckBox()
        Me.cb_Infra = New System.Windows.Forms.CheckBox()
        Me.cb_Dir_V = New System.Windows.Forms.CheckBox()
        Me.cb_Dir_R = New System.Windows.Forms.CheckBox()
        Me.cb_F0 = New System.Windows.Forms.CheckBox()
        Me.cb_F1 = New System.Windows.Forms.CheckBox()
        Me.cb_F2 = New System.Windows.Forms.CheckBox()
        Me.cb_F3 = New System.Windows.Forms.CheckBox()
        Me.cb_F4 = New System.Windows.Forms.CheckBox()
        Me.Timer_Refresh = New System.Windows.Forms.Timer(Me.components)
        Me.pb_speed = New MBCAN.MyVerticalProgessBar()
        CType(Me.nud_lok_nr, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'lbl_alias
        '
        Me.lbl_alias.AutoSize = True
        Me.lbl_alias.Location = New System.Drawing.Point(28, 49)
        Me.lbl_alias.Name = "lbl_alias"
        Me.lbl_alias.Size = New System.Drawing.Size(84, 13)
        Me.lbl_alias.TabIndex = 5
        Me.lbl_alias.Text = "Alias-Adr: .........."
        Me.lbl_alias.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'nud_lok_nr
        '
        Me.nud_lok_nr.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.nud_lok_nr.Font = New System.Drawing.Font("Microsoft Sans Serif", 14.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.nud_lok_nr.Location = New System.Drawing.Point(45, 15)
        Me.nud_lok_nr.Maximum = New Decimal(New Integer() {80, 0, 0, 0})
        Me.nud_lok_nr.Minimum = New Decimal(New Integer() {1, 0, 0, 0})
        Me.nud_lok_nr.Name = "nud_lok_nr"
        Me.nud_lok_nr.Size = New System.Drawing.Size(46, 25)
        Me.nud_lok_nr.TabIndex = 6
        Me.nud_lok_nr.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_lok_nr.Value = New Decimal(New Integer() {1, 0, 0, 0})
        '
        'lbl_speed
        '
        Me.lbl_speed.AutoSize = True
        Me.lbl_speed.Location = New System.Drawing.Point(50, 77)
        Me.lbl_speed.Name = "lbl_speed"
        Me.lbl_speed.Size = New System.Drawing.Size(38, 13)
        Me.lbl_speed.TabIndex = 8
        Me.lbl_speed.Text = "Speed"
        '
        'btn_ok
        '
        Me.btn_ok.Location = New System.Drawing.Point(158, 228)
        Me.btn_ok.Name = "btn_ok"
        Me.btn_ok.Size = New System.Drawing.Size(89, 23)
        Me.btn_ok.TabIndex = 14
        Me.btn_ok.Text = "Ok"
        Me.btn_ok.UseVisualStyleBackColor = True
        '
        'btn_Refresh
        '
        Me.btn_Refresh.Location = New System.Drawing.Point(12, 228)
        Me.btn_Refresh.Name = "btn_Refresh"
        Me.btn_Refresh.Size = New System.Drawing.Size(89, 23)
        Me.btn_Refresh.TabIndex = 15
        Me.btn_Refresh.Text = "Refresh"
        Me.btn_Refresh.UseVisualStyleBackColor = True
        '
        'cb_Mapped
        '
        Me.cb_Mapped.AutoSize = True
        Me.cb_Mapped.Enabled = False
        Me.cb_Mapped.Location = New System.Drawing.Point(139, 16)
        Me.cb_Mapped.Name = "cb_Mapped"
        Me.cb_Mapped.Size = New System.Drawing.Size(65, 17)
        Me.cb_Mapped.TabIndex = 16
        Me.cb_Mapped.Text = "Mapped"
        Me.cb_Mapped.UseVisualStyleBackColor = True
        '
        'cb_Infra
        '
        Me.cb_Infra.AutoSize = True
        Me.cb_Infra.Enabled = False
        Me.cb_Infra.Location = New System.Drawing.Point(139, 32)
        Me.cb_Infra.Name = "cb_Infra"
        Me.cb_Infra.Size = New System.Drawing.Size(99, 17)
        Me.cb_Infra.TabIndex = 17
        Me.cb_Infra.Text = "INFRA/TRACK"
        Me.cb_Infra.UseVisualStyleBackColor = True
        '
        'cb_Dir_V
        '
        Me.cb_Dir_V.AutoSize = True
        Me.cb_Dir_V.Enabled = False
        Me.cb_Dir_V.Location = New System.Drawing.Point(139, 67)
        Me.cb_Dir_V.Name = "cb_Dir_V"
        Me.cb_Dir_V.Size = New System.Drawing.Size(67, 17)
        Me.cb_Dir_V.TabIndex = 18
        Me.cb_Dir_V.Text = "Vorwärts"
        Me.cb_Dir_V.UseVisualStyleBackColor = True
        '
        'cb_Dir_R
        '
        Me.cb_Dir_R.AutoSize = True
        Me.cb_Dir_R.Enabled = False
        Me.cb_Dir_R.Location = New System.Drawing.Point(139, 83)
        Me.cb_Dir_R.Name = "cb_Dir_R"
        Me.cb_Dir_R.Size = New System.Drawing.Size(77, 17)
        Me.cb_Dir_R.TabIndex = 19
        Me.cb_Dir_R.Text = "Rückwärts"
        Me.cb_Dir_R.UseVisualStyleBackColor = True
        '
        'cb_F0
        '
        Me.cb_F0.AutoSize = True
        Me.cb_F0.Enabled = False
        Me.cb_F0.Location = New System.Drawing.Point(139, 117)
        Me.cb_F0.Name = "cb_F0"
        Me.cb_F0.Size = New System.Drawing.Size(82, 17)
        Me.cb_F0.TabIndex = 20
        Me.cb_F0.Text = "Funktion F0"
        Me.cb_F0.UseVisualStyleBackColor = True
        '
        'cb_F1
        '
        Me.cb_F1.AutoSize = True
        Me.cb_F1.Enabled = False
        Me.cb_F1.Location = New System.Drawing.Point(139, 133)
        Me.cb_F1.Name = "cb_F1"
        Me.cb_F1.Size = New System.Drawing.Size(82, 17)
        Me.cb_F1.TabIndex = 21
        Me.cb_F1.Text = "Funktion F1"
        Me.cb_F1.UseVisualStyleBackColor = True
        '
        'cb_F2
        '
        Me.cb_F2.AutoSize = True
        Me.cb_F2.Enabled = False
        Me.cb_F2.Location = New System.Drawing.Point(139, 149)
        Me.cb_F2.Name = "cb_F2"
        Me.cb_F2.Size = New System.Drawing.Size(82, 17)
        Me.cb_F2.TabIndex = 22
        Me.cb_F2.Text = "Funktion F2"
        Me.cb_F2.UseVisualStyleBackColor = True
        '
        'cb_F3
        '
        Me.cb_F3.AutoSize = True
        Me.cb_F3.Enabled = False
        Me.cb_F3.Location = New System.Drawing.Point(139, 165)
        Me.cb_F3.Name = "cb_F3"
        Me.cb_F3.Size = New System.Drawing.Size(82, 17)
        Me.cb_F3.TabIndex = 23
        Me.cb_F3.Text = "Funktion F3"
        Me.cb_F3.UseVisualStyleBackColor = True
        '
        'cb_F4
        '
        Me.cb_F4.AutoSize = True
        Me.cb_F4.Enabled = False
        Me.cb_F4.Location = New System.Drawing.Point(139, 181)
        Me.cb_F4.Name = "cb_F4"
        Me.cb_F4.Size = New System.Drawing.Size(82, 17)
        Me.cb_F4.TabIndex = 24
        Me.cb_F4.Text = "Funktion F4"
        Me.cb_F4.UseVisualStyleBackColor = True
        '
        'Timer_Refresh
        '
        Me.Timer_Refresh.Interval = 500
        '
        'pb_speed
        '
        Me.pb_speed.Enabled = False
        Me.pb_speed.Location = New System.Drawing.Point(42, 95)
        Me.pb_speed.Maximum = 15
        Me.pb_speed.Name = "pb_speed"
        Me.pb_speed.Size = New System.Drawing.Size(52, 99)
        Me.pb_speed.Step = 1
        Me.pb_speed.TabIndex = 13
        '
        'frmLoks
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.BackColor = System.Drawing.Color.White
        Me.ClientSize = New System.Drawing.Size(259, 263)
        Me.Controls.Add(Me.cb_F4)
        Me.Controls.Add(Me.cb_F3)
        Me.Controls.Add(Me.cb_F2)
        Me.Controls.Add(Me.cb_F1)
        Me.Controls.Add(Me.cb_F0)
        Me.Controls.Add(Me.cb_Dir_R)
        Me.Controls.Add(Me.cb_Dir_V)
        Me.Controls.Add(Me.cb_Infra)
        Me.Controls.Add(Me.cb_Mapped)
        Me.Controls.Add(Me.btn_Refresh)
        Me.Controls.Add(Me.btn_ok)
        Me.Controls.Add(Me.pb_speed)
        Me.Controls.Add(Me.lbl_speed)
        Me.Controls.Add(Me.nud_lok_nr)
        Me.Controls.Add(Me.lbl_alias)
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle
        Me.Icon = CType(resources.GetObject("$this.Icon"), System.Drawing.Icon)
        Me.MaximizeBox = False
        Me.MinimizeBox = False
        Me.Name = "frmLoks"
        Me.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent
        Me.Text = "MBCAN - Übersicht Lokomotiven"
        CType(Me.nud_lok_nr, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Friend WithEvents lbl_alias As Label
    Friend WithEvents nud_lok_nr As NumericUpDown
    Friend WithEvents lbl_speed As Label
    Friend WithEvents pb_speed As MyVerticalProgessBar
    Friend WithEvents btn_ok As Button
    Friend WithEvents btn_Refresh As Button
    Friend WithEvents cb_Mapped As CheckBox
    Friend WithEvents cb_Infra As CheckBox
    Friend WithEvents cb_Dir_V As CheckBox
    Friend WithEvents cb_Dir_R As CheckBox
    Friend WithEvents cb_F0 As CheckBox
    Friend WithEvents cb_F1 As CheckBox
    Friend WithEvents cb_F2 As CheckBox
    Friend WithEvents cb_F3 As CheckBox
    Friend WithEvents cb_F4 As CheckBox
    Friend WithEvents Timer_Refresh As Timer
End Class
