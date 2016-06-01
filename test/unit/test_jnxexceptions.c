/*
 * =====================================================================================
 *
 *       Filename:  test_jnxexceptions.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/19/2015 08:24:41 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include "jnx_log.h"
#include "jnx_check.h"

#define FOO_EXCEPTION (-2)
#define BAR_EXCEPTION (-3)

int test_jnxexceptions()
{
  JNXLOG(LDEBUG,"Running test_jnxexception tests");

  int ar[3] = { -1,-2,-3};
  int c;
  for(c=0;c<3;++c) {

    int current_exception = ar[c];
    jnx_try {
      jnx_throw( ar[c] );
    }
    jnx_catch( JNX_EXCEPTION ) {
      JNXLOG(LDEBUG,"Default system exception fired correctly");
      JNXCHECK(current_exception == JNX_EXCEPTION);
      break;
    }
    jnx_catch( FOO_EXCEPTION ) {
      JNXLOG(LDEBUG,"First exception fired correctly");
      JNXCHECK(current_exception == FOO_EXCEPTION);
      break;
    }
    jnx_catch( BAR_EXCEPTION ) {
      JNXLOG(LDEBUG,"Second exception fired correctly");
      JNXCHECK(current_exception == BAR_EXCEPTION);
      break;
    }
    jnx_finally {
      JNXLOG(LDEBUG,"Hit finally clause");
    }
    jnx_try_end
  }
  return 0;
}
