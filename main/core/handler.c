#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "string.h"
#include "utils/parser.h"
#include "core.h"

static const char *TAG = "HANDLER";

void _gpio_handler(void *pvParameters)
{
    char *payload = pvParameters;
    gpio_data *data = malloc(sizeof(gpio_data));
    if (parse_gpio_data(data, &payload) == ESP_OK){
        set_state(data->pin, data->state);
    }
    if (data != NULL){
        free(data);
    }
    vTaskDelete(NULL);
}

void _pwm_handler(void *pvParameters)
{
    char *payload = pvParameters;
    pwm_data *data = malloc(sizeof(pwm_data));
    if (parse_pwm_data(data, &payload) == ESP_OK){
        set_pwm(data->pin, data->channel, data->duty);
    }
    if (data != NULL){
        free(data);
    }
    vTaskDelete(NULL);
}

void handler(char *topic, int len, void *data)
{
    if (strncmp(topic, "controller/gpio/ctr", len) == 0)
    {
        xTaskCreate(_gpio_handler, "GPIO_HANDLER", 2048, data, tskIDLE_PRIORITY, NULL);
        // _gpio_handler(data);
    } else if (strncmp(topic, "controller/gpio/pwm", len) == 0)
    {
        xTaskCreate(_pwm_handler, "PWM_HANDLER", 2048, data, tskIDLE_PRIORITY, NULL);
        // _pwm_handler(data);
    }
}