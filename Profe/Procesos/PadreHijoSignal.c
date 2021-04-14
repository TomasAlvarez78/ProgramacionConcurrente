#include<sys/types.h>
#include<sys/wait.h>
#include<stdio.h>
#include<unistd.h>
#include<signal.h>
#include<errno.h>

void Comenzar     (int sig_num);
void Terminar     (int sig_num);

void HacerNada    (int sig_num);

int  ProcesoPadre (int childPid);
void ProcesoHijo  (void);

int main (int argc, char *argv[]) {
  int error=0, pid=0;

  pid=fork();
  if (pid==0) {
    ProcesoHijo();
  }
  else if (pid>0) {
    error=ProcesoPadre(pid);
  }
  else {
    error=pid;
  }

  if (error) {
    perror(argv[0]);
  }

  return error;
}


void Comenzar  (int sig_num) {
  int salir=0;

  printf("Signal %d: Proceso hijo comienza actividad\n",sig_num);
  while (!salir) {
    printf(".");
    fflush(NULL);
    usleep(100000);
  }
}

void Terminar (int sig_num) {
  printf("\nSignal %d: El hijo termino sus actividades\n",sig_num);
  raise(SIGTERM);
}


void HacerNada (int sig_num) {
  printf("\nSignal %d: continuar\n",sig_num);
}


void ProcesoHijo  (void) {
  int ownPid=0, parentPid=0;

  signal(SIGUSR1, Comenzar);
  signal(SIGUSR2, Terminar);

  ownPid=getpid();
  parentPid=getppid();
  printf("\nMi PID es %d y soy hijo de %d\nEspero signal de mi padre\n",
	 ownPid,parentPid);
  pause();
}


int  ProcesoPadre (int childPid) {
  int error=0, ownPid=0, parentPid=0, pid=0;

  ownPid=getpid();
  printf("\nMi PID es %d y mi hijo es %d\n",ownPid,childPid);

  signal(SIGALRM, HacerNada);

  printf("En 2 seg. envio signal al mi hijo\n\n");
  alarm(2);
  pause();
  printf("Enviando signal. Proxima signal en 5 seg.\n\n");
  error=kill(childPid,SIGUSR1);
  if (!error) {
    signal(SIGALRM, HacerNada);
    alarm(5);
    pause();
    printf("\nEnviando senal\n");
    error=kill(childPid,SIGUSR2);
    if (!error) {
      pid=wait(NULL);
      if (childPid!=pid) {
	error=childPid;
      }
      printf("Terminado!\n");
    }
  }

  return error;
}
