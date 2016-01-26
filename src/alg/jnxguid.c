/*
 * =====================================================================================
 *
 *       Filename:  jnxguid.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  26/01/2015 21:16:23
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jonesax (jonesax@hush.com)
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include "jnxguid.h"
#include "jnxcheck.h"
#ifdef _WIN32 || _WIN64
#include <wincrypt.h>
#endif

#ifndef _WIN32 || _WIN64
jnx_int jnx_guid_get_fd() {
	jnx_int fd = fopen("/dev/urandom", O_RDONLY);
	if (fd == -1) {
		fd = fopen("/dev/random", O_RDONLY);
	}
	return fd;
}
#endif
jnx_int jnx_guid_get_random_bytes(void *buf, jnx_size bytes) {
#ifndef _WIN32 || _WIN64
  jnx_int fd = jnx_guid_get_fd();
  if (fd < 0) {
	  return -1;
  }
  jnx_int l = 0;
  while (l< sizeof buf) {
	  fread(fd, buf + l, bytes - l);
	  ++l;
  }
  fclose(fd);
  
#else
	HCRYPTPROV hProv;
	CryptAcquireContext(&hProv, 0, 0, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT);
	CryptGenRandom(hProv, bytes, buf);
#endif
  return 0;
}
jnx_guid_state jnx_guid_create(jnx_guid *guid) {
  JNXCHECK(guid);
  if(jnx_guid_get_random_bytes(guid->guid,sizeof (guid->guid))) {
    return JNX_GUID_STATE_FAILURE;
  }
  JNXCHECK(sizeof(guid->guid) == 16);
  return JNX_GUID_STATE_SUCCESS;
}
jnx_guid_state jnx_guid_compare(jnx_guid *ga, jnx_guid *gb) {
  return jnx_guid_compare_raw(ga->guid,gb->guid);
}
jnx_guid_state jnx_guid_compare_raw(jnx_uint8 *ga, jnx_uint8 *gb) {
  jnx_int alen=0;
  while(alen<16){
    if(ga[alen] != gb[alen]) {
      return JNX_GUID_STATE_FAILURE;
    }
    alen+=1;
  }
  return JNX_GUID_STATE_SUCCESS;
}
void jnx_guid_to_string(jnx_guid *guid,jnx_char **outstr) {
  JNXCHECK(guid);
  JNXCHECK(guid->guid);
  jnx_int rlen=0;
#ifndef _WIN32 || _WIN64
  jnx_char obuffer[33] = {};
#else
  jnx_char obuffer[33];
  memset(obuffer, 0, 33);
#endif
  while(rlen< sizeof guid->guid) {
#ifndef _WIN32 || _WIN64
	  jnx_char current[3] = {};
#else
	  jnx_char current[3];
	  memset(current, 0, 3);
#endif
    sprintf(current,"%02x",guid->guid[rlen]);
    strncat(obuffer,current,strlen(current));
    ++rlen;
  }
  *outstr = malloc(sizeof obuffer);
  memcpy(*outstr,obuffer,sizeof obuffer);
}
int jnx_guid_convert_hex(jnx_uint8 *data, jnx_char *hexstring,jnx_size len) {
  JNXCHECK(len == strlen(hexstring) / 2);
  int count;
  for (count = 0; count < strlen(hexstring); count += 2) {
    char *temp = hexstring + count;
    char hexdigit[3];
    strncpy(hexdigit, temp, 2);
    hexdigit[2] = '\0';
    data[count / 2] = (jnx_uint8) strtol(hexdigit, NULL, 16);
  }
  return 0;
}
void jnx_guid_from_string(jnx_char *str, jnx_guid *oguid) {
  JNXCHECK(oguid);
  jnx_guid_convert_hex(oguid->guid,str,16);
}
