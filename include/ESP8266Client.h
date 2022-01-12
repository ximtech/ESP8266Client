#pragma once

#include "ESP8266WiFi.h"
#include "HTTPClient.h"

HTTP getESP8266HttpClient(WiFi *wifi);
void cleanESP8266Response(char *rawResponse);