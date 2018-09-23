#include<stdio.h>
#include<unistd.h>			// For execve()
#include<stdlib.h>          // For exit(0)
#include<string.h>			// For strcat



int main(int argc, char *argv[])
{
	int num_arguments= argc-1;
	char buffer[5]="/bin/";
	char chk[20];
	pid_t cpid;
	/*for(int i=0; i<num_arguments; i++)
	{
		strcpy(chk,buffer);
		strcat(chk,argv[i+1]);
		//printf("%s\n",chk);
		strcpy(argv[i],chk);
		//puts(argv[i]);
	}*/
	//argv[num_arguments-1]=NULL;
	int p;
	char *mypath[]={"Test.txt",NULL};
	p=execve("Test.txt",mypath,NULL);
	if(p==-1)
	{
		fprintf(stderr, "This is a Text File\n");
	}
	/*for(int i=0; i<num_arguments; i++)
	{
		printf("%s\n",argv[i]);
	}
	printf("%d\n",num_arguments);*/
	/*for(int i=0; i<num_arguments; i++)
	{
		cpid=fork();
		if(cpid==0)
		{
			char *mypath[]= {argv[i],NULL};

			int p= execve(argv[i],mypath,NULL);
			if(p==-1)
			{
				printf("Error\n");
			}
			exit(0);
		}
	}*/
}