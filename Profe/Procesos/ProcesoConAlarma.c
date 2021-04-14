#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

void TimeOut (int sig_num);

int main (int argc, char *argv[]) {
  int error=0, dormir=1;

  signal(SIGALRM, TimeOut);

  if (argc==2) {
    dormir=atoi(argv[1]);
  }
  else {
    fprintf(stderr,"Usage: %s seconds\n",argv[0]);
    error=1;
  }

  if (!error) {
    printf("Alarma establecida en %d seg.\n",dormir);
    alarm(dormir);
    pause();
    printf("Proceso terminado con exito\n");
  }

  return error;
}


void TimeOut (int sig_num) {
  printf("Tiempo fuera!\n");
}

