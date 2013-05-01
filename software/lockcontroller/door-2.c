#include "config.h"
#if DOOR_MODEL == DOOR_2
#include "door.h"
#include "pinutils.h"
#include <stdbool.h>
#include <stdint.h>

#include "leds.h"

static uint8_t door_doorstate;
static uint8_t door_desiredState;
static uint8_t door_barState;

enum {
    DOOR_IDLE,
    DOOR_LOCK,
    DOOR_LOCKING,
    DOOR_UNLOCK,
    DOOR_UNLOCKING,
} door_state;

enum {
    BAR_UNKNOWN,
    BAR_LOCKED,
    BAR_UNLOCKED,
} bar_state;


static void door_startLock(void)
{
    //PIN_SET(R1);
    PIN_SET(DOOR_HBRIDGE_IN1);
    PIN_SET(DOOR_HBRIDGE_ENABLE);
}

static void door_stopLock(void)
{
    PIN_CLEAR(DOOR_HBRIDGE_IN1);
    PIN_CLEAR(DOOR_HBRIDGE_IN2);
}

void door_init(void)
{
    door_doorstate = 0;
    DDR_CONFIG_IN(DOOR_REED_CONTACT);
    PIN_SET(DOOR_REED_CONTACT);

    DDR_CONFIG_IN(DOOR_LOCK_BRIDGE);
    PIN_SET(DOOR_LOCK_BRIDGE);

    DDR_CONFIG_IN(DOOR_LOCK_UNLOCKED_CONTACT);
    PIN_SET(DOOR_LOCK_UNLOCKED_CONTACT);

    DDR_CONFIG_IN(DOOR_LOCK_LOCKED_CONTACT);
    PIN_SET(DOOR_LOCK_LOCKED_CONTACT);

    DDR_CONFIG_IN(DOOR_HANDLE_CONTACT);
    PIN_SET(DOOR_HANDLE_CONTACT);

    DDR_CONFIG_IN(DOOR_DOOR_OPEN_CONTACT);
    PIN_SET(DOOR_DOOR_OPEN_CONTACT);

    PIN_SET(DOOR_HBRIDGE_ENABLE);
    PIN_CLEAR(DOOR_HBRIDGE_IN1);
    PIN_CLEAR(DOOR_HBRIDGE_IN2);
   
    DDR_CONFIG_OUT(DOOR_HBRIDGE_ENABLE);
    DDR_CONFIG_OUT(DOOR_HBRIDGE_IN1);
    DDR_CONFIG_OUT(DOOR_HBRIDGE_IN2);
 
    DDR_CONFIG_IN(DOOR_LOCK_CONTACT);
    PIN_SET(DOOR_LOCK_CONTACT);
    
    //PIN_CLEAR(R1);
    //DDR_CONFIG_OUT(R1);

    door_desiredState = DOOR_LOCK_LOCKED;
    door_barState = BAR_UNKNOWN;
    door_state = DOOR_LOCK;
}


static void door_update_inputs(void)
{
    door_doorstate &= ~(DOOR_DOOR_CLOSED | DOOR_LOCK_LOCKED | DOOR_HANDLE_PRESSED | DOOR_LOCK_UNLOCKED | DOOR_LOCK_MANUAL_UNLOCKED);
    //TODO: also query the reed contact here
    if( !PIN_HIGH(DOOR_DOOR_OPEN_CONTACT) )
        door_doorstate |= DOOR_DOOR_CLOSED;
    
    if( !PIN_HIGH(DOOR_HANDLE_CONTACT) )
        door_doorstate |= DOOR_HANDLE_PRESSED;
    
    if( !PIN_HIGH(DOOR_LOCK_LOCKED_CONTACT) )
        door_doorstate |= DOOR_LOCK_LOCKED;

    // TODO: These might be insufficient criteria!
    if( !PIN_HIGH(DOOR_LOCK_UNLOCKED_CONTACT) ){
        // The bar is retracted
        if( !PIN_HIGH(DOOR_LOCK_CONTACT) ){
            // The bar is retracted because we unlocked it
            door_doorstate |= DOOR_LOCK_UNLOCKED;
        }else{
            door_doorstate |= DOOR_LOCK_MANUAL_UNLOCKED;
        }
    }
}

//static uint8_t door_locked(void)
//{
//    return door_doorstate & DOOR_LOCK_LOCKED;
//}

//static uint8_t door_unlocked(void)
//{
//    return door_doorstate & DOOR_LOCK_UNLOCKED;
//}

static uint8_t door_closed(void)
{
    return door_doorstate & DOOR_DOOR_CLOSED;
}

static uint8_t door_handlePressed(void)
{
    return door_doorstate & DOOR_HANDLE_PRESSED;
}

static bool door_barSensor(void)
{
    if( PIN_HIGH(DOOR_LOCK_CONTACT) ){
        return true;
    }
    return false;
}

void door_tick(void)
{
    static uint16_t timeout = 0;
    static bool previousbarsensor = false;
    bool barsensor = door_barSensor();

    door_update_inputs();

    switch(door_state)
    {
        case DOOR_IDLE:
            if( door_desiredState == DOOR_LOCK_LOCKED && door_barState != BAR_LOCKED ){
                door_state = DOOR_LOCK;
            }else if( door_desiredState == DOOR_LOCK_UNLOCKED && door_barState != BAR_UNLOCKED ){
                door_state = DOOR_UNLOCK;
            }else if( door_barState == BAR_UNKNOWN ){
                door_state = DOOR_LOCKING;
            }
        break;
        case DOOR_LOCK:
            if( door_closed() && !door_handlePressed()){
                previousbarsensor = barsensor;
                door_startLock();
                door_state = DOOR_LOCKING;
                // Timeout in which the action must be
                // completed by the motor.
                timeout = 6000;
            }
        break;
        case DOOR_LOCKING:
            //if( door_locked() ){
            if( previousbarsensor == false && barsensor == true ){
                door_stopLock();
                door_state = DOOR_IDLE;
                timeout = 0;
                door_barState = BAR_LOCKED;
            }else if( --timeout == 0 ) {
                door_stopLock();
                // TODO: handle the error or retry.
                door_state = DOOR_IDLE;
            }
        break;

        case DOOR_UNLOCK:
            if( door_closed() && !door_handlePressed()){
                door_startLock();
                door_state = DOOR_UNLOCKING;
                // Timeout in which the action must be
                // completed by the motor.
                timeout = 6000;
            }
        break;
        case DOOR_UNLOCKING:
            //if( door_unlocked() ){
            if( previousbarsensor == true && barsensor == false ){
                door_stopLock();
                door_state = DOOR_IDLE;
                timeout = 0;
                door_barState = BAR_UNLOCKED;
            }else if( --timeout == 0 ) {
                door_stopLock();
                // TODO: handle the error or retry.
                door_state = DOOR_IDLE;
            }
        break;
    }
    
    previousbarsensor = barsensor;
    // TODO: put overcurrent detection here
}

void door_process(void)
{
}

uint8_t door_getState(void)
{
    return door_doorstate;
}

void door_setDesiredState(uint8_t desiredState)
{
    door_desiredState = desiredState;
}

#endif
