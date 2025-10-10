# C-Tetrio - A Tetris Clone made in C using only curses

## Installing

Clone the repository using `git clone`, ensure you have gcc and GNU Make 
installed, and that your system provides curses.h (all UNIX systems do), `cd`
into the directory you just cloned and use the `make` command to compile everything
automatically, `ls` should confirm that an executable ELF file called `ctetrio`
was just built in the current directory, if you'd like to clean object files, 
use `make clean`. An install option is not provided withing the Makefile, but
copying the executable to a file in your $PATH variable should have such effect.


## How to play

You can move the pieces using the arrow keys, rotate them to the right using the
Up key and to the left using the Down key. You can also rotate the next piece
(the one shown at the preview on left side of your screen) using the 'a' and 'd'
keys, the menu can be entered by pressing 'm', navigate trough it using the Up
and Down keys, and choose an option (currently, resuming or exiting) with the
Enter key.


## Modfying the code

If you'd like a different screen size, change the `TETRIO_SCREEN_X` and
`TETRIO_SCREEN_Y` macros at the top of main.c, for different keybindings, change
the case statments at menuwait() on menu.c, or passms() on main.c. For a 
different timing before each tetrio falls a block, change the initial value
of `rnd_ms` inside main() on main.c. All functions have prototypes and comments
at the top of their files.
