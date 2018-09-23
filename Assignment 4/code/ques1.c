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

/* following is a signal handler for the keypress ^C, that is, ctrl-c */
// typedef void (*sighandler_t)(int);
// void releaseSHM(int signum) {
//         int status;
//         // int shmctl(int shmid, int cmd, struct shmid_ds *buf);
//         status = shmctl(shmid, IPC_RMID, NULL); /* IPC_RMID is the command for destroying the shared memory*/
//         if (status == 0) {
//                 fprintf(stderr, "Remove shared memory with id = %d.\n", shmid);
//         } else if (status == -1) {
//                 fprintf(stderr, "Cannot remove shared memory with id = %d.\n", shmid);
//         } else {
//                 fprintf(stderr, "shmctl() returned wrong value while removing shared memory with id = %d.\n", shmid);
//         }

//         // int kill(pid_t pid, int sig);
//         status = kill(0, SIGKILL);
//         if (status == 0) {
//                 fprintf(stderr, "kill susccesful.\n"); /* this line may not be executed :P WHY?*/
//         } else if (status == -1) {
//                 perror("kill failed.\n");
//                 fprintf(stderr, "Cannot remove shared memory with id = %d.\n", shmid);
//         } else {
//                 fprintf(stderr, "kill(2) returned wrong value.\n");
//         }
// }




int main()
{
	int m, n, r;
	printf("Enter three sizes value m n r that will corresponds in m x n & n x r matrix: ");
	scanf("%d%d%d",&m,&n,&r);
	int mat1[m][n], mat2[n][r];
	printf("Enter the %d x %d matrix: \n",m,n);
	for(int i=0; i<m; i++)
	{
		for(int j=0; j<n; j++)
		{
			scanf("%d",&mat1[i][j]);
		}
	}
	printf("Enter the %d x %d matrix: \n",n,r);
	for(int i=0; i<n; i++)
	{
		for(int j=0; j<r; j++)
		{
			scanf("%d",&mat2[i][j]);
		}
	}
	int status;
    pid_t cpid;
    pid_t p1=0;

    //sighandler_t shandler;

    // 57586173
    /* install signal handler */
    //sighandler_t signal(int signum, sighandler_t handler);
    //shandler =  signal(SIGINT, releaseSHM); /* should we call this seperately in parent and child process */

	// int shmget(key_t key, size_t size, int shmflg);
    shmid =  shmget(IPC_PRIVATE, sizeof(int)*m*r, IPC_CREAT | 0777);

    if (shmid == -1) 
    { 	/* shmget() failed() */
        perror("shmget() failed: ");
        exit (1);
    }
    int c=-1;
    int *p;
    //p= (int *)shmat(shmid,NULL,0);

    for(int i=0; i<m; i++)
    {
    	for(int j=0; j<r; j++)
    	{
    		c++;
    		if(fork()==0)
    		{
    			p= (int *)shmat(shmid,NULL,0);
    			int sum=0;
    			for(int k=0; k<n; k++)
    			{
    				sum=sum+mat1[i][k]*mat2[k][j];
    			}
    			*(p+c)=sum;
    			shmdt(p);
    			exit(0);
    		}
            else                                    /* Waiting for Child process to end */
            {
                int kl;
                wait(&kl);
            }
    	}
    	printf("\n");
    }
    c=-1;
    printf("After multiplication: \n");
    for(int i=0; i<m; i++)
    {
    	for(int j=0; j<r; j++)
    	{
    		p= (int *)shmat(shmid,NULL,0);
    		c++;
    		printf("%d  ",*(p+c));
    		shmdt(p);
    	}
    	printf("\n");
    }
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
