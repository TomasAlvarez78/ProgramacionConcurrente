#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/mman.h>

int ProcesoPadre      (sem_t *s, int mem);
int ProcesoHijo       (sem_t *s, int mem, int nro);

int main (int argc, char *argv[]) {
  int error=0, idx=0, childError=0, pid[4];
  int memoria=0;
  sem_t *binario;

  srand(time(NULL));

  binario = sem_open("/semBinario", O_CREAT, O_RDWR, 0);
  if (binario == SEM_FAILED) {
    perror("sem_open()");
    error = -1;
  }

  if (!error) {
    printf("Semaforo creado!\n");

    memoria = shm_open("/memCompartida", O_CREAT | O_RDWR, 0660);
    if (memoria < 0) {
      perror("shm_open()");
      error = -1;
    }
  }


  if (!error) {
    printf("Descriptor de memoria creado!\n");
    error = ftruncate(memoria, sizeof(int));
    if (error)
      perror("ftruncate()");
  }

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
      error=ProcesoHijo(binario, memoria, idx);
      idx=10;
    }
  }
 
  if ((!error) && (idx == 4)) {
    error = ProcesoPadre(binario, memoria);
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

    if (memoria > 0) {
      error = shm_unlink("/memCompartida");
      if (error) {
	perror("unlink()");
      }
      else {
	printf("Descriptor de memoria borrado!\n");
      }
    }
  }

  return error;
}



int ProcesoPadre (sem_t *s, int mem) {
  int error=0, count=0, estado=0;
  int *datos=NULL;

  datos = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, mem, 0);

  if (datos == MAP_FAILED) {
    perror("mmap()");
    error = -1;
  }

  if (!error) {
    printf("Padre Listo!\n");
    error = sem_post(s);
    if (error)
      perror("sem_post()");
  }

  while ((count < 3) && (!error)) {
    usleep(800000);
    error = sem_getvalue(s, &estado);
    if (error) {
      perror("sem_getvalue");
    }
    else {
    fflush(NULL);
      printf("El semaforo tiene valor %d | memoria = %d\n", estado, *datos);
      if (estado) {
	count++;
      }
      else {
	count = 0;
      }
    }
  }

  if (datos != NULL) {
    error = munmap((void*)(datos), 2 * sizeof(int));
    if (error) {
      perror("munmap()");
    }
  }

  return error;
}

int ProcesoHijo (sem_t *s, int mem, int nro) {
  int error=0;
  int *datos=NULL;

  datos = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, mem, 0);
  if (datos == MAP_FAILED) {
    perror("mmap()");
    error = -1;
  }

  if (!error) {
    printf("Hijo nro %d listo [%d]\n", nro, getpid());

    error = sem_wait(s);
    if (error) {
      perror("sem_wait()");
    }
    if (!error) {
      printf("El hijo nro %d tiene el recurso\n", nro);
      *datos = nro;
      usleep(random() % 5000000);
      error = sem_post(s);
      if (error) {
	perror("sem_post()");
      }
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
    *datos = nro;
    usleep(random() % 5000000);
    error = sem_post(s);
    if (error) {
      perror("sem_post()");
    }
  }

  if (datos != NULL) {
    error = munmap((void*)(datos), 2 * sizeof(int));
    if (error) {
      perror("munmap()");
    }
  }

  return error;
}



