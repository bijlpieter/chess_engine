#include "sliding_piece_attacks.h"

Bitboard* ROOK_MOVES[NUM_SQUARES];
Bitboard ROOK_MASKS[NUM_SQUARES];
Bitboard* BISHOP_MOVES[NUM_SQUARES];
Bitboard BISHOP_MASKS[NUM_SQUARES];

Bitboard ROOK_DATABASE[102400];
Bitboard BISHOP_DATABASE[5248];

inline Bitboard ray_attacks(Square s, Direction d, Bitboard occ) {
	Bitboard ray = 0;
	Bitboard bb = BB_SQUARES[s];
	while (!(bb & occ) && (bb = shift(bb, d)))
		ray |= bb;
	return ray;
}

void init_rook_masks() {
	for (Square s = A1; s <= H8; s++) {
		for (Direction d : RookDirections)
			ROOK_MASKS[s] |= ray_attacks(s, d);
		ROOK_MASKS[s] &= ~(((RANK_1 | RANK_8) & ~rank(s)) | ((FILE_A | FILE_H) & ~file(s)));
	}
}

void init_bishop_masks() {
	for (Square s = A1; s <= H8; s++) {
		for (Direction d : BishopDirections)
			BISHOP_MASKS[s] |= ray_attacks(s, d);
		BISHOP_MASKS[s] &= ~(RANK_1 | RANK_8 | FILE_A | FILE_H);
	}
}

void init_rook_moves() {
	int index = 0;
	for (Square s = A1; s <= H8; s++) {
		ROOK_MOVES[s] = ROOK_DATABASE + index;
		Bitboard subset = 0;
		Bitboard mask = ROOK_MASKS[s];
		
		do {
			Bitboard attacks = 0;
			for (Direction d : RookDirections)
				attacks |= ray_attacks(s, d, subset);
			ROOK_MOVES[s][pext(subset, mask)] = attacks;
			subset = (subset - mask) & mask;
		} while (subset);

		index += (1 << popcount(mask));
	}
}

void init_bishop_moves() {
	int index = 0;
	for (Square s = A1; s <= H8; s++) {
		BISHOP_MOVES[s] = BISHOP_DATABASE + index;
		Bitboard subset = 0;
		Bitboard mask = BISHOP_MASKS[s];
		
		do {
			Bitboard attacks = 0;
			for (Direction d : BishopDirections)
				attacks |= ray_attacks(s, d, subset);
			BISHOP_MOVES[s][pext(subset, mask)] = attacks;
			subset = (subset - mask) & mask;
		} while (subset);

		index += (1 << popcount(mask));
	}
}