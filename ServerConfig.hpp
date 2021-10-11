#pragma once 
#define HTTPS //use HTTPS, comment to use HTTP Server

#ifdef HTTPS
#include "ServerConfigHTTPS.hpp"
#else
#include "ServerConfigHTTP.hpp"
#endif
