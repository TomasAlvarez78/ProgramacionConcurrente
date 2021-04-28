#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>

int EstadoDelSemaforo (sem_t *s);

int main (int argc, char *argv[]) {
  int error=0;
  sem_t *binario;

  srand(time(NULL));

  binario = sem_open("/semBinario", O_CREAT, O_RDWR, 1);
  if (binario == SEM_FAILED) {
    perror("sem_open()");
    error = -1;
  }

  if (!error) {
    printf("Semaforo creado!\n");

    usleep(rand()%500000);
    error = EstadoDelSemaforo(binario);
  }

  if (!error) {
    usleep(rand()%500000);
    error = sem_wait(binario);
  }

  if (!error) {
    error = EstadoDelSemaforo(binario);
  }

  if (!error) {
    usleep(rand()%500000);
    error = sem_post(binario);
  }

  if (!error) {
    error = EstadoDelSemaforo(binario);
  }

  if (error) {
    perror("sem()");
  }
  else {
    error=sem_close(binario);
    if (error) {
      perror("sem_close()");
    }
    else {
      printf("Semaforo cerrado\n");
      usleep(50000);
    }
  }

  if (!error) {
    error = sem_unlink("/semBinario");
    if (error) {
      perror("sem_unlink()");
    }
    else {
      printf("Semaforo borrado!\n");
    }
  }

  return error;
}


int EstadoDelSemaforo (sem_t *s) {
  int error=0, estado=0;

  error = sem_getvalue(s, &estado);
  if (error) {
    perror("sem_getvalue");
  }
  else {
    printf("El semaforo tiene valor %d\n", estado);
  }
  
  return error;
}

