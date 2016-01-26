/* LZFe.c - LZ78-Fangs Universal Encoder */
/* NAME: Fangs124                        */               
/* VERS: 0.0b (not sure how this works)  */ 
/* DATE: 28 November, 2015               */

//--------//  preprocessors //----------------------------------------------//

/* included standard libraries */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

/* general constants */
#define TRUE     1
#define FALSE    0
#define DEBUG    0    /* controls debug output    */
#define SUMMARY  1    /* controls summary output  */
#define ANALYSIS 1    /* controls analysis output */

//--------// struct definitions //------------------------------------------//

/* node_t - represents a node point of a prefix tree
 *
 * index: the nth factor in the dictionary  
 *  data: hold the data (char) for this factor
 *  next: points to the next node of the same layer (node of same parent)
 * child: points to the first node under this pointer 
 */

typedef struct NODES_EXT ext_t;
struct NODES_EXT {
	int child_count;
};

typedef struct NODES node_t;
struct NODES {
	unsigned int index;
	unsigned char data;
	node_t *next;
	node_t *child;
	ext_t extention;
};

/* dict_t - a chained data structure, holds factors of a dictionary,
 *          each representing a node in the prefix tree
 *
 *         node: points to the node that this factor represents
 * parent_index: indicates the index parent of this factor
 *         next: points to next factor in the dictionary
 */

typedef struct DICTIONARIES dict_t;
struct DICTIONARIES {
	node_t *node;
	unsigned int parent_index;
	dict_t *next;
};

//--------// function prototypes //-----------------------------------------//

/* prefix-tree building functions */
int ConstructTree(unsigned char in_data);
void CreateEmptyTrie(void);
node_t *SearchNode(unsigned char in_data);
dict_t *SearchDict(node_t *in_node);
void AddNode(unsigned char in_data);
void AddDict(node_t *in_node, unsigned int in_index);
void CreateLastNode(unsigned char in_data);

/* factor analysis */

//--------// global variables //-------------------------------------------//

dict_t DICT;                  /* global dictionary        */
dict_t *DICT_PTR = &DICT;     /* points to a dictionary   */
int INDEX_COUNTER = 0;        /* counts number of factors */

node_t TRIE = {               /* global prefix-tree       */
	.index = 0,
	.data = '\0',
	.next = NULL,
	.child = NULL,
};
node_t *CURRENT_NODE = &TRIE; /* points to a node         */

int DEEPEST_LAYER = 0;
int LAYER_COUNT = 0;

//--------// main() function //---------------------------------------------//

int main(int argc, char* argv[]){
	int data_buffer;
	unsigned char data;
	int bytes_counter = 0;
	int is_added = 0;

	//prefix-tree building

	/* retrieve data from stdin */
	while((data_buffer = fgetc(stdin)) != EOF){
		/* building prefix-tree */
		bytes_counter++;
		data = (unsigned char) data_buffer;
		is_added = ConstructTree(data);
	}

	/* create last factor if necessary */
	if(is_added == FALSE){
		CreateLastNode(data);
	}

	//factor analysis
	#if(ANALYSIS)
	{
		fprintf(stderr, "%s\n", );
	}
	#endif
	printf("DEEPEST_LAYER = %d\n", DEEPEST_LAYER);
	return 0;
}

//============// prefix-tree construction 

//------------// ConstructTree() //-----------------------------------------//

/* DESC: deals with prefix-tree building abstraction
 * NOTE: return value is weather in_data is_added to the tree
 */

int ConstructTree(unsigned char in_data){
	node_t *temp_node = SearchNode(in_data);

	/* traverse prefix-tree if in_data is represented*/
	if(temp_node != NULL){
		LAYER_COUNT++;
		CURRENT_NODE = temp_node;
		return FALSE;
	}

	/* extend prefix-tree otherwise */
	else{
		INDEX_COUNTER++;
		#if DEBUG
		{
			//fprintf(stderr, "data  = %c\n", in_data);
			//fprintf(stderr, "index = %d\n", INDEX_COUNTER);
		}
		#endif
		AddNode(in_data);
		return TRUE;
	}
}

//------------// SearchNode() //--------------------------------------------//

/* DESC: looks for corresponding child node of CURRENT_NODE
 *       that represents in_data as a factor
 * NOTE: returns the node factor if found,
 *       otherwise returns NULL
 */

node_t *SearchNode(unsigned char in_data){
	node_t *node_pointer = CURRENT_NODE->child;
	/* iterate through child nodes, look for in_data */
	while(node_pointer != NULL){
		if(node_pointer->data == in_data){
			/* return node factor representing in_data */
			return(node_pointer);
		}
		node_pointer = node_pointer->next;
	}
	/* in_data is not a factor */
	return(NULL);
}

//------------// SearchDict() //--------------------------------------------//

/* DESC: searches corresponding dictionary element 
 *       correspoinding to the in_node
 */

dict_t *SearchDict(node_t *in_node){
	dict_t* current_dict = &DICT;
	while(current_dict != NULL){
		if(current_dict->node == in_node){
			//printf("current_dict->data = %c", current_dict->node->data);
			return current_dict;
		}
		current_dict = current_dict->next;
	}
	//printf("is not found\n");
	return NULL;
}

//------------// AddNode() //-----------------------------------------------//

/* DESC: takes in_data and extends the prefix-tree as a child node
         of CURRENT_NODE 
 * NOTE: malloc() is used here
 */

void AddNode(unsigned char in_data){
	node_t *node_pointer;
	/*create child node if no child */
	if(CURRENT_NODE->child == NULL){
		/* allocating and checking for memory */
		CURRENT_NODE->child = (node_t*) malloc(sizeof(node_t));
		assert(CURRENT_NODE->child != NULL);
		node_pointer = CURRENT_NODE->child;
	}

	/* find last child node and extend it */
	else{
		node_pointer = CURRENT_NODE->child;
		while(node_pointer->next != NULL){
			node_pointer = node_pointer->next;
		}

		/* allocating and checking for memory */
		node_pointer->next = (node_t*) malloc(sizeof(node_t));
		assert(node_pointer->next != NULL);
		node_pointer = node_pointer->next;
	}
	/* appending info to node */
	node_pointer->index = INDEX_COUNTER;
	node_pointer->data = in_data;
	node_pointer->next = NULL;
	node_pointer->child = NULL;
	/* update the global dict */
	AddDict(node_pointer, CURRENT_NODE->index);
	/* reseting CURRENT_NODE */
	CURRENT_NODE = &TRIE;
	/* updating analysis information */
	if(DEEPEST_LAYER < LAYER_COUNT){
		DEEPEST_LAYER = LAYER_COUNT;
	}
	LAYER_COUNT = 0;
	
	return;
}

//------------// AddDict() /------------------------------------------------//

/* DESC: updates the global dictionary
 * NOTE: malloc() is used here
 */

void AddDict(node_t *in_node, unsigned int in_index){
	#if DEBUG
	{
		fprintf(stderr, "creating dictionary element:\n", );
		fprintf(stderr, "%c%d", in_node->data, in_index);
		fprintf(stderr, "\n");
	}
	#endif
	/* if dict is uninitialized */
	if(DICT_PTR == NULL){
		/* allocating and checking memory */
		DICT_PTR = (dict_t*) malloc(sizeof(dict_t));
		assert(DICT_PTR != NULL);
	}
	/* if dict is initialized */
	else{
		/* allocating and checking memory */
		DICT_PTR->next = (dict_t*) malloc(sizeof(dict_t));
		assert(DICT_PTR->next != NULL);
		DICT_PTR = DICT_PTR->next;
	
	}
	/* appending info to dict */
	DICT_PTR->node = in_node;
	DICT_PTR->parent_index = in_index;
	DICT_PTR->next = NULL;
	return;
}

//------------// CreateLastNode() //----------------------------------------//

/* DESC: an edge-case function for last factor to be added
 * NOTE: malloc() is used here.
 */

void CreateLastNode(unsigned char in_data){
	/* checks if CURRENT_NODE was added */
	dict_t *temp = SearchDict(CURRENT_NODE);
	if(temp == NULL){
		CURRENT_NODE = &TRIE;
	}
	/* find last child node and extend it */
	node_t *node_pointer = CURRENT_NODE;
	while(node_pointer->next != NULL){
		node_pointer = node_pointer->next;
	}

	/* allocating and checking for memory */
	node_pointer->next = (node_t*) malloc(sizeof(node_t));
	assert(node_pointer->next != NULL);
	node_pointer = node_pointer->next;

	/* appending info to node */
	node_pointer->index = INDEX_COUNTER;
	node_pointer->data = in_data;
	node_pointer->next = NULL;
	node_pointer->child = NULL;

	/* update the global dict */
	//printf("CURRENT_NODE->index = %d\n", temp->parent_index);
	AddDict(node_pointer, temp->parent_index);
	
	/* reseting CURRENT_NODE */
	CURRENT_NODE = &TRIE;
	/* updating analysis information */
	if(DEEPEST_LAYER < LAYER_COUNT){
		DEEPEST_LAYER = LAYER_COUNT;
	}
	LAYER_COUNT = 0;
	return;
}

// factors analysis //

//--------// reference list //----------------------------------------------//

/* http://www.tutorialspoint.com/cprogramming
 * https://en.wikipedia.org/wiki/Trie
 * https://en.wikipedia.org/wiki/LZ77_and_LZ78
 *
 * commenting style adapted from:
 * 2013 Assignment Sample Solution - by Alistair Moffat
 */

//--------// changes and notes //-------------------------------------------//

/*

19 OCT 2015:
decided to add a 6 bytes wide header bytes,
first four bytes indicates the length of the dictionary
last two bytes indicates the bytes width of data and index

19 NOV 2015:
re-wrote everything from scratch with hopes of simpler abstraction.
code divided into three major sections

 */

//--------------------------------------------------------------------------//