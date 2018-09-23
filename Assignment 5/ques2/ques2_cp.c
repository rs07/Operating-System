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

#include<ques2.h>

int shmid_glob,shmid_add,shmid3,count=0;


int create_lnode()
{
	int *p;
    p= (int *)shmat(shmid_glob,NULL,0);												/* For setting the condition for First Node */
    *(p)=-1;
    *(p+1)=-1;
    *(p+2)=1;
    shmdt(p);
}


int traverse(int shmid)
{
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
}




int add(int x)
{
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
	traverse(shmid_glob);
}

int del(int x)
{
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
}




int main()
{

	/*key_t keyval;
	keyval= ftok("/tmp",55);
	if(keyval == -1) {
      printf("ftok() call failed\n");
      exit(0);
    }
    else
    {
      printf("keyval value = %d\n",keyval);
    }*/

    // int shmget(key_t key, size_t size, int shmflg);
    shmid_glob = shmget(100000,sizeof(int)*3,IPC_CREAT | 0777);                     /* First for storing the current SHMID and starting SHMID  */
    if (shmid_glob == -1) 
    { 	/* shmget() failed() */
        perror("shmget() failed: ");
        exit (1);
    }
    printf("Shmid: %d\n",shmid_glob);
    int *p= shmat(shmid_glob,NULL,0);
    printf("1> %d  2> %d  3>%d\n",*p,*(p+1),*(p+2));
    int ch, value;
    if(*(p+2)!=1)
    	create_lnode(shmid_glob);
    printf("1> %d  2> %d  3>%d\n",*p,*(p+1),*(p+2));
	shmdt(p);
    while(1)
    {
    	printf("1. For Addition\n");
    	printf("2. For Deletion\n");
    	printf("3. For Showing Data\n");
    	scanf("%d",&ch);
    	switch(ch)
    	{
    		case 1: printf("Enter value to be Inserted: \n");
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