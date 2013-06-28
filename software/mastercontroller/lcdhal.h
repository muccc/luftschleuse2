#ifndef LCD_HAL_H_
#define LCD_HAL_H_

#include <stdint.h>
#include <stdbool.h>

#define CS 2,1
#define SCK 2,11
#define SDA 0,9
#define RST 2,2

void lcdhal_delayms(uint8_t ms);
void lcdhal_select(void);
void lcdhal_deselect(void);
void lcdhal_write(uint8_t cd, uint8_t data);
uint8_t lcdhal_read(uint8_t data);
void lcdhal_init(void);
#endif
