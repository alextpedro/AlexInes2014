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
};

typedef struct dictionary {
	text* word;
};

/**
 * @brief Initializes the dictionary for the provided file.
 * @details The function receives the number of words and a vector of the words in the dictionary. It initializes the dictionary with standard separators in indexes 1 to 14 and words from index 15 to 14 + numberOfWords.
 * 
 * @param int Number of words in the dictionary.
 * @param words Words in the dictionary.
 * 
 * @return Pointer to a dictionary structure.
 */
dictionary initialize_dictionary (unsigned int numberOfWords, char** words) {
	int size = numberOfWords+14;
	dictionary *dict = malloc(sizeof(text)*size); //treat errors

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
	for (int i = 15; i <= size; ++i) {
		for (int j = 0; j < count; ++j) {
			strcpy(dict[i]->word, words[j]);
		}
	}

	return dict;
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
		printf("Failed: %s has an invalid number of words. \n", filename);
		exit(1);
	}

	//Allocate memory for the words
	char **words = malloc (sizeof(char*)*numberOfWords);

	//Read each line in the file until the number of words indicated in the header has been reached.
	unsigned int i = 0;
	while (i < numberOfWords) {
		read = getline(&line, &len, myFile);
		line [read-1]=0; //Removes the line break
		words [i] = strdup(line);
		i++;
	}
	//Initialize the dictionary
	dictionary* dict = NULL;
	dict = initialize_dictionary (numberOfWords, words);


	//TODO: Write words to file. 
	for (i = 0; i < numberOfWords; i++) {
		DEBUG ("%s", words[i]);
	}
	write_to_file(words, filename, numberOfWords);
	

	free(words);
	if(fclose(myFile) != 0) {
		//treat error
	}
}

int is_header_PALZ (const char *header_first_row) {
	return (strcmp(header_first_row, "PALZ\n") == 0);		
}

/**
 * @brief Turns a number as string into a number that can be used as measure for the dictionary.
 * @details [long description]
 * 
 * @param str String read from the second line of palz
 * @param int Pointer to an unsigned int that will be set to the verified value represented by the string.
 * 
 * @return Returns 1 if successful, 0 otherwise.
 */
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

/**
 * @brief Writes decompressed text to file.
 * @details write_to_file receives a pointer to a vector of strings, a filename, and the number of words. It writes the content of the string vector to a file. It returns -1 if there was an error and 0 if the operation was succesful. If the file received doesn't have a .palz extension, it will be replaced. Otherwise, a new file, with the same name but no .palz extension, will be created.
 * 
 * @param strings Pointer to data which will be saved in the file.
 * @param filename Name of the original (compressed) file.
 * @param int Number of words in the dictionary.
 * @return Returns 0 on success.
 */
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