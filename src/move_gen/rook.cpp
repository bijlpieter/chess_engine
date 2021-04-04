#include "move_generation.h"

Bitboard ROOK_MOVES[1];
Bitboard ROOK_MASKS[NUM_SQUARES] = {0};

void init_rook_masks() {
	for (Square s = A1; s <= H8; s++) {
		for (Direction d : RookDirections) {
			Bitboard bb = BB_SQUARES[s];
			while ((bb = shift(bb, d)))
				ROOK_MASKS[s] |= bb;
		}
		ROOK_MASKS[s] &= ~(((BB_RANKS[RANK_1] | BB_RANKS[RANK_8]) & ~BB_RANKS[rank(s)]) | ((BB_FILES[FILE_A] | BB_FILES[FILE_H]) & ~BB_FILES[file(s)]));
	}
}