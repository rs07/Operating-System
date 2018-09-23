#include<stdio.h>
#include<unistd.h>			// For execve()
#include<stdlib.h>          // For exit(0)
#include<string.h>			// For strcat
#define MAX 100


int main(int argc, char *argv[])
{
	int num_arguments= argc;
	char path[]="/bin/";
	int l;
	pid_t cpid;

	for(int i=1; i<num_arguments; i++)
	{
		cpid= fork();
		if(cpid==0)
		{
			char *str= argv[i];
			char cache[20];
			if(str[0]=='-')
				exit(0);	
			int k=0;
			char newpath[20];
			strcpy(newpath,path);
			strcat(newpath,str);
			strcpy(cache,newpath);
			l=i+1;
			char *mypath[MAX];
			mypath[k]=cache;
			for(l; l<num_arguments; l++)
			{
				char *bin= argv[l];
				//printf("%s\n",argv[l]);
				if(bin[0]=='-')
				{
					k++;
					mypath[k]=bin;
				}
				else
					break;
			}
			k++;
			mypath[k]=NULL;
			//i=l-1;
			//printf("%d\n",i);
			//printf("%s\n",mypath[1]);
			/*for(int j=0; j<k; j++)
			{
				printf("%s\n",mypath[j]);
			}*/
			//printf("%s\n",str);
			int p = execve(cache,mypath,NULL);
			if(p==-1)
			{
				fprintf(stderr, "Error in Executing\n");
			}
			exit(0);
		}
	}
}