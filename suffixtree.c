#include <stdbool.h>
#include <list.h>

typedef unsigned char byte_t;

typedef struct tree_t* Tree;
struct tree_t {
	bool is_leaf;
	byte_t data;
	Tree child;
};

Tree_t new_tree(void){
	tree_t* T = (tree_t*) malloc(sizeof(tree_t));
	T->is_leaf = false;
	T->next = NULL;
	T->child = NULL;
	return T;
}

/* DFS, if data == node->data, add leaf_node */
void build_tree(tree_t* T, byte_t data){
	/* if T is a leaf node */
	if(T->child == NULL){
		T->is_leaf == false;
		T->child = malloc(sizeof(tree_t));
		T->child->is_leaf = true;
		T->child->data = data;
		T->child->child = NULL;
		T->child->next = NULL;
	}

	else{
		build_tree(tree_t*)
	}


}