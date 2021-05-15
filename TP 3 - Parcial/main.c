#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>

#include "MonitoresSimple.h"

#define PEDIDOS 10
#define HILOS 7

int main(){
	int error=0,numero=0;
	pthread_t hilos[7];
	struct Monitor_t
	
}