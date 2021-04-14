#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h> /* POSIX -> gcc -pthread */

void *LeerTeclado  (void *);
void *EsperaActiva (void *);

int main () {
  pthread_t hilos[2];

  pthread_create(&hilos[1], NULL, EsperaActiva, NULL);
  pthread_create(&hilos[0], NULL, LeerTeclado, NULL);

  pthread_join(hilos[0], NULL); /* Bloqueante */
  pthread_cancel(hilos[1]);

  sleep(3);
  printf("Fin del programa\n");
  return 0;
}


void *LeerTeclado  (void *tmp) {
  char nombre[50];

  printf("Ingrese su nombre: ");
  fgets(nombre, 50, stdin);
  printf("Bienvenido %s\n", nombre);

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


