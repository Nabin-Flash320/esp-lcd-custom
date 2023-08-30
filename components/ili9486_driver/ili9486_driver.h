

#ifndef __ILI9486_DRIVER_H__
#define __ILI9486_DRIVER_H__

#include "esp_err.h"

void ili9486_init();
void ili9486_draw_function(int x_start, int y_start, int x_end, int y_end, const void *color_map);

#endif // __ILI9486_DRIVER_H__
