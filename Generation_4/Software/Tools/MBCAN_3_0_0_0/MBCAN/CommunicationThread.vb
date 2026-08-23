'**************************************************************************************************
'*** Gemeinsame Thread-Lifecycle-Mechanik fuer die Empfangs-Threads von RS232/BT/TCP            ***
'*** Start/Stop-Sequenz war bisher 3x (RS232Oeffnen/Schliessen, BTOeffnen/Schliessen,           ***
'*** TCPOeffnen/Schliessen in frmMain.vb) identisch dupliziert. Die kanalspezifische Logik      ***
'*** (Menue-/Statusleisten-Umschaltung, Port-Oeffnen/Schliessen, Empfangs-Parsing) bleibt       ***
'*** bewusst in frmMain.vb, da sie sich je Kanal unterscheidet.                                 ***
'**************************************************************************************************

Module CommunicationThread

    ' Startet den Empfangs-Thread eines Kanals. isActive wird VOR dem Start gesetzt, damit die
    ' While-Schleife im Thread-Einstiegspunkt (receiveLoop) den Wert beim ersten Zugriff sicher sieht.

    Public Sub StartReceiveThread(ByRef thread As System.Threading.Thread, ByRef isActive As Boolean, receiveLoop As System.Threading.ThreadStart)

        thread = New System.Threading.Thread(receiveLoop)
        isActive = True
        thread.Start()

    End Sub

    ' Stoppt den Empfangs-Thread eines Kanals: Beendigung ueber das isActive-Flag anfordern, auf
    ' Zusammenfuehrung warten, bei Zeitueberschreitung hart abbrechen (bisherwiges Verhalten 1:1).

    Public Sub StopReceiveThread(ByRef isActive As Boolean, thread As System.Threading.Thread, timeoutMs As Integer)

        isActive = False
        thread.Join(timeoutMs)

        If thread.ThreadState = ThreadState.Running Then
            thread.Abort()
        End If

    End Sub

End Module
