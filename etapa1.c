/*@author Alexandra Pedro 2131314@my.ipleiria.pt && 
 * Inês Faria 2110922@my.ipleiria.pt */
#include <stdio.h>
#include <memory.h>
#include <errno.h>
#include <stdlib.h>
#include <limits.h>

#include "etapa1.h"
#include "common.h"
#include "debug.h"
#include "memory.h"

typedef struct text {
	char text[50];
}

typedef struct dictionary {
	text* word;
}

void initialize_dictionary (unsigned int numberOfWords, char** strings) {
	dictionary dict = malloc(sizeof(text)*numberOfWords + 14); //treat errors

	//Fill the first indexes with separators
	strcpy(dict[1]->word,"\n");
	strcpy(dict[2]->word,"\t");
	strcpy(dict[3]->word,"\r");
	strcpy(dict[4]->word," ");
	strcpy(dict[5]->word,"?");
	strcpy(dict[6]->word,"!");
	strcpy(dict[7]->word,".");
	strcpy(dict[8]->word,";");
	strcpy(dict[9]->word,",");
	strcpy(dict[10]->word,":");
	strcpy(dict[11]->word,"+");
	strcpy(dict[12]->word,"-");
	strcpy(dict[13]->word,"*");
	strcpy(dict[14]->word,"/");

	//Fill the following indexes with the words.
	for (i = 0; i < numberOfWords; i++){
		//TODO
	}
}

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
		printf("Failed: %s dictionary is too big. \n", filename);
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
	//TODO: Write words to dictionary

	//TODO: Write words to file. 
	for (i = 0; i < numberOfWords; i++) {
		DEBUG ("%s", strings[i]);
	}
	write_to_file(strings, filename, numberOfWords);
	

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
	FILE *newDoc = tmpfile();

	unsigned int i = 0;
	while (i < numberOfWords) {
			fputs(strings[i], newDoc); //EOF if error
			i++;
	} 
	rewind(newDoc);

	
	char *ptr = NULL;
	DEBUG ("%s", filename);
	ptr = strrchr(filename, '.');
	if (ptr != NULL && strcasecmp(ptr, ".palz") == 0)
	{
		*ptr = 0; //removes the palz
	}


	FILE *myFile = NULL;
	myFile = fopen(filename, "w");

	char buffer[2048];
	int n;
	 while( (n=fread(buffer, 1, 2048, newDoc)) > 0) {
	 	fwrite(buffer, 1, n, myFile);
	 }

	 long myFileSize;
	 long newDocSize;
	 myFileSize = ftell(myFile);
	 newDocSize = ftell(newDoc);
	 printf("Compression ratio:%ld\n", compression_ratio(newDocSize, myFileSize));

	 fclose(myFile);
	 fclose(newDoc);

	return 0; //success
}