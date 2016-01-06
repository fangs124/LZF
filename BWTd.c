/* BWTe.c - Burrows Wheeler Transform Decoder */
/* NAME: Fangs124                             */
/* VERS: 0.1a                                 */
/* DATE: 6 January, 2016                      */

//--------//  preprocessors //----------------------------------------------//

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define DEBUG 0 //debug output control

//--------// struct definitions //------------------------------------------//

typedef struct NODES node_t;
struct NODES {
	unsigned char data;
	unsigned int index;
	unsigned int left_nodes;
	node_t* left;
	node_t* right; 
};

typedef struct LISTS list_t;
struct LISTS{
	unsigned char data;
	unsigned int index;
};

typedef struct STRINGS string_t;
struct STRINGS {
	unsigned char* chars;
	size_t length; //location of terminating null-byte ('\0')
};

//--------// function prototypes //-----------------------------------------//

string_t GetString(FILE *in);
node_t* InsertNode(node_t* node, unsigned char c, unsigned int index);
void CreateNode(node_t* node, unsigned char c, unsigned int index);
void CreateList(node_t* node, unsigned int size);
void PrintNode(node_t* node);
//unsigned int PrintNode(node_t* node, unsigned int index);

//--------// global variables //-------------------------------------------//

list_t *LIST;
unsigned int LIST_INDEX = 0;

//--------------------------------------------------------------------------//

int main(int argc, char* argv[]){
	//retrieve raw string from stdin
	string_t string = GetString(stdin);
	
	//seperate string and key
	int x = 4;
	unsigned int key = 0;
	unsigned int buffer;
	while(x > 0){
		buffer = (unsigned int) string.chars[string.length - x];
		key <<= 8;
		key = (key | buffer);
		x--;
	}
	/*string.chars = (unsigned char*) realloc(string.chars,
		sizeof(char) * (string.length - 3));*/
	string.length = string.length - 4;
	string.chars[string.length] = '\0';

	#if DEBUG
	fprintf(stderr, "key = %X\n", key);
	fprintf(stderr, "string = %s\n", string.chars);
	#endif

	//sort chars with tree-sort
	x = 0;
	node_t *tree = NULL;
	while(x < string.length){
		tree = InsertNode(tree, string.chars[x], x);
		x++;
	}

	CreateList(tree, string.length);

	
	// print result
	unsigned int printed_char = 0;
	while(printed_char < string.length){
		#if DEBUG
		fprintf(stderr, "key = %X\n", key);
		#endif
		fprintf(stdout, "%c", LIST[key].data);
		key = LIST[key].index;
		printed_char++;
	}
	return 0;
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
	int i = 0;
	int buffer;

	while((buffer = fgetc(in)) != EOF){
		c[i] = (unsigned char) buffer;
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

node_t* InsertNode(node_t* node, unsigned char c, unsigned int index){
	/* if node is empty, create node */
	if(node == NULL){
		node = malloc(sizeof(node_t));
		assert(node != NULL);
		CreateNode(node, c, index);
		return node;
	}

	else{
		/* compare chars */
		if((unsigned int) node->data > (unsigned int) c){
			/* go to the left branch */
			node->left_nodes++;
			node->left = InsertNode(node->left, c, index);
			return node;
		}
		else if((unsigned int) node->data <= (unsigned int) c){
			node->right = InsertNode(node->right, c, index);
			return node;
		}
	}
	/* if code reach here, something went horribly wrong */
	return NULL;
}

//--------------------------------------------------------------------------//

void CreateNode(node_t* node, unsigned char c, unsigned int index){
	#if DEBUG
	fprintf(stderr, "data = %c\n", c);
	fprintf(stderr, "index = %X\n", index);
	#endif
	node->data = c;
	node->index = index;
	node->left_nodes = 0;
	node->left = NULL;
	node->right = NULL;
	return;
}

//returns next key to be printed out
//index is the ith element on the tree
//unsigned int PrintNode(node_t* node, unsigned int index){
//	#if DEBUG
//	fprintf(stderr, "current node->data = %c\n", node->data);
//	fprintf(stderr, "index = %X\n", index);
//	#endif
//
//
//	/* if node is found */
//	if(node->left_nodes == index){
//		/* print data and return next key index */
//		fprintf(stdout, "%c", node->data);
//		return node->index;
//	}
//	else if(node->left_nodes > index){
//		return PrintNode(node->left, index);
//	}
//	else {
//		return PrintNode(node->right, (index - node->left_nodes - 1));
//	}
//	/* if code reach here, something went horribly wrong */
//	return 0;
//}

void CreateList(node_t* node, unsigned int size){
	LIST = malloc(sizeof(list_t) * size);
	assert(LIST != NULL);
	PrintNode(node);
	return;
}

void PrintNode(node_t* node){
	/* append smallest node */
	if(node->left != NULL){
		PrintNode(node->left);
	}

	LIST[LIST_INDEX].data = node->data;
	LIST[LIST_INDEX].index = node->index;
	LIST_INDEX++;

	if(node->right != NULL){
		PrintNode(node->right);
	}
	return;
}



















