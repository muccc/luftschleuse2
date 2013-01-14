#include "door.h"
#include "pinutils.h"
#include <avr/io.h>

static uint8_t door_doorstate;

enum {
    DOOR_LOCKING,
    DOOR_IDLE,
    DOOR_CLOSING
} door_state;

void door_init(void)
{
    door_doorstate = 0;
    DDR_CONFIG_IN(DOOR_REED_CONTACT);
    DDR_CONFIG_IN(DOOR_LOCK_CONTACT);
    DDR_CONFIG_IN(DOOR_HANDLE_CONTACT);
    
    PIN_CLEAR(DOOR_HBRIDGE_ENABLE);
    PIN_CLEAR(DOOR_HBRIDGE_IN1);
    PIN_CLEAR(DOOR_HBRIDGE_IN2);
   
    DDR_CONFIG_OUT(DOOR_HBRIDGE_ENABLE);
    DDR_CONFIG_OUT(DOOR_HBRIDGE_IN1);
    DDR_CONFIG_OUT(DOOR_HBRIDGE_IN2);
    
    door_state = DOOR_IDLE;
}

static void door_startCloseLock(void)
{
    PIN_SET(DOOR_HBRIDGE_IN1);
    PIN_SET(DOOR_HBRIDGE_ENABLE);
}

static void door_startOpenLock(void)
{
    PIN_SET(DOOR_HBRIDGE_IN2);
    PIN_SET(DOOR_HBRIDGE_ENABLE);
}

static void door_stopLock(void)
{
    PIN_CLEAR(DOOR_HBRIDGE_IN1);
    PIN_CLEAR(DOOR_HBRIDGE_IN2);
    PIN_CLEAR(DOOR_HBRIDGE_ENABLE);
}

static void door_update_inputs(void)
{
    door_doorstate &= ~(DOOR_DOOR_CLOSED | DOOR_LOCK_LOCKED | DOOR_HANDLE_PRESSED);
    
    if( !PIN_HIGH(DOOR_REED_CONTACT) )
        door_doorstate |= DOOR_DOOR_CLOSED;
    if( !PIN_HIGH(DOOR_LOCK_CONTACT) )
        door_doorstate |= DOOR_LOCK_LOCKED;
    if( !PIN_HIGH(DOOR_HANDLE_CONTACT) )
        door_doorstate |= DOOR_HANDLE_PRESSED;
}

static uint8_t door_locked(void)
{
    return door_doorstate & DOOR_LOCK_LOCKED;
}

void door_tick(void)
{
    door_update_inputs();
    switch(door_state)
    {
        case DOOR_IDLE:
        break;
        case DOOR_LOCKING:
            if( door_locked() ){
                door_stopLock();
                door_state = DOOR_IDLE;
            }else{
                // TODO: put jam detection here
            }
        break;
    }
}

void door_process(void)
{
}

uint8_t door_getState(void)
{
    return 0;
}

void door_cmd(door_cmd_t cmd)
{
    switch(cmd){
        case DOOR_CMD_UNLOCK:
            PORTC |= 0x80;
        break;
        case DOOR_CMD_LOCK:
            PORTC &= ~0x80;
        break;
    }
}

