#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>

#include "MonitorBuffer.h"

struct Monitor *CrearMonitor()
{
	int error = 0;
	struct Monitor *aux = NULL;

	aux = (struct Monitor *)(calloc(1, sizeof(struct Monitor)));

	if (aux != NULL)
	{
		aux->inicio = 0;
		aux->fin = 0;
		aux->cont = 0;
		error += pthread_cond_init(&aux->cond, NULL);
		if (error)
			perror("pthread_cond_init()");

		error += pthread_mutex_init(&aux->leer, NULL);
		if (error)
			perror("pthread_mutex_init()");

		error += pthread_mutex_init(&aux->escribir, NULL);
		if (error)
			perror("pthread_mutex_init()");

		pthread_cond_broadcast(&aux->cond);
		pthread_mutex_lock(&aux->leer);
		pthread_mutex_unlock(&aux->escribir);
	}
	return aux;
}

int GuardarDato(struct Monitor *m, int dato)
{
	int error = 0;

	error = pthread_mutex_lock(&m->escribir);
	if (error)
		perror("pthread_mutex_lock()");
	else
		//ver si usamos cont como manejador de cantidad de pedidos
		while (m->cont > 9)
			error = pthread_cond_wait(&m->cond, &m->escribir);

	if (error)
		perror("pthread_cond_wait()");
	else
	{
		m->dato[m->fin] = dato;
		m->fin = ++m->fin % 10;
		++m->cont;
		pthread_cond_signal(&m->cond);
	}

	if (error)
		perror("pthread_cond_signal()");
	else
		error = pthread_mutex_unlock(&m->leer);

	if (error)
		perror("pthread_mutex_unlock()");

	return error;
}

int LeerDato(struct Monitor *m, int *dato)
{
	int error = 0;

	error = pthread_mutex_lock(&m->leer);
	if (error)
		perror("pthread_mutex_lock()");
	else
		while (m->cont < 0)
			//Si cont < 1 significa que no hay elementos a leer
			error = pthread_cond_wait(&m->cond, &m->leer);

	if (error)
		perror("pthread_cond_wait()");
	else
	{

		*dato = m->dato[m->inicio];
		m->inicio = ++m->inicio % 10;
		--m->cont;
		pthread_cond_signal(&m->cond);
	}

	if (error)
		perror("pthread_cond_signal()");
	else
		error = pthread_mutex_unlock(&m->escribir);

	if (error)
		perror("pthread_mutex_unlock()");

	return error;
}

void BorrarMonitor(struct Monitor *m)
{
	if (m != NULL)
	{
		pthread_cond_destroy(&m->cond);
		pthread_mutex_destroy(&m->leer);
		pthread_mutex_destroy(&m->escribir);
		free(m);
	}
}