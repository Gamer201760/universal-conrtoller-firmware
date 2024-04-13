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

/* 
W (2009764) transport_base: Poll timeout or error, errno=Success, fd=54, timeout_ms=10000
E (2009764) mqtt_client: Writing didn't complete in specified timeout: errno=0
*/