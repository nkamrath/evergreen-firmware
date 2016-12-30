#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "esp_ota_ops.h"
#include "nvs_flash.h"
#include "driver/gpio.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"

#include "pin.h"
#include "networking.h"
#include "light_controller.h"
#include "update_manager.h"

#include <stdio.h>
#include <string.h>

#define BLINK_GPIO 34
#define EXAMPLE_WIFI_SSID "ATT184"
#define EXAMPLE_WIFI_PASS "&nellie&"
#define BROADCAST_DATA "light_announce"

#define WRITE_STRING "TEST FROM ESP32"

static const char *TAG = "example";

/* FreeRTOS event group to signal when we are connected & ready to make a request */
static EventGroupHandle_t wifi_event_group;

/* The event group allows multiple bits for each event,
   but we only care about one event - are we connected
   to the AP with an IP? */
const int CONNECTED_BIT = BIT0;

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch(event->event_id) {
    case SYSTEM_EVENT_STA_START:
        esp_wifi_connect();
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        /* This is a workaround as ESP32 WiFi libs don't currently
           auto-reassociate. */
        esp_wifi_connect();
        xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
        break;
    default:
        break;
    }
    return ESP_OK;
}

static void initialise_wifi(void)
{
    tcpip_adapter_init();
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = EXAMPLE_WIFI_SSID,
            .password = EXAMPLE_WIFI_PASS,
        },
    };

    ESP_LOGI(TAG, "Setting WiFi configuration SSID %s...", wifi_config.sta.ssid);
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK( esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
    ESP_ERROR_CHECK( esp_wifi_start() );
}

void app_init(void *pvParameter)
{
    //initialize the wifi connection
    initialise_wifi();

    //wait for initialize wifi to get us a connection
    xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT, false, true, portMAX_DELAY);
    ESP_LOGI(TAG, "Connected to AP");

    UpdateManager_Create(NULL);
    udp_multicast_init();
    uint8_t loop_counter = 0;
    bool last_motion_sensor_state = false;
    //udp_broadcast(50008, (uint8_t*)"START", 5);
    printf("\r\nNEW FIRMWARE 5!!!!!!!!!!!!\r\n");
    while(1)
    {
        if(loop_counter == 9)
        {
            ESP_LOGI(TAG, "Broadcasting...");
            udp_broadcast(50007, (uint8_t*)BROADCAST_DATA, strlen(BROADCAST_DATA));
            loop_counter = 0;
        }
        else
        {
            loop_counter++;
        }

        udp_service_socket();
        vTaskDelay(100 / portTICK_RATE_MS);
        bool current_motion_sensor_state = Pin_ReadInput(MOTION_SENSOR_PIN_MASK);
        if(last_motion_sensor_state == false && current_motion_sensor_state == true)
        {
            printf("motion detected\r\n");
            LightController_MotionDetected();
        }
        last_motion_sensor_state = current_motion_sensor_state;
        LightController_Refresh();
    }

}

void app_main(void)
{

    nvs_flash_init();

    //setup blink pin
    gpio_pad_select_gpio(BLINK_GPIO);
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);

    gpio_config_t motion_sensor_pin_config = MOTION_SENSOR_PIN_CONFIG;
    Pin_Init(MOTION_SENSOR_PIN, &motion_sensor_pin_config);

    gpio_config_t light_relay_pin_config = LIGHT1_RELAY_PIN_CONFIG;
    Pin_Init(LIGHT1_RELAY_PIN, &light_relay_pin_config);
    Pin_ClearOutput(LIGHT1_RELAY_PIN_MASK);

    //setup debug pins
    gpio_config_t debug_pin1_config = DEBUG_PIN1_CONFIG;
    Pin_Init(DEBUG_PIN1, &debug_pin1_config);

    gpio_config_t debug_pin2_config = DEBUG_PIN2_CONFIG;
    Pin_Init(DEBUG_PIN1, &debug_pin2_config);

    gpio_config_t debug_pin3_config = DEBUG_PIN3_CONFIG;
    Pin_Init(DEBUG_PIN1, &debug_pin3_config);


    Pin_ClearOutput(DEBUG_PIN1_MASK | DEBUG_PIN2_MASK | DEBUG_PIN3_MASK);


    xTaskCreate(&app_init, "app_init", 4096, NULL, 5, NULL);

    while(1)
    {
        Pin_ClearOutput(DEBUG_PIN1_MASK | DEBUG_PIN2_MASK | DEBUG_PIN3_MASK);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }


}

