'**************************************************************************************************
'*** Popup zur Auswahl des Zielprozessors vor einem Firmware-Upgrade                            ***
'*** ATmega644P nutzt den bestehenden COMUpgradeButton()-Ablauf, die uebrigen drei Zielboards   ***
'*** (siehe MBCANLib-Sketches) haben eigene Einsprungmethoden, die aktuell noch als Platzhalter ***
'*** eine "Noch nicht implementiert"-Meldung zeigen (siehe frmMain.ComProtocol.vb).             ***
'**************************************************************************************************

Public Class frmProzessorAuswahl

    Public Enum ProzessorTyp
        ATmega644P
        RP2040Pico
        RP2040Zero
        ESP32C3
    End Enum

    Public ReadOnly Property SelectedProzessor As ProzessorTyp
        Get
            If rb_RP2040Pico.Checked Then Return ProzessorTyp.RP2040Pico
            If rb_RP2040Zero.Checked Then Return ProzessorTyp.RP2040Zero
            If rb_ESP32C3.Checked Then Return ProzessorTyp.ESP32C3
            Return ProzessorTyp.ATmega644P
        End Get
    End Property

    ' Vom Aufrufer (Register_btnUpgrade_Click) VOR ShowDialog() gesetzt - Vorauswahl anhand von
    ' ART_NR[2] (drittes Zeichen der Artikelnummer des aktuell gewaehlten Moduls, siehe dortige
    ' Kommentare in den Beispiel-Sketches). Legt zugleich verbindlich fest, welches Radiobutton
    ' waehlbar bleibt - die uebrigen drei werden in frmProzessorAuswahl_Load() gesperrt.

    Public Property VorschlagProzessor As ProzessorTyp = ProzessorTyp.ATmega644P

    Private Sub frmProzessorAuswahl_Load(sender As Object, e As EventArgs) Handles MyBase.Load
        frmMain.Enabled = False
        AddHandler Me.LocationChanged, AddressOf ClampInnerhalbEltern

        Select Case VorschlagProzessor
            Case ProzessorTyp.RP2040Pico : rb_RP2040Pico.Checked = True
            Case ProzessorTyp.RP2040Zero : rb_RP2040Zero.Checked = True
            Case ProzessorTyp.ESP32C3 : rb_ESP32C3.Checked = True
            Case Else : rb_ATmega644P.Checked = True
        End Select

        ' Die anderen drei Prozessortypen sperren - die Vorauswahl anhand der Artikelnummer ist
        ' verbindlich, ein falsch gewaehltes .bin/.hex fuer den falschen Prozessor waere sonst ein
        ' leicht moegliches Versehen (siehe VorschlagProzessor-Kommentar oben).

        rb_ATmega644P.Enabled = (VorschlagProzessor = ProzessorTyp.ATmega644P)
        rb_RP2040Pico.Enabled = (VorschlagProzessor = ProzessorTyp.RP2040Pico)
        rb_RP2040Zero.Enabled = (VorschlagProzessor = ProzessorTyp.RP2040Zero)
        rb_ESP32C3.Enabled = (VorschlagProzessor = ProzessorTyp.ESP32C3)
    End Sub

    Private Sub frmProzessorAuswahl_FormClosing(sender As Object, e As FormClosingEventArgs) Handles MyBase.FormClosing
        frmMain.Enabled = True
    End Sub

End Class
