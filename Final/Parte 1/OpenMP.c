#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <time.h>
#include <sys/sysinfo.h>

#define f(x) x

void midPointRule(double uplimit,double lowlimit,int interval);
void rectangleMethod(double uplimit,double lowlimit,int interval);
void trapezoidalMethod(double uplimit,double lowlimit,int interval);
void simpsonRuleMethod(double uplimit,double lowlimit,int interval);

int main(int argc, char *argv[]){
	int chunk = 1, tid;

	if(argc > 2){
		chunk = atoi(argv[1]);
	}

	double a,b;
	int n;

	//Checkear si es numero
	fprintf(stdout,"Ingrese el valor del limite inferior:\n");
	scanf("%lf",&a);
	fprintf(stdout,"Ingrese el valor del limite superior:\n");
	scanf("%lf",&b);
	if (a >= b){
		fprintf(stdout,"El limite inferior tiene que ser menor al limite superior\n");
		return 0;
	}
	fprintf(stdout,"Ingrese la cantidad de intervalos:\n");
	scanf("%d",&n);


	fprintf(stdout,"Se empezara a calcular...\n");

	struct timeval start, end;
	double elapsed;
	struct timespec start1, finish1;
	double elapsed1;

	gettimeofday(&start, NULL);
	clock_gettime(CLOCK_MONOTONIC,&start1);

	#pragma omp parallel shared(a,b,n) private(tid)
	{
		// tid = omp_get_thread_num();
		// fprintf(stdout,"Hola, soy el hilo %d\n",tid);
		#pragma omp sections 
		{
			#pragma omp section
			{
				tid = omp_get_thread_num();
				fprintf(stdout,"Hola, soy el hilo %d\n",tid);
				rectangleMethod(b,a,n);
			}
			#pragma omp section
			{
				tid = omp_get_thread_num();
				fprintf(stdout,"Hola, soy el hilo %d\n",tid);
				midPointRule(b,a,n);
			}
			#pragma omp section
			{	tid = omp_get_thread_num() ;	
				fprintf(stdout,"Hola, soy el hilo %d\n",tid);
				trapezoidalMethod(b,a,n);
			}	
			#pragma omp section
			{
				tid = omp_get_thread_num();
				fprintf(stdout,"Hola, soy el hilo %d\n",tid);
				simpsonRuleMethod(b,a,n);
			}
		}
	}

	gettimeofday(&end, NULL);
	clock_gettime(CLOCK_MONOTONIC,&finish1);
	//Calculo de tiempo por GetTimeOfDay
	elapsed = (end.tv_sec - start.tv_sec) - (start.tv_usec- end.tv_usec)/1E6;
	elapsed = (end.tv_sec+1E-6*end.tv_usec) - (start.tv_sec+1E-6*start.tv_usec);

	//Calculo de tiempo por Clock
	elapsed1 = (finish1.tv_sec - start1.tv_sec);
	elapsed1 += (finish1.tv_nsec - start1.tv_nsec)/1000000000.0;

	fprintf(stdout,"Tiempo de ejecucion medido por GetTimeOfDay: %lf\n",elapsed);
	fprintf(stdout,"Tiempo de ejecucion medido por Clock: %lf\n",elapsed1);
	return 0;
}

void rectangleMethod(double uplimit,double lowlimit,int interval){
	double w,sum,x;
	int z;
	w = (uplimit-lowlimit)/interval;
	sum = 0.0;
	for(z = 1;z <= interval;z++){
		x = lowlimit + (z-1)*w;
		sum = sum + (w * f(x)); // f(x) = x --> (w * <x>)
	}

	fprintf(stdout,"Resultado metodo del Rectangulo: %.10f\n",sum);

}


void midPointRule(double uplimit,double lowlimit,int interval){
	double w,sum;
	sum = 0.0;
	w = (uplimit-lowlimit)/interval;
	for (float i = lowlimit + (w/2); i < uplimit; i=i+w){
		//fprintf(stdout,"%.2f\n",i);
		sum = sum + f(i);
	}
	sum = w * sum;
	fprintf(stdout,"Resultado Metodo del Punto Medio: %.10f\n",sum);
	
}

void trapezoidalMethod(double uplimit,double lowlimit,int interval){
	double w,sum,x;
	int z;
	w = (uplimit-lowlimit)/interval;
	sum = 0.0;
	for(z = 1;z <= interval;z++){
		x = lowlimit + z*w;
		sum = sum + 2 * f(x);
	}

	sum = sum * w/2;

	fprintf(stdout,"Resultado metodo trapezoidal: %.10f\n", sum);
}

void simpsonRuleMethod(double uplimit,double lowlimit,int interval){
	int i;
	double w, x, sum = 0, integral;
	if (interval % 2 != 0){

		fprintf(stdout,"Para resolver mediante la regla de Simpson\nEl intervalo tiene que ser un numero par\n");
		return;
	}
	w = fabs(uplimit - lowlimit) / interval;
	for (i = 1; i < interval; i++){
		x = lowlimit + i * w;
		if (i % 2 == 0){
			sum = sum + 2 * f(x);
		}
		else{
			sum = sum + 4 * f(x);
		}
	}
	integral = (w / 3) * (f(lowlimit) + f(uplimit) + sum);
	printf("Resultado de Simpson 1/3 es: %10f\n", integral);
}