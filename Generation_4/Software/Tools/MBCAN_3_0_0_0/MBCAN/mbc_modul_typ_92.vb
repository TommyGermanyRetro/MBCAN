Imports System.Runtime.InteropServices

Public Class mbc_modul_typ_92_cl

    '**********************************************************************************************
    '*** Oeffentliche Attribute                                                                 ***
    '**********************************************************************************************

    ' Beinhaltet Typdaten

    Inherits mbc_modul_typdaten_cl

    ' Liste mit Modulen gleichen Typs

    Public Modul = New Object

    '**********************************************************************************************
    '*** Modulverwaltung                                                                        ***
    '**********************************************************************************************
    Sub New()
        MyBase.New("mbc-92", My.Resources.img_mbc_92, True, "24.07.20", "1.00", New Integer() {1, 2, 3, 4, 5, 6, 7, 8})
        Modul = New Collection
    End Sub

    Sub Add(Name As String)
        Dim inst As New mbc_modul_92_cl
        Modul.add(inst, Name)
        Count += 1
    End Sub

    Sub Clear()
        Modul = New Collection
        Count = 0
    End Sub

End Class
