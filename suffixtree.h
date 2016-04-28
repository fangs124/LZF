/* suffixtree for BWT */

typedef unsigned char byte_t;
typedef struct TREES* Tree_t;

struct TREES {
	byte_t data;
	Tree_t child;
	Tree_t next;
}