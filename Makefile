CC = gcc
CFLAGS = -Wall -Werror -Wextra -Wconversion -O2 -std=c99 -pedantic \
	-march=native -mtune=native -lncurses

main : main.c

.PHONY : clean
clean :
	-rm main 2>/dev/null || true
