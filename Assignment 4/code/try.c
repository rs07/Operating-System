/* Distributing the task of matrix multiplication among many processes */
#include<stdio.h>
#include<unistd.h>	 		/* for fork() */
#include<sys/wait.h> 		/* for wait() */
#include<sys/types.h> 		/* for wait() kill(2)*/
#include<sys/ipc.h> 		/* for shmget() shmctl() */
#include<sys/shm.h> 		/* for shmget() shmctl() */
#include<signal.h> 			/* for signal(2) kill(2) */
#include<errno.h> 			/* for perror */
#include<stdlib.h>	



int main()
{
	int shmid;
	shmid =  shmget(IPC_PRIVATE, sizeof(int)*4*5, IPC_CREAT | 0777);
	int *p;
	int c=-1;
	
	for(int i=0; i<4; i++)
	{
		for(int j=0; j<5; j++)
		{
			c++;
			if(fork()==0)
			{
				p= (int*)shmat(shmid,NULL,0);
				*(p+c)=c;
				shmdt(p);
				exit(0);
			}
		}
	}
	int k=0;
	for(int i=0; i<4; i++)
	{
		for(int j=0; j<5; j++)
		{
			k++;
			if(fork()==0)
			{
				p= (int*)shmat(shmid,NULL,0);
				printf("%d\n",*(p+k));
				shmdt(p);
				exit(0);
			}
		}
	}
}