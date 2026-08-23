Imports System.Windows.Forms.DataVisualization.Charting

Public Class frmServo

    ' Einzige Quelle: mbc_modul_84_cl.SERVO_MAX_STEPS (entspricht SERVO_MAX_STEPS in
    ' Mbc84Com.cpp, dort manuell synchron zu halten) - hier nur noch ausgelesen, keine eigene Kopie

    Public ServoSteps = mbc_modul_84_cl.SERVO_MAX_STEPS

    ' Physikalische PWM-Rohwertgrenzen des Servowegs - manuell synchron zu RAW_MIN/RAW_MAX in
    ' Mbc84Com.cpp zu halten (kein automatischer Abgleich, anders als ServoSteps oben).
    ' RawToChart()/ChartToRaw() bilden damit dieselbe proportionale Skalierung nach, die die
    ' Firmware in eepromWriteParaExtern()/servoCurveInit() verwendet (Konfigwert 0...RT_GN_MAX
    ' ist dort nur noch eine Stuetzstelle, KEIN 1:1-Rohwert-Offset mehr).

    Public ServoRawMin As Integer = 1
    Public ServoRawMax As Integer = 126

    Public ServoWeg((ServoSteps * 2)) As Integer ' Servoweg-Array (Real-Raum, Anzeige/Bearbeitung)

    ' Unverfaelschte Design-Kurve (0...ServoSteps, direkt aus RawToChart()/der Datei, OHNE den
    ' verlustbehafteten DesignToReal()/RealToDesign()-Umweg) plus Merker je Schrittindex, ob der
    ' Nutzer diesen Punkt tatsaechlich manuell bearbeitet hat. Ohne das wuerde jedes Oeffnen+Ok
    ' (auch ohne jede Aenderung) die volle Kurve durch die grobe Real-Raum-Rundung schleusen und
    ' dauerhaft vergroebern - siehe CurrentDesignValue().

    Public ServoWegDesign((ServoSteps * 2)) As Integer
    Public ServoWegEdited((ServoSteps * 2)) As Boolean

    Dim ServoLMax As Integer        ' Servoanschlag Links
    Dim ServoRMax As Integer        ' Servoanschlag Rechts
    Dim ServoCenter As Integer      ' Physikalischer Rohwert-Mittelpunkt (CENTER)
    Dim ServoNr As Integer          ' Servonummer
    Public FLAG_Change As Boolean   ' Anzeige einer gueltigen Aenderung
    Dim FLAG_ChangeView As Boolean  ' Aenderungssperre fuer Ausschlagszaehler

    ' Rechnet einen physikalischen Rohwert (ServoRawMin...ServoRawMax) proportional in einen
    ' Diagramm-/Ausschlagswert (0...ServoSteps, Mitte=ServoSteps\2) um - Umkehrung von ChartToRaw()

    Private Function RawToChart(ByVal raw As Integer) As Integer

        If raw <= ServoCenter Then
            Return (ServoSteps \ 2) - (((ServoCenter - raw) * (ServoSteps \ 2)) \ (ServoCenter - ServoRawMin))
        Else
            Return (ServoSteps \ 2) + (((raw - ServoCenter) * (ServoSteps \ 2)) \ (ServoRawMax - ServoCenter))
        End If

    End Function

    ' Rechnet einen Diagramm-/Ausschlagswert (0...ServoSteps) proportional in einen physikalischen
    ' Rohwert (ServoRawMin...ServoRawMax) um - Umkehrung von RawToChart(), spiegelt die
    ' Skalierungsformel aus Mbc84Com.cpp::eepromWriteParaExtern()

    Private Function ChartToRaw(ByVal chart As Integer) As Integer

        If chart <= (ServoSteps \ 2) Then
            Return ServoCenter - ((((ServoSteps \ 2) - chart) * (ServoCenter - ServoRawMin)) \ (ServoSteps \ 2))
        Else
            Return ServoCenter + (((chart - (ServoSteps \ 2)) * (ServoRawMax - ServoCenter)) \ (ServoSteps \ 2))
        End If

    End Function

    ' Rechnet einen Design-Ausschlagswert (0...ServoSteps, die volle im SYS_ARRAY/in Dateien
    ' gespeicherte Kurve) proportional auf den aktuell konfigurierten realen Stellbereich
    ' (ServoLMax...ServoRMax) um - spiegelt Mbc84Com.cpp::rawAtStepScaled(), nur in Chart- statt
    ' Rohwert-Einheiten. Divisor ist bewusst die feste Design-Spanne (ServoSteps\2), NICHT
    ' (ServoSteps\2 - ServoLMax)/(ServoRMax - ServoSteps\2) - die koennten bei RT-/GN-max=0 Null
    ' werden, hier aber nicht (nur RealToDesign(), die Umkehrung, braucht dort eine Absicherung).

    Public Function DesignToReal(ByVal designChart As Integer) As Integer
        Dim half As Integer = ServoSteps \ 2

        If designChart <= half Then
            Return half - (((half - designChart) * (half - ServoLMax)) \ half)
        Else
            Return half + (((designChart - half) * (ServoRMax - half)) \ half)
        End If

    End Function

    ' Rechnet einen realen, auf ServoLMax...ServoRMax begrenzten Ausschlagswert zurueck in den
    ' vollen Design-Wertebereich (0...ServoSteps) - Umkehrung von DesignToReal(), wird beim
    ' Speichern (SYS_ARRAY UND Servoweg-Dateien) gebraucht, damit die gespeicherte Kurve immer
    ' die volle Design-Kurve bleibt und bei spaeterer Grenzaenderung nicht verloren geht. Sonderfall
    ' RT-max/GN-max=0 (ServoLMax/ServoRMax=half, kein Ausschlag in diese Richtung konfiguriert):
    ' DesignToReal() bildet dann die gesamte betroffene Haelfte auf genau "half" ab, die Umkehrung
    ' ist dort nicht eindeutig definierbar - liefert dann ebenfalls "half" statt durch 0 zu teilen.

    Public Function RealToDesign(ByVal realChart As Integer) As Integer
        Dim half As Integer = ServoSteps \ 2

        If realChart <= half Then
            If (half - ServoLMax) = 0 Then Return half
            Return half - (((half - realChart) * half) \ (half - ServoLMax))
        Else
            If (ServoRMax - half) = 0 Then Return half
            Return half + (((realChart - half) * half) \ (ServoRMax - half))
        End If

    End Function

    ' Liefert den zu speichernden Design-Wert (0...ServoSteps) fuer einen Schrittindex - fuer vom
    ' Nutzer NICHT bearbeitete Punkte die unveraenderte, pixelgenaue Original-Kurve
    ' (ServoWegDesign), fuer tatsaechlich bearbeitete Punkte den neu eingestellten Real-Wert
    ' zurueckgerechnet (RealToDesign()). Gemeinsam genutzt von btn_servo_ok_Click() (SYS_ARRAY)
    ' und DB_Servo_Speichern() (Datei), damit beide Wege konsistent sind.

    Public Function CurrentDesignValue(ByVal index As Integer) As Integer

        If ServoWegEdited(index) Then
            Return RealToDesign(ServoWeg(index))
        Else
            Return ServoWegDesign(index)
        End If

    End Function

    ' Funktion beim Laden des Formulars mit Initialisierung des Servowegs fuer das augewaehlte Servo

    Private Sub frmServo_Load(sender As Object, e As EventArgs) Handles MyBase.Load
        Dim Schritt As Integer

        ' Hauptfenster sperren

        frmMain.Enabled = False
        AddHandler Me.LocationChanged, AddressOf ClampInnerhalbEltern

        ' Form-Variablen initialisieren

        'MBC.ModulTyp(AktTyp).Modul(AktNr).MMServo_Steps

        ServoNr = (MBC.ModulTyp(AktTyp).Modul(AktNr).MMServo_S_Weg - MBC.ModulTyp(AktTyp).Modul(AktNr).MMServoWegSpeicher) / (ServoSteps * 2)
        ServoCenter = Convert.ToInt32(MBC.ModulTyp(AktTyp).Modul(AktNr).MMServo_Center, 10)
        ServoLMax = Convert.ToInt32(MBC.ModulTyp(AktTyp).Modul(AktNr).SYS_ARRAY(MBC.ModulTyp(AktTyp).Modul(AktNr).MMServoMaxSpeicher + 2 * ServoNr), 16)
        ServoRMax = Convert.ToInt32(MBC.ModulTyp(AktTyp).Modul(AktNr).SYS_ARRAY(MBC.ModulTyp(AktTyp).Modul(AktNr).MMServoMaxSpeicher + (2 * ServoNr) + 1), 16)

        ' Servoanschlaege anzeigen (rohe Konfigwerte, 0...RT_GN_MAX)

        txt_servo_linksmax.Text = ServoLMax
        txt_servo_rechtsmax.Text = ServoRMax

        ' Servoanschlaege fuer den Ausschlagszaehler in reale Chart-Grenzen umrechnen (siehe
        ' DesignToReal()/RealToDesign()) - der Nutzer soll direkt sehen und darauf begrenzt sein,
        ' was der aktuelle Stellbereich tatsaechlich zulaesst

        ServoLMax = (ServoSteps \ 2) - ServoLMax
        ServoRMax = (ServoSteps \ 2) + ServoRMax

        nud_servo_wert.Minimum = ServoLMax
        nud_servo_wert.Maximum = ServoRMax

        ' Setze Schrittzaehler auf 1

        nud_servo_step.Value = 1

        ' Initiaisiere den Servoweg (real, auf ServoLMax...ServoRMax skaliert) aus dem SYS-ARRAY
        ' des entsprechenden Moduls - MUSS vor der ersten nud_servo_wert.Value-Zuweisung laufen,
        ' sonst koennte der (noch unskalierte, ggf. ausserhalb Minimum/Maximum liegende) Default-
        ' wert 0 die neuen, jetzt tatsaechlich durchgesetzten NUD-Grenzen verletzen

        servoweg_init()

        ' Setze Ausschlagszaehler auf den Wert des ersten Schritts

        Schritt = ServoWeg(nud_servo_step.Value - 1)

        nud_servo_wert.Value = Schritt

        ' Zeige das Startwegdiagramm des Servos an

        servoweg_chart_fuellen(nud_servo_step.Value)

        ' Setze alle Flags zurueck und deaktiviere den Verwerfen-Button

        FLAG_Change = False
        FLAG_ChangeView = False
        btn_servo_verwerfen.Enabled = FLAG_Change
        btn_servo_ok.Text = "Ok"

    End Sub

    ' Funktion beim Schliessen des Formulars

    Private Sub form_closing(sender As Object, e As FormClosingEventArgs) Handles MyBase.FormClosing

        ' Beim Schliessen das Hauptfenster wieder freigeben

        frmMain.Enabled = True

    End Sub

    ' Funktion zur Initialisierung des Servoweg-Arrays aus dem SYS-ARRAY des entsprechenden Moduls

    Private Sub servoweg_init()
        Dim Schritt As Integer

        For index = 0 To (ServoSteps * 2 - 1)

            Schritt = RawToChart(Convert.ToInt32(MBC.ModulTyp(AktTyp).Modul(AktNr).SYS_ARRAY(MBC.ModulTyp(AktTyp).Modul(AktNr).MMServo_S_Weg + index), 16))

            ServoWegDesign(index) = Schritt
            ServoWeg(index) = DesignToReal(Schritt)
            ServoWegEdited(index) = False

        Next

    End Sub

    ' Funktion zur Darstellung des Servowegs

    Private Sub servoweg_chart_fuellen(ByVal schritt As Integer)
        Dim index As Integer

        With chart_servo

            ' Richte Diagrammlinien ein

            .Series.Clear()
            .Series.Add(New Series("Nulllinie"))
            .Series.Add(New Series("Richtung"))
            .Series.Add(New Series("Servo"))
            .Series.Add(New Series("Lineal"))

            ' Richte Achsen ein

            .ChartAreas(0).AxisX.Minimum = 0
            .ChartAreas(0).AxisX.Maximum = (ServoSteps * 2)
            .ChartAreas(0).AxisX.Interval = 10
            .ChartAreas(0).AxisX.TitleAlignment = StringAlignment.Near
            .ChartAreas(0).AxisX.MinorGrid.LineColor = Color.Gray
            .ChartAreas(0).AxisX.MajorGrid.LineColor = Color.Gray
            .ChartAreas(0).AxisX.LabelStyle.Font = New Font("Thahoma", 8, FontStyle.Regular)
            .ChartAreas(0).AxisX.LabelStyle.Angle = -45

            .ChartAreas(0).AxisY.Minimum = 0
            .ChartAreas(0).AxisY.Maximum = ServoSteps
            .ChartAreas(0).AxisY.Interval = 10
            .ChartAreas(0).AxisY.TitleAlignment = StringAlignment.Near
            .ChartAreas(0).AxisY.MinorGrid.LineColor = Color.Gray
            .ChartAreas(0).AxisY.MajorGrid.LineColor = Color.Gray
            .ChartAreas(0).AxisY.LabelStyle.Font = New Font("Thahoma", 8, FontStyle.Regular)

            ' Zeige Nulllinie an

            .Series(0).Points.AddXY(0, (ServoSteps / 2))
            .Series(0).Points.AddXY((ServoSteps * 2 - 1), (ServoSteps / 2))
            .Series(0).BorderWidth = 2
            .Series(0).Color = Color.Orange
            .Series(0).ChartType = SeriesChartType.Line

            ' Zeige Richtung an

            .Series(1).Points.AddXY(ServoSteps, ServoSteps)
            .Series(1).Points.AddXY(ServoSteps, 0)
            .Series(1).BorderWidth = 2
            .Series(1).Color = Color.Orange
            .Series(1).ChartType = SeriesChartType.Line

            ' Zeige Servoweg an

            For index = 0 To (ServoSteps * 2 - 1)
                .Series(2).Points.AddXY(index, ServoWeg(index))
            Next

            .Series(2).Color = Color.Black
            .Series(2).BorderWidth = 3
            .Series(2).ChartType = SeriesChartType.Line

            ' Zeige Lineal an

            .Series(3).Points.AddXY(schritt, 1)
            .Series(3).Points.AddXY(schritt, 119)
            .Series(3).BorderWidth = 2
            .Series(3).Color = Color.Blue
            .Series(3).ChartType = SeriesChartType.Line

        End With

    End Sub

    ' Funktion zum Laden-Button

    Private Sub btn_servo_laden_Click(sender As Object, e As EventArgs) Handles btn_servo_laden.Click

        ' Hole neues Diagram aus der Datei

        DB_Servo_Laden()

        ' Zeige das geladene Wegdiagramm des Servos an

        If (FLAG_Change) Then
            servoweg_chart_fuellen(1)
            btn_servo_ok.Text = "Update"
            btn_servo_verwerfen.Enabled = True
            FLAG_ChangeView = False
        End If

    End Sub

    ' Funktion zum Speichern-Button

    Private Sub btn_servo_speichern_Click(sender As Object, e As EventArgs) Handles btn_servo_speichern.Click

        ' Speichere Datei mit aktuellem Servoweg

        DB_Servo_Speichern()

    End Sub

    ' Funktion zum Verwerfen-Button

    Private Sub btn_servo_verwerfen_Click(sender As Object, e As EventArgs) Handles btn_servo_verwerfen.Click

        servoweg_init()
        servoweg_chart_fuellen(nud_servo_step.Value)
        nud_servo_wert.Value = ServoWeg(nud_servo_step.Value - 1)
        FLAG_Change = False
        btn_servo_verwerfen.Enabled = FLAG_Change
        btn_servo_ok.Text = "Ok"

    End Sub

    ' Funktion zum Ok-Button

    Private Sub btn_servo_ok_Click(sender As Object, e As EventArgs) Handles btn_servo_ok.Click
        Dim Schritt As Integer

        ' Wenn sich ein Wert des Weges geaendert hat, dann schreibe den Wert ins SYS-ARRAY zurueck und
        ' aktiviere den Update-Button im Hauptfenster. Sperre ausserdem die anderen Eingabemoeglichkeiten
        ' des Registerblatts mbc-84

        If FLAG_Change Then

            For index = 0 To (ServoSteps * 2 - 1)
                Schritt = ChartToRaw(CurrentDesignValue(index))
                MBC.ModulTyp(AktTyp).Modul(AktNr).SYS_ARRAY(MBC.ModulTyp(AktTyp).Modul(AktNr).MMServo_S_Weg + index) = FormHEX("0", 2, Schritt)
            Next

            ' Eingaben sperren

            MBC.ModulTyp("mbc-84").Modul(AktNr).Tabpage_ServoWeg()

            ' Eingaben freigeben

            MBC.ModulTyp("mbc-84").Modul(AktNr).Tabpage_aktivieren()

            ' Starte Aktualisierung des Moduls

            frmMain.COMUpdateStart(MBC.ModulTyp(AktTyp).Modul(AktNr).MMServo_S_Weg, MBC.ModulTyp(AktTyp).Modul(AktNr).MMServo_S_Weg + MBC.ModulTyp(AktTyp).Modul(AktNr).MMServo_Steps * 2 - 1, MBC.ModulTyp(AktTyp).Modul(AktNr).MMServoUpdateSpeicher)

        End If

        Me.Close()

    End Sub

    ' Funktion zum Schrittzaehler

    Private Sub nud_servo_step_ValueChanged(sender As Object, e As EventArgs) Handles nud_servo_step.ValueChanged

        FLAG_ChangeView = True
        nud_servo_wert.Value = ServoWeg(nud_servo_step.Value - 1)
        servoweg_chart_fuellen(nud_servo_step.Value)

    End Sub

    ' Funktion zum Ausschlagszaehler

    Private Sub nud_servo_wert_ValueChanged(sender As Object, e As EventArgs) Handles nud_servo_wert.ValueChanged

        ServoWeg(nud_servo_step.Value - 1) = nud_servo_wert.Value
        servoweg_chart_fuellen(nud_servo_step.Value)
        If FLAG_ChangeView = False Then
            ServoWegEdited(nud_servo_step.Value - 1) = True
            FLAG_Change = True
            btn_servo_ok.Text = "Update"
        End If
        FLAG_ChangeView = False
        btn_servo_verwerfen.Enabled = FLAG_Change

    End Sub


End Class