'**************************************************************************************************
'*** Gemeinsame Schnittstelle aller Modul-Instanzklassen (mbc_modul_80_cl ... mbc_modul_93_cl)  ***
'*** frmMain spricht das jeweils aktive Modul ausschliesslich ueber                             ***
'*** MBC.ModulTyp(AktTyp).Modul(AktNr) an (Late Binding, da diese Collections als Object        ***
'*** typisiert sind). Dieses Interface schreibt den bisher nur durch Konvention eingehaltenen   ***
'*** Methodenvertrag fest, damit der Compiler eine fehlende oder abweichende Signatur in einer  ***
'*** der 14 Klassen sofort meldet.                                                              ***
'**************************************************************************************************

Public Interface IMbcModul

    ' Registerkarte aktivieren/befuellen

    Sub Tabpage_Aktivieren()
    Sub Tabpage_Werte_Show()

    ' Kommunikation mit dem Modul

    Sub Update()
    Sub Parameter_Fill()
    Sub Aktorstellung_Show()

    ' Magnetartikel-Adresse/-Stellung

    Function Get_MMAddr(index As Integer) As Object
    Sub Set_MMAddr(addr As Integer)
    Function Get_MMValue() As Object
    Sub Set_MMValue(index As Integer)

    ' s88-Rueckmeldeadresse/-Stellung

    Function Get_S88ModNr() As Object
    Sub Set_S88ModNr(nr As Integer)
    Function Get_S88Value() As Object
    Sub Set_S88Value(index As Integer)

End Interface
