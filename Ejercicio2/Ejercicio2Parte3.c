#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h> 

struct Punteros{
    int cantidadThreads;
    int size1;
    int mayor;
    int posicion;
    int numero_thread;
    int * ptr;
};
void * buscarmayor(void *);
int pedirValor(int);

int main(){
    //Inicializo valores
    struct Punteros puntero;
    puntero.size1 = pedirValor(0);
    puntero.mayor = 0;
    puntero.posicion = 0;
    puntero.numero_thread=0;

    puntero.cantidadThreads = pedirValor(1);

    //Si el usuario ingreso un valor que no quiero lo echo
    if(puntero.size1 < 1 || puntero.cantidadThreads < 1 || puntero.cantidadThreads > 9){
        printf("Ingreso un valor incorrecto, intentelo denuevo mas tarde\n");
        return 0;
    }
    
    int array[puntero.size1];


    //Creo el array con valores aleatorios del 0 al 149
    time_t t;
	srand((unsigned) time(&t));
	for (int i = 0; i < puntero.size1; i++)
	{
		array[i] = rand() %150;
	}

    //Enlazo el puntero con la primera posicion del array
    puntero.ptr = &array[0];

    //Printeo el array para simplemente verlo
    for (int i = 0; i < puntero.size1; i++)
	{
		printf("Numero %i ", array[i]);
        if(i != 0 && ((i % 4) == 0)){
            printf("\n");
        }
	}
    printf("\n");

    //Creo N cantidad de hilos y los ejecuto
    pthread_t hilos[puntero.cantidadThreads];
    for (int i = 0; i < puntero.cantidadThreads; i++)
    {
        pthread_create(&hilos[i],NULL,buscarmayor,&puntero);
    }
    
    sleep(3);
    printf("Numero mayor: %i\nEn Posicion: %i\n",puntero.mayor,puntero.posicion);  
    return 0;

}
//Funcion que permite el ingreso de valores
//Como la cantidad de elementos o cantidad de threads
int pedirValor(int x){
	
	char valor[5];
	if(x == 0){
        printf("Ingrese cantidad de elementos: ");	
    }else{
        printf("Ingrese la cantidad de threads: ");	
    }
    
	fgets(valor,6,stdin);
	fflush(stdin);
	return atoi(valor);
}


//Esta funcion busca el mayor con su propio mayor local
//Asi se sabe si el thread encontrado fue el mismo del ejecutado
void * buscarmayor(void *tmp){
    struct Punteros * puntero = (struct Punteros *)tmp;
    //Numero es el numero actual de thread y mayorlocal es el mayor del thread que encontro
	int numero = puntero->numero_thread++;
    int mayorlocal = 0;
	for(int i = numero * (puntero->size1/puntero->cantidadThreads); i < ((numero + 1) * (puntero->size1/puntero->cantidadThreads)); i++){
		if(puntero->ptr[i] > puntero->mayor){
            puntero->mayor = puntero->ptr[i];
            mayorlocal = puntero->mayor;
            puntero->posicion = i;
		}
	}
    //Espero un segundo para que un thread no crea que lo encontro
    //debido a que otro no termino su ejecucion
    sleep(1);
    if(mayorlocal == puntero->mayor){
        printf("El thread %i encontro el mayor ==> %i\n",numero+1,mayorlocal);
    }
	pthread_exit(NULL);
}