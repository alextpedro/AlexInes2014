/*@author Alexandra Pedro 2131314@my.ipleiria.pt && 
 * Inês Faria 2110922@my.ipleiria.pt */
#ifndef ETAPA2_H
#define ETAPA2_H

#include "common.h"

void compress(char*, int);
char* get_palz_file_name(char*);
int bytesForInt(unsigned int);
int isSeparator(char);
int separatorIndex(char);
void writeRepetition(int, int,FILE*);


#endif