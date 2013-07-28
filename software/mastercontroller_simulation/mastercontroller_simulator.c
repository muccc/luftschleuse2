#include "mastercontroller.h"
#include "cli.h"

#include <ncurses.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>

int main()
{	
    //cli_init();

    printf("Init..\n");
    mastercontroller_init();

    printf("Main Loop...\n");
    uint8_t i = 0;
    while(1) {
        for(i = 0; i < 5; i++) { 
            mastercontroller_process();
            usleep(200);
        }
        mastercontroller_tick();
    }
    //cli_exit();
	return 0;
}
