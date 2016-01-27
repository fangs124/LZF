/* HCe.c */
#include <stdio.h>
#include <stdlib.h>

typedef struct BUCKETS bucket_t; //external node
typedef struct QUES que_t;
typedef struct NODES node_t;

struct QUES {
	node_t *node;
	unsigned int count;
	que_t *prev;
	que_t *next;
};

struct BUCKETS {
	node_t *parent;
	unsigned char data;
	unsigned int count;
};

struct NODES {
	node_t* parent;
	bucket_t* child;
	node_t* left;
	node_t* right;
};

typedef struct TABLES table_t;
struct TABLES {
	unsigned char data;
	unsigned char* binary_sequence;
};

int CompareFrequency(const void* a, const void* b);

int main(int argc, char* argv[]){
	/* initialize counter */
	bucket_t counter[256];
	size_t index = 0x00;
	while(index <= 0xFF){
		counter[index].data = (unsigned char) index;
		counter[index].count = 0;
		counter[index].parent = NULL;
		index++;
	}

	/* count byte frequency */
	int buffer;
	unsigned char c;
	while((buffer = fgetc(stdin)) != EOF){
		c = (unsigned char) buffer;
		counter[c].count++;
	}

	/* sort the array */
	qsort(&counter[0x00], 256, sizeof(bucket_t), CompareFrequency);
	unsigned int highest_freq = counter[0x00].count;
	unsigned char h_c = counter[0x00].data;
	unsigned int lowest_freq = 0;
	unsigned char l_c = counter[0x00].data;
	/* convert the array into nodes and stacks */
	que_t *current = NULL;
	que_t *stack = current; //
	unsigned int total = 0;
	unsigned int elements = 0;
	for(index = 0x00; index <= 0xFF; index++){
		if(counter[index].count != 0){
			//
			if(lowest_freq == 0){
				lowest_freq = counter[index].count;
				l_c = counter[index].data;
			}
			else if(lowest_freq > counter[index].count){
				lowest_freq = counter[index].count;
				h_c = counter[index].data;
			}
			if(highest_freq <= counter[index].count){
				highest_freq = counter[index].count;
				h_c = counter[index].data;
			}
			//
			elements++;
			total += counter[index].count;
			/* initialize stack */
			if(current == NULL){
				current = (que_t*) malloc(sizeof(que_t));
				stack = current;
				current->prev = NULL;
			}
			else{
				current->next = (que_t*) malloc(sizeof(que_t));
				current->next->prev = current;
				current = current->next;
				
			}
			current->count = counter[index].count;
			current->next = NULL;

			/* initialize node */
			current->node = (node_t*) malloc(sizeof(node_t));
			current->node->parent = NULL;
			current->node->child = &counter[index];
			current->node->left = NULL;
			current->node->right = NULL;
			counter[index].parent = current->node; 
		}
	}

	fprintf(stderr, "total = %u\n", total);
	fprintf(stderr, "elements = %u\n", elements);
	
	/* construct the tree */
	current = stack;
	que_t *temp;
	node_t *node;
	while(current->count != total ){

		//fprintf(stderr, "stack length = %u\n", elements);
		//fprintf(stderr, "stack->count = %u\n", stack->count);
		//take two element from the stack
		temp = current;
		current = current->next;
		//form a node from the two elements
		node = (node_t*) malloc(sizeof(node_t));
		node->parent = NULL;
		node->child = NULL;
		node->left = temp->node; //link them together
		//fprintf(stderr, "here3\n");
		temp->node->parent = node;
		node->right = current->node;
		current->node->parent = node;
		//deatach the two elements from link and update 
		//fprintf(stderr, "here1\n");
		if(current->next != NULL){
			stack = current->next;
			current->next->prev = NULL;
		}
		else{
			stack = current;
		}
		//fprintf(stderr, "here2\n");
		//form a new stack element from the new node by recycling current node
		current->node = node;
		current->count = current->count + temp->count;
		current->prev = NULL;
		current->next = NULL;
		//delete temp node;
		elements--;
		temp->node = NULL;
		temp->prev = NULL;
		temp->next = NULL;
		free(temp);

		//insert new element into stack
		temp = current;
		current = stack;
		while(temp->count > current->count){
			/* keep iterating */ 
			if(current->next != NULL){
				current = current->next;
			}
			else{
				/* put it as last element */
				current->next = temp;
				temp->prev = current;
				temp->next = NULL;
				current = NULL;
				break;
			}
		}
		if(current != NULL){
			/* put temp before current */
			temp->next = current;
			if(current->prev != NULL){
				temp->prev = current->prev;
				current->prev->next = temp;
			}
			else if(current->prev == NULL){
				temp->prev = NULL;
				// if first element, update stack
				stack = temp;
			}
			current->prev = temp;
		}
		current = stack;
		//fprintf(stderr, "current->count = %u\n", current->count);
	}
	fprintf(stderr, "finished!\n");
	fprintf(stderr, "highest_freq = %u\nchar: %02X %c\n", highest_freq, h_c, h_c);
	fprintf(stderr, "lowest_freq = %u\nchar: %02X %c\n", lowest_freq, l_c, l_c);
	
	/* generate huffman table */

	
	//what to do with the huffman tree now..
	//fuck

	/* output */
	return 0;
}

int CompareFrequency(const void* a, const void* b){
	bucket_t left = *(bucket_t*) a;
	bucket_t right = *(bucket_t*) b;

	return (int) (left.count - right.count);
}
