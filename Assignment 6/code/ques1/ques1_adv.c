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

struct node
{
	int data;
	struct node *link;
};

struct node *root=NULL;

pthread_mutex_t lock;

void traverse()
{
	pthread_mutex_lock(&lock);
	struct node *ptr=root;
	if(root==NULL)
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
	pthread_mutex_unlock(&lock);
}


void add(int ptri)
{
	//printf("Foo\n");
	int value= ptri;
	pthread_mutex_lock(&lock);
	//printf("Boo\n");
	struct node *ptr= root;
	if(ptr==NULL)
	{
		//printf("Ho\n");
		struct node *tmp;
		tmp = (struct node *)malloc(sizeof(struct node));
		tmp->data=value;
		tmp->link=NULL;
		ptr=tmp;
		root=ptr;
		//free(tmp);
		//printf("Successfully Added\n");
	}
	else
	{
		//printf("Chk1\n");
		while(ptr->link!=NULL)
		{
			//printf("Hi\n");
			ptr=ptr->link;
		}
		//printf("Chk2\n");
		struct node *tmp;
		tmp = (struct node *)malloc(sizeof(struct node));
		tmp->data=value;
		tmp->link=NULL;
		ptr->link=tmp;
		//printf("Successfully Added(2)\n");
		//free(tmp);
	}
	pthread_mutex_unlock(&lock);
}

void del(int ptri)
{
	int value= ptri;
	pthread_mutex_lock(&lock);
	struct node *ptr= root;
	//printf("Check\n");
	if(root==NULL)
	{
		fprintf(stderr, "Error: empty list\n");
		pthread_mutex_unlock(&lock);
		return;
	}
	if(ptr->data==value)
	{
		root= ptr->link;
		printf("Successfully Deleted\n");
		pthread_mutex_unlock(&lock);
		return;
	}
	else
	{
		if(ptr->link==NULL)
		{
			printf("Not Found\n");
			pthread_mutex_unlock(&lock);
			return;
		}
		while(ptr->link->data!=value && ptr->link->link!=NULL)
		{
			//printf("Lo\n");
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
	pthread_mutex_unlock(&lock);
}




void *runner(void *ptr)
{

	int ch= *((int *)ptr);
	{
		int value;
		switch(ch)
		{
			case 1: printf("Enter the value to be added: ");
					scanf("%d",&value);
					printf("Value: %d\n",value);
					add(value);
					break;
			case 2:	printf("Enter the value to be deleted: ");
					scanf("%d",&value);
					del(value);
					break;
			case 3: traverse();
					break;
			default: exit(0);
		}
	}
}



int main()
{
	int ch;
	pthread_t oper;
	int value,status,stat;
	if(pthread_mutex_init(&lock,NULL)!= 0)
    {
        printf("\n mutex init has failed\n");
        return 1;
    }
	while(1)
	{
		printf("1. For adding the value in the node\n");
		printf("2. For deleting the node\n");
		printf("3. For Traversing the LL\n");
		scanf("%d",&ch);
		pthread_create(&oper,NULL,runner,&ch);
		status= pthread_join(oper,NULL);
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