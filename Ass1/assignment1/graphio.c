/*
 * COMP20007 Design of Algorithms
 * Semester 1 2016
 *
 * Clement Poh (cpoh@unimelb.edu.au)
 *
 * This module provides all the IO functionality related to graphs.
 *
*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "graphio.h"

#define MAX_LINE_LEN 256

typedef enum {
    VCOUNT    = 0,
    VLABEL    = 1,
    DEDGES    = 2,
} input_stage;

/* Loads the graph from input */
Graph load_graph(char *input) {//implement this function
	/* open file */
	FILE *file = fopen(input, "r");
	input_stage stage = VCOUNT;
	char *buffer = (char) malloc(sizeof(char) * MAX_LINE_LEN);
	assert(buffer);
	Graph graph = malloc(sizeof(*graph));
	int i = 0;
	while((buffer = fgets(buffer, MAX_LINE_LEN, file)) != NULL){
		//debug
		fprintf(stderr, "%s", buffer);
		/* get number of nodes */
		if(stage == VCOUNT){
			graph->order = atoi(buffer);
			graph->vertices = malloc(sizeof(*vertices) * graph->order);
		}
		/* assign labels */
		else if(stage == VLABEL){
			*graph->vertices[i]->id = i;
			*graph->vertices[i]->label = malloc(strlen(buffer) - 1);
			*graph->vertices[i]->label = memcpy(buffer, 
				*graph->vertices[i]->label, strlen(buffer) - 1);
			*graph->vertices[i]->label[strlen(buffer) - 1] = '\0';
		}
		stage++;
	}
	/* assign edges */
	return NULL;
}

/* Prints the graph */
void print_graph(char *output, Graph graph) {//implement this function
}

/* Prints the destination vertex label surrounded by spaces */
void print_vertex_label(FILE *file, void *vertex) {
}

/* Prints the id of a vertex then a newline */
void print_vertex_id(FILE *file, void *vertex) {
	if (vertex)
		fprintf(file, "%d\n", ((Vertex)vertex)->id);
}

/* Returns a sequence of vertices read from file */
List load_vertex_sequence(FILE *file, Graph graph) {
	const char *err_duplicate = "Error: duplicate vertex %d %s\n";
	const char *err_order = "Error: graph order %d, loaded %d vertices\n";
	List list = NULL;
	int id;

	while(fscanf(file, "%d\n", &id) == 1) {
		assert(id >= 0);
		assert(id < graph->order);

		if (!insert_if(id_eq, graph->vertices + id, &list)) {
			fprintf(stderr, err_duplicate, id, graph->vertices[id].label);
			exit(EXIT_FAILURE);
		}
	}

	if (len(list) != graph->order) {
		fprintf(stderr, err_order, graph->order, len(list));
		exit(EXIT_FAILURE);
	}

	return list;
}
