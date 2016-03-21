#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
typedef struct TABLES table_t;
struct TABLES { //(data, value) pairs
	unsigned int size; //last element is TABLES[size-1]
	unsigned char* data;
	unsigned int* value;
};

int main(int argc, char* argv[]){

}

table_t* GenerateTable(char* substring, unsigned int size){
	table_t* table = malloc(sizeof(table_t));
	assert(table != NULL);

}