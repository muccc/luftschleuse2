#ifndef LCD_HAL_H_
#define LCD_HAL_H_

#include <stdint.h>
#include <stdbool.h>

#define LCD_CS_PORT     B
#define LCD_CS_PIN      1

#define LCD_SCK_PORT    B
#define LCD_SCK_PIN     0

#define LCD_SDA_PORT    B
#define LCD_SDA_PIN     3

#define LCD_RST_PORT    B
#define LCD_RST_PIN     4

void lcdhal_delayms(uint16_t ms);
void lcdhal_select(void);
void lcdhal_deselect(void);
void lcdhal_write(uint8_t cd, uint8_t data);
uint8_t lcdhal_read(uint8_t data);
void lcdhal_init(void);
#endif
