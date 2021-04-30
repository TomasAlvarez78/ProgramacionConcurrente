#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <string.h>

sem_t sem[4];

void* telefono(void* arg);
void* encargado(void* arg);
void* cocinero(void* arg);
void* delivery(void* arg);

int estadoLocal;
/*
    int estadodel;
    sem_getvalue(&sem[3],&estadodel);
*/

int main(int argc, char* v[]){
    printf("Hola!\n");
    pthread_t *th;
    /*
    1 - telefono
    2 - encargado
    3 - cocinero
    4 - cocinero
    5 - delivery
    6 - delivery
    7 - delivery
    */
    th = (pthread_t *)(calloc(7,sizeof(pthread_t)));

    for (int i = 0; i < 4; i++)
    {
        if(i == 1){
            sem_init(&sem[i],0,1);
        }else{
            sem_init(&sem[i],0,0);
        }
    }
    pthread_create(&th[0], NULL, encargado, NULL);
    pthread_create(&th[1], NULL, telefono, NULL);
    pthread_create(&th[2], NULL, cocinero, NULL);
    pthread_create(&th[3], NULL, cocinero, NULL);
    for (int i = 4; i < 8; i++)
    {
       pthread_create(&th[i], NULL, delivery, NULL);
    }
    
    for (int i = 0; i < 7; i++)
    {
        pthread_join(th[i], NULL);
    }

    printf("Soy el main thread");

    for (int i = 0; i < 4; i++)
    {
        sem_destroy(&sem[i]);
    }

    return 0;
}


void* telefono(void* arg){
    //int* x = (int*)arg;
    for (int i = 0; i < 3; i++)
    {
        sem_wait(&sem[0]);
        
        time_t t;
        srand((unsigned) time(&t));
        int espera = rand() % (5) + 1;
        usleep(100000 * espera);
        printf("2 - Sono el telefono!\n");
        sem_post(&sem[1]);
    }
    pthread_exit(NULL);
}

void* encargado(void* arg){
    //int* x = (int*)arg;
    while(estadoLocal)
    {
        sem_wait(&sem[1]);
        printf("1 - Encargado trabajando\n");
        printf("Esperando telefono...\n");
        usleep(100000);
        sem_post(&sem[0]);
        sem_wait(&sem[1]);
        printf("1 - Tomando orden...\n");
        usleep(100000);
        sem_post(&sem[2]);

    }
    pthread_exit(NULL);
}

void* cocinero(void* arg){
    //int* x = (int*)arg;
    for (int i = 0; i < 3; i++)
    {
        sem_wait(&sem[2]);
        
        printf("3 - Cocinero trabajando\n");
        time_t t;
        srand((unsigned) time(&t));
        int espera = rand() % (3) + 1;
        usleep(100000 * espera);        
        sem_post(&sem[3]);       
    }
    pthread_exit(NULL);
}
void* delivery(void* arg){
    //int* x = (int*)arg;
    for (int i = 0; i < 3; i++)
    {
        sem_wait(&sem[3]);
        printf("4 - Delivery trabajando\n");
        usleep(100000);
        printf("4 - Delivery dando dinero\n");
        usleep(100000);
        sem_post(&sem[1]);
    }
    pthread_exit(NULL);
}