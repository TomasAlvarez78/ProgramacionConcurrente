#ifndef _MONITORSIMPLE_H_
#define _MONITORSIMPLE_H_

struct Monitor
{
	int cont;
	int dato[10];
	int inicio, fin;
	pthread_cond_t cond;
	pthread_mutex_t escribir, leer;
};

struct Monitor *CrearMonitor();
int GuardarDato(struct Monitor *m, int dato);
int LeerDato(struct Monitor *m, int *dato);
void BorrarMonitor(struct Monitor *m);

#endif
