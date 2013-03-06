/*
 * =====================================================================================
 *
 *       Filename:  jnxbplustree.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/05/2013 06:28:01 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Dragan Glumac (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef _JNX_B_TREE_
#define _JNX_B_TREE_

typedef struct {
    void *key;
    void *value;
} datum;

/*
 * Callback function that the user needs to supply and which compares keys of two nodes.
 *
 * The B-tree keys are required to follow some order, so with respect to that order
 * the function should return:
 *     - an integer < 0 if first parameter comes before second parameter
 *     - a 0 if first and second parameters are equal
 *     - an integer > 0 if the first parameter comes after the second parameter
 * 
 * Typically, these values can be -1, 0, and 1 respectivelly.
 */
typedef int (*compare) (datum*, datum*);

typedef struct {
    int count;
    datum **data;
    void **children;
} jnx_B_tree_node;

typedef struct {
    jnx_B_tree_node *root;
} jnx_B_tree;

jnx_B_tree* jnx_B_tree_init(int fanout, compare callback);

#endif // _JNX_B_TREE_
