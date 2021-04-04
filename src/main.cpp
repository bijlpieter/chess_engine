#include "./types/types.h"
#include "./move_gen/move_generation.h"

#include <iostream>

int main() {
	bb_init();
	init_all();
	int sum = 0;
	for (Square s = A1; s <= H8; s++)
		sum += (1 << popcount(BISHOP_MASKS[s]));
	std::cout << sum << std::endl;
	return 0;
}