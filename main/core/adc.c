#include "freertos/FreeRTOS.h"
#include "mqtt_client.h"
#include "utils/parser.h"
#include "core.h"

void adc_publisher(void *params)
{
    esp_mqtt_client_handle_t client = params;

    int adc_pins[] = {33, 35, 36};
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