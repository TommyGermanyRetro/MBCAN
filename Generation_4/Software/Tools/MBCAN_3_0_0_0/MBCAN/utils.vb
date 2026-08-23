'**************************************************************************************************
'*** Allgemeine Funktionen und Init-Routinen zum Programm                                       ***
'**************************************************************************************************

Module utils

    '**********************************************************************************************
    '*** HEX-Werte formatieren                                                                  ***
    '*** Lead = "0" oder "3" je nachdem, was das Programm erwartet                              ***
    '*** Ziffer = Anzahl der Ziffern des HEX-String ausgenommen des "Lead"                      ***
    '*** Wert = in einen HEX-String zu wandelnder Wert                                          ***
    '**********************************************************************************************

    Public Function FormHEX(Lead As String, Ziffer As Integer, Wert As Integer) As String
        Dim h As String

        ' Wandle Zahl in HEX um

        h = Hex(Wert)

        ' Wird ein zweistelliges Ergebnis erwartet aber der HEX-Wert ist nur einstellig dann
        ' fuege den Lead vorne an

        If Ziffer = 2 Then
            If Len(h) = 1 Then
                h = Lead & h
            End If
        End If

        ' Wird ein dreistelliges Ergebnis erwartet aber der HEX-Wert ist nur einstellig dann
        ' fuege den Lead und eine 0 vorne an
        ' Wird ein dreistelliges Ergebnis erwartet aber der HEX-Wert ist nur zweistellig dann
        ' fuege den Lead vorne an

        If Ziffer = 3 Then
            If Len(h) = 1 Then
                h = Lead & "0" & h
            ElseIf Len(h) = 2 Then
                h = Lead & h
            End If
        End If

        ' Wird ein vierstelliges Ergebnis erwartet aber der HEX-Wert ist nur einstellig dann
        ' fuege den Lead und 00 vorne an
        ' Wird ein vierstelliges Ergebnis erwartet aber der HEX-Wert ist nur zweistellig dann
        ' fuege den Lead und eine 0 vorne an
        ' Wird ein vierstelliges Ergebnis erwartet aber der HEX-Wert ist nur dreistellig dann
        ' fuege den Lead vorne an

        If Ziffer = 4 Then
            If Len(h) = 1 Then
                h = Lead & "00" & h
            ElseIf Len(h) = 2 Then
                h = Lead & "0" & h
            ElseIf Len(h) = 3 Then
                h = Lead & h
            End If
        End If

        FormHEX = h

    End Function

    '**********************************************************************************************
    '*** Timer für Delay                                                                        ***
    '**********************************************************************************************

    Public Sub Delay(ByVal dblSecs As Double)

        Const OneSec As Double = 1.0# / (1440.0# * 60.0#)

        Dim dblWaitTil As Date

        Now.AddSeconds(OneSec)

        dblWaitTil = Now.AddSeconds(OneSec).AddSeconds(dblSecs)

        Do Until Now > dblWaitTil
            Application.DoEvents() ' Allow windows messages to be processed
        Loop

    End Sub

    '**********************************************************************************************
    '*** Node im Treeview suchen mit der Eigenschaft "Fullpath"                                 ***
    '**********************************************************************************************

    Public Function FindNode(tncoll As TreeNodeCollection, strText As [String]) As TreeNode
        Dim tnFound As TreeNode
        For Each tnCurr As TreeNode In tncoll
            If tnCurr.FullPath = strText Then
                Return (tnCurr)
            End If
            tnFound = FindNode(tnCurr.Nodes, strText)
            If tnFound IsNot Nothing Then
                Return (tnFound)
            End If
        Next
        Return (Nothing)
    End Function

    '**********************************************************************************************
    '*** Schluessel=Wert-Zeile(n) parsen (mehrere Paare je Zeile durch ";" getrennt)            ***
    '*** Basis fuer das selbstmigrierende Speicherformat in datenbanken.vb                      ***
    '**********************************************************************************************

    Public Function KVParse(line As String) As Dictionary(Of String, String)
        Dim dict As New Dictionary(Of String, String)
        For Each part As String In line.Split(";"c)
            Dim eq As Integer = part.IndexOf("="c)
            If eq > 0 Then
                dict(part.Substring(0, eq)) = part.Substring(eq + 1)
            End If
        Next
        Return dict
    End Function

    '**********************************************************************************************
    '*** Wert aus einem KVParse-Dictionary lesen, mit Vorgabewert falls Schluessel fehlt        ***
    '**********************************************************************************************

    Public Function KVGet(dict As Dictionary(Of String, String), key As String,
                           Optional defaultValue As String = "") As String
        If dict.ContainsKey(key) Then Return dict(key)
        Return defaultValue
    End Function

    '**********************************************************************************************
    '*** Wert fuer die Verwendung in einer KV-Zeile absichern (reservierte Trennzeichen         ***
    '*** ersetzen)                                                                              ***
    '**********************************************************************************************

    Public Function KVSafe(value As String) As String
        Return value.Replace(";", "_").Replace("=", "_")
    End Function

    '**********************************************************************************************
    '*** CRC32 (Poly 0xEDB88320, wie zlib/PNG) ueber einen Datenblock berechnen                 ***
    '*** Muss exakt zum firmwareseitigen MbcOta::crc32Update (MbcOta.cpp in MBCANLib) passen -  ***
    '*** das Modul rechnet beim Anwenden eines OTA-Updates denselben Wert nach und vergleicht   ***
    '*** ihn mit dem im Header uebertragenen Wert.                                              ***
    '**********************************************************************************************

    Public Function Crc32(data As Byte(), length As Integer) As UInteger
        Dim crc As UInteger = &HFFFFFFFFUI

        For index As Integer = 0 To length - 1
            crc = crc Xor CUInt(data(index))

            For bit As Integer = 0 To 7
                If (crc And 1UI) <> 0UI Then
                    crc = (crc >> 1) Xor &HEDB88320UI
                Else
                    crc = crc >> 1
                End If
            Next
        Next

        Return crc Xor &HFFFFFFFFUI
    End Function

    '**********************************************************************************************
    '*** Popup-Fenster innerhalb der Grenzen seines Besitzerfensters halten                     ***
    '*** In Load jedes Popups per AddHandler Me.LocationChanged, AddressOf ClampInnerhalbEltern ***
    '*** verdrahten, damit es beim Verschieben nicht ueber das Hauptfenster hinaus gezogen wird.***
    '*** Ohne Owner (z.B. der Splash-Screen vor dem Start von frmMain) bleibt dies wirkungslos. ***
    '**********************************************************************************************

    Public Sub ClampInnerhalbEltern(sender As Object, e As EventArgs)

        Dim frm As Form = DirectCast(sender, Form)

        If frm.Owner Is Nothing Then Exit Sub

        Dim grenzen As Rectangle = frm.Owner.Bounds
        Dim neuX As Integer = frm.Left
        Dim neuY As Integer = frm.Top

        If neuX + frm.Width > grenzen.Right Then neuX = grenzen.Right - frm.Width
        If neuY + frm.Height > grenzen.Bottom Then neuY = grenzen.Bottom - frm.Height
        If neuX < grenzen.Left Then neuX = grenzen.Left
        If neuY < grenzen.Top Then neuY = grenzen.Top

        If neuX <> frm.Left Or neuY <> frm.Top Then
            frm.Location = New Point(neuX, neuY)
        End If

    End Sub

End Module
