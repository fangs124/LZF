/* This module contains bin related functions */
typedef unsigned char byte_t;

typedef struct BINS* Bin_t;
struct BINS {
	unsigned int* count; // set to 0x100
};

/* allocate new bin */
Bin_t new_bin(void);

/* free bin */
void free_bin(Bin_t bin);

/* count probability using bin */
void count_bin(Bin_t* bin, FILE* fp);

/* sort bin based on probability */
void sort_bin(Bin_t* bin);