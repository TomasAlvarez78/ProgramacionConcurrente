#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>

#define PEDIDOS 10
struct boundedBuffer_t {
  int inicio, fin, localAbierto;
  int buf[PEDIDOS];
  sem_t *lleno, *vacio, *leyendo, *escribiendo,*cocina;
};


void* telefono(void* arg);
void* encargado(void* arg);
void* cocinero(void* arg);
void* delivery(void* arg);

void inicializar(struct boundedBuffer_t * bb);
int destruirSemaforos(struct boundedBuffer_t * bb);
int estadoLocal;

int main(int argc, char* v[]){
    struct boundedBuffer_t* bb = NULL;
    pthread_t* th;
    th = (pthread_t *)(calloc(7,sizeof(pthread_t)));

    inicializar(&bb);
    //Creacion de threads
    pthread_create(&th[0], NULL, encargado, (void*)bb);
    pthread_create(&th[1], NULL, telefono, (void*)bb);
    for (int i = 2; i < 5; i++)
    {
       pthread_create(&th[i], NULL, cocinero, (void*)bb);
    }
    for (int i = 5; i < 7; i++)
    {
       pthread_create(&th[i], NULL, delivery, (void*)bb);
    }
    
    for (int i = 0; i < 7; i++)
    {
        pthread_join(th[i], NULL);
    }

    printf("Soy el main thread");

    destruirSemaforos(bb);

    free(bb);
    free(th);

    return 0;
}

void inicializar(struct boundedBuffer_t * bb){
    int error = 0;

    //Inicializacion punteros
    
    bb = (struct BoundedBuffer_t ** )(calloc(1,sizeof(struct boundedBuffer_t)));

    //Inicializacion BoundedBuffer
    bb->localAbierto=1;
    printf("asd");
    bb->lleno = sem_open("/lleno", O_CREAT, 0640, 0);
    if (bb->lleno != SEM_FAILED) {
        printf("Semaforo [lleno] creado!\n");
    }
    else {
        perror("sem_open()");
        error -= 1;
    }
    bb->vacio = sem_open("/vacio", O_CREAT, 0640, PEDIDOS);
    if (bb->vacio != SEM_FAILED) {
        printf("Semaforo [vacio] creado!\n");
    }
    else {
        perror("sem_open()");
        error -= 1;
    }
    bb->leyendo = sem_open("/leyendo", O_CREAT, 0640, 1);
    if (bb->leyendo != SEM_FAILED) {
        printf("Semaforo [leyendo] creado!\n");
    }
    else {
        perror("sem_open()");
        error -= 1;
    }
    bb->cocina = sem_open("/cocina", O_CREAT, 0640, 0);
    if (bb->cocina != SEM_FAILED) {
        printf("Semaforo [cocina] creado!\n");
    }
    else {
        perror("sem_open()");
        error -= 1;
    }
    bb->escribiendo = sem_open("/escribiendo", O_CREAT, 0640, 1);
    if (bb->escribiendo != SEM_FAILED) {
        printf("Semaforo [escribiendo] creado!\n");
    }
    else {
        perror("sem_open()");
        error -= 1;
    }
    printf("asd");
}

int destruirSemaforos(struct boundedBuffer_t* bb){
    int error = 0,status;
    status = sem_close(bb->lleno);
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
    status = sem_close(bb->vacio);
    if (!status) {
        status = sem_unlink("/vacio");
        if (!status)
        printf("Semaforo [vacio] borrado!\n");
        else {
        perror("sem_unlink()");
        error -= 1;
        }
    }
    else {
        perror("sem_close()");
        error -= 1;
    }
    status = sem_close(bb->escribiendo);
    if (!status) {
        status = sem_unlink("/escribiendo");
        if (!status)
        printf("Semaforo [escribiendo] borrado!\n");
        else {
        perror("sem_unlink()");
        error -= 1;
        }
    }
    else {
        perror("sem_close()");
        error -= 1;
    }
    status = sem_close(bb->leyendo);
    if (!status) {
        status = sem_unlink("/leyendo");
        if (!status)
        printf("Semaforo [leyendo] borrado!\n");
        else {
        perror("sem_unlink()");
        error -= 1;
        }
    }
    else {
        perror("sem_close()");
        error -= 1;
    }
    status = sem_close(bb->cocina);
    if (!status) {
        status = sem_unlink("/cocina");
        if (!status)
        printf("Semaforo [cocina] borrado!\n");
        else {
        perror("sem_unlink()");
        error -= 1;
        }
    }
    else {
        perror("sem_close()");
        error -= 1;
    }
    return error;
}

void* telefono(void* arg){
    struct boundedBuffer_t* bb = (struct boundedBuffer_t*) arg;
    int error;
    while(bb->localAbierto)
    {
        time_t t;
        srand((unsigned) time(&t));
        int espera = rand() % (5) + 1;
        usleep(100000 * espera);
        printf("TELEFONO ----> Sono el telefono!\n");
        error = sem_wait(bb->vacio);
        if (!error) {
            error = sem_wait(bb->escribiendo);
        }
        if(!error){
            bb->buf[bb->fin] = (rand() % 5) + 1;
            printf("Menu numero: %02d\n",bb->buf[bb->fin]);
            bb->fin = ++bb->fin % PEDIDOS;
            error = sem_post(bb->escribiendo);
        }
        if (!error) {
            error = sem_post(bb->lleno);
            usleep(rand() % 2000000);
        }
    }

    pthread_exit(NULL);
}
/*
    lleno = 0
    vacio = pedidos - 10
    escribiendo = 1
    leyendo = 1
*/
void* encargado(void* arg){
    struct boundedBuffer_t* bb = (struct boundedBuffer_t*) arg;
    int error,leido;
    while(bb->localAbierto)
    {
        time_t t;
        srand((unsigned) time(&t));
        int espera = rand() % (5) + 1;
        usleep(100000 * espera);
        printf("ENCARGADO ----> Esperando llamada!\n");

        error = sem_trywait(bb->lleno);
        if(!error){
            error = sem_wait(bb->leyendo);
        }
        if(!error){
            leido = bb->buf[bb->inicio];
            if(leido > 0){
                printf("Es hora de trabajar\n");
                error = sem_post(bb->cocina);
                if(error){
                    perror("sem_post(cocina)");
                }
            }
            bb->inicio = ++bb->inicio % PEDIDOS;
            error = sem_post(bb->leyendo);
        }
        
    }

    pthread_exit(NULL);
}
void* cocinero(void* arg){
    struct boundedBuffer_t* bb = (struct boundedBuffer_t*) arg;
    while(bb->localAbierto)
    {
        time_t t;
        srand((unsigned) time(&t));
        int espera = rand() % (5) + 1;
        usleep(100000 * espera);
        printf("COCINERO ESPERANDO \n");

        
    }
    pthread_exit(NULL);
}
void* delivery(void* arg){
    struct boundedBuffer_t* bb = (struct boundedBuffer_t*) arg;
    while(bb->localAbierto)
    {
        time_t t;
        srand((unsigned) time(&t));
        int espera = rand() % (5) + 1;
        usleep(100000 * espera);
        printf("COCINERO ESPERANDO \n");

        
    }
    pthread_exit(NULL);
}
/*
void* telefono(void* arg){
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

*/