/*
 * COMP20007 Design of Algorithms
 * Semester 1 2016
 *
 * NAME: Adrian Hendrawan Putra
 * ID  : 743206
 * DESC: 0x7c
 *       Provides basic graph-manipulation function
 *
*/

#include <stdlib.h>
#include <assert.h>

#include "graph.h"

/* Returns a pointer to a new graph with order vertices */
Graph new_graph(int order) {
    /* assign memory for graph */
    Graph graph = (Graph) malloc(sizeof(*graph));
    assert(graph != NULL);

    graph->order = order;
    graph->size = 0;

    /* assign memory for vertices */
    graph->vertices = (Vertex) malloc(sizeof(*graph->vertices) * order);
    assert(graph->vertices != NULL);

    return graph;
}

/* Returns whether aim and vertex are pointing to the same location */
bool ptr_eq(void *aim, void *vertex) {
    return(bool)((Vertex)aim == (Vertex)vertex);
}

/* Returns whether aim and vertex have the same id */
bool id_eq(void *aim, void *vertex) {
    return(bool)(((Vertex)aim)->id == ((Vertex)vertex)->id);
}

/* Add the edge from v1 to v2 to graph */
void add_edge(Graph graph, int v1, int v2) {
    graph->size++;
    graph->vertices[v1].out = push(graph->vertices[v1].out, &graph->vertices[v2]);
    graph->vertices[v2].in  = push(graph->vertices[v2].in , &graph->vertices[v1]);
    
    return;
}

/* Free the memory allocated to graph */
void free_graph(Graph graph) {
    int i; //vertex index

    /* free each vertex component in graph */
    for(i = 0; i < graph->order; i++) {
        free_list(graph->vertices[i].in);
        free_list(graph->vertices[i].out);
        free(graph->vertices[i].label);
    }

    /* free graph and vertices */
    free(graph->vertices);
    free(graph);
    
    return;
}
