#ifndef _LINKED_LISTS_H
#define _LINKED_LISTS_H

#include <stdio.h>


/*************************************************
			Linked List Structure
			8/2/94
**************************************************/

typedef struct{
					struct LinkedList *left;
					void *node;
					struct LinkedList *right;
					}LinkedList;

extern LinkedList *	create_LinkedList( void );

extern LinkedList *	append_LinkedList( LinkedList *, void *);
extern void				delete_LinkedList( LinkedList *, void (*)());

extern LinkedList *	append_CircularList( LinkedList *, void *);
extern void				delete_CircularList( LinkedList *, void (*)());

#endif