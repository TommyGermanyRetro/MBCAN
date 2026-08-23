Partial Class frmMain
    Inherits System.Windows.Forms.Form

    Public Sub New()
        ' Dieser Aufruf ist für den Designer erforderlich.
        InitializeComponent()
    End Sub

    'Form überschreibt den Löschvorgang, um die Komponentenliste zu bereinigen.
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
    <System.Diagnostics.DebuggerStepThrough()>
    Private Sub InitializeComponent()
        Dim resources As System.ComponentModel.ComponentResourceManager = New System.ComponentModel.ComponentResourceManager(GetType(frmMain))
        Me.pnl_Verbindung = New System.Windows.Forms.Panel()
        Me.btn_Trennen = New System.Windows.Forms.Button()
        Me.btn_Verbinden = New System.Windows.Forms.Button()
        Me.txt_IPAdresse = New System.Windows.Forms.TextBox()
        Me.lbl_IPAdresse = New System.Windows.Forms.Label()
        Me.rtb_Monitor = New System.Windows.Forms.RichTextBox()
        Me.stp_Status = New System.Windows.Forms.StatusStrip()
        Me.stp_Verbindung = New System.Windows.Forms.ToolStripStatusLabel()
        Me.pnl_Verbindung.SuspendLayout()
        Me.stp_Status.SuspendLayout()
        Me.SuspendLayout()
        '
        'pnl_Verbindung
        '
        Me.pnl_Verbindung.Controls.Add(Me.btn_Trennen)
        Me.pnl_Verbindung.Controls.Add(Me.btn_Verbinden)
        Me.pnl_Verbindung.Controls.Add(Me.txt_IPAdresse)
        Me.pnl_Verbindung.Controls.Add(Me.lbl_IPAdresse)
        Me.pnl_Verbindung.Dock = System.Windows.Forms.DockStyle.Top
        Me.pnl_Verbindung.Location = New System.Drawing.Point(0, 0)
        Me.pnl_Verbindung.Name = "pnl_Verbindung"
        Me.pnl_Verbindung.Size = New System.Drawing.Size(1029, 43)
        Me.pnl_Verbindung.TabIndex = 0
        '
        'btn_Trennen
        '
        Me.btn_Trennen.Enabled = False
        Me.btn_Trennen.Location = New System.Drawing.Point(351, 10)
        Me.btn_Trennen.Name = "btn_Trennen"
        Me.btn_Trennen.Size = New System.Drawing.Size(86, 23)
        Me.btn_Trennen.TabIndex = 3
        Me.btn_Trennen.Text = "Trennen"
        Me.btn_Trennen.UseVisualStyleBackColor = True
        '
        'btn_Verbinden
        '
        Me.btn_Verbinden.Enabled = False
        Me.btn_Verbinden.Location = New System.Drawing.Point(261, 10)
        Me.btn_Verbinden.Name = "btn_Verbinden"
        Me.btn_Verbinden.Size = New System.Drawing.Size(86, 23)
        Me.btn_Verbinden.TabIndex = 2
        Me.btn_Verbinden.Text = "Verbinden"
        Me.btn_Verbinden.UseVisualStyleBackColor = True
        '
        'txt_IPAdresse
        '
        Me.txt_IPAdresse.Location = New System.Drawing.Point(120, 12)
        Me.txt_IPAdresse.Name = "txt_IPAdresse"
        Me.txt_IPAdresse.Size = New System.Drawing.Size(116, 20)
        Me.txt_IPAdresse.TabIndex = 1
        '
        'lbl_IPAdresse
        '
        Me.lbl_IPAdresse.AutoSize = True
        Me.lbl_IPAdresse.Location = New System.Drawing.Point(10, 15)
        Me.lbl_IPAdresse.Name = "lbl_IPAdresse"
        Me.lbl_IPAdresse.Size = New System.Drawing.Size(95, 13)
        Me.lbl_IPAdresse.TabIndex = 0
        Me.lbl_IPAdresse.Text = "IP-Adresse Server:"
        '
        'rtb_Monitor
        '
        Me.rtb_Monitor.BackColor = System.Drawing.Color.White
        Me.rtb_Monitor.Dock = System.Windows.Forms.DockStyle.Fill
        Me.rtb_Monitor.Font = New System.Drawing.Font("Consolas", 9.0!)
        Me.rtb_Monitor.Location = New System.Drawing.Point(0, 43)
        Me.rtb_Monitor.Name = "rtb_Monitor"
        Me.rtb_Monitor.ReadOnly = True
        Me.rtb_Monitor.Size = New System.Drawing.Size(1029, 628)
        Me.rtb_Monitor.TabIndex = 1
        Me.rtb_Monitor.Text = ""
        '
        'stp_Status
        '
        Me.stp_Status.Items.AddRange(New System.Windows.Forms.ToolStripItem() {Me.stp_Verbindung})
        Me.stp_Status.Location = New System.Drawing.Point(0, 671)
        Me.stp_Status.Name = "stp_Status"
        Me.stp_Status.Padding = New System.Windows.Forms.Padding(1, 0, 12, 0)
        Me.stp_Status.Size = New System.Drawing.Size(1029, 22)
        Me.stp_Status.TabIndex = 2
        '
        'stp_Verbindung
        '
        Me.stp_Verbindung.Name = "stp_Verbindung"
        Me.stp_Verbindung.Size = New System.Drawing.Size(53, 17)
        Me.stp_Verbindung.Text = "Getrennt"
        '
        'frmMain
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(1029, 693)
        Me.Controls.Add(Me.rtb_Monitor)
        Me.Controls.Add(Me.stp_Status)
        Me.Controls.Add(Me.pnl_Verbindung)
        Me.Icon = CType(resources.GetObject("$this.Icon"), System.Drawing.Icon)
        Me.MinimumSize = New System.Drawing.Size(602, 395)
        Me.Name = "frmMain"
        Me.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen
        Me.Text = "MBCAN CAN-Monitor (TCP)"
        Me.pnl_Verbindung.ResumeLayout(False)
        Me.pnl_Verbindung.PerformLayout()
        Me.stp_Status.ResumeLayout(False)
        Me.stp_Status.PerformLayout()
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub

    Friend WithEvents pnl_Verbindung As System.Windows.Forms.Panel
    Friend WithEvents btn_Trennen As System.Windows.Forms.Button
    Friend WithEvents btn_Verbinden As System.Windows.Forms.Button
    Friend WithEvents txt_IPAdresse As System.Windows.Forms.TextBox
    Friend WithEvents lbl_IPAdresse As System.Windows.Forms.Label
    Friend WithEvents rtb_Monitor As System.Windows.Forms.RichTextBox
    Friend WithEvents stp_Status As System.Windows.Forms.StatusStrip
    Friend WithEvents stp_Verbindung As System.Windows.Forms.ToolStripStatusLabel

End Class
