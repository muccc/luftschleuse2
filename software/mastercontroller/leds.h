#ifndef LEDS_H_
#define LEDS_H_
#include <stdint.h>

#define LED_RED_0_PORT              A
#define LED_RED_0_PIN               0

#define LED_RED_1_PORT              A
#define LED_RED_1_PIN               3

#define LED_YELLOW_0_PORT           A
#define LED_YELLOW_0_PIN            1

#define LED_YELLOW_1_PORT           A
#define LED_YELLOW_1_PIN            4

#define LED_GREEN_0_PORT            A
#define LED_GREEN_0_PIN             2

//#define LED_GREEN_1_PORT            A
//#define LED_GREEN_1_PIN             5

typedef enum {
    LED_ON,
    LED_OFF,
    LED_BLINK_FAST,
    LED_BLINK_SLOW,
    LED_SHORT_FLASH
} leds_state_t;

typedef enum {
    LED_RED_0,
    LED_RED_1,
    LED_YELLOW_0,
    LED_YELLOW_1,
    LED_GREEN_0,
//    LED_GREEN_1,
} leds_led_t;

#define LEDS_COUNT  5

void leds_init(void);
uint8_t leds_getState(void);
void leds_set(leds_led_t led, leds_state_t state);
void leds_tick(void);
#endif
