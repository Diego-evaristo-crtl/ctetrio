#ifndef CTETRIO_MENU_H
#define CTETRIO_MENU_H	1

#include <curses.h>
#define MENU_COLOR 1

struct menuopt {
	const char *str;
	void (*func)(void);
};

/* Creates a menu, returns -1 if it fails */
int mkmenu(unsigned xlen, unsigned ylen) __attribute__((nothrow, cold));
/* Deletes a menu */
void demenu(void) __attribute__((nothrow, cold));
/* Add and options to the menu, returns -1 if it fails */
int addmopt(struct menuopt *opt) __attribute__((nothrow, nonnull, cold));
/* Waits for an action (calling an .optf) on the menu */
void menuwait(void) __attribute__((nothrow));

#endif /* CTETRIO_MENU_H */
