#include<stdio.h>
#include<unistd.h>			// For fork()
#include<stdlib.h>          // For exit(0)
#include<string.h>			// For strcat
#include<sys/types.h>		// For wait()
#include<sys/wait.h>		// For wait()
#include<signal.h>			// For SIGINT

int main(int argc, char *argv[])
{
	int num_arguments= argc;
	char buffer[5]="/bin/";
	char chk[20];
	int status;
	pid_t cpid;
	/*for(int i=0; i<num_arguments; i++)
	{
		strcpy(chk,buffer);
		strcat(chk,argv[i+1]);
		//printf("%s\n",chk);
		strcpy(argv[i],chk);
		//puts(argv[i]);
	}*/
	for(int i=1; i<num_arguments; i++)
	{
		cpid=fork();
		if(cpid==0)
		{
			char *mypath[]= {argv[i],NULL};

			int p;
			// if(i==1)
			// 	kill(cpid,SIGABRT);
			int t;
			if(i==1)
				t= 5/0;
			p= execve(argv[i],mypath,NULL);
			if(p==-1)
			{
				printf("Error\n");
			}
			//printf("Reached Exit\n");
			exit(0);
		}
		else
		{
			//printf("This is parent process waiting starts\n");
			wait(&status);
			//printf("This is parent process waiting ended\n");
			if(WIFEXITED(status))
			{
				printf("The Child process terminated Normally\n");
			}
			else if(WIFSIGNALED(status))
			{
				printf("The Child process terminated Abnormally using Signal\n");
				int z= WTERMSIG(status);
				printf("Number of Signal used to terminate the process: %d\n",z);
			}
			else if(WIFSTOPPED(status))
			{
				printf("The Child process stopped Abnormally using Signal\n");
				int z= WSTOPSIG(status);
				printf("Number of Signal used to stop the process: %d\n",z);
			}
			else if(WIFCONTINUED(status))
			{
				printf("The Child process was resumed by delivery of SIGCONT\n");
			}
		}
	}
}