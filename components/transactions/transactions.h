
#ifndef __TRANSACTIONS_H__
#define __TRANSACTIONS_H__

#include "stdint.h"
#include "string.h"

typedef struct s_transaction_pins{
    uint32_t data_pin_array[8];
    uint32_t rd_pin;
    uint32_t wr_pin; 
    uint32_t rs_pin;
    uint32_t cs_pin;
    uint32_t rst_pin;
}s_transaction_pins_t;

void disp_init_transactions();

void send_command(const uint8_t *command, size_t length);
void send_data(const uint8_t *data, size_t length);

#endif // __TRANSACTIONS_H__
