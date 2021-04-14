#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

typedef struct {
  char nombre[50];
  int listo;
} Nombre;

void *LeerTeclado  (void *);
void *EsperaActiva (void *);

int main () {
  Nombre usuario;
  pthread_t hilos[2];

  usuario.listo = 0;

  printf("Ingrese su nombre: ");

  pthread_create(&hilos[1], NULL, EsperaActiva, &usuario);
  pthread_create(&hilos[0], NULL, LeerTeclado, &usuario);

  pthread_join(hilos[0], NULL);
/*   pthread_join(hilos[1], NULL); */

  printf("Bienvenido %s\n", usuario.nombre);
  sleep(3);
  printf("Fin del programa\n");

  return 0;
}


void *LeerTeclado  (void *tmp) {
  Nombre *usr = (Nombre *)(tmp);
  fgets(usr->nombre, 50, stdin);
  usr->listo = 1;
  pthread_exit(NULL);
}


void *EsperaActiva (void *tmp) {
  Nombre *usr = (Nombre *)(tmp);
  int n = 0;
  char c[] = {'|', '-'};
  
  while (!usr->listo) {    
    printf("\r%c", c[! n % 2] );
    fflush(stdout);
    n = (n + 1) % 2;
    usleep(200000);
  }
  pthread_exit(NULL);  
}


