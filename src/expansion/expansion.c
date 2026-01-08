#define _POSIX_C_SOURCE 200809L

#include "expansion.h"

int expand(char **value){
	char *p = *value;

	char *buff;
	size_t size;

	FILE *stream = open_memstream(&buff, &size);

	while(*p){
		if(*p != '\''){
			fputc(*p, stream);
		}
		p++;
	}
	
	free(*value);
	fclose(stream);
	*value = buff;
	return 0;

error:
	fclose(stream);
	free(buff);
	return 1;
}
