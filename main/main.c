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

    // while (1)
    // {
    //     ESP_LOGI(TAG, "Taking picture...");
    //     camera_fb_t *pic = esp_camera_fb_get();

    //     // use pic->buf to access the image
    //     ESP_LOGI(TAG, "Picture taken! Its size was: %zu bytes", pic->len);
    //     esp_camera_fb_return(pic);

    //     vTaskDelay(5000 / portTICK_RATE_MS);
    // }
}