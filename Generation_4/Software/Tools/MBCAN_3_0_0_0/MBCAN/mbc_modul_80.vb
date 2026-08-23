Public Class mbc_modul_80_cl

    ' Beinhaltet Stammdaten

    Inherits mbc_modul_stammdaten_cl
    Implements IMbcModul

    '**********************************************************************************************
    '*** Sonderbelegung mbc-80                                                                  ***
    '**********************************************************************************************

    Private MBC_80_START = MBC_S_GENPURP                                 ' Start des Gesamtblocks mbc-80

    ' #define MBC_L_BT_PW		4									    // Laenge modulspezifische Parameter
    ' #define MBC_S_BT_PW		MBC_S_GENPURP						    // Index Start modulspezifische Parameter
    ' #define MBC_BT_PW			sys_array[MBC_S_BT_PW]				    // Password des Moduls

    Private MBC_80_L_BT_PW = 4                                           ' Laenge modulspezifische Parameter
    Private MBC_80_S_BT_PW = MBC_S_GENPURP                               ' Index Start modulspezifische Parameter
    Private MBC_80_BT_PW = MBC_80_S_BT_PW                                ' Password des Moduls

    ' #define MBC_L_BT_NAME		5									    // Laenge des Modulnamens
    ' #define MBC_S_BT_NAME		(MBC_L_BT_PW + MBC_S_BT_PW)			    // Index Start Modulname

    Private MBC_80_L_BT_NAME = 5                                         ' Laenge des BT-Modulnamens
    Private MBC_80_S_BT_NAME = (MBC_80_L_BT_PW + MBC_80_S_BT_PW)         ' Index Start Modulname
    Private MBC_80_BT_Name = MBC_80_S_BT_NAME                            ' Name des Moduls

    ' #define MBC_80_L_L88_SW	2                                       // Laenge der Softwareversion des L88
    ' #define MBC_80_S_L88_SW 	(MBC_80_L_BT_NAME + MBC_80_S_BT_NAME)   // Index Start Softwareversion des L88
    ' #define MBC_80_L88_SW_1	sys_array[MBC_80_S_L88_SW]              // Softwareversion 1er-Stelle
    ' #define MBC_80_L88_SW_2	sys_array[MBC_80_S_L88_SW + 1]          // Softwareversion 0,1er-Stelle

    Private MBC_80_L_L88_SW = 2                                          ' Laenge der Softwareversion des L88
    Private MBC_80_S_L88_SW = (MBC_80_L_BT_NAME + MBC_80_S_BT_NAME)      ' Index Start Softwareversion des L88
    Private MBC_80_L88_SW_1 = MBC_80_S_L88_SW                            ' Softwareversion Vorkomma-Stelle
    Private MBC_80_L88_SW_2 = MBC_80_S_L88_SW + 1                        ' Softwareversion Nachkomma-Stelle

    Private MBC_80_ENDE = (MBC_80_L_L88_SW + MBC_80_S_L88_SW - 1)        ' Ende des Gesamtblocks mbc-80

    ' #define EE_BT_Passwort		0x0A								// ID Block BT-Passwort
    ' #define EE_BT_Name			0x0B								// ID Block BT-Name
    ' #define EE_L88_SW			    0x0C			                    // ID Block SW-Version L88

    Private EE_BT_Passwort = "0A"                                        ' ID Block BT-Passwort
    Private EE_BT_Name = "0B"                                            ' ID Block BT-Name
    Private EE_L88_SW = "0C"                                             ' ID Block L88-SW-Version

    '**********************************************************************************************
    '*** Modul Updatesteuerung                                                                  ***
    '**********************************************************************************************

    Private PROG_INDEX_BTPWD = 1
    Private PROG_INDEX_BTName = 2
    Private PROG_INDEX_L88SW = 3

    '**********************************************************************************************
    '*** Modul SYS_ARRAY-Indizes                                                                ***
    '**********************************************************************************************

    Public IOStart As Integer = MBC_80_START                             ' Startindex Parameterbereich
    Public IOEnde As Integer = MBC_80_ENDE                               ' Endindex Parameterbereich

    '**********************************************************************************************
    '*** Modul Bewegungsdaten                                                                   ***
    '**********************************************************************************************

    ' Modulvariablen

    Public BTName_0 As String                   ' 1. Stelle BT-Name des Moduls
    Public BTName_1 As String                   ' 2. Stelle BT-Name des Moduls
    Public BTName_2 As String                   ' 3. Stelle BT-Name des Moduls
    Public BTName_3 As String                   ' 4. Stelle BT-Name des Moduls
    Public BTName_4 As String                   ' 5. Stelle BT-Name des Moduls
    Public BTPWD_0 As String                    ' 1. Stelle BT-Passwort
    Public BTPWD_1 As String                    ' 2. Stelle BT-Passwort
    Public BTPWD_2 As String                    ' 3. Stelle BT-Passwort
    Public BTPWD_3 As String                    ' 4. Stelle BT-Passwort
    Public L88SW_1 As Integer                   ' Softwareversion des L88
    Public L88SW_2 As Integer                   ' Softwareversion des L88
    Public S88AK As String                      ' Automatikkenner, vergeben von der CS2
    Public BTNameSpeicher As Integer            ' Speicherzeiger BT Name
    Public BTPWSpeicher As Integer              ' Speicherzeiger BT PW

    Private L88SW_1_New As Integer = 1
    Private L88SW_2_New As Integer = 0

    '**********************************************************************************************
    '*** Modul initialisieren                                                                   ***
    '**********************************************************************************************

    Sub New()

        L88SW_1 = L88SW_1_New
        L88SW_2 = L88SW_2_New
        BTName_0 = "M"
        BTName_1 = "B"
        BTName_2 = "C"
        BTName_3 = "A"
        BTName_4 = "M"
        BTPWD_0 = "1"
        BTPWD_1 = "2"
        BTPWD_2 = "3"
        BTPWD_3 = "4"
        S88AK = "FFFF"
        BTNameSpeicher = MBC_80_BT_Name
        BTPWSpeicher = MBC_80_BT_PW

    End Sub

    '**********************************************************************************************
    '*** Modul Funktionen                                                                       ***
    '**********************************************************************************************

    ' Sperren der Eingabemoeglichkeiten nach Aenderungen je
    ' nachdem welche Aenderung vorgenommen wurde
    ' 0 = BT-Name hat sich geaendert
    ' 1 = BT-Passwort hat sich geaendert
    ' 2 = Softwareversion des L88 hat sich geaendert

    Sub Tabpage_Sperren(block As Integer)

        If block = 0 Then
            frmMain.txt_mbc80_Passwort.Enabled = False
            frmMain.nud_mbc80_SW1.Enabled = False
            frmMain.nud_mbc80_SW2.Enabled = False
        End If

        If block = 1 Then
            frmMain.txt_mbc80_Name.Enabled = False
            frmMain.nud_mbc80_SW1.Enabled = False
            frmMain.nud_mbc80_SW2.Enabled = False
        End If

        If block = 2 Then
            frmMain.txt_mbc80_Name.Enabled = False
            frmMain.txt_mbc80_Passwort.Enabled = False
        End If

    End Sub

    ' Aktivieren der Eingabemoeglichkeiten

    Public Sub Tabpage_Aktivieren() Implements IMbcModul.Tabpage_Aktivieren

        If HW > "21.00.00" Or SW >= "3.00" Then
            frmMain.gb_mbc80_BT.Enabled = False
            frmMain.txt_mbc80_Name.Enabled = False
            frmMain.txt_mbc80_Name.BackColor = Color.White
            frmMain.txt_mbc80_Passwort.Enabled = False
            frmMain.txt_mbc80_Passwort.BackColor = Color.White
            If HW > "21.00.00" Then
                frmMain.pic_Status_Decoder.Image = My.Resources.img_mbc_80_neu
            ElseIf SW >= "3.00" Then
                frmMain.pic_Status_Decoder.Image = My.Resources.img_mbc_80_wlan
            Else
                frmMain.pic_Status_Decoder.Image = My.Resources.img_mbc_80
            End If

        Else
            frmMain.gb_mbc80_BT.Enabled = True
            frmMain.txt_mbc80_Name.Enabled = True
            frmMain.txt_mbc80_Name.BackColor = Color.White

            frmMain.txt_mbc80_Passwort.Enabled = True
            frmMain.txt_mbc80_Passwort.BackColor = Color.White
        End If

        ' Die CSx-Anwesenheitssperre der L88-Softwareversion (und aller anderen Eingabefelder
        ' dieser Registerkarte) laeuft jetzt generell ueber tab_Module.Enabled, siehe
        ' Register_Modul_CSxLock_Apply() in frmMain.vb - hier nur noch der eigene Standardzustand

        frmMain.nud_mbc80_SW1.Enabled = True
        frmMain.nud_mbc80_SW2.Enabled = True

        frmMain.nud_mbc80_SW1.BackColor = Color.White
        frmMain.nud_mbc80_SW2.BackColor = Color.White

    End Sub

    ' BT-Name setzen

    Sub Tabpage_NameSet()

        If (FLAG_Edit_Enable) Then
            FLAG_Prog_Index = PROG_INDEX_BTName
            Tabpage_Sperren(0)
            If Len(frmMain.txt_mbc80_Name.Text) = 5 Then
                frmMain.Register_Modul_Buttons_Enable(False, True, True, False, False, False)
                BTName_0 = Microsoft.VisualBasic.Mid(frmMain.txt_mbc80_Name.Text, 1, 1)
                BTName_1 = Microsoft.VisualBasic.Mid(frmMain.txt_mbc80_Name.Text, 2, 1)
                BTName_2 = Microsoft.VisualBasic.Mid(frmMain.txt_mbc80_Name.Text, 3, 1)
                BTName_3 = Microsoft.VisualBasic.Mid(frmMain.txt_mbc80_Name.Text, 4, 1)
                BTName_4 = Microsoft.VisualBasic.Mid(frmMain.txt_mbc80_Name.Text, 5, 1)
            Else
                frmMain.Register_Modul_Buttons_Enable(False, True, False, False, False, False)
            End If
            frmMain.txt_mbc80_Name.BackColor = Color.Wheat

        End If
    End Sub

    ' BT-Passwort setzen

    Sub Tabpage_PWDSet()

        If (FLAG_Edit_Enable) Then
            FLAG_Prog_Index = PROG_INDEX_BTPWD
            Tabpage_Sperren(1)
            If Len(frmMain.txt_mbc80_Passwort.Text) = 4 Then
                frmMain.Register_Modul_Buttons_Enable(False, True, True, False, False, False)
                BTPWD_0 = Microsoft.VisualBasic.Mid(frmMain.txt_mbc80_Passwort.Text, 1, 1)
                BTPWD_1 = Microsoft.VisualBasic.Mid(frmMain.txt_mbc80_Passwort.Text, 2, 1)
                BTPWD_2 = Microsoft.VisualBasic.Mid(frmMain.txt_mbc80_Passwort.Text, 3, 1)
                BTPWD_3 = Microsoft.VisualBasic.Mid(frmMain.txt_mbc80_Passwort.Text, 4, 1)
            Else
                frmMain.Register_Modul_Buttons_Enable(False, True, False, False, False, False)
            End If
            frmMain.txt_mbc80_Passwort.BackColor = Color.Wheat

        End If
    End Sub

    ' GFP-Softwareversion setzen
    Sub Tabpage_GFPSWSet()

        If (FLAG_Edit_Enable) Then

            FLAG_Prog_Index = PROG_INDEX_L88SW
            frmMain.Register_Modul_Buttons_Enable(False, True, True, False, False, False)
            Tabpage_Sperren(2)
            L88SW_1 = frmMain.nud_mbc80_SW1.Value
            L88SW_2 = frmMain.nud_mbc80_SW2.Value
            frmMain.nud_mbc80_SW1.BackColor = Color.Wheat
            frmMain.nud_mbc80_SW2.BackColor = Color.Wheat

        End If

    End Sub

    ' Zeige Modulwerte an
    Public Sub Tabpage_Werte_Show() Implements IMbcModul.Tabpage_Werte_Show

        Try
            frmMain.txt_mbc80_Passwort.Text = BTPWD_0 &
                                            BTPWD_1 &
                                            BTPWD_2 &
                                            BTPWD_3
            frmMain.txt_mbc80_Name.Text = BTName_0 &
                                             BTName_1 &
                                             BTName_2 &
                                             BTName_3 &
                                             BTName_4
        Catch e As Exception
            MsgBox("Beim Datenimport ist leider ein Fehler aufgetreten. Bitte versuchen Sie es noch einmal!", vbOK)
        End Try

        Try
            frmMain.txt_mbc80_AK.Text = Val(S88AK)
        Catch e As Exception
            frmMain.txt_mbc80_AK.Text = 1
        End Try

        Try
            frmMain.nud_mbc80_SW1.Value = L88SW_1
        Catch e As Exception
            frmMain.nud_mbc80_SW1.Value = L88SW_1_New
        End Try

        Try
            frmMain.nud_mbc80_SW2.Value = L88SW_2
        Catch e As Exception
            frmMain.nud_mbc80_SW2.Value = L88SW_2_New
        End Try


        frmMain.Register_Modul_Aktorstellung_Show()

    End Sub

    ' Fuehre Update des Moduls aus

    Public Sub Update() Implements IMbcModul.Update
        Dim MsgIOStart As Integer
        Dim MsgIOMax As Integer
        Dim MsgIOWrite As String
        Dim Button As Integer

        MsgIOStart = 0
        MsgIOMax = 0
        MsgIOWrite = ""

        ' Weise den neuen Name des Bluetooth-Moduls zu und aktiviere den Updatemodus

        If FLAG_Prog_Index = PROG_INDEX_BTName Then

            MsgIOStart = MBC_80_BT_Name
            MsgIOMax = MBC_80_BT_Name + 4
            MsgIOWrite = EE_BT_Name

            Tabpage_Aktivieren()

            Button = MsgBox("Die Änderungen an der Bluetooth-Einstellung haben zur Folge, dass das Modul nach einem Neustart neu mit dem PC gekoppelt werden muss. Sind Sie damit einverstanden?", vbYesNo, "MBCAN - Bluetooth")

            If (Button = vbYes) Then
                SYS_ARRAY(MBC_80_BT_Name) = FormHEX("0", 2, Asc(BTName_0))
                SYS_ARRAY(MBC_80_BT_Name + 1) = FormHEX("0", 2, Asc(BTName_1))
                SYS_ARRAY(MBC_80_BT_Name + 2) = FormHEX("0", 2, Asc(BTName_2))
                SYS_ARRAY(MBC_80_BT_Name + 3) = FormHEX("0", 2, Asc(BTName_3))
                SYS_ARRAY(MBC_80_BT_Name + 4) = FormHEX("0", 2, Asc(BTName_4))
            Else
                frmMain.COMRefreshStart(MBC_ALLG_START, IOEnde)
                Exit Sub
            End If

        End If

        ' Weise das Bluetooth-Passwort zu und aktiviere den Updatemodus

        If FLAG_Prog_Index = PROG_INDEX_BTPWD Then

            MsgIOStart = MBC_80_BT_PW
            MsgIOMax = MBC_80_BT_PW + 3
            MsgIOWrite = EE_BT_Passwort

            Tabpage_Aktivieren()

            Button = MsgBox("Die Änderungen an der Bluetooth-Einstellung haben zur Folge, dass das Modul nach einem Neustart neu mit dem PC gekoppelt werden muss. Sind Sie damit einverstanden?", vbYesNo, "MBCAN - Bluetooth")

            If (Button = vbYes) Then
                SYS_ARRAY(MBC_80_BT_PW) = FormHEX("0", 2, Asc(BTPWD_0))
                SYS_ARRAY(MBC_80_BT_PW + 1) = FormHEX("0", 2, Asc(BTPWD_1))
                SYS_ARRAY(MBC_80_BT_PW + 2) = FormHEX("0", 2, Asc(BTPWD_2))
                SYS_ARRAY(MBC_80_BT_PW + 3) = FormHEX("0", 2, Asc(BTPWD_3))
            Else
                frmMain.COMRefreshStart(MBC_ALLG_START, IOEnde)
                Exit Sub
            End If

        End If

        ' Weise die Softwareversion des L88 dem Modul zu

        If FLAG_Prog_Index = PROG_INDEX_L88SW Then

            MsgIOStart = MBC_80_L88_SW_1
            MsgIOMax = MBC_80_L88_SW_1 + 1
            MsgIOWrite = EE_L88_SW

            SYS_ARRAY(MBC_80_L88_SW_1) = FormHEX("0", 2, L88SW_1)
            SYS_ARRAY(MBC_80_L88_SW_2) = FormHEX("0", 2, L88SW_2)

            Tabpage_Aktivieren()

        End If

        ' Starte Aktualisierung des Moduls

        frmMain.COMUpdateStart(MsgIOStart, MsgIOMax, MsgIOWrite)

    End Sub

    ' Fuelle Modul mit den aus dem Modul gelesenen spezifischenWerten und Parameter
    Public Sub Parameter_Fill() Implements IMbcModul.Parameter_Fill

        ' BT-Name

        BTName_0 = Chr("&H" & SYS_ARRAY(MBC_80_BT_Name))
        BTName_1 = Chr("&H" & SYS_ARRAY(MBC_80_BT_Name + 1))
        BTName_2 = Chr("&H" & SYS_ARRAY(MBC_80_BT_Name + 2))
        BTName_3 = Chr("&H" & SYS_ARRAY(MBC_80_BT_Name + 3))
        BTName_4 = Chr("&H" & SYS_ARRAY(MBC_80_BT_Name + 4))

        ' BT-Passwort

        BTPWD_0 = Chr("&H" & SYS_ARRAY(MBC_80_BT_PW))
        BTPWD_1 = Chr("&H" & SYS_ARRAY(MBC_80_BT_PW + 1))
        BTPWD_2 = Chr("&H" & SYS_ARRAY(MBC_80_BT_PW + 2))
        BTPWD_3 = Chr("&H" & SYS_ARRAY(MBC_80_BT_PW + 3))

        ' s88-Autokennung

        S88AK = "&H" & SYS_ARRAY(MBC_AK_H) & SYS_ARRAY(MBC_AK_L)

        ' L88-Softwareversion

        L88SW_1 = Convert.ToInt32(SYS_ARRAY(MBC_80_L88_SW_1), 16)
        L88SW_2 = Convert.ToInt32(SYS_ARRAY(MBC_80_L88_SW_2), 16)

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
