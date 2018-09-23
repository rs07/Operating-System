#include<stdio.h>
#include<unistd.h>



/* My file is containing of character in this format and these are the contents: 


ajfngrjknkj


*/



int main()
{
	FILE *fp;
	char c;
	fp = fopen("test.txt","r");
	c= fgetc(fp);
	if(fork()==0)
	{					
		c= fgetc(fp);
		c= fgetc(fp);
		c= fgetc(fp);
		printf("This is Child process: %c\n %d\n",c,ftell(fp));         // It will print 'n'
	}
	else
	{
		c=fgetc(fp);
		printf("This is Parent process: %c\n %d\n",c,ftell(fp));        // It will print 'j'
		fclose(fp);
	}
}