/* MTFe.c - "Move To Front" Encoder */
#include <stdio.h>
#include <stdlib.h>

typedef struct NODES node_t;
struct NODES {
	unsigned char data;
	node_t* prev;
	node_t* next;
};

int main(int argc, char* argv[]){
	node_t *dictionary = (node_t*) malloc(sizeof(node_t) * 256);
	size_t index = 0x00;
	
	/* initialize dictionary */
	while(index <= 0xFF){
		//fprintf(stderr, "index = %02X\n", index);
		dictionary[index].data = (unsigned char) index;
		if(index == 0x00){
			dictionary[index].prev = NULL;
		}
		else{
			dictionary[index].prev = &dictionary[index-1];
			dictionary[index-1].next = &dictionary[index];
			if(index == 0xFF){
				dictionary[index].next = NULL;
			}
		}
		index++;
	}

	node_t *ptr;
	node_t *first = &dictionary[0x00];
	int buffer;
	unsigned char c;

	/* process byte-stream */
	while((buffer = fgetc(stdin)) != EOF){
		c = (unsigned char) buffer;

		/* find dictionary element */
		index = 0x00;
		ptr = first; 
		while(ptr->data != c){		
			ptr = ptr->next;
			index++;
		}

		/* output the said value */
		fprintf(stdout, "%c", (unsigned char) index);

		if(ptr != first){
			/* move to front */
			if(ptr->next == NULL){
				ptr->prev->next = NULL;
			}
			else if(ptr->prev == NULL){
				ptr->next->prev = NULL;
			}
			else{
				ptr->next->prev = ptr->prev; //link prev with next
				ptr->prev->next  = ptr->next;
			}
				
			ptr->next = first; //put ptr infront of first
			first->prev = ptr;
			ptr->prev = NULL;
		}
		first = ptr;
	}


	return 0;
}