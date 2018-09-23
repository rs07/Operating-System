/******************************************************************************************************************************************************


                                                   &*&@@@@&*&                          %%%%%%%%%%%%%%%%%%%%%%
                                                  &*&      &*&                         %%%%%%%%%%%%%%%%%%%%%
                                                &*&         &*&                        @@
                                              &*&            &*&                       @@
                                             &*&              &*&                      @@
                                            &*&                &*&                     @@
                                           &*&                 &*&                     @@
                                           **                   **                     %%%%%%%%%%%%%%%%%%%%%
                                          &*&                   &*&                     %%%%%%%%%%%%%%%%%%%%%
                                          &*&                   &*&                                        @@
                                           **                   **                                         @@
                                          &*&                   &*&                                        @@
                                          &*&                   &*&                                        @@
                                           &*&                 &*&                                         @@
                                            &*&               &*&                                          @@
                                              &*&            &*&                       %%%%%%%%%%%%%%%%%%%%%%
                                                &*&         &*&                         %%%%%%%%%%%%%%%%%%%%%
                                                 &*&@@@@@@&*&                                                


Author: Rishabh Kumar Sharma
Tested By: Rishabh Kumar Sharma
Last Updated:  19th February, 2018, 21:04 Hrs.

*/

// #include<stdio.h>
// #include<unistd.h>	 		/* for fork() */
// #include<sys/wait.h> 		/* for wait() */
// #include<sys/types.h> 		/* for wait() kill(2)  ftok() */
// #include<sys/ipc.h> 		/* for shmget() shmctl() ftok() */
// #include<sys/shm.h> 		/* for shmget() shmctl() */
// #include<signal.h> 			/* for signal(2) kill(2) */
// #include<errno.h> 			/* for perror */
// #include<stdlib.h>			/* for exit(0) */
// #include <sys/sem.h>        /* for semget(2) semop(2) semctl(2) */

// #define KEY 100000
// #define KEY_SEM 500000
// #define MAX 10000

#include"/home/hercules/Desktop/OS Lab/Assignment 5/ques2/ques2.h"

int shmid_glob, semid_glob;

// #define NO_SEM  1

// #define P(s) semop(s, &Pop, 1);
// #define V(s) semop(s, &Vop, 1);


struct sembuf Pop;
struct sembuf Vop;




void semaphore(int semid)
{
    union semun {
        int              val;    /* Value for SETVAL */
        struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
        unsigned short  *array;  /* Array for GETALL, SETALL */
        struct seminfo  *__buf;  /* Buffer for IPC_INFO (Linux-specific) */
    } setvalArg;

    setvalArg.val = 1;


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



int create_lnode(int shmid, int semid)
{
    //printf("BIG ERRRRRRRRRRRRRRRROOOOORRRRRRRRRRRRRRRRRR  WAAAAAARRRRRRRRRRNNNNNNNNNNIIIIIIIIINNNNNNNNNNGGGGGGGG\n");
	int *p;
    p= (int *)shmat(shmid,NULL,0);												/* For setting the condition for First Node */
    *(p)=-1;
    *(p+1)=-1;
    *(p+2)=1;
    semaphore(semid);
    shmdt(p);
}


int traverse()
{
    int semid,shmid;
    semid= semid_glob;
    shmid= shmid_glob;
    P(semid);
	int *p= shmat(shmid,NULL,0);
	int tr_id= *(p+1);
	int count=0;
    if(*(p+1)==-1)
    {
        printf("List is Empty\n");
        shmdt(p);
        V(semid);
        return 0;
    }
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
	printf("No. of nodes in present LL: %d\n",count);
    V(semid);
}




int add(int x)
{
    // int val= semctl(semid_glob,0,GETVAL);
    // printf("semaphore value: %d\n",val);
    int semid,shmid;
    semid= semid_glob;
    shmid= shmid_glob;
    P(semid);
	int shmid_add;
	shmid_add= shmget(IPC_PRIVATE,sizeof(int)*2,IPC_CREAT | 0777);
    // val= semctl(semid_glob,0,GETVAL);
    // printf("Next semaphore value: %d\n",val);
    // fflush(stdin);
    // int ca;
    // printf("Press Any Key to continue...\n");
    // scanf("%d",&ca);
    // getchar();
	int *p;
	p= (int *)shmat(shmid,NULL,0);
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
	//traverse(shmid,semid);

}

int del(int x)
{
    int semid,shmid;
    semid= semid_glob;
    shmid= shmid_glob;
    P(semid);
	int *p= shmat(shmid,NULL,0);
    //fflush(stdin);
    //printf("Press Any Key to continue...\n");
    //printf("%d ------------ %d\n",*(p),*(p+1));
    //getchar();
	if((*p)==-1)
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
					shmctl(cache,IPC_RMID,NULL);
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





int initialize()
{

    // int shmget(key_t key, size_t size, int shmflg);
    int counter= shmget(10000,sizeof(int),IPC_CREAT | 0777);
    int key_cache;
    if (counter == -1) 
    { 	/* shmget() failed() */
        perror("shmget() counter failed: ");
        exit (1);
   	}
   	int *p= shmat(counter,NULL,0);
   	if(*p<0)
   		*(p)=0;
   	printf("starting value: %d\n",*p);
   	int ch, value, shmid_cache,semid_cache;

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




    int cs=(*p);
    if(*p==0)
    {
    	//printf("Hi\n");
        key_cache= KEY+cs;
    	shmid_glob= shmget(KEY+cs,sizeof(int)*3, IPC_CREAT | 0777);
        semid_glob= semget(KEY+cs, NO_SEM, IPC_CREAT | 0777);
    	if (shmid_glob == -1) 
    	{ 	/* shmget() failed() */
      		perror("shmget() failed: ");
        	exit (1);
    	}
        if(semid_glob == -1) 
        {
            perror("semget() failed");
            exit(1);
        }
    	create_lnode(shmid_glob,semid_glob);
   		(*p)++;
   		cs= *p;
   	}
   	else
   	{
        //printf("Tech\n");
        key_cache= KEY+cs-1;
   		shmid_glob= shmget(KEY+(cs-1),sizeof(int)*3, IPC_CREAT | 0777);
        semid_glob= semget(KEY+(cs-1), NO_SEM, IPC_CREAT | 0777);
    	if (shmid_glob == -1) 
    	{ 	/* shmget() failed() */
      		perror("shmget() failed: ");
        	exit (1);
    	}
        if(semid_glob == -1) 
        {
            perror("semget() failed");
            exit(1);
        }
   	}
    while(1)
    {
    	if(cs>=0)
    		printf("Current Linked List Number: %d\n",cs);
    	printf("1. For Addition\n");
    	printf("2. For Deletion\n");
    	printf("3. For Showing Data\n");
    	printf("4. Creating a new node\n");
    	printf("5. Switch Linked List\n");
        // int val= semctl(semid_glob,0,GETVAL);
        // printf("semaphore value: %d\n",val);
    	scanf("%d",&ch);
    	switch(ch)
    	{
    		case 1: //P(semid_glob);
                    printf("Enter value to be Inserted: \n");
    				scanf("%d",&value);
    				add(value);
                    //V(semid_glob);
    				break;
    		case 2: printf("Enter the value to be Deleted: \n");
    				scanf("%d",&value);
    				del(value);
    				break;
    		case 3: traverse();
    				break;
    		case 4: shmid_cache;
                    key_cache= KEY+(*p);
    				shmid_cache = shmget(KEY+(*p),sizeof(int)*3,IPC_CREAT | 0777);                  /* First for storing the current SHMID and starting SHMID  */
    				semid_cache= semget(KEY+(*p), NO_SEM, IPC_CREAT | 0777);
                    if (shmid_cache == -1) 
    				{ 	/* shmget() failed() */
        				perror("shmget() failed: ");
        				exit (1);
    				}
                    if(semid_cache == -1) 
                    {
                        perror("semget() failed");
                        exit(1);
                    }
    				create_lnode(shmid_cache,semid_cache);
    				cs=*p;
    				(*p)++;
    				printf("LL number: %d\n",*p);
    				break;
    		case 5: printf("Enter the value of Linked List to be switched : \n");
    				scanf("%d",&value);
    				if(value-1>=*p)
    				{
    					fprintf(stderr, "Error: Value out of bound\n");
    				}
    				else
    				{
    					cs= value-1;
    					int shmid_ids;
    					//printf("Foo\n");
                        key_cache= KEY+cs;
    					shmid_ids= shmget(KEY+(cs),sizeof(int)*3, IPC_CREAT | 0777);
                        semid_glob= semget(KEY+(cs), NO_SEM, IPC_CREAT | 0777);
    					//printf("Pook\n");
    					if (shmid_ids == -1) 
    					{	 	/* shmget() failed() */
        					perror("shmget() switching failed: ");
        					exit (1);
    					}
    					else
    					{
    						printf("Shmid Uploaded\n");
    					}
                        if(semid_glob == -1) 
                        {
                            perror("semget() failed");
                            exit(1);
                        }
    					shmid_glob= shmid_ids;
    				}
    				break;
    		default: exit(0);
    	}
    }
    //int value;
	//add(value);
    shmdt(p);
	return 0;
}