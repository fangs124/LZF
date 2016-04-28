#include <stdio.h>

#include "fileio.c"

#define MAX_BUFFER_SIZE 0xFFFF

typedef unsigned char byte_t;

/* loads buffer from input */

void load_buffer(byte_t* buffer, FILE* fp);