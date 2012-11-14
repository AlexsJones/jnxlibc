#ifndef __LIST_H__
#define __LIST_H__
///A very simple linked list
typedef struct node{
	void *_data;
	struct node *next_node;
};
typedef struct list{
	struct node *head;
};
///Required argument is list pointer
struct list* list_make(void);
///Providing the pointer to the list and the data you wish to save as arguments, this will insert a node
void list_add(struct list *A,void *_datain);
///It is import to note that the struct pointer is just forgotten from the list and the data returned
///Required argument is the list pointer
void* list_remove(struct list *A);
///frees the nodes and the list pointer
void list_delete(struct list *A);
#endif
 
