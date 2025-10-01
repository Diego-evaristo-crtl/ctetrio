CC = gcc
CFLAGS = -Wall -Werror -Wextra -Wconversion -O2 -std=c99 -D_POSIX_C_SOURCE=1 \
	-DNDEBUG -march=native -mtune=native -lncurses

main : main.c

.PHONY : clean
clean :
	-rm main 2>/dev/null || true
