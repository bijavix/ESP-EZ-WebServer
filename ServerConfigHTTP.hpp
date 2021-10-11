#pragma once

#include <ESP8266WebServer.h> //now using wss

#define serverPort 80 //HTTP port is 80 (default)

ESP8266WebServer server(serverPort); //set the Web Server as server and with our port

void ServerConfig(){
  MDNS.addService("http", "tcp", serverPort); //set mDNS services
}
