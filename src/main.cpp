#include "./types/types.h"
#include "./move_gen/move_generation.h"

#include <iostream>

int main() {
	bb_init();
	init_all();
	for (Square s = A1; s <= H8; s++)
		std::cout << bb_string(PAWN_ATTACKS[BLACK][s]) << std::endl;
	return 0;
}