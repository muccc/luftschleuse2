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
#include "door-mock.h"
#include "door-config.h"
#include "sequence_numbers.h"
#include "eeprom-mock.h"
#include "bus_process.h"

#include <ncurses.h>
#include <string.h>
#include <stdint.h>

static uint32_t pressed_buttons;
static uint8_t door_state;
static bool leds[32]; 
static bool cli_device_reset;
static bool cli_exit_simulation;

#define DOOR_BUTTON     (1<<0)
#define BELL_BUTTON     (1<<1)

static void cli_update_screen(void);

void cli_init(void)
{
    uint32_t row;
	initscr();			/* Start curses mode 		  */
    erase();
    clear();
    cbreak();
    noecho();
    nodelay(stdscr,TRUE);
    curs_set(0);
    start_color();          /* Start color          */
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_BLACK, COLOR_GREEN);
    init_pair(3, COLOR_RED, COLOR_BLACK);

    row = 0;
    move(row + 0, 20); printw(DOOR_NAME);

    row = 5;
    move(row + 0,15); printw("Press 'D' to toggle the open button.");
    move(row + 1,15); printw("Press 'B' to ring the bell (Default: > 400 ms).");

    row = 8;
    move(row + 0,0); printw("Desired State: ");

    row = 10;
    move(row + 0,0); printw("Press C to toggle DOOR_DOOR_CLOSED: ");
    move(row + 1,0); printw("Press L to toggle DOOR_LOCK_LOCKED: ");
    move(row + 2,0); printw("Press U to toggle DOOR_LOCK_UNLOCKED: ");
    move(row + 3,0); printw("Press O to toggle DOOR_LOCK_LOCKING: ");
    move(row + 4,0); printw("Press N to toggle DOOR_LOCK_UNLOCKING: ");
    move(row + 5,0); printw("Press M to toggle DOOR_LOCK_MANUAL_UNLOCKED: ");
    move(row + 6,0); printw("Press H to toggle DOOR_HANDLE_PRESSED: ");
    
    move(row + 8,0); printw("Press R to reset the device");

    pressed_buttons = 0;
    door_state = DOOR_DOOR_CLOSED | DOOR_LOCK_LOCKED;
    cli_update_screen();
    uint32_t led;
    for(led = 0; led < 32; led++) {
        leds[led] = false;
    }

    cli_device_reset = false;
    cli_exit_simulation = false;
}

bool cli_get_device_reset(void)
{
    return cli_device_reset;
}

bool cli_get_exit_simulation(void)
{
    return cli_exit_simulation;
}

static void cli_update_screen(void)
{
    uint32_t row;
    
    //for(row = 0; row < 20; row++) {
    //    move(row,80); printw("X");
    //}

    row = 5;
    move(row + 0, 55); printw((pressed_buttons & DOOR_BUTTON) ? "<pressed>":"         ");
    move(row + 1, 65); printw((pressed_buttons & BELL_BUTTON) ? "<pressed>":"         ");

    row = 10;
    move(row + 0, 45); printw((door_state & DOOR_DOOR_CLOSED) ? "<active>":"         ");
    move(row + 1, 45); printw((door_state & DOOR_LOCK_LOCKED) ? "<active>":"         ");
    move(row + 2, 45); printw((door_state & DOOR_LOCK_UNLOCKED) ? "<active>":"         ");
    move(row + 3, 45); printw((door_state & DOOR_LOCK_LOCKING) ? "<active>":"         ");
    move(row + 4, 45); printw((door_state & DOOR_LOCK_UNLOCKING) ? "<active>":"         ");
    move(row + 5, 45); printw((door_state & DOOR_LOCK_MANUAL_UNLOCKED) ? "<active>":"         ");
    move(row + 6, 45); printw((door_state & DOOR_HANDLE_PRESSED) ? "<active>":"         ");

    uint32_t led;
    for(led = 0; led < 1; led++){
        switch(led) {
            case 0:
                move(5, 0);
            break;
            default:
                continue;
            break;
        }

        if(leds[led]) {
            attron(COLOR_PAIR(2));
        } else {
            attron(COLOR_PAIR(1));
        }
        
        //printw("%s",state ? "ON " : "OFF");
        printw(" ");

        if(leds[led]) {
            attroff(COLOR_PAIR(2));
        } else {
            attroff(COLOR_PAIR(1));
        }

        switch(led) {
            case 0:
                printw(" Open LED");
            break;
        }
    }

    row = 1;
    move(row + 0, 0);
    printw("Expected RX sequence:\t%08u\t",
            sequence_numbers_get_expected_rx());
    printw("Persisted RX sequence:\t%08u",
            sequence_numbers_get_persisted_rx());
    
    move(row + 1, 0);
    printw("EEPROM write count:\t%08u\t",
            eeprom_mock_get_write_counter());
    printw("Next TX sequence:\t%08u\t", sequence_numbers_get_tx());
    move(row + 2, 0);
    printw("Accepted packets:\t%08u\t", bus_get_accepted_packets());
    printw("Rejected packets:\t%08u", bus_get_rejected_packets());

    refresh();			/* Print it on to the real screen */
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
                pressed_buttons ^= DOOR_BUTTON;
            break;
            case 'b':
                pressed_buttons ^= BELL_BUTTON;
            break;
            case 'c':
                door_state ^= DOOR_DOOR_CLOSED;
            break;
            case 'l':
                door_state ^= DOOR_LOCK_LOCKED;
            break;
            case 'u':
                door_state ^= DOOR_LOCK_UNLOCKED;
            break;
            case 'o':
                door_state ^= DOOR_LOCK_LOCKING;
            break;
            case 'n':
                door_state ^= DOOR_LOCK_UNLOCKING;
            break;
            case 'm':
                door_state ^= DOOR_LOCK_MANUAL_UNLOCKED;
            break;
            case 'h':
                door_state ^= DOOR_HANDLE_PRESSED;
            break;
            case 'r':
                cli_device_reset = true;
            break;
            cli_update_screen();
        }

    }
}

void cli_exit(void)
{
	endwin();			/* End curses mode		  */
}

void cli_set_led(char *name, bool state)
{
    int led = -1;
    if(strcmp(name, "LED_OPEN_PIN") == 0) {
        led = 0;
    }
   
    if(led >= 0 && led < 32) {
        leds[led] = state;
        cli_update_screen();
    }
}

uint8_t cli_get_button(char *name)
{ 
    if(strcmp(name, "BUTTON_0") == 0) {
        if(pressed_buttons & DOOR_BUTTON) {
            return 1;
        }
    }else if(strcmp(name, "BUTTON_BELL") == 0) {
        if(pressed_buttons & BELL_BUTTON) {
            return 1;
        }
    }

    return 0;
}

void cli_updateDesiredState(uint8_t desiredState)
{
    if( desiredState == DOOR_LOCK_LOCKED ){
        attron(COLOR_PAIR(3));
        move(8,15); printw("LOCKED  ");
        attroff(COLOR_PAIR(3));
    }
    if(desiredState == DOOR_LOCK_UNLOCKED ){
        attron(COLOR_PAIR(1));
        move(8,15); printw("UNLOCKED");
        attroff(COLOR_PAIR(1));
    }
}

uint8_t cli_getState(void)
{
    return door_state;
}


