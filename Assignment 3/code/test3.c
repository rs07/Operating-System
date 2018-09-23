#include<stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

int main(int argc, char* argv[]){

	char* string[2];
	pid_t p;
	int arr=3;
	int ret;
	int i,childid;
	if(argc<=1){
	
	fprintf(stderr,"usage : %s<number>\n",argv[0]);
	
	}
	
	else
	{
		for(i=1;i<argc;i++)
		{
			string[0]=argv[i];
			string[1]=NULL;
			p=fork();
			if(p==0)
			{
				//printf("%d\n",arr/0);
				//inside child process
				printf("inside child process %d\n",i);
				ret=execve(argv[i],string,NULL);
				if(ret==-1)
				{
					//execve returns erroneous results
					printf("error");
					exit(1);
					
				}		
				
				
			}
			else if(p>0)
			{	
				wait(&childid);


				/*wait for the child process to terminate		
				wait functions return pid of the completed or terminated child process and returns the status of that
				completed or terminated child process*/
				//printf("childid:%d\n",(*childid));
				

				if(WIFEXITED(childid))             //return true value if child process terminated normally by exit
				{ 							
					printf("child process terminated normally by exit having status: %d\n",WEXITSTATUS(childid));
					
				}
				else if(WIFSIGNALED(childid))     //returns true value if child process terminated abnormally using a signal
				{
				
					printf("child process terminated abnormally using signal with signal no. %d\n",WTERMSIG(childid));
							
				}	
				else if(WIFSTOPPED(childid))      //returns true value if child process stopped abnormally using a signal
				{
					printf("child process stopped abnormally using signal with signal no. %d\n",WSTOPSIG(childid));
				}
				else if(WIFCONTINUED(childid))
				{
								
					printf("child process continued");
				}
			}
			printf("\n");	
		}
	}
	return 0;
}