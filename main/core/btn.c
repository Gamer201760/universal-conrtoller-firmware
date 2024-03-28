#include "freertos/FreeRTOS.h"
#include "mqtt_client.h"
#include "utils/parser.h"
#include "core.h"

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
            char *payload = malloc(32);
            
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
