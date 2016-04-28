/*
 * COMP20007 Design of Algorithms
 * Semester 1 2016
 *
 * NAME: Adrian Hendrawan Putra
 * ID  : 743206
 * DESC: 0x7c
 *
 * Clement Poh (cpoh@unimelb.edu.au)
 *
 * This module provides all the IO functionality related to graphs.
 *
 *
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "graphio.h"

#define MAX_LINE_LEN 256

/* Used to control the input stage process */
typedef enum {
    VCOUNT    = 0,
    VLABEL    = 1,
    DEDGES    = 2,
} input_stage;

/* Assigns relevant data to pointed vertex */
void assign_vertex(Vertex vertex, int id, char *buffer);

/* Assigns value pair form buffer to the edge: [v1, v2] */
void get_value_pair(int *edge, char *buffer);

/* Loads the graph from input */
Graph load_graph(char *input) { //need to do case for null graph
    /* open file */
    FILE *file = fopen(input, "r");

    /* assign memory for input buffer */
    char *buffer = (char*) malloc(sizeof(char) * MAX_LINE_LEN);
    assert(buffer != NULL);

    int i = 0; //vertex index
    int *edge; //ordered pair of vertices: [v1, v2]
    
    /* assign memory for edge */
    edge = (int*) malloc(sizeof(int) * 2);
    assert(edge != NULL);

    Graph graph;
    input_stage stage = VCOUNT;

    /* get per-line string from input */
    while((buffer = fgets(buffer, MAX_LINE_LEN, file)) != NULL) {
        switch(stage) {
            /* count and allocate memory for vertices */
            case VCOUNT:
                graph = new_graph(atoi(buffer));
                stage = VLABEL;
                break; 
            /* assign vertex labels */
            case VLABEL:
                /* process ith vertex */
                assign_vertex(&graph->vertices[i], i, buffer);
                i += 1;
                if(i == graph->order){
                    stage = DEDGES;
                }
                break;
            /* assign directed edges */
            case DEDGES:
                /* get [v1, v2] from buffer */
                get_value_pair(edge, buffer);
                /* create edge [v1, v2] */
                add_edge(graph, edge[0], edge[1]);
                break;
            /* code should not reach here */
            default:
                fprintf(stderr, "Error: invalid stage\n");
                exit(EXIT_FAILURE);
        }
    }

    /* free allocated memory */
    free(buffer);
    buffer = NULL;
    free(edge);
    edge = NULL;

    return graph;
}

/* Assigns relevant data to pointed vertex */
void assign_vertex(Vertex vertex, int id, char *buffer) {
    /* assigning data */
    vertex->id = id;
    vertex->in = NULL;
    vertex->out = NULL;
    
    /* length of label */
    size_t label_len = strlen(buffer);

    /* assign memory for label */
    vertex->label = (char*) malloc(label_len);
    assert(vertex->label != NULL);

    vertex->label = memcpy(vertex->label, buffer, label_len);
    vertex->label[label_len-1] = '\0'; //replacing '\n' with '\0'

    return;
}

/* Assigns value pair form buffer to the edge: [v1, v2] */
void get_value_pair(int *edge, char *buffer) {
    int start = 0; //starting index of v1 or v2
    int digit_count = 0;
    char *ptr = buffer;

    while(*ptr != '\0') {
        switch(*ptr) {
            /* assign v1 to edge: [v1, v2] */
            case ' ': 
                edge[0] = atoi(&buffer[start]);
                start = digit_count + 1;
                digit_count = 0;
                break;
            /* assign v2 to edge: [v1, v2] */
            case '\n':
                edge[1] = atoi(&buffer[start]);
                break;
            /* count digit */
            default:
                if(*ptr >= '0' || *ptr <= '9') { 
                    digit_count += 1;
                }
                else{ //char should be numeric
                    fprintf(stderr, "Error: invalid char\n");
                    exit(EXIT_FAILURE);
                }
        }
        ptr++;
    }

    return;
}

/* Prints the graph */
void print_graph(char *output, Graph graph) {
    /* open file */
    FILE *file = fopen(output, "w");

    /* print header */
    fprintf(file, "digraph {\n");

    int i; //vertex index
    List ptr;

    /* iterate through each vertex in graph */
    for(i = 0; i < graph->order; i++) {
        /* print vertex i */
        fprintf(file, " ");
        
        /* iterate through reachable vertices form vertex i */
        ptr = graph->vertices[i].out;
        if(ptr != NULL) {
            print_vertex_label(file, &graph->vertices[i]);
            fprintf(file, "-> {");
            
            /* print vertices */
            while(ptr != NULL) {
                print_vertex_label(file, (Vertex)ptr->data);
                ptr = ptr->next;
            }
            fprintf(file, "}");
        }

        /* edge case handling */
        else if(ptr == NULL) {
            //as observed on t1.dot, ass1 was not printed with extra space.
            fprintf(file, " %s", graph->vertices[i].label);
        }
        fprintf(file, "\n");
    }

    /* print tail */
    fprintf(file, "}\n");

    return;
}


/* Prints the destination vertex label surrounded by spaces */
void print_vertex_label(FILE *file, void *vertex) {
    if (vertex)
        fprintf(file, " %s ", ((Vertex)vertex)->label);
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
