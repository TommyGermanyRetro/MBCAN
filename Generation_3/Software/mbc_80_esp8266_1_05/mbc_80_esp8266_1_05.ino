//*******************************************************************************
//* UART- und TCP-Funktionen                                                    *
//* fuer PC-Interface beim ATMega644-Controller fuer MBCAN                      *
//*******************************************************************************
//* Autor:          Dr.-Ing. Thomas Wiesner                                     *
//* Dateiname:      MBCAN_mbc_80_esp8266_TCP                                    *
//* Firmware:       1.05                                                        *
//* Erstelldatum:   28.07.2024                                                  *
//* Aenderungen:    Webpage WLAN-Zugang ergaenzt                                *
//*                 WLAN/IP-Einstellungen per Web ergaenzt                      *
//*******************************************************************************
//* Quellen:                                                                    *
//* https://techtutorialsx.com                                                  *
//* https://arduino.cc                                                          *
//*******************************************************************************                    

#include <ESP8266WiFi.h> 
#include <ESP8266WebServer.h>
#include <EEPROM.h>

//*******************************************************************************
//* Definitionen                                                                *
//*******************************************************************************

// Webpage fuer die Eingabe der neuen WLAN/IP-Einstellungen

const char* wifisetup_1 = R"=====(
<!doctype html>
<html lang='de'>
  <head>
    <meta charset='utf-8'>
    <meta name='viewport' content='width=device-width, initial-scale=1'>
    <title>
      Wifi Setup
    </title>
    <style>
      *,::after,::before{box-sizing:border-box;}
	    body{margin:0;font-family:'Segoe UI',Roboto,'Helvetica Neue',Arial,'Noto Sans','Liberation Sans';font-size:1rem;font-weight:400;line-height:1.5;color:#212529;background-color:#f5f5f5;}
	    .form-control{display:block;width:100%;height:calc(1.5em + .75rem + 2px);border:1px solid #ced4da;}
	    button{cursor: pointer;border:1px solid transparent;color:#fff;background-color:#007bff;border-color:#007bff;padding:.5rem 1rem;font-size:1.25rem;line-height:1.5;border-radius:.3rem;width:100%}
	    .form-signin{width:100%;max-width:400px;padding:15px;margin:auto;}
	    h1{text-align: center}
	    input:invalid { background: hsl(5,30%,80%) }
    </style>
  </head>
  <body>
   <main class='form-signin'>
      <form action='/' method='post'>
	      <h1 class=''>
          Wifi-Setup zum MBCAN mbc-80
        </h1>
        <div class='form-floating'>
          <br/>
          <label>
            SSID aktuell
          </label>
          <br/>
          <i>
)=====";

const char* wifisetup_2 = R"=====(
          </i>
        </div>
        <div class='form-floating'>
          <br/>
          <label>
            IPv4 aktuell
          </label>
          <br/>
          <i>
)=====";

const char* wifisetup_3 = R"=====(
          </i>
        </div>
        <div class='form-floating'>
          <br/>
          <label>
            SSID neu
          </label>
          <br/>
          <input type='text' class='form-control' name='ssid' required>
        </div>
        <div class='form-floating'>
          <br/>
          <label>
            Passwort neu
          </label>
          <br/>
          <input type='password' class='form-control' name='password' required>
        </div>
        <div class='form-floating'>
          <br/>
          <label>
            IPv4 neu
          </label>
          <br/>
          <input type="text" class='form-control' name='ipv4' pattern="((^|\.)((25[0-5])|(2[0-4]\d)|(1\d\d)|([1-9]?\d))){4}$" required>
        </div>
        <div class='form-floating'>
          <br/>
          <label>
            Gateway neu
          </label>
          <br/>
          <input type="text" class='form-control' name='gwv4' pattern="((^|\.)((25[0-5])|(2[0-4]\d)|(1\d\d)|([1-9]?\d))){4}$" required>
        </div>
        <div class='form-floating'>
          <br/>
          <label>
            DNS neu
          </label>
          <br/>
          <input type="text" class='form-control' name='dnsv4' pattern="((^|\.)((25[0-5])|(2[0-4]\d)|(1\d\d)|([1-9]?\d))){4}$" required>
        </div>
	      <div class='form-floating'>
          <br/>
          <p style='text-align: center'>
            Subnet wird automatisch gesetzt auf: 
            <br/>
            <i>
              255.255.255.0 
            </i>
          </p>
          <br/>
        </div>
        <br/>
        <button type='submit'>
          Sichern
        </button>
        <p style='text-align: center'>
	  <i>
	  Weitere Informationen unter
	  </i>
          <a href='https://www.mbcan.de' style='color: #32C5FF'>
            mbcan.de
          </a>
        </p>
      </form>
    </main>
  </body>
</html>
)=====";

// Webpage fuer die Bestaetigung der neuen WLAN/IP-Einstellungen

const char* wifiok_1 = R"=====(
<!doctype html>
<html lang='de'>
  <head>
    <meta charset='utf-8'>
    <meta name='viewport' content='width=device-width, initial-scale=1'>
    <title>
      Wifi Setup
    </title>
    <style>
      *,::after,::before{box-sizing:border-box;}
      body{margin:0;font-family:'Segoe UI',Roboto,'Helvetica Neue',Arial,'Noto Sans','Liberation Sans';font-size:1rem;font-weight:400;line-height:1.5;color:#212529;background-color:#f5f5f5;}
      .form-control{display:block;width:100%;height:calc(1.5em + .75rem + 2px);border:1px solid #ced4da;}
      button{border:1px solid transparent;color:#fff;background-color:#007bff;border-color:#007bff;padding:.5rem 1rem;font-size:1.25rem;line-height:1.5;border-radius:.3rem;width:100%}
      .form-signin{width:100%;max-width:400px;padding:15px;margin:auto;}
      h1,p{text-align: center}
    </style>
    </head>
  <body>
    <main class='form-signin'>
      <h1>
       Wifi-Setup zum MBCAN mbc-80
      </h1>
      <div class='form-floating'>
        <br/>
        <label>
          SSID aktuell
        </label>
        <br/>
        <i>
)=====";

const char* wifiok_2 = R"=====(
        </i>
        </div>
        <div class='form-floating'>
          <br/>
          <label>
            IPv4 aktuell
          </label>
          <br/>
          <i>
)=====";

const char* wifiok_3 = R"=====(
          </i>
        </div>
      <br/>
      <p>
        Ihre Einstellungen wurden erfolgreich gespeichert.
        <br/>
        <br/>
        Die Kommunikation mit dem mbc-80 wird neu gestartet.
        <br/>
        <br/>
        Sie können dieses Fenster jetzt schließen!
      </p>
    </main>
  </body>
</html>
)=====";

// Struktur der WLAN-Verbindung

struct settings {
  char ssid[30];
  char password[30];
  IPAddress ip;
  IPAddress dns;
  IPAddress gateway;
  IPAddress subnet;
} user_wifi = {};

// WifiMode AP/Client

bool WifiIsClient = false;

// Webserver definieren

ESP8266WebServer    WEBserver(80);

// Anzeige fuer erfolgreiche Verbindung ueber GPIO 02

#define LED 2

// Kommunikationsport zum ESP8266 
// ggf. durch Firewall freizugeben

#define PORT         15731  

// WiFi-Objekte definieren

WiFiServer TCPserver(PORT); 

// Buffer fuer Nachrichten definieren

// UART-Nachricht - Aktuelle Anzahl von gelesenen Bytes aus dem TCP-Stream
// UART-Nachricht - Array mit gelesenen Bytes aus dem TCP-Stream
// UART-Nachricht - String der in den UART-Stream geschrieben wird

int  byte_tcp = 0;      
unsigned char tcp[30];      
unsigned char uartstring[28];

// TCP-Nachricht - Aktuelle Anzahl von gelesenen Bytes aus dem UART-Stream
// TCP-Nachricht - Array mit gelesenen Bytes aus dem UART-Stream
// TCP-Nachricht - String der in den TCP-Stream geschrieben wird
 
int  byte_uart = 0;     
unsigned char uart[30];     
unsigned char tcpstring[13];

//*******************************************************************************
//* Standard-Setup-Routine                                                      *
//*******************************************************************************

void setup() 
{ 
  uint8_t retry = 0;
    
  // Setze Baudrate der UART-Schnittstelle auf 500 kBit/s
    
  Serial.begin(125000); 

  uartstring[26]='\r';
  uartstring[27]='\n';

  // Zeitverzögerung um den internen Prozessen
  // den Start zu ermoeglichen 
    
  delay(1000); 

  // EEPROM zu den WLAN/IP-Einstellungen auslesen

  EEPROM.begin(sizeof(struct settings) );
  EEPROM.get( 0, user_wifi );

  // Baue Verbindung zum externen WLAN-Router auf und warte
  // bis Verbindung steht
  // Bei der Erstinstallation gibt es noch keine vergebene SSID
  // Daher generiere ein "MBCAN universal" und starte automatisch den AccessPoint
  // Die anderen Netzwerkparameter werden gleichfalls vorbelegt
  // und im EEPROM gespeichert

  if (user_wifi.ssid[0] == 0xff) 
  {
    // Initialisiere WLAN/IP-Einstellungen und schreibe es ins EEPROM

    strncpy(user_wifi.ssid,"MBCAN universal",sizeof(user_wifi.ssid) );
    user_wifi.ip.fromString("192.168.4.2");
    user_wifi.dns.fromString("192.168.4.1");
    user_wifi.gateway.fromString("192.168.4.1");
    user_wifi.subnet.fromString("255.255.255.0");
    WifiIsClient = false;
    EEPROM.put(0, user_wifi);
    EEPROM.commit();
    WiFi.mode(WIFI_AP);
    WiFi.softAP("MBCAN Setup Portal", "1234567890");
  }
  else 
  {
    // Versuche Anmneldung am gespeicherten Router 
    
    WiFi.mode(WIFI_STA);
    WiFi.config(user_wifi.ip, user_wifi.dns, user_wifi.gateway, user_wifi.subnet);
    WiFi.begin(user_wifi.ssid, user_wifi.password);
    WifiIsClient = true;
    byte tries = 0;

    while (WiFi.status() != WL_CONNECTED) 
    {
     delay(1000);

     // Wenn nach 15s keine Anmeldung möglich war dann starte internen Access-Point
     // zur ggf. Neuparametrierung der WLAN/IP-Funktionen

     if (tries++ > 15) 
     {
       WiFi.mode(WIFI_AP);
       WiFi.softAP("MBCAN Setup Portal", "1234567890");
       WifiIsClient = false;
       break;
      }

    }
  }

  // Starte Webserver

  WEBserver.on("/",  handlePortal);
  WEBserver.begin();

  // Starte den TCP-Server wenn im Client-Modus
        
  if (WifiIsClient) TCPserver.begin(PORT);
    
  // Verbindungsanzeige (LED 2) initialisieren
    
  pinMode(LED, OUTPUT);
    
} // SETUP-Ende

//*******************************************************************************
//* Standard-Main-Loop-Routine                                                  *
//*******************************************************************************

void loop() 
{ 
  WEBserver.handleClient();
    
  // Loesche Anzeige fuer Verbindung

  digitalWrite(LED, HIGH);

  // Wenn im Client-Modus dann Stream auswerten

  if ( WifiIsClient )
  {

    // Aktiviere eine ggf. vorhandene Clientanfrage 
  
    WiFiClient TCPclient = TCPserver.available();

    // Wenn ein Client aktiviert ist, dann ermoegliche die Datenauswertung

    if ( TCPclient )
    {
      // Zeige Verbindung an
      
      digitalWrite(LED, LOW);

      // Synchronisiere Zaehler fuer die Datenbytes

      byte_tcp = 0;
      byte_uart = 0;

      // Ist der Client noch mit dem PC verbunden?
      // Wenn ja, mache weiter mit der Datenauswertung
      // Wenn nein, dann loesche die Aktivierung des Client

      while ( TCPclient && TCPclient.connected() )
      {
        // Liegen TCP-Daten vor?
      
        if ( TCPclient.available()>0 ) 
        {
          // Wenn ja, dann hole Byte
        
          tcp[byte_tcp] = TCPclient.read();
       
          byte_tcp++;
                
          // Wenn 13 Bytes empfangen wurden
          // sende Nachricht über die UART an das Modul
          
          if ( byte_tcp == 13 )
          {
            for (int count = 0; count < 25; count+=2)
            {
              uartstring[count] = mbc_hex2ascii((tcp[count/2]&0xf0)>>4);
              uartstring[count+1] = mbc_hex2ascii((tcp[count/2]&0x0f));
            }
           
            // Sendepuffer frei?
            
            while(Serial.availableForWrite()<30)
            {
       
            }
            
            // Uebertrage Stream
            
            Serial.write(uartstring,28);
                   
            // Setze Zaehler wieder auf 0
           
            byte_tcp = 0;
          }
        }

        // Liegen UART-Daten vor?
        
        if ( Serial.available()>0 ) 
        {
          // Dann hole UART-Date und lege sie im Buffer ab
  
          uart[byte_uart] = Serial.read();

          // Wurde '\n' empfangen?

          if ( uart[byte_uart] == '\n' )
          {
            // Wurden 28 Zeichen empfangen?
            // 13 x 2 Bytes + 1 x '\r' + 1 x '\n'
            // Dann sende Nachricht via TCP
         
            if ( byte_uart == 27 )
            {
              // Baue TCP-Nachricht zusammen
              // Wandle dazu jeweils zwei fortlaufende Nibble aus dem UART-Stream zu einem Byte zusammen
              // Im Stream steht immer zuerst der High-Nibble, dann der Low-Nibble als ASCII-Wert kodiert

              for ( int count = 0; count < 25; count += 2 )
              {
                tcpstring[count/2] = (  (( ascii2hex(uart[count]) << 4 ) + ascii2hex(uart[count+1])) );
              }
              
              // Sendepuffer frei fuer den gesamten Datenstring?
              // Wenn NULL zurueckgegeben wird, dann ist die Verbindung weg
              // Beende dann die LOOP und starte neu
              
              while(TCPclient.availableForWrite()<15)
              {
                if (TCPclient.availableForWrite()==0) 
                {
                  // Loesche Anzeige fuer Verbindung

                  digitalWrite(LED, HIGH);
                
                  return;
                }
              }

              // Uebertrage Stream
              
              TCPclient.write(tcpstring,13);
              TCPclient.flush(); 

            }

            // Quittiere Verarbeitung

            // Serial.write("1\r\n");

            // Setze Zaehler wieder auf 0
 
            byte_uart = 0;
          }
          else
          {
            // Kein '\n' empfangen?
            // Dann erhoehe Zaehler
            // Maximum erreicht ohne dass '\n' empfangen wurde?
            // Dann setze Zaehler zurueck
      
            if ( ++byte_uart == 29 ) byte_uart = 0;   
          }
          
        } // Ende if (Serial.available())
        
      } // Ende While(TCPclient.connect())-Schleife

    } // Ende if (TCPclient)

    // Loesche Anzeige fuer Verbindung

    digitalWrite(LED, HIGH);
  
    // Stoppe Client

    TCPclient.stop();

  }
     
} // LOOP-Ende

//*******************************************************************************
//* Behandle Webserver-Anfragen                                                 *
//*******************************************************************************

void handlePortal() {
  
  // Wenn nur Aufruf der Webseite, dann poste Eingabemaske
  // sonst verarbeite HTTP_POST
  
  if ((WEBserver.method() == HTTP_POST)) {

    // Sichere Webdaten im EEPROM
    
    strncpy(user_wifi.ssid,     WEBserver.arg("ssid").c_str(),     sizeof(user_wifi.ssid) );
    strncpy(user_wifi.password, WEBserver.arg("password").c_str(), sizeof(user_wifi.password) );
    user_wifi.ssid[WEBserver.arg("ssid").length()] = user_wifi.password[WEBserver.arg("password").length()] = '\0';
    user_wifi.ip.fromString(WEBserver.arg("ipv4").c_str());
    user_wifi.dns.fromString(WEBserver.arg("dnsv4").c_str());
    user_wifi.gateway.fromString(WEBserver.arg("gwv4").c_str());
    EEPROM.put(0, user_wifi);
    EEPROM.commit();

    // Sende Bestaetigung an aufrufende Webseite

    WEBserver.send(200,   "text/html", wifiok_1 + String(user_wifi.ssid) + wifiok_2 + String(user_wifi.ip[0]) + "." + String(user_wifi.ip[1]) + "." + String(user_wifi.ip[2]) + "." + String(user_wifi.ip[3]) + wifiok_3);
 
    // Warte 5s und starte dann den ESP neu

    delay(5000);
    ESP.restart();

  } else {

    WEBserver.send(200,   "text/html", wifisetup_1 + String(user_wifi.ssid) + wifisetup_2 + String(user_wifi.ip[0]) + "." + String(user_wifi.ip[1]) + "." + String(user_wifi.ip[2]) + "." + String(user_wifi.ip[3]) + wifisetup_3);
  }
}

//*******************************************************************************
//* Wandle ASCII in 4-Bit-HEX-Wert um                                           *
//* digit:    ASCII (0...9 und A...F)                                           *
//* Return:   Umgewandelter Wert auf HEX-Basis 0x0...0xf                        *
//*******************************************************************************

int ascii2hex(int digit)
{
  if (digit < 0x3a)
  { 
    return(digit - 0x30);
  }
  else
  { 
    return(digit - 0x37); 
  }
}

//*******************************************************************************
//* Wandle 4-Bit-HEX-Wert in ASCII um                                           *
//* digit:    4-Bit-Wert auf HEX-Basis 0x0...0xf                                *
//* Return:   Umgewandelter Wert als ASCII (0...9 und A...F)                    *
//*******************************************************************************

uint8_t mbc_hex2ascii(uint8_t digit)
{
  if (digit < 10)
  { 
    return(0x30 + digit);
  }
  else
  { 
    return(0x37 + digit); 
  }
}


        
