#include "types.h"
#include "move_generation.h"

#include <iostream>

int main() {
	bb_init();
	bb_moves_init();
	bb_rays_init();


	// for (Square s = A1; s <= H8; s++)
	// 	std::cout << bb_string(BB_RAYS[s][E4]) << std::endl;

	// std::cout << bb_string(queen_moves(A1, 0)) << std::endl;

	// for (int i = 0; i < NUM_CASTLING; i++)
	// 	std::cout << bb_string(BB_CASTLING[i]) << std::endl;

	Position pos = Position("6k1/5p2/8/8/2B5/8/8/1K6 w KQkq - 0 1");
	std::cout << pos << std::endl;

	// for (Color c : {WHITE, BLACK})
	// 	for (PieceType pt : {PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING})
			
	// 		std::cout << bb_string(p.pieces[c][pt]) << std::endl;

	// for (Color c : {WHITE, BLACK})
	// 	std::cout << bb_string(p.colors[c]) << std::endl;

	std::cout << bb_string(pos.blockers(G8, BLACK, WHITE)) << std::endl;

	return 0;
}