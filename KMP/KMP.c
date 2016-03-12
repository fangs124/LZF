#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


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
	
	unsigned int index = 0;
	while(index < table->size){
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

	char* substring = &string[0];
	unsigned int i;
	unsigned int j = 0;
	//int value = 0;
	table->value[0] = -1; //setting first case fail
	//want to check string[i] and substring[0]
	//where i always increments by 1
	for(i = 1; i < size; i++){
		table->value[i] = j;
		
		if(string[i] == substring[j]){
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
	unsigned int size = sizeof(char);
	char *c = malloc(size);
	assert(c != NULL);

	/* retrieve char to buffer from in */
	unsigned int i = 0;
	int buffer;

	while((buffer = fgetc(in)) != EOF){
		c[i] = (char) buffer;
		i++;

		/* extend c if needed */
		if(size == i){
			size *= 2;
			c = realloc(c, size);
			assert(c != NULL);
		}
	}
	/* trimming memory space for c */
	c = realloc(c, (sizeof(char)*(i+1)));
	c[i] = '\0';

	/* creating string and returns */
	string_t string;
	string.chars = c;
	string.size = i;
	return string;
}
