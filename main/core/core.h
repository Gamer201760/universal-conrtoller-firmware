#pragma once

#include "driver/gpio.h"
#include "driver/ledc.h"
#include "mqtt_client.h"


void wifi_init();
void pwm_timer_init();
void mqtt_init();
void handler(char *topic, int len, void *data);
void set_state(gpio_num_t pin, bool state);
void set_pwm(gpio_num_t pin, ledc_channel_t channel, uint32_t duty);
void publisher(esp_mqtt_client_handle_t *client);
void publisher_stop();
void adc_init();
void adc_pin_init(gpio_num_t pin);
void adc_pin_read(gpio_num_t pin, int *data);