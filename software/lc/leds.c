#include "leds.h"
#include "pinutils.h"

static leds_state_t leds_open_state;
static uint16_t leds_counter;
void leds_init(void)
{
    DDR_CONFIG_OUT(LED_OPEN_PIN);
    PIN_SET(LED_OPEN_PIN);
    leds_open_state = LED_OFF;
    leds_counter = 0;
}

uint8_t leds_getState(void)
{
    return 0;
}

void leds_set(leds_led_t led, leds_state_t state)
{
    if( led == LED_OPEN ){
        leds_open_state = state;
    }
}

void leds_tick(void)
{
    leds_counter++;
    if( leds_open_state == LED_ON){
        PIN_SET(LED_OPEN_PIN);
    }else if( leds_open_state == LED_OFF ){
        PIN_CLEAR(LED_OPEN_PIN);
    }else if( leds_open_state == LED_SHORT_FLASH ){
        if( (leds_counter % 1024) < 50 ){
            PIN_SET(LED_OPEN_PIN);
        }else{
            PIN_CLEAR(LED_OPEN_PIN);
        }
    }

}
