#include <inttypes.h>
#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <esp_err.h>
#include <ds18x20.h>
#include "mqtt_client.h"
#include "utils/parser.h"

static const gpio_num_t SENSOR_GPIO = 32;
static const int MAX_SENSORS = 4;
static const uint32_t LOOP_DELAY_MS = 500;

static const char *TAG = "ds18x20";

void ds18x20_publisher(void *params)
{
    esp_mqtt_client_handle_t client = params;
    char topic[27] = "controller/sensors/temp";
    
    onewire_addr_t addrs[MAX_SENSORS];
    float temps[MAX_SENSORS];
    size_t sensor_count = 0;
    uint8_t errors = 0;

    gpio_set_pull_mode(SENSOR_GPIO, GPIO_PULLUP_ONLY);

    esp_err_t res;
    res = ds18x20_scan_devices(SENSOR_GPIO, addrs, MAX_SENSORS, &sensor_count);
    while (res != ESP_OK && errors < 10)
    {
        ESP_LOGE(TAG, "Sensors scan error %d (%s) %d", res, esp_err_to_name(res), errors);
        res = ds18x20_scan_devices(SENSOR_GPIO, addrs, MAX_SENSORS, &sensor_count);
        errors++;
    }

    if (!sensor_count)
    {
        ESP_LOGE(TAG, "No sensors detected! Exiting...");
        vTaskDelete(NULL);
        return;
    }

    ESP_LOGI(TAG, "%d sensors detected", sensor_count);

    if (sensor_count > MAX_SENSORS)
        sensor_count = MAX_SENSORS;

    errors = 0;
    while (1)
    {
        if (errors > 10)
            break;
        res = ds18x20_measure_and_read_multi(SENSOR_GPIO, addrs, sensor_count, temps);
        if (res != ESP_OK)
        {
            ESP_LOGE(TAG, "Sensors read error %d (%s)", res, esp_err_to_name(res));
            errors++;
            continue;
        }

        for (int i = 0; i < sensor_count; i++)
        {
            itoa(i, topic + 23, 10);
            char *payload = malloc(32); 
            serialize_ds18x20(&temps[i], &payload);
            esp_mqtt_client_publish(client, topic, payload, 0, 0, 0);

            free(payload);
        }
        vTaskDelay(pdMS_TO_TICKS(LOOP_DELAY_MS));
    }
    vTaskDelete(NULL);
}
