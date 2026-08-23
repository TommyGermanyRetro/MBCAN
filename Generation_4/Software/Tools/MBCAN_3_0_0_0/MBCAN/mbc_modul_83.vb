Public Class mbc_modul_83_cl

    ' Beinhaltet Stammdaten

    Inherits mbc_modul_stammdaten_cl
    Implements IMbcModul

    '**********************************************************************************************
    '*** Sonderbelegung mbc-83                                                                  ***
    '**********************************************************************************************

    Private MBC_83_START = MBC_S_GENPURP                                 ' Start des Gesamtblocks mbc-83

    ' #define MBC_83_L_MM		7									    // Laenge modulspezifische Parameter
    ' #define MBC_83_S_MM		MBC_S_GENPURP						    // Index Start modulspezifische Parameter
    ' #define MBC_83_MM_H		sys_array[MBC_83_S_MM]			        // MMAddr HIGH
    ' #define MBC_83_MM_L		sys_array[MBC_83_S_MM + 1]		        // MMAddr LOW
    ' #define MBC_83_MM		    sys_array[MBC_83_S_MM + 2]		        // Stellung der Aktoren
    ' #define MBC_83_MM_ART		sys_array[MBC_83_S_MM + 3]		        // Art der Aktoren
    ' #define MBC_83_AUS		sys_array[MBC_83_S_MM + 4]		        // Auto-Aus
    ' #define MBC_83_RUECK		sys_array[MBC_83_S_MM + 5]		        // Zwangsrueckmeldung
    ' #define MBC_83_DIMM		sys_array[MBC_83_S_MM + 6]		        // Dimmerstellung


    Private MBC_83_L_MM = 7                                              ' Laenge modulspezifische Parameter
    Private MBC_83_S_MM = MBC_S_GENPURP                                  ' Index Start modulspezifische Parameter
    Private MBC_83_MM_H = MBC_83_S_MM                                    ' MMAddr HIGH
    Private MBC_83_MM_L = MBC_83_S_MM + 1                                ' MMAddr LOW
    Private MBC_83_MM = MBC_83_S_MM + 2                                  ' Stellung der Aktoren
    Private MBC_83_MM_ART = MBC_83_S_MM + 3                              ' Art der Aktoren
    Private MBC_83_AUS = MBC_83_S_MM + 4                                 ' Auto-Aus
    Private MBC_83_RUECK = MBC_83_S_MM + 5                               ' Zwangsrueckmeldung
    Private MBC_83_DIMM = MBC_83_S_MM + 6                                ' Dimmerstellung

    ' #define MBC_83_L_ZEIT	    4									    // Laenge MM-Spulenzeit
    ' #define MBC_83_S_ZEIT	    (MBC_83_L_MM + MBC_83_S_MM)	            // Index Start MM-Spulenzeit
    ' #define MBC_83_TIO		sys_array[MBC_83_S_ZEIT]			    // Einschaltzeit MM-Spulen

    Private MBC_83_L_ZEIT = 4                                            ' Laenge MM-Spulenzeit
    Private MBC_83_S_ZEIT = (MBC_83_L_MM + MBC_83_S_MM)                  ' Index Start MM-Spulenzeit
    Private MBC_83_TIO = MBC_83_S_ZEIT                                   ' Einschaltzeit MM-Spulen

    Private MBC_83_ENDE = (MBC_83_S_ZEIT + MBC_83_L_ZEIT - 1)            ' Ende des Gesamtblocks mbc-83

    ' #define EE_83_MMAddr		0x0A								    // ID Block Adresse
    ' #define EE_83_MM_TIMING	0x0B								    // ID Block Timing
    ' #define EE_83_MM_ART	    0x0C								    // ID Block Art der Aktoren
    ' #define EE_83_AUS			0x0D								    // ID Block Aus
    ' #define EE_83_RUECK	    0x0E								    // ID Block Zwangsrueckmeldung
    ' #define EE_83_DIMM	    0x0F								    // ID Block Dimmerstellung

    Private EE_83_MMAddr = "0A"                                          ' ID Block Adresse
    Private EE_83_MM_TIMING = "0B"                                       ' ID Block Timing
    Private EE_83_MM_ART = "0C"                                          ' ID Block Art der Aktoren
    Private EE_83_AUS = "0D"                                             ' ID Block Aus
    Private EE_83_RUECK = "0E"                                           ' ID Block Zwangsrueckmeldung
    Private EE_83_DIMM = "0F"                                            ' ID Block Dimmerstellung

    '**********************************************************************************************
    '*** Modul Updatesteuerung                                                                  ***
    '**********************************************************************************************

    Private PROG_INDEX_MMAddr = 0
    Private PROG_INDEX_MMTiming = 1
    Private PROG_INDEX_MMDimmerStartUp = 2
    Private PROG_INDEX_MMAus = 3
    Private PROG_INDEX_MMRueck = 4
    Private PROG_INDEX_MMArt = 5

    '**********************************************************************************************
    '*** Modul SYS_ARRAY-Indizes                                                                ***
    '**********************************************************************************************

    Public IOStart As Integer = MBC_83_START                             ' Startindex Parameterbereich
    Public IOEnde As Integer = MBC_83_ENDE                               ' Endindex Parameterbereich

    '**********************************************************************************************
    '*** Modul Bewegungsdaten                                                                   ***
    '**********************************************************************************************

    ' Startwerte fuer die Modulvariablen

    Private MM_New As String = "&H33FC"
    Private MMArt_New As Integer = "&H00"
    Private MMAus_New As Integer = 1000
    Private MMRueck_New As Integer = 0
    Private MMDimmerStartUp_New As Integer = 0
    Private MMTiming_New As Integer = 10

    ' Modulvariablen

    Public MMValue As String           ' Stellungsanzeige der Magnetartikel
    Public MMAddr As String            ' Adresse im Maerklin-Adressbereich
    Public MMArt As String             ' Art der Aktoren
    Public MMDimmerStartUp As Integer  ' Dimmerstellung StartUp
    Public MMAus As Integer            ' Auto-Aus
    Public MMRueck As Integer          ' Zwangsrueckmeldung
    Public MMTiming_1 As Integer       ' Haltezeit MM-Aktor Spule 1
    Public MMTiming_2 As Integer       ' Haltezeit MM-Aktor Spule 2
    Public MMTiming_3 As Integer       ' Haltezeit MM-Aktor Spule 3
    Public MMTiming_4 As Integer       ' Haltezeit MM-Aktor Spule 4

    '**********************************************************************************************
    '*** Modul initialisieren                                                                   ***
    '**********************************************************************************************

    Sub New()

        MMAddr = MM_New
        MMArt = MMArt_New
        MMDimmerStartUp = MMDimmerStartUp_New
        MMAus = MMAus_New
        MMRueck = MMRueck_New
        MMTiming_1 = MMTiming_New
        MMTiming_2 = MMTiming_New
        MMTiming_3 = MMTiming_New
        MMTiming_4 = MMTiming_New

    End Sub

    '**********************************************************************************************
    '*** Modul Funktionen                                                                       ***
    '**********************************************************************************************

    ' Sperren der Eingabemoeglichkeiten nach Aenderungen je
    ' nachdem welche Aenderung vorgenommen wurde
    ' 0 = An-Zeit der Spule wurde geaendert
    ' 1 = MM-Adresse wurde geaendert
    ' 2 = Dimmer wurde geaendert
    ' 3 = Auto-AUS wurde geaendert
    ' 4 = Rueckmeldung wurde geaendert
    ' 5 = Nutzungstyp wurde geaendert

    Public Sub Tabpage_Sperren(block As Integer)

        If block = 0 Then
            frmMain.gb_mbc83_CH1_clb_Typ.Enabled = False
            frmMain.gb_mbc83_CH2_clb_Typ.Enabled = False
            frmMain.gb_mbc83_CH3_clb_Typ.Enabled = False
            frmMain.gb_mbc83_CH4_clb_Typ.Enabled = False
            frmMain.nud_mbc83_MMAddr.Enabled = False
            frmMain.nud_mbc83_Dimmer.Enabled = False
            frmMain.nud_mbc83_Aus.Enabled = False
            frmMain.nud_mbc83_Rueck.Enabled = False
        End If

        If block = 1 Then
            frmMain.gb_mbc83_CH1_nud_TIO.Enabled = False
            frmMain.gb_mbc83_CH2_nud_TIO.Enabled = False
            frmMain.gb_mbc83_CH3_nud_TIO.Enabled = False
            frmMain.gb_mbc83_CH4_nud_TIO.Enabled = False
            frmMain.gb_mbc83_CH1_clb_Typ.Enabled = False
            frmMain.gb_mbc83_CH2_clb_Typ.Enabled = False
            frmMain.gb_mbc83_CH3_clb_Typ.Enabled = False
            frmMain.gb_mbc83_CH4_clb_Typ.Enabled = False
            frmMain.nud_mbc83_Dimmer.Enabled = False
            frmMain.nud_mbc83_Aus.Enabled = False
            frmMain.nud_mbc83_Rueck.Enabled = False
        End If

        If block = 2 Then
            frmMain.gb_mbc83_CH1_nud_TIO.Enabled = False
            frmMain.gb_mbc83_CH2_nud_TIO.Enabled = False
            frmMain.gb_mbc83_CH3_nud_TIO.Enabled = False
            frmMain.gb_mbc83_CH4_nud_TIO.Enabled = False
            frmMain.gb_mbc83_CH1_clb_Typ.Enabled = False
            frmMain.gb_mbc83_CH2_clb_Typ.Enabled = False
            frmMain.gb_mbc83_CH3_clb_Typ.Enabled = False
            frmMain.gb_mbc83_CH4_clb_Typ.Enabled = False
            frmMain.nud_mbc83_MMAddr.Enabled = False
            frmMain.nud_mbc83_Aus.Enabled = False
            frmMain.nud_mbc83_Rueck.Enabled = False
        End If

        If block = 3 Then
            frmMain.gb_mbc83_CH1_nud_TIO.Enabled = False
            frmMain.gb_mbc83_CH2_nud_TIO.Enabled = False
            frmMain.gb_mbc83_CH3_nud_TIO.Enabled = False
            frmMain.gb_mbc83_CH4_nud_TIO.Enabled = False
            frmMain.gb_mbc83_CH1_clb_Typ.Enabled = False
            frmMain.gb_mbc83_CH2_clb_Typ.Enabled = False
            frmMain.gb_mbc83_CH3_clb_Typ.Enabled = False
            frmMain.gb_mbc83_CH4_clb_Typ.Enabled = False
            frmMain.nud_mbc83_MMAddr.Enabled = False
            frmMain.nud_mbc83_Dimmer.Enabled = False
            frmMain.nud_mbc83_Rueck.Enabled = False
        End If

        If block = 4 Then
            frmMain.gb_mbc83_CH1_nud_TIO.Enabled = False
            frmMain.gb_mbc83_CH2_nud_TIO.Enabled = False
            frmMain.gb_mbc83_CH3_nud_TIO.Enabled = False
            frmMain.gb_mbc83_CH4_nud_TIO.Enabled = False
            frmMain.gb_mbc83_CH1_clb_Typ.Enabled = False
            frmMain.gb_mbc83_CH2_clb_Typ.Enabled = False
            frmMain.gb_mbc83_CH3_clb_Typ.Enabled = False
            frmMain.gb_mbc83_CH4_clb_Typ.Enabled = False
            frmMain.nud_mbc83_MMAddr.Enabled = False
            frmMain.nud_mbc83_Dimmer.Enabled = False
            frmMain.nud_mbc83_Aus.Enabled = False
        End If

        If block = 5 Then
            frmMain.gb_mbc83_CH1_nud_TIO.Enabled = False
            frmMain.gb_mbc83_CH2_nud_TIO.Enabled = False
            frmMain.gb_mbc83_CH3_nud_TIO.Enabled = False
            frmMain.gb_mbc83_CH4_nud_TIO.Enabled = False
            frmMain.nud_mbc83_MMAddr.Enabled = False
            frmMain.nud_mbc83_Dimmer.Enabled = False
            frmMain.nud_mbc83_Aus.Enabled = False
            frmMain.nud_mbc83_Rueck.Enabled = False
        End If

    End Sub

    ' Aktivieren der Eingabemoeglichkeiten 

    Public Sub Tabpage_Aktivieren() Implements IMbcModul.Tabpage_Aktivieren

        frmMain.nud_mbc83_MMAddr.Enabled = True
        frmMain.nud_mbc83_MMAddr.BackColor = Color.White

        frmMain.nud_mbc83_Aus.Enabled = True
        frmMain.nud_mbc83_Aus.BackColor = Color.White

        frmMain.nud_mbc83_Rueck.Enabled = True
        frmMain.nud_mbc83_Rueck.BackColor = Color.White

        frmMain.gb_mbc83_CH1_nud_TIO.Enabled = True
        frmMain.gb_mbc83_CH1_nud_TIO.BackColor = Color.White

        frmMain.gb_mbc83_CH2_nud_TIO.Enabled = True
        frmMain.gb_mbc83_CH2_nud_TIO.BackColor = Color.White

        frmMain.gb_mbc83_CH3_nud_TIO.Enabled = True
        frmMain.gb_mbc83_CH3_nud_TIO.BackColor = Color.White

        frmMain.gb_mbc83_CH4_nud_TIO.Enabled = True
        frmMain.gb_mbc83_CH4_nud_TIO.BackColor = Color.White

        frmMain.gb_mbc83_CH1_clb_Typ.Enabled = True
        frmMain.gb_mbc83_CH1_clb_Typ.BackColor = Color.White

        frmMain.gb_mbc83_CH2_clb_Typ.Enabled = True
        frmMain.gb_mbc83_CH2_clb_Typ.BackColor = Color.White

        frmMain.gb_mbc83_CH3_clb_Typ.Enabled = True
        frmMain.gb_mbc83_CH3_clb_Typ.BackColor = Color.White

        frmMain.gb_mbc83_CH4_clb_Typ.Enabled = True
        frmMain.gb_mbc83_CH4_clb_Typ.BackColor = Color.White

        frmMain.nud_mbc83_Dimmer.Enabled = True
        frmMain.nud_mbc83_Dimmer.BackColor = Color.White

    End Sub


    ' Anzeigen des Modultyps je nach Nutzung

    Public Sub Tabpage_Typ()

        Dim index As Integer

        For index = 0 To 2
            frmMain.gb_mbc83_CH1_clb_Typ.SetItemChecked(index, False)
            frmMain.gb_mbc83_CH2_clb_Typ.SetItemChecked(index, False)
            frmMain.gb_mbc83_CH3_clb_Typ.SetItemChecked(index, False)
            frmMain.gb_mbc83_CH4_clb_Typ.SetItemChecked(index, False)
        Next

        index = (Val(MMArt) And &H3)
        If index < 3 Then frmMain.gb_mbc83_CH1_clb_Typ.SetItemChecked(index, True)

        index = ((Val(MMArt) And &HC) / 4)
        If index < 3 Then frmMain.gb_mbc83_CH2_clb_Typ.SetItemChecked(index, True)

        index = ((Val(MMArt) And &H30) / 16)
        If index < 3 Then frmMain.gb_mbc83_CH3_clb_Typ.SetItemChecked(index, True)

        index = ((Val(MMArt) And &HC0) / 64)
        If index < 3 Then frmMain.gb_mbc83_CH4_clb_Typ.SetItemChecked(index, True)

    End Sub

    ' Uebernehmen der Spulen-An-Zeit in Abhaengigkeit des ausgewaehlten Ausgangs

    Public Sub Tabpage_TimeSet(ByRef nud As Object, ByRef timing As Object)

        If (FLAG_Edit_Enable) Then
            Tabpage_Sperren(0)
            frmMain.Register_Modul_Buttons_Enable(False, True, True, False, False, False)
            FLAG_Prog_Index = PROG_INDEX_MMTiming
            timing = nud.value
            nud.BackColor = Color.Wheat
        End If

    End Sub

    ' Uebernehmen der MM-Adresse des Moduls

    Public Sub Tabpage_AddrSet()

        If (FLAG_Edit_Enable) Then
            FLAG_Prog_Index = PROG_INDEX_MMAddr
            Tabpage_Sperren(1)
            frmMain.Register_Modul_Buttons_Enable(False, True, True, False, False, False)
            MMAddr = "&h" & FormHEX("0", 4, frmMain.nud_mbc83_MMAddr.Value)
            frmMain.nud_mbc83_MMAddr.BackColor = Color.Wheat
            frmMain.gb_mbc83_CH1_txt_Adresse.Text = Val("&H" & Hex(frmMain.nud_mbc83_MMAddr.Value)) - &H3000 + 1
            frmMain.txt_mbc83_adr_2.Text = Val("&H" & Hex(frmMain.nud_mbc83_MMAddr.Value)) - &H3000 + 2
            frmMain.gb_mbc83_CH3_txt_Adresse.Text = Val("&H" & Hex(frmMain.nud_mbc83_MMAddr.Value)) - &H3000 + 3
            frmMain.gb_mbc83_CH4_txt_Adresse.Text = Val("&H" & Hex(frmMain.nud_mbc83_MMAddr.Value)) - &H3000 + 4
        End If

    End Sub

    ' Uebernehmen der Dimmerstellung des Moduls

    Public Sub Tabpage_DimmerSet()

        If (FLAG_Edit_Enable) Then
            FLAG_Prog_Index = PROG_INDEX_MMDimmerStartUp
            Tabpage_Sperren(2)
            frmMain.Register_Modul_Buttons_Enable(False, True, True, False, False, False)
            MMDimmerStartUp = frmMain.nud_mbc83_Dimmer.Value
            frmMain.nud_mbc83_Dimmer.BackColor = Color.Wheat
        End If

    End Sub

    ' Uebernehmen der Auto-Aus des Moduls

    Public Sub Tabpage_AusSet()

        If (FLAG_Edit_Enable) Then
            FLAG_Prog_Index = PROG_INDEX_MMAus
            Tabpage_Sperren(3)
            frmMain.Register_Modul_Buttons_Enable(False, True, True, False, False, False)
            MMAus = frmMain.nud_mbc83_Aus.Value
            frmMain.nud_mbc83_Aus.BackColor = Color.Wheat
        End If

    End Sub

    ' Uebernehmen des Rueckmeldezyklus des Moduls

    Public Sub Tabpage_RueckSet()

        If (FLAG_Edit_Enable) Then
            FLAG_Prog_Index = PROG_INDEX_MMRueck
            Tabpage_Sperren(4)
            frmMain.Register_Modul_Buttons_Enable(False, True, True, False, False, False)
            MMRueck = frmMain.nud_mbc83_Rueck.Value
            frmMain.nud_mbc83_Rueck.BackColor = Color.Wheat
        End If

    End Sub

    ' Uebernehmen des Nutzung des Moduls

    Public Sub Tabpage_NutzungSet(ByRef nud As Object, channel As Integer)

        Dim index As Integer
        Dim MMArtCH1, MMArtCH2, MMArtCH3, MMArtCH4 As Integer

        If (FLAG_Edit_Enable) Then
            FLAG_Prog_Index = PROG_INDEX_MMArt
            Tabpage_Sperren(5)
            frmMain.Register_Modul_Buttons_Enable(False, True, True, False, False, False)

            index = nud.SelectedIndex

            If index <> -1 Then

                If index = 0 Then
                    nud.SetItemChecked(1, False)
                    nud.SetItemChecked(2, False)
                End If
                If index = 1 Then
                    nud.SetItemChecked(0, False)
                    nud.SetItemChecked(2, False)
                End If
                If index = 2 Then
                    nud.SetItemChecked(0, False)
                    nud.SetItemChecked(1, False)
                End If

                nud.BackColor = Color.Wheat
                nud.ClearSelected()

                MMArtCH1 = Val(MMArt) And &H3
                MMArtCH2 = (Val(MMArt) And &HC) / 4
                MMArtCH3 = (Val(MMArt) And &H30) / 16
                MMArtCH4 = (Val(MMArt) And &HC0) / 64


                If channel = 1 Then MMArtCH1 = index
                If channel = 2 Then MMArtCH2 = index
                If channel = 3 Then MMArtCH3 = index
                If channel = 4 Then MMArtCH4 = index

                MMArt = "&H" & FormHEX("0", 2, MMArtCH1 + MMArtCH2 * 4 + MMArtCH3 * 16 + MMArtCH4 * 64)

            End If

        End If

    End Sub

    ' Zeige Modulwerte an

    Public Sub Tabpage_Werte_Show() Implements IMbcModul.Tabpage_Werte_Show

        Try
            frmMain.nud_mbc83_MMAddr.Value = Val(MMAddr)
        Catch e As Exception
            frmMain.nud_mbc83_MMAddr.Value = Val(MM_New)
        End Try

        frmMain.gb_mbc83_CH1_txt_Adresse.Text = Val("&H" & Hex(frmMain.nud_mbc83_MMAddr.Value)) - &H3000 + 1
        frmMain.txt_mbc83_adr_2.Text = Val("&H" & Hex(frmMain.nud_mbc83_MMAddr.Value)) - &H3000 + 2
        frmMain.gb_mbc83_CH3_txt_Adresse.Text = Val("&H" & Hex(frmMain.nud_mbc83_MMAddr.Value)) - &H3000 + 3
        frmMain.gb_mbc83_CH4_txt_Adresse.Text = Val("&H" & Hex(frmMain.nud_mbc83_MMAddr.Value)) - &H3000 + 4

        Try
            frmMain.nud_mbc83_Dimmer.Value = Val(MMDimmerStartUp)
        Catch e As Exception
            frmMain.nud_mbc83_Dimmer.Value = 0
        End Try
        Try
            frmMain.nud_mbc83_Aus.Value = Val(MMAus)
        Catch e As Exception
            frmMain.nud_mbc83_Aus.Value = 1000
        End Try
        Try
            frmMain.nud_mbc83_Rueck.Value = Val(MMRueck)
        Catch e As Exception
            frmMain.nud_mbc83_Rueck.Value = 0
        End Try
        Try
            frmMain.gb_mbc83_CH1_nud_TIO.Value = Val(MMTiming_1)
        Catch e As Exception
            frmMain.gb_mbc83_CH1_nud_TIO.Value = Val(MMTiming_New)
        End Try
        Try
            frmMain.gb_mbc83_CH2_nud_TIO.Value = Val(MMTiming_2)
        Catch e As Exception
            frmMain.gb_mbc83_CH2_nud_TIO.Value = Val(MMTiming_New)
        End Try
        Try
            frmMain.gb_mbc83_CH3_nud_TIO.Value = Val(MMTiming_3)
        Catch e As Exception
            frmMain.gb_mbc83_CH3_nud_TIO.Value = Val(MMTiming_New)
        End Try
        Try
            frmMain.gb_mbc83_CH4_nud_TIO.Value = Val(MMTiming_4)
        Catch e As Exception
            frmMain.gb_mbc83_CH4_nud_TIO.Value = Val(MMTiming_New)
        End Try

        Tabpage_Typ()

        frmMain.Register_Modul_Aktorstellung_Show()

    End Sub

    ' Fuehre Update des Moduls aus

    Public Sub Update() Implements IMbcModul.Update
        Dim MsgIOStart As Integer
        Dim MsgIOMax As Integer
        Dim MsgIOWrite As String

        MsgIOStart = 0
        MsgIOMax = 0
        MsgIOWrite = ""

        ' Weise die MMAddr zu und aktiviere den Updatemodus

        If FLAG_Prog_Index = PROG_INDEX_MMAddr Then

            ' Die Indices x_81_x stehen stellvertretend fuer alle MM-Module

            MsgIOStart = MBC_83_MM_H
            MsgIOMax = MBC_83_MM_L
            MsgIOWrite = EE_83_MMAddr

            Tabpage_Aktivieren()

            SYS_ARRAY(MBC_83_MM_H) = Microsoft.VisualBasic.Mid(MMAddr, 3, 2)
            SYS_ARRAY(MBC_83_MM_L) = Microsoft.VisualBasic.Right(MMAddr, 2)

        End If

        ' Weise das Spulentiming zu und aktiviere den Updatemodus

        If FLAG_Prog_Index = PROG_INDEX_MMTiming Then

            MsgIOStart = MBC_83_S_ZEIT
            MsgIOMax = MBC_83_S_ZEIT + MBC_83_L_ZEIT - 1
            MsgIOWrite = EE_83_MM_TIMING

            Tabpage_Aktivieren()

            SYS_ARRAY(MBC_83_S_ZEIT) = FormHEX("0", 2, MMTiming_1 / 10)
            SYS_ARRAY(MBC_83_S_ZEIT + 1) = FormHEX("0", 2, MMTiming_2 / 10)
            SYS_ARRAY(MBC_83_S_ZEIT + 2) = FormHEX("0", 2, MMTiming_3 / 10)
            SYS_ARRAY(MBC_83_S_ZEIT + 3) = FormHEX("0", 2, MMTiming_4 / 10)

        End If

        ' Weise das Spulentiming zu und aktiviere den Updatemodus

        If FLAG_Prog_Index = PROG_INDEX_MMTiming Then

            MsgIOStart = MBC_83_S_ZEIT
            MsgIOMax = MBC_83_S_ZEIT + MBC_83_L_ZEIT - 1
            MsgIOWrite = EE_83_MM_TIMING

            Tabpage_Aktivieren()

            SYS_ARRAY(MBC_83_S_ZEIT) = FormHEX("0", 2, MMTiming_1 / 10)
            SYS_ARRAY(MBC_83_S_ZEIT + 1) = FormHEX("0", 2, MMTiming_2 / 10)
            SYS_ARRAY(MBC_83_S_ZEIT + 2) = FormHEX("0", 2, MMTiming_3 / 10)
            SYS_ARRAY(MBC_83_S_ZEIT + 3) = FormHEX("0", 2, MMTiming_4 / 10)

        End If

        ' Weise die Dimmeinstellung zu

        If FLAG_Prog_Index = PROG_INDEX_MMDimmerStartUp Then

            MsgIOStart = MBC_83_DIMM
            MsgIOMax = MBC_83_DIMM
            MsgIOWrite = EE_83_DIMM

            Tabpage_Aktivieren()

            SYS_ARRAY(MBC_83_DIMM) = FormHEX("0", 2, MMDimmerStartUp)

        End If

        ' Weise Auto-Aus zu

        If FLAG_Prog_Index = PROG_INDEX_MMAus Then

            MsgIOStart = MBC_83_AUS
            MsgIOMax = MBC_83_AUS
            MsgIOWrite = EE_83_AUS

            Tabpage_Aktivieren()

            SYS_ARRAY(MBC_83_AUS) = FormHEX("0", 2, MMAus / 10)

        End If

        ' Weise Meldezyklus zu

        If FLAG_Prog_Index = PROG_INDEX_MMRueck Then

            MsgIOStart = MBC_83_RUECK
            MsgIOMax = MBC_83_RUECK
            MsgIOWrite = EE_83_RUECK

            Tabpage_Aktivieren()

            SYS_ARRAY(MBC_83_RUECK) = FormHEX("0", 2, MMRueck)

        End If

        ' Weise Nutzungsart zu

        If FLAG_Prog_Index = PROG_INDEX_MMArt Then

            MsgIOStart = MBC_83_MM_ART
            MsgIOMax = MBC_83_MM_ART
            MsgIOWrite = EE_83_MM_ART

            Tabpage_Aktivieren()

            SYS_ARRAY(MBC_83_MM_ART) = Microsoft.VisualBasic.Right(MMArt, 2)

        End If

        ' Starte Aktualisierung des Moduls

        frmMain.COMUpdateStart(MsgIOStart, MsgIOMax, MsgIOWrite)

    End Sub

    ' Fuelle Modul mit den aus dem Modul gelesenen spezifischenWerten und Parameter
    Public Sub Parameter_Fill() Implements IMbcModul.Parameter_Fill

        ' MMAdresse 

        MMAddr = "&H" & SYS_ARRAY(MBC_83_MM_H) & SYS_ARRAY(MBC_83_MM_L)

        ' MM-Stellungsanzeige

        MMValue = "&H" & SYS_ARRAY(MBC_83_MM)

        ' Dimmer StartUp mbc-83 

        MMDimmerStartUp = Convert.ToInt32(SYS_ARRAY(MBC_83_DIMM), 16)

        ' Auto-Aus an die Magnetartikel 

        MMAus = 10 * Convert.ToInt32(SYS_ARRAY(MBC_83_AUS), 16)

        ' Zwangsrueckmeldung der Magnetartikel 

        MMRueck = Convert.ToInt32(SYS_ARRAY(MBC_83_RUECK), 16)

        ' Bestueckung mbc-83

        MMArt = "&H" & SYS_ARRAY(MBC_83_MM_ART)

        ' Spulen-Timing mbc-83

        MMTiming_1 = 10 * Convert.ToInt32(SYS_ARRAY(MBC_83_S_ZEIT), 16)
        MMTiming_2 = 10 * Convert.ToInt32(SYS_ARRAY(MBC_83_S_ZEIT + 1), 16)
        MMTiming_3 = 10 * Convert.ToInt32(SYS_ARRAY(MBC_83_S_ZEIT + 2), 16)
        MMTiming_4 = 10 * Convert.ToInt32(SYS_ARRAY(MBC_83_S_ZEIT + 3), 16)

    End Sub

    ' Steuere die LED-Anzeigen im Modulbild an
    Public Sub Aktorstellung_Show() Implements IMbcModul.Aktorstellung_Show


        Dim mm = Val(frmMain.Register_Get_MMValue(AktTyp, AktNr))

        If ((mm And 2) = 2) Then
            frmMain.pic_Status_LED01.Image = ImROT
        Else
            frmMain.pic_Status_LED01.Image = ImGRAU
        End If

        If ((mm And 1) = 1) Then
            frmMain.pic_Status_LED03.Image = ImGRUEN
        Else
            frmMain.pic_Status_LED03.Image = ImGRAU
        End If

        If ((mm And 8) = 8) Then
            frmMain.pic_Status_LED06.Image = ImROT
        Else
            frmMain.pic_Status_LED06.Image = ImGRAU
        End If

        If ((mm And 4) = 4) Then
            frmMain.pic_Status_LED08.Image = ImGRUEN
        Else
            frmMain.pic_Status_LED08.Image = ImGRAU
        End If

        If ((mm And 32) = 32) Then
            frmMain.pic_Status_LED12.Image = ImROT
        Else
            frmMain.pic_Status_LED12.Image = ImGRAU
        End If

        If ((mm And 16) = 16) Then
            frmMain.pic_Status_LED14.Image = ImGRUEN
        Else
            frmMain.pic_Status_LED14.Image = ImGRAU
        End If

        If ((mm And 128) = 128) Then
            frmMain.pic_Status_LED17.Image = ImROT
        Else
            frmMain.pic_Status_LED17.Image = ImGRAU
        End If

        If ((mm And 64) = 64) Then
            frmMain.pic_Status_LED19.Image = ImGRUEN
        Else
            frmMain.pic_Status_LED19.Image = ImGRAU
        End If

    End Sub

    ' Hole MM-Adresse des Moduls zu
    Public Function Get_MMAddr(index As Integer) As Object Implements IMbcModul.Get_MMAddr

        Get_MMAddr = "&H" + FormHEX("0", 4, Val(MMAddr) + index)

    End Function

    ' Weise MM-Adresse des Moduls zu
    Public Sub Set_MMAddr(addr As Integer) Implements IMbcModul.Set_MMAddr

        MMAddr = "&H" + FormHEX("0", 4, addr)

    End Sub

    ' Hole den MM-Schaltzustand des Moduls
    Public Function Get_MMValue() As Object Implements IMbcModul.Get_MMValue

        Get_MMValue = MMValue

    End Function

    ' Setze den MM-Schaltzustand des Moduls
    Public Sub Set_MMValue(index As Integer) Implements IMbcModul.Set_MMValue

        MMValue = "&H" & FormHEX("0", 2, index)
        SYS_ARRAY(MBC_83_MM) = FormHEX("0", 2, index)

    End Sub

    ' Hole die S88-Modulnummer des Moduls
    Public Function Get_S88ModNr() As Object Implements IMbcModul.Get_S88ModNr

        Get_S88ModNr = "&H0000"

    End Function

    ' Setze die S88-Modulnummer des Moduls

    Public Sub Set_S88ModNr(nr As Integer) Implements IMbcModul.Set_S88ModNr


    End Sub

    ' Hole den S88-Schaltzustand des Moduls
    Public Function Get_S88Value() As Object Implements IMbcModul.Get_S88Value

        Get_S88Value = "&H0000"

    End Function

    ' Setze den S88-Schaltzustand des Moduls
    Public Sub Set_S88Value(index As Integer) Implements IMbcModul.Set_S88Value


    End Sub

End Class
