CC = gcc
CFLAGS = -Wall

spchk.o: spchk.c
	$(CC) $(CFLAGS) spchk.c -o spchk
