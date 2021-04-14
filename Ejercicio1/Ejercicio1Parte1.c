#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h> 

void Funcion1(int * temp,int tamano);
int pedirValor(int x);
int main(int arg,char *args[]){

	//Creo un puntero tipo entero y coloco x  posiciones de memoria en 0
	//Dejo al usuario ingresar el tamano de el array
	//Y tambien le dejo ingresar en que posicin estara el 1
	//gcc -pthread -Wall -pedantic search1inceros.c -o search1inceros
	
	//Declaro el tamano y le asigno un valor
	int size1;
	size1 = pedirValor(0);

	//Declaro el puntero con 0s del tamano de size
	int * ptr = calloc(size1,sizeof(int));
	
	//Declaro la posicion y le pregunto donde poner el 1
	int posicion = 0;
	posicion = pedirValor(size1);
	while(posicion == 0){
		posicion = pedirValor(size1);
	}
	
	ptr[posicion] = 1;
	
	Funcion1(ptr,size1);
}

//Esta funcion busca el 1 teniendo en cuenta el tamano
void Funcion1(int * temp,int tamano){
	int flag = 0;
	for(int i=0; i <= tamano;i++){
		if(temp[i] == 1){
			printf("Encontre un uno en la posicion %i\n",i);
			flag = 1;
		}
	}
	if(flag == 1){
		printf("Valor encontrado con exito\n");
	}else{
		printf("Valor no encontrado\n");
	}
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
