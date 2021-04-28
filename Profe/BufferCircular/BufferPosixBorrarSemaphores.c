#include <stdio.h>
#include <semaphore.h>

int main (int argc, char *argv[]) {
  int error=0, status=0;

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

  status = sem_unlink("/vacio");
  if (!status)
    printf("Semaforo [vacio] borrado!\n");
  else {
    perror("sem_unlink()");
    error -= 1;
  }

  status = sem_unlink("/escribiendo");
  if (!status)
    printf("Semaforo [escribiendo] borrado!\n");
  else {
    perror("sem_unlink()");
    error -= 1;
  }

  status = sem_unlink("/leyendo");
  if (!status)
    printf("Semaforo [leyendo] borrado!\n");
  else {
    perror("sem_unlink()");
    error -= 1;
  }

  return error;
}

