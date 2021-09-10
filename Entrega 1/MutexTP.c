#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

struct mtxsss{
    int id,llamadas;   
    int *ptr;
    pthread_mutex_t mtx[4];
};

int status = 0;

void *telefono(void *arg);
void *encargado(void *arg);
void *cocinero(void *arg);
void *delivery(void *arg);

int main(int argc, char*argv[]){
    pthread_t *hilos;
    struct mtxsss *data = NULL;

    hilos = (pthread_t *)(calloc(4, sizeof(pthread_t)));
    data = (struct mtxsss *)(calloc(1, sizeof(struct mtxsss)));
    data->id = 0;
    for (int i = 0; i < 4; i++)
    {
        pthread_mutex_init(&data->mtx[i], NULL);
    }

    for (int i = 1; i < 4; i++)
    {
        pthread_mutex_lock(&data->mtx[i]);
    }

    pthread_create(&hilos[0], NULL, telefono, (void *)data);
    pthread_create(&hilos[1], NULL, cocinero, (void *)data);
    pthread_create(&hilos[2], NULL, delivery, (void *)data);
    pthread_create(&hilos[3], NULL, encargado, (void *)data);

    for (int i = 0; i < 4; i++)
    {
        data->id = i;
        pthread_join(hilos[i], NULL);
    }
    
    for (int i = 0; i < 4; i++)
    {
        pthread_mutex_destroy(&data->mtx[i]);
    }
    
    sleep(5);
    free(hilos);
    free(data);
    return 0;
}

void *telefono(void *arg){
    struct mtxsss *data = (struct mtxsss *)(arg);
    
    //espera aleatoria
    for (int i = 0; i < 5; i++)
    {
        pthread_mutex_lock(&data->mtx[0]);
        printf("1 - Telefono trabajando\n");
        time_t t;
        srand((unsigned) time(&t));
        int espera = rand() % (5);
        sleep(espera);

        printf("Llamada recibida -> total: %d\n", ++data->llamadas);
        fflush(NULL);
        pthread_mutex_unlock(&data->mtx[1]);
    }

    pthread_exit(NULL);
}

void *encargado(void *arg){
    struct mtxsss *data = (struct mtxsss *)(arg);
    
    for (int i = 0; i < 5; i++)
    { 
        pthread_mutex_lock(&data->mtx[1]);
        printf("2 - Encargado trabajando\n");
        time_t t;
        srand((unsigned) time(&t));
        int menu = rand() % 5;
        sleep(2);
        fflush(NULL);
        pthread_mutex_unlock(&data->mtx[2]);
    }
    pthread_exit(NULL);
}

void *cocinero(void *arg){
    struct mtxsss *data = (struct mtxsss *)(arg);
    for (int i = 0; i < 5; i++)
    {
        pthread_mutex_lock(&data->mtx[2]);
        printf("3 - Cocinero trabajando\n");
        sleep(2);
        fflush(NULL);
        pthread_mutex_unlock(&data->mtx[3]);
    }
    pthread_exit(NULL);
}

void *delivery(void *arg){
    struct mtxsss *data = (struct mtxsss *)(arg);
    
    for (int i = 0; i < 5; i++)
    {
        pthread_mutex_lock(&data->mtx[3]);
        printf("4 - Delivery trabajando\n");
        sleep(2);
        fflush(NULL);
        pthread_mutex_unlock(&data->mtx[0]);
    }
    pthread_exit(NULL);
    
}
