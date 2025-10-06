#include <curses.h>
#include <stdlib.h>
#include "menu.h"


static WINDOW *menu_win;


int mkmenu(unsigned int xlen, unsigned int ylen)
{
	int x;
	int y;

	getmaxyx(stdscr, y, x);
	clear();
	refresh();
	if (menu_win || !(menu_win = newwin((int)ylen, (int)xlen,
	(y / 2) - ((int)ylen / 2) + 2, (x / 2) - ((int)xlen / 2) + 2)))
		return -1;

	box(menu_win, 0, 0);
	wrefresh(menu_win);
	return 0;
}

void demenu(void)
{
	wclear(menu_win);
	wrefresh(menu_win);
	delwin(menu_win);
	menu_win = NULL;
}
