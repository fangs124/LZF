/* BWTe.c - Burrows Wheeler Transform Decoder */
/* AUTHORS: originally Fangs124, refactored and ported to C++ (i.e. made good) by darkf */

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <cassert>

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

    std::cerr << "key: " << key << std::endl;
    return s;
}

int main() {
	size_t key;
	std::string string = readFile(std::cin, key);
	std::vector<size_t> buckets[256];

	// bucket sort
	for(size_t index = 0; index < string.size(); index++) {
		buckets[(unsigned char)string[index]].push_back(index);
	}

	for(size_t index = 0; index < string.size(); index++) {
		unsigned char c = 0;
		while(key + 1 > buckets[c].size()) {
			key -= buckets[c].size();
			c++;
		}

		std::cout << string[buckets[c][key]];
		key = buckets[c][key];
		c = 0;
	}

	return 0;
}
