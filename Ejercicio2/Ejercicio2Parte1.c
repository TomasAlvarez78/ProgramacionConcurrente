#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

int size1;

int pedirValor();
int main(int arg,char *args[]){

	//Creo un array, un puntero y el entero que me indica la posicion
	size1 = pedirValor();
	int array [size1];
	int * ptr[size1];
	int x = 0;

	//Esta parte del codigo genera numeros aleatorios dentro del array
	time_t t;
	srand((unsigned) time(&t));

	for (int i = 0; i < size1; i++)
	{
		array[i] = rand() %50;
	}
	//

	//Enlazo el puntero con el array
	for (int i = 0; i < size1; i++) {
		ptr[i] = &array[i];
	}

	int mayor = 0;

	//Busco el mayor y lo guardo en mayor y en la posicion
	for (int i = 0; i < size1; i++) {

		if((int) *ptr[i] > mayor){
			mayor = (int)*ptr[i];
			x = i;
		}
	}


	/*
	 *Funcion de muestra de puntero
	 *Solo de testeo
	 */
	for (int i = 0; i < size1; i++)
	{
		printf("Numero %i ", (int) *ptr[i]);
		if(i%5 == 0){
			printf("\n");
		}
	}
	printf("\n");

	//Printeo cual fue el mayor y su posicion
	printf("El mayor numero es: %i\nEn la posicion %i\n",mayor,x);
	
	return 0;
}


int pedirValor(){
	
	char valor[5]= "0";
	printf("Ingrese cantidad de elementos: ");	
	fgets(valor,6,stdin);
	
	fflush(stdin);
	return atoi(valor);
}
