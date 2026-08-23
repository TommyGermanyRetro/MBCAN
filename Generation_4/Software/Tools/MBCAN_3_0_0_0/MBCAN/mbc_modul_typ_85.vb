Imports System.Runtime.InteropServices

Public Class mbc_modul_typ_85_cl

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
        MyBase.New("mbc-85", My.Resources.img_mbc_85, False, "22.07.31", "1.00", New Integer() {1, 2, 3, 4, 9, 10, 11, 12, 13, 17, 18, 19})
        Modul = New Collection
    End Sub

    Sub Add(Name As String)
        Dim inst As New mbc_modul_85_cl
        Modul.add(inst, Name)
        Count += 1
    End Sub

    Sub Clear()
        Modul = New Collection
        Count = 0
    End Sub

End Class
