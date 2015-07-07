/*
 * =====================================================================================
 *
 *       Filename:  test_jnxhash.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  02/27/13 16:20:01
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Alex Jones (), alexsimonjones@gmail.com
 *   Organization:
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <sys/stat.h>
#include "jnxnetwork.h"
#include "jnxlog.h"
#include "jnxterm.h"

struct sockaddr* mapping_filter(struct ifaddrs* ifs) {
  struct sockaddr *ip = ifs->ifa_addr;
  return ip;
}
void test_endian() {
  JNXLOG(LDEBUG,"- displaying endianness\n");
  JNXLOG(LDEBUG,"BIG ENDIAN %s\n",jnx_network_get_endianness() == JNX_BIG_ENDIAN ? "YES" : "NO");
  JNXLOG(LDEBUG,"LITTLE ENDIAN %s\n",jnx_network_get_endianness() == JNX_LITTLE_ENDIAN ? "YES" : "NO");
  JNXLOG(LDEBUG,"OK");
}
void test_ip() {
  JNXLOG(LDEBUG, "- test_ip");
  jnx_char *buffer = calloc(16, sizeof(jnx_char));
  jnx_network_fetch_local_ipv4(buffer,mapping_filter);
  JNXLOG(LDEBUG,"Local ipv4 address => %s",buffer);
  free(buffer);
  buffer = calloc(16,sizeof(jnx_char));
  jnx_network_fetch_local_ipv6(buffer,mapping_filter);
  JNXLOG(LDEBUG,"Local ipv6 address => %s",buffer);
  free(buffer);
  JNXLOG(LDEBUG,"OK");
}
int test_jnxnetwork(int args, char **argv) {
  JNXLOG(LDEBUG,"Running network tests...\n");
  test_endian();
  test_ip();
  JNXLOG(LDEBUG,"OK");
  return 0;
}
