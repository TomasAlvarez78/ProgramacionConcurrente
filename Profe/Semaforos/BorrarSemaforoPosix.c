#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>

int main (int argc, char *argv[]) {
  int error=0;

  if (argc == 2) {
    error = sem_unlink(argv[1]);
    if (error) {
      perror("sem_unlink()");
    }
    else {
      printf("Semaforo borrado!\n");
    }
  }
  else {
    printf("Usar: %s NombreDelSemaforo\n", argv[0]);
  }

  return error;
}

