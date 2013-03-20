/*
 * =====================================================================================
 *
 *       Filename:  jnxbtree.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/05/2013 06:28:01 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author: Dragan Glumac 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "jnxbtree.h"

jnx_B_tree_node *new_node(int order, int is_leaf)
{
    jnx_B_tree_node *node = calloc(1, sizeof(jnx_B_tree_node));

    node->count = 0; // just to be explicit, calloc will already zero the field
    node->is_leaf = is_leaf;
    node->records = calloc(2 * order, sizeof(record*));
    node->children = calloc(2 * order + 1, sizeof(jnx_B_tree_node*)); 

    return node;
}

void move_contents_from_index(jnx_B_tree_node *source, jnx_B_tree_node *target, int index)
{
    // Copy the RHS half of the records and children to new node
    memmove((void *) target->records, 
            (const void*) (source->records + index),
            (source->count / 2) * sizeof(record *));
    memmove((void *) target->children,
            (const void*) (source->children + index),
            (source->count / 2 + 1) * sizeof(jnx_B_tree_node *));

    // Zero out the old records and children in the old node
    int new_count = source->count / 2;
    bzero((void *) (source->records + index - 1), (new_count + 1) * sizeof(record *));
    bzero((void *) (source->children + index), (new_count + 1) * sizeof(jnx_B_tree_node *));

    // Adjust record counts in the nodes
    source->count = new_count;
    target->count = new_count;
}

void shift_right_from_index(jnx_B_tree_node *node, int index)
{
    // Shift records to the right of i by one position
    memmove((void *)(node->records + (index + 1)),
            (const void*)(node->records + index),
            (node->count - index) * sizeof(record *));

    // Shift children to the rigth of i + 1 by one position
    memmove((void *)(node->children + (index + 2)),
            (const void*)(node->children + (index + 1)),
            (node->count - index + 1) * sizeof(jnx_B_tree_node *));
}

void shift_left_from_index(jnx_B_tree_node *node, int index)
{
    // Shift records to the right of i by one position
    memmove((void *)(node->records + index - 1),
            (const void*)(node->records + index),
            (node->count - index) * sizeof(record *));
    bzero((void *) (node->records + node->count - 1), sizeof(record *));
    
    memmove((void *)(node->children + index - 1),
            (const void*)(node->children + index),
            (node->count - index) * sizeof(jnx_B_tree_node *));
    bzero((void *) (node->children + node->count - 1), sizeof(jnx_B_tree_node *));
}

int is_node_full(jnx_B_tree *tree, jnx_B_tree_node *node)
{
    if ( node->count == 2 * tree->order - 1 )
    {
        return 1;
    }

    return 0;
}

int is_node_empty(jnx_B_tree_node *node)
{
    if ( node == NULL || node->count == 0 )
    {
        return 1;
    }

    return 0;
}

int find_index_for_record(jnx_B_tree *tree, jnx_B_tree_node *node, record *r)
{
    // Records are ordered so to make search efficient
    // (especially for arrays with lots of records)
    // we use binary chop, which is O(lg n) time.

    if ( node->count == 0 )
    {
        return 0;
    }

    int left_bound = 0;
    int right_bound = node->count - 1;
    int curr_index = (right_bound - left_bound) / 2; 

    do
    {
        void *key = node->records[curr_index]->key;
        int cf = tree->compare_function(key, r->key);

        if ( cf == 0 )
        {
            // Found exact match
            return curr_index;
        }

        if ( cf < 0 )
        {
            left_bound = curr_index + 1;
        }
        else
        {
            right_bound = curr_index - 1;
        }

        curr_index = left_bound + (right_bound - left_bound) / 2;
    }
    while ( right_bound >= left_bound );

    return curr_index;
}

void split_child_at_index(jnx_B_tree *tree, jnx_B_tree_node *node, int child_index)
{
    int tree_order = tree->order;

    jnx_B_tree_node *temp = node->children[child_index];
    jnx_B_tree_node *sibling = new_node(tree_order, temp->is_leaf);
    record *middle = temp->records[tree_order - 1];

    move_contents_from_index(temp, sibling, tree_order);

    // Now rearrange "node" to fit the new record and its children
    int i = find_index_for_record(tree, node, middle);
    if ( node->records[i] != NULL )
    {
        // Shift only if the record slot is not empty
        shift_right_from_index(node, i);
    }

    // move the middle record up
    node->records[i] = middle;
    // attach newly created sibling
    node->children[i + 1] = sibling;
    // finally, increment the record count in the current node
    node->count++;
}

void add_record_to_non_full_leaf(jnx_B_tree *tree, jnx_B_tree_node *node, record *r)
{
    int i = find_index_for_record(tree, node, r);

    if ( node->records[i] == NULL )
    {
        node->records[i] = r;
        node->count++;
    }
    else
    {
        void *key = node->records[i]->key;

        if ( tree->compare_function(key, r->key) == 0 )
        {
            // Replace the value only scenario
            free(node->records[i]);
            node->records[i] = r;
        }
        else
        {
            shift_right_from_index(node, i);
            node->records[i] = r;
            node->count++;
        }
    }  
}

/* 
 * To guarantee that leaf node is never full, we split any full
 * node on the way down the tree, find the appropriate subtree
 * to traverse, and insert the node into the subtree.
 *
 * The only special case is if the root is a leaf node. We handle
 * this in jnx_B_tree_add API function.
 */
void insert_into_tree_at_node(jnx_B_tree *tree, jnx_B_tree_node *node, record *r)
{
    if ( node->is_leaf )
    {
        add_record_to_non_full_leaf(tree, node, r);
        return;
    }

    int i = find_index_for_record(tree, node, r);

    if ( i < node->count )
    {
        if ( tree->compare_function(node->records[i]->key, r->key) == 0 )
        {
            // Same key so just replace the old record with the new one
            free(node->records[i]);
            node->records[i] = r;
            return;
        }
    }

    if ( is_node_full(tree, node->children[i]) )
    {
        split_child_at_index(tree, node, i);

        if ( tree->compare_function(node->records[i]->key, r->key) < 0 )
        {
            // We're going to the right of the record that moved up
            i++;
        }
    }

    // Recurse down into the appropriate subtree
    insert_into_tree_at_node(tree, node->children[i], r);
}

void delete_node_and_subtrees(jnx_B_tree_node *node)
{
    if ( node == NULL )
    {
        return;
    }

    // Delete all the children first
    int i;
    for ( i = 0; i <= node->count; i++ )
    {
        if ( node->children[i] != NULL )
        {
            delete_node_and_subtrees(node->children[i]);
        }
    }

    // Free memory for all the records
    for (i = 0; i < node->count; i++ )
    {
        if ( node->records[i] != NULL )
        {
            free(node->records[i]);
        }
    }

    // Finally, free the node itself
    free(node);
}

void *find_value_for_key_in_node(jnx_B_tree *tree, jnx_B_tree_node *node, void *key)
{
    if ( is_node_empty(node) )
    { 
        return NULL;
    }

    record r;
    r.key = key;

    int i =  find_index_for_record(tree, node, &r);

    if ( node->records[i] != NULL )
    {
        if ( tree->compare_function(r.key, node->records[i]->key) == 0 )
        {
            return node->records[i]->value;
        }
    }

    return find_value_for_key_in_node(tree, node->children[i], key);
}

record *find_rightmost_record_in_subtree_at_node(jnx_B_tree_node *node)
{
    jnx_B_tree_node *temp = node;
    
    while ( temp->children[temp->count] != NULL )
    {
        temp = temp->children[temp->count];
    }

    record *rec = temp->records[temp->count - 1];
    temp->records[temp->count - 1] = NULL;

    return rec;
}

record *find_leftmost_record_in_subtree_at_node(jnx_B_tree_node *node)
{
    jnx_B_tree_node *temp = node;
    
    while ( temp->children[0] != NULL )
    {
        temp = temp->children[0];
    }

    record *rec = temp->records[0];
    temp->records[0] = NULL;
    shift_right_from_index(temp, 1);

    return rec;
}

void merge_subtrees_around_index(jnx_B_tree_node *node, int index)
{
    jnx_B_tree_node *first = node->children[index];
    jnx_B_tree_node *second = node->children[index + 1];

    first->records[first->count] = node->records[index];
    first->count++;

    memmove((void *)(first->records + first->count + 1),
            (const void *)(second->records),
            second->count * sizeof(record));

    memmove((void *)(first->children + first->count + 1),
            (const void *)(second->children),
            (second->count + 1) * sizeof(jnx_B_tree_node *));
    
    // Adjust the count
    first->count += second->count;

    // Fill the gap in node by shifting records and children
    // that are to the right of index by 1 position to the left.
    shift_left_from_index(node, index + 1);

    // Delete the second node, i.e. free its memory on the heap
    free(second);
}

void delete_record_from_node(jnx_B_tree *tree, jnx_B_tree_node *node, record *r)
{
    int i = find_index_for_record(tree, node, r);
    
    if ( node->is_leaf )
    {
        if ( node == tree->root || node->count >= tree->order)
        {
            if ( tree->compare_function(node->records[i]->key, r->key) == 0 )
            {
                record *temp = node->records[i];
                shift_left_from_index(node, i + 1);
                node->count--;

                free(temp);
            }

            return;
        }
    }

    record *node_rec = node->records[i];
    record *temp = NULL;

    if ( tree->compare_function(node_rec->key, r->key) == 0 )
    {
        // It is safe not to check for NULL children here because
        // we are in an internal node, and since by definition
        // a B-tree grows from the leaves up, and the only way to
        // add a level is to split a node at the previous, a record
        // in an inner node will have both left and right subtrees.
 
        if ( node->children[i]->count >= tree->order )
        {
            temp = find_rightmost_record_in_subtree_at_node(node->children[i]);
            node->records[i] = temp;
        }
        else if ( node->children[i+1]->count >= tree->order )
        {
            temp = find_leftmost_record_in_subtree_at_node(node->children[i + 1]);
            node->records[i] = temp;
        }
        else
        {
            merge_subtrees_around_index(node, i);
        }
       
        return;
    }

    delete_record_from_node(tree, node->children[i], r);
}


/*
 * ===========================================
 *  API functions implementations start here!
 * ===========================================
 */

jnx_B_tree* jnx_B_tree_init(int order, compare_keys callback)
{
    if ( order <= 1 )
    {
        return NULL;
    }

    jnx_B_tree *tree = calloc(1, sizeof(jnx_B_tree));

    tree->order = order;
    tree->compare_function = callback;
    tree->root = new_node(tree->order, 1);

    return tree; 
}

void jnx_B_tree_add(jnx_B_tree *tree, void *key, void *value)
{
    if ( tree == NULL)
    {
        return;
    }

    record *r = malloc(sizeof(record));
    r->key = key;
    r->value = value;

    if ( is_node_full(tree, tree->root) )
    {
        jnx_B_tree_node *temp = tree->root;
        tree->root = new_node(tree->order, 0);
        tree->root->children[0] = temp;

        split_child_at_index(tree, tree->root, 0);
    }

    insert_into_tree_at_node(tree, tree->root, r);
}

void *jnx_B_tree_lookup(jnx_B_tree *tree, void *key)
{
    if ( tree == NULL )
    {
        return NULL;
    }

    return find_value_for_key_in_node(tree, tree->root, key);
}

void jnx_B_tree_remove(jnx_B_tree *tree, void *key)
{
    if ( tree == NULL || tree->root->count == 0 )
    {
        return;
    }

    record *r = malloc(sizeof(record));
    r->key = key;
    r->value = key;

    delete_record_from_node(tree, tree->root, r);

    free(r);
}

void jnx_B_tree_delete(jnx_B_tree* tree)
{
    if ( tree == NULL )
    {
        return;
    }

    delete_node_and_subtrees(tree->root);

    free(tree);
}
