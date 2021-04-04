#include "move_generation.h"

Bitboard BISHOP_DATABASE[5248];
Bitboard* BISHOP_MOVES[NUM_SQUARES];
Bitboard BISHOP_MASKS[NUM_SQUARES] = {0};

void init_bishop_masks() {
	for (Square s = A1; s <= H8; s++) {
		for (Direction d : BishopDirections) {
			Bitboard bb = BB_SQUARES[s];
			while ((bb = shift(bb, d)))
				BISHOP_MASKS[s] |= bb;
		}
		BISHOP_MASKS[s] &= ~(BB_RANKS[RANK_1] | BB_RANKS[RANK_8] | BB_FILES[FILE_A] | BB_FILES[FILE_H]);
	}
}