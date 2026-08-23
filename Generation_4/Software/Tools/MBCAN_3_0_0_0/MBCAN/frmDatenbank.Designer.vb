<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmDatenbank
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
        Me.dgv_Datenbank = New System.Windows.Forms.DataGridView()
        Me.col_db_Status = New System.Windows.Forms.DataGridViewImageColumn()
        Me.col_db_Typ = New System.Windows.Forms.DataGridViewTextBoxColumn()
        Me.col_db_Name = New System.Windows.Forms.DataGridViewTextBoxColumn()
        Me.col_db_ArtNum = New System.Windows.Forms.DataGridViewTextBoxColumn()
        Me.col_db_Prozessor = New System.Windows.Forms.DataGridViewTextBoxColumn()
        Me.col_db_SerNr = New System.Windows.Forms.DataGridViewTextBoxColumn()
        Me.col_db_GUiD = New System.Windows.Forms.DataGridViewTextBoxColumn()
        Me.col_db_SW = New System.Windows.Forms.DataGridViewTextBoxColumn()
        Me.col_db_HW = New System.Windows.Forms.DataGridViewTextBoxColumn()
        Me.col_db_MMAddr = New System.Windows.Forms.DataGridViewTextBoxColumn()
        Me.col_db_S88ModNr = New System.Windows.Forms.DataGridViewTextBoxColumn()
        Me.col_db_Plugged = New System.Windows.Forms.DataGridViewTextBoxColumn()
        Me.col_db_Spannung = New System.Windows.Forms.DataGridViewTextBoxColumn()
        Me.col_db_Temperatur = New System.Windows.Forms.DataGridViewTextBoxColumn()
        Me.btn_db_Aktualisieren = New System.Windows.Forms.Button()
        Me.btn_db_Schliessen = New System.Windows.Forms.Button()
        CType(Me.dgv_Datenbank, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'dgv_Datenbank
        '
        Me.dgv_Datenbank.AllowUserToAddRows = False
        Me.dgv_Datenbank.AllowUserToDeleteRows = False
        Me.dgv_Datenbank.AllowUserToResizeRows = False
        Me.dgv_Datenbank.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
            Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.dgv_Datenbank.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
        Me.dgv_Datenbank.Columns.AddRange(New System.Windows.Forms.DataGridViewColumn() {Me.col_db_Status, Me.col_db_Typ, Me.col_db_Name, Me.col_db_ArtNum, Me.col_db_Prozessor, Me.col_db_SerNr, Me.col_db_GUiD, Me.col_db_SW, Me.col_db_HW, Me.col_db_MMAddr, Me.col_db_S88ModNr, Me.col_db_Plugged, Me.col_db_Spannung, Me.col_db_Temperatur})
        Me.dgv_Datenbank.Location = New System.Drawing.Point(12, 12)
        Me.dgv_Datenbank.Name = "dgv_Datenbank"
        Me.dgv_Datenbank.ReadOnly = True
        Me.dgv_Datenbank.RowHeadersWidth = 25
        Me.dgv_Datenbank.ScrollBars = System.Windows.Forms.ScrollBars.Both
        Me.dgv_Datenbank.Size = New System.Drawing.Size(958, 470)
        Me.dgv_Datenbank.TabIndex = 0
        '
        'col_db_Status
        '
        Me.col_db_Status.HeaderText = "Status"
        Me.col_db_Status.ImageLayout = System.Windows.Forms.DataGridViewImageCellLayout.Zoom
        Me.col_db_Status.MinimumWidth = 8
        Me.col_db_Status.Name = "col_db_Status"
        Me.col_db_Status.ReadOnly = True
        Me.col_db_Status.Resizable = System.Windows.Forms.DataGridViewTriState.True
        Me.col_db_Status.Width = 50
        '
        'col_db_Typ
        '
        Me.col_db_Typ.HeaderText = "Typ"
        Me.col_db_Typ.MinimumWidth = 8
        Me.col_db_Typ.Name = "col_db_Typ"
        Me.col_db_Typ.ReadOnly = True
        Me.col_db_Typ.Width = 55
        '
        'col_db_Name
        '
        Me.col_db_Name.HeaderText = "Name"
        Me.col_db_Name.MinimumWidth = 8
        Me.col_db_Name.Name = "col_db_Name"
        Me.col_db_Name.ReadOnly = True
        Me.col_db_Name.Width = 120
        '
        'col_db_ArtNum
        '
        Me.col_db_ArtNum.HeaderText = "Artikelnummer"
        Me.col_db_ArtNum.MinimumWidth = 8
        Me.col_db_ArtNum.Name = "col_db_ArtNum"
        Me.col_db_ArtNum.ReadOnly = True
        Me.col_db_ArtNum.Width = 80
        '
        'col_db_Prozessor
        '
        Me.col_db_Prozessor.HeaderText = "Prozessor"
        Me.col_db_Prozessor.MinimumWidth = 8
        Me.col_db_Prozessor.Name = "col_db_Prozessor"
        Me.col_db_Prozessor.ReadOnly = True
        Me.col_db_Prozessor.Width = 130
        '
        'col_db_SerNr
        '
        Me.col_db_SerNr.HeaderText = "Seriennummer"
        Me.col_db_SerNr.MinimumWidth = 8
        Me.col_db_SerNr.Name = "col_db_SerNr"
        Me.col_db_SerNr.ReadOnly = True
        Me.col_db_SerNr.Width = 130
        '
        'col_db_GUiD
        '
        Me.col_db_GUiD.HeaderText = "GUiD"
        Me.col_db_GUiD.MinimumWidth = 8
        Me.col_db_GUiD.Name = "col_db_GUiD"
        Me.col_db_GUiD.ReadOnly = True
        Me.col_db_GUiD.Width = 80
        '
        'col_db_SW
        '
        Me.col_db_SW.HeaderText = "SW"
        Me.col_db_SW.MinimumWidth = 8
        Me.col_db_SW.Name = "col_db_SW"
        Me.col_db_SW.ReadOnly = True
        Me.col_db_SW.Width = 50
        '
        'col_db_HW
        '
        Me.col_db_HW.HeaderText = "HW"
        Me.col_db_HW.MinimumWidth = 8
        Me.col_db_HW.Name = "col_db_HW"
        Me.col_db_HW.ReadOnly = True
        Me.col_db_HW.Width = 70
        '
        'col_db_MMAddr
        '
        Me.col_db_MMAddr.HeaderText = "MM-Adresse"
        Me.col_db_MMAddr.MinimumWidth = 8
        Me.col_db_MMAddr.Name = "col_db_MMAddr"
        Me.col_db_MMAddr.ReadOnly = True
        Me.col_db_MMAddr.Width = 80
        '
        'col_db_S88ModNr
        '
        Me.col_db_S88ModNr.HeaderText = "s88-Modulnr."
        Me.col_db_S88ModNr.MinimumWidth = 8
        Me.col_db_S88ModNr.Name = "col_db_S88ModNr"
        Me.col_db_S88ModNr.ReadOnly = True
        Me.col_db_S88ModNr.Width = 80
        '
        'col_db_Plugged
        '
        Me.col_db_Plugged.HeaderText = "Steckernetzteil"
        Me.col_db_Plugged.MinimumWidth = 8
        Me.col_db_Plugged.Name = "col_db_Plugged"
        Me.col_db_Plugged.ReadOnly = True
        Me.col_db_Plugged.Width = 90
        '
        'col_db_Spannung
        '
        Me.col_db_Spannung.HeaderText = "Spannung"
        Me.col_db_Spannung.MinimumWidth = 8
        Me.col_db_Spannung.Name = "col_db_Spannung"
        Me.col_db_Spannung.ReadOnly = True
        Me.col_db_Spannung.Width = 70
        '
        'col_db_Temperatur
        '
        Me.col_db_Temperatur.HeaderText = "Temperatur"
        Me.col_db_Temperatur.MinimumWidth = 8
        Me.col_db_Temperatur.Name = "col_db_Temperatur"
        Me.col_db_Temperatur.ReadOnly = True
        Me.col_db_Temperatur.Width = 70
        '
        'btn_db_Aktualisieren
        '
        Me.btn_db_Aktualisieren.Anchor = CType((System.Windows.Forms.AnchorStyles.Bottom Or System.Windows.Forms.AnchorStyles.Left), System.Windows.Forms.AnchorStyles)
        Me.btn_db_Aktualisieren.Location = New System.Drawing.Point(12, 490)
        Me.btn_db_Aktualisieren.Name = "btn_db_Aktualisieren"
        Me.btn_db_Aktualisieren.Size = New System.Drawing.Size(110, 23)
        Me.btn_db_Aktualisieren.TabIndex = 1
        Me.btn_db_Aktualisieren.Text = "Aktualisieren"
        Me.btn_db_Aktualisieren.UseVisualStyleBackColor = True
        '
        'btn_db_Schliessen
        '
        Me.btn_db_Schliessen.Anchor = CType((System.Windows.Forms.AnchorStyles.Bottom Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.btn_db_Schliessen.Location = New System.Drawing.Point(895, 490)
        Me.btn_db_Schliessen.Name = "btn_db_Schliessen"
        Me.btn_db_Schliessen.Size = New System.Drawing.Size(75, 23)
        Me.btn_db_Schliessen.TabIndex = 2
        Me.btn_db_Schliessen.Text = "Schließen"
        Me.btn_db_Schliessen.UseVisualStyleBackColor = True
        '
        'frmDatenbank
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(982, 525)
        Me.Controls.Add(Me.btn_db_Schliessen)
        Me.Controls.Add(Me.btn_db_Aktualisieren)
        Me.Controls.Add(Me.dgv_Datenbank)
        Me.Icon = Global.MBCAN.My.Resources.Resources.mbc_icon
        Me.MinimumSize = New System.Drawing.Size(500, 300)
        Me.Name = "frmDatenbank"
        Me.ShowInTaskbar = False
        Me.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent
        Me.Text = "MBCAN - Datenbank"
        CType(Me.dgv_Datenbank, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)

    End Sub

    Friend WithEvents dgv_Datenbank As System.Windows.Forms.DataGridView
    Friend WithEvents col_db_Status As System.Windows.Forms.DataGridViewImageColumn
    Friend WithEvents col_db_Typ As System.Windows.Forms.DataGridViewTextBoxColumn
    Friend WithEvents col_db_Name As System.Windows.Forms.DataGridViewTextBoxColumn
    Friend WithEvents col_db_ArtNum As System.Windows.Forms.DataGridViewTextBoxColumn
    Friend WithEvents col_db_Prozessor As System.Windows.Forms.DataGridViewTextBoxColumn
    Friend WithEvents col_db_SerNr As System.Windows.Forms.DataGridViewTextBoxColumn
    Friend WithEvents col_db_GUiD As System.Windows.Forms.DataGridViewTextBoxColumn
    Friend WithEvents col_db_SW As System.Windows.Forms.DataGridViewTextBoxColumn
    Friend WithEvents col_db_HW As System.Windows.Forms.DataGridViewTextBoxColumn
    Friend WithEvents col_db_MMAddr As System.Windows.Forms.DataGridViewTextBoxColumn
    Friend WithEvents col_db_S88ModNr As System.Windows.Forms.DataGridViewTextBoxColumn
    Friend WithEvents col_db_Plugged As System.Windows.Forms.DataGridViewTextBoxColumn
    Friend WithEvents col_db_Spannung As System.Windows.Forms.DataGridViewTextBoxColumn
    Friend WithEvents col_db_Temperatur As System.Windows.Forms.DataGridViewTextBoxColumn
    Friend WithEvents btn_db_Aktualisieren As System.Windows.Forms.Button
    Friend WithEvents btn_db_Schliessen As System.Windows.Forms.Button

End Class
