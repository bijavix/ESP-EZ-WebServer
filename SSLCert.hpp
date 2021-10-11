#pragma once

//You can use letsencrypt to get a free cert

static const char serverCert[] PROGMEM = R"uwu(
-----BEGIN CERTIFICATE-----
HTTPS CERT HERE
-----END CERTIFICATE-----
)uwu";

static const char serverKey[] PROGMEM = R"uwu(
-----BEGIN RSA PRIVATE KEY-----
HTTPS KEY HERE
-----END RSA PRIVATE KEY-----
)uwu";
