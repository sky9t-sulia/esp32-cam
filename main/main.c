#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "wifi_config.h"
#include "camera_config.h"
#include "web_server.h"

static const char *TAG = "APP";

void app_main()
{
    esp_err_t err;

    connect_wifi();

    if (wifi_connect_status)
    {
        err = init_camera();
        if (err != ESP_OK)
        {
            printf("err: %s\n", esp_err_to_name(err));
            return;
        }
        setup_server();
        ESP_LOGI(TAG, "ESP32 CAM Web Server is up and running\n");
    }
    else
    {
        ESP_LOGI(TAG, "Failed to connected with Wi-Fi, check your network Credentials\n");
    }
}