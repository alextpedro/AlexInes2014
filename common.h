/*@author Alexandra Pedro 2131314@my.ipleiria.pt && 
 * Inês Faria 2110922@my.ipleiria.pt */
#ifndef COMMON_H
#define COMMON_H
#define ERR_SIG_INSTALL 1


	extern int running;


	void trata_sinal (int signal);
	void install_signal_handler (void);
	float compression_ratio(float sizeOfCompressed, float sizeOfDecompressed);
	void catch_signal (void);

#endif