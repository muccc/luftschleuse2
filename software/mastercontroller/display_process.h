#ifndef DISPLAY_PROCESS_H_
#define DISPLAY_PROCESS_H_
#include <stdint.h>

void display_init(void);
void display_tick(void);
void display_process(void);
void display_data(uint8_t channel, uint8_t *data, uint8_t len); 
#endif
