/*@author Alexandra Pedro 2131314@my.ipleiria.pt && 
 * Inês Faria 2110922@my.ipleiria.pt */
#include <stdio.h>
#include <memory.h>
#include <errno.h>
#include <stdlib.h>
#include <limits.h>

#include "etapa1.h"
#include "debug.h"
#include "memory.h"

void decompress (char *filename) {
	//Open the given file
	FILE *myFile = NULL;
	myFile = fopen(filename, "r");

	//Read the first line for PALZ headline
	char *line = NULL;
	size_t len = 0;
	ssize_t read;

	read = getline(&line, &len, myFile);

	if (!is_header_PALZ(line)){
		printf("Invalid file header.\n");
		exit(1);
	}

	//Read the second line for the number of words 
	read = getline(&line, &len, myFile);
	unsigned int numberOfWords = 0;
	if (!is_valid_size(line, &numberOfWords)) {
		printf("Invalid word size in %s\n", filename);
		exit(1);
	}

	//Allocate memory for the words
	char **strings = malloc (sizeof(char*)*numberOfWords);

	//Read each line in the file until the number of words indicated in the header has been reached.
	//This is supposedly the end of the file.
	unsigned int i = 0;
	while (i < numberOfWords) {
		read = getline(&line, &len, myFile);
		line [read-1]=0; //Removes the line break
		strings [i] = strdup(line);
		i++;
	}

	//TODO: Write words to file. If file has .palz extension, create a new file. If not, replace it.
	for (i = 0; i < numberOfWords; i++) {
		DEBUG ("%s", strings[i]);
	}

	free(strings);
	fclose(myFile);
}

int is_header_PALZ (const char *header_first_row) {
	return (strcmp(header_first_row, "PALZ\n") == 0);		
}

int is_valid_size(const char *str, unsigned int *value) {
	char *endptr;

	errno=0;
	long long number = strtoll(str, &endptr, 10);
	if (*endptr != '\0' && *endptr != '\n')
	{
		return 0;
	}

	if (errno != 0)
	{
		return 0;
	}

	if (number < 0 || number > UINT_MAX)
	{
		return 0;
	}

	*value = number;
	return 1;
}