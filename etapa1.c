/*@author Alexandra Pedro 2131314@my.ipleiria.pt && 
 * Inês Faria 2110922@my.ipleiria.pt */
#include <stdio.h>
#include <memory.h>
#include <errno.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>

#include "etapa1.h"
#include "common.h"
#include "debug.h"
#include "memory.h"

/**
 * @brief Decompresses a .palz file 
 * @details Function receives a filename, opens it, verifies that it is a valid .palz file with a valid dictionary and starts the proceedures to decompress the file into a new one, or itself, depending on whether or not the file given has a .palz extension.
 * 
 * @param filename Pointer to filename passed by the command line arguments. 
 */
void decompress (char *filename) {
	//Open the given file
	FILE *myFile = NULL;

	myFile = fopen(filename, "r");

	if (myFile == NULL){
		printf("fopen() failed.\n");
	}

	//Read the first line for PALZ headline
	char *line = NULL;
	size_t len = 0;
	ssize_t read;

	read = getline(&line, &len, myFile);

	//Verify if it's a valid .palz file.
	if (!is_header_PALZ(line)){
		printf("Failed: %s is not a valid .palz file.\n", filename);
		fclose(myFile);
		exit(1);
	}

	//Read the second line for the number of words in the dictionary.
	read = getline(&line, &len, myFile);
	unsigned int numberOfWords = 0;
	if (!is_valid_size(line, &numberOfWords)) {
		printf("Failed: %s has an invalid number of words.\n", filename);
		fclose(myFile);
		exit(1);
	}

	//Allocate memory for the dictionary
	char **words = malloc (sizeof(char*)*(numberOfWords+15));

	//Read each line in the file until the number of words indicated in the header has been reached.
	unsigned int i = 0;
	while (i < numberOfWords) {
		read = getline(&line, &len, myFile);
		line [read-1]=0; //Removes the line break
		words [i+15] = strdup(line);
		i++;
	}
	//Initialize the dictionary with separators
	words[1]="\n";
	words[2]="\r";
	words[3]="\t";
	words[4]=" ";
	words[5]="?";
	words[6]="!";
	words[7]=".";
	words[8]=";";
	words[9]=",";
	words[10]=":";
	words[11]="+";
	words[12]="-";
	words[13]="*";
	words[14]="/";
	
	//TODO
	// if(sizeof(words) >= ) {
	// 	printf("Failed: %s dictionary is too big.\n", filename);
	// }

	//start decompression and write to file
	write_to_file(words, filename, myFile, numberOfWords);
	
	//Free memory from words.
	int j = 0;
	for (j = numberOfWords+15; j >= 0; j--)
	{
		free(words[j]);
	}
	
	if(fclose(myFile) != 0) {
		printf("fclose() failed.\n");
		exit(1);
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

	if (number < 0 || number > pow(2,24))
	{
		return 0;
	}

	*value = number;
	return 1;
}

int bytes_for_int(unsigned int max_value) {
	double nbits = log2(((double)max_value)+1);
	int nbytes = ceil(nbits/8); 

	return nbytes;
}

/**
 * @brief Writes decompressed text to file.
 * @details write_to_file receives a pointer to a vector of words, a filename, and the number of words. It writes the content of the string vector to a file. It returns -1 if there was an error and 0 if the operation was succesful. If the file received doesn't have a .palz extension, it will be replaced. Otherwise, a new file, with the same name but no .palz extension, will be created.
 * 
 * @param words Pointer to data which will be saved in the file.
 * @param filename Name of the original (compressed) file.
 * @param int Number of words in the dictionary.
 * @return Returns 0 on success.
 */
int write_to_file (char** words, char *filename, FILE* compressed, unsigned int numberOfWords) {
	//TODO unfinished 
	FILE *tmpFile = tmpfile();
	unsigned int code = 0;

	//Decompress
	int bytes = bytes_for_int(numberOfWords+15);
	unsigned int prev_code = 0;
	int i;
	while (fread(&code, bytes , 1, compressed) > 0) {
		//detect repetitions
		if (code == 0 ) {
			//check if prev code exists and is separator
			for (i = 1; i < 15; ++i) {
				//prev code is a separator
				if(prev_code >= 4 && prev_code < 15){
					break; //exit for cycle
				}
				else { return -1; } //error leave function
			}

			//read next line
			fread(&code, bytes , 1, compressed);
			while (code != 0) {
				fputs (words[prev_code], tmpFile);
				code--;
			}
		}

		//code is not a repetition
		else {
			fputs (words[code], tmpFile);
			prev_code = code;
		}
	}

	if (!feof(compressed)){
		//erro de leitura
		DEBUG("read error");
	}


	//Searches and removes .palz extension if it exists
	char *ptr = NULL;
	ptr = strrchr(filename, '.');
	if (ptr != NULL && strcasecmp(ptr, ".palz") == 0)
	{
		*ptr = 0; 
	}

	//Write to file
	FILE *permFile = NULL;

	permFile = fopen(filename, "w");
	rewind(tmpFile);

	//Copy from the temporary file to the permanent file
	char buffer[8096];
	int n;
	while( (n=fread(buffer, 1, 8096, tmpFile)) > 0) {
	 	fwrite(buffer, 1, n, permFile);
	}
	fflush(tmpFile);


	//Return the compression ratio
	long permFileSize;
	long tmpFileSize;
	permFileSize = ftell(permFile);
	tmpFileSize = ftell(tmpFile);
	printf("Compression ratio:%ld\n", compression_ratio(tmpFileSize, permFileSize));

	if (fclose(permFile) != 0 || fclose(tmpFile) != 0) {
		printf("fclose() failed.\n");
		exit(1);
	}

	return 0; //success
}

void folderDecompress (char *folder) {
	//TODO
}