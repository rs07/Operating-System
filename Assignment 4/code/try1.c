#include <stdio.h>
#include <stdlib.h>	//rand()
#include <unistd.h> //for fork()
#include <sys/ipc.h> //for shmget(), shmctl()
#include <sys/shm.h> //for shmget(),shmat(), shmdt(), shmctl()
#include <sys/wait.h> //for wait()

int main()
{
	pid_t pid;
	int shmid, numMatrix, choice;
	srand(NULL);
	printf("Enter the number of matrices: ");
	scanf("%d", &numMatrix);
	int matrixDim[numMatrix][2];
	int *matrix[numMatrix];
	int maxIntermediate=0;
	int wpid, status;
	for (int i=0; i<numMatrix; i++)
	{
		printf("Enter dimensions of matrix %d: ", i+1);
		scanf("%d %d", &matrixDim[i][0], &matrixDim[i][1]);
		if (i>0)
		{
			if (matrixDim[i-1][1] != matrixDim[i][0])
			{
				printf("Please enter compatible matrix sizes\n");
				return -1;
			}
			if ((matrixDim[0][0] * matrixDim[i][1]) > maxIntermediate)
			{
				maxIntermediate = (matrixDim[0][0] * matrixDim[i][1]);
			}
		}
		matrix[i] = (int *) malloc(matrixDim[i][0] * matrixDim[i][1] * sizeof(int));
		for (int j=0; j<matrixDim[i][0]; j++)
		{
			for (int k=0; k<matrixDim[i][1]; k++)
			{
				*(matrix[i] + j*matrixDim[i][1] + k) = rand()%10;
			}
		}
	}
	int res[ (matrixDim[0][0]) ] [ (matrixDim[numMatrix-1][1]) ];

	printf("\n");
	for (int i=0; i<numMatrix; i++)
	{
		printf("Matrix %d:\n", i+1);
		for (int j=0; j<matrixDim[i][0]; j++)
		{
			for (int k=0; k<matrixDim[i][1]; k++)
			{
				printf("%d\t", *(matrix[i] + j*matrixDim[i][1] + k));
			}
			printf("\n");
		}
		printf("\n");
	}
	shmid=shmget(IPC_PRIVATE, maxIntermediate*2*sizeof(int), IPC_CREAT|0777);
	if (shmid==-1)
	{
		printf("Error in getting shared memory\n");
		exit(0);
	}
	int *att;
	att = (int *)shmat(shmid, NULL, 0);
	if (att == (void *)-1) //shmat fails
	{ 
    	perror("shmat() failed");
        exit (1);
    }
    for (int i=0; i<matrixDim[0][0]; i++)	//writing matrix 1 to shared memory initially
    //this will make the algorithm uniform i.e. one matrix in shared memory, second matrix present as a local variable copied from parent
    {
    	for (int j=0; j<matrixDim[0][1]; j++)
    	{
    		*(att + (i*matrixDim[0][1]) + j) = *(matrix[0] + (i*matrixDim[0][1]) + j);
    	}
    }
    for (int i=0; i<numMatrix-1; i++)
    {
    	pid=fork();
		if (pid==0)	//child process
		{
			for (int j=0; j<matrixDim[0][0]; j++)
			{
				for (int k=0; k<matrixDim[i+1][1]; k++)
				{
					pid=fork();
					if (pid==0)
					{
						int *att;
						att = (int *)shmat(shmid, NULL, 0);
						if (att == (void *)-1) //shmat fails
						{ 
		    				perror("shmat() failed");
		        			exit (1);
		    			}
		    			int res=0;
						for (int l=0; l<matrixDim[i+1][0]; l++)	//calculate one element of product in child process
						{
							res += (*(att + j*matrixDim[i][1] + l)) * (*(matrix[i+1] + l*matrixDim[i+1][1] + k));
						}
						*(att + (j*matrixDim[i+1][1]) + k + maxIntermediate) = res;	//store element such that it doesn't overwrite the input matrix
						shmdt(att);
						exit(0);
					}
				}
			}
			while ((wpid=wait(&status)) > 0);	//multiplication of present 2 matrices must be completed before the next matrix multiplication is done
			for (int m=0; m<matrixDim[0][0]; m++)	//copying newly calculated result to beginning of shared memory i.e. position of 1st input matrix
			{
				for (int n=0; n<matrixDim[i+1][1]; n++)
				{
					*(att + (m*matrixDim[i+1][1]) + n) = *(att + (m*matrixDim[i+1][1]) + n + maxIntermediate);
				}
			}
			exit(0);
		}
		else 	//calculation of each element of the result of multiplying 2 matrices can be parallel, but pairwise multiplication has to be sequential
		{
			int n;
			wait(&n);	//waiting for multiplication of two matrices to complete
		}
    }
		
	printf("Product of given matrices:\n");
	for (int i=0; i<matrixDim[0][0]; i++)
	{
		for (int j=0; j<matrixDim[numMatrix-1][1]; j++)
		{
			res[i][j]=*(att+(i*matrixDim[numMatrix-1][1])+j);	//store elements from shared memory to array
			printf("%d\t", res[i][j]);
		}
		printf("\n");
	}

	shmdt(att);
	shmctl(shmid,IPC_RMID,NULL);
	return 0;
}