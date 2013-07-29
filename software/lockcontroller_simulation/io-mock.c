#include "io-mock.h"
#include "cli.h"

#include <stdio.h>
#include <string.h>

void io_ddr_config_in(char *pin)
{
    //printf("io_ddr_config_in(%s)\n", pin);
}

void io_ddr_config_out(char *pin)
{
    //printf("io_ddr_config_out(%s)\n", pin);
}

void io_pin_set(char *pin)
{
    //printf("io_pin_set(%s)\n", pin);
}

void io_pin_clear(char *pin)
{
    //printf("io_pin_clear(%s)\n", pin);
}

void io_pin_toggle(char *pin)
{
    //printf("io_pin_toggle(%s)\n", pin);
}

void io_pin_pulse(char *pin)
{
    //printf("io_pin_pulse(%s)\n", pin);
}

uint8_t io_pin_high_(char *pin)
{
    //printf("io_pin_high(%s)\n", pin);
    return 1;
}

