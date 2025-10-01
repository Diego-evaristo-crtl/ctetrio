#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/* Tries changing magenta to purple during init(), fallback if it fails */
#define COLOR_PURPLE COLOR_MAGENTA

#define TETRIO_SCREEN_Y 20
#define TETRIO_SCREEN_X 10
/* Add this to x and y when printing to tetrio_win */
#define TETRIO_OFFSET_Y 1
#define TETRIO_OFFSET_X 1

enum tetrio_block {
	TETRIO_NONE=0, TETRIO_CYAN, TETRIO_BLUE, TETRIO_RED, TETRIO_YELLOW,
	TETRIO_GREEN, TETRIO_PURPLE, TETRIO_HITBOX,
};


static WINDOW *tetrio_win;


/* Initializes the screen, colors, etc. Will exit if an error is detected */
static void init(char *argv0) __attribute__((nothrow, cold));
static void deinit(void) __attribute__((nothrow, cold));


static void init(char *argv0)
{
	initscr();
	noecho();
	if (has_colors() == FALSE) {
		endwin();
		fprintf(stderr, "%s: init(): Your terminal does not "
				"support colors, Get a proper one!\n", argv0);
		exit(EXIT_FAILURE);
		__builtin_unreachable();
	}

	if (can_change_color() == TRUE)
		init_color(COLOR_PURPLE, 0x70, 0x29, 0x63);

	start_color();
	init_pair(TETRIO_CYAN, 		COLOR_CYAN, 	COLOR_CYAN);
	init_pair(TETRIO_BLUE, 		COLOR_BLUE, 	COLOR_BLUE);
	init_pair(TETRIO_RED,		COLOR_RED, 	COLOR_RED);
	init_pair(TETRIO_YELLOW, 	COLOR_YELLOW, 	COLOR_YELLOW);
	init_pair(TETRIO_GREEN,		COLOR_GREEN, 	COLOR_GREEN);
	init_pair(TETRIO_PURPLE, 	COLOR_PURPLE, 	COLOR_PURPLE);

	/* TODO: Centralize window based on getmaxyx() macro */
	refresh();
	if (!(tetrio_win = newwin(TETRIO_SCREEN_Y + 2, TETRIO_SCREEN_X + 2,
			0, 0))) {
		endwin();
		fprintf(stderr, "%s: init(): newwin(): %s", argv0,
				strerror(errno));
		exit(EXIT_FAILURE);
		__builtin_unreachable();
	}

	box(tetrio_win, 0, 0);
	wrefresh(tetrio_win);
}

static void deinit(void)
{
	wborder(tetrio_win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
	wrefresh(tetrio_win);
	delwin(tetrio_win);
	endwin();
}

int main(int argc, char *argv[argc + 1])
{
	init(*argv);

	/* TODO: make this fallback to manually calling deinit() */
	if (atexit(deinit)) {
		endwin();
		fprintf(stderr, "%s: Failed to register deinit() with"
				"atexit()\n", *argv);
		deinit();
		exit(EXIT_FAILURE);
		__builtin_unreachable();
	}

	exit(EXIT_SUCCESS);
}
