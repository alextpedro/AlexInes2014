/*@author Alexandra Pedro 2131314@my.ipleiria.pt && 
 * Inês Faria 2110922@my.ipleiria.pt */
#ifndef COMMON_H
#define COMMON_H
	extern int running;

	void trata_sinal (int signal);
	float compression_ratio(int sizeOfCompressed, int sizeOfDecompressed);
#endif