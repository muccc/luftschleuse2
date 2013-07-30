#include "door-mock.h"
#include "door-config.h"

#include <ncurses.h>
#include <string.h>
#include <stdint.h>

static uint8_t pressed_buttons;
static uint8_t door_state;

#define DOOR_BUTTON     (1<<0)

void cli_init(void)
{	
	initscr();			/* Start curses mode 		  */
    cbreak();
    noecho();
    nodelay(stdscr,TRUE);
    curs_set(0);
    start_color();          /* Start color          */
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_BLACK, COLOR_GREEN);
    init_pair(3, COLOR_RED, COLOR_BLACK);

    move(0,0); printw(DOOR_NAME);
    move(1, 15); printw("Press 'D' to toggle the open button.");
    move(3,0); printw("Desired State: ");
    move(4,0); printw("Press C to toggle DOOR_DOOR_CLOSED: ");
    move(5,0); printw("Press L to toggle DOOR_LOCK_LOCKED: ");
    move(6,0); printw("Press U to toggle DOOR_LOCK_UNLOCKED: ");
    move(7,0); printw("Press O to toggle DOOR_LOCK_LOCKING: ");
    move(8,0); printw("Press N to toggle DOOR_LOCK_UNLOCKING: ");
    move(9,0); printw("Press M to toggle DOOR_LOCK_MANUAL_UNLOCKED: ");
    move(10,0); printw("Press H to toggle DOOR_HANDLE_PRESSED: ");

    pressed_buttons = 0;
    door_state = 0;
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

        }

        move(1, 55); printw((pressed_buttons & DOOR_BUTTON) ? "<pressed>":"         ");
        move(4, 45); printw((door_state & DOOR_DOOR_CLOSED) ? "<active>":"         ");
        move(5, 45); printw((door_state & DOOR_LOCK_LOCKED) ? "<active>":"         ");
        move(6, 45); printw((door_state & DOOR_LOCK_UNLOCKED) ? "<active>":"         ");
        move(7, 45); printw((door_state & DOOR_LOCK_LOCKING) ? "<active>":"         ");
        move(8, 45); printw((door_state & DOOR_LOCK_UNLOCKING) ? "<active>":"         ");
        move(9, 45); printw((door_state & DOOR_LOCK_MANUAL_UNLOCKED) ? "<active>":"         ");
        move(10, 45); printw((door_state & DOOR_HANDLE_PRESSED) ? "<active>":"         ");

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
    if(strcmp(name, "LED_OPEN_PIN") == 0) {
        led = 0;
    }
   
    if(led >= 0) {
        move(led + 1, 0);
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
                printw(" Open LED");
            break;
        }

        refresh();
    }
}

uint8_t cli_get_button(char *name)
{ 
    if(strcmp(name, "BUTTON_0") == 0) {
        if(pressed_buttons & DOOR_BUTTON) {
            return 1;
        }
    }
    return 0;
}

void cli_updateDesiredState(uint8_t desiredState)
{
    if( desiredState == DOOR_LOCK_LOCKED ){
        attron(COLOR_PAIR(3));
        move(3,15); printw("LOCKED  ");
        attroff(COLOR_PAIR(3));
    }
    if(desiredState == DOOR_LOCK_UNLOCKED ){
        attron(COLOR_PAIR(1));
        move(3,15); printw("UNLOCKED");
        attroff(COLOR_PAIR(1));
    }
}

uint8_t cli_getState(void)
{
    return door_state;
}


