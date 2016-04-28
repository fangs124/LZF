/* Huffman Coding - Encoder - Fangs124 */

#include <stdio.h>
#include <stdlib.h>

#include "bins.h"

#define DEBUG 1 //?

int main(int argc, char* argv[]){
	/* file stuff */
	FILE* fp = stdin;
	Bin_t bin = new_bin();

	/* Step 1: count probability */
	count_bin(&bin, fp);

	/* Step 2: sort probability */
	sort_bin(&bin);
	/* Step 3: generate priority queue */


	return 0;
}

