#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
	srand((unsigned)time(NULL));
	long int argin;
	int i;  
	char cin;
	argin = atoi(argv[1]);
	for(i = 0; i < argin; i++){
		cin = (char) ((rand() % 27) + 64);
		if(cin == 64){
			cin = 32;
	}
	printf("%c", cin);

	}
	printf("\n");

return 0;
}
