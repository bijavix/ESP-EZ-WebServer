/*
bijavix - 2021
remake of my ESP8266Server from Q1 2019
You'll need Arduino: https://www.arduino.cc/en/software
And ESP8266 Package: http://arduino.esp8266.com/stable/package_esp8266com_index.json
Builtin Led: 2
Upload Speed: 115200
CPU Freq: 80MHz
Crystal Freq: 26MHz
Flash Size: 4MB (Should work with less but you might have problems with OTA Updates)
Flash Freq: 80MHz
Erase Flash: All Contents
Port: <ESP8266 OTA IP>

Ignore this, I was losing code as well as my mind.
5Aug:               Sketch 499893 bytes//global vars 35492 bytes
5Aug after cleanup: Sketch 488241 bytes//global vars 35372 bytes
5Aug but at 2AM:    Sketch 368601 bytes//global vars 34792 bytes (something's wrong, I can feel it!)
5Aug but at 3AM:    Sketch 499909 bytes//global vars 35492 bytes (Ok, now this makes sense)
5Aug now with http: Sketch 368601 bytes//global vars 34792 bytes
12Sep: Fixing everything cuz not working. (At least it's working with DDNS and proper SSL encryption)
17Sep: html finished and everything's working.
30Sep: Code cleaned and ready.
*/

//Import required libs
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h> //If not working, CHECK YOU FIREWALL and add the ota port
#include "users.hpp"
#include "htmlPages.hpp"
#include "ServerConfig.hpp"

const char* ssid = "SSID"; //WiFi SSID
const char* WPAPwd = "PWD"; //WiFi Password
const char* OTAPwd = "OTAPassword"; //Password for OTA Firmaware Updates
//const char* OTAHash = "014d616528efcdc559a03fa71b9a52a5"; //MD5 of the OTA Password
//#define DHCP //uncomment to use DHCP, otherwise will use the custom IPAddress settings
IPAddress ip(192,168,0,66); //Device IP Address
IPAddress gateway(192,168,0,1); //Router IP
IPAddress subnet(255,255,255,0); //Network Subnet
#define updatePort 8266 //OTA Port is 8266 (default)
const char* hostname = "esp8266ws"; //Name for the device used for OTA and mDNS
//uint8_t mac[6] {0x62, 0x6a, 0x76, 0x78, 0x16, 0x32}; //Custom MAC address (optional)
int espLed = LED_BUILTIN; //esp8266 on board led pin (HIGH->Off  LOW->On)

void setup(){
  pinMode(espLed, OUTPUT); //Set the on board led pin as output
  digitalWrite(espLed, LOW); //Blink the led at boot
  delay(500); 
  digitalWrite(espLed, HIGH);
  #ifdef mac //if mac is defined, set the custom mac address
    wifi_set_macaddr(0, const_cast<uint8*>(mac));
  #endif
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, WPAPwd); //connect to the wifi network
  #ifndef DHCP //if not using DHCP, use manual IP
    WiFi.config(ip, gateway, subnet); //set the custom IP settings
  #endif
  if(WiFi.waitForConnectResult() != WL_CONNECTED) {
    delay(2500); //successful wifi connection should be quick
    WiFi.beginWPSConfig(); //If not already connected, try using WPS
      while((WiFi.status() != WL_CONNECTED)){
        delay(90000); //If it couldn't connect, then restart
        ESP.restart();
      }
  }
  digitalWrite(espLed, LOW); //Turn on the led once connected to wifi

  //OTA firmware updater
  ArduinoOTA.setHostname(hostname); //Set the OTA device Name
  ArduinoOTA.setPort(updatePort); //Set the OTA Network Port
  ArduinoOTA.setPassword(OTAPwd); //Set the OTA Password
  //ArduinoOTA.setPasswordHash(OTAHash); //Set OTA pwd with a hash
  ArduinoOTA.begin(); //Start the OTA Service
  
  if(!MDNS.begin(hostname)){ //start the mDNS resolver
    return; 
    }

  ServerConfig(); //Set up the server for HTTP or HTTPS

  //SERVER WEB CODE -------------------------------------------------------
  //set the different web pages
  server.on("/", [](){server.send(200, "text/html", HTMLMain);});
  server.on("/robots.txt", [](){server.send(200, "text/html", robotsTXT);});
  //server.on("/bijavix", [](){server.send(200, "text/plain","Welcome to my ESP8266 Web Page. \n Made by bijavix");});
  server.on("/LED", setLED);
  server.on("/RR", [](){server.send(200, "text/html", HTMLRickRoll);});
  //server.on("/uptime", uptime);
  //server.on("/fss", [](){server.send(200, "text/plain", server.arg(ESP.getFreeSketchSpace()));});
  //server.on("/me", [](){server.send(200, "text/plain", server.arg(WiFiClient()));});
  server.on("/stop", stop);
  server.on("/restart", restart);
  
  /*
  server.on("/admin", []() { //Page with authentication
    if (!server.authenticate(user00, pass00)) {
      return server.requestAuthentication(DIGEST_AUTH, uMsg00, failedAuthMsg);
    }
    else{
      server.send(200, "text/plain", "wellcome admin");
    }
  });*/

  server.onNotFound([](){server.send(404,"text/plain", pageNotFound);});
  server.begin(); //start the server
}

void loop(){
  ArduinoOTA.handle(); //Handle Arduino Over The Air Firmware requests
  server.handleClient(); //Handle HTTP Resquests
  MDNS.update(); //Handle mDNS Requests
}

//WEB LINK ---------------------------------
void setLED(){
  if(digitalRead(espLed) == HIGH){
    digitalWrite(espLed, LOW); //Turn on the LED
    } 
  else{
    digitalWrite(espLed, HIGH); //Turn it off
    } 
  server.send(200, "text/plain", (digitalRead(espLed) ? "LED is ON" : "LED is OFF"));
}

/*
void uptime(){
  server.arg(millis());
  server.send(200, "text/plain", (millis()));
}*/

void restart(){
  server.send(200, "text/plain", "Restarting...");
  ESP.restart();
}
void stop(){
  server.stop();
  digitalWrite(espLed, HIGH); //Turn off the LED
}
