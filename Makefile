CC = gcc
CFLAGS = -Wall -fsanitize=leak

spchk.o: spchk.c
	$(CC) $(CFLAGS) spchk.c -o spchk
