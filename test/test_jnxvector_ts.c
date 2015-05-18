/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  03/16/13 11:42:57
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Alex Jones (), alexsimonjones@gmail.com
 *   Organization:
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include "jnxvector.h"
#include <stdio.h>
#include "jnxcheck.h"
#include "jnxlog.h"
#include <time.h>
#include <string.h>
#include "jnxterm.h"
typedef struct {
  int a;
} teststruct;

void test_complex_insertion() {
  JNXLOG(LDEBUG,"- test_complex_insertion");
  jnx_vector *vector = jnx_vector_create();
  int spread[5] = { 1, 200, 412, 55, 65 };
  //we are testing to see how well the vector handles distribution and reallocation on demand
  int x = 0;
  while(x < 5) {
    teststruct *temp = malloc(sizeof(teststruct));
    temp->a = x;
    jnx_vector_insert_at_ts(vector,spread[x],temp);
    teststruct *res = vector->vector[spread[x]]->data;
    JNXCHECK(res->a == x);
    ++x;
  }
  jnx_vector_destroy(&vector);
  jnx_term_printf_in_color(JNX_COL_GREEN, "  OK\n");
}
void test_sequential_insertion() {
  JNXLOG(LDEBUG,"- test_sequential_insertion");
  jnx_vector *vector = jnx_vector_create();

  int x = 0;
  do {
    jnx_vector_insert_ts(vector,x);
    ++x;
  } while(x < 100);
  int y = 0;
  while(y < vector->count) {
    JNXCHECK(vector->vector[y]->data == y);
    ++y;
  }
  jnx_vector_destroy(&vector);
  jnx_term_printf_in_color(JNX_COL_GREEN, "  OK\n");
}
void test_insert_position() {
  JNXLOG(LDEBUG,"- test_insert_position");
  jnx_vector *vector = jnx_vector_create();
  jnx_vector_insert_at_ts(vector,15,"Test");
  jnx_vector_insert_at_ts(vector,100,"Derp");
  JNXCHECK(vector->vector[15]->used == 1);
  JNXCHECK(vector->vector[100]->used == 1);
  JNXCHECK((char*)vector->vector[15]->data == "Test");
  JNXCHECK((char*)vector->vector[100]->data == "Derp");
  jnx_term_printf_in_color(JNX_COL_GREEN, "  OK\n");
}
void test_remove_position() {
  JNXLOG(LDEBUG,"- test_remove_position");
  jnx_vector *vector = jnx_vector_create();
  jnx_vector_insert_ts(vector,"Hello");
  jnx_vector_insert_at_ts(vector,90,"Bye");
  JNXCHECK(vector->vector[15]->data == NULL);
  JNXCHECK((char*)vector->vector[90]->data == "Bye");
  char *data = jnx_vector_remove_at_ts(vector,90);
  JNXCHECK(strcmp("Bye",(char*)data) == 0);
  jnx_vector_destroy(&vector);
  jnx_term_printf_in_color(JNX_COL_GREEN, "  OK\n");
}
int vector_compare(void *A, void *B) {
  if(A == B) {
    return 1;
  }
  return 0;
}
void test_contains() {
  JNXLOG(LDEBUG,"- test_contain");
  jnx_vector *vector = jnx_vector_create();
  jnx_vector_insert_ts(vector,"Hello");
  
  JNXCHECK(jnx_vector_contains_ts(vector,"Hello",vector_compare) == 1);
  JNXCHECK(jnx_vector_contains_ts(vector,"GoodBye",vector_compare) == 0);
  jnx_term_printf_in_color(JNX_COL_GREEN, "  OK\n");
}
void test_get() {
  JNXLOG(LDEBUG,"- test_get");
  jnx_vector *vector = jnx_vector_create();
  jnx_vector_insert(vector,"Hello");
  JNXCHECK(jnx_vector_contains(vector,"Hello",vector_compare) == 1);
  char *str = (char*)jnx_vector_get_at_ts(vector,0);
  JNXCHECK(strcmp("Hello",str) == 0);
  jnx_term_printf_in_color(JNX_COL_GREEN, "  OK\n");
}
int test_jnxvector_ts(int argc, char **argv) {
  JNXLOG(LDEBUG,"Running vector tests...\n");
  test_insert_position();
  test_remove_position();
  test_sequential_insertion();
  test_complex_insertion();
  test_contains();
  test_get();
  JNXLOG(LDEBUG,"Vector tests completed.\n");
  return 0;
}
