/* HCe.c */
#include <stdio.h>
#include <stdlib.h>

typedef struct BUCKETS bucket_t;
struct BUCKETS {
	unsigned char data;
	unsigned int count;
};

typedef struct STACKS stack_t;
struct STACKS {
	unsigned char data;
	unsigned int count;
	stack_t *next;
};

typedef struct NODES node_t;
struct NODES {
	node_t* parent;
	unsigned int count;
	stack_t data; 
	node_t* left;
	node_t* right;
};

int CompareFrequency(const void* a, const void* b);

int main(int argc, char* argv[]){
	/* initialize counter */
	bucket_t counter[256];
	size_t index = 0x00;
	while(index <= 0xFF){
		counter[index].data = (unsigned char) index;
		counter[index].count = 0;
		index++;
	}

	/* count byte frequency */
	int buffer;
	unsigned char c;
	while((buffer = fgetc(stdin)) != EOF){
		c = (unsigned char) buffer;
		counter[c].count++;
	}

	qsort(&counter[0x00], 256, sizeof(bucket_t), CompareFrequency);
	stack_t *ptr1 = NULL;
	stack_t *stack = ptr;
	
	unsigned int total_nodes = 0;
	/* sort the array */
	index = 0x00;
	while(index <= 0xFF){
		if(counter[index].count != 0){
			total_nodes++;
			if(ptr1 == NULL){
				ptr1 = malloc(sizeof(stack_t));
			}
			else{
				ptr1->next = malloc(sizeof(stack_t));
				ptr1 = ptr1->next;
			}
			ptr1->data = counter[index].data;
			ptr1->count = counter[index].count;
			ptr1->next = NULL; 
			//fprintf(stderr, "data:   %02X\n         %c\n", counter[index].data, counter[index].data);
			//fprintf(stderr, "freq: %04u\n", counter[index].count);
		}
		index++;
	}
	//take two lowest node and conbime them, insertion sort
	ptr = stack;

	/* construct the tree */
	/* output */
	return 0;
}

int CompareFrequency(const void* a, const void* b){
	bucket_t left = *(bucket_t*) a;
	bucket_t right = *(bucket_t*) b;

	return (int) (left.count - right.count);
}