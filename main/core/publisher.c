#include "freertos/FreeRTOS.h"
#include "mqtt_client.h"
#include "esp_log.h"
#include "publisher.h"

static const char *TAG = "PUB";

TaskHandle_t BTNHandler = NULL;
TaskHandle_t ADCHandler = NULL;


void publisher(esp_mqtt_client_handle_t *client)
{
    if (ADCHandler == NULL){
        xTaskCreate(adc_publisher, "ADC_PUB", 4096, (void *)*client, tskIDLE_PRIORITY, &ADCHandler);
        ESP_LOGI(TAG, "ADC task start");
    } else {
        vTaskResume(ADCHandler);
        ESP_LOGI(TAG, "ADC task resume");
    }
    if (BTNHandler == NULL){
        xTaskCreate(btn_publisher, "BTN_PUB", 4096, (void *)*client, tskIDLE_PRIORITY, &BTNHandler);
        ESP_LOGI(TAG, "BTN task start");
    } else {
        vTaskResume(BTNHandler);
        ESP_LOGI(TAG, "BTN task resume");
    }
}

void publisher_stop(){
    if (BTNHandler != NULL){
        vTaskSuspend(BTNHandler);
        ESP_LOGI(TAG, "BTN task stop");
    }
    if (ADCHandler != NULL){
        vTaskSuspend(ADCHandler);
        ESP_LOGI(TAG, "ADC task stop");
    }
}