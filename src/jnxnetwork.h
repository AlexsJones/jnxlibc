#ifndef __JNXNETWORK_H__
#define __JNXNETWORK_H__
//This is the maximum char buffer size, though is also linked to the read buffer size
#define MAXBUFFER 4028
/*
This is designed to be a highly simplified C network function set
Currently operates via char buffer transmission
*/
///function pointer callback
///usage Callback = &function;
///(*Callback)(receiver_buffer);
typedef void (*jnx_listener_callback)(char *msg);
///Setup listener will create a socket listener on the selected port
///Requires both (int) port and void(*Callback)(char*) functor as arguments
int jnx_setup_listener(int port, void (*Callback)(char *));
///Cancels the socket and closes it
void jnx_cancel_listener(void);
///Send message uses a simple charbuffer over write socket to the desired host-port
///Requires (char*)host (int) port and (char*) msg
///It is not necessary to free host or message
int jnx_send_message(char *host, int port, char *msg);
///Returns the local machine ip for the selected interface
char *jnx_local_ip(char *interface);
#endif
