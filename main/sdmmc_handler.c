#include "sdmmc_handler.h"

static const char* TAG = "SDMMC";
static sdmmc_card_t *card;

esp_err_t init_sdmmc() {
    esp_err_t ret;

    // Options for mounting the filesystem.
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .max_files = 512,
#ifdef CONFIG_EXAMPLE_FORMAT_IF_MOUNT_FAILED
        .format_if_mount_failed = true,
#else
        .format_if_mount_failed = false,
#endif // EXAMPLE_FORMAT_IF_MOUNT_FAILED
        .allocation_unit_size = 16 * 1024
    };

    const char mount_point[] = SD_MOUNT_POINT;
    ESP_LOGI(TAG, "Initializing SD card");

    // Use settings defined above to initialize SD card and mount FAT filesystem.
    // Note: esp_vfs_fat_sdmmc/sdspi_mount is all-in-one convenience functions.
    // Please check its source code and implement error recovery when developing
    // production applications.
    ESP_LOGI(TAG, "Using SDMMC peripheral");
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();

    // This initializes the slot without card detect (CD) and write protect (WP) signals.
    // Modify slot_config.gpio_cd and slot_config.gpio_wp if your board has these signals.
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    
    // Set bus width to use:
    slot_config.width = 4;

    // On chips where the GPIOs used for SD card can be configured, set them in
    // the slot_config structure:
    #ifdef CONFIG_SOC_SDMMC_USE_GPIO_MATRIX
    slot_config.clk = CONFIG_SDMMC_PIN_CLK;
    slot_config.cmd = CONFIG_SDMMC_PIN_CMD;
    slot_config.d0 = CONFIG_SDMMC_PIN_DATA0;
    #if CONFIG_SDMMC_BUS_WIDTH_4
    slot_config.d1 = CONFIG_SDMMC_PIN_DATA1;
    slot_config.d2 = CONFIG_SDMMC_PIN_DATA2;
    slot_config.d3 = CONFIG_SDMMC_PIN_DATA3;
    #endif // CONFIG_SDMMC_BUS_WIDTH_4
    #endif // CONFIG_SOC_SDMMC_USE_GPIO_MATRIX

    // Enable internal pullups on enabled pins. The internal pullups
    // are insufficient however, please make sure 10k external pullups are
    // connected on the bus. This is for debug / example purpose only.
    slot_config.flags |= SDMMC_SLOT_FLAG_INTERNAL_PULLUP;

    ESP_LOGI(TAG, "Mounting filesystem");
    ret = esp_vfs_fat_sdmmc_mount(mount_point, &host, &slot_config, &mount_config, &card);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount filesystem. "
                     "If you want the card to be formatted, set the FORMAT_IF_MOUNT_FAILED menuconfig option.");
        } else {
            ESP_LOGE(TAG, "Failed to initialize the card (%s). "
                     "Make sure SD card lines have pull-up resistors in place.", esp_err_to_name(ret));
        }
        return ret;
    }
    ESP_LOGI(TAG, "Filesystem mounted");

    return ret;
}

void unmount_sdcard() {
    //Unmount partition and disable SDMMC peripheral

    const char mount_point[] = SD_MOUNT_POINT;
    esp_vfs_fat_sdcard_unmount(mount_point, card);
    ESP_LOGI(TAG, "Card unmounted");
}

sdmmc_card_t *get_card() {
    return card;
}