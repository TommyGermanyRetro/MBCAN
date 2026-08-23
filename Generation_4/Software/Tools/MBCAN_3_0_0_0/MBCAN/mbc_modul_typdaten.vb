Public Class mbc_modul_typdaten_cl

    Public TypIsActive As Boolean       ' Modultyp ist aktiv
    Public Bezeichner As String         ' Modultyp
    Public Count As Integer             ' Anzahl der Module des Typs
    Public Image As Image               ' Image des Moduls
    Public Keyboard As Boolean          ' Keyboardnutzung des Moduls
    Public UpgradeDatum As String       ' Moduldatum ab dem Upgrades möglich sind
    Public UpgradeSoftware As String    ' Modulsoftware ab dem Upgrades möglich sind
    Public RMImageOn(19) As Boolean     ' Array mit den freizuschaltenden Rueckmeldesymbolen
    Public RMImage(19) As Image         ' Array mit den darzustellenden Rueckmeldesymbolen
    Public TImageOn(8) As Boolean       ' Array mit den freizuschaltenden Typsymbolen
    Public TImage(8) As Image           ' Array mit den darzustellenden Typsymbolen
    Sub New()

        ' Bilder fuer die Anzeige der Rueckmeldungen/Stellungen initialisieren

        For index = 1 To 19
            RMImageOn(index) = False
            RMImage(index) = ImGRAU
        Next

        ' Bilder fuer die Anzeige der Bestueckung mbc-83 initialisieren

        For index = 1 To 8
            TImageOn(index) = False
            TImage(index) = ImX
        Next

        ' Vorbelegung Typdaten

        TypIsActive = False
        Bezeichner = "-/-"
        Image = Nothing
        Keyboard = False
        UpgradeDatum = "-/-"
        UpgradeSoftware = "-/-"
        Count = 0

    End Sub

    ' Ueberladener Konstruktor fuer die 14 konkreten Modultypen (mbc_modul_typ_80_cl ... _93_cl):
    ' fasst die bisher in jeder Unterklasse wiederholt vorkommende Init-Logik an einer Stelle
    ' zusammen. rmIndices/tIndices: Indizes, die abweichend von der Vorbelegung freizuschalten sind.

    Sub New(bez As String, img As Image, hatKeyboard As Boolean, updDatum As String, updSw As String,
            Optional rmIndices As Integer() = Nothing, Optional tIndices As Integer() = Nothing, Optional tImg As Image = Nothing)

        Me.New()

        TypIsActive = True
        Bezeichner = bez
        Image = img
        Keyboard = hatKeyboard
        UpgradeDatum = updDatum
        UpgradeSoftware = updSw

        If rmIndices IsNot Nothing Then
            For Each i In rmIndices
                RMImageOn(i) = True
            Next
        End If

        If tIndices IsNot Nothing Then
            For Each i In tIndices
                TImageOn(i) = True
                TImage(i) = tImg
            Next
        End If

    End Sub

End Class
