'**************************************************************************************************
'*** CAN-Nachrichten - Dekodierung                                                              ***
'*** Deutet ein 13-Byte-CAN-Telegramm (Maerklin-CS2-Format ueber MBCAN) in zwei Anzeigezeilen   ***
'*** um: Rohdaten/Header in Zeile 1, eine lesbare Beschreibung (soweit bekannt) in Zeile 2. 1:1 ***
'*** aus dem MBCAN-Parametriercenter uebernommen (dort: frmMain.ComProtocol.vb/definitionen.vb  ***
'*** /utils.vb).                                                                                ***
'**************************************************************************************************

Module CanDecode

    '**********************************************************************************************
    '*** Nachrichtentabellen (Index = CAN-ID)                                                   ***
    '**********************************************************************************************

    Public NachrichtenMax As Integer = &H44

    Public Nachrichten As String() = {"Befehl:  System-Befehl Loc-ID: ",
                                      "Antwort: System-Befehl Loc-ID: ",
                                      "Befehl:  Lok Discovery ",
                                      "Antwort: Lok Discovery ",
                                      "Befehl:  MFX Bind ",
                                      "Antwort: MFX Bind ",
                                      "Befehl:  MFX Verify ",
                                      "Antwort: MFX Verify ",
                                      "Befehl:  Lok Speed ",
                                      "Antwort: Lok Speed ",
                                      "Befehl:  Lok Richtung ",
                                      "Antwort: Lok Richtung ",
                                      "Befehl:  Lok Funktion ",
                                      "Antwort: Lok Funktion ",
                                      "Befehl:  Read Config ",
                                      "Antwort: Read Config ",
                                      "Befehl:  Write Config ",
                                      "Antwort: Write Config ",
                                      "Befehl:  unknown          ",
                                      "Antwort: unknown          ",
                                      "Befehl:  unknown          ",
                                      "Antwort: unknown          ",
                                      "Befehl:  ",
                                      "Antwort: ",
                                      "Befehl:  Zubehör Konfig ",
                                      "Antwort: Zubehör Konfig ",
                                      "Befehl:  unknown          ",
                                      "Antwort: unknown          ",
                                      "Befehl:  unknown          ",
                                      "Antwort: unknown          ",
                                      "Befehl:  unknown          ",
                                      "Antwort: unknown          ",
                                      "Befehl:  s88 Polling ",
                                      "Antwort: s88 Polling ",
                                      "Befehl:  s88 Event ",
                                      "Antwort: s88 Event ",
                                      "Befehl:  SX1 Event ",
                                      "Antwort: SX1 Event ",
                                      "Befehl:  unknown          ",
                                      "Antwort: unknown          ",
                                      "Befehl:  unknown          ",
                                      "Antwort: unknown          ",
                                      "Befehl:  unknown          ",
                                      "Antwort: unknown          ",
                                      "Befehl:  unknown          ",
                                      "Antwort: unknown          ",
                                      "Befehl:  unknown          ",
                                      "Antwort: unknown          ",
                                      "Befehl:  Ping-Anfrage ",
                                      "Antwort: Ping-Anfrage ",
                                      "Befehl:  Uodate-Anfrage ",
                                      "Antwort: Update-Anfrage ",
                                      "Befehl:  Read Config Data ",
                                      "Antwort: Read Config Data ",
                                      "Befehl:  Bootloader CAN ",
                                      "Antwort: Bootloader CAN ",
                                      "Befehl:  Bootloader GLEIS ",
                                      "Antwort: Bootloader GLEIS ",
                                      "Befehl:  Statusdaten Konf ",
                                      "Antwort: Statusdaten Konf ",
                                      "Befehl:  Konf Daten ",
                                      "Antwort: Konf Daten ",
                                      "Befehl:  Konf Datastream ",
                                      "Antwort: Konf Datastream "}

    Public NachrichtenSystemBefehleMax As Integer = &HD

    Public NachrichtenSystemBefehle As String() = {"Cmd: System Stopp",
                                                   "Cmd: System Go",
                                                   "Cmd: System Halt",
                                                   "Cmd: Lok Nothalt",
                                                   "Cmd: Lok Zyklus beenden",
                                                   "Cmd: Lok Datenprotokoll",
                                                   "Cmd: Schaltzeit Zubehör",
                                                   "Cmd: Fast Read SID",
                                                   "Cmd: Gleisprotokoll",
                                                   "Cmd: MFX Neuanmeldezähler",
                                                   "Cmd: System Überlast",
                                                   "Cmd: System Status",
                                                   "Cmd: Gerätekennung"}

    Public NachrichtenSystemBefehle0x30 As String = "Cmd: MFX Seek"
    Public NachrichtenSystemBefehle0x80 As String = "Cmd: System Reset"

    Public NachrichtenMBCANMax = &H15

    Public NachrichtenMBCAN As String() = {"PC_DB_H", "PC_DB_M", "PC_DB_L", "PC_KENNER", "PC_NEU", "PC_NEU_DATA",
                                            "MD_NEU_DATA", "PC_RESET", "PC_MD_DEL", "PC_ALIVE", "MD_ALIVE", "PC_ARRAY",
                                            "MD_ARRAY", "PC_ARRAY_DATA", "MD_ARRAY_DATA", "PC_UPGRADE", "MD_UPGRADE",
                                            "PC_UPGRADE_DATA", "MD_UPGRADE_DATA", "PC_BOOT", "MD_S88", "MD_ZYKLUS"}

    '**********************************************************************************************
    '*** Byte-Offsets innerhalb des 13-Byte-Telegramms, als Hex-Text mit Trennzeichen           ***
    '*** (3 Zeichen je Byte: "xx ")                                                             ***
    '**********************************************************************************************

    Public MsgPRIO = 1
    Public MsgID = 4
    Public MsgHashH = 7
    Public MsgHashL = 10
    Public MsgDLC = 13
    Public MsgD0 = 16
    Public MsgD1 = 19
    Public MsgD2 = 22
    Public MsgD3 = 25
    Public MsgD4 = 28
    Public MsgD5 = 31
    Public MsgD6 = 34
    Public MsgD7 = 37

    '**********************************************************************************************
    '*** HEX-Werte formatieren                                                                  ***
    '*** Lead = "0" oder "3" je nachdem, was das Programm erwartet                              ***
    '*** Ziffer = Anzahl der Ziffern des HEX-String ausgenommen des "Lead"                      ***
    '*** Wert = in einen HEX-String zu wandelnder Wert                                          ***
    '**********************************************************************************************

    Public Function FormHEX(Lead As String, Ziffer As Integer, Wert As Integer) As String
        Dim h As String

        h = Hex(Wert)

        If Ziffer = 2 Then
            If Len(h) = 1 Then
                h = Lead & h
            End If
        End If

        If Ziffer = 3 Then
            If Len(h) = 1 Then
                h = Lead & "0" & h
            ElseIf Len(h) = 2 Then
                h = Lead & h
            End If
        End If

        FormHEX = h

    End Function

    '**********************************************************************************************
    '*** CAN-Telegramm in zwei Anzeigezeilen zerlegen (Rohdaten/Header, lesbare Beschreibung)   ***
    '**********************************************************************************************

    Public Sub BuildMsgMonitor(ByVal Msg As String, ByRef MsgLine1 As String, ByRef MsgLine2 As String)
        Dim ID As String
        Dim PRIO As String
        Dim HASH As String
        Dim DLC As String
        Dim Dx As String
        Dim Speed As Double
        Dim D0, D1, D2, D3, D4, D5, D6, D7 As String

        PRIO = Microsoft.VisualBasic.Mid(Msg, MsgPRIO, 2)
        ID = Microsoft.VisualBasic.Mid(Msg, MsgID, 2)
        HASH = Microsoft.VisualBasic.Mid(Msg, MsgHashH, 2) & Microsoft.VisualBasic.Mid(Msg, MsgHashL, 2)
        DLC = Microsoft.VisualBasic.Mid(Msg, MsgDLC + 1, 1)
        D0 = Microsoft.VisualBasic.Mid(Msg, MsgD0, 2)
        D1 = Microsoft.VisualBasic.Mid(Msg, MsgD1, 2)
        D2 = Microsoft.VisualBasic.Mid(Msg, MsgD2, 2)
        D3 = Microsoft.VisualBasic.Mid(Msg, MsgD3, 2)
        D4 = Microsoft.VisualBasic.Mid(Msg, MsgD4, 2)
        D5 = Microsoft.VisualBasic.Mid(Msg, MsgD5, 2)
        D6 = Microsoft.VisualBasic.Mid(Msg, MsgD6, 2)
        D7 = Microsoft.VisualBasic.Mid(Msg, MsgD7, 2)

        If Val("&H" & DLC) = 0 Then
            Dx = " -"
        Else
            Dx = Microsoft.VisualBasic.Mid(Msg, 15, 3 + 3 * (Val("&H" & DLC) - 1))
        End If

        MsgLine1 = "PRIO: " & PRIO & " ID: " & ID & " HASH: " & HASH & " DLC: " & DLC & " DATEN:" & Dx & vbCrLf

        If Val("&H" & ID) < NachrichtenMax Then

            Try
                MsgLine2 &= "             > " & Nachrichten(Val("&H" & ID))
            Catch ex As Exception
                MsgLine2 &= "             > Unknown Command"
            End Try

            ' Systembefehle

            If Val("&H" & ID) = 0 Or Val("&H" & ID) = 1 Then

                MsgLine2 &= "0x" & D0 & D1 & D2 & D3 & " "

                If Val("&H" & D4) < NachrichtenSystemBefehleMax Then
                    MsgLine2 &= NachrichtenSystemBefehle(Val("&H" & D4))
                Else
                    If Val("&H" & D4) = &H30 Then MsgLine2 &= NachrichtenSystemBefehle0x30
                    If Val("&H" & D4) = &H80 Then MsgLine2 &= NachrichtenSystemBefehle0x80
                End If

                If Val("&H" & D4) = &H5 Or
                   Val("&H" & D4) = &H8 Or
                   Val("&H" & D4) = &HA Then
                    MsgLine2 &= " W: 0x" & D5
                End If

                If Val("&H" & D4) = &H6 Or
                   Val("&H" & D4) = &H7 Or
                   Val("&H" & D4) = &H9 Then
                    MsgLine2 &= " W: " & "0x" & D5 & D6
                End If

                If Val("&H" & D4) = &HB Then
                    If DLC = "07" Then MsgLine2 &= " W: " & D5
                    If DLC = "08" Then MsgLine2 &= " W: " & "0x" & D5 & D6
                End If

                If Val("&H" & D4) = &HC Then
                    If DLC = "07" Then MsgLine2 &= " W: " & "0x" & D5 & D6
                End If

            End If

            ' Lok Discovery

            If Val("&H" & ID) = 2 Or Val("&H" & ID) = 3 Then

                If DLC = "1" Then MsgLine2 &= "Protokoll-Kennung: 0x" & D0

                If DLC = "5" Then
                    MsgLine2 &= "Loc-ID: 0x" & D0 & D1 & D2 & D3 & " "
                    MsgLine2 &= "Protokoll-Kennung: 0x" & D4
                End If

                If DLC = "6" Then
                    MsgLine2 &= "Loc-ID: 0x" & D0 & D1 & D2 & D3 & " "
                    MsgLine2 &= "Range: 0x" & D4 & " "
                    MsgLine2 &= "ASK: 0x" & D5
                End If

            End If

            ' MFX Bind

            If Val("&H" & ID) = 4 Or Val("&H" & ID) = 5 Then
                MsgLine2 &= "MFX-UID: 0x" & D0 & D1 & D2 & D3 & " "
                MsgLine2 &= "MFX-SID: 0x" & D4 & D5

            End If

            ' MFX Verify

            If Val("&H" & ID) = 6 Or Val("&H" & ID) = 7 Then
                MsgLine2 &= "MFX-UID: 0x" & D0 & D1 & D2 & D3 & " "
                MsgLine2 &= "MFX-SID: 0x" & D4 & D5
                If DLC = "07" Then
                    MsgLine2 &= " ASK: " & D6
                End If
            End If

            ' Lok Geschwindigkeit

            If Val("&H" & ID) = 8 Or Val("&H" & ID) = 9 Then
                If D2 = "00" Then
                    MsgLine2 &= "MM-Lok Adresse: " & Val("&H" & D3)
                End If
                If D2 = "08" Then
                    MsgLine2 &= "SX-Lok Adresse: " & Val("&H" & D3)
                End If
                If D2 = "40" Then
                    MsgLine2 &= "MFX-Lok Adresse: " & Val("&H" & D3)
                End If
                If D2 = "C0" Then
                    MsgLine2 &= "DCC-Lok Adresse: " & Val("&H" & D3)
                End If
                If DLC = "6" Then
                    Speed = Math.Round((Val("&H" & D4) * 256 + Val("&H" & D5)) / 10, 1)
                    MsgLine2 &= " Geschwindigkeit: " & CStr(Speed) & "%"
                End If
            End If

            ' Lok Richtung

            If Val("&H" & ID) = 10 Or Val("&H" & ID) = 11 Then
                If D2 = "00" Then
                    MsgLine2 &= "MM-Lok Adresse: " & Val("&H" & D3)
                End If
                If D2 = "08" Then
                    MsgLine2 &= "SX-Lok Adresse: " & Val("&H" & D3)
                End If
                If D2 = "40" Then
                    MsgLine2 &= "MFX-Lok Adresse: " & Val("&H" & D3)
                End If
                If D2 = "C0" Then
                    MsgLine2 &= "DCC-Lok Adresse: " & Val("&H" & D3)
                End If
                If DLC = "5" Then
                    MsgLine2 &= " Richtung: "
                    If D4 = "01" Then
                        MsgLine2 &= "vorwärts"
                    Else
                        If D4 = "02" Then
                            MsgLine2 &= "rückwärts"
                        Else
                            If D4 = "03" Then
                                MsgLine2 &= "umschalten"
                            Else
                                MsgLine2 &= "bleibt"
                            End If
                        End If
                    End If
                End If
            End If

            ' Lok Funktion

            If Val("&H" & ID) = 12 Or Val("&H" & ID) = 13 Then
                If D2 = "00" Then
                    MsgLine2 &= "MM-Lok Adresse: " & Val("&H" & D3)
                End If
                If D2 = "08" Then
                    MsgLine2 &= "SX-Lok Adresse: " & Val("&H" & D3)
                End If
                If D2 = "40" Then
                    MsgLine2 &= "MFX-Lok Adresse: " & Val("&H" & D3)
                End If
                If D2 = "C0" Then
                    MsgLine2 &= "DCC-Lok Adresse: " & Val("&H" & D3)
                End If
                MsgLine2 &= " Funktion: " & Val("&H" & D4)
                If DLC = "6" Or DLC = "8" Then MsgLine2 &= " W: 0x" & D5
                If DLC = "8" Then MsgLine2 &= " SW: 0x" & D6 & D7
            End If

            ' Read Config

            If Val("&H" & ID) = 14 Or Val("&H" & ID) = 15 Then
                MsgLine2 &= "Loc-ID: 0x" & D0 & D1 & D2 & D3 & " CV: 0x" & D4 & D5
                If DLC = "7" Then MsgLine2 &= " W: 0x" & D6
            End If

            ' Write Config

            If Val("&H" & ID) = 16 Or Val("&H" & ID) = 17 Then
                MsgLine2 &= "Loc-ID: 0x" & D0 & D1 & D2 & D3 & " CV: 0x" & D4 & D5 & " W1: 0x" & D6 & " W1: 0x" & D7
            End If

            ' Zubehoer schalten

            If Val("&H" & ID) = &H16 Or Val("&H" & ID) = &H17 Then
                If D2 <> "18" Then
                    MsgLine2 &= "Zubehör Schalten Adr: 0x" & D0 & D1 & D2 & D3 & " Pos: " & Val("&H" & D4) & " Str: " & Val("&H" & D5)
                    If DLC = "8" Then MsgLine2 &= " SW: " & Val("&H" & D6 & D7)
                Else
                    MsgLine2 &= "MBCAN Sub-Cmd: "
                    If Val("&H" & D3) <= NachrichtenMBCANMax Then MsgLine2 &= NachrichtenMBCAN(Val("&H" & D3))
                End If

            End If

            ' S88 Polling

            If Val("&H" & ID) = &H20 Or Val("&H" & ID) = &H21 Then
                MsgLine2 &= "GUiD: 0x" & D0 & D1 & D2 & D3 & " "
                If DLC = "5" Then MsgLine2 &= "Mod-Anzahl: " & Val("&H" & D4)
                If DLC = "7" Then MsgLine2 &= "Mod-Nr: " & Val("&H" & D4) & " Zustand: " & Val("&H" & D5)

            End If

            ' Rueckmelde Event

            If Val("&H" & ID) = &H22 Or Val("&H" & ID) = &H23 Then
                MsgLine2 &= "GK: 0x" & D0 & D1 & " KK: 0x" & D2 & D3 & " "
                If DLC = "5" Then MsgLine2 &= "Parameter: 0x" & D4
                If DLC = "8" Then MsgLine2 &= "Z(a): " & Val("&H" & D4) & " Z(n): " & Val("&H" & D5) & " Zeit: " & Val("&H" & D6 & D7) & " ms"
            End If

            ' Ping

            If Val("&H" & ID) = &H30 Or Val("&H" & ID) = &H31 Then
                If DLC <> "0" Then MsgLine2 &= "GUiD: 0x" & D0 & D1 & D2 & D3 & " SW: " & Val("&H" & D4) & "." & Val("&H" & D5) & " GK: 0x" & D6 & D7
            End If

            MsgLine2 &= vbCrLf

        Else

            MsgLine2 &= "               Nicht implementiert" & vbCrLf

        End If

    End Sub

End Module
