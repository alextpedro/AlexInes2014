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

#include "etapa2.h"
#include "common.h"
#include "debug.h"
#include "memory.h"
#include "listas.h"
#include "hashtables.h"

static int compare (const void *, const void *);


/**
 * @brief Compresses a given file using PALZ.
 * @details A new file called <filename>.palz is created with the compressed text.  
 * 
 * @param  *filename - File to compress.
 */
void compress(char *filename) {
		FILE *text = NULL;
		char* line = NULL;
		size_t len = 0;
		char* token = NULL;
		char* lastStop = NULL;
		HASHTABLE_T* words;
		LISTA_GENERICA_T* lista_aux;
		ITERADOR_T* it;


		text = fopen (filename, "r");
		if (!text) 
			ERROR(1, "fopen failed");
		
		words = tabela_criar(1024, (LIBERTAR_FUNC)free);

		getline(&line, &len, text);

		token = strtok_r(line, " \n\t.,", &lastStop);

		while(token != NULL) {
			
			if(tabela_consultar(words, token) == NULL) {
				unsigned int* index = MALLOC(sizeof(unsigned int)); //freed in tabela_destruir
				tabela_inserir(words, token, index);
				//DEBUG( "%s\n", token );
			}
	    
	    	token = strtok_r(NULL, " \n\t.,", &lastStop);
		}
		int nelems;
		nelems = tabela_numero_elementos(words);
		printf("%d\n", nelems);

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

		for (ind = 0; ind < nelems; ind++) {
			unsigned int* index = tabela_consultar(words, arrayWords[ind]); 
			*index = ind+15;
			printf("%s\n", arrayWords[ind]);
			fputs(arrayWords[ind], tmp);
		}

		//Fase 2

		fclose(tmp);
		fclose(text);
		free(line);
		free (arrayWords);
		tabela_destruir(&words);
}

static int compare (const void *p1, const void *p2) {
	return strcmp(* (char * const *) p1, * (char * const *) p2); //from man qsort
}