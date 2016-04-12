#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#define NMEMB 4096
#define BYTESIZE 1
#define GRAPHS 0
//--------------------------------------------------------------------------//

typedef struct TABLES table_t;
struct TABLES {
	unsigned int size; //TABLE[n], TABLE[n-1] is the last element
	char* data;
	int* value;
};

typedef struct STRINGS string_t;
struct STRINGS {
	unsigned int size; //location of terminating null-byte ('\0')
	char* chars;
};

//--------------------------------------------------------------------------//
table_t *GenerateTable(unsigned int size, char* string);
string_t GetString(FILE *in);
//--------------------------------------------------------------------------//

int main(int argc, char* argv[]){
	string_t string = GetString(stdin);
	table_t *table = GenerateTable(string.size, string.chars);
	#if GRAPHS
	unsigned int index = 0;
	while(index < t able->size){
		fprintf(stderr, "|%2c ", table->data[index]);
		index++;
	}
	fprintf(stderr, "|\n");
	index = 0;
	while(index < table->size){
		fprintf(stderr, "|%2d ", table->value[index]);
		index++;
	}
	fprintf(stderr, "|\n");
	#endif
	return 0;
}

//--------------------------------------------------------------------------//

table_t *GenerateTable(unsigned int size, char* string){
	table_t* table = malloc(sizeof(table_t));
	assert(table != NULL);
	table->size = size;
	table->data = string;
	table->value = malloc(size);
	assert(table->value != NULL);

	unsigned int i;
	unsigned int j = 0;
	//int value = 0;
	table->value[0] = -1; //setting first case fail
	//want to check string[i] and substring[0]
	//where i always increments by 1
	for(i = 1; i < size; i++){
		table->value[i] = j;
		
		if(string[i] == string[j]){
			j++;
		}
		
		else{
			j = 0;
		}
	}
	return table;
}

//--------------------------------------------------------------------------//

string_t GetString(FILE *in){
	/* initialize memory space for c */	
	char *c = malloc(BYTESIZE*NMEMB);
	assert(c != NULL);
	size_t size = fread(c, BYTESIZE, NMEMB, in);
	c = realloc(c, (BYTESIZE*(size+1)));
	c[size] = '\0';

	/* creating string and returns */
	string_t string;
	string.chars = c;
	string.size = size;
	return string;
}
