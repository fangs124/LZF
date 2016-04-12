/* BWTe.c - Burrows Wheeler Transform Decoder */
/* NAME: Fangs124                             */
/* VERS: 0.2a                                 */
/* DATE: 8 January, 2016                      */

//--------//  preprocessors //----------------------------------------------//

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

typedef struct STRINGS string_t;
struct STRINGS {
	unsigned char* chars;
	size_t key;
	size_t length; // location of terminating null-byte ('\0')
};

typedef struct NODES node_t;
struct NODES {
	unsigned char* data;
	size_t index;
	//node_t* next;
};

typedef struct BUCKETS bucket_t;
struct BUCKETS {
	size_t child_count;
	size_t size;
	node_t* childs;
};


//--------------------------------------------------------------------------//
string_t GetString(FILE *in);
//--------------------------------------------------------------------------//
int main(int argc, char* argv[]){
	/* get string from stdin */
	string_t string = GetString(stdin);
	//fprintf(stderr, "%s\n", string.chars);
	fprintf(stderr, "%zu\n", string.length);
	fprintf(stderr, "%zX\n", string.key);
	/* initialize buckets */
	bucket_t root[256];
	size_t index = 0x00;
	while(index <= 0xFF){
		root[index].child_count = 0;
		root[index].size = 2;
		root[index].childs = malloc(sizeof(node_t) * root[index].size);
		//fprintf(stderr, "%X", string.chars[index]);
		index++;
	}

	/* bucket sort */
	index = 0x00;
	unsigned char c;
	while(index < string.length){
		c = string.chars[index];
		root[c].childs[root[c].child_count].data = &string.chars[index];
		root[c].childs[root[c].child_count].index = index;
		//root[c].childs[root[c].child_count].next = NULL;
		root[c].child_count++;
		if(root[c].child_count == root[c].size){
			root[c].size *= 2;
			root[c].childs = realloc(root[c].childs, 
				sizeof(node_t) * root[c].size);
			assert(root[c].childs != NULL);
		}
		index++;
	}

	index = 0x00;
	size_t key = string.key;
	while(index < string.length){
		c = 0x00;
		while(key + 1 > root[c].child_count){
			key -= root[c].child_count;
			c++;
		}
		//fprintf(stderr, "%X\n", key);
		fprintf(stdout, "%c", *root[c].childs[key].data);
		key = root[c].childs[key].index;
		c = 0x00;
		index++;
	}


	//fprintf(stderr, "size_t = %zu\n", sizeof(size_t));
	//fprintf(stderr, "unsigned int = %zu\n", sizeof(unsigned int));
	return 0;
}

string_t GetString(FILE *in){
	#if DEBUG
	fprintf(stderr, "retrieving string: ");
	#endif

	/* initialize memory space for c */	
	size_t size = sizeof(char);
	unsigned char *c = malloc(size);
	assert(c != NULL);

	/* retrieve char to buffer from in */
	size_t i = 0;
	int buffer;

	while((buffer = fgetc(in)) != EOF){
		c[i] = (unsigned char) buffer;
		i++;

		/* extend c if needed */
		if(size == i){
			size *= 2;
			c = realloc(c, size);
			assert(c != NULL);
		}
	}
	/* retrieving key */
	int x = 8;
	unsigned char byteblock;
	size_t key;
	while(x > 0){
		byteblock = c[i - x];
		key <<= 8;
		key = (key | byteblock);
		x--;
	}
	/* trimming memory space for c */
	c = realloc(c, (sizeof(char)*(i - 7)));
	c[i - 7] = '\0';

	#if DEBUG
	fprintf(stderr, "%s\nlength = %d\n------\n", c, i);
	#endif

	/* creating string and returns */
	string_t string;
	string.key = key;
	string.chars = c;
	string.length = i - 8;
	return string;
}


