Public Class frmWinkel

    '**********************************************************************************************
    '*** Indizes fuer mbc85-Decoder                                                             ***
    '**********************************************************************************************

    Private MBC_85_START = MBC_S_GENPURP                                 ' Start des Gesamtblocks mbc-88

    ' #define MBC_85_L_S88		4									    // Laenge modulspezifische Parameter
    ' #define MBC_85_S_S88		MBC_S_GENPURP						    // Index Start modulspezifische Parameter
    ' #define MBC_85_S88_H		sys_array[MBC_85_S_S88]			        // MMAddr HIGH
    ' #define MBC_85_S88_L		sys_array[MBC_85_S_S88 + 1]		        // MMAddr LOW
    ' #define MBC_85_S88_RM_H   sys_array[MBC_85_S_S88 + 2]			    // Eingang 16-9
    ' #define MBC_85_S88_RM_L   sys_array[MBC_85_S_S88 + 3]		        // Eingang 8-1

    Private MBC_85_L_S88 = 4                                             ' Laenge Modulnummern
    Private MBC_85_S_S88 = MBC_S_GENPURP                                 ' Index Start Modulnummern
    Private MBC_85_S88_H = MBC_85_S_S88                                  ' Index S88Addr HIGH
    Private MBC_85_S88_L = MBC_85_S_S88 + 1                              ' Index S88Addr LOW
    Private MBC_85_S88_RM_H = MBC_85_S_S88 + 2                           ' Eingang 16-9
    Private MBC_85_S88_RM_L = MBC_85_S_S88 + 3                           ' Eingang 8-1

    ' #define MBC_L_INTS88		16									    // Anzahl der S88PINS
    ' #define MBC_S_INTS88		(MBC_85_L_S88 + MBC_85_S_S88)		    // Index Start S88PINS
    ' #define MBC_INTS88		sys_array[MBC_S_INTS88]			        // Integrationszeit s88 

    Private MBC_85_L_INTS88 = 16                                         ' Anzahl der S88PINS
    Private MBC_85_S_INTS88 = MBC_85_L_S88 + MBC_85_S_S88                ' Index Start S88PINS
    Private MBC_85_INTS88 = MBC_85_S_INTS88                              ' Integrationszeit s88

    '#define MBC_85_L_MMLOK		1                                       // Anzahl der MM-Lok-Adresse
    '#define MBC_85_S_MMLOK	    (MBC_85_L_INTS88 + MBC_85_S_INTS88)     // Index Start MM-Lok-Adresse
    '#define MBC_85_MMLOK		modul.sys_array[MBC_85_S_MMLOK]	        // MM-Lok-Adresse

    Private MBC_85_L_MMLOK = 1                                           ' Anzahl der MM-Lok-Adresse
    Private MBC_85_S_MMLOK = MBC_85_L_INTS88 + MBC_85_S_INTS88           ' Index Start MM-Lok-Adresse
    Private MBC_85_MMLOK = MBC_85_S_MMLOK                                ' MM-Lok-Adresse

    '#define MBC_85_L_MMLOK_G	1                                       // Anzahl der MM-Lok-Geschwindigkeit
    '#define MBC_85_S_MMLOK_G   (MBC_85_L_MMLOK_R + MBC_85_S_MMLOK_R)   // Index Start MM-Lok-Geschwindigkeit
    '#define MBC_85_MMLOK_G		modul.sys_array[MBC_85_S_MMLOK_G]       // MM-Lok-Geschwindigkeit

    Private MBC_85_L_MMLOK_G = 1                                         ' Anzahl der MM-Lok-Geschwindigkeit
    Private MBC_85_S_MMLOK_G = MBC_85_L_MMLOK + MBC_85_S_MMLOK           ' Index Start MM-Lok-Geschwindigkeit
    Private MBC_85_MMLOK_G = MBC_85_S_MMLOK_G                            ' MM-Lok-Geschwindigkeit

    '#define MBC_85_L_OFFSET 	1                                       // Laenge Angabe zum Winkeloffset
    '#define MBC_85_S_OFFSET 	(MBC_85_L_MMLOK + MBC_85_S_MMLOK)       // Index Angaben zum Winkeloffset
    '#define MBC_85_OFFSET_H	modul.sys_array[MBC_85_S_OFFSET]        // Angaben zum Winkeloffset High
    '#define MBC_85_OFFSET_L	modul.sys_array[MBC_85_S_OFFSET+1]      // Angaben zum Winkeloffset Low

    Private MBC_85_L_OFFSET = 2                                          ' Laenge Angabe zum Magentoffset
    Private MBC_85_S_OFFSET = MBC_85_L_MMLOK_G + MBC_85_S_MMLOK_G        ' Index Angaben zum Magentoffset
    Private MBC_85_OFFSET_H = MBC_85_S_OFFSET                            ' Angaben zum Magentoffset HIGH
    Private MBC_85_OFFSET_L = MBC_85_S_OFFSET + 1                        ' Angaben zum Magentoffset LO

    '#define MBC_85_L_POS		    1                                   // Laenge Angabe zur Drehscheibenposition
    '#define MBC_85_S_POS		    (MBC_85_L_OFFSET + MBC_85_S_OFFSET) // Index Angaben zur Drehscheibenposition
    '#define MBC_85_POS			    modul.sys_array[MBC_85_S_I2C]       // Angaben zur Drehscheibenposition

    Private MBC_85_L_POS = 1                                             ' Anzahl Drehscheibenposition
    Private MBC_85_S_POS = MBC_85_L_OFFSET + MBC_85_S_OFFSET             ' Index Drehscheibenposition
    Private MBC_85_POS = MBC_85_S_POS                                    ' Drehscheibenposition

    '#define MBC_85_L_ANG		    34                                  // Laenge Angabe zu Rohdaten Winkel
    '#define MBC_85_S_ANG		    (MBC_85_L_POS + MBC_85_S_POS)       // Index Angaben zu Rohdaten Winkel
    '#define MBC_85_ANG			    modul.sys_array[MBC_85_S_ANG]       // Angaben zur Drehscheibenposition

    Private MBC_85_L_ANG = 32                                            ' Laenge Angabe zu Rohdaten Winkel
    Private MBC_85_S_ANG = MBC_85_L_POS + MBC_85_S_POS                   ' Index Angaben zu Rohdaten Winkel
    Private MBC_85_ANG = MBC_85_S_ANG                                    ' Angaben zur Drehscheibenposition

    '#define MBC_85_L_ISTGLEIS 	2                                       // Laenge Angabe Gleis vorhanden
    '#define MBC_85_S_ISTGLEIS 	(MBC_85_L_ANG + MBC_85_S_ANG)           // Index Angaben Gleis vorhanden
    '#define MBC_85_ISTGLEIS_H	modul.sys_array[MBC_85_S_ISTGLEIS]      // Angaben Gleis vorhanden High
    '#define MBC_85_ISTGLEIS_L	modul.sys_array[MBC_85_S_ISTGLEIS+1]    // Angaben Gleis vorhanden Low

    Private MBC_85_L_ISTGLEIS = 2                                        ' Laenge Angabe zum Magnetoffset
    Private MBC_85_S_ISTGLEIS = MBC_85_L_ANG + MBC_85_S_ANG              ' Index Angaben zum Magnetoffset
    Private MBC_85_ISTGLEIS_H = MBC_85_S_ISTGLEIS                        ' Angaben zum Magnetoffset HIGH
    Private MBC_85_ISTGLEIS_L = MBC_85_S_ISTGLEIS + 1                    ' Angaben zum Magnetoffset LO

    Private MBC_85_ENDE = (MBC_85_L_ISTGLEIS + MBC_85_S_ISTGLEIS - 1)    ' Ende des Gesamtblocks mbc-88

    ' #define EE_S88                0x0A                                // ID Block Adresse an EE
    ' #define EE_S88_TIMING         0x0B                                // ID Block Integrationszeit an EE
    ' #define EE_85_MM_LOK	        0x0C				                // ID Block MM-Lok-Adresse
    ' #define EE_85_MM_LOK_G        0x0D                                // ID Block MM-Lok-Geschwindigkeit
    ' #define EE_85_POS		        0x0E				                // ID Block Buehnenposition
    ' #define EE_85_OFFSET	        0x0F				                // ID Block Magnetoffset
    ' #define EE_85_WINKEL	        0x10				                // ID Block Magnetoffset
    ' #define EE_85_ISTGLEIS        0x11				                // ID Block Magnetoffset

    Private EE_85_S88 = "0A"                                             ' ID Block Adresse an EE
    Private EE_85_S88_TIMING = "0B"                                      ' ID Block Spulentiming an EE
    Private EE_85_MM_LOK = "0C"                                          ' ID Block Lok-Adresse an EE
    Private EE_85_MM_LOK_G = "0D"                                        ' ID Block Lok-Geschwindigkeit an EE
    Private EE_85_POS = "0E"                                             ' ID Block Drehscheibenposition
    Private EE_85_OFFSET = "0F"                                          ' ID Block Drehscheibenposition Offset
    Private EE_85_WINKEL = "10"                                          ' ID Block Zielwinkel
    Private EE_85_ISTGLEIS = "11"                                        ' ID Block Gleis vorhanden


    Dim Initial As Boolean
    Dim PROG_INDEX As Integer

    Const PROG_INDEX_WINKEL = 1
    Const PROG_INDEX_GLEIS = 2

    ' Funktion beim Laden des Fensters
    Private Sub frmWinkel_Load(sender As Object, e As EventArgs) Handles MyBase.Load
        frmMain.Enabled = False
        AddHandler Me.LocationChanged, AddressOf ClampInnerhalbEltern
        btn_winkel_verwerfen.Enabled = False
        btn_winkel_speichern.Enabled = False
        InitialFuellen()
    End Sub

    ' Funktion beim Schließen des Fensters
    Private Sub frmWinkel_FormClosing(sender As Object, e As FormClosingEventArgs) Handles MyBase.FormClosing
        frmMain.Enabled = True
    End Sub

    ' Funktion Button OK
    Private Sub btn_winkel_ok_Click(sender As Object, e As EventArgs) Handles btn_winkel_ok.Click
        Me.Close()
    End Sub

    ' Funktion Button VERWERFEN
    Private Sub btn_winkel_verwerfen_Click(sender As Object, e As EventArgs) Handles btn_winkel_verwerfen.Click
        InitialFuellen()
        btn_winkel_ok.Enabled = True
        btn_winkel_verwerfen.Enabled = False
        btn_winkel_speichern.Enabled = False
        btn_winkel_ok.Select()
    End Sub

    ' Funktion Button SPEICHERN
    Private Sub btn_winkel_speichern_Click(sender As Object, e As EventArgs) Handles btn_winkel_speichern.Click

        Dim MsgIOStart As Integer
        Dim MsgIOMax As Integer
        Dim MsgIOWrite As String = ""
        Dim Index As Integer

        If PROG_INDEX = PROG_INDEX_WINKEL Then

            ' Sichere neue Winkelangaben im SysArray

            MBC.ModulTyp(AktTyp).Modul(AktNr).Winkel(1) = nud_mbc85_g1.Value
            MBC.ModulTyp(AktTyp).Modul(AktNr).Winkel(2) = nud_mbc85_g2.Value
            MBC.ModulTyp(AktTyp).Modul(AktNr).Winkel(3) = nud_mbc85_g3.Value
            MBC.ModulTyp(AktTyp).Modul(AktNr).Winkel(4) = nud_mbc85_g4.Value
            MBC.ModulTyp(AktTyp).Modul(AktNr).Winkel(5) = nud_mbc85_g5.Value
            MBC.ModulTyp(AktTyp).Modul(AktNr).Winkel(6) = nud_mbc85_g6.Value
            MBC.ModulTyp(AktTyp).Modul(AktNr).Winkel(7) = nud_mbc85_g7.Value
            MBC.ModulTyp(AktTyp).Modul(AktNr).Winkel(8) = nud_mbc85_g8.Value
            MBC.ModulTyp(AktTyp).Modul(AktNr).Winkel(9) = nud_mbc85_g9.Value
            MBC.ModulTyp(AktTyp).Modul(AktNr).Winkel(10) = nud_mbc85_g10.Value
            MBC.ModulTyp(AktTyp).Modul(AktNr).Winkel(11) = nud_mbc85_g11.Value
            MBC.ModulTyp(AktTyp).Modul(AktNr).Winkel(12) = nud_mbc85_g12.Value
            MBC.ModulTyp(AktTyp).Modul(AktNr).Winkel(13) = nud_mbc85_g13.Value
            MBC.ModulTyp(AktTyp).Modul(AktNr).Winkel(14) = nud_mbc85_g14.Value
            MBC.ModulTyp(AktTyp).Modul(AktNr).Winkel(15) = nud_mbc85_g15.Value
            MBC.ModulTyp(AktTyp).Modul(AktNr).Winkel(16) = nud_mbc85_g16.Value

            ' Bereite Uebergabe ans Modul vor

            For Index = 1 To 16
                MBC.ModulTyp(AktTyp).Modul(AktNr).SYS_ARRAY(MBC_85_ANG + (Index - 1) * 2) = FormHEX("0", 2, MBC.ModulTyp(AktTyp).Modul(AktNr).Winkel(Index) >> 8)
                MBC.ModulTyp(AktTyp).Modul(AktNr).SYS_ARRAY(MBC_85_ANG + (Index - 1) * 2 + 1) = FormHEX("0", 2, MBC.ModulTyp(AktTyp).Modul(AktNr).Winkel(Index) And &HFF)
            Next

            ' Starte Aktualisierung des Moduls

            MsgIOStart = MBC_85_S_ANG
            MsgIOMax = MBC_85_S_ANG + MBC_85_L_ANG - 1
            MsgIOWrite = EE_85_WINKEL

        End If

        If PROG_INDEX = PROG_INDEX_GLEIS Then

            ' Sichere befahrebare Gleise im Systemarray

            Index = 0

            If cb_mbc85_g1.Checked Then Index = Index Or 1
            If cb_mbc85_g6.Checked Then Index = Index Or 32
            If cb_mbc85_g8.Checked Then Index = Index Or 128
            If cb_mbc85_g9.Checked Then Index = Index Or 256
            If cb_mbc85_g10.Checked Then Index = Index Or 512
            If cb_mbc85_g11.Checked Then Index = Index Or 1024
            If cb_mbc85_g12.Checked Then Index = Index Or 2048
            If cb_mbc85_g13.Checked Then Index = Index Or 4096
            If cb_mbc85_g14.Checked Then Index = Index Or 8192
            If cb_mbc85_g15.Checked Then Index = Index Or 16384

            MBC.ModulTyp(AktTyp).Modul(AktNr).IstGleis = Index

            MBC.ModulTyp(AktTyp).Modul(AktNr).SYS_ARRAY(MBC_85_ISTGLEIS_H) = FormHEX("0", 2, Index >> 8)
            MBC.ModulTyp(AktTyp).Modul(AktNr).SYS_ARRAY(MBC_85_ISTGLEIS_L) = FormHEX("0", 2, Index And &HFF)

            ' Starte Aktualisierung des Moduls

            MsgIOStart = MBC_85_S_ISTGLEIS
            MsgIOMax = MBC_85_S_ISTGLEIS + MBC_85_L_ISTGLEIS - 1
            MsgIOWrite = EE_85_ISTGLEIS

        End If

        ' Fuehre Uodate aus

        frmMain.COMUpdateStart(MsgIOStart, MsgIOMax, MsgIOWrite)

        ' Gebe Formular den Fokus zurueck

        Me.Activate()

        ' Nehme Einfaerbungen und Button-Freigaben zurueck

        ColorWhite()
        btn_winkel_ok.Enabled = True
        btn_winkel_verwerfen.Enabled = False
        btn_winkel_speichern.Enabled = False
        btn_winkel_ok.Select()
    End Sub

    ' Funktionen bei Aenderung der Winkel Gleis 1 bis 16
    Private Sub nud_mbc85_g1_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc85_g1.ValueChanged
        txt_mbc85_g1.Text = "0°"
    End Sub

    Private Sub nud_mbc85_g2_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc85_g2.ValueChanged
        If Not Initial Then
            nud_mbc85_g2.BackColor = Color.Wheat
            btn_winkel_ok.Enabled = False
            btn_winkel_verwerfen.Enabled = True
            btn_winkel_speichern.Enabled = True
            PROG_INDEX = PROG_INDEX_WINKEL
            Sperren()
        End If
        txt_mbc85_g2.Text = Math.Round(nud_mbc85_g2.Value * 0.087890625, 1) & "°"
    End Sub

    Private Sub nud_mbc85_g3_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc85_g3.ValueChanged
        If Not Initial Then
            nud_mbc85_g3.BackColor = Color.Wheat
            btn_winkel_ok.Enabled = False
            btn_winkel_verwerfen.Enabled = True
            btn_winkel_speichern.Enabled = True
            PROG_INDEX = PROG_INDEX_WINKEL
            Sperren()
        End If
        txt_mbc85_g3.Text = Math.Round(nud_mbc85_g3.Value * 0.087890625, 1) & "°"
    End Sub

    Private Sub nud_mbc85_g4_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc85_g4.ValueChanged
        If Not Initial Then
            nud_mbc85_g4.BackColor = Color.Wheat
            btn_winkel_ok.Enabled = False
            btn_winkel_verwerfen.Enabled = True
            btn_winkel_speichern.Enabled = True
            PROG_INDEX = PROG_INDEX_WINKEL
            Sperren()
        End If
        txt_mbc85_g4.Text = Math.Round(nud_mbc85_g4.Value * 0.087890625, 1) & "°"
    End Sub

    Private Sub nud_mbc85_g5_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc85_g5.ValueChanged
        If Not Initial Then
            nud_mbc85_g5.BackColor = Color.Wheat
            btn_winkel_ok.Enabled = False
            btn_winkel_verwerfen.Enabled = True
            btn_winkel_speichern.Enabled = True
            PROG_INDEX = PROG_INDEX_WINKEL
            Sperren()
        End If
        txt_mbc85_g5.Text = Math.Round(nud_mbc85_g5.Value * 0.087890625, 1) & "°"
    End Sub

    Private Sub nud_mbc85_g6_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc85_g6.ValueChanged
        If Not Initial Then
            nud_mbc85_g6.BackColor = Color.Wheat
            btn_winkel_ok.Enabled = False
            btn_winkel_verwerfen.Enabled = True
            btn_winkel_speichern.Enabled = True
            PROG_INDEX = PROG_INDEX_WINKEL
            Sperren()
        End If
        txt_mbc85_g6.Text = Math.Round(nud_mbc85_g6.Value * 0.087890625, 1) & "°"
    End Sub

    Private Sub nud_mbc85_g7_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc85_g7.ValueChanged
        If Not Initial Then
            nud_mbc85_g7.BackColor = Color.Wheat
            btn_winkel_ok.Enabled = False
            btn_winkel_verwerfen.Enabled = True
            btn_winkel_speichern.Enabled = True
            PROG_INDEX = PROG_INDEX_WINKEL
            Sperren()
        End If
        txt_mbc85_g7.Text = Math.Round(nud_mbc85_g7.Value * 0.087890625, 1) & "°"
    End Sub

    Private Sub nud_mbc85_g8_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc85_g8.ValueChanged
        If Not Initial Then
            nud_mbc85_g8.BackColor = Color.Wheat
            btn_winkel_ok.Enabled = False
            btn_winkel_verwerfen.Enabled = True
            btn_winkel_speichern.Enabled = True
            PROG_INDEX = PROG_INDEX_WINKEL
            Sperren()
        End If
        txt_mbc85_g8.Text = Math.Round(nud_mbc85_g8.Value * 0.087890625, 1) & "°"
    End Sub

    Private Sub nud_mbc85_g9_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc85_g9.ValueChanged
        If Not Initial Then
            nud_mbc85_g9.BackColor = Color.Wheat
            btn_winkel_ok.Enabled = False
            btn_winkel_verwerfen.Enabled = True
            btn_winkel_speichern.Enabled = True
            PROG_INDEX = PROG_INDEX_WINKEL
            Sperren()
        End If
        txt_mbc85_g9.Text = Math.Round(nud_mbc85_g9.Value * 0.087890625, 1) & "°"
    End Sub

    Private Sub nud_mbc85_g10_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc85_g10.ValueChanged
        If Not Initial Then
            nud_mbc85_g10.BackColor = Color.Wheat
            btn_winkel_ok.Enabled = False
            btn_winkel_verwerfen.Enabled = True
            btn_winkel_speichern.Enabled = True
            PROG_INDEX = PROG_INDEX_WINKEL
            Sperren()
        End If
        txt_mbc85_g10.Text = Math.Round(nud_mbc85_g10.Value * 0.087890625, 1) & "°"
    End Sub

    Private Sub nud_mbc85_g11_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc85_g11.ValueChanged
        If Not Initial Then
            nud_mbc85_g11.BackColor = Color.Wheat
            btn_winkel_ok.Enabled = False
            btn_winkel_verwerfen.Enabled = True
            btn_winkel_speichern.Enabled = True
            PROG_INDEX = PROG_INDEX_WINKEL
            Sperren()
        End If
        txt_mbc85_g11.Text = Math.Round(nud_mbc85_g11.Value * 0.087890625, 1) & "°"
    End Sub

    Private Sub nud_mbc85_g12_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc85_g12.ValueChanged
        If Not Initial Then
            nud_mbc85_g12.BackColor = Color.Wheat
            btn_winkel_ok.Enabled = False
            btn_winkel_verwerfen.Enabled = True
            btn_winkel_speichern.Enabled = True
            PROG_INDEX = PROG_INDEX_WINKEL
            Sperren()
        End If
        txt_mbc85_g12.Text = Math.Round(nud_mbc85_g12.Value * 0.087890625, 1) & "°"
    End Sub

    Private Sub nud_mbc85_g13_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc85_g13.ValueChanged
        If Not Initial Then
            nud_mbc85_g13.BackColor = Color.Wheat
            btn_winkel_ok.Enabled = False
            btn_winkel_verwerfen.Enabled = True
            btn_winkel_speichern.Enabled = True
            PROG_INDEX = PROG_INDEX_WINKEL
            Sperren()
        End If
        txt_mbc85_g13.Text = Math.Round(nud_mbc85_g13.Value * 0.087890625, 1) & "°"
    End Sub

    Private Sub nud_mbc85_g14_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc85_g14.ValueChanged
        If Not Initial Then
            nud_mbc85_g14.BackColor = Color.Wheat
            btn_winkel_ok.Enabled = False
            btn_winkel_verwerfen.Enabled = True
            btn_winkel_speichern.Enabled = True
            PROG_INDEX = PROG_INDEX_WINKEL
            Sperren()
        End If
        txt_mbc85_g14.Text = Math.Round(nud_mbc85_g14.Value * 0.087890625, 1) & "°"
    End Sub

    Private Sub nud_mbc85_g15_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc85_g15.ValueChanged
        If Not Initial Then
            nud_mbc85_g15.BackColor = Color.Wheat
            btn_winkel_ok.Enabled = False
            btn_winkel_verwerfen.Enabled = True
            btn_winkel_speichern.Enabled = True
            PROG_INDEX = PROG_INDEX_WINKEL
            Sperren()
        End If
        txt_mbc85_g15.Text = Math.Round(nud_mbc85_g15.Value * 0.087890625, 1) & "°"
    End Sub

    Private Sub nud_mbc85_g16_ValueChanged(sender As Object, e As EventArgs) Handles nud_mbc85_g16.ValueChanged
        If Not Initial Then
            nud_mbc85_g16.BackColor = Color.Wheat
            btn_winkel_ok.Enabled = False
            btn_winkel_verwerfen.Enabled = True
            btn_winkel_speichern.Enabled = True
            PROG_INDEX = PROG_INDEX_WINKEL
            Sperren()
        End If
        txt_mbc85_g16.Text = Math.Round(nud_mbc85_g16.Value * 0.087890625, 1) & "°"
    End Sub

    ' Funktionen bei Aenderung ob befahrbare Gleise angeschlossen sind
    Private Sub cb_mbc85_g1_CheckedChanged(sender As Object, e As EventArgs) Handles cb_mbc85_g1.CheckedChanged
        If Not Initial Then
            btn_winkel_ok.Enabled = False
            btn_winkel_verwerfen.Enabled = True
            btn_winkel_speichern.Enabled = True
            PROG_INDEX = PROG_INDEX_GLEIS
            Sperren()
        End If
    End Sub

    Private Sub cb_mbc85_g6_CheckedChanged(sender As Object, e As EventArgs) Handles cb_mbc85_g6.CheckedChanged
        If Not Initial Then
            btn_winkel_ok.Enabled = False
            btn_winkel_verwerfen.Enabled = True
            btn_winkel_speichern.Enabled = True
            PROG_INDEX = PROG_INDEX_GLEIS
            Sperren()
        End If
    End Sub

    Private Sub cb_mbc85_g8_CheckedChanged(sender As Object, e As EventArgs) Handles cb_mbc85_g8.CheckedChanged
        If Not Initial Then
            btn_winkel_ok.Enabled = False
            btn_winkel_verwerfen.Enabled = True
            btn_winkel_speichern.Enabled = True
            PROG_INDEX = PROG_INDEX_GLEIS
            Sperren()
        End If
    End Sub

    Private Sub cb_mbc85_g9_CheckedChanged(sender As Object, e As EventArgs) Handles cb_mbc85_g9.CheckedChanged
        If Not Initial Then
            btn_winkel_ok.Enabled = False
            btn_winkel_verwerfen.Enabled = True
            btn_winkel_speichern.Enabled = True
            PROG_INDEX = PROG_INDEX_GLEIS
            Sperren()
        End If
    End Sub

    Private Sub cb_mbc85_g10_CheckedChanged(sender As Object, e As EventArgs) Handles cb_mbc85_g10.CheckedChanged
        If Not Initial Then
            btn_winkel_ok.Enabled = False
            btn_winkel_verwerfen.Enabled = True
            btn_winkel_speichern.Enabled = True
            PROG_INDEX = PROG_INDEX_GLEIS
            Sperren()
        End If
    End Sub

    Private Sub cb_mbc85_g11_CheckedChanged(sender As Object, e As EventArgs) Handles cb_mbc85_g11.CheckedChanged
        If Not Initial Then
            btn_winkel_ok.Enabled = False
            btn_winkel_verwerfen.Enabled = True
            btn_winkel_speichern.Enabled = True
            PROG_INDEX = PROG_INDEX_GLEIS
            Sperren()
        End If
    End Sub

    Private Sub cb_mbc85_g12_CheckedChanged(sender As Object, e As EventArgs) Handles cb_mbc85_g12.CheckedChanged
        If Not Initial Then
            btn_winkel_ok.Enabled = False
            btn_winkel_verwerfen.Enabled = True
            btn_winkel_speichern.Enabled = True
            PROG_INDEX = PROG_INDEX_GLEIS
            Sperren()
        End If
    End Sub

    Private Sub cb_mbc85_g13_CheckedChanged(sender As Object, e As EventArgs) Handles cb_mbc85_g13.CheckedChanged
        If Not Initial Then
            btn_winkel_ok.Enabled = False
            btn_winkel_verwerfen.Enabled = True
            btn_winkel_speichern.Enabled = True
            PROG_INDEX = PROG_INDEX_GLEIS
            Sperren()
        End If
    End Sub

    Private Sub cb_mbc85_g14_CheckedChanged(sender As Object, e As EventArgs) Handles cb_mbc85_g14.CheckedChanged
        If Not Initial Then
            btn_winkel_ok.Enabled = False
            btn_winkel_verwerfen.Enabled = True
            btn_winkel_speichern.Enabled = True
            PROG_INDEX = PROG_INDEX_GLEIS
            Sperren()
        End If
    End Sub

    Private Sub cb_mbc85_g15_CheckedChanged(sender As Object, e As EventArgs) Handles cb_mbc85_g15.CheckedChanged
        If Not Initial Then
            btn_winkel_ok.Enabled = False
            btn_winkel_verwerfen.Enabled = True
            btn_winkel_speichern.Enabled = True
            PROG_INDEX = PROG_INDEX_GLEIS
            Sperren()
        End If
    End Sub


    Private Sub Sperren()
        If PROG_INDEX = PROG_INDEX_GLEIS Then
            nud_mbc85_g2.Enabled = False
            nud_mbc85_g3.Enabled = False
            nud_mbc85_g4.Enabled = False
            nud_mbc85_g5.Enabled = False
            nud_mbc85_g6.Enabled = False
            nud_mbc85_g7.Enabled = False
            nud_mbc85_g8.Enabled = False
            nud_mbc85_g9.Enabled = False
            nud_mbc85_g10.Enabled = False
            nud_mbc85_g11.Enabled = False
            nud_mbc85_g12.Enabled = False
            nud_mbc85_g13.Enabled = False
            nud_mbc85_g14.Enabled = False
            nud_mbc85_g15.Enabled = False
            nud_mbc85_g16.Enabled = False
        End If

        If PROG_INDEX = PROG_INDEX_WINKEL Then
            cb_mbc85_g1.Enabled = False
            cb_mbc85_g6.Enabled = False
            cb_mbc85_g8.Enabled = False
            cb_mbc85_g9.Enabled = False
            cb_mbc85_g10.Enabled = False
            cb_mbc85_g11.Enabled = False
            cb_mbc85_g12.Enabled = False
            cb_mbc85_g13.Enabled = False
            cb_mbc85_g14.Enabled = False
            cb_mbc85_g15.Enabled = False
        End If
    End Sub

    ' Alle Felder wieder weiss hinterlegen
    Private Sub ColorWhite()
        nud_mbc85_g2.BackColor = Color.White
        nud_mbc85_g2.Enabled = True
        nud_mbc85_g3.BackColor = Color.White
        nud_mbc85_g3.Enabled = True
        nud_mbc85_g4.BackColor = Color.White
        nud_mbc85_g4.Enabled = True
        nud_mbc85_g5.BackColor = Color.White
        nud_mbc85_g5.Enabled = True
        nud_mbc85_g6.BackColor = Color.White
        nud_mbc85_g6.Enabled = True
        nud_mbc85_g7.BackColor = Color.White
        nud_mbc85_g7.Enabled = True
        nud_mbc85_g8.BackColor = Color.White
        nud_mbc85_g8.Enabled = True
        nud_mbc85_g9.BackColor = Color.White
        nud_mbc85_g9.Enabled = True
        nud_mbc85_g10.BackColor = Color.White
        nud_mbc85_g10.Enabled = True
        nud_mbc85_g11.BackColor = Color.White
        nud_mbc85_g11.Enabled = True
        nud_mbc85_g12.BackColor = Color.White
        nud_mbc85_g12.Enabled = True
        nud_mbc85_g13.BackColor = Color.White
        nud_mbc85_g13.Enabled = True
        nud_mbc85_g14.BackColor = Color.White
        nud_mbc85_g14.Enabled = True
        nud_mbc85_g15.BackColor = Color.White
        nud_mbc85_g15.Enabled = True
        nud_mbc85_g16.BackColor = Color.White
        nud_mbc85_g16.Enabled = True
        cb_mbc85_g1.Enabled = True
        cb_mbc85_g6.Enabled = True
        cb_mbc85_g8.Enabled = True
        cb_mbc85_g9.Enabled = True
        cb_mbc85_g10.Enabled = True
        cb_mbc85_g11.Enabled = True
        cb_mbc85_g12.Enabled = True
        cb_mbc85_g13.Enabled = True
        cb_mbc85_g14.Enabled = True
        cb_mbc85_g15.Enabled = True
    End Sub

    ' Initialfuellung der Winkel 
    Private Sub InitialFuellen()
        Initial = True
        txt_mbc85_g1.Text = "0°"
        nud_mbc85_g1.Value = MBC.ModulTyp(AktTyp).Modul(AktNr).Winkel(1)
        nud_mbc85_g2.Value = MBC.ModulTyp(AktTyp).Modul(AktNr).Winkel(2)
        nud_mbc85_g3.Value = MBC.ModulTyp(AktTyp).Modul(AktNr).Winkel(3)
        nud_mbc85_g4.Value = MBC.ModulTyp(AktTyp).Modul(AktNr).Winkel(4)
        nud_mbc85_g5.Value = MBC.ModulTyp(AktTyp).Modul(AktNr).Winkel(5)
        nud_mbc85_g6.Value = MBC.ModulTyp(AktTyp).Modul(AktNr).Winkel(6)
        nud_mbc85_g7.Value = MBC.ModulTyp(AktTyp).Modul(AktNr).Winkel(7)
        nud_mbc85_g8.Value = MBC.ModulTyp(AktTyp).Modul(AktNr).Winkel(8)
        nud_mbc85_g9.Value = MBC.ModulTyp(AktTyp).Modul(AktNr).Winkel(9)
        nud_mbc85_g10.Value = MBC.ModulTyp(AktTyp).Modul(AktNr).Winkel(10)
        nud_mbc85_g11.Value = MBC.ModulTyp(AktTyp).Modul(AktNr).Winkel(11)
        nud_mbc85_g12.Value = MBC.ModulTyp(AktTyp).Modul(AktNr).Winkel(12)
        nud_mbc85_g13.Value = MBC.ModulTyp(AktTyp).Modul(AktNr).Winkel(13)
        nud_mbc85_g14.Value = MBC.ModulTyp(AktTyp).Modul(AktNr).Winkel(14)
        nud_mbc85_g15.Value = MBC.ModulTyp(AktTyp).Modul(AktNr).Winkel(15)
        nud_mbc85_g16.Value = MBC.ModulTyp(AktTyp).Modul(AktNr).Winkel(16)
        If MBC.ModulTyp(AktTyp).Modul(AktNr).IstGleis And 1 Then cb_mbc85_g1.Checked = True
        If MBC.ModulTyp(AktTyp).Modul(AktNr).IstGleis And 32 Then cb_mbc85_g6.Checked = True
        If MBC.ModulTyp(AktTyp).Modul(AktNr).IstGleis And 128 Then cb_mbc85_g8.Checked = True
        If MBC.ModulTyp(AktTyp).Modul(AktNr).IstGleis And 256 Then cb_mbc85_g9.Checked = True
        If MBC.ModulTyp(AktTyp).Modul(AktNr).IstGleis And 512 Then cb_mbc85_g10.Checked = True
        If MBC.ModulTyp(AktTyp).Modul(AktNr).IstGleis And 1024 Then cb_mbc85_g11.Checked = True
        If MBC.ModulTyp(AktTyp).Modul(AktNr).IstGleis And 2048 Then cb_mbc85_g12.Checked = True
        If MBC.ModulTyp(AktTyp).Modul(AktNr).IstGleis And 4096 Then cb_mbc85_g13.Checked = True
        If MBC.ModulTyp(AktTyp).Modul(AktNr).IstGleis And 8192 Then cb_mbc85_g14.Checked = True
        If MBC.ModulTyp(AktTyp).Modul(AktNr).IstGleis And 16384 Then cb_mbc85_g15.Checked = True
        Initial = False
        ColorWhite()
    End Sub


End Class