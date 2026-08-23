Public Class mbc_system_cl

    Public ModulTyp As New Object

    ' Klassen erstellen und initialisieren
    Sub New()
        ModulTyp = New Collection
        ModulTyp.add(New mbc_modul_typ_80_cl, "mbc-80")
        ModulTyp.add(New mbc_modul_typ_81_cl, "mbc-81")
        ModulTyp.add(New mbc_modul_typ_82_cl, "mbc-82")
        ModulTyp.add(New mbc_modul_typ_83_cl, "mbc-83")
        ModulTyp.add(New mbc_modul_typ_84_cl, "mbc-84")
        ModulTyp.add(New mbc_modul_typ_85_cl, "mbc-85")
        ModulTyp.add(New mbc_modul_typ_86_cl, "mbc-86")
        ModulTyp.add(New mbc_modul_typ_87_cl, "mbc-87")
        ModulTyp.add(New mbc_modul_typ_88_cl, "mbc-88")
        ModulTyp.add(New mbc_modul_typ_89_cl, "mbc-89")
        ModulTyp.add(New mbc_modul_typ_90_cl, "mbc-90")
        ModulTyp.add(New mbc_modul_typ_91_cl, "mbc-91")
        ModulTyp.add(New mbc_modul_typ_92_cl, "mbc-92")
        ModulTyp.add(New mbc_modul_typ_93_cl, "mbc-93")
    End Sub

End Class
