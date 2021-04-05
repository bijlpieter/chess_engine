#include "./types/types.h"
#include "./move_gen/move_generation.h"

#include <iostream>

int main() {
	bb_init();
	init_all();
	// for (Square s = A1; s <= H8; s++)
	// 	std::cout << bb_string(ROOK_MASKS[s]) << std::endl;

	std::cout << bb_string(queen_moves(E5, RANK_2 | RANK_6 | FILE_B | FILE_F)) << std::endl;

	return 0;
}