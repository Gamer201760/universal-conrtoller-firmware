#pragma once

#include "esp_err.h"

typedef struct gpio_data
{
    int pin;
    int state;
} gpio_data;

typedef struct pwm_data
{
    int pin;
    int duty;
    int channel;
} pwm_data;

esp_err_t parse_gpio_data(gpio_data *data, char *payload);
esp_err_t parse_pwm_data(pwm_data *data, char *payload);
void serialize_btn(int *data, char **payload);
void serialize_adc(int *data, char **payload);