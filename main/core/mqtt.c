#include "freertos/FreeRTOS.h"
#include "mqtt_client.h"
#include "esp_log.h"
#include "core.h"

#define MQTT_CONNECTED_BIT BIT1

static EventGroupHandle_t s_mqtt_event_group;
static const char* TAG = "MQTT";

extern const uint8_t mqtt_io_pem_start[]   asm("_binary_mqtt_server_pem_start");
extern const uint8_t mqtt_io_pem_end[]   asm("_binary_mqtt_server_pem_end");



static void log_error_if_nonzero(const char *message, int error_code)
{
    if (error_code != 0)
    {
        ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
    }
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%" PRIi32, base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    switch ((esp_mqtt_event_id_t)event_id)
    {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        msg_id = esp_mqtt_client_subscribe(client, "controller/gpio/ctr", 0);
        ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
        msg_id = esp_mqtt_client_subscribe(client, "controller/gpio/pwm", 0);
        ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
        xEventGroupSetBits(s_mqtt_event_group, MQTT_CONNECTED_BIT);
        publisher(&client);
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        publisher_stop();
        esp_mqtt_client_reconnect(client);
        break;
    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        handler(event->topic, event->topic_len, (void *)event->data);
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT)
        {
            log_error_if_nonzero("captured as transport's socket errno", event->error_handle->esp_transport_sock_errno);
            ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
        }
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}

void mqtt_init(){
    s_mqtt_event_group = xEventGroupCreate();
    const esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = CONFIG_MQTT_BROKER_URL,
        .broker.verification.certificate = (const char *)mqtt_io_pem_start,
        .credentials.authentication = {
            .password = CONFIG_MQTT_PASSWORD
        },
        .credentials.username = CONFIG_MQTT_USERNAME
    };
    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    ESP_ERROR_CHECK(esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client));
    ESP_ERROR_CHECK(esp_mqtt_client_start(client));
    xEventGroupWaitBits(
        s_mqtt_event_group,
        MQTT_CONNECTED_BIT,
        pdFALSE,
        pdFALSE,
        portMAX_DELAY
    );
}

