Public Class mbc_modul_86_cl

    ' Beinhaltet Stammdaten

    Inherits mbc_modul_stammdaten_cl
    Implements IMbcModul

    '**********************************************************************************************
    '*** Sonderbelegung mbc-86                                                                  ***
    '**********************************************************************************************

    Private MBC_86_START = MBC_S_GENPURP                                 ' Start des Gesamtblocks mbc-86

    ' #define MBC_86_L_S88		4									    // Laenge modulspezifische Parameter
    ' #define MBC_86_S_S88		MBC_S_GENPURP						    // Index Start modulspezifische Parameter
    ' #define MBC_86_S88_H		sys_array[MBC_86_S_S88]			        // MMAddr HIGH
    ' #define MBC_86_S88_L		sys_array[MBC_86_S_S88 + 1]		        // MMAddr LOW
    ' #define MBC_86_S88_RM_H   sys_array[MBC_86_S_S88 + 2]			    // Eingang 16-9
    ' #define MBC_86_S88_RM_L   sys_array[MBC_86_S_S88 + 3]		        // Eingang 8-1

    Private MBC_86_L_S88 = 4                                             ' Laenge Modulnummern
    Private MBC_86_S_S88 = MBC_S_GENPURP                                 ' Index Start Modulnummern
    Private MBC_86_S88_H = MBC_86_S_S88                                  ' Index S88Addr HIGH
    Private MBC_86_S88_L = MBC_86_S_S88 + 1                              ' Index S88Addr LOW
    Private MBC_86_S88_RM_H = MBC_86_S_S88 + 2                           ' Eingang 16-9
    Private MBC_86_S88_RM_L = MBC_86_S_S88 + 3                           ' Eingang 8-1

    ' #define MBC_L_INTS88		16									    // Anzahl der S88PINS
    ' #define MBC_S_INTS88		(MBC_86_L_S88 + MBC_86_S_S88)		    // Index Start S88PINS
    ' #define MBC_INTS88		sys_array[MBC_S_INTS88]			        // Integrationszeit s88 

    Private MBC_86_L_INTS88 = 16                                         ' Anzahl der S88PINS
    Private MBC_86_S_INTS88 = MBC_86_L_S88 + MBC_86_S_S88                ' Index Start S88PINS
    Private MBC_86_INTS88 = MBC_86_S_INTS88                              ' Integrationszeit s88

    '#define MBC_86_L_MMLOK		1                                       // Anzahl der MM-Lok-Adresse
    '#define MBC_86_S_MMLOK	    (MBC_86_L_INTS88 + MBC_86_S_INTS88)     // Index Start MM-Lok-Adresse
    '#define MBC_86_MMLOK		modul.sys_array[MBC_86_S_MMLOK]	        // MM-Lok-Adresse

    Private MBC_86_L_MMLOK = 1                                           ' Anzahl der MM-Lok-Adresse
    Private MBC_86_S_MMLOK = MBC_86_L_INTS88 + MBC_86_S_INTS88           ' Index Start MM-Lok-Adresse
    Private MBC_86_MMLOK = MBC_86_S_MMLOK                                ' MM-Lok-Adresse

    Private MBC_86_ENDE = (MBC_86_S_MMLOK + MBC_86_L_MMLOK - 1)          ' Ende des Gesamtblocks mbc-86

    ' #define EE_S88                0x0A                                // ID Block Adresse an EE
    ' #define EE_S88_TIMING         0x0B                                // ID Block Integrationszeit an EE
    ' #define EE_85_MM_LOK	        0x0C				                // ID Block MM-Lok-Adresse

    Private EE_86_S88 = "0A"                                             ' ID Block Adresse an EE
    Private EE_86_S88_TIMING = "0B"                                      ' ID Block Spulentiming an EE
    Private EE_86_MM_LOK = "0C"                                          ' ID Block Lok-Adresse an EE

    '**********************************************************************************************
    '*** Modul Updatesteuerung                                                                  ***
    '**********************************************************************************************

    Private PROG_INDEX_S88ModNr = 1
    Private PROG_INDEX_S88Timing = 2
    Private PROG_INDEX_MMLOK = 3

    '**********************************************************************************************
    '*** Modul SYS_ARRAY-Indizes                                                                ***
    '**********************************************************************************************

    Public IOStart As Integer = MBC_86_START                             ' Startindex Parameterbereich
    Public IOEnde As Integer = MBC_86_ENDE                               ' Endindex Parameterbereich

    '**********************************************************************************************
    '*** Modul Bewegungsdaten                                                                   ***
    '**********************************************************************************************

    ' Startwerte fuer die Modulvariablen

    Private S88ModNr_New As String = "&H0000"
    Private S88Value_New As String = "&H0000"
    Private S88Timing_New As Integer = 10
    Private MMLok_New As Integer = 1

    ' Modulvariablen

    Public S88Value As String          ' Stellungsanzeige der s88-Rueckmeldung
    Public S88ModNr As String          ' S88-Modulnummer
    Public S88VZ_01 As Integer         ' Halteverzoegerung s88 Pin 1
    Public S88VZ_02 As Integer         ' Halteverzoegerung s88 Pin 2
    Public S88VZ_03 As Integer         ' Halteverzoegerung s88 Pin 3
    Public S88VZ_04 As Integer         ' Halteverzoegerung s88 Pin 4
    Public S88VZ_05 As Integer         ' Halteverzoegerung s88 Pin 5
    Public S88VZ_06 As Integer         ' Halteverzoegerung s88 Pin 6
    Public S88VZ_07 As Integer         ' Halteverzoegerung s88 Pin 7
    Public S88VZ_08 As Integer         ' Halteverzoegerung s88 Pin 8
    Public MMLok As Integer            ' MM-Lok-Adresse

    '**********************************************************************************************
    '*** Modul initialisieren                                                                   ***
    '**********************************************************************************************

    Sub New()

        S88ModNr = S88ModNr_New
        S88Value = S88Value_New
        S88VZ_01 = S88Timing_New
        S88VZ_02 = S88Timing_New
        S88VZ_03 = S88Timing_New
        S88VZ_04 = S88Timing_New
        S88VZ_05 = S88Timing_New
        S88VZ_06 = S88Timing_New
        S88VZ_07 = S88Timing_New
        S88VZ_08 = S88Timing_New
        MMLok = MMLok_New

    End Sub

    '**********************************************************************************************
    '*** Modul Funktionen                                                                       ***
    '**********************************************************************************************

    ' Sperren der Eingabemoeglichkeiten nach Aenderungen je
    ' nachdem welche Aenderung vorgenommen wurde
    ' 1 = s88 Modulnummer setzen
    ' 2 = Integrationszeit setzen
    ' 3 = MM-Adresse setzen

    Public Sub Tabpage_Sperren(block As Integer)

        If block = 1 Then
            frmMain.gb_mbc86_Integrator.Enabled = False
            frmMain.gb_mbc86_MMLok.Enabled = False
        End If

        If block = 2 Then
            frmMain.gb_mbc86_MMLok.Enabled = False
            frmMain.nud_mbc86_S88ModNr.Enabled = False
        End If

        If block = 3 Then
            frmMain.nud_mbc86_S88ModNr.Enabled = False
            frmMain.gb_mbc86_Integrator.Enabled = False
        End If

    End Sub

    ' Aktivieren der Eingabemoeglichkeiten 

    Public Sub Tabpage_Aktivieren() Implements IMbcModul.Tabpage_Aktivieren

        frmMain.nud_mbc86_S88ModNr.Enabled = True
        frmMain.nud_mbc86_S88ModNr.BackColor = Color.White

        frmMain.nud_mbc86_T_P01.Enabled = True
        frmMain.nud_mbc86_T_P01.BackColor = Color.White

        frmMain.nud_mbc86_T_P02.Enabled = True
        frmMain.nud_mbc86_T_P02.BackColor = Color.White

        frmMain.nud_mbc86_T_P03.Enabled = True
        frmMain.nud_mbc86_T_P03.BackColor = Color.White

        frmMain.nud_mbc86_T_P04.Enabled = True
        frmMain.nud_mbc86_T_P04.BackColor = Color.White

        frmMain.nud_mbc86_T_P05.Enabled = True
        frmMain.nud_mbc86_T_P05.BackColor = Color.White

        frmMain.nud_mbc86_T_P06.Enabled = True
        frmMain.nud_mbc86_T_P06.BackColor = Color.White

        frmMain.nud_mbc86_T_P07.Enabled = True
        frmMain.nud_mbc86_T_P07.BackColor = Color.White

        frmMain.nud_mbc86_T_P08.Enabled = True
        frmMain.nud_mbc86_T_P08.BackColor = Color.White

        frmMain.nud_mbc86_MMLok.Enabled = True
        frmMain.nud_mbc86_MMLok.BackColor = Color.White

        frmMain.gb_mbc86_Integrator.Enabled = True
        frmMain.gb_mbc86_MMLok.Enabled = True

    End Sub

    ' INDEX = 1
    ' Uebernehmen der s88-Modulnummer des Moduls

    Public Sub Tabpage_AddrSet()

        If (FLAG_Edit_Enable) Then
            FLAG_Prog_Index = PROG_INDEX_S88ModNr
            Tabpage_Sperren(1)
            frmMain.Register_Modul_Buttons_Enable(False, True, True, False, False, False)
            S88ModNr = "&h" & FormHEX("0", 4, frmMain.nud_mbc86_S88ModNr.Value)
            frmMain.nud_mbc86_S88ModNr.BackColor = Color.Wheat

            ' Busnummer eintragen

            If S88ModNr = 0 Then frmMain.txt_mbc86_BusNr.Text = 0
            If S88ModNr > 0 And S88ModNr < 32 Then frmMain.txt_mbc86_BusNr.Text = 1
            If S88ModNr > 31 And S88ModNr < 63 Then frmMain.txt_mbc86_BusNr.Text = 2
            If S88ModNr > 62 Then frmMain.txt_mbc86_BusNr.Text = 3

            ' Kontaktnummern eintragen

            If frmMain.nud_mbc86_S88ModNr.Value = 0 Then
                frmMain.txt_mbc86_kontakt1cs2.Text = Val(S88ModNr) + 1
                frmMain.txt_mbc86_kontakt2cs2.Text = Val(S88ModNr) + 16

            Else
                frmMain.txt_mbc86_kontakt1cs2.Text = frmMain.txt_mbc86_BusNr.Text * 1000 + (Val(S88ModNr) - 1 - (frmMain.txt_mbc86_BusNr.Text - 1) * 31) * 16 + 1
                frmMain.txt_mbc86_kontakt2cs2.Text = frmMain.txt_mbc86_BusNr.Text * 1000 + (Val(S88ModNr) - 1 - (frmMain.txt_mbc86_BusNr.Text - 1) * 31) * 16 + 16

            End If

        End If

    End Sub

    ' INDEX = 2
    ' Uebernehmen der Integrationszeit in Abhaengigkeit des ausgewaehlten Ausgangs

    Public Sub Tabpage_TimeSet(ByRef nud As Object, ByRef timing As Object)

        If (FLAG_Edit_Enable) Then
            Tabpage_Sperren(2)
            frmMain.Register_Modul_Buttons_Enable(False, True, True, False, False, False)
            FLAG_Prog_Index = PROG_INDEX_S88Timing
            timing = nud.value
            nud.BackColor = Color.Wheat
        End If

    End Sub


    ' INDEX = 3
    ' MM-Lok setzen
    Public Sub Tabpage_MM_Set()

        If (FLAG_Edit_Enable) Then

            FLAG_Prog_Index = PROG_INDEX_MMLOK

            frmMain.Register_Modul_Buttons_Enable(False, True, True, False, False, False)

            Tabpage_Sperren(3)

            MMLok = frmMain.nud_mbc86_MMLok.Value

            frmMain.nud_mbc86_MMLok.BackColor = Color.Wheat

        End If

    End Sub

    ' Zeige Modulwerte an
    Public Sub Tabpage_Werte_Show() Implements IMbcModul.Tabpage_Werte_Show

        ' Modulnummer eintragen

        Try
            frmMain.nud_mbc86_S88ModNr.Value = Val(S88ModNr)
        Catch e As Exception
            frmMain.nud_mbc86_S88ModNr.Value = Val(S88ModNr_New)
        End Try

        ' Busnummer eintragen

        If S88ModNr = 0 Then frmMain.txt_mbc86_BusNr.Text = 0
        If S88ModNr > 0 And S88ModNr < 32 Then frmMain.txt_mbc86_BusNr.Text = 1
        If S88ModNr > 31 And S88ModNr < 63 Then frmMain.txt_mbc86_BusNr.Text = 2
        If S88ModNr > 62 Then frmMain.txt_mbc86_BusNr.Text = 3

        ' Kontaktnummern eintragen

        If frmMain.nud_mbc86_S88ModNr.Value = 0 Then
            frmMain.txt_mbc86_kontakt1cs2.Text = Val(S88ModNr) + 1
            frmMain.txt_mbc86_kontakt2cs2.Text = Val(S88ModNr) + 16

        Else
            frmMain.txt_mbc86_kontakt1cs2.Text = frmMain.txt_mbc86_BusNr.Text * 1000 + (Val(S88ModNr) - 1 - (frmMain.txt_mbc86_BusNr.Text - 1) * 31) * 16 + 1
            frmMain.txt_mbc86_kontakt2cs2.Text = frmMain.txt_mbc86_BusNr.Text * 1000 + (Val(S88ModNr) - 1 - (frmMain.txt_mbc86_BusNr.Text - 1) * 31) * 16 + 16

        End If

        Try
            frmMain.nud_mbc86_T_P01.Value = Val(S88VZ_01)
        Catch e As Exception
            frmMain.nud_mbc86_T_P01.Value = Val(S88Timing_New)
        End Try
        Try
            frmMain.nud_mbc86_T_P02.Value = Val(S88VZ_02)
        Catch e As Exception
            frmMain.nud_mbc86_T_P02.Value = Val(S88Timing_New)
        End Try
        Try
            frmMain.nud_mbc86_T_P03.Value = Val(S88VZ_03)
        Catch e As Exception
            frmMain.nud_mbc86_T_P03.Value = Val(S88Timing_New)
        End Try
        Try
            frmMain.nud_mbc86_T_P04.Value = Val(S88VZ_04)
        Catch e As Exception
            frmMain.nud_mbc86_T_P04.Value = Val(S88Timing_New)
        End Try
        Try
            frmMain.nud_mbc86_T_P05.Value = Val(S88VZ_05)
        Catch e As Exception
            frmMain.nud_mbc86_T_P05.Value = Val(S88Timing_New)
        End Try
        Try
            frmMain.nud_mbc86_T_P06.Value = Val(S88VZ_06)
        Catch e As Exception
            frmMain.nud_mbc86_T_P06.Value = Val(S88Timing_New)
        End Try
        Try
            frmMain.nud_mbc86_T_P07.Value = Val(S88VZ_07)
        Catch e As Exception
            frmMain.nud_mbc86_T_P07.Value = Val(S88Timing_New)
        End Try
        Try
            frmMain.nud_mbc86_T_P08.Value = Val(S88VZ_08)
        Catch e As Exception
            frmMain.nud_mbc86_T_P08.Value = Val(S88Timing_New)
        End Try

        ' Trage MM-Lok ein

        Try
            frmMain.nud_mbc86_MMLok.Value = (MMLok)
        Catch e As Exception
            frmMain.nud_mbc86_MMLok.Value = 1
        End Try

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

        ' Weise die s88Addr zu und aktiviere den Updatemodus

        If FLAG_Prog_Index = PROG_INDEX_S88ModNr Then

            MsgIOStart = MBC_86_S88_H
            MsgIOMax = MBC_86_S88_L
            MsgIOWrite = EE_86_S88

            Tabpage_Aktivieren()

            SYS_ARRAY(MBC_86_S88_H) = Microsoft.VisualBasic.Mid(S88ModNr, 3, 2)
            SYS_ARRAY(MBC_86_S88_L) = Microsoft.VisualBasic.Right(S88ModNr, 2)

        End If

        ' Weise das Integrationszeit zu und aktiviere den Updatemodus

        If FLAG_Prog_Index = PROG_INDEX_S88Timing Then

            Tabpage_Aktivieren()

            MsgIOStart = MBC_86_S_INTS88
            MsgIOMax = MBC_86_S_INTS88 + MBC_86_L_INTS88 - 1
            MsgIOWrite = EE_86_S88_TIMING

            SYS_ARRAY(MBC_86_S_INTS88) = FormHEX("0", 2, S88VZ_01 / 10)
            SYS_ARRAY(MBC_86_S_INTS88 + 1) = FormHEX("0", 2, S88VZ_02 / 10)
            SYS_ARRAY(MBC_86_S_INTS88 + 2) = FormHEX("0", 2, S88VZ_03 / 10)
            SYS_ARRAY(MBC_86_S_INTS88 + 3) = FormHEX("0", 2, S88VZ_04 / 10)
            SYS_ARRAY(MBC_86_S_INTS88 + 4) = FormHEX("0", 2, S88VZ_05 / 10)
            SYS_ARRAY(MBC_86_S_INTS88 + 5) = FormHEX("0", 2, S88VZ_06 / 10)
            SYS_ARRAY(MBC_86_S_INTS88 + 6) = FormHEX("0", 2, S88VZ_07 / 10)
            SYS_ARRAY(MBC_86_S_INTS88 + 7) = FormHEX("0", 2, S88VZ_08 / 10)

        End If

        ' Weise ausgewaehlte MM-Lok dem Modul zu

        If FLAG_Prog_Index = PROG_INDEX_MMLOK Then

            MsgIOStart = MBC_86_S_MMLOK
            MsgIOMax = MBC_86_S_MMLOK
            MsgIOWrite = EE_86_MM_LOK

            SYS_ARRAY(MBC_86_MMLOK) = FormHEX("0", 2, MMLok)

            Tabpage_Aktivieren()

        End If

        ' Starte Aktualisierung des Moduls

        frmMain.COMUpdateStart(MsgIOStart, MsgIOMax, MsgIOWrite)

        ' Aktualisiere Anzeige

        Tabpage_Werte_Show()

    End Sub

    ' Fuelle Modul mit den aus dem Modul gelesenen spezifischenWerten und Parameter
    Public Sub Parameter_Fill() Implements IMbcModul.Parameter_Fill

        ' S88-Nummer

        S88ModNr = "&H" & SYS_ARRAY(MBC_86_S88_H) & SYS_ARRAY(MBC_86_S88_L)

        ' S88-Belegung

        S88Value = "&H" & SYS_ARRAY(MBC_86_S88_RM_H) & SYS_ARRAY(MBC_86_S88_RM_L)

        ' S88-Timing

        S88VZ_01 = 10 * Convert.ToInt32(SYS_ARRAY(MBC_86_S_INTS88), 16)
        S88VZ_02 = 10 * Convert.ToInt32(SYS_ARRAY(MBC_86_S_INTS88 + 1), 16)
        S88VZ_03 = 10 * Convert.ToInt32(SYS_ARRAY(MBC_86_S_INTS88 + 2), 16)
        S88VZ_04 = 10 * Convert.ToInt32(SYS_ARRAY(MBC_86_S_INTS88 + 3), 16)
        S88VZ_05 = 10 * Convert.ToInt32(SYS_ARRAY(MBC_86_S_INTS88 + 4), 16)
        S88VZ_06 = 10 * Convert.ToInt32(SYS_ARRAY(MBC_86_S_INTS88 + 5), 16)
        S88VZ_07 = 10 * Convert.ToInt32(SYS_ARRAY(MBC_86_S_INTS88 + 6), 16)
        S88VZ_08 = 10 * Convert.ToInt32(SYS_ARRAY(MBC_86_S_INTS88 + 7), 16)

        ' MM-Lok

        MMLok = Convert.ToInt32(SYS_ARRAY(MBC_86_MMLOK), 16)

    End Sub

    ' Steuere die LED-Anzeigen im Modulbild an
    Public Sub Aktorstellung_Show() Implements IMbcModul.Aktorstellung_Show

        Dim mm = Val(frmMain.Register_Get_S88Value(AktTyp, AktNr))

        If ((mm And 1) = 1) Then
            frmMain.pic_Status_LED11.Image = ImGELB
        Else
            frmMain.pic_Status_LED11.Image = ImGRAU
        End If

        If ((mm And 2) = 2) Then
            frmMain.pic_Status_LED12.Image = ImGELB
        Else
            frmMain.pic_Status_LED12.Image = ImGRAU
        End If

        If ((mm And 4) = 4) Then
            frmMain.pic_Status_LED13.Image = ImGELB
        Else
            frmMain.pic_Status_LED13.Image = ImGRAU
        End If

        If ((mm And 8) = 8) Then
            frmMain.pic_Status_LED14.Image = ImGELB
        Else
            frmMain.pic_Status_LED14.Image = ImGRAU
        End If

        If ((mm And 16) = 16) Then
            frmMain.pic_Status_LED15.Image = ImGELB
        Else
            frmMain.pic_Status_LED15.Image = ImGRAU
        End If

        If ((mm And 32) = 32) Then
            frmMain.pic_Status_LED16.Image = ImGELB
        Else
            frmMain.pic_Status_LED16.Image = ImGRAU
        End If

        If ((mm And 64) = 64) Then
            frmMain.pic_Status_LED17.Image = ImGELB
        Else
            frmMain.pic_Status_LED17.Image = ImGRAU
        End If

        If ((mm And 128) = 128) Then
            frmMain.pic_Status_LED18.Image = ImGELB
        Else
            frmMain.pic_Status_LED18.Image = ImGRAU
        End If

        If ((mm And 256) = 256) Then
            frmMain.pic_Status_LED19.Image = ImGELB
        Else
            frmMain.pic_Status_LED19.Image = ImGRAU
        End If

        If ((mm And 512) = 512) Then
            frmMain.pic_Status_LED07.Image = ImROT
        Else
            frmMain.pic_Status_LED07.Image = ImGRAU
        End If

    End Sub

    ' Hole MM-Adresse des Moduls zu
    Public Function Get_MMAddr(index As Integer) As Object Implements IMbcModul.Get_MMAddr

        Get_MMAddr = "&H0000"

    End Function

    ' Weise MM-Adresse des Moduls zu
    Public Sub Set_MMAddr(addr As Integer) Implements IMbcModul.Set_MMAddr

    End Sub

    ' Hole den MM-Schaltzustand des Moduls
    Public Function Get_MMValue() As Object Implements IMbcModul.Get_MMValue

        Get_MMValue = "&H00"

    End Function

    ' Setze den MM-Schaltzustand des Moduls
    Public Sub Set_MMValue(index As Integer) Implements IMbcModul.Set_MMValue


    End Sub

    ' Hole die S88-Modulnummer des Moduls
    Public Function Get_S88ModNr() As Object Implements IMbcModul.Get_S88ModNr

        Get_S88ModNr = S88ModNr

    End Function

    ' Setze die S88-Modulnummer des Moduls

    Public Sub Set_S88ModNr(nr As Integer) Implements IMbcModul.Set_S88ModNr

        S88ModNr = "&H" & FormHEX("0", 4, nr)

    End Sub

    ' Hole den S88-Schaltzustand des Moduls
    Public Function Get_S88Value() As Object Implements IMbcModul.Get_S88Value

        Get_S88Value = S88Value

    End Function

    ' Setze den S88-Schaltzustand des Moduls
    Public Sub Set_S88Value(index As Integer) Implements IMbcModul.Set_S88Value

        S88Value = "&H" & FormHEX("0", 4, index)

    End Sub


End Class
