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
#include <ncurses.h>
#include <string.h>
#include <stdint.h>

static uint8_t pressed_buttons;

#define DOWN_BUTTON     (1<<0)
#define CLOSED_BUTTON   (1<<1)
#define MEMBER_BUTTON   (1<<2)
#define PUBLIC_SWITCH   (1<<3)

void cli_init(void)
{	
	initscr();			/* Start curses mode 		  */
    cbreak();
    noecho();
    nodelay(stdscr,TRUE);
    curs_set(0);
    start_color();          /* Start color          */
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_BLACK, COLOR_RED);

    move(0, 10); printw("Press 'D' to toggle the DOWN button.");
    move(1, 10); printw("Press 'C' to toggle the CLOSED button.");
    move(2, 10); printw("Press 'M' to toggle the MEMBER button.");
    move(4, 10); printw("Press 'P' to toggle the PUBLIC switch.");
    pressed_buttons = 0;
}

void cli_process(void)
{
    int ch = getch();
    if(ch != ERR) {
        //attron(A_BOLD);
        //printw("%c", ch);
        //attroff(A_BOLD);
        switch(ch) {
            case 'd':
                pressed_buttons ^= DOWN_BUTTON;
            break;
            case 'c':
                pressed_buttons ^= CLOSED_BUTTON;
            break;
            case 'm':
                pressed_buttons ^= MEMBER_BUTTON;
            break;
            case 'p':
                pressed_buttons ^= PUBLIC_SWITCH;
            break;
        }

        move(0, 50); printw((pressed_buttons & DOWN_BUTTON) ? "<pressed>":"         ");
        move(1, 50); printw((pressed_buttons & CLOSED_BUTTON) ? "<pressed>":"         ");
        move(2, 50); printw((pressed_buttons & MEMBER_BUTTON) ? "<pressed>":"         ");
        move(4, 50); printw((pressed_buttons & PUBLIC_SWITCH) ? "<active>":"         ");

        refresh();			/* Print it on to the real screen */
    }
}

void cli_exit(void)
{
	endwin();			/* End curses mode		  */
}

void cli_set_led(char *name, bool state)
{
    int led = -1;
    if(strcmp(name, "LED_0") == 0) {
        led = 0;
    }
    if(strcmp(name, "LED_1") == 0) {
        led = 1;
    }
    if(strcmp(name, "LED_2") == 0) {
        led = 2;
    }
   
    if(led >= 0) {
        move(led,0);
        if(state) {
            attron(COLOR_PAIR(2));
        } else {
            attron(COLOR_PAIR(1));
        }
        
        //printw("%s",state ? "ON " : "OFF");
        printw(" ");

        if(state) {
            attroff(COLOR_PAIR(2));
        } else {
            attroff(COLOR_PAIR(1));
        }

        switch(led) {
            case 0:
                printw(" DOWN");
            break;
            case 1:
                printw(" CLOSED");
            break;
            case 2:
                printw(" MEMBER");
            break;
        }

        refresh();
    }
}


uint8_t cli_get_button(char *name)
{ 
    if(strcmp(name, "BUTTON_0") == 0) {
        if(pressed_buttons & DOWN_BUTTON) {
            return 1;
        }
    }
    if(strcmp(name, "BUTTON_1") == 0) {
        if(pressed_buttons & CLOSED_BUTTON) {
            return 1;
        }
    }
    if(strcmp(name, "BUTTON_2") == 0) {
        if(pressed_buttons & MEMBER_BUTTON) {
            return 1;
        }
    }
    if(strcmp(name, "BUTTON_3") == 0) {
        if(pressed_buttons & PUBLIC_SWITCH) {
            return 1;
        }
    }

    return 0;
}






