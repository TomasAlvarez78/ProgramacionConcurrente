#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>
#include <stddef.h>
#include <time.h>
#include <sys/sysinfo.h>

#define f(x) x

double midPointRule(double uplimit,double lowlimit,int interval);
double rectangleMethod(double uplimit,double lowlimit,int interval);
double trapezoidalMethod(double uplimit,double lowlimit,int interval);
double simpsonRuleMethod(double uplimit,double lowlimit,int interval);


int main(int argc, char * argv[]){
	int rank = 0,size = 0,color;
	double a,b,sumRectangulo,sumTotalRect,sumMidPoint,sumTotalMid,sumTrap,sumTotalTrap,sumSimp,sumTotalSimp;
	sumRectangulo = 0,sumMidPoint = 0,sumTotalMid = 0,sumTrap = 0,sumTotalTrap = 0,sumSimp = 0,sumTotalSimp = 0,sumTotalRect = 0;
	int n;
	a = 1;
	b = 2;
	n = 10;

	MPI_Comm row_comm;

	struct timeval start, end;
	double elapsed;
	struct timespec start1, finish1;
	double elapsed1;

	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  	MPI_Comm_size(MPI_COMM_WORLD,&size);

	// Get the group of processes in MPI_COMM_WORLD
	MPI_Group world_group;
	MPI_Comm_group(MPI_COMM_WORLD, &world_group);

	const int grupo1[4] = {0, 1, 2, 3};
	const int grupo2[4] = {4, 5, 6, 7};
	const int grupo3[4] = {8, 9, 10, 11};
	const int grupo4[4] = {12, 13, 14, 15};

	// Construct a group containing all of the prime ranks in world_group
	MPI_Group group1,group2,group3,group4;
	MPI_Group_incl(world_group, 1, grupo1, &group1);
	MPI_Group_incl(world_group, 2, grupo2, &group2);
	MPI_Group_incl(world_group, 3, grupo3, &group3);
	MPI_Group_incl(world_group, 4, grupo4, &group4);

	MPI_Comm group1_comm,group2_comm,group3_comm,group4_comm;
	MPI_Comm_create_group(MPI_COMM_WORLD, group1, 0, &group1_comm);
	MPI_Comm_create_group(MPI_COMM_WORLD, group2, 0, &group2_comm);
	MPI_Comm_create_group(MPI_COMM_WORLD, group3, 0, &group3_comm);
	MPI_Comm_create_group(MPI_COMM_WORLD, group4, 0, &group4_comm);

	if(rank == 0){
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

		gettimeofday(&start, NULL);
		clock_gettime(CLOCK_MONOTONIC,&start1);
	}
	MPI_Bcast(&a,1,MPI_DOUBLE,0,MPI_COMM_WORLD);
	MPI_Bcast(&b,1,MPI_DOUBLE,0,MPI_COMM_WORLD);
	MPI_Bcast(&n,1,MPI_INTEGER,0,MPI_COMM_WORLD);
	
	if(rank < 4){
		// 0 1 2 3
		sumRectangulo = rectangleMethod(b,a,n);
	}else if (rank < 8){
		// 4 5 6 7
		sumMidPoint = midPointRule(b,a,n);
	}else if (rank < 12){
		// 8 9 10 11
		sumTrap = trapezoidalMethod(b,a,n);
	}else if (rank < 16){
		// 12 13 14 15
		sumSimp = simpsonRuleMethod(b,a,n);
	}


	MPI_Reduce(&sumRectangulo,&sumTotalRect,1,MPI_DOUBLE,MPI_SUM,0,group1_comm);
	MPI_Reduce(&sumMidPoint,&sumTotalMid,1,MPI_DOUBLE,MPI_SUM,4,group2_comm);
	MPI_Reduce(&sumTrap,&sumTotalTrap,1,MPI_DOUBLE,MPI_SUM,8,group3_comm);
	MPI_Reduce(&sumSimp,&sumTotalSimp,1,MPI_DOUBLE,MPI_SUM,12,group4_comm);
	
	if(rank == 0){
		fprintf(stdout,"Resultado total metodo del rectangulo: %.10f\n",sumTotalRect);
	}
	if(rank == 4){
		fprintf(stdout,"Resultado total metodo del punto medio %.10f\n",sumTotalMid);
	}
	if(rank == 8){
		fprintf(stdout,"Resultado total metodo trapezoidal %.10f\n",sumTotalTrap);
	}
	if(rank == 12){
		fprintf(stdout,"Resultado total metodo de Simpson %.10f\n",sumTotalSimp);
	}
 	MPI_Finalize();

	if(rank == 0){
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
	}

	MPI_Group_free(&world_group);
	MPI_Group_free(&group1);
	MPI_Group_free(&group2);
	MPI_Group_free(&group3);
	MPI_Group_free(&group4);
	MPI_Comm_free(&group1_comm);
	MPI_Comm_free(&group2_comm);
	MPI_Comm_free(&group3_comm);
	MPI_Comm_free(&group4_comm);
}

double rectangleMethod(double uplimit,double lowlimit,int interval){
	int rank = 0,size = 0;
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  	MPI_Comm_size(MPI_COMM_WORLD,&size);
	
	int steps = interval / 4;	
	int localRank = rank % 4;
	int start = localRank * steps;
	int end = start + steps;

	if((interval % 4) > 0 && localRank == 3){
		int sobrante = interval % 4;
		end = end + sobrante;
	}

	double w,x,sum;
	int z;
	sum = 0.0;
	w = (uplimit-lowlimit)/interval;
	for(z = start;z < end;z++){
		x = lowlimit + (z)*w;
		sum = sum + (w * f(x)); // f(x) = x --> (w * <x>)
	}
	// fprintf(stdout,"Resultado del proceso %d (global %d) en la funcion RectangleMethod: %lf\n",localRank,rank,sum);
	return sum;
}

double midPointRule(double uplimit,double lowlimit,int interval){
	int rank = 0,size = 0;
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  	MPI_Comm_size(MPI_COMM_WORLD,&size);
	
	int steps = interval / 4;	// 3
	int localRank = rank % 4; // 0 1 2 3
	int start = localRank * steps; 
	int end = start + steps;

	if((interval % 4) > 0 && localRank == 3){
		int sobrante = interval % 4;
		end = end + sobrante;
	}

	int i;
	double w,sum,z;
	sum = 0.0;
	w = (uplimit-lowlimit)/interval;

	for (i = start; i < end; i++){
		z = lowlimit + (w/2) + i*w;
		sum = sum + f(z);
	}
	sum = w * sum;
	// fprintf(stdout,"Resultado del proceso %d (global %d) en la funcion midPointRule: %lf\n",localRank,rank,sum);
	return sum;

}

double trapezoidalMethod(double uplimit,double lowlimit,int interval){

	int rank = 0,size = 0;
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  	MPI_Comm_size(MPI_COMM_WORLD,&size);
	
	int steps = interval / 4;
	int localRank = rank % 4;
	int start = localRank * steps; 
	int end = start + steps;

	if((interval % 4) > 0 && localRank == 3){
		int sobrante = interval % 4;
		end = end + sobrante;
	}

	double w,sum,x;
	int z;
	w = (uplimit-lowlimit)/interval;
	sum = 0.0;
	for(z = start;z < end;z++){
		x = lowlimit + z*w;
		sum = sum + 2 * f(x);
	}
	sum = sum * w/2;
	// fprintf(stdout,"Resultado del proceso %d (global %d) en la funcion trapezoidalMethod: %lf\n",localRank,rank,sum);
	return sum;
}


double simpsonRuleMethod(double uplimit,double lowlimit,int interval){
	int rank = 0,size = 0;
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  	MPI_Comm_size(MPI_COMM_WORLD,&size);
	
	int steps = interval / 4;
	int localRank = rank % 4;
	int start = localRank * steps; 
	int end = start + steps;

	if((interval % 4) > 0 && localRank == 3){
		int sobrante = interval % 4;
		end = end + sobrante;
	}
	
	int i;
	double w, x, sum = 0, integral=0;
	if (interval % 2 != 0){

		fprintf(stdout,"Para resolver mediante la regla de Simpson\nEl intervalo tiene que ser un numero par\n");
		return 0;
	}
	w = (uplimit - lowlimit) / interval;
	for (i = start + 1; i < end; i++){
		x = lowlimit + i * w;
		if (i % 2 == 0){
			sum = sum + 2 * f(x);
		}
		else{
			sum = sum + 4 * f(x);
		}
	}
	integral = (w / 3) * (f(lowlimit) + f(uplimit) + sum);
	// fprintf(stdout,"Resultado del proceso %d (global %d) en la funcion simpsonRuleMethod: %lf\n",localRank,rank,integral);
	return integral;
}
