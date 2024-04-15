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

/*
E (370144) transport_base: esp_tls_conn_write error, errno=No more processes
E (370144) mqtt_client: Writing failed: errno=11
*/

/*
E (150524) esp-tls-mbedtls: mbedtls_ssl_setup returned -0x7F00
E (150524) esp-tls: create_ssl_handle failed
E (150524) esp-tls: Failed to open new connection
E (150534) transport_base: Failed to open a new connection
E (150534) mqtt_client: Error transport connect
*/