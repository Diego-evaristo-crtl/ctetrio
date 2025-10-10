CC = gcc
CFLAGS = -Wall -Werror -Wextra -Wconversion -O2 -std=c99 -pedantic \
	-march=native -mtune=native -lcurses
OBJS = menu.o main.o


ctetrio : $(OBJS) menu.h
	$(CC) $(CFLAGS) $(OBJS) -o ctetrio
main.o : main.c menu.h menu.o
menu.o : menu.c menu.h

.PHONY : clean
clean :
	-rm $(OBJS) ctetrio 2>/dev/null || true
