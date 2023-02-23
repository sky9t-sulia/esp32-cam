#ifndef SDMMC_HANDLER_H_
#define SDMMC_HANDLER_H_

#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "driver/sdmmc_host.h"

#define SD_MOUNT_POINT "/sdcard"

esp_err_t init_sdmmc();
void unmount_sdcard();
sdmmc_card_t *get_card();

#endif /* SDMMC_HANDLER_H_ */