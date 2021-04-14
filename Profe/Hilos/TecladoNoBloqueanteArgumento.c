#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

void *LeerTeclado  (void *);
void *EsperaActiva (void *);

int main () {
  char nombre[50];
  pthread_t hilos[2];

  printf("Ingrese su nombre: ");

  pthread_create(&hilos[1], NULL, EsperaActiva, NULL);
  pthread_create(&hilos[0], NULL, LeerTeclado, (void *)nombre);

  pthread_join(hilos[0], NULL);
  pthread_cancel(hilos[1]);

  printf("Bienvenido %s\n", nombre);
  sleep(3);
  printf("Fin del programa\n");

  return 0;
}


void *LeerTeclado  (void *tmp) {
  char *cadena = (char *)tmp;
  fgets(cadena, 50, stdin);
  pthread_exit(NULL);
}


void *EsperaActiva (void *tmp) {
  int n = 0;
  char c[] = {'|', '-'};
  while (1) {    
    printf("\r%c", c[! n % 2] );
    fflush(stdout);
    n = (n + 1) % 2;
    usleep(200000);
  }
}


