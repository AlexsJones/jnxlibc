/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  Main for multicast application for testing jnx_udp_socket multicast
 *                  across multiple machines.
 *
 *        Version:  1.0
 *        Created:  20/03/2015 14:40:23
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Dragan Glumac (DG), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include "jnx_udp_socket.h"
#include "jnxthread.h"
#include "jnxcheck.h"
#include "jnxlog.h"

#define TESTPORT "9090"
#define BGROUP "225.1.1.1"

static int test_udp_listener_complete = 0;

void *worker_multicast(void *args) {
  char *port = (char*)args;
  jnx_socket *t = jnx_socket_udp_create(AF_INET);
  jnx_socket_udp_send(t,BGROUP,port,"ping",5);
}
void fire_threaded_udp_packet_multicast(char *port) {
  jnx_thread_create_disposable(worker_multicast,port);
}
void test_udp_listener_callback(jnx_uint8 *payload,
    jnx_size bytes_read, jnx_socket *s, jnx_int connected_socket,void *args){
  JNX_LOG(NULL,"test_udp_listener_callback achieved");
  test_udp_listener_complete = 1;
}
void test_udp_multicast(){
  jnx_udp_listener *listener = 
    jnx_socket_udp_listener_multicast_create(TESTPORT,AF_INET,BGROUP);

  fire_threaded_udp_packet_multicast(TESTPORT);
  int x = 0;
  while(1) {
    jnx_socket_udp_listener_tick(listener,test_udp_listener_callback,NULL);
    if(test_udp_listener_complete)break;
  }
  jnx_socket_udp_listener_destroy(&listener);
  JNXCHECK(test_udp_listener_complete);
  JNXCHECK(listener == NULL);
}
int main(int argc, char **argv) {
  return 0;
}
