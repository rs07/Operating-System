#include<stdio.h>
#include<unistd.h>	 		/* for fork() */
#include<sys/wait.h> 		/* for wait() */
#include<sys/types.h> 		/* for wait() kill(2)  ftok() */
#include<sys/ipc.h> 		/* for shmget() shmctl() ftok() */
#include<sys/shm.h> 		/* for shmget() shmctl() */
#include<signal.h> 			/* for signal(2) kill(2) */
#include<errno.h> 			/* for perror */
#include<stdlib.h>			/* for exit(0) */
#define MAX 20

int shmid, *sh;


int main()
{
	key_t keyval;
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
    int roll;
    pid_t pid;
    pid= getpid();
    printf("Enter roll Number between 1 to 20: ");
    scanf("%d",&roll);
    while(1)
    {
        if(*sh==-1)
        {
            *sh=roll;
            *(sh+1)= pid;
            break;
        }
    }
    shmdt(sh);
    return 0;
}