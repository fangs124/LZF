/* BWTe.c - Burrows Wheeler Transform Decoder */
/* AUTHORS: originally Fangs124, refactored and ported to C++ (i.e. made good) by darkf */

#include <algorithm>
#include <string>
#include <vector>
#include <fstream>
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

std::string bwtd_bucket_sort(const std::string& string, size_t key) {
	std::vector<size_t> buckets[256];
	std::string out;
	out.resize(string.size());

	for(size_t index = 0; index < string.size(); index++) {
		buckets[(unsigned char)string[index]].push_back(index);
	}

	for(size_t index = 0; index < string.size(); index++) {
		unsigned char c = 0;
		while(key + 1 > buckets[c].size()) {
			key -= buckets[c].size();
			c++;
		}

		out[index] = string[buckets[c][key]];
		key = buckets[c][key];
		c = 0;
	}

	return out;
}

std::string bwtd_sort(const std::string& string, size_t key) {
	std::vector<size_t> indices;
	indices.resize(string.size());
	std::iota(indices.begin(), indices.end(), 0);

	std::string out;
	out.resize(string.size());

	std::stable_sort(indices.begin(), indices.end(), [&](size_t i, size_t j) { return string[i] < string[j]; });

	for(size_t index = 0; index < string.size(); index++) {
		out[index] = string[indices[key]];
		key = indices[key];
	}

	return out;
}

int main(int argc, char **argv) {
	if(argc != 3) {
		std::cout << "USAGE: " << argv[0] << " INPUT_FILE OUTPUT_FILE" << std::endl;
		return 1;
	}

	std::ifstream file(argv[1], std::ios::in | std::ios::binary);
	size_t key;
	auto string = readFile(file, key);
	file.close();

	std::string out = bwtd_sort(string, key);
	// std::string out = bwtd_bucket_sort(string, key);

	std::ofstream outfile(argv[2], std::ios::out | std::ios::binary);
	outfile << out;
	outfile.close();

	return 0;
}
