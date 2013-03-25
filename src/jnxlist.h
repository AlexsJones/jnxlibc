#ifndef __JNXLIST_H__
#define __JNXLIST_H__
/** @file jnxlist.h
 *  @brief API for jnxlibc implementation of list
 */

/**
 * @brief Node represents a link in the list 
 */
typedef struct node {
    void* _data;
    struct node* next_node;
} jnx_node;
/**
 * @brief Represents the list data structure as seen by the user
 */
typedef struct list {
    struct node* head;
    int counter;
} jnx_list;

/** @fn jnx_list_init(void)
 *  @brief Returns newly created list
 *  @return jnx_list* to a newly created list of 0 length
 */
jnx_list* jnx_list_init(void);

/** @fn jnx_list_add(jnx_list *A, void *_datain)
 *  @brief  Accepts a void pointer for input with list
 *  @param A is the target list
 *  @param _datain is the data to insert
 */
void jnx_list_add(jnx_list* A, void* _datain);

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  jnx_list_remove
 *  Description:  Removes the structure from list but returns the data for the user to handle
 * =====================================================================================
 */
void* jnx_list_remove(jnx_list* A);

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  jnx_list_delete
 *  Description:  Deletes the list node structure and list, not data inside
 * =====================================================================================
 */
void jnx_list_delete(jnx_list* A);
#endif

