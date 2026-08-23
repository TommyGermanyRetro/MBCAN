<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()>
Partial Class frmMain
    Inherits System.Windows.Forms.Form

    'Das Formular überschreibt den Löschvorgang, um die Komponentenliste zu bereinigen.
    <System.Diagnostics.DebuggerNonUserCode()>
    Protected Overrides Sub Dispose(ByVal disposing As Boolean)
        Try
            If disposing AndAlso components IsNot Nothing Then
                components.Dispose()
            End If
        Finally
            MyBase.Dispose(disposing)
        End Try
    End Sub

    'Wird vom Windows Form-Designer benötigt.
    Private components As System.ComponentModel.IContainer

    'Hinweis: Die folgende Prozedur ist für den Windows Form-Designer erforderlich.
    'Das Bearbeiten ist mit dem Windows Form-Designer möglich.  
    'Das Bearbeiten mit dem Code-Editor ist nicht möglich.
    <System.Diagnostics.DebuggerStepThrough()>
    Private Sub InitializeComponent()
        Me.components = New System.ComponentModel.Container()
        Dim resources As System.ComponentModel.ComponentResourceManager = New System.ComponentModel.ComponentResourceManager(GetType(frmMain))
        Me.ms_Menu = New System.Windows.Forms.MenuStrip()
        Me.ts_MenuSystem = New System.Windows.Forms.ToolStripMenuItem()
        Me.ts_MenuSystemNeu = New System.Windows.Forms.ToolStripMenuItem()
        Me.ts_MenuSystemSpeichern = New System.Windows.Forms.ToolStripMenuItem()
        Me.ts_MenuSystemAnzeigen = New System.Windows.Forms.ToolStripMenuItem()
        Me.ts_MenuSystemSprache = New System.Windows.Forms.ToolStripMenuItem()
        Me.ts_MenuSystemSprache_Deutsch = New System.Windows.Forms.ToolStripMenuItem()
        Me.ts_MenuSystemSprache_Französisch = New System.Windows.Forms.ToolStripMenuItem()
        Me.ts_MenuSystemSprache_Englisch = New System.Windows.Forms.ToolStripMenuItem()
        Me.ts_MenuSystemSchliessen = New System.Windows.Forms.ToolStripMenuItem()
        Me.ts_MenuVerbindung = New System.Windows.Forms.ToolStripMenuItem()
        Me.ts_MenuVerbindungEinstellen = New System.Windows.Forms.ToolStripMenuItem()
        Me.ts_MenuVerbindungRS232 = New System.Windows.Forms.ToolStripMenuItem()
        Me.ts_MenuVerbindungRS232Verbinden = New System.Windows.Forms.ToolStripMenuItem()
        Me.ts_MenuVerbindungRS232Trennen = New System.Windows.Forms.ToolStripMenuItem()
        Me.ts_MenuVerbindungBluetooth = New System.Windows.Forms.ToolStripMenuItem()
        Me.ts_MenuVerbindungBluetoothVerbinden = New System.Windows.Forms.ToolStripMenuItem()
        Me.ts_MenuVerbindungBluetoothTrennen = New System.Windows.Forms.ToolStripMenuItem()
        Me.ts_MenuVerbindungTCP = New System.Windows.Forms.ToolStripMenuItem()
        Me.ts_MenuVerbindungTCPVerbinden = New System.Windows.Forms.ToolStripMenuItem()
        Me.ts_MenuVerbindungTCPTrennen = New System.Windows.Forms.ToolStripMenuItem()
        Me.ts_MenuInfo = New System.Windows.Forms.ToolStripMenuItem()
        Me.ts_MenuInfoHilfe = New System.Windows.Forms.ToolStripMenuItem()
        Me.ts_MenuInfoBedienungsanleitung = New System.Windows.Forms.ToolStripMenuItem()
        Me.ts_MenuInfoCopyright = New System.Windows.Forms.ToolStripMenuItem()
        Me.sc_Main = New System.Windows.Forms.SplitContainer()
        Me.tv_Strukturbaum = New System.Windows.Forms.TreeView()
        Me.tab_Control = New System.Windows.Forms.TabControl()
        Me.tab_Modul = New System.Windows.Forms.TabPage()
        Me.btn_ControlUpdate = New System.Windows.Forms.Button()
        Me.btn_ControlVerwerfen = New System.Windows.Forms.Button()
        Me.btn_ControlUpgrade = New System.Windows.Forms.Button()
        Me.btn_ControlLoeschen = New System.Windows.Forms.Button()
        Me.btn_ControlReset = New System.Windows.Forms.Button()
        Me.btn_ControlRefresh = New System.Windows.Forms.Button()
        Me.tab_Module = New System.Windows.Forms.TabControl()
        Me.tab_mbc80 = New System.Windows.Forms.TabPage()
        Me.gb_mbc80_SW = New System.Windows.Forms.GroupBox()
        Me.nud_mbc80_SW2 = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc80_SWPunkt = New System.Windows.Forms.Label()
        Me.nud_mbc80_SW1 = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc80_SWBeschreibung = New System.Windows.Forms.TextBox()
        Me.lbl_mbc80_SWBezeichnung = New System.Windows.Forms.Label()
        Me.gb_mbc80_AK = New System.Windows.Forms.GroupBox()
        Me.lbl_mbc80_AK = New System.Windows.Forms.TextBox()
        Me.txt_mbc80_AK = New System.Windows.Forms.TextBox()
        Me.gb_mbc80_BT = New System.Windows.Forms.GroupBox()
        Me.txt_mbc80_Passwort = New System.Windows.Forms.TextBox()
        Me.lbl_mbc80_PasswortBeschreibung = New System.Windows.Forms.TextBox()
        Me.lbl_mbc80_BTNameBeschreibung = New System.Windows.Forms.TextBox()
        Me.txt_mbc80_Name = New System.Windows.Forms.TextBox()
        Me.lbl_mbc80_Name = New System.Windows.Forms.Label()
        Me.lbl_mbc80_Passwort = New System.Windows.Forms.Label()
        Me.tab_mbc81 = New System.Windows.Forms.TabPage()
        Me.lbl_mbc81_Nutzung_Erlaeuterung_3 = New System.Windows.Forms.Label()
        Me.lbl_mbc81_Nutzung_Erlaeuterung_2 = New System.Windows.Forms.Label()
        Me.lbl_mbc81_Nutzung_Erlaeuterung_1 = New System.Windows.Forms.Label()
        Me.lbl_mbc81_NutzungErlaeuterung = New System.Windows.Forms.Label()
        Me.lbl_mbc81_Nutzung = New System.Windows.Forms.Label()
        Me.cmb_mbc81_Nutzung = New System.Windows.Forms.ComboBox()
        Me.lbl_mbc81_Adresse2 = New System.Windows.Forms.Label()
        Me.txt_mbc81_Adresse2 = New System.Windows.Forms.TextBox()
        Me.txt_mbc81_Adresse1 = New System.Windows.Forms.TextBox()
        Me.lbl_mbc81_Adresse1 = New System.Windows.Forms.Label()
        Me.nud_mbc81_LocID = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc81_LocID = New System.Windows.Forms.Label()
        Me.tab_mbc82 = New System.Windows.Forms.TabPage()
        Me.gb_mbc82_Latenz = New System.Windows.Forms.GroupBox()
        Me.lbl_mbc82_LatenzBeschreibung_3 = New System.Windows.Forms.TextBox()
        Me.lbl_mbc82_LatenzBeschreibung_2 = New System.Windows.Forms.TextBox()
        Me.nud_mbc82_Latenz = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc82_LatenzBeschreibung_1 = New System.Windows.Forms.TextBox()
        Me.lbl_mbc82_LatenzBezeichnung = New System.Windows.Forms.Label()
        Me.gb_mbc82_GFP = New System.Windows.Forms.GroupBox()
        Me.lbl_mbc82_GFPBeschreibung_2 = New System.Windows.Forms.TextBox()
        Me.lbl_mbc82_GFPPunkt = New System.Windows.Forms.Label()
        Me.nud_mbc82_GFPSW2 = New System.Windows.Forms.NumericUpDown()
        Me.nud_mbc82_GFPSW1 = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc82_GFPBeschreibung_1 = New System.Windows.Forms.TextBox()
        Me.lbl_mbc82_GFPBezeichnung = New System.Windows.Forms.Label()
        Me.tab_mbc83 = New System.Windows.Forms.TabPage()
        Me.lbl_mbc83_Startup = New System.Windows.Forms.Label()
        Me.pic_mbc83_Startup = New System.Windows.Forms.PictureBox()
        Me.lbl_mbc83_Wechsel = New System.Windows.Forms.Label()
        Me.lbl_mbc83_Impuls = New System.Windows.Forms.Label()
        Me.pic_mbc83_Wechsel = New System.Windows.Forms.PictureBox()
        Me.pic_mbc83_Impuls = New System.Windows.Forms.PictureBox()
        Me.lbl_mbc83_ms = New System.Windows.Forms.Label()
        Me.lbl_mbc83_Meldezyklus = New System.Windows.Forms.Label()
        Me.lbl_mbc83_AutoAus = New System.Windows.Forms.Label()
        Me.nud_mbc83_Rueck = New System.Windows.Forms.NumericUpDown()
        Me.nud_mbc83_Aus = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc83_Dimmer = New System.Windows.Forms.Label()
        Me.nud_mbc83_Dimmer = New System.Windows.Forms.NumericUpDown()
        Me.gb_mbc83_CH4 = New System.Windows.Forms.GroupBox()
        Me.gb_mbc83_CH4_txt_Adresse = New System.Windows.Forms.TextBox()
        Me.gb_mbc83_CH4_lbl_Adresse = New System.Windows.Forms.Label()
        Me.gb_mbc83_CH4_lbl_Typ = New System.Windows.Forms.Label()
        Me.gb_mbc83_CH4_clb_Typ = New System.Windows.Forms.CheckedListBox()
        Me.gb_mbc83_CH4_nud_TIO = New System.Windows.Forms.NumericUpDown()
        Me.gb_mbc83_CH4_lbl_ON = New System.Windows.Forms.Label()
        Me.gb_mbc83_CH4_lbl_ms = New System.Windows.Forms.Label()
        Me.gb_mbc83_CH3 = New System.Windows.Forms.GroupBox()
        Me.gb_mbc83_CH3_txt_Adresse = New System.Windows.Forms.TextBox()
        Me.gb_mbc83_CH3_lbl_Adresse = New System.Windows.Forms.Label()
        Me.gb_mbc83_CH3_lbl_Typ = New System.Windows.Forms.Label()
        Me.gb_mbc83_CH3_clb_Typ = New System.Windows.Forms.CheckedListBox()
        Me.gb_mbc83_CH3_nud_TIO = New System.Windows.Forms.NumericUpDown()
        Me.gb_mbc83_CH3_lbl_ON = New System.Windows.Forms.Label()
        Me.gb_mbc83_CH3_lbl_ms = New System.Windows.Forms.Label()
        Me.gb_mbc83_CH2 = New System.Windows.Forms.GroupBox()
        Me.txt_mbc83_adr_2 = New System.Windows.Forms.TextBox()
        Me.gb_mbc83_CH2_lbl_Adresse = New System.Windows.Forms.Label()
        Me.gb_mbc83_CH2_lbl_Typ = New System.Windows.Forms.Label()
        Me.gb_mbc83_CH2_clb_Typ = New System.Windows.Forms.CheckedListBox()
        Me.gb_mbc83_CH2_nud_TIO = New System.Windows.Forms.NumericUpDown()
        Me.gb_mbc83_CH2_lbl_ON = New System.Windows.Forms.Label()
        Me.gb_mbc83_CH2_lbl_ms = New System.Windows.Forms.Label()
        Me.gb_mbc83_CH1 = New System.Windows.Forms.GroupBox()
        Me.gb_mbc83_CH1_txt_Adresse = New System.Windows.Forms.TextBox()
        Me.gb_mbc83_CH1_lbl_Adresse = New System.Windows.Forms.Label()
        Me.gb_mbc83_CH1_lbl_typ = New System.Windows.Forms.Label()
        Me.gb_mbc83_CH1_clb_Typ = New System.Windows.Forms.CheckedListBox()
        Me.gb_mbc83_CH1_nud_TIO = New System.Windows.Forms.NumericUpDown()
        Me.gb_mbc83_CH1_lbl_ON = New System.Windows.Forms.Label()
        Me.gb_mbc83_CH1_lbl_ms = New System.Windows.Forms.Label()
        Me.nud_mbc83_MMAddr = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc83_LocID = New System.Windows.Forms.Label()
        Me.tab_mbc84 = New System.Windows.Forms.TabPage()
        Me.nud_mbc84_Rueck = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc84_Hinweis = New System.Windows.Forms.Label()
        Me.gb_mbc84_CH4 = New System.Windows.Forms.GroupBox()
        Me.txt_mbc84_adr_4 = New System.Windows.Forms.TextBox()
        Me.nud_mbc84_Links_CH4 = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc84_adr_CH4 = New System.Windows.Forms.Label()
        Me.lbl_mbc84_Links_CH4 = New System.Windows.Forms.Label()
        Me.nud_mbc84_Rechts_CH4 = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc84_Rechts_CH4 = New System.Windows.Forms.Label()
        Me.btn_mbc84_Weg_CH4 = New System.Windows.Forms.Button()
        Me.nud_mbc84_Step_CH4 = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc84_Step_CH4 = New System.Windows.Forms.Label()
        Me.gb_mbc84_CH3 = New System.Windows.Forms.GroupBox()
        Me.txt_mbc84_adr_3 = New System.Windows.Forms.TextBox()
        Me.nud_mbc84_Links_CH3 = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc84_adr_CH3 = New System.Windows.Forms.Label()
        Me.lbl_mbc84_Links_CH3 = New System.Windows.Forms.Label()
        Me.nud_mbc84_Rechts_CH3 = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc84_Rechts_CH3 = New System.Windows.Forms.Label()
        Me.btn_mbc84_Weg_CH3 = New System.Windows.Forms.Button()
        Me.nud_mbc84_Step_CH3 = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc84_Step_CH3 = New System.Windows.Forms.Label()
        Me.gb_mbc84_CH2 = New System.Windows.Forms.GroupBox()
        Me.txt_mbc84_adr_2 = New System.Windows.Forms.TextBox()
        Me.nud_mbc84_Links_CH2 = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc84_adr_CH2 = New System.Windows.Forms.Label()
        Me.lbl_mbc84_Links_CH2 = New System.Windows.Forms.Label()
        Me.nud_mbc84_Rechts_CH2 = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc84_Rechts_CH2 = New System.Windows.Forms.Label()
        Me.btn_mbc84_Weg_CH2 = New System.Windows.Forms.Button()
        Me.nud_mbc84_Step_CH2 = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc84_Step_CH2 = New System.Windows.Forms.Label()
        Me.gb_mbc84_CH1 = New System.Windows.Forms.GroupBox()
        Me.txt_mbc84_adr_1 = New System.Windows.Forms.TextBox()
        Me.nud_mbc84_Links_CH1 = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc84_adr_CH1 = New System.Windows.Forms.Label()
        Me.lbl_mbc84_Links_CH1 = New System.Windows.Forms.Label()
        Me.nud_mbc84_Rechts_CH1 = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc84_Rechts_CH1 = New System.Windows.Forms.Label()
        Me.btn_mbc84_Weg_CH1 = New System.Windows.Forms.Button()
        Me.nud_mbc84_Step_CH1 = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc84_Step_CH1 = New System.Windows.Forms.Label()
        Me.nud_mbc84_MMAddr = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc84_LocID = New System.Windows.Forms.Label()
        Me.tab_mbc85 = New System.Windows.Forms.TabPage()
        Me.btn_mbc85_Winkel = New System.Windows.Forms.Button()
        Me.gb_mbc85_DS_Integrator = New System.Windows.Forms.GroupBox()
        Me.lbl_mbc85_P11 = New System.Windows.Forms.Label()
        Me.lbl_mbc85_P11a = New System.Windows.Forms.Label()
        Me.nud_mbc85_T_P11 = New System.Windows.Forms.NumericUpDown()
        Me.gb_mbc85_MMLok = New System.Windows.Forms.GroupBox()
        Me.nud_mbc85_MMSpeed = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc85_MMSpeed = New System.Windows.Forms.Label()
        Me.lbl_mbc85_MMLok = New System.Windows.Forms.Label()
        Me.nud_mbc85_MMLok = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc85_bis = New System.Windows.Forms.Label()
        Me.txt_mbc85_kontakt2cs2 = New System.Windows.Forms.TextBox()
        Me.txt_mbc85_kontakt1cs2 = New System.Windows.Forms.TextBox()
        Me.lbl_mbc85_Kontakte = New System.Windows.Forms.Label()
        Me.txt_mbc85_BusNr = New System.Windows.Forms.TextBox()
        Me.lbl_mbc85_BusNr = New System.Windows.Forms.Label()
        Me.nud_mbc85_S88ModNr = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc85_ModulNr = New System.Windows.Forms.Label()
        Me.gb_mbc85_Integrator = New System.Windows.Forms.GroupBox()
        Me.nud_mbc85_T_P05 = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc85_P05a = New System.Windows.Forms.Label()
        Me.lbl_mbc85_P05 = New System.Windows.Forms.Label()
        Me.nud_mbc85_T_P06 = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc85_P06a = New System.Windows.Forms.Label()
        Me.lbl_mbc85_P06 = New System.Windows.Forms.Label()
        Me.nud_mbc85_T_P04 = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc85_P04 = New System.Windows.Forms.Label()
        Me.nud_mbc85_T_P08 = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc85_P04a = New System.Windows.Forms.Label()
        Me.lbl_mbc85_P08 = New System.Windows.Forms.Label()
        Me.nud_mbc85_T_P03 = New System.Windows.Forms.NumericUpDown()
        Me.nud_mbc85_T_P07 = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc85_P03 = New System.Windows.Forms.Label()
        Me.lbl_mbc85_P08a = New System.Windows.Forms.Label()
        Me.lbl_mbc85_P03a = New System.Windows.Forms.Label()
        Me.lbl_mbc85_P07a = New System.Windows.Forms.Label()
        Me.nud_mbc85_T_P02 = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc85_P07 = New System.Windows.Forms.Label()
        Me.lbl_mbc85_P02 = New System.Windows.Forms.Label()
        Me.nud_mbc85_T_P10 = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc85_P02a = New System.Windows.Forms.Label()
        Me.lbl_mbc85_P10 = New System.Windows.Forms.Label()
        Me.nud_mbc85_T_P01 = New System.Windows.Forms.NumericUpDown()
        Me.nud_mbc85_T_P09 = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc85_P01 = New System.Windows.Forms.Label()
        Me.lbl_mbc85_P10a = New System.Windows.Forms.Label()
        Me.lbl_mbc85_P01a = New System.Windows.Forms.Label()
        Me.lbl_mbc85_P09a = New System.Windows.Forms.Label()
        Me.lbl_mbc85_P09 = New System.Windows.Forms.Label()
        Me.tab_mbc86 = New System.Windows.Forms.TabPage()
        Me.gb_mbc86_MMLok = New System.Windows.Forms.GroupBox()
        Me.lbl_mbc86_MMLok = New System.Windows.Forms.Label()
        Me.nud_mbc86_MMLok = New System.Windows.Forms.NumericUpDown()
        Me.Label39 = New System.Windows.Forms.Label()
        Me.txt_mbc86_kontakt2cs2 = New System.Windows.Forms.TextBox()
        Me.txt_mbc86_kontakt1cs2 = New System.Windows.Forms.TextBox()
        Me.lbl_mbc86_Kontakte = New System.Windows.Forms.Label()
        Me.txt_mbc86_BusNr = New System.Windows.Forms.TextBox()
        Me.lbl_mbc86_BusNr = New System.Windows.Forms.Label()
        Me.nud_mbc86_S88ModNr = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc86_ModulNr = New System.Windows.Forms.Label()
        Me.gb_mbc86_Integrator = New System.Windows.Forms.GroupBox()
        Me.nud_mbc86_T_P05 = New System.Windows.Forms.NumericUpDown()
        Me.Label12 = New System.Windows.Forms.Label()
        Me.Label13 = New System.Windows.Forms.Label()
        Me.nud_mbc86_T_P06 = New System.Windows.Forms.NumericUpDown()
        Me.Label14 = New System.Windows.Forms.Label()
        Me.Label15 = New System.Windows.Forms.Label()
        Me.nud_mbc86_T_P04 = New System.Windows.Forms.NumericUpDown()
        Me.Label16 = New System.Windows.Forms.Label()
        Me.nud_mbc86_T_P08 = New System.Windows.Forms.NumericUpDown()
        Me.Label17 = New System.Windows.Forms.Label()
        Me.Label20 = New System.Windows.Forms.Label()
        Me.nud_mbc86_T_P03 = New System.Windows.Forms.NumericUpDown()
        Me.nud_mbc86_T_P07 = New System.Windows.Forms.NumericUpDown()
        Me.Label21 = New System.Windows.Forms.Label()
        Me.Label22 = New System.Windows.Forms.Label()
        Me.Label23 = New System.Windows.Forms.Label()
        Me.Label24 = New System.Windows.Forms.Label()
        Me.nud_mbc86_T_P02 = New System.Windows.Forms.NumericUpDown()
        Me.Label25 = New System.Windows.Forms.Label()
        Me.Label28 = New System.Windows.Forms.Label()
        Me.Label29 = New System.Windows.Forms.Label()
        Me.nud_mbc86_T_P01 = New System.Windows.Forms.NumericUpDown()
        Me.Label31 = New System.Windows.Forms.Label()
        Me.Label33 = New System.Windows.Forms.Label()
        Me.tab_mbc87 = New System.Windows.Forms.TabPage()
        Me.nud_mbc87_MMAddr = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc87_1 = New System.Windows.Forms.Label()
        Me.tab_mbc88 = New System.Windows.Forms.TabPage()
        Me.cmb_mbc88_Halbmodul = New System.Windows.Forms.ComboBox()
        Me.lbl_mbc88_bis = New System.Windows.Forms.Label()
        Me.txt_mbc88_kontakt2cs2 = New System.Windows.Forms.TextBox()
        Me.txt_mbc88_kontakt1cs2 = New System.Windows.Forms.TextBox()
        Me.lbl_mbc88_kontaktecs2 = New System.Windows.Forms.Label()
        Me.txt_mbc88_busnr = New System.Windows.Forms.TextBox()
        Me.lbl_mbc88_busnr = New System.Windows.Forms.Label()
        Me.lbl_mbc88_halbmodul = New System.Windows.Forms.Label()
        Me.nud_mbc88_T_P16 = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc88_P16 = New System.Windows.Forms.Label()
        Me.lbl_mbc88_P16a = New System.Windows.Forms.Label()
        Me.nud_mbc88_T_P12 = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc88_P12 = New System.Windows.Forms.Label()
        Me.lbl_mbc88_P12a = New System.Windows.Forms.Label()
        Me.nud_mbc88_T_P08 = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc88_P08 = New System.Windows.Forms.Label()
        Me.lbl_mbc88_P08a = New System.Windows.Forms.Label()
        Me.nud_mbc88_T_P04 = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc88_P04 = New System.Windows.Forms.Label()
        Me.lbl_mbc88_P04a = New System.Windows.Forms.Label()
        Me.nud_mbc88_T_P15 = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc88_P15 = New System.Windows.Forms.Label()
        Me.lbl_mbc88_P15a = New System.Windows.Forms.Label()
        Me.nud_mbc88_T_P11 = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc88_P11 = New System.Windows.Forms.Label()
        Me.lbl_mbc88_P11a = New System.Windows.Forms.Label()
        Me.nud_mbc88_T_P07 = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc88_P07 = New System.Windows.Forms.Label()
        Me.lbl_mbc88_P07a = New System.Windows.Forms.Label()
        Me.nud_mbc88_T_P03 = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc88_P03 = New System.Windows.Forms.Label()
        Me.lbl_mbc88_P03a = New System.Windows.Forms.Label()
        Me.nud_mbc88_T_P14 = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc88_P14 = New System.Windows.Forms.Label()
        Me.lbl_mbc88_P14a = New System.Windows.Forms.Label()
        Me.nud_mbc88_T_P10 = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc88_P10 = New System.Windows.Forms.Label()
        Me.lbl_mbc88_P10a = New System.Windows.Forms.Label()
        Me.nud_mbc88_T_P06 = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc88_P06 = New System.Windows.Forms.Label()
        Me.lbl_mbc88_P06a = New System.Windows.Forms.Label()
        Me.nud_mbc88_T_P02 = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc88_P02 = New System.Windows.Forms.Label()
        Me.lbl_mbc88_P02a = New System.Windows.Forms.Label()
        Me.nud_mbc88_T_P13 = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc88_P13 = New System.Windows.Forms.Label()
        Me.lbl_mbc88_P13a = New System.Windows.Forms.Label()
        Me.nud_mbc88_T_P09 = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc88_P09 = New System.Windows.Forms.Label()
        Me.lbl_mbc88_P09a = New System.Windows.Forms.Label()
        Me.nud_mbc88_T_P05 = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc88_P05 = New System.Windows.Forms.Label()
        Me.lbl_mbc88_P05a = New System.Windows.Forms.Label()
        Me.nud_mbc88_T_P01 = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc88_P01 = New System.Windows.Forms.Label()
        Me.lbl_mbc88_P01a = New System.Windows.Forms.Label()
        Me.nud_mbc88_S88ModNr = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc88_modulnr = New System.Windows.Forms.Label()
        Me.gb_mbc88 = New System.Windows.Forms.GroupBox()
        Me.tab_mbc89 = New System.Windows.Forms.TabPage()
        Me.gb_mbc89_I2C = New System.Windows.Forms.GroupBox()
        Me.lbl_mbc89_frequenz = New System.Windows.Forms.Label()
        Me.lbl_mbc89_takt = New System.Windows.Forms.Label()
        Me.nud_mbc89_i2c = New System.Windows.Forms.NumericUpDown()
        Me.gb_mbc89_Basisadresse = New System.Windows.Forms.GroupBox()
        Me.lbl_mbc89_Basisadresse = New System.Windows.Forms.Label()
        Me.nud_mbc89_basis = New System.Windows.Forms.NumericUpDown()
        Me.btn_mbc89_Loks = New System.Windows.Forms.Button()
        Me.gb_mbc89_Mapping = New System.Windows.Forms.GroupBox()
        Me.clb_mbc89_Mapping = New System.Windows.Forms.CheckedListBox()
        Me.gb_mbc89_Keyboard = New System.Windows.Forms.GroupBox()
        Me.clb_mbc89_D = New System.Windows.Forms.CheckedListBox()
        Me.clb_mbc89_C = New System.Windows.Forms.CheckedListBox()
        Me.clb_mbc89_B = New System.Windows.Forms.CheckedListBox()
        Me.clb_mbc89_A = New System.Windows.Forms.CheckedListBox()
        Me.lbl_mbc89_D = New System.Windows.Forms.Label()
        Me.lbl_mbc89_C = New System.Windows.Forms.Label()
        Me.lbl_mbc89_B = New System.Windows.Forms.Label()
        Me.lbl_mbc89_A = New System.Windows.Forms.Label()
        Me.nud_mbc89_keyboard_nr = New System.Windows.Forms.NumericUpDown()
        Me.pb_mbc89_keyboard = New System.Windows.Forms.PictureBox()
        Me.gb_mbc89_Version = New System.Windows.Forms.GroupBox()
        Me.nud_mbc89_SW_2 = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc89_Dot = New System.Windows.Forms.Label()
        Me.nud_mbc89_SW_1 = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc89_Version = New System.Windows.Forms.Label()
        Me.txt_mbc89_alt = New System.Windows.Forms.TextBox()
        Me.tab_mbc90 = New System.Windows.Forms.TabPage()
        Me.lbl_mbc90_kontaktecs2bis = New System.Windows.Forms.Label()
        Me.txt_mbc90_kontakt2cs2 = New System.Windows.Forms.TextBox()
        Me.txt_mbc90_kontakt1cs2 = New System.Windows.Forms.TextBox()
        Me.lbl_mbc90_kontaktecs2 = New System.Windows.Forms.Label()
        Me.txt_mbc90_busnr = New System.Windows.Forms.TextBox()
        Me.lbl_mbc90_busnr = New System.Windows.Forms.Label()
        Me.nud_mbc90_T_P16 = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc90_P16 = New System.Windows.Forms.Label()
        Me.lbl_mbc90_P16a = New System.Windows.Forms.Label()
        Me.nud_mbc90_T_P12 = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc90_P12 = New System.Windows.Forms.Label()
        Me.lbl_mbc90_P12a = New System.Windows.Forms.Label()
        Me.nud_mbc90_T_P08 = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc90_P08 = New System.Windows.Forms.Label()
        Me.lbl_mbc90_P08a = New System.Windows.Forms.Label()
        Me.nud_mbc90_T_P04 = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc90_P04 = New System.Windows.Forms.Label()
        Me.lbl_mbc90_P04a = New System.Windows.Forms.Label()
        Me.nud_mbc90_T_P15 = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc90_P15 = New System.Windows.Forms.Label()
        Me.lbl_mbc90_P15a = New System.Windows.Forms.Label()
        Me.nud_mbc90_T_P11 = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc90_P11 = New System.Windows.Forms.Label()
        Me.lbl_mbc90_P11a = New System.Windows.Forms.Label()
        Me.nud_mbc90_T_P07 = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc90_P07 = New System.Windows.Forms.Label()
        Me.lbl_mbc90_P07a = New System.Windows.Forms.Label()
        Me.nud_mbc90_T_P03 = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc90_P03 = New System.Windows.Forms.Label()
        Me.lbl_mbc90_P03a = New System.Windows.Forms.Label()
        Me.nud_mbc90_T_P14 = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc90_P14 = New System.Windows.Forms.Label()
        Me.lbl_mbc90_P14a = New System.Windows.Forms.Label()
        Me.nud_mbc90_T_P10 = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc90_P10 = New System.Windows.Forms.Label()
        Me.lbl_mbc90_P10a = New System.Windows.Forms.Label()
        Me.nud_mbc90_T_P06 = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc90_P06 = New System.Windows.Forms.Label()
        Me.lbl_mbc90_P06a = New System.Windows.Forms.Label()
        Me.nud_mbc90_T_P02 = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc90_P02 = New System.Windows.Forms.Label()
        Me.lbl_mbc90_P02a = New System.Windows.Forms.Label()
        Me.nud_mbc90_T_P13 = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc90_P13 = New System.Windows.Forms.Label()
        Me.lbl_mbc90_P13a = New System.Windows.Forms.Label()
        Me.nud_mbc90_T_P09 = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc90_P09 = New System.Windows.Forms.Label()
        Me.lbl_mbc90_P09a = New System.Windows.Forms.Label()
        Me.nud_mbc90_T_P05 = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc90_P05 = New System.Windows.Forms.Label()
        Me.lbl_mbc90_P05a = New System.Windows.Forms.Label()
        Me.nud_mbc90_T_P01 = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc90_P01 = New System.Windows.Forms.Label()
        Me.lbl_mbc90_P01a = New System.Windows.Forms.Label()
        Me.nud_mbc90_S88ModNr = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc90_S88ModulNr = New System.Windows.Forms.Label()
        Me.gb_mbc90 = New System.Windows.Forms.GroupBox()
        Me.tab_mbc91 = New System.Windows.Forms.TabPage()
        Me.lbl_mbc91_AusEinheit = New System.Windows.Forms.Label()
        Me.lbl_mbc91_Rueck = New System.Windows.Forms.Label()
        Me.lbl_mbc91_Aus = New System.Windows.Forms.Label()
        Me.nud_mbc91_Rueck = New System.Windows.Forms.NumericUpDown()
        Me.nud_mbc91_Aus = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc91_Wechsel = New System.Windows.Forms.Label()
        Me.lbl_mbc91_Impuls = New System.Windows.Forms.Label()
        Me.gb_mbc91_CH4 = New System.Windows.Forms.GroupBox()
        Me.txt_mbc91_CH4_Addr = New System.Windows.Forms.TextBox()
        Me.lbl_mbc91_CH4_Addr = New System.Windows.Forms.Label()
        Me.nud_mbc91_CH4_TIO = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc91_CH4_Typ = New System.Windows.Forms.Label()
        Me.lbl_mbc91_CH4_TIO = New System.Windows.Forms.Label()
        Me.clb_mbc91_CH4 = New System.Windows.Forms.CheckedListBox()
        Me.lbl_mbc91_CH4_TIOEinheit = New System.Windows.Forms.Label()
        Me.gb_mbc91_CH3 = New System.Windows.Forms.GroupBox()
        Me.txt_mbc91_CH3_Addr = New System.Windows.Forms.TextBox()
        Me.lbl_mbc91_CH3_Addr = New System.Windows.Forms.Label()
        Me.nud_mbc91_CH3_TIO = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc91_CH3_Typ = New System.Windows.Forms.Label()
        Me.lbl_mbc91_CH3_TIO = New System.Windows.Forms.Label()
        Me.clb_mbc91_CH3 = New System.Windows.Forms.CheckedListBox()
        Me.lbl_mbc91_CH3_TIOEinheit = New System.Windows.Forms.Label()
        Me.gb_mbc91_CH2 = New System.Windows.Forms.GroupBox()
        Me.txt_mbc91_CH2_Addr = New System.Windows.Forms.TextBox()
        Me.lbl_mbc91_CH2_Addr = New System.Windows.Forms.Label()
        Me.nud_mbc91_CH2_TIO = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc91_CH2_Typ = New System.Windows.Forms.Label()
        Me.lbl_mbc91_CH2_TIO = New System.Windows.Forms.Label()
        Me.clb_mbc91_CH2 = New System.Windows.Forms.CheckedListBox()
        Me.lbl_mbc91_CH2_TIOEinheit = New System.Windows.Forms.Label()
        Me.gb_mbc91_CH1 = New System.Windows.Forms.GroupBox()
        Me.txt_mbc91_CH1_Addr = New System.Windows.Forms.TextBox()
        Me.lbl_mbc91_CH1_Addr = New System.Windows.Forms.Label()
        Me.nud_mbc91_CH1_TIO = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc91_CH1_Typ = New System.Windows.Forms.Label()
        Me.lbl_mbc91_CH1_TIO = New System.Windows.Forms.Label()
        Me.clb_mbc91_CH1 = New System.Windows.Forms.CheckedListBox()
        Me.lbl_mbc91_CH1_TIOEinheit = New System.Windows.Forms.Label()
        Me.nud_mbc91_MMAddr = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc91_MMAddr = New System.Windows.Forms.Label()
        Me.pic_mbc91_Wechsel = New System.Windows.Forms.PictureBox()
        Me.pic_mbc91_Impuls = New System.Windows.Forms.PictureBox()
        Me.tab_mbc92 = New System.Windows.Forms.TabPage()
        Me.lbl_mbc92_AusEinheit = New System.Windows.Forms.Label()
        Me.lbl_mbc92_Rueck = New System.Windows.Forms.Label()
        Me.lbl_mbc92_Aus = New System.Windows.Forms.Label()
        Me.nud_mbc92_Rueck = New System.Windows.Forms.NumericUpDown()
        Me.nud_mbc92_Aus = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc92_Wechsel = New System.Windows.Forms.Label()
        Me.lbl_mbc92_Impuls = New System.Windows.Forms.Label()
        Me.gb_mbc92_CH4 = New System.Windows.Forms.GroupBox()
        Me.txt_mbc92_CH4_Addr = New System.Windows.Forms.TextBox()
        Me.lbl_mbc92_CH4_Addr = New System.Windows.Forms.Label()
        Me.nud_mbc92_CH4_TIO = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc92_CH4_Typ = New System.Windows.Forms.Label()
        Me.lbl_mbc92_CH4_TIO = New System.Windows.Forms.Label()
        Me.clb_mbc92_CH4 = New System.Windows.Forms.CheckedListBox()
        Me.lbl_mbc92_CH4_TIOEinheit = New System.Windows.Forms.Label()
        Me.gb_mbc92_CH3 = New System.Windows.Forms.GroupBox()
        Me.txt_mbc92_CH3_Addr = New System.Windows.Forms.TextBox()
        Me.lbl_mbc92_CH3_Addr = New System.Windows.Forms.Label()
        Me.nud_mbc92_CH3_TIO = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc92_CH3_Typ = New System.Windows.Forms.Label()
        Me.lbl_mbc92_CH3_TIO = New System.Windows.Forms.Label()
        Me.clb_mbc92_CH3 = New System.Windows.Forms.CheckedListBox()
        Me.lbl_mbc92_CH3_TIOEinheit = New System.Windows.Forms.Label()
        Me.gb_mbc92_CH2 = New System.Windows.Forms.GroupBox()
        Me.txt_mbc92_CH2_Addr = New System.Windows.Forms.TextBox()
        Me.lbl_mbc92_CH2_Addr = New System.Windows.Forms.Label()
        Me.nud_mbc92_CH2_TIO = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc92_CH2_Typ = New System.Windows.Forms.Label()
        Me.lbl_mbc92_CH2_TIO = New System.Windows.Forms.Label()
        Me.clb_mbc92_CH2 = New System.Windows.Forms.CheckedListBox()
        Me.lbl_mbc92_CH2_TIOEinheit = New System.Windows.Forms.Label()
        Me.gb_mbc92_CH1 = New System.Windows.Forms.GroupBox()
        Me.txt_mbc92_CH1_Addr = New System.Windows.Forms.TextBox()
        Me.lbl_mbc92_CH1_Addr = New System.Windows.Forms.Label()
        Me.nud_mbc92_CH1_TIO = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc92_CH1_Typ = New System.Windows.Forms.Label()
        Me.lbl_mbc92_CH1_TIO = New System.Windows.Forms.Label()
        Me.clb_mbc92_CH1 = New System.Windows.Forms.CheckedListBox()
        Me.lbl_mbc92_CH1_TIOEinheit = New System.Windows.Forms.Label()
        Me.nud_mbc92_MMAddr = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc92_MMAddr = New System.Windows.Forms.Label()
        Me.pic_mbc92_Wechsel = New System.Windows.Forms.PictureBox()
        Me.pic_mbc92_Impuls = New System.Windows.Forms.PictureBox()
        Me.tab_mbc93 = New System.Windows.Forms.TabPage()
        Me.gb_mbc93_Latenz = New System.Windows.Forms.GroupBox()
        Me.lbl_mbc93_LatenzBeschreibung_3 = New System.Windows.Forms.TextBox()
        Me.lbl_mbc93_LatenzBeschreibung_2 = New System.Windows.Forms.TextBox()
        Me.nud_mbc93_Latenz = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc93_LatenzBeschreibung_1 = New System.Windows.Forms.TextBox()
        Me.lbl_mbc93_LatenzBezeichnung = New System.Windows.Forms.Label()
        Me.gb_mbc93_GFP = New System.Windows.Forms.GroupBox()
        Me.lbl_mbc93_GFPBeschreibung_2 = New System.Windows.Forms.TextBox()
        Me.Label32 = New System.Windows.Forms.Label()
        Me.nud_mbc93_GFPSW2 = New System.Windows.Forms.NumericUpDown()
        Me.nud_mbc93_GFPSW1 = New System.Windows.Forms.NumericUpDown()
        Me.lbl_mbc93_GFPBeschreibung_1 = New System.Windows.Forms.TextBox()
        Me.lbl_mbc93_GFPBezeichnung = New System.Windows.Forms.Label()
        Me.gb_Keyboard = New System.Windows.Forms.GroupBox()
        Me.btn_KeyboardG4 = New System.Windows.Forms.Button()
        Me.btn_KeyboardG3 = New System.Windows.Forms.Button()
        Me.btn_KeyboardG2 = New System.Windows.Forms.Button()
        Me.btn_KeyboardG1 = New System.Windows.Forms.Button()
        Me.btn_KeyboardR4 = New System.Windows.Forms.Button()
        Me.btn_KeyboardR3 = New System.Windows.Forms.Button()
        Me.btn_KeyboardR2 = New System.Windows.Forms.Button()
        Me.btn_KeyboardR1 = New System.Windows.Forms.Button()
        Me.gb_Status = New System.Windows.Forms.GroupBox()
        Me.pic_Status_ASM08 = New System.Windows.Forms.PictureBox()
        Me.pic_Status_ASM07 = New System.Windows.Forms.PictureBox()
        Me.pic_Status_ASM06 = New System.Windows.Forms.PictureBox()
        Me.pic_Status_ASM05 = New System.Windows.Forms.PictureBox()
        Me.pic_Status_ASM04 = New System.Windows.Forms.PictureBox()
        Me.pic_Status_ASM03 = New System.Windows.Forms.PictureBox()
        Me.pic_Status_ASM02 = New System.Windows.Forms.PictureBox()
        Me.pic_Status_ASM01 = New System.Windows.Forms.PictureBox()
        Me.pic_Status_Decoder = New System.Windows.Forms.PictureBox()
        Me.pic_Status_LED19 = New System.Windows.Forms.PictureBox()
        Me.pic_Status_LED17 = New System.Windows.Forms.PictureBox()
        Me.pic_Status_LED14 = New System.Windows.Forms.PictureBox()
        Me.pic_Status_LED12 = New System.Windows.Forms.PictureBox()
        Me.pic_Status_LED08 = New System.Windows.Forms.PictureBox()
        Me.pic_Status_LED06 = New System.Windows.Forms.PictureBox()
        Me.pic_Status_LED03 = New System.Windows.Forms.PictureBox()
        Me.pic_Status_LED01 = New System.Windows.Forms.PictureBox()
        Me.pic_Status_LED18 = New System.Windows.Forms.PictureBox()
        Me.pic_Status_LED16 = New System.Windows.Forms.PictureBox()
        Me.pic_Status_LED15 = New System.Windows.Forms.PictureBox()
        Me.pic_Status_LED13 = New System.Windows.Forms.PictureBox()
        Me.pic_Status_LED07 = New System.Windows.Forms.PictureBox()
        Me.pic_Status_LED05 = New System.Windows.Forms.PictureBox()
        Me.pic_Status_LED04 = New System.Windows.Forms.PictureBox()
        Me.pic_Status_LED02 = New System.Windows.Forms.PictureBox()
        Me.pic_Status_LED09 = New System.Windows.Forms.PictureBox()
        Me.pic_Status_LED10 = New System.Windows.Forms.PictureBox()
        Me.pic_Status_LED11 = New System.Windows.Forms.PictureBox()
        Me.gb_Parameter = New System.Windows.Forms.GroupBox()
        Me.lbl_ParameterHW = New System.Windows.Forms.Label()
        Me.txt_ParameterHW = New System.Windows.Forms.TextBox()
        Me.txt_ParameterSW = New System.Windows.Forms.TextBox()
        Me.lbl_ParameterSW = New System.Windows.Forms.Label()
        Me.lbl_ParameterName = New System.Windows.Forms.Label()
        Me.txt_ParameterName = New System.Windows.Forms.TextBox()
        Me.lbl_ParameterEinheitTemperatur = New System.Windows.Forms.Label()
        Me.txt_ParameterArtNum = New System.Windows.Forms.TextBox()
        Me.txt_ParameterTemperatur = New System.Windows.Forms.TextBox()
        Me.lbl_ParameterEinheitSpannung = New System.Windows.Forms.Label()
        Me.lbl_ParameterTemperatur = New System.Windows.Forms.Label()
        Me.lbl_ParameterArtNum = New System.Windows.Forms.Label()
        Me.txt_ParameterPlugged = New System.Windows.Forms.TextBox()
        Me.txt_ParameterSpannung = New System.Windows.Forms.TextBox()
        Me.lbl_ParameterPlugged = New System.Windows.Forms.Label()
        Me.lbl_ParameterSpannung = New System.Windows.Forms.Label()
        Me.lbl_ParameterGUID = New System.Windows.Forms.Label()
        Me.txt_ParameterGUID = New System.Windows.Forms.TextBox()
        Me.txt_ParameterSerNr = New System.Windows.Forms.TextBox()
        Me.txt_ParameterDecTyp = New System.Windows.Forms.TextBox()
        Me.lbl_ParameterSerNr = New System.Windows.Forms.Label()
        Me.lbl_ParameterDecTyp = New System.Windows.Forms.Label()
        Me.tab_Monitor = New System.Windows.Forms.TabPage()
        Me.rtb_MonitorReceived = New System.Windows.Forms.RichTextBox()
        Me.lbl_MonitorNachrichtenherkunft = New System.Windows.Forms.Label()
        Me.lbl_MonitorInhalt = New System.Windows.Forms.Label()
        Me.tab_Adressen = New System.Windows.Forms.TabPage()
        Me.btn_AdressenS88Base = New System.Windows.Forms.Button()
        Me.lbl_AdressenS88Base = New System.Windows.Forms.Label()
        Me.dgv_AdressenS88Base = New System.Windows.Forms.DataGridView()
        Me.S88ColumnModNr = New System.Windows.Forms.DataGridViewTextBoxColumn()
        Me.S88ColumnS88Typ = New System.Windows.Forms.DataGridViewTextBoxColumn()
        Me.S88ColumnS88Name = New System.Windows.Forms.DataGridViewTextBoxColumn()
        Me.S88ColumnS88Index = New System.Windows.Forms.DataGridViewTextBoxColumn()
        Me.dgv_AdressenLocID = New System.Windows.Forms.DataGridView()
        Me.MMColumnLocID = New System.Windows.Forms.DataGridViewTextBoxColumn()
        Me.MMColumnLocIDTyp = New System.Windows.Forms.DataGridViewTextBoxColumn()
        Me.MMColumnLocIDName = New System.Windows.Forms.DataGridViewTextBoxColumn()
        Me.MMColumnLocIDIndex = New System.Windows.Forms.DataGridViewTextBoxColumn()
        Me.btn_AdressenLocID = New System.Windows.Forms.Button()
        Me.lbl_AdressenLocID = New System.Windows.Forms.Label()
        Me.stp_Status = New System.Windows.Forms.StatusStrip()
        Me.stp_RS232 = New System.Windows.Forms.ToolStripStatusLabel()
        Me.stp_BT = New System.Windows.Forms.ToolStripStatusLabel()
        Me.stp_TCP = New System.Windows.Forms.ToolStripStatusLabel()
        Me.stp_Refresh = New System.Windows.Forms.ToolStripStatusLabel()
        Me.spb_Refresh = New System.Windows.Forms.ToolStripProgressBar()
        Me.stp_Alive = New System.Windows.Forms.ToolStripStatusLabel()
        Me.stp_CSx = New System.Windows.Forms.ToolStripStatusLabel()
        Me.stp_Upgrade = New System.Windows.Forms.ToolStripStatusLabel()
        Me.tm_Neu = New System.Windows.Forms.Timer(Me.components)
        Me.tm_Alive = New System.Windows.Forms.Timer(Me.components)
        Me.tm_ComTimeout = New System.Windows.Forms.Timer(Me.components)
        Me.ofd_Oeffnen = New System.Windows.Forms.OpenFileDialog()
        Me.ofd_OeffnenBin = New System.Windows.Forms.OpenFileDialog()
        Me.tm_Update = New System.Windows.Forms.Timer(Me.components)
        Me.ms_Menu.SuspendLayout()
        CType(Me.sc_Main, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.sc_Main.Panel1.SuspendLayout()
        Me.sc_Main.Panel2.SuspendLayout()
        Me.sc_Main.SuspendLayout()
        Me.tab_Control.SuspendLayout()
        Me.tab_Modul.SuspendLayout()
        Me.tab_Module.SuspendLayout()
        Me.tab_mbc80.SuspendLayout()
        Me.gb_mbc80_SW.SuspendLayout()
        CType(Me.nud_mbc80_SW2, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_mbc80_SW1, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.gb_mbc80_AK.SuspendLayout()
        Me.gb_mbc80_BT.SuspendLayout()
        Me.tab_mbc81.SuspendLayout()
        CType(Me.nud_mbc81_LocID, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.tab_mbc82.SuspendLayout()
        Me.gb_mbc82_Latenz.SuspendLayout()
        CType(Me.nud_mbc82_Latenz, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.gb_mbc82_GFP.SuspendLayout()
        CType(Me.nud_mbc82_GFPSW2, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_mbc82_GFPSW1, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.tab_mbc83.SuspendLayout()
        CType(Me.pic_mbc83_Startup, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.pic_mbc83_Wechsel, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.pic_mbc83_Impuls, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_mbc83_Rueck, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_mbc83_Aus, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_mbc83_Dimmer, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.gb_mbc83_CH4.SuspendLayout()
        CType(Me.gb_mbc83_CH4_nud_TIO, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.gb_mbc83_CH3.SuspendLayout()
        CType(Me.gb_mbc83_CH3_nud_TIO, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.gb_mbc83_CH2.SuspendLayout()
        CType(Me.gb_mbc83_CH2_nud_TIO, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.gb_mbc83_CH1.SuspendLayout()
        CType(Me.gb_mbc83_CH1_nud_TIO, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_mbc83_MMAddr, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.tab_mbc84.SuspendLayout()
        CType(Me.nud_mbc84_Rueck, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.gb_mbc84_CH4.SuspendLayout()
        CType(Me.nud_mbc84_Links_CH4, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_mbc84_Rechts_CH4, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_mbc84_Step_CH4, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.gb_mbc84_CH3.SuspendLayout()
        CType(Me.nud_mbc84_Links_CH3, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_mbc84_Rechts_CH3, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_mbc84_Step_CH3, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.gb_mbc84_CH2.SuspendLayout()
        CType(Me.nud_mbc84_Links_CH2, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_mbc84_Rechts_CH2, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_mbc84_Step_CH2, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.gb_mbc84_CH1.SuspendLayout()
        CType(Me.nud_mbc84_Links_CH1, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_mbc84_Rechts_CH1, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_mbc84_Step_CH1, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_mbc84_MMAddr, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.tab_mbc85.SuspendLayout()
        Me.gb_mbc85_DS_Integrator.SuspendLayout()
        CType(Me.nud_mbc85_T_P11, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.gb_mbc85_MMLok.SuspendLayout()
        CType(Me.nud_mbc85_MMSpeed, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_mbc85_MMLok, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_mbc85_S88ModNr, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.gb_mbc85_Integrator.SuspendLayout()
        CType(Me.nud_mbc85_T_P05, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_mbc85_T_P06, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_mbc85_T_P04, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_mbc85_T_P08, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_mbc85_T_P03, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_mbc85_T_P07, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_mbc85_T_P02, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_mbc85_T_P10, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_mbc85_T_P01, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_mbc85_T_P09, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.tab_mbc86.SuspendLayout()
        Me.gb_mbc86_MMLok.SuspendLayout()
        CType(Me.nud_mbc86_MMLok, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_mbc86_S88ModNr, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.gb_mbc86_Integrator.SuspendLayout()
        CType(Me.nud_mbc86_T_P05, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_mbc86_T_P06, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_mbc86_T_P04, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_mbc86_T_P08, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_mbc86_T_P03, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_mbc86_T_P07, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_mbc86_T_P02, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_mbc86_T_P01, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.tab_mbc87.SuspendLayout()
        CType(Me.nud_mbc87_MMAddr, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.tab_mbc88.SuspendLayout()
        CType(Me.nud_mbc88_T_P16, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_mbc88_T_P12, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_mbc88_T_P08, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_mbc88_T_P04, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_mbc88_T_P15, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_mbc88_T_P11, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_mbc88_T_P07, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_mbc88_T_P03, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_mbc88_T_P14, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_mbc88_T_P10, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_mbc88_T_P06, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_mbc88_T_P02, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_mbc88_T_P13, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_mbc88_T_P09, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_mbc88_T_P05, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_mbc88_T_P01, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_mbc88_S88ModNr, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.tab_mbc89.SuspendLayout()
        Me.gb_mbc89_I2C.SuspendLayout()
        CType(Me.nud_mbc89_i2c, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.gb_mbc89_Basisadresse.SuspendLayout()
        CType(Me.nud_mbc89_basis, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.gb_mbc89_Mapping.SuspendLayout()
        Me.gb_mbc89_Keyboard.SuspendLayout()
        CType(Me.nud_mbc89_keyboard_nr, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.pb_mbc89_keyboard, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.gb_mbc89_Version.SuspendLayout()
        CType(Me.nud_mbc89_SW_2, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_mbc89_SW_1, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.tab_mbc90.SuspendLayout()
        CType(Me.nud_mbc90_T_P16, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_mbc90_T_P12, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_mbc90_T_P08, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_mbc90_T_P04, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_mbc90_T_P15, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_mbc90_T_P11, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_mbc90_T_P07, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_mbc90_T_P03, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_mbc90_T_P14, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_mbc90_T_P10, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_mbc90_T_P06, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_mbc90_T_P02, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_mbc90_T_P13, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_mbc90_T_P09, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_mbc90_T_P05, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_mbc90_T_P01, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_mbc90_S88ModNr, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.tab_mbc91.SuspendLayout()
        CType(Me.nud_mbc91_Rueck, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_mbc91_Aus, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.gb_mbc91_CH4.SuspendLayout()
        CType(Me.nud_mbc91_CH4_TIO, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.gb_mbc91_CH3.SuspendLayout()
        CType(Me.nud_mbc91_CH3_TIO, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.gb_mbc91_CH2.SuspendLayout()
        CType(Me.nud_mbc91_CH2_TIO, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.gb_mbc91_CH1.SuspendLayout()
        CType(Me.nud_mbc91_CH1_TIO, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_mbc91_MMAddr, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.pic_mbc91_Wechsel, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.pic_mbc91_Impuls, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.tab_mbc92.SuspendLayout()
        CType(Me.nud_mbc92_Rueck, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_mbc92_Aus, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.gb_mbc92_CH4.SuspendLayout()
        CType(Me.nud_mbc92_CH4_TIO, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.gb_mbc92_CH3.SuspendLayout()
        CType(Me.nud_mbc92_CH3_TIO, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.gb_mbc92_CH2.SuspendLayout()
        CType(Me.nud_mbc92_CH2_TIO, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.gb_mbc92_CH1.SuspendLayout()
        CType(Me.nud_mbc92_CH1_TIO, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_mbc92_MMAddr, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.pic_mbc92_Wechsel, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.pic_mbc92_Impuls, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.tab_mbc93.SuspendLayout()
        Me.gb_mbc93_Latenz.SuspendLayout()
        CType(Me.nud_mbc93_Latenz, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.gb_mbc93_GFP.SuspendLayout()
        CType(Me.nud_mbc93_GFPSW2, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_mbc93_GFPSW1, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.gb_Keyboard.SuspendLayout()
        Me.gb_Status.SuspendLayout()
        CType(Me.pic_Status_ASM08, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.pic_Status_ASM07, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.pic_Status_ASM06, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.pic_Status_ASM05, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.pic_Status_ASM04, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.pic_Status_ASM03, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.pic_Status_ASM02, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.pic_Status_ASM01, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.pic_Status_Decoder, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.pic_Status_LED19, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.pic_Status_LED17, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.pic_Status_LED14, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.pic_Status_LED12, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.pic_Status_LED08, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.pic_Status_LED06, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.pic_Status_LED03, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.pic_Status_LED01, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.pic_Status_LED18, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.pic_Status_LED16, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.pic_Status_LED15, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.pic_Status_LED13, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.pic_Status_LED07, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.pic_Status_LED05, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.pic_Status_LED04, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.pic_Status_LED02, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.pic_Status_LED09, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.pic_Status_LED10, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.pic_Status_LED11, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.gb_Parameter.SuspendLayout()
        Me.tab_Monitor.SuspendLayout()
        Me.tab_Adressen.SuspendLayout()
        CType(Me.dgv_AdressenS88Base, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.dgv_AdressenLocID, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.stp_Status.SuspendLayout()
        Me.SuspendLayout()
        '
        'ms_Menu
        '
        Me.ms_Menu.BackgroundImageLayout = System.Windows.Forms.ImageLayout.None
        Me.ms_Menu.ImageScalingSize = New System.Drawing.Size(24, 24)
        Me.ms_Menu.Items.AddRange(New System.Windows.Forms.ToolStripItem() {Me.ts_MenuSystem, Me.ts_MenuVerbindung, Me.ts_MenuInfo})
        Me.ms_Menu.LayoutStyle = System.Windows.Forms.ToolStripLayoutStyle.HorizontalStackWithOverflow
        Me.ms_Menu.Location = New System.Drawing.Point(7, 0)
        Me.ms_Menu.Name = "ms_Menu"
        Me.ms_Menu.Padding = New System.Windows.Forms.Padding(4, 1, 0, 1)
        Me.ms_Menu.RenderMode = System.Windows.Forms.ToolStripRenderMode.System
        Me.ms_Menu.Size = New System.Drawing.Size(924, 24)
        Me.ms_Menu.TabIndex = 0
        Me.ms_Menu.Text = "MenuStrip"
        '
        'ts_MenuSystem
        '
        Me.ts_MenuSystem.DropDownItems.AddRange(New System.Windows.Forms.ToolStripItem() {Me.ts_MenuSystemNeu, Me.ts_MenuSystemSpeichern, Me.ts_MenuSystemAnzeigen, Me.ts_MenuSystemSprache, Me.ts_MenuSystemSchliessen})
        Me.ts_MenuSystem.Name = "ts_MenuSystem"
        Me.ts_MenuSystem.Size = New System.Drawing.Size(57, 22)
        Me.ts_MenuSystem.Text = "System"
        '
        'ts_MenuSystemNeu
        '
        Me.ts_MenuSystemNeu.Name = "ts_MenuSystemNeu"
        Me.ts_MenuSystemNeu.Size = New System.Drawing.Size(197, 22)
        Me.ts_MenuSystemNeu.Text = "Datenbank erneuern ..."
        '
        'ts_MenuSystemSpeichern
        '
        Me.ts_MenuSystemSpeichern.Name = "ts_MenuSystemSpeichern"
        Me.ts_MenuSystemSpeichern.Size = New System.Drawing.Size(197, 22)
        Me.ts_MenuSystemSpeichern.Text = "Datenbank speichern ..."
        '
        'ts_MenuSystemAnzeigen
        '
        Me.ts_MenuSystemAnzeigen.Name = "ts_MenuSystemAnzeigen"
        Me.ts_MenuSystemAnzeigen.Size = New System.Drawing.Size(197, 22)
        Me.ts_MenuSystemAnzeigen.Text = "Datenbank zeigen ..."
        '
        'ts_MenuSystemSprache
        '
        Me.ts_MenuSystemSprache.DropDownItems.AddRange(New System.Windows.Forms.ToolStripItem() {Me.ts_MenuSystemSprache_Deutsch, Me.ts_MenuSystemSprache_Französisch, Me.ts_MenuSystemSprache_Englisch})
        Me.ts_MenuSystemSprache.Name = "ts_MenuSystemSprache"
        Me.ts_MenuSystemSprache.Size = New System.Drawing.Size(197, 22)
        Me.ts_MenuSystemSprache.Text = "Sprache"
        '
        'ts_MenuSystemSprache_Deutsch
        '
        Me.ts_MenuSystemSprache_Deutsch.Name = "ts_MenuSystemSprache_Deutsch"
        Me.ts_MenuSystemSprache_Deutsch.Size = New System.Drawing.Size(135, 22)
        Me.ts_MenuSystemSprache_Deutsch.Text = "Deutsch"
        '
        'ts_MenuSystemSprache_Französisch
        '
        Me.ts_MenuSystemSprache_Französisch.Name = "ts_MenuSystemSprache_Französisch"
        Me.ts_MenuSystemSprache_Französisch.Size = New System.Drawing.Size(135, 22)
        Me.ts_MenuSystemSprache_Französisch.Text = "Französisch"
        '
        'ts_MenuSystemSprache_Englisch
        '
        Me.ts_MenuSystemSprache_Englisch.Name = "ts_MenuSystemSprache_Englisch"
        Me.ts_MenuSystemSprache_Englisch.Size = New System.Drawing.Size(135, 22)
        Me.ts_MenuSystemSprache_Englisch.Text = "Englisch"
        '
        'ts_MenuSystemSchliessen
        '
        Me.ts_MenuSystemSchliessen.Name = "ts_MenuSystemSchliessen"
        Me.ts_MenuSystemSchliessen.Size = New System.Drawing.Size(197, 22)
        Me.ts_MenuSystemSchliessen.Text = "Beenden"
        '
        'ts_MenuVerbindung
        '
        Me.ts_MenuVerbindung.DropDownItems.AddRange(New System.Windows.Forms.ToolStripItem() {Me.ts_MenuVerbindungEinstellen, Me.ts_MenuVerbindungRS232, Me.ts_MenuVerbindungBluetooth, Me.ts_MenuVerbindungTCP})
        Me.ts_MenuVerbindung.Name = "ts_MenuVerbindung"
        Me.ts_MenuVerbindung.Size = New System.Drawing.Size(80, 22)
        Me.ts_MenuVerbindung.Text = "Verbindung"
        '
        'ts_MenuVerbindungEinstellen
        '
        Me.ts_MenuVerbindungEinstellen.Name = "ts_MenuVerbindungEinstellen"
        Me.ts_MenuVerbindungEinstellen.Size = New System.Drawing.Size(126, 22)
        Me.ts_MenuVerbindungEinstellen.Text = "Einstellen"
        '
        'ts_MenuVerbindungRS232
        '
        Me.ts_MenuVerbindungRS232.DropDownItems.AddRange(New System.Windows.Forms.ToolStripItem() {Me.ts_MenuVerbindungRS232Verbinden, Me.ts_MenuVerbindungRS232Trennen})
        Me.ts_MenuVerbindungRS232.Name = "ts_MenuVerbindungRS232"
        Me.ts_MenuVerbindungRS232.Size = New System.Drawing.Size(126, 22)
        Me.ts_MenuVerbindungRS232.Text = "RS232"
        '
        'ts_MenuVerbindungRS232Verbinden
        '
        Me.ts_MenuVerbindungRS232Verbinden.Name = "ts_MenuVerbindungRS232Verbinden"
        Me.ts_MenuVerbindungRS232Verbinden.Size = New System.Drawing.Size(127, 22)
        Me.ts_MenuVerbindungRS232Verbinden.Text = "Verbinden"
        '
        'ts_MenuVerbindungRS232Trennen
        '
        Me.ts_MenuVerbindungRS232Trennen.Name = "ts_MenuVerbindungRS232Trennen"
        Me.ts_MenuVerbindungRS232Trennen.Size = New System.Drawing.Size(127, 22)
        Me.ts_MenuVerbindungRS232Trennen.Text = "Trennen"
        '
        'ts_MenuVerbindungBluetooth
        '
        Me.ts_MenuVerbindungBluetooth.DropDownItems.AddRange(New System.Windows.Forms.ToolStripItem() {Me.ts_MenuVerbindungBluetoothVerbinden, Me.ts_MenuVerbindungBluetoothTrennen})
        Me.ts_MenuVerbindungBluetooth.Name = "ts_MenuVerbindungBluetooth"
        Me.ts_MenuVerbindungBluetooth.Size = New System.Drawing.Size(126, 22)
        Me.ts_MenuVerbindungBluetooth.Text = "Bluetooth"
        '
        'ts_MenuVerbindungBluetoothVerbinden
        '
        Me.ts_MenuVerbindungBluetoothVerbinden.Name = "ts_MenuVerbindungBluetoothVerbinden"
        Me.ts_MenuVerbindungBluetoothVerbinden.Size = New System.Drawing.Size(127, 22)
        Me.ts_MenuVerbindungBluetoothVerbinden.Text = "Verbinden"
        '
        'ts_MenuVerbindungBluetoothTrennen
        '
        Me.ts_MenuVerbindungBluetoothTrennen.Name = "ts_MenuVerbindungBluetoothTrennen"
        Me.ts_MenuVerbindungBluetoothTrennen.Size = New System.Drawing.Size(127, 22)
        Me.ts_MenuVerbindungBluetoothTrennen.Text = "Trennen"
        '
        'ts_MenuVerbindungTCP
        '
        Me.ts_MenuVerbindungTCP.DropDownItems.AddRange(New System.Windows.Forms.ToolStripItem() {Me.ts_MenuVerbindungTCPVerbinden, Me.ts_MenuVerbindungTCPTrennen})
        Me.ts_MenuVerbindungTCP.Name = "ts_MenuVerbindungTCP"
        Me.ts_MenuVerbindungTCP.Size = New System.Drawing.Size(126, 22)
        Me.ts_MenuVerbindungTCP.Text = "TCP"
        '
        'ts_MenuVerbindungTCPVerbinden
        '
        Me.ts_MenuVerbindungTCPVerbinden.Name = "ts_MenuVerbindungTCPVerbinden"
        Me.ts_MenuVerbindungTCPVerbinden.Size = New System.Drawing.Size(127, 22)
        Me.ts_MenuVerbindungTCPVerbinden.Text = "Verbinden"
        '
        'ts_MenuVerbindungTCPTrennen
        '
        Me.ts_MenuVerbindungTCPTrennen.Name = "ts_MenuVerbindungTCPTrennen"
        Me.ts_MenuVerbindungTCPTrennen.Size = New System.Drawing.Size(127, 22)
        Me.ts_MenuVerbindungTCPTrennen.Text = "Trennen"
        '
        'ts_MenuInfo
        '
        Me.ts_MenuInfo.DropDownItems.AddRange(New System.Windows.Forms.ToolStripItem() {Me.ts_MenuInfoHilfe, Me.ts_MenuInfoBedienungsanleitung, Me.ts_MenuInfoCopyright})
        Me.ts_MenuInfo.Name = "ts_MenuInfo"
        Me.ts_MenuInfo.Size = New System.Drawing.Size(40, 22)
        Me.ts_MenuInfo.Text = "Info"
        '
        'ts_MenuInfoHilfe
        '
        Me.ts_MenuInfoHilfe.Name = "ts_MenuInfoHilfe"
        Me.ts_MenuInfoHilfe.Size = New System.Drawing.Size(186, 22)
        Me.ts_MenuInfoHilfe.Text = "Über MBCAN ..."
        '
        'ts_MenuInfoBedienungsanleitung
        '
        Me.ts_MenuInfoBedienungsanleitung.Name = "ts_MenuInfoBedienungsanleitung"
        Me.ts_MenuInfoBedienungsanleitung.Size = New System.Drawing.Size(186, 22)
        Me.ts_MenuInfoBedienungsanleitung.Text = "Bedienungsanleitung"
        '
        'ts_MenuInfoCopyright
        '
        Me.ts_MenuInfoCopyright.Name = "ts_MenuInfoCopyright"
        Me.ts_MenuInfoCopyright.Size = New System.Drawing.Size(186, 22)
        Me.ts_MenuInfoCopyright.Text = "Copyright"
        '
        'sc_Main
        '
        Me.sc_Main.BackColor = System.Drawing.Color.White
        Me.sc_Main.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.sc_Main.Dock = System.Windows.Forms.DockStyle.Top
        Me.sc_Main.Location = New System.Drawing.Point(7, 24)
        Me.sc_Main.Name = "sc_Main"
        '
        'sc_Main.Panel1
        '
        Me.sc_Main.Panel1.Controls.Add(Me.tv_Strukturbaum)
        '
        'sc_Main.Panel2
        '
        Me.sc_Main.Panel2.Controls.Add(Me.tab_Control)
        Me.sc_Main.Size = New System.Drawing.Size(924, 638)
        Me.sc_Main.SplitterDistance = 206
        Me.sc_Main.TabIndex = 1
        '
        'tv_Strukturbaum
        '
        Me.tv_Strukturbaum.Dock = System.Windows.Forms.DockStyle.Fill
        Me.tv_Strukturbaum.Location = New System.Drawing.Point(0, 0)
        Me.tv_Strukturbaum.Name = "tv_Strukturbaum"
        Me.tv_Strukturbaum.Size = New System.Drawing.Size(204, 636)
        Me.tv_Strukturbaum.TabIndex = 0
        '
        'tab_Control
        '
        Me.tab_Control.Controls.Add(Me.tab_Modul)
        Me.tab_Control.Controls.Add(Me.tab_Monitor)
        Me.tab_Control.Controls.Add(Me.tab_Adressen)
        Me.tab_Control.Dock = System.Windows.Forms.DockStyle.Fill
        Me.tab_Control.Location = New System.Drawing.Point(0, 0)
        Me.tab_Control.Name = "tab_Control"
        Me.tab_Control.SelectedIndex = 0
        Me.tab_Control.Size = New System.Drawing.Size(712, 636)
        Me.tab_Control.TabIndex = 0
        '
        'tab_Modul
        '
        Me.tab_Modul.BackColor = System.Drawing.Color.White
        Me.tab_Modul.Controls.Add(Me.btn_ControlUpdate)
        Me.tab_Modul.Controls.Add(Me.btn_ControlVerwerfen)
        Me.tab_Modul.Controls.Add(Me.btn_ControlUpgrade)
        Me.tab_Modul.Controls.Add(Me.btn_ControlLoeschen)
        Me.tab_Modul.Controls.Add(Me.btn_ControlReset)
        Me.tab_Modul.Controls.Add(Me.btn_ControlRefresh)
        Me.tab_Modul.Controls.Add(Me.tab_Module)
        Me.tab_Modul.Controls.Add(Me.gb_Keyboard)
        Me.tab_Modul.Controls.Add(Me.gb_Status)
        Me.tab_Modul.Controls.Add(Me.gb_Parameter)
        Me.tab_Modul.Location = New System.Drawing.Point(4, 22)
        Me.tab_Modul.Name = "tab_Modul"
        Me.tab_Modul.Padding = New System.Windows.Forms.Padding(3)
        Me.tab_Modul.Size = New System.Drawing.Size(704, 610)
        Me.tab_Modul.TabIndex = 1
        Me.tab_Modul.Text = "Modul"
        '
        'btn_ControlUpdate
        '
        Me.btn_ControlUpdate.Location = New System.Drawing.Point(250, 566)
        Me.btn_ControlUpdate.Name = "btn_ControlUpdate"
        Me.btn_ControlUpdate.Size = New System.Drawing.Size(96, 26)
        Me.btn_ControlUpdate.TabIndex = 71
        Me.btn_ControlUpdate.Text = "Update"
        Me.btn_ControlUpdate.UseVisualStyleBackColor = True
        '
        'btn_ControlVerwerfen
        '
        Me.btn_ControlVerwerfen.Location = New System.Drawing.Point(134, 566)
        Me.btn_ControlVerwerfen.Name = "btn_ControlVerwerfen"
        Me.btn_ControlVerwerfen.Size = New System.Drawing.Size(96, 26)
        Me.btn_ControlVerwerfen.TabIndex = 70
        Me.btn_ControlVerwerfen.Text = "Verwerfen"
        Me.btn_ControlVerwerfen.UseVisualStyleBackColor = True
        '
        'btn_ControlUpgrade
        '
        Me.btn_ControlUpgrade.Location = New System.Drawing.Point(366, 566)
        Me.btn_ControlUpgrade.Name = "btn_ControlUpgrade"
        Me.btn_ControlUpgrade.Size = New System.Drawing.Size(96, 26)
        Me.btn_ControlUpgrade.TabIndex = 69
        Me.btn_ControlUpgrade.Text = "Upgrade"
        Me.btn_ControlUpgrade.UseVisualStyleBackColor = True
        '
        'btn_ControlLoeschen
        '
        Me.btn_ControlLoeschen.Location = New System.Drawing.Point(598, 566)
        Me.btn_ControlLoeschen.Name = "btn_ControlLoeschen"
        Me.btn_ControlLoeschen.Size = New System.Drawing.Size(96, 26)
        Me.btn_ControlLoeschen.TabIndex = 68
        Me.btn_ControlLoeschen.Text = "Löschen"
        Me.btn_ControlLoeschen.UseVisualStyleBackColor = True
        '
        'btn_ControlReset
        '
        Me.btn_ControlReset.Location = New System.Drawing.Point(482, 566)
        Me.btn_ControlReset.Name = "btn_ControlReset"
        Me.btn_ControlReset.Size = New System.Drawing.Size(96, 26)
        Me.btn_ControlReset.TabIndex = 67
        Me.btn_ControlReset.Text = "Reset"
        Me.btn_ControlReset.UseVisualStyleBackColor = True
        '
        'btn_ControlRefresh
        '
        Me.btn_ControlRefresh.Location = New System.Drawing.Point(18, 566)
        Me.btn_ControlRefresh.Name = "btn_ControlRefresh"
        Me.btn_ControlRefresh.Size = New System.Drawing.Size(96, 26)
        Me.btn_ControlRefresh.TabIndex = 66
        Me.btn_ControlRefresh.Text = "Refresh"
        Me.btn_ControlRefresh.UseVisualStyleBackColor = True
        '
        'tab_Module
        '
        Me.tab_Module.Controls.Add(Me.tab_mbc80)
        Me.tab_Module.Controls.Add(Me.tab_mbc81)
        Me.tab_Module.Controls.Add(Me.tab_mbc82)
        Me.tab_Module.Controls.Add(Me.tab_mbc83)
        Me.tab_Module.Controls.Add(Me.tab_mbc84)
        Me.tab_Module.Controls.Add(Me.tab_mbc85)
        Me.tab_Module.Controls.Add(Me.tab_mbc86)
        Me.tab_Module.Controls.Add(Me.tab_mbc87)
        Me.tab_Module.Controls.Add(Me.tab_mbc88)
        Me.tab_Module.Controls.Add(Me.tab_mbc89)
        Me.tab_Module.Controls.Add(Me.tab_mbc90)
        Me.tab_Module.Controls.Add(Me.tab_mbc91)
        Me.tab_Module.Controls.Add(Me.tab_mbc92)
        Me.tab_Module.Controls.Add(Me.tab_mbc93)
        Me.tab_Module.Location = New System.Drawing.Point(14, 286)
        Me.tab_Module.Name = "tab_Module"
        Me.tab_Module.SelectedIndex = 0
        Me.tab_Module.Size = New System.Drawing.Size(688, 274)
        Me.tab_Module.TabIndex = 65
        '
        'tab_mbc80
        '
        Me.tab_mbc80.BackColor = System.Drawing.Color.White
        Me.tab_mbc80.Controls.Add(Me.gb_mbc80_SW)
        Me.tab_mbc80.Controls.Add(Me.gb_mbc80_AK)
        Me.tab_mbc80.Controls.Add(Me.gb_mbc80_BT)
        Me.tab_mbc80.Location = New System.Drawing.Point(4, 22)
        Me.tab_mbc80.Name = "tab_mbc80"
        Me.tab_mbc80.Padding = New System.Windows.Forms.Padding(3)
        Me.tab_mbc80.Size = New System.Drawing.Size(680, 248)
        Me.tab_mbc80.TabIndex = 0
        Me.tab_mbc80.Text = "mbc-80"
        '
        'gb_mbc80_SW
        '
        Me.gb_mbc80_SW.Controls.Add(Me.nud_mbc80_SW2)
        Me.gb_mbc80_SW.Controls.Add(Me.lbl_mbc80_SWPunkt)
        Me.gb_mbc80_SW.Controls.Add(Me.nud_mbc80_SW1)
        Me.gb_mbc80_SW.Controls.Add(Me.lbl_mbc80_SWBeschreibung)
        Me.gb_mbc80_SW.Controls.Add(Me.lbl_mbc80_SWBezeichnung)
        Me.gb_mbc80_SW.Location = New System.Drawing.Point(226, 85)
        Me.gb_mbc80_SW.Name = "gb_mbc80_SW"
        Me.gb_mbc80_SW.Size = New System.Drawing.Size(437, 80)
        Me.gb_mbc80_SW.TabIndex = 66
        Me.gb_mbc80_SW.TabStop = False
        Me.gb_mbc80_SW.Text = "L88-Softwareversion"
        '
        'nud_mbc80_SW2
        '
        Me.nud_mbc80_SW2.Location = New System.Drawing.Point(376, 37)
        Me.nud_mbc80_SW2.Maximum = New Decimal(New Integer() {255, 0, 0, 0})
        Me.nud_mbc80_SW2.Name = "nud_mbc80_SW2"
        Me.nud_mbc80_SW2.Size = New System.Drawing.Size(52, 20)
        Me.nud_mbc80_SW2.TabIndex = 70
        Me.nud_mbc80_SW2.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'lbl_mbc80_SWPunkt
        '
        Me.lbl_mbc80_SWPunkt.AutoSize = True
        Me.lbl_mbc80_SWPunkt.Font = New System.Drawing.Font("Microsoft Sans Serif", 14.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.lbl_mbc80_SWPunkt.Location = New System.Drawing.Point(365, 33)
        Me.lbl_mbc80_SWPunkt.Name = "lbl_mbc80_SWPunkt"
        Me.lbl_mbc80_SWPunkt.Size = New System.Drawing.Size(15, 24)
        Me.lbl_mbc80_SWPunkt.TabIndex = 71
        Me.lbl_mbc80_SWPunkt.Text = "."
        '
        'nud_mbc80_SW1
        '
        Me.nud_mbc80_SW1.Location = New System.Drawing.Point(311, 37)
        Me.nud_mbc80_SW1.Maximum = New Decimal(New Integer() {255, 0, 0, 0})
        Me.nud_mbc80_SW1.Name = "nud_mbc80_SW1"
        Me.nud_mbc80_SW1.Size = New System.Drawing.Size(52, 20)
        Me.nud_mbc80_SW1.TabIndex = 69
        Me.nud_mbc80_SW1.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'lbl_mbc80_SWBeschreibung
        '
        Me.lbl_mbc80_SWBeschreibung.BackColor = System.Drawing.SystemColors.Window
        Me.lbl_mbc80_SWBeschreibung.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.lbl_mbc80_SWBeschreibung.Location = New System.Drawing.Point(15, 24)
        Me.lbl_mbc80_SWBeschreibung.Multiline = True
        Me.lbl_mbc80_SWBeschreibung.Name = "lbl_mbc80_SWBeschreibung"
        Me.lbl_mbc80_SWBeschreibung.ReadOnly = True
        Me.lbl_mbc80_SWBeschreibung.Size = New System.Drawing.Size(193, 45)
        Me.lbl_mbc80_SWBeschreibung.TabIndex = 65
        Me.lbl_mbc80_SWBeschreibung.Text = "Die aktuelle Softwareversion des L88 von Märklin, um die Update-Aufforderung von " &
    "der CSx zu umgehen. "
        '
        'lbl_mbc80_SWBezeichnung
        '
        Me.lbl_mbc80_SWBezeichnung.AutoSize = True
        Me.lbl_mbc80_SWBezeichnung.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.lbl_mbc80_SWBezeichnung.Location = New System.Drawing.Point(224, 39)
        Me.lbl_mbc80_SWBezeichnung.Name = "lbl_mbc80_SWBezeichnung"
        Me.lbl_mbc80_SWBezeichnung.Size = New System.Drawing.Size(84, 13)
        Me.lbl_mbc80_SWBezeichnung.TabIndex = 63
        Me.lbl_mbc80_SWBezeichnung.Text = "L88-SW-Version"
        '
        'gb_mbc80_AK
        '
        Me.gb_mbc80_AK.Controls.Add(Me.lbl_mbc80_AK)
        Me.gb_mbc80_AK.Controls.Add(Me.txt_mbc80_AK)
        Me.gb_mbc80_AK.Location = New System.Drawing.Point(226, 24)
        Me.gb_mbc80_AK.Name = "gb_mbc80_AK"
        Me.gb_mbc80_AK.Size = New System.Drawing.Size(437, 47)
        Me.gb_mbc80_AK.TabIndex = 65
        Me.gb_mbc80_AK.TabStop = False
        Me.gb_mbc80_AK.Text = "s88-Automatikgerät"
        '
        'lbl_mbc80_AK
        '
        Me.lbl_mbc80_AK.BackColor = System.Drawing.SystemColors.Window
        Me.lbl_mbc80_AK.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.lbl_mbc80_AK.Location = New System.Drawing.Point(15, 21)
        Me.lbl_mbc80_AK.Multiline = True
        Me.lbl_mbc80_AK.Name = "lbl_mbc80_AK"
        Me.lbl_mbc80_AK.ReadOnly = True
        Me.lbl_mbc80_AK.Size = New System.Drawing.Size(236, 17)
        Me.lbl_mbc80_AK.TabIndex = 65
        Me.lbl_mbc80_AK.Text = "Durch CSx zugewiesener Automatik-Kenner:"
        '
        'txt_mbc80_AK
        '
        Me.txt_mbc80_AK.BackColor = System.Drawing.SystemColors.Control
        Me.txt_mbc80_AK.Location = New System.Drawing.Point(311, 18)
        Me.txt_mbc80_AK.Name = "txt_mbc80_AK"
        Me.txt_mbc80_AK.ReadOnly = True
        Me.txt_mbc80_AK.Size = New System.Drawing.Size(117, 20)
        Me.txt_mbc80_AK.TabIndex = 62
        Me.txt_mbc80_AK.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        '
        'gb_mbc80_BT
        '
        Me.gb_mbc80_BT.Controls.Add(Me.txt_mbc80_Passwort)
        Me.gb_mbc80_BT.Controls.Add(Me.lbl_mbc80_PasswortBeschreibung)
        Me.gb_mbc80_BT.Controls.Add(Me.lbl_mbc80_BTNameBeschreibung)
        Me.gb_mbc80_BT.Controls.Add(Me.txt_mbc80_Name)
        Me.gb_mbc80_BT.Controls.Add(Me.lbl_mbc80_Name)
        Me.gb_mbc80_BT.Controls.Add(Me.lbl_mbc80_Passwort)
        Me.gb_mbc80_BT.Location = New System.Drawing.Point(14, 24)
        Me.gb_mbc80_BT.Name = "gb_mbc80_BT"
        Me.gb_mbc80_BT.Size = New System.Drawing.Size(206, 200)
        Me.gb_mbc80_BT.TabIndex = 6
        Me.gb_mbc80_BT.TabStop = False
        Me.gb_mbc80_BT.Text = "Bluetooth"
        '
        'txt_mbc80_Passwort
        '
        Me.txt_mbc80_Passwort.CharacterCasing = System.Windows.Forms.CharacterCasing.Upper
        Me.txt_mbc80_Passwort.Location = New System.Drawing.Point(83, 148)
        Me.txt_mbc80_Passwort.MaxLength = 4
        Me.txt_mbc80_Passwort.Name = "txt_mbc80_Passwort"
        Me.txt_mbc80_Passwort.Size = New System.Drawing.Size(108, 20)
        Me.txt_mbc80_Passwort.TabIndex = 13
        Me.txt_mbc80_Passwort.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        '
        'lbl_mbc80_PasswortBeschreibung
        '
        Me.lbl_mbc80_PasswortBeschreibung.BackColor = System.Drawing.SystemColors.Window
        Me.lbl_mbc80_PasswortBeschreibung.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.lbl_mbc80_PasswortBeschreibung.Location = New System.Drawing.Point(15, 102)
        Me.lbl_mbc80_PasswortBeschreibung.Multiline = True
        Me.lbl_mbc80_PasswortBeschreibung.Name = "lbl_mbc80_PasswortBeschreibung"
        Me.lbl_mbc80_PasswortBeschreibung.ReadOnly = True
        Me.lbl_mbc80_PasswortBeschreibung.Size = New System.Drawing.Size(176, 32)
        Me.lbl_mbc80_PasswortBeschreibung.TabIndex = 12
        Me.lbl_mbc80_PasswortBeschreibung.Text = "Das Passwort ist numerisch im Bereich 0000 ... 9999 einstellbar."
        '
        'lbl_mbc80_BTNameBeschreibung
        '
        Me.lbl_mbc80_BTNameBeschreibung.BackColor = System.Drawing.SystemColors.Window
        Me.lbl_mbc80_BTNameBeschreibung.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.lbl_mbc80_BTNameBeschreibung.Location = New System.Drawing.Point(15, 21)
        Me.lbl_mbc80_BTNameBeschreibung.Multiline = True
        Me.lbl_mbc80_BTNameBeschreibung.Name = "lbl_mbc80_BTNameBeschreibung"
        Me.lbl_mbc80_BTNameBeschreibung.ReadOnly = True
        Me.lbl_mbc80_BTNameBeschreibung.Size = New System.Drawing.Size(176, 35)
        Me.lbl_mbc80_BTNameBeschreibung.TabIndex = 7
        Me.lbl_mbc80_BTNameBeschreibung.Text = "Die Länge des Modulnamens ist mit fünf Buchstaben fest vorgegeben. "
        '
        'txt_mbc80_Name
        '
        Me.txt_mbc80_Name.CharacterCasing = System.Windows.Forms.CharacterCasing.Upper
        Me.txt_mbc80_Name.Location = New System.Drawing.Point(83, 62)
        Me.txt_mbc80_Name.MaxLength = 5
        Me.txt_mbc80_Name.Name = "txt_mbc80_Name"
        Me.txt_mbc80_Name.Size = New System.Drawing.Size(108, 20)
        Me.txt_mbc80_Name.TabIndex = 7
        Me.txt_mbc80_Name.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        '
        'lbl_mbc80_Name
        '
        Me.lbl_mbc80_Name.AutoSize = True
        Me.lbl_mbc80_Name.Location = New System.Drawing.Point(12, 65)
        Me.lbl_mbc80_Name.Name = "lbl_mbc80_Name"
        Me.lbl_mbc80_Name.Size = New System.Drawing.Size(38, 13)
        Me.lbl_mbc80_Name.TabIndex = 6
        Me.lbl_mbc80_Name.Text = "Name:"
        '
        'lbl_mbc80_Passwort
        '
        Me.lbl_mbc80_Passwort.AutoSize = True
        Me.lbl_mbc80_Passwort.Location = New System.Drawing.Point(12, 151)
        Me.lbl_mbc80_Passwort.Name = "lbl_mbc80_Passwort"
        Me.lbl_mbc80_Passwort.Size = New System.Drawing.Size(53, 13)
        Me.lbl_mbc80_Passwort.TabIndex = 5
        Me.lbl_mbc80_Passwort.Text = "Passwort:"
        '
        'tab_mbc81
        '
        Me.tab_mbc81.BackColor = System.Drawing.Color.White
        Me.tab_mbc81.Controls.Add(Me.lbl_mbc81_Nutzung_Erlaeuterung_3)
        Me.tab_mbc81.Controls.Add(Me.lbl_mbc81_Nutzung_Erlaeuterung_2)
        Me.tab_mbc81.Controls.Add(Me.lbl_mbc81_Nutzung_Erlaeuterung_1)
        Me.tab_mbc81.Controls.Add(Me.lbl_mbc81_NutzungErlaeuterung)
        Me.tab_mbc81.Controls.Add(Me.lbl_mbc81_Nutzung)
        Me.tab_mbc81.Controls.Add(Me.cmb_mbc81_Nutzung)
        Me.tab_mbc81.Controls.Add(Me.lbl_mbc81_Adresse2)
        Me.tab_mbc81.Controls.Add(Me.txt_mbc81_Adresse2)
        Me.tab_mbc81.Controls.Add(Me.txt_mbc81_Adresse1)
        Me.tab_mbc81.Controls.Add(Me.lbl_mbc81_Adresse1)
        Me.tab_mbc81.Controls.Add(Me.nud_mbc81_LocID)
        Me.tab_mbc81.Controls.Add(Me.lbl_mbc81_LocID)
        Me.tab_mbc81.Location = New System.Drawing.Point(4, 22)
        Me.tab_mbc81.Name = "tab_mbc81"
        Me.tab_mbc81.Padding = New System.Windows.Forms.Padding(3)
        Me.tab_mbc81.Size = New System.Drawing.Size(680, 248)
        Me.tab_mbc81.TabIndex = 1
        Me.tab_mbc81.Text = "mbc-81"
        '
        'lbl_mbc81_Nutzung_Erlaeuterung_3
        '
        Me.lbl_mbc81_Nutzung_Erlaeuterung_3.AutoSize = True
        Me.lbl_mbc81_Nutzung_Erlaeuterung_3.Location = New System.Drawing.Point(14, 182)
        Me.lbl_mbc81_Nutzung_Erlaeuterung_3.Name = "lbl_mbc81_Nutzung_Erlaeuterung_3"
        Me.lbl_mbc81_Nutzung_Erlaeuterung_3.Size = New System.Drawing.Size(507, 13)
        Me.lbl_mbc81_Nutzung_Erlaeuterung_3.TabIndex = 49
        Me.lbl_mbc81_Nutzung_Erlaeuterung_3.Text = "CS3: Bei der Einstellung <Tastenmatrix> entsprechen die Kontakte 1-32 und 33-64 d" &
    "en jeweiligen Tasten. "
        '
        'lbl_mbc81_Nutzung_Erlaeuterung_2
        '
        Me.lbl_mbc81_Nutzung_Erlaeuterung_2.AutoSize = True
        Me.lbl_mbc81_Nutzung_Erlaeuterung_2.Location = New System.Drawing.Point(14, 146)
        Me.lbl_mbc81_Nutzung_Erlaeuterung_2.Name = "lbl_mbc81_Nutzung_Erlaeuterung_2"
        Me.lbl_mbc81_Nutzung_Erlaeuterung_2.Size = New System.Drawing.Size(490, 13)
        Me.lbl_mbc81_Nutzung_Erlaeuterung_2.TabIndex = 48
        Me.lbl_mbc81_Nutzung_Erlaeuterung_2.Text = "CS2: Die Tasten 1-32 entsprechen den Kontakten 101-132, die Tasten 33-64 den Kont" &
    "akte 133-164.  "
        '
        'lbl_mbc81_Nutzung_Erlaeuterung_1
        '
        Me.lbl_mbc81_Nutzung_Erlaeuterung_1.Location = New System.Drawing.Point(14, 85)
        Me.lbl_mbc81_Nutzung_Erlaeuterung_1.Name = "lbl_mbc81_Nutzung_Erlaeuterung_1"
        Me.lbl_mbc81_Nutzung_Erlaeuterung_1.Size = New System.Drawing.Size(641, 61)
        Me.lbl_mbc81_Nutzung_Erlaeuterung_1.TabIndex = 47
        Me.lbl_mbc81_Nutzung_Erlaeuterung_1.Text = resources.GetString("lbl_mbc81_Nutzung_Erlaeuterung_1.Text")
        '
        'lbl_mbc81_NutzungErlaeuterung
        '
        Me.lbl_mbc81_NutzungErlaeuterung.AutoSize = True
        Me.lbl_mbc81_NutzungErlaeuterung.Location = New System.Drawing.Point(11, 104)
        Me.lbl_mbc81_NutzungErlaeuterung.Name = "lbl_mbc81_NutzungErlaeuterung"
        Me.lbl_mbc81_NutzungErlaeuterung.Size = New System.Drawing.Size(0, 13)
        Me.lbl_mbc81_NutzungErlaeuterung.TabIndex = 46
        '
        'lbl_mbc81_Nutzung
        '
        Me.lbl_mbc81_Nutzung.AutoSize = True
        Me.lbl_mbc81_Nutzung.Location = New System.Drawing.Point(11, 29)
        Me.lbl_mbc81_Nutzung.Name = "lbl_mbc81_Nutzung"
        Me.lbl_mbc81_Nutzung.Size = New System.Drawing.Size(104, 13)
        Me.lbl_mbc81_Nutzung.TabIndex = 45
        Me.lbl_mbc81_Nutzung.Text = "Nutzung des Moduls"
        '
        'cmb_mbc81_Nutzung
        '
        Me.cmb_mbc81_Nutzung.FormattingEnabled = True
        Me.cmb_mbc81_Nutzung.Items.AddRange(New Object() {"Switchboard", "Bus 0 Matrix  1...32", "Bus 0 Matrix 33...64"})
        Me.cmb_mbc81_Nutzung.Location = New System.Drawing.Point(14, 45)
        Me.cmb_mbc81_Nutzung.Name = "cmb_mbc81_Nutzung"
        Me.cmb_mbc81_Nutzung.Size = New System.Drawing.Size(182, 21)
        Me.cmb_mbc81_Nutzung.TabIndex = 44
        '
        'lbl_mbc81_Adresse2
        '
        Me.lbl_mbc81_Adresse2.AutoSize = True
        Me.lbl_mbc81_Adresse2.Location = New System.Drawing.Point(561, 48)
        Me.lbl_mbc81_Adresse2.Name = "lbl_mbc81_Adresse2"
        Me.lbl_mbc81_Adresse2.Size = New System.Drawing.Size(20, 13)
        Me.lbl_mbc81_Adresse2.TabIndex = 43
        Me.lbl_mbc81_Adresse2.Text = "bis"
        '
        'txt_mbc81_Adresse2
        '
        Me.txt_mbc81_Adresse2.Location = New System.Drawing.Point(596, 45)
        Me.txt_mbc81_Adresse2.Name = "txt_mbc81_Adresse2"
        Me.txt_mbc81_Adresse2.ReadOnly = True
        Me.txt_mbc81_Adresse2.Size = New System.Drawing.Size(54, 20)
        Me.txt_mbc81_Adresse2.TabIndex = 42
        '
        'txt_mbc81_Adresse1
        '
        Me.txt_mbc81_Adresse1.Location = New System.Drawing.Point(493, 45)
        Me.txt_mbc81_Adresse1.Name = "txt_mbc81_Adresse1"
        Me.txt_mbc81_Adresse1.ReadOnly = True
        Me.txt_mbc81_Adresse1.Size = New System.Drawing.Size(54, 20)
        Me.txt_mbc81_Adresse1.TabIndex = 41
        '
        'lbl_mbc81_Adresse1
        '
        Me.lbl_mbc81_Adresse1.AutoSize = True
        Me.lbl_mbc81_Adresse1.Location = New System.Drawing.Point(435, 48)
        Me.lbl_mbc81_Adresse1.Name = "lbl_mbc81_Adresse1"
        Me.lbl_mbc81_Adresse1.Size = New System.Drawing.Size(48, 13)
        Me.lbl_mbc81_Adresse1.TabIndex = 40
        Me.lbl_mbc81_Adresse1.Text = "Adresse:"
        '
        'nud_mbc81_LocID
        '
        Me.nud_mbc81_LocID.Hexadecimal = True
        Me.nud_mbc81_LocID.Increment = New Decimal(New Integer() {16, 0, 0, 0})
        Me.nud_mbc81_LocID.Location = New System.Drawing.Point(314, 46)
        Me.nud_mbc81_LocID.Maximum = New Decimal(New Integer() {13296, 0, 0, 0})
        Me.nud_mbc81_LocID.Minimum = New Decimal(New Integer() {12288, 0, 0, 0})
        Me.nud_mbc81_LocID.Name = "nud_mbc81_LocID"
        Me.nud_mbc81_LocID.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc81_LocID.TabIndex = 9
        Me.nud_mbc81_LocID.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc81_LocID.Value = New Decimal(New Integer() {12288, 0, 0, 0})
        '
        'lbl_mbc81_LocID
        '
        Me.lbl_mbc81_LocID.AutoSize = True
        Me.lbl_mbc81_LocID.Location = New System.Drawing.Point(220, 48)
        Me.lbl_mbc81_LocID.Name = "lbl_mbc81_LocID"
        Me.lbl_mbc81_LocID.Size = New System.Drawing.Size(75, 13)
        Me.lbl_mbc81_LocID.TabIndex = 7
        Me.lbl_mbc81_LocID.Text = "Loc-ID in HEX"
        '
        'tab_mbc82
        '
        Me.tab_mbc82.BackColor = System.Drawing.Color.White
        Me.tab_mbc82.Controls.Add(Me.gb_mbc82_Latenz)
        Me.tab_mbc82.Controls.Add(Me.gb_mbc82_GFP)
        Me.tab_mbc82.Location = New System.Drawing.Point(4, 22)
        Me.tab_mbc82.Name = "tab_mbc82"
        Me.tab_mbc82.Padding = New System.Windows.Forms.Padding(3)
        Me.tab_mbc82.Size = New System.Drawing.Size(680, 248)
        Me.tab_mbc82.TabIndex = 2
        Me.tab_mbc82.Text = "mbc-82"
        '
        'gb_mbc82_Latenz
        '
        Me.gb_mbc82_Latenz.Controls.Add(Me.lbl_mbc82_LatenzBeschreibung_3)
        Me.gb_mbc82_Latenz.Controls.Add(Me.lbl_mbc82_LatenzBeschreibung_2)
        Me.gb_mbc82_Latenz.Controls.Add(Me.nud_mbc82_Latenz)
        Me.gb_mbc82_Latenz.Controls.Add(Me.lbl_mbc82_LatenzBeschreibung_1)
        Me.gb_mbc82_Latenz.Controls.Add(Me.lbl_mbc82_LatenzBezeichnung)
        Me.gb_mbc82_Latenz.Location = New System.Drawing.Point(338, 16)
        Me.gb_mbc82_Latenz.Name = "gb_mbc82_Latenz"
        Me.gb_mbc82_Latenz.Size = New System.Drawing.Size(317, 212)
        Me.gb_mbc82_Latenz.TabIndex = 69
        Me.gb_mbc82_Latenz.TabStop = False
        Me.gb_mbc82_Latenz.Text = "Latenzzeit Kurzschlusserkennung"
        '
        'lbl_mbc82_LatenzBeschreibung_3
        '
        Me.lbl_mbc82_LatenzBeschreibung_3.BackColor = System.Drawing.SystemColors.Window
        Me.lbl_mbc82_LatenzBeschreibung_3.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.lbl_mbc82_LatenzBeschreibung_3.Location = New System.Drawing.Point(9, 125)
        Me.lbl_mbc82_LatenzBeschreibung_3.Multiline = True
        Me.lbl_mbc82_LatenzBeschreibung_3.Name = "lbl_mbc82_LatenzBeschreibung_3"
        Me.lbl_mbc82_LatenzBeschreibung_3.ReadOnly = True
        Me.lbl_mbc82_LatenzBeschreibung_3.Size = New System.Drawing.Size(302, 42)
        Me.lbl_mbc82_LatenzBeschreibung_3.TabIndex = 71
        Me.lbl_mbc82_LatenzBeschreibung_3.Text = "Bei einer CS2 bitte diesen Wert ausschließlich in der CS2-GUI ändern und über den" &
    " <Refresh>-Button aktualisieren. " & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10)
        '
        'lbl_mbc82_LatenzBeschreibung_2
        '
        Me.lbl_mbc82_LatenzBeschreibung_2.BackColor = System.Drawing.SystemColors.Window
        Me.lbl_mbc82_LatenzBeschreibung_2.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.lbl_mbc82_LatenzBeschreibung_2.Location = New System.Drawing.Point(9, 74)
        Me.lbl_mbc82_LatenzBeschreibung_2.Multiline = True
        Me.lbl_mbc82_LatenzBeschreibung_2.Name = "lbl_mbc82_LatenzBeschreibung_2"
        Me.lbl_mbc82_LatenzBeschreibung_2.ReadOnly = True
        Me.lbl_mbc82_LatenzBeschreibung_2.Size = New System.Drawing.Size(302, 45)
        Me.lbl_mbc82_LatenzBeschreibung_2.TabIndex = 70
        Me.lbl_mbc82_LatenzBeschreibung_2.Text = "Nach dem <Update> des Moduls bitte dieses aus der GUI der CS3 löschen. Der neue L" &
    "atenzwert wird dann in der CS3 angezeigt."
        '
        'nud_mbc82_Latenz
        '
        Me.nud_mbc82_Latenz.Increment = New Decimal(New Integer() {50, 0, 0, 0})
        Me.nud_mbc82_Latenz.Location = New System.Drawing.Point(75, 174)
        Me.nud_mbc82_Latenz.Maximum = New Decimal(New Integer() {1000, 0, 0, 0})
        Me.nud_mbc82_Latenz.Minimum = New Decimal(New Integer() {50, 0, 0, 0})
        Me.nud_mbc82_Latenz.Name = "nud_mbc82_Latenz"
        Me.nud_mbc82_Latenz.Size = New System.Drawing.Size(52, 20)
        Me.nud_mbc82_Latenz.TabIndex = 69
        Me.nud_mbc82_Latenz.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        Me.nud_mbc82_Latenz.Value = New Decimal(New Integer() {50, 0, 0, 0})
        '
        'lbl_mbc82_LatenzBeschreibung_1
        '
        Me.lbl_mbc82_LatenzBeschreibung_1.BackColor = System.Drawing.SystemColors.Window
        Me.lbl_mbc82_LatenzBeschreibung_1.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.lbl_mbc82_LatenzBeschreibung_1.Location = New System.Drawing.Point(9, 32)
        Me.lbl_mbc82_LatenzBeschreibung_1.Multiline = True
        Me.lbl_mbc82_LatenzBeschreibung_1.Name = "lbl_mbc82_LatenzBeschreibung_1"
        Me.lbl_mbc82_LatenzBeschreibung_1.ReadOnly = True
        Me.lbl_mbc82_LatenzBeschreibung_1.Size = New System.Drawing.Size(302, 36)
        Me.lbl_mbc82_LatenzBeschreibung_1.TabIndex = 65
        Me.lbl_mbc82_LatenzBeschreibung_1.Text = "Die Latenzzeit bis zum Ansprechen der Kurzschlusserkennung kann hier in Schritten" &
    " von 50 ms eingestellt werden."
        '
        'lbl_mbc82_LatenzBezeichnung
        '
        Me.lbl_mbc82_LatenzBezeichnung.AutoSize = True
        Me.lbl_mbc82_LatenzBezeichnung.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.lbl_mbc82_LatenzBezeichnung.Location = New System.Drawing.Point(8, 176)
        Me.lbl_mbc82_LatenzBezeichnung.Name = "lbl_mbc82_LatenzBezeichnung"
        Me.lbl_mbc82_LatenzBezeichnung.Size = New System.Drawing.Size(61, 13)
        Me.lbl_mbc82_LatenzBezeichnung.TabIndex = 63
        Me.lbl_mbc82_LatenzBezeichnung.Text = "Latenz [ms]"
        '
        'gb_mbc82_GFP
        '
        Me.gb_mbc82_GFP.Controls.Add(Me.lbl_mbc82_GFPBeschreibung_2)
        Me.gb_mbc82_GFP.Controls.Add(Me.lbl_mbc82_GFPPunkt)
        Me.gb_mbc82_GFP.Controls.Add(Me.nud_mbc82_GFPSW2)
        Me.gb_mbc82_GFP.Controls.Add(Me.nud_mbc82_GFPSW1)
        Me.gb_mbc82_GFP.Controls.Add(Me.lbl_mbc82_GFPBeschreibung_1)
        Me.gb_mbc82_GFP.Controls.Add(Me.lbl_mbc82_GFPBezeichnung)
        Me.gb_mbc82_GFP.Location = New System.Drawing.Point(19, 16)
        Me.gb_mbc82_GFP.Name = "gb_mbc82_GFP"
        Me.gb_mbc82_GFP.Size = New System.Drawing.Size(270, 212)
        Me.gb_mbc82_GFP.TabIndex = 67
        Me.gb_mbc82_GFP.TabStop = False
        Me.gb_mbc82_GFP.Text = "GFP-Softwareversion"
        '
        'lbl_mbc82_GFPBeschreibung_2
        '
        Me.lbl_mbc82_GFPBeschreibung_2.BackColor = System.Drawing.SystemColors.Window
        Me.lbl_mbc82_GFPBeschreibung_2.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.lbl_mbc82_GFPBeschreibung_2.Location = New System.Drawing.Point(9, 86)
        Me.lbl_mbc82_GFPBeschreibung_2.Multiline = True
        Me.lbl_mbc82_GFPBeschreibung_2.Name = "lbl_mbc82_GFPBeschreibung_2"
        Me.lbl_mbc82_GFPBeschreibung_2.ReadOnly = True
        Me.lbl_mbc82_GFPBeschreibung_2.Size = New System.Drawing.Size(231, 50)
        Me.lbl_mbc82_GFPBeschreibung_2.TabIndex = 69
        Me.lbl_mbc82_GFPBeschreibung_2.Text = "Nach dem <Update> das Modul bitte per <Reset> neu starten. Die neue Softwareversi" &
    "on wird dann in der CSx angezeigt."
        '
        'lbl_mbc82_GFPPunkt
        '
        Me.lbl_mbc82_GFPPunkt.AutoSize = True
        Me.lbl_mbc82_GFPPunkt.Font = New System.Drawing.Font("Microsoft Sans Serif", 14.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.lbl_mbc82_GFPPunkt.Location = New System.Drawing.Point(153, 170)
        Me.lbl_mbc82_GFPPunkt.Name = "lbl_mbc82_GFPPunkt"
        Me.lbl_mbc82_GFPPunkt.Size = New System.Drawing.Size(15, 24)
        Me.lbl_mbc82_GFPPunkt.TabIndex = 68
        Me.lbl_mbc82_GFPPunkt.Text = "."
        '
        'nud_mbc82_GFPSW2
        '
        Me.nud_mbc82_GFPSW2.Location = New System.Drawing.Point(173, 174)
        Me.nud_mbc82_GFPSW2.Maximum = New Decimal(New Integer() {255, 0, 0, 0})
        Me.nud_mbc82_GFPSW2.Name = "nud_mbc82_GFPSW2"
        Me.nud_mbc82_GFPSW2.Size = New System.Drawing.Size(52, 20)
        Me.nud_mbc82_GFPSW2.TabIndex = 67
        Me.nud_mbc82_GFPSW2.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'nud_mbc82_GFPSW1
        '
        Me.nud_mbc82_GFPSW1.Location = New System.Drawing.Point(99, 174)
        Me.nud_mbc82_GFPSW1.Maximum = New Decimal(New Integer() {255, 0, 0, 0})
        Me.nud_mbc82_GFPSW1.Name = "nud_mbc82_GFPSW1"
        Me.nud_mbc82_GFPSW1.Size = New System.Drawing.Size(52, 20)
        Me.nud_mbc82_GFPSW1.TabIndex = 66
        Me.nud_mbc82_GFPSW1.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'lbl_mbc82_GFPBeschreibung_1
        '
        Me.lbl_mbc82_GFPBeschreibung_1.BackColor = System.Drawing.SystemColors.Window
        Me.lbl_mbc82_GFPBeschreibung_1.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.lbl_mbc82_GFPBeschreibung_1.Location = New System.Drawing.Point(9, 32)
        Me.lbl_mbc82_GFPBeschreibung_1.Multiline = True
        Me.lbl_mbc82_GFPBeschreibung_1.Name = "lbl_mbc82_GFPBeschreibung_1"
        Me.lbl_mbc82_GFPBeschreibung_1.ReadOnly = True
        Me.lbl_mbc82_GFPBeschreibung_1.Size = New System.Drawing.Size(231, 48)
        Me.lbl_mbc82_GFPBeschreibung_1.TabIndex = 65
        Me.lbl_mbc82_GFPBeschreibung_1.Text = "Die aktuelle Softwareversion des GFP/Booster von Märklin, um die Update-Aufforder" &
    "ung von der CSx zu umgehen."
        '
        'lbl_mbc82_GFPBezeichnung
        '
        Me.lbl_mbc82_GFPBezeichnung.AutoSize = True
        Me.lbl_mbc82_GFPBezeichnung.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.lbl_mbc82_GFPBezeichnung.Location = New System.Drawing.Point(6, 176)
        Me.lbl_mbc82_GFPBezeichnung.Name = "lbl_mbc82_GFPBezeichnung"
        Me.lbl_mbc82_GFPBezeichnung.Size = New System.Drawing.Size(87, 13)
        Me.lbl_mbc82_GFPBezeichnung.TabIndex = 63
        Me.lbl_mbc82_GFPBezeichnung.Text = "GFP-SW-Version"
        '
        'tab_mbc83
        '
        Me.tab_mbc83.BackColor = System.Drawing.Color.White
        Me.tab_mbc83.Controls.Add(Me.lbl_mbc83_Startup)
        Me.tab_mbc83.Controls.Add(Me.pic_mbc83_Startup)
        Me.tab_mbc83.Controls.Add(Me.lbl_mbc83_Wechsel)
        Me.tab_mbc83.Controls.Add(Me.lbl_mbc83_Impuls)
        Me.tab_mbc83.Controls.Add(Me.pic_mbc83_Wechsel)
        Me.tab_mbc83.Controls.Add(Me.pic_mbc83_Impuls)
        Me.tab_mbc83.Controls.Add(Me.lbl_mbc83_ms)
        Me.tab_mbc83.Controls.Add(Me.lbl_mbc83_Meldezyklus)
        Me.tab_mbc83.Controls.Add(Me.lbl_mbc83_AutoAus)
        Me.tab_mbc83.Controls.Add(Me.nud_mbc83_Rueck)
        Me.tab_mbc83.Controls.Add(Me.nud_mbc83_Aus)
        Me.tab_mbc83.Controls.Add(Me.lbl_mbc83_Dimmer)
        Me.tab_mbc83.Controls.Add(Me.nud_mbc83_Dimmer)
        Me.tab_mbc83.Controls.Add(Me.gb_mbc83_CH4)
        Me.tab_mbc83.Controls.Add(Me.gb_mbc83_CH3)
        Me.tab_mbc83.Controls.Add(Me.gb_mbc83_CH2)
        Me.tab_mbc83.Controls.Add(Me.gb_mbc83_CH1)
        Me.tab_mbc83.Controls.Add(Me.nud_mbc83_MMAddr)
        Me.tab_mbc83.Controls.Add(Me.lbl_mbc83_LocID)
        Me.tab_mbc83.Location = New System.Drawing.Point(4, 22)
        Me.tab_mbc83.Name = "tab_mbc83"
        Me.tab_mbc83.Padding = New System.Windows.Forms.Padding(3)
        Me.tab_mbc83.Size = New System.Drawing.Size(680, 248)
        Me.tab_mbc83.TabIndex = 3
        Me.tab_mbc83.Text = "mbc-83"
        '
        'lbl_mbc83_Startup
        '
        Me.lbl_mbc83_Startup.AutoSize = True
        Me.lbl_mbc83_Startup.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.lbl_mbc83_Startup.Location = New System.Drawing.Point(379, 218)
        Me.lbl_mbc83_Startup.Name = "lbl_mbc83_Startup"
        Me.lbl_mbc83_Startup.Size = New System.Drawing.Size(75, 13)
        Me.lbl_mbc83_Startup.TabIndex = 37
        Me.lbl_mbc83_Startup.Text = "StartUp-Signal"
        '
        'pic_mbc83_Startup
        '
        Me.pic_mbc83_Startup.InitialImage = Global.MBCAN.My.Resources.Resources.img_wechsel
        Me.pic_mbc83_Startup.Location = New System.Drawing.Point(344, 210)
        Me.pic_mbc83_Startup.Name = "pic_mbc83_Startup"
        Me.pic_mbc83_Startup.Size = New System.Drawing.Size(26, 26)
        Me.pic_mbc83_Startup.TabIndex = 36
        Me.pic_mbc83_Startup.TabStop = False
        '
        'lbl_mbc83_Wechsel
        '
        Me.lbl_mbc83_Wechsel.AutoSize = True
        Me.lbl_mbc83_Wechsel.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.lbl_mbc83_Wechsel.Location = New System.Drawing.Point(214, 218)
        Me.lbl_mbc83_Wechsel.Name = "lbl_mbc83_Wechsel"
        Me.lbl_mbc83_Wechsel.Size = New System.Drawing.Size(86, 13)
        Me.lbl_mbc83_Wechsel.TabIndex = 31
        Me.lbl_mbc83_Wechsel.Text = "Wechselschalter"
        '
        'lbl_mbc83_Impuls
        '
        Me.lbl_mbc83_Impuls.AutoSize = True
        Me.lbl_mbc83_Impuls.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.lbl_mbc83_Impuls.Location = New System.Drawing.Point(46, 218)
        Me.lbl_mbc83_Impuls.Name = "lbl_mbc83_Impuls"
        Me.lbl_mbc83_Impuls.Size = New System.Drawing.Size(78, 13)
        Me.lbl_mbc83_Impuls.TabIndex = 30
        Me.lbl_mbc83_Impuls.Text = "Impulsausgang"
        '
        'pic_mbc83_Wechsel
        '
        Me.pic_mbc83_Wechsel.InitialImage = Global.MBCAN.My.Resources.Resources.img_wechsel
        Me.pic_mbc83_Wechsel.Location = New System.Drawing.Point(179, 210)
        Me.pic_mbc83_Wechsel.Name = "pic_mbc83_Wechsel"
        Me.pic_mbc83_Wechsel.Size = New System.Drawing.Size(26, 26)
        Me.pic_mbc83_Wechsel.TabIndex = 29
        Me.pic_mbc83_Wechsel.TabStop = False
        '
        'pic_mbc83_Impuls
        '
        Me.pic_mbc83_Impuls.InitialImage = Global.MBCAN.My.Resources.Resources.img_impuls
        Me.pic_mbc83_Impuls.Location = New System.Drawing.Point(12, 210)
        Me.pic_mbc83_Impuls.Name = "pic_mbc83_Impuls"
        Me.pic_mbc83_Impuls.Size = New System.Drawing.Size(26, 26)
        Me.pic_mbc83_Impuls.TabIndex = 28
        Me.pic_mbc83_Impuls.TabStop = False
        '
        'lbl_mbc83_ms
        '
        Me.lbl_mbc83_ms.AutoSize = True
        Me.lbl_mbc83_ms.Location = New System.Drawing.Point(470, 23)
        Me.lbl_mbc83_ms.Name = "lbl_mbc83_ms"
        Me.lbl_mbc83_ms.Size = New System.Drawing.Size(26, 13)
        Me.lbl_mbc83_ms.TabIndex = 11
        Me.lbl_mbc83_ms.Text = "[ms]"
        '
        'lbl_mbc83_Meldezyklus
        '
        Me.lbl_mbc83_Meldezyklus.AutoSize = True
        Me.lbl_mbc83_Meldezyklus.Location = New System.Drawing.Point(521, 23)
        Me.lbl_mbc83_Meldezyklus.Name = "lbl_mbc83_Meldezyklus"
        Me.lbl_mbc83_Meldezyklus.Size = New System.Drawing.Size(65, 13)
        Me.lbl_mbc83_Meldezyklus.TabIndex = 19
        Me.lbl_mbc83_Meldezyklus.Text = "Meldezyklus"
        '
        'lbl_mbc83_AutoAus
        '
        Me.lbl_mbc83_AutoAus.AutoSize = True
        Me.lbl_mbc83_AutoAus.Location = New System.Drawing.Point(353, 23)
        Me.lbl_mbc83_AutoAus.Name = "lbl_mbc83_AutoAus"
        Me.lbl_mbc83_AutoAus.Size = New System.Drawing.Size(54, 13)
        Me.lbl_mbc83_AutoAus.TabIndex = 18
        Me.lbl_mbc83_AutoAus.Text = "Auto-AUS"
        '
        'nud_mbc83_Rueck
        '
        Me.nud_mbc83_Rueck.Location = New System.Drawing.Point(593, 21)
        Me.nud_mbc83_Rueck.Maximum = New Decimal(New Integer() {1, 0, 0, 0})
        Me.nud_mbc83_Rueck.Name = "nud_mbc83_Rueck"
        Me.nud_mbc83_Rueck.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc83_Rueck.TabIndex = 17
        Me.nud_mbc83_Rueck.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        '
        'nud_mbc83_Aus
        '
        Me.nud_mbc83_Aus.Increment = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc83_Aus.Location = New System.Drawing.Point(411, 21)
        Me.nud_mbc83_Aus.Maximum = New Decimal(New Integer() {2000, 0, 0, 0})
        Me.nud_mbc83_Aus.Name = "nud_mbc83_Aus"
        Me.nud_mbc83_Aus.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc83_Aus.TabIndex = 16
        Me.nud_mbc83_Aus.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        '
        'lbl_mbc83_Dimmer
        '
        Me.lbl_mbc83_Dimmer.AutoSize = True
        Me.lbl_mbc83_Dimmer.Location = New System.Drawing.Point(187, 23)
        Me.lbl_mbc83_Dimmer.Name = "lbl_mbc83_Dimmer"
        Me.lbl_mbc83_Dimmer.Size = New System.Drawing.Size(81, 13)
        Me.lbl_mbc83_Dimmer.TabIndex = 15
        Me.lbl_mbc83_Dimmer.Text = "Dimmer StartUp"
        '
        'nud_mbc83_Dimmer
        '
        Me.nud_mbc83_Dimmer.Location = New System.Drawing.Point(275, 21)
        Me.nud_mbc83_Dimmer.Maximum = New Decimal(New Integer() {200, 0, 0, 0})
        Me.nud_mbc83_Dimmer.Name = "nud_mbc83_Dimmer"
        Me.nud_mbc83_Dimmer.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc83_Dimmer.TabIndex = 14
        Me.nud_mbc83_Dimmer.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        '
        'gb_mbc83_CH4
        '
        Me.gb_mbc83_CH4.Controls.Add(Me.gb_mbc83_CH4_txt_Adresse)
        Me.gb_mbc83_CH4.Controls.Add(Me.gb_mbc83_CH4_lbl_Adresse)
        Me.gb_mbc83_CH4.Controls.Add(Me.gb_mbc83_CH4_lbl_Typ)
        Me.gb_mbc83_CH4.Controls.Add(Me.gb_mbc83_CH4_clb_Typ)
        Me.gb_mbc83_CH4.Controls.Add(Me.gb_mbc83_CH4_nud_TIO)
        Me.gb_mbc83_CH4.Controls.Add(Me.gb_mbc83_CH4_lbl_ON)
        Me.gb_mbc83_CH4.Controls.Add(Me.gb_mbc83_CH4_lbl_ms)
        Me.gb_mbc83_CH4.Location = New System.Drawing.Point(510, 62)
        Me.gb_mbc83_CH4.Name = "gb_mbc83_CH4"
        Me.gb_mbc83_CH4.Size = New System.Drawing.Size(155, 142)
        Me.gb_mbc83_CH4.TabIndex = 13
        Me.gb_mbc83_CH4.TabStop = False
        Me.gb_mbc83_CH4.Text = "Ausgang 4"
        '
        'gb_mbc83_CH4_txt_Adresse
        '
        Me.gb_mbc83_CH4_txt_Adresse.Location = New System.Drawing.Point(66, 24)
        Me.gb_mbc83_CH4_txt_Adresse.Name = "gb_mbc83_CH4_txt_Adresse"
        Me.gb_mbc83_CH4_txt_Adresse.ReadOnly = True
        Me.gb_mbc83_CH4_txt_Adresse.Size = New System.Drawing.Size(54, 20)
        Me.gb_mbc83_CH4_txt_Adresse.TabIndex = 45
        '
        'gb_mbc83_CH4_lbl_Adresse
        '
        Me.gb_mbc83_CH4_lbl_Adresse.AutoSize = True
        Me.gb_mbc83_CH4_lbl_Adresse.Location = New System.Drawing.Point(8, 27)
        Me.gb_mbc83_CH4_lbl_Adresse.Name = "gb_mbc83_CH4_lbl_Adresse"
        Me.gb_mbc83_CH4_lbl_Adresse.Size = New System.Drawing.Size(48, 13)
        Me.gb_mbc83_CH4_lbl_Adresse.TabIndex = 44
        Me.gb_mbc83_CH4_lbl_Adresse.Text = "Adresse:"
        '
        'gb_mbc83_CH4_lbl_Typ
        '
        Me.gb_mbc83_CH4_lbl_Typ.AutoSize = True
        Me.gb_mbc83_CH4_lbl_Typ.Location = New System.Drawing.Point(8, 55)
        Me.gb_mbc83_CH4_lbl_Typ.Name = "gb_mbc83_CH4_lbl_Typ"
        Me.gb_mbc83_CH4_lbl_Typ.Size = New System.Drawing.Size(28, 13)
        Me.gb_mbc83_CH4_lbl_Typ.TabIndex = 15
        Me.gb_mbc83_CH4_lbl_Typ.Text = "Typ:"
        '
        'gb_mbc83_CH4_clb_Typ
        '
        Me.gb_mbc83_CH4_clb_Typ.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.gb_mbc83_CH4_clb_Typ.CheckOnClick = True
        Me.gb_mbc83_CH4_clb_Typ.ColumnWidth = 60
        Me.gb_mbc83_CH4_clb_Typ.FormattingEnabled = True
        Me.gb_mbc83_CH4_clb_Typ.Items.AddRange(New Object() {"Impuls", "Wechsel", "StartUp"})
        Me.gb_mbc83_CH4_clb_Typ.Location = New System.Drawing.Point(66, 55)
        Me.gb_mbc83_CH4_clb_Typ.Name = "gb_mbc83_CH4_clb_Typ"
        Me.gb_mbc83_CH4_clb_Typ.Size = New System.Drawing.Size(77, 45)
        Me.gb_mbc83_CH4_clb_Typ.TabIndex = 14
        Me.gb_mbc83_CH4_clb_Typ.TabStop = False
        '
        'gb_mbc83_CH4_nud_TIO
        '
        Me.gb_mbc83_CH4_nud_TIO.Increment = New Decimal(New Integer() {10, 0, 0, 0})
        Me.gb_mbc83_CH4_nud_TIO.Location = New System.Drawing.Point(66, 108)
        Me.gb_mbc83_CH4_nud_TIO.Maximum = New Decimal(New Integer() {2500, 0, 0, 0})
        Me.gb_mbc83_CH4_nud_TIO.Minimum = New Decimal(New Integer() {10, 0, 0, 0})
        Me.gb_mbc83_CH4_nud_TIO.Name = "gb_mbc83_CH4_nud_TIO"
        Me.gb_mbc83_CH4_nud_TIO.Size = New System.Drawing.Size(53, 20)
        Me.gb_mbc83_CH4_nud_TIO.TabIndex = 8
        Me.gb_mbc83_CH4_nud_TIO.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.gb_mbc83_CH4_nud_TIO.Value = New Decimal(New Integer() {200, 0, 0, 0})
        '
        'gb_mbc83_CH4_lbl_ON
        '
        Me.gb_mbc83_CH4_lbl_ON.AutoSize = True
        Me.gb_mbc83_CH4_lbl_ON.Location = New System.Drawing.Point(8, 110)
        Me.gb_mbc83_CH4_lbl_ON.Name = "gb_mbc83_CH4_lbl_ON"
        Me.gb_mbc83_CH4_lbl_ON.Size = New System.Drawing.Size(52, 13)
        Me.gb_mbc83_CH4_lbl_ON.TabIndex = 9
        Me.gb_mbc83_CH4_lbl_ON.Text = "ON-Time:"
        '
        'gb_mbc83_CH4_lbl_ms
        '
        Me.gb_mbc83_CH4_lbl_ms.AutoSize = True
        Me.gb_mbc83_CH4_lbl_ms.Location = New System.Drawing.Point(125, 110)
        Me.gb_mbc83_CH4_lbl_ms.Name = "gb_mbc83_CH4_lbl_ms"
        Me.gb_mbc83_CH4_lbl_ms.Size = New System.Drawing.Size(26, 13)
        Me.gb_mbc83_CH4_lbl_ms.TabIndex = 10
        Me.gb_mbc83_CH4_lbl_ms.Text = "[ms]"
        '
        'gb_mbc83_CH3
        '
        Me.gb_mbc83_CH3.Controls.Add(Me.gb_mbc83_CH3_txt_Adresse)
        Me.gb_mbc83_CH3.Controls.Add(Me.gb_mbc83_CH3_lbl_Adresse)
        Me.gb_mbc83_CH3.Controls.Add(Me.gb_mbc83_CH3_lbl_Typ)
        Me.gb_mbc83_CH3.Controls.Add(Me.gb_mbc83_CH3_clb_Typ)
        Me.gb_mbc83_CH3.Controls.Add(Me.gb_mbc83_CH3_nud_TIO)
        Me.gb_mbc83_CH3.Controls.Add(Me.gb_mbc83_CH3_lbl_ON)
        Me.gb_mbc83_CH3.Controls.Add(Me.gb_mbc83_CH3_lbl_ms)
        Me.gb_mbc83_CH3.Location = New System.Drawing.Point(344, 62)
        Me.gb_mbc83_CH3.Name = "gb_mbc83_CH3"
        Me.gb_mbc83_CH3.Size = New System.Drawing.Size(155, 142)
        Me.gb_mbc83_CH3.TabIndex = 13
        Me.gb_mbc83_CH3.TabStop = False
        Me.gb_mbc83_CH3.Text = "Ausgang 3"
        '
        'gb_mbc83_CH3_txt_Adresse
        '
        Me.gb_mbc83_CH3_txt_Adresse.Location = New System.Drawing.Point(67, 24)
        Me.gb_mbc83_CH3_txt_Adresse.Name = "gb_mbc83_CH3_txt_Adresse"
        Me.gb_mbc83_CH3_txt_Adresse.ReadOnly = True
        Me.gb_mbc83_CH3_txt_Adresse.Size = New System.Drawing.Size(54, 20)
        Me.gb_mbc83_CH3_txt_Adresse.TabIndex = 43
        '
        'gb_mbc83_CH3_lbl_Adresse
        '
        Me.gb_mbc83_CH3_lbl_Adresse.AutoSize = True
        Me.gb_mbc83_CH3_lbl_Adresse.Location = New System.Drawing.Point(9, 27)
        Me.gb_mbc83_CH3_lbl_Adresse.Name = "gb_mbc83_CH3_lbl_Adresse"
        Me.gb_mbc83_CH3_lbl_Adresse.Size = New System.Drawing.Size(48, 13)
        Me.gb_mbc83_CH3_lbl_Adresse.TabIndex = 42
        Me.gb_mbc83_CH3_lbl_Adresse.Text = "Adresse:"
        '
        'gb_mbc83_CH3_lbl_Typ
        '
        Me.gb_mbc83_CH3_lbl_Typ.AutoSize = True
        Me.gb_mbc83_CH3_lbl_Typ.Location = New System.Drawing.Point(9, 55)
        Me.gb_mbc83_CH3_lbl_Typ.Name = "gb_mbc83_CH3_lbl_Typ"
        Me.gb_mbc83_CH3_lbl_Typ.Size = New System.Drawing.Size(28, 13)
        Me.gb_mbc83_CH3_lbl_Typ.TabIndex = 15
        Me.gb_mbc83_CH3_lbl_Typ.Text = "Typ:"
        '
        'gb_mbc83_CH3_clb_Typ
        '
        Me.gb_mbc83_CH3_clb_Typ.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.gb_mbc83_CH3_clb_Typ.CheckOnClick = True
        Me.gb_mbc83_CH3_clb_Typ.ColumnWidth = 60
        Me.gb_mbc83_CH3_clb_Typ.FormattingEnabled = True
        Me.gb_mbc83_CH3_clb_Typ.Items.AddRange(New Object() {"Impuls", "Wechsel", "StartUp"})
        Me.gb_mbc83_CH3_clb_Typ.Location = New System.Drawing.Point(67, 55)
        Me.gb_mbc83_CH3_clb_Typ.Name = "gb_mbc83_CH3_clb_Typ"
        Me.gb_mbc83_CH3_clb_Typ.Size = New System.Drawing.Size(77, 45)
        Me.gb_mbc83_CH3_clb_Typ.TabIndex = 14
        Me.gb_mbc83_CH3_clb_Typ.TabStop = False
        '
        'gb_mbc83_CH3_nud_TIO
        '
        Me.gb_mbc83_CH3_nud_TIO.Increment = New Decimal(New Integer() {10, 0, 0, 0})
        Me.gb_mbc83_CH3_nud_TIO.Location = New System.Drawing.Point(67, 108)
        Me.gb_mbc83_CH3_nud_TIO.Maximum = New Decimal(New Integer() {2500, 0, 0, 0})
        Me.gb_mbc83_CH3_nud_TIO.Minimum = New Decimal(New Integer() {10, 0, 0, 0})
        Me.gb_mbc83_CH3_nud_TIO.Name = "gb_mbc83_CH3_nud_TIO"
        Me.gb_mbc83_CH3_nud_TIO.Size = New System.Drawing.Size(53, 20)
        Me.gb_mbc83_CH3_nud_TIO.TabIndex = 8
        Me.gb_mbc83_CH3_nud_TIO.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.gb_mbc83_CH3_nud_TIO.Value = New Decimal(New Integer() {200, 0, 0, 0})
        '
        'gb_mbc83_CH3_lbl_ON
        '
        Me.gb_mbc83_CH3_lbl_ON.AutoSize = True
        Me.gb_mbc83_CH3_lbl_ON.Location = New System.Drawing.Point(9, 110)
        Me.gb_mbc83_CH3_lbl_ON.Name = "gb_mbc83_CH3_lbl_ON"
        Me.gb_mbc83_CH3_lbl_ON.Size = New System.Drawing.Size(52, 13)
        Me.gb_mbc83_CH3_lbl_ON.TabIndex = 9
        Me.gb_mbc83_CH3_lbl_ON.Text = "ON-Time:"
        '
        'gb_mbc83_CH3_lbl_ms
        '
        Me.gb_mbc83_CH3_lbl_ms.AutoSize = True
        Me.gb_mbc83_CH3_lbl_ms.Location = New System.Drawing.Point(126, 110)
        Me.gb_mbc83_CH3_lbl_ms.Name = "gb_mbc83_CH3_lbl_ms"
        Me.gb_mbc83_CH3_lbl_ms.Size = New System.Drawing.Size(26, 13)
        Me.gb_mbc83_CH3_lbl_ms.TabIndex = 10
        Me.gb_mbc83_CH3_lbl_ms.Text = "[ms]"
        '
        'gb_mbc83_CH2
        '
        Me.gb_mbc83_CH2.Controls.Add(Me.txt_mbc83_adr_2)
        Me.gb_mbc83_CH2.Controls.Add(Me.gb_mbc83_CH2_lbl_Adresse)
        Me.gb_mbc83_CH2.Controls.Add(Me.gb_mbc83_CH2_lbl_Typ)
        Me.gb_mbc83_CH2.Controls.Add(Me.gb_mbc83_CH2_clb_Typ)
        Me.gb_mbc83_CH2.Controls.Add(Me.gb_mbc83_CH2_nud_TIO)
        Me.gb_mbc83_CH2.Controls.Add(Me.gb_mbc83_CH2_lbl_ON)
        Me.gb_mbc83_CH2.Controls.Add(Me.gb_mbc83_CH2_lbl_ms)
        Me.gb_mbc83_CH2.Location = New System.Drawing.Point(178, 62)
        Me.gb_mbc83_CH2.Name = "gb_mbc83_CH2"
        Me.gb_mbc83_CH2.Size = New System.Drawing.Size(155, 142)
        Me.gb_mbc83_CH2.TabIndex = 13
        Me.gb_mbc83_CH2.TabStop = False
        Me.gb_mbc83_CH2.Text = "Ausgang 2"
        '
        'txt_mbc83_adr_2
        '
        Me.txt_mbc83_adr_2.Location = New System.Drawing.Point(66, 24)
        Me.txt_mbc83_adr_2.Name = "txt_mbc83_adr_2"
        Me.txt_mbc83_adr_2.ReadOnly = True
        Me.txt_mbc83_adr_2.Size = New System.Drawing.Size(54, 20)
        Me.txt_mbc83_adr_2.TabIndex = 41
        '
        'gb_mbc83_CH2_lbl_Adresse
        '
        Me.gb_mbc83_CH2_lbl_Adresse.AutoSize = True
        Me.gb_mbc83_CH2_lbl_Adresse.Location = New System.Drawing.Point(8, 27)
        Me.gb_mbc83_CH2_lbl_Adresse.Name = "gb_mbc83_CH2_lbl_Adresse"
        Me.gb_mbc83_CH2_lbl_Adresse.Size = New System.Drawing.Size(48, 13)
        Me.gb_mbc83_CH2_lbl_Adresse.TabIndex = 40
        Me.gb_mbc83_CH2_lbl_Adresse.Text = "Adresse:"
        '
        'gb_mbc83_CH2_lbl_Typ
        '
        Me.gb_mbc83_CH2_lbl_Typ.AutoSize = True
        Me.gb_mbc83_CH2_lbl_Typ.Location = New System.Drawing.Point(9, 55)
        Me.gb_mbc83_CH2_lbl_Typ.Name = "gb_mbc83_CH2_lbl_Typ"
        Me.gb_mbc83_CH2_lbl_Typ.Size = New System.Drawing.Size(28, 13)
        Me.gb_mbc83_CH2_lbl_Typ.TabIndex = 15
        Me.gb_mbc83_CH2_lbl_Typ.Text = "Typ:"
        '
        'gb_mbc83_CH2_clb_Typ
        '
        Me.gb_mbc83_CH2_clb_Typ.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.gb_mbc83_CH2_clb_Typ.CheckOnClick = True
        Me.gb_mbc83_CH2_clb_Typ.ColumnWidth = 60
        Me.gb_mbc83_CH2_clb_Typ.FormattingEnabled = True
        Me.gb_mbc83_CH2_clb_Typ.Items.AddRange(New Object() {"Impuls", "Wechsel", "StartUp"})
        Me.gb_mbc83_CH2_clb_Typ.Location = New System.Drawing.Point(67, 55)
        Me.gb_mbc83_CH2_clb_Typ.Name = "gb_mbc83_CH2_clb_Typ"
        Me.gb_mbc83_CH2_clb_Typ.Size = New System.Drawing.Size(77, 45)
        Me.gb_mbc83_CH2_clb_Typ.TabIndex = 14
        Me.gb_mbc83_CH2_clb_Typ.TabStop = False
        '
        'gb_mbc83_CH2_nud_TIO
        '
        Me.gb_mbc83_CH2_nud_TIO.Increment = New Decimal(New Integer() {10, 0, 0, 0})
        Me.gb_mbc83_CH2_nud_TIO.Location = New System.Drawing.Point(67, 108)
        Me.gb_mbc83_CH2_nud_TIO.Maximum = New Decimal(New Integer() {2500, 0, 0, 0})
        Me.gb_mbc83_CH2_nud_TIO.Minimum = New Decimal(New Integer() {10, 0, 0, 0})
        Me.gb_mbc83_CH2_nud_TIO.Name = "gb_mbc83_CH2_nud_TIO"
        Me.gb_mbc83_CH2_nud_TIO.Size = New System.Drawing.Size(53, 20)
        Me.gb_mbc83_CH2_nud_TIO.TabIndex = 8
        Me.gb_mbc83_CH2_nud_TIO.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.gb_mbc83_CH2_nud_TIO.Value = New Decimal(New Integer() {200, 0, 0, 0})
        '
        'gb_mbc83_CH2_lbl_ON
        '
        Me.gb_mbc83_CH2_lbl_ON.AutoSize = True
        Me.gb_mbc83_CH2_lbl_ON.Location = New System.Drawing.Point(9, 110)
        Me.gb_mbc83_CH2_lbl_ON.Name = "gb_mbc83_CH2_lbl_ON"
        Me.gb_mbc83_CH2_lbl_ON.Size = New System.Drawing.Size(52, 13)
        Me.gb_mbc83_CH2_lbl_ON.TabIndex = 9
        Me.gb_mbc83_CH2_lbl_ON.Text = "ON-Time:"
        '
        'gb_mbc83_CH2_lbl_ms
        '
        Me.gb_mbc83_CH2_lbl_ms.AutoSize = True
        Me.gb_mbc83_CH2_lbl_ms.Location = New System.Drawing.Point(126, 110)
        Me.gb_mbc83_CH2_lbl_ms.Name = "gb_mbc83_CH2_lbl_ms"
        Me.gb_mbc83_CH2_lbl_ms.Size = New System.Drawing.Size(26, 13)
        Me.gb_mbc83_CH2_lbl_ms.TabIndex = 10
        Me.gb_mbc83_CH2_lbl_ms.Text = "[ms]"
        '
        'gb_mbc83_CH1
        '
        Me.gb_mbc83_CH1.Controls.Add(Me.gb_mbc83_CH1_txt_Adresse)
        Me.gb_mbc83_CH1.Controls.Add(Me.gb_mbc83_CH1_lbl_Adresse)
        Me.gb_mbc83_CH1.Controls.Add(Me.gb_mbc83_CH1_lbl_typ)
        Me.gb_mbc83_CH1.Controls.Add(Me.gb_mbc83_CH1_clb_Typ)
        Me.gb_mbc83_CH1.Controls.Add(Me.gb_mbc83_CH1_nud_TIO)
        Me.gb_mbc83_CH1.Controls.Add(Me.gb_mbc83_CH1_lbl_ON)
        Me.gb_mbc83_CH1.Controls.Add(Me.gb_mbc83_CH1_lbl_ms)
        Me.gb_mbc83_CH1.Location = New System.Drawing.Point(12, 62)
        Me.gb_mbc83_CH1.Name = "gb_mbc83_CH1"
        Me.gb_mbc83_CH1.Size = New System.Drawing.Size(155, 142)
        Me.gb_mbc83_CH1.TabIndex = 12
        Me.gb_mbc83_CH1.TabStop = False
        Me.gb_mbc83_CH1.Text = "Ausgang 1"
        '
        'gb_mbc83_CH1_txt_Adresse
        '
        Me.gb_mbc83_CH1_txt_Adresse.Location = New System.Drawing.Point(67, 24)
        Me.gb_mbc83_CH1_txt_Adresse.Name = "gb_mbc83_CH1_txt_Adresse"
        Me.gb_mbc83_CH1_txt_Adresse.ReadOnly = True
        Me.gb_mbc83_CH1_txt_Adresse.Size = New System.Drawing.Size(54, 20)
        Me.gb_mbc83_CH1_txt_Adresse.TabIndex = 39
        '
        'gb_mbc83_CH1_lbl_Adresse
        '
        Me.gb_mbc83_CH1_lbl_Adresse.AutoSize = True
        Me.gb_mbc83_CH1_lbl_Adresse.Location = New System.Drawing.Point(9, 27)
        Me.gb_mbc83_CH1_lbl_Adresse.Name = "gb_mbc83_CH1_lbl_Adresse"
        Me.gb_mbc83_CH1_lbl_Adresse.Size = New System.Drawing.Size(48, 13)
        Me.gb_mbc83_CH1_lbl_Adresse.TabIndex = 38
        Me.gb_mbc83_CH1_lbl_Adresse.Text = "Adresse:"
        '
        'gb_mbc83_CH1_lbl_typ
        '
        Me.gb_mbc83_CH1_lbl_typ.AutoSize = True
        Me.gb_mbc83_CH1_lbl_typ.Location = New System.Drawing.Point(9, 55)
        Me.gb_mbc83_CH1_lbl_typ.Name = "gb_mbc83_CH1_lbl_typ"
        Me.gb_mbc83_CH1_lbl_typ.Size = New System.Drawing.Size(28, 13)
        Me.gb_mbc83_CH1_lbl_typ.TabIndex = 13
        Me.gb_mbc83_CH1_lbl_typ.Text = "Typ:"
        '
        'gb_mbc83_CH1_clb_Typ
        '
        Me.gb_mbc83_CH1_clb_Typ.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.gb_mbc83_CH1_clb_Typ.CheckOnClick = True
        Me.gb_mbc83_CH1_clb_Typ.ColumnWidth = 60
        Me.gb_mbc83_CH1_clb_Typ.FormattingEnabled = True
        Me.gb_mbc83_CH1_clb_Typ.Items.AddRange(New Object() {"Impuls", "Wechsel", "StartUp"})
        Me.gb_mbc83_CH1_clb_Typ.Location = New System.Drawing.Point(67, 55)
        Me.gb_mbc83_CH1_clb_Typ.Name = "gb_mbc83_CH1_clb_Typ"
        Me.gb_mbc83_CH1_clb_Typ.Size = New System.Drawing.Size(77, 45)
        Me.gb_mbc83_CH1_clb_Typ.TabIndex = 12
        Me.gb_mbc83_CH1_clb_Typ.TabStop = False
        '
        'gb_mbc83_CH1_nud_TIO
        '
        Me.gb_mbc83_CH1_nud_TIO.Increment = New Decimal(New Integer() {10, 0, 0, 0})
        Me.gb_mbc83_CH1_nud_TIO.Location = New System.Drawing.Point(67, 108)
        Me.gb_mbc83_CH1_nud_TIO.Maximum = New Decimal(New Integer() {2500, 0, 0, 0})
        Me.gb_mbc83_CH1_nud_TIO.Minimum = New Decimal(New Integer() {10, 0, 0, 0})
        Me.gb_mbc83_CH1_nud_TIO.Name = "gb_mbc83_CH1_nud_TIO"
        Me.gb_mbc83_CH1_nud_TIO.Size = New System.Drawing.Size(53, 20)
        Me.gb_mbc83_CH1_nud_TIO.TabIndex = 8
        Me.gb_mbc83_CH1_nud_TIO.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.gb_mbc83_CH1_nud_TIO.Value = New Decimal(New Integer() {200, 0, 0, 0})
        '
        'gb_mbc83_CH1_lbl_ON
        '
        Me.gb_mbc83_CH1_lbl_ON.AutoSize = True
        Me.gb_mbc83_CH1_lbl_ON.Location = New System.Drawing.Point(9, 110)
        Me.gb_mbc83_CH1_lbl_ON.Name = "gb_mbc83_CH1_lbl_ON"
        Me.gb_mbc83_CH1_lbl_ON.Size = New System.Drawing.Size(52, 13)
        Me.gb_mbc83_CH1_lbl_ON.TabIndex = 9
        Me.gb_mbc83_CH1_lbl_ON.Text = "ON-Time:"
        '
        'gb_mbc83_CH1_lbl_ms
        '
        Me.gb_mbc83_CH1_lbl_ms.AutoSize = True
        Me.gb_mbc83_CH1_lbl_ms.Location = New System.Drawing.Point(126, 110)
        Me.gb_mbc83_CH1_lbl_ms.Name = "gb_mbc83_CH1_lbl_ms"
        Me.gb_mbc83_CH1_lbl_ms.Size = New System.Drawing.Size(26, 13)
        Me.gb_mbc83_CH1_lbl_ms.TabIndex = 10
        Me.gb_mbc83_CH1_lbl_ms.Text = "[ms]"
        '
        'nud_mbc83_MMAddr
        '
        Me.nud_mbc83_MMAddr.Hexadecimal = True
        Me.nud_mbc83_MMAddr.Increment = New Decimal(New Integer() {4, 0, 0, 0})
        Me.nud_mbc83_MMAddr.Location = New System.Drawing.Point(108, 21)
        Me.nud_mbc83_MMAddr.Maximum = New Decimal(New Integer() {13308, 0, 0, 0})
        Me.nud_mbc83_MMAddr.Minimum = New Decimal(New Integer() {12288, 0, 0, 0})
        Me.nud_mbc83_MMAddr.Name = "nud_mbc83_MMAddr"
        Me.nud_mbc83_MMAddr.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc83_MMAddr.TabIndex = 6
        Me.nud_mbc83_MMAddr.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc83_MMAddr.Value = New Decimal(New Integer() {12288, 0, 0, 0})
        '
        'lbl_mbc83_LocID
        '
        Me.lbl_mbc83_LocID.AutoSize = True
        Me.lbl_mbc83_LocID.Location = New System.Drawing.Point(14, 23)
        Me.lbl_mbc83_LocID.Name = "lbl_mbc83_LocID"
        Me.lbl_mbc83_LocID.Size = New System.Drawing.Size(75, 13)
        Me.lbl_mbc83_LocID.TabIndex = 4
        Me.lbl_mbc83_LocID.Text = "Loc-ID in HEX"
        '
        'tab_mbc84
        '
        Me.tab_mbc84.BackColor = System.Drawing.Color.White
        Me.tab_mbc84.Controls.Add(Me.nud_mbc84_Rueck)
        Me.tab_mbc84.Controls.Add(Me.lbl_mbc84_Hinweis)
        Me.tab_mbc84.Controls.Add(Me.gb_mbc84_CH4)
        Me.tab_mbc84.Controls.Add(Me.gb_mbc84_CH3)
        Me.tab_mbc84.Controls.Add(Me.gb_mbc84_CH2)
        Me.tab_mbc84.Controls.Add(Me.gb_mbc84_CH1)
        Me.tab_mbc84.Controls.Add(Me.nud_mbc84_MMAddr)
        Me.tab_mbc84.Controls.Add(Me.lbl_mbc84_LocID)
        Me.tab_mbc84.Location = New System.Drawing.Point(4, 22)
        Me.tab_mbc84.Name = "tab_mbc84"
        Me.tab_mbc84.Padding = New System.Windows.Forms.Padding(3)
        Me.tab_mbc84.Size = New System.Drawing.Size(680, 248)
        Me.tab_mbc84.TabIndex = 4
        Me.tab_mbc84.Text = "mbc-84"
        '
        'nud_mbc84_Rueck
        '
        Me.nud_mbc84_Rueck.Location = New System.Drawing.Point(270, 21)
        Me.nud_mbc84_Rueck.Maximum = New Decimal(New Integer() {1, 0, 0, 0})
        Me.nud_mbc84_Rueck.Name = "nud_mbc84_Rueck"
        Me.nud_mbc84_Rueck.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc84_Rueck.TabIndex = 21
        Me.nud_mbc84_Rueck.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        '
        'lbl_mbc84_Hinweis
        '
        Me.lbl_mbc84_Hinweis.AutoSize = True
        Me.lbl_mbc84_Hinweis.Location = New System.Drawing.Point(199, 23)
        Me.lbl_mbc84_Hinweis.Name = "lbl_mbc84_Hinweis"
        Me.lbl_mbc84_Hinweis.Size = New System.Drawing.Size(65, 13)
        Me.lbl_mbc84_Hinweis.TabIndex = 20
        Me.lbl_mbc84_Hinweis.Text = "Meldezyklus"
        '
        'gb_mbc84_CH4
        '
        Me.gb_mbc84_CH4.Controls.Add(Me.txt_mbc84_adr_4)
        Me.gb_mbc84_CH4.Controls.Add(Me.nud_mbc84_Links_CH4)
        Me.gb_mbc84_CH4.Controls.Add(Me.lbl_mbc84_adr_CH4)
        Me.gb_mbc84_CH4.Controls.Add(Me.lbl_mbc84_Links_CH4)
        Me.gb_mbc84_CH4.Controls.Add(Me.nud_mbc84_Rechts_CH4)
        Me.gb_mbc84_CH4.Controls.Add(Me.lbl_mbc84_Rechts_CH4)
        Me.gb_mbc84_CH4.Controls.Add(Me.btn_mbc84_Weg_CH4)
        Me.gb_mbc84_CH4.Controls.Add(Me.nud_mbc84_Step_CH4)
        Me.gb_mbc84_CH4.Controls.Add(Me.lbl_mbc84_Step_CH4)
        Me.gb_mbc84_CH4.Location = New System.Drawing.Point(510, 62)
        Me.gb_mbc84_CH4.Name = "gb_mbc84_CH4"
        Me.gb_mbc84_CH4.Size = New System.Drawing.Size(155, 166)
        Me.gb_mbc84_CH4.TabIndex = 15
        Me.gb_mbc84_CH4.TabStop = False
        Me.gb_mbc84_CH4.Text = "Servo 4"
        '
        'txt_mbc84_adr_4
        '
        Me.txt_mbc84_adr_4.Location = New System.Drawing.Point(80, 23)
        Me.txt_mbc84_adr_4.Name = "txt_mbc84_adr_4"
        Me.txt_mbc84_adr_4.ReadOnly = True
        Me.txt_mbc84_adr_4.Size = New System.Drawing.Size(52, 20)
        Me.txt_mbc84_adr_4.TabIndex = 82
        '
        'nud_mbc84_Links_CH4
        '
        Me.nud_mbc84_Links_CH4.Location = New System.Drawing.Point(80, 52)
        Me.nud_mbc84_Links_CH4.Maximum = New Decimal(New Integer() {30, 0, 0, 0})
        Me.nud_mbc84_Links_CH4.Name = "nud_mbc84_Links_CH4"
        Me.nud_mbc84_Links_CH4.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc84_Links_CH4.TabIndex = 81
        Me.nud_mbc84_Links_CH4.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc84_Links_CH4.Value = New Decimal(New Integer() {60, 0, 0, 0})
        '
        'lbl_mbc84_adr_CH4
        '
        Me.lbl_mbc84_adr_CH4.AutoSize = True
        Me.lbl_mbc84_adr_CH4.Location = New System.Drawing.Point(22, 26)
        Me.lbl_mbc84_adr_CH4.Name = "lbl_mbc84_adr_CH4"
        Me.lbl_mbc84_adr_CH4.Size = New System.Drawing.Size(48, 13)
        Me.lbl_mbc84_adr_CH4.TabIndex = 81
        Me.lbl_mbc84_adr_CH4.Text = "Adresse:"
        '
        'lbl_mbc84_Links_CH4
        '
        Me.lbl_mbc84_Links_CH4.AutoSize = True
        Me.lbl_mbc84_Links_CH4.Location = New System.Drawing.Point(22, 81)
        Me.lbl_mbc84_Links_CH4.Name = "lbl_mbc84_Links_CH4"
        Me.lbl_mbc84_Links_CH4.Size = New System.Drawing.Size(33, 13)
        Me.lbl_mbc84_Links_CH4.TabIndex = 82
        Me.lbl_mbc84_Links_CH4.Text = "Grün:"
        '
        'nud_mbc84_Rechts_CH4
        '
        Me.nud_mbc84_Rechts_CH4.Location = New System.Drawing.Point(80, 79)
        Me.nud_mbc84_Rechts_CH4.Maximum = New Decimal(New Integer() {30, 0, 0, 0})
        Me.nud_mbc84_Rechts_CH4.Name = "nud_mbc84_Rechts_CH4"
        Me.nud_mbc84_Rechts_CH4.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc84_Rechts_CH4.TabIndex = 81
        Me.nud_mbc84_Rechts_CH4.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc84_Rechts_CH4.Value = New Decimal(New Integer() {60, 0, 0, 0})
        '
        'lbl_mbc84_Rechts_CH4
        '
        Me.lbl_mbc84_Rechts_CH4.AutoSize = True
        Me.lbl_mbc84_Rechts_CH4.Location = New System.Drawing.Point(22, 54)
        Me.lbl_mbc84_Rechts_CH4.Name = "lbl_mbc84_Rechts_CH4"
        Me.lbl_mbc84_Rechts_CH4.Size = New System.Drawing.Size(27, 13)
        Me.lbl_mbc84_Rechts_CH4.TabIndex = 82
        Me.lbl_mbc84_Rechts_CH4.Text = "Rot:"
        '
        'btn_mbc84_Weg_CH4
        '
        Me.btn_mbc84_Weg_CH4.Location = New System.Drawing.Point(23, 134)
        Me.btn_mbc84_Weg_CH4.Name = "btn_mbc84_Weg_CH4"
        Me.btn_mbc84_Weg_CH4.Size = New System.Drawing.Size(109, 26)
        Me.btn_mbc84_Weg_CH4.TabIndex = 75
        Me.btn_mbc84_Weg_CH4.Text = "Wegdiagramm"
        Me.btn_mbc84_Weg_CH4.UseVisualStyleBackColor = True
        '
        'nud_mbc84_Step_CH4
        '
        Me.nud_mbc84_Step_CH4.Location = New System.Drawing.Point(80, 105)
        Me.nud_mbc84_Step_CH4.Maximum = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc84_Step_CH4.Name = "nud_mbc84_Step_CH4"
        Me.nud_mbc84_Step_CH4.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc84_Step_CH4.TabIndex = 8
        Me.nud_mbc84_Step_CH4.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc84_Step_CH4.Value = New Decimal(New Integer() {10, 0, 0, 0})
        '
        'lbl_mbc84_Step_CH4
        '
        Me.lbl_mbc84_Step_CH4.AutoSize = True
        Me.lbl_mbc84_Step_CH4.Location = New System.Drawing.Point(22, 107)
        Me.lbl_mbc84_Step_CH4.Name = "lbl_mbc84_Step_CH4"
        Me.lbl_mbc84_Step_CH4.Size = New System.Drawing.Size(32, 13)
        Me.lbl_mbc84_Step_CH4.TabIndex = 9
        Me.lbl_mbc84_Step_CH4.Text = "Step:"
        '
        'gb_mbc84_CH3
        '
        Me.gb_mbc84_CH3.Controls.Add(Me.txt_mbc84_adr_3)
        Me.gb_mbc84_CH3.Controls.Add(Me.nud_mbc84_Links_CH3)
        Me.gb_mbc84_CH3.Controls.Add(Me.lbl_mbc84_adr_CH3)
        Me.gb_mbc84_CH3.Controls.Add(Me.lbl_mbc84_Links_CH3)
        Me.gb_mbc84_CH3.Controls.Add(Me.nud_mbc84_Rechts_CH3)
        Me.gb_mbc84_CH3.Controls.Add(Me.lbl_mbc84_Rechts_CH3)
        Me.gb_mbc84_CH3.Controls.Add(Me.btn_mbc84_Weg_CH3)
        Me.gb_mbc84_CH3.Controls.Add(Me.nud_mbc84_Step_CH3)
        Me.gb_mbc84_CH3.Controls.Add(Me.lbl_mbc84_Step_CH3)
        Me.gb_mbc84_CH3.Location = New System.Drawing.Point(344, 62)
        Me.gb_mbc84_CH3.Name = "gb_mbc84_CH3"
        Me.gb_mbc84_CH3.Size = New System.Drawing.Size(155, 166)
        Me.gb_mbc84_CH3.TabIndex = 16
        Me.gb_mbc84_CH3.TabStop = False
        Me.gb_mbc84_CH3.Text = "Servo 3"
        '
        'txt_mbc84_adr_3
        '
        Me.txt_mbc84_adr_3.Location = New System.Drawing.Point(80, 23)
        Me.txt_mbc84_adr_3.Name = "txt_mbc84_adr_3"
        Me.txt_mbc84_adr_3.ReadOnly = True
        Me.txt_mbc84_adr_3.Size = New System.Drawing.Size(53, 20)
        Me.txt_mbc84_adr_3.TabIndex = 80
        '
        'nud_mbc84_Links_CH3
        '
        Me.nud_mbc84_Links_CH3.Location = New System.Drawing.Point(80, 52)
        Me.nud_mbc84_Links_CH3.Maximum = New Decimal(New Integer() {30, 0, 0, 0})
        Me.nud_mbc84_Links_CH3.Name = "nud_mbc84_Links_CH3"
        Me.nud_mbc84_Links_CH3.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc84_Links_CH3.TabIndex = 79
        Me.nud_mbc84_Links_CH3.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc84_Links_CH3.Value = New Decimal(New Integer() {60, 0, 0, 0})
        '
        'lbl_mbc84_adr_CH3
        '
        Me.lbl_mbc84_adr_CH3.AutoSize = True
        Me.lbl_mbc84_adr_CH3.Location = New System.Drawing.Point(22, 26)
        Me.lbl_mbc84_adr_CH3.Name = "lbl_mbc84_adr_CH3"
        Me.lbl_mbc84_adr_CH3.Size = New System.Drawing.Size(48, 13)
        Me.lbl_mbc84_adr_CH3.TabIndex = 79
        Me.lbl_mbc84_adr_CH3.Text = "Adresse:"
        '
        'lbl_mbc84_Links_CH3
        '
        Me.lbl_mbc84_Links_CH3.AutoSize = True
        Me.lbl_mbc84_Links_CH3.Location = New System.Drawing.Point(22, 81)
        Me.lbl_mbc84_Links_CH3.Name = "lbl_mbc84_Links_CH3"
        Me.lbl_mbc84_Links_CH3.Size = New System.Drawing.Size(33, 13)
        Me.lbl_mbc84_Links_CH3.TabIndex = 80
        Me.lbl_mbc84_Links_CH3.Text = "Grün:"
        '
        'nud_mbc84_Rechts_CH3
        '
        Me.nud_mbc84_Rechts_CH3.Location = New System.Drawing.Point(80, 79)
        Me.nud_mbc84_Rechts_CH3.Maximum = New Decimal(New Integer() {30, 0, 0, 0})
        Me.nud_mbc84_Rechts_CH3.Name = "nud_mbc84_Rechts_CH3"
        Me.nud_mbc84_Rechts_CH3.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc84_Rechts_CH3.TabIndex = 79
        Me.nud_mbc84_Rechts_CH3.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc84_Rechts_CH3.Value = New Decimal(New Integer() {60, 0, 0, 0})
        '
        'lbl_mbc84_Rechts_CH3
        '
        Me.lbl_mbc84_Rechts_CH3.AutoSize = True
        Me.lbl_mbc84_Rechts_CH3.Location = New System.Drawing.Point(22, 54)
        Me.lbl_mbc84_Rechts_CH3.Name = "lbl_mbc84_Rechts_CH3"
        Me.lbl_mbc84_Rechts_CH3.Size = New System.Drawing.Size(27, 13)
        Me.lbl_mbc84_Rechts_CH3.TabIndex = 80
        Me.lbl_mbc84_Rechts_CH3.Text = "Rot:"
        '
        'btn_mbc84_Weg_CH3
        '
        Me.btn_mbc84_Weg_CH3.Location = New System.Drawing.Point(24, 134)
        Me.btn_mbc84_Weg_CH3.Name = "btn_mbc84_Weg_CH3"
        Me.btn_mbc84_Weg_CH3.Size = New System.Drawing.Size(109, 26)
        Me.btn_mbc84_Weg_CH3.TabIndex = 74
        Me.btn_mbc84_Weg_CH3.Text = "Wegdiagramm"
        Me.btn_mbc84_Weg_CH3.UseVisualStyleBackColor = True
        '
        'nud_mbc84_Step_CH3
        '
        Me.nud_mbc84_Step_CH3.Location = New System.Drawing.Point(80, 105)
        Me.nud_mbc84_Step_CH3.Maximum = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc84_Step_CH3.Name = "nud_mbc84_Step_CH3"
        Me.nud_mbc84_Step_CH3.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc84_Step_CH3.TabIndex = 8
        Me.nud_mbc84_Step_CH3.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc84_Step_CH3.Value = New Decimal(New Integer() {10, 0, 0, 0})
        '
        'lbl_mbc84_Step_CH3
        '
        Me.lbl_mbc84_Step_CH3.AutoSize = True
        Me.lbl_mbc84_Step_CH3.Location = New System.Drawing.Point(22, 107)
        Me.lbl_mbc84_Step_CH3.Name = "lbl_mbc84_Step_CH3"
        Me.lbl_mbc84_Step_CH3.Size = New System.Drawing.Size(32, 13)
        Me.lbl_mbc84_Step_CH3.TabIndex = 9
        Me.lbl_mbc84_Step_CH3.Text = "Step:"
        '
        'gb_mbc84_CH2
        '
        Me.gb_mbc84_CH2.Controls.Add(Me.txt_mbc84_adr_2)
        Me.gb_mbc84_CH2.Controls.Add(Me.nud_mbc84_Links_CH2)
        Me.gb_mbc84_CH2.Controls.Add(Me.lbl_mbc84_adr_CH2)
        Me.gb_mbc84_CH2.Controls.Add(Me.lbl_mbc84_Links_CH2)
        Me.gb_mbc84_CH2.Controls.Add(Me.nud_mbc84_Rechts_CH2)
        Me.gb_mbc84_CH2.Controls.Add(Me.lbl_mbc84_Rechts_CH2)
        Me.gb_mbc84_CH2.Controls.Add(Me.btn_mbc84_Weg_CH2)
        Me.gb_mbc84_CH2.Controls.Add(Me.nud_mbc84_Step_CH2)
        Me.gb_mbc84_CH2.Controls.Add(Me.lbl_mbc84_Step_CH2)
        Me.gb_mbc84_CH2.Location = New System.Drawing.Point(178, 62)
        Me.gb_mbc84_CH2.Name = "gb_mbc84_CH2"
        Me.gb_mbc84_CH2.Size = New System.Drawing.Size(155, 166)
        Me.gb_mbc84_CH2.TabIndex = 17
        Me.gb_mbc84_CH2.TabStop = False
        Me.gb_mbc84_CH2.Text = "Servo 2"
        '
        'txt_mbc84_adr_2
        '
        Me.txt_mbc84_adr_2.Location = New System.Drawing.Point(80, 23)
        Me.txt_mbc84_adr_2.Name = "txt_mbc84_adr_2"
        Me.txt_mbc84_adr_2.ReadOnly = True
        Me.txt_mbc84_adr_2.Size = New System.Drawing.Size(53, 20)
        Me.txt_mbc84_adr_2.TabIndex = 78
        '
        'nud_mbc84_Links_CH2
        '
        Me.nud_mbc84_Links_CH2.Location = New System.Drawing.Point(80, 52)
        Me.nud_mbc84_Links_CH2.Maximum = New Decimal(New Integer() {30, 0, 0, 0})
        Me.nud_mbc84_Links_CH2.Name = "nud_mbc84_Links_CH2"
        Me.nud_mbc84_Links_CH2.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc84_Links_CH2.TabIndex = 77
        Me.nud_mbc84_Links_CH2.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc84_Links_CH2.Value = New Decimal(New Integer() {60, 0, 0, 0})
        '
        'lbl_mbc84_adr_CH2
        '
        Me.lbl_mbc84_adr_CH2.AutoSize = True
        Me.lbl_mbc84_adr_CH2.Location = New System.Drawing.Point(22, 26)
        Me.lbl_mbc84_adr_CH2.Name = "lbl_mbc84_adr_CH2"
        Me.lbl_mbc84_adr_CH2.Size = New System.Drawing.Size(48, 13)
        Me.lbl_mbc84_adr_CH2.TabIndex = 77
        Me.lbl_mbc84_adr_CH2.Text = "Adresse:"
        '
        'lbl_mbc84_Links_CH2
        '
        Me.lbl_mbc84_Links_CH2.AutoSize = True
        Me.lbl_mbc84_Links_CH2.Location = New System.Drawing.Point(22, 81)
        Me.lbl_mbc84_Links_CH2.Name = "lbl_mbc84_Links_CH2"
        Me.lbl_mbc84_Links_CH2.Size = New System.Drawing.Size(33, 13)
        Me.lbl_mbc84_Links_CH2.TabIndex = 78
        Me.lbl_mbc84_Links_CH2.Text = "Grün:"
        '
        'nud_mbc84_Rechts_CH2
        '
        Me.nud_mbc84_Rechts_CH2.Location = New System.Drawing.Point(80, 79)
        Me.nud_mbc84_Rechts_CH2.Maximum = New Decimal(New Integer() {30, 0, 0, 0})
        Me.nud_mbc84_Rechts_CH2.Name = "nud_mbc84_Rechts_CH2"
        Me.nud_mbc84_Rechts_CH2.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc84_Rechts_CH2.TabIndex = 77
        Me.nud_mbc84_Rechts_CH2.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc84_Rechts_CH2.Value = New Decimal(New Integer() {60, 0, 0, 0})
        '
        'lbl_mbc84_Rechts_CH2
        '
        Me.lbl_mbc84_Rechts_CH2.AutoSize = True
        Me.lbl_mbc84_Rechts_CH2.Location = New System.Drawing.Point(22, 54)
        Me.lbl_mbc84_Rechts_CH2.Name = "lbl_mbc84_Rechts_CH2"
        Me.lbl_mbc84_Rechts_CH2.Size = New System.Drawing.Size(27, 13)
        Me.lbl_mbc84_Rechts_CH2.TabIndex = 78
        Me.lbl_mbc84_Rechts_CH2.Text = "Rot:"
        '
        'btn_mbc84_Weg_CH2
        '
        Me.btn_mbc84_Weg_CH2.Location = New System.Drawing.Point(24, 134)
        Me.btn_mbc84_Weg_CH2.Name = "btn_mbc84_Weg_CH2"
        Me.btn_mbc84_Weg_CH2.Size = New System.Drawing.Size(109, 26)
        Me.btn_mbc84_Weg_CH2.TabIndex = 73
        Me.btn_mbc84_Weg_CH2.Text = "Wegdiagramm"
        Me.btn_mbc84_Weg_CH2.UseVisualStyleBackColor = True
        '
        'nud_mbc84_Step_CH2
        '
        Me.nud_mbc84_Step_CH2.Location = New System.Drawing.Point(80, 105)
        Me.nud_mbc84_Step_CH2.Maximum = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc84_Step_CH2.Name = "nud_mbc84_Step_CH2"
        Me.nud_mbc84_Step_CH2.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc84_Step_CH2.TabIndex = 8
        Me.nud_mbc84_Step_CH2.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc84_Step_CH2.Value = New Decimal(New Integer() {10, 0, 0, 0})
        '
        'lbl_mbc84_Step_CH2
        '
        Me.lbl_mbc84_Step_CH2.AutoSize = True
        Me.lbl_mbc84_Step_CH2.Location = New System.Drawing.Point(22, 107)
        Me.lbl_mbc84_Step_CH2.Name = "lbl_mbc84_Step_CH2"
        Me.lbl_mbc84_Step_CH2.Size = New System.Drawing.Size(32, 13)
        Me.lbl_mbc84_Step_CH2.TabIndex = 9
        Me.lbl_mbc84_Step_CH2.Text = "Step:"
        '
        'gb_mbc84_CH1
        '
        Me.gb_mbc84_CH1.Controls.Add(Me.txt_mbc84_adr_1)
        Me.gb_mbc84_CH1.Controls.Add(Me.nud_mbc84_Links_CH1)
        Me.gb_mbc84_CH1.Controls.Add(Me.lbl_mbc84_adr_CH1)
        Me.gb_mbc84_CH1.Controls.Add(Me.lbl_mbc84_Links_CH1)
        Me.gb_mbc84_CH1.Controls.Add(Me.nud_mbc84_Rechts_CH1)
        Me.gb_mbc84_CH1.Controls.Add(Me.lbl_mbc84_Rechts_CH1)
        Me.gb_mbc84_CH1.Controls.Add(Me.btn_mbc84_Weg_CH1)
        Me.gb_mbc84_CH1.Controls.Add(Me.nud_mbc84_Step_CH1)
        Me.gb_mbc84_CH1.Controls.Add(Me.lbl_mbc84_Step_CH1)
        Me.gb_mbc84_CH1.Location = New System.Drawing.Point(12, 62)
        Me.gb_mbc84_CH1.Name = "gb_mbc84_CH1"
        Me.gb_mbc84_CH1.Size = New System.Drawing.Size(155, 166)
        Me.gb_mbc84_CH1.TabIndex = 14
        Me.gb_mbc84_CH1.TabStop = False
        Me.gb_mbc84_CH1.Text = "Servo 1"
        '
        'txt_mbc84_adr_1
        '
        Me.txt_mbc84_adr_1.Location = New System.Drawing.Point(79, 23)
        Me.txt_mbc84_adr_1.Name = "txt_mbc84_adr_1"
        Me.txt_mbc84_adr_1.ReadOnly = True
        Me.txt_mbc84_adr_1.Size = New System.Drawing.Size(54, 20)
        Me.txt_mbc84_adr_1.TabIndex = 19
        '
        'nud_mbc84_Links_CH1
        '
        Me.nud_mbc84_Links_CH1.Location = New System.Drawing.Point(80, 52)
        Me.nud_mbc84_Links_CH1.Maximum = New Decimal(New Integer() {30, 0, 0, 0})
        Me.nud_mbc84_Links_CH1.Name = "nud_mbc84_Links_CH1"
        Me.nud_mbc84_Links_CH1.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc84_Links_CH1.TabIndex = 75
        Me.nud_mbc84_Links_CH1.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc84_Links_CH1.Value = New Decimal(New Integer() {60, 0, 0, 0})
        '
        'lbl_mbc84_adr_CH1
        '
        Me.lbl_mbc84_adr_CH1.AutoSize = True
        Me.lbl_mbc84_adr_CH1.Location = New System.Drawing.Point(21, 26)
        Me.lbl_mbc84_adr_CH1.Name = "lbl_mbc84_adr_CH1"
        Me.lbl_mbc84_adr_CH1.Size = New System.Drawing.Size(48, 13)
        Me.lbl_mbc84_adr_CH1.TabIndex = 18
        Me.lbl_mbc84_adr_CH1.Text = "Adresse:"
        '
        'lbl_mbc84_Links_CH1
        '
        Me.lbl_mbc84_Links_CH1.AutoSize = True
        Me.lbl_mbc84_Links_CH1.Location = New System.Drawing.Point(21, 81)
        Me.lbl_mbc84_Links_CH1.Name = "lbl_mbc84_Links_CH1"
        Me.lbl_mbc84_Links_CH1.Size = New System.Drawing.Size(33, 13)
        Me.lbl_mbc84_Links_CH1.TabIndex = 76
        Me.lbl_mbc84_Links_CH1.Text = "Grün:"
        '
        'nud_mbc84_Rechts_CH1
        '
        Me.nud_mbc84_Rechts_CH1.Location = New System.Drawing.Point(80, 79)
        Me.nud_mbc84_Rechts_CH1.Maximum = New Decimal(New Integer() {30, 0, 0, 0})
        Me.nud_mbc84_Rechts_CH1.Name = "nud_mbc84_Rechts_CH1"
        Me.nud_mbc84_Rechts_CH1.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc84_Rechts_CH1.TabIndex = 73
        Me.nud_mbc84_Rechts_CH1.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc84_Rechts_CH1.Value = New Decimal(New Integer() {60, 0, 0, 0})
        '
        'lbl_mbc84_Rechts_CH1
        '
        Me.lbl_mbc84_Rechts_CH1.AutoSize = True
        Me.lbl_mbc84_Rechts_CH1.Location = New System.Drawing.Point(21, 54)
        Me.lbl_mbc84_Rechts_CH1.Name = "lbl_mbc84_Rechts_CH1"
        Me.lbl_mbc84_Rechts_CH1.Size = New System.Drawing.Size(27, 13)
        Me.lbl_mbc84_Rechts_CH1.TabIndex = 74
        Me.lbl_mbc84_Rechts_CH1.Text = "Rot:"
        '
        'btn_mbc84_Weg_CH1
        '
        Me.btn_mbc84_Weg_CH1.Location = New System.Drawing.Point(24, 134)
        Me.btn_mbc84_Weg_CH1.Name = "btn_mbc84_Weg_CH1"
        Me.btn_mbc84_Weg_CH1.Size = New System.Drawing.Size(109, 26)
        Me.btn_mbc84_Weg_CH1.TabIndex = 72
        Me.btn_mbc84_Weg_CH1.Text = "Wegdiagramm"
        Me.btn_mbc84_Weg_CH1.UseVisualStyleBackColor = True
        '
        'nud_mbc84_Step_CH1
        '
        Me.nud_mbc84_Step_CH1.Location = New System.Drawing.Point(80, 105)
        Me.nud_mbc84_Step_CH1.Maximum = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc84_Step_CH1.Name = "nud_mbc84_Step_CH1"
        Me.nud_mbc84_Step_CH1.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc84_Step_CH1.TabIndex = 8
        Me.nud_mbc84_Step_CH1.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc84_Step_CH1.Value = New Decimal(New Integer() {10, 0, 0, 0})
        '
        'lbl_mbc84_Step_CH1
        '
        Me.lbl_mbc84_Step_CH1.AutoSize = True
        Me.lbl_mbc84_Step_CH1.Location = New System.Drawing.Point(22, 107)
        Me.lbl_mbc84_Step_CH1.Name = "lbl_mbc84_Step_CH1"
        Me.lbl_mbc84_Step_CH1.Size = New System.Drawing.Size(32, 13)
        Me.lbl_mbc84_Step_CH1.TabIndex = 9
        Me.lbl_mbc84_Step_CH1.Text = "Step:"
        '
        'nud_mbc84_MMAddr
        '
        Me.nud_mbc84_MMAddr.Hexadecimal = True
        Me.nud_mbc84_MMAddr.Increment = New Decimal(New Integer() {4, 0, 0, 0})
        Me.nud_mbc84_MMAddr.Location = New System.Drawing.Point(108, 21)
        Me.nud_mbc84_MMAddr.Maximum = New Decimal(New Integer() {13308, 0, 0, 0})
        Me.nud_mbc84_MMAddr.Minimum = New Decimal(New Integer() {12288, 0, 0, 0})
        Me.nud_mbc84_MMAddr.Name = "nud_mbc84_MMAddr"
        Me.nud_mbc84_MMAddr.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc84_MMAddr.TabIndex = 9
        Me.nud_mbc84_MMAddr.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc84_MMAddr.Value = New Decimal(New Integer() {12288, 0, 0, 0})
        '
        'lbl_mbc84_LocID
        '
        Me.lbl_mbc84_LocID.AutoSize = True
        Me.lbl_mbc84_LocID.Location = New System.Drawing.Point(14, 23)
        Me.lbl_mbc84_LocID.Name = "lbl_mbc84_LocID"
        Me.lbl_mbc84_LocID.Size = New System.Drawing.Size(75, 13)
        Me.lbl_mbc84_LocID.TabIndex = 7
        Me.lbl_mbc84_LocID.Text = "Loc-ID in HEX"
        '
        'tab_mbc85
        '
        Me.tab_mbc85.BackColor = System.Drawing.Color.White
        Me.tab_mbc85.Controls.Add(Me.btn_mbc85_Winkel)
        Me.tab_mbc85.Controls.Add(Me.gb_mbc85_DS_Integrator)
        Me.tab_mbc85.Controls.Add(Me.gb_mbc85_MMLok)
        Me.tab_mbc85.Controls.Add(Me.lbl_mbc85_bis)
        Me.tab_mbc85.Controls.Add(Me.txt_mbc85_kontakt2cs2)
        Me.tab_mbc85.Controls.Add(Me.txt_mbc85_kontakt1cs2)
        Me.tab_mbc85.Controls.Add(Me.lbl_mbc85_Kontakte)
        Me.tab_mbc85.Controls.Add(Me.txt_mbc85_BusNr)
        Me.tab_mbc85.Controls.Add(Me.lbl_mbc85_BusNr)
        Me.tab_mbc85.Controls.Add(Me.nud_mbc85_S88ModNr)
        Me.tab_mbc85.Controls.Add(Me.lbl_mbc85_ModulNr)
        Me.tab_mbc85.Controls.Add(Me.gb_mbc85_Integrator)
        Me.tab_mbc85.Location = New System.Drawing.Point(4, 22)
        Me.tab_mbc85.Name = "tab_mbc85"
        Me.tab_mbc85.Padding = New System.Windows.Forms.Padding(3)
        Me.tab_mbc85.Size = New System.Drawing.Size(680, 248)
        Me.tab_mbc85.TabIndex = 5
        Me.tab_mbc85.Text = "mbc-85"
        '
        'btn_mbc85_Winkel
        '
        Me.btn_mbc85_Winkel.Location = New System.Drawing.Point(519, 189)
        Me.btn_mbc85_Winkel.Name = "btn_mbc85_Winkel"
        Me.btn_mbc85_Winkel.Size = New System.Drawing.Size(136, 27)
        Me.btn_mbc85_Winkel.TabIndex = 188
        Me.btn_mbc85_Winkel.Text = "Button1"
        Me.btn_mbc85_Winkel.UseVisualStyleBackColor = True
        '
        'gb_mbc85_DS_Integrator
        '
        Me.gb_mbc85_DS_Integrator.Controls.Add(Me.lbl_mbc85_P11)
        Me.gb_mbc85_DS_Integrator.Controls.Add(Me.lbl_mbc85_P11a)
        Me.gb_mbc85_DS_Integrator.Controls.Add(Me.nud_mbc85_T_P11)
        Me.gb_mbc85_DS_Integrator.Location = New System.Drawing.Point(320, 62)
        Me.gb_mbc85_DS_Integrator.Name = "gb_mbc85_DS_Integrator"
        Me.gb_mbc85_DS_Integrator.Size = New System.Drawing.Size(173, 51)
        Me.gb_mbc85_DS_Integrator.TabIndex = 187
        Me.gb_mbc85_DS_Integrator.TabStop = False
        Me.gb_mbc85_DS_Integrator.Text = "Drehscheibe Integrator"
        '
        'lbl_mbc85_P11
        '
        Me.lbl_mbc85_P11.AutoSize = True
        Me.lbl_mbc85_P11.Location = New System.Drawing.Point(12, 24)
        Me.lbl_mbc85_P11.Name = "lbl_mbc85_P11"
        Me.lbl_mbc85_P11.Size = New System.Drawing.Size(29, 13)
        Me.lbl_mbc85_P11.TabIndex = 182
        Me.lbl_mbc85_P11.Text = "P11:"
        '
        'lbl_mbc85_P11a
        '
        Me.lbl_mbc85_P11a.AutoSize = True
        Me.lbl_mbc85_P11a.Location = New System.Drawing.Point(69, 23)
        Me.lbl_mbc85_P11a.Name = "lbl_mbc85_P11a"
        Me.lbl_mbc85_P11a.Size = New System.Drawing.Size(26, 13)
        Me.lbl_mbc85_P11a.TabIndex = 166
        Me.lbl_mbc85_P11a.Text = "[ms]"
        '
        'nud_mbc85_T_P11
        '
        Me.nud_mbc85_T_P11.Increment = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc85_T_P11.Location = New System.Drawing.Point(101, 21)
        Me.nud_mbc85_T_P11.Maximum = New Decimal(New Integer() {2500, 0, 0, 0})
        Me.nud_mbc85_T_P11.Minimum = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc85_T_P11.Name = "nud_mbc85_T_P11"
        Me.nud_mbc85_T_P11.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc85_T_P11.TabIndex = 166
        Me.nud_mbc85_T_P11.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc85_T_P11.Value = New Decimal(New Integer() {10, 0, 0, 0})
        '
        'gb_mbc85_MMLok
        '
        Me.gb_mbc85_MMLok.Controls.Add(Me.nud_mbc85_MMSpeed)
        Me.gb_mbc85_MMLok.Controls.Add(Me.lbl_mbc85_MMSpeed)
        Me.gb_mbc85_MMLok.Controls.Add(Me.lbl_mbc85_MMLok)
        Me.gb_mbc85_MMLok.Controls.Add(Me.nud_mbc85_MMLok)
        Me.gb_mbc85_MMLok.Location = New System.Drawing.Point(320, 119)
        Me.gb_mbc85_MMLok.Name = "gb_mbc85_MMLok"
        Me.gb_mbc85_MMLok.Size = New System.Drawing.Size(173, 110)
        Me.gb_mbc85_MMLok.TabIndex = 184
        Me.gb_mbc85_MMLok.TabStop = False
        Me.gb_mbc85_MMLok.Text = "Mfx-Lok"
        '
        'nud_mbc85_MMSpeed
        '
        Me.nud_mbc85_MMSpeed.Location = New System.Drawing.Point(101, 74)
        Me.nud_mbc85_MMSpeed.Maximum = New Decimal(New Integer() {250, 0, 0, 0})
        Me.nud_mbc85_MMSpeed.Name = "nud_mbc85_MMSpeed"
        Me.nud_mbc85_MMSpeed.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc85_MMSpeed.TabIndex = 32
        Me.nud_mbc85_MMSpeed.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc85_MMSpeed.Value = New Decimal(New Integer() {1, 0, 0, 0})
        '
        'lbl_mbc85_MMSpeed
        '
        Me.lbl_mbc85_MMSpeed.AutoSize = True
        Me.lbl_mbc85_MMSpeed.Location = New System.Drawing.Point(9, 76)
        Me.lbl_mbc85_MMSpeed.Name = "lbl_mbc85_MMSpeed"
        Me.lbl_mbc85_MMSpeed.Size = New System.Drawing.Size(38, 13)
        Me.lbl_mbc85_MMSpeed.TabIndex = 31
        Me.lbl_mbc85_MMSpeed.Text = "Speed"
        '
        'lbl_mbc85_MMLok
        '
        Me.lbl_mbc85_MMLok.AutoSize = True
        Me.lbl_mbc85_MMLok.Location = New System.Drawing.Point(9, 32)
        Me.lbl_mbc85_MMLok.Name = "lbl_mbc85_MMLok"
        Me.lbl_mbc85_MMLok.Size = New System.Drawing.Size(45, 13)
        Me.lbl_mbc85_MMLok.TabIndex = 30
        Me.lbl_mbc85_MMLok.Text = "Adresse"
        '
        'nud_mbc85_MMLok
        '
        Me.nud_mbc85_MMLok.Location = New System.Drawing.Point(101, 30)
        Me.nud_mbc85_MMLok.Maximum = New Decimal(New Integer() {255, 0, 0, 0})
        Me.nud_mbc85_MMLok.Name = "nud_mbc85_MMLok"
        Me.nud_mbc85_MMLok.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc85_MMLok.TabIndex = 29
        Me.nud_mbc85_MMLok.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc85_MMLok.Value = New Decimal(New Integer() {1, 0, 0, 0})
        '
        'lbl_mbc85_bis
        '
        Me.lbl_mbc85_bis.AutoSize = True
        Me.lbl_mbc85_bis.Location = New System.Drawing.Point(373, 26)
        Me.lbl_mbc85_bis.Name = "lbl_mbc85_bis"
        Me.lbl_mbc85_bis.Size = New System.Drawing.Size(10, 13)
        Me.lbl_mbc85_bis.TabIndex = 178
        Me.lbl_mbc85_bis.Text = "-"
        '
        'txt_mbc85_kontakt2cs2
        '
        Me.txt_mbc85_kontakt2cs2.Location = New System.Drawing.Point(387, 23)
        Me.txt_mbc85_kontakt2cs2.Name = "txt_mbc85_kontakt2cs2"
        Me.txt_mbc85_kontakt2cs2.ReadOnly = True
        Me.txt_mbc85_kontakt2cs2.Size = New System.Drawing.Size(41, 20)
        Me.txt_mbc85_kontakt2cs2.TabIndex = 177
        '
        'txt_mbc85_kontakt1cs2
        '
        Me.txt_mbc85_kontakt1cs2.Location = New System.Drawing.Point(327, 23)
        Me.txt_mbc85_kontakt1cs2.Name = "txt_mbc85_kontakt1cs2"
        Me.txt_mbc85_kontakt1cs2.ReadOnly = True
        Me.txt_mbc85_kontakt1cs2.Size = New System.Drawing.Size(40, 20)
        Me.txt_mbc85_kontakt1cs2.TabIndex = 176
        '
        'lbl_mbc85_Kontakte
        '
        Me.lbl_mbc85_Kontakte.AutoSize = True
        Me.lbl_mbc85_Kontakte.Location = New System.Drawing.Point(249, 26)
        Me.lbl_mbc85_Kontakte.Name = "lbl_mbc85_Kontakte"
        Me.lbl_mbc85_Kontakte.Size = New System.Drawing.Size(73, 13)
        Me.lbl_mbc85_Kontakte.TabIndex = 175
        Me.lbl_mbc85_Kontakte.Text = "Kontakte CS2"
        '
        'txt_mbc85_BusNr
        '
        Me.txt_mbc85_BusNr.Location = New System.Drawing.Point(200, 23)
        Me.txt_mbc85_BusNr.Name = "txt_mbc85_BusNr"
        Me.txt_mbc85_BusNr.ReadOnly = True
        Me.txt_mbc85_BusNr.Size = New System.Drawing.Size(25, 20)
        Me.txt_mbc85_BusNr.TabIndex = 174
        '
        'lbl_mbc85_BusNr
        '
        Me.lbl_mbc85_BusNr.AutoSize = True
        Me.lbl_mbc85_BusNr.Location = New System.Drawing.Point(161, 25)
        Me.lbl_mbc85_BusNr.Name = "lbl_mbc85_BusNr"
        Me.lbl_mbc85_BusNr.Size = New System.Drawing.Size(36, 13)
        Me.lbl_mbc85_BusNr.TabIndex = 173
        Me.lbl_mbc85_BusNr.Text = "BusNr"
        '
        'nud_mbc85_S88ModNr
        '
        Me.nud_mbc85_S88ModNr.Location = New System.Drawing.Point(67, 23)
        Me.nud_mbc85_S88ModNr.Maximum = New Decimal(New Integer() {93, 0, 0, 0})
        Me.nud_mbc85_S88ModNr.Minimum = New Decimal(New Integer() {1, 0, 0, 0})
        Me.nud_mbc85_S88ModNr.Name = "nud_mbc85_S88ModNr"
        Me.nud_mbc85_S88ModNr.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc85_S88ModNr.TabIndex = 123
        Me.nud_mbc85_S88ModNr.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc85_S88ModNr.Value = New Decimal(New Integer() {62, 0, 0, 0})
        '
        'lbl_mbc85_ModulNr
        '
        Me.lbl_mbc85_ModulNr.AutoSize = True
        Me.lbl_mbc85_ModulNr.Location = New System.Drawing.Point(15, 25)
        Me.lbl_mbc85_ModulNr.Name = "lbl_mbc85_ModulNr"
        Me.lbl_mbc85_ModulNr.Size = New System.Drawing.Size(47, 13)
        Me.lbl_mbc85_ModulNr.TabIndex = 122
        Me.lbl_mbc85_ModulNr.Text = "ModulNr"
        '
        'gb_mbc85_Integrator
        '
        Me.gb_mbc85_Integrator.Controls.Add(Me.nud_mbc85_T_P05)
        Me.gb_mbc85_Integrator.Controls.Add(Me.lbl_mbc85_P05a)
        Me.gb_mbc85_Integrator.Controls.Add(Me.lbl_mbc85_P05)
        Me.gb_mbc85_Integrator.Controls.Add(Me.nud_mbc85_T_P06)
        Me.gb_mbc85_Integrator.Controls.Add(Me.lbl_mbc85_P06a)
        Me.gb_mbc85_Integrator.Controls.Add(Me.lbl_mbc85_P06)
        Me.gb_mbc85_Integrator.Controls.Add(Me.nud_mbc85_T_P04)
        Me.gb_mbc85_Integrator.Controls.Add(Me.lbl_mbc85_P04)
        Me.gb_mbc85_Integrator.Controls.Add(Me.nud_mbc85_T_P08)
        Me.gb_mbc85_Integrator.Controls.Add(Me.lbl_mbc85_P04a)
        Me.gb_mbc85_Integrator.Controls.Add(Me.lbl_mbc85_P08)
        Me.gb_mbc85_Integrator.Controls.Add(Me.nud_mbc85_T_P03)
        Me.gb_mbc85_Integrator.Controls.Add(Me.nud_mbc85_T_P07)
        Me.gb_mbc85_Integrator.Controls.Add(Me.lbl_mbc85_P03)
        Me.gb_mbc85_Integrator.Controls.Add(Me.lbl_mbc85_P08a)
        Me.gb_mbc85_Integrator.Controls.Add(Me.lbl_mbc85_P03a)
        Me.gb_mbc85_Integrator.Controls.Add(Me.lbl_mbc85_P07a)
        Me.gb_mbc85_Integrator.Controls.Add(Me.nud_mbc85_T_P02)
        Me.gb_mbc85_Integrator.Controls.Add(Me.lbl_mbc85_P07)
        Me.gb_mbc85_Integrator.Controls.Add(Me.lbl_mbc85_P02)
        Me.gb_mbc85_Integrator.Controls.Add(Me.nud_mbc85_T_P10)
        Me.gb_mbc85_Integrator.Controls.Add(Me.lbl_mbc85_P02a)
        Me.gb_mbc85_Integrator.Controls.Add(Me.lbl_mbc85_P10)
        Me.gb_mbc85_Integrator.Controls.Add(Me.nud_mbc85_T_P01)
        Me.gb_mbc85_Integrator.Controls.Add(Me.nud_mbc85_T_P09)
        Me.gb_mbc85_Integrator.Controls.Add(Me.lbl_mbc85_P01)
        Me.gb_mbc85_Integrator.Controls.Add(Me.lbl_mbc85_P10a)
        Me.gb_mbc85_Integrator.Controls.Add(Me.lbl_mbc85_P01a)
        Me.gb_mbc85_Integrator.Controls.Add(Me.lbl_mbc85_P09a)
        Me.gb_mbc85_Integrator.Controls.Add(Me.lbl_mbc85_P09)
        Me.gb_mbc85_Integrator.Location = New System.Drawing.Point(18, 62)
        Me.gb_mbc85_Integrator.Name = "gb_mbc85_Integrator"
        Me.gb_mbc85_Integrator.Size = New System.Drawing.Size(284, 167)
        Me.gb_mbc85_Integrator.TabIndex = 172
        Me.gb_mbc85_Integrator.TabStop = False
        Me.gb_mbc85_Integrator.Text = "Integrator"
        '
        'nud_mbc85_T_P05
        '
        Me.nud_mbc85_T_P05.Increment = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc85_T_P05.Location = New System.Drawing.Point(49, 132)
        Me.nud_mbc85_T_P05.Maximum = New Decimal(New Integer() {2500, 0, 0, 0})
        Me.nud_mbc85_T_P05.Minimum = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc85_T_P05.Name = "nud_mbc85_T_P05"
        Me.nud_mbc85_T_P05.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc85_T_P05.TabIndex = 127
        Me.nud_mbc85_T_P05.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc85_T_P05.Value = New Decimal(New Integer() {10, 0, 0, 0})
        '
        'lbl_mbc85_P05a
        '
        Me.lbl_mbc85_P05a.AutoSize = True
        Me.lbl_mbc85_P05a.Location = New System.Drawing.Point(108, 134)
        Me.lbl_mbc85_P05a.Name = "lbl_mbc85_P05a"
        Me.lbl_mbc85_P05a.Size = New System.Drawing.Size(26, 13)
        Me.lbl_mbc85_P05a.TabIndex = 129
        Me.lbl_mbc85_P05a.Text = "[ms]"
        '
        'lbl_mbc85_P05
        '
        Me.lbl_mbc85_P05.AutoSize = True
        Me.lbl_mbc85_P05.Location = New System.Drawing.Point(12, 134)
        Me.lbl_mbc85_P05.Name = "lbl_mbc85_P05"
        Me.lbl_mbc85_P05.Size = New System.Drawing.Size(29, 13)
        Me.lbl_mbc85_P05.TabIndex = 128
        Me.lbl_mbc85_P05.Text = "P05:"
        '
        'nud_mbc85_T_P06
        '
        Me.nud_mbc85_T_P06.Increment = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc85_T_P06.Location = New System.Drawing.Point(180, 23)
        Me.nud_mbc85_T_P06.Maximum = New Decimal(New Integer() {2500, 0, 0, 0})
        Me.nud_mbc85_T_P06.Minimum = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc85_T_P06.Name = "nud_mbc85_T_P06"
        Me.nud_mbc85_T_P06.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc85_T_P06.TabIndex = 139
        Me.nud_mbc85_T_P06.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc85_T_P06.Value = New Decimal(New Integer() {10, 0, 0, 0})
        '
        'lbl_mbc85_P06a
        '
        Me.lbl_mbc85_P06a.AutoSize = True
        Me.lbl_mbc85_P06a.Location = New System.Drawing.Point(239, 25)
        Me.lbl_mbc85_P06a.Name = "lbl_mbc85_P06a"
        Me.lbl_mbc85_P06a.Size = New System.Drawing.Size(26, 13)
        Me.lbl_mbc85_P06a.TabIndex = 141
        Me.lbl_mbc85_P06a.Text = "[ms]"
        '
        'lbl_mbc85_P06
        '
        Me.lbl_mbc85_P06.AutoSize = True
        Me.lbl_mbc85_P06.Location = New System.Drawing.Point(143, 25)
        Me.lbl_mbc85_P06.Name = "lbl_mbc85_P06"
        Me.lbl_mbc85_P06.Size = New System.Drawing.Size(29, 13)
        Me.lbl_mbc85_P06.TabIndex = 140
        Me.lbl_mbc85_P06.Text = "P06:"
        '
        'nud_mbc85_T_P04
        '
        Me.nud_mbc85_T_P04.Increment = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc85_T_P04.Location = New System.Drawing.Point(49, 106)
        Me.nud_mbc85_T_P04.Maximum = New Decimal(New Integer() {2500, 0, 0, 0})
        Me.nud_mbc85_T_P04.Minimum = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc85_T_P04.Name = "nud_mbc85_T_P04"
        Me.nud_mbc85_T_P04.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc85_T_P04.TabIndex = 160
        Me.nud_mbc85_T_P04.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc85_T_P04.Value = New Decimal(New Integer() {10, 0, 0, 0})
        '
        'lbl_mbc85_P04
        '
        Me.lbl_mbc85_P04.AutoSize = True
        Me.lbl_mbc85_P04.Location = New System.Drawing.Point(12, 108)
        Me.lbl_mbc85_P04.Name = "lbl_mbc85_P04"
        Me.lbl_mbc85_P04.Size = New System.Drawing.Size(29, 13)
        Me.lbl_mbc85_P04.TabIndex = 161
        Me.lbl_mbc85_P04.Text = "P04:"
        '
        'nud_mbc85_T_P08
        '
        Me.nud_mbc85_T_P08.Increment = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc85_T_P08.Location = New System.Drawing.Point(180, 79)
        Me.nud_mbc85_T_P08.Maximum = New Decimal(New Integer() {2500, 0, 0, 0})
        Me.nud_mbc85_T_P08.Minimum = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc85_T_P08.Name = "nud_mbc85_T_P08"
        Me.nud_mbc85_T_P08.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc85_T_P08.TabIndex = 163
        Me.nud_mbc85_T_P08.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc85_T_P08.Value = New Decimal(New Integer() {10, 0, 0, 0})
        '
        'lbl_mbc85_P04a
        '
        Me.lbl_mbc85_P04a.AutoSize = True
        Me.lbl_mbc85_P04a.Location = New System.Drawing.Point(108, 108)
        Me.lbl_mbc85_P04a.Name = "lbl_mbc85_P04a"
        Me.lbl_mbc85_P04a.Size = New System.Drawing.Size(26, 13)
        Me.lbl_mbc85_P04a.TabIndex = 162
        Me.lbl_mbc85_P04a.Text = "[ms]"
        '
        'lbl_mbc85_P08
        '
        Me.lbl_mbc85_P08.AutoSize = True
        Me.lbl_mbc85_P08.Location = New System.Drawing.Point(143, 81)
        Me.lbl_mbc85_P08.Name = "lbl_mbc85_P08"
        Me.lbl_mbc85_P08.Size = New System.Drawing.Size(29, 13)
        Me.lbl_mbc85_P08.TabIndex = 164
        Me.lbl_mbc85_P08.Text = "P08:"
        '
        'nud_mbc85_T_P03
        '
        Me.nud_mbc85_T_P03.Increment = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc85_T_P03.Location = New System.Drawing.Point(49, 79)
        Me.nud_mbc85_T_P03.Maximum = New Decimal(New Integer() {2500, 0, 0, 0})
        Me.nud_mbc85_T_P03.Minimum = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc85_T_P03.Name = "nud_mbc85_T_P03"
        Me.nud_mbc85_T_P03.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc85_T_P03.TabIndex = 148
        Me.nud_mbc85_T_P03.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc85_T_P03.Value = New Decimal(New Integer() {10, 0, 0, 0})
        '
        'nud_mbc85_T_P07
        '
        Me.nud_mbc85_T_P07.Increment = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc85_T_P07.Location = New System.Drawing.Point(180, 51)
        Me.nud_mbc85_T_P07.Maximum = New Decimal(New Integer() {2500, 0, 0, 0})
        Me.nud_mbc85_T_P07.Minimum = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc85_T_P07.Name = "nud_mbc85_T_P07"
        Me.nud_mbc85_T_P07.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc85_T_P07.TabIndex = 151
        Me.nud_mbc85_T_P07.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc85_T_P07.Value = New Decimal(New Integer() {10, 0, 0, 0})
        '
        'lbl_mbc85_P03
        '
        Me.lbl_mbc85_P03.AutoSize = True
        Me.lbl_mbc85_P03.Location = New System.Drawing.Point(12, 81)
        Me.lbl_mbc85_P03.Name = "lbl_mbc85_P03"
        Me.lbl_mbc85_P03.Size = New System.Drawing.Size(29, 13)
        Me.lbl_mbc85_P03.TabIndex = 149
        Me.lbl_mbc85_P03.Text = "P03:"
        '
        'lbl_mbc85_P08a
        '
        Me.lbl_mbc85_P08a.AutoSize = True
        Me.lbl_mbc85_P08a.Location = New System.Drawing.Point(239, 81)
        Me.lbl_mbc85_P08a.Name = "lbl_mbc85_P08a"
        Me.lbl_mbc85_P08a.Size = New System.Drawing.Size(26, 13)
        Me.lbl_mbc85_P08a.TabIndex = 165
        Me.lbl_mbc85_P08a.Text = "[ms]"
        '
        'lbl_mbc85_P03a
        '
        Me.lbl_mbc85_P03a.AutoSize = True
        Me.lbl_mbc85_P03a.Location = New System.Drawing.Point(108, 81)
        Me.lbl_mbc85_P03a.Name = "lbl_mbc85_P03a"
        Me.lbl_mbc85_P03a.Size = New System.Drawing.Size(26, 13)
        Me.lbl_mbc85_P03a.TabIndex = 150
        Me.lbl_mbc85_P03a.Text = "[ms]"
        '
        'lbl_mbc85_P07a
        '
        Me.lbl_mbc85_P07a.AutoSize = True
        Me.lbl_mbc85_P07a.Location = New System.Drawing.Point(239, 53)
        Me.lbl_mbc85_P07a.Name = "lbl_mbc85_P07a"
        Me.lbl_mbc85_P07a.Size = New System.Drawing.Size(26, 13)
        Me.lbl_mbc85_P07a.TabIndex = 153
        Me.lbl_mbc85_P07a.Text = "[ms]"
        '
        'nud_mbc85_T_P02
        '
        Me.nud_mbc85_T_P02.Increment = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc85_T_P02.Location = New System.Drawing.Point(49, 51)
        Me.nud_mbc85_T_P02.Maximum = New Decimal(New Integer() {2500, 0, 0, 0})
        Me.nud_mbc85_T_P02.Minimum = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc85_T_P02.Name = "nud_mbc85_T_P02"
        Me.nud_mbc85_T_P02.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc85_T_P02.TabIndex = 136
        Me.nud_mbc85_T_P02.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc85_T_P02.Value = New Decimal(New Integer() {10, 0, 0, 0})
        '
        'lbl_mbc85_P07
        '
        Me.lbl_mbc85_P07.AutoSize = True
        Me.lbl_mbc85_P07.Location = New System.Drawing.Point(143, 53)
        Me.lbl_mbc85_P07.Name = "lbl_mbc85_P07"
        Me.lbl_mbc85_P07.Size = New System.Drawing.Size(29, 13)
        Me.lbl_mbc85_P07.TabIndex = 152
        Me.lbl_mbc85_P07.Text = "P07:"
        '
        'lbl_mbc85_P02
        '
        Me.lbl_mbc85_P02.AutoSize = True
        Me.lbl_mbc85_P02.Location = New System.Drawing.Point(12, 53)
        Me.lbl_mbc85_P02.Name = "lbl_mbc85_P02"
        Me.lbl_mbc85_P02.Size = New System.Drawing.Size(29, 13)
        Me.lbl_mbc85_P02.TabIndex = 137
        Me.lbl_mbc85_P02.Text = "P02:"
        '
        'nud_mbc85_T_P10
        '
        Me.nud_mbc85_T_P10.Increment = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc85_T_P10.Location = New System.Drawing.Point(180, 132)
        Me.nud_mbc85_T_P10.Maximum = New Decimal(New Integer() {2500, 0, 0, 0})
        Me.nud_mbc85_T_P10.Minimum = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc85_T_P10.Name = "nud_mbc85_T_P10"
        Me.nud_mbc85_T_P10.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc85_T_P10.TabIndex = 142
        Me.nud_mbc85_T_P10.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc85_T_P10.Value = New Decimal(New Integer() {10, 0, 0, 0})
        '
        'lbl_mbc85_P02a
        '
        Me.lbl_mbc85_P02a.AutoSize = True
        Me.lbl_mbc85_P02a.Location = New System.Drawing.Point(108, 53)
        Me.lbl_mbc85_P02a.Name = "lbl_mbc85_P02a"
        Me.lbl_mbc85_P02a.Size = New System.Drawing.Size(26, 13)
        Me.lbl_mbc85_P02a.TabIndex = 138
        Me.lbl_mbc85_P02a.Text = "[ms]"
        '
        'lbl_mbc85_P10
        '
        Me.lbl_mbc85_P10.AutoSize = True
        Me.lbl_mbc85_P10.Location = New System.Drawing.Point(143, 134)
        Me.lbl_mbc85_P10.Name = "lbl_mbc85_P10"
        Me.lbl_mbc85_P10.Size = New System.Drawing.Size(29, 13)
        Me.lbl_mbc85_P10.TabIndex = 143
        Me.lbl_mbc85_P10.Text = "P10:"
        '
        'nud_mbc85_T_P01
        '
        Me.nud_mbc85_T_P01.Increment = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc85_T_P01.Location = New System.Drawing.Point(49, 23)
        Me.nud_mbc85_T_P01.Maximum = New Decimal(New Integer() {2500, 0, 0, 0})
        Me.nud_mbc85_T_P01.Minimum = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc85_T_P01.Name = "nud_mbc85_T_P01"
        Me.nud_mbc85_T_P01.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc85_T_P01.TabIndex = 124
        Me.nud_mbc85_T_P01.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc85_T_P01.Value = New Decimal(New Integer() {10, 0, 0, 0})
        '
        'nud_mbc85_T_P09
        '
        Me.nud_mbc85_T_P09.Increment = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc85_T_P09.Location = New System.Drawing.Point(180, 106)
        Me.nud_mbc85_T_P09.Maximum = New Decimal(New Integer() {2500, 0, 0, 0})
        Me.nud_mbc85_T_P09.Minimum = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc85_T_P09.Name = "nud_mbc85_T_P09"
        Me.nud_mbc85_T_P09.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc85_T_P09.TabIndex = 130
        Me.nud_mbc85_T_P09.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc85_T_P09.Value = New Decimal(New Integer() {10, 0, 0, 0})
        '
        'lbl_mbc85_P01
        '
        Me.lbl_mbc85_P01.AutoSize = True
        Me.lbl_mbc85_P01.Location = New System.Drawing.Point(12, 25)
        Me.lbl_mbc85_P01.Name = "lbl_mbc85_P01"
        Me.lbl_mbc85_P01.Size = New System.Drawing.Size(29, 13)
        Me.lbl_mbc85_P01.TabIndex = 125
        Me.lbl_mbc85_P01.Text = "P01:"
        '
        'lbl_mbc85_P10a
        '
        Me.lbl_mbc85_P10a.AutoSize = True
        Me.lbl_mbc85_P10a.Location = New System.Drawing.Point(239, 134)
        Me.lbl_mbc85_P10a.Name = "lbl_mbc85_P10a"
        Me.lbl_mbc85_P10a.Size = New System.Drawing.Size(26, 13)
        Me.lbl_mbc85_P10a.TabIndex = 144
        Me.lbl_mbc85_P10a.Text = "[ms]"
        '
        'lbl_mbc85_P01a
        '
        Me.lbl_mbc85_P01a.AutoSize = True
        Me.lbl_mbc85_P01a.Location = New System.Drawing.Point(108, 25)
        Me.lbl_mbc85_P01a.Name = "lbl_mbc85_P01a"
        Me.lbl_mbc85_P01a.Size = New System.Drawing.Size(26, 13)
        Me.lbl_mbc85_P01a.TabIndex = 126
        Me.lbl_mbc85_P01a.Text = "[ms]"
        '
        'lbl_mbc85_P09a
        '
        Me.lbl_mbc85_P09a.AutoSize = True
        Me.lbl_mbc85_P09a.Location = New System.Drawing.Point(239, 108)
        Me.lbl_mbc85_P09a.Name = "lbl_mbc85_P09a"
        Me.lbl_mbc85_P09a.Size = New System.Drawing.Size(26, 13)
        Me.lbl_mbc85_P09a.TabIndex = 132
        Me.lbl_mbc85_P09a.Text = "[ms]"
        '
        'lbl_mbc85_P09
        '
        Me.lbl_mbc85_P09.AutoSize = True
        Me.lbl_mbc85_P09.Location = New System.Drawing.Point(143, 108)
        Me.lbl_mbc85_P09.Name = "lbl_mbc85_P09"
        Me.lbl_mbc85_P09.Size = New System.Drawing.Size(29, 13)
        Me.lbl_mbc85_P09.TabIndex = 131
        Me.lbl_mbc85_P09.Text = "P09:"
        '
        'tab_mbc86
        '
        Me.tab_mbc86.BackColor = System.Drawing.Color.White
        Me.tab_mbc86.Controls.Add(Me.gb_mbc86_MMLok)
        Me.tab_mbc86.Controls.Add(Me.Label39)
        Me.tab_mbc86.Controls.Add(Me.txt_mbc86_kontakt2cs2)
        Me.tab_mbc86.Controls.Add(Me.txt_mbc86_kontakt1cs2)
        Me.tab_mbc86.Controls.Add(Me.lbl_mbc86_Kontakte)
        Me.tab_mbc86.Controls.Add(Me.txt_mbc86_BusNr)
        Me.tab_mbc86.Controls.Add(Me.lbl_mbc86_BusNr)
        Me.tab_mbc86.Controls.Add(Me.nud_mbc86_S88ModNr)
        Me.tab_mbc86.Controls.Add(Me.lbl_mbc86_ModulNr)
        Me.tab_mbc86.Controls.Add(Me.gb_mbc86_Integrator)
        Me.tab_mbc86.Location = New System.Drawing.Point(4, 22)
        Me.tab_mbc86.Name = "tab_mbc86"
        Me.tab_mbc86.Padding = New System.Windows.Forms.Padding(3)
        Me.tab_mbc86.Size = New System.Drawing.Size(680, 248)
        Me.tab_mbc86.TabIndex = 6
        Me.tab_mbc86.Text = "mbc-86"
        '
        'gb_mbc86_MMLok
        '
        Me.gb_mbc86_MMLok.Controls.Add(Me.lbl_mbc86_MMLok)
        Me.gb_mbc86_MMLok.Controls.Add(Me.nud_mbc86_MMLok)
        Me.gb_mbc86_MMLok.Location = New System.Drawing.Point(320, 119)
        Me.gb_mbc86_MMLok.Name = "gb_mbc86_MMLok"
        Me.gb_mbc86_MMLok.Size = New System.Drawing.Size(173, 110)
        Me.gb_mbc86_MMLok.TabIndex = 187
        Me.gb_mbc86_MMLok.TabStop = False
        Me.gb_mbc86_MMLok.Text = "MM2-Lok"
        '
        'lbl_mbc86_MMLok
        '
        Me.lbl_mbc86_MMLok.AutoSize = True
        Me.lbl_mbc86_MMLok.Location = New System.Drawing.Point(9, 32)
        Me.lbl_mbc86_MMLok.Name = "lbl_mbc86_MMLok"
        Me.lbl_mbc86_MMLok.Size = New System.Drawing.Size(45, 13)
        Me.lbl_mbc86_MMLok.TabIndex = 30
        Me.lbl_mbc86_MMLok.Text = "Adresse"
        '
        'nud_mbc86_MMLok
        '
        Me.nud_mbc86_MMLok.Location = New System.Drawing.Point(101, 30)
        Me.nud_mbc86_MMLok.Maximum = New Decimal(New Integer() {80, 0, 0, 0})
        Me.nud_mbc86_MMLok.Minimum = New Decimal(New Integer() {1, 0, 0, 0})
        Me.nud_mbc86_MMLok.Name = "nud_mbc86_MMLok"
        Me.nud_mbc86_MMLok.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc86_MMLok.TabIndex = 29
        Me.nud_mbc86_MMLok.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc86_MMLok.Value = New Decimal(New Integer() {1, 0, 0, 0})
        '
        'Label39
        '
        Me.Label39.AutoSize = True
        Me.Label39.Location = New System.Drawing.Point(373, 26)
        Me.Label39.Name = "Label39"
        Me.Label39.Size = New System.Drawing.Size(10, 13)
        Me.Label39.TabIndex = 186
        Me.Label39.Text = "-"
        '
        'txt_mbc86_kontakt2cs2
        '
        Me.txt_mbc86_kontakt2cs2.Location = New System.Drawing.Point(387, 23)
        Me.txt_mbc86_kontakt2cs2.Name = "txt_mbc86_kontakt2cs2"
        Me.txt_mbc86_kontakt2cs2.ReadOnly = True
        Me.txt_mbc86_kontakt2cs2.Size = New System.Drawing.Size(41, 20)
        Me.txt_mbc86_kontakt2cs2.TabIndex = 185
        '
        'txt_mbc86_kontakt1cs2
        '
        Me.txt_mbc86_kontakt1cs2.Location = New System.Drawing.Point(327, 23)
        Me.txt_mbc86_kontakt1cs2.Name = "txt_mbc86_kontakt1cs2"
        Me.txt_mbc86_kontakt1cs2.ReadOnly = True
        Me.txt_mbc86_kontakt1cs2.Size = New System.Drawing.Size(40, 20)
        Me.txt_mbc86_kontakt1cs2.TabIndex = 184
        '
        'lbl_mbc86_Kontakte
        '
        Me.lbl_mbc86_Kontakte.AutoSize = True
        Me.lbl_mbc86_Kontakte.Location = New System.Drawing.Point(249, 26)
        Me.lbl_mbc86_Kontakte.Name = "lbl_mbc86_Kontakte"
        Me.lbl_mbc86_Kontakte.Size = New System.Drawing.Size(73, 13)
        Me.lbl_mbc86_Kontakte.TabIndex = 183
        Me.lbl_mbc86_Kontakte.Text = "Kontakte CS2"
        '
        'txt_mbc86_BusNr
        '
        Me.txt_mbc86_BusNr.Location = New System.Drawing.Point(200, 23)
        Me.txt_mbc86_BusNr.Name = "txt_mbc86_BusNr"
        Me.txt_mbc86_BusNr.ReadOnly = True
        Me.txt_mbc86_BusNr.Size = New System.Drawing.Size(25, 20)
        Me.txt_mbc86_BusNr.TabIndex = 182
        '
        'lbl_mbc86_BusNr
        '
        Me.lbl_mbc86_BusNr.AutoSize = True
        Me.lbl_mbc86_BusNr.Location = New System.Drawing.Point(161, 25)
        Me.lbl_mbc86_BusNr.Name = "lbl_mbc86_BusNr"
        Me.lbl_mbc86_BusNr.Size = New System.Drawing.Size(36, 13)
        Me.lbl_mbc86_BusNr.TabIndex = 181
        Me.lbl_mbc86_BusNr.Text = "BusNr"
        '
        'nud_mbc86_S88ModNr
        '
        Me.nud_mbc86_S88ModNr.Location = New System.Drawing.Point(67, 23)
        Me.nud_mbc86_S88ModNr.Maximum = New Decimal(New Integer() {93, 0, 0, 0})
        Me.nud_mbc86_S88ModNr.Minimum = New Decimal(New Integer() {1, 0, 0, 0})
        Me.nud_mbc86_S88ModNr.Name = "nud_mbc86_S88ModNr"
        Me.nud_mbc86_S88ModNr.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc86_S88ModNr.TabIndex = 180
        Me.nud_mbc86_S88ModNr.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc86_S88ModNr.Value = New Decimal(New Integer() {62, 0, 0, 0})
        '
        'lbl_mbc86_ModulNr
        '
        Me.lbl_mbc86_ModulNr.AutoSize = True
        Me.lbl_mbc86_ModulNr.Location = New System.Drawing.Point(15, 25)
        Me.lbl_mbc86_ModulNr.Name = "lbl_mbc86_ModulNr"
        Me.lbl_mbc86_ModulNr.Size = New System.Drawing.Size(47, 13)
        Me.lbl_mbc86_ModulNr.TabIndex = 179
        Me.lbl_mbc86_ModulNr.Text = "ModulNr"
        '
        'gb_mbc86_Integrator
        '
        Me.gb_mbc86_Integrator.Controls.Add(Me.nud_mbc86_T_P05)
        Me.gb_mbc86_Integrator.Controls.Add(Me.Label12)
        Me.gb_mbc86_Integrator.Controls.Add(Me.Label13)
        Me.gb_mbc86_Integrator.Controls.Add(Me.nud_mbc86_T_P06)
        Me.gb_mbc86_Integrator.Controls.Add(Me.Label14)
        Me.gb_mbc86_Integrator.Controls.Add(Me.Label15)
        Me.gb_mbc86_Integrator.Controls.Add(Me.nud_mbc86_T_P04)
        Me.gb_mbc86_Integrator.Controls.Add(Me.Label16)
        Me.gb_mbc86_Integrator.Controls.Add(Me.nud_mbc86_T_P08)
        Me.gb_mbc86_Integrator.Controls.Add(Me.Label17)
        Me.gb_mbc86_Integrator.Controls.Add(Me.Label20)
        Me.gb_mbc86_Integrator.Controls.Add(Me.nud_mbc86_T_P03)
        Me.gb_mbc86_Integrator.Controls.Add(Me.nud_mbc86_T_P07)
        Me.gb_mbc86_Integrator.Controls.Add(Me.Label21)
        Me.gb_mbc86_Integrator.Controls.Add(Me.Label22)
        Me.gb_mbc86_Integrator.Controls.Add(Me.Label23)
        Me.gb_mbc86_Integrator.Controls.Add(Me.Label24)
        Me.gb_mbc86_Integrator.Controls.Add(Me.nud_mbc86_T_P02)
        Me.gb_mbc86_Integrator.Controls.Add(Me.Label25)
        Me.gb_mbc86_Integrator.Controls.Add(Me.Label28)
        Me.gb_mbc86_Integrator.Controls.Add(Me.Label29)
        Me.gb_mbc86_Integrator.Controls.Add(Me.nud_mbc86_T_P01)
        Me.gb_mbc86_Integrator.Controls.Add(Me.Label31)
        Me.gb_mbc86_Integrator.Controls.Add(Me.Label33)
        Me.gb_mbc86_Integrator.Location = New System.Drawing.Point(18, 62)
        Me.gb_mbc86_Integrator.Name = "gb_mbc86_Integrator"
        Me.gb_mbc86_Integrator.Size = New System.Drawing.Size(284, 167)
        Me.gb_mbc86_Integrator.TabIndex = 173
        Me.gb_mbc86_Integrator.TabStop = False
        Me.gb_mbc86_Integrator.Text = "Integrator"
        '
        'nud_mbc86_T_P05
        '
        Me.nud_mbc86_T_P05.Increment = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc86_T_P05.Location = New System.Drawing.Point(49, 132)
        Me.nud_mbc86_T_P05.Maximum = New Decimal(New Integer() {2500, 0, 0, 0})
        Me.nud_mbc86_T_P05.Minimum = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc86_T_P05.Name = "nud_mbc86_T_P05"
        Me.nud_mbc86_T_P05.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc86_T_P05.TabIndex = 127
        Me.nud_mbc86_T_P05.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc86_T_P05.Value = New Decimal(New Integer() {10, 0, 0, 0})
        '
        'Label12
        '
        Me.Label12.AutoSize = True
        Me.Label12.Location = New System.Drawing.Point(108, 134)
        Me.Label12.Name = "Label12"
        Me.Label12.Size = New System.Drawing.Size(26, 13)
        Me.Label12.TabIndex = 129
        Me.Label12.Text = "[ms]"
        '
        'Label13
        '
        Me.Label13.AutoSize = True
        Me.Label13.Location = New System.Drawing.Point(12, 134)
        Me.Label13.Name = "Label13"
        Me.Label13.Size = New System.Drawing.Size(29, 13)
        Me.Label13.TabIndex = 128
        Me.Label13.Text = "P05:"
        '
        'nud_mbc86_T_P06
        '
        Me.nud_mbc86_T_P06.Increment = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc86_T_P06.Location = New System.Drawing.Point(180, 23)
        Me.nud_mbc86_T_P06.Maximum = New Decimal(New Integer() {2500, 0, 0, 0})
        Me.nud_mbc86_T_P06.Minimum = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc86_T_P06.Name = "nud_mbc86_T_P06"
        Me.nud_mbc86_T_P06.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc86_T_P06.TabIndex = 139
        Me.nud_mbc86_T_P06.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc86_T_P06.Value = New Decimal(New Integer() {10, 0, 0, 0})
        '
        'Label14
        '
        Me.Label14.AutoSize = True
        Me.Label14.Location = New System.Drawing.Point(239, 25)
        Me.Label14.Name = "Label14"
        Me.Label14.Size = New System.Drawing.Size(26, 13)
        Me.Label14.TabIndex = 141
        Me.Label14.Text = "[ms]"
        '
        'Label15
        '
        Me.Label15.AutoSize = True
        Me.Label15.Location = New System.Drawing.Point(143, 25)
        Me.Label15.Name = "Label15"
        Me.Label15.Size = New System.Drawing.Size(29, 13)
        Me.Label15.TabIndex = 140
        Me.Label15.Text = "P06:"
        '
        'nud_mbc86_T_P04
        '
        Me.nud_mbc86_T_P04.Increment = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc86_T_P04.Location = New System.Drawing.Point(49, 106)
        Me.nud_mbc86_T_P04.Maximum = New Decimal(New Integer() {2500, 0, 0, 0})
        Me.nud_mbc86_T_P04.Minimum = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc86_T_P04.Name = "nud_mbc86_T_P04"
        Me.nud_mbc86_T_P04.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc86_T_P04.TabIndex = 160
        Me.nud_mbc86_T_P04.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc86_T_P04.Value = New Decimal(New Integer() {10, 0, 0, 0})
        '
        'Label16
        '
        Me.Label16.AutoSize = True
        Me.Label16.Location = New System.Drawing.Point(12, 108)
        Me.Label16.Name = "Label16"
        Me.Label16.Size = New System.Drawing.Size(29, 13)
        Me.Label16.TabIndex = 161
        Me.Label16.Text = "P04:"
        '
        'nud_mbc86_T_P08
        '
        Me.nud_mbc86_T_P08.Increment = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc86_T_P08.Location = New System.Drawing.Point(180, 79)
        Me.nud_mbc86_T_P08.Maximum = New Decimal(New Integer() {2500, 0, 0, 0})
        Me.nud_mbc86_T_P08.Minimum = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc86_T_P08.Name = "nud_mbc86_T_P08"
        Me.nud_mbc86_T_P08.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc86_T_P08.TabIndex = 163
        Me.nud_mbc86_T_P08.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc86_T_P08.Value = New Decimal(New Integer() {10, 0, 0, 0})
        '
        'Label17
        '
        Me.Label17.AutoSize = True
        Me.Label17.Location = New System.Drawing.Point(108, 108)
        Me.Label17.Name = "Label17"
        Me.Label17.Size = New System.Drawing.Size(26, 13)
        Me.Label17.TabIndex = 162
        Me.Label17.Text = "[ms]"
        '
        'Label20
        '
        Me.Label20.AutoSize = True
        Me.Label20.Location = New System.Drawing.Point(143, 81)
        Me.Label20.Name = "Label20"
        Me.Label20.Size = New System.Drawing.Size(29, 13)
        Me.Label20.TabIndex = 164
        Me.Label20.Text = "P08:"
        '
        'nud_mbc86_T_P03
        '
        Me.nud_mbc86_T_P03.Increment = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc86_T_P03.Location = New System.Drawing.Point(49, 79)
        Me.nud_mbc86_T_P03.Maximum = New Decimal(New Integer() {2500, 0, 0, 0})
        Me.nud_mbc86_T_P03.Minimum = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc86_T_P03.Name = "nud_mbc86_T_P03"
        Me.nud_mbc86_T_P03.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc86_T_P03.TabIndex = 148
        Me.nud_mbc86_T_P03.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc86_T_P03.Value = New Decimal(New Integer() {10, 0, 0, 0})
        '
        'nud_mbc86_T_P07
        '
        Me.nud_mbc86_T_P07.Increment = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc86_T_P07.Location = New System.Drawing.Point(180, 51)
        Me.nud_mbc86_T_P07.Maximum = New Decimal(New Integer() {2500, 0, 0, 0})
        Me.nud_mbc86_T_P07.Minimum = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc86_T_P07.Name = "nud_mbc86_T_P07"
        Me.nud_mbc86_T_P07.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc86_T_P07.TabIndex = 151
        Me.nud_mbc86_T_P07.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc86_T_P07.Value = New Decimal(New Integer() {10, 0, 0, 0})
        '
        'Label21
        '
        Me.Label21.AutoSize = True
        Me.Label21.Location = New System.Drawing.Point(12, 81)
        Me.Label21.Name = "Label21"
        Me.Label21.Size = New System.Drawing.Size(29, 13)
        Me.Label21.TabIndex = 149
        Me.Label21.Text = "P03:"
        '
        'Label22
        '
        Me.Label22.AutoSize = True
        Me.Label22.Location = New System.Drawing.Point(239, 81)
        Me.Label22.Name = "Label22"
        Me.Label22.Size = New System.Drawing.Size(26, 13)
        Me.Label22.TabIndex = 165
        Me.Label22.Text = "[ms]"
        '
        'Label23
        '
        Me.Label23.AutoSize = True
        Me.Label23.Location = New System.Drawing.Point(108, 81)
        Me.Label23.Name = "Label23"
        Me.Label23.Size = New System.Drawing.Size(26, 13)
        Me.Label23.TabIndex = 150
        Me.Label23.Text = "[ms]"
        '
        'Label24
        '
        Me.Label24.AutoSize = True
        Me.Label24.Location = New System.Drawing.Point(239, 53)
        Me.Label24.Name = "Label24"
        Me.Label24.Size = New System.Drawing.Size(26, 13)
        Me.Label24.TabIndex = 153
        Me.Label24.Text = "[ms]"
        '
        'nud_mbc86_T_P02
        '
        Me.nud_mbc86_T_P02.Increment = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc86_T_P02.Location = New System.Drawing.Point(49, 51)
        Me.nud_mbc86_T_P02.Maximum = New Decimal(New Integer() {2500, 0, 0, 0})
        Me.nud_mbc86_T_P02.Minimum = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc86_T_P02.Name = "nud_mbc86_T_P02"
        Me.nud_mbc86_T_P02.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc86_T_P02.TabIndex = 136
        Me.nud_mbc86_T_P02.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc86_T_P02.Value = New Decimal(New Integer() {10, 0, 0, 0})
        '
        'Label25
        '
        Me.Label25.AutoSize = True
        Me.Label25.Location = New System.Drawing.Point(143, 53)
        Me.Label25.Name = "Label25"
        Me.Label25.Size = New System.Drawing.Size(29, 13)
        Me.Label25.TabIndex = 152
        Me.Label25.Text = "P07:"
        '
        'Label28
        '
        Me.Label28.AutoSize = True
        Me.Label28.Location = New System.Drawing.Point(12, 53)
        Me.Label28.Name = "Label28"
        Me.Label28.Size = New System.Drawing.Size(29, 13)
        Me.Label28.TabIndex = 137
        Me.Label28.Text = "P02:"
        '
        'Label29
        '
        Me.Label29.AutoSize = True
        Me.Label29.Location = New System.Drawing.Point(108, 53)
        Me.Label29.Name = "Label29"
        Me.Label29.Size = New System.Drawing.Size(26, 13)
        Me.Label29.TabIndex = 138
        Me.Label29.Text = "[ms]"
        '
        'nud_mbc86_T_P01
        '
        Me.nud_mbc86_T_P01.Increment = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc86_T_P01.Location = New System.Drawing.Point(49, 23)
        Me.nud_mbc86_T_P01.Maximum = New Decimal(New Integer() {2500, 0, 0, 0})
        Me.nud_mbc86_T_P01.Minimum = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc86_T_P01.Name = "nud_mbc86_T_P01"
        Me.nud_mbc86_T_P01.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc86_T_P01.TabIndex = 124
        Me.nud_mbc86_T_P01.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc86_T_P01.Value = New Decimal(New Integer() {10, 0, 0, 0})
        '
        'Label31
        '
        Me.Label31.AutoSize = True
        Me.Label31.Location = New System.Drawing.Point(12, 25)
        Me.Label31.Name = "Label31"
        Me.Label31.Size = New System.Drawing.Size(29, 13)
        Me.Label31.TabIndex = 125
        Me.Label31.Text = "P01:"
        '
        'Label33
        '
        Me.Label33.AutoSize = True
        Me.Label33.Location = New System.Drawing.Point(108, 25)
        Me.Label33.Name = "Label33"
        Me.Label33.Size = New System.Drawing.Size(26, 13)
        Me.Label33.TabIndex = 126
        Me.Label33.Text = "[ms]"
        '
        'tab_mbc87
        '
        Me.tab_mbc87.BackColor = System.Drawing.Color.White
        Me.tab_mbc87.Controls.Add(Me.nud_mbc87_MMAddr)
        Me.tab_mbc87.Controls.Add(Me.lbl_mbc87_1)
        Me.tab_mbc87.Location = New System.Drawing.Point(4, 22)
        Me.tab_mbc87.Name = "tab_mbc87"
        Me.tab_mbc87.Padding = New System.Windows.Forms.Padding(3)
        Me.tab_mbc87.Size = New System.Drawing.Size(680, 248)
        Me.tab_mbc87.TabIndex = 7
        Me.tab_mbc87.Text = "mbc-87"
        '
        'nud_mbc87_MMAddr
        '
        Me.nud_mbc87_MMAddr.Location = New System.Drawing.Point(108, 21)
        Me.nud_mbc87_MMAddr.Maximum = New Decimal(New Integer() {80, 0, 0, 0})
        Me.nud_mbc87_MMAddr.Minimum = New Decimal(New Integer() {1, 0, 0, 0})
        Me.nud_mbc87_MMAddr.Name = "nud_mbc87_MMAddr"
        Me.nud_mbc87_MMAddr.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc87_MMAddr.TabIndex = 12
        Me.nud_mbc87_MMAddr.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc87_MMAddr.Value = New Decimal(New Integer() {80, 0, 0, 0})
        '
        'lbl_mbc87_1
        '
        Me.lbl_mbc87_1.AutoSize = True
        Me.lbl_mbc87_1.Location = New System.Drawing.Point(14, 23)
        Me.lbl_mbc87_1.Name = "lbl_mbc87_1"
        Me.lbl_mbc87_1.Size = New System.Drawing.Size(93, 13)
        Me.lbl_mbc87_1.TabIndex = 10
        Me.lbl_mbc87_1.Text = "MM2-Lok-Adresse"
        '
        'tab_mbc88
        '
        Me.tab_mbc88.BackColor = System.Drawing.Color.White
        Me.tab_mbc88.Controls.Add(Me.cmb_mbc88_Halbmodul)
        Me.tab_mbc88.Controls.Add(Me.lbl_mbc88_bis)
        Me.tab_mbc88.Controls.Add(Me.txt_mbc88_kontakt2cs2)
        Me.tab_mbc88.Controls.Add(Me.txt_mbc88_kontakt1cs2)
        Me.tab_mbc88.Controls.Add(Me.lbl_mbc88_kontaktecs2)
        Me.tab_mbc88.Controls.Add(Me.txt_mbc88_busnr)
        Me.tab_mbc88.Controls.Add(Me.lbl_mbc88_busnr)
        Me.tab_mbc88.Controls.Add(Me.lbl_mbc88_halbmodul)
        Me.tab_mbc88.Controls.Add(Me.nud_mbc88_T_P16)
        Me.tab_mbc88.Controls.Add(Me.lbl_mbc88_P16)
        Me.tab_mbc88.Controls.Add(Me.lbl_mbc88_P16a)
        Me.tab_mbc88.Controls.Add(Me.nud_mbc88_T_P12)
        Me.tab_mbc88.Controls.Add(Me.lbl_mbc88_P12)
        Me.tab_mbc88.Controls.Add(Me.lbl_mbc88_P12a)
        Me.tab_mbc88.Controls.Add(Me.nud_mbc88_T_P08)
        Me.tab_mbc88.Controls.Add(Me.lbl_mbc88_P08)
        Me.tab_mbc88.Controls.Add(Me.lbl_mbc88_P08a)
        Me.tab_mbc88.Controls.Add(Me.nud_mbc88_T_P04)
        Me.tab_mbc88.Controls.Add(Me.lbl_mbc88_P04)
        Me.tab_mbc88.Controls.Add(Me.lbl_mbc88_P04a)
        Me.tab_mbc88.Controls.Add(Me.nud_mbc88_T_P15)
        Me.tab_mbc88.Controls.Add(Me.lbl_mbc88_P15)
        Me.tab_mbc88.Controls.Add(Me.lbl_mbc88_P15a)
        Me.tab_mbc88.Controls.Add(Me.nud_mbc88_T_P11)
        Me.tab_mbc88.Controls.Add(Me.lbl_mbc88_P11)
        Me.tab_mbc88.Controls.Add(Me.lbl_mbc88_P11a)
        Me.tab_mbc88.Controls.Add(Me.nud_mbc88_T_P07)
        Me.tab_mbc88.Controls.Add(Me.lbl_mbc88_P07)
        Me.tab_mbc88.Controls.Add(Me.lbl_mbc88_P07a)
        Me.tab_mbc88.Controls.Add(Me.nud_mbc88_T_P03)
        Me.tab_mbc88.Controls.Add(Me.lbl_mbc88_P03)
        Me.tab_mbc88.Controls.Add(Me.lbl_mbc88_P03a)
        Me.tab_mbc88.Controls.Add(Me.nud_mbc88_T_P14)
        Me.tab_mbc88.Controls.Add(Me.lbl_mbc88_P14)
        Me.tab_mbc88.Controls.Add(Me.lbl_mbc88_P14a)
        Me.tab_mbc88.Controls.Add(Me.nud_mbc88_T_P10)
        Me.tab_mbc88.Controls.Add(Me.lbl_mbc88_P10)
        Me.tab_mbc88.Controls.Add(Me.lbl_mbc88_P10a)
        Me.tab_mbc88.Controls.Add(Me.nud_mbc88_T_P06)
        Me.tab_mbc88.Controls.Add(Me.lbl_mbc88_P06)
        Me.tab_mbc88.Controls.Add(Me.lbl_mbc88_P06a)
        Me.tab_mbc88.Controls.Add(Me.nud_mbc88_T_P02)
        Me.tab_mbc88.Controls.Add(Me.lbl_mbc88_P02)
        Me.tab_mbc88.Controls.Add(Me.lbl_mbc88_P02a)
        Me.tab_mbc88.Controls.Add(Me.nud_mbc88_T_P13)
        Me.tab_mbc88.Controls.Add(Me.lbl_mbc88_P13)
        Me.tab_mbc88.Controls.Add(Me.lbl_mbc88_P13a)
        Me.tab_mbc88.Controls.Add(Me.nud_mbc88_T_P09)
        Me.tab_mbc88.Controls.Add(Me.lbl_mbc88_P09)
        Me.tab_mbc88.Controls.Add(Me.lbl_mbc88_P09a)
        Me.tab_mbc88.Controls.Add(Me.nud_mbc88_T_P05)
        Me.tab_mbc88.Controls.Add(Me.lbl_mbc88_P05)
        Me.tab_mbc88.Controls.Add(Me.lbl_mbc88_P05a)
        Me.tab_mbc88.Controls.Add(Me.nud_mbc88_T_P01)
        Me.tab_mbc88.Controls.Add(Me.lbl_mbc88_P01)
        Me.tab_mbc88.Controls.Add(Me.lbl_mbc88_P01a)
        Me.tab_mbc88.Controls.Add(Me.nud_mbc88_S88ModNr)
        Me.tab_mbc88.Controls.Add(Me.lbl_mbc88_modulnr)
        Me.tab_mbc88.Controls.Add(Me.gb_mbc88)
        Me.tab_mbc88.Location = New System.Drawing.Point(4, 22)
        Me.tab_mbc88.Name = "tab_mbc88"
        Me.tab_mbc88.Padding = New System.Windows.Forms.Padding(3)
        Me.tab_mbc88.Size = New System.Drawing.Size(680, 248)
        Me.tab_mbc88.TabIndex = 8
        Me.tab_mbc88.Text = "mbc-88"
        '
        'cmb_mbc88_Halbmodul
        '
        Me.cmb_mbc88_Halbmodul.FormattingEnabled = True
        Me.cmb_mbc88_Halbmodul.Items.AddRange(New Object() {"Eingänge 1 - 8", "Eingänge 9 - 16"})
        Me.cmb_mbc88_Halbmodul.Location = New System.Drawing.Point(218, 21)
        Me.cmb_mbc88_Halbmodul.Name = "cmb_mbc88_Halbmodul"
        Me.cmb_mbc88_Halbmodul.Size = New System.Drawing.Size(112, 21)
        Me.cmb_mbc88_Halbmodul.TabIndex = 69
        '
        'lbl_mbc88_bis
        '
        Me.lbl_mbc88_bis.AutoSize = True
        Me.lbl_mbc88_bis.Location = New System.Drawing.Point(598, 23)
        Me.lbl_mbc88_bis.Name = "lbl_mbc88_bis"
        Me.lbl_mbc88_bis.Size = New System.Drawing.Size(10, 13)
        Me.lbl_mbc88_bis.TabIndex = 68
        Me.lbl_mbc88_bis.Text = "-"
        '
        'txt_mbc88_kontakt2cs2
        '
        Me.txt_mbc88_kontakt2cs2.Location = New System.Drawing.Point(612, 21)
        Me.txt_mbc88_kontakt2cs2.Name = "txt_mbc88_kontakt2cs2"
        Me.txt_mbc88_kontakt2cs2.ReadOnly = True
        Me.txt_mbc88_kontakt2cs2.Size = New System.Drawing.Size(50, 20)
        Me.txt_mbc88_kontakt2cs2.TabIndex = 67
        '
        'txt_mbc88_kontakt1cs2
        '
        Me.txt_mbc88_kontakt1cs2.Location = New System.Drawing.Point(543, 21)
        Me.txt_mbc88_kontakt1cs2.Name = "txt_mbc88_kontakt1cs2"
        Me.txt_mbc88_kontakt1cs2.ReadOnly = True
        Me.txt_mbc88_kontakt1cs2.Size = New System.Drawing.Size(50, 20)
        Me.txt_mbc88_kontakt1cs2.TabIndex = 66
        '
        'lbl_mbc88_kontaktecs2
        '
        Me.lbl_mbc88_kontaktecs2.AutoSize = True
        Me.lbl_mbc88_kontaktecs2.Location = New System.Drawing.Point(463, 23)
        Me.lbl_mbc88_kontaktecs2.Name = "lbl_mbc88_kontaktecs2"
        Me.lbl_mbc88_kontaktecs2.Size = New System.Drawing.Size(73, 13)
        Me.lbl_mbc88_kontaktecs2.TabIndex = 65
        Me.lbl_mbc88_kontaktecs2.Text = "Kontakte CS2"
        '
        'txt_mbc88_busnr
        '
        Me.txt_mbc88_busnr.Location = New System.Drawing.Point(404, 21)
        Me.txt_mbc88_busnr.Name = "txt_mbc88_busnr"
        Me.txt_mbc88_busnr.ReadOnly = True
        Me.txt_mbc88_busnr.Size = New System.Drawing.Size(40, 20)
        Me.txt_mbc88_busnr.TabIndex = 64
        '
        'lbl_mbc88_busnr
        '
        Me.lbl_mbc88_busnr.AutoSize = True
        Me.lbl_mbc88_busnr.Location = New System.Drawing.Point(357, 23)
        Me.lbl_mbc88_busnr.Name = "lbl_mbc88_busnr"
        Me.lbl_mbc88_busnr.Size = New System.Drawing.Size(36, 13)
        Me.lbl_mbc88_busnr.TabIndex = 63
        Me.lbl_mbc88_busnr.Text = "BusNr"
        '
        'lbl_mbc88_halbmodul
        '
        Me.lbl_mbc88_halbmodul.AutoSize = True
        Me.lbl_mbc88_halbmodul.Location = New System.Drawing.Point(155, 23)
        Me.lbl_mbc88_halbmodul.Name = "lbl_mbc88_halbmodul"
        Me.lbl_mbc88_halbmodul.Size = New System.Drawing.Size(57, 13)
        Me.lbl_mbc88_halbmodul.TabIndex = 62
        Me.lbl_mbc88_halbmodul.Text = "Halbmodul"
        '
        'nud_mbc88_T_P16
        '
        Me.nud_mbc88_T_P16.Increment = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc88_T_P16.Location = New System.Drawing.Point(558, 198)
        Me.nud_mbc88_T_P16.Maximum = New Decimal(New Integer() {2500, 0, 0, 0})
        Me.nud_mbc88_T_P16.Minimum = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc88_T_P16.Name = "nud_mbc88_T_P16"
        Me.nud_mbc88_T_P16.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc88_T_P16.TabIndex = 56
        Me.nud_mbc88_T_P16.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc88_T_P16.Value = New Decimal(New Integer() {10, 0, 0, 0})
        '
        'lbl_mbc88_P16
        '
        Me.lbl_mbc88_P16.AutoSize = True
        Me.lbl_mbc88_P16.Location = New System.Drawing.Point(521, 200)
        Me.lbl_mbc88_P16.Name = "lbl_mbc88_P16"
        Me.lbl_mbc88_P16.Size = New System.Drawing.Size(29, 13)
        Me.lbl_mbc88_P16.TabIndex = 57
        Me.lbl_mbc88_P16.Text = "P16:"
        '
        'lbl_mbc88_P16a
        '
        Me.lbl_mbc88_P16a.AutoSize = True
        Me.lbl_mbc88_P16a.Location = New System.Drawing.Point(618, 200)
        Me.lbl_mbc88_P16a.Name = "lbl_mbc88_P16a"
        Me.lbl_mbc88_P16a.Size = New System.Drawing.Size(26, 13)
        Me.lbl_mbc88_P16a.TabIndex = 58
        Me.lbl_mbc88_P16a.Text = "[ms]"
        '
        'nud_mbc88_T_P12
        '
        Me.nud_mbc88_T_P12.Increment = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc88_T_P12.Location = New System.Drawing.Point(394, 198)
        Me.nud_mbc88_T_P12.Maximum = New Decimal(New Integer() {2500, 0, 0, 0})
        Me.nud_mbc88_T_P12.Minimum = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc88_T_P12.Name = "nud_mbc88_T_P12"
        Me.nud_mbc88_T_P12.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc88_T_P12.TabIndex = 53
        Me.nud_mbc88_T_P12.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc88_T_P12.Value = New Decimal(New Integer() {10, 0, 0, 0})
        '
        'lbl_mbc88_P12
        '
        Me.lbl_mbc88_P12.AutoSize = True
        Me.lbl_mbc88_P12.Location = New System.Drawing.Point(357, 200)
        Me.lbl_mbc88_P12.Name = "lbl_mbc88_P12"
        Me.lbl_mbc88_P12.Size = New System.Drawing.Size(29, 13)
        Me.lbl_mbc88_P12.TabIndex = 54
        Me.lbl_mbc88_P12.Text = "P12:"
        '
        'lbl_mbc88_P12a
        '
        Me.lbl_mbc88_P12a.AutoSize = True
        Me.lbl_mbc88_P12a.Location = New System.Drawing.Point(453, 200)
        Me.lbl_mbc88_P12a.Name = "lbl_mbc88_P12a"
        Me.lbl_mbc88_P12a.Size = New System.Drawing.Size(26, 13)
        Me.lbl_mbc88_P12a.TabIndex = 55
        Me.lbl_mbc88_P12a.Text = "[ms]"
        '
        'nud_mbc88_T_P08
        '
        Me.nud_mbc88_T_P08.Increment = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc88_T_P08.Location = New System.Drawing.Point(230, 198)
        Me.nud_mbc88_T_P08.Maximum = New Decimal(New Integer() {2500, 0, 0, 0})
        Me.nud_mbc88_T_P08.Minimum = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc88_T_P08.Name = "nud_mbc88_T_P08"
        Me.nud_mbc88_T_P08.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc88_T_P08.TabIndex = 50
        Me.nud_mbc88_T_P08.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc88_T_P08.Value = New Decimal(New Integer() {10, 0, 0, 0})
        '
        'lbl_mbc88_P08
        '
        Me.lbl_mbc88_P08.AutoSize = True
        Me.lbl_mbc88_P08.Location = New System.Drawing.Point(193, 200)
        Me.lbl_mbc88_P08.Name = "lbl_mbc88_P08"
        Me.lbl_mbc88_P08.Size = New System.Drawing.Size(29, 13)
        Me.lbl_mbc88_P08.TabIndex = 51
        Me.lbl_mbc88_P08.Text = "P08:"
        '
        'lbl_mbc88_P08a
        '
        Me.lbl_mbc88_P08a.AutoSize = True
        Me.lbl_mbc88_P08a.Location = New System.Drawing.Point(289, 200)
        Me.lbl_mbc88_P08a.Name = "lbl_mbc88_P08a"
        Me.lbl_mbc88_P08a.Size = New System.Drawing.Size(26, 13)
        Me.lbl_mbc88_P08a.TabIndex = 52
        Me.lbl_mbc88_P08a.Text = "[ms]"
        '
        'nud_mbc88_T_P04
        '
        Me.nud_mbc88_T_P04.Increment = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc88_T_P04.Location = New System.Drawing.Point(66, 198)
        Me.nud_mbc88_T_P04.Maximum = New Decimal(New Integer() {2500, 0, 0, 0})
        Me.nud_mbc88_T_P04.Minimum = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc88_T_P04.Name = "nud_mbc88_T_P04"
        Me.nud_mbc88_T_P04.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc88_T_P04.TabIndex = 47
        Me.nud_mbc88_T_P04.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc88_T_P04.Value = New Decimal(New Integer() {10, 0, 0, 0})
        '
        'lbl_mbc88_P04
        '
        Me.lbl_mbc88_P04.AutoSize = True
        Me.lbl_mbc88_P04.Location = New System.Drawing.Point(29, 200)
        Me.lbl_mbc88_P04.Name = "lbl_mbc88_P04"
        Me.lbl_mbc88_P04.Size = New System.Drawing.Size(29, 13)
        Me.lbl_mbc88_P04.TabIndex = 48
        Me.lbl_mbc88_P04.Text = "P04:"
        '
        'lbl_mbc88_P04a
        '
        Me.lbl_mbc88_P04a.AutoSize = True
        Me.lbl_mbc88_P04a.Location = New System.Drawing.Point(125, 200)
        Me.lbl_mbc88_P04a.Name = "lbl_mbc88_P04a"
        Me.lbl_mbc88_P04a.Size = New System.Drawing.Size(26, 13)
        Me.lbl_mbc88_P04a.TabIndex = 49
        Me.lbl_mbc88_P04a.Text = "[ms]"
        '
        'nud_mbc88_T_P15
        '
        Me.nud_mbc88_T_P15.Increment = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc88_T_P15.Location = New System.Drawing.Point(558, 158)
        Me.nud_mbc88_T_P15.Maximum = New Decimal(New Integer() {2500, 0, 0, 0})
        Me.nud_mbc88_T_P15.Minimum = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc88_T_P15.Name = "nud_mbc88_T_P15"
        Me.nud_mbc88_T_P15.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc88_T_P15.TabIndex = 44
        Me.nud_mbc88_T_P15.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc88_T_P15.Value = New Decimal(New Integer() {10, 0, 0, 0})
        '
        'lbl_mbc88_P15
        '
        Me.lbl_mbc88_P15.AutoSize = True
        Me.lbl_mbc88_P15.Location = New System.Drawing.Point(521, 160)
        Me.lbl_mbc88_P15.Name = "lbl_mbc88_P15"
        Me.lbl_mbc88_P15.Size = New System.Drawing.Size(29, 13)
        Me.lbl_mbc88_P15.TabIndex = 45
        Me.lbl_mbc88_P15.Text = "P15:"
        '
        'lbl_mbc88_P15a
        '
        Me.lbl_mbc88_P15a.AutoSize = True
        Me.lbl_mbc88_P15a.Location = New System.Drawing.Point(618, 160)
        Me.lbl_mbc88_P15a.Name = "lbl_mbc88_P15a"
        Me.lbl_mbc88_P15a.Size = New System.Drawing.Size(26, 13)
        Me.lbl_mbc88_P15a.TabIndex = 46
        Me.lbl_mbc88_P15a.Text = "[ms]"
        '
        'nud_mbc88_T_P11
        '
        Me.nud_mbc88_T_P11.Increment = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc88_T_P11.Location = New System.Drawing.Point(394, 158)
        Me.nud_mbc88_T_P11.Maximum = New Decimal(New Integer() {2500, 0, 0, 0})
        Me.nud_mbc88_T_P11.Minimum = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc88_T_P11.Name = "nud_mbc88_T_P11"
        Me.nud_mbc88_T_P11.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc88_T_P11.TabIndex = 41
        Me.nud_mbc88_T_P11.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc88_T_P11.Value = New Decimal(New Integer() {10, 0, 0, 0})
        '
        'lbl_mbc88_P11
        '
        Me.lbl_mbc88_P11.AutoSize = True
        Me.lbl_mbc88_P11.Location = New System.Drawing.Point(357, 160)
        Me.lbl_mbc88_P11.Name = "lbl_mbc88_P11"
        Me.lbl_mbc88_P11.Size = New System.Drawing.Size(29, 13)
        Me.lbl_mbc88_P11.TabIndex = 42
        Me.lbl_mbc88_P11.Text = "P11:"
        '
        'lbl_mbc88_P11a
        '
        Me.lbl_mbc88_P11a.AutoSize = True
        Me.lbl_mbc88_P11a.Location = New System.Drawing.Point(453, 160)
        Me.lbl_mbc88_P11a.Name = "lbl_mbc88_P11a"
        Me.lbl_mbc88_P11a.Size = New System.Drawing.Size(26, 13)
        Me.lbl_mbc88_P11a.TabIndex = 43
        Me.lbl_mbc88_P11a.Text = "[ms]"
        '
        'nud_mbc88_T_P07
        '
        Me.nud_mbc88_T_P07.Increment = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc88_T_P07.Location = New System.Drawing.Point(230, 158)
        Me.nud_mbc88_T_P07.Maximum = New Decimal(New Integer() {2500, 0, 0, 0})
        Me.nud_mbc88_T_P07.Minimum = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc88_T_P07.Name = "nud_mbc88_T_P07"
        Me.nud_mbc88_T_P07.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc88_T_P07.TabIndex = 38
        Me.nud_mbc88_T_P07.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc88_T_P07.Value = New Decimal(New Integer() {10, 0, 0, 0})
        '
        'lbl_mbc88_P07
        '
        Me.lbl_mbc88_P07.AutoSize = True
        Me.lbl_mbc88_P07.Location = New System.Drawing.Point(193, 160)
        Me.lbl_mbc88_P07.Name = "lbl_mbc88_P07"
        Me.lbl_mbc88_P07.Size = New System.Drawing.Size(29, 13)
        Me.lbl_mbc88_P07.TabIndex = 39
        Me.lbl_mbc88_P07.Text = "P07:"
        '
        'lbl_mbc88_P07a
        '
        Me.lbl_mbc88_P07a.AutoSize = True
        Me.lbl_mbc88_P07a.Location = New System.Drawing.Point(289, 160)
        Me.lbl_mbc88_P07a.Name = "lbl_mbc88_P07a"
        Me.lbl_mbc88_P07a.Size = New System.Drawing.Size(26, 13)
        Me.lbl_mbc88_P07a.TabIndex = 40
        Me.lbl_mbc88_P07a.Text = "[ms]"
        '
        'nud_mbc88_T_P03
        '
        Me.nud_mbc88_T_P03.Increment = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc88_T_P03.Location = New System.Drawing.Point(66, 158)
        Me.nud_mbc88_T_P03.Maximum = New Decimal(New Integer() {2500, 0, 0, 0})
        Me.nud_mbc88_T_P03.Minimum = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc88_T_P03.Name = "nud_mbc88_T_P03"
        Me.nud_mbc88_T_P03.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc88_T_P03.TabIndex = 35
        Me.nud_mbc88_T_P03.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc88_T_P03.Value = New Decimal(New Integer() {10, 0, 0, 0})
        '
        'lbl_mbc88_P03
        '
        Me.lbl_mbc88_P03.AutoSize = True
        Me.lbl_mbc88_P03.Location = New System.Drawing.Point(29, 160)
        Me.lbl_mbc88_P03.Name = "lbl_mbc88_P03"
        Me.lbl_mbc88_P03.Size = New System.Drawing.Size(29, 13)
        Me.lbl_mbc88_P03.TabIndex = 36
        Me.lbl_mbc88_P03.Text = "P03:"
        '
        'lbl_mbc88_P03a
        '
        Me.lbl_mbc88_P03a.AutoSize = True
        Me.lbl_mbc88_P03a.Location = New System.Drawing.Point(125, 160)
        Me.lbl_mbc88_P03a.Name = "lbl_mbc88_P03a"
        Me.lbl_mbc88_P03a.Size = New System.Drawing.Size(26, 13)
        Me.lbl_mbc88_P03a.TabIndex = 37
        Me.lbl_mbc88_P03a.Text = "[ms]"
        '
        'nud_mbc88_T_P14
        '
        Me.nud_mbc88_T_P14.Increment = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc88_T_P14.Location = New System.Drawing.Point(558, 118)
        Me.nud_mbc88_T_P14.Maximum = New Decimal(New Integer() {2500, 0, 0, 0})
        Me.nud_mbc88_T_P14.Minimum = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc88_T_P14.Name = "nud_mbc88_T_P14"
        Me.nud_mbc88_T_P14.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc88_T_P14.TabIndex = 32
        Me.nud_mbc88_T_P14.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc88_T_P14.Value = New Decimal(New Integer() {10, 0, 0, 0})
        '
        'lbl_mbc88_P14
        '
        Me.lbl_mbc88_P14.AutoSize = True
        Me.lbl_mbc88_P14.Location = New System.Drawing.Point(521, 120)
        Me.lbl_mbc88_P14.Name = "lbl_mbc88_P14"
        Me.lbl_mbc88_P14.Size = New System.Drawing.Size(29, 13)
        Me.lbl_mbc88_P14.TabIndex = 33
        Me.lbl_mbc88_P14.Text = "P14:"
        '
        'lbl_mbc88_P14a
        '
        Me.lbl_mbc88_P14a.AutoSize = True
        Me.lbl_mbc88_P14a.Location = New System.Drawing.Point(618, 120)
        Me.lbl_mbc88_P14a.Name = "lbl_mbc88_P14a"
        Me.lbl_mbc88_P14a.Size = New System.Drawing.Size(26, 13)
        Me.lbl_mbc88_P14a.TabIndex = 34
        Me.lbl_mbc88_P14a.Text = "[ms]"
        '
        'nud_mbc88_T_P10
        '
        Me.nud_mbc88_T_P10.Increment = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc88_T_P10.Location = New System.Drawing.Point(394, 118)
        Me.nud_mbc88_T_P10.Maximum = New Decimal(New Integer() {2500, 0, 0, 0})
        Me.nud_mbc88_T_P10.Minimum = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc88_T_P10.Name = "nud_mbc88_T_P10"
        Me.nud_mbc88_T_P10.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc88_T_P10.TabIndex = 29
        Me.nud_mbc88_T_P10.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc88_T_P10.Value = New Decimal(New Integer() {10, 0, 0, 0})
        '
        'lbl_mbc88_P10
        '
        Me.lbl_mbc88_P10.AutoSize = True
        Me.lbl_mbc88_P10.Location = New System.Drawing.Point(357, 120)
        Me.lbl_mbc88_P10.Name = "lbl_mbc88_P10"
        Me.lbl_mbc88_P10.Size = New System.Drawing.Size(29, 13)
        Me.lbl_mbc88_P10.TabIndex = 30
        Me.lbl_mbc88_P10.Text = "P10:"
        '
        'lbl_mbc88_P10a
        '
        Me.lbl_mbc88_P10a.AutoSize = True
        Me.lbl_mbc88_P10a.Location = New System.Drawing.Point(453, 120)
        Me.lbl_mbc88_P10a.Name = "lbl_mbc88_P10a"
        Me.lbl_mbc88_P10a.Size = New System.Drawing.Size(26, 13)
        Me.lbl_mbc88_P10a.TabIndex = 31
        Me.lbl_mbc88_P10a.Text = "[ms]"
        '
        'nud_mbc88_T_P06
        '
        Me.nud_mbc88_T_P06.Increment = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc88_T_P06.Location = New System.Drawing.Point(230, 118)
        Me.nud_mbc88_T_P06.Maximum = New Decimal(New Integer() {2500, 0, 0, 0})
        Me.nud_mbc88_T_P06.Minimum = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc88_T_P06.Name = "nud_mbc88_T_P06"
        Me.nud_mbc88_T_P06.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc88_T_P06.TabIndex = 26
        Me.nud_mbc88_T_P06.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc88_T_P06.Value = New Decimal(New Integer() {10, 0, 0, 0})
        '
        'lbl_mbc88_P06
        '
        Me.lbl_mbc88_P06.AutoSize = True
        Me.lbl_mbc88_P06.Location = New System.Drawing.Point(193, 120)
        Me.lbl_mbc88_P06.Name = "lbl_mbc88_P06"
        Me.lbl_mbc88_P06.Size = New System.Drawing.Size(29, 13)
        Me.lbl_mbc88_P06.TabIndex = 27
        Me.lbl_mbc88_P06.Text = "P06:"
        '
        'lbl_mbc88_P06a
        '
        Me.lbl_mbc88_P06a.AutoSize = True
        Me.lbl_mbc88_P06a.Location = New System.Drawing.Point(289, 120)
        Me.lbl_mbc88_P06a.Name = "lbl_mbc88_P06a"
        Me.lbl_mbc88_P06a.Size = New System.Drawing.Size(26, 13)
        Me.lbl_mbc88_P06a.TabIndex = 28
        Me.lbl_mbc88_P06a.Text = "[ms]"
        '
        'nud_mbc88_T_P02
        '
        Me.nud_mbc88_T_P02.Increment = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc88_T_P02.Location = New System.Drawing.Point(66, 118)
        Me.nud_mbc88_T_P02.Maximum = New Decimal(New Integer() {2500, 0, 0, 0})
        Me.nud_mbc88_T_P02.Minimum = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc88_T_P02.Name = "nud_mbc88_T_P02"
        Me.nud_mbc88_T_P02.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc88_T_P02.TabIndex = 23
        Me.nud_mbc88_T_P02.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc88_T_P02.Value = New Decimal(New Integer() {10, 0, 0, 0})
        '
        'lbl_mbc88_P02
        '
        Me.lbl_mbc88_P02.AutoSize = True
        Me.lbl_mbc88_P02.Location = New System.Drawing.Point(29, 120)
        Me.lbl_mbc88_P02.Name = "lbl_mbc88_P02"
        Me.lbl_mbc88_P02.Size = New System.Drawing.Size(29, 13)
        Me.lbl_mbc88_P02.TabIndex = 24
        Me.lbl_mbc88_P02.Text = "P02:"
        '
        'lbl_mbc88_P02a
        '
        Me.lbl_mbc88_P02a.AutoSize = True
        Me.lbl_mbc88_P02a.Location = New System.Drawing.Point(125, 120)
        Me.lbl_mbc88_P02a.Name = "lbl_mbc88_P02a"
        Me.lbl_mbc88_P02a.Size = New System.Drawing.Size(26, 13)
        Me.lbl_mbc88_P02a.TabIndex = 25
        Me.lbl_mbc88_P02a.Text = "[ms]"
        '
        'nud_mbc88_T_P13
        '
        Me.nud_mbc88_T_P13.Increment = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc88_T_P13.Location = New System.Drawing.Point(558, 78)
        Me.nud_mbc88_T_P13.Maximum = New Decimal(New Integer() {2500, 0, 0, 0})
        Me.nud_mbc88_T_P13.Minimum = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc88_T_P13.Name = "nud_mbc88_T_P13"
        Me.nud_mbc88_T_P13.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc88_T_P13.TabIndex = 20
        Me.nud_mbc88_T_P13.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc88_T_P13.Value = New Decimal(New Integer() {10, 0, 0, 0})
        '
        'lbl_mbc88_P13
        '
        Me.lbl_mbc88_P13.AutoSize = True
        Me.lbl_mbc88_P13.Location = New System.Drawing.Point(521, 80)
        Me.lbl_mbc88_P13.Name = "lbl_mbc88_P13"
        Me.lbl_mbc88_P13.Size = New System.Drawing.Size(29, 13)
        Me.lbl_mbc88_P13.TabIndex = 21
        Me.lbl_mbc88_P13.Text = "P13:"
        '
        'lbl_mbc88_P13a
        '
        Me.lbl_mbc88_P13a.AutoSize = True
        Me.lbl_mbc88_P13a.Location = New System.Drawing.Point(618, 80)
        Me.lbl_mbc88_P13a.Name = "lbl_mbc88_P13a"
        Me.lbl_mbc88_P13a.Size = New System.Drawing.Size(26, 13)
        Me.lbl_mbc88_P13a.TabIndex = 22
        Me.lbl_mbc88_P13a.Text = "[ms]"
        '
        'nud_mbc88_T_P09
        '
        Me.nud_mbc88_T_P09.Increment = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc88_T_P09.Location = New System.Drawing.Point(394, 78)
        Me.nud_mbc88_T_P09.Maximum = New Decimal(New Integer() {2500, 0, 0, 0})
        Me.nud_mbc88_T_P09.Minimum = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc88_T_P09.Name = "nud_mbc88_T_P09"
        Me.nud_mbc88_T_P09.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc88_T_P09.TabIndex = 17
        Me.nud_mbc88_T_P09.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc88_T_P09.Value = New Decimal(New Integer() {10, 0, 0, 0})
        '
        'lbl_mbc88_P09
        '
        Me.lbl_mbc88_P09.AutoSize = True
        Me.lbl_mbc88_P09.Location = New System.Drawing.Point(357, 80)
        Me.lbl_mbc88_P09.Name = "lbl_mbc88_P09"
        Me.lbl_mbc88_P09.Size = New System.Drawing.Size(29, 13)
        Me.lbl_mbc88_P09.TabIndex = 18
        Me.lbl_mbc88_P09.Text = "P09:"
        '
        'lbl_mbc88_P09a
        '
        Me.lbl_mbc88_P09a.AutoSize = True
        Me.lbl_mbc88_P09a.Location = New System.Drawing.Point(453, 80)
        Me.lbl_mbc88_P09a.Name = "lbl_mbc88_P09a"
        Me.lbl_mbc88_P09a.Size = New System.Drawing.Size(26, 13)
        Me.lbl_mbc88_P09a.TabIndex = 19
        Me.lbl_mbc88_P09a.Text = "[ms]"
        '
        'nud_mbc88_T_P05
        '
        Me.nud_mbc88_T_P05.Increment = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc88_T_P05.Location = New System.Drawing.Point(230, 78)
        Me.nud_mbc88_T_P05.Maximum = New Decimal(New Integer() {2500, 0, 0, 0})
        Me.nud_mbc88_T_P05.Minimum = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc88_T_P05.Name = "nud_mbc88_T_P05"
        Me.nud_mbc88_T_P05.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc88_T_P05.TabIndex = 14
        Me.nud_mbc88_T_P05.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc88_T_P05.Value = New Decimal(New Integer() {10, 0, 0, 0})
        '
        'lbl_mbc88_P05
        '
        Me.lbl_mbc88_P05.AutoSize = True
        Me.lbl_mbc88_P05.Location = New System.Drawing.Point(193, 80)
        Me.lbl_mbc88_P05.Name = "lbl_mbc88_P05"
        Me.lbl_mbc88_P05.Size = New System.Drawing.Size(29, 13)
        Me.lbl_mbc88_P05.TabIndex = 15
        Me.lbl_mbc88_P05.Text = "P05:"
        '
        'lbl_mbc88_P05a
        '
        Me.lbl_mbc88_P05a.AutoSize = True
        Me.lbl_mbc88_P05a.Location = New System.Drawing.Point(289, 80)
        Me.lbl_mbc88_P05a.Name = "lbl_mbc88_P05a"
        Me.lbl_mbc88_P05a.Size = New System.Drawing.Size(26, 13)
        Me.lbl_mbc88_P05a.TabIndex = 16
        Me.lbl_mbc88_P05a.Text = "[ms]"
        '
        'nud_mbc88_T_P01
        '
        Me.nud_mbc88_T_P01.Increment = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc88_T_P01.Location = New System.Drawing.Point(66, 78)
        Me.nud_mbc88_T_P01.Maximum = New Decimal(New Integer() {2500, 0, 0, 0})
        Me.nud_mbc88_T_P01.Minimum = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc88_T_P01.Name = "nud_mbc88_T_P01"
        Me.nud_mbc88_T_P01.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc88_T_P01.TabIndex = 11
        Me.nud_mbc88_T_P01.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc88_T_P01.Value = New Decimal(New Integer() {10, 0, 0, 0})
        '
        'lbl_mbc88_P01
        '
        Me.lbl_mbc88_P01.AutoSize = True
        Me.lbl_mbc88_P01.Location = New System.Drawing.Point(29, 80)
        Me.lbl_mbc88_P01.Name = "lbl_mbc88_P01"
        Me.lbl_mbc88_P01.Size = New System.Drawing.Size(29, 13)
        Me.lbl_mbc88_P01.TabIndex = 12
        Me.lbl_mbc88_P01.Text = "P01:"
        '
        'lbl_mbc88_P01a
        '
        Me.lbl_mbc88_P01a.AutoSize = True
        Me.lbl_mbc88_P01a.Location = New System.Drawing.Point(125, 80)
        Me.lbl_mbc88_P01a.Name = "lbl_mbc88_P01a"
        Me.lbl_mbc88_P01a.Size = New System.Drawing.Size(26, 13)
        Me.lbl_mbc88_P01a.TabIndex = 13
        Me.lbl_mbc88_P01a.Text = "[ms]"
        '
        'nud_mbc88_S88ModNr
        '
        Me.nud_mbc88_S88ModNr.Location = New System.Drawing.Point(66, 21)
        Me.nud_mbc88_S88ModNr.Maximum = New Decimal(New Integer() {93, 0, 0, 0})
        Me.nud_mbc88_S88ModNr.Minimum = New Decimal(New Integer() {1, 0, 0, 0})
        Me.nud_mbc88_S88ModNr.Name = "nud_mbc88_S88ModNr"
        Me.nud_mbc88_S88ModNr.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc88_S88ModNr.TabIndex = 9
        Me.nud_mbc88_S88ModNr.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc88_S88ModNr.Value = New Decimal(New Integer() {62, 0, 0, 0})
        '
        'lbl_mbc88_modulnr
        '
        Me.lbl_mbc88_modulnr.AutoSize = True
        Me.lbl_mbc88_modulnr.Location = New System.Drawing.Point(14, 23)
        Me.lbl_mbc88_modulnr.Name = "lbl_mbc88_modulnr"
        Me.lbl_mbc88_modulnr.Size = New System.Drawing.Size(47, 13)
        Me.lbl_mbc88_modulnr.TabIndex = 7
        Me.lbl_mbc88_modulnr.Text = "ModulNr"
        '
        'gb_mbc88
        '
        Me.gb_mbc88.Location = New System.Drawing.Point(17, 47)
        Me.gb_mbc88.Name = "gb_mbc88"
        Me.gb_mbc88.Size = New System.Drawing.Size(648, 186)
        Me.gb_mbc88.TabIndex = 60
        Me.gb_mbc88.TabStop = False
        Me.gb_mbc88.Text = "Integrator"
        '
        'tab_mbc89
        '
        Me.tab_mbc89.BackColor = System.Drawing.Color.White
        Me.tab_mbc89.Controls.Add(Me.gb_mbc89_I2C)
        Me.tab_mbc89.Controls.Add(Me.gb_mbc89_Basisadresse)
        Me.tab_mbc89.Controls.Add(Me.btn_mbc89_Loks)
        Me.tab_mbc89.Controls.Add(Me.gb_mbc89_Mapping)
        Me.tab_mbc89.Controls.Add(Me.gb_mbc89_Keyboard)
        Me.tab_mbc89.Controls.Add(Me.gb_mbc89_Version)
        Me.tab_mbc89.Controls.Add(Me.txt_mbc89_alt)
        Me.tab_mbc89.Location = New System.Drawing.Point(4, 22)
        Me.tab_mbc89.Name = "tab_mbc89"
        Me.tab_mbc89.Padding = New System.Windows.Forms.Padding(3)
        Me.tab_mbc89.Size = New System.Drawing.Size(680, 248)
        Me.tab_mbc89.TabIndex = 9
        Me.tab_mbc89.Text = "mbc-89"
        '
        'gb_mbc89_I2C
        '
        Me.gb_mbc89_I2C.Controls.Add(Me.lbl_mbc89_frequenz)
        Me.gb_mbc89_I2C.Controls.Add(Me.lbl_mbc89_takt)
        Me.gb_mbc89_I2C.Controls.Add(Me.nud_mbc89_i2c)
        Me.gb_mbc89_I2C.Location = New System.Drawing.Point(440, 108)
        Me.gb_mbc89_I2C.Name = "gb_mbc89_I2C"
        Me.gb_mbc89_I2C.Size = New System.Drawing.Size(234, 41)
        Me.gb_mbc89_I2C.TabIndex = 72
        Me.gb_mbc89_I2C.TabStop = False
        Me.gb_mbc89_I2C.Text = "I2C-Bus-Clock"
        '
        'lbl_mbc89_frequenz
        '
        Me.lbl_mbc89_frequenz.AutoSize = True
        Me.lbl_mbc89_frequenz.Location = New System.Drawing.Point(176, 17)
        Me.lbl_mbc89_frequenz.Name = "lbl_mbc89_frequenz"
        Me.lbl_mbc89_frequenz.Size = New System.Drawing.Size(32, 13)
        Me.lbl_mbc89_frequenz.TabIndex = 31
        Me.lbl_mbc89_frequenz.Text = "[kHz]"
        '
        'lbl_mbc89_takt
        '
        Me.lbl_mbc89_takt.AutoSize = True
        Me.lbl_mbc89_takt.Location = New System.Drawing.Point(11, 17)
        Me.lbl_mbc89_takt.Name = "lbl_mbc89_takt"
        Me.lbl_mbc89_takt.Size = New System.Drawing.Size(29, 13)
        Me.lbl_mbc89_takt.TabIndex = 30
        Me.lbl_mbc89_takt.Text = "Takt"
        '
        'nud_mbc89_i2c
        '
        Me.nud_mbc89_i2c.Location = New System.Drawing.Point(95, 15)
        Me.nud_mbc89_i2c.Minimum = New Decimal(New Integer() {1, 0, 0, 0})
        Me.nud_mbc89_i2c.Name = "nud_mbc89_i2c"
        Me.nud_mbc89_i2c.Size = New System.Drawing.Size(69, 20)
        Me.nud_mbc89_i2c.TabIndex = 29
        Me.nud_mbc89_i2c.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        Me.nud_mbc89_i2c.Value = New Decimal(New Integer() {1, 0, 0, 0})
        '
        'gb_mbc89_Basisadresse
        '
        Me.gb_mbc89_Basisadresse.Controls.Add(Me.lbl_mbc89_Basisadresse)
        Me.gb_mbc89_Basisadresse.Controls.Add(Me.nud_mbc89_basis)
        Me.gb_mbc89_Basisadresse.Location = New System.Drawing.Point(440, 6)
        Me.gb_mbc89_Basisadresse.Name = "gb_mbc89_Basisadresse"
        Me.gb_mbc89_Basisadresse.Size = New System.Drawing.Size(234, 42)
        Me.gb_mbc89_Basisadresse.TabIndex = 71
        Me.gb_mbc89_Basisadresse.TabStop = False
        Me.gb_mbc89_Basisadresse.Text = "Basisadresse"
        '
        'lbl_mbc89_Basisadresse
        '
        Me.lbl_mbc89_Basisadresse.AutoSize = True
        Me.lbl_mbc89_Basisadresse.Location = New System.Drawing.Point(11, 17)
        Me.lbl_mbc89_Basisadresse.Name = "lbl_mbc89_Basisadresse"
        Me.lbl_mbc89_Basisadresse.Size = New System.Drawing.Size(78, 13)
        Me.lbl_mbc89_Basisadresse.TabIndex = 30
        Me.lbl_mbc89_Basisadresse.Text = "Keyboard Nr. 1"
        '
        'nud_mbc89_basis
        '
        Me.nud_mbc89_basis.Increment = New Decimal(New Integer() {256, 0, 0, 0})
        Me.nud_mbc89_basis.Location = New System.Drawing.Point(95, 15)
        Me.nud_mbc89_basis.Maximum = New Decimal(New Integer() {769, 0, 0, 0})
        Me.nud_mbc89_basis.Minimum = New Decimal(New Integer() {1, 0, 0, 0})
        Me.nud_mbc89_basis.Name = "nud_mbc89_basis"
        Me.nud_mbc89_basis.Size = New System.Drawing.Size(69, 20)
        Me.nud_mbc89_basis.TabIndex = 29
        Me.nud_mbc89_basis.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        Me.nud_mbc89_basis.Value = New Decimal(New Integer() {1, 0, 0, 0})
        '
        'btn_mbc89_Loks
        '
        Me.btn_mbc89_Loks.Location = New System.Drawing.Point(499, 207)
        Me.btn_mbc89_Loks.Name = "btn_mbc89_Loks"
        Me.btn_mbc89_Loks.Size = New System.Drawing.Size(119, 29)
        Me.btn_mbc89_Loks.TabIndex = 70
        Me.btn_mbc89_Loks.Text = "Lokeigenschaften"
        Me.btn_mbc89_Loks.UseVisualStyleBackColor = True
        '
        'gb_mbc89_Mapping
        '
        Me.gb_mbc89_Mapping.Controls.Add(Me.clb_mbc89_Mapping)
        Me.gb_mbc89_Mapping.Location = New System.Drawing.Point(440, 52)
        Me.gb_mbc89_Mapping.Name = "gb_mbc89_Mapping"
        Me.gb_mbc89_Mapping.Size = New System.Drawing.Size(234, 54)
        Me.gb_mbc89_Mapping.TabIndex = 68
        Me.gb_mbc89_Mapping.TabStop = False
        Me.gb_mbc89_Mapping.Text = "Mapping"
        '
        'clb_mbc89_Mapping
        '
        Me.clb_mbc89_Mapping.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.clb_mbc89_Mapping.FormattingEnabled = True
        Me.clb_mbc89_Mapping.Items.AddRange(New Object() {"Mapping", "Mapping & Direkt"})
        Me.clb_mbc89_Mapping.Location = New System.Drawing.Point(13, 17)
        Me.clb_mbc89_Mapping.Name = "clb_mbc89_Mapping"
        Me.clb_mbc89_Mapping.Size = New System.Drawing.Size(136, 30)
        Me.clb_mbc89_Mapping.TabIndex = 69
        '
        'gb_mbc89_Keyboard
        '
        Me.gb_mbc89_Keyboard.Controls.Add(Me.clb_mbc89_D)
        Me.gb_mbc89_Keyboard.Controls.Add(Me.clb_mbc89_C)
        Me.gb_mbc89_Keyboard.Controls.Add(Me.clb_mbc89_B)
        Me.gb_mbc89_Keyboard.Controls.Add(Me.clb_mbc89_A)
        Me.gb_mbc89_Keyboard.Controls.Add(Me.lbl_mbc89_D)
        Me.gb_mbc89_Keyboard.Controls.Add(Me.lbl_mbc89_C)
        Me.gb_mbc89_Keyboard.Controls.Add(Me.lbl_mbc89_B)
        Me.gb_mbc89_Keyboard.Controls.Add(Me.lbl_mbc89_A)
        Me.gb_mbc89_Keyboard.Controls.Add(Me.nud_mbc89_keyboard_nr)
        Me.gb_mbc89_Keyboard.Controls.Add(Me.pb_mbc89_keyboard)
        Me.gb_mbc89_Keyboard.Location = New System.Drawing.Point(7, 6)
        Me.gb_mbc89_Keyboard.Name = "gb_mbc89_Keyboard"
        Me.gb_mbc89_Keyboard.Size = New System.Drawing.Size(422, 229)
        Me.gb_mbc89_Keyboard.TabIndex = 69
        Me.gb_mbc89_Keyboard.TabStop = False
        Me.gb_mbc89_Keyboard.Text = "Keyboard"
        '
        'clb_mbc89_D
        '
        Me.clb_mbc89_D.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.clb_mbc89_D.FormattingEnabled = True
        Me.clb_mbc89_D.Items.AddRange(New Object() {"MM", "DCC"})
        Me.clb_mbc89_D.Location = New System.Drawing.Point(317, 153)
        Me.clb_mbc89_D.Name = "clb_mbc89_D"
        Me.clb_mbc89_D.Size = New System.Drawing.Size(49, 30)
        Me.clb_mbc89_D.TabIndex = 28
        '
        'clb_mbc89_C
        '
        Me.clb_mbc89_C.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.clb_mbc89_C.FormattingEnabled = True
        Me.clb_mbc89_C.Items.AddRange(New Object() {"MM", "DCC"})
        Me.clb_mbc89_C.Location = New System.Drawing.Point(21, 153)
        Me.clb_mbc89_C.Name = "clb_mbc89_C"
        Me.clb_mbc89_C.Size = New System.Drawing.Size(49, 30)
        Me.clb_mbc89_C.TabIndex = 27
        '
        'clb_mbc89_B
        '
        Me.clb_mbc89_B.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.clb_mbc89_B.FormattingEnabled = True
        Me.clb_mbc89_B.Items.AddRange(New Object() {"MM", "DCC"})
        Me.clb_mbc89_B.Location = New System.Drawing.Point(317, 88)
        Me.clb_mbc89_B.Name = "clb_mbc89_B"
        Me.clb_mbc89_B.Size = New System.Drawing.Size(49, 30)
        Me.clb_mbc89_B.TabIndex = 26
        '
        'clb_mbc89_A
        '
        Me.clb_mbc89_A.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.clb_mbc89_A.FormattingEnabled = True
        Me.clb_mbc89_A.Items.AddRange(New Object() {"MM", "DCC"})
        Me.clb_mbc89_A.Location = New System.Drawing.Point(21, 88)
        Me.clb_mbc89_A.Name = "clb_mbc89_A"
        Me.clb_mbc89_A.Size = New System.Drawing.Size(49, 30)
        Me.clb_mbc89_A.TabIndex = 25
        '
        'lbl_mbc89_D
        '
        Me.lbl_mbc89_D.AutoSize = True
        Me.lbl_mbc89_D.Location = New System.Drawing.Point(317, 136)
        Me.lbl_mbc89_D.Name = "lbl_mbc89_D"
        Me.lbl_mbc89_D.Size = New System.Drawing.Size(70, 13)
        Me.lbl_mbc89_D.TabIndex = 24
        Me.lbl_mbc89_D.Text = "D: xxxx - xxxx"
        '
        'lbl_mbc89_C
        '
        Me.lbl_mbc89_C.AutoSize = True
        Me.lbl_mbc89_C.Location = New System.Drawing.Point(21, 137)
        Me.lbl_mbc89_C.Name = "lbl_mbc89_C"
        Me.lbl_mbc89_C.Size = New System.Drawing.Size(69, 13)
        Me.lbl_mbc89_C.TabIndex = 23
        Me.lbl_mbc89_C.Text = "C: xxxx - xxxx"
        '
        'lbl_mbc89_B
        '
        Me.lbl_mbc89_B.AutoSize = True
        Me.lbl_mbc89_B.Location = New System.Drawing.Point(317, 71)
        Me.lbl_mbc89_B.Name = "lbl_mbc89_B"
        Me.lbl_mbc89_B.Size = New System.Drawing.Size(69, 13)
        Me.lbl_mbc89_B.TabIndex = 22
        Me.lbl_mbc89_B.Text = "B: xxxx - xxxx"
        '
        'lbl_mbc89_A
        '
        Me.lbl_mbc89_A.AutoSize = True
        Me.lbl_mbc89_A.Location = New System.Drawing.Point(21, 71)
        Me.lbl_mbc89_A.Name = "lbl_mbc89_A"
        Me.lbl_mbc89_A.Size = New System.Drawing.Size(69, 13)
        Me.lbl_mbc89_A.TabIndex = 21
        Me.lbl_mbc89_A.Text = "A: xxxx - xxxx"
        '
        'nud_mbc89_keyboard_nr
        '
        Me.nud_mbc89_keyboard_nr.BackColor = System.Drawing.Color.Black
        Me.nud_mbc89_keyboard_nr.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.nud_mbc89_keyboard_nr.Font = New System.Drawing.Font("Arial", 15.0!, CType((System.Drawing.FontStyle.Bold Or System.Drawing.FontStyle.Italic), System.Drawing.FontStyle), System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.nud_mbc89_keyboard_nr.ForeColor = System.Drawing.Color.White
        Me.nud_mbc89_keyboard_nr.Location = New System.Drawing.Point(132, 28)
        Me.nud_mbc89_keyboard_nr.Maximum = New Decimal(New Integer() {16, 0, 0, 0})
        Me.nud_mbc89_keyboard_nr.Minimum = New Decimal(New Integer() {1, 0, 0, 0})
        Me.nud_mbc89_keyboard_nr.Name = "nud_mbc89_keyboard_nr"
        Me.nud_mbc89_keyboard_nr.Size = New System.Drawing.Size(53, 26)
        Me.nud_mbc89_keyboard_nr.TabIndex = 20
        Me.nud_mbc89_keyboard_nr.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc89_keyboard_nr.Value = New Decimal(New Integer() {1, 0, 0, 0})
        '
        'pb_mbc89_keyboard
        '
        Me.pb_mbc89_keyboard.ErrorImage = Global.MBCAN.My.Resources.Resources.img_mbc_89_keyboard
        Me.pb_mbc89_keyboard.Image = Global.MBCAN.My.Resources.Resources.img_mbc_89_keyboard
        Me.pb_mbc89_keyboard.InitialImage = Global.MBCAN.My.Resources.Resources.img_mbc_89_keyboard
        Me.pb_mbc89_keyboard.Location = New System.Drawing.Point(12, 16)
        Me.pb_mbc89_keyboard.Name = "pb_mbc89_keyboard"
        Me.pb_mbc89_keyboard.Size = New System.Drawing.Size(396, 207)
        Me.pb_mbc89_keyboard.TabIndex = 19
        Me.pb_mbc89_keyboard.TabStop = False
        '
        'gb_mbc89_Version
        '
        Me.gb_mbc89_Version.Controls.Add(Me.nud_mbc89_SW_2)
        Me.gb_mbc89_Version.Controls.Add(Me.lbl_mbc89_Dot)
        Me.gb_mbc89_Version.Controls.Add(Me.nud_mbc89_SW_1)
        Me.gb_mbc89_Version.Controls.Add(Me.lbl_mbc89_Version)
        Me.gb_mbc89_Version.Location = New System.Drawing.Point(440, 155)
        Me.gb_mbc89_Version.Name = "gb_mbc89_Version"
        Me.gb_mbc89_Version.Size = New System.Drawing.Size(234, 46)
        Me.gb_mbc89_Version.TabIndex = 67
        Me.gb_mbc89_Version.TabStop = False
        Me.gb_mbc89_Version.Text = "Connect6021-Softwareversion"
        '
        'nud_mbc89_SW_2
        '
        Me.nud_mbc89_SW_2.Location = New System.Drawing.Point(125, 19)
        Me.nud_mbc89_SW_2.Maximum = New Decimal(New Integer() {255, 0, 0, 0})
        Me.nud_mbc89_SW_2.Name = "nud_mbc89_SW_2"
        Me.nud_mbc89_SW_2.Size = New System.Drawing.Size(52, 20)
        Me.nud_mbc89_SW_2.TabIndex = 70
        Me.nud_mbc89_SW_2.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'lbl_mbc89_Dot
        '
        Me.lbl_mbc89_Dot.AutoSize = True
        Me.lbl_mbc89_Dot.Font = New System.Drawing.Font("Microsoft Sans Serif", 14.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.lbl_mbc89_Dot.Location = New System.Drawing.Point(114, 15)
        Me.lbl_mbc89_Dot.Name = "lbl_mbc89_Dot"
        Me.lbl_mbc89_Dot.Size = New System.Drawing.Size(15, 24)
        Me.lbl_mbc89_Dot.TabIndex = 71
        Me.lbl_mbc89_Dot.Text = "."
        '
        'nud_mbc89_SW_1
        '
        Me.nud_mbc89_SW_1.Location = New System.Drawing.Point(60, 19)
        Me.nud_mbc89_SW_1.Maximum = New Decimal(New Integer() {255, 0, 0, 0})
        Me.nud_mbc89_SW_1.Name = "nud_mbc89_SW_1"
        Me.nud_mbc89_SW_1.Size = New System.Drawing.Size(52, 20)
        Me.nud_mbc89_SW_1.TabIndex = 69
        Me.nud_mbc89_SW_1.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'lbl_mbc89_Version
        '
        Me.lbl_mbc89_Version.AutoSize = True
        Me.lbl_mbc89_Version.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.lbl_mbc89_Version.Location = New System.Drawing.Point(12, 21)
        Me.lbl_mbc89_Version.Name = "lbl_mbc89_Version"
        Me.lbl_mbc89_Version.Size = New System.Drawing.Size(42, 13)
        Me.lbl_mbc89_Version.TabIndex = 63
        Me.lbl_mbc89_Version.Text = "Version"
        '
        'txt_mbc89_alt
        '
        Me.txt_mbc89_alt.BackColor = System.Drawing.SystemColors.Window
        Me.txt_mbc89_alt.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.txt_mbc89_alt.Location = New System.Drawing.Point(165, 107)
        Me.txt_mbc89_alt.Multiline = True
        Me.txt_mbc89_alt.Name = "txt_mbc89_alt"
        Me.txt_mbc89_alt.ReadOnly = True
        Me.txt_mbc89_alt.Size = New System.Drawing.Size(351, 35)
        Me.txt_mbc89_alt.TabIndex = 3
        Me.txt_mbc89_alt.Text = "Das Modul mbc-89 bietet derzeit keine Parametrierungsmöglichkeiten!"
        Me.txt_mbc89_alt.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        '
        'tab_mbc90
        '
        Me.tab_mbc90.BackColor = System.Drawing.Color.White
        Me.tab_mbc90.Controls.Add(Me.lbl_mbc90_kontaktecs2bis)
        Me.tab_mbc90.Controls.Add(Me.txt_mbc90_kontakt2cs2)
        Me.tab_mbc90.Controls.Add(Me.txt_mbc90_kontakt1cs2)
        Me.tab_mbc90.Controls.Add(Me.lbl_mbc90_kontaktecs2)
        Me.tab_mbc90.Controls.Add(Me.txt_mbc90_busnr)
        Me.tab_mbc90.Controls.Add(Me.lbl_mbc90_busnr)
        Me.tab_mbc90.Controls.Add(Me.nud_mbc90_T_P16)
        Me.tab_mbc90.Controls.Add(Me.lbl_mbc90_P16)
        Me.tab_mbc90.Controls.Add(Me.lbl_mbc90_P16a)
        Me.tab_mbc90.Controls.Add(Me.nud_mbc90_T_P12)
        Me.tab_mbc90.Controls.Add(Me.lbl_mbc90_P12)
        Me.tab_mbc90.Controls.Add(Me.lbl_mbc90_P12a)
        Me.tab_mbc90.Controls.Add(Me.nud_mbc90_T_P08)
        Me.tab_mbc90.Controls.Add(Me.lbl_mbc90_P08)
        Me.tab_mbc90.Controls.Add(Me.lbl_mbc90_P08a)
        Me.tab_mbc90.Controls.Add(Me.nud_mbc90_T_P04)
        Me.tab_mbc90.Controls.Add(Me.lbl_mbc90_P04)
        Me.tab_mbc90.Controls.Add(Me.lbl_mbc90_P04a)
        Me.tab_mbc90.Controls.Add(Me.nud_mbc90_T_P15)
        Me.tab_mbc90.Controls.Add(Me.lbl_mbc90_P15)
        Me.tab_mbc90.Controls.Add(Me.lbl_mbc90_P15a)
        Me.tab_mbc90.Controls.Add(Me.nud_mbc90_T_P11)
        Me.tab_mbc90.Controls.Add(Me.lbl_mbc90_P11)
        Me.tab_mbc90.Controls.Add(Me.lbl_mbc90_P11a)
        Me.tab_mbc90.Controls.Add(Me.nud_mbc90_T_P07)
        Me.tab_mbc90.Controls.Add(Me.lbl_mbc90_P07)
        Me.tab_mbc90.Controls.Add(Me.lbl_mbc90_P07a)
        Me.tab_mbc90.Controls.Add(Me.nud_mbc90_T_P03)
        Me.tab_mbc90.Controls.Add(Me.lbl_mbc90_P03)
        Me.tab_mbc90.Controls.Add(Me.lbl_mbc90_P03a)
        Me.tab_mbc90.Controls.Add(Me.nud_mbc90_T_P14)
        Me.tab_mbc90.Controls.Add(Me.lbl_mbc90_P14)
        Me.tab_mbc90.Controls.Add(Me.lbl_mbc90_P14a)
        Me.tab_mbc90.Controls.Add(Me.nud_mbc90_T_P10)
        Me.tab_mbc90.Controls.Add(Me.lbl_mbc90_P10)
        Me.tab_mbc90.Controls.Add(Me.lbl_mbc90_P10a)
        Me.tab_mbc90.Controls.Add(Me.nud_mbc90_T_P06)
        Me.tab_mbc90.Controls.Add(Me.lbl_mbc90_P06)
        Me.tab_mbc90.Controls.Add(Me.lbl_mbc90_P06a)
        Me.tab_mbc90.Controls.Add(Me.nud_mbc90_T_P02)
        Me.tab_mbc90.Controls.Add(Me.lbl_mbc90_P02)
        Me.tab_mbc90.Controls.Add(Me.lbl_mbc90_P02a)
        Me.tab_mbc90.Controls.Add(Me.nud_mbc90_T_P13)
        Me.tab_mbc90.Controls.Add(Me.lbl_mbc90_P13)
        Me.tab_mbc90.Controls.Add(Me.lbl_mbc90_P13a)
        Me.tab_mbc90.Controls.Add(Me.nud_mbc90_T_P09)
        Me.tab_mbc90.Controls.Add(Me.lbl_mbc90_P09)
        Me.tab_mbc90.Controls.Add(Me.lbl_mbc90_P09a)
        Me.tab_mbc90.Controls.Add(Me.nud_mbc90_T_P05)
        Me.tab_mbc90.Controls.Add(Me.lbl_mbc90_P05)
        Me.tab_mbc90.Controls.Add(Me.lbl_mbc90_P05a)
        Me.tab_mbc90.Controls.Add(Me.nud_mbc90_T_P01)
        Me.tab_mbc90.Controls.Add(Me.lbl_mbc90_P01)
        Me.tab_mbc90.Controls.Add(Me.lbl_mbc90_P01a)
        Me.tab_mbc90.Controls.Add(Me.nud_mbc90_S88ModNr)
        Me.tab_mbc90.Controls.Add(Me.lbl_mbc90_S88ModulNr)
        Me.tab_mbc90.Controls.Add(Me.gb_mbc90)
        Me.tab_mbc90.Location = New System.Drawing.Point(4, 22)
        Me.tab_mbc90.Name = "tab_mbc90"
        Me.tab_mbc90.Padding = New System.Windows.Forms.Padding(3)
        Me.tab_mbc90.Size = New System.Drawing.Size(680, 248)
        Me.tab_mbc90.TabIndex = 10
        Me.tab_mbc90.Text = "mbc-90"
        '
        'lbl_mbc90_kontaktecs2bis
        '
        Me.lbl_mbc90_kontaktecs2bis.AutoSize = True
        Me.lbl_mbc90_kontaktecs2bis.Location = New System.Drawing.Point(404, 23)
        Me.lbl_mbc90_kontaktecs2bis.Name = "lbl_mbc90_kontaktecs2bis"
        Me.lbl_mbc90_kontaktecs2bis.Size = New System.Drawing.Size(10, 13)
        Me.lbl_mbc90_kontaktecs2bis.TabIndex = 121
        Me.lbl_mbc90_kontaktecs2bis.Text = "-"
        '
        'txt_mbc90_kontakt2cs2
        '
        Me.txt_mbc90_kontakt2cs2.Location = New System.Drawing.Point(418, 20)
        Me.txt_mbc90_kontakt2cs2.Name = "txt_mbc90_kontakt2cs2"
        Me.txt_mbc90_kontakt2cs2.ReadOnly = True
        Me.txt_mbc90_kontakt2cs2.Size = New System.Drawing.Size(50, 20)
        Me.txt_mbc90_kontakt2cs2.TabIndex = 120
        '
        'txt_mbc90_kontakt1cs2
        '
        Me.txt_mbc90_kontakt1cs2.Location = New System.Drawing.Point(349, 20)
        Me.txt_mbc90_kontakt1cs2.Name = "txt_mbc90_kontakt1cs2"
        Me.txt_mbc90_kontakt1cs2.ReadOnly = True
        Me.txt_mbc90_kontakt1cs2.Size = New System.Drawing.Size(50, 20)
        Me.txt_mbc90_kontakt1cs2.TabIndex = 119
        '
        'lbl_mbc90_kontaktecs2
        '
        Me.lbl_mbc90_kontaktecs2.AutoSize = True
        Me.lbl_mbc90_kontaktecs2.Location = New System.Drawing.Point(272, 23)
        Me.lbl_mbc90_kontaktecs2.Name = "lbl_mbc90_kontaktecs2"
        Me.lbl_mbc90_kontaktecs2.Size = New System.Drawing.Size(73, 13)
        Me.lbl_mbc90_kontaktecs2.TabIndex = 118
        Me.lbl_mbc90_kontaktecs2.Text = "Kontakte CS2"
        '
        'txt_mbc90_busnr
        '
        Me.txt_mbc90_busnr.Location = New System.Drawing.Point(197, 21)
        Me.txt_mbc90_busnr.Name = "txt_mbc90_busnr"
        Me.txt_mbc90_busnr.ReadOnly = True
        Me.txt_mbc90_busnr.Size = New System.Drawing.Size(40, 20)
        Me.txt_mbc90_busnr.TabIndex = 117
        '
        'lbl_mbc90_busnr
        '
        Me.lbl_mbc90_busnr.AutoSize = True
        Me.lbl_mbc90_busnr.Location = New System.Drawing.Point(158, 23)
        Me.lbl_mbc90_busnr.Name = "lbl_mbc90_busnr"
        Me.lbl_mbc90_busnr.Size = New System.Drawing.Size(36, 13)
        Me.lbl_mbc90_busnr.TabIndex = 116
        Me.lbl_mbc90_busnr.Text = "BusNr"
        '
        'nud_mbc90_T_P16
        '
        Me.nud_mbc90_T_P16.Increment = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc90_T_P16.Location = New System.Drawing.Point(558, 202)
        Me.nud_mbc90_T_P16.Maximum = New Decimal(New Integer() {2500, 0, 0, 0})
        Me.nud_mbc90_T_P16.Minimum = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc90_T_P16.Name = "nud_mbc90_T_P16"
        Me.nud_mbc90_T_P16.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc90_T_P16.TabIndex = 111
        Me.nud_mbc90_T_P16.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc90_T_P16.Value = New Decimal(New Integer() {10, 0, 0, 0})
        '
        'lbl_mbc90_P16
        '
        Me.lbl_mbc90_P16.AutoSize = True
        Me.lbl_mbc90_P16.Location = New System.Drawing.Point(521, 204)
        Me.lbl_mbc90_P16.Name = "lbl_mbc90_P16"
        Me.lbl_mbc90_P16.Size = New System.Drawing.Size(29, 13)
        Me.lbl_mbc90_P16.TabIndex = 112
        Me.lbl_mbc90_P16.Text = "P16:"
        '
        'lbl_mbc90_P16a
        '
        Me.lbl_mbc90_P16a.AutoSize = True
        Me.lbl_mbc90_P16a.Location = New System.Drawing.Point(618, 204)
        Me.lbl_mbc90_P16a.Name = "lbl_mbc90_P16a"
        Me.lbl_mbc90_P16a.Size = New System.Drawing.Size(26, 13)
        Me.lbl_mbc90_P16a.TabIndex = 113
        Me.lbl_mbc90_P16a.Text = "[ms]"
        '
        'nud_mbc90_T_P12
        '
        Me.nud_mbc90_T_P12.Increment = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc90_T_P12.Location = New System.Drawing.Point(394, 202)
        Me.nud_mbc90_T_P12.Maximum = New Decimal(New Integer() {2500, 0, 0, 0})
        Me.nud_mbc90_T_P12.Minimum = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc90_T_P12.Name = "nud_mbc90_T_P12"
        Me.nud_mbc90_T_P12.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc90_T_P12.TabIndex = 108
        Me.nud_mbc90_T_P12.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc90_T_P12.Value = New Decimal(New Integer() {10, 0, 0, 0})
        '
        'lbl_mbc90_P12
        '
        Me.lbl_mbc90_P12.AutoSize = True
        Me.lbl_mbc90_P12.Location = New System.Drawing.Point(357, 204)
        Me.lbl_mbc90_P12.Name = "lbl_mbc90_P12"
        Me.lbl_mbc90_P12.Size = New System.Drawing.Size(29, 13)
        Me.lbl_mbc90_P12.TabIndex = 109
        Me.lbl_mbc90_P12.Text = "P12:"
        '
        'lbl_mbc90_P12a
        '
        Me.lbl_mbc90_P12a.AutoSize = True
        Me.lbl_mbc90_P12a.Location = New System.Drawing.Point(453, 204)
        Me.lbl_mbc90_P12a.Name = "lbl_mbc90_P12a"
        Me.lbl_mbc90_P12a.Size = New System.Drawing.Size(26, 13)
        Me.lbl_mbc90_P12a.TabIndex = 110
        Me.lbl_mbc90_P12a.Text = "[ms]"
        '
        'nud_mbc90_T_P08
        '
        Me.nud_mbc90_T_P08.Increment = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc90_T_P08.Location = New System.Drawing.Point(230, 202)
        Me.nud_mbc90_T_P08.Maximum = New Decimal(New Integer() {2500, 0, 0, 0})
        Me.nud_mbc90_T_P08.Minimum = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc90_T_P08.Name = "nud_mbc90_T_P08"
        Me.nud_mbc90_T_P08.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc90_T_P08.TabIndex = 104
        Me.nud_mbc90_T_P08.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc90_T_P08.Value = New Decimal(New Integer() {10, 0, 0, 0})
        '
        'lbl_mbc90_P08
        '
        Me.lbl_mbc90_P08.AutoSize = True
        Me.lbl_mbc90_P08.Location = New System.Drawing.Point(193, 204)
        Me.lbl_mbc90_P08.Name = "lbl_mbc90_P08"
        Me.lbl_mbc90_P08.Size = New System.Drawing.Size(29, 13)
        Me.lbl_mbc90_P08.TabIndex = 105
        Me.lbl_mbc90_P08.Text = "P08:"
        '
        'lbl_mbc90_P08a
        '
        Me.lbl_mbc90_P08a.AutoSize = True
        Me.lbl_mbc90_P08a.Location = New System.Drawing.Point(289, 204)
        Me.lbl_mbc90_P08a.Name = "lbl_mbc90_P08a"
        Me.lbl_mbc90_P08a.Size = New System.Drawing.Size(26, 13)
        Me.lbl_mbc90_P08a.TabIndex = 107
        Me.lbl_mbc90_P08a.Text = "[ms]"
        '
        'nud_mbc90_T_P04
        '
        Me.nud_mbc90_T_P04.Increment = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc90_T_P04.Location = New System.Drawing.Point(66, 202)
        Me.nud_mbc90_T_P04.Maximum = New Decimal(New Integer() {2500, 0, 0, 0})
        Me.nud_mbc90_T_P04.Minimum = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc90_T_P04.Name = "nud_mbc90_T_P04"
        Me.nud_mbc90_T_P04.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc90_T_P04.TabIndex = 101
        Me.nud_mbc90_T_P04.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc90_T_P04.Value = New Decimal(New Integer() {10, 0, 0, 0})
        '
        'lbl_mbc90_P04
        '
        Me.lbl_mbc90_P04.AutoSize = True
        Me.lbl_mbc90_P04.Location = New System.Drawing.Point(29, 204)
        Me.lbl_mbc90_P04.Name = "lbl_mbc90_P04"
        Me.lbl_mbc90_P04.Size = New System.Drawing.Size(29, 13)
        Me.lbl_mbc90_P04.TabIndex = 102
        Me.lbl_mbc90_P04.Text = "P04:"
        '
        'lbl_mbc90_P04a
        '
        Me.lbl_mbc90_P04a.AutoSize = True
        Me.lbl_mbc90_P04a.Location = New System.Drawing.Point(125, 204)
        Me.lbl_mbc90_P04a.Name = "lbl_mbc90_P04a"
        Me.lbl_mbc90_P04a.Size = New System.Drawing.Size(26, 13)
        Me.lbl_mbc90_P04a.TabIndex = 103
        Me.lbl_mbc90_P04a.Text = "[ms]"
        '
        'nud_mbc90_T_P15
        '
        Me.nud_mbc90_T_P15.Increment = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc90_T_P15.Location = New System.Drawing.Point(558, 158)
        Me.nud_mbc90_T_P15.Maximum = New Decimal(New Integer() {2500, 0, 0, 0})
        Me.nud_mbc90_T_P15.Minimum = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc90_T_P15.Name = "nud_mbc90_T_P15"
        Me.nud_mbc90_T_P15.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc90_T_P15.TabIndex = 98
        Me.nud_mbc90_T_P15.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc90_T_P15.Value = New Decimal(New Integer() {10, 0, 0, 0})
        '
        'lbl_mbc90_P15
        '
        Me.lbl_mbc90_P15.AutoSize = True
        Me.lbl_mbc90_P15.Location = New System.Drawing.Point(521, 160)
        Me.lbl_mbc90_P15.Name = "lbl_mbc90_P15"
        Me.lbl_mbc90_P15.Size = New System.Drawing.Size(29, 13)
        Me.lbl_mbc90_P15.TabIndex = 99
        Me.lbl_mbc90_P15.Text = "P15:"
        '
        'lbl_mbc90_P15a
        '
        Me.lbl_mbc90_P15a.AutoSize = True
        Me.lbl_mbc90_P15a.Location = New System.Drawing.Point(618, 160)
        Me.lbl_mbc90_P15a.Name = "lbl_mbc90_P15a"
        Me.lbl_mbc90_P15a.Size = New System.Drawing.Size(26, 13)
        Me.lbl_mbc90_P15a.TabIndex = 100
        Me.lbl_mbc90_P15a.Text = "[ms]"
        '
        'nud_mbc90_T_P11
        '
        Me.nud_mbc90_T_P11.Increment = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc90_T_P11.Location = New System.Drawing.Point(394, 158)
        Me.nud_mbc90_T_P11.Maximum = New Decimal(New Integer() {2500, 0, 0, 0})
        Me.nud_mbc90_T_P11.Minimum = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc90_T_P11.Name = "nud_mbc90_T_P11"
        Me.nud_mbc90_T_P11.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc90_T_P11.TabIndex = 95
        Me.nud_mbc90_T_P11.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc90_T_P11.Value = New Decimal(New Integer() {10, 0, 0, 0})
        '
        'lbl_mbc90_P11
        '
        Me.lbl_mbc90_P11.AutoSize = True
        Me.lbl_mbc90_P11.Location = New System.Drawing.Point(357, 160)
        Me.lbl_mbc90_P11.Name = "lbl_mbc90_P11"
        Me.lbl_mbc90_P11.Size = New System.Drawing.Size(29, 13)
        Me.lbl_mbc90_P11.TabIndex = 96
        Me.lbl_mbc90_P11.Text = "P11:"
        '
        'lbl_mbc90_P11a
        '
        Me.lbl_mbc90_P11a.AutoSize = True
        Me.lbl_mbc90_P11a.Location = New System.Drawing.Point(453, 160)
        Me.lbl_mbc90_P11a.Name = "lbl_mbc90_P11a"
        Me.lbl_mbc90_P11a.Size = New System.Drawing.Size(26, 13)
        Me.lbl_mbc90_P11a.TabIndex = 97
        Me.lbl_mbc90_P11a.Text = "[ms]"
        '
        'nud_mbc90_T_P07
        '
        Me.nud_mbc90_T_P07.Increment = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc90_T_P07.Location = New System.Drawing.Point(230, 158)
        Me.nud_mbc90_T_P07.Maximum = New Decimal(New Integer() {2500, 0, 0, 0})
        Me.nud_mbc90_T_P07.Minimum = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc90_T_P07.Name = "nud_mbc90_T_P07"
        Me.nud_mbc90_T_P07.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc90_T_P07.TabIndex = 92
        Me.nud_mbc90_T_P07.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc90_T_P07.Value = New Decimal(New Integer() {10, 0, 0, 0})
        '
        'lbl_mbc90_P07
        '
        Me.lbl_mbc90_P07.AutoSize = True
        Me.lbl_mbc90_P07.Location = New System.Drawing.Point(193, 160)
        Me.lbl_mbc90_P07.Name = "lbl_mbc90_P07"
        Me.lbl_mbc90_P07.Size = New System.Drawing.Size(29, 13)
        Me.lbl_mbc90_P07.TabIndex = 93
        Me.lbl_mbc90_P07.Text = "P07:"
        '
        'lbl_mbc90_P07a
        '
        Me.lbl_mbc90_P07a.AutoSize = True
        Me.lbl_mbc90_P07a.Location = New System.Drawing.Point(289, 160)
        Me.lbl_mbc90_P07a.Name = "lbl_mbc90_P07a"
        Me.lbl_mbc90_P07a.Size = New System.Drawing.Size(26, 13)
        Me.lbl_mbc90_P07a.TabIndex = 94
        Me.lbl_mbc90_P07a.Text = "[ms]"
        '
        'nud_mbc90_T_P03
        '
        Me.nud_mbc90_T_P03.Increment = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc90_T_P03.Location = New System.Drawing.Point(66, 158)
        Me.nud_mbc90_T_P03.Maximum = New Decimal(New Integer() {2500, 0, 0, 0})
        Me.nud_mbc90_T_P03.Minimum = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc90_T_P03.Name = "nud_mbc90_T_P03"
        Me.nud_mbc90_T_P03.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc90_T_P03.TabIndex = 89
        Me.nud_mbc90_T_P03.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc90_T_P03.Value = New Decimal(New Integer() {10, 0, 0, 0})
        '
        'lbl_mbc90_P03
        '
        Me.lbl_mbc90_P03.AutoSize = True
        Me.lbl_mbc90_P03.Location = New System.Drawing.Point(29, 160)
        Me.lbl_mbc90_P03.Name = "lbl_mbc90_P03"
        Me.lbl_mbc90_P03.Size = New System.Drawing.Size(29, 13)
        Me.lbl_mbc90_P03.TabIndex = 90
        Me.lbl_mbc90_P03.Text = "P03:"
        '
        'lbl_mbc90_P03a
        '
        Me.lbl_mbc90_P03a.AutoSize = True
        Me.lbl_mbc90_P03a.Location = New System.Drawing.Point(125, 160)
        Me.lbl_mbc90_P03a.Name = "lbl_mbc90_P03a"
        Me.lbl_mbc90_P03a.Size = New System.Drawing.Size(26, 13)
        Me.lbl_mbc90_P03a.TabIndex = 91
        Me.lbl_mbc90_P03a.Text = "[ms]"
        '
        'nud_mbc90_T_P14
        '
        Me.nud_mbc90_T_P14.Increment = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc90_T_P14.Location = New System.Drawing.Point(558, 114)
        Me.nud_mbc90_T_P14.Maximum = New Decimal(New Integer() {2500, 0, 0, 0})
        Me.nud_mbc90_T_P14.Minimum = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc90_T_P14.Name = "nud_mbc90_T_P14"
        Me.nud_mbc90_T_P14.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc90_T_P14.TabIndex = 86
        Me.nud_mbc90_T_P14.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc90_T_P14.Value = New Decimal(New Integer() {10, 0, 0, 0})
        '
        'lbl_mbc90_P14
        '
        Me.lbl_mbc90_P14.AutoSize = True
        Me.lbl_mbc90_P14.Location = New System.Drawing.Point(521, 116)
        Me.lbl_mbc90_P14.Name = "lbl_mbc90_P14"
        Me.lbl_mbc90_P14.Size = New System.Drawing.Size(29, 13)
        Me.lbl_mbc90_P14.TabIndex = 87
        Me.lbl_mbc90_P14.Text = "P14:"
        '
        'lbl_mbc90_P14a
        '
        Me.lbl_mbc90_P14a.AutoSize = True
        Me.lbl_mbc90_P14a.Location = New System.Drawing.Point(618, 116)
        Me.lbl_mbc90_P14a.Name = "lbl_mbc90_P14a"
        Me.lbl_mbc90_P14a.Size = New System.Drawing.Size(26, 13)
        Me.lbl_mbc90_P14a.TabIndex = 88
        Me.lbl_mbc90_P14a.Text = "[ms]"
        '
        'nud_mbc90_T_P10
        '
        Me.nud_mbc90_T_P10.Increment = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc90_T_P10.Location = New System.Drawing.Point(394, 114)
        Me.nud_mbc90_T_P10.Maximum = New Decimal(New Integer() {2500, 0, 0, 0})
        Me.nud_mbc90_T_P10.Minimum = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc90_T_P10.Name = "nud_mbc90_T_P10"
        Me.nud_mbc90_T_P10.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc90_T_P10.TabIndex = 83
        Me.nud_mbc90_T_P10.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc90_T_P10.Value = New Decimal(New Integer() {10, 0, 0, 0})
        '
        'lbl_mbc90_P10
        '
        Me.lbl_mbc90_P10.AutoSize = True
        Me.lbl_mbc90_P10.Location = New System.Drawing.Point(357, 116)
        Me.lbl_mbc90_P10.Name = "lbl_mbc90_P10"
        Me.lbl_mbc90_P10.Size = New System.Drawing.Size(29, 13)
        Me.lbl_mbc90_P10.TabIndex = 84
        Me.lbl_mbc90_P10.Text = "P10:"
        '
        'lbl_mbc90_P10a
        '
        Me.lbl_mbc90_P10a.AutoSize = True
        Me.lbl_mbc90_P10a.Location = New System.Drawing.Point(453, 116)
        Me.lbl_mbc90_P10a.Name = "lbl_mbc90_P10a"
        Me.lbl_mbc90_P10a.Size = New System.Drawing.Size(26, 13)
        Me.lbl_mbc90_P10a.TabIndex = 85
        Me.lbl_mbc90_P10a.Text = "[ms]"
        '
        'nud_mbc90_T_P06
        '
        Me.nud_mbc90_T_P06.Increment = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc90_T_P06.Location = New System.Drawing.Point(230, 114)
        Me.nud_mbc90_T_P06.Maximum = New Decimal(New Integer() {2500, 0, 0, 0})
        Me.nud_mbc90_T_P06.Minimum = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc90_T_P06.Name = "nud_mbc90_T_P06"
        Me.nud_mbc90_T_P06.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc90_T_P06.TabIndex = 80
        Me.nud_mbc90_T_P06.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc90_T_P06.Value = New Decimal(New Integer() {10, 0, 0, 0})
        '
        'lbl_mbc90_P06
        '
        Me.lbl_mbc90_P06.AutoSize = True
        Me.lbl_mbc90_P06.Location = New System.Drawing.Point(193, 116)
        Me.lbl_mbc90_P06.Name = "lbl_mbc90_P06"
        Me.lbl_mbc90_P06.Size = New System.Drawing.Size(29, 13)
        Me.lbl_mbc90_P06.TabIndex = 81
        Me.lbl_mbc90_P06.Text = "P06:"
        '
        'lbl_mbc90_P06a
        '
        Me.lbl_mbc90_P06a.AutoSize = True
        Me.lbl_mbc90_P06a.Location = New System.Drawing.Point(289, 116)
        Me.lbl_mbc90_P06a.Name = "lbl_mbc90_P06a"
        Me.lbl_mbc90_P06a.Size = New System.Drawing.Size(26, 13)
        Me.lbl_mbc90_P06a.TabIndex = 82
        Me.lbl_mbc90_P06a.Text = "[ms]"
        '
        'nud_mbc90_T_P02
        '
        Me.nud_mbc90_T_P02.Increment = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc90_T_P02.Location = New System.Drawing.Point(66, 114)
        Me.nud_mbc90_T_P02.Maximum = New Decimal(New Integer() {2500, 0, 0, 0})
        Me.nud_mbc90_T_P02.Minimum = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc90_T_P02.Name = "nud_mbc90_T_P02"
        Me.nud_mbc90_T_P02.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc90_T_P02.TabIndex = 77
        Me.nud_mbc90_T_P02.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc90_T_P02.Value = New Decimal(New Integer() {10, 0, 0, 0})
        '
        'lbl_mbc90_P02
        '
        Me.lbl_mbc90_P02.AutoSize = True
        Me.lbl_mbc90_P02.Location = New System.Drawing.Point(29, 116)
        Me.lbl_mbc90_P02.Name = "lbl_mbc90_P02"
        Me.lbl_mbc90_P02.Size = New System.Drawing.Size(29, 13)
        Me.lbl_mbc90_P02.TabIndex = 78
        Me.lbl_mbc90_P02.Text = "P02:"
        '
        'lbl_mbc90_P02a
        '
        Me.lbl_mbc90_P02a.AutoSize = True
        Me.lbl_mbc90_P02a.Location = New System.Drawing.Point(125, 116)
        Me.lbl_mbc90_P02a.Name = "lbl_mbc90_P02a"
        Me.lbl_mbc90_P02a.Size = New System.Drawing.Size(26, 13)
        Me.lbl_mbc90_P02a.TabIndex = 79
        Me.lbl_mbc90_P02a.Text = "[ms]"
        '
        'nud_mbc90_T_P13
        '
        Me.nud_mbc90_T_P13.Increment = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc90_T_P13.Location = New System.Drawing.Point(558, 70)
        Me.nud_mbc90_T_P13.Maximum = New Decimal(New Integer() {2500, 0, 0, 0})
        Me.nud_mbc90_T_P13.Minimum = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc90_T_P13.Name = "nud_mbc90_T_P13"
        Me.nud_mbc90_T_P13.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc90_T_P13.TabIndex = 74
        Me.nud_mbc90_T_P13.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc90_T_P13.Value = New Decimal(New Integer() {10, 0, 0, 0})
        '
        'lbl_mbc90_P13
        '
        Me.lbl_mbc90_P13.AutoSize = True
        Me.lbl_mbc90_P13.Location = New System.Drawing.Point(521, 72)
        Me.lbl_mbc90_P13.Name = "lbl_mbc90_P13"
        Me.lbl_mbc90_P13.Size = New System.Drawing.Size(29, 13)
        Me.lbl_mbc90_P13.TabIndex = 75
        Me.lbl_mbc90_P13.Text = "P13:"
        '
        'lbl_mbc90_P13a
        '
        Me.lbl_mbc90_P13a.AutoSize = True
        Me.lbl_mbc90_P13a.Location = New System.Drawing.Point(618, 72)
        Me.lbl_mbc90_P13a.Name = "lbl_mbc90_P13a"
        Me.lbl_mbc90_P13a.Size = New System.Drawing.Size(26, 13)
        Me.lbl_mbc90_P13a.TabIndex = 76
        Me.lbl_mbc90_P13a.Text = "[ms]"
        '
        'nud_mbc90_T_P09
        '
        Me.nud_mbc90_T_P09.Increment = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc90_T_P09.Location = New System.Drawing.Point(394, 70)
        Me.nud_mbc90_T_P09.Maximum = New Decimal(New Integer() {2500, 0, 0, 0})
        Me.nud_mbc90_T_P09.Minimum = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc90_T_P09.Name = "nud_mbc90_T_P09"
        Me.nud_mbc90_T_P09.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc90_T_P09.TabIndex = 71
        Me.nud_mbc90_T_P09.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc90_T_P09.Value = New Decimal(New Integer() {10, 0, 0, 0})
        '
        'lbl_mbc90_P09
        '
        Me.lbl_mbc90_P09.AutoSize = True
        Me.lbl_mbc90_P09.Location = New System.Drawing.Point(357, 72)
        Me.lbl_mbc90_P09.Name = "lbl_mbc90_P09"
        Me.lbl_mbc90_P09.Size = New System.Drawing.Size(29, 13)
        Me.lbl_mbc90_P09.TabIndex = 72
        Me.lbl_mbc90_P09.Text = "P09:"
        '
        'lbl_mbc90_P09a
        '
        Me.lbl_mbc90_P09a.AutoSize = True
        Me.lbl_mbc90_P09a.Location = New System.Drawing.Point(453, 72)
        Me.lbl_mbc90_P09a.Name = "lbl_mbc90_P09a"
        Me.lbl_mbc90_P09a.Size = New System.Drawing.Size(26, 13)
        Me.lbl_mbc90_P09a.TabIndex = 73
        Me.lbl_mbc90_P09a.Text = "[ms]"
        '
        'nud_mbc90_T_P05
        '
        Me.nud_mbc90_T_P05.Increment = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc90_T_P05.Location = New System.Drawing.Point(230, 70)
        Me.nud_mbc90_T_P05.Maximum = New Decimal(New Integer() {2500, 0, 0, 0})
        Me.nud_mbc90_T_P05.Minimum = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc90_T_P05.Name = "nud_mbc90_T_P05"
        Me.nud_mbc90_T_P05.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc90_T_P05.TabIndex = 68
        Me.nud_mbc90_T_P05.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc90_T_P05.Value = New Decimal(New Integer() {10, 0, 0, 0})
        '
        'lbl_mbc90_P05
        '
        Me.lbl_mbc90_P05.AutoSize = True
        Me.lbl_mbc90_P05.Location = New System.Drawing.Point(193, 72)
        Me.lbl_mbc90_P05.Name = "lbl_mbc90_P05"
        Me.lbl_mbc90_P05.Size = New System.Drawing.Size(29, 13)
        Me.lbl_mbc90_P05.TabIndex = 69
        Me.lbl_mbc90_P05.Text = "P05:"
        '
        'lbl_mbc90_P05a
        '
        Me.lbl_mbc90_P05a.AutoSize = True
        Me.lbl_mbc90_P05a.Location = New System.Drawing.Point(289, 72)
        Me.lbl_mbc90_P05a.Name = "lbl_mbc90_P05a"
        Me.lbl_mbc90_P05a.Size = New System.Drawing.Size(26, 13)
        Me.lbl_mbc90_P05a.TabIndex = 70
        Me.lbl_mbc90_P05a.Text = "[ms]"
        '
        'nud_mbc90_T_P01
        '
        Me.nud_mbc90_T_P01.Increment = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc90_T_P01.Location = New System.Drawing.Point(66, 70)
        Me.nud_mbc90_T_P01.Maximum = New Decimal(New Integer() {2500, 0, 0, 0})
        Me.nud_mbc90_T_P01.Minimum = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc90_T_P01.Name = "nud_mbc90_T_P01"
        Me.nud_mbc90_T_P01.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc90_T_P01.TabIndex = 65
        Me.nud_mbc90_T_P01.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc90_T_P01.Value = New Decimal(New Integer() {10, 0, 0, 0})
        '
        'lbl_mbc90_P01
        '
        Me.lbl_mbc90_P01.AutoSize = True
        Me.lbl_mbc90_P01.Location = New System.Drawing.Point(29, 72)
        Me.lbl_mbc90_P01.Name = "lbl_mbc90_P01"
        Me.lbl_mbc90_P01.Size = New System.Drawing.Size(29, 13)
        Me.lbl_mbc90_P01.TabIndex = 66
        Me.lbl_mbc90_P01.Text = "P01:"
        '
        'lbl_mbc90_P01a
        '
        Me.lbl_mbc90_P01a.AutoSize = True
        Me.lbl_mbc90_P01a.Location = New System.Drawing.Point(125, 72)
        Me.lbl_mbc90_P01a.Name = "lbl_mbc90_P01a"
        Me.lbl_mbc90_P01a.Size = New System.Drawing.Size(26, 13)
        Me.lbl_mbc90_P01a.TabIndex = 67
        Me.lbl_mbc90_P01a.Text = "[ms]"
        '
        'nud_mbc90_S88ModNr
        '
        Me.nud_mbc90_S88ModNr.Location = New System.Drawing.Point(66, 21)
        Me.nud_mbc90_S88ModNr.Maximum = New Decimal(New Integer() {93, 0, 0, 0})
        Me.nud_mbc90_S88ModNr.Name = "nud_mbc90_S88ModNr"
        Me.nud_mbc90_S88ModNr.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc90_S88ModNr.TabIndex = 64
        Me.nud_mbc90_S88ModNr.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc90_S88ModNr.Value = New Decimal(New Integer() {62, 0, 0, 0})
        '
        'lbl_mbc90_S88ModulNr
        '
        Me.lbl_mbc90_S88ModulNr.AutoSize = True
        Me.lbl_mbc90_S88ModulNr.Location = New System.Drawing.Point(14, 23)
        Me.lbl_mbc90_S88ModulNr.Name = "lbl_mbc90_S88ModulNr"
        Me.lbl_mbc90_S88ModulNr.Size = New System.Drawing.Size(47, 13)
        Me.lbl_mbc90_S88ModulNr.TabIndex = 62
        Me.lbl_mbc90_S88ModulNr.Text = "ModulNr"
        '
        'gb_mbc90
        '
        Me.gb_mbc90.Location = New System.Drawing.Point(17, 47)
        Me.gb_mbc90.Name = "gb_mbc90"
        Me.gb_mbc90.Size = New System.Drawing.Size(648, 186)
        Me.gb_mbc90.TabIndex = 115
        Me.gb_mbc90.TabStop = False
        Me.gb_mbc90.Text = "Integrator"
        '
        'tab_mbc91
        '
        Me.tab_mbc91.BackColor = System.Drawing.Color.White
        Me.tab_mbc91.Controls.Add(Me.lbl_mbc91_AusEinheit)
        Me.tab_mbc91.Controls.Add(Me.lbl_mbc91_Rueck)
        Me.tab_mbc91.Controls.Add(Me.lbl_mbc91_Aus)
        Me.tab_mbc91.Controls.Add(Me.nud_mbc91_Rueck)
        Me.tab_mbc91.Controls.Add(Me.nud_mbc91_Aus)
        Me.tab_mbc91.Controls.Add(Me.lbl_mbc91_Wechsel)
        Me.tab_mbc91.Controls.Add(Me.lbl_mbc91_Impuls)
        Me.tab_mbc91.Controls.Add(Me.gb_mbc91_CH4)
        Me.tab_mbc91.Controls.Add(Me.gb_mbc91_CH3)
        Me.tab_mbc91.Controls.Add(Me.gb_mbc91_CH2)
        Me.tab_mbc91.Controls.Add(Me.gb_mbc91_CH1)
        Me.tab_mbc91.Controls.Add(Me.nud_mbc91_MMAddr)
        Me.tab_mbc91.Controls.Add(Me.lbl_mbc91_MMAddr)
        Me.tab_mbc91.Controls.Add(Me.pic_mbc91_Wechsel)
        Me.tab_mbc91.Controls.Add(Me.pic_mbc91_Impuls)
        Me.tab_mbc91.Location = New System.Drawing.Point(4, 22)
        Me.tab_mbc91.Name = "tab_mbc91"
        Me.tab_mbc91.Padding = New System.Windows.Forms.Padding(3)
        Me.tab_mbc91.Size = New System.Drawing.Size(680, 248)
        Me.tab_mbc91.TabIndex = 11
        Me.tab_mbc91.Text = "mbc-91"
        '
        'lbl_mbc91_AusEinheit
        '
        Me.lbl_mbc91_AusEinheit.AutoSize = True
        Me.lbl_mbc91_AusEinheit.Location = New System.Drawing.Point(470, 23)
        Me.lbl_mbc91_AusEinheit.Name = "lbl_mbc91_AusEinheit"
        Me.lbl_mbc91_AusEinheit.Size = New System.Drawing.Size(26, 13)
        Me.lbl_mbc91_AusEinheit.TabIndex = 28
        Me.lbl_mbc91_AusEinheit.Text = "[ms]"
        '
        'lbl_mbc91_Rueck
        '
        Me.lbl_mbc91_Rueck.AutoSize = True
        Me.lbl_mbc91_Rueck.Location = New System.Drawing.Point(521, 23)
        Me.lbl_mbc91_Rueck.Name = "lbl_mbc91_Rueck"
        Me.lbl_mbc91_Rueck.Size = New System.Drawing.Size(65, 13)
        Me.lbl_mbc91_Rueck.TabIndex = 32
        Me.lbl_mbc91_Rueck.Text = "Meldezyklus"
        '
        'lbl_mbc91_Aus
        '
        Me.lbl_mbc91_Aus.AutoSize = True
        Me.lbl_mbc91_Aus.Location = New System.Drawing.Point(353, 23)
        Me.lbl_mbc91_Aus.Name = "lbl_mbc91_Aus"
        Me.lbl_mbc91_Aus.Size = New System.Drawing.Size(54, 13)
        Me.lbl_mbc91_Aus.TabIndex = 31
        Me.lbl_mbc91_Aus.Text = "Auto-AUS"
        '
        'nud_mbc91_Rueck
        '
        Me.nud_mbc91_Rueck.Location = New System.Drawing.Point(593, 21)
        Me.nud_mbc91_Rueck.Maximum = New Decimal(New Integer() {1, 0, 0, 0})
        Me.nud_mbc91_Rueck.Name = "nud_mbc91_Rueck"
        Me.nud_mbc91_Rueck.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc91_Rueck.TabIndex = 30
        Me.nud_mbc91_Rueck.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        '
        'nud_mbc91_Aus
        '
        Me.nud_mbc91_Aus.Increment = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc91_Aus.Location = New System.Drawing.Point(411, 21)
        Me.nud_mbc91_Aus.Maximum = New Decimal(New Integer() {2000, 0, 0, 0})
        Me.nud_mbc91_Aus.Name = "nud_mbc91_Aus"
        Me.nud_mbc91_Aus.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc91_Aus.TabIndex = 29
        Me.nud_mbc91_Aus.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        '
        'lbl_mbc91_Wechsel
        '
        Me.lbl_mbc91_Wechsel.AutoSize = True
        Me.lbl_mbc91_Wechsel.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.lbl_mbc91_Wechsel.Location = New System.Drawing.Point(214, 218)
        Me.lbl_mbc91_Wechsel.Name = "lbl_mbc91_Wechsel"
        Me.lbl_mbc91_Wechsel.Size = New System.Drawing.Size(86, 13)
        Me.lbl_mbc91_Wechsel.TabIndex = 27
        Me.lbl_mbc91_Wechsel.Text = "Wechselschalter"
        '
        'lbl_mbc91_Impuls
        '
        Me.lbl_mbc91_Impuls.AutoSize = True
        Me.lbl_mbc91_Impuls.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.lbl_mbc91_Impuls.Location = New System.Drawing.Point(46, 218)
        Me.lbl_mbc91_Impuls.Name = "lbl_mbc91_Impuls"
        Me.lbl_mbc91_Impuls.Size = New System.Drawing.Size(78, 13)
        Me.lbl_mbc91_Impuls.TabIndex = 26
        Me.lbl_mbc91_Impuls.Text = "Impulsausgang"
        '
        'gb_mbc91_CH4
        '
        Me.gb_mbc91_CH4.Controls.Add(Me.txt_mbc91_CH4_Addr)
        Me.gb_mbc91_CH4.Controls.Add(Me.lbl_mbc91_CH4_Addr)
        Me.gb_mbc91_CH4.Controls.Add(Me.nud_mbc91_CH4_TIO)
        Me.gb_mbc91_CH4.Controls.Add(Me.lbl_mbc91_CH4_Typ)
        Me.gb_mbc91_CH4.Controls.Add(Me.lbl_mbc91_CH4_TIO)
        Me.gb_mbc91_CH4.Controls.Add(Me.clb_mbc91_CH4)
        Me.gb_mbc91_CH4.Controls.Add(Me.lbl_mbc91_CH4_TIOEinheit)
        Me.gb_mbc91_CH4.Location = New System.Drawing.Point(510, 62)
        Me.gb_mbc91_CH4.Name = "gb_mbc91_CH4"
        Me.gb_mbc91_CH4.Size = New System.Drawing.Size(155, 142)
        Me.gb_mbc91_CH4.TabIndex = 22
        Me.gb_mbc91_CH4.TabStop = False
        Me.gb_mbc91_CH4.Text = "Ausgang 4"
        '
        'txt_mbc91_CH4_Addr
        '
        Me.txt_mbc91_CH4_Addr.Location = New System.Drawing.Point(67, 23)
        Me.txt_mbc91_CH4_Addr.Name = "txt_mbc91_CH4_Addr"
        Me.txt_mbc91_CH4_Addr.ReadOnly = True
        Me.txt_mbc91_CH4_Addr.Size = New System.Drawing.Size(50, 20)
        Me.txt_mbc91_CH4_Addr.TabIndex = 16
        '
        'lbl_mbc91_CH4_Addr
        '
        Me.lbl_mbc91_CH4_Addr.AutoSize = True
        Me.lbl_mbc91_CH4_Addr.Location = New System.Drawing.Point(9, 26)
        Me.lbl_mbc91_CH4_Addr.Name = "lbl_mbc91_CH4_Addr"
        Me.lbl_mbc91_CH4_Addr.Size = New System.Drawing.Size(48, 13)
        Me.lbl_mbc91_CH4_Addr.TabIndex = 15
        Me.lbl_mbc91_CH4_Addr.Text = "Adresse:"
        '
        'nud_mbc91_CH4_TIO
        '
        Me.nud_mbc91_CH4_TIO.Increment = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc91_CH4_TIO.Location = New System.Drawing.Point(67, 108)
        Me.nud_mbc91_CH4_TIO.Maximum = New Decimal(New Integer() {2500, 0, 0, 0})
        Me.nud_mbc91_CH4_TIO.Minimum = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc91_CH4_TIO.Name = "nud_mbc91_CH4_TIO"
        Me.nud_mbc91_CH4_TIO.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc91_CH4_TIO.TabIndex = 8
        Me.nud_mbc91_CH4_TIO.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc91_CH4_TIO.Value = New Decimal(New Integer() {200, 0, 0, 0})
        '
        'lbl_mbc91_CH4_Typ
        '
        Me.lbl_mbc91_CH4_Typ.AutoSize = True
        Me.lbl_mbc91_CH4_Typ.Location = New System.Drawing.Point(9, 60)
        Me.lbl_mbc91_CH4_Typ.Name = "lbl_mbc91_CH4_Typ"
        Me.lbl_mbc91_CH4_Typ.Size = New System.Drawing.Size(28, 13)
        Me.lbl_mbc91_CH4_Typ.TabIndex = 11
        Me.lbl_mbc91_CH4_Typ.Text = "Typ:"
        '
        'lbl_mbc91_CH4_TIO
        '
        Me.lbl_mbc91_CH4_TIO.AutoSize = True
        Me.lbl_mbc91_CH4_TIO.Location = New System.Drawing.Point(9, 110)
        Me.lbl_mbc91_CH4_TIO.Name = "lbl_mbc91_CH4_TIO"
        Me.lbl_mbc91_CH4_TIO.Size = New System.Drawing.Size(52, 13)
        Me.lbl_mbc91_CH4_TIO.TabIndex = 9
        Me.lbl_mbc91_CH4_TIO.Text = "ON-Time:"
        '
        'clb_mbc91_CH4
        '
        Me.clb_mbc91_CH4.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.clb_mbc91_CH4.ColumnWidth = 60
        Me.clb_mbc91_CH4.Enabled = False
        Me.clb_mbc91_CH4.FormattingEnabled = True
        Me.clb_mbc91_CH4.Items.AddRange(New Object() {"Impuls", "Wechsel"})
        Me.clb_mbc91_CH4.Location = New System.Drawing.Point(67, 60)
        Me.clb_mbc91_CH4.Name = "clb_mbc91_CH4"
        Me.clb_mbc91_CH4.Size = New System.Drawing.Size(77, 30)
        Me.clb_mbc91_CH4.TabIndex = 7
        Me.clb_mbc91_CH4.TabStop = False
        '
        'lbl_mbc91_CH4_TIOEinheit
        '
        Me.lbl_mbc91_CH4_TIOEinheit.AutoSize = True
        Me.lbl_mbc91_CH4_TIOEinheit.Location = New System.Drawing.Point(126, 110)
        Me.lbl_mbc91_CH4_TIOEinheit.Name = "lbl_mbc91_CH4_TIOEinheit"
        Me.lbl_mbc91_CH4_TIOEinheit.Size = New System.Drawing.Size(26, 13)
        Me.lbl_mbc91_CH4_TIOEinheit.TabIndex = 10
        Me.lbl_mbc91_CH4_TIOEinheit.Text = "[ms]"
        '
        'gb_mbc91_CH3
        '
        Me.gb_mbc91_CH3.Controls.Add(Me.txt_mbc91_CH3_Addr)
        Me.gb_mbc91_CH3.Controls.Add(Me.lbl_mbc91_CH3_Addr)
        Me.gb_mbc91_CH3.Controls.Add(Me.nud_mbc91_CH3_TIO)
        Me.gb_mbc91_CH3.Controls.Add(Me.lbl_mbc91_CH3_Typ)
        Me.gb_mbc91_CH3.Controls.Add(Me.lbl_mbc91_CH3_TIO)
        Me.gb_mbc91_CH3.Controls.Add(Me.clb_mbc91_CH3)
        Me.gb_mbc91_CH3.Controls.Add(Me.lbl_mbc91_CH3_TIOEinheit)
        Me.gb_mbc91_CH3.Location = New System.Drawing.Point(344, 62)
        Me.gb_mbc91_CH3.Name = "gb_mbc91_CH3"
        Me.gb_mbc91_CH3.Size = New System.Drawing.Size(155, 142)
        Me.gb_mbc91_CH3.TabIndex = 23
        Me.gb_mbc91_CH3.TabStop = False
        Me.gb_mbc91_CH3.Text = "Ausgang 3"
        '
        'txt_mbc91_CH3_Addr
        '
        Me.txt_mbc91_CH3_Addr.Location = New System.Drawing.Point(67, 23)
        Me.txt_mbc91_CH3_Addr.Name = "txt_mbc91_CH3_Addr"
        Me.txt_mbc91_CH3_Addr.ReadOnly = True
        Me.txt_mbc91_CH3_Addr.Size = New System.Drawing.Size(50, 20)
        Me.txt_mbc91_CH3_Addr.TabIndex = 15
        '
        'lbl_mbc91_CH3_Addr
        '
        Me.lbl_mbc91_CH3_Addr.AutoSize = True
        Me.lbl_mbc91_CH3_Addr.Location = New System.Drawing.Point(9, 26)
        Me.lbl_mbc91_CH3_Addr.Name = "lbl_mbc91_CH3_Addr"
        Me.lbl_mbc91_CH3_Addr.Size = New System.Drawing.Size(48, 13)
        Me.lbl_mbc91_CH3_Addr.TabIndex = 14
        Me.lbl_mbc91_CH3_Addr.Text = "Adresse:"
        '
        'nud_mbc91_CH3_TIO
        '
        Me.nud_mbc91_CH3_TIO.Increment = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc91_CH3_TIO.Location = New System.Drawing.Point(67, 108)
        Me.nud_mbc91_CH3_TIO.Maximum = New Decimal(New Integer() {2500, 0, 0, 0})
        Me.nud_mbc91_CH3_TIO.Minimum = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc91_CH3_TIO.Name = "nud_mbc91_CH3_TIO"
        Me.nud_mbc91_CH3_TIO.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc91_CH3_TIO.TabIndex = 8
        Me.nud_mbc91_CH3_TIO.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc91_CH3_TIO.Value = New Decimal(New Integer() {200, 0, 0, 0})
        '
        'lbl_mbc91_CH3_Typ
        '
        Me.lbl_mbc91_CH3_Typ.AutoSize = True
        Me.lbl_mbc91_CH3_Typ.Location = New System.Drawing.Point(9, 60)
        Me.lbl_mbc91_CH3_Typ.Name = "lbl_mbc91_CH3_Typ"
        Me.lbl_mbc91_CH3_Typ.Size = New System.Drawing.Size(28, 13)
        Me.lbl_mbc91_CH3_Typ.TabIndex = 11
        Me.lbl_mbc91_CH3_Typ.Text = "Typ:"
        '
        'lbl_mbc91_CH3_TIO
        '
        Me.lbl_mbc91_CH3_TIO.AutoSize = True
        Me.lbl_mbc91_CH3_TIO.Location = New System.Drawing.Point(9, 110)
        Me.lbl_mbc91_CH3_TIO.Name = "lbl_mbc91_CH3_TIO"
        Me.lbl_mbc91_CH3_TIO.Size = New System.Drawing.Size(52, 13)
        Me.lbl_mbc91_CH3_TIO.TabIndex = 9
        Me.lbl_mbc91_CH3_TIO.Text = "ON-Time:"
        '
        'clb_mbc91_CH3
        '
        Me.clb_mbc91_CH3.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.clb_mbc91_CH3.ColumnWidth = 60
        Me.clb_mbc91_CH3.Enabled = False
        Me.clb_mbc91_CH3.FormattingEnabled = True
        Me.clb_mbc91_CH3.Items.AddRange(New Object() {"Impuls", "Wechsel"})
        Me.clb_mbc91_CH3.Location = New System.Drawing.Point(67, 60)
        Me.clb_mbc91_CH3.Name = "clb_mbc91_CH3"
        Me.clb_mbc91_CH3.Size = New System.Drawing.Size(77, 30)
        Me.clb_mbc91_CH3.TabIndex = 7
        Me.clb_mbc91_CH3.TabStop = False
        '
        'lbl_mbc91_CH3_TIOEinheit
        '
        Me.lbl_mbc91_CH3_TIOEinheit.AutoSize = True
        Me.lbl_mbc91_CH3_TIOEinheit.Location = New System.Drawing.Point(126, 110)
        Me.lbl_mbc91_CH3_TIOEinheit.Name = "lbl_mbc91_CH3_TIOEinheit"
        Me.lbl_mbc91_CH3_TIOEinheit.Size = New System.Drawing.Size(26, 13)
        Me.lbl_mbc91_CH3_TIOEinheit.TabIndex = 10
        Me.lbl_mbc91_CH3_TIOEinheit.Text = "[ms]"
        '
        'gb_mbc91_CH2
        '
        Me.gb_mbc91_CH2.Controls.Add(Me.txt_mbc91_CH2_Addr)
        Me.gb_mbc91_CH2.Controls.Add(Me.lbl_mbc91_CH2_Addr)
        Me.gb_mbc91_CH2.Controls.Add(Me.nud_mbc91_CH2_TIO)
        Me.gb_mbc91_CH2.Controls.Add(Me.lbl_mbc91_CH2_Typ)
        Me.gb_mbc91_CH2.Controls.Add(Me.lbl_mbc91_CH2_TIO)
        Me.gb_mbc91_CH2.Controls.Add(Me.clb_mbc91_CH2)
        Me.gb_mbc91_CH2.Controls.Add(Me.lbl_mbc91_CH2_TIOEinheit)
        Me.gb_mbc91_CH2.Location = New System.Drawing.Point(178, 62)
        Me.gb_mbc91_CH2.Name = "gb_mbc91_CH2"
        Me.gb_mbc91_CH2.Size = New System.Drawing.Size(155, 142)
        Me.gb_mbc91_CH2.TabIndex = 24
        Me.gb_mbc91_CH2.TabStop = False
        Me.gb_mbc91_CH2.Text = "Ausgang 2"
        '
        'txt_mbc91_CH2_Addr
        '
        Me.txt_mbc91_CH2_Addr.Location = New System.Drawing.Point(67, 23)
        Me.txt_mbc91_CH2_Addr.Name = "txt_mbc91_CH2_Addr"
        Me.txt_mbc91_CH2_Addr.ReadOnly = True
        Me.txt_mbc91_CH2_Addr.Size = New System.Drawing.Size(50, 20)
        Me.txt_mbc91_CH2_Addr.TabIndex = 14
        '
        'lbl_mbc91_CH2_Addr
        '
        Me.lbl_mbc91_CH2_Addr.AutoSize = True
        Me.lbl_mbc91_CH2_Addr.Location = New System.Drawing.Point(9, 26)
        Me.lbl_mbc91_CH2_Addr.Name = "lbl_mbc91_CH2_Addr"
        Me.lbl_mbc91_CH2_Addr.Size = New System.Drawing.Size(48, 13)
        Me.lbl_mbc91_CH2_Addr.TabIndex = 13
        Me.lbl_mbc91_CH2_Addr.Text = "Adresse:"
        '
        'nud_mbc91_CH2_TIO
        '
        Me.nud_mbc91_CH2_TIO.Increment = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc91_CH2_TIO.Location = New System.Drawing.Point(67, 108)
        Me.nud_mbc91_CH2_TIO.Maximum = New Decimal(New Integer() {2500, 0, 0, 0})
        Me.nud_mbc91_CH2_TIO.Minimum = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc91_CH2_TIO.Name = "nud_mbc91_CH2_TIO"
        Me.nud_mbc91_CH2_TIO.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc91_CH2_TIO.TabIndex = 8
        Me.nud_mbc91_CH2_TIO.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc91_CH2_TIO.Value = New Decimal(New Integer() {200, 0, 0, 0})
        '
        'lbl_mbc91_CH2_Typ
        '
        Me.lbl_mbc91_CH2_Typ.AutoSize = True
        Me.lbl_mbc91_CH2_Typ.Location = New System.Drawing.Point(9, 60)
        Me.lbl_mbc91_CH2_Typ.Name = "lbl_mbc91_CH2_Typ"
        Me.lbl_mbc91_CH2_Typ.Size = New System.Drawing.Size(28, 13)
        Me.lbl_mbc91_CH2_Typ.TabIndex = 11
        Me.lbl_mbc91_CH2_Typ.Text = "Typ:"
        '
        'lbl_mbc91_CH2_TIO
        '
        Me.lbl_mbc91_CH2_TIO.AutoSize = True
        Me.lbl_mbc91_CH2_TIO.Location = New System.Drawing.Point(9, 110)
        Me.lbl_mbc91_CH2_TIO.Name = "lbl_mbc91_CH2_TIO"
        Me.lbl_mbc91_CH2_TIO.Size = New System.Drawing.Size(52, 13)
        Me.lbl_mbc91_CH2_TIO.TabIndex = 9
        Me.lbl_mbc91_CH2_TIO.Text = "ON-Time:"
        '
        'clb_mbc91_CH2
        '
        Me.clb_mbc91_CH2.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.clb_mbc91_CH2.ColumnWidth = 60
        Me.clb_mbc91_CH2.Enabled = False
        Me.clb_mbc91_CH2.FormattingEnabled = True
        Me.clb_mbc91_CH2.Items.AddRange(New Object() {"Impuls", "Wechsel"})
        Me.clb_mbc91_CH2.Location = New System.Drawing.Point(67, 60)
        Me.clb_mbc91_CH2.Name = "clb_mbc91_CH2"
        Me.clb_mbc91_CH2.Size = New System.Drawing.Size(77, 30)
        Me.clb_mbc91_CH2.TabIndex = 7
        Me.clb_mbc91_CH2.TabStop = False
        '
        'lbl_mbc91_CH2_TIOEinheit
        '
        Me.lbl_mbc91_CH2_TIOEinheit.AutoSize = True
        Me.lbl_mbc91_CH2_TIOEinheit.Location = New System.Drawing.Point(126, 110)
        Me.lbl_mbc91_CH2_TIOEinheit.Name = "lbl_mbc91_CH2_TIOEinheit"
        Me.lbl_mbc91_CH2_TIOEinheit.Size = New System.Drawing.Size(26, 13)
        Me.lbl_mbc91_CH2_TIOEinheit.TabIndex = 10
        Me.lbl_mbc91_CH2_TIOEinheit.Text = "[ms]"
        '
        'gb_mbc91_CH1
        '
        Me.gb_mbc91_CH1.Controls.Add(Me.txt_mbc91_CH1_Addr)
        Me.gb_mbc91_CH1.Controls.Add(Me.lbl_mbc91_CH1_Addr)
        Me.gb_mbc91_CH1.Controls.Add(Me.nud_mbc91_CH1_TIO)
        Me.gb_mbc91_CH1.Controls.Add(Me.lbl_mbc91_CH1_Typ)
        Me.gb_mbc91_CH1.Controls.Add(Me.lbl_mbc91_CH1_TIO)
        Me.gb_mbc91_CH1.Controls.Add(Me.clb_mbc91_CH1)
        Me.gb_mbc91_CH1.Controls.Add(Me.lbl_mbc91_CH1_TIOEinheit)
        Me.gb_mbc91_CH1.Location = New System.Drawing.Point(12, 62)
        Me.gb_mbc91_CH1.Name = "gb_mbc91_CH1"
        Me.gb_mbc91_CH1.Size = New System.Drawing.Size(155, 142)
        Me.gb_mbc91_CH1.TabIndex = 21
        Me.gb_mbc91_CH1.TabStop = False
        Me.gb_mbc91_CH1.Text = "Ausgang 1"
        '
        'txt_mbc91_CH1_Addr
        '
        Me.txt_mbc91_CH1_Addr.Location = New System.Drawing.Point(67, 23)
        Me.txt_mbc91_CH1_Addr.Name = "txt_mbc91_CH1_Addr"
        Me.txt_mbc91_CH1_Addr.ReadOnly = True
        Me.txt_mbc91_CH1_Addr.Size = New System.Drawing.Size(50, 20)
        Me.txt_mbc91_CH1_Addr.TabIndex = 13
        '
        'lbl_mbc91_CH1_Addr
        '
        Me.lbl_mbc91_CH1_Addr.AutoSize = True
        Me.lbl_mbc91_CH1_Addr.Location = New System.Drawing.Point(9, 26)
        Me.lbl_mbc91_CH1_Addr.Name = "lbl_mbc91_CH1_Addr"
        Me.lbl_mbc91_CH1_Addr.Size = New System.Drawing.Size(48, 13)
        Me.lbl_mbc91_CH1_Addr.TabIndex = 12
        Me.lbl_mbc91_CH1_Addr.Text = "Adresse:"
        '
        'nud_mbc91_CH1_TIO
        '
        Me.nud_mbc91_CH1_TIO.Increment = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc91_CH1_TIO.Location = New System.Drawing.Point(67, 108)
        Me.nud_mbc91_CH1_TIO.Maximum = New Decimal(New Integer() {2500, 0, 0, 0})
        Me.nud_mbc91_CH1_TIO.Minimum = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc91_CH1_TIO.Name = "nud_mbc91_CH1_TIO"
        Me.nud_mbc91_CH1_TIO.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc91_CH1_TIO.TabIndex = 8
        Me.nud_mbc91_CH1_TIO.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc91_CH1_TIO.Value = New Decimal(New Integer() {200, 0, 0, 0})
        '
        'lbl_mbc91_CH1_Typ
        '
        Me.lbl_mbc91_CH1_Typ.AutoSize = True
        Me.lbl_mbc91_CH1_Typ.Location = New System.Drawing.Point(9, 60)
        Me.lbl_mbc91_CH1_Typ.Name = "lbl_mbc91_CH1_Typ"
        Me.lbl_mbc91_CH1_Typ.Size = New System.Drawing.Size(28, 13)
        Me.lbl_mbc91_CH1_Typ.TabIndex = 11
        Me.lbl_mbc91_CH1_Typ.Text = "Typ:"
        '
        'lbl_mbc91_CH1_TIO
        '
        Me.lbl_mbc91_CH1_TIO.AutoSize = True
        Me.lbl_mbc91_CH1_TIO.Location = New System.Drawing.Point(9, 110)
        Me.lbl_mbc91_CH1_TIO.Name = "lbl_mbc91_CH1_TIO"
        Me.lbl_mbc91_CH1_TIO.Size = New System.Drawing.Size(52, 13)
        Me.lbl_mbc91_CH1_TIO.TabIndex = 9
        Me.lbl_mbc91_CH1_TIO.Text = "ON-Time:"
        '
        'clb_mbc91_CH1
        '
        Me.clb_mbc91_CH1.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.clb_mbc91_CH1.ColumnWidth = 60
        Me.clb_mbc91_CH1.Enabled = False
        Me.clb_mbc91_CH1.FormattingEnabled = True
        Me.clb_mbc91_CH1.Items.AddRange(New Object() {"Impuls", "Wechsel"})
        Me.clb_mbc91_CH1.Location = New System.Drawing.Point(67, 60)
        Me.clb_mbc91_CH1.Name = "clb_mbc91_CH1"
        Me.clb_mbc91_CH1.Size = New System.Drawing.Size(77, 45)
        Me.clb_mbc91_CH1.TabIndex = 7
        Me.clb_mbc91_CH1.TabStop = False
        '
        'lbl_mbc91_CH1_TIOEinheit
        '
        Me.lbl_mbc91_CH1_TIOEinheit.AutoSize = True
        Me.lbl_mbc91_CH1_TIOEinheit.Location = New System.Drawing.Point(126, 110)
        Me.lbl_mbc91_CH1_TIOEinheit.Name = "lbl_mbc91_CH1_TIOEinheit"
        Me.lbl_mbc91_CH1_TIOEinheit.Size = New System.Drawing.Size(26, 13)
        Me.lbl_mbc91_CH1_TIOEinheit.TabIndex = 10
        Me.lbl_mbc91_CH1_TIOEinheit.Text = "[ms]"
        '
        'nud_mbc91_MMAddr
        '
        Me.nud_mbc91_MMAddr.Hexadecimal = True
        Me.nud_mbc91_MMAddr.Increment = New Decimal(New Integer() {4, 0, 0, 0})
        Me.nud_mbc91_MMAddr.Location = New System.Drawing.Point(108, 21)
        Me.nud_mbc91_MMAddr.Maximum = New Decimal(New Integer() {13308, 0, 0, 0})
        Me.nud_mbc91_MMAddr.Minimum = New Decimal(New Integer() {12288, 0, 0, 0})
        Me.nud_mbc91_MMAddr.Name = "nud_mbc91_MMAddr"
        Me.nud_mbc91_MMAddr.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc91_MMAddr.TabIndex = 19
        Me.nud_mbc91_MMAddr.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc91_MMAddr.Value = New Decimal(New Integer() {12288, 0, 0, 0})
        '
        'lbl_mbc91_MMAddr
        '
        Me.lbl_mbc91_MMAddr.AutoSize = True
        Me.lbl_mbc91_MMAddr.Location = New System.Drawing.Point(14, 23)
        Me.lbl_mbc91_MMAddr.Name = "lbl_mbc91_MMAddr"
        Me.lbl_mbc91_MMAddr.Size = New System.Drawing.Size(75, 13)
        Me.lbl_mbc91_MMAddr.TabIndex = 17
        Me.lbl_mbc91_MMAddr.Text = "Loc-ID in HEX"
        '
        'pic_mbc91_Wechsel
        '
        Me.pic_mbc91_Wechsel.InitialImage = Global.MBCAN.My.Resources.Resources.img_wechsel
        Me.pic_mbc91_Wechsel.Location = New System.Drawing.Point(179, 210)
        Me.pic_mbc91_Wechsel.Name = "pic_mbc91_Wechsel"
        Me.pic_mbc91_Wechsel.Size = New System.Drawing.Size(26, 26)
        Me.pic_mbc91_Wechsel.TabIndex = 25
        Me.pic_mbc91_Wechsel.TabStop = False
        '
        'pic_mbc91_Impuls
        '
        Me.pic_mbc91_Impuls.InitialImage = Global.MBCAN.My.Resources.Resources.img_impuls
        Me.pic_mbc91_Impuls.Location = New System.Drawing.Point(12, 210)
        Me.pic_mbc91_Impuls.Name = "pic_mbc91_Impuls"
        Me.pic_mbc91_Impuls.Size = New System.Drawing.Size(26, 26)
        Me.pic_mbc91_Impuls.TabIndex = 20
        Me.pic_mbc91_Impuls.TabStop = False
        '
        'tab_mbc92
        '
        Me.tab_mbc92.BackColor = System.Drawing.Color.White
        Me.tab_mbc92.Controls.Add(Me.lbl_mbc92_AusEinheit)
        Me.tab_mbc92.Controls.Add(Me.lbl_mbc92_Rueck)
        Me.tab_mbc92.Controls.Add(Me.lbl_mbc92_Aus)
        Me.tab_mbc92.Controls.Add(Me.nud_mbc92_Rueck)
        Me.tab_mbc92.Controls.Add(Me.nud_mbc92_Aus)
        Me.tab_mbc92.Controls.Add(Me.lbl_mbc92_Wechsel)
        Me.tab_mbc92.Controls.Add(Me.lbl_mbc92_Impuls)
        Me.tab_mbc92.Controls.Add(Me.gb_mbc92_CH4)
        Me.tab_mbc92.Controls.Add(Me.gb_mbc92_CH3)
        Me.tab_mbc92.Controls.Add(Me.gb_mbc92_CH2)
        Me.tab_mbc92.Controls.Add(Me.gb_mbc92_CH1)
        Me.tab_mbc92.Controls.Add(Me.nud_mbc92_MMAddr)
        Me.tab_mbc92.Controls.Add(Me.lbl_mbc92_MMAddr)
        Me.tab_mbc92.Controls.Add(Me.pic_mbc92_Wechsel)
        Me.tab_mbc92.Controls.Add(Me.pic_mbc92_Impuls)
        Me.tab_mbc92.Location = New System.Drawing.Point(4, 22)
        Me.tab_mbc92.Name = "tab_mbc92"
        Me.tab_mbc92.Padding = New System.Windows.Forms.Padding(3)
        Me.tab_mbc92.Size = New System.Drawing.Size(680, 248)
        Me.tab_mbc92.TabIndex = 12
        Me.tab_mbc92.Text = "mbc-92"
        '
        'lbl_mbc92_AusEinheit
        '
        Me.lbl_mbc92_AusEinheit.AutoSize = True
        Me.lbl_mbc92_AusEinheit.Location = New System.Drawing.Point(470, 23)
        Me.lbl_mbc92_AusEinheit.Name = "lbl_mbc92_AusEinheit"
        Me.lbl_mbc92_AusEinheit.Size = New System.Drawing.Size(26, 13)
        Me.lbl_mbc92_AusEinheit.TabIndex = 43
        Me.lbl_mbc92_AusEinheit.Text = "[ms]"
        '
        'lbl_mbc92_Rueck
        '
        Me.lbl_mbc92_Rueck.AutoSize = True
        Me.lbl_mbc92_Rueck.Location = New System.Drawing.Point(521, 23)
        Me.lbl_mbc92_Rueck.Name = "lbl_mbc92_Rueck"
        Me.lbl_mbc92_Rueck.Size = New System.Drawing.Size(65, 13)
        Me.lbl_mbc92_Rueck.TabIndex = 47
        Me.lbl_mbc92_Rueck.Text = "Meldezyklus"
        '
        'lbl_mbc92_Aus
        '
        Me.lbl_mbc92_Aus.AutoSize = True
        Me.lbl_mbc92_Aus.Location = New System.Drawing.Point(353, 23)
        Me.lbl_mbc92_Aus.Name = "lbl_mbc92_Aus"
        Me.lbl_mbc92_Aus.Size = New System.Drawing.Size(54, 13)
        Me.lbl_mbc92_Aus.TabIndex = 46
        Me.lbl_mbc92_Aus.Text = "Auto-AUS"
        '
        'nud_mbc92_Rueck
        '
        Me.nud_mbc92_Rueck.Location = New System.Drawing.Point(593, 21)
        Me.nud_mbc92_Rueck.Maximum = New Decimal(New Integer() {1, 0, 0, 0})
        Me.nud_mbc92_Rueck.Name = "nud_mbc92_Rueck"
        Me.nud_mbc92_Rueck.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc92_Rueck.TabIndex = 45
        Me.nud_mbc92_Rueck.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        '
        'nud_mbc92_Aus
        '
        Me.nud_mbc92_Aus.Increment = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc92_Aus.Location = New System.Drawing.Point(411, 21)
        Me.nud_mbc92_Aus.Maximum = New Decimal(New Integer() {2000, 0, 0, 0})
        Me.nud_mbc92_Aus.Name = "nud_mbc92_Aus"
        Me.nud_mbc92_Aus.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc92_Aus.TabIndex = 44
        Me.nud_mbc92_Aus.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        '
        'lbl_mbc92_Wechsel
        '
        Me.lbl_mbc92_Wechsel.AutoSize = True
        Me.lbl_mbc92_Wechsel.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.lbl_mbc92_Wechsel.Location = New System.Drawing.Point(214, 218)
        Me.lbl_mbc92_Wechsel.Name = "lbl_mbc92_Wechsel"
        Me.lbl_mbc92_Wechsel.Size = New System.Drawing.Size(86, 13)
        Me.lbl_mbc92_Wechsel.TabIndex = 42
        Me.lbl_mbc92_Wechsel.Text = "Wechselschalter"
        '
        'lbl_mbc92_Impuls
        '
        Me.lbl_mbc92_Impuls.AutoSize = True
        Me.lbl_mbc92_Impuls.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.lbl_mbc92_Impuls.Location = New System.Drawing.Point(46, 218)
        Me.lbl_mbc92_Impuls.Name = "lbl_mbc92_Impuls"
        Me.lbl_mbc92_Impuls.Size = New System.Drawing.Size(78, 13)
        Me.lbl_mbc92_Impuls.TabIndex = 41
        Me.lbl_mbc92_Impuls.Text = "Impulsausgang"
        '
        'gb_mbc92_CH4
        '
        Me.gb_mbc92_CH4.Controls.Add(Me.txt_mbc92_CH4_Addr)
        Me.gb_mbc92_CH4.Controls.Add(Me.lbl_mbc92_CH4_Addr)
        Me.gb_mbc92_CH4.Controls.Add(Me.nud_mbc92_CH4_TIO)
        Me.gb_mbc92_CH4.Controls.Add(Me.lbl_mbc92_CH4_Typ)
        Me.gb_mbc92_CH4.Controls.Add(Me.lbl_mbc92_CH4_TIO)
        Me.gb_mbc92_CH4.Controls.Add(Me.clb_mbc92_CH4)
        Me.gb_mbc92_CH4.Controls.Add(Me.lbl_mbc92_CH4_TIOEinheit)
        Me.gb_mbc92_CH4.Location = New System.Drawing.Point(510, 62)
        Me.gb_mbc92_CH4.Name = "gb_mbc92_CH4"
        Me.gb_mbc92_CH4.Size = New System.Drawing.Size(155, 142)
        Me.gb_mbc92_CH4.TabIndex = 37
        Me.gb_mbc92_CH4.TabStop = False
        Me.gb_mbc92_CH4.Text = "Ausgang 4"
        '
        'txt_mbc92_CH4_Addr
        '
        Me.txt_mbc92_CH4_Addr.Location = New System.Drawing.Point(67, 23)
        Me.txt_mbc92_CH4_Addr.Name = "txt_mbc92_CH4_Addr"
        Me.txt_mbc92_CH4_Addr.ReadOnly = True
        Me.txt_mbc92_CH4_Addr.Size = New System.Drawing.Size(50, 20)
        Me.txt_mbc92_CH4_Addr.TabIndex = 16
        '
        'lbl_mbc92_CH4_Addr
        '
        Me.lbl_mbc92_CH4_Addr.AutoSize = True
        Me.lbl_mbc92_CH4_Addr.Location = New System.Drawing.Point(9, 26)
        Me.lbl_mbc92_CH4_Addr.Name = "lbl_mbc92_CH4_Addr"
        Me.lbl_mbc92_CH4_Addr.Size = New System.Drawing.Size(48, 13)
        Me.lbl_mbc92_CH4_Addr.TabIndex = 15
        Me.lbl_mbc92_CH4_Addr.Text = "Adresse:"
        '
        'nud_mbc92_CH4_TIO
        '
        Me.nud_mbc92_CH4_TIO.Increment = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc92_CH4_TIO.Location = New System.Drawing.Point(67, 108)
        Me.nud_mbc92_CH4_TIO.Maximum = New Decimal(New Integer() {2500, 0, 0, 0})
        Me.nud_mbc92_CH4_TIO.Minimum = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc92_CH4_TIO.Name = "nud_mbc92_CH4_TIO"
        Me.nud_mbc92_CH4_TIO.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc92_CH4_TIO.TabIndex = 8
        Me.nud_mbc92_CH4_TIO.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc92_CH4_TIO.Value = New Decimal(New Integer() {200, 0, 0, 0})
        '
        'lbl_mbc92_CH4_Typ
        '
        Me.lbl_mbc92_CH4_Typ.AutoSize = True
        Me.lbl_mbc92_CH4_Typ.Location = New System.Drawing.Point(9, 60)
        Me.lbl_mbc92_CH4_Typ.Name = "lbl_mbc92_CH4_Typ"
        Me.lbl_mbc92_CH4_Typ.Size = New System.Drawing.Size(28, 13)
        Me.lbl_mbc92_CH4_Typ.TabIndex = 11
        Me.lbl_mbc92_CH4_Typ.Text = "Typ:"
        '
        'lbl_mbc92_CH4_TIO
        '
        Me.lbl_mbc92_CH4_TIO.AutoSize = True
        Me.lbl_mbc92_CH4_TIO.Location = New System.Drawing.Point(9, 110)
        Me.lbl_mbc92_CH4_TIO.Name = "lbl_mbc92_CH4_TIO"
        Me.lbl_mbc92_CH4_TIO.Size = New System.Drawing.Size(52, 13)
        Me.lbl_mbc92_CH4_TIO.TabIndex = 9
        Me.lbl_mbc92_CH4_TIO.Text = "ON-Time:"
        '
        'clb_mbc92_CH4
        '
        Me.clb_mbc92_CH4.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.clb_mbc92_CH4.ColumnWidth = 60
        Me.clb_mbc92_CH4.Enabled = False
        Me.clb_mbc92_CH4.FormattingEnabled = True
        Me.clb_mbc92_CH4.Items.AddRange(New Object() {"Impuls", "Wechsel"})
        Me.clb_mbc92_CH4.Location = New System.Drawing.Point(67, 60)
        Me.clb_mbc92_CH4.Name = "clb_mbc92_CH4"
        Me.clb_mbc92_CH4.Size = New System.Drawing.Size(77, 30)
        Me.clb_mbc92_CH4.TabIndex = 7
        Me.clb_mbc92_CH4.TabStop = False
        '
        'lbl_mbc92_CH4_TIOEinheit
        '
        Me.lbl_mbc92_CH4_TIOEinheit.AutoSize = True
        Me.lbl_mbc92_CH4_TIOEinheit.Location = New System.Drawing.Point(126, 110)
        Me.lbl_mbc92_CH4_TIOEinheit.Name = "lbl_mbc92_CH4_TIOEinheit"
        Me.lbl_mbc92_CH4_TIOEinheit.Size = New System.Drawing.Size(26, 13)
        Me.lbl_mbc92_CH4_TIOEinheit.TabIndex = 10
        Me.lbl_mbc92_CH4_TIOEinheit.Text = "[ms]"
        '
        'gb_mbc92_CH3
        '
        Me.gb_mbc92_CH3.Controls.Add(Me.txt_mbc92_CH3_Addr)
        Me.gb_mbc92_CH3.Controls.Add(Me.lbl_mbc92_CH3_Addr)
        Me.gb_mbc92_CH3.Controls.Add(Me.nud_mbc92_CH3_TIO)
        Me.gb_mbc92_CH3.Controls.Add(Me.lbl_mbc92_CH3_Typ)
        Me.gb_mbc92_CH3.Controls.Add(Me.lbl_mbc92_CH3_TIO)
        Me.gb_mbc92_CH3.Controls.Add(Me.clb_mbc92_CH3)
        Me.gb_mbc92_CH3.Controls.Add(Me.lbl_mbc92_CH3_TIOEinheit)
        Me.gb_mbc92_CH3.Location = New System.Drawing.Point(344, 62)
        Me.gb_mbc92_CH3.Name = "gb_mbc92_CH3"
        Me.gb_mbc92_CH3.Size = New System.Drawing.Size(155, 142)
        Me.gb_mbc92_CH3.TabIndex = 38
        Me.gb_mbc92_CH3.TabStop = False
        Me.gb_mbc92_CH3.Text = "Ausgang 3"
        '
        'txt_mbc92_CH3_Addr
        '
        Me.txt_mbc92_CH3_Addr.Location = New System.Drawing.Point(67, 23)
        Me.txt_mbc92_CH3_Addr.Name = "txt_mbc92_CH3_Addr"
        Me.txt_mbc92_CH3_Addr.ReadOnly = True
        Me.txt_mbc92_CH3_Addr.Size = New System.Drawing.Size(50, 20)
        Me.txt_mbc92_CH3_Addr.TabIndex = 15
        '
        'lbl_mbc92_CH3_Addr
        '
        Me.lbl_mbc92_CH3_Addr.AutoSize = True
        Me.lbl_mbc92_CH3_Addr.Location = New System.Drawing.Point(9, 26)
        Me.lbl_mbc92_CH3_Addr.Name = "lbl_mbc92_CH3_Addr"
        Me.lbl_mbc92_CH3_Addr.Size = New System.Drawing.Size(48, 13)
        Me.lbl_mbc92_CH3_Addr.TabIndex = 14
        Me.lbl_mbc92_CH3_Addr.Text = "Adresse:"
        '
        'nud_mbc92_CH3_TIO
        '
        Me.nud_mbc92_CH3_TIO.Increment = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc92_CH3_TIO.Location = New System.Drawing.Point(67, 108)
        Me.nud_mbc92_CH3_TIO.Maximum = New Decimal(New Integer() {2500, 0, 0, 0})
        Me.nud_mbc92_CH3_TIO.Minimum = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc92_CH3_TIO.Name = "nud_mbc92_CH3_TIO"
        Me.nud_mbc92_CH3_TIO.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc92_CH3_TIO.TabIndex = 8
        Me.nud_mbc92_CH3_TIO.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc92_CH3_TIO.Value = New Decimal(New Integer() {200, 0, 0, 0})
        '
        'lbl_mbc92_CH3_Typ
        '
        Me.lbl_mbc92_CH3_Typ.AutoSize = True
        Me.lbl_mbc92_CH3_Typ.Location = New System.Drawing.Point(9, 60)
        Me.lbl_mbc92_CH3_Typ.Name = "lbl_mbc92_CH3_Typ"
        Me.lbl_mbc92_CH3_Typ.Size = New System.Drawing.Size(28, 13)
        Me.lbl_mbc92_CH3_Typ.TabIndex = 11
        Me.lbl_mbc92_CH3_Typ.Text = "Typ:"
        '
        'lbl_mbc92_CH3_TIO
        '
        Me.lbl_mbc92_CH3_TIO.AutoSize = True
        Me.lbl_mbc92_CH3_TIO.Location = New System.Drawing.Point(9, 110)
        Me.lbl_mbc92_CH3_TIO.Name = "lbl_mbc92_CH3_TIO"
        Me.lbl_mbc92_CH3_TIO.Size = New System.Drawing.Size(52, 13)
        Me.lbl_mbc92_CH3_TIO.TabIndex = 9
        Me.lbl_mbc92_CH3_TIO.Text = "ON-Time:"
        '
        'clb_mbc92_CH3
        '
        Me.clb_mbc92_CH3.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.clb_mbc92_CH3.ColumnWidth = 60
        Me.clb_mbc92_CH3.Enabled = False
        Me.clb_mbc92_CH3.FormattingEnabled = True
        Me.clb_mbc92_CH3.Items.AddRange(New Object() {"Impuls", "Wechsel"})
        Me.clb_mbc92_CH3.Location = New System.Drawing.Point(67, 60)
        Me.clb_mbc92_CH3.Name = "clb_mbc92_CH3"
        Me.clb_mbc92_CH3.Size = New System.Drawing.Size(77, 30)
        Me.clb_mbc92_CH3.TabIndex = 7
        Me.clb_mbc92_CH3.TabStop = False
        '
        'lbl_mbc92_CH3_TIOEinheit
        '
        Me.lbl_mbc92_CH3_TIOEinheit.AutoSize = True
        Me.lbl_mbc92_CH3_TIOEinheit.Location = New System.Drawing.Point(126, 110)
        Me.lbl_mbc92_CH3_TIOEinheit.Name = "lbl_mbc92_CH3_TIOEinheit"
        Me.lbl_mbc92_CH3_TIOEinheit.Size = New System.Drawing.Size(26, 13)
        Me.lbl_mbc92_CH3_TIOEinheit.TabIndex = 10
        Me.lbl_mbc92_CH3_TIOEinheit.Text = "[ms]"
        '
        'gb_mbc92_CH2
        '
        Me.gb_mbc92_CH2.Controls.Add(Me.txt_mbc92_CH2_Addr)
        Me.gb_mbc92_CH2.Controls.Add(Me.lbl_mbc92_CH2_Addr)
        Me.gb_mbc92_CH2.Controls.Add(Me.nud_mbc92_CH2_TIO)
        Me.gb_mbc92_CH2.Controls.Add(Me.lbl_mbc92_CH2_Typ)
        Me.gb_mbc92_CH2.Controls.Add(Me.lbl_mbc92_CH2_TIO)
        Me.gb_mbc92_CH2.Controls.Add(Me.clb_mbc92_CH2)
        Me.gb_mbc92_CH2.Controls.Add(Me.lbl_mbc92_CH2_TIOEinheit)
        Me.gb_mbc92_CH2.Location = New System.Drawing.Point(178, 62)
        Me.gb_mbc92_CH2.Name = "gb_mbc92_CH2"
        Me.gb_mbc92_CH2.Size = New System.Drawing.Size(155, 142)
        Me.gb_mbc92_CH2.TabIndex = 39
        Me.gb_mbc92_CH2.TabStop = False
        Me.gb_mbc92_CH2.Text = "Ausgang 2"
        '
        'txt_mbc92_CH2_Addr
        '
        Me.txt_mbc92_CH2_Addr.Location = New System.Drawing.Point(67, 23)
        Me.txt_mbc92_CH2_Addr.Name = "txt_mbc92_CH2_Addr"
        Me.txt_mbc92_CH2_Addr.ReadOnly = True
        Me.txt_mbc92_CH2_Addr.Size = New System.Drawing.Size(50, 20)
        Me.txt_mbc92_CH2_Addr.TabIndex = 14
        '
        'lbl_mbc92_CH2_Addr
        '
        Me.lbl_mbc92_CH2_Addr.AutoSize = True
        Me.lbl_mbc92_CH2_Addr.Location = New System.Drawing.Point(9, 26)
        Me.lbl_mbc92_CH2_Addr.Name = "lbl_mbc92_CH2_Addr"
        Me.lbl_mbc92_CH2_Addr.Size = New System.Drawing.Size(48, 13)
        Me.lbl_mbc92_CH2_Addr.TabIndex = 13
        Me.lbl_mbc92_CH2_Addr.Text = "Adresse:"
        '
        'nud_mbc92_CH2_TIO
        '
        Me.nud_mbc92_CH2_TIO.Increment = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc92_CH2_TIO.Location = New System.Drawing.Point(67, 108)
        Me.nud_mbc92_CH2_TIO.Maximum = New Decimal(New Integer() {2500, 0, 0, 0})
        Me.nud_mbc92_CH2_TIO.Minimum = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc92_CH2_TIO.Name = "nud_mbc92_CH2_TIO"
        Me.nud_mbc92_CH2_TIO.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc92_CH2_TIO.TabIndex = 8
        Me.nud_mbc92_CH2_TIO.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc92_CH2_TIO.Value = New Decimal(New Integer() {200, 0, 0, 0})
        '
        'lbl_mbc92_CH2_Typ
        '
        Me.lbl_mbc92_CH2_Typ.AutoSize = True
        Me.lbl_mbc92_CH2_Typ.Location = New System.Drawing.Point(9, 60)
        Me.lbl_mbc92_CH2_Typ.Name = "lbl_mbc92_CH2_Typ"
        Me.lbl_mbc92_CH2_Typ.Size = New System.Drawing.Size(28, 13)
        Me.lbl_mbc92_CH2_Typ.TabIndex = 11
        Me.lbl_mbc92_CH2_Typ.Text = "Typ:"
        '
        'lbl_mbc92_CH2_TIO
        '
        Me.lbl_mbc92_CH2_TIO.AutoSize = True
        Me.lbl_mbc92_CH2_TIO.Location = New System.Drawing.Point(9, 110)
        Me.lbl_mbc92_CH2_TIO.Name = "lbl_mbc92_CH2_TIO"
        Me.lbl_mbc92_CH2_TIO.Size = New System.Drawing.Size(52, 13)
        Me.lbl_mbc92_CH2_TIO.TabIndex = 9
        Me.lbl_mbc92_CH2_TIO.Text = "ON-Time:"
        '
        'clb_mbc92_CH2
        '
        Me.clb_mbc92_CH2.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.clb_mbc92_CH2.ColumnWidth = 60
        Me.clb_mbc92_CH2.Enabled = False
        Me.clb_mbc92_CH2.FormattingEnabled = True
        Me.clb_mbc92_CH2.Items.AddRange(New Object() {"Impuls", "Wechsel"})
        Me.clb_mbc92_CH2.Location = New System.Drawing.Point(67, 60)
        Me.clb_mbc92_CH2.Name = "clb_mbc92_CH2"
        Me.clb_mbc92_CH2.Size = New System.Drawing.Size(77, 30)
        Me.clb_mbc92_CH2.TabIndex = 7
        Me.clb_mbc92_CH2.TabStop = False
        '
        'lbl_mbc92_CH2_TIOEinheit
        '
        Me.lbl_mbc92_CH2_TIOEinheit.AutoSize = True
        Me.lbl_mbc92_CH2_TIOEinheit.Location = New System.Drawing.Point(126, 110)
        Me.lbl_mbc92_CH2_TIOEinheit.Name = "lbl_mbc92_CH2_TIOEinheit"
        Me.lbl_mbc92_CH2_TIOEinheit.Size = New System.Drawing.Size(26, 13)
        Me.lbl_mbc92_CH2_TIOEinheit.TabIndex = 10
        Me.lbl_mbc92_CH2_TIOEinheit.Text = "[ms]"
        '
        'gb_mbc92_CH1
        '
        Me.gb_mbc92_CH1.Controls.Add(Me.txt_mbc92_CH1_Addr)
        Me.gb_mbc92_CH1.Controls.Add(Me.lbl_mbc92_CH1_Addr)
        Me.gb_mbc92_CH1.Controls.Add(Me.nud_mbc92_CH1_TIO)
        Me.gb_mbc92_CH1.Controls.Add(Me.lbl_mbc92_CH1_Typ)
        Me.gb_mbc92_CH1.Controls.Add(Me.lbl_mbc92_CH1_TIO)
        Me.gb_mbc92_CH1.Controls.Add(Me.clb_mbc92_CH1)
        Me.gb_mbc92_CH1.Controls.Add(Me.lbl_mbc92_CH1_TIOEinheit)
        Me.gb_mbc92_CH1.Location = New System.Drawing.Point(12, 62)
        Me.gb_mbc92_CH1.Name = "gb_mbc92_CH1"
        Me.gb_mbc92_CH1.Size = New System.Drawing.Size(155, 142)
        Me.gb_mbc92_CH1.TabIndex = 36
        Me.gb_mbc92_CH1.TabStop = False
        Me.gb_mbc92_CH1.Text = "Ausgang 1"
        '
        'txt_mbc92_CH1_Addr
        '
        Me.txt_mbc92_CH1_Addr.Location = New System.Drawing.Point(67, 23)
        Me.txt_mbc92_CH1_Addr.Name = "txt_mbc92_CH1_Addr"
        Me.txt_mbc92_CH1_Addr.ReadOnly = True
        Me.txt_mbc92_CH1_Addr.Size = New System.Drawing.Size(50, 20)
        Me.txt_mbc92_CH1_Addr.TabIndex = 13
        '
        'lbl_mbc92_CH1_Addr
        '
        Me.lbl_mbc92_CH1_Addr.AutoSize = True
        Me.lbl_mbc92_CH1_Addr.Location = New System.Drawing.Point(9, 26)
        Me.lbl_mbc92_CH1_Addr.Name = "lbl_mbc92_CH1_Addr"
        Me.lbl_mbc92_CH1_Addr.Size = New System.Drawing.Size(48, 13)
        Me.lbl_mbc92_CH1_Addr.TabIndex = 12
        Me.lbl_mbc92_CH1_Addr.Text = "Adresse:"
        '
        'nud_mbc92_CH1_TIO
        '
        Me.nud_mbc92_CH1_TIO.Increment = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc92_CH1_TIO.Location = New System.Drawing.Point(67, 108)
        Me.nud_mbc92_CH1_TIO.Maximum = New Decimal(New Integer() {2500, 0, 0, 0})
        Me.nud_mbc92_CH1_TIO.Minimum = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nud_mbc92_CH1_TIO.Name = "nud_mbc92_CH1_TIO"
        Me.nud_mbc92_CH1_TIO.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc92_CH1_TIO.TabIndex = 8
        Me.nud_mbc92_CH1_TIO.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc92_CH1_TIO.Value = New Decimal(New Integer() {200, 0, 0, 0})
        '
        'lbl_mbc92_CH1_Typ
        '
        Me.lbl_mbc92_CH1_Typ.AutoSize = True
        Me.lbl_mbc92_CH1_Typ.Location = New System.Drawing.Point(9, 60)
        Me.lbl_mbc92_CH1_Typ.Name = "lbl_mbc92_CH1_Typ"
        Me.lbl_mbc92_CH1_Typ.Size = New System.Drawing.Size(28, 13)
        Me.lbl_mbc92_CH1_Typ.TabIndex = 11
        Me.lbl_mbc92_CH1_Typ.Text = "Typ:"
        '
        'lbl_mbc92_CH1_TIO
        '
        Me.lbl_mbc92_CH1_TIO.AutoSize = True
        Me.lbl_mbc92_CH1_TIO.Location = New System.Drawing.Point(9, 110)
        Me.lbl_mbc92_CH1_TIO.Name = "lbl_mbc92_CH1_TIO"
        Me.lbl_mbc92_CH1_TIO.Size = New System.Drawing.Size(52, 13)
        Me.lbl_mbc92_CH1_TIO.TabIndex = 9
        Me.lbl_mbc92_CH1_TIO.Text = "ON-Time:"
        '
        'clb_mbc92_CH1
        '
        Me.clb_mbc92_CH1.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.clb_mbc92_CH1.ColumnWidth = 60
        Me.clb_mbc92_CH1.Enabled = False
        Me.clb_mbc92_CH1.FormattingEnabled = True
        Me.clb_mbc92_CH1.Items.AddRange(New Object() {"Impuls", "Wechsel"})
        Me.clb_mbc92_CH1.Location = New System.Drawing.Point(67, 60)
        Me.clb_mbc92_CH1.Name = "clb_mbc92_CH1"
        Me.clb_mbc92_CH1.Size = New System.Drawing.Size(77, 30)
        Me.clb_mbc92_CH1.TabIndex = 7
        Me.clb_mbc92_CH1.TabStop = False
        '
        'lbl_mbc92_CH1_TIOEinheit
        '
        Me.lbl_mbc92_CH1_TIOEinheit.AutoSize = True
        Me.lbl_mbc92_CH1_TIOEinheit.Location = New System.Drawing.Point(126, 110)
        Me.lbl_mbc92_CH1_TIOEinheit.Name = "lbl_mbc92_CH1_TIOEinheit"
        Me.lbl_mbc92_CH1_TIOEinheit.Size = New System.Drawing.Size(26, 13)
        Me.lbl_mbc92_CH1_TIOEinheit.TabIndex = 10
        Me.lbl_mbc92_CH1_TIOEinheit.Text = "[ms]"
        '
        'nud_mbc92_MMAddr
        '
        Me.nud_mbc92_MMAddr.Hexadecimal = True
        Me.nud_mbc92_MMAddr.Increment = New Decimal(New Integer() {4, 0, 0, 0})
        Me.nud_mbc92_MMAddr.Location = New System.Drawing.Point(108, 21)
        Me.nud_mbc92_MMAddr.Maximum = New Decimal(New Integer() {13308, 0, 0, 0})
        Me.nud_mbc92_MMAddr.Minimum = New Decimal(New Integer() {12288, 0, 0, 0})
        Me.nud_mbc92_MMAddr.Name = "nud_mbc92_MMAddr"
        Me.nud_mbc92_MMAddr.Size = New System.Drawing.Size(53, 20)
        Me.nud_mbc92_MMAddr.TabIndex = 34
        Me.nud_mbc92_MMAddr.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        Me.nud_mbc92_MMAddr.Value = New Decimal(New Integer() {12288, 0, 0, 0})
        '
        'lbl_mbc92_MMAddr
        '
        Me.lbl_mbc92_MMAddr.AutoSize = True
        Me.lbl_mbc92_MMAddr.Location = New System.Drawing.Point(14, 23)
        Me.lbl_mbc92_MMAddr.Name = "lbl_mbc92_MMAddr"
        Me.lbl_mbc92_MMAddr.Size = New System.Drawing.Size(75, 13)
        Me.lbl_mbc92_MMAddr.TabIndex = 33
        Me.lbl_mbc92_MMAddr.Text = "Loc-ID in HEX"
        '
        'pic_mbc92_Wechsel
        '
        Me.pic_mbc92_Wechsel.InitialImage = Global.MBCAN.My.Resources.Resources.img_wechsel
        Me.pic_mbc92_Wechsel.Location = New System.Drawing.Point(179, 210)
        Me.pic_mbc92_Wechsel.Name = "pic_mbc92_Wechsel"
        Me.pic_mbc92_Wechsel.Size = New System.Drawing.Size(26, 26)
        Me.pic_mbc92_Wechsel.TabIndex = 40
        Me.pic_mbc92_Wechsel.TabStop = False
        '
        'pic_mbc92_Impuls
        '
        Me.pic_mbc92_Impuls.InitialImage = Global.MBCAN.My.Resources.Resources.img_impuls
        Me.pic_mbc92_Impuls.Location = New System.Drawing.Point(12, 210)
        Me.pic_mbc92_Impuls.Name = "pic_mbc92_Impuls"
        Me.pic_mbc92_Impuls.Size = New System.Drawing.Size(26, 26)
        Me.pic_mbc92_Impuls.TabIndex = 35
        Me.pic_mbc92_Impuls.TabStop = False
        '
        'tab_mbc93
        '
        Me.tab_mbc93.BackColor = System.Drawing.Color.White
        Me.tab_mbc93.Controls.Add(Me.gb_mbc93_Latenz)
        Me.tab_mbc93.Controls.Add(Me.gb_mbc93_GFP)
        Me.tab_mbc93.Location = New System.Drawing.Point(4, 22)
        Me.tab_mbc93.Name = "tab_mbc93"
        Me.tab_mbc93.Padding = New System.Windows.Forms.Padding(3)
        Me.tab_mbc93.Size = New System.Drawing.Size(680, 248)
        Me.tab_mbc93.TabIndex = 13
        Me.tab_mbc93.Text = "mbc-93"
        '
        'gb_mbc93_Latenz
        '
        Me.gb_mbc93_Latenz.Controls.Add(Me.lbl_mbc93_LatenzBeschreibung_3)
        Me.gb_mbc93_Latenz.Controls.Add(Me.lbl_mbc93_LatenzBeschreibung_2)
        Me.gb_mbc93_Latenz.Controls.Add(Me.nud_mbc93_Latenz)
        Me.gb_mbc93_Latenz.Controls.Add(Me.lbl_mbc93_LatenzBeschreibung_1)
        Me.gb_mbc93_Latenz.Controls.Add(Me.lbl_mbc93_LatenzBezeichnung)
        Me.gb_mbc93_Latenz.Location = New System.Drawing.Point(339, 16)
        Me.gb_mbc93_Latenz.Name = "gb_mbc93_Latenz"
        Me.gb_mbc93_Latenz.Size = New System.Drawing.Size(317, 212)
        Me.gb_mbc93_Latenz.TabIndex = 71
        Me.gb_mbc93_Latenz.TabStop = False
        Me.gb_mbc93_Latenz.Text = "Latenzzeit Kurzschlusserkennung"
        '
        'lbl_mbc93_LatenzBeschreibung_3
        '
        Me.lbl_mbc93_LatenzBeschreibung_3.BackColor = System.Drawing.SystemColors.Window
        Me.lbl_mbc93_LatenzBeschreibung_3.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.lbl_mbc93_LatenzBeschreibung_3.Location = New System.Drawing.Point(9, 125)
        Me.lbl_mbc93_LatenzBeschreibung_3.Multiline = True
        Me.lbl_mbc93_LatenzBeschreibung_3.Name = "lbl_mbc93_LatenzBeschreibung_3"
        Me.lbl_mbc93_LatenzBeschreibung_3.ReadOnly = True
        Me.lbl_mbc93_LatenzBeschreibung_3.Size = New System.Drawing.Size(302, 42)
        Me.lbl_mbc93_LatenzBeschreibung_3.TabIndex = 71
        Me.lbl_mbc93_LatenzBeschreibung_3.Text = "Bei einer CS2 bitte diesen Wert ausschließlich in der CS2-GUI ändern und über den" &
    " <Refresh>-Button aktualisieren. " & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10)
        '
        'lbl_mbc93_LatenzBeschreibung_2
        '
        Me.lbl_mbc93_LatenzBeschreibung_2.BackColor = System.Drawing.SystemColors.Window
        Me.lbl_mbc93_LatenzBeschreibung_2.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.lbl_mbc93_LatenzBeschreibung_2.Location = New System.Drawing.Point(9, 74)
        Me.lbl_mbc93_LatenzBeschreibung_2.Multiline = True
        Me.lbl_mbc93_LatenzBeschreibung_2.Name = "lbl_mbc93_LatenzBeschreibung_2"
        Me.lbl_mbc93_LatenzBeschreibung_2.ReadOnly = True
        Me.lbl_mbc93_LatenzBeschreibung_2.Size = New System.Drawing.Size(302, 45)
        Me.lbl_mbc93_LatenzBeschreibung_2.TabIndex = 70
        Me.lbl_mbc93_LatenzBeschreibung_2.Text = "Nach dem <Update> des Moduls bitte dieses aus der GUI der CS3 löschen. Der neue L" &
    "atenzwert wird dann in der CS3 angezeigt."
        '
        'nud_mbc93_Latenz
        '
        Me.nud_mbc93_Latenz.Increment = New Decimal(New Integer() {50, 0, 0, 0})
        Me.nud_mbc93_Latenz.Location = New System.Drawing.Point(75, 174)
        Me.nud_mbc93_Latenz.Maximum = New Decimal(New Integer() {1000, 0, 0, 0})
        Me.nud_mbc93_Latenz.Minimum = New Decimal(New Integer() {50, 0, 0, 0})
        Me.nud_mbc93_Latenz.Name = "nud_mbc93_Latenz"
        Me.nud_mbc93_Latenz.Size = New System.Drawing.Size(52, 20)
        Me.nud_mbc93_Latenz.TabIndex = 69
        Me.nud_mbc93_Latenz.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        Me.nud_mbc93_Latenz.Value = New Decimal(New Integer() {50, 0, 0, 0})
        '
        'lbl_mbc93_LatenzBeschreibung_1
        '
        Me.lbl_mbc93_LatenzBeschreibung_1.BackColor = System.Drawing.SystemColors.Window
        Me.lbl_mbc93_LatenzBeschreibung_1.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.lbl_mbc93_LatenzBeschreibung_1.Location = New System.Drawing.Point(9, 32)
        Me.lbl_mbc93_LatenzBeschreibung_1.Multiline = True
        Me.lbl_mbc93_LatenzBeschreibung_1.Name = "lbl_mbc93_LatenzBeschreibung_1"
        Me.lbl_mbc93_LatenzBeschreibung_1.ReadOnly = True
        Me.lbl_mbc93_LatenzBeschreibung_1.Size = New System.Drawing.Size(302, 36)
        Me.lbl_mbc93_LatenzBeschreibung_1.TabIndex = 65
        Me.lbl_mbc93_LatenzBeschreibung_1.Text = "Die Latenzzeit bis zum Ansprechen der Kurzschlusserkennung kann hier in Schritten" &
    " von 50 ms eingestellt werden."
        '
        'lbl_mbc93_LatenzBezeichnung
        '
        Me.lbl_mbc93_LatenzBezeichnung.AutoSize = True
        Me.lbl_mbc93_LatenzBezeichnung.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.lbl_mbc93_LatenzBezeichnung.Location = New System.Drawing.Point(8, 176)
        Me.lbl_mbc93_LatenzBezeichnung.Name = "lbl_mbc93_LatenzBezeichnung"
        Me.lbl_mbc93_LatenzBezeichnung.Size = New System.Drawing.Size(61, 13)
        Me.lbl_mbc93_LatenzBezeichnung.TabIndex = 63
        Me.lbl_mbc93_LatenzBezeichnung.Text = "Latenz [ms]"
        '
        'gb_mbc93_GFP
        '
        Me.gb_mbc93_GFP.Controls.Add(Me.lbl_mbc93_GFPBeschreibung_2)
        Me.gb_mbc93_GFP.Controls.Add(Me.Label32)
        Me.gb_mbc93_GFP.Controls.Add(Me.nud_mbc93_GFPSW2)
        Me.gb_mbc93_GFP.Controls.Add(Me.nud_mbc93_GFPSW1)
        Me.gb_mbc93_GFP.Controls.Add(Me.lbl_mbc93_GFPBeschreibung_1)
        Me.gb_mbc93_GFP.Controls.Add(Me.lbl_mbc93_GFPBezeichnung)
        Me.gb_mbc93_GFP.Location = New System.Drawing.Point(20, 16)
        Me.gb_mbc93_GFP.Name = "gb_mbc93_GFP"
        Me.gb_mbc93_GFP.Size = New System.Drawing.Size(270, 212)
        Me.gb_mbc93_GFP.TabIndex = 70
        Me.gb_mbc93_GFP.TabStop = False
        Me.gb_mbc93_GFP.Text = "GFP-Softwareversion"
        '
        'lbl_mbc93_GFPBeschreibung_2
        '
        Me.lbl_mbc93_GFPBeschreibung_2.BackColor = System.Drawing.SystemColors.Window
        Me.lbl_mbc93_GFPBeschreibung_2.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.lbl_mbc93_GFPBeschreibung_2.Location = New System.Drawing.Point(9, 86)
        Me.lbl_mbc93_GFPBeschreibung_2.Multiline = True
        Me.lbl_mbc93_GFPBeschreibung_2.Name = "lbl_mbc93_GFPBeschreibung_2"
        Me.lbl_mbc93_GFPBeschreibung_2.ReadOnly = True
        Me.lbl_mbc93_GFPBeschreibung_2.Size = New System.Drawing.Size(231, 50)
        Me.lbl_mbc93_GFPBeschreibung_2.TabIndex = 69
        Me.lbl_mbc93_GFPBeschreibung_2.Text = "Nach dem <Update> das Modul bitte per <Reset> neu starten. Die neue Softwareversi" &
    "on wird dann in der CSx angezeigt."
        '
        'Label32
        '
        Me.Label32.AutoSize = True
        Me.Label32.Font = New System.Drawing.Font("Microsoft Sans Serif", 14.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label32.Location = New System.Drawing.Point(153, 170)
        Me.Label32.Name = "Label32"
        Me.Label32.Size = New System.Drawing.Size(15, 24)
        Me.Label32.TabIndex = 68
        Me.Label32.Text = "."
        '
        'nud_mbc93_GFPSW2
        '
        Me.nud_mbc93_GFPSW2.Location = New System.Drawing.Point(173, 174)
        Me.nud_mbc93_GFPSW2.Maximum = New Decimal(New Integer() {255, 0, 0, 0})
        Me.nud_mbc93_GFPSW2.Name = "nud_mbc93_GFPSW2"
        Me.nud_mbc93_GFPSW2.Size = New System.Drawing.Size(52, 20)
        Me.nud_mbc93_GFPSW2.TabIndex = 67
        Me.nud_mbc93_GFPSW2.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'nud_mbc93_GFPSW1
        '
        Me.nud_mbc93_GFPSW1.Location = New System.Drawing.Point(99, 174)
        Me.nud_mbc93_GFPSW1.Maximum = New Decimal(New Integer() {255, 0, 0, 0})
        Me.nud_mbc93_GFPSW1.Name = "nud_mbc93_GFPSW1"
        Me.nud_mbc93_GFPSW1.Size = New System.Drawing.Size(52, 20)
        Me.nud_mbc93_GFPSW1.TabIndex = 66
        Me.nud_mbc93_GFPSW1.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'lbl_mbc93_GFPBeschreibung_1
        '
        Me.lbl_mbc93_GFPBeschreibung_1.BackColor = System.Drawing.SystemColors.Window
        Me.lbl_mbc93_GFPBeschreibung_1.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.lbl_mbc93_GFPBeschreibung_1.Location = New System.Drawing.Point(9, 32)
        Me.lbl_mbc93_GFPBeschreibung_1.Multiline = True
        Me.lbl_mbc93_GFPBeschreibung_1.Name = "lbl_mbc93_GFPBeschreibung_1"
        Me.lbl_mbc93_GFPBeschreibung_1.ReadOnly = True
        Me.lbl_mbc93_GFPBeschreibung_1.Size = New System.Drawing.Size(231, 48)
        Me.lbl_mbc93_GFPBeschreibung_1.TabIndex = 65
        Me.lbl_mbc93_GFPBeschreibung_1.Text = "Die aktuelle Softwareversion des GFP/Booster von Märklin, um die Update-Aufforder" &
    "ung von der CSx zu umgehen."
        '
        'lbl_mbc93_GFPBezeichnung
        '
        Me.lbl_mbc93_GFPBezeichnung.AutoSize = True
        Me.lbl_mbc93_GFPBezeichnung.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.lbl_mbc93_GFPBezeichnung.Location = New System.Drawing.Point(6, 176)
        Me.lbl_mbc93_GFPBezeichnung.Name = "lbl_mbc93_GFPBezeichnung"
        Me.lbl_mbc93_GFPBezeichnung.Size = New System.Drawing.Size(87, 13)
        Me.lbl_mbc93_GFPBezeichnung.TabIndex = 63
        Me.lbl_mbc93_GFPBezeichnung.Text = "GFP-SW-Version"
        '
        'gb_Keyboard
        '
        Me.gb_Keyboard.Controls.Add(Me.btn_KeyboardG4)
        Me.gb_Keyboard.Controls.Add(Me.btn_KeyboardG3)
        Me.gb_Keyboard.Controls.Add(Me.btn_KeyboardG2)
        Me.gb_Keyboard.Controls.Add(Me.btn_KeyboardG1)
        Me.gb_Keyboard.Controls.Add(Me.btn_KeyboardR4)
        Me.gb_Keyboard.Controls.Add(Me.btn_KeyboardR3)
        Me.gb_Keyboard.Controls.Add(Me.btn_KeyboardR2)
        Me.gb_Keyboard.Controls.Add(Me.btn_KeyboardR1)
        Me.gb_Keyboard.Location = New System.Drawing.Point(283, 234)
        Me.gb_Keyboard.Name = "gb_Keyboard"
        Me.gb_Keyboard.Size = New System.Drawing.Size(419, 46)
        Me.gb_Keyboard.TabIndex = 64
        Me.gb_Keyboard.TabStop = False
        Me.gb_Keyboard.Text = "Stellpult"
        '
        'btn_KeyboardG4
        '
        Me.btn_KeyboardG4.BackColor = System.Drawing.Color.Green
        Me.btn_KeyboardG4.ForeColor = System.Drawing.SystemColors.Window
        Me.btn_KeyboardG4.Location = New System.Drawing.Point(367, 16)
        Me.btn_KeyboardG4.Name = "btn_KeyboardG4"
        Me.btn_KeyboardG4.Size = New System.Drawing.Size(43, 23)
        Me.btn_KeyboardG4.TabIndex = 88
        Me.btn_KeyboardG4.Text = "G"
        Me.btn_KeyboardG4.UseVisualStyleBackColor = False
        '
        'btn_KeyboardG3
        '
        Me.btn_KeyboardG3.BackColor = System.Drawing.Color.Green
        Me.btn_KeyboardG3.ForeColor = System.Drawing.SystemColors.Window
        Me.btn_KeyboardG3.Location = New System.Drawing.Point(263, 16)
        Me.btn_KeyboardG3.Name = "btn_KeyboardG3"
        Me.btn_KeyboardG3.Size = New System.Drawing.Size(43, 23)
        Me.btn_KeyboardG3.TabIndex = 87
        Me.btn_KeyboardG3.Text = "G"
        Me.btn_KeyboardG3.UseVisualStyleBackColor = False
        '
        'btn_KeyboardG2
        '
        Me.btn_KeyboardG2.BackColor = System.Drawing.Color.Green
        Me.btn_KeyboardG2.ForeColor = System.Drawing.SystemColors.Window
        Me.btn_KeyboardG2.Location = New System.Drawing.Point(159, 16)
        Me.btn_KeyboardG2.Name = "btn_KeyboardG2"
        Me.btn_KeyboardG2.Size = New System.Drawing.Size(43, 23)
        Me.btn_KeyboardG2.TabIndex = 86
        Me.btn_KeyboardG2.Text = "G"
        Me.btn_KeyboardG2.UseVisualStyleBackColor = False
        '
        'btn_KeyboardG1
        '
        Me.btn_KeyboardG1.BackColor = System.Drawing.Color.Green
        Me.btn_KeyboardG1.ForeColor = System.Drawing.SystemColors.Window
        Me.btn_KeyboardG1.Location = New System.Drawing.Point(55, 16)
        Me.btn_KeyboardG1.Name = "btn_KeyboardG1"
        Me.btn_KeyboardG1.Size = New System.Drawing.Size(43, 23)
        Me.btn_KeyboardG1.TabIndex = 85
        Me.btn_KeyboardG1.Text = "G"
        Me.btn_KeyboardG1.UseVisualStyleBackColor = False
        '
        'btn_KeyboardR4
        '
        Me.btn_KeyboardR4.BackColor = System.Drawing.Color.Red
        Me.btn_KeyboardR4.ForeColor = System.Drawing.SystemColors.Window
        Me.btn_KeyboardR4.Location = New System.Drawing.Point(321, 16)
        Me.btn_KeyboardR4.Name = "btn_KeyboardR4"
        Me.btn_KeyboardR4.Size = New System.Drawing.Size(43, 23)
        Me.btn_KeyboardR4.TabIndex = 84
        Me.btn_KeyboardR4.Text = "R"
        Me.btn_KeyboardR4.UseVisualStyleBackColor = False
        '
        'btn_KeyboardR3
        '
        Me.btn_KeyboardR3.BackColor = System.Drawing.Color.Red
        Me.btn_KeyboardR3.ForeColor = System.Drawing.SystemColors.Window
        Me.btn_KeyboardR3.Location = New System.Drawing.Point(217, 16)
        Me.btn_KeyboardR3.Name = "btn_KeyboardR3"
        Me.btn_KeyboardR3.Size = New System.Drawing.Size(43, 23)
        Me.btn_KeyboardR3.TabIndex = 83
        Me.btn_KeyboardR3.Text = "R"
        Me.btn_KeyboardR3.UseVisualStyleBackColor = False
        '
        'btn_KeyboardR2
        '
        Me.btn_KeyboardR2.BackColor = System.Drawing.Color.Red
        Me.btn_KeyboardR2.ForeColor = System.Drawing.SystemColors.Window
        Me.btn_KeyboardR2.Location = New System.Drawing.Point(113, 16)
        Me.btn_KeyboardR2.Name = "btn_KeyboardR2"
        Me.btn_KeyboardR2.Size = New System.Drawing.Size(43, 23)
        Me.btn_KeyboardR2.TabIndex = 82
        Me.btn_KeyboardR2.Text = "R"
        Me.btn_KeyboardR2.UseVisualStyleBackColor = False
        '
        'btn_KeyboardR1
        '
        Me.btn_KeyboardR1.BackColor = System.Drawing.Color.Red
        Me.btn_KeyboardR1.ForeColor = System.Drawing.SystemColors.Window
        Me.btn_KeyboardR1.Location = New System.Drawing.Point(9, 16)
        Me.btn_KeyboardR1.Name = "btn_KeyboardR1"
        Me.btn_KeyboardR1.Size = New System.Drawing.Size(43, 23)
        Me.btn_KeyboardR1.TabIndex = 0
        Me.btn_KeyboardR1.Text = "R"
        Me.btn_KeyboardR1.UseVisualStyleBackColor = False
        '
        'gb_Status
        '
        Me.gb_Status.Controls.Add(Me.pic_Status_ASM08)
        Me.gb_Status.Controls.Add(Me.pic_Status_ASM07)
        Me.gb_Status.Controls.Add(Me.pic_Status_ASM06)
        Me.gb_Status.Controls.Add(Me.pic_Status_ASM05)
        Me.gb_Status.Controls.Add(Me.pic_Status_ASM04)
        Me.gb_Status.Controls.Add(Me.pic_Status_ASM03)
        Me.gb_Status.Controls.Add(Me.pic_Status_ASM02)
        Me.gb_Status.Controls.Add(Me.pic_Status_ASM01)
        Me.gb_Status.Controls.Add(Me.pic_Status_Decoder)
        Me.gb_Status.Controls.Add(Me.pic_Status_LED19)
        Me.gb_Status.Controls.Add(Me.pic_Status_LED17)
        Me.gb_Status.Controls.Add(Me.pic_Status_LED14)
        Me.gb_Status.Controls.Add(Me.pic_Status_LED12)
        Me.gb_Status.Controls.Add(Me.pic_Status_LED08)
        Me.gb_Status.Controls.Add(Me.pic_Status_LED06)
        Me.gb_Status.Controls.Add(Me.pic_Status_LED03)
        Me.gb_Status.Controls.Add(Me.pic_Status_LED01)
        Me.gb_Status.Controls.Add(Me.pic_Status_LED18)
        Me.gb_Status.Controls.Add(Me.pic_Status_LED16)
        Me.gb_Status.Controls.Add(Me.pic_Status_LED15)
        Me.gb_Status.Controls.Add(Me.pic_Status_LED13)
        Me.gb_Status.Controls.Add(Me.pic_Status_LED07)
        Me.gb_Status.Controls.Add(Me.pic_Status_LED05)
        Me.gb_Status.Controls.Add(Me.pic_Status_LED04)
        Me.gb_Status.Controls.Add(Me.pic_Status_LED02)
        Me.gb_Status.Controls.Add(Me.pic_Status_LED09)
        Me.gb_Status.Controls.Add(Me.pic_Status_LED10)
        Me.gb_Status.Controls.Add(Me.pic_Status_LED11)
        Me.gb_Status.Location = New System.Drawing.Point(283, 9)
        Me.gb_Status.Name = "gb_Status"
        Me.gb_Status.Size = New System.Drawing.Size(419, 225)
        Me.gb_Status.TabIndex = 25
        Me.gb_Status.TabStop = False
        Me.gb_Status.Text = "Status"
        '
        'pic_Status_ASM08
        '
        Me.pic_Status_ASM08.Location = New System.Drawing.Point(340, 87)
        Me.pic_Status_ASM08.Name = "pic_Status_ASM08"
        Me.pic_Status_ASM08.Size = New System.Drawing.Size(13, 14)
        Me.pic_Status_ASM08.TabIndex = 24
        Me.pic_Status_ASM08.TabStop = False
        '
        'pic_Status_ASM07
        '
        Me.pic_Status_ASM07.Location = New System.Drawing.Point(311, 87)
        Me.pic_Status_ASM07.Name = "pic_Status_ASM07"
        Me.pic_Status_ASM07.Size = New System.Drawing.Size(13, 14)
        Me.pic_Status_ASM07.TabIndex = 23
        Me.pic_Status_ASM07.TabStop = False
        '
        'pic_Status_ASM06
        '
        Me.pic_Status_ASM06.Location = New System.Drawing.Point(258, 87)
        Me.pic_Status_ASM06.Name = "pic_Status_ASM06"
        Me.pic_Status_ASM06.Size = New System.Drawing.Size(13, 14)
        Me.pic_Status_ASM06.TabIndex = 22
        Me.pic_Status_ASM06.TabStop = False
        '
        'pic_Status_ASM05
        '
        Me.pic_Status_ASM05.Location = New System.Drawing.Point(228, 87)
        Me.pic_Status_ASM05.Name = "pic_Status_ASM05"
        Me.pic_Status_ASM05.Size = New System.Drawing.Size(13, 14)
        Me.pic_Status_ASM05.TabIndex = 21
        Me.pic_Status_ASM05.TabStop = False
        '
        'pic_Status_ASM04
        '
        Me.pic_Status_ASM04.Location = New System.Drawing.Point(175, 87)
        Me.pic_Status_ASM04.Name = "pic_Status_ASM04"
        Me.pic_Status_ASM04.Size = New System.Drawing.Size(13, 14)
        Me.pic_Status_ASM04.TabIndex = 20
        Me.pic_Status_ASM04.TabStop = False
        '
        'pic_Status_ASM03
        '
        Me.pic_Status_ASM03.Location = New System.Drawing.Point(146, 87)
        Me.pic_Status_ASM03.Name = "pic_Status_ASM03"
        Me.pic_Status_ASM03.Size = New System.Drawing.Size(13, 14)
        Me.pic_Status_ASM03.TabIndex = 19
        Me.pic_Status_ASM03.TabStop = False
        '
        'pic_Status_ASM02
        '
        Me.pic_Status_ASM02.Location = New System.Drawing.Point(93, 87)
        Me.pic_Status_ASM02.Name = "pic_Status_ASM02"
        Me.pic_Status_ASM02.Size = New System.Drawing.Size(13, 14)
        Me.pic_Status_ASM02.TabIndex = 18
        Me.pic_Status_ASM02.TabStop = False
        '
        'pic_Status_ASM01
        '
        Me.pic_Status_ASM01.Location = New System.Drawing.Point(63, 87)
        Me.pic_Status_ASM01.Name = "pic_Status_ASM01"
        Me.pic_Status_ASM01.Size = New System.Drawing.Size(13, 14)
        Me.pic_Status_ASM01.TabIndex = 17
        Me.pic_Status_ASM01.TabStop = False
        '
        'pic_Status_Decoder
        '
        Me.pic_Status_Decoder.Location = New System.Drawing.Point(44, 54)
        Me.pic_Status_Decoder.Name = "pic_Status_Decoder"
        Me.pic_Status_Decoder.Size = New System.Drawing.Size(346, 164)
        Me.pic_Status_Decoder.TabIndex = 16
        Me.pic_Status_Decoder.TabStop = False
        '
        'pic_Status_LED19
        '
        Me.pic_Status_LED19.Location = New System.Drawing.Point(354, 18)
        Me.pic_Status_LED19.Name = "pic_Status_LED19"
        Me.pic_Status_LED19.Size = New System.Drawing.Size(10, 50)
        Me.pic_Status_LED19.TabIndex = 7
        Me.pic_Status_LED19.TabStop = False
        '
        'pic_Status_LED17
        '
        Me.pic_Status_LED17.Location = New System.Drawing.Point(323, 18)
        Me.pic_Status_LED17.Name = "pic_Status_LED17"
        Me.pic_Status_LED17.Size = New System.Drawing.Size(10, 50)
        Me.pic_Status_LED17.TabIndex = 6
        Me.pic_Status_LED17.TabStop = False
        '
        'pic_Status_LED14
        '
        Me.pic_Status_LED14.Location = New System.Drawing.Point(275, 18)
        Me.pic_Status_LED14.Name = "pic_Status_LED14"
        Me.pic_Status_LED14.Size = New System.Drawing.Size(10, 50)
        Me.pic_Status_LED14.TabIndex = 5
        Me.pic_Status_LED14.TabStop = False
        '
        'pic_Status_LED12
        '
        Me.pic_Status_LED12.Location = New System.Drawing.Point(243, 18)
        Me.pic_Status_LED12.Name = "pic_Status_LED12"
        Me.pic_Status_LED12.Size = New System.Drawing.Size(10, 50)
        Me.pic_Status_LED12.TabIndex = 4
        Me.pic_Status_LED12.TabStop = False
        '
        'pic_Status_LED08
        '
        Me.pic_Status_LED08.Location = New System.Drawing.Point(180, 18)
        Me.pic_Status_LED08.Name = "pic_Status_LED08"
        Me.pic_Status_LED08.Size = New System.Drawing.Size(10, 50)
        Me.pic_Status_LED08.TabIndex = 3
        Me.pic_Status_LED08.TabStop = False
        '
        'pic_Status_LED06
        '
        Me.pic_Status_LED06.Location = New System.Drawing.Point(148, 18)
        Me.pic_Status_LED06.Name = "pic_Status_LED06"
        Me.pic_Status_LED06.Size = New System.Drawing.Size(10, 50)
        Me.pic_Status_LED06.TabIndex = 2
        Me.pic_Status_LED06.TabStop = False
        '
        'pic_Status_LED03
        '
        Me.pic_Status_LED03.Location = New System.Drawing.Point(100, 18)
        Me.pic_Status_LED03.Name = "pic_Status_LED03"
        Me.pic_Status_LED03.Size = New System.Drawing.Size(10, 50)
        Me.pic_Status_LED03.TabIndex = 1
        Me.pic_Status_LED03.TabStop = False
        '
        'pic_Status_LED01
        '
        Me.pic_Status_LED01.Location = New System.Drawing.Point(68, 18)
        Me.pic_Status_LED01.Name = "pic_Status_LED01"
        Me.pic_Status_LED01.Size = New System.Drawing.Size(10, 50)
        Me.pic_Status_LED01.TabIndex = 0
        Me.pic_Status_LED01.TabStop = False
        '
        'pic_Status_LED18
        '
        Me.pic_Status_LED18.Location = New System.Drawing.Point(339, 18)
        Me.pic_Status_LED18.Name = "pic_Status_LED18"
        Me.pic_Status_LED18.Size = New System.Drawing.Size(10, 50)
        Me.pic_Status_LED18.TabIndex = 32
        Me.pic_Status_LED18.TabStop = False
        '
        'pic_Status_LED16
        '
        Me.pic_Status_LED16.Location = New System.Drawing.Point(307, 18)
        Me.pic_Status_LED16.Name = "pic_Status_LED16"
        Me.pic_Status_LED16.Size = New System.Drawing.Size(10, 50)
        Me.pic_Status_LED16.TabIndex = 31
        Me.pic_Status_LED16.TabStop = False
        '
        'pic_Status_LED15
        '
        Me.pic_Status_LED15.Location = New System.Drawing.Point(291, 18)
        Me.pic_Status_LED15.Name = "pic_Status_LED15"
        Me.pic_Status_LED15.Size = New System.Drawing.Size(10, 50)
        Me.pic_Status_LED15.TabIndex = 30
        Me.pic_Status_LED15.TabStop = False
        '
        'pic_Status_LED13
        '
        Me.pic_Status_LED13.Location = New System.Drawing.Point(259, 18)
        Me.pic_Status_LED13.Name = "pic_Status_LED13"
        Me.pic_Status_LED13.Size = New System.Drawing.Size(10, 50)
        Me.pic_Status_LED13.TabIndex = 29
        Me.pic_Status_LED13.TabStop = False
        '
        'pic_Status_LED07
        '
        Me.pic_Status_LED07.Location = New System.Drawing.Point(164, 18)
        Me.pic_Status_LED07.Name = "pic_Status_LED07"
        Me.pic_Status_LED07.Size = New System.Drawing.Size(10, 50)
        Me.pic_Status_LED07.TabIndex = 28
        Me.pic_Status_LED07.TabStop = False
        '
        'pic_Status_LED05
        '
        Me.pic_Status_LED05.Location = New System.Drawing.Point(132, 18)
        Me.pic_Status_LED05.Name = "pic_Status_LED05"
        Me.pic_Status_LED05.Size = New System.Drawing.Size(10, 50)
        Me.pic_Status_LED05.TabIndex = 27
        Me.pic_Status_LED05.TabStop = False
        '
        'pic_Status_LED04
        '
        Me.pic_Status_LED04.Location = New System.Drawing.Point(116, 18)
        Me.pic_Status_LED04.Name = "pic_Status_LED04"
        Me.pic_Status_LED04.Size = New System.Drawing.Size(10, 50)
        Me.pic_Status_LED04.TabIndex = 26
        Me.pic_Status_LED04.TabStop = False
        '
        'pic_Status_LED02
        '
        Me.pic_Status_LED02.Location = New System.Drawing.Point(84, 18)
        Me.pic_Status_LED02.Name = "pic_Status_LED02"
        Me.pic_Status_LED02.Size = New System.Drawing.Size(10, 50)
        Me.pic_Status_LED02.TabIndex = 25
        Me.pic_Status_LED02.TabStop = False
        '
        'pic_Status_LED09
        '
        Me.pic_Status_LED09.Location = New System.Drawing.Point(196, 18)
        Me.pic_Status_LED09.Name = "pic_Status_LED09"
        Me.pic_Status_LED09.Size = New System.Drawing.Size(10, 50)
        Me.pic_Status_LED09.TabIndex = 33
        Me.pic_Status_LED09.TabStop = False
        '
        'pic_Status_LED10
        '
        Me.pic_Status_LED10.Location = New System.Drawing.Point(212, 18)
        Me.pic_Status_LED10.Name = "pic_Status_LED10"
        Me.pic_Status_LED10.Size = New System.Drawing.Size(10, 51)
        Me.pic_Status_LED10.TabIndex = 34
        Me.pic_Status_LED10.TabStop = False
        '
        'pic_Status_LED11
        '
        Me.pic_Status_LED11.Location = New System.Drawing.Point(227, 18)
        Me.pic_Status_LED11.Name = "pic_Status_LED11"
        Me.pic_Status_LED11.Size = New System.Drawing.Size(10, 51)
        Me.pic_Status_LED11.TabIndex = 35
        Me.pic_Status_LED11.TabStop = False
        '
        'gb_Parameter
        '
        Me.gb_Parameter.Controls.Add(Me.lbl_ParameterHW)
        Me.gb_Parameter.Controls.Add(Me.txt_ParameterHW)
        Me.gb_Parameter.Controls.Add(Me.txt_ParameterSW)
        Me.gb_Parameter.Controls.Add(Me.lbl_ParameterSW)
        Me.gb_Parameter.Controls.Add(Me.lbl_ParameterName)
        Me.gb_Parameter.Controls.Add(Me.txt_ParameterName)
        Me.gb_Parameter.Controls.Add(Me.lbl_ParameterEinheitTemperatur)
        Me.gb_Parameter.Controls.Add(Me.txt_ParameterArtNum)
        Me.gb_Parameter.Controls.Add(Me.txt_ParameterTemperatur)
        Me.gb_Parameter.Controls.Add(Me.lbl_ParameterEinheitSpannung)
        Me.gb_Parameter.Controls.Add(Me.lbl_ParameterTemperatur)
        Me.gb_Parameter.Controls.Add(Me.lbl_ParameterArtNum)
        Me.gb_Parameter.Controls.Add(Me.txt_ParameterPlugged)
        Me.gb_Parameter.Controls.Add(Me.txt_ParameterSpannung)
        Me.gb_Parameter.Controls.Add(Me.lbl_ParameterPlugged)
        Me.gb_Parameter.Controls.Add(Me.lbl_ParameterSpannung)
        Me.gb_Parameter.Controls.Add(Me.lbl_ParameterGUID)
        Me.gb_Parameter.Controls.Add(Me.txt_ParameterGUID)
        Me.gb_Parameter.Controls.Add(Me.txt_ParameterSerNr)
        Me.gb_Parameter.Controls.Add(Me.txt_ParameterDecTyp)
        Me.gb_Parameter.Controls.Add(Me.lbl_ParameterSerNr)
        Me.gb_Parameter.Controls.Add(Me.lbl_ParameterDecTyp)
        Me.gb_Parameter.Location = New System.Drawing.Point(14, 9)
        Me.gb_Parameter.Name = "gb_Parameter"
        Me.gb_Parameter.Size = New System.Drawing.Size(263, 271)
        Me.gb_Parameter.TabIndex = 24
        Me.gb_Parameter.TabStop = False
        Me.gb_Parameter.Text = "Parameter"
        '
        'lbl_ParameterHW
        '
        Me.lbl_ParameterHW.AutoSize = True
        Me.lbl_ParameterHW.Location = New System.Drawing.Point(8, 164)
        Me.lbl_ParameterHW.Name = "lbl_ParameterHW"
        Me.lbl_ParameterHW.Size = New System.Drawing.Size(67, 13)
        Me.lbl_ParameterHW.TabIndex = 51
        Me.lbl_ParameterHW.Text = "HW-Version:"
        '
        'txt_ParameterHW
        '
        Me.txt_ParameterHW.Location = New System.Drawing.Point(112, 161)
        Me.txt_ParameterHW.Name = "txt_ParameterHW"
        Me.txt_ParameterHW.ReadOnly = True
        Me.txt_ParameterHW.Size = New System.Drawing.Size(136, 20)
        Me.txt_ParameterHW.TabIndex = 50
        '
        'txt_ParameterSW
        '
        Me.txt_ParameterSW.Location = New System.Drawing.Point(112, 137)
        Me.txt_ParameterSW.Name = "txt_ParameterSW"
        Me.txt_ParameterSW.ReadOnly = True
        Me.txt_ParameterSW.Size = New System.Drawing.Size(136, 20)
        Me.txt_ParameterSW.TabIndex = 49
        '
        'lbl_ParameterSW
        '
        Me.lbl_ParameterSW.AutoSize = True
        Me.lbl_ParameterSW.Location = New System.Drawing.Point(8, 140)
        Me.lbl_ParameterSW.Name = "lbl_ParameterSW"
        Me.lbl_ParameterSW.Size = New System.Drawing.Size(66, 13)
        Me.lbl_ParameterSW.TabIndex = 48
        Me.lbl_ParameterSW.Text = "SW-Version:"
        '
        'lbl_ParameterName
        '
        Me.lbl_ParameterName.AutoSize = True
        Me.lbl_ParameterName.Location = New System.Drawing.Point(8, 20)
        Me.lbl_ParameterName.Name = "lbl_ParameterName"
        Me.lbl_ParameterName.Size = New System.Drawing.Size(38, 13)
        Me.lbl_ParameterName.TabIndex = 47
        Me.lbl_ParameterName.Text = "Name:"
        '
        'txt_ParameterName
        '
        Me.txt_ParameterName.Location = New System.Drawing.Point(112, 17)
        Me.txt_ParameterName.Name = "txt_ParameterName"
        Me.txt_ParameterName.ReadOnly = True
        Me.txt_ParameterName.Size = New System.Drawing.Size(136, 20)
        Me.txt_ParameterName.TabIndex = 46
        '
        'lbl_ParameterEinheitTemperatur
        '
        Me.lbl_ParameterEinheitTemperatur.AutoSize = True
        Me.lbl_ParameterEinheitTemperatur.Location = New System.Drawing.Point(176, 235)
        Me.lbl_ParameterEinheitTemperatur.Name = "lbl_ParameterEinheitTemperatur"
        Me.lbl_ParameterEinheitTemperatur.Size = New System.Drawing.Size(24, 13)
        Me.lbl_ParameterEinheitTemperatur.TabIndex = 8
        Me.lbl_ParameterEinheitTemperatur.Text = "[°C]"
        '
        'txt_ParameterArtNum
        '
        Me.txt_ParameterArtNum.Location = New System.Drawing.Point(112, 65)
        Me.txt_ParameterArtNum.Name = "txt_ParameterArtNum"
        Me.txt_ParameterArtNum.ReadOnly = True
        Me.txt_ParameterArtNum.Size = New System.Drawing.Size(136, 20)
        Me.txt_ParameterArtNum.TabIndex = 45
        '
        'txt_ParameterTemperatur
        '
        Me.txt_ParameterTemperatur.BackColor = System.Drawing.SystemColors.Control
        Me.txt_ParameterTemperatur.Location = New System.Drawing.Point(112, 233)
        Me.txt_ParameterTemperatur.Name = "txt_ParameterTemperatur"
        Me.txt_ParameterTemperatur.ReadOnly = True
        Me.txt_ParameterTemperatur.Size = New System.Drawing.Size(53, 20)
        Me.txt_ParameterTemperatur.TabIndex = 5
        '
        'lbl_ParameterEinheitSpannung
        '
        Me.lbl_ParameterEinheitSpannung.AutoSize = True
        Me.lbl_ParameterEinheitSpannung.Location = New System.Drawing.Point(177, 212)
        Me.lbl_ParameterEinheitSpannung.Name = "lbl_ParameterEinheitSpannung"
        Me.lbl_ParameterEinheitSpannung.Size = New System.Drawing.Size(20, 13)
        Me.lbl_ParameterEinheitSpannung.TabIndex = 6
        Me.lbl_ParameterEinheitSpannung.Text = "[V]"
        '
        'lbl_ParameterTemperatur
        '
        Me.lbl_ParameterTemperatur.AutoSize = True
        Me.lbl_ParameterTemperatur.Location = New System.Drawing.Point(8, 236)
        Me.lbl_ParameterTemperatur.Name = "lbl_ParameterTemperatur"
        Me.lbl_ParameterTemperatur.Size = New System.Drawing.Size(64, 13)
        Me.lbl_ParameterTemperatur.TabIndex = 2
        Me.lbl_ParameterTemperatur.Text = "Temperatur:"
        '
        'lbl_ParameterArtNum
        '
        Me.lbl_ParameterArtNum.AutoSize = True
        Me.lbl_ParameterArtNum.Location = New System.Drawing.Point(8, 68)
        Me.lbl_ParameterArtNum.Name = "lbl_ParameterArtNum"
        Me.lbl_ParameterArtNum.Size = New System.Drawing.Size(76, 13)
        Me.lbl_ParameterArtNum.TabIndex = 44
        Me.lbl_ParameterArtNum.Text = "Artikelnummer:"
        '
        'txt_ParameterPlugged
        '
        Me.txt_ParameterPlugged.Location = New System.Drawing.Point(112, 185)
        Me.txt_ParameterPlugged.Name = "txt_ParameterPlugged"
        Me.txt_ParameterPlugged.ReadOnly = True
        Me.txt_ParameterPlugged.Size = New System.Drawing.Size(136, 20)
        Me.txt_ParameterPlugged.TabIndex = 43
        '
        'txt_ParameterSpannung
        '
        Me.txt_ParameterSpannung.BackColor = System.Drawing.SystemColors.Control
        Me.txt_ParameterSpannung.Location = New System.Drawing.Point(112, 209)
        Me.txt_ParameterSpannung.Name = "txt_ParameterSpannung"
        Me.txt_ParameterSpannung.ReadOnly = True
        Me.txt_ParameterSpannung.Size = New System.Drawing.Size(53, 20)
        Me.txt_ParameterSpannung.TabIndex = 3
        '
        'lbl_ParameterPlugged
        '
        Me.lbl_ParameterPlugged.AutoSize = True
        Me.lbl_ParameterPlugged.Location = New System.Drawing.Point(8, 188)
        Me.lbl_ParameterPlugged.Name = "lbl_ParameterPlugged"
        Me.lbl_ParameterPlugged.Size = New System.Drawing.Size(49, 13)
        Me.lbl_ParameterPlugged.TabIndex = 9
        Me.lbl_ParameterPlugged.Text = "Plugged:"
        '
        'lbl_ParameterSpannung
        '
        Me.lbl_ParameterSpannung.AutoSize = True
        Me.lbl_ParameterSpannung.Location = New System.Drawing.Point(8, 212)
        Me.lbl_ParameterSpannung.Name = "lbl_ParameterSpannung"
        Me.lbl_ParameterSpannung.Size = New System.Drawing.Size(59, 13)
        Me.lbl_ParameterSpannung.TabIndex = 0
        Me.lbl_ParameterSpannung.Text = "Spannung:"
        '
        'lbl_ParameterGUID
        '
        Me.lbl_ParameterGUID.AutoSize = True
        Me.lbl_ParameterGUID.Location = New System.Drawing.Point(8, 116)
        Me.lbl_ParameterGUID.Name = "lbl_ParameterGUID"
        Me.lbl_ParameterGUID.Size = New System.Drawing.Size(63, 13)
        Me.lbl_ParameterGUID.TabIndex = 36
        Me.lbl_ParameterGUID.Text = "Geräte-UiD:"
        '
        'txt_ParameterGUID
        '
        Me.txt_ParameterGUID.Location = New System.Drawing.Point(112, 113)
        Me.txt_ParameterGUID.Name = "txt_ParameterGUID"
        Me.txt_ParameterGUID.ReadOnly = True
        Me.txt_ParameterGUID.Size = New System.Drawing.Size(136, 20)
        Me.txt_ParameterGUID.TabIndex = 35
        '
        'txt_ParameterSerNr
        '
        Me.txt_ParameterSerNr.Location = New System.Drawing.Point(112, 89)
        Me.txt_ParameterSerNr.Name = "txt_ParameterSerNr"
        Me.txt_ParameterSerNr.ReadOnly = True
        Me.txt_ParameterSerNr.Size = New System.Drawing.Size(136, 20)
        Me.txt_ParameterSerNr.TabIndex = 31
        '
        'txt_ParameterDecTyp
        '
        Me.txt_ParameterDecTyp.Location = New System.Drawing.Point(112, 41)
        Me.txt_ParameterDecTyp.Name = "txt_ParameterDecTyp"
        Me.txt_ParameterDecTyp.ReadOnly = True
        Me.txt_ParameterDecTyp.Size = New System.Drawing.Size(136, 20)
        Me.txt_ParameterDecTyp.TabIndex = 29
        '
        'lbl_ParameterSerNr
        '
        Me.lbl_ParameterSerNr.AutoSize = True
        Me.lbl_ParameterSerNr.Location = New System.Drawing.Point(8, 92)
        Me.lbl_ParameterSerNr.Name = "lbl_ParameterSerNr"
        Me.lbl_ParameterSerNr.Size = New System.Drawing.Size(77, 13)
        Me.lbl_ParameterSerNr.TabIndex = 24
        Me.lbl_ParameterSerNr.Text = "Seriennummer:"
        '
        'lbl_ParameterDecTyp
        '
        Me.lbl_ParameterDecTyp.AutoSize = True
        Me.lbl_ParameterDecTyp.Location = New System.Drawing.Point(8, 44)
        Me.lbl_ParameterDecTyp.Name = "lbl_ParameterDecTyp"
        Me.lbl_ParameterDecTyp.Size = New System.Drawing.Size(28, 13)
        Me.lbl_ParameterDecTyp.TabIndex = 23
        Me.lbl_ParameterDecTyp.Text = "Typ:"
        '
        'tab_Monitor
        '
        Me.tab_Monitor.Controls.Add(Me.rtb_MonitorReceived)
        Me.tab_Monitor.Controls.Add(Me.lbl_MonitorNachrichtenherkunft)
        Me.tab_Monitor.Controls.Add(Me.lbl_MonitorInhalt)
        Me.tab_Monitor.Location = New System.Drawing.Point(4, 22)
        Me.tab_Monitor.Name = "tab_Monitor"
        Me.tab_Monitor.Padding = New System.Windows.Forms.Padding(3)
        Me.tab_Monitor.Size = New System.Drawing.Size(704, 610)
        Me.tab_Monitor.TabIndex = 2
        Me.tab_Monitor.Text = "Monitor"
        Me.tab_Monitor.UseVisualStyleBackColor = True
        '
        'rtb_MonitorReceived
        '
        Me.rtb_MonitorReceived.BackColor = System.Drawing.Color.White
        Me.rtb_MonitorReceived.Font = New System.Drawing.Font("Courier New", 8.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.rtb_MonitorReceived.Location = New System.Drawing.Point(19, 46)
        Me.rtb_MonitorReceived.Name = "rtb_MonitorReceived"
        Me.rtb_MonitorReceived.ReadOnly = True
        Me.rtb_MonitorReceived.ScrollBars = System.Windows.Forms.RichTextBoxScrollBars.ForcedVertical
        Me.rtb_MonitorReceived.Size = New System.Drawing.Size(667, 526)
        Me.rtb_MonitorReceived.TabIndex = 31
        Me.rtb_MonitorReceived.Text = ""
        '
        'lbl_MonitorNachrichtenherkunft
        '
        Me.lbl_MonitorNachrichtenherkunft.AutoSize = True
        Me.lbl_MonitorNachrichtenherkunft.Location = New System.Drawing.Point(129, 14)
        Me.lbl_MonitorNachrichtenherkunft.Name = "lbl_MonitorNachrichtenherkunft"
        Me.lbl_MonitorNachrichtenherkunft.Size = New System.Drawing.Size(242, 13)
        Me.lbl_MonitorNachrichtenherkunft.TabIndex = 30
        Me.lbl_MonitorNachrichtenherkunft.Text = "CAN-Telegramme auf dem Märklin-Bus via RS232"
        '
        'lbl_MonitorInhalt
        '
        Me.lbl_MonitorInhalt.AutoSize = True
        Me.lbl_MonitorInhalt.Location = New System.Drawing.Point(16, 14)
        Me.lbl_MonitorInhalt.Name = "lbl_MonitorInhalt"
        Me.lbl_MonitorInhalt.Size = New System.Drawing.Size(93, 13)
        Me.lbl_MonitorInhalt.TabIndex = 29
        Me.lbl_MonitorInhalt.Text = "Nachrichteninhalt:"
        '
        'tab_Adressen
        '
        Me.tab_Adressen.Controls.Add(Me.btn_AdressenS88Base)
        Me.tab_Adressen.Controls.Add(Me.lbl_AdressenS88Base)
        Me.tab_Adressen.Controls.Add(Me.dgv_AdressenS88Base)
        Me.tab_Adressen.Controls.Add(Me.dgv_AdressenLocID)
        Me.tab_Adressen.Controls.Add(Me.btn_AdressenLocID)
        Me.tab_Adressen.Controls.Add(Me.lbl_AdressenLocID)
        Me.tab_Adressen.Location = New System.Drawing.Point(4, 22)
        Me.tab_Adressen.Name = "tab_Adressen"
        Me.tab_Adressen.Padding = New System.Windows.Forms.Padding(3)
        Me.tab_Adressen.Size = New System.Drawing.Size(704, 610)
        Me.tab_Adressen.TabIndex = 3
        Me.tab_Adressen.Text = "Adressen"
        Me.tab_Adressen.UseVisualStyleBackColor = True
        '
        'btn_AdressenS88Base
        '
        Me.btn_AdressenS88Base.Location = New System.Drawing.Point(553, 35)
        Me.btn_AdressenS88Base.Name = "btn_AdressenS88Base"
        Me.btn_AdressenS88Base.Size = New System.Drawing.Size(126, 23)
        Me.btn_AdressenS88Base.TabIndex = 35
        Me.btn_AdressenS88Base.Text = "Übersicht generieren"
        Me.btn_AdressenS88Base.UseVisualStyleBackColor = True
        '
        'lbl_AdressenS88Base
        '
        Me.lbl_AdressenS88Base.AutoSize = True
        Me.lbl_AdressenS88Base.Location = New System.Drawing.Point(362, 40)
        Me.lbl_AdressenS88Base.Name = "lbl_AdressenS88Base"
        Me.lbl_AdressenS88Base.Size = New System.Drawing.Size(171, 13)
        Me.lbl_AdressenS88Base.TabIndex = 34
        Me.lbl_AdressenS88Base.Text = "Zuordnung Module zu Rückmelder"
        '
        'dgv_AdressenS88Base
        '
        Me.dgv_AdressenS88Base.AllowUserToAddRows = False
        Me.dgv_AdressenS88Base.AllowUserToDeleteRows = False
        Me.dgv_AdressenS88Base.AllowUserToResizeColumns = False
        Me.dgv_AdressenS88Base.AllowUserToResizeRows = False
        Me.dgv_AdressenS88Base.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
        Me.dgv_AdressenS88Base.Columns.AddRange(New System.Windows.Forms.DataGridViewColumn() {Me.S88ColumnModNr, Me.S88ColumnS88Typ, Me.S88ColumnS88Name, Me.S88ColumnS88Index})
        Me.dgv_AdressenS88Base.Location = New System.Drawing.Point(348, 87)
        Me.dgv_AdressenS88Base.Name = "dgv_AdressenS88Base"
        Me.dgv_AdressenS88Base.ReadOnly = True
        Me.dgv_AdressenS88Base.RowHeadersWidth = 62
        Me.dgv_AdressenS88Base.ScrollBars = System.Windows.Forms.ScrollBars.Vertical
        Me.dgv_AdressenS88Base.Size = New System.Drawing.Size(357, 504)
        Me.dgv_AdressenS88Base.TabIndex = 33
        '
        'S88ColumnModNr
        '
        Me.S88ColumnModNr.HeaderText = "Mod-Nr"
        Me.S88ColumnModNr.MinimumWidth = 8
        Me.S88ColumnModNr.Name = "S88ColumnModNr"
        Me.S88ColumnModNr.ReadOnly = True
        Me.S88ColumnModNr.Width = 70
        '
        'S88ColumnS88Typ
        '
        Me.S88ColumnS88Typ.HeaderText = "Typ"
        Me.S88ColumnS88Typ.MinimumWidth = 8
        Me.S88ColumnS88Typ.Name = "S88ColumnS88Typ"
        Me.S88ColumnS88Typ.ReadOnly = True
        Me.S88ColumnS88Typ.Width = 45
        '
        'S88ColumnS88Name
        '
        Me.S88ColumnS88Name.HeaderText = "Name"
        Me.S88ColumnS88Name.MinimumWidth = 8
        Me.S88ColumnS88Name.Name = "S88ColumnS88Name"
        Me.S88ColumnS88Name.ReadOnly = True
        Me.S88ColumnS88Name.Width = 140
        '
        'S88ColumnS88Index
        '
        Me.S88ColumnS88Index.HeaderText = "Index"
        Me.S88ColumnS88Index.MinimumWidth = 8
        Me.S88ColumnS88Index.Name = "S88ColumnS88Index"
        Me.S88ColumnS88Index.ReadOnly = True
        Me.S88ColumnS88Index.Width = 40
        '
        'dgv_AdressenLocID
        '
        Me.dgv_AdressenLocID.AllowUserToAddRows = False
        Me.dgv_AdressenLocID.AllowUserToDeleteRows = False
        Me.dgv_AdressenLocID.AllowUserToResizeColumns = False
        Me.dgv_AdressenLocID.AllowUserToResizeRows = False
        Me.dgv_AdressenLocID.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
        Me.dgv_AdressenLocID.Columns.AddRange(New System.Windows.Forms.DataGridViewColumn() {Me.MMColumnLocID, Me.MMColumnLocIDTyp, Me.MMColumnLocIDName, Me.MMColumnLocIDIndex})
        Me.dgv_AdressenLocID.Location = New System.Drawing.Point(4, 87)
        Me.dgv_AdressenLocID.Name = "dgv_AdressenLocID"
        Me.dgv_AdressenLocID.ReadOnly = True
        Me.dgv_AdressenLocID.RowHeadersWidth = 62
        Me.dgv_AdressenLocID.ScrollBars = System.Windows.Forms.ScrollBars.Vertical
        Me.dgv_AdressenLocID.Size = New System.Drawing.Size(340, 504)
        Me.dgv_AdressenLocID.TabIndex = 32
        '
        'MMColumnLocID
        '
        Me.MMColumnLocID.HeaderText = "Loc-ID"
        Me.MMColumnLocID.MinimumWidth = 8
        Me.MMColumnLocID.Name = "MMColumnLocID"
        Me.MMColumnLocID.ReadOnly = True
        Me.MMColumnLocID.Width = 50
        '
        'MMColumnLocIDTyp
        '
        Me.MMColumnLocIDTyp.HeaderText = "Typ"
        Me.MMColumnLocIDTyp.MinimumWidth = 8
        Me.MMColumnLocIDTyp.Name = "MMColumnLocIDTyp"
        Me.MMColumnLocIDTyp.ReadOnly = True
        Me.MMColumnLocIDTyp.Width = 45
        '
        'MMColumnLocIDName
        '
        Me.MMColumnLocIDName.HeaderText = "Name"
        Me.MMColumnLocIDName.MinimumWidth = 8
        Me.MMColumnLocIDName.Name = "MMColumnLocIDName"
        Me.MMColumnLocIDName.ReadOnly = True
        Me.MMColumnLocIDName.Width = 140
        '
        'MMColumnLocIDIndex
        '
        Me.MMColumnLocIDIndex.HeaderText = "Index"
        Me.MMColumnLocIDIndex.MinimumWidth = 8
        Me.MMColumnLocIDIndex.Name = "MMColumnLocIDIndex"
        Me.MMColumnLocIDIndex.ReadOnly = True
        Me.MMColumnLocIDIndex.Width = 40
        '
        'btn_AdressenLocID
        '
        Me.btn_AdressenLocID.Location = New System.Drawing.Point(190, 35)
        Me.btn_AdressenLocID.Name = "btn_AdressenLocID"
        Me.btn_AdressenLocID.Size = New System.Drawing.Size(126, 23)
        Me.btn_AdressenLocID.TabIndex = 31
        Me.btn_AdressenLocID.Text = "Übersicht generieren"
        Me.btn_AdressenLocID.UseVisualStyleBackColor = True
        '
        'lbl_AdressenLocID
        '
        Me.lbl_AdressenLocID.AutoSize = True
        Me.lbl_AdressenLocID.Location = New System.Drawing.Point(16, 40)
        Me.lbl_AdressenLocID.Name = "lbl_AdressenLocID"
        Me.lbl_AdressenLocID.Size = New System.Drawing.Size(146, 13)
        Me.lbl_AdressenLocID.TabIndex = 30
        Me.lbl_AdressenLocID.Text = "Zuordnung Module zu Loc-ID"
        '
        'stp_Status
        '
        Me.stp_Status.Dock = System.Windows.Forms.DockStyle.Fill
        Me.stp_Status.GripStyle = System.Windows.Forms.ToolStripGripStyle.Visible
        Me.stp_Status.ImageScalingSize = New System.Drawing.Size(24, 24)
        Me.stp_Status.Items.AddRange(New System.Windows.Forms.ToolStripItem() {Me.stp_RS232, Me.stp_BT, Me.stp_TCP, Me.stp_CSx, Me.stp_Refresh, Me.spb_Refresh, Me.stp_Alive, Me.stp_Upgrade})
        Me.stp_Status.Location = New System.Drawing.Point(7, 662)
        Me.stp_Status.Name = "stp_Status"
        Me.stp_Status.Size = New System.Drawing.Size(924, 6)
        Me.stp_Status.SizingGrip = False
        Me.stp_Status.TabIndex = 2
        Me.stp_Status.TabStop = True
        Me.stp_Status.Text = "StatusStrip"
        '
        'stp_RS232
        '
        Me.stp_RS232.BorderSides = System.Windows.Forms.ToolStripStatusLabelBorderSides.Right
        Me.stp_RS232.Name = "stp_RS232"
        Me.stp_RS232.Size = New System.Drawing.Size(77, 1)
        Me.stp_RS232.Text = "RS232: None"
        '
        'stp_BT
        '
        Me.stp_BT.BorderSides = System.Windows.Forms.ToolStripStatusLabelBorderSides.Right
        Me.stp_BT.Name = "stp_BT"
        Me.stp_BT.Size = New System.Drawing.Size(58, 1)
        Me.stp_BT.Text = "BT: None"
        '
        'stp_TCP
        '
        Me.stp_TCP.BorderSides = System.Windows.Forms.ToolStripStatusLabelBorderSides.Right
        Me.stp_TCP.Name = "stp_TCP"
        Me.stp_TCP.Size = New System.Drawing.Size(66, 1)
        Me.stp_TCP.Text = "TCP: None"
        '
        'stp_Refresh
        '
        Me.stp_Refresh.Name = "stp_Refresh"
        Me.stp_Refresh.Size = New System.Drawing.Size(76, 1)
        Me.stp_Refresh.Text = "Refresh: Stop"
        '
        'spb_Refresh
        '
        Me.spb_Refresh.Name = "spb_Refresh"
        Me.spb_Refresh.Size = New System.Drawing.Size(100, 1)
        '
        'stp_Alive
        '
        Me.stp_Alive.BorderSides = CType((System.Windows.Forms.ToolStripStatusLabelBorderSides.Left Or System.Windows.Forms.ToolStripStatusLabelBorderSides.Right), System.Windows.Forms.ToolStripStatusLabelBorderSides)
        Me.stp_Alive.Name = "stp_Alive"
        Me.stp_Alive.Size = New System.Drawing.Size(230, 1)
        Me.stp_Alive.Text = "Alive: Modultyp mbcxx Modulnummer: 0"
        '
        'stp_CSx
        '
        Me.stp_CSx.BorderSides = System.Windows.Forms.ToolStripStatusLabelBorderSides.Right
        Me.stp_CSx.Name = "stp_CSx"
        Me.stp_CSx.Size = New System.Drawing.Size(90, 1)
        Me.stp_CSx.Text = "CSx: absent"
        '
        'stp_Upgrade
        '
        Me.stp_Upgrade.Name = "stp_Upgrade"
        Me.stp_Upgrade.Size = New System.Drawing.Size(45, 1)
        Me.stp_Upgrade.Text = "0 von 0"
        '
        'tm_Neu
        '
        Me.tm_Neu.Interval = 10000
        '
        'tm_Alive
        '
        Me.tm_Alive.Interval = 1000
        '
        'tm_ComTimeout
        '
        Me.tm_ComTimeout.Interval = 1500
        '
        'ofd_Oeffnen
        '
        Me.ofd_Oeffnen.DefaultExt = "hex"
        Me.ofd_Oeffnen.FileName = "*.hex"
        '
        'ofd_OeffnenBin
        '
        Me.ofd_OeffnenBin.DefaultExt = "bin"
        Me.ofd_OeffnenBin.FileName = "*.bin"
        Me.ofd_OeffnenBin.Filter = "bin files (*.bin)|*.bin|All files (*.*)|*.*"
        '
        'tm_Update
        '
        Me.tm_Update.Interval = 1000
        '
        'frmMain
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.AutoScroll = True
        Me.AutoSize = True
        Me.ClientSize = New System.Drawing.Size(938, 668)
        Me.Controls.Add(Me.stp_Status)
        Me.Controls.Add(Me.sc_Main)
        Me.Controls.Add(Me.ms_Menu)
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle
        Me.Icon = CType(resources.GetObject("$this.Icon"), System.Drawing.Icon)
        Me.MainMenuStrip = Me.ms_Menu
        Me.MaximumSize = New System.Drawing.Size(958, 718)
        Me.Name = "frmMain"
        Me.Padding = New System.Windows.Forms.Padding(7, 0, 7, 0)
        Me.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen
        Me.Text = "MBCAN - Parametriercenter für mbc-Module"
        Me.ms_Menu.ResumeLayout(False)
        Me.ms_Menu.PerformLayout()
        Me.sc_Main.Panel1.ResumeLayout(False)
        Me.sc_Main.Panel2.ResumeLayout(False)
        CType(Me.sc_Main, System.ComponentModel.ISupportInitialize).EndInit()
        Me.sc_Main.ResumeLayout(False)
        Me.tab_Control.ResumeLayout(False)
        Me.tab_Modul.ResumeLayout(False)
        Me.tab_Module.ResumeLayout(False)
        Me.tab_mbc80.ResumeLayout(False)
        Me.gb_mbc80_SW.ResumeLayout(False)
        Me.gb_mbc80_SW.PerformLayout()
        CType(Me.nud_mbc80_SW2, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_mbc80_SW1, System.ComponentModel.ISupportInitialize).EndInit()
        Me.gb_mbc80_AK.ResumeLayout(False)
        Me.gb_mbc80_AK.PerformLayout()
        Me.gb_mbc80_BT.ResumeLayout(False)
        Me.gb_mbc80_BT.PerformLayout()
        Me.tab_mbc81.ResumeLayout(False)
        Me.tab_mbc81.PerformLayout()
        CType(Me.nud_mbc81_LocID, System.ComponentModel.ISupportInitialize).EndInit()
        Me.tab_mbc82.ResumeLayout(False)
        Me.gb_mbc82_Latenz.ResumeLayout(False)
        Me.gb_mbc82_Latenz.PerformLayout()
        CType(Me.nud_mbc82_Latenz, System.ComponentModel.ISupportInitialize).EndInit()
        Me.gb_mbc82_GFP.ResumeLayout(False)
        Me.gb_mbc82_GFP.PerformLayout()
        CType(Me.nud_mbc82_GFPSW2, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_mbc82_GFPSW1, System.ComponentModel.ISupportInitialize).EndInit()
        Me.tab_mbc83.ResumeLayout(False)
        Me.tab_mbc83.PerformLayout()
        CType(Me.pic_mbc83_Startup, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.pic_mbc83_Wechsel, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.pic_mbc83_Impuls, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_mbc83_Rueck, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_mbc83_Aus, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_mbc83_Dimmer, System.ComponentModel.ISupportInitialize).EndInit()
        Me.gb_mbc83_CH4.ResumeLayout(False)
        Me.gb_mbc83_CH4.PerformLayout()
        CType(Me.gb_mbc83_CH4_nud_TIO, System.ComponentModel.ISupportInitialize).EndInit()
        Me.gb_mbc83_CH3.ResumeLayout(False)
        Me.gb_mbc83_CH3.PerformLayout()
        CType(Me.gb_mbc83_CH3_nud_TIO, System.ComponentModel.ISupportInitialize).EndInit()
        Me.gb_mbc83_CH2.ResumeLayout(False)
        Me.gb_mbc83_CH2.PerformLayout()
        CType(Me.gb_mbc83_CH2_nud_TIO, System.ComponentModel.ISupportInitialize).EndInit()
        Me.gb_mbc83_CH1.ResumeLayout(False)
        Me.gb_mbc83_CH1.PerformLayout()
        CType(Me.gb_mbc83_CH1_nud_TIO, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_mbc83_MMAddr, System.ComponentModel.ISupportInitialize).EndInit()
        Me.tab_mbc84.ResumeLayout(False)
        Me.tab_mbc84.PerformLayout()
        CType(Me.nud_mbc84_Rueck, System.ComponentModel.ISupportInitialize).EndInit()
        Me.gb_mbc84_CH4.ResumeLayout(False)
        Me.gb_mbc84_CH4.PerformLayout()
        CType(Me.nud_mbc84_Links_CH4, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_mbc84_Rechts_CH4, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_mbc84_Step_CH4, System.ComponentModel.ISupportInitialize).EndInit()
        Me.gb_mbc84_CH3.ResumeLayout(False)
        Me.gb_mbc84_CH3.PerformLayout()
        CType(Me.nud_mbc84_Links_CH3, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_mbc84_Rechts_CH3, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_mbc84_Step_CH3, System.ComponentModel.ISupportInitialize).EndInit()
        Me.gb_mbc84_CH2.ResumeLayout(False)
        Me.gb_mbc84_CH2.PerformLayout()
        CType(Me.nud_mbc84_Links_CH2, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_mbc84_Rechts_CH2, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_mbc84_Step_CH2, System.ComponentModel.ISupportInitialize).EndInit()
        Me.gb_mbc84_CH1.ResumeLayout(False)
        Me.gb_mbc84_CH1.PerformLayout()
        CType(Me.nud_mbc84_Links_CH1, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_mbc84_Rechts_CH1, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_mbc84_Step_CH1, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_mbc84_MMAddr, System.ComponentModel.ISupportInitialize).EndInit()
        Me.tab_mbc85.ResumeLayout(False)
        Me.tab_mbc85.PerformLayout()
        Me.gb_mbc85_DS_Integrator.ResumeLayout(False)
        Me.gb_mbc85_DS_Integrator.PerformLayout()
        CType(Me.nud_mbc85_T_P11, System.ComponentModel.ISupportInitialize).EndInit()
        Me.gb_mbc85_MMLok.ResumeLayout(False)
        Me.gb_mbc85_MMLok.PerformLayout()
        CType(Me.nud_mbc85_MMSpeed, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_mbc85_MMLok, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_mbc85_S88ModNr, System.ComponentModel.ISupportInitialize).EndInit()
        Me.gb_mbc85_Integrator.ResumeLayout(False)
        Me.gb_mbc85_Integrator.PerformLayout()
        CType(Me.nud_mbc85_T_P05, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_mbc85_T_P06, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_mbc85_T_P04, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_mbc85_T_P08, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_mbc85_T_P03, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_mbc85_T_P07, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_mbc85_T_P02, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_mbc85_T_P10, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_mbc85_T_P01, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_mbc85_T_P09, System.ComponentModel.ISupportInitialize).EndInit()
        Me.tab_mbc86.ResumeLayout(False)
        Me.tab_mbc86.PerformLayout()
        Me.gb_mbc86_MMLok.ResumeLayout(False)
        Me.gb_mbc86_MMLok.PerformLayout()
        CType(Me.nud_mbc86_MMLok, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_mbc86_S88ModNr, System.ComponentModel.ISupportInitialize).EndInit()
        Me.gb_mbc86_Integrator.ResumeLayout(False)
        Me.gb_mbc86_Integrator.PerformLayout()
        CType(Me.nud_mbc86_T_P05, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_mbc86_T_P06, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_mbc86_T_P04, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_mbc86_T_P08, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_mbc86_T_P03, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_mbc86_T_P07, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_mbc86_T_P02, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_mbc86_T_P01, System.ComponentModel.ISupportInitialize).EndInit()
        Me.tab_mbc87.ResumeLayout(False)
        Me.tab_mbc87.PerformLayout()
        CType(Me.nud_mbc87_MMAddr, System.ComponentModel.ISupportInitialize).EndInit()
        Me.tab_mbc88.ResumeLayout(False)
        Me.tab_mbc88.PerformLayout()
        CType(Me.nud_mbc88_T_P16, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_mbc88_T_P12, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_mbc88_T_P08, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_mbc88_T_P04, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_mbc88_T_P15, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_mbc88_T_P11, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_mbc88_T_P07, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_mbc88_T_P03, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_mbc88_T_P14, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_mbc88_T_P10, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_mbc88_T_P06, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_mbc88_T_P02, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_mbc88_T_P13, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_mbc88_T_P09, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_mbc88_T_P05, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_mbc88_T_P01, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_mbc88_S88ModNr, System.ComponentModel.ISupportInitialize).EndInit()
        Me.tab_mbc89.ResumeLayout(False)
        Me.tab_mbc89.PerformLayout()
        Me.gb_mbc89_I2C.ResumeLayout(False)
        Me.gb_mbc89_I2C.PerformLayout()
        CType(Me.nud_mbc89_i2c, System.ComponentModel.ISupportInitialize).EndInit()
        Me.gb_mbc89_Basisadresse.ResumeLayout(False)
        Me.gb_mbc89_Basisadresse.PerformLayout()
        CType(Me.nud_mbc89_basis, System.ComponentModel.ISupportInitialize).EndInit()
        Me.gb_mbc89_Mapping.ResumeLayout(False)
        Me.gb_mbc89_Keyboard.ResumeLayout(False)
        Me.gb_mbc89_Keyboard.PerformLayout()
        CType(Me.nud_mbc89_keyboard_nr, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.pb_mbc89_keyboard, System.ComponentModel.ISupportInitialize).EndInit()
        Me.gb_mbc89_Version.ResumeLayout(False)
        Me.gb_mbc89_Version.PerformLayout()
        CType(Me.nud_mbc89_SW_2, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_mbc89_SW_1, System.ComponentModel.ISupportInitialize).EndInit()
        Me.tab_mbc90.ResumeLayout(False)
        Me.tab_mbc90.PerformLayout()
        CType(Me.nud_mbc90_T_P16, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_mbc90_T_P12, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_mbc90_T_P08, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_mbc90_T_P04, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_mbc90_T_P15, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_mbc90_T_P11, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_mbc90_T_P07, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_mbc90_T_P03, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_mbc90_T_P14, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_mbc90_T_P10, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_mbc90_T_P06, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_mbc90_T_P02, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_mbc90_T_P13, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_mbc90_T_P09, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_mbc90_T_P05, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_mbc90_T_P01, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_mbc90_S88ModNr, System.ComponentModel.ISupportInitialize).EndInit()
        Me.tab_mbc91.ResumeLayout(False)
        Me.tab_mbc91.PerformLayout()
        CType(Me.nud_mbc91_Rueck, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_mbc91_Aus, System.ComponentModel.ISupportInitialize).EndInit()
        Me.gb_mbc91_CH4.ResumeLayout(False)
        Me.gb_mbc91_CH4.PerformLayout()
        CType(Me.nud_mbc91_CH4_TIO, System.ComponentModel.ISupportInitialize).EndInit()
        Me.gb_mbc91_CH3.ResumeLayout(False)
        Me.gb_mbc91_CH3.PerformLayout()
        CType(Me.nud_mbc91_CH3_TIO, System.ComponentModel.ISupportInitialize).EndInit()
        Me.gb_mbc91_CH2.ResumeLayout(False)
        Me.gb_mbc91_CH2.PerformLayout()
        CType(Me.nud_mbc91_CH2_TIO, System.ComponentModel.ISupportInitialize).EndInit()
        Me.gb_mbc91_CH1.ResumeLayout(False)
        Me.gb_mbc91_CH1.PerformLayout()
        CType(Me.nud_mbc91_CH1_TIO, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_mbc91_MMAddr, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.pic_mbc91_Wechsel, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.pic_mbc91_Impuls, System.ComponentModel.ISupportInitialize).EndInit()
        Me.tab_mbc92.ResumeLayout(False)
        Me.tab_mbc92.PerformLayout()
        CType(Me.nud_mbc92_Rueck, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_mbc92_Aus, System.ComponentModel.ISupportInitialize).EndInit()
        Me.gb_mbc92_CH4.ResumeLayout(False)
        Me.gb_mbc92_CH4.PerformLayout()
        CType(Me.nud_mbc92_CH4_TIO, System.ComponentModel.ISupportInitialize).EndInit()
        Me.gb_mbc92_CH3.ResumeLayout(False)
        Me.gb_mbc92_CH3.PerformLayout()
        CType(Me.nud_mbc92_CH3_TIO, System.ComponentModel.ISupportInitialize).EndInit()
        Me.gb_mbc92_CH2.ResumeLayout(False)
        Me.gb_mbc92_CH2.PerformLayout()
        CType(Me.nud_mbc92_CH2_TIO, System.ComponentModel.ISupportInitialize).EndInit()
        Me.gb_mbc92_CH1.ResumeLayout(False)
        Me.gb_mbc92_CH1.PerformLayout()
        CType(Me.nud_mbc92_CH1_TIO, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_mbc92_MMAddr, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.pic_mbc92_Wechsel, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.pic_mbc92_Impuls, System.ComponentModel.ISupportInitialize).EndInit()
        Me.tab_mbc93.ResumeLayout(False)
        Me.gb_mbc93_Latenz.ResumeLayout(False)
        Me.gb_mbc93_Latenz.PerformLayout()
        CType(Me.nud_mbc93_Latenz, System.ComponentModel.ISupportInitialize).EndInit()
        Me.gb_mbc93_GFP.ResumeLayout(False)
        Me.gb_mbc93_GFP.PerformLayout()
        CType(Me.nud_mbc93_GFPSW2, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_mbc93_GFPSW1, System.ComponentModel.ISupportInitialize).EndInit()
        Me.gb_Keyboard.ResumeLayout(False)
        Me.gb_Status.ResumeLayout(False)
        CType(Me.pic_Status_ASM08, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.pic_Status_ASM07, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.pic_Status_ASM06, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.pic_Status_ASM05, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.pic_Status_ASM04, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.pic_Status_ASM03, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.pic_Status_ASM02, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.pic_Status_ASM01, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.pic_Status_Decoder, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.pic_Status_LED19, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.pic_Status_LED17, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.pic_Status_LED14, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.pic_Status_LED12, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.pic_Status_LED08, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.pic_Status_LED06, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.pic_Status_LED03, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.pic_Status_LED01, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.pic_Status_LED18, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.pic_Status_LED16, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.pic_Status_LED15, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.pic_Status_LED13, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.pic_Status_LED07, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.pic_Status_LED05, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.pic_Status_LED04, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.pic_Status_LED02, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.pic_Status_LED09, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.pic_Status_LED10, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.pic_Status_LED11, System.ComponentModel.ISupportInitialize).EndInit()
        Me.gb_Parameter.ResumeLayout(False)
        Me.gb_Parameter.PerformLayout()
        Me.tab_Monitor.ResumeLayout(False)
        Me.tab_Monitor.PerformLayout()
        Me.tab_Adressen.ResumeLayout(False)
        Me.tab_Adressen.PerformLayout()
        CType(Me.dgv_AdressenS88Base, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.dgv_AdressenLocID, System.ComponentModel.ISupportInitialize).EndInit()
        Me.stp_Status.ResumeLayout(False)
        Me.stp_Status.PerformLayout()
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Friend WithEvents ms_Menu As System.Windows.Forms.MenuStrip
    Friend WithEvents ts_MenuSystem As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents ts_MenuSystemSchliessen As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents ts_MenuVerbindung As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents ts_MenuInfo As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents sc_Main As System.Windows.Forms.SplitContainer
    Friend WithEvents tv_Strukturbaum As System.Windows.Forms.TreeView
    Friend WithEvents stp_Status As System.Windows.Forms.StatusStrip
    Friend WithEvents stp_RS232 As System.Windows.Forms.ToolStripStatusLabel
    Friend WithEvents ts_MenuVerbindungEinstellen As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents tm_Neu As System.Windows.Forms.Timer
    Friend WithEvents tm_Alive As System.Windows.Forms.Timer
    Friend WithEvents tm_ComTimeout As System.Windows.Forms.Timer
    Friend WithEvents stp_Refresh As System.Windows.Forms.ToolStripStatusLabel
    Friend WithEvents stp_Alive As System.Windows.Forms.ToolStripStatusLabel
    Friend WithEvents spb_Refresh As System.Windows.Forms.ToolStripProgressBar
    Friend WithEvents ts_MenuInfoCopyright As ToolStripMenuItem
    Friend WithEvents ts_MenuVerbindungRS232 As ToolStripMenuItem
    Friend WithEvents ts_MenuVerbindungRS232Verbinden As ToolStripMenuItem
    Friend WithEvents ts_MenuVerbindungRS232Trennen As ToolStripMenuItem
    Friend WithEvents ts_MenuSystemNeu As ToolStripMenuItem
    Friend WithEvents ts_MenuInfoHilfe As ToolStripMenuItem
    Friend WithEvents ts_MenuSystemSpeichern As ToolStripMenuItem
    Friend WithEvents ts_MenuSystemAnzeigen As ToolStripMenuItem
    Friend WithEvents ts_MenuVerbindungBluetooth As ToolStripMenuItem
    Friend WithEvents ts_MenuVerbindungBluetoothVerbinden As ToolStripMenuItem
    Friend WithEvents ts_MenuVerbindungBluetoothTrennen As ToolStripMenuItem
    Friend WithEvents stp_BT As ToolStripStatusLabel
    Friend WithEvents tab_Control As TabControl
    Friend WithEvents tab_Modul As TabPage
    Friend WithEvents gb_Keyboard As GroupBox
    Friend WithEvents btn_KeyboardG4 As Button
    Friend WithEvents btn_KeyboardG3 As Button
    Friend WithEvents btn_KeyboardG2 As Button
    Friend WithEvents btn_KeyboardG1 As Button
    Friend WithEvents btn_KeyboardR4 As Button
    Friend WithEvents btn_KeyboardR3 As Button
    Friend WithEvents btn_KeyboardR2 As Button
    Friend WithEvents btn_KeyboardR1 As Button
    Friend WithEvents gb_Status As GroupBox
    Friend WithEvents pic_Status_Decoder As PictureBox
    Friend WithEvents pic_Status_LED19 As PictureBox
    Friend WithEvents pic_Status_LED17 As PictureBox
    Friend WithEvents pic_Status_LED14 As PictureBox
    Friend WithEvents pic_Status_LED12 As PictureBox
    Friend WithEvents pic_Status_LED08 As PictureBox
    Friend WithEvents pic_Status_LED06 As PictureBox
    Friend WithEvents pic_Status_LED03 As PictureBox
    Friend WithEvents pic_Status_LED01 As PictureBox
    Friend WithEvents gb_Parameter As GroupBox
    Friend WithEvents lbl_ParameterHW As Label
    Friend WithEvents txt_ParameterHW As TextBox
    Friend WithEvents txt_ParameterSW As TextBox
    Friend WithEvents lbl_ParameterSW As Label
    Friend WithEvents lbl_ParameterName As Label
    Friend WithEvents txt_ParameterName As TextBox
    Friend WithEvents lbl_ParameterEinheitTemperatur As Label
    Friend WithEvents txt_ParameterArtNum As TextBox
    Friend WithEvents txt_ParameterTemperatur As TextBox
    Friend WithEvents lbl_ParameterEinheitSpannung As Label
    Friend WithEvents lbl_ParameterTemperatur As Label
    Friend WithEvents lbl_ParameterArtNum As Label
    Friend WithEvents txt_ParameterPlugged As TextBox
    Friend WithEvents txt_ParameterSpannung As TextBox
    Friend WithEvents lbl_ParameterPlugged As Label
    Friend WithEvents lbl_ParameterSpannung As Label
    Friend WithEvents lbl_ParameterGUID As Label
    Friend WithEvents txt_ParameterGUID As TextBox
    Friend WithEvents txt_ParameterSerNr As TextBox
    Friend WithEvents txt_ParameterDecTyp As TextBox
    Friend WithEvents lbl_ParameterSerNr As Label
    Friend WithEvents lbl_ParameterDecTyp As Label
    Friend WithEvents tab_Monitor As TabPage
    Friend WithEvents rtb_MonitorReceived As RichTextBox
    Friend WithEvents lbl_MonitorNachrichtenherkunft As Label
    Friend WithEvents lbl_MonitorInhalt As Label
    Friend WithEvents tab_Adressen As TabPage
    Friend WithEvents btn_AdressenS88Base As Button
    Friend WithEvents lbl_AdressenS88Base As Label
    Friend WithEvents dgv_AdressenS88Base As DataGridView
    Friend WithEvents dgv_AdressenLocID As DataGridView
    Friend WithEvents btn_AdressenLocID As Button
    Friend WithEvents lbl_AdressenLocID As Label
    Friend WithEvents btn_ControlUpdate As Button
    Friend WithEvents btn_ControlVerwerfen As Button
    Friend WithEvents btn_ControlUpgrade As Button
    Friend WithEvents btn_ControlLoeschen As Button
    Friend WithEvents btn_ControlReset As Button
    Friend WithEvents btn_ControlRefresh As Button
    Friend WithEvents pic_Status_ASM08 As PictureBox
    Friend WithEvents pic_Status_ASM07 As PictureBox
    Friend WithEvents pic_Status_ASM06 As PictureBox
    Friend WithEvents pic_Status_ASM05 As PictureBox
    Friend WithEvents pic_Status_ASM04 As PictureBox
    Friend WithEvents pic_Status_ASM03 As PictureBox
    Friend WithEvents pic_Status_ASM02 As PictureBox
    Friend WithEvents pic_Status_ASM01 As PictureBox
    Friend WithEvents ofd_Oeffnen As OpenFileDialog
    Friend WithEvents ofd_OeffnenBin As OpenFileDialog
    Friend WithEvents stp_Upgrade As ToolStripStatusLabel
    Friend WithEvents tab_Module As TabControl
    Friend WithEvents tab_mbc80 As TabPage
    Friend WithEvents gb_mbc80_BT As GroupBox
    Friend WithEvents txt_mbc80_Passwort As TextBox
    Friend WithEvents lbl_mbc80_PasswortBeschreibung As TextBox
    Friend WithEvents lbl_mbc80_BTNameBeschreibung As TextBox
    Friend WithEvents txt_mbc80_Name As TextBox
    Friend WithEvents lbl_mbc80_Name As Label
    Friend WithEvents lbl_mbc80_Passwort As Label
    Friend WithEvents tab_mbc81 As TabPage
    Friend WithEvents nud_mbc81_LocID As NumericUpDown
    Friend WithEvents lbl_mbc81_LocID As Label
    Friend WithEvents tab_mbc82 As TabPage
    Friend WithEvents tab_mbc83 As TabPage
    Friend WithEvents gb_mbc83_CH4 As GroupBox
    Friend WithEvents gb_mbc83_CH4_nud_TIO As NumericUpDown
    Friend WithEvents gb_mbc83_CH4_lbl_ON As Label
    Friend WithEvents gb_mbc83_CH4_lbl_ms As Label
    Friend WithEvents gb_mbc83_CH3 As GroupBox
    Friend WithEvents gb_mbc83_CH3_nud_TIO As NumericUpDown
    Friend WithEvents gb_mbc83_CH3_lbl_ON As Label
    Friend WithEvents gb_mbc83_CH3_lbl_ms As Label
    Friend WithEvents gb_mbc83_CH2 As GroupBox
    Friend WithEvents gb_mbc83_CH2_nud_TIO As NumericUpDown
    Friend WithEvents gb_mbc83_CH2_lbl_ON As Label
    Friend WithEvents gb_mbc83_CH2_lbl_ms As Label
    Friend WithEvents gb_mbc83_CH1 As GroupBox
    Friend WithEvents gb_mbc83_CH1_nud_TIO As NumericUpDown
    Friend WithEvents gb_mbc83_CH1_lbl_ON As Label
    Friend WithEvents gb_mbc83_CH1_lbl_ms As Label
    Friend WithEvents nud_mbc83_MMAddr As NumericUpDown
    Friend WithEvents lbl_mbc83_LocID As Label
    Friend WithEvents tab_mbc84 As TabPage
    Friend WithEvents gb_mbc84_CH4 As GroupBox
    Friend WithEvents nud_mbc84_Links_CH4 As NumericUpDown
    Friend WithEvents lbl_mbc84_Links_CH4 As Label
    Friend WithEvents nud_mbc84_Rechts_CH4 As NumericUpDown
    Friend WithEvents lbl_mbc84_Rechts_CH4 As Label
    Friend WithEvents btn_mbc84_Weg_CH4 As Button
    Friend WithEvents nud_mbc84_Step_CH4 As NumericUpDown
    Friend WithEvents lbl_mbc84_Step_CH4 As Label
    Friend WithEvents gb_mbc84_CH3 As GroupBox
    Friend WithEvents nud_mbc84_Links_CH3 As NumericUpDown
    Friend WithEvents lbl_mbc84_Links_CH3 As Label
    Friend WithEvents nud_mbc84_Rechts_CH3 As NumericUpDown
    Friend WithEvents lbl_mbc84_Rechts_CH3 As Label
    Friend WithEvents btn_mbc84_Weg_CH3 As Button
    Friend WithEvents nud_mbc84_Step_CH3 As NumericUpDown
    Friend WithEvents lbl_mbc84_Step_CH3 As Label
    Friend WithEvents gb_mbc84_CH2 As GroupBox
    Friend WithEvents nud_mbc84_Links_CH2 As NumericUpDown
    Friend WithEvents lbl_mbc84_Links_CH2 As Label
    Friend WithEvents nud_mbc84_Rechts_CH2 As NumericUpDown
    Friend WithEvents lbl_mbc84_Rechts_CH2 As Label
    Friend WithEvents btn_mbc84_Weg_CH2 As Button
    Friend WithEvents nud_mbc84_Step_CH2 As NumericUpDown
    Friend WithEvents lbl_mbc84_Step_CH2 As Label
    Friend WithEvents gb_mbc84_CH1 As GroupBox
    Friend WithEvents nud_mbc84_Links_CH1 As NumericUpDown
    Friend WithEvents lbl_mbc84_Links_CH1 As Label
    Friend WithEvents nud_mbc84_Rechts_CH1 As NumericUpDown
    Friend WithEvents lbl_mbc84_Rechts_CH1 As Label
    Friend WithEvents btn_mbc84_Weg_CH1 As Button
    Friend WithEvents nud_mbc84_Step_CH1 As NumericUpDown
    Friend WithEvents lbl_mbc84_Step_CH1 As Label
    Friend WithEvents nud_mbc84_MMAddr As NumericUpDown
    Friend WithEvents lbl_mbc84_LocID As Label
    Friend WithEvents tab_mbc85 As TabPage
    Friend WithEvents tab_mbc86 As TabPage
    Friend WithEvents tab_mbc87 As TabPage
    Friend WithEvents tab_mbc88 As TabPage
    Friend WithEvents tab_mbc89 As TabPage
    Friend WithEvents tab_mbc90 As TabPage
    Friend WithEvents tab_mbc91 As TabPage
    Friend WithEvents lbl_mbc91_Wechsel As Label
    Friend WithEvents lbl_mbc91_Impuls As Label
    Friend WithEvents pic_mbc91_Wechsel As PictureBox
    Friend WithEvents pic_mbc91_Impuls As PictureBox
    Friend WithEvents gb_mbc91_CH4 As GroupBox
    Friend WithEvents nud_mbc91_CH4_TIO As NumericUpDown
    Friend WithEvents lbl_mbc91_CH4_Typ As Label
    Friend WithEvents lbl_mbc91_CH4_TIO As Label
    Friend WithEvents clb_mbc91_CH4 As CheckedListBox
    Friend WithEvents lbl_mbc91_CH4_TIOEinheit As Label
    Friend WithEvents gb_mbc91_CH3 As GroupBox
    Friend WithEvents nud_mbc91_CH3_TIO As NumericUpDown
    Friend WithEvents lbl_mbc91_CH3_Typ As Label
    Friend WithEvents lbl_mbc91_CH3_TIO As Label
    Friend WithEvents clb_mbc91_CH3 As CheckedListBox
    Friend WithEvents lbl_mbc91_CH3_TIOEinheit As Label
    Friend WithEvents gb_mbc91_CH2 As GroupBox
    Friend WithEvents nud_mbc91_CH2_TIO As NumericUpDown
    Friend WithEvents lbl_mbc91_CH2_Typ As Label
    Friend WithEvents lbl_mbc91_CH2_TIO As Label
    Friend WithEvents clb_mbc91_CH2 As CheckedListBox
    Friend WithEvents lbl_mbc91_CH2_TIOEinheit As Label
    Friend WithEvents gb_mbc91_CH1 As GroupBox
    Friend WithEvents nud_mbc91_CH1_TIO As NumericUpDown
    Friend WithEvents lbl_mbc91_CH1_Typ As Label
    Friend WithEvents lbl_mbc91_CH1_TIO As Label
    Friend WithEvents clb_mbc91_CH1 As CheckedListBox
    Friend WithEvents lbl_mbc91_CH1_TIOEinheit As Label
    Friend WithEvents nud_mbc91_MMAddr As NumericUpDown
    Friend WithEvents lbl_mbc91_MMAddr As Label
    Friend WithEvents nud_mbc88_S88ModNr As NumericUpDown
    Friend WithEvents lbl_mbc88_modulnr As Label
    Friend WithEvents pic_Status_LED18 As PictureBox
    Friend WithEvents pic_Status_LED16 As PictureBox
    Friend WithEvents pic_Status_LED15 As PictureBox
    Friend WithEvents pic_Status_LED13 As PictureBox
    Friend WithEvents pic_Status_LED07 As PictureBox
    Friend WithEvents pic_Status_LED05 As PictureBox
    Friend WithEvents pic_Status_LED04 As PictureBox
    Friend WithEvents pic_Status_LED02 As PictureBox
    Friend WithEvents nud_mbc88_T_P13 As NumericUpDown
    Friend WithEvents lbl_mbc88_P13 As Label
    Friend WithEvents lbl_mbc88_P13a As Label
    Friend WithEvents nud_mbc88_T_P09 As NumericUpDown
    Friend WithEvents lbl_mbc88_P09 As Label
    Friend WithEvents lbl_mbc88_P09a As Label
    Friend WithEvents nud_mbc88_T_P05 As NumericUpDown
    Friend WithEvents lbl_mbc88_P05 As Label
    Friend WithEvents lbl_mbc88_P05a As Label
    Friend WithEvents nud_mbc88_T_P01 As NumericUpDown
    Friend WithEvents lbl_mbc88_P01 As Label
    Friend WithEvents lbl_mbc88_P01a As Label
    Friend WithEvents nud_mbc88_T_P16 As NumericUpDown
    Friend WithEvents lbl_mbc88_P16 As Label
    Friend WithEvents lbl_mbc88_P16a As Label
    Friend WithEvents nud_mbc88_T_P12 As NumericUpDown
    Friend WithEvents lbl_mbc88_P12 As Label
    Friend WithEvents lbl_mbc88_P12a As Label
    Friend WithEvents nud_mbc88_T_P08 As NumericUpDown
    Friend WithEvents lbl_mbc88_P08 As Label
    Friend WithEvents lbl_mbc88_P08a As Label
    Friend WithEvents nud_mbc88_T_P04 As NumericUpDown
    Friend WithEvents lbl_mbc88_P04 As Label
    Friend WithEvents lbl_mbc88_P04a As Label
    Friend WithEvents nud_mbc88_T_P15 As NumericUpDown
    Friend WithEvents lbl_mbc88_P15 As Label
    Friend WithEvents lbl_mbc88_P15a As Label
    Friend WithEvents nud_mbc88_T_P11 As NumericUpDown
    Friend WithEvents lbl_mbc88_P11 As Label
    Friend WithEvents lbl_mbc88_P11a As Label
    Friend WithEvents nud_mbc88_T_P07 As NumericUpDown
    Friend WithEvents lbl_mbc88_P07 As Label
    Friend WithEvents lbl_mbc88_P07a As Label
    Friend WithEvents nud_mbc88_T_P03 As NumericUpDown
    Friend WithEvents lbl_mbc88_P03 As Label
    Friend WithEvents lbl_mbc88_P03a As Label
    Friend WithEvents nud_mbc88_T_P14 As NumericUpDown
    Friend WithEvents lbl_mbc88_P14 As Label
    Friend WithEvents lbl_mbc88_P14a As Label
    Friend WithEvents nud_mbc88_T_P10 As NumericUpDown
    Friend WithEvents lbl_mbc88_P10 As Label
    Friend WithEvents lbl_mbc88_P10a As Label
    Friend WithEvents nud_mbc88_T_P06 As NumericUpDown
    Friend WithEvents lbl_mbc88_P06 As Label
    Friend WithEvents lbl_mbc88_P06a As Label
    Friend WithEvents nud_mbc88_T_P02 As NumericUpDown
    Friend WithEvents lbl_mbc88_P02 As Label
    Friend WithEvents lbl_mbc88_P02a As Label
    Friend WithEvents gb_mbc88 As GroupBox
    Friend WithEvents nud_mbc90_T_P16 As NumericUpDown
    Friend WithEvents lbl_mbc90_P16 As Label
    Friend WithEvents lbl_mbc90_P16a As Label
    Friend WithEvents nud_mbc90_T_P12 As NumericUpDown
    Friend WithEvents lbl_mbc90_P12 As Label
    Friend WithEvents lbl_mbc90_P12a As Label
    Friend WithEvents nud_mbc90_T_P08 As NumericUpDown
    Friend WithEvents lbl_mbc90_P08 As Label
    Friend WithEvents lbl_mbc90_P08a As Label
    Friend WithEvents nud_mbc90_T_P04 As NumericUpDown
    Friend WithEvents lbl_mbc90_P04 As Label
    Friend WithEvents lbl_mbc90_P04a As Label
    Friend WithEvents nud_mbc90_T_P15 As NumericUpDown
    Friend WithEvents lbl_mbc90_P15 As Label
    Friend WithEvents lbl_mbc90_P15a As Label
    Friend WithEvents nud_mbc90_T_P11 As NumericUpDown
    Friend WithEvents lbl_mbc90_P11 As Label
    Friend WithEvents lbl_mbc90_P11a As Label
    Friend WithEvents nud_mbc90_T_P07 As NumericUpDown
    Friend WithEvents lbl_mbc90_P07 As Label
    Friend WithEvents lbl_mbc90_P07a As Label
    Friend WithEvents nud_mbc90_T_P03 As NumericUpDown
    Friend WithEvents lbl_mbc90_P03 As Label
    Friend WithEvents lbl_mbc90_P03a As Label
    Friend WithEvents nud_mbc90_T_P14 As NumericUpDown
    Friend WithEvents lbl_mbc90_P14 As Label
    Friend WithEvents lbl_mbc90_P14a As Label
    Friend WithEvents nud_mbc90_T_P10 As NumericUpDown
    Friend WithEvents lbl_mbc90_P10 As Label
    Friend WithEvents lbl_mbc90_P10a As Label
    Friend WithEvents nud_mbc90_T_P06 As NumericUpDown
    Friend WithEvents lbl_mbc90_P06 As Label
    Friend WithEvents lbl_mbc90_P06a As Label
    Friend WithEvents nud_mbc90_T_P02 As NumericUpDown
    Friend WithEvents lbl_mbc90_P02 As Label
    Friend WithEvents lbl_mbc90_P02a As Label
    Friend WithEvents nud_mbc90_T_P13 As NumericUpDown
    Friend WithEvents lbl_mbc90_P13 As Label
    Friend WithEvents lbl_mbc90_P13a As Label
    Friend WithEvents nud_mbc90_T_P09 As NumericUpDown
    Friend WithEvents lbl_mbc90_P09 As Label
    Friend WithEvents lbl_mbc90_P09a As Label
    Friend WithEvents nud_mbc90_T_P05 As NumericUpDown
    Friend WithEvents lbl_mbc90_P05 As Label
    Friend WithEvents lbl_mbc90_P05a As Label
    Friend WithEvents nud_mbc90_T_P01 As NumericUpDown
    Friend WithEvents lbl_mbc90_P01 As Label
    Friend WithEvents lbl_mbc90_P01a As Label
    Friend WithEvents nud_mbc90_S88ModNr As NumericUpDown
    Friend WithEvents lbl_mbc90_S88ModulNr As Label
    Friend WithEvents gb_mbc90 As GroupBox
    Friend WithEvents nud_mbc83_Dimmer As NumericUpDown
    Friend WithEvents lbl_mbc83_Dimmer As Label
    Friend WithEvents lbl_mbc83_Meldezyklus As Label
    Friend WithEvents lbl_mbc83_AutoAus As Label
    Friend WithEvents nud_mbc83_Rueck As NumericUpDown
    Friend WithEvents nud_mbc83_Aus As NumericUpDown
    Friend WithEvents lbl_mbc83_ms As Label
    Friend WithEvents txt_mbc80_AK As TextBox
    Friend WithEvents gb_mbc80_AK As GroupBox
    Friend WithEvents lbl_mbc80_AK As TextBox
    Friend WithEvents lbl_mbc88_halbmodul As Label
    Friend WithEvents ts_MenuInfoBedienungsanleitung As ToolStripMenuItem
    Friend WithEvents lbl_mbc91_AusEinheit As Label
    Friend WithEvents lbl_mbc91_Rueck As Label
    Friend WithEvents lbl_mbc91_Aus As Label
    Friend WithEvents nud_mbc91_Rueck As NumericUpDown
    Friend WithEvents nud_mbc91_Aus As NumericUpDown
    Friend WithEvents gb_mbc83_CH4_lbl_Typ As Label
    Friend WithEvents gb_mbc83_CH4_clb_Typ As CheckedListBox
    Friend WithEvents gb_mbc83_CH3_lbl_Typ As Label
    Friend WithEvents gb_mbc83_CH3_clb_Typ As CheckedListBox
    Friend WithEvents gb_mbc83_CH2_lbl_Typ As Label
    Friend WithEvents gb_mbc83_CH2_clb_Typ As CheckedListBox
    Friend WithEvents gb_mbc83_CH1_lbl_typ As Label
    Friend WithEvents gb_mbc83_CH1_clb_Typ As CheckedListBox
    Friend WithEvents lbl_mbc83_Startup As Label
    Friend WithEvents pic_mbc83_Startup As PictureBox
    Friend WithEvents lbl_mbc83_Wechsel As Label
    Friend WithEvents lbl_mbc83_Impuls As Label
    Friend WithEvents pic_mbc83_Wechsel As PictureBox
    Friend WithEvents pic_mbc83_Impuls As PictureBox
    Friend WithEvents nud_mbc87_MMAddr As NumericUpDown
    Friend WithEvents lbl_mbc87_1 As Label
    Friend WithEvents txt_mbc91_CH4_Addr As TextBox
    Friend WithEvents lbl_mbc91_CH4_Addr As Label
    Friend WithEvents txt_mbc91_CH3_Addr As TextBox
    Friend WithEvents lbl_mbc91_CH3_Addr As Label
    Friend WithEvents txt_mbc91_CH2_Addr As TextBox
    Friend WithEvents lbl_mbc91_CH2_Addr As Label
    Friend WithEvents txt_mbc91_CH1_Addr As TextBox
    Friend WithEvents lbl_mbc91_CH1_Addr As Label
    Friend WithEvents txt_mbc84_adr_4 As TextBox
    Friend WithEvents lbl_mbc84_adr_CH4 As Label
    Friend WithEvents txt_mbc84_adr_3 As TextBox
    Friend WithEvents lbl_mbc84_adr_CH3 As Label
    Friend WithEvents txt_mbc84_adr_2 As TextBox
    Friend WithEvents lbl_mbc84_adr_CH2 As Label
    Friend WithEvents txt_mbc84_adr_1 As TextBox
    Friend WithEvents lbl_mbc84_adr_CH1 As Label
    Friend WithEvents gb_mbc83_CH4_txt_Adresse As TextBox
    Friend WithEvents gb_mbc83_CH4_lbl_Adresse As Label
    Friend WithEvents gb_mbc83_CH3_txt_Adresse As TextBox
    Friend WithEvents gb_mbc83_CH3_lbl_Adresse As Label
    Friend WithEvents txt_mbc83_adr_2 As TextBox
    Friend WithEvents gb_mbc83_CH2_lbl_Adresse As Label
    Friend WithEvents gb_mbc83_CH1_txt_Adresse As TextBox
    Friend WithEvents gb_mbc83_CH1_lbl_Adresse As Label
    Friend WithEvents lbl_mbc81_Adresse2 As Label
    Friend WithEvents txt_mbc81_Adresse2 As TextBox
    Friend WithEvents txt_mbc81_Adresse1 As TextBox
    Friend WithEvents lbl_mbc81_Adresse1 As Label
    Friend WithEvents txt_mbc88_busnr As TextBox
    Friend WithEvents lbl_mbc88_busnr As Label
    Friend WithEvents lbl_mbc88_bis As Label
    Friend WithEvents txt_mbc88_kontakt2cs2 As TextBox
    Friend WithEvents txt_mbc88_kontakt1cs2 As TextBox
    Friend WithEvents lbl_mbc88_kontaktecs2 As Label
    Friend WithEvents lbl_mbc90_kontaktecs2bis As Label
    Friend WithEvents txt_mbc90_kontakt2cs2 As TextBox
    Friend WithEvents txt_mbc90_kontakt1cs2 As TextBox
    Friend WithEvents lbl_mbc90_kontaktecs2 As Label
    Friend WithEvents txt_mbc90_busnr As TextBox
    Friend WithEvents lbl_mbc90_busnr As Label
    Friend WithEvents gb_mbc80_SW As GroupBox
    Friend WithEvents lbl_mbc80_SWBeschreibung As TextBox
    Friend WithEvents lbl_mbc80_SWBezeichnung As Label
    Friend WithEvents gb_mbc82_GFP As GroupBox
    Friend WithEvents lbl_mbc82_GFPBeschreibung_1 As TextBox
    Friend WithEvents lbl_mbc82_GFPBezeichnung As Label
    Friend WithEvents lbl_mbc82_GFPPunkt As Label
    Friend WithEvents nud_mbc82_GFPSW2 As NumericUpDown
    Friend WithEvents nud_mbc82_GFPSW1 As NumericUpDown
    Friend WithEvents gb_mbc82_Latenz As GroupBox
    Friend WithEvents nud_mbc82_Latenz As NumericUpDown
    Friend WithEvents lbl_mbc82_LatenzBeschreibung_1 As TextBox
    Friend WithEvents lbl_mbc82_LatenzBezeichnung As Label
    Friend WithEvents lbl_mbc80_SWPunkt As Label
    Friend WithEvents nud_mbc80_SW2 As NumericUpDown
    Friend WithEvents nud_mbc80_SW1 As NumericUpDown
    Friend WithEvents lbl_mbc81_NutzungErlaeuterung As Label
    Friend WithEvents lbl_mbc81_Nutzung As Label
    Friend WithEvents cmb_mbc81_Nutzung As ComboBox
    Friend WithEvents ts_MenuVerbindungTCP As ToolStripMenuItem
    Friend WithEvents ts_MenuVerbindungTCPVerbinden As ToolStripMenuItem
    Friend WithEvents ts_MenuVerbindungTCPTrennen As ToolStripMenuItem
    Friend WithEvents stp_TCP As ToolStripStatusLabel
    Friend WithEvents stp_CSx As ToolStripStatusLabel
    Friend WithEvents nud_mbc84_Rueck As NumericUpDown
    Friend WithEvents lbl_mbc84_Hinweis As Label
    Friend WithEvents gb_mbc89_Version As GroupBox
    Friend WithEvents nud_mbc89_SW_2 As NumericUpDown
    Friend WithEvents lbl_mbc89_Dot As Label
    Friend WithEvents nud_mbc89_SW_1 As NumericUpDown
    Friend WithEvents lbl_mbc89_Version As Label
    Friend WithEvents gb_mbc89_Mapping As GroupBox
    Friend WithEvents clb_mbc89_Mapping As CheckedListBox
    Friend WithEvents gb_mbc89_Keyboard As GroupBox
    Friend WithEvents clb_mbc89_D As CheckedListBox
    Friend WithEvents clb_mbc89_C As CheckedListBox
    Friend WithEvents clb_mbc89_B As CheckedListBox
    Friend WithEvents clb_mbc89_A As CheckedListBox
    Friend WithEvents lbl_mbc89_D As Label
    Friend WithEvents lbl_mbc89_C As Label
    Friend WithEvents lbl_mbc89_B As Label
    Friend WithEvents lbl_mbc89_A As Label
    Friend WithEvents nud_mbc89_keyboard_nr As NumericUpDown
    Friend WithEvents pb_mbc89_keyboard As PictureBox
    Friend WithEvents txt_mbc89_alt As TextBox
    Friend WithEvents btn_mbc89_Loks As Button
    Friend WithEvents tm_Update As Timer
    Friend WithEvents lbl_mbc89_Basisadresse As Label
    Friend WithEvents nud_mbc89_basis As NumericUpDown
    Friend WithEvents gb_mbc89_Basisadresse As GroupBox
    Friend WithEvents lbl_mbc85_bis As Label
    Friend WithEvents txt_mbc85_kontakt2cs2 As TextBox
    Friend WithEvents txt_mbc85_kontakt1cs2 As TextBox
    Friend WithEvents lbl_mbc85_Kontakte As Label
    Friend WithEvents txt_mbc85_BusNr As TextBox
    Friend WithEvents lbl_mbc85_BusNr As Label
    Friend WithEvents nud_mbc85_T_P08 As NumericUpDown
    Friend WithEvents lbl_mbc85_P08 As Label
    Friend WithEvents lbl_mbc85_P08a As Label
    Friend WithEvents nud_mbc85_T_P04 As NumericUpDown
    Friend WithEvents lbl_mbc85_P04 As Label
    Friend WithEvents lbl_mbc85_P04a As Label
    Friend WithEvents nud_mbc85_T_P07 As NumericUpDown
    Friend WithEvents lbl_mbc85_P07 As Label
    Friend WithEvents lbl_mbc85_P07a As Label
    Friend WithEvents nud_mbc85_T_P03 As NumericUpDown
    Friend WithEvents lbl_mbc85_P03 As Label
    Friend WithEvents lbl_mbc85_P03a As Label
    Friend WithEvents nud_mbc85_T_P10 As NumericUpDown
    Friend WithEvents lbl_mbc85_P10 As Label
    Friend WithEvents lbl_mbc85_P10a As Label
    Friend WithEvents nud_mbc85_T_P06 As NumericUpDown
    Friend WithEvents lbl_mbc85_P06 As Label
    Friend WithEvents lbl_mbc85_P06a As Label
    Friend WithEvents nud_mbc85_T_P02 As NumericUpDown
    Friend WithEvents lbl_mbc85_P02 As Label
    Friend WithEvents lbl_mbc85_P02a As Label
    Friend WithEvents nud_mbc85_T_P09 As NumericUpDown
    Friend WithEvents lbl_mbc85_P09 As Label
    Friend WithEvents lbl_mbc85_P09a As Label
    Friend WithEvents nud_mbc85_T_P05 As NumericUpDown
    Friend WithEvents lbl_mbc85_P05 As Label
    Friend WithEvents lbl_mbc85_P05a As Label
    Friend WithEvents nud_mbc85_T_P01 As NumericUpDown
    Friend WithEvents lbl_mbc85_P01 As Label
    Friend WithEvents lbl_mbc85_P01a As Label
    Friend WithEvents nud_mbc85_S88ModNr As NumericUpDown
    Friend WithEvents lbl_mbc85_ModulNr As Label
    Friend WithEvents gb_mbc85_Integrator As GroupBox
    Friend WithEvents pic_Status_LED09 As PictureBox
    Friend WithEvents pic_Status_LED10 As PictureBox
    Friend WithEvents gb_mbc89_I2C As GroupBox
    Friend WithEvents lbl_mbc89_frequenz As Label
    Friend WithEvents lbl_mbc89_takt As Label
    Friend WithEvents nud_mbc89_i2c As NumericUpDown
    Friend WithEvents pic_Status_LED11 As PictureBox
    Friend WithEvents cmb_mbc88_Halbmodul As ComboBox
    Friend WithEvents S88ColumnModNr As DataGridViewTextBoxColumn
    Friend WithEvents S88ColumnS88Typ As DataGridViewTextBoxColumn
    Friend WithEvents S88ColumnS88Name As DataGridViewTextBoxColumn
    Friend WithEvents S88ColumnS88Index As DataGridViewTextBoxColumn
    Friend WithEvents MMColumnLocID As DataGridViewTextBoxColumn
    Friend WithEvents MMColumnLocIDTyp As DataGridViewTextBoxColumn
    Friend WithEvents MMColumnLocIDName As DataGridViewTextBoxColumn
    Friend WithEvents MMColumnLocIDIndex As DataGridViewTextBoxColumn
    Friend WithEvents lbl_mbc82_LatenzBeschreibung_3 As TextBox
    Friend WithEvents lbl_mbc82_LatenzBeschreibung_2 As TextBox
    Friend WithEvents lbl_mbc82_GFPBeschreibung_2 As TextBox
    Friend WithEvents lbl_mbc81_Nutzung_Erlaeuterung_3 As Label
    Friend WithEvents lbl_mbc81_Nutzung_Erlaeuterung_2 As Label
    Friend WithEvents lbl_mbc81_Nutzung_Erlaeuterung_1 As Label
    Friend WithEvents ts_MenuSystemSprache As ToolStripMenuItem
    Friend WithEvents ts_MenuSystemSprache_Deutsch As ToolStripMenuItem
    Friend WithEvents ts_MenuSystemSprache_Französisch As ToolStripMenuItem
    Friend WithEvents ts_MenuSystemSprache_Englisch As ToolStripMenuItem
    Friend WithEvents gb_mbc85_DS_Integrator As GroupBox
    Friend WithEvents lbl_mbc85_P11 As Label
    Friend WithEvents lbl_mbc85_P11a As Label
    Friend WithEvents nud_mbc85_T_P11 As NumericUpDown
    Friend WithEvents gb_mbc85_MMLok As GroupBox
    Friend WithEvents nud_mbc85_MMSpeed As NumericUpDown
    Friend WithEvents lbl_mbc85_MMSpeed As Label
    Friend WithEvents lbl_mbc85_MMLok As Label
    Friend WithEvents nud_mbc85_MMLok As NumericUpDown
    Friend WithEvents btn_mbc85_Winkel As Button
    Friend WithEvents tab_mbc92 As TabPage
    Friend WithEvents gb_mbc86_MMLok As GroupBox
    Friend WithEvents lbl_mbc86_MMLok As Label
    Friend WithEvents nud_mbc86_MMLok As NumericUpDown
    Friend WithEvents Label39 As Label
    Friend WithEvents txt_mbc86_kontakt2cs2 As TextBox
    Friend WithEvents txt_mbc86_kontakt1cs2 As TextBox
    Friend WithEvents lbl_mbc86_Kontakte As Label
    Friend WithEvents txt_mbc86_BusNr As TextBox
    Friend WithEvents lbl_mbc86_BusNr As Label
    Friend WithEvents nud_mbc86_S88ModNr As NumericUpDown
    Friend WithEvents lbl_mbc86_ModulNr As Label
    Friend WithEvents gb_mbc86_Integrator As GroupBox
    Friend WithEvents nud_mbc86_T_P05 As NumericUpDown
    Friend WithEvents Label12 As Label
    Friend WithEvents Label13 As Label
    Friend WithEvents nud_mbc86_T_P06 As NumericUpDown
    Friend WithEvents Label14 As Label
    Friend WithEvents Label15 As Label
    Friend WithEvents nud_mbc86_T_P04 As NumericUpDown
    Friend WithEvents Label16 As Label
    Friend WithEvents nud_mbc86_T_P08 As NumericUpDown
    Friend WithEvents Label17 As Label
    Friend WithEvents Label20 As Label
    Friend WithEvents nud_mbc86_T_P03 As NumericUpDown
    Friend WithEvents nud_mbc86_T_P07 As NumericUpDown
    Friend WithEvents Label21 As Label
    Friend WithEvents Label22 As Label
    Friend WithEvents Label23 As Label
    Friend WithEvents Label24 As Label
    Friend WithEvents nud_mbc86_T_P02 As NumericUpDown
    Friend WithEvents Label25 As Label
    Friend WithEvents Label28 As Label
    Friend WithEvents Label29 As Label
    Friend WithEvents nud_mbc86_T_P01 As NumericUpDown
    Friend WithEvents Label31 As Label
    Friend WithEvents Label33 As Label
    Friend WithEvents tab_mbc93 As TabPage
    Friend WithEvents gb_mbc93_Latenz As GroupBox
    Friend WithEvents lbl_mbc93_LatenzBeschreibung_3 As TextBox
    Friend WithEvents lbl_mbc93_LatenzBeschreibung_2 As TextBox
    Friend WithEvents nud_mbc93_Latenz As NumericUpDown
    Friend WithEvents lbl_mbc93_LatenzBeschreibung_1 As TextBox
    Friend WithEvents lbl_mbc93_LatenzBezeichnung As Label
    Friend WithEvents gb_mbc93_GFP As GroupBox
    Friend WithEvents lbl_mbc93_GFPBeschreibung_2 As TextBox
    Friend WithEvents Label32 As Label
    Friend WithEvents nud_mbc93_GFPSW2 As NumericUpDown
    Friend WithEvents nud_mbc93_GFPSW1 As NumericUpDown
    Friend WithEvents lbl_mbc93_GFPBeschreibung_1 As TextBox
    Friend WithEvents lbl_mbc93_GFPBezeichnung As Label
    Friend WithEvents lbl_mbc92_AusEinheit As Label
    Friend WithEvents lbl_mbc92_Rueck As Label
    Friend WithEvents lbl_mbc92_Aus As Label
    Friend WithEvents nud_mbc92_Rueck As NumericUpDown
    Friend WithEvents nud_mbc92_Aus As NumericUpDown
    Friend WithEvents lbl_mbc92_Wechsel As Label
    Friend WithEvents lbl_mbc92_Impuls As Label
    Friend WithEvents gb_mbc92_CH4 As GroupBox
    Friend WithEvents txt_mbc92_CH4_Addr As TextBox
    Friend WithEvents lbl_mbc92_CH4_Addr As Label
    Friend WithEvents nud_mbc92_CH4_TIO As NumericUpDown
    Friend WithEvents lbl_mbc92_CH4_Typ As Label
    Friend WithEvents lbl_mbc92_CH4_TIO As Label
    Friend WithEvents clb_mbc92_CH4 As CheckedListBox
    Friend WithEvents lbl_mbc92_CH4_TIOEinheit As Label
    Friend WithEvents gb_mbc92_CH3 As GroupBox
    Friend WithEvents txt_mbc92_CH3_Addr As TextBox
    Friend WithEvents lbl_mbc92_CH3_Addr As Label
    Friend WithEvents nud_mbc92_CH3_TIO As NumericUpDown
    Friend WithEvents lbl_mbc92_CH3_Typ As Label
    Friend WithEvents lbl_mbc92_CH3_TIO As Label
    Friend WithEvents clb_mbc92_CH3 As CheckedListBox
    Friend WithEvents lbl_mbc92_CH3_TIOEinheit As Label
    Friend WithEvents gb_mbc92_CH2 As GroupBox
    Friend WithEvents txt_mbc92_CH2_Addr As TextBox
    Friend WithEvents lbl_mbc92_CH2_Addr As Label
    Friend WithEvents nud_mbc92_CH2_TIO As NumericUpDown
    Friend WithEvents lbl_mbc92_CH2_Typ As Label
    Friend WithEvents lbl_mbc92_CH2_TIO As Label
    Friend WithEvents clb_mbc92_CH2 As CheckedListBox
    Friend WithEvents lbl_mbc92_CH2_TIOEinheit As Label
    Friend WithEvents gb_mbc92_CH1 As GroupBox
    Friend WithEvents txt_mbc92_CH1_Addr As TextBox
    Friend WithEvents lbl_mbc92_CH1_Addr As Label
    Friend WithEvents nud_mbc92_CH1_TIO As NumericUpDown
    Friend WithEvents lbl_mbc92_CH1_Typ As Label
    Friend WithEvents lbl_mbc92_CH1_TIO As Label
    Friend WithEvents clb_mbc92_CH1 As CheckedListBox
    Friend WithEvents lbl_mbc92_CH1_TIOEinheit As Label
    Friend WithEvents nud_mbc92_MMAddr As NumericUpDown
    Friend WithEvents lbl_mbc92_MMAddr As Label
    Friend WithEvents pic_mbc92_Wechsel As PictureBox
    Friend WithEvents pic_mbc92_Impuls As PictureBox
End Class
