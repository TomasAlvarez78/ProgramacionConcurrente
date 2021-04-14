#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

/* #define _XOPEN_SOURCE 500 */

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
    lock=open("archivoLock", 0660 | O_CREAT | O_EXCL);
    while (lock<0) { 
      usleep(300);
      lock=open("archivoLock", 0660 | O_CREAT | O_EXCL);
    }
    for (i=0;i<10;i++) {
      printf(".");
    }
    fflush(NULL);
    if (unlink("archivoLock")) {
      perror("Parent unlink()");
    }
    usleep(1250);
  }

  return error;
}


int StarProcess (void) {
  int error=0, salir=0, i=0, j=0, lock=0;

  for (j=0;j<100;j++) {
    lock=open("archivoLock", 0660 | O_CREAT | O_EXCL);
    while (lock<0) { 
      usleep(150);
      lock=open("archivoLock", 0660 | O_CREAT | O_EXCL);
    }
    for (i=0;i<10;i++) {
      printf("*");
    }
    fflush(NULL);
    if (unlink("archivoLock")) {
      perror("Child unlink()");
    }
    usleep(2500);
  }

  return error;
}



