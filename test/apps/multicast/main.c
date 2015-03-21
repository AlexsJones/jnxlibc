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
#include <stdio.h>
#include "jnx_udp_socket.h"
#include "jnxthread.h"
#include "jnxcheck.h"
#include "jnxlog.h"
#include "jnxnetwork.h"

#define TESTPORT "9090"
#define BGROUP "225.1.1.1"

int test_udp_listener_complete = 0;

/* Function prototypes */
struct sockaddr *filter_local_ip_address(struct ifaddrs *addr); 
void *worker_multicast(void *args); 
void fire_threaded_udp_packet_multicast(char *port); 
void test_udp_listener_callback(const jnx_uint8 *payload,
    jnx_size bytes_read, void *args);
void test_udp_multicast();

int main(int argc, char **argv) {
  test_udp_multicast();
  return 0;
}

void test_udp_multicast() {
  char *ip = calloc(16,sizeof(jnx_char));
  jnx_network_fetch_local_ipv4(ip,filter_local_ip_address);
  JNX_LOG(0,"Using IP => %s",ip);
  jnx_udp_listener *listener = 
    jnx_socket_udp_listener_multicast_create(TESTPORT,AF_INET,ip,BGROUP);
  
  fire_threaded_udp_packet_multicast(TESTPORT);
  while(1) {
    jnx_socket_udp_listener_tick(listener,test_udp_listener_callback,NULL);
    if(test_udp_listener_complete)break;
  }
  jnx_socket_udp_listener_destroy(&listener);
  JNXCHECK(test_udp_listener_complete);
  JNXCHECK(listener == NULL);
}
void test_udp_listener_callback(const jnx_uint8 *payload,
    jnx_size bytes_read, void *args) {
  JNX_LOG(0,"test_udp_listener_callback achieved");
  test_udp_listener_complete = 1;
}
void fire_threaded_udp_packet_multicast(char *port) {
  jnx_thread_create_disposable(worker_multicast,port);
}
void *worker_multicast(void *args) {
  char *port = (char*)args;
  jnx_socket *t = jnx_socket_udp_create(AF_INET);
  char *ip = calloc(16,sizeof(jnx_char));
  jnx_network_fetch_local_ipv4(ip,filter_local_ip_address);
  JNX_LOG(0,"Using IP => %s",ip);
  jnx_socket_udp_multicast_send(t,BGROUP,port,(jnx_uint8 *) "ping",5);
  return NULL;
}
struct sockaddr *filter_local_ip_address(struct ifaddrs *addr) {
  struct sockaddr *ip = addr->ifa_addr;
  return ip;
}

