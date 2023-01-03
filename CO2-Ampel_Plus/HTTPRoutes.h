#ifndef HTTP_ROUTES_H
#define HTTP_ROUTES_H

#include "WiFiClient.h"

void get_root(WiFiClient& client);
void get_api_sensor(WiFiClient& client);

#endif
