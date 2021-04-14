#include<stdio.h>

int main (int argc, char *argv[]) {
	int i=0, error=0;

	if (argc==1) {
		fprintf(stderr,"Usar %s args\n",argv[0]);
		error=1;
	}
	else {
		for (i=1;i<argc;i++) {
			fprintf(stdout,"%s\n",argv[i]);
		}
	}

	return error;
}

