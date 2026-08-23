'**************************************************************************************************
'*** Moduldatenbanken                                                                           ***
'**************************************************************************************************

Imports System.IO

Module datenbanken

    '**********************************************************************************************
    '*** Menue - Kommunikationsparameter laden                                                  ***
    '**********************************************************************************************

    Public Sub DB_KomPara_Laden()
        Dim RecFile As FileStream
        Dim RecFileRd As StreamReader

        ' Ordner erstellen wenn nicht existent

        If System.IO.Directory.Exists("Inifiles") = False Then

            System.IO.Directory.CreateDirectory("Inifiles")

        End If

        Try

            If File.Exists("Inifiles/mbcan_init.txt") Then
                RecFile = New FileStream("Inifiles/mbcan_init.txt", FileMode.Open)
                RecFileRd = New StreamReader(RecFile)

                Dim FirstLine As String = RecFileRd.ReadLine

                If FirstLine IsNot Nothing AndAlso FirstLine.Contains("=") Then

                    ' Neues Schluessel=Wert-Format (ein Schluessel je Zeile)

                    Dim kv As New Dictionary(Of String, String)
                    For Each pair As KeyValuePair(Of String, String) In KVParse(FirstLine)
                        kv(pair.Key) = pair.Value
                    Next
                    While Not RecFileRd.EndOfStream
                        For Each pair As KeyValuePair(Of String, String) In KVParse(RecFileRd.ReadLine)
                            kv(pair.Key) = pair.Value
                        Next
                    End While

                    RS232Port = KVGet(kv, "RS232Port", "-")
                    BTPort = KVGet(kv, "BTPort", "-")
                    CS2_Kenner = KVGet(kv, "CS2_Kenner", "")
                    CS2_Identifier = KVGet(kv, "CS2_Identifier", "")
                    TCPIP = KVGet(kv, "TCPIP", "0.0.0.0")
                    SprachIndex = Convert.ToInt32(KVGet(kv, "SprachIndex", "0"))

                Else

                    ' Altes Positionszeilen-Format - fuer bereits vorhandene Dateien

                    RS232Port = FirstLine
                    BTPort = RecFileRd.ReadLine
                    CS2_Kenner = RecFileRd.ReadLine
                    CS2_Identifier = RecFileRd.ReadLine
                    TCPIP = RecFileRd.ReadLine
                    SprachIndex = Convert.ToInt32(RecFileRd.ReadLine)

                End If

                RecFileRd.Close()
                RecFile.Close()
            End If

        Catch ex As Exception

        End Try

        If CS2_Kenner = "" Then
            CS2_Kenner = "AAAA"
        End If

        If CS2_Identifier = "" Then
            CS2_Identifier = "6D"
        End If

        If RS232Port <> "-" Then
            With RS232Client
                .PortName = RS232Port
                .BaudRate = Val(RS232BaudRate)
                .Parity = IO.Ports.Parity.None
                .StopBits = IO.Ports.StopBits.One
                .DataBits = 8
                .ReadTimeout = 1000
                .WriteTimeout = 1000
            End With
            frmMain.ts_MenuVerbindungRS232.Enabled = True
            frmMain.ts_MenuVerbindungRS232Trennen.Enabled = False
            frmMain.ts_MenuVerbindungRS232Verbinden.Enabled = True
            RS232IsInit = True
        End If

        If (BTPort <> "-") Then
            With BTClient
                .PortName = BTPort
                .BaudRate = BTVirtualBaudRate
                .DataBits = 8
                .Parity = IO.Ports.Parity.None
                .StopBits = IO.Ports.StopBits.One
                .Handshake = IO.Ports.Handshake.None
                .Encoding = System.Text.Encoding.Default
                .ReadTimeout = 1000
                .WriteTimeout = 1000
            End With
            frmMain.ts_MenuVerbindungBluetooth.Enabled = True
            frmMain.ts_MenuVerbindungBluetoothTrennen.Enabled = False
            frmMain.ts_MenuVerbindungBluetoothVerbinden.Enabled = True
            BTIsInit = True
        End If

        If TCPIP <> "0.0.0.0" Then
            frmMain.ts_MenuVerbindungTCP.Enabled = True
            frmMain.ts_MenuVerbindungTCPTrennen.Enabled = False
            frmMain.ts_MenuVerbindungTCPVerbinden.Enabled = True
            TCPIsInit = True
        End If

    End Sub

    '**********************************************************************************************
    '*** Menue - Kommunikationsparameter speichern                                              ***
    '**********************************************************************************************

    Public Sub DB_KomPara_Speichern()
        Dim RecFile As FileStream
        Dim RecFileWr As StreamWriter

        ' Ordner erstellen wenn nicht existent

        If System.IO.Directory.Exists("Inifiles") = False Then

            System.IO.Directory.CreateDirectory("Inifiles")

        End If

        If File.Exists("Inifiles/mbcan_init.txt") Then
            File.Delete("Inifiles/mbcan_init.txt")
        End If

        RecFile = New FileStream("Inifiles/mbcan_init.txt", FileMode.Create)
        RecFileWr = New StreamWriter(RecFile)
        RecFileWr.WriteLine("RS232Port=" & RS232Port)
        RecFileWr.WriteLine("BTPort=" & BTPort)
        RecFileWr.WriteLine("CS2_Kenner=" & CS2_Kenner)
        RecFileWr.WriteLine("CS2_Identifier=" & CS2_Identifier)
        RecFileWr.WriteLine("TCPIP=" & TCPIP)
        RecFileWr.WriteLine("SprachIndex=" & Str(SprachIndex))
        RecFileWr.Close()
        RecFile.Close()
    End Sub

    '**********************************************************************************************
    '*** Menue - Moduldatenbank laden                                                           ***
    '**********************************************************************************************

    Public Sub DB_Modul_Laden()
        Dim RecFile As FileStream
        Dim RecFileRd As StreamReader
        Dim RdLine As String
        Dim IndexT As Integer
        Dim IndexM As Integer
        Dim Status As Integer
        Dim Typ As String
        Dim ArtNum As String
        Dim SerNr As String
        Dim GUiD As String
        Dim SW As String
        Dim HW As String
        Dim MMAddr As String
        Dim S88ModNr As String
        Dim Name As String

        ' Ordner erstellen wenn nicht existent

        If System.IO.Directory.Exists("Inifiles") = False Then

            System.IO.Directory.CreateDirectory("Inifiles")

        End If

        ' Wenn Datenbank bereits angelegt war dann oeffne sie und lese
        ' die Daten aus

        If File.Exists("Inifiles/mbcan_db.txt") Then

            RecFile = New FileStream("Inifiles/mbcan_db.txt", FileMode.Open)
            RecFileRd = New StreamReader(RecFile)

            ' Im Header steht die Info zur Datenbankversion
            ' Lese die Datenbankversion aus fall die Datenbank nicht leer ist

            If Not (RecFileRd.EndOfStream) Then DBVersion = RecFileRd.ReadLine

            ' Solange die Datenbank nicht das Ende erreicht hat
            ' Lese Zeilen aus der Datenbank aus

            While Not (RecFileRd.EndOfStream)

                ' Lese Zeile aus Datenbank aus

                RdLine = RecFileRd.ReadLine

                If RdLine.Contains("=") Then

                    ' Neues Schluessel=Wert-Format (mehrere Paare je Zeile, per ";" getrennt)

                    Dim kv = KVParse(RdLine)
                    Status = Val(KVGet(kv, "Status", "0"))
                    Typ = KVGet(kv, "Typ", "")
                    ArtNum = KVGet(kv, "ArtNum", "")
                    SerNr = KVGet(kv, "SerNr", "")
                    GUiD = KVGet(kv, "GUiD", "")
                    SW = KVGet(kv, "SW", "")
                    HW = KVGet(kv, "HW", "")
                    MMAddr = KVGet(kv, "MMAddr", "0")
                    S88ModNr = KVGet(kv, "S88ModNr", "0")
                    Name = KVGet(kv, "Name", "")

                Else

                    ' Altes Format mit festen Zeichenspalten - fuer bereits vorhandene Datenbanken

                    Status = Val(Microsoft.VisualBasic.Mid(RdLine, 1, 1))
                    Typ = Microsoft.VisualBasic.Mid(RdLine, 3, 6)
                    ArtNum = Microsoft.VisualBasic.Mid(RdLine, 10, 4)
                    SerNr = Microsoft.VisualBasic.Mid(RdLine, 15, 18)
                    GUiD = Microsoft.VisualBasic.Mid(RdLine, 34, 10)
                    SW = Microsoft.VisualBasic.Mid(RdLine, 45, 4)
                    HW = Microsoft.VisualBasic.Mid(RdLine, 50, 8)
                    MMAddr = Microsoft.VisualBasic.Mid(RdLine, 59, 6)
                    S88ModNr = Microsoft.VisualBasic.Mid(RdLine, 66, 6)
                    Name = Microsoft.VisualBasic.Mid(RdLine, 73, 18)

                End If

                ' Bestimme Modultyp

                IndexT = frmMain.Strukturbaum_GetTypIndex(Typ)

                ' Erhoehe die Gesamtzahl der Module eines Typs

                MBC.ModulTyp(IndexT).Add(Name)

                IndexM = MBC.ModulTyp(IndexT).Count

                ' Weise die Infos dem Modul zu und speichere die
                ' Belegung des Modulplatzes

                With MBC.ModulTyp(IndexT).Modul(IndexM)
                    .ArtNum = ArtNum
                    .IsAlive = 0
                    .SerNr = SerNr
                    .SW = SW
                    .HW = HW
                    .GUiD = GUiD
                    .Name = Name
                    frmMain.Register_Set_MMAddr(Val(MMAddr), IndexT, IndexM)
                    frmMain.Register_Set_S88ModNr(Val(S88ModNr), IndexT, IndexM)
                    .MsgGUiD = Microsoft.VisualBasic.Mid(.GUiD, 3, 2) & " " _
                                & Microsoft.VisualBasic.Mid(.GUiD, 5, 2) & " " _
                                & Microsoft.VisualBasic.Mid(.GUiD, 7, 2) & " " _
                                & Microsoft.VisualBasic.Mid(.GUiD, 9, 2)
                End With

                ' Wenn das Modul aus der aktuellen Datenbank geloescht war dann zeige es als gesperrt an

                If Status <> IsDeleted Then
                    MBC.ModulTyp(IndexT).Modul(IndexM).Status = IsMissing
                Else
                    MBC.ModulTyp(IndexT).Modul(IndexM).Status = IsDeleted
                End If

                ' Fuege Modul in den Strukturmodul ein

                frmMain.Strukturbaum_MenueAddItem(IndexT)

            End While

            RecFileRd.Close()
            RecFile.Close()

        Else

            ' DB-Versionsnummer neu vergeben

            DBVersion = Microsoft.VisualBasic.DateString & Microsoft.VisualBasic.TimeOfDay
            DBVersion = Microsoft.VisualBasic.Mid(DBVersion, 1, 2) &
                        Microsoft.VisualBasic.Mid(DBVersion, 4, 2) &
                        Microsoft.VisualBasic.Mid(DBVersion, 9, 4) &
                        Microsoft.VisualBasic.Mid(DBVersion, 14, 2) &
                        Microsoft.VisualBasic.Mid(DBVersion, 17, 2)

        End If

        ' DB-Versionsnummer in CAN-Nachricht an die Module eintragen

        MsgDatenbankHIGH = MsgDatenbankHIGH & FormHEX("0", 2, Asc(Microsoft.VisualBasic.Mid(DBVersion, 1, 1))) _
                            & " " & FormHEX("0", 2, Asc(Microsoft.VisualBasic.Mid(DBVersion, 2, 1))) _
                            & " " & FormHEX("0", 2, Asc(Microsoft.VisualBasic.Mid(DBVersion, 3, 1))) _
                            & " " & FormHEX("0", 2, Asc(Microsoft.VisualBasic.Mid(DBVersion, 4, 1)))
        MsgDatenbankMID = MsgDatenbankMID & FormHEX("0", 2, Asc(Microsoft.VisualBasic.Mid(DBVersion, 5, 1))) _
                            & " " & FormHEX("0", 2, Asc(Microsoft.VisualBasic.Mid(DBVersion, 6, 1))) _
                            & " " & FormHEX("0", 2, Asc(Microsoft.VisualBasic.Mid(DBVersion, 7, 1))) _
                            & " " & FormHEX("0", 2, Asc(Microsoft.VisualBasic.Mid(DBVersion, 8, 1)))
        MsgDatenbankLOW = MsgDatenbankLOW & FormHEX("0", 2, Asc(Microsoft.VisualBasic.Mid(DBVersion, 9, 1))) _
                            & " " & FormHEX("0", 2, Asc(Microsoft.VisualBasic.Mid(DBVersion, 10, 1))) _
                            & " " & FormHEX("0", 2, Asc(Microsoft.VisualBasic.Mid(DBVersion, 11, 1))) _
                            & " " & FormHEX("0", 2, Asc(Microsoft.VisualBasic.Mid(DBVersion, 12, 1)))

    End Sub

    '**********************************************************************************************
    '*** Menue - Moduldatenbank speichern                                                       ***
    '**********************************************************************************************

    Public Sub DB_Modul_Speichern(Block As Integer)
        Dim UserPfad As String
        Dim RecFile As FileStream
        Dim RecFileWr As StreamWriter
        Dim IndexT As Integer
        Dim IndexM As Integer

        ' Block = 0: Interne Speicherung. Hier sind auch der Modulstatus und die Moduladressen von Interesse
        ' Block = 1: Externe Speicherung nur mit den Moduldaten die auch auf der CS2 zu sehen sind

        If Block = 0 Then

            ' Dateipfad bestimmen fuer internes Speichern

            UserPfad = "Inifiles/"

            ' Ordner erstellen wenn nicht existent

            If System.IO.Directory.Exists("Inifiles") = False Then

                System.IO.Directory.CreateDirectory("Inifiles")

            End If

            ' Loesche Datei falls schon vorhanden

            If File.Exists(UserPfad & "mbcan_db.txt") Then
                File.Delete(UserPfad & "mbcan_db.txt")
            End If

            RecFile = New FileStream(UserPfad & "mbcan_db.txt", FileMode.Create)
            RecFileWr = New StreamWriter(RecFile)

            RecFileWr.WriteLine(DBVersion)

        Else

            ' Dateipfad bestimmen fuer externes Speichern

            UserPfad = Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments)

            UserPfad &= "/MBCAN/Datenbank/"

            ' Ordner erstellen wenn nicht existent

            If System.IO.Directory.Exists(UserPfad) = False Then

                System.IO.Directory.CreateDirectory(UserPfad)

            End If

            ' Loesche Datei falls schon vorhanden

            If File.Exists(UserPfad & "mbcan_db_" & DBVersion & ".txt") Then
                File.Delete(UserPfad & "mbcan_db_" & DBVersion & ".txt")
            End If

            RecFile = New FileStream(UserPfad & "mbcan_db_" & DBVersion & ".txt", FileMode.Create)
            RecFileWr = New StreamWriter(RecFile)

        End If

        ' Schreibe Moduldaten in die Datei

        For IndexT = 1 To MaxTyp
            For IndexM = 1 To MBC.ModulTyp(IndexT).Count

                With MBC.ModulTyp(IndexT).Modul(IndexM)

                    ' Schreibe Moduldaten nur dann wenn das Modul nicht geloescht wurde

                    If .Status <> IsDeleted Then

                        If Block = 0 Then
                            RecFileWr.WriteLine("Status=" & FormHEX("0", 1, .Status) &
                                                 ";Typ=" & MBC.ModulTyp(IndexT).Bezeichner &
                                                 ";ArtNum=" & .ArtNum &
                                                 ";SerNr=" & .SerNr &
                                                 ";GUiD=" & .Guid &
                                                 ";SW=" & .SW &
                                                 ";HW=" & .HW &
                                                 ";MMAddr=" & frmMain.Register_Get_MMAddr(0, IndexT, IndexM) &
                                                 ";S88ModNr=" & frmMain.Register_Get_S88ModNr(IndexT, IndexM) &
                                                 ";Name=" & KVSafe(.Name))
                        Else
                            RecFileWr.WriteLine(MBC.ModulTyp(IndexT).Bezeichner & " " & .ArtNum & " " & .SerNr & " " & .Guid & " " & .SW & " " & .HW & " " & .Name)
                        End If

                    End If

                End With

            Next
        Next

        RecFileWr.Close()
        RecFile.Close()

    End Sub

    '**********************************************************************************************
    '*** Menue - Moduldatenbank neu anlegen                                                     ***
    '**********************************************************************************************

    Public Sub DB_Modul_Neu()

        If MsgBox(Sprachen(i_MSGBOX_MAIN_SYSDB, i_MSGBOX_MAIN_SYSDB_2), vbYesNo, Sprachen(i_MSGBOX_MAIN_SYSDB, i_MSGBOX_MAIN_SYSDB_1)) = vbYes Then

            'Timer fuer Neuanmeldung stoppen

            frmMain.tm_Neu.Stop()

            ' Statusleiste Searching zuruecksetzen

            frmMain.stp_Alive.Text = "Searching:  -/-"

            ' Loesche vorhandene Datenbank

            If File.Exists("Inifiles/mbcan_db.txt") Then
                File.Delete("Inifiles/mbcan_db.txt")
            End If

            ' Loesche vorhandene Datenstruktur

            MBC = New mbc_system_cl

            ' DB-Versionsnummer neu vergeben

            DBVersion = Microsoft.VisualBasic.DateString & Microsoft.VisualBasic.TimeOfDay
            DBVersion = Microsoft.VisualBasic.Mid(DBVersion, 1, 2) &
                        Microsoft.VisualBasic.Mid(DBVersion, 4, 2) &
                        Microsoft.VisualBasic.Mid(DBVersion, 9, 4) &
                        Microsoft.VisualBasic.Mid(DBVersion, 14, 2) &
                        Microsoft.VisualBasic.Mid(DBVersion, 17, 2)

            ' DB-Versionsnummer in CAN-Nachricht an die Module eintragen

            MsgDatenbankHIGH = Microsoft.VisualBasic.Left(MsgDatenbankHIGH, 27) & FormHEX("0", 2, Asc(Microsoft.VisualBasic.Mid(DBVersion, 1, 1))) _
                            & " " & FormHEX("0", 2, Asc(Microsoft.VisualBasic.Mid(DBVersion, 2, 1))) _
                            & " " & FormHEX("0", 2, Asc(Microsoft.VisualBasic.Mid(DBVersion, 3, 1))) _
                            & " " & FormHEX("0", 2, Asc(Microsoft.VisualBasic.Mid(DBVersion, 4, 1)))
            MsgDatenbankMID = Microsoft.VisualBasic.Left(MsgDatenbankMID, 27) & FormHEX("0", 2, Asc(Microsoft.VisualBasic.Mid(DBVersion, 5, 1))) _
                            & " " & FormHEX("0", 2, Asc(Microsoft.VisualBasic.Mid(DBVersion, 6, 1))) _
                            & " " & FormHEX("0", 2, Asc(Microsoft.VisualBasic.Mid(DBVersion, 7, 1))) _
                            & " " & FormHEX("0", 2, Asc(Microsoft.VisualBasic.Mid(DBVersion, 8, 1)))
            MsgDatenbankLOW = Microsoft.VisualBasic.Left(MsgDatenbankLOW, 27) & FormHEX("0", 2, Asc(Microsoft.VisualBasic.Mid(DBVersion, 9, 1))) _
                            & " " & FormHEX("0", 2, Asc(Microsoft.VisualBasic.Mid(DBVersion, 10, 1))) _
                            & " " & FormHEX("0", 2, Asc(Microsoft.VisualBasic.Mid(DBVersion, 11, 1))) _
                            & " " & FormHEX("0", 2, Asc(Microsoft.VisualBasic.Mid(DBVersion, 12, 1)))

            ' Decoder neu initialisieren

            frmMain.Modul_Init()

            ' Strukturbaum zuruecksetzen

            frmMain.tv_Strukturbaum.Nodes.Clear()

            ' Strukturbaum neu initialisieren

            frmMain.Strukturbaum_Init()

            ' Leeres Modul anzeigen

            frmMain.Register_Modul_Werte_Show(False)

            frmMain.Register_Modul_Bilder_Show(0)

            'Timer fuer Neuanmeldung starten

            frmMain.tm_Neu.Start()

        End If

    End Sub

    '**********************************************************************************************
    '*** Servo - Servowegdiagramm laden                                                         ***
    '**********************************************************************************************

    Public Sub DB_Servo_Laden()
        Dim UserPfad As String
        Dim RecFile As FileStream
        Dim RecFileRd As StreamReader
        Dim RdLine As String
        Dim Index As Integer

        ' Dateipfad bestimmen fuer externes Speichern

        UserPfad = Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments)

        UserPfad &= "\MBCAN\Servos\"

        ' Ordner erstellen wenn nicht existent

        If System.IO.Directory.Exists(UserPfad) = False Then

            System.IO.Directory.CreateDirectory(UserPfad)

        End If

        ' Bereite Dateidialogfeld vor

        frmServo.OpenFileDialogServo.InitialDirectory = UserPfad
        frmServo.OpenFileDialogServo.Filter = "txt files (*.txt)|*.txt|All files (*.*)|*.*"
        frmServo.OpenFileDialogServo.FilterIndex = 2

        ' Lese Daten aus der Datei aus

        If frmServo.OpenFileDialogServo.ShowDialog() = System.Windows.Forms.DialogResult.OK Then

            RecFile = New FileStream(frmServo.OpenFileDialogServo.FileName, FileMode.Open)
            RecFileRd = New StreamReader(RecFile)

            For Index = 0 To (frmServo.ServoSteps * 2 - 1)

                ' Pruefe Format
                ' 1: Dateilaenge muss (ServoSteps * 2) Zeilen umfassen

                Try
                    RdLine = RecFileRd.ReadLine
                Catch e As Exception
                    MsgBox(Sprachen(i_MSGBOX_MAIN_SYSDB, i_MSGBOX_MAIN_SYSDB_3) & Str(Index) & " " & Sprachen(i_MSGBOX_MAIN_SYSDB, i_MSGBOX_MAIN_SYSDB_4), vbOK, Sprachen(i_MSGBOX_MAIN_SYSDB, i_MSGBOX_MAIN_SYSDB_1))
                    RecFileRd.Close()
                    RecFile.Close()
                    frmServo.FLAG_Change = False
                    Exit Sub
                End Try

                ' 2: Index aus der Datei muss dem Index des ServoWeg-Arrays entsprechen

                If (FormHEX("0", 2, Index) <> Microsoft.VisualBasic.Left(RdLine, 2)) Then
                    MsgBox(Sprachen(i_MSGBOX_MAIN_SYSDB, i_MSGBOX_MAIN_SYSDB_5) & Str(Index) & " !", vbOK, Sprachen(i_MSGBOX_MAIN_SYSDB, i_MSGBOX_MAIN_SYSDB_1))
                    RecFileRd.Close()
                    RecFile.Close()
                    frmServo.FLAG_Change = False
                    Exit Sub
                End If

                ' 3: Die maximale Auslenkung der Servos darf ServoSteps betragen

                If Convert.ToInt32(Microsoft.VisualBasic.Right(RdLine, 2), 16) > frmServo.ServoSteps Then
                    MsgBox(Sprachen(i_MSGBOX_MAIN_SYSDB, i_MSGBOX_MAIN_SYSDB_6) & Str(Index) & " " & Sprachen(i_MSGBOX_MAIN_SYSDB, i_MSGBOX_MAIN_SYSDB_7) & Str(Convert.ToInt32(Microsoft.VisualBasic.Right(RdLine, 2), 16)) & " " & Sprachen(i_MSGBOX_MAIN_SYSDB, i_MSGBOX_MAIN_SYSDB_8), vbOK, Sprachen(i_MSGBOX_MAIN_SYSDB, i_MSGBOX_MAIN_SYSDB_1))
                    RecFileRd.Close()
                    RecFile.Close()
                    frmServo.FLAG_Change = False
                    Exit Sub
                End If

                ' Wenn alles ok: Datei = volle Design-Kurve, unveraendert in ServoWegDesign
                ' uebernehmen (keine verlustbehaftete Rundung) und zusaetzlich fuer die Anzeige
                ' auf den aktuellen Stellbereich skaliert in ServoWeg - eine frisch geladene
                ' Datei ersetzt jede vorherige manuelle Bearbeitung dieses Punkts

                frmServo.ServoWegDesign(Index) = Convert.ToInt32(Microsoft.VisualBasic.Right(RdLine, 2), 16)
                frmServo.ServoWeg(Index) = frmServo.DesignToReal(frmServo.ServoWegDesign(Index))
                frmServo.ServoWegEdited(Index) = False

            Next

            RecFileRd.Close()
            RecFile.Close()
            frmServo.FLAG_Change = True

        End If

    End Sub

    '**********************************************************************************************
    '*** Servo - Servowegdiagramm laden                                                         ***
    '**********************************************************************************************

    Public Sub DB_Servo_Speichern()
        Dim UserPfad As String
        Dim RecFile As FileStream
        Dim RecFileWr As StreamWriter
        Dim Index As Integer

        ' Dateipfad bestimmen fuer externes Speichern

        UserPfad = Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments)

        UserPfad &= "\MBCAN\Servos\"

        ' Ordner erstellen wenn nicht existent

        If System.IO.Directory.Exists(UserPfad) = False Then

            System.IO.Directory.CreateDirectory(UserPfad)

        End If

        ' Bereite Dateidialogfeld vor

        frmServo.SaveFileDialogServo.InitialDirectory = UserPfad
        frmServo.SaveFileDialogServo.Filter = "txt files (*.txt)|*.txt|All files (*.*)|*.*"
        frmServo.SaveFileDialogServo.FilterIndex = 2

        ' Schreibe Daten in die Datei

        If frmServo.SaveFileDialogServo.ShowDialog() = System.Windows.Forms.DialogResult.OK Then

            RecFile = New FileStream(frmServo.SaveFileDialogServo.FileName, FileMode.Create)
            RecFileWr = New StreamWriter(RecFile)

            ' Datei speichert immer die volle Design-Kurve (0...ServoSteps), unabhaengig vom
            ' aktuell konfigurierten Stellbereich - CurrentDesignValue() liefert fuer
            ' unbearbeitete Punkte die unveraenderte Original-Kurve, nur fuer tatsaechlich
            ' bearbeitete Punkte den neuen Wert (siehe dortigen Kommentar)

            For Index = 0 To (frmServo.ServoSteps * 2 - 1)
                RecFileWr.WriteLine(FormHEX("0", 2, Index) & ";" & FormHEX("0", 2, frmServo.CurrentDesignValue(Index)))
            Next

            RecFileWr.Close()
            RecFile.Close()

        End If

    End Sub

    '**********************************************************************************************
    '*** Servo - Mitinstallierte Beispiel-Fahrwege beim ersten Start bereitstellen              ***
    '**********************************************************************************************

    Public Sub DB_Servo_Beispiele_Bereitstellen()
        Dim UserPfad As String
        Dim QuellPfad As String
        Dim Datei As String
        Dim Ziel As String

        Try

            ' Zielordner bestimmen (identisch zu DB_Servo_Laden()/DB_Servo_Speichern())

            UserPfad = Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments)
            UserPfad &= "\MBCAN\Servos\"

            If System.IO.Directory.Exists(UserPfad) = False Then
                System.IO.Directory.CreateDirectory(UserPfad)
            End If

            ' Die mitinstallierten Beispieldateien liegen neben der EXE im Unterordner "Servos"
            ' (siehe Content-Eintraege in MBCAN.vbproj)

            QuellPfad = My.Application.Info.DirectoryPath & "\Servos\"

            If System.IO.Directory.Exists(QuellPfad) = False Then Exit Sub

            ' Nur fehlende Dateien ergaenzen - bereits vorhandene (z.B. vom Nutzer bearbeitete
            ' oder geloeschte) Dateien bleiben unangetastet

            For Each Datei In System.IO.Directory.GetFiles(QuellPfad, "*.txt")

                Ziel = UserPfad & System.IO.Path.GetFileName(Datei)

                If System.IO.File.Exists(Ziel) = False Then
                    System.IO.File.Copy(Datei, Ziel)
                End If

            Next

        Catch e As Exception

            ' Bereitstellung ist reine Komfortfunktion - ein Fehler hier darf den
            ' Programmstart nicht verhindern

        End Try

    End Sub

End Module
