/**
* @file main.c
* @brief palz – Compressor de ficheiros de texto
* @date 10-2014
* @author Alexandra Pedro 2131314@my.ipleiria.pt && Inês Faria 2110922@my.ipleiria.pt
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <time.h>

#include "debug.h"
#include "memory.h"
#include "etapa1.h"
#include "config.h"


int main(int argc, char *argv[])
{
	//Gengetopt arguments
	struct gengetopt_args_info args; 
	cmdline_parser(argc,argv,&args);
	struct timespec init_op, end_op;

	/* Main code */
	//Temporary validation code
	if (argc > 5) {
		printf("Invalid number of arguments. Try: ./palz --help \n");
		exit(1);
	}

	if (argc < 3) {
		printf("Invalid number of arguments. Try: ./palz --help \n");
		exit(1);
	}

	if (clock_gettime(CLOCK_REALTIME, &init_op) == -1)
	{
		printf("clock_gettime() failed.\n");
		exit(1);
	}

	if (args.decompress_arg)
	{
		decompress(args.decompress_arg);
	}

	if (args.folder_decompress_arg)
	{
		printf("%s\n", args.folder_decompress_arg);
		//funcao a fazer
	}
	
	if(args.compress_given)
	{
		printf("[TODO] option not implemented yet. \n");
		exit(1);
	}
	
	if (args.parallel_folder_compress_given)
	{
		printf("[TODO] option not implemented yet. \n");
		exit(1);
	}

	if (args.parallel_folder_decompress_given)
	{
		printf("[TODO] option not implemented yet. \n");
		exit(1);
	}

	if (args.about_given)
	{
		printf("PALZ Author\n");
		printf("Alexandra Pedro Nº2131314\n");
		printf("Ines Faria Nº2110922\n");
		exit(1);
	}

	if (clock_gettime(CLOCK_REALTIME, &end_op) == -1)
	{
		printf("clock_gettime() failed.\n");
		exit(1);
	}

	/*******************************
		Gives total operation time.
	********************************/
	double total_op_time;
	total_op_time = (end_op.tv_sec - init_op.tv_sec) + (end_op.tv_nsec - init_op.tv_nsec)/1000000000.0;
	printf("Execution time:%lf\n", total_op_time);

	//End of Code
	cmdline_parser_free(&args);
	return 0;
}

