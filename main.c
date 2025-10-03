#include <curses.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <time.h>


/* Tries changing magenta to purple during init(), fallback if it fails */
#define COLOR_PURPLE COLOR_MAGENTA
#define TETRIO_SCREEN_Y 20
#define TETRIO_SCREEN_X 10
/* Add this to x and y when printing to tetrio_win */
#define TETRIO_OFFSET_Y 1
#define TETRIO_OFFSET_X 1
/* Number of different pieces && size of blocks per piece */
#define TETRIO_TEMPLATE_NUM 	7
#define TETRIO_ARR_SIZE 	4
#define TETRIO_COLOR_NUM 	6
/* Generalized macros */
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))


/* Different types of tetrio blocks */
enum tetrio_block {
	TETRIO_NONE=0, TETRIO_CYAN, TETRIO_BLUE, TETRIO_RED, TETRIO_YELLOW,
	TETRIO_GREEN, TETRIO_PURPLE, TETRIO_HITBOX,
};

/* A tetrio piece, vec has the cordinates for all blocks that compose it */
struct tetrio {
	struct vec2d {
		short int x;
		short int y;
	} arr[TETRIO_ARR_SIZE];
	struct vec2d org;
	enum tetrio_block color;
	_Bool hashit;
};


/* All blocks on screen,  block_vec[TETRIO_SCREEN_Y] only has TETRIO_HITBOXEs */
static enum tetrio_block block_vec[TETRIO_SCREEN_Y][TETRIO_SCREEN_X];
static WINDOW *tetrio_win;

/* Initializes the screen, colors, etc. Will exit if an error is detected */
static void init(char *argv0) __attribute__((nothrow, cold));
static void deinit(void) __attribute__((nothrow, cold));
/* Moves `tet` around the screen, sets tet.hashit if it hits something */
static void updtet(struct tetrio *tet) __attribute__((nothrow, nonnull));
/* Updates the screen based on the main block */
static void updscr(struct tetrio *tet) __attribute__((nothrow, nonnull, hot));
/* Creates a tetrio object, it must be deleted by a call to free() */
static struct tetrio *gettet(void) __attribute__((nothrow, malloc, malloc(
	free, 1), cold));
/* Moves `tet` to right of left if `d` is 1 or -1. Sets hashit on failure */
static void mvside(struct tetrio *tet, short int d) __attribute__((nothrow,
	nonnull));
/* Collects and operates the input for ms time, does not move tetrio down */
static void passms(struct tetrio *tet, unsigned ms) __attribute__((nothrow,
	nonnull));
/* Returns 1 if tetrio has hit something */
static _Bool hashit(struct tetrio *tet, short int dy, short int dx)
	__attribute__((nothrow, nonnull, hot));


int main(int argc, char *argv[argc + 1])
{
	struct tetrio *tet;
	unsigned short rnd_ms;
	unsigned int i;

	init(*argv);

	for (rnd_ms = 200; ; rnd_ms -= 2) {
		tet = gettet();
		while (!tet->hashit) {
			passms(tet, rnd_ms);
			updtet(tet);
			updscr(tet);
		}

		/* LOST */
		for (i = 0; i < TETRIO_ARR_SIZE; ++i) {
			if (!tet->arr[i].y) {
				free(tet);
				return EXIT_FAILURE;
				__builtin_unreachable();
			}
		}
		free(tet);
	}

	exit(EXIT_SUCCESS);
}

static void init(char *argv0)
{
	int x;

	initscr();
	noecho();
	keypad(stdscr, TRUE);
	nodelay(stdscr, TRUE);
	curs_set(0);

	if (has_colors() == FALSE) {
		endwin();
		fprintf(stderr, "%s: init(): Your terminal does not "
				"support colors, Get a proper one!\n", argv0);
		exit(EXIT_FAILURE);
		__builtin_unreachable();
	}

	start_color();
	if (can_change_color() == TRUE)
		init_color(COLOR_PURPLE, 0x70, 0x29, 0x63);
	init_pair(TETRIO_CYAN, 		COLOR_CYAN, 	COLOR_CYAN);
	init_pair(TETRIO_BLUE, 		COLOR_BLUE, 	COLOR_BLUE);
	init_pair(TETRIO_RED,		COLOR_RED, 	COLOR_RED);
	init_pair(TETRIO_YELLOW, 	COLOR_YELLOW, 	COLOR_YELLOW);
	init_pair(TETRIO_GREEN,		COLOR_GREEN, 	COLOR_GREEN);
	init_pair(TETRIO_PURPLE, 	COLOR_PURPLE, 	COLOR_PURPLE);

	x = getmaxx(stdscr);
	refresh();
	if (!(tetrio_win = newwin(TETRIO_SCREEN_Y + 2, TETRIO_SCREEN_X + 2,
			0, x / 2 - (TETRIO_SCREEN_X + 2) / 2))) {
		endwin();
		fprintf(stderr, "%s: init(): newwin(): %s", argv0,
				strerror(errno));
		exit(EXIT_FAILURE);
		__builtin_unreachable();
	}

	box(tetrio_win, 0, 0);
	wrefresh(tetrio_win);

	/* TODO: make this fallback to manually calling deinit() */
	if (atexit(deinit)) {
		endwin();
		fprintf(stderr, "%s: Failed to register deinit() with"
				"atexit()\n", argv0);
		deinit();
		exit(EXIT_FAILURE);
		__builtin_unreachable();
	}
}

static void deinit(void)
{
	wborder(tetrio_win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
	wrefresh(tetrio_win);
	delwin(tetrio_win);
	endwin();
}

static void updtet(struct tetrio *tet)
{
	size_t i;
	for (i = 0; i < TETRIO_ARR_SIZE && !tet->hashit; ++i)
		tet->hashit = hashit(tet, 1, 0);

	if (!tet->hashit) {
		for (i = 0; i < TETRIO_ARR_SIZE; ++i)
			tet->arr[i].y += 1;
		tet->org.y += 1;
	} else {
		for (i = 0; i < TETRIO_ARR_SIZE; ++i)
			block_vec[tet->arr[i].y][tet->arr[i].x] = tet->color;
	}
}

static void updscr(struct tetrio *tet)
{
	int x;
	int y;
	_Bool clrrow;

	/*
	 * TODO: optimize this to not clear the entire screen and only call
	 * 	delch()/addch() on characters that differ from block_vec and
	 * 	a second buffer that actually represents the current screen
	 */

	wclear(tetrio_win);
	x = getmaxx(stdscr);
	mvwin(tetrio_win, 0, x / 2 - (TETRIO_SCREEN_X + 2) / 2);
	if (!tet->hashit) {
		for (x = 0; (size_t)x < TETRIO_ARR_SIZE; ++x) {
			mvwaddch(tetrio_win, tet->arr[x].y + TETRIO_OFFSET_Y,
				tet->arr[x].x + TETRIO_OFFSET_X, '#' |
				COLOR_PAIR(tet->color));
		}
	}

	for (y = 0; y < TETRIO_SCREEN_Y; ++y) {
		for (clrrow = 1, x = 0; x < TETRIO_SCREEN_X && clrrow; ++x)
			if (!block_vec[y][x])
				clrrow = 0;
		for (x = 0; x < TETRIO_SCREEN_X && clrrow; ++x) {
			block_vec[y][x] = TETRIO_NONE;
			mvwdelch(tetrio_win, y, x);
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

static struct tetrio *gettet(void)
{
	static const struct vec2d vec_tmpl[TETRIO_TEMPLATE_NUM][TETRIO_ARR_SIZE] =
	{
		{{0, 0}, {0, 1}, {1, 0}, {1, 1}},
		{{0, 0}, {0, 1}, {0, 2}, {0, 3}},
		{{0, 0}, {0, 1}, {0, 2}, {1, 2}},
		{{0, 0}, {0, 1}, {1, 1}, {0, 2}},
		{{1, 0}, {1, 1}, {1, 2}, {0, 2}},
		{{0, 1}, {1, 1}, {0, 1}, {0, 2}},
		{{0, 0}, {0, 1}, {1, 1}, {1, 2}},
	};
	static const short int orgy_tmpl[TETRIO_TEMPLATE_NUM] = {
		0, 2, 1, 1, 1, 1, 1
	};
	static const enum tetrio_block col_tmpl[TETRIO_COLOR_NUM] = {
		TETRIO_GREEN, TETRIO_RED, TETRIO_CYAN, TETRIO_BLUE,
		TETRIO_YELLOW, TETRIO_PURPLE,
	};

	struct tetrio *tet;
	int ti;
	int ci;

	assert(ARRAY_SIZE(vec_tmpl) == TETRIO_TEMPLATE_NUM);
	assert(ARRAY_SIZE(col_tmpl) == TETRIO_COLOR_NUM);

	srand((unsigned int)time(NULL));
	ti = rand() % TETRIO_TEMPLATE_NUM;
	ci = rand() % TETRIO_COLOR_NUM;

	if (!(tet = malloc(sizeof(*tet))))
		return NULL;
	memcpy(tet->arr, vec_tmpl[ti], sizeof(tet->arr));
	for (ti = 0; ti < TETRIO_ARR_SIZE; ++ti)
		tet->arr[ti].x += TETRIO_SCREEN_X / 2;
	tet->org.x = TETRIO_SCREEN_X / 2;
	tet->org.y = orgy_tmpl[ti];
	tet->color = col_tmpl[ci];
	tet->hashit = 0;
	return tet;
}

static void mvside(struct tetrio *tet, short int d)
{
	int i;
	if (tet->org.x + d > TETRIO_SCREEN_X || tet->org.x + d < 0) {
		tet->hashit = 1;
		return;
	}

	for (i = 0; i < TETRIO_ARR_SIZE; ++i)
		if ((tet->hashit = hashit(tet, 0, d)))
			return;

	for (i = 0; i < TETRIO_ARR_SIZE; ++i)
		tet->arr[i].x += d;
	tet->org.x += d;
}

static void passms(struct tetrio *tet, unsigned ms)
{
	clock_t beg;
	int in;

	beg = clock();
	updscr(tet);
	while (((double)(clock() - beg) / CLOCKS_PER_SEC) * 1000 < ms) {
		switch ((in = getch())) {
		case KEY_RIGHT:
			mvside(tet, 1);
			updscr(tet);
			break;
		case KEY_LEFT:
			mvside(tet, -1);
			updscr(tet);
			break;
		}
	}
}

static _Bool hashit(struct tetrio *tet, short int dy, short int dx)
{
	unsigned int i;
	if (tet->org.x >= TETRIO_SCREEN_X 	|| tet->org.x < 0 	||
		tet->org.y >= TETRIO_SCREEN_Y 	|| tet->org.y < 0 	||
		block_vec[tet->org.y][tet->org.x] || tet->hashit)
			return 1;
	for (i = 0; i < TETRIO_ARR_SIZE; ++i)
		if (tet->arr[i].x + dx >= TETRIO_SCREEN_X 	||
		tet->arr[i].y + dy >= TETRIO_SCREEN_Y 		||
		tet->arr[i].x + dx < 0 				||
		tet->arr[i].y + dy < 0 				||
		block_vec[tet->arr[i].y + dy][tet->arr[i].x + dx])
			return 1;
	return 0;
}
