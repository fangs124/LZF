/* BWTe.c - Burrows Wheeler Transform Decoder */
/* NAME: Fangs124                             */
/* VERS: 0.0a                                 */
/* DATE: 4 January, 2016                      */

//--------//  preprocessors //----------------------------------------------//

#include <stdio.h>
#include <stdli.h>
#include <string.h>

typedef struct STRINGS string_t;
struct STRINGS {
	char* chars;
	size_t length; //location of terminating null-byte ('\0')
};

int main(int argc, char* argv[]){
	//store string from stdin
	string_t STRING = GetString(stdin);
	//seperate string and key
	//sort string and store sorted string
	//decode
	return 0;
}

string_t GetString(FILE *in){
	#if DEBUG
	fprintf(stderr, "retrieving string: ");
	#endif

	/* initialize memory space for c */	
	size_t size = sizeof(char);
	char *c = malloc(size);
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
