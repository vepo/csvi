#include "matrix-presentation.h"

#include <curses.h>

struct screen{
	int x;
	int y;
};

void matrix_presentation_init() {
    initscr(); 
    start_color();
    
    noecho();
	curs_set(0);
	timeout(0);
	cbreak();
	nodelay(stdscr,true);

    struct screen field;
    getmaxyx(stdscr, field.y, field.x);

    mvprintw((field.y/2),(field.x/2),"Press Any Key");

    refresh();
    getch();
}

void matrix_presentation_exit() {
    endwin();
}