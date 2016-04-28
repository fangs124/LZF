/* bit field experimentation */
//doesnt work.

#include <stdio.h>
#include <stdlib.h>

typedef (unsigned char : 1) bit_t;
typedef struct BINARIES {
	size_t bits : 4;
	bit_t* data;
} Bin_t;
int main(int argc, char* argv[]){
	bit_t* bit_ptr;
	printf("sizeof(bit_t):%2ld\n", sizeof(bit_t));
	printf("sizeof(bit_t*):%2ld\n", sizeof(bit_t*));
	printf("sizeof(bit_ptr):%2ld\n", sizeof(bit_ptr));
	return 0;
}