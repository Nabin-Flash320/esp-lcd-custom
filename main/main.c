
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_err.h"
#include "stdlib.h"
#include "transactions.h"
#include "ili9486_driver.h"
#include "lvgl.h"
#include "esp_timer.h"

static void lvgl_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map)
{
    printf("HERE\n");
    // int offsetx1 = area->x1;
    // int offsetx2 = area->x2;
    // int offsety1 = area->y1;
    // int offsety2 = area->y2;
    // copy a buffer's content to a specific area of the display
    // ili9486_draw_function(offsetx1, offsety1, offsetx2 + 1, offsety2 + 1, color_map);
}

void create_button(lv_obj_t *screen)
{
    lv_obj_t *button = lv_btn_create(screen);
    lv_obj_set_pos(button, 100, 100);
    lv_obj_set_size(button, 120, 50);
    lv_obj_t *button_label = lv_label_create(button);
    lv_label_set_text(button_label, "Button");
    lv_obj_center(button_label);
}

static void increase_lvgl_tick(void *arg)
{
    /* Tell LVGL how many milliseconds has elapsed */
    lv_tick_inc(2);
}

void app_main()
{

    static lv_disp_draw_buf_t disp_buf; // contains internal graphic buffer(s) called draw buffer(s)
    static lv_disp_drv_t disp_drv;      // contains callback functions

    ESP_LOGI(__FILE__, "%d HERE", __LINE__);
    disp_init_transactions();
    ili9486_init();
    printf("%s:[%d]: Initializing LVGL\n", __FILE__, __LINE__);
    lv_init();

    lv_color_t *buffer1 = malloc(320 * 20 * sizeof(lv_color_t));
    assert(buffer1);
    lv_color_t *buffer2 = malloc(320 * 20 * sizeof(lv_color_t));
    assert(buffer2);

    lv_disp_draw_buf_init(&disp_buf, buffer1, buffer2, 320 * 20);
    printf("%s:[%d]: Registering display driver.\n", __FILE__, __LINE__);
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = 320;
    disp_drv.ver_res = 480;
    disp_drv.flush_cb = lvgl_flush_cb;
    disp_drv.draw_buf = &disp_buf;
    disp_drv.user_data = NULL;
    
    lv_disp_t *disp = lv_disp_drv_register(&disp_drv);
    const esp_timer_create_args_t lvgl_tick_timer_args = {
        .callback = &increase_lvgl_tick,
        .name = "lvgl_tick"
    };

    esp_timer_handle_t lvgl_tick_timer = NULL;
    ESP_ERROR_CHECK(esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(lvgl_tick_timer, 2 * 1000));

    lv_obj_t *screen = lv_disp_get_scr_act(disp);
    create_button(screen);
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(10));
        lv_timer_handler();
    }
    

}
