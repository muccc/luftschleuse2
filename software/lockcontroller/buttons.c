#include "buttons.h"
#include "pinutils.h"

static uint8_t buttons_red_debounce;
static bool buttons_red_state;
static uint16_t buttons_red_counter;

static uint8_t buttons_pending;
static uint8_t buttons_toggle_state;
static uint8_t buttons_latched_state;

void buttons_init(void)
{
    DDR_CONFIG_IN(BUTTON_RED);
    PIN_SET(BUTTON_RED);
    buttons_pending = 0;
    buttons_red_state = false;
    buttons_red_debounce = 0;
    buttons_toggle_state = 0;
    buttons_red_counter = 0;
    buttons_latched_state = 0;
}

uint8_t buttons_getPendingButtons(void)
{
    return buttons_pending;
}

void buttons_clearPendingButtons(uint8_t buttons)
{
    buttons_pending &= ~(buttons);
}

uint8_t buttons_getButtonsToggleState(void)
{
    return buttons_toggle_state;
}

uint8_t buttons_getButtonsLatchedState(void)
{
    return buttons_latched_state;
}

bool buttons_getButtonState(buttons_button_t button)
{
    switch( button ){
        case BUTTON_RED:
            return !PIN_HIGH(BUTTON_RED);
        break;
    }
    return false;
}

void buttons_tick(void)
{
    //TODO: generalize for more buttons
    if( buttons_getButtonState(BUTTON_RED) ){
        if( buttons_red_state == false ){
            if( buttons_red_debounce++ > 100 ){
                buttons_red_state = true;
                buttons_red_debounce = 0;
                buttons_pending |= BUTTON_RED;
                buttons_toggle_state ^= BUTTON_RED;
                buttons_latched_state |= BUTTON_RED;
                buttons_red_counter = 2000;
            }
        }else{
            buttons_red_debounce = 0;
        }
    }else{
        if( buttons_red_state == true ){
            if( buttons_red_debounce++ > 100 ){
                buttons_red_state = false;
                buttons_red_debounce = 0;
            }
        }else{
            buttons_red_debounce = 0;
        }
    }

    if( buttons_red_counter && --buttons_red_counter == 0 ){
        buttons_latched_state &= ~(BUTTON_RED);
    }
}
