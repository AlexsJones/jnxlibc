/*
 * =====================================================================================
 *
 *       Filename:  jnxlog.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  18/08/2014 18:34:40
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jonesax (jonesax@hush.com), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdarg.h>
#include "jnxlog.h"
#include "jnxcheck.h"
#include "jnxfile.h"

#define MAX_SIZE 2048
#define TIMEBUFFER 256
jnx_log_config* jnx_log_create(const char *path,jnx_log_type output){  
  jnx_log_config *conf = malloc(sizeof(jnx_log_config));
  conf->log_path = path;
  conf->output = output;
  conf->pstart = malloc(sizeof(struct timeval));
  conf->pend = malloc(sizeof(struct timeval));
  gettimeofday(conf->pstart,NULL);
  conf->pcurrent = 0;
  conf->internal_lock = jnx_thread_mutex_create();
  return conf;
}
void jnx_log(jnx_log_config *config, const char *file, const char *function,const int32_t line,const char *format,...){
  JNXCHECK(file);
  JNXCHECK(function);
  JNXCHECK(format);
  char buffer[MAX_SIZE];
  char msgbuffer[MAX_SIZE];
  memset(buffer,0,MAX_SIZE);
  memset(msgbuffer,0,MAX_SIZE);
  va_list ap;
  va_start(ap,format);
  vsprintf(msgbuffer,format,ap);
  va_end(ap);
  if(config == DEFAULT_CONTEXT) {
    time_t ptime;
    time(&ptime);
    char pbuffer[TIMEBUFFER];
    sprintf(pbuffer,"%s",ctime(&ptime));
    pbuffer[strlen(pbuffer)-1] = '\0';
    sprintf(buffer,"[%s][%s:%d][t:%s]%s\n",file,function,line,pbuffer, msgbuffer);
    printf("%s",buffer);
    return;
  }
  sprintf(buffer,"[%s][%s:%d][t:%f]%s\n",file,function,line,config ? config->pcurrent : 0, msgbuffer);
  switch(config->output) {
    case FILETYPE:
      JNXCHECK(config->log_path);
      jnx_thread_lock(config->internal_lock);
      jnx_file_write((char*)config->log_path ? (char*)config->log_path : "default.log",buffer,strlen(buffer),"a");
      jnx_thread_unlock(config->internal_lock);
      break;
    case CONSOLETYPE:
      printf("%s",buffer);
      break;
  }
  gettimeofday(config->pend,NULL);
  double elapsed_time = ((*config->pend).tv_sec - (*config->pstart).tv_sec) * 1000.0;
  elapsed_time += ((*config->pend).tv_usec - (*config->pstart).tv_usec) / 1000.0;
  config->pcurrent = elapsed_time;
}
void jnx_log_destroy(jnx_log_config **config){
  JNXCHECK(*config);
  jnx_thread_mutex_destroy(&(*config)->internal_lock);
  free((*config)->pstart);
  free((*config)->pend);
  free(*config);
  *config = NULL;
}
