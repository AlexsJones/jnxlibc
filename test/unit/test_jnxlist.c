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
#include <string.h>
#include "jnx_check.h"
#include "jnx_list.h"
#include "jnx_term.h"
#include "jnx_log.h"
struct foo {
  int number;
} foo;

static void test_list_creation() {
  JNXLOG(LDEBUG,"- test_list_creation");
  jnx_list *secondlist = jnx_list_create();
  JNXCHECK(secondlist != NULL);
  JNXLOG(LDEBUG,"OK");
  struct foo *f = malloc(sizeof(foo));
  f->number = 10;
  JNXLOG(LDEBUG,"- test_list_insertion");
  jnx_list_add(secondlist,(void*)f);
  JNXLOG(LDEBUG,"OK");
  JNXLOG(LDEBUG,"- test_list_removal");
  struct foo *output = (struct foo*)jnx_list_remove(&secondlist);
  JNXCHECK(output->number == 10);
  free(output);
  JNXLOG(LDEBUG,"OK");
  JNXLOG(LDEBUG,"- test_list_deletion");
  jnx_list_destroy(&secondlist);
  JNXLOG(LDEBUG,"OK");
}
static void test_list_index() {
  JNXLOG(LDEBUG,"- test_list_sequence");
  jnx_list *j = jnx_list_create();
  int count = 6;
  char *ar[] = { "A", "B", "C", "D", "E", "F" };
  int y;
  for(y = 0; y < count; ++y) {
    jnx_list_add(j,ar[y]);
  }
  int x;
  for(x = count - 1; x >= 0; x--) {
    char *current = jnx_list_remove(&j);
    JNXCHECK(strcmp(current,ar[x]) == 0);
  }
  JNXLOG(LDEBUG,"OK");
  jnx_list_destroy(&j);
  JNXCHECK(j == NULL);
}
static void test_data_removal() {
  JNXLOG(LDEBUG,"- test_data_removal");
  char *test_string = "AABBCC";
  jnx_list *list = jnx_list_create();
  int count = 10;
  int x;
  for(x = 0; x < count; ++x) {
    char *dynamic_string = malloc(strlen(test_string));
    jnx_list_add(list,dynamic_string);
  }
  char *ret;
  while((ret = jnx_list_remove(&list)) != NULL) {
    free(ret);
  }
  jnx_list_destroy(&list);
  JNXCHECK(list == NULL);
  JNXLOG(LDEBUG,"OK");
}
static void test_list_tail() {
  jnx_list *l = jnx_list_create();

  int count = 3;
  char *ar[] = { "A", "B", "C" };
  int x;
  for(x = 0; x < count; ++x) {
    jnx_list_add(l,ar[x]);
  }
  int c=count;
  while(l->tail != NULL) {

    JNXCHECK(strcmp((char*)l->tail->_data,ar[c-1]) == 0);
    --c;
    l->tail = l->tail->prev_node;
  }

  jnx_list_destroy(&l);
}
static void test_removal_front() {
  JNXLOG(LDEBUG,"- test_removal_front\n");
  jnx_list *l = jnx_list_create();
  int count = 3;
  char *ar[] = { "A", "B", "C" };
  int x;
  for(x = 0; x < count; x++) {
    jnx_list_add(l,ar[x]);
  }
  JNXCHECK(l->counter == 3);
  void *data = jnx_list_remove_front(&l);
  JNXCHECK(strcmp(data,"A") == 0);
  JNXCHECK(l->counter == 2);
  data = jnx_list_remove_front(&l);
  JNXCHECK(strcmp(data, "B") == 0);
  JNXCHECK(l->counter == 1);
  data = jnx_list_remove_front(&l);
  JNXCHECK(strcmp(data, "C") == 0);
  JNXCHECK(l->counter == 0);
}

static int test_contains_comparator(void *A, void *B) {
  if(A == B) {
    return 1;
  }
  return 0;
}
static void test_contains() {
  jnx_list *l = jnx_list_create();

  char *text = "Text";
  char *another = "Another";
  char *fake = "Fake";
  jnx_list_add(l,text);
  jnx_list_add(l,another);

  JNXCHECK(jnx_list_contains(l,text,test_contains_comparator) == 1);
  JNXCHECK(jnx_list_contains(l,another,test_contains_comparator) == 1);
  JNXCHECK(jnx_list_contains(l,fake,test_contains_comparator) == 0);

}

int compare_ints(void *first, void *second) {
  int f = (int) first;
  int s = (int) second;
  if (f > s) return 1;
  else if (f == s) return 0;
  else return -1;
}

static void test_removal_from() {
  JNXLOG(LDEBUG,"- test_removal_from\n");
  jnx_list *list = jnx_list_create();

  // Test the empty list removal should succeed without blowing up
  jnx_list_remove_from(&list, 5, compare_ints);

  int i;
  for (i = 1; i < 10; i++)
    jnx_list_add(list, (void *)i);

  // Remove from the middle
  jnx_list_remove_from(&list, 3, compare_ints);
  // Remove the head
  jnx_list_remove_from(&list, 1, compare_ints);
  // Remove the tail
  jnx_list_remove_from(&list, 9, compare_ints);

  // Check the remaining elements are 245678
  char digit[2];
  char leftover[10];
  leftover[0] = '\0';
  jnx_node *current = list->head;
  while (current != NULL) {
    sprintf(digit, "%d", (int) current->_data);
    strcat(leftover, digit);
    current = current->next_node;		
  }

  JNXCHECK(strcmp(leftover, "245678") == 0);
}

static void test_removal_returns_correct_data() {
  JNXLOG(LDEBUG,"- test_removal_returns_correct_data\n");

  char *data = malloc(strlen("data item") + 1);
  strcpy(data, "data item");

  char *lookup = malloc(strlen("data item") + 1);
  strcpy(lookup, "data item");

  jnx_list *ls = jnx_list_create();
  jnx_list_add(ls, (void *) data);

  void *result = jnx_list_remove_from(&ls, (void *) lookup, (ordering) strcmp);
  JNXCHECK(result == data);
  JNXCHECK(result != lookup);
  JNXCHECK(ls->head == NULL);
}

int test_jnxlist() {
  JNXLOG(LDEBUG,"Running list tests...\n");
  test_list_creation();
  test_data_removal();
  test_list_index();
  test_list_tail();
  test_removal_front();
  test_removal_from();
  test_removal_returns_correct_data();
  test_contains();
  JNXLOG(LDEBUG,"List tests completed.\n");
  return 0;
}
