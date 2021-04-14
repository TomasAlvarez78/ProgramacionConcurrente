#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

//Estructura basica con cantidad de llamadas
//y mutex de cada thread
struct mtxsss{
    int llamadas;
    pthread_mutex_t mtx[4];
};

//Sin uso, implementar
int status = 0;

//Una funcion por thread
//Cada uno se desbloquea al otro
//Tel -> Encargado -> Cocinero -> Delivery-.
// ^---------------------------------------'
void *telefono(void *arg);
void *encargado(void *arg);
void *cocinero(void *arg);
void *delivery(void *arg);


int main(int argc, char*argv[]){
    pthread_t *hilos;
    struct mtxsss *data = NULL;

    //Creo los hilos(4) y la estructura de datos
    hilos = (pthread_t *)(calloc(4, sizeof(pthread_t)));
    data = (struct mtxsss *)(calloc(1, sizeof(struct mtxsss)));
    
    //Inicializo los mutex
    for (int i = 0; i < 4; i++)
    {
        pthread_mutex_init(&data->mtx[i], NULL);
    }

    //Bloqueo todos los mutex menos el primero
    //De esta forma me aseguro que telefono se ejecute
    //primero
    for (int i = 1; i < 4; i++)
    {
        pthread_mutex_lock(&data->mtx[i]);
    }

    //Creo los hilos
    pthread_create(&hilos[0], NULL, telefono, (void *)data);
    pthread_create(&hilos[1], NULL, cocinero, (void *)data);
    pthread_create(&hilos[2], NULL, delivery, (void *)data);
    pthread_create(&hilos[3], NULL, encargado, (void *)data);

    //Hago un join por cada hilo
    for (int i = 0; i < 4; i++)
    {
        pthread_join(hilos[i], NULL);
    }
    
    //Destruyo todos los mutex como Hulk
    for (int i = 0; i < 4; i++)
    {
        pthread_mutex_destroy(&data->mtx[i]);
    }
    
    //Libero
    free(hilos);
    free(data);
    return 0;
}

/*
    Explicacion antes de cada funcion:
    Por defecto todas las funciones se ejecutan 5 veces
    Todas se ejecutan de forma paralela, pero no avanzan
    debido al lockeo de mutex, cada uno se lockea a si mismo al trabajar y
    desbloquea al siguiente que tiene que trabajar

    Tel -> Encargado -> Cocinero -> Delivery-.
     ^---------------------------------------'

*/

//La funcion telefono se trata de el trabajo del mismo telefono
//donde tiene una espera aleatorio de 0 a 5 seg.
//No pueden haber mas de una llamadas
//Las llamadas se acumulan, hasta 5.
//Solo hay llamadas una vez se completa un ciclo de trabajo
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

//El encargado se encarga de "recibir" la llamada
//despues de su trabajo (2 segundos falsos) desbloquea al cocinero

//Implementar
//Generacion aleatoria de un numero para un menu aleatorio
//Cuyo menu tiene un tiempo especifico de cocina
//Implementar en la estructura
void *encargado(void *arg){
    struct mtxsss *data = (struct mtxsss *)(arg);
    for (int i = 0; i < 5; i++)
    { 
        pthread_mutex_lock(&data->mtx[1]);
        printf("2 - Encargado trabajando\n");
        sleep(2);
        fflush(NULL);
        pthread_mutex_unlock(&data->mtx[2]);
    }
    pthread_exit(NULL);
}


//El cocinera se encarga de "cocinar"
//despues de su trabajo (2 segundos falsos)
//da la comida al delivery (desbloqueo)

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


//El delivery se encarga de "entregar" la comida
//despues de su trabajo (2 segundos falsos) desbloquea al telefono
//En este momento se inicia otro ciclo de trabajo
//Implementar tiempo aleatorio de 0 a 10 seg. que tarda en entregar

//Cambiar
//No permitir que delivery habilite al telefono
//Que le habilite al encargado, y que ahi habilite el telefono
//Mediante un recurso compartido por ejemplo

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
