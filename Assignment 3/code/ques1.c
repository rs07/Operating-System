#include<stdio.h>
#include<unistd.h>			// For execve()
#include<stdlib.h>          // For exit(0)
#include<string.h>			// For strcat



int main(int argc, char *argv[])
{
	int num_arguments= argc;
	char buffer[5]="/bin/";
	char chk[20];
	pid_t cpid;
	
	for(int i=1; i<num_arguments; i++)
	{
		cpid=fork();
		if(cpid==0)
		{
			//printf("%s\n",argv[i]);
			char bufi[20];
			fflush(stdin);
			strcpy(chk,buffer);
			strcat(chk,argv[i]);
			char *mypath[]= {chk,NULL};

			int p= execve(chk,mypath,NULL);
			if(p==-1)
			{
				printf("Error\n");
			}
			exit(0);
		}
	}
}