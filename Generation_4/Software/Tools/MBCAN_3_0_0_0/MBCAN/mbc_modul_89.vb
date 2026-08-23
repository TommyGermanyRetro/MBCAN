Public Class mbc_modul_89_cl

    ' Beinhaltet Stammdaten

    Inherits mbc_modul_stammdaten_cl
    Implements IMbcModul

    '**********************************************************************************************
    '*** Sonderbelegung mbc-89                                                                  ***
    '**********************************************************************************************

    Private MBC_89_START = MBC_S_GENPURP                                 ' Start des Gesamtblocks mbc-80

    ' #define MBC_89_L_SW	2                                           // Laenge der Softwareversion des Connect6021
    ' #define MBC_89_S_SW 	MBC_S_GENPURP						        // Index Start Softwareversion des Connect6021
    ' #define MBC_89_SW_1	sys_array[MBC_80_S_SW]                      // Softwareversion 1er-Stelle
    ' #define MBC_89_SW_2	sys_array[MBC_80_S_SW + 1]                  // Softwareversion 0,1er-Stelle

    Private MBC_89_L_SW = 2                                              ' Laenge der Softwareversion des Connect6021
    Private MBC_89_S_SW = MBC_S_GENPURP                                  ' Index Start Softwareversion des Connect6021
    Private MBC_89_SW_1 = MBC_89_S_SW                                    ' Softwareversion Vorkomma-Stelle
    Private MBC_89_SW_2 = MBC_89_S_SW + 1                                ' Softwareversion Nachkomma-Stelle

    ' #define MBC_89_L_LOKS			810									// Laenge lokspezifische Parameter
    ' #define MBC_89_S_LOKS			(MBC_89_S_SW + MBC_89_L_SW)         // Index Start lokspezifische Parameter
    '       //						*( mm_lok )		= 0x00; Speed
    '       //						*( mm_lok + 1 ) = 0x00; Dir
    '       //						*( mm_lok + 2 ) = 0x00; F0
    '       //						*( mm_lok + 3 ) = 0x00; F1...F4
    '       //						*( mm_lok + 4 ) = 0x00; GUiD 4
    '       //						*( mm_lok + 5 ) = 0x00; GUiD 3
    '       //						*( mm_lok + 6 ) = 0x00; GUiD 2
    '       //						*( mm_lok + 7 ) = 0x00; GUiD 1
    '       //						*( mm_lok + 8 ) = 0x00; 6021Connect
    '       //						*( mm_lok + 9 ) = 0x00; INFRAConnect

    Private MBC_89_L_LOKS = 810                                          ' Laenge lokspezifische Parameter
    Private MBC_89_S_LOKS = (MBC_89_S_SW + MBC_89_L_SW)                  ' Index Start lokspezifische Parameter
    Private MBC_89_LOKS = MBC_89_S_LOKS                                  ' Lokspeicher
    Private MBC_89_LOKS_ITEMS = 10                                       ' Anzahl Lokeigenschaften

    ' #define MBC_89_L_MAGNET		65                                  // Laenge magnetartikelspezifische Parameter
    ' #define MBC_89_S_MAGNET		(MBC_89_L_LOKS + MBC_89_S_LOKS)     // Index Start magnetartikelspezifische Parameter
    ' #define MBC_89_MAGNET			modul.sys_array[MBC_89_S_MAGNET]    // Protokollspeicher MM2/DCC fuer Keyboard

    Private MBC_89_L_MAGNET = 65                                         ' Laenge magnetartikelspezifische Parameter
    Private MBC_89_S_MAGNET = (MBC_89_L_LOKS + MBC_89_S_LOKS)            ' Index Start magnetartikelspezifische Parameter
    Private MBC_89_MAGNET = MBC_89_S_MAGNET                              ' Protokollspeicher MM2/DCC fuer Keyboard

    ' #define MBC_89_L_MAPPING		1                                   // Laenge Angaben zur Zentrale
    ' #define MBC_89_S_MAPPING		(MBC_89_L_MAGNET + MBC_89_S_MAGNET) // Index Angaben zur Zentrale
    ' #define MBC_89_MAPPING		modul.sys_array[MBC_89_S_MAPPING]	// Angaben zur Zentrale

    Private MBC_89_L_MAPPING = 1                                        ' Laenge Angaben zur Zentrale
    Private MBC_89_S_MAPPING = (MBC_89_L_MAGNET + MBC_89_S_MAGNET)      ' Index Angaben zur Zentrale
    Private MBC_89_MAPPING = MBC_89_S_MAPPING                           ' Angaben zur Zentrale

    ' #define MBC_89_L_BASIS		1                                   // Laenge Angabe zur MM-Basis
    ' #define MBC_89_S_BASIS		(MBC_89_L_MAPPING + MBC_89_S_MAPPING) // Index Angaben zur MM-Basis
    ' #define MBC_89_BASIS			modul.sys_array[MBC_89_S_BASIS]     // Angaben zur MM-Basis

    Private MBC_89_L_BASIS = 1                                           ' Laenge Angabe zur MM-Basis
    Private MBC_89_S_BASIS = (MBC_89_L_MAPPING + MBC_89_S_MAPPING)       ' Index Angaben zur MM-Basis
    Private MBC_89_BASIS = MBC_89_S_BASIS                                ' Angaben zur MM-Basis

    ' #define MBC_89_L_I2C		    1                                   // Laenge Angabe zur I2C-Frequenz
    ' #define MBC_89_S_I2C		    (MBC_89_L_BASIS + MBC_89_S_BASIS)   // Index Angaben zur I2C-Frequenz
    ' #define MBC_89_I2C			modul.sys_array[MBC_89_S_I2C]       // Angaben zur I2C-Frequenz

    Private MBC_89_L_I2C = 1                                             ' Laenge Angabe zur MM-Basis
    Private MBC_89_S_I2C = (MBC_89_L_BASIS + MBC_89_S_BASIS)             ' Index Angaben zur MM-Basis
    Private MBC_89_I2C = MBC_89_S_I2C                                    ' Angaben zur MM-Basis

    Private MBC_89_ENDE = (MBC_89_L_I2C + MBC_89_S_I2C - 1)              ' Ende des Gesamtblocks mbc-89

    ' #define EE_SW			        0x0A								// ID Block SW-Version 6021Connect
    ' #define EE_PROTOKOLL			0x0B								// ID Block Protokoll Magnetartikel
    ' #define EE_MAPPING			0x0C								// ID Block Angabe zur Zentrale
    ' #define EE_BASIS  			0x0D								// ID Block Angabe zur Basisadresse
    ' #define EE_I2C  			    0x0E								// ID Block Angabe zur I2C-Frequenz

    Private EE_89_SW = "0A"                                              ' ID Block SW-Version 6021Connect
    Private EE_89_PROTOKOLL = "0B"                                       ' ID Block Protokoll Magnetartikel
    Private EE_89_MAPPING = "0C"                                         ' ID Block Angabe zur Zentrale
    Private EE_89_BASIS = "0D"                                           ' ID Block Angabe zur Basisadresse
    Private EE_89_I2C = "0E"                                             ' ID Block Angabe zur I2C-Frequenz

    '**********************************************************************************************
    '*** Modul Updatesteuerung                                                                  ***
    '**********************************************************************************************

    Private PROG_INDEX_SW = 1
    Private PROG_INDEX_PROTOKOLL = 2
    Private PROG_INDEX_MAPPING = 3
    Private PROG_INDEX_BASIS = 4
    Private PROG_INDEX_I2C = 5

    '**********************************************************************************************
    '*** Modul SYS_ARRAY-Indizes                                                                ***
    '**********************************************************************************************

    Public IOStart As Integer = MBC_89_START                             ' Startindex Parameterbereich
    Public IOEnde As Integer = MBC_89_ENDE                               ' Endindex Parameterbereich

    '**********************************************************************************************
    '*** Modul Bewegungsdaten                                                                   ***
    '**********************************************************************************************

    ' Startwerte fuer die Modulvariablen

    Private SW_1_New As Integer = 1
    Private SW_2_New As Integer = 0
    Private Basisadressblock_New As Integer = 1
    Private Mapping_New As Integer = 0
    Private I2C_New As Integer = 40
    Private Protokoll_New As Integer = 0

    ' Modulvariablen

    Public SW_1 As Integer                          ' Softwareversion des Connect6021
    Public SW_2 As Integer                          ' Softwareversion des Connect6021
    Public Basisadressblock As Integer              ' Basisadressblock des 1. Keyboard
    Public Mapping As Integer                       ' Mapping u/o Direkt
    Public Protokoll(MBC_89_L_MAGNET) As Integer    ' Protokoll fuer Zubehoerartikel 
    Public I2C As Integer                           ' Frequenz I2C-Bus
    Public Lokspeicher As Integer                   ' Lokspeicher

    '**********************************************************************************************
    '*** Modul initialisieren                                                                   ***
    '**********************************************************************************************

    Sub New()

        Dim index As Integer

        SW_1 = SW_1_New
        SW_2 = SW_2_New
        Basisadressblock = Basisadressblock_New
        Mapping = Mapping_New
        I2C = I2C_New
        Lokspeicher = MBC_89_LOKS

        For index = 0 To 64
            Protokoll(index) = Protokoll_New
        Next

    End Sub

    '**********************************************************************************************
    '*** Modul Funktionen                                                                       ***
    '**********************************************************************************************

    ' Sperren der Eingabemoeglichkeiten nach Aenderungen je
    ' nachdem welche Aenderung vorgenommen wurde
    ' 0 = Softwareversion hat sich geaendert
    ' 1 = Zentrale hat sich geaendert
    ' 2 = Protokoll Keyboard hat sich geaendert
    ' 3 = Basisadresse hat sich geaendert

    Public Sub Tabpage_Sperren(block As Integer)

        If block = 0 Then

            frmMain.nud_mbc89_keyboard_nr.Enabled = False
            frmMain.clb_mbc89_A.Enabled = False
            frmMain.clb_mbc89_B.Enabled = False
            frmMain.clb_mbc89_C.Enabled = False
            frmMain.clb_mbc89_D.Enabled = False
            frmMain.clb_mbc89_Mapping.Enabled = False
            frmMain.nud_mbc89_basis.Enabled = False
            frmMain.nud_mbc89_i2c.Enabled = False

        End If

        If block = 1 Then

            frmMain.nud_mbc89_keyboard_nr.Enabled = False
            frmMain.clb_mbc89_A.Enabled = False
            frmMain.clb_mbc89_B.Enabled = False
            frmMain.clb_mbc89_C.Enabled = False
            frmMain.clb_mbc89_D.Enabled = False
            frmMain.nud_mbc89_SW_1.Enabled = False
            frmMain.nud_mbc89_SW_2.Enabled = False
            frmMain.nud_mbc89_basis.Enabled = False
            frmMain.nud_mbc89_i2c.Enabled = False

        End If

        If block = 2 Then

            frmMain.clb_mbc89_Mapping.Enabled = False
            frmMain.nud_mbc89_SW_1.Enabled = False
            frmMain.nud_mbc89_SW_2.Enabled = False
            frmMain.nud_mbc89_basis.Enabled = False
            frmMain.nud_mbc89_i2c.Enabled = False

        End If

        If block = 3 Then

            frmMain.nud_mbc89_keyboard_nr.Enabled = False
            frmMain.clb_mbc89_A.Enabled = False
            frmMain.clb_mbc89_B.Enabled = False
            frmMain.clb_mbc89_C.Enabled = False
            frmMain.clb_mbc89_D.Enabled = False
            frmMain.clb_mbc89_Mapping.Enabled = False
            frmMain.nud_mbc89_SW_1.Enabled = False
            frmMain.nud_mbc89_SW_2.Enabled = False
            frmMain.nud_mbc89_i2c.Enabled = False

        End If

        If block = 4 Then

            frmMain.nud_mbc89_keyboard_nr.Enabled = False
            frmMain.clb_mbc89_A.Enabled = False
            frmMain.clb_mbc89_B.Enabled = False
            frmMain.clb_mbc89_C.Enabled = False
            frmMain.clb_mbc89_D.Enabled = False
            frmMain.clb_mbc89_Mapping.Enabled = False
            frmMain.nud_mbc89_SW_1.Enabled = False
            frmMain.nud_mbc89_SW_2.Enabled = False
            frmMain.nud_mbc89_basis.Enabled = False

        End If

    End Sub

    ' Aktivieren der Eingabemoeglichkeiten 

    Public Sub Tabpage_Aktivieren() Implements IMbcModul.Tabpage_Aktivieren


        If HW > "21.00.00" Then

            frmMain.pic_Status_Decoder.Image = My.Resources.img_mbc_89_neu

            frmMain.gb_mbc89_Keyboard.Visible = True
            frmMain.nud_mbc89_keyboard_nr.Enabled = True
            frmMain.clb_mbc89_A.Enabled = True
            frmMain.clb_mbc89_A.BackColor = Color.White
            frmMain.clb_mbc89_B.Enabled = True
            frmMain.clb_mbc89_B.BackColor = Color.White
            frmMain.clb_mbc89_C.Enabled = True
            frmMain.clb_mbc89_C.BackColor = Color.White
            frmMain.clb_mbc89_D.Enabled = True
            frmMain.clb_mbc89_D.BackColor = Color.White

            frmMain.gb_mbc89_Basisadresse.Visible = True
            frmMain.nud_mbc89_basis.Enabled = True
            frmMain.nud_mbc89_basis.BackColor = Color.White

            frmMain.gb_mbc89_Version.Visible = True
            frmMain.nud_mbc89_SW_1.Enabled = True
            frmMain.nud_mbc89_SW_1.BackColor = Color.White
            frmMain.nud_mbc89_SW_2.Enabled = True
            frmMain.nud_mbc89_SW_2.BackColor = Color.White

            frmMain.gb_mbc89_Mapping.Visible = True
            frmMain.clb_mbc89_Mapping.Enabled = True
            frmMain.clb_mbc89_Mapping.BackColor = Color.White

            frmMain.gb_mbc89_I2C.Visible = True
            frmMain.nud_mbc89_i2c.Enabled = True
            frmMain.nud_mbc89_i2c.BackColor = Color.White

            frmMain.txt_mbc89_alt.Visible = False

        Else

            frmMain.pic_Status_Decoder.Image = My.Resources.img_mbc_89

            frmMain.gb_mbc89_Keyboard.Visible = False
            frmMain.gb_mbc89_Basisadresse.Visible = False
            frmMain.gb_mbc89_Version.Visible = False
            frmMain.gb_mbc89_Mapping.Visible = False
            frmMain.gb_mbc89_I2C.Visible = False

            frmMain.txt_mbc89_alt.Visible = True

        End If

    End Sub

    ' INDEX 0
    ' Connect6021-Softwareversion setzen
    Public Sub Tabpage_SW_Set()

        If (FLAG_Edit_Enable) Then

            FLAG_Prog_Index = PROG_INDEX_SW

            frmMain.Register_Modul_Buttons_Enable(False, True, True, False, False, False)

            Tabpage_Sperren(0)

            SW_1 = frmMain.nud_mbc89_SW_1.Value
            SW_2 = frmMain.nud_mbc89_SW_2.Value

            frmMain.nud_mbc89_SW_1.BackColor = Color.Wheat
            frmMain.nud_mbc89_SW_2.BackColor = Color.Wheat

        End If

    End Sub

    ' INDEX 1
    ' Uebernehmen die ausgewaehlte Zentrale
    Public Sub mbc89_tabpage_Mapping_Set(ByRef nud As Object, AktTyp As Integer, AktNr As Integer)

        Dim index As Integer

        If (FLAG_Edit_Enable) Then

            FLAG_Prog_Index = PROG_INDEX_MAPPING

            Tabpage_Sperren(1)

            frmMain.Register_Modul_Buttons_Enable(False, True, True, False, False, False)

            index = nud.SelectedIndex

            If index <> -1 Then

                If index = 0 Then
                    nud.SetItemChecked(0, True)
                    nud.SetItemChecked(1, False)
                End If
                If index = 1 Then
                    nud.SetItemChecked(0, False)
                    nud.SetItemChecked(1, True)
                End If

                nud.BackColor = Color.Wheat
                nud.ClearSelected()

                Mapping = index

            End If

        End If

    End Sub

    ' Anzeigen des Protokolls je Stellpult und Keyboard
    Private Sub Tabpage_Mapping_Show()

        Dim index As Integer

        For index = 0 To 1
            frmMain.clb_mbc89_Mapping.SetItemChecked(index, False)
        Next

        index = SYS_ARRAY(MBC_89_MAPPING)

        If index < 2 Then frmMain.clb_mbc89_Mapping.SetItemChecked(index, True)

    End Sub

    ' INDEX 2
    ' Uebernehmen das Protokoll je Stellpult und Keyboard
    Public Sub Tabpage_Protokoll_Set(ByRef nud As Object, AktTyp As Integer, AktNr As Integer, channel As Integer)

        Dim index As Integer
        Dim keyboard As Integer

        If (FLAG_Edit_Enable) Then

            FLAG_Prog_Index = PROG_INDEX_PROTOKOLL

            Tabpage_Sperren(2)

            frmMain.Register_Modul_Buttons_Enable(False, True, True, False, False, False)

            index = nud.SelectedIndex

            If index <> -1 Then

                keyboard = frmMain.nud_mbc89_keyboard_nr.Value - 1

                If index = 0 Then
                    nud.SetItemChecked(0, True)
                    nud.SetItemChecked(1, False)
                End If

                If index = 1 Then
                    nud.SetItemChecked(0, False)
                    nud.SetItemChecked(1, True)
                End If

                nud.BackColor = Color.Wheat
                nud.ClearSelected()

                Protokoll(keyboard * 4 + channel - 1) = index

            End If

        End If

    End Sub

    ' Anzeigen des Protokolls je Stellpult und Keyboard

    Private Sub Tabpage_Protokoll_Show()

        Dim index As Integer
        Dim keyboard As Integer
        Dim basis As Integer

        For index = 0 To 1
            frmMain.clb_mbc89_A.SetItemChecked(index, False)
            frmMain.clb_mbc89_B.SetItemChecked(index, False)
            frmMain.clb_mbc89_C.SetItemChecked(index, False)
            frmMain.clb_mbc89_D.SetItemChecked(index, False)
        Next

        keyboard = frmMain.nud_mbc89_keyboard_nr.Value - 1
        basis = frmMain.nud_mbc89_basis.Value - 1

        index = Protokoll(keyboard * 4)
        If index < 2 Then frmMain.clb_mbc89_A.SetItemChecked(index, True)
        frmMain.lbl_mbc89_A.Text = "A: " & (keyboard * 16 + 1 + basis) & " - " & (keyboard * 16 + 4 + basis)

        index = Protokoll(keyboard * 4 + 1)
        If index < 2 Then frmMain.clb_mbc89_B.SetItemChecked(index, True)
        frmMain.lbl_mbc89_B.Text = "B: " & (keyboard * 16 + 5 + basis) & " - " & (keyboard * 16 + 8 + basis)

        index = Protokoll(keyboard * 4 + 2)
        If index < 2 Then frmMain.clb_mbc89_C.SetItemChecked(index, True)
        frmMain.lbl_mbc89_C.Text = "C: " & (keyboard * 16 + 9 + basis) & " - " & (keyboard * 16 + 12 + basis)

        index = Protokoll(keyboard * 4 + 3)
        If index < 2 Then frmMain.clb_mbc89_D.SetItemChecked(index, True)
        frmMain.lbl_mbc89_D.Text = "D: " & (keyboard * 16 + 13 + basis) & " - " & (keyboard * 16 + 16 + basis)

    End Sub

    ' Zeige Adressen und Protokoll je Keyboard an
    Public Sub Tabpage_Keyboard_Set()

        If (FLAG_Edit_Enable) Then

            Tabpage_Protokoll_Show()

        End If

    End Sub

    ' INDEX 3
    ' Zeige Adressen und Protokoll je Keyboard an
    Public Sub Tabpage_Basis_Set()

        If (FLAG_Edit_Enable) Then

            FLAG_Prog_Index = PROG_INDEX_BASIS

            Tabpage_Sperren(3)

            frmMain.Register_Modul_Buttons_Enable(False, True, True, False, False, False)

            Basisadressblock = (frmMain.nud_mbc89_basis.Value - 1) / 256 + 1

            frmMain.nud_mbc89_basis.BackColor = Color.Wheat

            Tabpage_Werte_Show()

        End If

    End Sub

    ' INDEX 4
    ' I2C-Frequenz setzen setzen
    Public Sub Tabpage_I2C_Set()

        If (FLAG_Edit_Enable) Then

            FLAG_Prog_Index = PROG_INDEX_I2C

            frmMain.Register_Modul_Buttons_Enable(False, True, True, False, False, False)

            Tabpage_Sperren(4)

            I2C = frmMain.nud_mbc89_i2c.Value

            frmMain.nud_mbc89_i2c.BackColor = Color.Wheat

            Tabpage_Werte_Show()

        End If

    End Sub

    ' Zeige Modulwerte an
    Public Sub Tabpage_Werte_Show() Implements IMbcModul.Tabpage_Werte_Show

        ' Trage Softwareversion ein

        Try
            frmMain.nud_mbc89_SW_1.Value = SW_1
        Catch e As Exception
            frmMain.nud_mbc89_SW_1.Value = 1
        End Try

        Try
            frmMain.nud_mbc89_SW_2.Value = SW_2
        Catch e As Exception
            frmMain.nud_mbc89_SW_2.Value = 0
        End Try

        ' Trage Basisadresse ein

        Try
            frmMain.nud_mbc89_basis.Value = (Basisadressblock - 1) * 256 + 1
        Catch e As Exception
            frmMain.nud_mbc89_basis.Value = 1
        End Try

        ' Trage Protokoll ein

        Tabpage_Protokoll_Show()

        ' Trage Mapping ein

        Tabpage_Mapping_Show()

        ' Trage I2C-Frequenz ein

        Try
            frmMain.nud_mbc89_i2c.Value = (I2C)
        Catch e As Exception
            frmMain.nud_mbc89_i2c.Value = I2C_New
        End Try

        ' Blende Aktorenanzeige aus

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

        ' Weise die Softwareversion des Connect6021 dem Modul zu

        If FLAG_Prog_Index = PROG_INDEX_SW Then

            MsgIOStart = MBC_89_SW_1
            MsgIOMax = MBC_89_SW_1 + 1
            MsgIOWrite = EE_89_SW

            SYS_ARRAY(MBC_89_SW_1) = FormHEX("0", 2, SW_1)
            SYS_ARRAY(MBC_89_SW_2) = FormHEX("0", 2, SW_2)

            Tabpage_Aktivieren()

        End If

        ' Weise die ausgewaehlte Zentrale zu

        If FLAG_Prog_Index = PROG_INDEX_MAPPING Then

            MsgIOStart = MBC_89_MAPPING
            MsgIOMax = MBC_89_MAPPING
            MsgIOWrite = EE_89_MAPPING

            SYS_ARRAY(MBC_89_MAPPING) = FormHEX("0", 2, Mapping)

            Tabpage_Aktivieren()

            MsgBox("Das Modul wird nach dem Schreibvorgang neu gestartet." & Chr(13) & Chr(10) & "Dies dauert ca. 1 min., danach ist ein Neueinlesen im Parametriercenter notwendig!", vbOKOnly)

            frmMain.tm_Update.Start()

        End If

        ' Weise das ausgewaehlte Protokoll je Stellpult und Keyboard zu

        If FLAG_Prog_Index = PROG_INDEX_PROTOKOLL Then

            MsgIOStart = MBC_89_MAGNET
            MsgIOMax = MBC_89_MAGNET + MBC_89_L_MAGNET - 1
            MsgIOWrite = EE_89_PROTOKOLL

            For Index = 0 To 64
                SYS_ARRAY(MBC_89_MAGNET + Index) = FormHEX("0", 2, Protokoll(Index))
            Next

            Tabpage_Aktivieren()

        End If

        ' Weise ausgewaehlte Basis dem Modul zu

        If FLAG_Prog_Index = PROG_INDEX_BASIS Then

            MsgIOStart = MBC_89_BASIS
            MsgIOMax = MBC_89_BASIS
            MsgIOWrite = EE_89_BASIS

            SYS_ARRAY(MBC_89_BASIS) = FormHEX("0", 2, Basisadressblock)

            Tabpage_Aktivieren()

        End If

        ' Weise ausgewaehlte I2C-Frequenz dem Modul zu

        If FLAG_Prog_Index = PROG_INDEX_I2C Then

            MsgIOStart = MBC_89_I2C
            MsgIOMax = MBC_89_I2C
            MsgIOWrite = EE_89_I2C

            SYS_ARRAY(MBC_89_I2C) = FormHEX("0", 2, I2C)

            Tabpage_Aktivieren()

        End If

        ' Starte Aktualisierung des Moduls

        frmMain.COMUpdateStart(MsgIOStart, MsgIOMax, MsgIOWrite)

    End Sub

    ' Fuelle Modul mit den aus dem Modul gelesenen spezifischen Werten und Parameter
    Public Sub Parameter_Fill() Implements IMbcModul.Parameter_Fill

        Dim Index As Integer

        ReDim Protokoll(65)

        ' Connect6021-Softwareversion

        SW_1 = Convert.ToInt32(SYS_ARRAY(MBC_89_SW_1), 16)
        SW_2 = Convert.ToInt32(SYS_ARRAY(MBC_89_SW_2), 16)

        ' Basisadressblock 1...4 des 1. Keyboards

        Basisadressblock = Convert.ToInt32(SYS_ARRAY(MBC_89_BASIS), 16)

        ' Mapping

        Mapping = Convert.ToInt32(SYS_ARRAY(MBC_89_MAPPING), 16)

        ' I2C-Bus-Frequenz

        I2C = Convert.ToInt32(SYS_ARRAY(MBC_89_I2C), 16)

        ' Protokoll

        For Index = 0 To 64
            Protokoll(Index) = Convert.ToInt32(SYS_ARRAY(MBC_89_MAGNET + Index), 16)
        Next

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
