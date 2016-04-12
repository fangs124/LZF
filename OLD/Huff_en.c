/* Huffman Coding - Encoder - Fangs124 */

//--------//

#include <stdio.h>
#include <stdlib.h>

//--------//

typedef struct COUNTERS count_t;
typedef struct QUES que_t;
typedef struct NODES node_t;
typedef struct TABLES table_t;
typedef struct LINKEDLIST list_t;

struct COUNTERS { //leaf nodes
	node_t* parent;
	unsigned char data;
	unsigned int count;
};

struct QUES {
	node_t *node;
	unsigned int count;
	que_t *prev;
	que_t *next;
};

struct NODES { //huffman tree nodes
	node_t* parent;
	count_t* child;
	node_t* left;
	node_t* right;
};

struct TABLES {
	unsigned char* sequence;
	unsigned int bits;
};

struct LINKEDLIST {
	unsigned char data;
	list_t* prev;
	list_t* next;
};
//--------//
int CompareFrequency(const void* a, const void* b);
que_t* GenerateQue(count_t* counter, unsigned int *total);
que_t* CreateQueElement(count_t *counter, unsigned int index);
node_t* CreateLeafNode(count_t *child);
node_t* CreateCombinedNode(node_t* node1, node_t* node2);
que_t* CreateNewElement(que_t* curr, que_t* prev, node_t* node);
void InsertNewElement(que_t* priority_q, que_t* new_element);
node_t* GenerateTree(que_t* priority_q, unsigned int *total);
void GenerateHuffTable(node_t* root, table_t* huff_table);
void TraverseTree(node_t* node, list_t* list, table_t* huff_table);
unsigned char* FindSequence(list_t* last);
//--------//

int main(int argc, char* argv[]){
	/* initialize counters and table */
	count_t counter[256]; //frequency counter
	count_t debug_counter[256];
	table_t huff_table[256];
	unsigned int index = 0x00;
	while(index <= 0xFF){
		counter[index].data = (unsigned char) index;
		counter[index].count = 0;
		counter[index].parent = NULL;
		debug_counter[index].data = (unsigned char) index;
		debug_counter[index].count = 0;
		debug_counter[index].parent = NULL;
		huff_table[index].sequence = NULL;
		huff_table[index].bits = 0;
		index++;
	}

	/* step1 - count byte frequency */
	unsigned int char_buffer;
	unsigned char c;
	while((char_buffer = fgetc(stdin)) != EOF){
		c = (unsigned char) char_buffer;
		counter[c].count++;
		debug_counter[c].count++;
	}
	
	/* step2 - sort the array */
	qsort(&counter[0x00], 256, sizeof(count_t), CompareFrequency);

	/* step3 - generate priority que */
	unsigned int total = 0;
	que_t *priority_q = GenerateQue(counter, &total);

	//print que
	
	que_t *que_ptr = priority_q;
	int loop_condition = 1;
	while(loop_condition){
		fprintf((stderr), "node->child->data  = %02X\n", que_ptr->node->child->data);
		fprintf((stderr), "count  = %u\n", que_ptr->count);
		if(que_ptr->next == NULL){
			loop_condition--;
		}
		que_ptr = que_ptr->next;
	}
	//que is generated correctly

	/* step4 - construct huffman tree */
	node_t *root = GenerateTree(priority_q, &total); //segafault here
	fprintf(stderr, "tree-generated...\n");

	/* step5 - generate huffman table */

	GenerateHuffTable(root, huff_table);
	fprintf(stderr, "table-generated...\n");
	unsigned int temp;
	unsigned int db_i;
	for(db_i = 0x00; db_i <= 0xFF; db_i++){
		if(huff_table[db_i].sequence != NULL){
			fprintf(stderr, "(%02X): ", (unsigned char) db_i);
			for(temp = 0; huff_table[db_i].sequence[temp] != '\0'; temp++){
				if(huff_table[db_i].sequence[temp] == 0x0A){
					fprintf(stderr, "0");
				}
				else if(huff_table[db_i].sequence[temp] == 0x0B){
					fprintf(stderr, "1");
				}
				else{
					return 1;
				}
			}
			fprintf(stderr, "\n");
		}
	}

	fprintf(stderr, "---end of table ---\n");

	return 0;
}

//--------//


//coparison function for qsort
//sorts in terms of count, and secondarily by lexicon ordering
int CompareFrequency(const void* a, const void* b){
	count_t left = *(count_t*) a;
	count_t right = *(count_t*) b;
	int result;
	if(left.count == right.count){
		result = (int) ((unsigned char) left.data - (unsigned char) right.data);
	}
	else{
		result = (int) (left.count - right.count);
	}
	return result;
}

//--------//

que_t* GenerateQue(count_t* counter, unsigned int *total){
	que_t *que_ptr = NULL;
	que_t *priority_q;
	unsigned int index;
	for(index = 0x00; index <= 0xFF; index++){
		/* process non-empty counters */
		if(counter[index].count != 0){
			*total += counter[index].count;
			//create que element
			if(que_ptr != NULL){
				que_ptr->next = CreateQueElement(counter, index);
				que_ptr->next->prev = que_ptr;
				que_ptr = que_ptr->next;
			}
			else{
				que_ptr = CreateQueElement(counter, index);
				priority_q = que_ptr;

			}

			//que_ptr = que_ptr->next;

		}
	}
	return priority_q;
}

//--------//

que_t* CreateQueElement(count_t *counter, unsigned int index){
	que_t* element = (que_t*) malloc(sizeof(que_t));
	element->prev = NULL;
	element->next = NULL;
	element->count = counter[index].count;
	element->node = CreateLeafNode(&counter[index]);
	counter[index].parent = element->node;

	return element;
}

//--------//

node_t* CreateLeafNode(count_t *child){
	node_t* node = (node_t*) malloc(sizeof(node_t));
	node->parent = NULL;
	node->child = child;
	node->left = NULL;
	node->right = NULL;
	return node;
}

node_t* CreateCombinedNode(node_t* node1, node_t* node2){
	/* allocate memory for new_node */
	//fprintf(stderr, "here\n");
	node_t* new_node = (node_t*) malloc(sizeof(node_t));
	new_node->parent = NULL;
	new_node->child = NULL;
	/* link node1 with new_node */
	new_node->left = node1;
	node1->parent = new_node;
	/* link node2 with new_node */
	new_node->right = node2;
	node2->parent = new_node;
	return new_node;
}

que_t* CreateNewElement(que_t* curr, que_t* prev, node_t* node){
	//assign values to curr node
	curr->node = node;
	curr->count += prev->count;
	curr->prev = NULL;
	curr->next = NULL;

	//free prev pointer
	prev->node = NULL;
	prev->prev = NULL;
	prev->next = NULL;
	free(prev);

	return curr;
}

void InsertNewElement(que_t* priority_q, que_t* new_element){
	//use que_ptr to navigate priority que
	que_t* que_ptr = priority_q;

	//iterate until new_element can be inserted in the appropriate place
	while(new_element->count > que_ptr->count){
		if(que_ptr->next != NULL){
			que_ptr = que_ptr->next;
		}
		else{
			/* put new_element as last element */
			que_ptr->next = new_element;
			new_element->prev = que_ptr;
			new_element->next = NULL;
			fprintf(stderr, "blah\n");
			return;
		}
	}
	/* put new_element before que_ptr */
	//handling previous element before que_ptr
	if(que_ptr->prev != NULL){
		new_element->prev = que_ptr->prev;
		que_ptr->prev->next = new_element;
	}
	else if(que_ptr->prev == NULL){
		new_element->prev = NULL;
		priority_q = new_element;
	}
	new_element->next = que_ptr;
	que_ptr->prev = new_element;
	//priority_q = que_ptr;
	return;

}

//segfault here
node_t* GenerateTree(que_t* priority_q, unsigned int* total){
	//to generate tree, take 2 elements from que
	que_t* que_ptr1 = priority_q; //curr
	que_t* que_ptr2; //prev
	node_t* root;

	while(priority_q->count != *total){//work with stack
		/* take two elements from que and combine them */
		que_ptr2 = priority_q;
		que_ptr1 = priority_q->next; //need to check bound here
		if(que_ptr1 == NULL){
			fprintf(stderr, "que_ptr1 == NULL\n");
		}
		if(que_ptr2 == NULL){
			fprintf(stderr, "que_ptr2 == NULL\n");
		}
		if(que_ptr1->node == NULL){
			fprintf(stderr, "que_ptr1->node == NULL\n");
		}
		/*if(que_ptr2->node == NULL){
			fprintf(stderr, "que_ptr2->node == NULL\n");
		}*/
		fprintf(stderr, "here1\n");
		root = CreateCombinedNode(que_ptr1->node, que_ptr2->node);
		fprintf(stderr, "here2\n");
		que_ptr1 = CreateNewElement(que_ptr1, que_ptr2, root);
		/* create new que elements for the new node */
		if(priority_q->node == NULL){
			fprintf(stderr, "priority_q->node == NULL\n");
		}
		
		/* insert new element to the priority que */
		InsertNewElement(priority_q, que_ptr1);
		//que_ptr1 = priority_q;
		
	}
	return root;

}

void GenerateHuffTable(node_t* root, table_t* huff_table){
	list_t* list = (list_t*) malloc(sizeof(list_t));
	list->prev = NULL;
	TraverseTree(root, list, huff_table);
	//fprintf(stderr, "here generate\n");
	return;
}

void TraverseTree(node_t* node, list_t* list, table_t* huff_table){
	//fprintf(stderr, "here traverse\n");
	if(node->child != NULL){
<<<<<<< HEAD
		/* set char sequence for found alphabet */
		unsigned char index = node->child->data;
		huff_table[index].sequence = FindSequence(list);
=======
		/* assign bit-sequence */
		unsigned char index = node->child->data;
		huff_table[index].sequence = FindSequence(list);
		//fprintf(stderr, "here\n");
>>>>>>> master
	}

	else{ //traverse tree
		if(node->left != NULL){
			list->data = 0x0A; //represents bit-0
			list->next = (list_t*) malloc(sizeof(list_t));
			list->next->prev = list;
			TraverseTree(node->left, list->next, huff_table);
		}
		if(node->right != NULL){
			list->data = 0x0B; //represents bit-1
			list->next = (list_t*) malloc(sizeof(list_t));
			list->next->prev = list;
			TraverseTree(node->right, list->next, huff_table);
		}
		//free(list->next); //bad idea?
	}
	return;
}

<<<<<<< HEAD
char* FindSequence(list_t* last){
=======
unsigned char* FindSequence(list_t* last){
	//fprintf(stderr, "finding sequence\n");
>>>>>>> master
	unsigned int length = 0;
	unsigned int i;
	unsigned char* sequence;
	list_t* ptr = last;
	while(ptr->prev != NULL){
		length++;
		ptr = ptr->prev;
	}
	sequence = (unsigned char*) malloc(sizeof(char) * (length+1));
	sequence[length] = '\0';
<<<<<<< HEAD
	for(i = 0; i < length; i++){
		sequence[i] = (unsigned char) ptr->data;
		ptr = ptr->next;
	}
=======
	for(i = 0; i < length ; i++){
		sequence[i] = (unsigned char) ptr->data;
		ptr = ptr->next;
	}

>>>>>>> master
	return sequence;
}


