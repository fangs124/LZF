typedef unsigned char byte_t;

typedef struct NODES node_t;
struct NODES {
	unsigned int rank; //number of nodes to the left
	byte_t data; //stores 1byte
	node_t* left;
	node_t* right;
};

typedef struct TREES* Tree_t;
struct TREES {
	size_t order;
	node_t* root;
	node_t* max;
	node_t* min;
};

Tree_t new_tree(void);
void insert_data(Tree_t tree, byte_t data);
node_t* find_nthNode(Tree_t tree, unsigned int n);
node_t* find_valNode(Tree_t tree, byte_t data);

