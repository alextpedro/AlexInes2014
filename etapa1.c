#include <stdio.h>
#include <memory.h>
#include <errno.h>
#include <stdlib.h>
#include <limits.h>

#include "etapa1.h"

int is_header_PALZ (const char *header_first_row) {
	return (strcmp(header_first_row, "PALZ\n") == 0);		
}

int is_valid_size(const char *str, unsigned int *value) {
	char *endptr;

	errno=0;
	long long number = strtoll(str, &endptr, 10);
	if (*endptr != '\0' && *endptr != '\n')
	{
		return 0;
	}

	if (errno != 0)
	{
		return 0;
	}

	if (number < 0 || number > UINT_MAX)
	{
		return 0;
	}

	*value = number;
	return 1;
}