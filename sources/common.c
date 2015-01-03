/*@author Alexandra Pedro 2131314@my.ipleiria.pt && 
 * Inês Faria 2110922@my.ipleiria.pt */
#include <signal.h>
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


#include "listas.h"
#include "hashtables.h"
#include "common.h"
#include "debug.h"
#include "memory.h"

int running = 1;

static void signal_handler(int);

void install_signal_handler(void) {
	struct sigaction config;

	catch_signal();
	config.sa_handler = signal_handler;
	sigemptyset(&config.sa_mask);
	config.sa_flags = SA_RESTART;

	if (sigaction(SIGINT, &config, NULL) != 0) {
		printf("sigaction() failed");
	}
}

void signal_handler(int signal) {
	(void) signal;
	running = 0;
	
}

void catch_signal () {
	if (running == 0) {
		printf("Operation interrupted by user @<todo date and time>\n");
	}
}

float compression_ratio(float sizeOfCompressed, float sizeOfDecompressed) {
	float ratio = 0;

	if (sizeOfDecompressed == 0) {
		return ratio;
	}

	ratio = ((1 - (sizeOfCompressed / sizeOfDecompressed) ) * 100);
	return ratio;
}

int is_extension_palz (const char *fullfilename) {
	char *fileExt = strrchr(fullfilename, '.');
	return fileExt!= NULL && strcasecmp(fileExt, ".palz") == 0;		
}

int is_not_extension_palz (const char *fullfilename) {
	return !is_extension_palz(fullfilename);		
}

void thread_main(char *filename, int numT, void (*function)(char *, int), int(*extension)(char*)){
	
	int nthreads = numT;
	PARAM_T param;
	param.max = nthreads*2;
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
	    param.function = function;
	    param.total = 0;
	    param.index_escrita = 0;
	    param.index_leitura = 0;

	    

	for(i = 0; i < nthreads; i++) 

	    // Cria thread para executar o consumidor 
		if ((errno = pthread_create(&tids[i], NULL, consumidor, &param)) != 0)
			ERROR(C_ERRO_PTHREAD_CREATE, "pthread_create() failed!");
	
		//executar o folderDecompress 
		parallel_folder(filename, &param, extension);
		//printf("Chegou aqui.\n");

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

void parallel_folder (const char *dirname, PARAM_T *p, int(*extension)(char*)) {
	
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
            return;

        	}

	        if( S_ISDIR(info.st_mode) ) {

	            lista_inserir_fim(listOfDir, strdup(fullname));

				printf("--Folder!--%s--\n",fullname);

	        } else if(S_ISREG(info.st_mode)) {
	            	
	            printf("||%s\n", fullname);

	            if (extension(fullname))
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

        // Sai da seccao critica 	
		if ((errno = pthread_mutex_unlock(&(p->mutex))) != 0) {
			WARNING("pthread_mutex_unlock() failed");
			return;
		}
	
}

void *produtor(PARAM_T *p, char * fullname )
{
    if ((errno = pthread_mutex_lock(&(p->mutex))) != 0) {
		WARNING("pthread_mutex_lock() failed\n");
		return NULL;
	}

        // Espera que o buffer tenha espaco disponivel 
        while (p->total == p->max)
			if ((errno = pthread_cond_wait(&(p->cond), &(p->mutex))) != 0) {
				WARNING("pthread_cond_wait() failed");
				return NULL;
			}

        // Coloca um valor no buffer 
        p->buffer[p->index_escrita] = strdup(fullname);
        printf("coloca valor buffer>> %s\n", p->buffer[p->index_escrita]);
        p->index_escrita = (p->index_escrita + 1) % p->max;
        p->total++;

        // Notifica consumidores 'a espera 
        if (p->total == 1)
			if ((errno = pthread_cond_broadcast(&(p->cond))) != 0) {
				WARNING("pthread_cond_broadcast() failed");
				return NULL;
			}

        // Sai da seccao critica 
		if ((errno = pthread_mutex_unlock(&(p->mutex))) != 0) {
			WARNING("pthread_mutex_unlock() failed");
			return NULL;
		}

    return NULL;
}

void *consumidor(void *arg) 
{
    PARAM_T *p = (PARAM_T *) arg; 

    int value = 1;

    while(1) {

		if ((errno = pthread_mutex_lock(&(p->mutex))) != 0) {
			WARNING("pthread_mutex_lock() failed\n");
			return NULL;
		}

        // Espera que o buffer tenha dados 
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

        // Retira um valor no buffer
		char *path = p->buffer[p->index_leitura];
        p->index_leitura = (p->index_leitura + 1) % p->max;
        p->total--;

        // Notifica produtores 'a espera 
        if (p->total == (p->max)-1)
			if ((errno = pthread_cond_signal(&(p->cond))) != 0) {
				WARNING("pthread_cond_signal() failed");
				return NULL;
			}

        // Sai da seccao critica 
		if ((errno = pthread_mutex_unlock(&(p->mutex))) != 0) {
			WARNING("pthread_mutex_unlock() failed");
			return NULL;
		}
        printf("consumidor << %s\n", path);

        p->function(path,value);
    }
    return NULL;
}

