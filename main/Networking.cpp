#include "Networking.h"

const char* WIFI_SSID = "6 Guys 1D";
const char* WIFI_PASS = "EveryCupIsAPongCup";

const char* WEBSOCKET_URI = "ws://192.168.1.49";
const int WEBSOCKET_PORT = 2222;

static int s_retry_num = 0;
static EventGroupHandle_t s_wifi_event_group;
static void event_handler(void* arg, esp_event_base_t event_base,int32_t event_id, void* event_data){
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < 10) {
            esp_wifi_connect();
            s_retry_num++;
            printf("Wifi connection failed. Retying\n");
        } else {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        printf("Retried a bunch. Failed\n");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        printf("got ip:%s\n",
                 ip4addr_ntoa(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

void ConnectToWifi(){
    s_wifi_event_group = xEventGroupCreate(); 

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL);

    wifi_config_t wifi_config = {};
    strcpy((char*)wifi_config.sta.ssid, WIFI_SSID);
    //wifi_config.sta.ssid = WIFI_SSID;
    strcpy((char*)wifi_config.sta.password, WIFI_PASS);
    //wifi_config.sta.password = WIFI_PASS;
	wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
    wifi_config.sta.pmf_cfg.capable = true;
    wifi_config.sta.pmf_cfg.required = false; 

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config);
    esp_wifi_start();
    esp_wifi_connect();
    printf("Attempted to connect to Wifi\n");
    
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
            WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
            pdFALSE,
            pdFALSE,
            portMAX_DELAY);

    if (bits & WIFI_CONNECTED_BIT) {
        printf("connected to ap SSID:%s\n", WIFI_SSID);
    } else if (bits & WIFI_FAIL_BIT) {
        printf("Failed to connect to SSID:%s\n", WIFI_SSID);
    } else {
        printf("UNEXPECTED EVENT\n");
    }

    /*
    tcpip_adapter_ip_info_t ipInfo; 
    char ipdata[256];
    	
    // IP address.
    tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_STA, &ipInfo);
    printf(ipdata, "%x", ipInfo.ip.addr);
    */

    esp_event_handler_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler);
    esp_event_handler_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler);
    vEventGroupDelete(s_wifi_event_group);
}

esp_websocket_client_handle_t SetupWebsocketClient(){
    esp_websocket_client_config_t websocket_cfg = {};
    websocket_cfg.uri = WEBSOCKET_URI;
    websocket_cfg.port = WEBSOCKET_PORT;
    esp_websocket_client_handle_t client = esp_websocket_client_init(&websocket_cfg);
    esp_websocket_client_start(client);
    printf("Attempted to connect to websocket\n");
    return client;
}


