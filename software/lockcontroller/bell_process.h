#ifndef BELL_PROCESS_H_
#define BELL_PROCESS_H_
#include <stdint.h>
#include <stdbool.h>

#define BELL_PORT              D
#define BELL_PIN               6

//#define BELL_PORT              C
//#define BELL_PIN               1

void bell_init(void);
void bell_tick(void);
void bell_process(void);
bool bell_isAccepted(void);

#endif
