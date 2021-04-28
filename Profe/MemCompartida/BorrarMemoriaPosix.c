#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>

int main (int argc, char *argv[]) {
  int error=0;

  if (argc == 2) {
    error = shm_unlink(argv[1]);
    if (error) {
      perror("shm_unlink()");
    }
    else {
      printf("Memoria compartida borrada!\n");
    }
  }
  else {
    printf("Usar: %s NombreDelaMemoriaCompartida\n", argv[0]);
  }

  return error;
}

