#ifndef BUTTONS_H_
#define BUTTONS_H_
#include <stdint.h>
#include <stdbool.h>

#define BUTTON_0_PORT              C
#define BUTTON_0_PIN               0

#define BUTTON_1_PORT              C
#define BUTTON_1_PIN               1

#define BUTTON_2_PORT              A
#define BUTTON_2_PIN               3

#define BUTTON_3_PORT              A
#define BUTTON_3_PIN               4

typedef enum {
    BUTTON_0 = 1,
    BUTTON_1 = 2,
    BUTTON_2 = 4,
    BUTTON_3 = 8
} buttons_button_t;

void buttons_init(void);
void buttons_tick(void);
uint8_t buttons_getButtonsLatchedState(void);

#endif
