#ifndef IO_MOCK_H
#define IO_MOCK_H

#include <stdint.h>

void io_ddr_config_in(char *pin);
void io_ddr_config_out(char *pin);
void io_pin_set(char *pin);
void io_pin_clear(char *pin);
void io_pin_toggle(char *pin);
void io_pin_pulse(char *pin);
uint8_t io_pin_high_(char *pin);
#endif
