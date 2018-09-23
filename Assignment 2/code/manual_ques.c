#include<stdio.h>
#include<unistd.h>
int main()
{
	int n;
	printf("Enter the number of child process want to create: ");
	scanf("%d",&n);
	for(int i=0; i<n; i++)
	{        
		if(fork()==0)
		{
			printf("Child process created\n");
			exit(0);
		}
	}
}