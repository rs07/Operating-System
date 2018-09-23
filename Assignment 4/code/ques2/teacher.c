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

struct att
{
	int roll;
	int pres;
	int process_id;
};
int shmid, *sh;


/* following is a signal handler for the keypress ^C, that is, ctrl-c */
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
    printf("%d\n",kill(0,SIGKILL));
    //status = kill(0, SIGKILL);
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
	key_t keyval;
	//key_t ftok(const char *pathname, int proj_id);
	keyval= ftok("/tmp",55);
	struct att attendence[MAX];
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
    sighandler_t shandler;

	/* install signal handler */
    // sighandler_t signal(int signum, sighandler_t handler);
    shandler =  signal(SIGINT, releaseSHM); /* should we call this seperately in parent and child process */

    sh= (int *)shmat(shmid,NULL,0);
    for(int i=0; i<MAX; i++)
    {
    	attendence[i].roll= i+1;
    	attendence[i].pres=0;
    	attendence[i].process_id=0;
    }
    *sh=-1;
    int coun=0, main_coun=0;
    while(main_coun<MAX)
    {
    	if(*sh!=-1)
    	{
            if(attendence[*(sh)-1].pres!=1)
            {
    		  attendence[(*sh)-1].pres=1;
    		  attendence[(*sh)-1].process_id= *(sh+1);
    		  coun++;
    		  main_coun++;
            }
            else
            {
                printf("Student Already Present\n");
            }
    		*sh=-1;
    	}
    	if(coun==3)
    	{
    		printf("These are the details of the student: \n");
    		for(int i=0; i<MAX; i++)
    		{
    			if(attendence[i].pres==1)
    				printf("Roll Number: %d | Present | Process Id: %d\n",attendence[i].roll,attendence[i].process_id);
    			else
    				printf("Roll Number: %d | Absent\n",attendence[i].roll);
    		}
    		coun=0;
    	}
    }
    shmdt(sh);

}