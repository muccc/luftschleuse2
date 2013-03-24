#include "config.h"
#if DOOR_MODEL == DOOR_2
#include "door.h"
#include "pinutils.h"
#include <stdbool.h>
#include <stdint.h>

#include "leds.h"

static uint8_t door_doorstate;
static uint8_t door_desiredState;

enum {
    DOOR_IDLE,
    DOOR_LOCKING,
    DOOR_UNLOCKING,
} door_state;

static void door_startCloseLock(void)
{
    //PIN_SET(R1);
    PIN_SET(DOOR_HBRIDGE_IN1);
    PIN_SET(DOOR_HBRIDGE_ENABLE);
}

static void door_startOpenLock(void)
{
    //PIN_SET(R1);
    PIN_SET(DOOR_HBRIDGE_IN1);
    PIN_SET(DOOR_HBRIDGE_ENABLE);
}

static void door_stopLock(void)
{
    //PIN_CLEAR(R1);
    PIN_CLEAR(DOOR_HBRIDGE_IN1);
    PIN_CLEAR(DOOR_HBRIDGE_IN2);
    //PIN_CLEAR(DOOR_HBRIDGE_ENABLE);
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

    //PIN_CLEAR(DOOR_HBRIDGE_ENABLE);
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

    door_state = DOOR_IDLE;
    door_desiredState = DOOR_LOCK_LOCKED;
#if 0
    volatile uint32_t l;
    while(1){
        door_startCloseLock();
        //PIN_SET(R1);
        for(l=0; l<2000000; l++);
        door_stopLock();
        for(l=0; l<500000; l++);
        PIN_CLEAR(R1);
        door_startOpenLock();
        for(l=0; l<2000000; l++);
        door_stopLock();
        for(l=0; l<500000; l++);
    }
#endif
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
        if( !PIN_HIGH(DOOR_LOCK_CONTACT) ){
            door_doorstate |= DOOR_LOCK_UNLOCKED;
        }else{
            door_doorstate |= DOOR_LOCK_MANUAL_UNLOCKED;
        }
    }
}

static uint8_t door_locked(void)
{
    return door_doorstate & DOOR_LOCK_LOCKED;
}

static uint8_t door_unlocked(void)
{
    return door_doorstate & DOOR_LOCK_UNLOCKED;
}

static uint8_t door_closed(void)
{
    return door_doorstate & DOOR_DOOR_CLOSED;
}

static uint8_t door_handlePressed(void)
{
    return door_doorstate & DOOR_HANDLE_PRESSED;
}

void door_tick(void)
{
    static uint16_t timeout = 0;

    door_update_inputs();

    switch(door_state)
    {
        case DOOR_IDLE:
            if( door_desiredState == DOOR_LOCK_LOCKED && !door_locked() ){
                //if( !door_locked() && door_closed() ){
                if( door_closed() && !door_handlePressed()){
                    door_startCloseLock();
                    door_state = DOOR_LOCKING;
                    // Timeout in which the action must be
                    // completed by the motor.
                    timeout = 6000;
                }
            }else if( door_desiredState == DOOR_LOCK_UNLOCKED &&
                        !door_unlocked() ){
                //if( door_locked() && door_closed() ){
                if( door_closed() && !door_handlePressed()){
                    door_startOpenLock();
                    door_state = DOOR_UNLOCKING;
                    // Timeout in which the action must be
                    // completed by the motor.
                    timeout = 6000;
                }
            }
        break;
        case DOOR_LOCKING:
            if( door_locked() ){
                door_stopLock();
                door_state = DOOR_IDLE;
                timeout = 0;
            }else if( --timeout == 0 ) {
                door_stopLock();
                // TODO: handle the error or retry.
                door_state = DOOR_IDLE;
            }
        break;
        case DOOR_UNLOCKING:
            if( door_unlocked() ){
                door_stopLock();
                door_state = DOOR_IDLE;
                timeout = 0;
            }else if( --timeout == 0 ) {
                door_stopLock();
                // TODO: handle the error or retry.
                door_state = DOOR_IDLE;
            }
        break;
    }
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
