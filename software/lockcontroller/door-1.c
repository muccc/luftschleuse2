#include "config.h"
#include "door-config.h"
#if DOOR_MODEL == DOOR_MODEL_1
#include "door-1.h"
#include "pinutils.h"
#include <stdbool.h>
#include <stdint.h>

static uint8_t door_desiredState;
static uint8_t door_doorstate;

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

    PIN_CLEAR(DOOR_LOCK);
    DDR_CONFIG_OUT(DOOR_LOCK);
}

static void door_lock(void)
{
    PIN_CLEAR(DOOR_LOCK);
}

static void door_unlock(void)
{
    PIN_SET(DOOR_LOCK);
}

static bool door_locking(void)
{
    if( PIN_HIGH(DOOR_LOCK) ){
        return false;
    }
    return true;
}

static void door_update_inputs(void)
{
    door_doorstate &= ~(DOOR_DOOR_CLOSED | DOOR_LOCK_LOCKED | DOOR_HANDLE_PRESSED | DOOR_LOCK_UNLOCKED| DOOR_LOCK_MANUAL_UNLOCKED);

    if( !PIN_HIGH(DOOR_DOOR_OPEN_CONTACT) && !PIN_HIGH(DOOR_REED_CONTACT) )
        door_doorstate |= DOOR_DOOR_CLOSED;
    
    if( !PIN_HIGH(DOOR_HANDLE_CONTACT) )
        door_doorstate |= DOOR_HANDLE_PRESSED;

    if( !PIN_HIGH(DOOR_LOCK_LOCKED_CONTACT) )
        door_doorstate |= DOOR_LOCK_LOCKED;
   
    // TODO: These are insufficient criteria!
    if( !PIN_HIGH(DOOR_LOCK_UNLOCKED_CONTACT) ){
        // The bar is retracted
        if( door_locking() ){
            door_doorstate |= DOOR_LOCK_MANUAL_UNLOCKED;
        }else{
            // The bar is retracted because we unlocked it
            door_doorstate |= DOOR_LOCK_UNLOCKED;
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

void door_tick(void)
{
    door_update_inputs();

    if( door_desiredState == DOOR_LOCK_LOCKED && !door_locking() ){
        if( door_closed() && !door_handlePressed() ){
            door_lock();
        }
    }else if( door_desiredState == DOOR_LOCK_UNLOCKED ){
        if( door_closed() && !door_handlePressed() ){
            door_unlock();
        }
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
