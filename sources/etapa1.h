/*@author Alexandra Pedro 2131314@my.ipleiria.pt && 
 * Inês Faria 2110922@my.ipleiria.pt */
#ifndef ETAPA1_H
#define ETAPA1_H

int is_header_PALZ (const char *header_first_row);
int is_valid_size(const char *size_str, unsigned int *value, char *filename);
void decompress (char *filename, int function);
int write_to_file (char**, char *filename, FILE*, unsigned int numberOfWords, int function);
int bytes_for_int(unsigned int max_value);
int folderDecompress(const char *dirname);
int is_extension_palz (const char *fullfilename);


#endif
