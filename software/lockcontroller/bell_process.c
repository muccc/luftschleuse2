#include "bell_process.h"
#include "pinutils.h"
#include <stdint.h>
#include <stdbool.h>

#define MIN_PRESS   50
#define MAX_PRESS   2500
#define MIN_BREAK   50
#define MAX_BREAK   2500

//static const int16_t times[] = {-200, -200, 500, -200, -200};
static const int16_t times[] = {-400, -400, 800, -400, -400};
static uint8_t step;
static uint8_t debounce;
static bool pressed;
static int16_t press_timer;
static int16_t break_timer;
static bool accepted;
static uint16_t c = 0;

void bell_init(void)
{
    DDR_CONFIG_IN(BELL);
    PIN_SET(BELL);
    step = 0;
    debounce = 0;
    pressed = false;
    press_timer = -1;
    break_timer = -1;
    accepted = false;
    c = 0;
}

static inline bool isPressed(void)
{
    if( !PIN_HIGH(BELL) ){
        return true;

    }
    return false;
}

void bell_tick(void)
{

    if( c && --c==0 ){
        accepted = false;
    }

    if( debounce == 0 && !pressed && isPressed() ){
        debounce = 50;
        pressed = true;
        press_timer = 0;
    }

    if( debounce == 0 && pressed && !isPressed() ){
        debounce = 50;
        pressed = false;
        break_timer = 0;
    }

    if( debounce ){
        debounce--;
    }

    if( pressed && (press_timer >= 0) ){
        press_timer++;
    }

    if( !pressed && (break_timer >= 0) ){
        break_timer++;
    }

 
    if( !pressed && press_timer > 0 ){
        if( times[step] > 0 ){
            if( press_timer > MIN_PRESS &&
                press_timer > times[step] ){
                step++;
            }else{
                step = 0;
            }
        }else{
            if( press_timer > MIN_PRESS &&
                press_timer < -times[step] ){
                step++;
            }else{
                step = 0;
            }
        }
        press_timer = -1;

        if( step * sizeof(times[0]) == sizeof(times) ){
            accepted = true;
            c = 2000;
        }
    }

    if( press_timer > MAX_PRESS ){
        press_timer = -1;
        step = 0;
    }

    if( break_timer > MAX_BREAK ){
        break_timer = -1;
        step = 0;
    }

}

bool bell_isAccepted(void)
{
    return accepted;
}

void bell_process(void)
{
}

