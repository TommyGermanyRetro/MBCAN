Public Class mbc_modul_84_cl

    ' Beinhaltet Stammdaten

    Inherits mbc_modul_stammdaten_cl
    Implements IMbcModul

    '**********************************************************************************************
    '*** Sonderbelegung mbc-84                                                                  ***
    '**********************************************************************************************

    Private MBC_84_START = MBC_S_GENPURP                                 ' Start des Gesamtblocks mbc-84

    ' #define MBC_84_L_MM			4									// Laenge modulspezifische Parameter
    ' #define MBC_84_S_MM			MBC_S_GENPURP						// Index Start modulspezifische Parameter
    ' #define MBC_84_MM_H			sys_array[MBC_84_S_MM]			    // MMAddr HIGH
    ' #define MBC_84_MM_L			sys_array[MBC_84_S_MM + 1]		    // MMAddr LOW
    ' #define MBC_84_MM			    sys_array[MBC_84_S_MM + 2]		    // Stellung der Aktoren
    ' #define MBC_84_RUECK		    sys_array[MBC_84_S_MM + 3]	        // Zwangsrueckmeldung

    Private MBC_84_L_MM = 4                                              ' Laenge modulspezifische Parameter
    Private MBC_84_S_MM = MBC_S_GENPURP                                  ' Index Start modulspezifische Parameter
    Private MBC_84_MM_H = MBC_84_S_MM                                    ' MMAddr HIGH
    Private MBC_84_MM_L = MBC_84_S_MM + 1                                ' MMAddr LOW
    Private MBC_84_MM = MBC_84_S_MM + 2                                  ' Stellung der Aktoren
    Private MBC_84_RUECK = MBC_84_S_MM + 3                               ' Zwangsrueckmeldung 

    ' #define MBC_84_L_SVMAX		13									// Laenge Maximalwerte Servos, Schrittweite und Center
    ' #define MBC_84_S_SVMAX		(MBC_84_L_MM + MBC_84_S_MM)			// Index Start Servomaximalwerte
    ' #define MBC_84_SVMAX		    sys_array[MBC_84_S_SVMAX]			// Servoanschlaege

    Private MBC_84_L_SVMAX = 13                                          ' Laenge Maximalwerte Servos, Schrittweite und Center
    Private MBC_84_S_SVMAX = (MBC_84_L_MM + MBC_84_S_MM)                 ' Index Start Servomaximalwerte
    Private MBC_84_SVMAX = MBC_84_S_SVMAX                                ' Servoanschlaege

    ' #define MBC_84_L_SVWEG		960									// Laenge Servoweg 4x120x2
    ' #define MBC_84_S_SVWEG		(MBC_84_L_SVMAX + MBC_84_S_SVMAX)	// Index Start Servoweg
    ' #define MBC_84_SVWEG		    sys_array[MBC_84_S_SVWEG]			// Servowege

    Private MBC_84_L_SVWEG = 480                                         ' Laenge Servoweg 4x60x2
    Private MBC_84_S_SVWEG = (MBC_84_L_SVMAX + MBC_84_S_SVMAX)           ' Index Start Servoweg
    Private MBC_84_SVWEG = MBC_84_S_SVWEG                                ' Servowege

    Private MBC_84_ENDE = (MBC_84_L_SVWEG + MBC_84_S_SVWEG - 1)          ' Ende des Gesamtblocks mbc-84

    ' #define EE_84_MMAddr		    0x0A								// ID Block Adresse
    ' #define EE_84_AKTOR		    0x0B								// ID Block Aktorenstellung
    ' #define EE_84_SERVOSMAX	    0x0C								// ID Block Servomaximalwerte
    ' #define EE_84_SERVOSWEG	    0x0D								// ID Block Servowege

    Private EE_84_MMAddr = "0A"                                          ' ID Block Adresse
    Private EE_84_AKTOR = "0B"                                           ' ID Block Aktorenstellung
    Private EE_84_SERVOSMAX = "0C"                                       ' ID Block Servomaximalwerte
    Private EE_84_SERVOSWEG = "0D"                                       ' ID Block Servowege
    Private EE_84_RUECK = "0E"                                           ' ID Block Zwangsrueckmeldung

    '**********************************************************************************************
    '*** Modul Updatesteuerung                                                                  ***
    '**********************************************************************************************

    Private PROG_INDEX_MMAddr = 0
    Private PROG_INDEX_MMRueck = 1
    Private PROG_INDEX_SERVOMAX = 2
    Private PROG_INDEX_SERVOWEG = 3

    '**********************************************************************************************
    '*** Modul SYS_ARRAY-Indizes                                                                ***
    '**********************************************************************************************

    Public IOStart As Integer = MBC_84_START                             ' Startindex Parameterbereich
    Public IOEnde As Integer = MBC_84_ENDE                               ' Endindex Parameterbereich

    '**********************************************************************************************
    '*** Modul Bewegungsdaten                                                                   ***
    '**********************************************************************************************

    ' Anzahl der Servoweg-Schritte je Richtung - einzige Quelle dieses Werts (entspricht
    ' SERVO_MAX_STEPS in Mbc84Com.cpp, dort manuell synchron zu halten). frmServo.vb liest diese
    ' Konstante aus, statt eine eigene Kopie zu fuehren (siehe frmServo.vb::ServoSteps).

    Public Const SERVO_MAX_STEPS As Integer = 60

    ' Startwerte fuer die Modulvariablen

    Private MM_New As String = "&H33FC"
    Private MMRueck_New As Integer = 0
    Private MMServo_L_New As Integer = 0
    Private MMServo_R_New As Integer = 0
    Private MMServo_VZ_New As Integer = 0
    Private MMServo_Steps_New As Integer = SERVO_MAX_STEPS
    Private MMServo_S_Weg_New As Integer = 0

    ' Modulvariablen

    Public MMValue As String           ' Stellungsanzeige der Magnetartikel
    Public MMAddr As String            ' Adresse im Maerklin-Adressbereich
    Public MMRueck As Integer          ' Zwangsrueckmeldung
    Public MMServo_L1 As Integer       ' Endlage Servo 1 Links
    Public MMServo_L2 As Integer       ' Endlage Servo 2 Links
    Public MMServo_L3 As Integer       ' Endlage Servo 3 Links
    Public MMServo_L4 As Integer       ' Endlage Servo 4 Links
    Public MMServo_R1 As Integer       ' Endlage Servo 1 Rechts
    Public MMServo_R2 As Integer       ' Endlage Servo 2 Rechts
    Public MMServo_R3 As Integer       ' Endlage Servo 3 Rechts
    Public MMServo_R4 As Integer       ' Endlage Servo 4 Rechts
    Public MMServo_Center As Integer   ' Mittenlage aller Servoss
    Public MMServo_1VZ As Integer      ' Verzoegerung Schrittweite Servo 1
    Public MMServo_2VZ As Integer      ' Verzoegerung Schrittweite Servo 2
    Public MMServo_3VZ As Integer      ' Verzoegerung Schrittweite Servo 3
    Public MMServo_4VZ As Integer      ' Verzoegerung Schrittweite Servo 4
    Public MMServo_S_Weg As Integer
    Public MMServo_Steps As Integer
    Public MMServoWegSpeicher As Integer
    Public MMServoMaxSpeicher As Integer
    Public MMServoUpdateSpeicher As String

    '**********************************************************************************************
    '*** Modul initialisieren                                                                   ***
    '**********************************************************************************************

    Sub New()

        MMAddr = MM_New
        MMRueck = MMRueck_New
        MMServo_L1 = MMServo_L_New
        MMServo_L2 = MMServo_L_New
        MMServo_L3 = MMServo_L_New
        MMServo_L4 = MMServo_L_New
        MMServo_R1 = MMServo_R_New
        MMServo_R2 = MMServo_R_New
        MMServo_R3 = MMServo_R_New
        MMServo_R4 = MMServo_R_New
        MMServo_1VZ = MMServo_VZ_New
        MMServo_2VZ = MMServo_VZ_New
        MMServo_3VZ = MMServo_VZ_New
        MMServo_4VZ = MMServo_VZ_New
        MMServo_Steps = MMServo_Steps_New
        MMServo_S_Weg = MMServo_S_Weg_New
        MMServoWegSpeicher = MBC_84_S_SVWEG
        MMServoMaxSpeicher = MBC_84_SVMAX
        MMServoUpdateSpeicher = EE_84_SERVOSWEG

        ' RT-/GN-max-Eingabefelder auf den tatsaechlich gueltigen Wertebereich setzen (0...RT_GN_MAX
        ' der Firmware, dort aus SERVO_MAX_STEPS/2 abgeleitet, siehe Mbc84Com.cpp) - der
        ' Designer-Literalwert (Maximum-Property) kann nicht direkt an MMServo_Steps gebunden werden
        ' (wird beim Oeffnen im Form-Designer sonst wieder ueberschrieben), daher hier zur Laufzeit
        ' einmalig aus derselben Konstante nachgezogen, die diese Klasse fuer den Servoweg selbst
        ' schon fuehrt (MMServo_Steps_New, gerade oben in MMServo_Steps uebernommen)

        frmMain.nud_mbc84_Links_CH1.Maximum = CDec(MMServo_Steps \ 2)
        frmMain.nud_mbc84_Rechts_CH1.Maximum = CDec(MMServo_Steps \ 2)
        frmMain.nud_mbc84_Links_CH2.Maximum = CDec(MMServo_Steps \ 2)
        frmMain.nud_mbc84_Rechts_CH2.Maximum = CDec(MMServo_Steps \ 2)
        frmMain.nud_mbc84_Links_CH3.Maximum = CDec(MMServo_Steps \ 2)
        frmMain.nud_mbc84_Rechts_CH3.Maximum = CDec(MMServo_Steps \ 2)
        frmMain.nud_mbc84_Links_CH4.Maximum = CDec(MMServo_Steps \ 2)
        frmMain.nud_mbc84_Rechts_CH4.Maximum = CDec(MMServo_Steps \ 2)

    End Sub

    '**********************************************************************************************
    '*** Modul Funktionen                                                                       ***
    '**********************************************************************************************

    ' Sperren der Eingabemoeglichkeiten nach Aenderungen je
    ' nachdem welche Aenderung vorgenommen wurde
    ' 0 = Servoparameter wurden geaendert
    ' 1 = MM-Adresse wurde geaendert

    Public Sub Tabpage_Sperren(block As Integer)

        frmMain.btn_mbc84_Weg_CH1.Enabled = False
        frmMain.btn_mbc84_Weg_CH2.Enabled = False
        frmMain.btn_mbc84_Weg_CH3.Enabled = False
        frmMain.btn_mbc84_Weg_CH4.Enabled = False

        If block = 0 Then
            frmMain.nud_mbc84_MMAddr.Enabled = False
            frmMain.nud_mbc84_Rueck.Enabled = False
        End If

        If block = 1 Then
            frmMain.nud_mbc84_Rueck.Enabled = False
            frmMain.nud_mbc84_Links_CH1.Enabled = False
            frmMain.nud_mbc84_Rechts_CH1.Enabled = False
            frmMain.nud_mbc84_Links_CH2.Enabled = False
            frmMain.nud_mbc84_Rechts_CH2.Enabled = False
            frmMain.nud_mbc84_Links_CH3.Enabled = False
            frmMain.nud_mbc84_Rechts_CH3.Enabled = False
            frmMain.nud_mbc84_Links_CH4.Enabled = False
            frmMain.nud_mbc84_Rechts_CH4.Enabled = False
            frmMain.nud_mbc84_Step_CH1.Enabled = False
            frmMain.nud_mbc84_Step_CH2.Enabled = False
            frmMain.nud_mbc84_Step_CH3.Enabled = False
            frmMain.nud_mbc84_Step_CH4.Enabled = False
        End If

        If block = 2 Then
            frmMain.nud_mbc84_MMAddr.Enabled = False
            frmMain.nud_mbc84_Rueck.Enabled = False
            frmMain.nud_mbc84_Links_CH1.Enabled = False
            frmMain.nud_mbc84_Rechts_CH1.Enabled = False
            frmMain.nud_mbc84_Links_CH2.Enabled = False
            frmMain.nud_mbc84_Rechts_CH2.Enabled = False
            frmMain.nud_mbc84_Links_CH3.Enabled = False
            frmMain.nud_mbc84_Rechts_CH3.Enabled = False
            frmMain.nud_mbc84_Links_CH4.Enabled = False
            frmMain.nud_mbc84_Rechts_CH4.Enabled = False
            frmMain.nud_mbc84_Step_CH1.Enabled = False
            frmMain.nud_mbc84_Step_CH2.Enabled = False
            frmMain.nud_mbc84_Step_CH3.Enabled = False
            frmMain.nud_mbc84_Step_CH4.Enabled = False
        End If

        If block = 3 Then
            frmMain.nud_mbc84_MMAddr.Enabled = False
            frmMain.nud_mbc84_Links_CH1.Enabled = False
            frmMain.nud_mbc84_Rechts_CH1.Enabled = False
            frmMain.nud_mbc84_Links_CH2.Enabled = False
            frmMain.nud_mbc84_Rechts_CH2.Enabled = False
            frmMain.nud_mbc84_Links_CH3.Enabled = False
            frmMain.nud_mbc84_Rechts_CH3.Enabled = False
            frmMain.nud_mbc84_Links_CH4.Enabled = False
            frmMain.nud_mbc84_Rechts_CH4.Enabled = False
            frmMain.nud_mbc84_Step_CH1.Enabled = False
            frmMain.nud_mbc84_Step_CH2.Enabled = False
            frmMain.nud_mbc84_Step_CH3.Enabled = False
            frmMain.nud_mbc84_Step_CH4.Enabled = False
        End If

    End Sub

    ' Aktivieren der Eingabemoeglichkeiten

    Public Sub Tabpage_Aktivieren() Implements IMbcModul.Tabpage_Aktivieren

        frmMain.nud_mbc84_MMAddr.Enabled = True
        frmMain.nud_mbc84_MMAddr.BackColor = Color.White

        frmMain.nud_mbc84_Rueck.Enabled = True
        frmMain.nud_mbc84_Rueck.BackColor = Color.White

        frmMain.nud_mbc84_Links_CH1.Enabled = True
        frmMain.nud_mbc84_Links_CH1.BackColor = Color.White

        frmMain.nud_mbc84_Rechts_CH1.Enabled = True
        frmMain.nud_mbc84_Rechts_CH1.BackColor = Color.White

        frmMain.nud_mbc84_Links_CH2.Enabled = True
        frmMain.nud_mbc84_Links_CH2.BackColor = Color.White

        frmMain.nud_mbc84_Rechts_CH2.Enabled = True
        frmMain.nud_mbc84_Rechts_CH2.BackColor = Color.White

        frmMain.nud_mbc84_Links_CH3.Enabled = True
        frmMain.nud_mbc84_Links_CH3.BackColor = Color.White

        frmMain.nud_mbc84_Rechts_CH3.Enabled = True
        frmMain.nud_mbc84_Rechts_CH3.BackColor = Color.White

        frmMain.nud_mbc84_Links_CH4.Enabled = True
        frmMain.nud_mbc84_Links_CH4.BackColor = Color.White

        frmMain.nud_mbc84_Rechts_CH4.Enabled = True
        frmMain.nud_mbc84_Rechts_CH4.BackColor = Color.White

        frmMain.nud_mbc84_Step_CH1.Enabled = True
        frmMain.nud_mbc84_Step_CH1.BackColor = Color.White

        frmMain.nud_mbc84_Step_CH2.Enabled = True
        frmMain.nud_mbc84_Step_CH2.BackColor = Color.White

        frmMain.nud_mbc84_Step_CH3.Enabled = True
        frmMain.nud_mbc84_Step_CH3.BackColor = Color.White

        frmMain.nud_mbc84_Step_CH4.Enabled = True
        frmMain.nud_mbc84_Step_CH4.BackColor = Color.White

        frmMain.btn_mbc84_Weg_CH1.Enabled = True
        frmMain.btn_mbc84_Weg_CH2.Enabled = True
        frmMain.btn_mbc84_Weg_CH3.Enabled = True
        frmMain.btn_mbc84_Weg_CH4.Enabled = True

    End Sub

    ' Uebernehmen der MM-Adrresse des Moduls

    Public Sub Tabpage_AddrSet()

        If (FLAG_Edit_Enable) Then
            FLAG_Prog_Index = PROG_INDEX_MMAddr
            Tabpage_Sperren(1)
            frmMain.Register_Modul_Buttons_Enable(False, True, True, False, False, False)
            MMAddr = "&h" & FormHEX("0", 4, frmMain.nud_mbc84_MMAddr.Value)
            frmMain.nud_mbc84_MMAddr.BackColor = Color.Wheat
            frmMain.txt_mbc84_adr_1.Text = Val("&H" & Hex(frmMain.nud_mbc84_MMAddr.Value)) - &H3000 + 1
            frmMain.txt_mbc84_adr_2.Text = Val("&H" & Hex(frmMain.nud_mbc84_MMAddr.Value)) - &H3000 + 2
            frmMain.txt_mbc84_adr_3.Text = Val("&H" & Hex(frmMain.nud_mbc84_MMAddr.Value)) - &H3000 + 3
            frmMain.txt_mbc84_adr_4.Text = Val("&H" & Hex(frmMain.nud_mbc84_MMAddr.Value)) - &H3000 + 4
        End If

    End Sub

    ' Uebernehmen des Rueckmeldezyklus des Moduls

    Public Sub Tabpage_RueckSet()

        If (FLAG_Edit_Enable) Then
            FLAG_Prog_Index = PROG_INDEX_MMRueck
            Tabpage_Sperren(3)
            frmMain.Register_Modul_Buttons_Enable(False, True, True, False, False, False)
            MMRueck = frmMain.nud_mbc84_Rueck.Value
            frmMain.nud_mbc84_Rueck.BackColor = Color.Wheat
        End If

    End Sub

    ' Uebernehmen der Servoeinstellungen des Moduls

    Public Sub Tabpage_ServoSet(ByRef nud As Object, ByRef servo As Object)

        If (FLAG_Edit_Enable) Then
            FLAG_Prog_Index = PROG_INDEX_SERVOMAX
            Tabpage_Sperren(0)
            frmMain.Register_Modul_Buttons_Enable(False, True, True, False, False, False)
            servo = nud.Value
            nud.BackColor = Color.Wheat
        End If

    End Sub

    ' Uebernehmen der Servoeinstellungen des Moduls

    Public Sub Tabpage_ServoWeg()

        If (FLAG_Edit_Enable) Then
            FLAG_Prog_Index = PROG_INDEX_SERVOWEG
            Tabpage_Sperren(2)
            frmMain.Register_Modul_Buttons_Enable(False, False, False, False, False, False)
        End If

    End Sub

    ' Zeige Servo-Parametriefenster an

    Public Sub Tabpage_Servos_Show(index As Integer)
        MMServo_S_Weg = MBC_84_S_SVWEG + (MMServo_Steps_New * 2) * index
        frmServo.ShowDialog()
    End Sub

    ' Zeige Modulwerte an

    Public Sub Tabpage_Werte_Show() Implements IMbcModul.Tabpage_Werte_Show

        Try
            frmMain.nud_mbc84_MMAddr.Value = Val(MMAddr)
        Catch e As Exception
            frmMain.nud_mbc84_MMAddr.Value = Val(MM_New)
        End Try

        Try
            frmMain.nud_mbc84_Rueck.Value = Val(MMRueck)
        Catch e As Exception
            frmMain.nud_mbc84_Rueck.Value = 0
        End Try

        frmMain.txt_mbc84_adr_1.Text = Val("&H" & Hex(frmMain.nud_mbc84_MMAddr.Value)) - &H3000 + 1
        frmMain.txt_mbc84_adr_2.Text = Val("&H" & Hex(frmMain.nud_mbc84_MMAddr.Value)) - &H3000 + 2
        frmMain.txt_mbc84_adr_3.Text = Val("&H" & Hex(frmMain.nud_mbc84_MMAddr.Value)) - &H3000 + 3
        frmMain.txt_mbc84_adr_4.Text = Val("&H" & Hex(frmMain.nud_mbc84_MMAddr.Value)) - &H3000 + 4

        Try
            frmMain.nud_mbc84_Links_CH1.Value = Val(MMServo_L1)
        Catch e As Exception
            frmMain.nud_mbc84_Links_CH1.Value = MMServo_L_New
        End Try

        Try
            frmMain.nud_mbc84_Rechts_CH1.Value = Val(MMServo_R1)
        Catch e As Exception
            frmMain.nud_mbc84_Rechts_CH1.Value = MMServo_R_New
        End Try

        Try
            frmMain.nud_mbc84_Step_CH1.Value = Val(MMServo_1VZ)
        Catch e As Exception
            frmMain.nud_mbc84_Step_CH1.Value = MMServo_VZ_New
        End Try

        Try
            frmMain.nud_mbc84_Links_CH2.Value = Val(MMServo_L2)
        Catch e As Exception
            frmMain.nud_mbc84_Links_CH2.Value = MMServo_L_New
        End Try

        Try
            frmMain.nud_mbc84_Rechts_CH2.Value = Val(MMServo_R2)
        Catch e As Exception
            frmMain.nud_mbc84_Rechts_CH2.Value = MMServo_R_New
        End Try

        Try
            frmMain.nud_mbc84_Step_CH2.Value = Val(MMServo_2VZ)
        Catch e As Exception
            frmMain.nud_mbc84_Step_CH2.Value = MMServo_VZ_New
        End Try

        Try
            frmMain.nud_mbc84_Links_CH3.Value = Val(MMServo_L3)
        Catch e As Exception
            frmMain.nud_mbc84_Links_CH3.Value = MMServo_L_New
        End Try

        Try
            frmMain.nud_mbc84_Rechts_CH3.Value = Val(MMServo_R3)
        Catch e As Exception
            frmMain.nud_mbc84_Rechts_CH3.Value = MMServo_R_New
        End Try

        Try
            frmMain.nud_mbc84_Step_CH3.Value = Val(MMServo_3VZ)
        Catch e As Exception
            frmMain.nud_mbc84_Step_CH3.Value = MMServo_VZ_New
        End Try

        Try
            frmMain.nud_mbc84_Links_CH4.Value = Val(MMServo_L4)
        Catch e As Exception
            frmMain.nud_mbc84_Links_CH4.Value = MMServo_L_New
        End Try

        Try
            frmMain.nud_mbc84_Rechts_CH4.Value = Val(MMServo_R4)
        Catch e As Exception
            frmMain.nud_mbc84_Rechts_CH4.Value = MMServo_R_New
        End Try

        Try
            frmMain.nud_mbc84_Step_CH4.Value = Val(MMServo_4VZ)
        Catch e As Exception
            frmMain.nud_mbc84_Step_CH4.Value = MMServo_VZ_New
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

        ' Weise die MMAddr zu und aktiviere den Updatemodus

        If FLAG_Prog_Index = PROG_INDEX_MMAddr Then

            ' Die Indices x_81_x stehen stellvertretend fuer alle MM-Module

            MsgIOStart = MBC_84_MM_H
            MsgIOMax = MBC_84_MM_L
            MsgIOWrite = EE_84_MMAddr

            Tabpage_Aktivieren()


            SYS_ARRAY(MBC_84_MM_H) = Microsoft.VisualBasic.Mid(MMAddr, 3, 2)
            SYS_ARRAY(MBC_84_MM_L) = Microsoft.VisualBasic.Right(MMAddr, 2)

        End If

        ' Weise die Servoanschlaege und Schrittweite zu und aktiviere den Updatemodus

        If FLAG_Prog_Index = PROG_INDEX_SERVOMAX Then

            MsgIOStart = MBC_84_S_SVMAX
            MsgIOMax = MBC_84_S_SVMAX + MBC_84_L_SVMAX - 1
            MsgIOWrite = EE_84_SERVOSMAX

            Tabpage_Aktivieren()

            SYS_ARRAY(MBC_84_S_SVMAX) = FormHEX("0", 2, MMServo_L1)
            SYS_ARRAY(MBC_84_S_SVMAX + 1) = FormHEX("0", 2, MMServo_R1)
            SYS_ARRAY(MBC_84_S_SVMAX + 2) = FormHEX("0", 2, MMServo_L2)
            SYS_ARRAY(MBC_84_S_SVMAX + 3) = FormHEX("0", 2, MMServo_R2)
            SYS_ARRAY(MBC_84_S_SVMAX + 4) = FormHEX("0", 2, MMServo_L3)
            SYS_ARRAY(MBC_84_S_SVMAX + 5) = FormHEX("0", 2, MMServo_R3)
            SYS_ARRAY(MBC_84_S_SVMAX + 6) = FormHEX("0", 2, MMServo_L4)
            SYS_ARRAY(MBC_84_S_SVMAX + 7) = FormHEX("0", 2, MMServo_R4)
            SYS_ARRAY(MBC_84_S_SVMAX + 9) = FormHEX("0", 2, MMServo_1VZ)
            SYS_ARRAY(MBC_84_S_SVMAX + 10) = FormHEX("0", 2, MMServo_2VZ)
            SYS_ARRAY(MBC_84_S_SVMAX + 11) = FormHEX("0", 2, MMServo_3VZ)
            SYS_ARRAY(MBC_84_S_SVMAX + 12) = FormHEX("0", 2, MMServo_4VZ)

        End If

        ' Weise Meldezyklus zu

        If FLAG_Prog_Index = PROG_INDEX_MMRueck Then

            MsgIOStart = MBC_84_RUECK
            MsgIOMax = MBC_84_RUECK
            MsgIOWrite = EE_84_RUECK

            Tabpage_Aktivieren()

            SYS_ARRAY(MBC_84_RUECK) = FormHEX("0", 2, MMRueck)

        End If

        ' mbc-84
        ' Weise die Servowege zu und aktiviere den Updatemodus

        ' Funktion wird in der class frmServo abgearbeitet

        ' Starte Aktualisierung des Moduls

        frmMain.COMUpdateStart(MsgIOStart, MsgIOMax, MsgIOWrite)

    End Sub

    ' Fuelle Modul mit den aus dem Modul gelesenen spezifischenWerten und Parameter
    Public Sub Parameter_Fill() Implements IMbcModul.Parameter_Fill

        ' MMAdresse 

        MMAddr = "&H" & SYS_ARRAY(MBC_84_MM_H) & SYS_ARRAY(MBC_84_MM_L)

        ' MM-Stellungsanzeige

        MMValue = "&H" & SYS_ARRAY(MBC_84_MM)

        ' Zwangsrueckmeldung der Magnetartikel 

        MMRueck = Convert.ToInt32(SYS_ARRAY(MBC_84_RUECK), 16)

        ' Servo-Anschlaege und Schrittweite

        MMServo_L1 = Convert.ToInt32(SYS_ARRAY(MBC_84_SVMAX), 16)
        MMServo_R1 = Convert.ToInt32(SYS_ARRAY(MBC_84_SVMAX + 1), 16)
        MMServo_L2 = Convert.ToInt32(SYS_ARRAY(MBC_84_SVMAX + 2), 16)
        MMServo_R2 = Convert.ToInt32(SYS_ARRAY(MBC_84_SVMAX + 3), 16)
        MMServo_L3 = Convert.ToInt32(SYS_ARRAY(MBC_84_SVMAX + 4), 16)
        MMServo_R3 = Convert.ToInt32(SYS_ARRAY(MBC_84_SVMAX + 5), 16)
        MMServo_L4 = Convert.ToInt32(SYS_ARRAY(MBC_84_SVMAX + 6), 16)
        MMServo_R4 = Convert.ToInt32(SYS_ARRAY(MBC_84_SVMAX + 7), 16)
        MMServo_Center = Convert.ToInt32(SYS_ARRAY(MBC_84_SVMAX + 8), 16)
        MMServo_1VZ = Convert.ToInt32(SYS_ARRAY(MBC_84_SVMAX + 9), 16)
        MMServo_2VZ = Convert.ToInt32(SYS_ARRAY(MBC_84_SVMAX + 10), 16)
        MMServo_3VZ = Convert.ToInt32(SYS_ARRAY(MBC_84_SVMAX + 11), 16)
        MMServo_4VZ = Convert.ToInt32(SYS_ARRAY(MBC_84_SVMAX + 12), 16)


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
        SYS_ARRAY(MBC_84_MM) = FormHEX("0", 2, index)

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
