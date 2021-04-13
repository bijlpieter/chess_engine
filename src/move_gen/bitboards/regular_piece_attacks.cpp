#include "regular_piece_attacks.h"

Bitboard KING_MOVES[NUM_SQUARES] = {0};
Bitboard KNIGHT_MOVES[NUM_SQUARES] = {0};

void init_knight_moves() {
	for (Square s = A1; s <= H8; s++)
		for (Direction d : KnightDirections)
			KNIGHT_MOVES[s] |= shift(BB_SQUARES[s], d);
}

void init_king_moves() {
	for (Square s = A1; s <= H8; s++)
		for (Direction d : KingDirections)
			KING_MOVES[s] |= shift(BB_SQUARES[s], d);
}