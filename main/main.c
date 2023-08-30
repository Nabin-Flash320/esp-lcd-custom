
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
    int offsetx1 = area->x1;
    int offsetx2 = area->x2;
    int offsety1 = area->y1;
    int offsety2 = area->y2;
    printf("offsetx1: %d, offsetx2 : %d, offsety1: %d, offsety2: %d\n", offsetx1, offsetx2, offsety1, offsety2);
    // copy a buffer's content to a specific area of the display
    ili9486_draw_function(offsetx1, offsety1, offsetx2 + 1, offsety2 + 1, color_map);
}

void create_button(lv_obj_t *screen)
{
    static lv_style_t button_style;
    lv_style_init(&button_style);
    lv_style_set_radius(&button_style, 10);
    lv_style_set_bg_opa(&button_style, LV_OPA_COVER);
    lv_style_set_bg_color(&button_style, lv_palette_lighten(LV_PALETTE_GREY, 3));
    lv_style_set_bg_grad_color(&button_style, lv_palette_main(LV_PALETTE_GREY));
    lv_style_set_bg_grad_dir(&button_style, LV_GRAD_DIR_VER);
    lv_style_set_border_color(&button_style, lv_color_black());
    lv_style_set_border_opa(&button_style, LV_OPA_20);
    lv_style_set_border_width(&button_style, 2);
    lv_style_set_text_color(&button_style, lv_color_black());
    
    lv_obj_t *button = lv_btn_create(screen);
    lv_obj_set_pos(button, 100, 100);
    lv_obj_set_size(button, 120, 150);
    lv_obj_add_style(button, &button_style, 0);
    lv_obj_t *button_label = lv_label_create(button);
    lv_label_set_text(button_label, "Button");
    lv_obj_center(button_label);
}

static void increase_lvgl_tick(void *arg)
{
    /* Tell LVGL how many milliseconds has elapsed */
    lv_tick_inc(2);
}

static void lcd_task(void *param)
{
    static lv_disp_draw_buf_t disp_buf; // contains internal graphic buffer(s) called draw buffer(s)
    static lv_disp_drv_t disp_drv;      // contains callback functions

    disp_init_transactions();
    ili9486_init();
    printf("%s:[%d]: Initializing LVGL\n", __FILE__, __LINE__);
    vTaskDelay(pdMS_TO_TICKS(10));
    lv_init();

    lv_color_t *buffer1 = malloc(320 * 32 * sizeof(lv_color_t));
    assert(buffer1);
    lv_color_t *buffer2 = malloc(320 * 32 * sizeof(lv_color_t));
    assert(buffer2);

    vTaskDelay(pdMS_TO_TICKS(10));
    lv_disp_draw_buf_init(&disp_buf, buffer1, buffer2, 320 * 480);
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
        .name = "lvgl_tick"};

    esp_timer_handle_t lvgl_tick_timer = NULL;
    ESP_ERROR_CHECK(esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(lvgl_tick_timer, 2));

    lv_obj_t *screen = lv_disp_get_scr_act(disp);
    create_button(screen);

    while (1)
    {
        vTaskDelay(10 / portTICK_PERIOD_MS);
        lv_timer_handler();
    }
}

void app_main()
{
    xTaskCreate(lcd_task, "lcd_task", 4 * 2048, NULL, 0, NULL);
}
