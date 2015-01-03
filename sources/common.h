/*@author Alexandra Pedro 2131314@my.ipleiria.pt && 
 * Inês Faria 2110922@my.ipleiria.pt */
#ifndef COMMON_H
#define COMMON_H
#define ERR_SIG_INSTALL 1

#include "etapa2.h"
#include <pthread.h>

#define C_ERRO_PTHREAD_CREATE           1
#define C_ERRO_PTHREAD_JOIN             2
#define C_ERRO_MUTEX_INIT               3
#define C_ERRO_MUTEX_DESTROY            4
#define C_ERRO_CONDITION_INIT           5
#define C_ERRO_CONDITION_DESTROY        6


/*Structures*/
typedef struct //estrutura a explicar no rel
{
	void (*function)(char *, int); //distint comp ou decomp
    char **buffer;					//memoria partilhada pelas threads
    int index_leitura;				//ler os dados do buffer
    int index_escrita;				//escrever dados no buffer
    int total;						//total das threads
    pthread_mutex_t mutex;			//informar que apenas um cons ou um prod podem trabalhar ao mesmo tempo
    pthread_cond_t cond;			//sincronizar as threads
    int max;						//max threads
    int stop;						//condicao de paragem
} PARAM_T;

	extern int running;


	void trata_sinal (int signal);
	void install_signal_handler (void);
	float compression_ratio(float sizeOfCompressed, float sizeOfDecompressed);
	void catch_signal (void);
	int is_extension_palz (const char *fullfilename);
	void thread_main(char*, int, void (*function)(char *, int), int(*extension)(char*));
	void parallel_folder (const char *dirname, PARAM_T *p, int(*extension)(char*));
	void *produtor(PARAM_T *p, char * fullname );
	void *consumidor(void *arg);
	int is_not_extension_palz (const char *fullfilename);

#endif