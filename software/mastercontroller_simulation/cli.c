#include <ncurses.h>

void cli_init(void)
{	
	initscr();			/* Start curses mode 		  */
    cbreak();
    noecho();
    halfdelay(1);
}

void cli_process(void)
{
    static int i = 0;
    move(0,0);
    printw("Type any character to see it in bold %d\n", i++);
    int ch = getch();
    if(ch != ERR) {
        printw("The pressed key is ");
        attron(A_BOLD);
        printw("%c", ch);
        attroff(A_BOLD);
    }
    refresh();			/* Print it on to the real screen */
}

void cli_exit(void)
{
	endwin();			/* End curses mode		  */
}
