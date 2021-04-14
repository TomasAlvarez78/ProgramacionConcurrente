#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

//Struct con datos de punteros

struct mtxsss
{
    int size1, mayor;
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
    int numHilos = pedirValor(1);
    int valMax = pedirValor(2);

    puntero = (struct mtxsss *)(calloc(numHilos, sizeof(struct mtxsss)));
    hilos = (pthread_t *)(calloc(numHilos, sizeof(pthread_t)));
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

    for (int i = 0; i < numHilos; i++)
    {
        puntero[i].numHilos = numHilos;
        puntero[i].size1 = size1;
        puntero[i].id = i;
        puntero[i].mayor = array[0];
        puntero[i].ptr = array;
        pthread_mutex_init(&puntero[i].mtx1,NULL);
    }

    for (int i = 0; i < numHilos; i++)
    {
        pthread_create(&hilos[i], NULL, buscarmayor, &puntero[i]);
    }
    
    pthread_mutex_lock(&puntero[0].mtx1);

    for (int i = 0; i < numHilos; i++)
    {
        pthread_join(hilos[i], NULL);
    }

    for (int i = 0; i < numHilos; i++)
    {
        pthread_mutex_destroy(&puntero[i].mtx1);
    }
       
    int mayor=0;
    int posicion=0;

    for (int i = 0; i < numHilos; i++)
    {
        if(puntero[i].mayor > mayor){
            mayor = puntero[i].mayor;
            posicion = i;
        }
    }
    
    printf("El mayor del id %i fue el numero %i\n",posicion+1,mayor);

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
    for (int i = puntero->id * (puntero->size1 / puntero->numHilos); i < ((puntero->id + 1) * (puntero->size1 / puntero->numHilos)); i++)
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
