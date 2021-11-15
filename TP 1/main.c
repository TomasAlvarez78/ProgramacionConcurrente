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

#include "MonitorBuffer.h"
#include "MemoriaCompartida.h"

#define HILOS 9
#define PEDIDOS 10

void *telefono(void *arg);
void *cocinero(void *arg);
void *delivery(void *arg);
void *interfazJuego(void *arg);
void *leerUsuario(void * arg);
void *decrementarTiempo(void * arg);
int menu();
int crearJuego(void *arg);
int borrarJuego(void *arg);

// Funciones de texto especial
// Hacen que los textos se vean de colores
// diferentes

void textoRojo() {
  printf("\033[0;31m");
}
void textoAmarillo() {
  printf("\033[0;33m");
}
void textoVerde() {
  printf("\033[0;32m");
}
void textoDefecto() {
  printf("\033[0m");
}

struct Juego{
	//Monitores

	//Monitor de comunicacion entre encargado - cocinero
	struct Monitor *m1;
	//Monitor de comunicacion entre cocinero - encargado
	struct Monitor *m2;
	//Memoria compartida para informar al encargado de que puede cobrar
	struct Memoria *mem;
	
	//Semaforo que indica que el telefono esta sonando
	//Se utiliza en conjunto con la bandera telefono
	sem_t *llamada;
	int telefono;

	//Indica el tiempo de juego
	int tiempoDeJuego;

	//Indica la opcion ingresada por el usuario
	int opcion;

	//Enteros acumulativos que indican valores a mostrar al final del juego
	int llamadasPerdidas;
	int pedidosTomados;
	int pedidosCocinados;
	int pedidosEntregados;
	int pedidosCobrados;
	int dineroTotal;

	//Bandera utilizada para mostrar si hay un pedido a cobrar o no
	int cobro;
	
	//Variable que indica a cada cocinero/delivery cual es su numero
	int id;

	//Banderas de estado de cocineros/deliverys
	int estadoCocineros[3];
	int estadoDelivery[2];
	
	//Bandera de estado que indica el cierre de el local
	int cierre;
};

// El main es la funcion mas importante, ya que sincroniza todos los otros threads
// tambien permite la conexion entre ellos, creando el struct Juegito y manipulandolo
// En total crea 9 threads
// 6 threads principales --> 1 telefono, 3 cocineros, 2 delivery's
// 3 threads secundarios --> 1 interfazJuego, 1 leerUsuario, 1 decrementarTiempo
int main(int argc, char *v[]){
	system("clear");
	if(menu() == 2){
		return 0;
	}

	int error;
	int dinero = 0;
	pthread_t *th;
	th = (pthread_t *)(calloc(HILOS, sizeof(pthread_t)));

	struct Juego *juegito = (struct Juego *)(calloc(1, sizeof(struct Juego)));

	error = crearJuego((void *)juegito);
	if(error) perror("crearJuego()");

	//	CREO LOS THREADS
	pthread_create(&th[0], NULL, telefono, (void *)(juegito));
	//	COCINEROS
	for (int i = 1; i < 4; i++)
	{
		pthread_create(&th[i], NULL, cocinero, (void *)(juegito));
		usleep(50000);
		juegito->id++;
	}
	juegito->id=0;
	//	DELIVERY
	for (int i = 4; i < 6; i++)
	{
		pthread_create(&th[i], NULL, delivery, (void *)(juegito));
		usleep(50000);
		juegito->id++;
	}

	//EMPIEZA EL ENVIO DE DATOS
	time_t t;
	srand((unsigned)time(&t));
	pthread_create(&th[6], NULL, interfazJuego, (void *)(juegito));
	pthread_create(&th[7], NULL, leerUsuario, (void *)(juegito));
	pthread_create(&th[8], NULL, decrementarTiempo, (void *)(juegito));
	int pedidoAleatorio = -1;
	while (juegito->tiempoDeJuego > 0){

		switch(juegito->opcion){
			case 1:
				error = sem_trywait(juegito->llamada);
				if(!error){
					juegito->pedidosTomados++;
					pedidoAleatorio = (rand() % 4);
					juegito->opcion = 0;
				}
				break;
			case 2:
				if(pedidoAleatorio > -1){
					error = GuardarDato(juegito->m1, pedidoAleatorio);
				}
				pedidoAleatorio = -1;
				if (error)
					perror("GuardarDato()");
				juegito->opcion = 0;
				usleep(2000);
				break;
			case 3:
				error = LeerMemoria(juegito->mem,&dinero);
				if(error) perror("LeerMemoria()");
				if(dinero > 0){
					switch(dinero){
						case 0:
							juegito->dineroTotal += 50;
							break;
						case 1:
							juegito->dineroTotal += 100;
							break;
						case 2:
							juegito->dineroTotal += 200;
							break;	
						case 3:
							juegito->dineroTotal += 100;
							break;
						case 4:
							juegito->dineroTotal += 400;
							break;
					}
					juegito->pedidosCobrados++;
					juegito->cobro = 0;
					dinero = 0;
					juegito->opcion = 0;
				}
				break;
			case 0:
			break;
			default:
				printf("Opcion invalida\n");
			break;
		}
	}

	juegito->cierre=1;
	error = GuardarDato(juegito->m1, -1);
	error = GuardarDato(juegito->m1, -1);
	error = GuardarDato(juegito->m1, -2);
			
	//TERMINA EL ENVIO DE DATOS
	
	//Finaliza el juego

	printf("Cerrando local...\n");
	for (int i = 0; i < HILOS; i++)
	{
		pthread_join(th[i], NULL);
	}

	printf("Pedidos tomados: %d\n",juegito->pedidosTomados);
	printf("Pedidos cocinados: %d\n",juegito->pedidosCocinados);
	printf("Pedidos entregados: %d\n",juegito->pedidosEntregados);
	printf("Pedidos cobrados: %d\n",juegito->pedidosCobrados);
	printf("Llamadas perdidas: %d\n",juegito->llamadasPerdidas);
	printf("Dinero total: %d\n",juegito->dineroTotal);
	error = borrarJuego((void *)juegito);
	if(error) perror("borrarJuego()");
	
	free(th);	
}

// Telefono genera una llamada aleatoria entre 1 a 3 segundos
// Si es la primera llamada tarda 1 seg.
// Y mantiene la llamada por 3 segundos
// Tras esperar esos 3 segundos confirma si se fue contestada o no
// y incrementa el valor de llamadas perdidas en caso de no contestar
void *telefono(void *arg){
	struct Juego *juegito = (struct Juego *)(arg);
	int estadoLlamada;
	time_t t2;
	srand((unsigned)time(&t2));
	int dormir = 1;
	while(juegito->tiempoDeJuego > 0){
		sleep(dormir);
		sem_post(juegito->llamada);
		sleep(3);
		sem_getvalue(juegito->llamada,&estadoLlamada);
		if(estadoLlamada > 0){
			juegito->llamadasPerdidas++;
			sem_wait(juegito->llamada);
		}
		dormir = (rand() % 2) + 1;
	}
	pthread_exit(NULL);
}

// Cocinero lee el monitor de comunicacion con Encargado
// Si encuentra que hay un pedido, lo cocina, incrementa la cantidad
// de pedidos cocinados y escribe en el monitor de comunicacion con Delivery
// Los cocineros tambien pueden recibir un pedido especial el cual indica que deben
// dejar de trabajar, el ultimo de los cocineros recibira el pedido especial -2
// el cual indica que el es el encargado de avisar a los deliverys que su
// turno termino mediante un pedido especial ( -1 )
void *cocinero(void *arg){
	struct Juego *juegito = (struct Juego *)(arg);
	int error, x;
	int id = juegito->id;
	int dormir = 2;

	// char *comidas[5];
	// comidas[0] = "Sushi";
	// comidas[1] = "Milanesa";
	// comidas[2] = "Pizza";
	// comidas[3] = "Lomito";
	// comidas[4] = "Empanadas";

	while (1)
	{
		error = LeerDato(juegito->m1, &x);

		if (error) perror("LeerDato()");
		if (x == -1)
		{	
			break;
		}else if(x == -2){
			error = GuardarDato(juegito->m2,-1);
			if (error) perror("GuardarDato()");
			error = GuardarDato(juegito->m2,-1);
			if (error) perror("GuardarDato()");
			break;
		}
		else if(x > 0)
		{
			juegito->estadoCocineros[id] = 1;
			sleep(dormir);
			error = GuardarDato(juegito->m2,x);
			if (error) perror("GuardarDato()");
			juegito->pedidosCocinados++;
			juegito->estadoCocineros[id] = 0;

		}
	}
	pthread_exit(NULL);
}

// Delivery lee el monitor de comunicacion de Cocinero
// Si encuentra un pedido, lo entrega, e incrementa la cantidad
// de pedidos, tambien aumenta el estado de la bandera "cobro"
// cual objetivo es saber cuando hay un pago a cobrar
// Si el cocinero lee en el monitor es estado -1, significa que debe terminar
// de trabajar
void *delivery(void *arg){
	struct Juego *juegito = (struct Juego *)(arg);
	int error, x;
	int id = juegito->id;
	int dormir=2000;
	while (1)
	{
		error = LeerDato(juegito->m2, &x);
		if (error) perror("LeerDato()");
		if (x == -1){
			break;
		}else if(x > 0){
			juegito->estadoDelivery[id] = 1;
			usleep(dormir);
			error= GuardarMemoria(juegito->mem,x);
			if(error) perror("GuardarMemoria()");
			juegito->cobro = 1;
			juegito->pedidosEntregados++;
			juegito->estadoDelivery[id] = 0;
		}
	}
	pthread_exit(NULL);
}

// Esta funcion es la primera que ejecuta encargado para la interfaz
// de usuario
int menu(){
	int exit = 0;
	char ch;
	printf("======================================\n");
	printf("=            1 - Jugar               =\n");
	printf("=            2 - Salir               =\n");
	printf("======================================\n");
	
	do{
		printf("Ingrese la opcion: ");
		fflush(stdin);
		ch = getchar();
		switch(ch){
			case 49:
				system("clear");
				printf("A jugar!!\n");
				return 1;
				break;
			case 50:
				system("clear");
				printf("Adios!\n");
				return 2;
				break;
			default:
				printf("Dato Incorrecto!\n");
				break;
		}
	}while(!exit);
	return 0;
}

// Esta funcion la ejecuta un hilo secundario a los 4 principales
// Se encarga de mostrar en pantalla el estado actual del juego
// utilizando banderas
void *interfazJuego(void *arg){
	struct Juego *juegito = (struct Juego *)(arg);

	while(juegito->tiempoDeJuego > 0){
		if(juegito->tiempoDeJuego >= 20) textoVerde();
		else if (juegito->tiempoDeJuego >= 10 && juegito->tiempoDeJuego < 20) textoAmarillo();
		else textoRojo();
		
		printf("Tiempo de Juego restante: %d\n\n",juegito->tiempoDeJuego);
		textoDefecto();
		sem_getvalue(juegito->llamada,&juegito->telefono);
		if(juegito->telefono == 1){
			textoAmarillo();
			printf("Telefono sonando...\n");
		}else{
			printf("Telefono inactivo\n");
		}

		if(juegito->cobro == 1){
			textoVerde();
			printf("Hay un delivery para cobrar...\n");
		}else{
			textoDefecto();
			printf("No hay dinero a cobrar\n");
		}

		for (int i = 0; i < 3; i++){
			textoDefecto();
			printf("Estado del Cocinero %d: ",i+1);
			if(juegito->estadoCocineros[i] == 0){
				textoVerde();
				printf("Desocupado\n");
			}else{
				textoRojo();
				printf("Cocinando...\n");
			}
		}
		textoDefecto();
		
		for (int i = 0; i < 2; i++){
			textoDefecto();
			printf("Estado del Delivery %d: ",i+1);
			if(juegito->estadoDelivery[i] == 0){
				textoVerde();
				printf("Desocupado\n");
			}else{
				textoRojo();
				printf("Entregando\n");
			}
		}

		if(juegito->cierre == 1){
			textoRojo();
			printf("\nEstamos por cerrar el local!\n");
		}else{
			textoVerde();
			printf("\nLocal abierto\n\n");
		}
		textoDefecto();

		printf("1 - Contestar telefono\n");
		printf("2 - Dar pedido a cocinero\n");
		printf("3 - Cobrar delivery\n");
		printf("Ingrese la opcion: \n");
		sleep(1);

		// juegito->tiempoDeJuego--;
		if(juegito->tiempoDeJuego > 0){
			system("clear");
		}
	}
	pthread_exit(NULL);
}


void *decrementarTiempo(void *arg){
	struct Juego *juegito = (struct Juego *)(arg);
	while(juegito->tiempoDeJuego > 0){
		juegito->tiempoDeJuego--;
		sleep(1);
	}
	pthread_exit(NULL);
}

// Esta funcion la ejecuta un hilo secundario a los 4 principales
// Se encarga de tomar los datos ingresados por el usuario
// y lo guarda en un valor en el struct Juego, con el objetivo
// de que el encargado ejecute la accion segun la opcion que eligamos
void *leerUsuario(void *arg){

	struct Juego *juegito = (struct Juego *)(arg);
	int opcion;
	do{
		fflush(stdin);
		scanf("%d",&opcion);
		switch(opcion){
			case 1:
				juegito->opcion=1;
				break;
			case 2:
				juegito->opcion=2;
				break;
			case 3:
				juegito->opcion=3;
				break;
			default:
				printf("Dato Incorrecto!\n");
				break;
		}
		opcion = 0;
	}while(juegito->tiempoDeJuego > 0);

	pthread_exit(NULL);
}

// Esta funcion se encarga de inicializar todas las variables del 
// struct Juego, como banderas, monitores y memorias
int crearJuego(void *arg ){
	struct Juego *juegito = (struct Juego *)(arg);
	int error = 0;
	juegito->m1 = NULL;
	juegito->m2 = NULL;
	juegito->tiempoDeJuego = 30;
	juegito->opcion = 0;
	juegito->id = 0;
	juegito->dineroTotal = 0;
	juegito->telefono=0;
	juegito->cierre=0;
	juegito->cobro=0;
	juegito->pedidosTomados= 0;
	juegito->llamadasPerdidas = 0;
	juegito->pedidosCocinados = 0;
	juegito->pedidosEntregados = 0;
	juegito->pedidosCobrados = 0;

	for (int i = 0; i < 3; i++)
	{
		juegito->estadoCocineros[i] = 0;
		if(i != 2){
			juegito->estadoDelivery[i] = 0;
		}
	}

	juegito->llamada = sem_open("/semBinario2", O_CREAT, O_RDWR, 0);
	if (juegito->llamada == SEM_FAILED){
		perror("sem_open()");
		error = -1;
	}
	if (!error){
		// printf("Semaforo creado!\n");
	}
	// CREAR MONITORES
	juegito->m1 = CrearMonitor();
	if (juegito->m1 != NULL)
	{
		// printf("Monitor creado!\n");
	}
	else
	{
		perror("CrearMonitor()");
		error = -1;
	}

	juegito->m2 = CrearMonitor();

	if (juegito->m2 != NULL){
		// printf("Monitor creado!\n\n");
	}
	else
	{
		perror("CrearMonitor()");
		error = -1;
	}
	// TERMINO CREAR MONITORES
	//CREO MEMORIA COMPARTIDA
	juegito->mem = CrearMemoria();
	if (juegito->mem != NULL)
	{
		// printf("Memoria creada!\n");
	}
	else
	{
		perror("CrearMemoria()");
		error = -1;
	}
	//TERMINO CREAR MEMORIA COMPARTIDA
	return error;
}

// Esta funcion se encarga de borrar todas las variables que
// se tienen que eliminar de forma especial como semaforos, monitores
// y memoria compartida.
int borrarJuego(void *arg){
	struct Juego *juegito = (struct Juego *)(arg);
	int error = 0;
	if (!error){
		error = sem_close(juegito->llamada);
		if (error){
			perror("sem_close()");
		}
		else{
			// printf("Semaforo cerrado\n");
		}
	}

	if (!error){
		error = sem_unlink("/semBinario2");
		if (error)
		{
			perror("sem_unlink()");
		}
		else
		{
			// printf("Semaforo borrado!\n");
		}
	}

	BorrarMemoria(juegito->mem);
	BorrarMonitor(juegito->m1);
	BorrarMonitor(juegito->m2);
	free(juegito);
	return error;
}

