/*
 * COMP20007 Design of Algorithms
 * Semester 1 2016
 *
 * Clement Poh (cpoh@unimelb.edu.au)
 *
 * This provides all the non-IO functionality related to graphs.
 *
*/
#ifndef GRAPH_H
#define GRAPH_H

#include "list.h"

typedef struct graph_t* Graph;
typedef struct vertex_t* Vertex;

/* A vertex is an id, a label and a list of incoming and outgoing edges */
struct vertex_t {
    int id;
    char *label;

    /* A list of vertices representing incoming edges */
    List in;
    /* A List of vertices representing outgoing edges */
    List out;
};

/* A graph is its order, size, and an array of vertices */
struct graph_t {
    /* Number of vertices */
    int order;
    /* Number of edges */
    int size;
    Vertex vertices;
};

/* Returns a pointer to a new graph with order vertices */
extern Graph new_graph(int order);

/* Returns whether aim and vertex are pointing to the same location */
extern bool ptr_eq(void *aim, void *vertex);

/* Returns whether aim and vertex have the same id */
extern bool id_eq(void *aim, void *vertex);

/* Add the edge from v1 to v2 graph */
extern void add_edge(Graph graph, int v1, int v2);

/* Free the memory allocated to graph */
extern void free_graph(Graph graph);

#endif
