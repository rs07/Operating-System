#include<stdio.h>





int main()
{
	FILE *fp;
	char c;
	fp = fopen("test1.txt","w");
	//fputc(c,fp);

	//c= fgetc(fp);
	if(fork()==0)
	{	
		c='E';				
		fputc(c,fp);
		printf("This is Child process");         
	}
	else
	{
		char d='B';
		putc(d,fp);
		printf("This is Parent process");        
		fclose(fp);
	}
}