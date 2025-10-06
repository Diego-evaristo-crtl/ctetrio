#ifndef CTETRIO_MENU_H
#define CTETRIO_MENU_H	1

/* Creates a menu, returns -1 if one alredy exists */
int mkmenu(unsigned int xlen, unsigned int ylen) __attribute__((nothrow, cold));
/* Deletes a menu */
void demenu(void) __attribute__((nothrow, cold));

#endif /* CTETRIO_MENU_H */
