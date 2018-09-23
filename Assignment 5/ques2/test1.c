#include<stdio.h>
#include <sys/types.h> /* for semget(2) ftok(3) semop(2) semctl(2) */
#include <sys/ipc.h> /* for semget(2) ftok(3) semop(2) semctl(2) */
#include <sys/sem.h>        /* for semget(2) semop(2) semctl(2) */
#include<sys/shm.h> 		/* for shmget() shmctl() */

#define KEY_SEM 500000

#define NO_SEM  1

#define P(s) semop(s, &Pop, 1);
#define V(s) semop(s, &Vop, 1);


struct sembuf Pop;
struct sembuf Vop;


int main()
{
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
    Pop.sem_flg = 0;

    Vop.sem_num = 0;
    Vop.sem_op = 1;
    Vop.sem_flg = 0;
    int shmid= shmget(KEY_SEM,sizeof(int),IPC_CREAT | 0777);
    int *p= shmat(shmid,NULL,0);
    int semid= semget(KEY_SEM,NO_SEM,IPC_CREAT | 0777);
    if(semid == -1) 
    {
        perror("semget() failed");
        exit(1);
    }
    else
    {
    	printf("Semid: %d\n",semid);
    }

    if(*p!=1)
    {
    	int status = semctl(semid, 0, SETVAL, setvalArg);
    	printf("Hi\n");
    	if(status == -1) 
    	{
        	perror("semctl() failed");
        	exit(1);
    	}
    	else
    	{
        	printf("semaphore is set\n");
    	}
    	*p=1;
    }
    shmdt(p);
    //shmctl(shmid,IPC_RMID,NULL);
    int val;
    while(1)
    {
    	printf("Enter a number to enter in semaphore:");
    	scanf("%d",&val);
    	int value= semctl(semid,0,GETVAL);
        printf("semaphore value: %d\n",value);
    	P(semid);
    	printf("Value: %d\n",val);
    	value= semctl(semid,0,GETVAL);
        printf("semaphore value: %d\n",value);
    	scanf("%d",&val);
    	V(semid);
    	if(val==0)
    		exit(0);
    }
}