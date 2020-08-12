#ifndef NETWORKING_H
#define NETWORKING_H

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#include "esp_wifi.h"
#include "esp_websocket_client.h"

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

void ConnectToWifi();
esp_websocket_client_handle_t SetupWebsocketClient();

#endif
