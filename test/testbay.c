#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "../src/jnxstring.h"


int main(int argc, char **argv)
{
// 	printf("starting\n");
// 	char *res = jnx_file_tostr("Makefile");
// 	printf("%s",res);

	char *boo = "boo";
	char *who = "who";
	
	printf("%s\n",jnx_string_append(boo,who));
	
	return 0;
}
