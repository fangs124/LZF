/*
 * COMP20007 Design of Algorithms
 * Semester 1 2016
 *
 * Clement Poh (cpoh@unimelb.edu.au)
 *
 * This module provides all the IO functionality related to graphs.
 *
*/
#ifndef GRAPHIO_H
#define GRAPHIO_H

#include "graph.h"

/* Loads the graph from file */
extern Graph load_graph(char *input);

/* Prints the graph as a DOT file to output */
extern void print_graph(char *output, Graph graph);

/* Prints the destination vertex label surrounded by spaces */
extern void print_vertex_label(FILE *file, void *vertex);

/* Prints the vertex id followed by a newline */
extern void print_vertex_id(FILE *file, void *vertex);

/* Returns a sequence of vertices read from file */
extern List load_vertex_sequence(FILE *file, Graph graph);


#endif
