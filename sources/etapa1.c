/*@author Alexandra Pedro 2131314@my.ipleiria.pt && 
 * Inês Faria 2110922@my.ipleiria.pt */
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <errno.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "etapa1.h"
#include "common.h"
#include "debug.h"
#include "memory.h"
#include "listas.h"

#define DICT 15
/**
 * @brief Decompresses a .palz file 
 * @details Function receives a filename, opens it, verifies that it is a valid .palz file with a valid dictionary and starts the proceedures to decompress the file into a new file, or itself, depending on whether or not the file given has a .palz extension.
 * 
 * @param filename Pointer to filename.
 */
void decompress (char *filename, int function) {
	//Open the given file
	FILE *myFile = NULL;
	myFile = fopen(filename, "r");

	//Read the first line for PALZ headline
	char *line = NULL;
	size_t len = 0;
	ssize_t read;

	read = getline(&line, &len, myFile);
	int palz = is_header_PALZ(line);
	//Verify if it's a valid .palz file.
	if (!palz){
		fprintf(stderr, "Failed: %s is not a valid .palz file\n", filename);
		fclose(myFile);
		if(function ==1) {
			exit(1);
		} else {
			FREE (line);
			return;
		}
	}

	//Read the second line for the number of words 
	read = getline(&line, &len, myFile);
	unsigned int numberOfWords = 0;
	int size = is_valid_size(line, &numberOfWords, filename);
	if (!size) {
		if(function == 1) {
			exit(1);
		} else {
			FREE (line);
			return;
		}
	}

	//Allocate memory for the words
	char **words = MALLOC (sizeof(char*)*(numberOfWords+DICT));


	//Read each line in the file until the number of words indicated in the header has been reached.
	unsigned int i = 0;
	while (running && i < numberOfWords) {
		read = getline(&line, &len, myFile);
		if(read == -1) {
			fprintf(stderr, "Failed: %s is corrupted\n", filename);
			if(function ==1) {
				exit(1);
			} else {
				FREE (line);
				FREE (words);
				return;
			}
		}
		line [read-1]=0; //Removes the line break
		words [i+DICT] = strdup(line);
		i++;
	}
	//Initialize the dictionary
	words[1]="\n";
	words[3]="\r";
	words[2]="\t";
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
	
	//Verify dictionary file
	if(sizeof(words) >= pow(2,24)) {
		fprintf(stderr,"Failed: %s dictionary is too big\n", filename);
	}

	//start decompression and write to file
	write_to_file(words, filename, myFile, numberOfWords, function);
	
	unsigned int auxis;
	for (auxis = DICT; auxis < numberOfWords+DICT; auxis++){
		FREE(words[auxis]);
	}
	FREE(words);
	FREE (line);
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
int is_valid_size(const char *str, unsigned int *value, char *filename) {
	char *endptr;

	errno=0;
	long long number = strtoll(str, &endptr, 10);
	if (number < 0 || (*endptr != '\0' && *endptr != '\n'))
	{
		fprintf(stderr, "Failed: %s is corrupted\n", filename);
		return 0;
	}

	if (errno != 0)
	{
		return 0;
	}

	if (number >= pow(2,24))
	{
		fprintf(stderr,"Failed: %s dictionary is too big\n", filename);
		return 0;
	}

	*value = number;
	return 1;
}

/**
 * @brief Calculates the number of bytes necessary to store an int.
 * @details The function receives an unsigned int and calculates the number of bytes necessary to represent it 
 * 
 * @param unsigned int The number you need to store.
 * @return Returns the number of bytes necessary to store the parameter int.
 */
int bytes_for_int(unsigned int max_value) {
	double nbits = log2(((double)max_value));
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
int write_to_file (char** words, char *filename, FILE* compressed, unsigned int numberOfWords, int function) {
	
	FILE *newDoc = tmpfile();
	unsigned int code = 0;

	//Decompress
	int bytes = bytes_for_int(numberOfWords+15);
	unsigned int prev_code = 0;
	while (running && fread(&code, bytes , 1, compressed)>0) {

		if(code > numberOfWords+14) {
			fprintf(stderr, "Failed: %s is corrupted\n", filename);
			if(function ==1) {
				exit(1);
			} else {
				return 0;
			}
		}
		//detect repetitions
		if (code == 0 ) {
			//check if prev code exists and is separator
			//read next line
			fread(&code, bytes , 1, compressed);
			while (code != 0) {
				fputs (words[prev_code], newDoc);
				code--;
				}
				
			}//code is not a repetition
			else {
				fputs (words[code], newDoc);
				prev_code = code;
			}
	}

	if (!feof(compressed)){
		fprintf(stderr, "fread() on %s failed.\n", filename);
	}


	//Searches and removes .palz extension if it exists
	char *ptr = NULL;
	ptr = strrchr(filename, '.');
	if (ptr != NULL && strcasecmp(ptr, ".palz") == 0 && running == 1)
	{
		*ptr = 0; 
	}

	//Write to file
	FILE *outFile = NULL;
	rewind(newDoc);
	outFile = fopen(filename, "w");

	//Copy from the temporary file to the permanent file
	char buffer[8096];
	int n;
	while( (n=fread(buffer, 1, 8096, newDoc)) > 0) {
	 	fwrite(buffer, 1, n, outFile);
	}
	fflush(outFile);
	fclose(newDoc);


	//Return the compression ratio
	float outFileSize;
	float compressedSize;
	compressedSize = ftell(compressed);
	outFileSize = ftell(outFile);
	printf("Compression ratio: %s %4.2f%%\n", filename, compression_ratio(compressedSize, outFileSize));

	if ((fclose(outFile) != 0) || ((fclose(compressed) != 0) && (running == 1))) {
		printf("fclose() failed.\n");
		exit(1);
	}
	
	return 0; //success
}

/**
 * @brief Decompresses all palz files in a folder
 * @details Receives a folder name and scans it looking for palz files. If it finds one it calls the decompression routine.
 * 
 * @param dirname Name of the folder (directory) to decompress.
 * @return Returns 0 if succesful.
 */
int folderDecompress (const char *dirname) {
	int function = 2;
	LISTA_GENERICA_T* listOfDir = lista_criar(NULL);
	lista_inserir_inicio(listOfDir, strdup(dirname));

	while (lista_numero_elementos(listOfDir) > 0) {
		char *fdirname = lista_remover_inicio(listOfDir);
		DIR *workdir = opendir(fdirname);
		struct dirent *entry;

		while ( ( entry = readdir(workdir) ) ) {
			if( strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0 )
            	continue;

        	char *fullname = MALLOC(strlen(dirname) + strlen(entry->d_name) + 2);

        	sprintf(fullname, "%s/%s", dirname, entry->d_name);

        	struct stat info;

        	if( lstat(fullname, &info) != 0 ) {

            WARNING("lstat() failed for item %s", fullname);
            return -1;

        	}

	        if( S_ISDIR(info.st_mode) ) {

	            folderDecompress(fullname);

	        } else {

				if (is_extension_palz(fullname) == -1)
				{
					FREE(fullname);
					continue;
	               
				}else{
					printf("Decompress file %s\n", fullname);
	            	decompress(fullname, function);
				}
		         
        	}

        	FREE(fullname);
		}
		closedir(workdir);
		FREE(fdirname);
	}

	lista_destruir(&listOfDir);
	return 0;
}