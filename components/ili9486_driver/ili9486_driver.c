
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_err.h"
#include "transactions.h"
#include "ili9486_driver.h"
#include "driver/gpio.h"
#include "stdlib.h"

/*=========================Static function declearation section start=========================*/
static int x_gap = 0;
static int y_gap = 0;
/*=========================Static function declearation section end=========================*/

/*=========================Public function definition section start=========================*/
esp_err_t init_ili9486_helper_pins(); // Initialize WR, RD, CS, RS and RST pins

void ili9486_init()
{
    // ILI9486 fresh start
    const uint8_t reset_cmd[1] = {0x01};
    send_command(reset_cmd, sizeof(reset_cmd));
    vTaskDelay(150 / portTICK_PERIOD_MS);

    const uint8_t display_off_cmd[1] = {0x28};
    send_command(display_off_cmd, sizeof(display_off_cmd));

    const uint8_t interface_pixel_format[1] = {0x3A};
    send_command(interface_pixel_format, sizeof(interface_pixel_format));
    const uint8_t interface_pixel_format_data[1] = {0x55};
    send_data(interface_pixel_format_data, sizeof(interface_pixel_format_data));

    // ILI9486 wakeup
    const uint8_t display_sleep_out[1] = {0x11};
    send_command(display_sleep_out, sizeof(display_sleep_out));
    vTaskDelay(150 / portTICK_PERIOD_MS);

    const uint8_t display_on_cmd[1] = {0x29};
    send_command(display_on_cmd, sizeof(display_on_cmd));

    // ILI9486 set following register values
    // Interface mode control
    const uint8_t interface_mode_control[1] = {0xB0};
    send_command(interface_mode_control, sizeof(interface_mode_control));
    const uint8_t interface_mode_control_data[1] = {0x00};
    send_data(interface_mode_control_data, sizeof(interface_mode_control_data));

    // Frame rate control 0xB3
    const uint8_t frame_rate_control_cmd[1] = {0xB3};
    send_command(frame_rate_control_cmd, sizeof(frame_rate_control_cmd));
    const uint8_t frame_rate_control_data[4] = {0x02, 0x00, 0x00, 0x00};
    send_data(frame_rate_control_data, sizeof(frame_rate_control_data));

    // Display inversion control 0xB4
    const uint8_t inversion_control_cmd[1] = {0xB4};
    send_command(inversion_control_cmd, sizeof(inversion_control_cmd));
    const uint8_t inversion_control_data[1] = {0x00};
    send_data(inversion_control_data, sizeof(inversion_control_data));

    // Display function control 0xB6
    const uint8_t display_function_control_cmd[1] = {0xB6};
    send_command(display_function_control_cmd, sizeof(display_function_control_cmd));
    const uint8_t display_function_control_data[3] = {0x02, 0x42, 0x3B};
    send_data(display_function_control_data, sizeof(display_function_control_data));

    // NV memory write 0xD0
    const uint8_t nv_memory_write_cmd[1] = {0xD0};
    send_command(nv_memory_write_cmd, sizeof(nv_memory_write_cmd));
    const uint8_t nv_memory_write_data[3] = {0x07, 0x42, 0x18};
    send_data(nv_memory_write_data, sizeof(nv_memory_write_data));

    // NV Memory Protection Key 0xD1
    const uint8_t nv_memory_protection_key_cmd[1] = {0xD1};
    send_command(nv_memory_protection_key_cmd, sizeof(nv_memory_protection_key_cmd));
    const uint8_t nv_memory_protection_key_data[3] = {0x00, 0x07, 0x81};
    send_data(nv_memory_protection_key_data, sizeof(nv_memory_protection_key_data));

    // NV Memory Status Read 0xD2
    const uint8_t nv_memory_read_status_cmd[1] = {0xD2};
    send_command(nv_memory_read_status_cmd, sizeof(nv_memory_read_status_cmd));
    const uint8_t nv_memory_read_status_data[2] = {0x01, 0x02};
    send_data(nv_memory_read_status_data, sizeof(nv_memory_read_status_data));

    // // Read ID4 0xD3
    const uint8_t read_id4_cmd[1] = {0xD3};
    send_command(read_id4_cmd, sizeof(read_id4_cmd));
    const uint8_t read_id4_data[2] = {0x01, 0x02};
    send_data(read_id4_data, sizeof(read_id4_data));

    // Set Power for Idle Mode [01 22] 0xD4
    const uint8_t ideal_power_cmd[1] = {0xD4};
    send_command(ideal_power_cmd, sizeof(ideal_power_cmd));
    const uint8_t ideal_power_data[2] = {0x01, 0x02};
    send_data(ideal_power_data, sizeof(ideal_power_data));

    // Power Control 1 0xC0
    const uint8_t power_control_1_cmd[1] = {0xC0};
    send_command(power_control_1_cmd, sizeof(power_control_1_cmd));
    const uint8_t power_control_1_data[5] = {0x14, 0x3B, 0x00, 0x02, 0x11};
    send_data(power_control_1_data, sizeof(power_control_1_data));

    // Power Control 2 0xC1
    const uint8_t power_control_2_cmd[1] = {0xC1};
    send_command(power_control_2_cmd, sizeof(power_control_2_cmd));
    const uint8_t power_control_2_data[3] = {0x10, 0x10, 0x88};
    send_data(power_control_2_data, sizeof(power_control_2_data));

    // VCOM Control 1 0xC5
    const uint8_t VCOM_control_cmd[1] = {0xC5};
    send_command(VCOM_control_cmd, sizeof(VCOM_control_cmd));
    const uint8_t VCOM_control_data[1] = {0x03};
    send_data(VCOM_control_data, sizeof(VCOM_control_data));

    // CABC Control 1 0xC6
    const uint8_t CABC_control_1_cmd[1] = {0xC6};
    send_command(CABC_control_1_cmd, sizeof(CABC_control_1_cmd));
    const uint8_t CABC_control_1_data[1] = {0x02};
    send_data(CABC_control_1_data, sizeof(CABC_control_1_data));

    // CABC Control 2 0xC8
    const uint8_t CABC_control_2_cmd[1] = {0xC8};
    send_command(CABC_control_2_cmd, sizeof(CABC_control_2_cmd));
    const uint8_t CABC_control_2_data[12] = {0x00, 0x32, 0x36, 0x45, 0x06, 0x16, 0x37, 0x75, 0x77, 0x54, 0x0C, 0x00};
    send_data(CABC_control_2_data, sizeof(CABC_control_2_data));

    // CABC Control 6 0xCC
    const uint8_t CABC_control_6_cmd[1] = {0xCC};
    send_command(CABC_control_6_cmd, sizeof(CABC_control_6_cmd));
    const uint8_t CABC_control_6_data[1] = {0x00};
    send_data(CABC_control_6_data, sizeof(CABC_control_6_data));

    // Memory Access Control 0x36
    const uint8_t memory_access_control_cmd[1] = {0x36};
    send_command(memory_access_control_cmd, sizeof(memory_access_control_cmd));
    const uint8_t memory_access_control_data[1] = {0x18};
    send_data(memory_access_control_data, sizeof(memory_access_control_data));
    ESP_LOGI(__FILE__, "[%d]: ILI9486 initialized successfully.", __LINE__);
}

void ili9486_draw_function(int x_start, int y_start, int x_end, int y_end, const void *color_map)
{
    assert((x_start < x_end) && (y_start < y_end));
    x_start += x_gap;
    x_end += y_gap;
    y_start += y_gap;
    y_end += y_gap;

    uint8_t col_sel[1] = {0x2A};
    send_command(col_sel, 1);
    const uint8_t col_location_data[4] = {(x_start >> 8) & 0xFF, 
                                        x_start & 0xFF, 
                                        ((x_end - 1) >> 8) & 0xFF, 
                                        (x_end - 1) & 0xFF};
    send_data(col_location_data, 4);


    uint8_t row_sel[1] = {0x2B};
    send_command(row_sel, 1);
    const uint8_t row_location_data[4] = {(y_start >> 8) & 0xFF, 
                                        y_start & 0xFF, 
                                        ((y_end - 1) >> 8) & 0xFF, 
                                        (y_end - 1) & 0xFF};
    send_data(row_location_data, 4);

    size_t len = ((x_end - x_start) * (y_end - y_start) * 16) / 8;
    printf("length %d\n", len);
    // uint8_t meme_write[1] = {0x2C};
    // send_command(meme_write, 1);
    // send_data((uint8_t*)color_map, len);
}

/*=========================Public function definition section end=========================*/

/*=========================Static function definition section start=========================*/

/*=========================Static function definition section end=========================*/