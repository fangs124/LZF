#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "bins.h"

#define MAX_BUFFER_SIZE 0xFFFF

/* comparison function for quick sort */
static int cmp(const void* a, const void* b);

/* allocate new bin */
Bin_t new_bin(void){
	Bin_t bin = (Bin_t) malloc(sizeof(*bin));
	bin->count = (unsigned int*) malloc(sizeof(unsigned int) * 0x100);
	bin->count = memset(bin->count, 0x00, 0x100);
	return bin;
}

/* free bin */
void free_bin(Bin_t bin){
	free(bin->count);
	bin->count = NULL;
	free(bin);
	bin = NULL;
	return;
}

/* count probability using bin */
void count_bin(Bin_t* bin, FILE* fp){
	unsigned int i;
	size_t char_count;
	bool EOF_REACHED = false;

	/* allocate memory */
	byte_t* buffer = (byte_t*) malloc(sizeof(byte_t) * MAX_BUFFER_SIZE);
	
	/* load buffer and count */
	while(!EOF_REACHED){
		char_count = fread(buffer, sizeof(byte_t), MAX_BUFFER_SIZE, fp);
		for(i = 0; i < char_count; i++) bin->count[buffer[i]] += 1;

		if(char_count != MAX_BUFFER_SIZE)
			EOF_REACHED = true;
	}
}

/* sort bin based on probability */
void sort_bin(Bin_t* bin){
	qsort(&bin->count[0x00], 0x100, sizeof(unsigned int), cmp);
}

/* comparison function for quick sort */
static int cmp(const void* a, const void* b){
	return (int) ((unsigned int*)a - (unsigned int*)b);
}