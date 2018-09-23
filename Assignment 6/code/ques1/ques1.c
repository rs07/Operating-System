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

void *traverse()
{
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
}


void *add(void *ptri)
{
	//printf("Foo\n");
	int value= *((int *)ptri);
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
		printf("Successfully Added\n");
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
		printf("Successfully Added(2)\n");
		//free(tmp);
	}
}

void *del(void *ptri)
{
	int value= *((int *)ptri);
	struct node *ptr= root;
	//printf("Check\n");
	if(root==NULL)
	{
		fprintf(stderr, "Error: empty list\n");
		return;
	}
	if(ptr->data==value)
	{
		root= ptr->link;
		printf("Successfully Deleted\n");
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
}


int main()
{
	int ch;
	pthread_t oper;
	int value,status;
	while(1)
	{
		printf("1. For adding the value in the node\n");
		printf("2. For deleting the node\n");
		printf("3. For Traversing the LL\n");
		scanf("%d",&ch);
		switch(ch)
		{
			case 1: printf("Enter the value to be added: ");
					scanf("%d",&value);
					printf("Value: %d\n",value);
					status= pthread_create(&oper,NULL,add,&value);
					if(status!=0)
					{
						fprintf(stderr, "Error: pthread_create() failed\n");
					}
					break;
			case 2:	printf("Enter the value to be deleted: ");
					scanf("%d",&value);
					status= pthread_create(&oper,NULL,del,&value);
					if(status!=0)
					{
						fprintf(stderr, "Error: pthread_create() failed\n");
					}
					break;
			case 3: status= pthread_create(&oper,NULL,traverse,NULL);
					if(status!=0)
					{
						fprintf(stderr, "Error: pthread_create() failed\n");
					}
					break;
			default: exit(0);
		}
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
