/*
  WiFiEsp example: ConnectWPA

  This example connects to an encrypted WiFi network using an ESP8266 module.
  Then it prints the  MAC address of the WiFi shield, the IP address obtained
  and other network details.
  For more details see: http://yaab-arduino.blogspot.com/p/wifiesp-example-connect.html
*/
#include <WiFiEsp.h>
#include <WiFiEspClient.h>
#include <WiFiEspUdp.h>
//#include "SoftwareSerial.h"
#include <PubSubClient.h>
#include <TimeAlarms.h>
#include <Time.h>

#include <ArduinoJson.h>
char ssid[] = "Local-AVOCARBON";            // your network SSID (name)
char pass[] = "a52e6f74d56bcd85496bb451edac";        // your network password
/*char ssid[] = "Androidbarbas";            // your network SSID (name)
  char pass[] = "ceciestmaclef";        // your network password*/
int status = WL_IDLE_STATUS;     // the Wifi radio's status
// Initialize the Ethernet client object
WiFiEspClient espClient;
PubSubClient client(espClient);
//String  A8, A9, A10, A11, A12, A13, A14, A15;
int InitTrame [8];
int StatusTrame [8];
int PieceBonne = 0;
int PieceMauvaise = 0;
int tampon = -1;
String message = "";
#include <SPI.h>
#include <SD.h>
char buf[20];
File myFile;
File myFile2;
char* str = new char[300];
// set up variables using the SD utility library functions:
Sd2Card card;
SdVolume volume;
SdFile root;

// change this to match your SD shield or module;
// Arduino Ethernet shield: pin 4
// Adafruit SD shields and modules: pin 10
// Sparkfun SD shield: pin 8
const int chipSelect = 8;
char* cfg = new char[300];
int boucle_read = 0 ;
DynamicJsonBuffer jsonBuffer;
int tmp;
void setup()
{


  InitTrame[0] = 1;
  InitTrame[1] = 1;
  InitTrame[2] = 0;
  InitTrame[3] = 0;
  InitTrame[4] = 0;
  InitTrame[5] = 0;
  InitTrame[6] = 0;
  InitTrame[7] = 0;
  if (initTrame() == -1)
  {
    Serial.println("initTrame");
  }
  setTime(01, 00, 00, 14, 05, 2018);
  // initialize serial for debugging
  Serial.begin(9600);
  // initialize serial for ESP module
  Serial1.begin(115200);
  // initialize ESP module
  WiFi.init(&Serial1);
  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
    //while (true);
  }
  Serial.print("Initializing SD card...");

  if (!SD.begin(8)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");

  // re-open the file for reading:
  myFile = SD.open("config.txt");
  if (myFile) {
    Serial.println("config.txt:");
    // read from the file until there's nothing else in it:
    int i = 0;
    while (myFile.available()) {
      cfg[i] = myFile.read();
      i++;
    }
    Serial.println(cfg);
    // close the file:
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening config.txt");
  }
  JsonObject& root = jsonBuffer.parseObject(cfg);
  const char*  sssid = root["ssid"];
  const char* pass = root["pass"];
  Serial.println(sssid);
  Serial.println(pass);
  // attempt to connect to WiFi network
  //while ( status != WL_CONNECTED) {
  Serial.print("Attempting to connect to WPA SSID: ");
  Serial.println(sssid);
  // Connect to WPA/WPA2 network
  status = WiFi.begin(sssid, pass);
  //}

  Serial.println("You're connected to the network");
  const char* MqttAdress = root["MqttAdress"];
  uint16_t MqttPort = root["MqttPort"];
  client.setServer(MqttAdress, MqttPort);
  client.setCallback(callback);
  Serial.println("Setup done");
  byte mac[6];
  WiFi.macAddress(mac);
  sprintf(buf, "%02X:%02X:%02X:%02X:%02X:%02X", mac[5], mac[4], mac[3], mac[2], mac[1], mac[0]);

}

void loop()
{
  // print the network connection information every 10 seconds
  //printCurrentNet();
  //printWifiData();
  tmp = initTrame();
  if (tmp != tampon)
  {
    switch ( tmp )
    {
      case 0:
        Serial.println(""); tampon = 0; SendMessage(); break;
      case 1:
        Serial.println(""); tampon = 1; SendMessage(); break;
      case 2:
        Serial.println("PieceBonne : "); PieceBonne++; tampon = 2; break;//Serial.print(PieceBonne); Serial.print( "hours :" ); Serial.print(hour()) ; Serial.print("minutes :" ); Serial.print(minute()); Serial.print("days :"); Serial.print(day());
      case 3:
        Serial.println("PieceMauvaise : "); PieceMauvaise++; tampon = 3; Serial.print(PieceMauvaise); break;
      case 4:
        Serial.println("missPm1"); tampon = 4; SendMessage(); break;
      case 5:
        Serial.println("missPm2"); tampon = 5; SendMessage(); break;
      case 6:
        Serial.println("Maintenance"); tampon = 6; SendMessage(); break;
      case 7:
        Serial.println("Stop");  tampon = 7; SendMessage(); break;
      default :
        Serial.println("Production"); tampon = -1; break;
    }
  }
  if (PieceBonne > 10)
  {
    SendMessage();
    PieceBonne = 0;
  }


}
void SendMessage()
{

  Serial.println("Envoie Message");
  Serial.println("formater Message");
  formatage(str);
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  Serial.println("Message formater");

  if (client.connect("arduinoClient")) {
    Serial.println("connecter");
    if (client.publish("/avocarbon/test/", str)) {
      if (tmp > 3) {
        myFile = SD.open("data.txt");
        Serial.println(myFile.size());
        if( sizeof (cfg)>1){
          free(cfg);
        }
        cfg = new char[myFile.size()];
        if (myFile) {
          Serial.println("data.txt old value:");
          // read from the file until there's nothing else in it:
          boucle_read = 0;
          while (myFile.available()&&boucle_read<(myFile.size()+1)) {
            cfg[boucle_read] = myFile.read();
            boucle_read++;
          }
          Serial.println(boucle_read);
          // close the file:
          myFile.close();
        } else {
          // if the file didn't open, print an error:
          Serial.println("error opening data.txt");
        }

        if (boucle_read > 3 ) {
          myFile = SD.open("data.txt");
          char* chars_array = new char[myFile.size()+1];
          myFile.close();
          int fail = 0;
          chars_array = strtok(cfg, "}}");
          while (chars_array != NULL)
          {
            strcat(chars_array, "}}");
            if (client.publish("/avocarbon/test/", chars_array)) {
              Serial.println("publish");
              chars_array = strtok("}}", "}}");
            }
            else
            {
              strcat(str, chars_array);
              fail++;
              Serial.println("fail");
            }
          }
          free(chars_array);
          if (fail > 0)
          {
            myFile = SD.open("data.txt", FILE_WRITE);
            // if the file opened okay, write to it:
            if (myFile) {
              Serial.print("Writing to data.txt...");
              myFile.println(str);
              // close the file:
              myFile.close();
              Serial.println("done.");
            } else {
              // if the file didn't open, print an error:
              Serial.println("error opening data.txt");
            }
          }
          free(chars_array);
        }
        Serial.println(str);
        free(str);
      }
      Serial.println(str);
      free(str);
    }
  }
  else
  {
    myFile = SD.open("data.txt", FILE_WRITE);
    // if the file opened okay, write to it:
    if (myFile) {
      Serial.print("Writing to data.txt...");
      Serial.println(str);
      myFile.println(str);
      // close the file:
      myFile.close();
      Serial.println("done.");
    } else {
      // if the file didn't open, print an error:
      Serial.println("error opening data.txt");
    }

    free(str);
  }
  //client.subscribe("/avocarbon/test/");


  //free(cfg);
  Serial.println("Message Envoye");
  PieceBonne = 0;

}

void formatage(char* strr)
{
  Serial.print("en cours");
  char tmp[10];
  sprintf(tmp, "%d", PieceBonne );
  strcpy(strr, "{ \"date\":\"\", \"nbPieceBonne\":\"");
  strcat(strr, tmp); strcat(strr, "\",\"Mac\":\"");
  strcat(strr, buf);
  strcat(strr, "\",\"nbPieceMauvaise\":\"\",\"trame\":{\"entry0\":\""); sprintf(tmp, "%d", StatusTrame[0] ); strcat(strr, tmp);
  strcat(strr, "\",\"input1\":\""); sprintf(tmp, "%d", StatusTrame[1] ); strcat(strr, tmp);
  strcat(strr, "\",\"input2\":\""); sprintf(tmp, "%d", StatusTrame[2] ); strcat(strr, tmp);
  strcat(strr, "\",\"input3\":\""); sprintf(tmp, "%d", StatusTrame[3] ); strcat(strr, tmp);
  strcat(strr, "\",\"input4\":\""); sprintf(tmp, "%d", StatusTrame[4] ); strcat(strr, tmp);
  strcat(strr, "\",\"input5\":\""); sprintf(tmp, "%d", StatusTrame[5] ); strcat(strr, tmp);
  strcat(strr, "\",\"input6\":\""); sprintf(tmp, "%d", StatusTrame[6] ); strcat(strr, tmp);
  strcat(strr, "\",\"input7\":\""); sprintf(tmp, "%d", StatusTrame[7] ); strcat(strr, tmp);
  strcat(strr, "\"}} ");
  //return strr;
  Serial.print("finis");
}
int initTrame()
{
  int i = 0;
  int fail = -1;
  while (i < 8)
  {
    if (InitTrame[i] != ((analogRead(i) > 512)))
    {
      fail = i;
    }
    StatusTrame[i]  = ((analogRead(i) > 512));
    i++  ;
  }
  return fail;

}
void printWifiData()
{
  // print your WiFi shield's IP address
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print your MAC address
  byte mac[6];
  WiFi.macAddress(mac);
  char buf[20];
  sprintf(buf, "%02X:%02X:%02X:%02X:%02X:%02X", mac[5], mac[4], mac[3], mac[2], mac[1], mac[0]);
  Serial.print("MAC address: ");
  Serial.println(buf);
}

void printCurrentNet()
{
  // print the SSID of the network you're attached to
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print the MAC address of the router you're attached to
  byte bssid[6];
  WiFi.BSSID(bssid);
  char buf[20];
  sprintf(buf, "%02X:%02X:%02X:%02X:%02X:%02X", bssid[5], bssid[4], bssid[3], bssid[2], bssid[1], bssid[0]);
  Serial.print("BSSID: ");
  Serial.println(buf);

  // print the received signal strength
  long rssi = WiFi.RSSI();
  Serial.print("Signal strength (RSSI): ");
  Serial.println(rssi);
}



void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    char receivedChar = (char)payload[i];
    Serial.print(receivedChar);


  }
  Serial.println();
}


void reconnect() {
  // Loop until we're reconnected
  // while (!client.connected()) {
  Serial.print("Attempting MQTT connection...");
  // Attempt to connect
  if (client.connect("ESP8266 Client")) {
    Serial.println("connected");
    // ... and subscribe to topic
    client.subscribe("ledStatus");
  } else {
    Serial.print("failed, rc=");
    Serial.print(client.state());
    Serial.println(" try again in 5 seconds");
    // Wait 5 seconds before retrying
    //delay(5000);
  }
  //}
}
