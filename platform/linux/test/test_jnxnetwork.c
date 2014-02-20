/*
 * =====================================================================================
 *
 *       Filename:  test_jnxnetwork.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/20/14 12:57:51
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jonesax (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include "jnxnetwork.h"
#include "jnxthread.h"
#include "jnxlog.h"
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
static int PORT = 9030;
static char* HOST = "localhost";
unsigned int FAMILY = AF_INET6;
static int iscompleted = 0;
static jnx_socket *server_socket;
static jnx_socket *write_socket;
void test_create()
{
	JNX_LOGC("Test create:");
	jnx_socket *s = jnx_network_socket_create(FAMILY,SOCK_STREAM);
	assert(s != NULL);
	jnx_network_socket_destroy(s);
	JNX_LOGC(" Ok\n");
}
void cb(char *msg, size_t bytes, char *ip)
{
	iscompleted = 1;
	assert(strcmp("Hello!",msg) == 0);

	free(msg);
	exit(0);
}
void *server_loop(void *args)
{
	listen_callback c = cb;
	server_socket = jnx_network_socket_create(FAMILY,SOCK_STREAM);
	jnx_network_listen(server_socket,PORT,4,c);
	
}
void test_write()
{

	JNX_LOGC("Test write[%u]:",FAMILY);
	//set up our server component
	jnxthread_create_disposable(server_loop,NULL);		
	write_socket = jnx_network_socket_create(FAMILY,SOCK_STREAM);
	assert(write_socket != NULL);
	size_t sent = jnx_network_send(write_socket,HOST,PORT,"Hello!",strlen("Hello!"));
	JNX_LOGC(" Ok\n");
}
void test_sequence()
{
	test_create();
	test_write();

	int elapsed = 0;
	time_t s;
	time(&s);	
	while(iscompleted != 1 && elapsed < 5)
	{
		sleep(1);
		time_t e;
		time(&e);
		elapsed = e - s;
	}
	assert(elapsed <= 5);

}
int main(int argc, char **argv)
{
	JNX_LOGC("Starting networking tests...\n");
	/*  */
	JNX_LOGC("IPV4 tests...\n");

	FAMILY = AF_INET;
	PORT = 2501;	

	test_sequence();

	jnx_network_socket_destroy(server_socket);
	jnx_network_socket_destroy(write_socket);

	JNX_LOGC("IPV6 tests...\n");
	
	iscompleted = 0;	
	FAMILY = AF_INET6;
	PORT = 6903;	

	test_sequence();

	jnx_network_socket_destroy(server_socket);
	jnx_network_socket_destroy(write_socket);


	return 0;
}
