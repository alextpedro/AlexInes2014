/*@author Alexandra Pedro 2131314@my.ipleiria.pt && 
 * Inês Faria 2110922@my.ipleiria.pt */
#ifndef ETAPA1_H
#define ETAPA1_H

int is_header_PALZ (const char *header_first_row);
int is_valid_size(const char *size_str, unsigned int *value);
void decompress (char *filename);

#endif