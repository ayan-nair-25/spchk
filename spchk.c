#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>

// include directory traversal
// read file, generate sequence of words annotated by position
// check if the word is in the dictionary

// implement word search: to do this we need:
// 	1. a function to insert everything into an array of words
//	2. a function to implement a binsearch on this array of words

#define DEBUG 0

// to read the file and annotate position of words:

void annotate_file(int fd) {
	// init the buffer and check the num bytes
	char buf;
	int bytes;

	// this defines our row and column number
	int charcount = 0;
	int linecount = 1;

	// while we still have bytes to read
	while((bytes = read(fd, &buf, 1)) > 0) {
		// we increment our column
		charcount++;
		// if we encounter a new line, then we need to move to the next row and reset our column
		if (buf == '\n') {
			printf("\n");
			linecount++;	
			charcount = 1;
		}
		// if we get a space, then we encounter a word and hence we print the position
		else if(isspace(buf)) {
			printf("\n (%d, %d)", linecount, charcount);
		}
		else if(charcount == 1 && linecount == 1) {
			printf("(%d, %d)", linecount, charcount);
			printf("%c", buf);
		}
		// otherwise just print the char
		else {
			printf("%c", buf);
		}

	}

}
int main(int argc, char ** argv) {

	
	if (argc < 2) {
		printf("Please input a filename.");
		return EXIT_FAILURE;
	}

	int fd = open(argv[1], O_RDONLY);
	annotate_file(fd);

	return EXIT_SUCCESS;
}
