#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

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

int main(int argc, char ** argv) {

	if (argc < 2) {
		return;	
	}

	int fd = open(argv[1], O_RDONLY);

	int bytes;
	char buf;

	int n_strings = 10;
	char ** arr = malloc(n_strings * sizeof(char *));

	int idx = 0;
	int current_word_idx = 0;
	int word_size = 5;
	
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
		if (current_word_idx == 4) {
			word_size *= 2;
			arr[idx] = realloc(arr[idx], word_size);
		}
		if (buf == '\n') {
			arr[idx][current_word_idx] = '\0';
		//	printf("in this case\n");
			idx++;	
			current_word_idx = 0;
			word_size = 5;
		}
		else {
			arr[idx][current_word_idx++] = buf;
		}

		//printf("\nloop succeeded\n");
		//printf("current array index: %d, current word idx: %d\n", idx, current_word_idx);

	}

	for (int i = 0; i < idx; i++) {
		printf("%s\n", arr[i]);
	}

	int word_idx = binary_search(arr, "Achaean", idx);
	printf("Word index: %d | Word: %s\n", word_idx, arr[word_idx]);
	printf("\n");
	printf("%s, %s\n", arr[130], arr[131]);
	printf("%d\n", (int) (strcmp(arr[130], arr[131])));
}
