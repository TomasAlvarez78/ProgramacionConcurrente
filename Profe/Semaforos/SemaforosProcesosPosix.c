#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>

int ProcesoPadre      (sem_t *s);
int ProcesoHijo       (sem_t *s, int nro);

int main (int argc, char *argv[]) {
  int error=0, idx=0, childError=0, pid[4];
  sem_t *binario;

  srand(time(NULL));

  binario = sem_open("/semBinario", O_CREAT, O_RDWR, 1);
  if (binario == SEM_FAILED) {
    perror("sem_open()");
    error = -1;
  }

  if (!error) {
    printf("Semaforo creado!\n");

    while ((!error) && (idx < 4)) {
      pid[idx] = fork();
      if (pid[idx] < 0) {
	perror("fork()");
	error = pid[idx];
      }
      else if (pid[idx] != 0) {
	idx++;
      }
      else {
	error=ProcesoHijo(binario, idx);
	idx=10;
      }
    }

    if ((!error) && (idx == 4)) {
      error = ProcesoPadre(binario);
      idx = 0;
      while (idx < 4) {
	pid[idx] = wait(&childError);
	if (childError) {
	  perror("wait()");
	  error = childError;
	}
	idx++;
      }
    }
  }

  if (idx == 4) {
    if (!error) {
      error=sem_close(binario);
      if (error) {
	perror("sem_close()");
      }
      else {
	printf("Semaforo cerrado\n");
      }
    }
    
    if (!error) {
      error = sem_unlink("/semBinario");
      if (error) {
	perror("sem_unlink()");
      }
      else {
	printf("Semaforo borrado!\n");
      }
    }
  }

  return error;
}



int ProcesoPadre (sem_t *s) {
  int error=0, count=0, estado=0;

  while ((count < 3) && (!error)) {
    usleep(800000);
    error = sem_getvalue(s, &estado);
    if (error) {
      perror("sem_getvalue");
    }
    else {
      printf("El semaforo tiene valor %d\n", estado);
      if (estado) {
	count++;
      }
      else {
	count = 0;
      }
    }
  }

  return error;
}

int ProcesoHijo (sem_t *s, int nro) {
  int error=0;

  printf("Hijo nro %d listo [%d]\n", nro, getpid());

  error = sem_wait(s);
  if (error) {
    perror("sem_wait()");
  }
  if (!error) {
    printf("El hijo nro %d tiene el recurso\n", nro);
    usleep(random() % 5000000);
    error = sem_post(s);
    if (error) {
      perror("sem_post()");
    }
  }

  if (!error) {
    usleep(random() % 10000000);
    error = sem_wait(s);
    if (error) {
      perror("sem_wait()");
    }
  }
  if (!error) {
    printf("El hijo nro %d tiene el recurso\n", nro);
    usleep(random() % 5000000);
    error = sem_post(s);
    if (error) {
      perror("sem_post()");
    }
  }

  return error;
}



