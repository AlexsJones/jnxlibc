/*
 * =====================================================================================
 *
 *       Filename:  jnx_thread.c
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
#include "jnxlog.h"
#include "jnxcheck.h"
jnx_thread_mutex* jnx_thread_mutex_create() {
  jnx_thread_mutex *m = malloc(sizeof(jnx_thread_mutex));
#ifndef _WIN32 || _WIN64
  pthread_mutex_init(&m->system_mutex, NULL);
#else
  m->system_mutex = CreateMutex(NULL, FALSE, NULL);
#endif
  m->is_initialized = 1;
  return m;
}
void jnx_thread_mutex_destroy(jnx_thread_mutex **m) {
  JNXCHECK(*m);
  if((*m)->is_initialized) {	
#ifndef _WIN32 || _WIN64
	  pthread_mutex_destroy(&(*m)->system_mutex);
#else
	  CloseHandle(&(*m)->system_mutex);
#endif
    (*m)->is_initialized = 0;
  }
  free(*m);
  *m = NULL;
}
jnx_int32 jnx_thread_unlock(jnx_thread_mutex *m) {
  JNXCHECK(m);
  jnx_int32 ret = 0;

#ifndef _WIN32 || _WIN64
  ret = pthread_mutex_unlock(&m->system_mutex);
#else
  ret = ReleaseMutex(&m->system_mutex) > 0 ? 1 : 0;
#endif
  return ret;
}
#ifndef _WIN32 || _WIN64
jnx_int32 jnx_thread_trylock(jnx_thread_mutex *m) {
  JNXCHECK(m);
  jnx_int32 ret = 0;
  if(m->is_initialized != 1) {
    m = jnx_thread_mutex_create();
  }
  ret = pthread_mutex_trylock(&m->system_mutex);
  return ret;
}
#endif
#ifndef _WIN32 || _WIN64
void 
#else
HANDLE
#endif
jnx_thread_lock(jnx_thread_mutex *m) {
  JNXCHECK(m);
  if(m->is_initialized != 1) {
    m = jnx_thread_mutex_create();
  }

#ifndef _WIN32 || _WIN64
  pthread_mutex_lock(&m->system_mutex);
#else
  return m->system_mutex;
#endif
}
void jnx_thread_handle_destroy(jnx_thread *thr) {
  JNXCHECK(thr);
  if(thr == NULL) {
    return;
  }
  /*
  if(thr->attributes->has_custom_attr) {
    pthread_attr_destroy(thr->attributes->system_attributes);
  }
  */
  free(thr->attributes);
  free(thr);
}
jnx_thread* jnx_thread_create(
#ifndef _WIN32 || _WIN64
	entry_point e,
#else
	LPTHREAD_START_ROUTINE e,
#endif
	void *args) {
  jnx_thread *thr = malloc(sizeof(jnx_thread));
  jnx_thread_attributes *attr = malloc(sizeof(jnx_thread_attributes));
  attr->has_custom_attr = 0;
  thr->attributes = attr;
  //platform specific zone//
#ifndef _WIN32 || _WIN64
  pthread_attr_t *default_attr = NULL;
  pthread_create(&thr->system_thread,default_attr,e,args);
#else
  thr->system_thread = CreateThread(NULL, 0, e, NULL, 0, NULL);
#endif
  //platform specific zone//
  return thr;
}
jnx_int32 jnx_thread_create_disposable(
#ifndef _WIN32 || _WIN64
	entry_point e,
#else
	LPTHREAD_START_ROUTINE e,
#endif
	void *args) {
  jnx_int32 ret = 0;
  //platform specific zone//
#ifndef _WIN32 || _WIN64
  pthread_t disposable_thr;
  pthread_attr_t *default_attr = NULL;
  ret = pthread_create(&disposable_thr,default_attr,e,args);
#else
  CreateThread(NULL, 0, e, NULL, 0, NULL);
#endif
  //platform specific zone//
  return ret;
}
jnx_int32 jnx_thread_join(jnx_thread *thr, void **data) {
  JNXCHECK(thr);
  jnx_int32 ret = 0;
  //platform specific zone//
  ret = pthread_join(thr->system_thread,data);

#ifndef _WIN32 || _WIN64
  ret = pthread_join(thr->system_thread, data);
#else
  WaitForSingleObject(thr->system_thread, INFINITE);
#endif
  //platform specific zone//
  return ret;
}
