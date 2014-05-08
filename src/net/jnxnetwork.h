/*
 * =====================================================================================
 *
 *       Filename:  jnxnetwork.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/02/14 17:09:37
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jonesax (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef __JNXNETWORK_H__
#define __JNXNETWORK_H__
#include <ifaddrs.h>
#ifdef __cplusplus
	extern "C" {
#endif
#if defined(linux)
char *jnx_network_interface_to_string(char *interface, unsigned int family);
#endif
#ifdef __cplusplus
	}
#endif
#endif
