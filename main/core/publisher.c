#include "freertos/FreeRTOS.h"
#include "mqtt_client.h"
#include "esp_log.h"
#include "utils/parser.h"
#include "core.h"

const char *PUB_TAG = "PUB";

TaskHandle_t BTNHandler = NULL;
TaskHandle_t ADCHandler = NULL;

void btn_publisher(void *params)
{
    esp_mqtt_client_handle_t client = params;
    char topic[18] = "controller/gpio/";

    int btn_pins[] = {25, 26, 39};
    for (int i = 0; i < sizeof(btn_pins) / sizeof(btn_pins[0]); i++)
    {
        gpio_set_direction(btn_pins[i], GPIO_MODE_INPUT);
    }

    while (1)
    {
        for (int i = 0; i < sizeof(btn_pins) / sizeof(btn_pins[0]); i++)
        {
            int data;
            char *payload;
            payload = malloc(32);
            
            itoa(btn_pins[i], topic + 16, 10);
            data = gpio_get_level(btn_pins[i]);
            serialize_btn(&data, &payload);
            esp_mqtt_client_publish(client, topic, payload, 0, 0, 0);

            free(payload);
        }

        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

void adc_publisher(void *params)
{
    esp_mqtt_client_handle_t client = params;

    int adc_pins[] = {33, 32, 35, 36};
    char topic[18] = "controller/gpio/";
    for (int i = 0; i < sizeof(adc_pins) / sizeof(adc_pins[0]); i++)
    {
        adc_pin_init(adc_pins[i]);
    }

    while (1)
    {
        for (int i = 0; i < sizeof(adc_pins) / sizeof(adc_pins[0]); i++)
        {
            int data;
            char *payload;
            payload = malloc(32);

            itoa(adc_pins[i], topic + 16, 10);

            adc_pin_read(adc_pins[i], &data);
            serialize_adc(&data, &payload);

            esp_mqtt_client_publish(client, topic, payload, 0, 0, 0);

            free(payload);
        }

        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

void publisher(esp_mqtt_client_handle_t *client)
{
    if (ADCHandler == NULL){
        xTaskCreate(adc_publisher, "ADC_PUB", 4096, (void *)*client, tskIDLE_PRIORITY, &ADCHandler);
        ESP_LOGI(PUB_TAG, "ADC task start");
    } else {
        vTaskResume(ADCHandler);
        ESP_LOGI(PUB_TAG, "ADC task resume");
    }
    if (BTNHandler == NULL){
        xTaskCreate(btn_publisher, "BTN_PUB", 4096, (void *)*client, tskIDLE_PRIORITY, &BTNHandler);
        ESP_LOGI(PUB_TAG, "BTN task start");
    } else {
        vTaskResume(BTNHandler);
        ESP_LOGI(PUB_TAG, "BTN task resume");
    }
}

void publisher_stop(){
    if (BTNHandler != NULL){
        vTaskSuspend(BTNHandler);
        ESP_LOGI(PUB_TAG, "BTN task stop");
    }
    if (ADCHandler != NULL){
        vTaskSuspend(ADCHandler);
        ESP_LOGI(PUB_TAG, "ADC task stop");
    }
}