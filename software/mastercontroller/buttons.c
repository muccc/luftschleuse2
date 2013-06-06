#include "buttons.h"
#include "pinutils.h"

static uint16_t buttons_0_counter;
static uint16_t buttons_1_counter;
static uint16_t buttons_2_counter;
static uint16_t buttons_3_counter;

static uint8_t buttons_latched_state;

void buttons_init(void)
{
    DDR_CONFIG_IN(BUTTON_0);
    PIN_SET(BUTTON_0);

    DDR_CONFIG_IN(BUTTON_1);
    PIN_SET(BUTTON_1);

    DDR_CONFIG_IN(BUTTON_2);
    PIN_SET(BUTTON_2);

    DDR_CONFIG_IN(BUTTON_3);
    PIN_SET(BUTTON_3);

    buttons_0_counter = 0;
    buttons_1_counter = 0;
    buttons_2_counter = 0;
    buttons_3_counter = 0;
    buttons_latched_state = 0;
}

uint8_t buttons_getButtonsLatchedState(void)
{
    return buttons_latched_state;
}

bool buttons_getButtonState(buttons_button_t button)
{
    switch( button ){
        case BUTTON_0:
            return !PIN_HIGH(BUTTON_0);
        break;
        case BUTTON_1:
            return !PIN_HIGH(BUTTON_1);
        break;
        case BUTTON_2:
            return !PIN_HIGH(BUTTON_2);
        break;
        case BUTTON_3:
            return !PIN_HIGH(BUTTON_3);
        break;
    }
    return false;
}

void buttons_tick(void)
{
    //TODO: generalize for more buttons
    if( buttons_getButtonState(BUTTON_0) ){
        buttons_latched_state |= BUTTON_0;
        buttons_0_counter = 2000;
    }

    if( buttons_0_counter && --buttons_0_counter == 0 ){
        buttons_latched_state &= ~BUTTON_0;
    }

    if( buttons_getButtonState(BUTTON_1) ){
        buttons_latched_state |= BUTTON_1;
        buttons_1_counter = 2000;
    }

    if( buttons_1_counter && --buttons_1_counter == 0 ){
        buttons_latched_state &= ~BUTTON_1;
    }

    if( buttons_getButtonState(BUTTON_2) ){
        buttons_latched_state |= BUTTON_2;
        buttons_2_counter = 2000;
    }

    if( buttons_2_counter && --buttons_2_counter == 0 ){
        buttons_latched_state &= ~BUTTON_2;
    }

    if( buttons_getButtonState(BUTTON_3) ){
        buttons_latched_state |= BUTTON_3;
        buttons_2_counter = 2000;
    }

    if( buttons_3_counter && --buttons_3_counter == 0 ){
        buttons_latched_state &= ~BUTTON_3;
    }

}

