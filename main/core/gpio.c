#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_continuous.h"

#define ADC_UNIT ADC_UNIT_1
#define ADC_ATTEN ADC_ATTEN_DB_12

const char* GPIO_TAG = "GPIO";

adc_oneshot_unit_handle_t adc_handle;
const adc_oneshot_chan_cfg_t config = {
    .bitwidth = ADC_BITWIDTH_DEFAULT,
    .atten = ADC_ATTEN,
};

void set_state(gpio_num_t pin, bool state){
    ESP_ERROR_CHECK(gpio_set_direction(pin, GPIO_MODE_OUTPUT));
    ESP_ERROR_CHECK(gpio_set_level(pin, state));
}

void pwm_timer_init(){
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_LOW_SPEED_MODE,
        .timer_num        = LEDC_TIMER_0,
        .duty_resolution  = LEDC_TIMER_11_BIT,
        .freq_hz          = 4000,
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));
}

void set_pwm(gpio_num_t pin, ledc_channel_t channel, uint32_t duty){
    ESP_ERROR_CHECK(ledc_set_pin(pin, LEDC_LOW_SPEED_MODE, channel));
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, channel, duty));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, channel));
}

void adc_init(){
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config, &adc_handle));
}

void adc_pin_init(gpio_num_t pin){
    adc_channel_t channel;
    adc_unit_t unit_id;
    if (adc_continuous_io_to_channel(pin, &unit_id, &channel) != ESP_OK || unit_id != ADC_UNIT){
        ESP_LOGE(GPIO_TAG, "%d adc pin is not valid, please select another", pin);
        return;
    }
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, channel, &config));
}

void adc_pin_read(gpio_num_t pin, int *data){
    adc_channel_t channel;
    adc_unit_t unit_id;

    if (adc_continuous_io_to_channel(pin, &unit_id, &channel) != ESP_OK || unit_id != ADC_UNIT){
        ESP_LOGE(GPIO_TAG, "%d adc pin is not valid, please select another", pin);
        return;
    }
    ESP_ERROR_CHECK(adc_oneshot_read(adc_handle, channel, data));
}