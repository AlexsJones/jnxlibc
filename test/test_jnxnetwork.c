/*
 * =====================================================================================
 *
 *       Filename:  test_jnxnetwork.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/23/13 16:06:18
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Alex Jones (), alexsimonjones@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include "jnxlog.h"
#include "jnxnetwork.h"
#include "jnxterm.h"
#include <pthread.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
void network_listener_callback(char *message)
{
    assert(strcmp(message,"Hello there") == 0);
    jnx_term_JNX_LOGC_in_color(JNX_COL_GREEN, "  OK\n");
    exit(0);
}
void *worker_thread(void*args)
{
    jnx_network_broadcast_listener_callback c = &network_listener_callback;
    jnx_network_broadcast_listener(12345,"225.0.0.37",c);    
}
int main(int argc, char **argv)
{
    JNX_LOGC("Running tests for jnxnetwork\n");
    pthread_t threadone;
    JNX_LOGC("- test multicast");
    pthread_create(&threadone, NULL, &worker_thread, NULL); 
    sleep(1);
    jnx_network_send_broadcast(12345,"225.0.0.37","Hello there");
    sleep(2);

    JNX_LOGC("Network tests completed.\n");
    return 0;
}
