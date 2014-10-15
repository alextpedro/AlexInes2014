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
		printf("Failed: %s is not a valid .palz file. \n", filename);
		exit(1);
	}

	//Read the second line for the number of words 
	read = getline(&line, &len, myFile);
	unsigned int numberOfWords = 0;
	if (!is_valid_size(line, &numberOfWords)) {
		//TODO Why did it actually fail? Too big? Not valid? Too small?
		printf("Failed: %s dictionary is too big. \n", filename); //this error message might not be related to this function.
		exit(1);
	}

	//Allocate memory for the words
	char **strings = malloc (sizeof(char*)*numberOfWords);

	//Read each line in the file until the number of words indicated in the header has been reached.
	unsigned int i = 0;
	while (i < numberOfWords) {
		read = getline(&line, &len, myFile);
		line [read-1]=0; //Removes the line break
		strings [i] = strdup(line);
		i++;
	}

	//TODO: Write words to file. 
	write_to_file(strings, filename, numberOfWords);
	for (i = 0; i < numberOfWords; i++) {
		DEBUG ("%s", strings[i]);
	}

	free(strings);
	if(fclose(myFile) != 0) {
		//treat error
	}
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

/********************************************
	\brief Writes a string vector to a file.

	write_to_file receives a pointer to a vector of strings, a filename, and the number of words. It writes the content of the string vector to a file. It returns -1 if there was an error and 0 if the operation was succesful. If the file received doesn't have a .palz extension, it will be replaced. Otherwise, a new file, with the same name but no .palz extension, will be created.
*********************************************/
int write_to_file (char** strings, char *filename, unsigned int numberOfWords) {
	//TODO unfinished 
	//tmpfile function?
	FILE *newDoc = tmpfile();

	unsigned int i = 0;
	while (i < numberOfWords) {
			fputs(strings[i], newDoc); //EOF if error
			i++;
	} 

	return 0; //success
}