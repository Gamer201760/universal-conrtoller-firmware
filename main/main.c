#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "core/core.h"

static const char *TAG = "MAIN";

void app_main(void)
{
    pwm_timer_init();
    adc_init();
    wifi_init();
    mqtt_init();
}