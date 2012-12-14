#include "lists.h"

/******************************************************
	standard List functions
*******************************************************/

/****
*
*	create_LinkedList:	void
*		allocate the memory space for a list wrapper
*		and return it
*
*****/
LinkedList *create_LinkedList( void )
{
	LinkedList *ll;

	if((ll=(LinkedList *)malloc(sizeof(LinkedList))) == NULL)
	{
		fprintf(stderr,"\nCould not allocate enough space for linked list!\n");
		exit(1);
	}

	ll->left=NULL;
	ll->node=NULL;
	ll->right=NULL;

	return ll;
}

/*************
*					Standard Linked List functions
*************/
/******
*
*	append_LinkedList:	LinkedList *, node *
*		Append a given node to a linked list by creating a LinkedList
*		wrapper for the node and tagging it on the back of the given
*		LinkedList.
*		return the head of the LinkedList
*
******/
LinkedList *append_LinkedList( LinkedList *head, void *node )
{
	LinkedList *llp;
	LinkedList *wrapping;

	/* create wrapping */
	wrapping=create_LinkedList();
	wrapping->node=node;

	if(head==NULL)	/* blank linked list */
	{
		head=wrapping;
	}
	else
	{
		
		/* find end of list */
		llp=head;
		while(llp->right!=NULL)
			llp=llp->right;

		/* join */
		llp->right=wrapping;
		wrapping->left=llp;
	}

	return head;
}

/******
*
*	delete_LinkedList:	LinkedList *, function * used to delete nodes 
*		Given a List and a function to delete a single node
*		loop around the list deleting the nodes and the wrappers
*
******/
void delete_LinkedList( LinkedList *llp, void (*func)())
{
	LinkedList *loop, *temp;
	
	loop=llp;

	while(loop!=NULL)
	{
		(*func)(loop->node);
		temp=loop;
		loop=loop->right;
		free(temp);
	}

}

/***********************************************************
	Circular list functions, uses standard LinkedList wrapper
************************************************************/
/******
*
*	append_CircularList:	LinkedList *, node *
*		Append a given node to a circular list by creating a LinkedList
*		wrapper for the node and tagging it on the back of the given
*		LinkedList. The back is the portion which loops around to
*		the head which is supplied as the LinkedList argument
*		return the head of the LinkedList
*
******/
LinkedList *	append_CircularList( LinkedList *head, void *node)
{
	LinkedList *llp;
	LinkedList *wrapping;

	/* create wrapping */
	wrapping=create_LinkedList();
	wrapping->node=node;

	if(head==NULL)	/* blank linked list */
	{
		wrapping->left=wrapping;
		head=wrapping;
		wrapping->right=wrapping;
	}
	else
	{
		
		/* find end of list */
		llp=head;
		while(llp->right!=head)
			llp=llp->right;

		/* join */
		llp->right=wrapping;
		wrapping->left=llp;
		wrapping->right=head;
		head->left=wrapping;
	}

	return head;
}


/******
*
*	delete_CircularList:	LinkedList *, function * used to delete nodes 
*		Given a List and a function to delete a single node
*		loop around the list deleting the nodes and the wrappers
*
******/
void delete_CircularList( LinkedList *llp, void (*func)())
{
	LinkedList *loop, *temp, *head;
	
	loop=llp;
	head=llp;

	if(loop!=NULL)
	{
		(*func)(loop->node);
		temp=loop;
		loop=loop->right;
		free(temp);
	}
	
	while(loop!=head)
	{
		(*func)(loop->node);
		temp=loop;
		loop=loop->right;
		free(temp);
	}

}
