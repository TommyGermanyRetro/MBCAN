Public Class mbc_modul_91_cl

    ' Beinhaltet Stammdaten

    Inherits mbc_modul_stammdaten_cl
    Implements IMbcModul

    '**********************************************************************************************
    '*** Sonderbelegung mbc-91                                                                  ***
    '**********************************************************************************************

    Private MBC_91_START = MBC_S_GENPURP                                 ' Start des Gesamtblocks mbc-83

    ' #define MBC_91_L_MM		6									    // Laenge modulspezifische Parameter
    ' #define MBC_91_S_MM		MBC_S_GENPURP						    // Index Start modulspezifische Parameter
    ' #define MBC_91_MM_H		sys_array[MBC_91_S_MM]			        // MMAddr HIGH
    ' #define MBC_91_MM_L		sys_array[MBC_91_S_MM + 1]		        // MMAddr LOW
    ' #define MBC_91_MM			sys_array[MBC_91_S_MM + 2]		        // Stellung der Aktoren
    ' #define MBC_91_MM_ART		sys_array[MBC_91_S_MM + 3]		        // Art der Aktoren
    ' #define MBC_91_AUS		sys_array[MBC_83_S_MM + 4]		        // Auto-Aus
    ' #define MBC_91_RUECK		sys_array[MBC_83_S_MM + 5]		        // Zwangsrueckmeldung

    Private MBC_91_L_MM = 6                                              ' Laenge modulspezifische Parameter
    Private MBC_91_S_MM = MBC_S_GENPURP                                  ' Index Start modulspezifische Parameter
    Private MBC_91_MM_H = MBC_91_S_MM                                    ' MMAddr HIGH
    Private MBC_91_MM_L = MBC_91_S_MM + 1                                ' MMAddr LOW
    Private MBC_91_MM = MBC_91_S_MM + 2                                  ' Stellung der Aktoren
    Private MBC_91_MM_ART = MBC_91_S_MM + 3                              ' Art der Aktoren
    Private MBC_91_AUS = MBC_91_S_MM + 4                                 ' Auto-Aus
    Private MBC_91_RUECK = MBC_91_S_MM + 5                               ' Zwangsrueckmeldung

    ' #define MBC_91_L_ZEIT	    4									    // Laenge MM-Spulenzeit
    ' #define MBC_91_S_ZEIT	    (MBC_91_L_MM + MBC_91_S_MM)	            // Index Start MM-Spulenzeit
    ' #define MBC_91_TIO		sys_array[MBC_91_S_ZEIT]			    // Einschaltzeit MM-Spulen

    Private MBC_91_L_ZEIT = 4                                            ' Laenge MM-Spulenzeit
    Private MBC_91_S_ZEIT = (MBC_91_L_MM + MBC_91_S_MM)                  ' Index Start MM-Spulenzeit
    Private MBC_91_TIO = MBC_91_S_ZEIT                                   ' Einschaltzeit MM-Spulen

    Private MBC_91_ENDE = (MBC_91_S_ZEIT + MBC_91_L_ZEIT - 1)            ' Ende des Gesamtblocks mbc-83

    ' #define EE_91_MMAddr		0x0A								    // ID Block Adresse
    ' #define EE_91_MM_TIMING	0x0B								    // ID Block Timing
    ' #define EE_91_MM_ART	    0x0C								    // ID Block Art der Aktoren
    ' #define EE_91_AUS			0x0D								    // ID Block Aus
    ' #define EE_91_RUECK	    0x0E								    // ID Block Zwangsrueckmeldung

    Private EE_91_MMAddr = "0A"                                          ' ID Block Adresse
    Private EE_91_MM_TIMING = "0B"                                       ' ID Block Timing
    Private EE_91_MM_ART = "0C"                                          ' ID Block Art der Aktoren
    Private EE_91_AUS = "0D"                                             ' ID Block Aus
    Private EE_91_RUECK = "0E"                                           ' ID Block Zwangsrueckmeldung

    '**********************************************************************************************
    '*** Modul Updatesteuerung                                                                  ***
    '**********************************************************************************************

    Private PROG_INDEX_MMAddr = 0
    Private PROG_INDEX_MMTiming = 1
    Private PROG_INDEX_MMAus = 2
    Private PROG_INDEX_MMRueck = 3

    '**********************************************************************************************
    '*** Modul SYS_ARRAY-Indizes                                                                ***
    '**********************************************************************************************

    Public IOStart As Integer = MBC_91_START                             ' Startindex Parameterbereich
    Public IOEnde As Integer = MBC_91_ENDE                               ' Endindex Parameterbereich

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
            frmMain.nud_mbc91_MMAddr.Enabled = False
            frmMain.nud_mbc91_Aus.Enabled = False
            frmMain.nud_mbc91_Rueck.Enabled = False
        End If

        If block = 1 Then
            frmMain.nud_mbc91_CH1_TIO.Enabled = False
            frmMain.nud_mbc91_CH2_TIO.Enabled = False
            frmMain.nud_mbc91_CH3_TIO.Enabled = False
            frmMain.nud_mbc91_CH4_TIO.Enabled = False
            frmMain.nud_mbc91_Aus.Enabled = False
            frmMain.nud_mbc91_Rueck.Enabled = False
        End If

        If block = 2 Then
            frmMain.nud_mbc91_CH1_TIO.Enabled = False
            frmMain.nud_mbc91_CH2_TIO.Enabled = False
            frmMain.nud_mbc91_CH3_TIO.Enabled = False
            frmMain.nud_mbc91_CH4_TIO.Enabled = False
            frmMain.nud_mbc91_MMAddr.Enabled = False
            frmMain.nud_mbc91_Rueck.Enabled = False
        End If

        If block = 3 Then
            frmMain.nud_mbc91_CH1_TIO.Enabled = False
            frmMain.nud_mbc91_CH2_TIO.Enabled = False
            frmMain.nud_mbc91_CH3_TIO.Enabled = False
            frmMain.nud_mbc91_CH4_TIO.Enabled = False
            frmMain.nud_mbc91_MMAddr.Enabled = False
            frmMain.nud_mbc91_Aus.Enabled = False
        End If

    End Sub

    ' Aktivieren der Eingabemoeglichkeiten 

    Public Sub Tabpage_Aktivieren() Implements IMbcModul.Tabpage_Aktivieren

        frmMain.nud_mbc91_MMAddr.Enabled = True
        frmMain.nud_mbc91_MMAddr.BackColor = Color.White

        frmMain.nud_mbc91_Aus.Enabled = True
        frmMain.nud_mbc91_Aus.BackColor = Color.White

        frmMain.nud_mbc91_Rueck.Enabled = True
        frmMain.nud_mbc91_Rueck.BackColor = Color.White

        frmMain.nud_mbc91_CH1_TIO.Enabled = True
        frmMain.nud_mbc91_CH1_TIO.BackColor = Color.White

        frmMain.nud_mbc91_CH2_TIO.Enabled = True
        frmMain.nud_mbc91_CH2_TIO.BackColor = Color.White

        frmMain.nud_mbc91_CH3_TIO.Enabled = True
        frmMain.nud_mbc91_CH3_TIO.BackColor = Color.White

        frmMain.nud_mbc91_CH4_TIO.Enabled = True
        frmMain.nud_mbc91_CH4_TIO.BackColor = Color.White

    End Sub

    ' Anzeigen des Modultyps je nach Bestueckung

    Public Sub Tabpage_Typ()

        If (Val(MMArt) And 1) Then
            ' Checkbox in der Registerkarte
            frmMain.clb_mbc91_CH1.SetItemChecked(1, True)
            frmMain.clb_mbc91_CH1.SetItemChecked(0, False)
            ' Checkbox im der Modulbild
            frmMain.pic_Status_ASM02.Visible = True
            frmMain.pic_Status_ASM01.Visible = False
        Else
            ' Checkbox in der Registerkarte
            frmMain.clb_mbc91_CH1.SetItemChecked(1, False)
            frmMain.clb_mbc91_CH1.SetItemChecked(0, True)
            ' Checkbox im der Modulbild
            frmMain.pic_Status_ASM01.Visible = True
            frmMain.pic_Status_ASM02.Visible = False
        End If

        If (Val(MMArt) And 4) Then
            ' Checkbox in der Registerkarte
            frmMain.clb_mbc91_CH2.SetItemChecked(1, True)
            frmMain.clb_mbc91_CH2.SetItemChecked(0, False)
            ' Checkbox im der Modulbild
            frmMain.pic_Status_ASM04.Visible = True
            frmMain.pic_Status_ASM03.Visible = False
        Else
            ' Checkbox in der Registerkarte
            frmMain.clb_mbc91_CH2.SetItemChecked(1, False)
            frmMain.clb_mbc91_CH2.SetItemChecked(0, True)
            ' Checkbox im der Modulbild
            frmMain.pic_Status_ASM03.Visible = True
            frmMain.pic_Status_ASM04.Visible = False
        End If

        If (Val(MMArt) And 16) Then
            ' Checkbox in der Registerkarte
            frmMain.clb_mbc91_CH3.SetItemChecked(1, True)
            frmMain.clb_mbc91_CH3.SetItemChecked(0, False)
            ' Checkbox im der Modulbild
            frmMain.pic_Status_ASM06.Visible = True
            frmMain.pic_Status_ASM05.Visible = False
        Else
            ' Checkbox in der Registerkarte
            frmMain.clb_mbc91_CH3.SetItemChecked(1, False)
            frmMain.clb_mbc91_CH3.SetItemChecked(0, True)
            ' Checkbox im der Modulbild
            frmMain.pic_Status_ASM05.Visible = True
            frmMain.pic_Status_ASM06.Visible = False
        End If

        If (Val(MMArt) And 64) Then
            ' Checkbox in der Registerkarte
            frmMain.clb_mbc91_CH4.SetItemChecked(1, True)
            frmMain.clb_mbc91_CH4.SetItemChecked(0, False)
            ' Checkbox im der Modulbild
            frmMain.pic_Status_ASM08.Visible = True
            frmMain.pic_Status_ASM07.Visible = False
        Else
            ' Checkbox in der Registerkarte
            frmMain.clb_mbc91_CH4.SetItemChecked(1, False)
            frmMain.clb_mbc91_CH4.SetItemChecked(0, True)
            ' Checkbox im der Modulbild
            frmMain.pic_Status_ASM07.Visible = True
            frmMain.pic_Status_ASM08.Visible = False
        End If


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
            MMAddr = "&h" & FormHEX("0", 4, frmMain.nud_mbc91_MMAddr.Value)
            frmMain.nud_mbc91_MMAddr.BackColor = Color.Wheat
            frmMain.txt_mbc91_CH1_Addr.Text = Val("&H" & Hex(frmMain.nud_mbc91_MMAddr.Value)) - &H3000 + 1
            frmMain.txt_mbc91_CH2_Addr.Text = Val("&H" & Hex(frmMain.nud_mbc91_MMAddr.Value)) - &H3000 + 2
            frmMain.txt_mbc91_CH3_Addr.Text = Val("&H" & Hex(frmMain.nud_mbc91_MMAddr.Value)) - &H3000 + 3
            frmMain.txt_mbc91_CH4_Addr.Text = Val("&H" & Hex(frmMain.nud_mbc91_MMAddr.Value)) - &H3000 + 4
        End If

    End Sub

    ' Uebernehmen der Auto-Aus des Moduls

    Public Sub Tabpage_AusSet()

        If (FLAG_Edit_Enable) Then
            FLAG_Prog_Index = PROG_INDEX_MMAus
            Tabpage_Sperren(2)
            frmMain.Register_Modul_Buttons_Enable(False, True, True, False, False, False)
            MMAus = frmMain.nud_mbc91_Aus.Value
            frmMain.nud_mbc91_Aus.BackColor = Color.Wheat
        End If

    End Sub

    ' Uebernehmen des Rueckmeldezyklus des Moduls

    Public Sub Tabpage_RueckSet()

        If (FLAG_Edit_Enable) Then
            FLAG_Prog_Index = PROG_INDEX_MMRueck
            Tabpage_Sperren(3)
            frmMain.Register_Modul_Buttons_Enable(False, True, True, False, False, False)
            MMRueck = frmMain.nud_mbc91_Rueck.Value
            frmMain.nud_mbc91_Rueck.BackColor = Color.Wheat
        End If

    End Sub

    ' Zeige Modulwerte an

    Public Sub Tabpage_Werte_Show() Implements IMbcModul.Tabpage_Werte_Show

        Try
            frmMain.nud_mbc91_MMAddr.Value = Val(MMAddr)
        Catch e As Exception
            frmMain.nud_mbc91_MMAddr.Value = Val(MM_New)
        End Try

        frmMain.txt_mbc91_CH1_Addr.Text = Val("&H" & Hex(frmMain.nud_mbc91_MMAddr.Value)) - &H3000 + 1
        frmMain.txt_mbc91_CH2_Addr.Text = Val("&H" & Hex(frmMain.nud_mbc91_MMAddr.Value)) - &H3000 + 2
        frmMain.txt_mbc91_CH3_Addr.Text = Val("&H" & Hex(frmMain.nud_mbc91_MMAddr.Value)) - &H3000 + 3
        frmMain.txt_mbc91_CH4_Addr.Text = Val("&H" & Hex(frmMain.nud_mbc91_MMAddr.Value)) - &H3000 + 4

        Try
            frmMain.nud_mbc91_Aus.Value = Val(MMAus)
        Catch e As Exception
            frmMain.nud_mbc91_Aus.Value = 1000
        End Try
        Try
            frmMain.nud_mbc91_Rueck.Value = Val(MMRueck)
        Catch e As Exception
            frmMain.nud_mbc91_Rueck.Value = 0
        End Try
        Try
            frmMain.nud_mbc91_CH1_TIO.Value = Val(MMTiming_1)
        Catch e As Exception
            frmMain.nud_mbc91_CH1_TIO.Value = Val(MMTiming_New)
        End Try
        Try
            frmMain.nud_mbc91_CH2_TIO.Value = Val(MMTiming_2)
        Catch e As Exception
            frmMain.nud_mbc91_CH2_TIO.Value = Val(MMTiming_New)
        End Try
        Try
            frmMain.nud_mbc91_CH3_TIO.Value = Val(MMTiming_3)
        Catch e As Exception
            frmMain.nud_mbc91_CH3_TIO.Value = Val(MMTiming_New)
        End Try
        Try
            frmMain.nud_mbc91_CH4_TIO.Value = Val(MMTiming_4)
        Catch e As Exception
            frmMain.nud_mbc91_CH4_TIO.Value = Val(MMTiming_New)
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

            MsgIOStart = MBC_91_MM_H
            MsgIOMax = MBC_91_MM_L
            MsgIOWrite = EE_91_MMAddr

            Tabpage_Aktivieren()

            SYS_ARRAY(MBC_91_MM_H) = Microsoft.VisualBasic.Mid(MMAddr, 3, 2)
            SYS_ARRAY(MBC_91_MM_L) = Microsoft.VisualBasic.Right(MMAddr, 2)

        End If

        ' Weise das Spulentiming zu und aktiviere den Updatemodus

        If FLAG_Prog_Index = PROG_INDEX_MMTiming Then

            MsgIOStart = MBC_91_S_ZEIT
            MsgIOMax = MBC_91_S_ZEIT + MBC_91_L_ZEIT - 1
            MsgIOWrite = EE_91_MM_TIMING

            Tabpage_Aktivieren()

            SYS_ARRAY(MBC_91_S_ZEIT) = FormHEX("0", 2, MMTiming_1 / 10)
            SYS_ARRAY(MBC_91_S_ZEIT + 1) = FormHEX("0", 2, MMTiming_2 / 10)
            SYS_ARRAY(MBC_91_S_ZEIT + 2) = FormHEX("0", 2, MMTiming_3 / 10)
            SYS_ARRAY(MBC_91_S_ZEIT + 3) = FormHEX("0", 2, MMTiming_4 / 10)
        End If

        ' Weise Auto-Aus zu

        If FLAG_Prog_Index = PROG_INDEX_MMAus Then

            MsgIOStart = MBC_91_AUS
            MsgIOMax = MBC_91_AUS
            MsgIOWrite = EE_91_AUS

            Tabpage_Aktivieren()

            SYS_ARRAY(MBC_91_AUS) = FormHEX("0", 2, MMAus / 10)

        End If

        ' Weise Meldezyklus zu

        If FLAG_Prog_Index = PROG_INDEX_MMRueck Then

            MsgIOStart = MBC_91_RUECK
            MsgIOMax = MBC_91_RUECK
            MsgIOWrite = EE_91_RUECK

            Tabpage_Aktivieren()

            SYS_ARRAY(MBC_91_RUECK) = FormHEX("0", 2, MMRueck)

        End If

        ' Starte Aktualisierung des Moduls

        frmMain.COMUpdateStart(MsgIOStart, MsgIOMax, MsgIOWrite)

    End Sub

    ' Fuelle Modul mit den aus dem Modul gelesenen spezifischenWerten und Parameter
    Public Sub Parameter_Fill() Implements IMbcModul.Parameter_Fill

        ' MMAdresse 

        MMAddr = "&H" & SYS_ARRAY(MBC_91_MM_H) & SYS_ARRAY(MBC_91_MM_L)

        ' MM-Stellungsanzeige

        MMValue = "&H" & SYS_ARRAY(MBC_91_MM)

        ' Bestueckung mbc91

        MMArt = "&H" & SYS_ARRAY(MBC_91_MM_ART)

        ' Auto-Aus an die Magnetartikel 

        MMAus = 10 * Convert.ToInt32(SYS_ARRAY(MBC_91_AUS), 16)

        ' Zwangsrueckmeldung der Magnetartikel 

        MMRueck = Convert.ToInt32(SYS_ARRAY(MBC_91_RUECK), 16)

        ' Spulen-Timing mbc-91

        MMTiming_1 = 10 * Convert.ToInt32(SYS_ARRAY(MBC_91_S_ZEIT), 16)
        MMTiming_2 = 10 * Convert.ToInt32(SYS_ARRAY(MBC_91_S_ZEIT + 1), 16)
        MMTiming_3 = 10 * Convert.ToInt32(SYS_ARRAY(MBC_91_S_ZEIT + 2), 16)
        MMTiming_4 = 10 * Convert.ToInt32(SYS_ARRAY(MBC_91_S_ZEIT + 3), 16)

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
        SYS_ARRAY(MBC_91_MM) = FormHEX("0", 2, index)

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
