#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>

#define ELEMENTOS 6
#define HIJOS     4

struct boundedBuffer_t {
  int inicio, fin;
  int buf[ELEMENTOS];
  sem_t *lleno, *vacio, *leyendo, *escribiendo;
};

int CrearSemaforos  (struct boundedBuffer_t *bb);
int BorrarSemaforos (struct boundedBuffer_t *bb);

void *ProcesoHijo   (void *bb);
void  ProcesoPadre  (struct boundedBuffer_t *bb);

int main (int argc, char *argv[]) {
  int error=0, i=0, status=0, terminacion=0;
  struct boundedBuffer_t boundedBuffer;

  pthread_t hijos[HIJOS];

  boundedBuffer.inicio = 0;
  boundedBuffer.fin    = 0;

  error = CrearSemaforos(&boundedBuffer);

  if (!error) {
    for (i = 0; i < HIJOS; i++) {
      status = pthread_create(&hijos[i], NULL, ProcesoHijo, (void*)(&boundedBuffer));
      if (status) {
	perror("pthread_create()");
	error += status;
      }
    }
  }

  if (!error) {
    ProcesoPadre(&boundedBuffer);
    printf("Estado final: inicio %d - final %d\n", boundedBuffer.inicio, boundedBuffer.fin);

    for (i = 0; i < HIJOS; i++) {
      status = pthread_join(hijos[i], (void **)(&terminacion));
      if (status) {
	      perror("pthread_join()");
	      error += status;
      }
    }
  }

  error += BorrarSemaforos(&boundedBuffer);

  if (error)
    fprintf(stderr,"Se produjeron %d errores\n", -error);

  return error;
}


void *ProcesoHijo (void *b) {
  int error=0, i=0;
  struct boundedBuffer_t *bb = (struct boundedBuffer_t *)(b);

  srand(time(NULL) + (unsigned)(pthread_self()));

  printf("Soy Hijo nro %p\n", (void *)(pthread_self()));

  while ((!error) && (i < ELEMENTOS)) {
    error = sem_wait(bb->vacio);
    if (!error) {
      error = sem_wait(bb->escribiendo);
    }
    if (!error) {      
      bb->buf[bb->fin] = (i < ELEMENTOS-1) ? rand() % 100 : -1 ;
      printf("HIJO %p -> %02d (idx=%2d)\n", (void *)(pthread_self()), bb->buf[bb->fin], bb->fin);
      bb->fin = ++bb->fin % ELEMENTOS;
      i++;
      error = sem_post(bb->escribiendo);
    }
    if (!error) {
      error = sem_post(bb->lleno);
      usleep(rand() % 2000000);
    }
  }

  if (error) {
    perror("HIJO sem_?");
  }

  pthread_exit(NULL);
}


void  ProcesoPadre  (struct boundedBuffer_t *bb) {
  int error=0, leidos=0, terminar=HIJOS;

  printf("Proceso PADRE listo para leer datos\n");
  while (terminar > 0) {
    error = sem_wait(bb->lleno);
    if (!error) {
      error = sem_wait(bb->leyendo);
    }

    if (!error) {
      leidos = bb->buf[bb->inicio];
      if (leidos >= 0) {
	      printf("PADRE -> Dato leido -> %02d <- | DEBUG: inicio %d - final %d\n", leidos, bb->inicio, bb->fin);
      }
      else {
	      terminar--;
      }
      bb->inicio = ++bb->inicio % ELEMENTOS;
      error = sem_post(bb->leyendo);
    }
    else {
      perror("sem_wait()");
    }

    if (!error) {
      error = sem_post(bb->vacio);
    }

    if (error) {
      perror("sem_post()");
    }
  }
}


int CrearSemaforos (struct boundedBuffer_t *bb) {
  int error=0;

  bb->lleno = sem_open("/lleno", O_CREAT, 0640, 0);
  if (bb->lleno != SEM_FAILED) {
    printf("Semaforo [lleno] creado!\n");
  }
  else {
    perror("sem_open()");
    error -= 1;
  }

  bb->vacio = sem_open("/vacio", O_CREAT, 0640, ELEMENTOS);
  if (bb->lleno != SEM_FAILED) {
    printf("Semaforo [vacio] creado!\n");
  }
  else {
    perror("sem_open()");
    error -= 1;
  }

  bb->leyendo = sem_open("/leyendo", O_CREAT, 0640, 1);
  if (bb->lleno != SEM_FAILED) {
    printf("Semaforo [leyendo] creado!\n");
  }
  else {
    perror("sem_open()");
    error -= 1;
  }

  bb->escribiendo = sem_open("/escribiendo", O_CREAT, 0640, 1);
  if (bb->lleno != SEM_FAILED) {
    printf("Semaforo [escribiendo] creado!\n");
  }
  else {
    perror("sem_open()");
    error -= 1;
  }

  return error;
}


int BorrarSemaforos (struct boundedBuffer_t *bb) {
  int error=0, status=0;

  status = sem_close(bb->lleno);
  if (!status) {
    status = sem_unlink("/lleno");
    if (!status)
      printf("Semaforo [lleno] borrado!\n");
    else {
      perror("sem_unlink()");
      error -= 1;
    }
  }
  else {
    perror("sem_close()");
    error -= 1;
  }

  status = sem_close(bb->vacio);
  if (!status) {
    status = sem_unlink("/vacio");
    if (!status)
      printf("Semaforo [vacio] borrado!\n");
    else {
      perror("sem_unlink()");
      error -= 1;
    }
  }
  else {
    perror("sem_close()");
    error -= 1;
  }

  status = sem_close(bb->escribiendo);
  if (!status) {
    status = sem_unlink("/escribiendo");
    if (!status)
      printf("Semaforo [escribiendo] borrado!\n");
    else {
      perror("sem_unlink()");
      error -= 1;
    }
  }
  else {
    perror("sem_close()");
    error -= 1;
  }

  status = sem_close(bb->leyendo);
  if (!status) {
    status = sem_unlink("/leyendo");
    if (!status)
      printf("Semaforo [leyendo] borrado!\n");
    else {
      perror("sem_unlink()");
      error -= 1;
    }
  }
  else {
    perror("sem_close()");
    error -= 1;
  }


  return error;
}

