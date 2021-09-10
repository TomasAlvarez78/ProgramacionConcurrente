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

#define PEDIDOS 10
#define HILOS 7

struct boundedBuffer_t
{
	int inicio, fin;
	int buf[PEDIDOS];
	sem_t *lleno, *vacio, *leyendo, *escribiendo, *cocinar;
};

void *telefono(void *arg);
void *encargado(void *arg);
void *cocinero(void *arg);
void *delivery(void *arg);
void *prueba(void *b);

int CrearSemaforos(struct boundedBuffer_t *bb);
int BorrarSemaforos(struct boundedBuffer_t *bb);

int main(int argc, char *v[])
{
	pthread_t *th;
	struct boundedBuffer_t *bb;
	bb = (struct boundedBuffer_t *)(calloc(1, sizeof(struct boundedBuffer_t)));
	CrearSemaforos(&bb);

	th = (pthread_t *)(calloc(HILOS, sizeof(pthread_t)));

	pthread_create(&th[0], NULL, telefono, &bb);
	pthread_create(&th[1], NULL, encargado, &bb);
	for (int i = 2; i < 5; i++)
	{
		pthread_create(&th[i], NULL, cocinero, &bb);
	}

	for (int i = 0; i < HILOS; i++)
	{
		pthread_join(th[i], NULL);
	}

	BorrarSemaforos(&bb);
	free(th);
	free(bb);
}

void *prueba(void *b)
{
	struct boundedBuffer_t *bb = (struct boundedBuffer_t *)(b);
	int id = 0;
	printf("Hola %d\n", id);
	pthread_exit(NULL);
}

void *telefono(void *arg)
{
	struct boundedBuffer_t *bb = (struct boundedBuffer_t *)(arg);
	printf("Inicio %d\n",bb->inicio);
	printf("Fin %d\n",bb->fin);
	int error = 0, i = 0, *x;
	x = calloc(1,sizeof(int));
	while (!error && i < 1)
	{
		printf("vacio\n");
		sem_getvalue(bb->vacio,x);
		printf("Valor vacio %d",x);
		
		error = sem_wait(bb->vacio);
		printf("TELEFONO\n");
		if (!error)
		{
			printf("1\n");
			error = sem_wait(bb->escribiendo);
		}
		if (!error)
		{
			printf("2\n");
			bb->buf[bb->fin] = (rand() % 5) + 1;
			bb->fin = ++bb->fin % PEDIDOS;
			i++;
			error = sem_post(bb->escribiendo);
		}
		if (!error)
		{
			printf("3\n");
			error = sem_post(bb->lleno);
			usleep(rand() % 200000000);
		}
	}
	pthread_exit(NULL);
}
void *encargado(void *arg)
{
	struct boundedBuffer_t *bb = (struct boundedBuffer_t *)(arg);
	int error = 0,temporal;
	while (!error )
	{
		error = sem_wait(bb->lleno);
		printf("ENCARGADO \n");
		if (!error)
		{
			error = sem_wait(bb->leyendo);
		}
		if (!error)
		{
			temporal = bb->buf[bb->inicio];
			if(temporal > 0){
				printf("Encargado -- cocinar --> %d\n",temporal);
			}
			bb->inicio = ++bb->inicio % PEDIDOS;
			error = sem_post(bb->leyendo);
		}
		if(!error){
			error = sem_post(bb->vacio);
		}
		if (error) {
			perror("sem_post()");
		}
	}
	pthread_exit(NULL);
}
void *cocinero(void *arg)
{
	struct boundedBuffer_t *bb = (struct boundedBuffer_t *)(arg);
	int error = 0;
	while (!error)
	{
		error = sem_wait(bb->cocinar);
		printf("COCINERO \n");
		if(!error){
			printf("Cocinero -- cocinanando --> ");
		}
	}
	pthread_exit(NULL);
}
void *delivery(void *arg)
{
	struct boundedBuffer_t *bb = (struct boundedBuffer_t *)(arg);
	int error = 0;
	while (!error)
	{

	}
	pthread_exit(NULL);
}

int CrearSemaforos(struct boundedBuffer_t *bb)
{
	int error = 0;
	
	bb->inicio = 0;
	bb->fin = 0;

	bb->cocinar = sem_open("/cocinar", O_CREAT, 0640, 0);
	if (bb->cocinar != SEM_FAILED)
	{
		printf("Semaforo [cocinar] creado!\n");
	}
	else
	{
		perror("sem_open()");
		error -= 1;
	}

	bb->lleno = sem_open("/lleno", O_CREAT, 0640, 0);
	if (bb->lleno != SEM_FAILED)
	{
		printf("Semaforo [lleno] creado!\n");
	}
	else
	{
		perror("sem_open()");
		error -= 1;
	}

	bb->vacio = sem_open("/vacio", O_CREAT, 0640, PEDIDOS);
	if (bb->vacio != SEM_FAILED)
	{
		printf("Semaforo [vacio] creado!\n");
	}
	else
	{
		perror("sem_open()");
		error -= 1;
	}

	bb->leyendo = sem_open("/leyendo", O_CREAT, 6040, 1);
	if (bb->leyendo != SEM_FAILED)
	{
		printf("Semaforo [leyendo] creado!\n");
	}
	else
	{
		perror("sem_open()");
		error -= 1;
	}

	bb->escribiendo = sem_open("/escribiendo", O_CREAT, 0640, 1);
	if (bb->escribiendo != SEM_FAILED)
	{
		printf("Semaforo [escribiendo] creado!\n");
	}
	else
	{
		perror("sem_open()");
		error -= 1;
	}

	return error;
}

int BorrarSemaforos(struct boundedBuffer_t *bb)
{
	int error = 0, status = 0;

	status = sem_close(bb->cocinar);
	if (!status)
	{
		status = sem_unlink("/cocinar");
		if (!status)
			printf("Semaforo [cocinar] borrado!\n");
		else
		{
			perror("sem_unlink()");
			error -= 1;
		}
	}
	else
	{
		perror("sem_close()");
		error -= 1;
	}

	status = sem_close(bb->lleno);
	if (!status)
	{
		status = sem_unlink("/lleno");
		if (!status)
			printf("Semaforo [lleno] borrado!\n");
		else
		{
			perror("sem_unlink()");
			error -= 1;
		}
	}
	else
	{
		perror("sem_close()");
		error -= 1;
	}

	status = sem_close(bb->vacio);
	if (!status)
	{
		status = sem_unlink("/vacio");
		if (!status)
			printf("Semaforo [vacio] borrado!\n");
		else
		{
			perror("sem_unlink()");
			error -= 1;
		}
	}
	else
	{
		perror("sem_close()");
		error -= 1;
	}

	status = sem_close(bb->escribiendo);
	if (!status)
	{
		status = sem_unlink("/escribiendo");
		if (!status)
			printf("Semaforo [escribiendo] borrado!\n");
		else
		{
			perror("sem_unlink()");
			error -= 1;
		}
	}
	else
	{
		perror("sem_close()");
		error -= 1;
	}

	status = sem_close(bb->leyendo);
	if (!status)
	{
		status = sem_unlink("/leyendo");
		if (!status)
			printf("Semaforo [leyendo] borrado!\n");
		else
		{
			perror("sem_unlink()");
			error -= 1;
		}
	}
	else
	{
		perror("sem_close()");
		error -= 1;
	}

	return error;
}
