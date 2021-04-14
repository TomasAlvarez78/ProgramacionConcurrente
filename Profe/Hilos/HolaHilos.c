#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

void *Saludar(void *);

int seguir = 1;

int main () {
    int N = 0, i = 0, *id;
    pthread_t *hilos;

    fprintf(stdout, "cantidad? ");
    fscanf(stdin, "%d", &N);

    id = (int *)(calloc(N, sizeof(int)));
    hilos = (pthread_t *)(calloc(N, sizeof(pthread_t)));

    if ((id == NULL) || (hilos == NULL)) {
        exit(-1);
    }

    for (i = 0; i < N; i++) {
        id[i] = 1000 + i;
        pthread_create(&hilos[i], NULL, Saludar, &id[i]);
    }

    sleep(10);
    seguir = 0;

    for (i = 0; i < N; i++) {
        pthread_join(hilos[i], NULL);
    }

    printf("Fin del programa\n");

    free(id);
    free(hilos);

    return 0;
}


void *Saludar (void *tmp) {
    int *id = (int *)(tmp);
    int x = 50, y = 0;
    fprintf(stdout, "Hola, soy hilo %d\n", *id);
    while (seguir) {
        y = x / 1000 + 120 * x;
    }
    fprintf(stdout, "Adios! (%d)\n", *id);
    pthread_exit(NULL);
}
