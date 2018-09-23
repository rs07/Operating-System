/* Distributing the task of matrix multiplication among many processes */
#include<stdio.h>
#include<unistd.h>	 		/* for fork() */
#include<sys/wait.h> 		/* for wait() */
#include<sys/types.h> 		/* for wait() kill(2)*/
#include<sys/ipc.h> 		/* for shmget() shmctl() */
#include<sys/shm.h> 		/* for shmget() shmctl() */
#include<signal.h> 			/* for signal(2) kill(2) */
#include<errno.h> 			/* for perror */
#include<stdlib.h>			/* for exit(0) */






int shmid; /* defined globally so that signal handler can access it */


struct index
{
	int r;
	int c;
};


typedef void (*sighandler_t)(int);
void releaseSHM(int signum) 
{
    int status;
    // int shmctl(int shmid, int cmd, struct shmid_ds *buf);
    status = shmctl(shmid, IPC_RMID, NULL); /* IPC_RMID is the command for destroying the shared memory*/
    if (status == 0) 
    {
        fprintf(stderr, "Remove shared memory with id = %d.\n", shmid);
    }
    else if (status == -1) 
    {
        fprintf(stderr, "Cannot remove shared memory with id = %d.\n", shmid);
    } 
    else 
    {
        fprintf(stderr, "shmctl() returned wrong value while removing shared memory with id = %d.\n", shmid);
    }

    // int kill(pid_t pid, int sig);
    //printf("%d\n",kill(0,SIGKILL));
    status = kill(0, SIGKILL);
    if (status == 0) 
    {
        fprintf(stderr, "kill susccesful.\n"); /* this line may not be executed :P WHY?*/
    } 
    else if (status == -1) 
    {        
        perror("kill failed.\n");
        fprintf(stderr, "Cannot remove shared memory with id = %d.\n", shmid);
    } 
    else 
    {
        fprintf(stderr, "kill(2) returned wrong value.\n");
    }
}





int main()
{
	int m, n, r, num_matrix,c,*p;
	printf("Enter the number of matrix want to enter: ");
	scanf("%d",&num_matrix);
	struct index obj[num_matrix];
	int arr[num_matrix+1];
	int max_size=0;
	printf("Enter the %d +1 number of matrix row - column values: ",num_matrix);
	for(int i=0; i<=num_matrix; i++)
	{
		scanf("%d",&arr[i]);
		if(i>0)
		{
			if(arr[i]*arr[0]>max_size)
				max_size= arr[i]*arr[0];
		}
	}
	for(int i=0; i<num_matrix; i++)
	{
		obj[i].r= arr[i];
		obj[i].c= arr[i+1];
	}
	printf("At first enter 2 matrix value: \n");
	int *mat1, *mat2;
	mat1= (int *)malloc(obj[0].r*obj[0].c*sizeof(int));
	printf("Enter the %d x %d matrix: \n",obj[0].r,obj[0].c);
	for(int i=0; i<obj[0].r; i++)
	{
		for(int j=0; j<obj[0].c; j++)
		{
			scanf("%d",(mat1 + i*obj[0].c + j));
			//printf("%d ",*(mat1 + i*obj[0].c + j));
		}
	}
	mat2= (int *)malloc(obj[1].r*obj[1].c*sizeof(int));
	printf("Enter the %d x %d matrix: \n",obj[1].r,obj[1].c);
	for(int i=0; i<obj[1].r; i++)
	{
		for(int j=0; j<obj[1].c; j++)
		{
			scanf("%d",(mat2 + i*obj[1].c + j));
		}
	}
	int status;
    pid_t cpid;
    pid_t p1=0;

    sighandler_t shandler;

    /* install signal handler */
    //sighandler_t signal(int signum, sighandler_t handler);
    shandler =  signal(SIGINT, releaseSHM); /* should we call this seperately in parent and child process */

	// int shmget(key_t key, size_t size, int shmflg);
    shmid =  shmget(IPC_PRIVATE, sizeof(int)*max_size, IPC_CREAT | 0777);

    if (shmid == -1) 
    { 	/* shmget() failed() */
        perror("shmget() failed: ");
        exit (1);
    }



    for(int z=1; z<num_matrix; z++)
    {
    	if(fork()==0)
    	{
    		c=-1;
    		//p= (int *)shmat(shmid,NULL,0);
    		if(z>=2)
    		{
    			free(mat1);
    			getchar();
    			c=-1;
    			mat1= (int *)malloc(obj[0].r*obj[z-1].c*sizeof(int));
    			printf("New multiplication: \n");
    			//printf("%d & %d\n",obj[0].r,obj[z].r);
    			for(int i=0; i<obj[0].r; i++)
    			{
    				for(int j=0; j<obj[z].r; j++)
    				{
    					p= (int *)shmat(shmid,NULL,0);
    					c++;
    					*(mat1 + i*obj[z].r + j)= *(p+c);
    					printf("%d  ",*(p+c));
    					//printf(" %d  ",c);
    					//printf("%d  ",*(mat1 + i*obj[z-1].c + j));
    					shmdt(p);
    				}
    				printf("\n");
    			}
    			free(mat2);
    			mat2= (int *)malloc(obj[z].r*obj[z].c*sizeof(int));
    			printf("Enter next matrix of size %d x %d:\n",obj[z].r,obj[z].c);
    			for(int i=0; i<obj[z].r; i++)
    			{
    				for(int j=0; j<obj[z].c; j++)
    				{
    					scanf("%d",(mat2 + i*obj[z].c + j));
    				}
    			}
    		}
    		//printf("Let take it as a challenge\n");

    		c=-1;
    		m= obj[0].r;
    		r= obj[z].c;
    		n= obj[z].r;
    		for(int i=0; i<obj[0].r; i++)
    		{
    			//printf("Dare\n");
    			for(int j=0; j<obj[z].c; j++)
    			{
    				//printf("Hi\n");
    				c++;
    				cpid= fork();
    				if(cpid==0)
    				{
    					p= (int *)shmat(shmid,NULL,0);
    					int sum=0;
    					for(int k=0; k<obj[z].r; k++)
    					{
    						int a1= *(mat1 + i*obj[z].r + k);
    						int a2= *(mat2 + k*obj[z].c + j);
    						sum=sum+a1*a2;
    					}
    					*(p+c)=sum;
    					//printf("Sum: %d\n",sum);
    					shmdt(p);
    					exit(0);
    				}
    				else
    				{
    					int kl;
    					wait(&kl);
    				}
    			}
    			//printf("\n");
    			int kl;
    			wait(&kl);
    		}
    		int kl;
    		wait(&kl);
    		exit(0);
    	}
    	else
    	{
    		int kl;
    		wait(&kl);
    	}
    }

    /*  For Showing the Value after the Multiplication */


    printf("\nValue after multiplication: \n");
    c=-1;
    for(int i=0; i<obj[0].r; i++)
    {
    	for(int j=0; j<obj[num_matrix-1].c; j++)
    	{
    		p= (int *)shmat(shmid,NULL,0);
    		c++;
    		printf("%d  ",*(p+c));
    		shmdt(p);
    	}
   		printf("\n");
    }


    /* For Deallocating the Shared Space  */


    status = shmctl(shmid, IPC_RMID, NULL); /* IPC_RMID is the command for destroying the shared memory*/
        
    if (status == 0) 
    {
       	fprintf(stderr, "Remove shared memory with id = %d.\n", shmid);
    } 
    else if (status == -1) 
    {
        fprintf(stderr, "Cannot remove shared memory with id = %d.\n", shmid);
    } 
    else 
    {
        fprintf(stderr, "shmctl() returned wrong value while removing shared memory with id = %d.\n", shmid);
    }
    return 0;
}
