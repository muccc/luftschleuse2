#include "door.h"
#include "pinutils.h"
#include <avr/io.h>

static uint8_t door_doorstate;

enum {
    DOOR_IDLE,
    DOOR_LOCKING,
    DOOR_UNLOCKING
} door_state;

uint8_t door_nextcmd;

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
    door_nextcmd = DOOR_CMD_NONE;
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
    
    // TODO: This is an insufficient criteria!
    if( PIN_HIGH(DOOR_LOCK_CONTACT) )
        door_doorstate |= DOOR_LOCK_UNLOCKED;
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
            if( door_nextcmd == DOOR_CMD_LOCK &&
                    !door_locked() &&
                    door_closed() && 
                    !door_handlePressed() ){
                door_nextcmd = DOOR_CMD_NONE;
                door_startCloseLock();
                door_state = DOOR_LOCKING;
                door_doorstate &= ~DOOR_LOCK_PERM_UNLOCKED;
                timeout = 2000;
            }else if( door_nextcmd == DOOR_CMD_UNLOCK &&
                    door_locked() &&
                    door_closed() &&
                    !door_handlePressed() ){
                door_nextcmd = DOOR_CMD_NONE;
                door_startOpenLock();
                door_state = DOOR_UNLOCKING;
                timeout = 2000;
            }else if( door_nextcmd == DOOR_CMD_UNLOCK_PERM &&
                    door_locked() &&
                    door_closed() &&
                    !door_handlePressed() ){
                    door_startOpenLock();
                    door_state = DOOR_UNLOCKING;
                    timeout = 2000;
            }else if( door_nextcmd != DOOR_CMD_NONE &&
                    timeout == 0 ){
                // Timeout if a command can not be started
                // after 5 seconds.
                timeout = 5000;
            }else if( door_nextcmd != DOOR_CMD_NONE &&
                    --timeout == 0 ){
                // Discard the command
                // TODO: is there a race condition when new
                // command get put into the system?
                door_nextcmd = DOOR_CMD_NONE;
            }

        break;
        case DOOR_LOCKING:
            if( door_locked() ){
                door_stopLock();
                door_state = DOOR_IDLE;
                door_nextcmd = DOOR_CMD_NONE;
                timeout = 0;
            }else if( --timeout == 0 ) {
                door_stopLock();
                // TODO: handle the error or retry.
                door_nextcmd = DOOR_CMD_NONE;
            }
        break;
        case DOOR_UNLOCKING:
            if( door_unlocked() ){
                door_stopLock();
                door_state = DOOR_IDLE;
                door_doorstate &= ~DOOR_LOCK_PERM_UNLOCKED;
                if( door_nextcmd == DOOR_LOCK_PERM_UNLOCKED ){
                    door_doorstate |= DOOR_LOCK_PERM_UNLOCKED;
                }
                door_nextcmd = DOOR_CMD_NONE;
                timeout = 0;
            }else if( --timeout == 0 ) {
                door_stopLock();
                // TODO: handle the error or retry.
                door_nextcmd = DOOR_CMD_NONE;
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

void door_cmd(door_cmd_t cmd)
{
    door_nextcmd = cmd;
    /*switch(cmd){
        case DOOR_CMD_UNLOCK:
            PORTC |= 0x80;
        break;
        case DOOR_CMD_LOCK:
            PORTC &= ~0x80;
        break;
    }*/
}

