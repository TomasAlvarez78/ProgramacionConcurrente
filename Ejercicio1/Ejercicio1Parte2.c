#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h> 

int pedirValor(int x);
void * buscaruno(void *tmp);

int size1;
int numero_thread;

int main(int arg,char *args[]){

	//Declaro el struct punteros
	//struct Punteros puntero;
	numero_thread = 0;
	size1 = pedirValor(0);

	//Inicializo los punteros del struct
	int * ptr = (int *)(calloc(size1,sizeof(int)));

	//Salgo si el usuario es malo y pone un numero que no debe
	if((ptr == NULL)){
		printf("Ingreso un valor incorrecto\n");
		exit(-1);
	}

	//Ingreso la posicion en la que quiero poner un 1
	int posicion = 0;
	while(posicion == 0){
		posicion = pedirValor(size1);
	}
	
	ptr[posicion] = 1;

	//Creo los hilos
	pthread_t hilos[2];

	//Pongo a ejecutar los hilos
	for (int i = 0; i < 2; i++)
	{
		pthread_create(&hilos[i],NULL,buscaruno,(void *)ptr);
	}
	

	fflush(stdout);
	//Espero a que terminen los hilos
	sleep(3);


	return 0;
}

//Vector3d *vector = (Vector3d*) &acceleration;
void * buscaruno(void *tmp){
	int flag = 0;
	int * ptr = (int *)tmp;
	int numero = numero_thread++;
	//int i = num * (MAX / 4); i < ((num + 1) * (MAX / 4)); i++
	for(int i = numero * (size1/2); i < ((numero + 1) * (size1/2)); i++){
		if(ptr[i] == 1){
			printf("Encontre un uno en la posicion %i\n",i);
			flag = 1;
		}
	}
	if(flag == 1){
		printf("El thread %i lo encontro\n",numero_thread);
	}
	pthread_exit(NULL);
}

//En esta funcion pido al usuario que ingrese cantidad
//de elemetnos en el array
//Y despues ingresa en que posicion esta el 1
//sin que se pase del maximo del array
int pedirValor(int x){
	
	char valor[5]= "0";
	if(x == 0){
		printf("Ingrese cantidad de elementos: ");	
		fgets(valor,6,stdin);
	}else{

 		printf("Ingrese posicion donde quiere ingresar el 1: ");
		fgets(valor,6,stdin);
		if(atoi(valor) > x){
			printf("Ingreso un numero mayor a la cantidad de posiciones\nIntentelo denuevo\n");
			fflush(stdin);
			return 0;
		}
	}
	fflush(stdin);
	return atoi(valor);
}
