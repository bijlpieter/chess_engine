#include "types.h"
#include "move_generation.h"

#include <iostream>

int main() {
	bb_init();
	init_all();
	// for (Square s = A1; s <= H8; s++)
	// 	std::cout << bb_string(ROOK_MASKS[s]) << std::endl;

	// std::cout << bb_string(queen_moves(A1, 0)) << std::endl;

	// for (int i = 0; i < NUM_CASTLING; i++)
	// 	std::cout << bb_string(BB_CASTLING[i]) << std::endl;

	Position p = Position();
	std::cout << p << std::endl;

	for (Color c : {WHITE, BLACK})
		for (PieceType pt : {PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING})
			
			std::cout << bb_string(p.pieces[c][pt]) << std::endl;

	for (Color c : {WHITE, BLACK})
		std::cout << bb_string(p.colors[c]) << std::endl;

	std::cout << bb_string(p.all_pieces) << std::endl;

	return 0;
}