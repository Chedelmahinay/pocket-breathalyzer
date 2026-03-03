#pragma once
#include <stdint.h>

#define LCD_WIDTH  128
#define LCD_HEIGHT 160
#define PARALLEL_LINES 8

void lcd_init_driver(void);
void lcd_display_image(uint16_t *img);
void lcd_display_black(void);

void draw_string_buf(uint16_t *buf, int x, int y, const char *s, uint16_t color);
void send_lines(int ypos, uint16_t *linedata);
void send_line_finish(void);

void lcd_show_bac_screen(void);
