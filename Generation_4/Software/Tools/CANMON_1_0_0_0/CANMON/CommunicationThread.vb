'**************************************************************************************************
'*** Gemeinsame Thread-Lifecycle-Mechanik fuer den Empfangs-Thread der TCP-Verbindung           ***
'*** 1:1 aus dem MBCAN-Parametriercenter uebernommen (dort: CommunicationThread.vb, dort        ***
'*** gemeinsame Basis fuer RS232/BT/TCP - hier nur fuer TCP gebraucht).                         ***
'**************************************************************************************************

Module CommunicationThread

    ' Startet den Empfangs-Thread. isActive wird VOR dem Start gesetzt, damit die While-Schleife
    ' im Thread-Einstiegspunkt (receiveLoop) den Wert beim ersten Zugriff sicher sieht.

    Public Sub StartReceiveThread(ByRef thread As System.Threading.Thread, ByRef isActive As Boolean, receiveLoop As System.Threading.ThreadStart)

        thread = New System.Threading.Thread(receiveLoop)
        isActive = True
        thread.Start()

    End Sub

    ' Stoppt den Empfangs-Thread: Beendigung ueber das isActive-Flag anfordern, auf Zusammenfuehrung
    ' warten, bei Zeitueberschreitung hart abbrechen.

    Public Sub StopReceiveThread(ByRef isActive As Boolean, thread As System.Threading.Thread, timeoutMs As Integer)

        isActive = False
        thread.Join(timeoutMs)

        If thread.ThreadState = ThreadState.Running Then
            thread.Abort()
        End If

    End Sub

End Module
