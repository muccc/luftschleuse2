#include "mastercontroller.h"
#include "cli.h"

#include <ncurses.h>
#include <stdio.h>

int main()
{	
    //cli_init();

    printf("Init..\n");
    mastercontroller_init();

    printf("Main Loop...\n");
    while(1) {
        mastercontroller_tick();
        mastercontroller_process();
    }
    //cli_exit();
	return 0;
}
