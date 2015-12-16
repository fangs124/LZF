/* LZ78d.c - LZ78 Universal Decoder */
/* warning: botch code */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#define SUMMARY 1
#define DEBUG   0
typedef struct FACTORS factor_t;

struct FATORS{
	unsigned char        data;
	unsigned int parent_index;
	factor_t*            next;
};

void PrintDict(unsigned char* in_dict, unsigned int print_index);

int main(int argc, char* argv[]){
	unsigned int x = 0;
	unsigned int factors_created = 0;
	int i = 0, j = 0; //bytes retrieved
	unsigned char data;
	unsigned char *dict = NULL;
	unsigned char *header = (unsigned char*) malloc (sizeof(char) * 6);
	
	/* retrieving header data */
	while(x <= 6){
		data = fgetc(stdin);
		header[x] = data;
		
		x++;
	}
	/* retrieving dict length */
	unsigned int dict_length = 0;
	while(i < 4){
		dict_length <<= 8;
		dict_length = dict_length | header[i];
		i++;
	}
	unsigned int data_width = (unsigned int) header[i];
	unsigned int index_width = (unsigned int) header[i+1];
	#if DEBUG
	{
		fprintf(stderr, "dict_length = %05d \n", dict_length);
		fprintf(stderr, "data_width  =   %03d \n", data_width);	
		fprintf(stderr, "index_width =   %03d \n", index_width);

	}
	#endif
	free(header);
	header = NULL;
	dict = (unsigned char*) malloc(sizeof(char) * dict_length * 5);
	/* up to here */
	i = 0;
	while(factors_created <= dict_length){
		data = fgetc(stdin);
		if (i == 0){
			dict[i+j] = data;
			dict[i+j+1] = (unsigned char) 0;
		}
		else{
			dict[i+j+1] = data;
			if(i == 3){
				j += 5;
				i = -1;
				factors_created++;
			}
		}
		i++;
	}


	x = 0;
	i = 0;
	while(x <= dict_length){
		PrintDict(dict, x);
		x++;
	}
	#if DEBUG
	{
		fprintf(stderr, "\n");

	}
	#endif
	return 0;
}


void PrintDict(unsigned char* in_dict, unsigned int print_index){
	if (print_index == 0){
		return;
	}
	unsigned int x = (print_index - 1) * 5;
	#if DEBUG
	{
		fprintf(stderr, "print_index = %u\n", print_index);
		fprintf(stderr, "putting char ( %c )\n", in_dict[x]);
	}
	#endif

	unsigned int new_index = 0;
	int i = 1;
	while(i < 4){
		#if DEBUG
		{
			fprintf(stderr, "%02X ", in_dict[x+i+1]);
		}
		#endif
		new_index <<= 8;
		new_index = new_index | in_dict[x+i+1];
		i++;
	}
	#if DEBUG
	{
		fprintf(stderr, "\n");
		fprintf(stderr, "new_index = %u\n", new_index);
	}
	#endif
	PrintDict(in_dict, new_index);
	fputc((char)in_dict[x], stdout);
	return;
	//needs to get print_index from current print_index
}

/*
factor_t *CreateFactor(factor_t* factor_ptr, unsigned char *data_block){
	unsigned char p_data = data_block[0];
	unsigned int p_index = 0;
	unsigned char temp = (unsigned char) 0Xff;
	int i = 1;
	while (i < 4){
		p_index = (data_block[i] & temp);
		p_index <<= 8;
		i++;
	}

	factor_ptr.data = p_data;
	factor_ptr.parent_index = p_index;
	factor_ptr.next = NULL;
	return factor_ptr;
}
*/