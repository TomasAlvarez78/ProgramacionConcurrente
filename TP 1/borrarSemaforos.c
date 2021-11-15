#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>

int main(int argc, char *v[]){

	sem_unlink("/semBinario");
	sem_unlink("/semBinario2");
	shm_unlink("/memCompartida");

	printf("Semaforos y Memoria borrados");
	return 0;
}