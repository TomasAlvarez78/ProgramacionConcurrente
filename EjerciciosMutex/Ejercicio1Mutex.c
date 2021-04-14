#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
//Prueba
//Struct con datos de punteros

struct mtxsss
{
    int size1, mayor,pos;
    int id;
    int numHilos;
    int *ptr;
    pthread_mutex_t mtx1;
};

void *buscarmayor(void *);
int pedirValor(int x);

int main()
{
    pthread_t *hilos;

    struct mtxsss *puntero = NULL;
    int *array = NULL;
    int size1 = pedirValor(0);
    int valMax = pedirValor(2);

    puntero = (struct mtxsss *)(calloc(1, sizeof(struct mtxsss)));
    hilos = (pthread_t *)(calloc(1, sizeof(pthread_t)));
    array = (int *)(calloc(size1, sizeof(int)));

    time_t t;
    srand((unsigned) time(&t));
    for (int i = 0; i < size1; i++)
    {
        array[i] = rand() % (valMax-1);
    }
    
    array[rand() % size1] = valMax;

    for (int i = 0; i < size1; i++)
    {
        printf("Numero %i: %i ",i,array[i]);
        printf("\n");
    }

    puntero[0].numHilos = 1;
    puntero[0].size1 = size1;
    puntero[0].id = 1;
    puntero[0].mayor = array[0];
    puntero[0].ptr = array;
    puntero[0].pos = 0;
    pthread_mutex_init(&puntero[0].mtx1,NULL);

    
    pthread_create(&hilos[0], NULL, buscarmayor, &puntero[0]);
    
    pthread_mutex_lock(&puntero[0].mtx1);

    pthread_join(hilos[0], NULL);
    
    pthread_mutex_destroy(&puntero[0].mtx1);
    
    printf("El mayor del id %i fue el numero %i\n",puntero->pos,puntero->mayor);

    free(puntero);
    free(array);
    free(hilos);
}

int pedirValor(int x)
{

    char valor[5];
    if(x == 0){
        printf("Ingrese la cantidad de elementos: ");
    }else if(x == 1){
        printf("Ingrese la cantidad de hilos: ");
    }else{
        printf("Ingrese el valor maximo del array: ");
    }
    
    fgets(valor, 6, stdin);

    fflush(stdin);
    return atoi(valor);
}

void *buscarmayor(void *tmp)
{
    //guardo la variable puntero del main en la variable de la funcion
    struct mtxsss *puntero = (struct mtxsss *)tmp;
    //recorre el array en busca del mayor
    for (int i = 0; i < puntero->size1; i++)
    {
        if (puntero->ptr[i] > puntero->mayor)
        {
            pthread_mutex_lock(&puntero->mtx1);
            puntero->mayor = puntero->ptr[i];
            pthread_mutex_unlock(&puntero->mtx1);
        }
    }
    pthread_exit(NULL);
}
