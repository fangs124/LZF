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
}

struct TABLES {
	unsigned char* sequence;
	unsigned int bits;
}

struct LINKEDLIST {
	unsigned char data;
	list_t* prev;
	list_t* next;
}
//--------//
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
		huff_table[index].sequence = NULL
		huff_table[index].bits = 0;
		index++;
	}

	/* step1 - count byte frequency */
	unsigned int char_buffer
	unsigned char c;
	while((char_buffer = fgetc(stdin)) != EOF){
		c = (unsigned char) char_buffer;
		counter[c].count++;
		debug_counter[c].count++;
	}
	
	/* step2 - sort the array */
	qsort(&counter[0x00], 256, sizeof(count_t), CompareFrequency);

	/* step3 - generate priority que */
	unsigned int elements = 0;
	unsigned int total = 0;
	que_t *priority_q = GenerateQue(&counter, &elements, &total);

	/* step4 - construct huffman tree */
	node_t *root = GenerateTree(priority_q, &total, &elements);

	/* step5 - generate huffman table */
	huff_table = *GenerateHuffTable(root, &huff_table);
	return 0;
}

//--------//


//coparison function for qsort
//sorts in terms of count, and secondarily by lexicon ordering
int CompareFrequency(const void* a, const void* b){
	bucket_t left = *(bucket_t*) a;
	bucket_t right = *(bucket_t*) b;
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

que_t* GenerateQue(count_t* counter, unsigned int *elements, unsigned int *total){
	que_t *que_ptr = NULL;
	que_t *priority_q = que_ptr;
	unsigned int index;
	for(index = 0x00; index <= 0xFF; index++){
		/* process non-empty counters */
		if(counter[index].count != 0){
			elements++;
			total += counter[index]->count;
			//create que element
			que_ptr = CreateQueElement(que_ptr, counter, index);

		}
	}
	return priority_q;
}

//--------//

que_t* CreateQueElement(que_t* current, count_t *counter, unsigned int index){
	current = (que_t*) malloc(sizeof(que_t));
	current->prev = NULL;
	current->next = NULL;
	current->count = counter[index].count;
	current->node = CreateLeafNode(current->node, &counter[index]);
	counter[index]->parent = current->node;

	return current->next;
}

//--------//

node_t* CreateLeafNode(node_t* node, count_t *child){
	node = (node_t*) malloc(sizeof(node_t));
	node->parent = NULL;
	node->child = child;
	node->left = NULL;
	node->right = NULL;
	return node;
}
node_t* CreateCombinedNode(node_t* node1, node_t* node2){
	/* allocate memory for new_node */
	node_t* new_node = (node_t*) malloc(sizeof(node_t));
	new_node->parent = NULL;
	new_node->child = NULL;
	/* link node1 with new_node */
	new_node->left = node1;
	node1->parent = new_node;
	/* link node1 with new_node */
	new_node->right = node2;
	node2->parent = new_node;
	return new_node;
}

que_t* CreateNewElement(que_t* current, que_t* prev,  node_t* node){
	current->node = node;
	current->count += prev->count;
	current->prev = NULL;
	current->next = NULL;
	prev->node = NULL;
	prev->prev = NULL;
	prev->next = NULL;
	free(prev);

}

que_t* InsertNewElement(que_t* priority_q, que_t* new_element){
	que_t* que_ptr = priority_q;
	/* find the first element that is less or equal to new_element->count */
	while(new_element->count > que_ptr->count){
		if(que_ptr->next != NULL){
			que_ptr = que_ptr->next;
		}
		else{
			/* put new_element as last element */
			que_ptr->next = new_element;
			new_element->prev = que_ptr;
			new_element->next = NULL;
			return priority_q;
		}
	}

	if(que_ptr != NULL){
		/* put new_element before que_ptr */
		new_element->next = que_ptr;
		if(que_ptr->prev != NULL){
			new_element->prev = que_ptr->prev;
			que_ptr->prev->next = new_element;
		}
		else if(que_ptr->prev == NULL){
			new_element->prev = NULL;
			priority_q = new_element;
		}
		que_ptr->prev = new_element;
	}
	return priority_q;
}

node_t* GenerateTree(que_t* priority_q, unsigned int *total, unsigned int *elements){
	que_t* list_ptr1 = priority_q; //current
	que_t* list_ptr2; //previous
	node_t* root;
	//keep combining two nodes unstil probability == 1, i.e. count == total
	while(list_ptr1->count != *total){
			list_ptr2 = list_ptr1;
			list_ptr1 = list_ptr1->next;
			root = CreateCombinedNode(list_ptr2->node, list_ptr1->node);
			/* if there is only two elements left, skip */
			if(list_ptr1->next == NULL){
				priority_q = list_ptr1;
			}
			/* otherwise, de-link the first element */
			else{
				priority_q = current->next;
				current->next->prev = NULL;
			}
			/* make new priority_q element */
			list_ptr1 = CreateNewElement(list_ptr1, list_ptr2, root);
			priority_q = InsertNewElement(priority_q, list_ptr1);
	}
	return root;
}

table_t* GenerateHuffTable(node_t* root, table_t* huff_table){
	list_t* list = (list_t*) malloc(sizeof(list_t));
	TraverseTree(root, list, huff_table);
}

void TraverseTree(node_t* node, list_t* list, table_t* huff_table){
	if(node->child != NULL){
		unsigned char index = node->child->data;
		huff_table[index].sequence = FindSequence(index, list, huff_table);
		//base case, end reccursion 
		//count length of sequence
		//create table element
		/* find char index */
	}

	else{ //traverse tree
		list->next = (list_t*) malloc(sizeof(list_t));
		list->next->prev = list;
		if(node->left != NULL){
			list->data = 0x0A;
			TraverseTree(node->left, list->next);
		}
		if(node->right != NULL){
			list->data = 0x0B;
			TraverseTree(node->right, list->next);
		}
	}
	return;
}

char* FindSequence(unsigned char index, list_t* last, table_t* huff_table){
	unsigned int length = 0;
	unsigned int i;
	unsigned char* sequence;
	list_t* ptr;
	for(ptr = last; ptr->prev != NULL; ptr = ptr->prev){
		length++;
	}
	sequence = (unsigned char*) malloc(sin  zeof(char) * (length+1));
	sequence[length] = '\0';
	for(i = 0; i < length; i++){
		sequence[i] = ptr->data;
		ptr = ptr->next;
	}

}


