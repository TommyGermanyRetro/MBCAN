Public Class mbc_modul_93_cl

    ' Beinhaltet Stammdaten

    Inherits mbc_modul_stammdaten_cl
    Implements IMbcModul

    '**********************************************************************************************
    '*** Sonderbelegung mbc-93                                                                  ***
    '**********************************************************************************************

    Private MBC_93_START = MBC_S_GENPURP                     ' Start des Gesamtblocks mbc-82

    '#define MBC_93_L_PARA			                        1
    '#define MBC_93_S_PARA			                        MBC_S_GENPURP
    '#define MBC_93_SPG_1			                        sys_array[MBC_93_S_PARA]
    '#define MBC_93_STATUS				                    sys_array[MBC_93_S_PARA]

    Private MBC_93_L_PARA = 1                                ' Laenge Parametersatz
    Private MBC_93_S_PARA = MBC_93_START                     ' Index Start Parametersatz
    Private MBC_93_STATUS = MBC_93_S_PARA                    ' Booster aktiv BIT x, Kurzschluss BIT y 

    '#define MBC_93_L_SW				                    2
    '#define MBC_93_S_SW 			                        (MBC_93_L_PARA + MBC_93_S_PARA)
    '#define MBC_93_SW_1				                    sys_array[MBC_93_S_SW]
    '#define MBC_93_SW_2				                    sys_array[MBC_93_S_SW + 1]

    Private MBC_93_L_SW = 2                                  ' Laenge Softwareversion GFP
    Private MBC_93_S_SW = (MBC_93_L_PARA + MBC_93_S_PARA)    ' Index Start Softwareversion GFP
    Private MBC_93_SW_1 = MBC_93_S_SW                        ' Softwareversion Vorkomma-Stelle
    Private MBC_93_SW_2 = MBC_93_S_SW + 1                    ' Softwareversion Nachkomma-Stellen

    '#define MBC_93_L_SH				                    1
    '#define MBC_93_S_SH 			                        (MBC_93_L_SW + MBC_93_S_SW)
    '#define MBC_93_SH				                        sys_array[MBC_93_S_SH]

    Private MBC_93_L_SH = 1                                  ' Laenge Kurzschlusslaenge
    Private MBC_93_S_SH = (MBC_93_L_SW + MBC_93_S_SW)        ' Index Start Kurzschlusslaenge
    Private MBC_93_SH = MBC_93_S_SH                          ' Kurzschlusslaenge

    Private MBC_93_ENDE = MBC_93_L_SH + MBC_93_S_SH - 1      ' Ende des Gesamtblocks mbc-82

    '#define EE_93_SW		0x0A			                // Softwareversion GFP/Booster
    '#define EE_93_SH	    0x0B			                // Kurzschlusslaenge

    Private EE_93_SW = "0A"                                  ' Softwareversion GFP/Booster
    Private EE_93_SH = "0B"                                  ' Latenzzeit Kurzschlusserkennung

    '**********************************************************************************************
    '*** Modul Updatesteuerung                                                                  ***
    '**********************************************************************************************

    Private PROG_INDEX_GFPSW = 1
    Private PROG_INDEX_KURZLAT = 2

    '**********************************************************************************************
    '*** Modul SYS_ARRAY-Indizes                                                                ***
    '**********************************************************************************************

    Public IOStart As Integer = MBC_93_START                             ' Startindex Parameterbereich
    Public IOEnde As Integer = MBC_93_ENDE                               ' Endindex Parameterbereich

    '**********************************************************************************************
    '*** Modul Bewegungsdaten                                                                   ***
    '**********************************************************************************************

    ' Startwerte fuer die Modulvariablen

    Private TRACKStatus_New As Integer = 0
    Private GFPSW_1_New As Integer = 3
    Private GFPSW_2_New As Integer = 81
    Private KURZLAT_New As Integer = 0
    Private KURZLAT_Min As Integer = 50

    ' Modulvariablen

    Public TRACKStatus As Integer      ' Booster an/aus und  Booster Kurzschluss
    Public GFPSW_1 As Integer          ' Softwareversion des GFP/Booster
    Public GFPSW_2 As Integer          ' Softwareversion des GFP/Booster
    Public KURZLAT As Integer          ' Latenzzeit Kurzschlusserkennung

    '**********************************************************************************************
    '*** Modul initialisieren                                                                   ***
    '**********************************************************************************************

    Sub New()

        TRACKStatus = TRACKStatus_New
        GFPSW_1 = GFPSW_1_New
        GFPSW_2 = GFPSW_2_New
        KURZLAT = KURZLAT_New

    End Sub

    '**********************************************************************************************
    '*** Modul Funktionen                                                                       ***
    '**********************************************************************************************

    ' Sperren der Eingabemoeglichkeiten nach Aenderungen je
    ' nachdem welche Aenderung vorgenommen wurde
    ' 0 = xxx
    ' 1 = xxx

    Public Sub Tabpage_Sperren(block As Integer)

        If block = 0 Then
            frmMain.nud_mbc93_Latenz.Enabled = False
        End If

        If block = 1 Then
            frmMain.nud_mbc93_GFPSW1.Enabled = False
            frmMain.nud_mbc93_GFPSW2.Enabled = False
        End If

    End Sub

    ' Aktivieren der Eingabemoeglichkeiten
    Public Sub Tabpage_Aktivieren() Implements IMbcModul.Tabpage_Aktivieren

        ' Die CSx-Anwesenheitssperre der Latenzzeit (und aller anderen Eingabefelder dieser
        ' Registerkarte) laeuft jetzt generell ueber tab_Module.Enabled, siehe
        ' Register_Modul_CSxLock_Apply() in frmMain.vb - hier nur noch der eigene Standardzustand

        frmMain.nud_mbc93_Latenz.Enabled = True
        frmMain.nud_mbc93_GFPSW1.Enabled = True
        frmMain.nud_mbc93_GFPSW2.Enabled = True

        frmMain.nud_mbc93_GFPSW1.BackColor = Color.White
        frmMain.nud_mbc93_GFPSW2.BackColor = Color.White
        frmMain.nud_mbc93_Latenz.BackColor = Color.White

    End Sub

    ' Zeige Modulwerte an
    Public Sub Tabpage_Werte_Show() Implements IMbcModul.Tabpage_Werte_Show

        Try
            frmMain.nud_mbc93_Latenz.Value = KURZLAT
        Catch e As Exception
            frmMain.nud_mbc93_Latenz.Value = KURZLAT_Min
        End Try

        Try
            frmMain.nud_mbc93_GFPSW1.Value = GFPSW_1
        Catch e As Exception
            frmMain.nud_mbc93_GFPSW1.Value = GFPSW_1_New
        End Try

        Try
            frmMain.nud_mbc93_GFPSW2.Value = GFPSW_2
        Catch e As Exception
            frmMain.nud_mbc93_GFPSW2.Value = GFPSW_2_New
        End Try

        frmMain.Register_Modul_Aktorstellung_Show()

    End Sub




    ' Latenzzeit Kurzschlusserkennung setzen
    Public Sub Tabpage_KurzLatenz()

        If (FLAG_Edit_Enable) Then

            FLAG_Prog_Index = PROG_INDEX_KURZLAT
            Tabpage_Sperren(1)
            frmMain.Register_Modul_Buttons_Enable(False, True, True, False, False, False)
            KURZLAT = frmMain.nud_mbc93_Latenz.Value
            frmMain.nud_mbc93_Latenz.BackColor = Color.Wheat

        End If

    End Sub

    ' GFP-Softwareversion setzen
    Public Sub Tabpage_GFPSWSet()

        If (FLAG_Edit_Enable) Then

            FLAG_Prog_Index = PROG_INDEX_GFPSW
            frmMain.Register_Modul_Buttons_Enable(False, True, True, False, False, False)
            Tabpage_Sperren(0)
            GFPSW_1 = frmMain.nud_mbc93_GFPSW1.Value
            GFPSW_2 = frmMain.nud_mbc93_GFPSW2.Value
            frmMain.nud_mbc93_GFPSW1.BackColor = Color.Wheat
            frmMain.nud_mbc93_GFPSW2.BackColor = Color.Wheat

        End If

    End Sub

    ' Fuehre Update des Moduls aus
    Public Sub Update() Implements IMbcModul.Update
        Dim MsgIOStart As Integer
        Dim MsgIOMax As Integer
        Dim MsgIOWrite As String

        MsgIOStart = 0
        MsgIOMax = 0
        MsgIOWrite = ""

        ' Weise die Softwareversion des L88 dem Modul zu

        If FLAG_Prog_Index = PROG_INDEX_GFPSW Then

            MsgIOStart = MBC_93_SW_1
            MsgIOMax = MBC_93_SW_2
            MsgIOWrite = EE_93_SW

            SYS_ARRAY(MBC_93_SW_1) = FormHEX("0", 2, GFPSW_1)
            SYS_ARRAY(MBC_93_SW_2) = FormHEX("0", 2, GFPSW_2)

            Tabpage_Aktivieren()

        End If

        ' Weise die Latenzzeit fuer die Kurzschlusserkennung dem Modul zu

        If FLAG_Prog_Index = PROG_INDEX_KURZLAT Then

            MsgIOStart = MBC_93_SH
            MsgIOMax = MBC_93_SH
            MsgIOWrite = EE_93_SH

            SYS_ARRAY(MBC_93_SH) = FormHEX("0", 2, KURZLAT / 50 - 1)

            Tabpage_Aktivieren()

        End If

        ' Starte Aktualisierung des Moduls

        frmMain.COMUpdateStart(MsgIOStart, MsgIOMax, MsgIOWrite)

    End Sub

    ' Fuelle Modul mit den aus dem Modul gelesenen spezifischen Werten und Parameter
    Public Sub Parameter_Fill() Implements IMbcModul.Parameter_Fill

        ' Latenzzeit Kurzschlusserkennung

        KURZLAT = (Convert.ToInt32(SYS_ARRAY(MBC_93_SH), 16) + 1) * 50

        ' GFP-Softwareversion

        GFPSW_1 = Convert.ToInt32(SYS_ARRAY(MBC_93_SW_1), 16)
        GFPSW_2 = Convert.ToInt32(SYS_ARRAY(MBC_93_SW_2), 16)

        ' Status

        TRACKStatus = Convert.ToInt32(SYS_ARRAY(MBC_93_STATUS), 16)

        If ((TRACKStatus And 16) = 16) Then
            frmMain.pic_Status_LED14.Image = ImROT
        Else
            frmMain.pic_Status_LED14.Image = ImGRAU
        End If

        If ((TRACKStatus And 32) = 32) Then
            frmMain.pic_Status_LED16.Image = ImGRUEN
        Else
            frmMain.pic_Status_LED16.Image = ImGRAU
        End If

    End Sub

    ' Steuere die LED-Anzeigen im Modulbild an
    Public Sub Aktorstellung_Show() Implements IMbcModul.Aktorstellung_Show


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
