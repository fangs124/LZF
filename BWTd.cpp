/* BWTe.c - Burrows Wheeler Transform Decoder */
/* AUTHORS: originally Fangs124, refactored and ported to C++ (i.e. made good) by darkf */

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstring>

#include <string>
#include <vector>
#include <iostream>
#include <sstream>

struct Node {
	unsigned char* data;
	size_t index;

	Node(unsigned char *data, size_t index) : data(data), index(index) { }
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

	std::vector<Node> root[256];

	// bucket sort

	for(size_t index = 0; index < string.size(); index++) {
		unsigned char c = string[index];
		root[c].emplace_back((unsigned char*) &string[index], index);
	}

	for(size_t index = 0; index < string.size(); index++) {
		unsigned char c = 0;
		while(key + 1 > root[c].size()) {
			key -= root[c].size();
			c++;
		}

		//fprintf(stderr, "%X\n", key);
		fprintf(stdout, "%c", *root[c][key].data);
		key = root[c][key].index;
		c = 0;
	}

	return 0;
}
