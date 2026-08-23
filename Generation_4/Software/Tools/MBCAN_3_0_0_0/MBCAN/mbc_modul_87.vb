Public Class mbc_modul_87_cl

    ' Beinhaltet Stammdaten

    Inherits mbc_modul_stammdaten_cl
    Implements IMbcModul

    '**********************************************************************************************
    '*** Sonderbelegung mbc-87                                                                  ***
    '**********************************************************************************************

    Private MBC_87_START = MBC_S_GENPURP                                 ' Start des Gesamtblocks mbc-84

    ' #define MBC_87_L_MM			2									// Laenge modulspezifische Parameter
    ' #define MBC_87_S_MM			MBC_S_GENPURP						// Index Start modulspezifische Parameter
    ' #define MBC_87_MM_H			sys_array[MBC_87_S_MM]			    // MMAddr HIGH
    ' #define MBC_87_MM_L			sys_array[MBC_87_S_MM + 1]		    // MMAddr LOW

    Private MBC_87_L_MM = 2                                              ' Laenge modulspezifische Parameter
    Private MBC_87_S_MM = MBC_S_GENPURP                                  ' Index Start modulspezifische Parameter
    Private MBC_87_MM_H = MBC_87_S_MM                                    ' MMAddr HIGH
    Private MBC_87_MM_L = MBC_87_S_MM + 1                                ' MMAddr LOW

    Private MBC_87_ENDE = (MBC_87_S_MM + MBC_87_L_MM - 1)                ' Ende des Gesamtblocks mbc-87

    ' #define EE_87_MMAddr		    0x0A								// ID Block Adresse

    Private EE_87_MMAddr = "0A"                                          ' ID Block Adresse

    '**********************************************************************************************
    '*** Modul Updatesteuerung                                                                  ***
    '**********************************************************************************************

    Private PROG_INDEX_MMAddr = 0

    '**********************************************************************************************
    '*** Modul SYS_ARRAY-Indizes                                                                ***
    '**********************************************************************************************

    Public IOStart As Integer = MBC_87_START                             ' Startindex Parameterbereich
    Public IOEnde As Integer = MBC_87_ENDE                               ' Endindex Parameterbereich

    '**********************************************************************************************
    '*** Modul Bewegungsdaten                                                                   ***
    '**********************************************************************************************

    ' Startwerte fuer die Modulvariablen

    Private MM_New As String = "&H33"

    ' Modulvariablen

    Public MMAddr As String            ' Adresse im Maerklin-Adressbereich

    '**********************************************************************************************
    '*** Modul initialisieren                                                                   ***
    '**********************************************************************************************

    Sub New()

        MMAddr = MM_New

    End Sub

    '**********************************************************************************************
    '*** Modul Funktionen                                                                       ***
    '**********************************************************************************************

    ' Sperren der Eingabemoeglichkeiten nach Aenderungen je
    ' nachdem welche Aenderung vorgenommen wurde
    ' 1 = MM-Adresse wurde geaendert

    Public Sub Tabpage_Sperren(block As Integer)

        ' Adresse wurde geaendert

        If block = 1 Then

        End If

    End Sub

    ' Aktivieren der Eingabemoeglichkeiten
    Public Sub Tabpage_Aktivieren() Implements IMbcModul.Tabpage_Aktivieren

        ' Adressbox enablen

        frmMain.nud_mbc87_MMAddr.Enabled = True
        frmMain.nud_mbc87_MMAddr.BackColor = Color.White

    End Sub

    ' Uebernehmen der MM-Adrresse des Moduls

    Public Sub Tabpage_AddrSet()

        If (FLAG_Edit_Enable) Then
            FLAG_Prog_Index = PROG_INDEX_MMAddr
            Tabpage_Sperren(1)
            frmMain.Register_Modul_Buttons_Enable(False, True, True, False, False, False)
            MMAddr = "&h" & FormHEX("0", 4, frmMain.nud_mbc87_MMAddr.Value)
            frmMain.nud_mbc87_MMAddr.BackColor = Color.Wheat
        End If

    End Sub

    ' Zeige Modulwerte an

    Public Sub Tabpage_Werte_Show() Implements IMbcModul.Tabpage_Werte_Show

        ' Adresse eintragen

        Try
            frmMain.nud_mbc87_MMAddr.Value = Val(MMAddr)
        Catch e As Exception
            frmMain.nud_mbc87_MMAddr.Value = Val(MM_New)
        End Try

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

            MsgIOStart = MBC_87_MM_H
            MsgIOMax = MBC_87_MM_L
            MsgIOWrite = EE_87_MMAddr

            Tabpage_Aktivieren()

            SYS_ARRAY(MBC_87_MM_H) = Microsoft.VisualBasic.Mid(MMAddr, 3, 2)
            SYS_ARRAY(MBC_87_MM_L) = Microsoft.VisualBasic.Right(MMAddr, 2)

        End If

        ' Starte Aktualisierung des Moduls

        frmMain.COMUpdateStart(MsgIOStart, MsgIOMax, MsgIOWrite)

    End Sub

    ' Fuelle Modul mit den aus dem Modul gelesenen spezifischenWerten und Parameter
    Public Sub Parameter_Fill() Implements IMbcModul.Parameter_Fill

        ' MMAdresse 

        MMAddr = "&H" & SYS_ARRAY(MBC_87_MM_H) & SYS_ARRAY(MBC_87_MM_L)

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
