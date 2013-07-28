#ifndef DELAY_H_
#define DELAY_H_
#include <unistd.h>

#define _delay_us(x) usleep(x)
#define _delay_ms(x) do{ int i = x; while(i--) usleep(1000);}while(0)

#endif
