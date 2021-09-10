#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

void* roll_dice(){
    int x = (rand() % 6)+1;
    void *res = malloc(sizeof(int));
    *res = x;
    //printf("Numero aleatorio: %d\n",x);
    printf("Memoria funcion: %p\n",res);
    return (void*)res;
}


int main(int argc,char* argv[]){
    pthread_t th;
    int* res;
    srand(time(NULL));
    pthread_create(&th,NULL,&roll_dice,NULL);
    pthread_join(th,(void **) &res);
    printf("Memoria main: %p\n",res);
    printf("Numero aleatorio: %d\n",*res);
    return 0;
}