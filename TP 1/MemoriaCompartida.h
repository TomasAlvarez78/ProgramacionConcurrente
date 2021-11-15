#ifndef _MEMORIA_H_
#define _MEMORIA_H_

#include <semaphore.h>

struct Memoria{
	int memoria;
	sem_t *binario;
};

struct Memoria *CrearMemoria();
int GuardarMemoria(struct Memoria *m,int dato);
int LeerMemoria(struct Memoria *m,int *dato);
void BorrarMemoria(struct Memoria *m);

#endif