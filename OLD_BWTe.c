/* LZFe.c - LZ78-Fangs Universal Encoder */
/* NAME: Fangs124                        */               
/* VERS: 0.0b (not sure how this works)  */ 
/* DATE: 28 November, 2015               */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

typedef struct BINARYTREES btree_t;
struct BINARYTREES {
	char* entry;
	int entryindex;
	char* output;
	btree_t *left;
	btree_t *right;
};

char* GetString(FILE *in);
void FindNode(btree_t *node, char *string, int index);
void CreateNode(btree_t *node, char *in);
void PrintTree(btree_t *node);

unsigned int TEMP;

int main (int argc, char* argv[]){
	/* create empty tree */
	btree_t *tree = NULL;
	/* get string */
	char* string = GetString(stdin);
	int stringlength = strlen(string);
	int index = 1;
	tree = malloc(sizeof(btree_t));
	assert(tree != NULL);
	CreateNode(tree, &string[0]);
	tree->entryindex = 0;
	//fprintf(stderr, "output:%c\n", string[stringlength - 1]);
	tree->output = &string[stringlength - 1];
	while(index < stringlength){
		FindNode(tree, string, index);
		index++;
	}
	PrintTree(tree);
	fprintf(stdout, "\n");
	fprintf(stderr, "comparisons: %d\n", TEMP);

	return 0;
}

void PrintTree(btree_t *node){
	//fprintf(stderr, "at node: %c\n", *node->entry);
	if(node->left != NULL){
		PrintTree(node->left);
	}
	fprintf(stdout, "%c", *node->output);
	if(node->right != NULL){
		PrintTree(node->right);
	}
	return;
}

void FindNode(btree_t *node, char *string, int index){
	TEMP++;
	//fprintf(stderr, "finding node location for %c\n", string[index]);
	int i = 0, j = 0;
	int is_found = 0;
	int stringlength = strlen(string);
	while(is_found == 0){
		//fprintf(stderr, "comparing %c with %c\n", node->entry[j], string[index+i]);
		//continous comparison to find lexicographic ordering
		if((int)node->entry[j] > (int)string[index+i]){
			//fprintf(stderr, "%c is larger than %c\n", node->entry[j], string[index+i]);
			if(node->left == NULL){
				node->left = malloc(sizeof(btree_t));
				assert(node->left != NULL);
				CreateNode(node->left, &string[index]);
				node->left->entryindex = index;
				if(index == 0){
					node->left->output = &string[stringlength-1];
				}
				else {
					node->left->output = &string[index-1];
				}
			} 
			else {
				FindNode(node->left, string, index);
			}
			return;
		}
		else if((int)node->entry[j] < (int)string[index+i]){
			//fprintf(stderr, "%c is smaller than %c\n", node->entry[j], string[index+i]);
			if(node->right == NULL){
				node->right = malloc(sizeof(btree_t));
				assert(node->right != NULL);
				CreateNode(node->right, &string[index]);
				node->right->entryindex = index;
				if(index == 0){
					node->right->output = &string[stringlength-1];
				}
				else {
					node->right->output = &string[index-1];
				}
			}
			else {
				FindNode(node->right, string, index);
			}
			return;

		}
		//check index string boundaries
		i++;
		j++;
		//fprintf(stderr, "both char are equal, incementing..\n");
		if(string[index+i] == '\0'){
			i = 0 - index;
		}
		if(node->entry[j] == '\0'){
			j = 0 - node->entryindex;
		}
	}	
}

void CreateNode(btree_t *node, char *in){
	//fprintf(stderr, "creating node, char = %c\n", *in);

	/* appending info */
	node->entry = in;
	node->left = NULL;
	node->right = NULL;
	return;
}

char* GetString(FILE *in){
	char* string = malloc(sizeof(char));
	size_t size = 1;
	int i = 0;
	int buffer;
	//retrieve char
	while((buffer = fgetc(in)) != EOF){
		//append char
		string[i] = (char) buffer;
		i++;
		//extend string by twice if needed
		if(size == i){
			size *= 2;
			string = realloc(string, (sizeof(char) * size));
			assert(string != NULL);
		}
	}
	//null-terminating
	string[i] = '\0';
	//fprintf(stderr, "string:\n%s\n", string);
	return string;
}