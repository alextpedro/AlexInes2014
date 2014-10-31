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
<<<<<<< HEAD
#include <sys/types.h>
#include <sys/stat.h>
=======
>>>>>>> 2a900a9222353a2135804f49ac429150b641489f

#include "etapa1.h"
#include "common.h"
#include "debug.h"
#include "memory.h"

<<<<<<< HEAD
=======
#define DIC_HEAD 15
>>>>>>> 2a900a9222353a2135804f49ac429150b641489f

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
		exit(1);
	}

	//Read the first line for PALZ headline
	char *line = NULL;
	size_t len = 0;
	ssize_t read;

	read = getline(&line, &len, myFile);

	//Verify if it's a valid .palz file.
	if (!is_header_PALZ(line)){
<<<<<<< HEAD
		fprintf(stderr, "Failed: %s is not a valid .palz file\n", filename);
=======
		printf("Failed: %s is not a valid .palz file.\n", filename);
>>>>>>> 2a900a9222353a2135804f49ac429150b641489f
		fclose(myFile);
		exit(1);
	}

	//Read the second line for the number of words in the dictionary.
	read = getline(&line, &len, myFile);
	unsigned int numberOfWords = 0;
<<<<<<< HEAD
	if (!is_valid_size(line, &numberOfWords, filename)) {
		//fprintf(stderr,"Failed: %s is corrupted\n", filename);
=======
	if (!is_valid_size(line, &numberOfWords)) {
		printf("Failed: %s has an invalid number of words.\n", filename);
>>>>>>> 2a900a9222353a2135804f49ac429150b641489f
		fclose(myFile);
		exit(1);
	}

	//Allocate memory for the dictionary
	char **words = MALLOC (sizeof(char*) * (numberOfWords+15) +1);

<<<<<<< HEAD
=======
	unsigned int k = 0;
	for (k = 0; k < numberOfWords+16; ++k) {
		words[k] = MALLOC (sizeof(char)*100);
	}

>>>>>>> 2a900a9222353a2135804f49ac429150b641489f
	//Read each line in the file until the number of words indicated in the header has been reached.
	unsigned int i = 0;
	while (i < numberOfWords && running == 1) {
		read = getline(&line, &len, myFile);
		line [read-1]='\0'; //Removes the line break
		words [i+DIC_HEAD] = strdup(line);
		i++;
	}
	//Initialize the dictionary with separators
	strcpy(words[1],"\n");
	strcpy(words[2],"\r");
	strcpy(words[3],"\t");
	strcpy(words[4]," ");
	strcpy(words[5],"?");
	strcpy(words[6],"!");
	strcpy(words[7],".");
	strcpy(words[8],";");
	strcpy(words[9],",");
	strcpy(words[10],":");
	strcpy(words[11],"+");
	strcpy(words[12],"-");
	strcpy(words[13],"*");
	strcpy(words[14],"/");

	// words[1]="\n";
	// words[2]="\r";
	// words[3]="\t";
	// words[4]=" ";
	// words[5]="?";
	// words[6]="!";
	// words[7]=".";
	// words[8]=";";
	// words[9]=",";
	// words[10]=":";
	// words[11]="+";
	// words[12]="-";
	// words[13]="*";
	// words[14]="/";
	
<<<<<<< HEAD
	
	if(sizeof(words) >= pow(2,24)) {
		fprintf(stderr,"Failed: %s dictionary is too big\n", filename);
	}
=======
	//TODO
	// if(sizeof(words) >= ) {
	// 	printf("Failed: %s dictionary is too big.\n", filename);
	// }
>>>>>>> 2a900a9222353a2135804f49ac429150b641489f

	//start decompression and write to file
	write_to_file(words, filename, myFile, numberOfWords);

	
	
<<<<<<< HEAD
	FREE(words);

=======
	//Free memory from words.
	unsigned int j = 0;
	for (j = 0; j < numberOfWords+16; ++j)
	{
		FREE(words[j]);
	}
	FREE(words);
>>>>>>> 2a900a9222353a2135804f49ac429150b641489f

	if(fclose(myFile) != 0) {
		printf("fclose() failed.\n");
		exit(1);
	}

}

int is_header_PALZ (const char *header_first_row) {
	return (strcmp(header_first_row, "PALZ\n") == 0);		
}

int is_extension_palz (const char *fullfilename) {
	return (strstr(fullfilename, ".palz") ? 0 : -1);		
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

<<<<<<< HEAD
	if (number >= pow(2,24))
=======
	if (number < 0 || number > pow(2,24))
>>>>>>> 2a900a9222353a2135804f49ac429150b641489f
	{
		fprintf(stderr,"Failed: %s dictionary is too big\n", filename);
		return 0;
	}

	*value = number;
	return 1;
}

int bytes_for_int(unsigned int max_value) {
<<<<<<< HEAD
	double nbits = log2(((double)max_value));
=======
	double nbits = log2(((double)max_value)+1);
>>>>>>> 2a900a9222353a2135804f49ac429150b641489f
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
	
	FILE *newDoc = tmpfile();
	unsigned int code = 0;

	//Decompress
	int bytes = bytes_for_int(numberOfWords+15);
	unsigned int prev_code = 0;
	while (fread(&code, bytes , 1, compressed)>0 && running == 1) {
		//detect repetitions
		if (code == 0 ) {
			//check if prev code exists and is separator
			for (i = 1; i < DIC_HEAD && running == 1; ++i) {
				//prev code is a separator
				if(prev_code >= 4 && prev_code < 15){
					break; //exit for cycle
				}
				else { return -1; } //error leave function
			}

			//read next line
			fread(&code, bytes , 1, compressed);
			while (code != 0 && running ==1) {
				fputs (words[prev_code], newDoc);
				code--;
			}
		}//code is not a repetition
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
	if (ptr != NULL && strcasecmp(ptr, ".palz") == 0 && running == 1)
	{
		*ptr = 0; 
	}

	//Write to file
	FILE *permFile = NULL;

	permFile = fopen(filename, "w");
	if (permFile == NULL) {
		printf("fopen() failed.\n");
	}
	rewind(tmpFile);

	//Copy from the temporary file to the permanent file
	char buffer[8096];
	int n;
	while( (n=fread(buffer, 1, 8096, tmpFile) && running == 1) > 0) {
	 	fwrite(buffer, 1, n, permFile);
	}


	//Return the compression ratio
	float compressedSize;
	float newDocSize;
	compressedSize = ftell(compressed);
	newDocSize = ftell(newDoc);
	printf("Compression ratio: %s %4.2f%%\n", filename, compression_ratio(compressedSize, newDocSize));

	if ((fclose(myFile) != 0) || ((fclose(newDoc) != 0) && (running == 1))) {
		printf("fclose() failed.\n");
		exit(1);
	}
	
	return 0; //success
}

/**
 * @brief [brief description]
 * @details [long description]
 * 
 * @param dirname [description]
 * @return [description]
 */
int folderDecompress(const char *dirname) {

    DIR *dir = opendir(dirname);
    
    if( dir == NULL ) {

        WARNING("opendir() failed");
        return -1;

    }

    struct dirent *pDirent;

    while( running && (pDirent = readdir(dir)) != NULL ) {

        if( strcmp(pDirent->d_name, ".") == 0 || strcmp(pDirent->d_name, "..") == 0 )
            continue;

        char *fullname = MALLOC(strlen(dirname) + strlen(pDirent->d_name) + 2);

        sprintf(fullname, "%s/%s", dirname, pDirent->d_name);

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
				//printf("ext null %s\n", fullname);
				fprintf(stderr, "Failed: %s is not a valid .palz file\n", fullname);
               
			}else{
				printf("Decompress file %s\n", fullname);
            	decompress(fullname);
			}
		         
        }

        FREE(fullname);

    }

    closedir(dir);
    return 0;

}
