/*
 * COMP20007 Design of Algorithms
 * Semester 1 2016
 *
 * Clement Poh (cpoh@unimelb.edu.au)
 *
 * This module contains useful functions for list manipulation and creation.
 *
 * XXX: This module may be used as a black box. It is not necessary to
 * XXX: understand the implementation details of the functions to use them.
 * XXX: The intrepid student is welcome to explore and learn the dark arts.
 *
*/
#include <stdlib.h>
#include <assert.h>

#include "list.h"

#define MAX_LINE_LEN 256

/* An abstraction of recursively traversing a list from right to left */
static void *foldl(void *(*f)(void *acc, void *data), void *acc, List list);

/* An abstraction of recursively traversing a list from left to right */
static void *foldr(void *(*f)(void *data, void *res), void *res, List list);

/* Add one to accumulator */
static void *plus_one(void *acc, void *data);

/* Pushes data as the new head of list. May be used to create a new list:
 * new_list = push(NULL, data) */
List push(List list, void *data) {
    List node = malloc(sizeof(*node));
    assert(node);

    node->data = data;
    node->next = list;

    return node;
}

/* Pop the head off the list */
void *pop(List *list) {
    if (!*list) {
        return NULL;
    } else {
        void *data = (*list)->data;
        List ptr = *list;

        /* Update list to point at the next element */
        *list = (*list)->next;

        /* Free the memory allocated to the list node */
        free(ptr);
        return data;
    }
}

/* Return the length of the list */
int len(List list) {
    return (long) foldl(plus_one, 0, list);
}

/* Returns a reversed copy of list */
List reverse(List list) {
    return (List) foldl((void *(*)(void *, void *))push, NULL, list);
}

/* Prepend data to list and update list */
List prepend(List *list, void *data) {
    *list = push(*list, data);
    return *list;
}

/* Append l1 to the end of l2 */
void append(List l1, List *l2) {
    if (!*l2)
        *l2 = l1;
    else
        append(l1, &(*l2)->next);
}

/* Inserts data into the tail of list */
void insert(void *data, List *list) {
    insert_by(NULL, data, list);
}

/* Inserts data into the tail of list or position equal to the next element */
void insert_by(bool (*eq)(void *data, void *node), void *data, List *list) {
    if (!*list)
        *list = push(NULL, data);
    else if (eq && eq(data, (*list)->data))
        (*list)->next = push((*list)->next, data);
    else
        insert_by(eq, data, &(*list)->next);
}

/* Inserts data into the tail of list: returns true if sucessful,
 * false if it finds an element already equal to data */
bool insert_if(bool (*eq)(void *data, void *node), void *data, List *list) {
    if (!*list) {
        *list = push(NULL, data);
        return true;
    } else return (eq && eq(data, (*list)->data))
        ? false
        : insert_if(eq, data, &(*list)->next);
}

/* Find the node equal to aim in list, returns NULL if not found */
List find(bool (*eq)(void *aim, void *node), void *aim, List list) {
    return !list
        ? NULL
        : eq(aim, list->data)
            ? list
            : find(eq, aim, list->next);
}

/* Removes and returns the element equal to aim in list,
 * returns NULL if not found */
void *del(bool (*eq)(void *aim, void *node), void *aim, List *list) {
    return !*list
        ? NULL
        : eq(aim, (*list)->data)
            ? pop(list)
            : del(eq, aim, &(*list)->next);
}

/* Print list to file by applying print to each node that is not NULL */
void print_list(void (*print)(FILE *f, void *data), FILE *f, List list) {
    if (list) {
        print(f, list->data);
        print_list(print, f, list->next);
    }
}

/* Free the memory allocated to each list node */
void free_list(List list) {
    if (list) {
        free_list(list->next);
        free(list);
    }
}

/* Returns a new list that passes the predicate p */
List filter(bool (*p)(void *data), List list) {
    /* Nested functions are not ISO C */
    __extension__
    void *f(void *data, void *res) {
        return p(data) ? push(res, data) : res;
    };
    return foldr(f, NULL, list);
}

/* Add one to accumulator */
static void *plus_one(void *acc, void *data) {
    (void) data; /* data is unused */
    return (void *)((long)acc + 1);
}

/* An abstraction of recursively traversing a list from right to left */
static void *foldr(void *(*f)(void *data, void *res), void *res, List list) {
    return !list
        ? res
        : f(list->data, foldr(f, res, list->next));
}

/* An abstraction of recursively traversing a list from left to right */
static void *foldl(void *(*f)(void *acc, void *data), void *acc, List list) {
    return !list
        ? acc
        : foldl(f, f(acc, list->data), list->next);
}

