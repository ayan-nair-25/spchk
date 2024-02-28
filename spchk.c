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
#define INITIAL_WORD_SIZE 5
#define INITIAL_DICT_SIZE 10

// to read the file and annotate position of words:

// modify this to take the double char array as an input so we can avoid needing the struct
int build_word_dict(char * fname, char ** arr, int n_strings, int initial_word_size) {
	int fd = open(fname, O_RDONLY);

	int bytes;
	char buf;

	int idx = 0;
	int current_word_idx = 0;
	int word_size = initial_word_size;
	
	while((bytes = read(fd, &buf, 1)) > 0) {
		// printf("%c", buf);
		if (idx == n_strings - 1) {
			// printf("resized array\n");
			n_strings *= 2;
			arr = realloc(arr, n_strings * sizeof(char *));
		}
		if (arr[idx] == NULL) {
			arr[idx] = malloc(word_size);
		}
		if (current_word_idx == word_size - 1) {
			word_size *= 2;
			arr[idx] = realloc(arr[idx], word_size);
		}
		if (buf == '\n') {
			arr[idx][current_word_idx] = '\0';
		//	printf("in this case\n");
			idx++;	
			current_word_idx = 0;
			word_size = initial_word_size;
		}
		else {
			arr[idx][current_word_idx++] = buf;
		}

		//printf("\nloop succeeded\n");
		//printf("current array index: %d, current word idx: %d\n", idx, current_word_idx);
	}
	// debug
	// for (int i = 0; i < idx; i++) {
        //	printf("%s\n", arr[i]);
	//}
	return idx;
}

int binary_search(char ** arr, char * target, int length) {
	int left = 0;
	int right = length - 1;

	while (left <= right) {
		int mid = ((int)(left + right)) / 2;
		if (strcmp(target, arr[mid]) == 0) {
			return mid;	
		}
		else if (strcmp(target, arr[mid]) < 0) {
			right = mid - 1;
		}
		else {
			left = mid + 1;	
		}
	}
	return -1;
}

int word_in_dict(char ** dict, char * word, int size) {
	int out = binary_search(dict, word, size);
	return !(out == -1);
}

// need to include some way to keep track of each word, and put it into the dictionary to see if it is a word
void annotate_file(char * fname, char ** dict, int size) {
	// init the buffer and check the num bytes
	int fd = open(fname, O_RDONLY);
	char buf;
	int bytes;

	int word_size = 100;
	char * current_word = malloc(word_size);
	int word_length = 0;

	// this defines our row and column number
	int charcount = 1;
	int linecount = 1;
	char prev_space = 0;

	// while we still have bytes to read
	while((bytes = read(fd, &buf, 1)) > 0) {
		// we increment our column
		// if we encounter a new line, then we need to move to the next row and reset our column

		// CHANGE THIS 100 TO A MACRO
		if (word_length == 100) {
			word_size *= 2;
			current_word = realloc(current_word, word_size);
		}

		if (!isspace(buf)) {
			current_word[word_length++] = buf;
		}

		if(charcount == 1 && linecount == 1 && !isspace(buf)) {
			printf("(%d, %d)", linecount, charcount);
		}

		if (buf == '\n') {
			printf("\n");
			linecount++;	

			if (word_length > 0) {
				current_word[word_length] = '\0';
				printf("\n Found word %s\n", current_word);
			}

			free(current_word);
			current_word = NULL;

			current_word = malloc(word_size);
			word_length = 0;

			prev_space = 1;
			charcount = 1;
		}
		else if (isspace(buf)) {
			charcount++;	
			if (word_length > 0) {
				current_word[word_length] = '\0';
				printf("\n Found word %s\n", current_word);
			}

			free(current_word);
			current_word = NULL;

			current_word = malloc(word_size);
			word_length = 0;

			prev_space = 1;
		}
		else if (prev_space && !isspace(buf)) {
			printf("\n (%d, %d)", linecount, charcount);
			printf("%c", buf);

			charcount++;
			prev_space = 0;
		}
		// if we get a space, then we encounter a word and hence we print the position
		else {
	 		printf("%c", buf);
			charcount++;
		}
	}
}

int main(int argc, char ** argv) {
	if (argc < 3) {
		printf("Please input a filename.");
		return EXIT_FAILURE;
	}

	char ** dict = malloc(INITIAL_DICT_SIZE * sizeof(char *));
	int size = build_word_dict(argv[1], dict, INITIAL_DICT_SIZE, INITIAL_WORD_SIZE);
	annotate_file(argv[2], dict, size);

	return EXIT_SUCCESS;
}
