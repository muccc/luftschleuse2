#ifndef CMD_PROCESS_H_
#define CMD_PROCESS_H_

#include <stdint.h>

void cmd_init(void);
void cmd_tick(void);
void cmd_process(void);
void cmd_new(uint8_t cmd, uint8_t *data);

#define CMD_SET_LED     'L'
#define CMD_WRITE_LCD   'W'
#define CMD_SEND_STATE  'S'
#define CMD_CLEAR_BUTTONS 'C'
#define CMD_ACK         'A'

#endif
