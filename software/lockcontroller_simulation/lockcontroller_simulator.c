/*
 *  This file is part of the luftschleuse2 project.
 *
 *  See https://github.com/muccc/luftschleuse2 for more information.
 *
 *  Copyright (C) 2013 Tobias Schneider <schneider@muc.ccc.de> 
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "lockcontroller.h"
#include "cli.h"

#include <ncurses.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>

int main()
{
    uint8_t i = 0;
    do {
        cli_init();
        lockcontroller_init();
        while(!cli_get_device_reset() && !cli_get_exit_simulation()) {
            for(i = 0; i < 5; i++) { 
                lockcontroller_process();
                usleep(200);
            }
            lockcontroller_tick();
            cli_process();
        }
    } while(!cli_get_exit_simulation());
    cli_exit();
	return 0;
}
