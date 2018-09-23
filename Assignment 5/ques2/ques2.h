#ifndef QUES2
    #define QUES2

	
#include<stdio.h>
#include<unistd.h>	 		/* for fork() */
#include<sys/wait.h> 		/* for wait() */
#include<sys/types.h> 		/* for wait() kill(2)  ftok() */
#include<sys/ipc.h> 		/* for shmget() shmctl() ftok() */
#include<sys/shm.h> 		/* for shmget() shmctl() */
#include<signal.h> 			/* for signal(2) kill(2) */
#include<errno.h> 			/* for perror */
#include<stdlib.h>			/* for exit(0) */
#include <sys/sem.h>        /* for semget(2) semop(2) semctl(2) */


#define KEY 100000
#define KEY_SEM 500000
#define MAX 10000


#define NO_SEM  1

#define P(s) semop(s, &Pop, 1);
#define V(s) semop(s, &Vop, 1);


struct sembuf Pop;
struct sembuf Vop;


void semaphore(int);
int create_lnode(int,int);
int traverse();
int add(int);
int del(int);
int initialize();

#endif