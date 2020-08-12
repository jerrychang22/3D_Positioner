#include <stdio.h>

#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"

#include "Board.h"
#include "Networking.h"
#include "Encoder.h"

Encoder enc_yaw     = Encoder(17, 16);
Encoder enc_pitch   = Encoder(23, 22); 
Encoder enc_dist    = Encoder(18, 19);

int yaw_read, pitch_read, dist_read;

static void ReadEncoderTask(void* args){
    yaw_read = pitch_read = dist_read = 0;

    while(1){    
        yaw_read = (int) enc_yaw.GetCount();
        pitch_read = (int) enc_pitch.GetCount();
        dist_read = (int) enc_dist.GetCount();
        //vTaskDelay(10/portTICK_PERIOD_MS);
    }
}

static void SendDataTask(void* args){
    esp_websocket_client_handle_t client = (esp_websocket_client_handle_t) args;
    char data[128];
    int len;
    while(1){
        //Output to console
        //printf("Encoder readings (Yaw, Pitch, Dist) : [%d , %d, %d]\n", yaw_read, pitch_read, dist_read);

        //Send via Websocket
        len = sprintf(data,"Encoder readings (Yaw, Pitch, Dist) : [%d , %d, %d]", yaw_read, pitch_read, dist_read);
        
        ///*
        if (!esp_websocket_client_is_connected(client)) {
            printf("Websocket client not connected\n");
        }
        else{
            esp_websocket_client_send_text(client, data, len, portMAX_DELAY); 
        }   
        //*/
        //vTaskDelay(100/portTICK_PERIOD_MS);
    }

}

extern "C" void app_main(){
    
    //Initialization for network purposes
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    tcpip_adapter_init();
    esp_event_loop_create_default();


    //Board info
    PrintBoardInfo();

    //Network
    ConnectToWifi();
    esp_websocket_client_handle_t client = SetupWebsocketClient();

    //Start encoder reading task
    xTaskCreatePinnedToCore(&ReadEncoderTask, "ReadEncoderTask", 2048, NULL, 5, NULL, 1);
    xTaskCreatePinnedToCore(&SendDataTask, "SendDataTask", 2048, (void*)client, 5, NULL, 1);

    vTaskDelete(NULL);
}
