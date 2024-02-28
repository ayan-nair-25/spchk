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

typedef struct {
	char ** dict;	
	int size;
} ret_dict;

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

char word_in_dict(char ** dict, char * word, int size) {
	int out = binary_search(dict, word, size);
	if (out == -1) {
		return 0;	
	}
	return 1;

}

void annotate_file(char * fname, char ** dict, int size) {
	// init the buffer and check the num bytes
	int fd = open(fname, O_RDONLY);
	char buf;
	int bytes;

	// this defines our row and column number
	int charcount = 1;
	int linecount = 1;
	char prev_space = 0;

	// while we still have bytes to read
	while((bytes = read(fd, &buf, 1)) > 0) {
		// we increment our column
		// if we encounter a new line, then we need to move to the next row and reset our column

		if(charcount == 1 && linecount == 1 && !isspace(buf)) {
			printf("(%d, %d)", linecount, charcount);
		}

		if (buf == '\n') {
			printf("\n");
			linecount++;	
			prev_space = 1;
			charcount = 1;
		}
		else if (isspace(buf)) {
			charcount++;	
			prev_space = 1;
		}
		else if(prev_space && !isspace(buf)) {
			printf("\n (%d, %d)", linecount, charcount);
			printf("%c", buf);
			charcount++;
			prev_space = 0;
		}
		// if we get a space, then we encounter a word and hence we print the position
		else{
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

	int n_strings_in_arr = INITIAL_DICT_SIZE;
	char ** dict = malloc(n_strings_in_arr * sizeof(char *));
	int size = build_word_dict(argv[1], dict, INITIAL_DICT_SIZE, INITIAL_WORD_SIZE);
	annotate_file(argv[2], dict, size);

	return EXIT_SUCCESS;
}
