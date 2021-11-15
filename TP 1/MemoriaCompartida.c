

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/mman.h>

#include "MemoriaCompartida.h"

struct Memoria *CrearMemoria(){
	int error=0;
	struct Memoria *aux = NULL;
	aux = (struct Memoria *)(calloc(1, sizeof(struct Memoria)));
	aux->memoria = 0;
	aux->binario = sem_open("/semBinario", O_CREAT, O_RDWR, 0);
	if (aux->binario == SEM_FAILED){
		perror("sem_open()");
		error = -1;
	}
	if (!error){
		// printf("Semaforo creado!\n");

		aux->memoria = shm_open("/memCompartida", O_CREAT | O_RDWR, 0660);
		if (aux->memoria < 0){
			perror("shm_open()");
			error = -1;
		}
	}
	if (!error){
		// printf("Descriptor de memoria creado!\n");
		error = ftruncate(aux->memoria, sizeof(int));
		if (error)
			perror("ftruncate()");
	}
	return aux;
}

int GuardarMemoria(struct Memoria *m, int dato){
	int error=0;
	int *datos = NULL;

	//datos = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, m->memoria, 0);
	datos = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, m->memoria, 0);
	if (datos == MAP_FAILED){
		perror("mmap()");
		error = -1;
  	}
	if (!error){
		error = sem_wait(m->binario);
		if (error){
			perror("sem_wait()");
		}
		if (!error){
			*datos = dato;
			usleep(random() % 5000000);
			error = sem_post(m->binario);
			if (error){
				perror("sem_post()");
			}
		}
	}
	if (datos != NULL){
		error = munmap((void *)(datos), 2 * sizeof(int));
		if (error)
		{
			perror("munmap()");
		}
	}
	return error;
}


int LeerMemoria(struct Memoria *m, int *dato){
	int error= 0;
	int *mapeo = NULL;

	mapeo = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, m->memoria, 0);

	if (mapeo == MAP_FAILED){
		perror("mmap()");
		error = -1;
	}

	if (!error){
		error = sem_post(m->binario);
		if (error)
			perror("sem_post()");
	}

	fflush(NULL);
	*dato = *mapeo;
	*mapeo = 0;

	if (mapeo != NULL){
		error = munmap((void *)(mapeo), 2 * sizeof(int));
		if (error){
			perror("munmap()");
		}
	}

	return error;
}

void BorrarMemoria(struct Memoria *m){
	int error=0;
	if(m != NULL){

		if (!error){
			error = sem_close(m->binario);
			if (error){
				perror("sem_close()");
			}
			else{
				// printf("Semaforo cerrado\n");
			}
		}

		if (!error){
			error = sem_unlink("/semBinario");
			if (error)
			{
				perror("sem_unlink()");
			}
			else
			{
				// printf("Semaforo borrado!\n");
			}
		}

		if (m->memoria > 0){
			error = shm_unlink("/memCompartida");
			if (error){
				perror("unlink()");
			}
			else{
				// printf("Descriptor de memoria borrado!\n");
			}
		}
	}
}