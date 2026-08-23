Imports System.Runtime.InteropServices

Public Class mbc_modul_typ_82_cl

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
        MyBase.New("mbc-82", My.Resources.img_mbc_82, False, "20.11.01", "1.00", New Integer() {6, 15})
        Modul = New Collection
    End Sub

    Sub Add(Name As String)
        Dim inst As New mbc_modul_82_cl
        Modul.add(inst, Name)
        Count += 1
    End Sub

    Sub Clear()
        Modul = New Collection
        Count = 0
    End Sub

End Class
