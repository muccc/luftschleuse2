#include "mastercontroller.h"
#include "cli.h"

#include <ncurses.h>

int main()
{	
    //cli_init();

    mastercontroller_init();

    while(1) {
        mastercontroller_tick();
        mastercontroller_process();
    }
    //cli_exit();
	return 0;
}
