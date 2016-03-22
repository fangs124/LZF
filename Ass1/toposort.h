/*
 * COMP20007 Design of Algorithms
 * Semester 1 2016
 *
 * Clement Poh (cpoh@unimelb.edu.au)
 *
 * This module provides all the topological sorting functionality.
 *
*/
#ifndef TOPOSORT_H
#define TOPOSORT_H

#include "graph.h"

/* Returns a list of topologically sorted vertices using the DFS method */
extern List dfs_sort(Graph graph);

/* Returns a list of topologically sorted vertices using the Kahn method */
extern List kahn_sort(Graph graph);

/* Uses graph to verify vertices are topologically sorted */
extern bool verify(Graph graph, List list);

#endif
