#pragma once

#include <ESP8266WebServerSecure.h> //*SSL*
#include <DNSServer.h> //*SSL*
#include "SSLCert.hpp" //*SSL*

#define serverPort 443 //*SSL* HTTPS port is 443 (default)
#define DNS_PORT 53 //DNS port is 53 (default)

BearSSL::ESP8266WebServerSecure server(serverPort); //*SSL*set the Secure Server as server with out port
BearSSL::ServerSessions serverCache(5); //*SSL* cache
DNSServer dnsServer; //*SSL*

void ServerConfig(){
  configTime(3 * 3600, 0, "pool.ntp.org"); //Set time via NTP for x.509 validation
  MDNS.addService("https", "tcp", serverPort); //*SSL*set mDNS services for HTTPS
  server.getServer().setRSACert(new BearSSL::X509List(serverCert), new BearSSL::PrivateKey(serverKey)); //*SSL*
  server.getServer().setCache(&serverCache); //*SSL* Cache SSL sessions to accelerate the TLS handshake.
}
