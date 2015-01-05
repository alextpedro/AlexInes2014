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
#include <assert.h>

#include "etapa2.h"
#include "common.h"
#include "debug.h"
#include "memory.h"
#include "listas.h"
#include "hashtables.h"
#include <dirent.h>
#include <pthread.h>


/*Prototypes*/
static int compare (const void *, const void *);
void writeBinary(HASHTABLE_T* words, FILE* originalFile, FILE* tmp);

/*Functions*/

/**
 * @brief Compresses a given file using PALZ.
 * @details A new file called <filename>.palz is created with the compressed text.  
 * 
 * @param  *filename - Name of the file to compress.
 */
void compress(char *filename, int function) {
		FILE *originalFile = NULL;
		char* line = NULL;
		size_t len = 0;
		char* token = NULL;
		char* lastStop = NULL;
		HASHTABLE_T* words;
		LISTA_GENERICA_T* lista_aux;
		ITERADOR_T* it;


		originalFile = fopen (filename, "r");
		if (!originalFile) 
			ERROR(1, "fopen failed");
		
		words = tabela_criar(1024, (LIBERTAR_FUNC)free);

		getline(&line, &len, originalFile);

		token = strtok_r(line, " \n\t.,", &lastStop);

		while(token != NULL) {
			
			if(tabela_consultar(words, token) == NULL) {
				unsigned int* index = MALLOC(sizeof(unsigned int)); //freed in tabela_destruir
				tabela_inserir(words, token, index);
			}
	    
	    	token = strtok_r(NULL, " \n\t.,", &lastStop);
		}
		int nelems;
		nelems = tabela_numero_elementos(words);

		int numberOfElements = nelems + 14;
		int numBytes = bytesForInt(numberOfElements);

		if (numBytes > 3)
		{
				fprintf(stderr, "Failed: %s dictionary is too big\n", filename);
				if(function ==1) {
					exit(1);
				} else {
					return;
				}
		}

		lista_aux = tabela_criar_lista_chaves(words);
		it = lista_criar_iterador(lista_aux);
		char *str;

		char **arrayWords = NULL;
		arrayWords = MALLOC (sizeof(char*)*nelems);
		int ind = 0;
		while ((str = iterador_proximo_elemento(it)) != NULL)
			arrayWords[ind++] = str;

		iterador_destruir(&it);
		lista_destruir(&lista_aux);

		qsort(arrayWords, (size_t)nelems, sizeof(char*), compare);

		FILE* tmp = NULL;
		tmp = tmpfile();

		fputs("PALZ\n", tmp);
		fprintf(tmp, "%d\n", nelems);
		

		for (ind = 0; ind < nelems; ind++) {
			unsigned int* index = tabela_consultar(words, arrayWords[ind]); 
			*index = ind+15;
			//printf("%s\n", arrayWords[ind]);
			fputs(arrayWords[ind], tmp);
			fputs("\n", tmp);
		}

		rewind(originalFile);
		//Replace text with numerical indexes
		writeBinary(words,originalFile,tmp);

		//Phase 2
		//Save original filename length
		size_t originalFileLen = strlen(filename);

		FILE *newFile = NULL;
		char *compressedFileName = (char*) MALLOC(sizeof(char)*(strlen(filename)*strlen(".palz")));
		strcpy(compressedFileName,filename);
		newFile = fopen(strcat(compressedFileName, ".palz"), "w");
		if (!newFile) 
			ERROR(1, "fopen failed");	

		//Return filename to the original filename
		filename[originalFileLen] = '\0';
		
		rewind(tmp);

		//write to a new file
		//Copy from the temporary file to the permanent file
		char buffer[8096];
		int n;
		while( (n=fread(buffer, 1, 8096, tmp)) > 0) {
		 	fwrite(buffer, n, 1, newFile);
		}
		fflush(newFile);

		//Clean up files and memory
		fclose(tmp);
		fclose(newFile);
		fclose(originalFile);
		free(compressedFileName);
		free(line);
		free (arrayWords);
		tabela_remover_todos(words);
		tabela_destruir(&words);
}

static int compare (const void *p1, const void *p2) {
	return strcmp(* (char * const *) p1, * (char * const *) p2); //from man qsort
}

/**
 * @brief writeBinary write binary code.
 * @details writes the binary code associated with each word and each tab.
 * 
 * @param  words - hashtable of words present in the original file.
 * @param  originalFile - original file, file given in compress function.
 * @param  tmp - temporary file.
 * 
 */
void writeBinary(HASHTABLE_T* words, FILE* originalFile, FILE* tmp){
	int phrase = 0;
	char* line = NULL;
	size_t len = 0;
	int i = 0;
	char buffer[200];
	int x = 0;
	void * wordValue;
	unsigned int binaryWord;
	int numberOfElements = tabela_numero_elementos(words);
	numberOfElements = numberOfElements+14;
	int numBytes = bytesForInt(numberOfElements);
	int lastSeparator = 0;
	int numRep = 0;

	while((phrase=getline(&line,&len,originalFile)) != -1){
		for(i=0;i<phrase;i++){
			if (isSeparator(line[i]) == 0)//nao e separador
			{
				buffer[x] = line[i];
				x++;
			}
			else
			{
				if (buffer[0] != '\0'){
					writeRepetition(numRep,numBytes,tmp);
					buffer[x] = '\0';
					wordValue = tabela_consultar(words,buffer);
					binaryWord = *((unsigned int *)wordValue);
					fwrite(&binaryWord, numBytes, 1, tmp);
					buffer[0]= '\0';
					x = 0;
					lastSeparator = 0;
					numRep=0;
				}

				if (lastSeparator == 0)
					{
						lastSeparator = separatorIndex(line[i]);
						fwrite(&lastSeparator, numBytes,1 ,tmp);
					}
					else if(lastSeparator==separatorIndex(line[i])){
						numRep++;
						}else{
							writeRepetition(numRep,numBytes,tmp);
							lastSeparator = separatorIndex(line[i]);
							fwrite(&lastSeparator, numBytes,1 ,tmp);
							numRep = 0;
						}
			}
		}

		if (buffer[0]=='\n')//comecar por espaço ou linha
		{
			writeRepetition(numRep,numBytes,tmp);
		}

		FREE(line);
		line = NULL;
	}
	FREE(line);
	writeRepetition(numRep,numBytes,tmp);

	if (buffer[0] != '\0'){ //verificar se termina numa palavra
		buffer[x] = '\0';
		wordValue = tabela_consultar(words,buffer);
		binaryWord = *((unsigned int *)wordValue);
		fwrite(&binaryWord, numBytes, 1, tmp);
		buffer[0]= '\0';
		x = 0;
		wordValue = NULL;
	}
}

/**
 * @brief writeRepetition write repetitions.
 * @details writes the repetitions the number of times that exists in the original file.
 * 
 * @param  numRep - number of repetitions.
 * @param  numBytes - number of bytes necessary to store the parameter int.
 * @param  tmp - temporary file.
 * 
 */
void writeRepetition(int numRep, int numBytes,FILE* tmp){
	int maxRep = (pow(256, numBytes)-1);
	int valueR = 0;
	//printf("%d\n", numRep);
	while (numRep >= maxRep){
		numRep = numRep- maxRep;
		fwrite(&valueR,numBytes,1,tmp);
		fwrite(&maxRep,numBytes,1,tmp);
	}

	if (numRep != 0)
	{
		fwrite(&valueR,numBytes,1,tmp);
		fwrite(&numRep,numBytes,1,tmp);
	}
}

/**
 * @fnFunction Calculates the number of bytes necessary to store an int.
 * @details The function receives an unsigned int and calculates the number of bytes necessary to represent it 
 * 
 * @param unsigned int The number you need to store.
 * @return Returns the number of bytes necessary to store the parameter int.
 */
 
 //dado pelos profs
int bytesForInt(unsigned int value) {
	double nbytes = 1;
	unsigned long long max = 256;

	while(max<=value){
		nbytes++;
		max*=256;
	}
	 
	return nbytes;
}

/**
 * @brief isSeparator checks it is a separator.
 * @details The function receives an character and checks it is a separation.
 * 
 * @param  sp - character.
 * 
 * @return Returns 1 if the character is a separator, returns 0 if it is not.
 */
int isSeparator(char sp){
	switch(sp){
		case '\n': return 1;
		case '\t': return 1;
		case '\r': return 1;
		case ' ': return 1;
		case '?': return 1;
		case '!': return 1;
		case '.': return 1;
		case ';': return 1;
		case ',': return 1;
		case ':': return 1;
		case '+': return 1;
		case '-': return 1;
		case '*': return 1;
		case '/': return 1;
		default: return 0;
	}
}

/**
 * @brief separatorIndex check separator index.
 * @details The function receives a character and checks your index.
 * 
 * @param  sp - character.
 * 
 * @return Returns 0 if not exists, return the index of the separator
 */
int separatorIndex(char sp){
	switch(sp){
		case '\n': return 1;
		case '\t': return 2;
		case '\r': return 3;
		case ' ': return 4;
		case '?': return 5;
		case '!': return 6;
		case '.': return 7;
		case ';': return 8;
		case ',': return 9;
		case ':': return 10;
		case '+': return 11;
		case '-': return 12;
		case '*': return 13;
		case '/': return 14;
		default: return 0;
	}
}
