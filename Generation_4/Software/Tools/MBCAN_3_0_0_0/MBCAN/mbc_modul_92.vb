Imports System.Threading.Tasks

Public Class mbc_modul_92_cl
    ' Beinhaltet Stammdaten

    Inherits mbc_modul_stammdaten_cl
    Implements IMbcModul

    '**********************************************************************************************
    '*** Sonderbelegung mbc-92                                                                  ***
    '**********************************************************************************************

    Private MBC_92_START = MBC_S_GENPURP                                 ' Start des Gesamtblocks mbc-83

    ' #define MBC_92_L_MM		6									    // Laenge modulspezifische Parameter
    ' #define MBC_92_S_MM		MBC_S_GENPURP						    // Index Start modulspezifische Parameter
    ' #define MBC_92_MM_H		sys_array[MBC_92_S_MM]			        // MMAddr HIGH
    ' #define MBC_92_MM_L		sys_array[MBC_92_S_MM + 1]		        // MMAddr LOW
    ' #define MBC_92_MM			sys_array[MBC_92_S_MM + 2]		        // Stellung der Aktoren
    ' #define MBC_92_MM_ART		sys_array[MBC_92_S_MM + 3]		        // Art der Aktoren
    ' #define MBC_92_AUS		sys_array[MBC_83_S_MM + 4]		        // Auto-Aus
    ' #define MBC_92_RUECK		sys_array[MBC_83_S_MM + 5]		        // Zwangsrueckmeldung

    Private MBC_92_L_MM = 6                                              ' Laenge modulspezifische Parameter
    Private MBC_92_S_MM = MBC_S_GENPURP                                  ' Index Start modulspezifische Parameter
    Private MBC_92_MM_H = MBC_92_S_MM                                    ' MMAddr HIGH
    Private MBC_92_MM_L = MBC_92_S_MM + 1                                ' MMAddr LOW
    Private MBC_92_MM = MBC_92_S_MM + 2                                  ' Stellung der Aktoren
    Private MBC_92_MM_ART = MBC_92_S_MM + 3                              ' Art der Aktoren
    Private MBC_92_AUS = MBC_92_S_MM + 4                                 ' Auto-Aus
    Private MBC_92_RUECK = MBC_92_S_MM + 5                               ' Zwangsrueckmeldung

    ' #define MBC_92_L_ZEIT	    4									    // Laenge MM-Spulenzeit
    ' #define MBC_92_S_ZEIT	    (MBC_92_L_MM + MBC_92_S_MM)	            // Index Start MM-Spulenzeit
    ' #define MBC_92_TIO		sys_array[MBC_92_S_ZEIT]			    // Einschaltzeit MM-Spulen

    Private MBC_92_L_ZEIT = 4                                            ' Laenge MM-Spulenzeit
    Private MBC_92_S_ZEIT = (MBC_92_L_MM + MBC_92_S_MM)                  ' Index Start MM-Spulenzeit
    Private MBC_92_TIO = MBC_92_S_ZEIT                                   ' Einschaltzeit MM-Spulen

    Private MBC_92_ENDE = (MBC_92_S_ZEIT + MBC_92_L_ZEIT - 1)            ' Ende des Gesamtblocks mbc-83

    ' #define EE_92_MMAddr		0x0A				                    // ID Block Adresse
    ' #define EE_92_MM_TIMING	0x0B				                    // ID Block Timing
    ' #define EE_92_MM_ART	    0x0C				                    // ID Block Art der Aktoren
    ' #define EE_92_AUS			0x0D				                    // ID Block Aus
    ' #define EE_92_RUECK		0x0E				                    // ID Block Zwangsrueckmeldung
    ' #define EE_92_AKTOR		0x0F				                    // ID Block Aktorenstellung

    Private EE_92_MMAddr = "0A"                                          ' ID Block Adresse
    Private EE_92_MM_TIMING = "0B"                                       ' ID Block Timing
    Private EE_92_MM_ART = "0C"                                          ' ID Block Art der Aktoren
    Private EE_92_AUS = "0D"                                             ' ID Block Aus
    Private EE_92_RUECK = "0E"                                           ' ID Block Zwangsrueckmeldung

    '**********************************************************************************************
    '*** Modul Updatesteuerung                                                                  ***
    '**********************************************************************************************

    Private PROG_INDEX_MMAddr = 0
    Private PROG_INDEX_MMTiming = 1
    Private PROG_INDEX_MMAus = 2
    Private PROG_INDEX_MMRueck = 3
    Private PROG_INDEX_MMArt = 4

    '**********************************************************************************************
    '*** Modul SYS_ARRAY-Indizes                                                                ***
    '**********************************************************************************************

    Public IOStart As Integer = MBC_92_START                             ' Startindex Parameterbereich
    Public IOEnde As Integer = MBC_92_ENDE                               ' Endindex Parameterbereich

    '**********************************************************************************************
    '*** Modul Bewegungsdaten                                                                   ***
    '**********************************************************************************************

    ' Startwerte fuer die Modulvariablen

    Private MM_New As String = "&H33FC"
    Private MMArt_New As Integer = "&H00"
    Private MMAus_New As Integer = 1000
    Private MMRueck_New As Integer = 0
    Private MMTiming_New As Integer = 10

    ' Modulvariablen

    Public MMValue As String           ' Stellungsanzeige der Magnetartikel
    Public MMAddr As String            ' Adresse im Maerklin-Adressbereich
    Public MMArt As String             ' Art der Aktoren
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
        MMAus = MMAus_New
        MMRueck = MMRueck_New
        MMValue = &B1010101
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

    Public Sub Tabpage_Sperren(block As Integer)

        If block = 0 Then
            frmMain.nud_mbc92_MMAddr.Enabled = False
            frmMain.nud_mbc92_Aus.Enabled = False
            frmMain.nud_mbc92_Rueck.Enabled = False
            frmMain.clb_mbc92_CH1.Enabled = False
            frmMain.clb_mbc92_CH2.Enabled = False
            frmMain.clb_mbc92_CH3.Enabled = False
            frmMain.clb_mbc92_CH4.Enabled = False
        End If

        If block = 1 Then
            frmMain.nud_mbc92_CH1_TIO.Enabled = False
            frmMain.nud_mbc92_CH2_TIO.Enabled = False
            frmMain.nud_mbc92_CH3_TIO.Enabled = False
            frmMain.nud_mbc92_CH4_TIO.Enabled = False
            frmMain.nud_mbc92_Aus.Enabled = False
            frmMain.nud_mbc92_Rueck.Enabled = False
            frmMain.clb_mbc92_CH1.Enabled = False
            frmMain.clb_mbc92_CH2.Enabled = False
            frmMain.clb_mbc92_CH3.Enabled = False
            frmMain.clb_mbc92_CH4.Enabled = False
        End If

        If block = 2 Then
            frmMain.nud_mbc92_CH1_TIO.Enabled = False
            frmMain.nud_mbc92_CH2_TIO.Enabled = False
            frmMain.nud_mbc92_CH3_TIO.Enabled = False
            frmMain.nud_mbc92_CH4_TIO.Enabled = False
            frmMain.nud_mbc92_MMAddr.Enabled = False
            frmMain.nud_mbc92_Rueck.Enabled = False
            frmMain.clb_mbc92_CH1.Enabled = False
            frmMain.clb_mbc92_CH2.Enabled = False
            frmMain.clb_mbc92_CH3.Enabled = False
            frmMain.clb_mbc92_CH4.Enabled = False
        End If

        If block = 3 Then
            frmMain.nud_mbc92_CH1_TIO.Enabled = False
            frmMain.nud_mbc92_CH2_TIO.Enabled = False
            frmMain.nud_mbc92_CH3_TIO.Enabled = False
            frmMain.nud_mbc92_CH4_TIO.Enabled = False
            frmMain.nud_mbc92_MMAddr.Enabled = False
            frmMain.nud_mbc92_Aus.Enabled = False
            frmMain.clb_mbc92_CH1.Enabled = False
            frmMain.clb_mbc92_CH2.Enabled = False
            frmMain.clb_mbc92_CH3.Enabled = False
            frmMain.clb_mbc92_CH4.Enabled = False
        End If


        If block = 4 Then
            frmMain.nud_mbc92_CH1_TIO.Enabled = False
            frmMain.nud_mbc92_CH2_TIO.Enabled = False
            frmMain.nud_mbc92_CH3_TIO.Enabled = False
            frmMain.nud_mbc92_CH4_TIO.Enabled = False
            frmMain.nud_mbc92_MMAddr.Enabled = False
            frmMain.nud_mbc92_Rueck.Enabled = False
        End If

    End Sub

    ' Aktivieren der Eingabemoeglichkeiten 

    Public Sub Tabpage_Aktivieren() Implements IMbcModul.Tabpage_Aktivieren

        frmMain.nud_mbc92_MMAddr.Enabled = True
        frmMain.nud_mbc92_MMAddr.BackColor = Color.White

        frmMain.nud_mbc92_Aus.Enabled = True
        frmMain.nud_mbc92_Aus.BackColor = Color.White

        frmMain.nud_mbc92_Rueck.Enabled = True
        frmMain.nud_mbc92_Rueck.BackColor = Color.White

        frmMain.nud_mbc92_CH1_TIO.Enabled = True
        frmMain.nud_mbc92_CH1_TIO.BackColor = Color.White

        frmMain.nud_mbc92_CH2_TIO.Enabled = True
        frmMain.nud_mbc92_CH2_TIO.BackColor = Color.White

        frmMain.nud_mbc92_CH3_TIO.Enabled = True
        frmMain.nud_mbc92_CH3_TIO.BackColor = Color.White

        frmMain.nud_mbc92_CH4_TIO.Enabled = True
        frmMain.nud_mbc92_CH4_TIO.BackColor = Color.White

        frmMain.clb_mbc92_CH1.Enabled = True
        frmMain.clb_mbc92_CH1.BackColor = Color.White

        frmMain.clb_mbc92_CH2.Enabled = True
        frmMain.clb_mbc92_CH2.BackColor = Color.White

        frmMain.clb_mbc92_CH3.Enabled = True
        frmMain.clb_mbc92_CH3.BackColor = Color.White

        frmMain.clb_mbc92_CH4.Enabled = True
        frmMain.clb_mbc92_CH4.BackColor = Color.White

    End Sub

    ' Anzeigen des Modultyps je nach Bestueckung

    Public Sub Tabpage_Typ()

        Dim index As Integer

        For index = 0 To 1
            frmMain.clb_mbc92_CH1.SetItemChecked(index, False)
            frmMain.clb_mbc92_CH2.SetItemChecked(index, False)
            frmMain.clb_mbc92_CH3.SetItemChecked(index, False)
            frmMain.clb_mbc92_CH4.SetItemChecked(index, False)
        Next

        index = (Val(MMArt) And &H3)
        If index < 2 Then frmMain.clb_mbc92_CH1.SetItemChecked(index, True)

        index = ((Val(MMArt) And &HC) / 4)
        If index < 2 Then frmMain.clb_mbc92_CH2.SetItemChecked(index, True)

        index = ((Val(MMArt) And &H30) / 16)
        If index < 2 Then frmMain.clb_mbc92_CH3.SetItemChecked(index, True)

        index = ((Val(MMArt) And &HC0) / 64)
        If index < 2 Then frmMain.clb_mbc92_CH4.SetItemChecked(index, True)


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

    ' Uebernehmen der MM-Adrresse des Moduls

    Public Sub Tabpage_AddrSet()

        If (FLAG_Edit_Enable) Then
            FLAG_Prog_Index = PROG_INDEX_MMAddr
            Tabpage_Sperren(1)
            frmMain.Register_Modul_Buttons_Enable(False, True, True, False, False, False)
            MMAddr = "&h" & FormHEX("0", 4, frmMain.nud_mbc92_MMAddr.Value)
            frmMain.nud_mbc92_MMAddr.BackColor = Color.Wheat
            frmMain.txt_mbc92_CH1_Addr.Text = Val("&H" & Hex(frmMain.nud_mbc92_MMAddr.Value)) - &H3000 + 1
            frmMain.txt_mbc92_CH2_Addr.Text = Val("&H" & Hex(frmMain.nud_mbc92_MMAddr.Value)) - &H3000 + 2
            frmMain.txt_mbc92_CH3_Addr.Text = Val("&H" & Hex(frmMain.nud_mbc92_MMAddr.Value)) - &H3000 + 3
            frmMain.txt_mbc92_CH4_Addr.Text = Val("&H" & Hex(frmMain.nud_mbc92_MMAddr.Value)) - &H3000 + 4
        End If

    End Sub

    ' Uebernehmen der Auto-Aus des Moduls

    Public Sub Tabpage_AusSet()

        If (FLAG_Edit_Enable) Then
            FLAG_Prog_Index = PROG_INDEX_MMAus
            Tabpage_Sperren(2)
            frmMain.Register_Modul_Buttons_Enable(False, True, True, False, False, False)
            MMAus = frmMain.nud_mbc92_Aus.Value
            frmMain.nud_mbc92_Aus.BackColor = Color.Wheat
        End If

    End Sub

    ' Uebernehmen des Rueckmeldezyklus des Moduls

    Public Sub Tabpage_RueckSet()

        If (FLAG_Edit_Enable) Then
            FLAG_Prog_Index = PROG_INDEX_MMRueck
            Tabpage_Sperren(3)
            frmMain.Register_Modul_Buttons_Enable(False, True, True, False, False, False)
            MMRueck = frmMain.nud_mbc92_Rueck.Value
            frmMain.nud_mbc92_Rueck.BackColor = Color.Wheat
        End If

    End Sub

    ' Uebernehmen des Nutzung des Moduls

    Public Sub Tabpage_NutzungSet(ByRef nud As Object, channel As Integer)

        Dim index As Integer
        Dim MMArtCH1, MMArtCH2, MMArtCH3, MMArtCH4 As Integer

        If (FLAG_Edit_Enable) Then
            FLAG_Prog_Index = PROG_INDEX_MMArt
            Tabpage_Sperren(4)
            frmMain.Register_Modul_Buttons_Enable(False, True, True, False, False, False)

            index = nud.SelectedIndex

            If index <> -1 Then

                If index = 0 Then
                    nud.SetItemChecked(1, False)
                    nud.SetItemChecked(0, True)
                End If
                If index = 1 Then
                    nud.SetItemChecked(0, False)
                    nud.SetItemChecked(1, True)
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
            frmMain.nud_mbc92_MMAddr.Value = Val(MMAddr)
        Catch e As Exception
            frmMain.nud_mbc92_MMAddr.Value = Val(MM_New)
        End Try

        frmMain.txt_mbc92_CH1_Addr.Text = Val("&H" & Hex(frmMain.nud_mbc92_MMAddr.Value)) - &H3000 + 1
        frmMain.txt_mbc92_CH2_Addr.Text = Val("&H" & Hex(frmMain.nud_mbc92_MMAddr.Value)) - &H3000 + 2
        frmMain.txt_mbc92_CH3_Addr.Text = Val("&H" & Hex(frmMain.nud_mbc92_MMAddr.Value)) - &H3000 + 3
        frmMain.txt_mbc92_CH4_Addr.Text = Val("&H" & Hex(frmMain.nud_mbc92_MMAddr.Value)) - &H3000 + 4

        Try
            frmMain.nud_mbc92_Aus.Value = Val(MMAus)
        Catch e As Exception
            frmMain.nud_mbc92_Aus.Value = 1000
        End Try
        Try
            frmMain.nud_mbc92_Rueck.Value = Val(MMRueck)
        Catch e As Exception
            frmMain.nud_mbc92_Rueck.Value = 0
        End Try
        Try
            frmMain.nud_mbc92_CH1_TIO.Value = Val(MMTiming_1)
        Catch e As Exception
            frmMain.nud_mbc92_CH1_TIO.Value = Val(MMTiming_New)
        End Try
        Try
            frmMain.nud_mbc92_CH2_TIO.Value = Val(MMTiming_2)
        Catch e As Exception
            frmMain.nud_mbc92_CH2_TIO.Value = Val(MMTiming_New)
        End Try
        Try
            frmMain.nud_mbc92_CH3_TIO.Value = Val(MMTiming_3)
        Catch e As Exception
            frmMain.nud_mbc92_CH3_TIO.Value = Val(MMTiming_New)
        End Try
        Try
            frmMain.nud_mbc92_CH4_TIO.Value = Val(MMTiming_4)
        Catch e As Exception
            frmMain.nud_mbc92_CH4_TIO.Value = Val(MMTiming_New)
        End Try

        Tabpage_Typ()

        Aktorstellung_Show()

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

            MsgIOStart = MBC_92_MM_H
            MsgIOMax = MBC_92_MM_L
            MsgIOWrite = EE_92_MMAddr

            Tabpage_Aktivieren()

            SYS_ARRAY(MBC_92_MM_H) = Microsoft.VisualBasic.Mid(MMAddr, 3, 2)
            SYS_ARRAY(MBC_92_MM_L) = Microsoft.VisualBasic.Right(MMAddr, 2)

        End If

        ' Weise das Spulentiming zu und aktiviere den Updatemodus

        If FLAG_Prog_Index = PROG_INDEX_MMTiming Then

            MsgIOStart = MBC_92_S_ZEIT
            MsgIOMax = MBC_92_S_ZEIT + MBC_92_L_ZEIT - 1
            MsgIOWrite = EE_92_MM_TIMING

            Tabpage_Aktivieren()

            SYS_ARRAY(MBC_92_S_ZEIT) = FormHEX("0", 2, MMTiming_1 / 10)
            SYS_ARRAY(MBC_92_S_ZEIT + 1) = FormHEX("0", 2, MMTiming_2 / 10)
            SYS_ARRAY(MBC_92_S_ZEIT + 2) = FormHEX("0", 2, MMTiming_3 / 10)
            SYS_ARRAY(MBC_92_S_ZEIT + 3) = FormHEX("0", 2, MMTiming_4 / 10)
        End If

        ' Weise Auto-Aus zu

        If FLAG_Prog_Index = PROG_INDEX_MMAus Then

            MsgIOStart = MBC_92_AUS
            MsgIOMax = MBC_92_AUS
            MsgIOWrite = EE_92_AUS

            Tabpage_Aktivieren()

            SYS_ARRAY(MBC_92_AUS) = FormHEX("0", 2, MMAus / 10)

        End If

        ' Weise Meldezyklus zu

        If FLAG_Prog_Index = PROG_INDEX_MMRueck Then

            MsgIOStart = MBC_92_RUECK
            MsgIOMax = MBC_92_RUECK
            MsgIOWrite = EE_92_RUECK

            Tabpage_Aktivieren()

            SYS_ARRAY(MBC_92_RUECK) = FormHEX("0", 2, MMRueck)

        End If

        ' Weise Nutzungsart zu

        If FLAG_Prog_Index = PROG_INDEX_MMArt Then

            MsgIOStart = MBC_92_MM_ART
            MsgIOMax = MBC_92_MM_ART
            MsgIOWrite = EE_92_MM_ART

            Tabpage_Aktivieren()

            SYS_ARRAY(MBC_92_MM_ART) = Microsoft.VisualBasic.Right(MMArt, 2)

        End If

        ' Starte Aktualisierung des Moduls

        frmMain.COMUpdateStart(MsgIOStart, MsgIOMax, MsgIOWrite)

    End Sub

    ' Fuelle Modul mit den aus dem Modul gelesenen spezifischenWerten und Parameter
    Public Sub Parameter_Fill() Implements IMbcModul.Parameter_Fill

        ' MMAdresse 

        MMAddr = "&H" & SYS_ARRAY(MBC_92_MM_H) & SYS_ARRAY(MBC_92_MM_L)

        ' MM-Stellungsanzeige

        MMValue = "&H" & SYS_ARRAY(MBC_92_MM)

        ' Bestueckung mbc-92

        MMArt = "&H" & SYS_ARRAY(MBC_92_MM_ART)

        ' Auto-Aus an die Magnetartikel 

        MMAus = 10 * Convert.ToInt32(SYS_ARRAY(MBC_92_AUS), 16)

        ' Zwangsrueckmeldung der Magnetartikel 

        MMRueck = Convert.ToInt32(SYS_ARRAY(MBC_92_RUECK), 16)

        ' Spulen-Timing mbc-92

        MMTiming_1 = 10 * Convert.ToInt32(SYS_ARRAY(MBC_92_S_ZEIT), 16)
        MMTiming_2 = 10 * Convert.ToInt32(SYS_ARRAY(MBC_92_S_ZEIT + 1), 16)
        MMTiming_3 = 10 * Convert.ToInt32(SYS_ARRAY(MBC_92_S_ZEIT + 2), 16)
        MMTiming_4 = 10 * Convert.ToInt32(SYS_ARRAY(MBC_92_S_ZEIT + 3), 16)

    End Sub

    ' Steuere die LED-Anzeigen im Modulbild an
    Public Sub Aktorstellung_Show() Implements IMbcModul.Aktorstellung_Show


        Dim mm = MMValue

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
        SYS_ARRAY(MBC_92_MM) = FormHEX("0", 2, index)

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
