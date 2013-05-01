#ifndef CMD_PROCESS_H_
#define CMD_PROCESS_H_

#include <stdint.h>


typedef enum {
    CMD_SET_LED  = 'L',
    CMD_WRITE_LCD = 'W',
    CMD_GET_STATE = 'S',
    CMD_SEND_STATE = 'S'
} cmd_t;

void cmd_init(void);
void cmd_tick(void);
void cmd_process(void);
void cmd_new(cmd_t cmd, uint8_t *data);

#endif
