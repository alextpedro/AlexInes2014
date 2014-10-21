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
	if (argc > 3) {
		printf("Invalid number of arguments. Try: ./palz --help \n");
		exit(1);
	}
	
	if (args.decompress_arg)
	{
		decompress(args.decompress_arg);
	}
	
	if(args.compress_arg)
	{
		printf("[TODO] option not implemented yet. \n");
		exit(1);
	}
	
	if (args.parallel_folder_compress_arg)
	{
		printf("[TODO] option not implemented yet. \n");
		exit(1);
	}

	if (args.parallel_folder_decompress_arg)
	{
		printf("[TODO] option not implemented yet. \n");
		exit(1);
	}


	//End of Code
	cmdline_parser_free(&args);
	return 0;
}

