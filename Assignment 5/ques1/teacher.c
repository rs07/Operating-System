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
#define MAX 20

struct att
{
	int roll;
	int pres;
	int process_id;
};
int shmid, *sh, semid;

#define NO_SEM  1

#define P(s) semop(s, &Pop, 1);
#define V(s) semop(s, &Vop, 1);


struct sembuf Pop;
struct sembuf Vop;



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

    int status1;

    status1 = semctl(semid, IPC_RMID, NULL);
    if (status == 0) 
    {
        fprintf(stderr, "Remove shared semaphore with id = %d.\n", semid);
    }
    else if (status == -1) 
    {
        fprintf(stderr, "Cannot remove shared semaphore with id = %d.\n", semid);
    } 
    else 
    {
        fprintf(stderr, "shmctl() returned wrong value while removing shared semaphore with id = %d.\n", semid);
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
	key_t keyval, mykey;
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
    int status = semctl(semid, 0, SETVAL, setvalArg);
    if(status == -1) {
        perror("semctl() failed");
        exit(1);
    }


    for(int i=0; i<MAX; i++)
    {
    	attendence[i].roll= i+1;
    	attendence[i].pres=0;
    	attendence[i].process_id=0;
    }
    *sh=-1;
    int coun=0, main_coun=0;
    P(semid);
    while(main_coun<MAX)
    {
    	//if(*sh!=-1)
    	{
            //P(semid);
            P(semid);
    		attendence[(*sh)-1].pres=1;
    		attendence[(*sh)-1].process_id= *(sh+1);
    		coun++;
    		main_coun++;
    		*sh=-1;
            V(semid);
    	}
    	if(coun==3)
    	{
    		printf("These are the details of the student: \n");
    		for(int i=0; i<MAX; i++)
    		{
    			if(attendence[i].pres==1)
    				printf("Roll Number: %d | Present %d\n",attendence[i].roll,attendence[i].process_id);
    			else
    				printf("Roll Number: %d | Absent\n",attendence[i].roll);
    		}
    		coun=0;
    	}
    }
    shmdt(sh);

}