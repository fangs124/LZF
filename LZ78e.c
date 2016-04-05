 /* LZ78e.c - LZ78 Universal Encoder */
/* warning: botch code              */
/* by: Fangs124                     */
//--------//  preprocessors //----------------------------------------------//

/* included standard libraries */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

/* general constants */
#define TRUE          1
#define FALSE        -1
#define NAN_INDEX    -1    /* indicates an uninitialized index */
#define DEBUG         0    /* controls debug output            */
#define SUMMARY       1    /* controls summary output          */
#define WIDTH_DATA    1    /* the width (bytes) of a data      */
#define WIDTH_INDEX   3    /* the width (bytes) of an index    */
#define WIDTH_HEADER  6    /* the width (bytes) of the header  */

//--------// struct definitions //------------------------------------------//

/* node_t - represents a node point of a prefix tree
 *
 * index: the nth factor in the dictionary  
 *  data: hold the data (char) for this factor
 *  next: points to the next node of the same layer (node of same parent)
 * child: points to the first node under this pointer
 */

typedef struct NODES node_t;
struct NODES {
	unsigned int index;
	unsigned char data;
	node_t       *next;
	node_t      *child;
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
	node_t              *node;
	unsigned int parent_index;
	dict_t              *next;
};

//--------// function prototypes //-----------------------------------------//

node_t RootNode(void);
dict_t EmptyDict(void);
node_t *SearchNode(node_t *node, unsigned char in_data);
node_t *AddNode(node_t *node, unsigned char in_data, unsigned int in_index);
dict_t *AddDict(dict_t *dict_ptr, node_t *in_node, unsigned int in_index);
unsigned char* PrintDict(dict_t *dict, unsigned int total_index);
unsigned char* IntToChar(unsigned int in_index);
unsigned char* HeaderDictSize(unsigned int in_index);

//--------// main() function //---------------------------------------------//
int main(int argc, char* argv[]){
	/* general variables */
	int retrieve_buffer = 0;	
	unsigned char data;                     /* retrieve data from stdin           */
	int byte_counter = 0;          /* counts bytes received from stdin   */
	unsigned int index_counter = 0;         /* counts factors created             */
	node_t trie = RootNode();
	node_t *parent_node = &trie;
	node_t *temp_node = &trie;
	dict_t dict = EmptyDict();
	dict_t *dict_ptr = &dict;

	unsigned char* output;
	size_t t_memblock = 0;
	/* last-factor variables */
	unsigned char data_buffer;              /* retains data when EOF is reached   */
	int is_added = FALSE;          /* indicates if data_buffer is added  */
	
	while((retrieve_buffer = fgetc(stdin)) != EOF){
		//fprintf(stderr, "%02x  ", retrieve_buffer);
		data = (unsigned char) retrieve_buffer;
		byte_counter++;

		/* update current parent_node */
		parent_node = temp_node;

		/* if data is a member of current parent_node:
		 * set the node that representing data as next parent_node
		 */

		if((temp_node = SearchNode(parent_node, data)) != NULL){
			/* set data to data_buffer */
			is_added = FALSE;
			data_buffer = data;
			continue;
		}
		
		/* if data is not a member of current parent_node:
		 * create a child node to represent data,
		 * and create the associated dictionary element
		 */

		index_counter++;
		temp_node = AddNode(parent_node, data, index_counter);
		dict_ptr = AddDict(dict_ptr, temp_node, parent_node->index);
		is_added = TRUE;
		
		/* reset parent_node */
		temp_node = &trie;
	}

	/* create factor for data_buffer if required  */
	if(is_added == FALSE){
		
		/* temp_node is NULL if data_buffer is not member of parent_node,
		 * indicating parent_node needs to be reset
		 */

		if(temp_node == NULL){
			parent_node = &trie;
		}

		index_counter++;
		temp_node = AddNode(parent_node, data_buffer, index_counter);
		dict_ptr = AddDict(dict_ptr, temp_node, parent_node->index);
	}


	/* skip the first element of the dictionary */
	output = PrintDict(dict.next, index_counter);
	t_memblock = (index_counter * 4) + WIDTH_HEADER + 1;
	fwrite(output, sizeof(char), t_memblock, stdout);
	

	#if DEBUG
	{
		char temp_char;
		int k = 0;
		while(k < t_memblock){
			temp_char = output[k];
			fprintf(stderr, "output = %02x\n", temp_char);
			k++;
		}
	}
	#endif
	

	#if SUMMARY
	{ /* SUMMARY INFO */
		fprintf(stderr, "encode: %6d bytes input\n", byte_counter);
		fprintf(stderr, "encode: %6d factors generated\n", index_counter);
		fprintf(stderr, "encode: %6zu bytes output\n", t_memblock);
	}
	#endif
	

	free(output);
	output = NULL;
	return 0;
}

//--------// RootNode() //--------------------------------------------------//

/* DESC: used to create a root node
 * NOTE: root node is a pre-defined factor of index 0
 */

node_t RootNode(void){
	node_t node;
	node.index = 0;
	node.data = '\0';
	node.next = NULL;
	node.child = NULL;
	return node;
}

//--------// EmptyDict() //-------------------------------------------------//

/* DESC: used to create an empty dictionary
 * NOTE: the first element is not meant to be processed
 */

dict_t EmptyDict(void){
	dict_t dict;
	dict.parent_index = (unsigned int) NAN_INDEX;
	dict.node = NULL;
	dict.next = NULL;
	return dict;
}

//--------// SearchNode() //------------------------------------------------//

/* DESC: takes in a parent node and looks through it's child nodes if 
 *       in_data (input data) is represented as a factor
 * NOTE: returns the node that represent in_data, or
 *       returns NULL if in_data is not a member of parent node
 */
 
 node_t *SearchNode(node_t *node, unsigned char in_data){
	node_t *node_pointer = node->child;
	
	/* iterate through child nodes and check for membership */
	while(node_pointer != NULL){
		/* returns node that represents in_data */
		if(node_pointer->data == in_data){
			return(node_pointer);
		}
		node_pointer = node_pointer->next;
	}

	/* in_data is not a member */
	return (NULL);
}

//--------// AddNode() //---------------------------------------------------//

/* DESC: takes in_data (input data) and in_index (input index)
 *       and adds it as a child of the input node
 * NOTE: this function will extend the last child node,
 *       or create one if node does not have child
 * NOTE: malloc() is used here
 */

node_t *AddNode(node_t *node, unsigned char in_data, unsigned int in_index){
	node_t *node_pointer;
	
	/* if node has no child, create one */
	if(node->child == NULL){
		/* allocating and checking for memory */
		node->child = (node_t*) malloc(sizeof(node_t));
		assert(node->child != NULL);
		node_pointer = node->child;
	}
	
	else{
		/* find last child node and extend it */
		node_pointer = node->child;
		while(node_pointer->next != NULL){
			node_pointer = node_pointer->next;
		}	
		
		/* allocating and checking for memory  */
		node_pointer->next = (node_t*) malloc(sizeof(node_t));
		assert(node_pointer->next != NULL);
		node_pointer = node_pointer->next;
	}
	
	#if DEBUG
	{ /* DEBUG INFO */
		fprintf(stderr, "node created\n");
		fprintf(stderr, " (data:)%4c\n", in_data);
		fprintf(stderr, "(index:)%4d\n---\n", in_index);
	}
	#endif

	/* appending info to pointed node and returns it */
	node_pointer->index = in_index;
	node_pointer->data = in_data;
	node_pointer->next = NULL;
	node_pointer->child = NULL;
	return (node_pointer); 
}

//--------// AddDict() /----------------------------------------------------//

/* DESC: takes in dict_ptr (should point to last element),
 *       and creates a new dictionary factor based on 
 *       the node this factor represents, and it's parent index
 * NOTE: malloc() is used here
 */

dict_t *AddDict(dict_t *dict_ptr, node_t *in_node, unsigned int in_index){
	/* allocating and checking for memory */
	dict_ptr->next = (dict_t*) malloc(sizeof(dict_t));
	assert(dict_ptr->next != NULL);	
	
	/* appending info to next dict element and returns it */
	dict_ptr->next->node = in_node;
	dict_ptr->next->parent_index = in_index;
	dict_ptr->next->next = NULL;
	
	#if DEBUG
	{ /* DEBUG INFO */
		fprintf(stderr, "dictionary created\n");
		fprintf(stderr, "        (data:)%4c\n", in_node->data);
		fprintf(stderr, "(parent_index:)%4d\n---\n", in_index);
	}
	#endif

	return(dict_ptr->next);
}

//--------// PrintDict() //-------------------------------------------------//
/* NOTE: output should be aa xx yy zz (in hexadecimal)
 *          here index is    xx yy zz
 *            and data is aa
 */

unsigned char* PrintDict(dict_t *dict, unsigned int total_index){	
	dict_t *dict_ptr = dict;
	dict_t *temp_ptr;

	/* calculating needed allocated memory */
	size_t factor_size = WIDTH_DATA + WIDTH_INDEX;
	size_t total_size = factor_size * total_index;
	total_size = total_size + WIDTH_HEADER;
	
	/* allocating and checking for memory */
	unsigned char* output_stream = (unsigned char*) malloc(total_size);
	assert(output_stream != NULL);	
	
	unsigned char* index_char;
	
	int i = 0, j = 0;
	
	/* creating header bytes for output */
	index_char = HeaderDictSize(total_index);
	while(j < WIDTH_HEADER - 1){
		output_stream[j] = index_char[j];
		j++;
	}
	/* freeing memory */
	free(index_char);

	output_stream[j] = (unsigned char) WIDTH_DATA;
	j++;
	output_stream[j] = (unsigned char) WIDTH_INDEX;
	j++;

	/* appending dictionary for output */
	while(dict_ptr != NULL){
		index_char = IntToChar(dict_ptr->parent_index); 
		while(i < 4){
			if(i == 0){
				output_stream[j+i] = dict_ptr->node->data;
			}
			else{
				output_stream[j+i] = index_char[i-1];
			}
			i++;
		}
		temp_ptr = dict_ptr;
		dict_ptr = dict_ptr->next;
		
		/* freeing memory */
		free(index_char);
		index_char = NULL;
		free(temp_ptr->node);
		temp_ptr->node = NULL;
		free(temp_ptr);
		temp_ptr = NULL;
	
		i = 0;
		j += 4;
	}

	/* NULL-dereferencing for safety */
	dict_ptr = NULL;
	return output_stream;
}

//--------------------------------------------------------------------------//

/* unsigned_int   = 0000 0000 xxxx xxxx yyyy yyyy zzzz zzzz 
 * r_char         =                               zzzz zzzz
 * m_char         =                     yyyy yyyy
 * l_char         =           xxxx xxxx
 * 
 * NOTE: unsigned_int is 4 bytes wide, but this code assumes
 *       that the index will not be larger than 16,777,215
 *
 */
unsigned char* HeaderDictSize(unsigned int in_index){
	unsigned char char0, char1, char2, char3;
	/* segmenting into to byte chunks */
	char3 = (char) in_index;
	in_index >>= 8;
	char2 = (char) in_index;
	in_index >>= 8;
	char1 = (char) in_index;
	in_index >>= 8;
	char0 = (char) in_index;

	unsigned char *return_chars;
	
	/* allocating and checking for memory */
	return_chars = (unsigned char*) malloc(sizeof(unsigned char) * 5);
	assert(return_chars != NULL);

	/* appending to allocated memory */
	return_chars[0] = char0;
	return_chars[1] = char1;
	return_chars[2] = char2;
	return_chars[3] = char3;
	return_chars[4] = '\0';
	return return_chars;
}

unsigned char* IntToChar(unsigned int in_index){
	/* assign and shift in_index to get two halves */
	unsigned int temp = in_index;
	assert(temp < 0x01000000);
	unsigned char l_char, m_char, r_char;
	/* segmenting into to byte chunks */
	r_char = (char) in_index;
	in_index >>= 8;
	m_char = (char) in_index;
	in_index >>= 8;
	l_char = (char) in_index;
	
	#if DEBUG
	{ /* DEBUG INFO */
		fprintf(stderr, " index = %08X\n", temp);
		fprintf(stderr, "l_char = %02X\n", l_char);
		fprintf(stderr, "m_char =   %02X\n", m_char);
		fprintf(stderr, "r_char =     %02X\n", r_char);
		fprintf(stderr, "---\n");
	}
	#endif
	unsigned char *return_chars;
	
	/* allocating and checking for memory */
	return_chars = (unsigned char*) malloc(sizeof(unsigned char) * 4);
	assert(return_chars != NULL);
	
	/* appending to allocated memory */
	return_chars[0] = l_char;
	return_chars[1] = m_char;
	return_chars[2] = r_char;
	return_chars[3] = '\0';
	return return_chars;
}

//--------//  changes and notes //------------------------------------------//

/* 19 October 2015
 * decided to add a 6 bytes wide header bytes,
 * first four bytes indicates the length of the dictionary
 * last two bytes indicates the bytes width of data and index
 */


//--------------------------------------------------------------------------//

/* reference list:
 * http://www.tutorialspoint.com/cprogramming
 * https://en.wikipedia.org/wiki/Trie
 * https://en.wikipedia.org/wiki/LZ77_and_LZ78
 *
 * commenting style adapted from:
 * 2013 Assignment Sample Solution - by Alistair Moffat
 */

//--------------------------------------------------------------------------//