#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>

void scan_dir(char * path) {
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
		if (strcmp(de->d_name, ".") != 0 && strcmp(de->d_name, "..") != 0) {
			printf("%s\n", de->d_name);
			strcpy(new_path, path);
			strcat(new_path, "/");
			strcat(new_path, de->d_name);

			scan_dir(new_path);
		}
	}

	closedir(handle);
}

int main(int argc, char ** argv) {
	if (argc < 2) {
		printf("Please specify a dir\n");
		return EXIT_FAILURE;
	}

	scan_dir(argv[1]);
	return EXIT_SUCCESS;
	
}
