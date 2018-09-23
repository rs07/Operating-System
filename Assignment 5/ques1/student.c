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
#define MAX 10

int shmid, *sh, semid;

#define NO_SEM  1

#define P(s) semop(s, &Pop, 1);
#define V(s) semop(s, &Vop, 1);


struct sembuf Pop;
struct sembuf Vop;



int main()
{
	key_t keyval, mykey;
	//key_t ftok(const char *pathname, int proj_id);
	keyval= ftok("/tmp",55);
	if(keyval == -1) {
      printf("ftok() call failed\n");
      exit(0);
    }
    else
    {
      printf("keyval value = %d\n",keyval);
    }

    // int shmget(key_t key, size_t size, int shmflg);
    shmid = shmget(keyval,sizeof(int)*2,IPC_CREAT | 0777);
    if (shmid == -1) 
    { 	/* shmget() failed() */
        perror("shmget() failed: ");
        exit (1);
    }
    sh= (int *)shmat(shmid,NULL,0);

    union semun {
        int              val;    /* Value for SETVAL */
        struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
        unsigned short  *array;  /* Array for GETALL, SETALL */
        struct seminfo  *__buf;  /* Buffer for IPC_INFO (Linux-specific) */
    } setvalArg;

    setvalArg.val = 1;

    /* struct sembuf has the following fields */
    //unsigned short sem_num;  /* semaphore number */
    //short          sem_op;   /* semaphore operation */
    //short          sem_flg;  /* operation flags */

    Pop.sem_num = 0;
    Pop.sem_op = -1;
    Pop.sem_flg = SEM_UNDO;

    Vop.sem_num = 0;
    Vop.sem_op = 1;
    Vop.sem_flg = SEM_UNDO;

    mykey= ftok("/tmp",55);

    if (mykey == -1) {
        perror("ftok() failed");
        exit(1);
    }


    //       int semget(key_t key, int nsems, int semflg);
    semid = semget(mykey, NO_SEM, IPC_CREAT | 0777);
    if(semid == -1) {
        perror("semget() failed");
        exit(1);
    }

    // int semctl(int semid, int semnum, int cmd, ...);
    /*int status = semctl(semid, 0, SETVAL, setvalArg);
    if(status == -1) {
        perror("semctl() failed");
        exit(1);
    }*/





    int roll;
    pid_t pid;
    pid= getpid();
    printf("Enter roll Number: ");
    scanf("%d",&roll);
    //while(1)
    {
        if(*sh==-1)
        {
            P(semid);
            *sh=roll;
            *(sh+1)= pid;
            V(semid);
            //break;
        }
    }
    shmdt(sh);
    return 0;
}