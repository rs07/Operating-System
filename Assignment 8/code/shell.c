#include<stdio.h>
#include<string.h>
#include<unistd.h>			// For execve()
#include<stdlib.h>          // For exit(0)
#include<sys/types.h>		//open()
#include<sys/stat.h>		//open()
#include<fcntl.h>			//open()

#define NUM_INTERNAL_CMD 45
#define CMD_LENGTH 100
#define WORD_LENGTH 20
#define MAX_LENGTH 1024

char internal_cmd[NUM_INTERNAL_CMD][20]={
	"alias",
	"bg",
	"bind",
	"break",
	"builtin",
	"caller",
	"cd",
	"command",
	"compgen",
	"complete",
	"compopt",
	"continue",
	"declare",
	"dirs",
	"disown",
	"echo",
	"enable",
	"eval",
	"exec",
	"exit",
	"export",
	"false",
	"fc",
	"fg",
	"getopts",
	"hash",
	"help",
	"history",
	"jobs",
	"kill",
	"let",
	"local",
	"logout",
	"mapfile",
	"popd",
	"printf",
	"pushd",
	"pwd",
	"read",
	"readarray",
	"readonly",
	"return",
	"set",
	"shift",
	"shopt"
};


char **words(char *buffer)
{
	int num_space=0;
	for(int i=0; i<CMD_LENGTH; i++)
	{
		if(buffer[i]=='\0')
			break;
		if(buffer[i]==' ')
			num_space++;
	}
	char **argv;
	(argv)= (char **)malloc((num_space+1)*sizeof(char *));
	int k=0,c=0;
	char cache[WORD_LENGTH];
	//printf("%s\n",buffer);
	for(int i=0; i<=num_space; i++)
	{
		(argv[i])= (char *)malloc(WORD_LENGTH*sizeof(char));
	}
	for(int i=0; i<CMD_LENGTH; i++)
	{
		//printf("hi");
		if(buffer[i]=='\0')
		{
			argv[k][c]='\0';
			break;
		}
		if(buffer[i]==' ')
		{
			argv[k][c]='\0';
			//strcpy(argv[k],cache);
			k++;
			c=0;
		}
		else
		{
			if(buffer[i]!=' ' && buffer[i]!='&' && buffer[i]!='|' && buffer[i]!='<' && buffer[i]!='>')
				argv[k][c]=buffer[i];
			c++;
		}
	}
	/*printf("%s\n",argv[0]);
	printf("%s\n",argv[1]);
	printf("%s\n",argv[2]);*/
	return argv;
}


/**********************************************************************************************************************************

                                                      EXTERNAL SYSTEM CALL

***********************************************************************************************************************************/


void external(char *buffer)
{
	char **para= words(buffer);
	int num_space=0;
	for(int i=0; i<CMD_LENGTH; i++)
	{
		if(buffer[i]==' ')
			num_space++;
	}

	int k=0;
	pid_t cpid;
	cpid= fork();
	if(cpid==0)
	{
		char *mypath[WORD_LENGTH];
		mypath[k]=para[0];
		k++;
		for(int i=1; i<=num_space; i++)
		{
			mypath[k]=para[i];
			k++;
		}
		mypath[k]=NULL;
		int p = execvp(para[0],mypath);
		if(p==-1)
		{
			fprintf(stderr, "\033[1m\033[31mError in Executing\n");
		}
		exit(0);
	}
	else
	{
		wait();
	}
}


/**********************************************************************************************************************************

                                                      INTERNAL SYSTEM CALL

***********************************************************************************************************************************/



void pres_dir()
{
	char pwd[MAX_LENGTH];
	if(getcwd(pwd,sizeof(pwd))!=NULL)
		fprintf(stdout, "%s\n",pwd);
	else
		perror("System is not working...");
}

void change_dir(char *buffer)
{
	char **para= words(buffer);
	if(para[0]==NULL)
	{
		fprintf(stderr, "Usage: cd [folder_name]\n");
	}
	else
	{
		if(chdir(para[1])!=0)
		{
			perror("System is not working...");
		}
	}
}


void internal(char *buffer)
{
	char **para= words(buffer);
	if(strcmp(para[0],"pwd")==0)
	{
		pres_dir();
	}
	else if(strcmp(para[0],"cd")==0)
	{
		change_dir(buffer);
	}
}



int check_pipe(char *buffer)
{
	for(int i=0; i<strlen(buffer); i++)
	{
		if(buffer[i]=='|')
		{
			return 1;
		}
	}
	return 0;
}

int checkIO(char *buffer)
{
	for(int i=0; i<strlen(buffer); i++)
	{
		if(buffer[i]=='<')
			return 1;
		if(buffer[i]=='>')
			return 2;
	}
	return 0;
}

int check_bool(char *buffer)
{
	for(int i=0; i<strlen(buffer)-1; i++)
	{
		if(buffer[i]=='&' && buffer[i+1]=='&')
			return 1;
		if(buffer[i]=='|' && buffer[i+1]=='|')
			return 2;
	}
	return 0;
}

int internalcmd(char *buffer)
{
	char **para= words(buffer);
	for(int i=0; i<NUM_INTERNAL_CMD; i++)
	{
		if(strcmp(internal_cmd[i],para[0])==0)
		{
			return 1;
		}
	}
	return 0;
}


/**********************************************************************************************************************************

                                                      PIPE SYSTEM CALL

***********************************************************************************************************************************/


void pipe_sys(char *buffer)
{
	int num_pipe=0;
	for(int i=0; i<strlen(buffer); i++)
	{
		if(buffer[i]=='|' || ((buffer[i]==' '&& buffer[i+1]=='-')))
		{
			num_pipe++;
		}
	}
	char **argv;
	(argv)= (char **)malloc((num_pipe+1)*sizeof(char *));
	int k=0,c=0;
	char cache[WORD_LENGTH];
	//printf("%s\n",buffer);
	for(int i=0; i<=num_pipe; i++)
	{
		(argv[i])= (char *)malloc(WORD_LENGTH*sizeof(char));
	}
	for(int i=0; i<CMD_LENGTH; i++)
	{
		//printf("hi");
		if(buffer[i]=='\0')
		{
			argv[k][c]='\0';
			k++;
			break;
		}
		if(buffer[i]=='|' || (buffer[i]==' '&& buffer[i+1]=='-'))
		{
			argv[k][c]='\0';
			//strcpy(argv[k],cache);
			k++;
			c=0;
		}
		else
		{
			if(buffer[i]!=' '&& buffer[i]!='|')
			{
				argv[k][c]=buffer[i];
				c++;
			}
		}
	}
	/*printf("%d\n",num_pipe);
	printf("Al1: -%s-\n",argv[0]);
	printf("Al2: -%s-\n",argv[1]);
	printf("Al3: -%s-\n",argv[2]);
	printf("Al4: -%s-\n",argv[3]);
	printf("Al5: -%s-\n",argv[4]);*/

	int pipefd[2];
	if(pipe(pipefd)<0)
		printf("Pipe cannot be initialized\n");
	int num_val=0;
	if(fork()==0)
	{
		int p=0;
		int l=num_val+1;
		char *mypath[WORD_LENGTH];
		mypath[p]=argv[0];
		for(l; l<=num_pipe; l++)
		{
			char *bin= argv[l];
			//printf("%s\n",argv[l]);
			if(bin[0]=='-')
			{
				p++;
				mypath[p]=bin;
			}
			else
				break;
		}
		p++;
		mypath[p]=NULL;
		close(pipefd[0]);
		dup2(pipefd[1],1);	//1= stdout file descriptor
		//close(pipefd[1]);
		//printf("Lend: \n");
		if(execvp(argv[0],mypath)<0)
		{
			fprintf(stderr, "Error: Unable to execute %s\n",mypath[0]);
			exit(0);
		}
		//close(pipefd[1]);
	}
	else
	{
		wait();
		num_val++;
		int flag=0;
		int pipefd_1[2];
		if(pipe(pipefd_1)<0)
			printf("Pipe cannot be initialized_1\n");
		for(; num_val<=num_pipe; num_val++)
		{
			if(argv[num_val][0]!='-')
				break;
		}
		//printf("Pef\n");
		for(num_val; num_val<=num_pipe; num_val++)
		{
			if(argv[num_val][0]=='-')
				continue;
			// if(flag==0)
			// {
			// 	close(pipefd[1]);
			// 	close(pipefd_1[0]);
			// }
			// if(flag==1)
			// 	close(pipefd_1[1]);
			// flag= 1-flag;
			close(pipefd[1]);
			if(fork()==0)
			{
				char *str= argv[num_val];
				int k=0;
				int l=num_val+1;
				char *mypath[WORD_LENGTH];
				mypath[k]=argv[num_val];
				for(l; l<=num_pipe; l++)
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
				dup2(pipefd[0],0);	//0= stdin file descriptor
				close(pipefd[0]);

				/*if(flag==1)
				{
					close(pipefd[1]);
					dup2(pipefd[0],0);
					close(pipefd[0]);
					close(pipefd_1[0]);
					dup2(pipefd_1[1],1);
					close(pipefd_1[1]);
				}
				else
				{
					close(pipefd_1[1]);
					dup2(pipefd_1[0],0);
					close(pipefd_1[0]);
					close(pipefd[0]);
					dup2(pipefd[1],1);
					close(pipefd[1]);
				}*/

				int p = execvp(argv[num_val],mypath);
				if(p==-1)
				{
					fprintf(stderr, "Error in Executing\n");
					exit(0);
				}
			}
			else
			{
				wait();
			}
		}
	}
}



/**********************************************************************************************************************************

                                                      BOOLEAN 'AND' FUNCTION

***********************************************************************************************************************************/


void and(char *buffer)
{
	int num_and=0;
	for(int i=0; i<strlen(buffer); i++)
	{
		if((buffer[i]=='&'&&buffer[i+1]=='&') || (buffer[i]==' '&& buffer[i+1]=='-'))
		{
			num_and++;
		}
	}
	char **argv;
	(argv)= (char **)malloc((num_and+1)*sizeof(char *));
	int k=0,c=0;
	char cache[WORD_LENGTH];
	//printf("%s\n",buffer);
	for(int i=0; i<=num_and; i++)
	{
		(argv[i])= (char *)malloc(WORD_LENGTH*sizeof(char));
	}
	for(int i=0; i<CMD_LENGTH; i++)
	{
		//printf("hi");
		if(buffer[i]=='\0')
		{
			argv[k][c]='\0';
			k++;
			break;
		}
		if((buffer[i]=='&'&&buffer[i+1]=='&') || (buffer[i]==' '&& buffer[i+1]=='-'))
		{
			argv[k][c]='\0';
			//strcpy(argv[k],cache);
			k++;
			c=0;
		}
		else
		{
			if(buffer[i]!=' '&& buffer[i]!='&')
			{
				argv[k][c]=buffer[i];
				c++;
			}
		}
	}
	/*printf("%d\n",num_and);
	printf("Al1: -%s-\n",argv[0]);
	printf("Al2: -%s-\n",argv[1]);
	printf("Al3: -%s-\n",argv[2]);
	printf("Al4: -%s-\n",argv[3]);
	printf("Al5: -%s-\n",argv[4]);*/
	if(fork()==0)
	{
		//printf("KLLL\n");
		int flag=1;
		for(int i=0; i<=num_and; i++)
		{
			if(argv[i][0]=='-')
				continue;
			//printf("And: -%s-\n",argv[i]);
			if(vfork()==0)
			{
				//printf("Jolo\n");
				int k=0;
				int l=i+1;
				char *mypath[WORD_LENGTH];
				mypath[k]=argv[i];
				for(l; l<=num_and; l++)
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

				int p = execvp(argv[i],mypath);
				if(p==-1)
				{
					fprintf(stderr, "Error in Executing\n");
					flag=0;
					exit(0);
				}
			}
			else 
			{
				wait();
				if(flag==0)
					exit(0);
			}
		}
		exit(0);
	}
	else
	{
		wait();
	}
}




/**********************************************************************************************************************************

                                                      BOOLEAN 'AND' FUNCTION

***********************************************************************************************************************************/


void or(char *buffer)
{
	int num_or=0;
	for(int i=0; i<strlen(buffer); i++)
	{
		if((buffer[i]=='|'&&buffer[i+1]=='|') || (buffer[i]==' '&& buffer[i+1]=='-'))
		{
			num_or++;
		}
	}
	char **argv;
	(argv)= (char **)malloc((num_or+1)*sizeof(char *));
	int k=0,c=0;
	char cache[WORD_LENGTH];
	//printf("%s\n",buffer);
	for(int i=0; i<=num_or; i++)
	{
		(argv[i])= (char *)malloc(WORD_LENGTH*sizeof(char));
	}
	for(int i=0; i<CMD_LENGTH; i++)
	{
		//printf("hi");
		if(buffer[i]=='\0')
		{
			argv[k][c]='\0';
			k++;
			break;
		}
		if((buffer[i]=='|'&&buffer[i+1]=='|') || (buffer[i]==' '&& buffer[i+1]=='-'))
		{
			argv[k][c]='\0';
			//strcpy(argv[k],cache);
			k++;
			c=0;
		}
		else
		{
			if(buffer[i]!=' '&& buffer[i]!='|')
			{
				argv[k][c]=buffer[i];
				c++;
			}
		}
	}
	/*printf("%d\n",num_and);
	printf("Al1: -%s-\n",argv[0]);
	printf("Al2: -%s-\n",argv[1]);
	printf("Al3: -%s-\n",argv[2]);
	printf("Al4: -%s-\n",argv[3]);
	printf("Al5: -%s-\n",argv[4]);*/
	if(fork()==0)
	{
		//printf("KLLL\n");
		int flag=1;
		for(int i=0; i<=num_or; i++)
		{
			if(argv[i][0]=='-')
				continue;
			//printf("And: %s\n",argv[i]);
			if(vfork()==0)
			{
				//printf("Jolo\n");
				int k=0;
				int l=i+1;
				char *mypath[WORD_LENGTH];
				mypath[k]=argv[i];
				for(l; l<=num_or; l++)
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
				flag=1;
				mypath[k]=NULL;

				int p = execvp(argv[i],mypath);
				if(p==-1)
				{
					fprintf(stderr, "Error in Executing\n");
					flag=0;
					exit(0);
				}
			}
			else 
			{
				wait();
				if(flag==1)
					exit(0);
			}
		}
		exit(0);
	}
	else
	{
		wait();
	}
}


/**********************************************************************************************************************************

                                                      BACKGROUND

***********************************************************************************************************************************/


void background(char *buffer)
{
	buffer[strlen(buffer)-2]='\0';
	if(fork()==0)
	{
		setpgid(0,0);
		external(buffer);
		exit(0);
	}
	else
	{
		wait();
	}
}




/**********************************************************************************************************************************

                                                      IO REDIRECTION '>'

***********************************************************************************************************************************/

void IO(char *buffer, int flag)
{
	char argv[WORD_LENGTH], file_name[WORD_LENGTH];
	int i, file_id=0;
	for(i=0; i<strlen(buffer); i++)
	{
		if(buffer[i]=='>' || buffer[i]=='<')
		{
			break;
		}
		argv[i]=buffer[i];
	}
	argv[i]='\0';
	i++;
	for(i; i<strlen(buffer); i++)
	{
		if(buffer[i]!=' ')
			file_name[file_id++]= buffer[i];
	}
	file_name[file_id]='\0';
	/*printf("Val: %s\n",argv);
	printf("kjl: %s\n",file_name);*/
	int p=0;
	while(argv[strlen(argv)-1]==' ')
	{
		argv[strlen(argv)-1]='\0';
	}

	char **para1= words(argv);
	/*printf("-%s-\n",para1[0]);
	printf("-%s-\n",para1[1]);
	printf("-%s-\n",para1[2]);*/
	int num_space=0;
	for(i=0; i<strlen(argv); i++)
	{
		if(argv[i]==' ')
			num_space++;
	}
	//printf("Spaces: %d\n",num_space);

	int fd= open(file_name,O_CREAT | O_RDWR);
	chmod(file_name,0777);
	if(vfork()==0)
	{
		for(i=0; i<=num_space; i++)
		{
			if(para1[i][0]=='-')
				continue;
			//printf("And: %s\n",argv[i]);
			if(vfork()==0)
			{
				//printf("Jolo\n");
				int k=0;
				int l=i+1;
				char *mypath[WORD_LENGTH];
				mypath[k]=para1[i];
				for(l; l<=num_space; l++)
				{
					char *bin= para1[l];
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
				if(flag==2)
					dup2(fd,1);
				else if(flag==1)
					dup2(fd,0);
				int p = execvp(para1[i],mypath);
				if(p==-1)
				{
					fprintf(stderr, "Error in Executing\n");
					exit(0);
				}
			}
			else 
			{
				wait();
			}
		}
		exit(0);
	}
	else
	{
		wait();
		close(fd);
	}
}










/**********************************************************************************************************************************

                                                      MAIN

***********************************************************************************************************************************/





int main(int argc, char *argv[])
{
	char buffer[CMD_LENGTH];
	char *prompt="myfs:~";
	if(argc==2)
	{
		//char **para= words(argv[1]);
		//char p[2]="./";
		//strcat(p,argv[1]);
		FILE *fp;
		fp = fopen(argv[1],"r");
		if (fp == NULL)
        	exit(EXIT_FAILURE);

        char buffer[100];
       	while (fgets(buffer,sizeof(buffer), fp) != NULL)
  		{
  			//printf("Line: -%s-\n",buffer);
    		buffer[strlen(buffer)-1] = '\0'; // eat the newline fgets() stores
    		//printf("Line: -%s-\n",buffer);
    		
			
			if(check_bool(buffer)>0)
			{
				int val = check_bool(buffer);
				if(val==1)
				{
					//printf("&& present\n");
					and(buffer);
				}
				else if(val==2)
				{
					//printf("|| present\n");
					or(buffer);
				}
			}
			else if(checkIO(buffer))
			{
				int val= checkIO(buffer);
				if(val==2 || val==1)
				{
					IO(buffer, val);
				}

			}
			else if(check_pipe(buffer))
			{
				pipe_sys(buffer);
			}
			else if(internalcmd(buffer))
			{
				printf("\033[1m\033[36m");
				internal(buffer);
			}
			else
			{
				printf("\033[1m\033[36m");
				external(buffer);
			}

  		}
		//external(p);
		//fflush(stdin);
		return 0;
	}
	while(1)
	{
		printf("\033[1m\033[32m%s \033[1m\033[37m",prompt);
		printf("# ");
		//fflush(stdin);
		strcpy(buffer,"");
		char prev[CMD_LENGTH];
		strcpy(prev,buffer);
		gets(buffer);
		if(strcmp(buffer,"exit")==0)
			exit(0);
		if(strcmp(buffer,"\n")==0 || strcmp(buffer,prev)==0)
		{
			printf("");
			continue;
		}
		if(buffer[strlen(buffer)-1]=='&' && buffer[strlen(buffer)-2]==' ')
		{
			printf("Background app\n");
			background(buffer);
		}
		else if(internalcmd(buffer))
		{
			printf("\033[1m\033[36m");
			internal(buffer);
		}
		else if(check_bool(buffer)>0)
		{
			//printf("Bool Command\n");
			int val = check_bool(buffer);
			if(val==1)
			{
				//printf("&& present\n");
				and(buffer);
			}
			else if(val==2)
			{
				//printf("|| present\n");
				or(buffer);
			}
		}
		else if(checkIO(buffer))
		{
			//printf("IO command\n");
			int val= checkIO(buffer);
			if(val==2 || val==1)
			{
				IO(buffer, val);
			}

		}
		else if(check_pipe(buffer))
		{
			//printf("Pipe present\n");
			pipe_sys(buffer);
		}
		else
		{
			printf("\033[1m\033[36m");
			external(buffer);
		}
	}
}