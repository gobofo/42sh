#include "input.h"

int main(int argc, char* argv[]){

	FILE *f = getInputFile(argc, argv);

	if(!f)
		return -1;

	char c;
	while((c = fgetc(f))!=EOF){
		putchar(c);
	}
	return 0;
}
