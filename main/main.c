#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "wifi_handler.h"
#include "camera_handler.h"
#include "httpd_handler.h"
#include "sdmmc_handler.h"
#include "freertos/queue.h"

#include "st7789.h"

#define QUEUE_LENGTH    20
QueueHandle_t frame_queue;

static const char *TAG = "APP";

void camera_task(void *pv) {
    esp_err_t err = init_camera();
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "%s", esp_err_to_name(err));
        vTaskDelete(NULL);
    }

    camera_fb_t *frame = NULL;
    while(true) {
        frame = esp_camera_fb_get();

        if(!frame) {
            continue;
        }

        if (xQueueSendToBack(frame_queue, &frame, portMAX_DELAY) != pdPASS) {

        }

        vTaskDelay(10 / portTICK_RATE_MS);
    }
}

// void view_finder_task(void *pv)
// {
//     st7789_t lcd = {
//         .io_handle = NULL,
//         .panel_handle = NULL};

//     init_lcd(&lcd);

//     printf("SCL %d, SDA %d, RES %d, DC %d, CS %d\n", CONFIG_SCLK_GPIO, CONFIG_MOSI_GPIO, CONFIG_RESET_GPIO, CONFIG_DC_GPIO, CONFIG_CS_GPIO);

//     int buff_size = (sizeof(uint16_t) * LCD_HOR_RES * PARALLEL_LINES);
//     uint16_t *line = (uint16_t *)heap_caps_malloc(buff_size, MALLOC_CAP_DMA);

//     while (true)
//     {
//             int page = 0;
//             for (int y = 0; y < LCD_VER_RES; y += PARALLEL_LINES)
//             {
//                 memcpy(line, fb->buf + (page * buff_size), buff_size);
//                 esp_lcd_panel_draw_bitmap(lcd.panel_handle,
//                                           0,
//                                           y,
//                                           LCD_HOR_RES,
//                                           y + PARALLEL_LINES,
//                                           line);
//                 page++;
//             }

//         esp_camera_fb_return(fb);
//     }
// }

void sd_card(void *pv) {
    esp_err_t err = init_sdmmc();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s", esp_err_to_name(err));
        vTaskDelete(NULL);
    }

    int i = 0;
    
    while(true) {
        camera_fb_t *frame = NULL;
        if (uxQueueMessagesWaiting(frame_queue) > 0) {
            if (xQueueReceive(frame_queue, &frame, portMAX_DELAY) == pdTRUE) {
                char filename[100];
                sprintf(filename, "/sdcard/cam-%d.jpg", i++);

                FILE* fp = fopen(filename, "w");
                if (fp == NULL) {
                    printf("Error opening file for write %s\n", filename);
                } else {
                    size_t size = fwrite(frame->buf, 1, frame->len, fp);
                    if (size == frame->len) {
                        fflush(fp);
                        fclose(fp);
                    }
                }

                esp_camera_fb_return(frame);
            }
        }
        vTaskDelay(10 / portTICK_RATE_MS);
    }
}

void app_main()
{
    frame_queue = xQueueCreate(QUEUE_LENGTH, sizeof(camera_fb_t *));

    xTaskCreate(camera_task, "Camera Task", 4096, NULL, tskIDLE_PRIORITY + 1, NULL);
    //xTaskCreate(view_finder_task, "View Finder Task", 20000, NULL, 1, NULL);
    xTaskCreate(sd_card, "SD Card Task", 4096, NULL, tskIDLE_PRIORITY + 1, NULL);

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