Public Class mbc_modul_81_cl

    ' Beinhaltet Stammdaten

    Inherits mbc_modul_stammdaten_cl
    Implements IMbcModul

    '**********************************************************************************************
    '*** Sonderbelegung mbc-81                                                                  ***
    '**********************************************************************************************

    Private MBC_81_START = MBC_S_GENPURP                                 ' Start des Gesamtblocks mbc-81

    ' #define MBC_81_L_MM		2									    // Laenge modulspezifische Parameter
    ' #define MBC_81_S_MM		MBC_S_GENPURP						    // Index Start modulspezifische Parameter
    ' #define MBC_81_MM_H		sys_array[MBC_81_S_MM]			        // MMAddr HIGH
    ' #define MBC_81_MM_L		sys_array[MBC_81_S_MM + 1]		        // MMAddr LOW

    Private MBC_81_L_MM = 2                                              ' Laenge modulspezifische Parameter
    Private MBC_81_S_MM = MBC_S_GENPURP                                  ' Index Start modulspezifische Parameter
    Private MBC_81_MM_H = MBC_81_S_MM                                    ' MMAddr HIGH
    Private MBC_81_MM_L = MBC_81_S_MM + 1                                ' MMAddr LOW

    '#define MBC_81_L_STATUS	1									    // Laenge modulspezifische Parameter
    '#define MBC_81_S_STATUS	(MBC_81_L_MM + MBC_81_S_MM)			    // Index Start modulspezifische Parameter
    '#define MBC_81_STATUS	sys_array[MBC_81_S_STATUS]		            // Nutzung des Moduls

    Private MBC_81_L_STATUS = 1                                          ' Laenge modulspezifische Parameter
    Private MBC_81_S_STATUS = MBC_81_L_MM + MBC_81_S_MM                  ' Index Start modulspezifische Parameter
    Private MBC_81_STATUS = MBC_81_S_STATUS                              ' MMAddr HIGH

    Private MBC_81_ENDE = (MBC_81_S_STATUS + MBC_81_L_STATUS - 1)        ' Ende des Gesamtblocks mbc-81

    ' #define EE_81_MMAddr		0x0A							        // ID Block Adresse
    ' #define EE_81_Nutzung		0x0B				                    // ID Block Nutzung

    Private EE_81_MMAddr = "0A"                                          ' ID Block Adresse
    Private EE_81_Nutzung = "0B"                                         ' ID Block Nutzung

    '**********************************************************************************************
    '*** Modul Updatesteuerung                                                                  ***
    '**********************************************************************************************

    Private PROG_INDEX_MMAddr = 0
    Private PROG_INDEX_BUS0 = 1

    '**********************************************************************************************
    '*** Modul SYS_ARRAY-Indizes                                                                ***
    '**********************************************************************************************

    Public IOStart As Integer = MBC_81_START                             ' Startindex Parameterbereich
    Public IOEnde As Integer = MBC_81_ENDE                               ' Endindex Parameterbereich

    '**********************************************************************************************
    '*** Modul Bewegungsdaten                                                                   ***
    '**********************************************************************************************

    ' Startwerte fuer die Modulvariablen

    Private MM_SW_New As String = "&H33F0"

    ' Modulvariablen

    Public MMAddr As String            ' Adresse im Maerklin-Adressbereich
    Public BUS0 As Integer             ' Nutzung des Bus 0

    '**********************************************************************************************
    '*** Modul initialisieren                                                                   ***
    '**********************************************************************************************

    Sub New()

        MMAddr = MM_SW_New
        BUS0 = 0

    End Sub

    '**********************************************************************************************
    '*** Modul Funktionen                                                                       ***
    '**********************************************************************************************

    ' Sperren der Eingabemoeglichkeiten nach Aenderungen je
    ' nachdem welche Aenderung vorgenommen wurde

    Public Sub Tabpage_Sperren(block As Integer)

        If block = 0 Then
            frmMain.cmb_mbc81_Nutzung.Enabled = False
        End If

        If block = 1 Then

            frmMain.nud_mbc81_LocID.Enabled = False
        End If

    End Sub

    ' Aktivieren der Eingabemoeglichkeiten

    Public Sub Tabpage_Aktivieren() Implements IMbcModul.Tabpage_Aktivieren

        frmMain.cmb_mbc81_Nutzung.Enabled = True
        frmMain.cmb_mbc81_Nutzung.BackColor = Color.White

        Try
            frmMain.cmb_mbc81_Nutzung.SelectedIndex = BUS0
        Catch ex As Exception

        End Try

        If BUS0 > 0 Then
            frmMain.nud_mbc81_LocID.Enabled = False
            frmMain.txt_mbc81_Adresse1.Enabled = False
            frmMain.txt_mbc81_Adresse2.Enabled = False
        Else
            frmMain.nud_mbc81_LocID.Enabled = True
            frmMain.txt_mbc81_Adresse1.Enabled = True
            frmMain.txt_mbc81_Adresse2.Enabled = True
        End If

        frmMain.nud_mbc81_LocID.BackColor = Color.White

    End Sub

    ' Uebernehmen der MM-Adrresse des Moduls

    Public Sub Tabpage_AddrSet()

        If (FLAG_Edit_Enable) Then
            FLAG_Prog_Index = PROG_INDEX_MMAddr
            Tabpage_Sperren(0)
            frmMain.Register_Modul_Buttons_Enable(False, True, True, False, False, False)
            MMAddr = "&h" & FormHEX("0", 4, frmMain.nud_mbc81_LocID.Value)
            frmMain.nud_mbc81_LocID.BackColor = Color.Wheat
            frmMain.txt_mbc81_Adresse1.Text = Val("&H" & Hex(frmMain.nud_mbc81_LocID.Value)) - &H3000 + 1
            frmMain.txt_mbc81_Adresse2.Text = Val("&H" & Hex(frmMain.nud_mbc81_LocID.Value)) - &H3000 + 16
        End If

    End Sub

    ' Uebernehmen der Bus-0-Nutzung des Moduls
    Public Sub Tabpage_Bus_0()
        If (FLAG_Edit_Enable) Then
            FLAG_Prog_Index = PROG_INDEX_BUS0
            Tabpage_Sperren(1)
            frmMain.Register_Modul_Buttons_Enable(False, True, True, False, False, False)
            BUS0 = frmMain.cmb_mbc81_Nutzung.SelectedIndex
            frmMain.cmb_mbc81_Nutzung.BackColor = Color.Wheat
        End If
    End Sub

    ' Zeige Modulwerte an

    Public Sub Tabpage_Werte_Show() Implements IMbcModul.Tabpage_Werte_Show

        Try
            frmMain.nud_mbc81_LocID.Value = Val(MMAddr)
        Catch e As Exception
            frmMain.nud_mbc81_LocID.Value = Val(MM_SW_New)
        End Try

        frmMain.txt_mbc81_Adresse1.Text = Val("&H" & Hex(frmMain.nud_mbc81_LocID.Value)) - &H3000 + 1
        frmMain.txt_mbc81_Adresse2.Text = Val("&H" & Hex(frmMain.nud_mbc81_LocID.Value)) - &H3000 + 16

        ' Zeige den aktuellen Schaltzustand an wenn dieser für das Modul gilt

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

            MsgIOStart = MBC_81_MM_H
            MsgIOMax = MBC_81_MM_L
            MsgIOWrite = EE_81_MMAddr

            Tabpage_Aktivieren()

            SYS_ARRAY(MBC_81_MM_H) = Microsoft.VisualBasic.Mid(MMAddr, 3, 2)
            SYS_ARRAY(MBC_81_MM_L) = Microsoft.VisualBasic.Right(MMAddr, 2)

        End If

        ' Weise die Bus-0-Nutzung zu und aktiviere den Updatemodus

        If FLAG_Prog_Index = PROG_INDEX_BUS0 Then

            MsgIOStart = MBC_81_STATUS
            MsgIOMax = MBC_81_STATUS
            MsgIOWrite = EE_81_Nutzung

            Tabpage_Aktivieren()

            SYS_ARRAY(MBC_81_STATUS) = FormHEX("0", 2, BUS0)

        End If

        ' Starte Aktualisierung des Moduls

        frmMain.COMUpdateStart(MsgIOStart, MsgIOMax, MsgIOWrite)

    End Sub

    ' Fuelle Modul mit den aus dem Modul gelesenen spezifischenWerten und Parameter
    Public Sub Parameter_Fill() Implements IMbcModul.Parameter_Fill

        ' BUS 0 Nutzung

        BUS0 = Val("&H" & SYS_ARRAY(MBC_81_STATUS))

        If BUS0 > 0 Then
            frmMain.nud_mbc81_LocID.Enabled = False
            frmMain.txt_mbc81_Adresse1.Enabled = False
            frmMain.txt_mbc81_Adresse2.Enabled = False
        End If

        Try
            frmMain.cmb_mbc81_Nutzung.SelectedIndex = BUS0
        Catch ex As Exception

        End Try


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
