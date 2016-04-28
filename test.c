#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]){
	int x = 21;
	unsigned long long y = &x;
	printf("x = %d\n", *((int*)y));
	return 0;
}