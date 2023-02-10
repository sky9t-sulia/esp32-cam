#ifndef WEB_SERVER_H_
#define WEB_SERVEr_H_

#include "esp_http_server.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "camera_config.h"

#define PART_BOUNDARY "123456789000000000000987654321"
httpd_handle_t setup_server(void);

#endif