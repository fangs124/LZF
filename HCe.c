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
	unsigned char* sequence;
	unsigned int bits;
};

typedef struct DICTIONARY dict_t;
struct DICTIONARY {
	unsigned int length;
	unsigned char data;
	table_t *tables;
};

typedef struct LISTS list_t;
struct LISTS {
	unsigned char data;
	list_t *prev;
	list_t *next;
};

void TraverseTree(node_t* node, list_t* list);
table_t* GenerateTable(node_t* root);
int CompareFrequency(const void* a, const void* b);

table_t TABLE[256];

int main(int argc, char* argv[]){
	/* initialize counter */
	bucket_t counter[256];
	size_t index = 0x00;
	while(index <= 0xFF){
		counter[index].data = (unsigned char) index;
		counter[index].count = 0;
		counter[index].parent = NULL;
		TABLE[index].sequence = NULL;
		TABLE[index].bits = 0;
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

	//fprintf(stderr, "total = %u\n", total);
	//fprintf(stderr, "elements = %u\n", elements);
	
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
	//fprintf(stderr, "finished!\n");
	//fprintf(stderr, "highest_freq = %u\nchar: %02X %c\n", highest_freq, h_c, h_c);
	//fprintf(stderr, "lowest_freq = %u\nchar: %02X %c\n", lowest_freq, l_c, l_c);
	
	/* generate huffman table */
	node_t* root = node;
	list_t* list = NULL;
	TraverseTree(root, list);
	/* generate huffman table and print the stderr output */
	unsigned int i;
	unsigned int j;
	unsigned int factor_count = 0;
	for(i = 0x00; i <= 0xFF; i++){
		if(TABLE[i].sequence != NULL){
			factor_count++;
			//fprintf(stderr, "char: %c (%02X)\nsequence: ", 
			//	(unsigned char) i, i);
			j = 0x00;
			while(TABLE[i].sequence[j] != '\0'){
				TABLE[i].bits++;
				/*
				if(TABLE[i].sequence[j] == 0x0A){
					fprintf(stderr, "0");
				}
				else if(TABLE[i].sequence[j] == 0x0B){
					fprintf(stderr, "1");
				}
				*/
				j++;
			}
			//fprintf(stderr, "\nlength: %u\n", TABLE[i].bits);
		}
	}

	dict_t dictionary[factor_count];

	index = 0x00;
	j = 0;
	unsigned int length_search = 1;
	while(index < factor_count){
		for(i = 0x00; i <= 0xFF; i++){
			if(TABLE[i].bits == length_search){
				dictionary[index].length = length_search;
				dictionary[index].data = (unsigned char) i;
				dictionary[index].tables = &TABLE[i];
				index++;
			}
		}
		length_search++;
	}
	/*
	fprintf(stderr, "-----------------------\n");
	for(i = 0x00; i < factor_count; i++){
		fprintf(stderr, "char: %c (%02X)\n", dictionary[i].data, dictionary[i].data);
		fprintf(stderr, "sequence length: %u\n", dictionary[i].length);
	}
	fprintf(stderr, "-----------------------\n");
	*/
	/* output */
	//first 4byte : number of factors for the dictionary
	unsigned int factor_copy = factor_count;
	unsigned char* output;
	for(i = 0; i < 4; i++){
		c = (unsigned char) factor_copy;
		fprintf(stdout, "%c", c);
		factor_copy >>= 8;
	}
	unsigned int x = 0;
	unsigned char char_buffer = 0;
	//unsigned int printed = 0x00;
	unsigned int shifts = 0;
	output = 0x00;
	c = 0x00;
	unsigned char* c_ptr;
	unsigned char bit_wise_copy;
	/* print the dictionary */
	for(i = 0x00; i < factor_count; i++){
		j = 0;
		c_ptr = dictionary[i].tables->sequence;
		/*  print the sequence */
		while(c_ptr[j] != '\0'){
			if(c_ptr[j] == 0x0B){
				//fprintf(stderr, "1");
				c = (c | 0x01);
			}
			else{
				//fprintf(stderr, "0");
			}

			if(shifts != 7) {
				//fprintf(stderr, " ");
				c <<= 1;
				shifts++;
			}
			else{
				fprintf(stdout, "%c", c);
				c = 0x00;
				shifts = 0;

			}
			j++;
		}
		/* print the char-byte */
		x = 0;
		char_buffer = dictionary[i].data;
		while(x < 8){
			//fprintf(stderr, "%02X ",  char_buffer);
			if(char_buffer & 0x80){
				c = (c | 0x01);
			}
			else{

			}
			if(shifts != 7){
				c <<= 1;
				shifts++;
			}
			else{
				fprintf(stdout, "%c", c);
				c = 0x00;
				shifts = 0;

			}
			char_buffer <<= 1;
			x++;
		}
	}
	/* next 4 bytes: number of chars encoded */
	/* print the file */
	rewind(stdin);
	unsigned char c_index = 0x00;
	while((buffer = fgetc(stdin)) != EOF){
		c_index = (unsigned char) buffer;
		//fprintf(stderr, "here\n");
		c_ptr = TABLE[c_index].sequence;
		j = 0;
		while(c_ptr[j] != '\0'){
			if(c_ptr[j] == 0x0B){
				//fprintf(stderr, "1");
				c = (c | 0x01);
			}
			else{
				//fprintf(stderr, "0");
			}
			if(shifts != 7) {
				//fprintf(stderr, " ");
				c <<= 1;
				shifts++;
			}
			else{
				fprintf(stdout, "%c", c);
				c = 0x00;
				shifts = 0;

			}
			j++;
		}
	}
	fprintf(stderr, "\n");
	if(shifts != 0){
		unsigned int bits_left = 7 - shifts;
		for(i = 0; i < bits_left; i++){
			c <<= 1;
		}
		fprintf(stdout, "%c", c);
	}


	return 0;
}


void TraverseTree(node_t* node, list_t* list){
	if(node->child != NULL){
		unsigned int length = 0;
		unsigned int i;
		unsigned char index = node->child->data;
		list_t* ptr;
		for(ptr = list; ptr->prev != NULL; ptr = ptr->prev){
			length++;
		}
		TABLE[index].sequence = (
			unsigned char*) malloc(sizeof(char) * (length+1));
		TABLE[index].sequence[length] = '\0';
		for(i = 0; i < length; i++){
			TABLE[index].sequence[i] = ptr->data;
			ptr = ptr->next;
		}
	}

	else{
		if(list == NULL){
			list = (list_t*) malloc(sizeof(list_t));
			list->prev = NULL;
			list->next = NULL;
		}

		if(node->left != NULL){
			list->data = 0x0A;
			list->next = (list_t*) malloc(sizeof(list_t));
			list->next->prev = list;
			TraverseTree(node->left, list->next);
		}
		if(node->right != NULL){
			list->data = 0x0B;
			list->next = (list_t*) malloc(sizeof(list_t));
			list->next->prev = list;
			TraverseTree(node->right, list->next);
		}
	}
	return;
}

int CompareFrequency(const void* a, const void* b){
	bucket_t left = *(bucket_t*) a;
	bucket_t right = *(bucket_t*) b;
	if(left.count == right.count){
		return (int) ((unsigned int) left.data - (unsigned int) right.data);
	}
	else{
		return (int) (left.count - right.count);
	}
}
