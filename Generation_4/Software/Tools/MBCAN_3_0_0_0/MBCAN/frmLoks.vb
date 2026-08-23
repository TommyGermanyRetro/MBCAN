Public Class frmLoks

    Private Sub frmLoks_Load(sender As Object, e As EventArgs) Handles MyBase.Load

        frmMain.Enabled = False
        AddHandler Me.LocationChanged, AddressOf ClampInnerhalbEltern

        nud_lok_nr.Value = 1

        If (AktTyp <> 10) Then Exit Sub

    End Sub

    Private Sub frmLoks_FormClosing(sender As Object, e As FormClosingEventArgs) Handles MyBase.FormClosing
        frmMain.Enabled = True
    End Sub

    Private Sub lok_fill()

        Dim lok As Integer

        If (AktTyp <> 10) Then Exit Sub

        ' Bestimme anzuzeigende Lok

        If (nud_lok_nr.Value <> 80) Then
            lok = nud_lok_nr.Value * 10 + MBC.ModulTyp(AktTyp).Modul(AktNr).Lokspeicher
        Else
            lok = MBC.ModulTyp(AktTyp).Modul(AktNr).Lokspeicher
        End If

        ' Zeige Alias-Adresse

        lbl_alias.Text = "Alias: " & Str((Val("&H" & MBC.ModulTyp(AktTyp).Modul(AktNr).SYS_ARRAY(lok + 4)) * 2 ^ 32 + Val("&H" & MBC.ModulTyp(AktTyp).Modul(AktNr).SYS_ARRAY(lok + 5)) * 2 ^ 16 + Val("&H" & MBC.ModulTyp(AktTyp).Modul(AktNr).SYS_ARRAY(lok + 6)) * 2 ^ 8 + Val("&H" & MBC.ModulTyp(AktTyp).Modul(AktNr).SYS_ARRAY(lok + 7))))

        ' Zeige Geschwindigkeit

        pb_speed.Value = Val("&H" & MBC.ModulTyp(AktTyp).Modul(AktNr).SYS_ARRAY(lok))

        ' Zeige Richtung

        If MBC.ModulTyp(AktTyp).Modul(AktNr).SYS_ARRAY(lok + 1) = 1 Then
            cb_Dir_V.Checked = True
            cb_Dir_R.Checked = False
        Else
            cb_Dir_V.Checked = False
            cb_Dir_R.Checked = True
        End If

        ' Zeige ob Lok gemappt

        If MBC.ModulTyp(AktTyp).Modul(AktNr).SYS_ARRAY(lok + 8) = 1 Then
            cb_Mapped.Checked = True
        Else
            cb_Mapped.Checked = False
        End If

        ' Zeige ob Lok durch INFRA/TRACK belegt ist

        If MBC.ModulTyp(AktTyp).Modul(AktNr).SYS_ARRAY(lok + 9) = 1 Then
            cb_Infra.Checked = True
        Else
            cb_Infra.Checked = False
        End If

        ' Zeige Funktionen an

        If Val("&H" & MBC.ModulTyp(AktTyp).Modul(AktNr).SYS_ARRAY(lok + 2)) = 1 Then
            cb_F0.Checked = True
        Else
            cb_F0.Checked = False
        End If

        If (Val("&H" & MBC.ModulTyp(AktTyp).Modul(AktNr).SYS_ARRAY(lok + 3)) And &H1) = 1 Then
            cb_F1.Checked = True
        Else
            cb_F1.Checked = False
        End If

        If (Val("&H" & MBC.ModulTyp(AktTyp).Modul(AktNr).SYS_ARRAY(lok + 3)) And &H2) = 2 Then
            cb_F2.Checked = True
        Else
            cb_F2.Checked = False
        End If

        If (Val("&H" & MBC.ModulTyp(AktTyp).Modul(AktNr).SYS_ARRAY(lok + 3)) And &H4) = 4 Then
            cb_F3.Checked = True
        Else
            cb_F3.Checked = False
        End If

        If (Val("&H" & MBC.ModulTyp(AktTyp).Modul(AktNr).SYS_ARRAY(lok + 3)) And &H8) = 8 Then
            cb_F4.Checked = True
        Else
            cb_F4.Checked = False
        End If


    End Sub

    Private Sub btn_ok_Click(sender As Object, e As EventArgs) Handles btn_ok.Click
        Me.Close()
    End Sub

    Private Sub nud_lok_nr_ValueChanged(sender As Object, e As EventArgs) Handles nud_lok_nr.ValueChanged

        If (AktTyp = 10) And (AktNr > 0) Then

            ' Bestimme anzuzeigende Lok

            Dim lok As Integer

            If (nud_lok_nr.Value <> 80) Then

                lok = nud_lok_nr.Value * 10 + MBC.ModulTyp(AktTyp).Modul(AktNr).Lokspeicher

            Else

                lok = MBC.ModulTyp(AktTyp).Modul(AktNr).Lokspeicher

            End If

            ' Starte Auslesen der Lokinfos des Moduls

            frmMain.COMRefreshStart(lok, lok + 10)

            Timer_Refresh.Start()

            ' Aktualisiere Anzeige

            lok_fill()

        End If

    End Sub

    Private Sub btn_Refresh_Click(sender As Object, e As EventArgs) Handles btn_Refresh.Click

        ' Bestimme anzuzeigende Lok

        Dim lok As Integer

        If (nud_lok_nr.Value <> 80) Then

            lok = nud_lok_nr.Value * 10 + MBC.ModulTyp(AktTyp).Modul(AktNr).Lokspeicher

        Else

            lok = MBC.ModulTyp(AktTyp).Modul(AktNr).Lokspeicher

        End If

        ' Starte Auslesen der Lokinfos des Moduls

        frmMain.COMRefreshStart(lok, lok + 10)

        Timer_Refresh.Start()

        ' Aktualisiere Anzeige

        lok_fill()

    End Sub

    Private Sub Timer_Refresh_Tick(sender As Object, e As EventArgs) Handles Timer_Refresh.Tick

        If (FLAG_Refresh_Acomplished = True) Then

            FLAG_Refresh_Acomplished = False

            Timer_Refresh.Stop()

            ' Aktualisiere Anzeige

            lok_fill()

        End If

    End Sub


End Class