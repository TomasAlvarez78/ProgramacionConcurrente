#include<stdio.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<errno.h>

int  ProcesoPadre (int childPid);
int  ProcesoHijo  (void);

int main (int argc, char *argv[]) {
  int error=0, pid=0;

  pid=fork();
  if (pid==0) {
    error=ProcesoHijo();
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



int  ProcesoHijo  (void) {
  int error=0, ownPid=0, parentPid=0;
  ownPid=getpid();
  parentPid=getppid();
  printf("\nMi PID es %d y soy hijo de %d\n",ownPid,parentPid);
  sleep(2);
  printf("El hijo termino sus actividades\n");
  return error;
}


int  ProcesoPadre (int childPid) {
  int error=0, ownPid=0, parentPid=0, pid=0;
  ownPid=getpid();
  parentPid=getppid();
  printf("\nMi PID es %d y mi hijo es %d\n",ownPid,pid);
  printf("Mi padre es %d ?Quien es?\n\n",parentPid);
  printf("Esperando que termine %d\n",childPid);
  pid=wait(NULL);
  if (childPid!=pid) {
    error=childPid;
  }
  printf("Terminado!\n");
  return error;
}
