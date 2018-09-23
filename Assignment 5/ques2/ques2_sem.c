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

int shmid_glob,shmid_add,shmid3,count=0,semid;



#define NO_SEM  1

#define P(s) semop(s, &Pop, 1);
#define V(s) semop(s, &Vop, 1);


struct sembuf Pop;
struct sembuf Vop;




void semaphore()
{
    union semun {
        int              val;    /* Value for SETVAL */
        struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
        unsigned short  *array;  /* Array for GETALL, SETALL */
        struct seminfo  *__buf;  /* Buffer for IPC_INFO (Linux-specific) */
    } setvalArg;

    setvalArg.val = 1;

    

    /*mykey= ftok("/tmp",55);

    if (mykey == -1) {
        perror("ftok() failed");
        exit(1);
    }*/


    //       int semget(key_t key, int nsems, int semflg);
    /*semid = semget(mykey, NO_SEM, IPC_CREAT | 0777);
    if(semid == -1) {
        perror("semget() failed");
        exit(1);
    }*/

    // int semctl(int semid, int semnum, int cmd, ...);
    int status = semctl(semid, 0, SETVAL, setvalArg);
    if(status == -1) {
        perror("semctl() failed");
        exit(1);
    }
    else
    {
        printf("semaphore is set\n");
    }
}



int create_lnode()
{
	int *p;
    p= (int *)shmat(shmid_glob,NULL,0);												/* For setting the condition for First Node */
    *(p)=-1;
    *(p+1)=-1;
    *(p+2)=1;
    semaphore();
    shmdt(p);
}


int traverse(int shmid)
{
    P(semid);
	int *p= shmat(shmid,NULL,0);
	int tr_id= *(p+1);
	count=0;
	while(1)
	{
		int *t= shmat(tr_id,NULL,0);
		printf("Value: %d\n",*t);
		count++;
		if(*(t+1)==-1)
		{
			shmdt(t);
			break;
		}
		tr_id=*(t+1);
		shmdt(t);	
	}
	shmdt(p);
	printf("%d\n",count);
    V(semid);
}




int add(int x)
{
	P(semid);
	shmid_add= shmget(IPC_PRIVATE,sizeof(int)*2,IPC_CREAT | 0777);
	int *p;
	p= (int *)shmat(shmid_glob,NULL,0);
	int *a;
	a= (int *)shmat(shmid_add,NULL,0);
	if(*p==-1)
	{
		*a=x;
		*p=shmid_add;
		*(p+1)=shmid_add;
	}
	else if(*p!=-1)
	{
		*a=x;
		int *t;
		t= (int *)shmat(*p,NULL,0);
		*(t+1)=shmid_add;
		shmdt(t);
		*(p)=shmid_add;
	}
	*(a+1)=-1;
	shmdt(a);
	shmdt(p);
	V(semid);
	traverse(shmid_glob);
}

int del(int x)
{
    P(semid);
	int *p= shmat(shmid_glob,NULL,0);
	if(*p==-1)
	{
		fprintf(stderr, "Error: The List is empty\n");
	}
	else
	{
		int c=1;
		int *a= shmat(*(p+1),NULL,0);
		if(*(a+0)==x)
		{
			int cache= *(p+1);
			*(p+1)=*(a+1);
            if(*(p+1)==-1)
            {
                *(p)=-1;
            }
			int status = shmctl(cache, IPC_RMID, NULL); /* IPC_RMID is the command for destroying the shared memory*/
    		if (status == 0) 
    		{
        		fprintf(stderr, "Remove shared memory with id = %d.\n", cache);
    		}
    		else if (status == -1) 
    		{
        		fprintf(stderr, "Cannot remove shared memory with id = %d.\n", cache);
    		} 
    		else 
    		{
        		fprintf(stderr, "shmctl() returned wrong value while removing shared memory with id = %d.\n", cache);
    		}
		}
		else
		{	shmdt(a);
			int shmid_trav=*(p+1);
			int i=0;
			while(1)
			{
				a= shmat(shmid_trav,NULL,0);
				int *curr=shmat(*(a+1),NULL,0);
				if(*(curr+0)==x)
				{
					int cache= *(a+1);
					*(a+1)= *(curr+1);
					shmdt(curr);
					semctl(cache,IPC_RMID,NULL);
					break;
				}
				else
				{
					int flag=0;
					if(*(curr+1)==-1)
					{
						flag=1;
					}
					shmdt(curr);
					shmid_trav=*(a+1);
					shmdt(a);
					if(flag)
						break;
				}
			}
		}
	}
    V(semid);
}




int main()
{


    // int shmget(key_t key, size_t size, int shmflg);
    shmid_glob = shmget(100000,sizeof(int)*3,IPC_CREAT | 0777);                     /* First for storing the current SHMID and starting SHMID  */
    if (shmid_glob == -1) 
    { 	/* shmget() failed() */
        perror("shmget() failed: ");
        exit (1);
    }
    semid= semget(100000, NO_SEM, IPC_CREAT | 0777);
    if(semid==-1)
    {
    	perror("shmget failed");
    }

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


    printf("Shmid: %d\n",shmid_glob);
    int *p= shmat(shmid_glob,NULL,0);
    //printf("1> %d  2> %d  3>%d\n",*p,*(p+1),*(p+2));
    int ch, value,val;
    if(*(p+2)!=1)
    	create_lnode(shmid_glob);
    //printf("1> %d  2> %d  3>%d\n",*p,*(p+1),*(p+2));
	shmdt(p);
    while(1)
    {
    	printf("1. For Addition\n");
    	printf("2. For Deletion\n");
    	printf("3. For Showing Data\n");
    	scanf("%d",&ch);
    	switch(ch)
    	{
    		case 1: 
                    printf("Enter value to be Inserted: \n");
    				scanf("%d",&value);
    				add(value);
    				break;
    		case 2: printf("Enter the value to be Deleted: \n");
    				scanf("%d",&value);
    				del(value);
    				break;
    		case 3: traverse(shmid_glob);
    				break;
    		default: exit(0);
    	}
    }
    //int value;
	//add(value);

	return 0;
}