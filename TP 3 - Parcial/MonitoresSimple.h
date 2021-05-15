#ifndef _MONITORSIMPLE_H_
#define _MONITORSIMPLE_H_

#define PEDIDOS 10

struct Monitor_t {
  int cont;
  int dato[PEDIDOS];
  int ready;
  pthread_cond_t  cond;
  pthread_mutex_t escribir, leer;
};

struct Monitor_t* CrearMonitor  ();
int        GuardarDato   (struct Monitor_t *m, int dato);
int        LeerDato      (struct Monitor_t *m, int *dato);
void       BorrarMonitor (struct Monitor_t *m);

#endif
