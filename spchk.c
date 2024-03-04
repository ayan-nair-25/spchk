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
#define FILE_INITIAL_WORD_SIZE 100

// to read the file and annotate position of words:

// modify this to take the double char array as an input so we can avoid needing the struct
// this isn't working, we need to introduce a triple pointer so that we can modify the actual double array that we pass in 
// otherwise we need to init the double array inside this function and return this + word size in a struct, which is really inefficient for memory
int strcmp_wrap(const void * s1, const void * s2) {
	const char * ss1 = *(const char**) s1;
    	const char * ss2 = *(const char**) s2;
	return strcmp(ss1, ss2);
}

int build_word_dict(char * fname, char *** arr, int n_strings, int initial_word_size) {

	int fd = open(fname, O_RDONLY);

	int bytes;
	char buf;

	int idx = 0;
	int current_word_idx = 0;
	int word_size = initial_word_size;
	
	while((bytes = read(fd, &buf, 1)) > 0) {
		//printf("---new loop iter---\n");
		if (idx == n_strings - 1) {
			//printf("resized array\n");
			n_strings *= 2;
			(*arr) = realloc((*arr), n_strings * sizeof(char *));
		}
		if ((*arr)[idx] == NULL) {
			//printf("created new array\n");
			(*arr)[idx] = malloc(word_size);
		}
		if (current_word_idx == word_size - 1) {
			//printf("doubled array sized and realloced\n");
			word_size *= 2;
			(*arr)[idx] = realloc((*arr)[idx], word_size);
		}
		if (buf == '\n') {
			(*arr)[idx][current_word_idx] = '\0';
			//printf("encountered new line\n");
			//printf("added word %s\n", (*arr)[idx]);
			idx++;	
			current_word_idx = 0;
			word_size = initial_word_size;
		}
		else {
			//printf("inserted character into buffer\n");
			(*arr)[idx][current_word_idx++] = buf;
		}

		//printf("\nloop succeeded\n");
		//printf("current array index: %d, current word idx: %d\n", idx, current_word_idx);
	}
	qsort((*arr), idx, sizeof(char *), strcmp_wrap);
	// debug
	//printf("idx: %d", idx);
	//if (arr == NULL) {
	//	printf("NULL\n");
	//}
	//else {
		//for (int i = 0; i < idx; i++) {
		//	printf("%s\n", (*arr)[idx]);
		//}
	//}
	return idx;
}

void free_dict(char *** dict, int dict_size) {
	for (int i = 0; i < dict_size; i++) {
		free((*dict)[i]);
	}
	free((*dict));
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
void check_word(char * word, int word_length, char ** dict, int dict_size, int linecount, int charcount) {
	word[word_length] = '\0';
	if(!word_in_dict(dict, word, dict_size)) {
		printf("Word not found: (%d %d) |%s|\n", linecount, charcount, word);
	}
}

void reinitialize_word(char * word, int word_size) {
	free(word);
	word = NULL;
	word = malloc(word_size);
}

void annotate_file(char * fname, char ** dict, int dict_size) {
	// init the buffer and check the num bytes
	int fd = open(fname, O_RDONLY);
	char buf;
	int bytes;

	int word_size = FILE_INITIAL_WORD_SIZE;
	char * current_word = malloc(word_size);
	int word_length = 0;

	// this defines our row and column number
	int charcount = 1;
	int linecount = 1;
	//char prev_space = 0;

	// while we still have bytes to read
	while((bytes = read(fd, &buf, 1)) > 0) {
		// we increment our column
		// if we encounter a new line, then we need to move to the next row and reset our column

		if (word_length == word_size) {
			word_size *= 2;
			current_word = realloc(current_word, word_size);
		}

		if (!isspace(buf) && !ispunct(buf)) {
			current_word[word_length++] = buf;
		}

		if (isspace(buf)) {
			if (buf == '\n') {
				linecount++;	
				charcount = 1;
			}
			else {
				charcount++;	
			}
			if (word_length > 0) {

				check_word(current_word, word_length, dict, dict_size, linecount, charcount); 
				reinitialize_word(current_word, word_size);

				word_length = 0;
			}
		}
		else {
			charcount++;
		}
	}

	// close our fd
	int out = close(fd);
	// free any allocated memory that remains
	free(current_word);
}

int check_if_txt(char * str1, char * str2) {
	char * loc = strstr(str1, str2);
	if (loc != NULL) {
		if (*(loc + 4) == '\0') {
			return 1;
		}
	}
	return 0;
}
void scan_dir(char * path, char ** dict, int size) {
	// open the directory and check for failure

	DIR * handle = opendir(path);
	char new_path[1000];

	if (!handle) {
		//perror(path);
		return;
	}


	// create our struct that stores the info from readdir
	struct dirent * de;
	// print out all the directories that we traverse
	while((de = readdir(handle))) {
		char fstart[2] = {de->d_name[0], '\0'};
		//if (strcmp(de->d_name, ".") != 0 && strcmp(de->d_name, "..") != 0) {
		if (strcmp(fstart, ".") != 0 && check_if_txt(de->d_name, "txt") != 0) {
			annotate_file(de->d_name, dict, size);

			strcpy(new_path, path);
			strcat(new_path, "/");
			strcat(new_path, de->d_name);

			scan_dir(new_path, dict, size);
		}
	}

	closedir(handle);
}

int main(int argc, char ** argv) {
	if (argc < 3) {
		printf("Please input a filename.");
		return EXIT_FAILURE;
	}

	char ** dict = malloc(INITIAL_DICT_SIZE * sizeof(char *));
	int size = build_word_dict(argv[1], &dict, INITIAL_DICT_SIZE, INITIAL_WORD_SIZE);
	//for (int k = 0; k < size; k++) {
	//	printf("%s\n", dict[k]);
	//}

	// check for directories here
	// if we encounter a directory:
	// 1. if the directory starts with . or .., remove these characters
	// 2. send through recursive file traversal
	// 3. if a file that we recurse through doesn't start with . and ends with .txt:
	// 4. run program on that file
	// 5. else
	// 6. do nothing

	/*
	for (int i = 2; i < argc; i++) {
		DIR * handle = opendir(argv[i]);
		if (handle == NULL) {
			annotate_file(argv[i], dict, size);
		}
		else {
			closedir(handle);
			scan_dir(argv[i], dict, size);
			//	
		}	
	}
	*/
	printf("%d\n", size);
	for (int i = 0; i < size; i++) {
		printf("Word %s in dict: %d\n", dict[i],  word_in_dict(dict, dict[i], size));
	}

	free_dict(&dict, size);

	return EXIT_SUCCESS;
	
	
}
