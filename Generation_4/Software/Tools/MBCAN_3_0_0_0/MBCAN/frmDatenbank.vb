'**************************************************************************************************
'*** Schreibgeschuetzte Uebersicht aller Module in der Moduldatenbank                           ***
'*** Aufruf ueber Menue System / Datenbank zeigen. Zeigt je Modul die gemeinsamen Stammdaten    ***
'*** sowie MM-Adresse/s88-Modulnummer ueber das IMbcModul-Interface - dadurch funktioniert die  ***
'*** Befuellung einheitlich fuer alle 14 Modultypen, ohne deren Typ unterscheiden zu muessen.   ***
'**************************************************************************************************

Public Class frmDatenbank

    Private Sub frmDatenbank_Load(sender As Object, e As EventArgs) Handles MyBase.Load
        frmMain.Enabled = False
        AddHandler Me.LocationChanged, AddressOf ClampInnerhalbEltern
        LadeDatenbank()
    End Sub

    Private Sub frmDatenbank_FormClosing(sender As Object, e As FormClosingEventArgs) Handles MyBase.FormClosing
        frmMain.Enabled = True
    End Sub

    Private Sub btn_db_Aktualisieren_Click(sender As Object, e As EventArgs) Handles btn_db_Aktualisieren.Click
        LadeDatenbank()
    End Sub

    Private Sub btn_db_Schliessen_Click(sender As Object, e As EventArgs) Handles btn_db_Schliessen.Click
        Me.Close()
    End Sub

    '**********************************************************************************************
    '*** Tabelle aus der Moduldatenbank neu befuellen                                           ***
    '**********************************************************************************************

    Private Sub LadeDatenbank()

        Dim IndexT As Integer
        Dim IndexM As Integer
        Dim modul As Object
        Dim mmAddr As String
        Dim s88ModNr As String

        dgv_Datenbank.Rows.Clear()

        For IndexT = 1 To MaxTyp
            For IndexM = 1 To MBC.ModulTyp(IndexT).Count

                modul = MBC.ModulTyp(IndexT).Modul(IndexM)

                ' MM-Adresse/s88-Modulnummer ueber das gemeinsame Interface holen - fuer Modultypen,
                ' die das jeweils nicht nutzen, liefert dies einen bedeutungslosen Platzhalterwert,
                ' der hier unveraendert mit angezeigt wird (kein Fehler, nur nicht relevant)

                Try
                    mmAddr = CStr(modul.Get_MMAddr(0))
                Catch ex As Exception
                    mmAddr = ""
                End Try

                Try
                    s88ModNr = CStr(modul.Get_S88ModNr())
                Catch ex As Exception
                    s88ModNr = ""
                End Try

                Dim RowIndex As Integer = dgv_Datenbank.Rows.Add(
                    StatusImage(modul.Status),
                    MBC.ModulTyp(IndexT).Bezeichner,
                    modul.Name,
                    modul.ArtNum,
                    ProzessorText(modul.ArtNum),
                    modul.SerNr,
                    modul.GUiD,
                    modul.SW,
                    modul.HW,
                    mmAddr,
                    s88ModNr,
                    modul.Plugged,
                    modul.Spannung,
                    modul.Temperatur)

                ' Statustext weiterhin als Tooltip verfuegbar machen (Icon allein ist nicht
                ' barrierefrei und schwerer zu unterscheiden als im groesseren Baum-Icon)

                dgv_Datenbank.Rows(RowIndex).Cells(col_db_Status.Index).ToolTipText = StatusText(modul.Status)

            Next
        Next

    End Sub

    '**********************************************************************************************
    '*** Numerischen Modulstatus in dasselbe Icon uebersetzen, das auch im Strukturbaum         ***
    '*** verwendet wird (siehe Strukturbaum_MenueIconChange: ImageIndex = Status auf MenueImage,***
    '*** Reihenfolge ImGLEIS/ImMODUL/ImHAKEN/ImPUNKT/ImKREUZ/ImDELETE = Status 0..5)            ***
    '**********************************************************************************************

    Private Function StatusImage(status As Integer) As Image

        Select Case status
            Case IsComplete
                Return ImHAKEN
            Case IsAlive
                Return ImPUNKT
            Case IsMissing
                Return ImKREUZ
            Case IsDeleted
                Return ImDELETE
            Case Else
                Return ImMODUL
        End Select

    End Function

    '**********************************************************************************************
    '*** Drittes Zeichen der Artikelnummer (ART_NR[2] im Sketch) in den Prozessornamen          ***
    '*** uebersetzen - siehe VorschlagProzessor in frmProzessorAuswahl.vb fuer dieselbe         ***
    '*** Zuordnung.                                                                             ***
    '*** Die Prozessornamen selbst bleiben unlokalisiert (Hardware-Bezeichner, wie schon bei den***
    '*** Radiobuttons in frmProzessorAuswahl).                                                  ***
    '**********************************************************************************************

    Private Function ProzessorText(artNum As String) As String

        If Len(artNum) < 3 Then Return ""

        Select Case Mid(artNum, 3, 1)
            Case "1" : Return "ESP32-C3"
            Case "2" : Return "RP2040 (Raspberry Pi Pico)"
            Case "3" : Return "RP2040 Zero"
            Case Else : Return "ATmega644P"
        End Select

    End Function

    '**********************************************************************************************
    '*** Numerischen Modulstatus in lesbaren, lokalisierten Text uebersetzen (als Tooltip)      ***
    '**********************************************************************************************

    Private Function StatusText(status As Integer) As String

        Select Case status
            Case IsComplete
                Return Sprachen(i_DATENBANK, i_DATENBANK_Status_Angemeldet)
            Case IsAlive
                Return Sprachen(i_DATENBANK, i_DATENBANK_Status_Aktiv)
            Case IsMissing
                Return Sprachen(i_DATENBANK, i_DATENBANK_Status_Fehlt)
            Case IsDeleted
                Return Sprachen(i_DATENBANK, i_DATENBANK_Status_Geloescht)
            Case Else
                Return Sprachen(i_DATENBANK, i_DATENBANK_Status_Unbekannt)
        End Select

    End Function

End Class
