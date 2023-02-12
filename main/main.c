#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "wifi_config.h"
#include "camera_config.h"
#include "web_server.h"

#include "st7789.h"

static const char *TAG = "APP";

void view_finder_task(void *pv)
{
    esp_err_t err;
    err = init_camera();
    if (err != ESP_OK)
    {
        printf("err: %s\n", esp_err_to_name(err));
    }

    camera_fb_t *fb = NULL;
    st7789_t lcd = {
        .io_handle = NULL,
        .panel_handle = NULL};

    init_lcd(&lcd);

    printf("SCL %d, SDA %d, RES %d, DC %d, CS %d\n", CONFIG_SCLK_GPIO, CONFIG_MOSI_GPIO, CONFIG_RESET_GPIO, CONFIG_DC_GPIO, CONFIG_CS_GPIO);

    int buff_size = (sizeof(uint16_t) * LCD_HOR_RES * PARALLEL_LINES);

    uint16_t *line = (uint16_t *)heap_caps_malloc(buff_size, MALLOC_CAP_DMA);

    while (true)
    {
        fb = esp_camera_fb_get();

        if (fb)
        {
            int page = 0;
            for (int y = 0; y < LCD_VER_RES; y += PARALLEL_LINES)
            {
                memcpy(line, fb->buf + (page * buff_size), buff_size);
                esp_lcd_panel_draw_bitmap(lcd.panel_handle,
                                          0,
                                          y,
                                          LCD_HOR_RES,
                                          y + PARALLEL_LINES,
                                          line);
                page++;
            }
        }

        esp_camera_fb_return(fb);
    }
}

void app_main()
{
    xTaskCreate(view_finder_task, "View Finder Task", 20000, NULL, 1, NULL);

    // connect_wifi();

    // if (wifi_connect_status)
    // {
    //     setup_server();
    //     xTaskCreate(taskA, "taskA", 20048, NULL, 1, NULL);

    //     ESP_LOGI(TAG, "ESP32 CAM Web Server is up and running\n");
    // }
    // else
    // {
    //     ESP_LOGI(TAG, "Failed to connected with Wi-Fi, check your network Credentials\n");
    // }
}