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


/* Different types of tetrio blocks */
enum tetrio_block {
	TETRIO_NONE=0, TETRIO_CYAN, TETRIO_BLUE, TETRIO_RED, TETRIO_YELLOW,
	TETRIO_GREEN, TETRIO_PURPLE, TETRIO_HITBOX,
};

/* A tetrio piece, vec has the cordinates for all blocks that compose it */
struct tetrio {
	unsigned size: 31;
	unsigned hashit: 1;
	enum tetrio_block color;
	struct point {
		unsigned short x;
		unsigned short y;
	} *vec;
};


/* All blocks on screen,  block_vec[TETRIO_SCREEN_Y] only has TETRIO_HITBOXEs */
static enum tetrio_block block_vec[TETRIO_SCREEN_Y + 1][TETRIO_SCREEN_X];
static WINDOW *tetrio_win;


/* Initializes the screen, colors, etc. Will exit if an error is detected */
static void init(char *argv0) __attribute__((nothrow, cold));
static void deinit(void) __attribute__((nothrow, cold));
/* Moves `tet` around the screen, sets tet.hashit if it hits something */
static void updtet(struct tetrio *tet) __attribute__((nothrow, nonnull, hot));
/* Updates the screen based on the main block */
static void updscr(struct tetrio *tet) __attribute__((nothrow, nonnull, hot));

static void init(char *argv0)
{
	size_t i;

	initscr();
	noecho();
	curs_set(0);

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

	for (i = 0; i < TETRIO_SCREEN_X; ++i)
		block_vec[TETRIO_SCREEN_Y][i] = TETRIO_HITBOX;
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

	/* START TEST: */
	struct tetrio tet = {
		.size = 3,
		.color = TETRIO_RED,
	};

	tet.vec = malloc(tet.size * sizeof(*tet.vec));
	tet.vec[0] = (struct point){0, 0};
	tet.vec[1] = (struct point){0, 1};
	tet.vec[2] = (struct point){1, 0};

	getch();
	for (size_t i = 0; i < 25; ++i) {
		updtet(&tet);
		updscr(&tet);
		getch();
	}
	getch();
	free(tet.vec);
	/* END TEST */

	exit(EXIT_SUCCESS);
}

static void updtet(struct tetrio *tet)
{
	size_t i;
	for (i = 0; i < tet->size; ++i)
		if (block_vec[tet->vec[i].y + 1][tet->vec[i].x] || tet->hashit)
			tet->hashit = 1;
	for (i = 0; i < tet->size; ++i)
		if (!tet->hashit)
			tet->vec[i].y += 1;


	if (tet->hashit)
		for (i = 0; i < tet->size; ++i)
			block_vec[tet->vec[i].y][tet->vec[i].x] = tet->color;
}

static void updscr(struct tetrio *tet)
{
	int x;
	int y;

	wclear(tetrio_win);
	if (!tet->hashit) {
		for (x = 0; (size_t)x < tet->size; ++x) {
			mvwaddch(tetrio_win, tet->vec[x].y + TETRIO_OFFSET_Y,
				tet->vec[x].x + TETRIO_OFFSET_X, '#' |
				COLOR_PAIR(tet->color));
		}
	}

	for (y = 0; y < TETRIO_SCREEN_Y; ++y) {
		for (x = 0; x < TETRIO_SCREEN_X; ++x) {
			if (block_vec[y][x]) {
				mvwaddch(tetrio_win, y + TETRIO_OFFSET_Y,
					x + TETRIO_OFFSET_X, '#' |
					COLOR_PAIR(block_vec[y][x]));
			}
		}
	}

	box(tetrio_win, 0, 0);
	wrefresh(tetrio_win);
}
