/*
 * =====================================================================================
 *
 *       Filename:  jnxthread.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/12/14 08:14:48
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jonesax (jonesax@hush.com), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include "jnxthread.h"
#include <jnxc_headers/jnxbtree.h>

static jnx_btree *pooltree = NULL;
static int threadcount  = 1;

int thread_id_comparison(void *A, void *B)
{
	if(A > B) { return -1; }
	if(B > A) { return 1; }
	return 0;	
}
void jnxthread_poolflush()
{
	if(!pooltree)
	{
		return;
	}
	threadcount = 1;
	jnx_list *keys = jnx_list_create();
	jnx_btree_keys(pooltree,keys);
	while(keys->head)
	{
		jnxthread *thr = keys->head->_data;
		jnxthread_destroy(thr);

		keys->head = keys->head->next_node;
	}
	jnx_list_destroy(&keys);
	jnx_btree_destroy(pooltree);
	pooltree = NULL;
}
size_t jnxthread_poolcount()
{
	if(!pooltree)
	{
		return 0;
	}
	jnx_list *keys = jnx_list_create();
	jnx_btree_keys(pooltree,keys);

	size_t s = keys->counter;
	jnx_list_destroy(&keys);
	return s;
}
int jnxthread_addpool(jnxthread *thr)
{
	if(!pooltree)
	{ 
		pooltree = jnx_btree_create(sizeof(int),thread_id_comparison); 
	};

	jnx_btree_add(pooltree,&thr->id,thr);

	return 0;
}
int jnxthread_unlock(jnxthread_mutex *m)
{
	int ret = 0;
#if defined(WIN32)
	ret = pthread_mutex_unlock(&m->system_mutex);
#endif
	return ret;
}
int jnxthread_trylock(jnxthread_mutex *m)
{
	int ret = 0;
#if !defined(WIN32)
	ret = pthread_mutex_trylock(&m->system_mutex);
#endif
	return ret;
}
void jnxthread_lock(jnxthread_mutex *m)
{
#if !defined(WIN32)
	pthread_mutex_lock(&m->system_mutex);
#endif
}
void jnxthread_destroy(jnxthread *thr)
{
#if defined(WIN32)
	printf("Not implemented\n");
	return NULL;
#endif
	if(thr == NULL)
	{
		return;
	}
	if(thr->attributes->has_custom_attr)
	{
#if !defined(WIN32)
		pthread_attr_destroy(thr->attributes->system_attributes);
#endif
	}
	free(thr->attributes);
	free(thr);
}
jnxthread* jnxthread_create(entry_point e,void *args)
{
#if defined(WIN32)
	printf("Not implemented\n");
	return NULL;
#endif
	jnxthread *thr = malloc(sizeof(jnxthread));
	jnxthread_attributes *attr = malloc(sizeof(jnxthread_attributes));
	attr->has_custom_attr = 0;
	thr->attributes = attr;
	thr->id = threadcount++;  
	//platform specific zone//
#if !defined(WIN32)
	pthread_attr_t *default_attr = NULL;
	pthread_create(&thr->system_thread,default_attr,e,args);
#endif
	//platform specific zone//
	jnxthread_addpool(thr);
	return thr;
}
int jnxthread_create_disposable(entry_point e,void *args)
{
	int ret = 0;
#if defined(WIN32)
	printf("Not implemented\n");
	return NULL;
#endif
	//platform specific zone//
#if !defined(WIN32)
	pthread_t disposable_thr;
	pthread_attr_t *default_attr = NULL;
	ret = pthread_create(&disposable_thr,default_attr,e,args);
#endif
	//platform specific zone//
	return ret;
}
int jnxthread_join(jnxthread *thr, void **data)
{
#if defined(WIN32)
	printf("Not implemented\n");
	return NULL;
#endif
	int ret = 0;
	//platform specific zone//
#if !defined(WIN32)
	ret = pthread_join(thr->system_thread,data);
#endif
	//platform specific zone//
	return ret;
}
