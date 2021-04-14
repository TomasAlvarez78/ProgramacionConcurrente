#include<unistd.h>
#include<stdio.h>
#include<signal.h>
#include<sys/types.h>

void DetenerseKyb (int sig_num);
void Reanudar     (int sig_num);
void Terminar     (int sig_num);

int main (int argc, char *argv[]) {
  int error=0;

  signal(SIGTSTP, DetenerseKyb);
  signal(SIGCONT, Reanudar);
  signal(SIGTERM, Terminar);

  printf("El proceso %d espera una signal para detenerse o terminar\n",
	 getpid());
  pause();
  pause();

  return error;
}

void DetenerseKyb (int sig_num) {
  printf("Signal %2d: Proceso detenido (keyboard)\n",sig_num);
}

void Reanudar     (int sig_num) {
  printf("Signal %2d: Reanudar actividad\n",sig_num);
}

void Terminar     (int sig_num) {
  printf("Signal %2d: Terminando\n",sig_num);
}



