/******************************************************************************************************************************************************


                                                   &*&@@@@&*&                          %%%%%%%%%%%%%%%%%%%%%%
                                                  &*&      &*&                         %%%%%%%%%%%%%%%%%%%%%
                                                &*&         &*&                        @@
                                              &*&            &*&                       @@
                                             &*&              &*&                      @@
                                            &*&                &*&                     @@
                                           &*&                 &*&                     @@
                                           **                   **                     %%%%%%%%%%%%%%%%%%%%%
                                          &*&                   &*&                     %%%%%%%%%%%%%%%%%%%%%
                                          &*&                   &*&                                        @@
                                           **                   **                                         @@
                                          &*&                   &*&                                        @@
                                          &*&                   &*&                                        @@
                                           &*&                 &*&                                         @@
                                            &*&               &*&                                          @@
                                              &*&            &*&                       %%%%%%%%%%%%%%%%%%%%%%
                                                &*&         &*&                         %%%%%%%%%%%%%%%%%%%%%
                                                 &*&@@@@@@&*&                                                


Author: Rishabh Kumar Sharma
Tested By: Rishabh Kumar Sharma
Last Updated:  19th February, 2018, 21:27 Hrs.

*/




/* Program of a Shared Linked List using Threads */

#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>						/* For using Threads----> pthread_create(3)  */
#define MAX 10


struct node
{
	int data;
	struct node *link;
};

struct node *root[MAX];

void init()
{
	for(int i=0; i<MAX; i++)
	{
		root[i]=NULL;
	}
}

struct pass_thread
{
	int choice;
	int threads;
	int list;
};

pthread_mutex_t lock[MAX];
int curr_list=0;

void traverse(int list)
{
	pthread_mutex_lock(&lock[list]);
	struct node *ptr=root[list];
	if(root[list]==NULL)
	{
		fprintf(stderr, "Error: empty list\n");
	}
	while(1)
	{
		printf("Data: %d\n",ptr->data);
		//getchar();
		if(ptr->link==NULL)
			break;
		ptr= ptr->link;
	}
	pthread_mutex_unlock(&lock[list]);
}


void add(int ptri, int list)
{
	printf("Foo\n");
	int value= ptri;
	pthread_mutex_lock(&lock[list]);
	//printf("Boo\n");
	struct node *ptr= root[list];
	if(ptr==NULL)
	{
		//printf("Ho\n");
		struct node *tmp;
		tmp = (struct node *)malloc(sizeof(struct node));
		tmp->data=value;
		tmp->link=NULL;
		ptr=tmp;
		root[list]=ptr;
		//free(tmp);
		//printf("Successfully Added\n");
		return;
	}
	else
	{
		while(ptr->link!=NULL)
		{
			//printf("Hi\n");
			ptr=ptr->link;
		}
		struct node *tmp;
		tmp = (struct node *)malloc(sizeof(struct node));
		tmp->data=value;
		tmp->link=NULL;
		ptr->link=tmp;
		//printf("Successfully Added(2)\n");
		//free(tmp);
	}
	pthread_mutex_unlock(&lock[list]);
}

void del(int ptri, int list)
{
	int value= ptri;
	pthread_mutex_lock(&lock[list]);
	struct node *ptr= root[list];
	//printf("Check\n");
	if(root[list]==NULL)
	{
		fprintf(stderr, "Error: empty list\n");
		return;
	}
	if(ptr->data==value)
	{
		root[list]= ptr->link;
		printf("Successfully Deleted\n");
		return;
	}
	else
	{
		if(ptr->link==NULL)
		{
			printf("Not Found\n");
			return;
		}
		while(ptr->link->data!=value && ptr->link->link!=NULL)
		{
			printf("Lo\n");
			ptr= ptr->link;
		}
		if(ptr->link->data==value)
		{
			ptr->link= ptr->link->link;
		}
		else
		{
			printf("Not Found\n");
		}
	}
	pthread_mutex_unlock(&lock[list]);
}




void *runner(void *ptr)
{

	struct pass_thread *val= (struct pass_thread*) ptr;
	{
		int value, thread, list,ch;
		ch= val->choice;
		thread= val->threads;
		list= val->list;
		printf("Boo ch: %d\n",ch);
		switch(ch)
		{
			case 1: printf("Enter the value to be added in %d thread: ",(thread+1));
					scanf("%d",&value);
					printf("Value: %d\n",value);
					add(value,list);
					break;
			case 2:	printf("Enter the value to be deleted in %d thread: ",(thread+1));
					scanf("%d",&value);
					del(value,list);
					break;
			case 3: traverse(list);
					break;
			case 4: printf("Enter the value of linked list to be switched: ");
					scanf("%d",&curr_list);
					curr_list=curr_list-1;
					break;
			default: exit(0);
		}
	}
	//free(val);
}



int main()
{
	init();
	int num_list, num_op;
	printf("Enter the number of Linked List : ");
	scanf("%d",&num_list);
	printf("Enter the number of Operation: ");
	scanf("%d",&num_op);
	int ch;
	pthread_t oper[num_list][num_op];
	int value,status,stat, curr_thread[num_list];
	for(int i=0; i<num_list; i++)
	{
		if(pthread_mutex_init(&lock[i],NULL)!= 0)
    	{
        	printf("\n mutex init has failed\n");
        	return 1;
    	}
    }
    curr_list=0;
    for(int i=0; i<num_list; i++)
		curr_thread[i]=0;
	while(1)
	{
		printf("Current List: %d\n",curr_list+1);
		printf("1. For adding the value in the node\n");
		printf("2. For deleting the node\n");
		printf("3. For Traversing the LL\n");
		printf("4. Switch to another list\n");
		printf("Else anything for exit\n");
		fflush(stdin);
		scanf("%d",&ch);
		struct pass_thread *dim;
		dim = (struct pass_thread *)malloc(sizeof(struct pass_thread));
		dim->choice= ch;
		dim->threads= curr_thread[curr_list];
		dim->list= curr_list;
		printf("Hi\n");
		pthread_create(&oper[curr_list][curr_thread[curr_list]],NULL,runner,dim);
		printf("Hi2\n");
		curr_thread[curr_list]++;
		//free(dim);
	}
	
	for(int i=0; i<num_list; i++)
	{
		for(int j=0; j<curr_thread[i]; j++)
		{
			status= pthread_join(oper[i][j],NULL);
			if(status!=0)
			{
				fprintf(stderr, "Error: pthread_join() fails\n");
				exit(0);
			}
			else
			{
				printf("Successfully Joined pthread\n");
			}
		}
	}
}
