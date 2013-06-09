#include "leds.h"
#include "pinutils.h"
#include <stdbool.h>
#include <stdint.h>

static leds_state_t leds_state[LEDS_COUNT];

void leds_init(void)
{
    DDR_CONFIG_OUT(LED_0);
    PIN_CLEAR(LED_0);

    DDR_CONFIG_OUT(LED_1);
    PIN_CLEAR(LED_1);

    DDR_CONFIG_OUT(LED_2);
    PIN_CLEAR(LED_2);

    uint8_t i;
    for(i=0; i<LEDS_COUNT; i++){
        leds_state[i] = LED_OFF;
    }

}

uint8_t leds_getState(void)
{
    return 0;
}

static void leds_set_pin(leds_led_t led, bool on)
{
    switch( led ){
        case LED_0:
            if( on ){
                PIN_SET(LED_0);
            }else{
                PIN_CLEAR(LED_0);
            }
        break;
        case LED_1:
            if( on ){
                PIN_SET(LED_1);
            }else{
                PIN_CLEAR(LED_1);
            }
        break;
        case LED_2:
            if( on ){
                PIN_SET(LED_2);
            }else{
                PIN_CLEAR(LED_2);
            }
        break;
    }
}

void leds_set(leds_led_t led, leds_state_t state)
{
    if( led < LEDS_COUNT)
        leds_state[led] = state;
}

void leds_tick(void)
{
    static uint16_t counter = 0;
    counter++;

    uint8_t i;
    for(i=0; i<LEDS_COUNT; i++){
        switch( leds_state[i] ){
            case LED_ON:
                leds_set_pin(i, true);
            break;
            case LED_OFF:
                leds_set_pin(i, false);
            break;
            case LED_SHORT_FLASH:
                if( (counter % 1024) < 50 ){
                    leds_set_pin(i, true);
                }else{
                    leds_set_pin(i, false);
                }
            break;
            case LED_BLINK_FAST:
                if( (counter % 256) < 128 ){
                    leds_set_pin(i, true);
                }else{
                    leds_set_pin(i, false);
                }
            break;
            case LED_BLINK_SLOW:
                if( (counter % 1024) < 512 ){
                    leds_set_pin(i, true);
                }else{
                    leds_set_pin(i, false);
                }
            break;

            default:
            break;
        }
    }
}
