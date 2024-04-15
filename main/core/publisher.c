#include "freertos/FreeRTOS.h"
#include "mqtt_client.h"
#include "esp_log.h"
#include "publisher.h"

static const char *TAG = "PUB";

TaskHandle_t BTNHandler = NULL;
TaskHandle_t ADCHandler = NULL;
TaskHandle_t DS18X20Handler = NULL;


void publisher(esp_mqtt_client_handle_t *client)
{
    if (ADCHandler == NULL){
        xTaskCreate(adc_publisher, "ADC_PUB", configMINIMAL_STACK_SIZE * 4, (void *)*client, tskIDLE_PRIORITY, &ADCHandler);
        ESP_LOGI(TAG, "ADC task start");
    } else {
        vTaskResume(ADCHandler);
        ESP_LOGI(TAG, "ADC task resume");
    }
    if (BTNHandler == NULL){
        xTaskCreate(btn_publisher, "BTN_PUB", configMINIMAL_STACK_SIZE * 4, (void *)*client, tskIDLE_PRIORITY, &BTNHandler);
        ESP_LOGI(TAG, "BTN task start");
    } else {
        vTaskResume(BTNHandler);
        ESP_LOGI(TAG, "BTN task resume");
    }
    if (DS18X20Handler == NULL){
        xTaskCreate(ds18x20_publisher, "ds18x20_PUB", configMINIMAL_STACK_SIZE * 4, (void *)*client, tskIDLE_PRIORITY, &DS18X20Handler);
        ESP_LOGI(TAG, "DS18X20 task start");
    } else {
        vTaskResume(DS18X20Handler);
        ESP_LOGI(TAG, "DS18X20 task resume");
    }
}

void publisher_stop(){
    ESP_LOGI(TAG, "Stop tasks");
    if (BTNHandler != NULL){
        vTaskSuspend(BTNHandler);
        ESP_LOGI(TAG, "BTN task stop");
    }
    if (ADCHandler != NULL){
        vTaskSuspend(ADCHandler);
        ESP_LOGI(TAG, "ADC task stop");
    }
    if (DS18X20Handler != NULL){
        vTaskSuspend(DS18X20Handler);
        ESP_LOGI(TAG, "DS18X20 task stop");
    }
}