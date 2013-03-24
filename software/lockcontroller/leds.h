#ifndef LEDS_H_
#define LEDS_H_
#include <stdint.h>

#define LED_OPEN_PIN_PORT              C
#define LED_OPEN_PIN_PIN               0

typedef enum {
    LED_ON,
    LED_OFF,
    LED_BLINK_FAST,
    LED_BLINK_SLOW,
    LED_SHORT_FLASH
} leds_state_t;

typedef enum {
    LED_OPEN
} leds_led_t;

void leds_init(void);
uint8_t leds_getState(void);
void leds_set(leds_led_t led, leds_state_t state);
void leds_tick(void);
#endif
