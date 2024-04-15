#include "cJSON.h"
#include "esp_log.h"
#include "stdbool.h"
#include "parser.h"

const char* PARSER_TAG = "PARSER";
const char* PWM_DATA_MEMBERS[] = {"pin", "duty", "channel"};
const char* GPIO_DATA_MEMBERS[] = {"pin", "state"};

bool validate_json(const cJSON *object, const char *members[], const int len){
	for (int i = 0; i < len; i++){
		if (!cJSON_HasObjectItem(object, members[i])){
			ESP_LOGE(PARSER_TAG, "NOT FOUND %s in MSG", members[i]);
			return 0;
		}
	}
	return 1;
}

esp_err_t parse_gpio_data(gpio_data *data, char **payload){
    cJSON *root = cJSON_Parse(*payload);
	if (!validate_json(root, GPIO_DATA_MEMBERS, 2)){
		return ESP_FAIL;
	}

	data->pin = cJSON_GetObjectItem(root, "pin")->valueint;
	data->state = cJSON_GetObjectItem(root, "state")->valueint;
	cJSON_Delete(root);
	return ESP_OK;
}

esp_err_t parse_pwm_data(pwm_data *data, char **payload){
    cJSON *root = cJSON_Parse(*payload);
	if (!validate_json(root, PWM_DATA_MEMBERS, 3)){
		return ESP_FAIL;
	}
	data->pin = cJSON_GetObjectItem(root, "pin")->valueint;
	data->duty = cJSON_GetObjectItem(root, "duty")->valueint;
	data->channel = cJSON_GetObjectItem(root, "channel")->valueint;
	cJSON_Delete(root);
	return ESP_OK;
}

void serialize_btn(int *data, char **payload){
	sprintf(*payload, "{\"state\": %d}", *data);
}

void serialize_adc(int *data, char **payload){
	sprintf(*payload, "{\"value\": %d}", *data);
}

void serialize_ds18x20(float *data, char **payload){
	sprintf(*payload, "{\"value\": %.3f}", *data);
}
