CC = gcc
CFLAGS = -Wall -Werror -Wextra -Wconversion -O2 -std=c99 -pedantic \
	-march=native -mtune=native -lcurses
OBJS = menu.o main.o


main : $(OBJS) menu.h
	$(CC) $(CFLAGS) $(OBJS) -o main
main.o : main.c menu.h menu.o
menu.o : menu.c menu.h

.PHONY : clean
clean :
	-rm $(OBJS) main 2>/dev/null || true
