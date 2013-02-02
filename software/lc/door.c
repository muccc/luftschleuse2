#include "door.h"
#include "pinutils.h"
#include <stdbool.h>
#include <stdint.h>

static uint8_t door_doorstate;

enum {
    DOOR_IDLE,
    DOOR_LOCKING,
    DOOR_UNLOCKING,
    DOOR_RELOCK
} door_state;

uint8_t door_nextcmd;

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
    
    door_state = DOOR_IDLE;
    door_nextcmd = DOOR_CMD_NONE;
}

static void door_startCloseLock(void)
{
    PIN_CLEAR(DOOR_LOCK);
}

static void door_startOpenLock(void)
{
    PIN_SET(DOOR_LOCK);
}

static void door_stopLock(void)
{
}

static void door_update_inputs(void)
{
    door_doorstate &= ~(DOOR_DOOR_CLOSED | DOOR_LOCK_LOCKED | DOOR_HANDLE_PRESSED | DOOR_LOCK_UNLOCKED);
    //TODO: also query reed contact here
    if( !PIN_HIGH(DOOR_DOOR_OPEN_CONTACT) )
        door_doorstate |= DOOR_DOOR_CLOSED;
    
    if( !PIN_HIGH(DOOR_HANDLE_CONTACT) )
        door_doorstate |= DOOR_HANDLE_PRESSED;
    
    // TODO: These are insufficient criteria!
    if( !PIN_HIGH(DOOR_LOCK_UNLOCKED_CONTACT) )
        door_doorstate |= DOOR_LOCK_UNLOCKED;
    if( !PIN_HIGH(DOOR_LOCK_LOCKED_CONTACT) )
        door_doorstate |= DOOR_LOCK_LOCKED;
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
            if( door_nextcmd == DOOR_CMD_LOCK ){
                if( !door_locked() ){
                    door_startCloseLock();
                    door_state = DOOR_LOCKING;
                    // Timeout in which the action must be
                    // completed by the motor.
                    timeout = 3000;
                }
                door_doorstate &= ~DOOR_LOCK_PERM_UNLOCKED;
                door_nextcmd = DOOR_CMD_NONE;
            }else if( door_nextcmd == DOOR_CMD_UNLOCK ){
                if( door_locked() ){
                    door_startOpenLock();
                    door_state = DOOR_UNLOCKING;
                    // Timeout in which the action must be
                    // completed by the motor.
                    timeout = 3000;
                }
                // The door_nextcmd and door_doorstate
                // variables will be updated in the
                // DOOR_UNLOCKING state.
            }else if( door_nextcmd == DOOR_CMD_UNLOCK_PERM ){
                if( door_locked() ){
                    door_startOpenLock();
                    door_state = DOOR_UNLOCKING;
                    // Timeout in which the action must be
                    // completed by the motor.
                    timeout = 3000;
                }
                // The door_nextcmd and door_doorstate
                // variables will be updated in the
                // DOOR_UNLOCKING state.
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
                if( door_nextcmd == DOOR_CMD_UNLOCK_PERM ){
                    // Keep the door unlocked
                    // and accept new commands.
                    door_doorstate |= DOOR_LOCK_PERM_UNLOCKED;
                    door_state = DOOR_IDLE;
                    door_nextcmd = DOOR_CMD_NONE;
                    timeout = 0;
                }else{
                    // Lock the door again after a
                    // few seconds.
                    door_doorstate &= ~DOOR_LOCK_PERM_UNLOCKED;
                    door_state = DOOR_RELOCK;
                    timeout = 5000;
                }
            }else if( --timeout == 0 ) {
                door_stopLock();
                // TODO: handle the error or retry.
                door_nextcmd = DOOR_CMD_NONE;
            }
        break;
        case DOOR_RELOCK:
            if( timeout-- == 0 ){
                door_startCloseLock();
                door_state = DOOR_LOCKING;
                // Timeout in which the action must be
                // completed by the motor.
                timeout = 3000;
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

bool door_cmd(door_cmd_t cmd)
{
    if( door_nextcmd == DOOR_CMD_NONE ){
        // TODO: give feedback, that the command will not be
        // executed
        door_nextcmd = cmd;
        return true;
    }
    return false;
}

