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
#ifndef LIST_H
#define LIST_H

#include <stdbool.h>
#include <stdio.h>

/* List is a pointer to a list_t struct */
typedef struct list_t* List;

struct list_t {
    void *data;
    List next;
};

/* Pushes data as the new head of list. May be used to create a new list:
 * new_list = push(NULL, data) */
extern List push(List list, void *data);

/* Pop the head off the list */
extern void *pop(List *list);

/* Return the length of the list */
extern int len(List list);

/* Returns a reversed copy of list */
List reverse(List list);

/* Prepend data to list and update list */
extern List prepend(List *list, void *data);

/* Append l1 to the end of l2 */
void append(List l1, List *l2);

/* Inserts data into the tail of list */
void insert(void *data, List *list);

/* Inserts data into the tail of list or position equal to the next element */
void insert_by(bool (*eq)(void *data, void *node), void *data, List *list);

/* Inserts data into the tail of list. Returns true if sucessful,
 * false if it finds an element already equal to data */
bool insert_if(bool (*eq)(void *data, void *node), void *data, List *list);

/* Returns the node equal to aim in list, returns NULL if not found */
extern List find(bool (*eq)(void *aim, void *node), void *aim, List list);

/* Removes and returns the element equal to aim in list,
 * returns NULL if not found */
extern void *del(bool (*eq)(void *aim, void *node), void *aim, List *list);

/* Returns a new list that passes the predicate p */
List filter(bool (*p)(void *data), List list);

/* Print list to f by applying print to each node that is not NULL */
extern void print_list(void (*print)(FILE *f, void *data), FILE *f, List node);

/* Free the memory allocated to each list node */
extern void free_list(List node);

#endif
