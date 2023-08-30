
#include "esp_log.h"
#include "esp_err.h"
#include "stdlib.h"
#include "transactions.h"
#include "ili9486_driver.h"
#include "lvgl.h"

void app_main()
{
    ESP_LOGI(__FILE__, "%d HERE", __LINE__);
    disp_init_transactions();
    ili9486_init();
    printf("%s:[%d]: Initializing LVGL\n", __FILE__, __LINE__);
    lv_init();

    lv_color_t *buffer1 = malloc(320 * 20 * sizeof(lv_color_t));
    assert(buffer1);
    lv_color_t *buffer2 = malloc(320 * 20 * sizeof(lv_color_t));
    assert(buffer2);
    
}
