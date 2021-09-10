#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

//Struct con datos de punteros

struct mtxsss
{
    int size1, mayor, posicion, numero_thread;
    int *ptr;
};

void *buscarmayor(void *);
int pedirValor();

int main()
{
    pthread_t *hilos;

    struct mtxsss *puntero = NULL;
    int *array = NULL;
    int size1 = pedirValor();
    int numHilos = 2;

    puntero = (struct mtxsss *)(calloc(numHilos, sizeof(struct mtxsss)));
    hilos = (pthread_t *)(calloc(numHilos, sizeof(pthread_t)));
    array = (int *)(calloc(size1, sizeof(int)));

    //puntero->ptr = (int *)(calloc(size1, sizeof(int)));

    //if (!= NULL)

    {

        for (int i = 0; i < size1; i++)
        {
            array[i] = rand() % 150;
            //printf("Numero: %i ", puntero->ptr[i]);
            //printf("\n");
        }

        for (int i = 0; i < size1; i++)
        {

            printf("Numero: %i ", array[i]);
            printf("\n");
        }

       

        for (int i = 0; i < numHilos; i++)
        {
        puntero[i].size1 = size1;
        puntero[i].numero_thread = i;
        puntero[i].mayor = array[0];
        puntero[i].ptr = array;
        pthread_create(&hilos[i], NULL, buscarmayor, &puntero[i]);
        }
        
       

        sleep(3);

        free(puntero);
        free(array);
        free(hilos);

        /*int mayor = 0;
        for ( int i = 0; i < size1; i++)
        {
            if(puntero->ptr[i] > mayor) {
                mayor = puntero->ptr[i];
            }
        }

        printf("mayor es %i :", mayor);
        */
    }
}

int pedirValor()
{

    char valor[5];
    printf("Ingrese cantidad de elementos: ");
    fgets(valor, 6, stdin);

    fflush(stdin);
    return atoi(valor);
}

void *buscarmayor(void *tmp)
{
    //guardo la variable puntero del main en la variable de la funcion
    struct mtxsss *puntero = (struct mtxsss *)tmp;
    int numero = puntero->numero_thread++;
    int mayorlocal = 0;
    int impar = 0;

    //recorre el array en busca del mayor
    //if ((puntero->size1 % 2) == 1)
    // impar++;
    for (int i = numero * (puntero->size1 / 2); i < ((numero + 1) * (puntero->size1 / 2)) + impar; i++)
    {
        printf("%i", puntero->ptr[i]);
        printf("\n");

        //pthread_mutex_lock(&puntero->mtx1);
        /*if (puntero->ptr[i] > puntero->mayor)
        {
            puntero->mayor = puntero->ptr[i];
            mayorlocal = puntero->mayor;
            puntero->posicion = i;
        }*/
        //pthread_mutex_unlock(&puntero->mtx1);
    }
    //print del thread que encontro el mayor
    printf("Puntero mayor: %i ", puntero->mayor);
    printf("Puntero mayor: %i ", mayorlocal);
    sleep(1);
    if (mayorlocal == puntero->mayor)
    {
        printf("El thread %i encontro el mayor ==> %i\n", numero + 1, mayorlocal);
    }
    pthread_exit(NULL);
}
