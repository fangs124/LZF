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

typedef struct BUCKETS bucket_t;
struct BUCKETS {
	size_t child_count;
	size_t size;
	node_t* childs;
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
	// fprintf(stderr, "%zu\n", string.length);
	// fprintf(stderr, "%zX\n", string.key);

	/* initialize buckets */
	bucket_t root[256];
	size_t index = 0;

	while(index <= 255) {
		root[index].child_count = 0;
		root[index].size = 2;
		root[index].childs = (node_t *)malloc(sizeof(node_t) * root[index].size);
		//fprintf(stderr, "%X", string.chars[index]);
		index++;
	}

	/* bucket sort */
	index = 0;
	unsigned char c;

	while(index < string.size()) {
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
		index++;
	}

	index = 0;
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
