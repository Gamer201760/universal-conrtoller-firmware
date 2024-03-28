#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "string.h"
#include "utils/parser.h"
#include "core.h"

const char *HANDLER_TAG = "HANDLER";

void _gpio_handler(void *pvParameters)
{
    char *payload = pvParameters;
    gpio_data data;
    if (parse_gpio_data(&data, payload) == ESP_OK){
        set_state(data.pin, data.state);
    }
    free(payload);
    free(&data);
    vTaskDelete(NULL);
}

void _pwm_handler(void *pvParameters)
{
    char *payload = pvParameters;
    pwm_data data;
    if (parse_pwm_data(&data, payload) == ESP_OK){
        set_pwm(data.pin, data.channel, data.duty);
    }
    free(payload);
    free(&data);
    vTaskDelete(NULL);
}

void handler(char *topic, int len, void *data)
{
    if (strncmp(topic, "controller/gpio", len) == 0)
    {
        xTaskCreate(_gpio_handler, "GPIO_HANDLER", 2048, data, tskIDLE_PRIORITY, NULL);
    } else if (strncmp(topic, "controller/gpio/pwm", len) == 0)
    {
        xTaskCreate(_pwm_handler, "PWM_HANDLER", 2048, data, tskIDLE_PRIORITY, NULL);
    }
}