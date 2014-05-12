/*
 * =====================================================================================
 *
 *       Filename:  jnxcheck.h
 *
 *    Description:  This gives the option to disable assertions in release mode
 *
 *        Version:  1.0
 *        Created:  05/08/14 07:57:45
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jonesax (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef __JNX_CHECK_H__
#define __JNX_CHECK_H__
#include "jnxlog.h"

#if !defined(RELEASE)

#define JNXCHECK(X)\
	do \
	{ \
		if(!(X)) {\
		JNX_LOGC(JLOG_CRITICAL,"CHECK FAILED: >%s< [%s:%s:%d]\n",#X,__FILE__,__FUNCTION__,__LINE__); \
			exit(1);\
		} \
	} \
	while(0) \

#else
#define JNXCHECK(X)\
	do { (void)sizeof(X); } while(0)
#endif

#endif