#ifndef DOOR_H_
#define DOOR_H_
#include <stdbool.h>
#include <stdint.h>

#define DOOR_DOOR_CLOSED            (1<<0)
#define DOOR_LOCK_LOCKED            (1<<1)
#define DOOR_LOCK_UNLOCKED          (1<<2)
#define DOOR_LOCK_LOCKING           (1<<3)
#define DOOR_LOCK_UNLOCKING         (1<<4)
#define DOOR_HANDLE_PRESSED         (1<<5)
#define DOOR_LOCK_PERM_UNLOCKED     (1<<6)

#define DOOR_REED_CONTACT_PORT              A
#define DOOR_REED_CONTACT_PIN               5

#define DOOR_LOCK_BRIDGE_PORT               A
#define DOOR_LOCK_BRIDGE_PIN                4

#define DOOR_LOCK_UNLOCKED_CONTACT_PORT       A
#define DOOR_LOCK_UNLOCKED_CONTACT_PIN        3

#define DOOR_LOCK_LOCKED_CONTACT_PORT       A
#define DOOR_LOCK_LOCKED_CONTACT_PIN        2

#define DOOR_HANDLE_CONTACT_PORT            A
#define DOOR_HANDLE_CONTACT_PIN             1

#define DOOR_DOOR_OPEN_CONTACT_PORT         A
#define DOOR_DOOR_OPEN_CONTACT_PIN          0

#define DOOR_HBRIDGE_ENABLE_PORT            B
#define DOOR_HBRIDGE_ENABLE_PIN             0

#define DOOR_HBRIDGE_IN1_PORT               B
#define DOOR_HBRIDGE_IN1_PIN                1

#define DOOR_HBRIDGE_IN2_PORT               B
#define DOOR_HBRIDGE_IN2_PIN                4

#define DOOR_LOCK_CONTACT_PORT              B
#define DOOR_LOCK_CONTACT_PIN               3

#define R1_PORT C
#define R1_PIN  7

typedef enum {
    DOOR_CMD_LOCK=0,
    DOOR_CMD_UNLOCK=1,
    DOOR_CMD_UNLOCK_PERM=2,
    DOOR_CMD_NONE
} door_cmd_t;

void door_init(void);
void door_tick(void);
void door_process(void);
uint8_t door_getState(void);
bool door_cmd(door_cmd_t cmd);

#endif

