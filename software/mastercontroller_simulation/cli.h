#ifndef CLI_H_
#define CLI_H_
#include <stdbool.h>
#include <stdint.h>

void cli_init(void);
void cli_exit(void);
void cli_process(void);
void cli_set_led(char *name, bool state);
uint8_t cli_get_button(char *name);

#endif

