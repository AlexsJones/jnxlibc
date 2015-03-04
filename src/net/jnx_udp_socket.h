/*
 * jnx_udp_socket.h
 * Copyright (C) 2015 tibbar <tibbar@debian>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef JNX_UDP_SOCKET_H
#define JNX_UDP_SOCKET_H
#include "jnxsocket.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef jnx_int32 (*udp_socket_listener_callback)(jnx_uint8 *payload, jnx_size bytesread,\
    jnx_socket *s);
typedef jnx_int32 (*udp_socket_listener_callback_with_context)(jnx_uint8 *payload, jnx_size bytesread,\
    jnx_socket *s, void *context);
  /**
   * @fn jnx_socket *jnx_socket_udp_create(jnx_unsigned_int addrfamily)
   * @brief creates a jnx udp socket
   * @param addrfamily this is the address family to use (e.g. AF_INET)
   * @return jnx_socket
   */
  jnx_socket *jnx_socket_udp_create(jnx_unsigned_int addrfamily);
  /**
   * @fn jnx_char *jnx_socket_udp_resolve_ipaddress(struct sockaddr_storage sa);
   * @brief if successful it will return a string displaying the readable IP address
   * @param sa is the sockaddr_stoage to pass through from the accept call
   * @return jnx_char string or NULL on error
   */   
  jnx_char *jnx_socket_udp_resolve_ipaddress(struct sockaddr_storage sa);
  /**
   *@fn jnx_size jnx_socket_udp_enable_broadcast(socket *s)
   *@brief this IPV4 option will enable udp broadcasting
   *@param s is the socket object to enable broadcasting
   *@return 0 on success, -1 on error
   */
  jnx_size jnx_socket_udp_enable_broadcast_send_or_listen(jnx_socket *s);
  /**
   *@fn jnx_size jnx_socket_udp_enable_multicast_send
   (jnx_socket *s, jnx_char *interface, jnx_int32 ignore_local)
   *@brief This is function will enable the given socket to transmit multicast packets over domain
   *@param s is the socket to enable multicast sending on
   *@param interface is the IP address of the interface to use on the local machine
   *@param ignore_local is a flag to either disable local multicast loopback
   *@param returns 0 on success
   */
  jnx_size jnx_socket_udp_enable_multicast_send(jnx_socket *s,\
    jnx_char *interface, jnx_int32 ignore_local);
  /**
   *@fn jnx_size jnx_socket_udp_enable_multicast_listen
   (jnx_socket *s, jnx_char *interface,jnx_char *group)
   *@brief This function will enable the given socket to 
   recieve multicast packets on the given interface for the subscribed group
   *@param s is the socket to enable multicast listening on
   *@param interface is the IP address of the interface to use on the local machine
   *@param group is the multicast group to subscribe too
   *@return returns 0 on success
   */
  jnx_size jnx_socket_udp_enable_multicast_listen(jnx_socket *s,\
    jnx_char *interface, jnx_char *group);
   /**
   * @fn jnx_socket_udp_send(jnx_socket *s, jnx_char *host,
    jnx_char* port, jnx_uint8 *msg, jnx_size msg_len)
   * @param s is the socket to use to send
   * @param host is the target destination
   * @param port is the target port
   * @param msg is the payload to send
   * @param msg_len is the size of payload
   * @warning UDP header is 16bit which means 65k is the max file transmission size
   * @return jnx_size of bytes sent
   */
  jnx_size jnx_socket_udp_send(jnx_socket *s, jnx_char *host,\
    jnx_char* port, jnx_uint8 *msg, jnx_size msg_len);
   /**
   * @fn jnx_int32 jnx_socket_udp_listen(jnx_socket *s, jnx_char* port,
    jnx_size max_connections, socket_listener_callback c)
   * @param s is the socket to use to send
   * @param port is the target port
   * @param max_connections are the number of connetions in the queue
   * @param c is the function pointer callback for received messages
   * @return -1 on error
   */
  int jnx_socket_udp_listen(jnx_socket *s, jnx_char* port,\
    jnx_size max_connections, udp_socket_listener_callback c);
   /**
   * @fn jnx_int32 jnx_socket_udp_listen(jnx_socket *s, jnx_char* port,
    jnx_size max_connections, socket_listener_callback c)
   * @param s is the socket to use to send
   * @param port is the target port
   * @param max_connections are the number of connetions in the queue
   * @param c is the function pointer callback for received messages
   * @param context is the data to pass to the callback as contextual information
   * @return -1 on error
   */
  int jnx_socket_udp_listen_with_context(jnx_socket *s,\
    jnx_char* port, jnx_size max_connections,\
    udp_socket_listener_callback_with_context c, void *context);

#ifdef __cplusplus
}
#endif
#endif /* !JNX_UDP_SOCKET_H */
