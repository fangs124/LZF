/* BWTe.c - Burrows Wheeler Transform Decoder */
/* NAME: Fangs124                             */
/* VERS: 0.0a                                 */
/* DATE: 4 January, 2016                      */

//--------//  preprocessors //----------------------------------------------//

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

typedef struct STRINGS string_t;
struct STRINGS {
	unsigned char* chars;
	size_t length; //location of terminating null-byte ('\0')
};


typedef struct BINARYTREES btree_t;
struct BINARYTREES {
	char *entry;
	unsigned int entryindex;
	char *output;
	btree_t *left;
	btree_t *right;
};

typedef struct LISTS list_t;
struct LISTS {
	char* data;//tells you what char is stored in here
	unsigned int ith_char;//tells you that this is the ith char in the string
	unsigned int char_index; //index of char in the string
	list_t* next;

};

/*
typedef struct BUCKETS bucket_t;
struct BUCKETS {
	char data;
	bucket_t* next;

};
*/
//--------------------------------------------------------------------------//
string_t GetString(FILE *in);
int Compare(const void* a, const void* b);
//--------------------------------------------------------------------------//
int main(int argc, char* argv[]){
	//store string from stdin
	string_t RAW = GetString(stdin);
	//printf("RAW.length = %zu\n", RAW.length);
	//seperate string and key
	int x = 4;
	unsigned int key = 0;
	unsigned int buffer;
	unsigned char c;
	while(x > 0){
		c = RAW.chars[RAW.length - x];
		//printf("c = %d\n", c);
		buffer = (unsigned int) c;
		key <<= 8;
		key = (key | buffer);
		//printf("bleh\n");
		x--;
	}
	//printf("key = %X\n", key);
	//reduce string to exclude key
	string_t string;
	unsigned int size = RAW.length-4;
	string.chars = malloc(sizeof(char) * size);
	memcpy(string.chars, RAW.chars, size);
	string.chars[size] = '\0';
	//sort string and store sorted string
	//duplicate string
	string_t sorted_string;
	sorted_string.chars = malloc(sizeof(char) * size);
	memcpy(sorted_string.chars, string.chars, size);
	sorted_string.chars[size] = '\0';
	//printf("string.chars = %s\n", sorted_string.chars);

	//pointers to function nonsense
	qsort(sorted_string.chars, size, sizeof(char), Compare);
	//printf("string.chars = %s\n", sorted_string.chars);
	//decode
	int j = 0;
	int i = 0;
	int match_count = 0;
	int printed_char = 0;
	//unsigned char current_char = sorted_string.string[key];
	i = key;
	while(printed_char < size){
		//print char
		printf("%c",sorted_string.chars[key]);
		printed_char++;
		//count maches
		while((i >= 0) && (sorted_string.chars[key] == sorted_string.chars[i])){
			match_count++;
			i--;
		}
		j = 0;
		//look up at original string
		while(j < size){
			if(string.chars[j] == sorted_string.chars [key]){
				match_count--;
			}
			if(match_count == 0){
				key = j;
				i = key;
				break;
			}
			j++;
		}
	}
	return 0;
}
//--------------------------------------------------------------------------//
string_t GetString(FILE *in){
	#if DEBUG
	fprintf(stderr, "retrieving string: ");
	#endif

	/* initialize memory space for c */	
	size_t size = sizeof(char);
	unsigned char *c = malloc(size);
	assert(c != NULL);

	/* retrieve char to buffer from in */
	int i = 0;
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

	#if DEBUG
	fprintf(stderr, "%s\nlength = %d\n------\n", c, i);
	#endif

	/* creating string and returns */
	string_t string;
	string.chars = c;
	string.length = i;
	return string;
}

int Compare(const void* a, const void* b){
   return ( *(char*)a - *(char*)b );
}
//--------------------------------------------------------------------------//