#include <curses.h>
#include <stdlib.h>
#include "menu.h"


static WINDOW *menu_win;
static size_t moptind; 	/* Current used size of mopts */
static size_t moptlen; 	/* Current allocated size of mopts */
static size_t copt;	/* Current selected option, index in mopts*/
struct menuopt **mopts; /* Options of the menu */

/* Redraws the menu, is the option currently choosen */
static void updmenu() __attribute__((nothrow));


int mkmenu(unsigned xlen, unsigned ylen)
{
	int x;
	int y;

	moptind = 0;
	moptlen = 0;

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
	if (mopts) {
		free(mopts);
		mopts = NULL;
	}
	wclear(menu_win);
	wrefresh(menu_win);
	delwin(menu_win);
	menu_win = NULL;
}

int addmopt(struct menuopt *opt)
{
	if (!mopts && !(mopts = malloc((moptlen = 1) * sizeof(*mopts))))
		return -1;
	else if (moptind >= moptlen && !(mopts = realloc(mopts, (moptlen <<= 1)
	* sizeof(*mopts))))
		return -1;
	mopts[moptind++] = opt;
	return 0;
}

void menuwait(void)
{
	int ch;

	updmenu(copt);
	while (1) {
		switch ((ch = getch())) {
		case KEY_UP:
			if (copt > 0) {
				--copt;
				updmenu(copt);
			}
			break;
		case KEY_DOWN:
			if (copt + 1 < moptind) {
				++copt;
				updmenu(copt);
			}
			break;
		case '\n':
		case '\r':
		case KEY_RIGHT:
			mopts[copt]->func();
			return;
		}
	}
}

static void updmenu()
{
	size_t i;
	short save_fg;
	short save_bg;

	pair_content(MENU_COLOR, &save_fg, &save_bg);
	init_pair(MENU_COLOR, COLOR_BLACK, COLOR_WHITE);

	wclear(menu_win);
	box(menu_win, 0, 0);

	for (i = 0; i < moptind; ++i) {
		if (i == copt) {
			wattron(menu_win, COLOR_PAIR(MENU_COLOR));
			mvwprintw(menu_win, (int)i + 1, 1, mopts[i]->str);
			wattroff(menu_win, COLOR_PAIR(MENU_COLOR));
		} else {
			mvwprintw(menu_win, (int)i + 1, 1, mopts[i]->str);
		}
	}

	wrefresh(menu_win);
	init_pair(MENU_COLOR, save_fg, save_bg);
}
