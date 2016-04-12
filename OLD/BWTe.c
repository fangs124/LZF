/* BWTe.c - Burrows Wheeler Transform Encoder */
/* NAME: Fangs124                             */
/* VERS: 0.1c                                 */
/* DATE: 9 December, 2015                     */

//--------------------------------------------------------------------------//

/* 

The BWT works by sorting all strings rotation lexicographically,
where the output is the last characters of all the sorted rotations.

Here a string rotation of length L is defined as string[x+i], where x is
the starting index of the rotation, and i increments from 0 to L. e.g. 
let string = "^BANANA", then the string rotations are as follow:

string[0+i] = "^BANANA"
string[1+i] = "BANANA^"
string[2+i] = "	ANANA^B"
and so on...

This program fundementally sorts the rotations by linear comparison iterating
through the string, and storing its result in a binary tree to reduce the
number of comparison needed.

In other words, the program will compare string[x+i] with string [j+i],
where x is the starting index of the currently processed rotation,
while j is the starting index of the rotation  stored in the current node.

*/

//--------//  preprocessors //----------------------------------------------//

/* libraries */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/* output control */
#define DEBUG         0

/* constants */
#define TRUE          1
#define FALSE         0

//--------// struct definitions //------------------------------------------//

typedef struct BINARYTREES btree_t;
struct BINARYTREES {
	unsigned char *entry;
	size_t entryindex;
	unsigned char *output; // equivalent to string[x-1]
	btree_t *left;
	btree_t *right;
};

typedef struct ROOTS root_t;
struct ROOTS {
	unsigned char value;
	btree_t *node;
};

typedef struct STRINGS string_t;
struct STRINGS {
	unsigned char* chars;
	size_t length; //location of terminating null-byte ('\0')
};

//--------// function prototypes //-----------------------------------------//

btree_t *InsertNode(btree_t *node, string_t *string, size_t index);
void CreateNode(btree_t *node, string_t *string, size_t index);
void PrintTree(btree_t *node);
string_t GetString(FILE *in);

//--------// global variables //-------------------------------------------//

size_t EOF_INDEX;
size_t COUNTER;
size_t KEY;

//--------------------------------------------------------------------------//

int main(int argc, char* argv[]){
	/* initializing root nodes */
	root_t rootnodes[256];
	size_t index = 0x00;
	while(index <= 0xFF){
		rootnodes[index].value = (unsigned char) index;
		rootnodes[index].node = NULL;
		index++;
	}

	/* retrieve string */
	string_t string = GetString(stdin);

	size_t i = 0;
	/* insert char into binary tree */
	while(i < string.length){
		index = (unsigned char) string.chars[i];
		rootnodes[index].node = InsertNode(rootnodes[index].node, &string, i);
		i++;
	}

	index = 0x00;
	while(index <= 0xFF){
		if(rootnodes[index].node != NULL){
			PrintTree(rootnodes[index].node);
		}
		index++;
	}
	//print EOF index
	fprintf(stderr, "COUNTER == %zu\n", KEY);
	int x = 0;
	unsigned char *c = malloc(sizeof(char) * 8);
	while(x < 8){
		c[7-x] = (unsigned char)KEY;
		KEY >>= 8;
		x++;
	}
	x = 0;
	//temporary makeshift way to print int in the right order as raw char
	while(x < 8){
		fprintf(stdout, "%c", c[x]);
		x++;
	}
	return 0;
}

//--------------------------------------------------------------------------//

btree_t *InsertNode(btree_t *node, string_t *string, size_t index){
	/* if node is empty assign value to node */
	if(node == NULL){
		node = malloc(sizeof(btree_t));
		assert(node != NULL);
		CreateNode(node, string, index);
		return node;
	}
	else{
		/* comnpare string */
		int i = 0, j = 0;
		unsigned char entry = string->chars[node->entryindex]; 
		unsigned char chars = string->chars[index];
		/* linear search until chars are not the same */
		while((int)entry == (int)chars){
			i++;
			j++;
			entry = string->chars[node->entryindex + j];
			chars = string->chars[index + i];
			/* check if index is out of bounds */
			if(entry == '\0'){
				j = 0 - node->entryindex;
				entry = string->chars[node->entryindex + j];
			}
			if(chars == '\0'){
				i = 0 - index;
				chars = string->chars[index + i];
			}
		}
		/* if node entry is larger than char string */
		if((int)node->entry[j] > (int)string->chars[index+i]){
			node->left = InsertNode(node->left, string, index);
			return node;
		}
		/* if node entry is smaller than char string */
		else if((int)node->entry[j] < (int)string->chars[index+i]){
			node->right = InsertNode(node->right, string, index);
			return node;
		}
	}
	return node;
}

//--------------------------------------------------------------------------//

void CreateNode(btree_t *node, string_t *string, size_t index){
	#if DEBUG
	fprintf(stderr, "creating node for %c\n", string->chars[index]);
	#endif
	/* appending node info */
	node->entry = &string->chars[index];
	node->left = NULL;
	node->right = NULL;
	node->entryindex = index;
	/* adjusting string bound output */
	if(index == 0){
		EOF_INDEX = index;
		node->output = &string->chars[string->length-1];
	}
	else{
		node->output = &string->chars[index-1];
	}
	return;
}

//--------------------------------------------------------------------------//

string_t GetString(FILE *in){
	#if DEBUG
	fprintf(stderr, "retrieving string: ");
	#endif

	/* initialize memory space for c */	
	size_t size = sizeof(char);
	unsigned char *c = malloc(size);
	assert(c != NULL);

	/* retrieve char to buffer from in */
	size_t i = 0;
	int buffer;

	while((buffer = fgetc(in)) != EOF){
		c[i] = (char) buffer;
		i++;

		/* extend c if needed */
		if(size == i){
			size *= 2;
			c = realloc(c, size);
			assert(c != NULL);
		}
	}
	/* trimming memory space for c */
	c = realloc(c, (sizeof(char)*(i+1)));
	c[i] = '\0';

	#if DEBUG
	fprintf(stderr, "%s\nlength = %d\n------\n", c, i);
	#endif

	/* creating string and returns */
	string_t string;
	string.chars = c;
	string.length = i;
	return string;
}

//--------------------------------------------------------------------------//

void PrintTree(btree_t *node){
	/* prints smallest value */
	if(node->left != NULL){
		PrintTree(node->left);
	}
	if(EOF_INDEX == node->entryindex){
		KEY = COUNTER;
	}
	else{
		COUNTER++;
	}
	fprintf(stdout, "%c", *node->output);
	/* check for right branch */
	if(node->right != NULL){
		PrintTree(node->right);
	}
	return;
}