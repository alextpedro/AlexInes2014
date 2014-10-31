/*@author Alexandra Pedro 2131314@my.ipleiria.pt && 
 * Inês Faria 2110922@my.ipleiria.pt */
#ifndef COMMON_H
#define COMMON_H
#define ERR_SIG_INSTALL 1


	extern int running;


	void trata_sinal (int signal);
	void install_signal_handler (void);
	void catch_signal();
	long compression_ratio(long sizeOfCompressed, long sizeOfDecompressed);
#endif