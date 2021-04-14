#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>

struct {
  pthread_mutex_t mutex;
  char symbol; } shared = { PTHREAD_MUTEX_INITIALIZER };

int DotProcess  (void);
int StarProcess (void);

int main (void) {
  int error=0, pid=0, childError=0, childPid=0;

  pid=fork();
  if (pid>0) {
    error=DotProcess();
    childPid=wait(&childError);
    printf("\n");
  }
  else if (pid==0) {
    error=StarProcess();
  }
  else {
    perror("fork()");
  }

  return error;
}

int DotProcess  (void) {
  int error=0, salir=0, i=0, j=0, lock=0;

  for (j=0;j<100;j++) {
    pthread_mutex_lock(&shared.mutex);
    for (i=0;i<10;i++) {
      printf(".");
    }
    fflush(NULL);
    pthread_mutex_unlock(&shared.mutex);
    usleep(1250);
  }

  return error;
}


int StarProcess (void) {
  int error=0, salir=0, i=0, j=0, lock=0;

  for (j=0;j<100;j++) {
    pthread_mutex_lock(&shared.mutex);
    for (i=0;i<10;i++) {
      printf("*");
    }
    fflush(NULL);
    pthread_mutex_unlock(&shared.mutex);
    usleep(5000);
  }

  return error;
}



