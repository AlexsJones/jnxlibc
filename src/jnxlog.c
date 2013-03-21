/*
 * =====================================================================================
 *
 *       Filename:  jnx_log.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/20/13 10:17:02
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <sys/file.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include "jnxlog.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
pthread_mutex_t _locker;
char *log_path = NULL;

void jnx_write_to_log(char *message)
{
    pthread_mutex_lock(&_locker);
    FILE *fp = fopen(log_path,"a");
    if(fp == NULL) { 
        printf("Unable to open file for log writing\n");
        pthread_mutex_unlock(&_locker);
        return;
    };
    fprintf(fp,"%s",(char*)message);
    fclose(fp);
    //free our string
    pthread_mutex_unlock(&_locker);
}
int jnx_log_setup(char *path)
{
    struct stat s;
    int err = stat(path,&s);
    if(-1 == err)
    {
        printf("No log file at %s\n",path);
        printf("Creating new log\n");
        FILE *fp = fopen(path,"w+");
        if(fp == NULL)
        {
            printf("Error creating log file\n");
            return 1;
        }
        else
        {
            printf("Done\n");
            fclose(fp);
            //after our successful log creation, we'll assign the log_path
            log_path = path;
        } 

    }else
    {
        if(S_ISDIR(s.st_mode))
        {
            printf("Exists but is a directory\n");
            return 1;
        }
        printf("Found existing log, will continue logging\n");
        //assigning to our global log path
        log_path = path;
    } 
    return 0;
}
char* jnx_get_time()
{
    time_t t;
    char *buf;
    time(&t);
    buf = (char*)malloc(strlen(ctime(&t)) +1);
    snprintf(buf,strlen(ctime(&t)),"%s",ctime(&t));
    return buf;
}
void jnx_log(const char * format, ...)
{
    char buffer[1024];
    va_list ap;
    va_start(ap,format);
    vsprintf(buffer,format,ap);
    va_end(ap);
    pthread_t _thr;
    pthread_create(&_thr,NULL,(void*)jnx_write_to_log,buffer);    

}

