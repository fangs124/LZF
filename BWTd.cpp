/* BWTe.c - Burrows Wheeler Transform Decoder */
/* AUTHORS: Fangs124, darkf                   */

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstring>

#include <string>
#include <iostream>
#include <sstream>

typedef struct NODES node_t;
struct NODES {
	unsigned char* data;
	size_t index;
	//node_t* next;
};

struct Bucket {
	size_t child_count = 0;
	size_t size = 2;
	node_t* childs;

	Bucket() {
		childs = (node_t *)malloc(sizeof(node_t) * size);
	}
};

std::string readFile(std::istream& is, size_t& key) {
	is.seekg(0, is.end);
	size_t filesize = is.tellg();
	is.seekg(0, is.beg);

	std::string s;
	s.resize(filesize - 8);
	is.read(&s[0], filesize - 8);
	assert((bool)is);

    std::string keybuf;
    keybuf.resize(8);
    is.read(&keybuf[0], 8);
	assert((bool)is);

    key = 0;
    for(int i = 0; i < 8; i++) {
    	key <<= 8;
    	key |= (unsigned char)keybuf[i];
    }

    std::cout << "key: " << key << std::endl;

    return s;
}


int main() {
	size_t key;
	std::string string = readFile(std::cin, key);

	Bucket root[256];

	// bucket sort
	unsigned char c;

	for(size_t index = 0; index < string.size(); index++) {
		c = string[index];
		root[c].childs[root[c].child_count].data = (unsigned char*) &string[index];
		root[c].childs[root[c].child_count].index = index;
		//root[c].childs[root[c].child_count].next = NULL;
		root[c].child_count++;
		if(root[c].child_count == root[c].size){
			root[c].size *= 2;
			root[c].childs = (node_t *)realloc(root[c].childs, 
				sizeof(node_t) * root[c].size);
			assert(root[c].childs != NULL);
		}
	}

	size_t index = 0;
	while(index < string.size()) {
		c = 0;
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
