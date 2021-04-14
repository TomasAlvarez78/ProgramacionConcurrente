#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

struct mtxsss {
  pthread_mutex_t mtx1, mtx2;
  int cant, valor;
};

void *p1 (void *arg);
void *p2 (void *arg);

int main (int argc, char *argv[]) {
  pthread_t h1, h2;
  struct mtxsss *data = NULL;

  if (argc != 2) {
    fprintf(stderr, "Usar:\n\t%s [cant]\n\ndonde [cant] es la cantidad de veces a ejecutarse.\n", argv[0]);
    exit(-1);
  }

  data = (struct mtxsss *)(calloc(1, sizeof(struct mtxsss)));
  if (data != NULL) {
    data->cant = atoi(argv[1]);
    data->valor = 10;
    pthread_mutex_init(&data->mtx1, NULL);
    pthread_mutex_init(&data->mtx2, NULL);
    pthread_mutex_lock(&data->mtx1);

    pthread_create(&h1, NULL, p1, (void *)(data));
    pthread_create(&h2, NULL, p2, (void *)(data));

    pthread_join(h1, NULL);
    pthread_join(h2, NULL);

    pthread_mutex_destroy(&data->mtx1);
    pthread_mutex_destroy(&data->mtx2);
    free(data);
  }

  return 0;
}

void *p1 (void *arg) {
  int i = 0;
  struct mtxsss *data = (struct mtxsss *)(arg);
  for (i = 0; i < data->cant; ++i) {
    pthread_mutex_lock(&data->mtx1);
    printf("p1 -> %3d\n", ++data->valor);
    fflush(NULL);
    pthread_mutex_unlock(&data->mtx2);
  }
}

void *p2 (void *arg) {
  int i = 0;
  struct mtxsss *data = (struct mtxsss *)(arg);
  for (i = 0; i < data->cant; ++i) {
    pthread_mutex_lock(&data->mtx2);
    printf("p2 -> %3d\n", ++data->valor);
    fflush(NULL);
    pthread_mutex_unlock(&data->mtx1);
  }
}


