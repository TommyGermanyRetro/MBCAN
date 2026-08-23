Public Class mbc_modul_stammdaten_cl

    Public Status As Integer                    ' Zeigt an, ob das jeweilige Modul aktiv oder inaktiv ist
    Public IsAlive As Integer                   ' Zeigt an, ob das jeweilige Modul am Bus ist
    Public Name As String                       ' Modulname
    Public SW As String                         ' Softwareversion
    Public HW As String                         ' Hardwareversion
    Public ArtNum As String                     ' Artikelnummer
    Public GUiD As String                       ' CS2-GUiD
    Public MsgGUiD As String                    ' Nachricht fuer CS2-Kommunikation zur GUiD
    Public SerNr As String                      ' Seriennummer des Moduls
    Public Plugged As String                    ' Status Steckernetzteil
    Public Spannung As String                   ' Spannungswert der Modulversorgung
    Public Temperatur As String                 ' Temperaturwert des Moduls
    Public SYS_ARRAY(MBC_ARRAY_ENDE) As String  ' Systemarray des Moduls fuer Kommunikation zwischen PC und Modul

    Sub New()

        ' Stammdaten initialisieren

        Status = 0
        IsAlive = 0
        Name = "-/-"
        SW = "-/-"
        HW = "-/-"
        ArtNum = "-/-"
        GUiD = "-/-"
        MsgGUiD = "-/-"
        SerNr = "-/-"
        Plugged = "-/-"
        Spannung = "0.00"
        Temperatur = "0.00"

        ' SYS_ARRAY initialisieren

        For index = 1 To MBC_ARRAY_ENDE
            SYS_ARRAY(index) = "00"
        Next

    End Sub

End Class
