// Write a C program to create 'n' processes and to print factorial.

#include<stdio.h>
#include<unistd.h>
//using namespace std;

int factorial(int c)
{
	int t=1;
	while(c>0)
	{
		t=t*c;
		c--;
	}
	return t;
}


int main(int argc, char *argv[])
{
	int n;
	//printf("%d\n",n);                   <---------------------- Here \n part i.e., printing each time with calling of fork
	//printf("%d\n",argc);
	for(int i=1; i<argc; i++)                       // "n" processes
	{
		if(fork()==0)
		{
			printf("Child process created: Facorial of %d is %d",atoi(argv[i]),factorial(atoi(argv[i])));
			if(i==argc-1)
				printf(" <---------This is the Last Argument");
			printf("\n");
			exit(0);
		}
	}
}


// I have to explain the '\n' part