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

#define C_ERRO_PTHREAD_CREATE           1
#define C_ERRO_PTHREAD_JOIN             2
#define C_ERRO_MUTEX_INIT               3
#define C_ERRO_MUTEX_DESTROY            4
#define C_ERRO_CONDITION_INIT           5
#define C_ERRO_CONDITION_DESTROY        6

static int compare (const void *, const void *);


typedef struct 
{
    char **buffer;
    int index_leitura;
    int index_escrita;
    int total;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int max;
    int stop;
} PARAM_T;

void *produtor(PARAM_T *p, char * fullname);
void *consumidor(void *arg);

int folderDecompressThreads (const char *dirname, PARAM_T *p);

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
		char *newFileName = NULL;
		newFileName = get_palz_file_name(filename);

		printf("%s\n", newFileName);

/*----------nao esta a funcionar----------*/

		//write to a new file
		FILE *outFile = NULL;
		rewind(tmp);
		outFile = fopen("test_1.txt.palz", "w");//nao deixa utilizar o newFileName, da segmentation fault

		//write num elements
		  //nelems
		
		//Copy from the temporary file to the permanent file
		char buffer[8096];
		int n;
		while( (n=fread(buffer, 1, 8096, tmp)) > 0) {
		 	fwrite(buffer, 1, n, outFile);
		}
		fflush(outFile);
		fclose(outFile);
			
/*-----------------------------------------*/

		fclose(tmp);
		fclose(text);
		free(line);
		free (arrayWords);
		tabela_destruir(&words);
}

static int compare (const void *p1, const void *p2) {
	return strcmp(* (char * const *) p1, * (char * const *) p2); //from man qsort
}

/**
 * @brief Creates a filename with added .palz extension.
 * @details Recives a filename and returns a new filename with the added .palz extension.
 * 
 * @param filename The original filename.
 * @return Original filename with added .palz extension.
 */
char* get_palz_file_name(char* filename) {
	char* workFileName = NULL;

	//Add .palz extension
	char buffer[256];
	snprintf(buffer, sizeof buffer, "%s%s", filename, ".palz");

	workFileName = buffer;

	return workFileName;
}

void parallel_folder_decompress(char *filename, int numT){
	
	int nthreads = numT;
	PARAM_T param;
	param.max = nthreads;
	param.buffer = malloc(sizeof(char *) * param.max);
	pthread_t tids[nthreads];
	int i;
	param.stop = 0;

	  // Inicia o mutex 
		if ((errno = pthread_mutex_init(&param.mutex, NULL)) != 0)
			ERROR(C_ERRO_MUTEX_INIT, "pthread_mutex_init() failed!");

	    // Inicia variavel de condicao 
		if ((errno = pthread_cond_init(&param.cond, NULL)) != 0)
			ERROR(C_ERRO_CONDITION_INIT, "pthread_cond_init() failed!");

	    // Inicia os restantes parametros a passar 'as threads 
	    param.total = 0;
	    param.index_escrita = 0;
	    param.index_leitura = 0;

	    

	for(i = 0; i < nthreads; i++) 

	    // Cria thread para executar o consumidor 
		if ((errno = pthread_create(&tids[i], NULL, consumidor, &param)) != 0)
			ERROR(C_ERRO_PTHREAD_CREATE, "pthread_create() failed!");
		

        //cenas a fazer
	
		//executar o folderDecompress 
		folderDecompressThreads(filename, &param);

		/* Espera que todas as threads terminem */
		for(i = 0; i < nthreads; i++) {
			if ((errno = pthread_join(tids[i], NULL)) != 0)
				ERROR(C_ERRO_PTHREAD_JOIN, "pthread_join() failed!");	
		}
			/* Destroi o mutex */
			if ((errno = pthread_mutex_destroy(&param.mutex)) != 0)
				ERROR(C_ERRO_MUTEX_DESTROY, "pthread_mutex_destroy() failed!");
			
			/* Destroi a condicao */
			if ((errno = pthread_cond_destroy(&param.cond)) != 0)
				ERROR(C_ERRO_CONDITION_DESTROY, "pthread_cond_destroy() failed!");
	

}

void parallel_folder_compress(char *filename, int numT){
	printf("-------------------------------:\n");
	printf("%s\n", filename);
	printf("%d\n", numT);
	printf("-------------------------------:\n");

}

int folderDecompressThreads (const char *dirname, PARAM_T *p) {
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

	            lista_inserir_fim(listOfDir, strdup(fullname));

				printf("--Folder!--\n");
				
	            //folderDecompress(fullname);

	        } else {

				/*if (is_extension_palz(fullname) == -1)
				{
					FREE(fullname);
					continue;
	               
				}else{
					printf("Decompress file %s\n", fullname);
	            	decompress(fullname, function);
				}*/
	            	
	            printf("%s\n", fullname);
				produtor(p, fullname);
		        
        	}

        	FREE(fullname);
		}
		closedir(workdir);
		FREE(fdirname);
	}

	lista_destruir(&listOfDir);

	if ((errno = pthread_mutex_lock(&(p->mutex))) != 0) {
		WARNING("pthread_mutex_lock() failed\n");
		return;
	}

	p->stop = 1;

			if ((errno = pthread_cond_broadcast(&(p->cond))) != 0) {
				WARNING("pthread_cond_broadcast() failed");
				return;
			}

        /* Sai da seccao critica */	
		if ((errno = pthread_mutex_unlock(&(p->mutex))) != 0) {
			WARNING("pthread_mutex_unlock() failed");
			return;
		}
	
}

void *produtor(PARAM_T *p, char * fullname )
{
    //PARAM_T *p = (PARAM_T *) arg; 

    if ((errno = pthread_mutex_lock(&(p->mutex))) != 0) {
		WARNING("pthread_mutex_lock() failed\n");
		return NULL;
	}

        /* Espera que o buffer tenha espaco disponivel */
        while (p->total == p->max)
			if ((errno = pthread_cond_wait(&(p->cond), &(p->mutex))) != 0) {
				WARNING("pthread_cond_wait() failed");
				return NULL;
			}

        /* Coloca um valor no buffer */
        p->buffer[p->index_escrita] = strdup(fullname);
        printf(">> %s\n", p->buffer[p->index_escrita]);
        p->index_escrita = (p->index_escrita + 1) % p->max;
        p->total++;

        /* Notifica consumidores 'a espera */
        if (p->total == 1)
			if ((errno = pthread_cond_broadcast(&(p->cond))) != 0) {
				WARNING("pthread_cond_broadcast() failed");
				return NULL;
			}

        /* Sai da seccao critica */	
		if ((errno = pthread_mutex_unlock(&(p->mutex))) != 0) {
			WARNING("pthread_mutex_unlock() failed");
			return NULL;
		}

    return NULL;
}

void *consumidor(void *arg) 
{
    PARAM_T *p = (PARAM_T *) arg; 

    while(1) {

		if ((errno = pthread_mutex_lock(&(p->mutex))) != 0) {
			WARNING("pthread_mutex_lock() failed\n");
			return NULL;
		}

        /* Espera que o buffer tenha dados */
        while (p->total == 0 && !p->stop)
			if ((errno = pthread_cond_wait(&(p->cond), &(p->mutex))) != 0) {
				WARNING("pthread_cond_wait() failed");
				return NULL;
			}

		if(p->total == 0) {
			if ((errno = pthread_mutex_unlock(&(p->mutex))) != 0) {
				WARNING("pthread_mutex_unlock() failed");
				return NULL;
			}
			break;
		}			

        /* Retira um valor no buffer */
		char *path = p->buffer[p->index_leitura];
        p->index_leitura = (p->index_leitura + 1) % p->max;
        p->total--;

        /* Notifica produtores 'a espera */
        if (p->total == (p->max)-1)
			if ((errno = pthread_cond_signal(&(p->cond))) != 0) {
				WARNING("pthread_cond_signal() failed");
				return NULL;
			}

        /* Sai da seccao critica */	
		if ((errno = pthread_mutex_unlock(&(p->mutex))) != 0) {
			WARNING("pthread_mutex_unlock() failed");
			return NULL;
		}
        printf("<< %s\n", path);

        /* Adormece entre 0 a 4 segundos */
        //sleep(random() % 5);
    }
    return NULL;
}