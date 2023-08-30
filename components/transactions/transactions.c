
#include "esp_err.h"
#include "stdbool.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "transactions.h"
#include "esp_log.h"

#if CONFIG_IDF_TARGET_ESP32S3
static const s_transaction_pins_t transaction_pins = {
    .data_pin_array = {14, 13, 12, 11, 10, 19, 46, 18},
    .rd_pin = 2,
    .wr_pin = 4,
    .rs_pin = 5,
    .cs_pin = 33,
    .rst_pin = 32,
};
#else
static const s_transaction_pins_t transaction_pins = {
    .data_pin_array = {12, 13, 26, 25, 21, 18, 23, 14},
    .rd_pin = 2,
    .wr_pin = 4,
    .rs_pin = 5,
    .cs_pin = 33,
    .rst_pin = 32,
};
#endif
static bool ili9486_transaction_inititalized = false;
static SemaphoreHandle_t transaction_mutex = NULL;

static esp_err_t disp_init_transaction_pins();
static esp_err_t init_ili9486_helper_pins();
static int disp_send_data(const uint8_t *data, size_t length);

void disp_init_transactions()
{
    if (!ili9486_transaction_inititalized)
    {
        ESP_LOGI(__FILE__, "%d %s", __LINE__, __func__);
        disp_init_transaction_pins();
        init_ili9486_helper_pins();
        transaction_mutex = xSemaphoreCreateMutex();
        ili9486_transaction_inititalized = true;
    }
}

void send_command(const uint8_t *command, size_t length)
{
    ESP_ERROR_CHECK(gpio_set_level(transaction_pins.wr_pin, 0));
    ESP_ERROR_CHECK(gpio_set_level(transaction_pins.cs_pin, 0));
    ESP_ERROR_CHECK(gpio_set_level(transaction_pins.rs_pin, 0));
    disp_send_data(command, length);
    ESP_ERROR_CHECK(gpio_set_level(transaction_pins.wr_pin, 1));
    ESP_ERROR_CHECK(gpio_set_level(transaction_pins.cs_pin, 1));
}
void send_data(const uint8_t *data, size_t length)
{
    ESP_ERROR_CHECK(gpio_set_level(transaction_pins.wr_pin, 0));
    ESP_ERROR_CHECK(gpio_set_level(transaction_pins.cs_pin, 0));
    ESP_ERROR_CHECK(gpio_set_level(transaction_pins.rs_pin, 1));
    disp_send_data(data, length);
    ESP_ERROR_CHECK(gpio_set_level(transaction_pins.wr_pin, 1));
    ESP_ERROR_CHECK(gpio_set_level(transaction_pins.cs_pin, 1));
}

static esp_err_t disp_init_transaction_pins()
{
    esp_err_t ret = ESP_OK;

    ESP_LOGI(__FILE__, "%d %s", __LINE__, __func__);
    for (int i = 0; i < 8; i++)
    {
        gpio_config_t config = {
            .pin_bit_mask = (1ULL << transaction_pins.data_pin_array[i]),
            .mode = GPIO_MODE_OUTPUT,
            .intr_type = GPIO_INTR_DISABLE,
            .pull_down_en = GPIO_PULLDOWN_ENABLE,
            .pull_up_en = GPIO_PULLUP_DISABLE,
        };
        gpio_config(&config);
    }
    return ret;
}

static esp_err_t init_ili9486_helper_pins()
{
    int ret = ESP_OK;
    gpio_config_t helper_config = {
        .pin_bit_mask = (1ULL << transaction_pins.rd_pin),
        .mode = GPIO_MODE_OUTPUT,
        .intr_type = GPIO_INTR_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
    };
    ESP_ERROR_CHECK(gpio_config(&helper_config));
    ESP_ERROR_CHECK(gpio_set_level(transaction_pins.rd_pin, 1));

    helper_config.pin_bit_mask = (1ULL << transaction_pins.wr_pin);
    ESP_ERROR_CHECK(gpio_config(&helper_config));
    ESP_ERROR_CHECK(gpio_set_level(transaction_pins.wr_pin, 1));

    helper_config.pin_bit_mask = (1ULL << transaction_pins.rs_pin);
    ESP_ERROR_CHECK(gpio_config(&helper_config));
    ESP_ERROR_CHECK(gpio_set_level(transaction_pins.rs_pin, 0));

    helper_config.pin_bit_mask = (1ULL << transaction_pins.cs_pin);
    ESP_ERROR_CHECK(gpio_config(&helper_config));
    ESP_ERROR_CHECK(gpio_set_level(transaction_pins.cs_pin, 1));

    helper_config.pin_bit_mask = (1ULL << transaction_pins.rst_pin);
    ESP_ERROR_CHECK(gpio_config(&helper_config));
    ESP_ERROR_CHECK(gpio_set_level(transaction_pins.rst_pin, 1));
    return ret;
}

static int disp_send_data(const uint8_t *data, size_t length)
{
    int ret = 0;
    if ((NULL == data) || (0 == length))
    {
        ret = -1;
    }
    else
    {
        if (xSemaphoreTake(transaction_mutex, portMAX_DELAY))
        {
            for (int i = 0; i < length; i++)
            {
                printf("0x%02X: [", data[i]);
                for (int j = 7; j >= 0; j--)
                {
                    printf("[%d]:", transaction_pins.data_pin_array[j]);
                    printf("%d ", (data[i] >> j) & 0x01);
                    // ESP_ERROR_CHECK(gpio_set_level(transaction_pins.data_pin_array[j], ((data[i] >> j) & 0x01)));
                }
                printf("]\n");
            }
            xSemaphoreGive(transaction_mutex);
        }
    }
    return ret;
}
