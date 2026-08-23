<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmProzessorAuswahl
    Inherits System.Windows.Forms.Form

    'Das Formular überschreibt den Löschvorgang, um die Komponentenliste zu bereinigen.
    <System.Diagnostics.DebuggerNonUserCode()> _
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
    <System.Diagnostics.DebuggerStepThrough()> _
    Private Sub InitializeComponent()
        Me.lbl_Hinweis = New System.Windows.Forms.Label()
        Me.gb_Prozessor = New System.Windows.Forms.GroupBox()
        Me.rb_ATmega644P = New System.Windows.Forms.RadioButton()
        Me.rb_RP2040Pico = New System.Windows.Forms.RadioButton()
        Me.rb_RP2040Zero = New System.Windows.Forms.RadioButton()
        Me.rb_ESP32C3 = New System.Windows.Forms.RadioButton()
        Me.btn_OK = New System.Windows.Forms.Button()
        Me.btn_Abbrechen = New System.Windows.Forms.Button()
        Me.gb_Prozessor.SuspendLayout()
        Me.SuspendLayout()
        '
        'lbl_Hinweis
        '
        Me.lbl_Hinweis.AutoSize = True
        Me.lbl_Hinweis.Location = New System.Drawing.Point(13, 9)
        Me.lbl_Hinweis.Name = "lbl_Hinweis"
        Me.lbl_Hinweis.Size = New System.Drawing.Size(296, 13)
        Me.lbl_Hinweis.TabIndex = 3
        Me.lbl_Hinweis.Text = "Der aktuelle Prozessortyp gemäß Artikelnummer lautet:"
        '
        'gb_Prozessor
        '
        Me.gb_Prozessor.Controls.Add(Me.rb_ATmega644P)
        Me.gb_Prozessor.Controls.Add(Me.rb_RP2040Pico)
        Me.gb_Prozessor.Controls.Add(Me.rb_RP2040Zero)
        Me.gb_Prozessor.Controls.Add(Me.rb_ESP32C3)
        Me.gb_Prozessor.Location = New System.Drawing.Point(12, 42)
        Me.gb_Prozessor.Name = "gb_Prozessor"
        Me.gb_Prozessor.Size = New System.Drawing.Size(296, 140)
        Me.gb_Prozessor.TabIndex = 0
        Me.gb_Prozessor.TabStop = False
        Me.gb_Prozessor.Text = "Zielprozessor"
        '
        'rb_ATmega644P
        '
        Me.rb_ATmega644P.AutoSize = True
        Me.rb_ATmega644P.Checked = True
        Me.rb_ATmega644P.Location = New System.Drawing.Point(16, 24)
        Me.rb_ATmega644P.Name = "rb_ATmega644P"
        Me.rb_ATmega644P.Size = New System.Drawing.Size(94, 17)
        Me.rb_ATmega644P.TabIndex = 0
        Me.rb_ATmega644P.TabStop = True
        Me.rb_ATmega644P.Text = "ATmega644P"
        Me.rb_ATmega644P.UseVisualStyleBackColor = True
        '
        'rb_RP2040Pico
        '
        Me.rb_RP2040Pico.AutoSize = True
        Me.rb_RP2040Pico.Location = New System.Drawing.Point(16, 54)
        Me.rb_RP2040Pico.Name = "rb_RP2040Pico"
        Me.rb_RP2040Pico.Size = New System.Drawing.Size(178, 17)
        Me.rb_RP2040Pico.TabIndex = 1
        Me.rb_RP2040Pico.Text = "RP2040 (Raspberry Pi Pico)"
        Me.rb_RP2040Pico.UseVisualStyleBackColor = True
        '
        'rb_RP2040Zero
        '
        Me.rb_RP2040Zero.AutoSize = True
        Me.rb_RP2040Zero.Location = New System.Drawing.Point(16, 84)
        Me.rb_RP2040Zero.Name = "rb_RP2040Zero"
        Me.rb_RP2040Zero.Size = New System.Drawing.Size(93, 17)
        Me.rb_RP2040Zero.TabIndex = 2
        Me.rb_RP2040Zero.Text = "RP2040 Zero"
        Me.rb_RP2040Zero.UseVisualStyleBackColor = True
        '
        'rb_ESP32C3
        '
        Me.rb_ESP32C3.AutoSize = True
        Me.rb_ESP32C3.Location = New System.Drawing.Point(16, 114)
        Me.rb_ESP32C3.Name = "rb_ESP32C3"
        Me.rb_ESP32C3.Size = New System.Drawing.Size(73, 17)
        Me.rb_ESP32C3.TabIndex = 3
        Me.rb_ESP32C3.Text = "ESP32-C3"
        Me.rb_ESP32C3.UseVisualStyleBackColor = True
        '
        'btn_OK
        '
        Me.btn_OK.DialogResult = System.Windows.Forms.DialogResult.OK
        Me.btn_OK.Location = New System.Drawing.Point(152, 196)
        Me.btn_OK.Name = "btn_OK"
        Me.btn_OK.Size = New System.Drawing.Size(75, 23)
        Me.btn_OK.TabIndex = 1
        Me.btn_OK.Text = "OK"
        Me.btn_OK.UseVisualStyleBackColor = True
        '
        'btn_Abbrechen
        '
        Me.btn_Abbrechen.DialogResult = System.Windows.Forms.DialogResult.Cancel
        Me.btn_Abbrechen.Location = New System.Drawing.Point(233, 196)
        Me.btn_Abbrechen.Name = "btn_Abbrechen"
        Me.btn_Abbrechen.Size = New System.Drawing.Size(75, 23)
        Me.btn_Abbrechen.TabIndex = 2
        Me.btn_Abbrechen.Text = "Abbrechen"
        Me.btn_Abbrechen.UseVisualStyleBackColor = True
        '
        'frmProzessorAuswahl
        '
        Me.AcceptButton = Me.btn_OK
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.CancelButton = Me.btn_Abbrechen
        Me.ClientSize = New System.Drawing.Size(320, 231)
        Me.Controls.Add(Me.btn_Abbrechen)
        Me.Controls.Add(Me.btn_OK)
        Me.Controls.Add(Me.gb_Prozessor)
        Me.Controls.Add(Me.lbl_Hinweis)
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog
        Me.Icon = Global.MBCAN.My.Resources.Resources.mbc_icon
        Me.MaximizeBox = False
        Me.MinimizeBox = False
        Me.Name = "frmProzessorAuswahl"
        Me.ShowInTaskbar = False
        Me.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent
        Me.Text = "MBCAN - Zielprozessor auswählen"
        Me.gb_Prozessor.ResumeLayout(False)
        Me.gb_Prozessor.PerformLayout()
        Me.ResumeLayout(False)

    End Sub

    Friend WithEvents lbl_Hinweis As System.Windows.Forms.Label
    Friend WithEvents gb_Prozessor As System.Windows.Forms.GroupBox
    Friend WithEvents rb_ATmega644P As System.Windows.Forms.RadioButton
    Friend WithEvents rb_RP2040Pico As System.Windows.Forms.RadioButton
    Friend WithEvents rb_RP2040Zero As System.Windows.Forms.RadioButton
    Friend WithEvents rb_ESP32C3 As System.Windows.Forms.RadioButton
    Friend WithEvents btn_OK As System.Windows.Forms.Button
    Friend WithEvents btn_Abbrechen As System.Windows.Forms.Button

End Class
