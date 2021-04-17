#include "regular_piece_attacks.h"

Bitboard KING_MOVES[NUM_SQUARES] = {0};
Bitboard KNIGHT_MOVES[NUM_SQUARES] = {0};

void init_knight_moves() {
	for (Square s = A1; s <= H8; s++) {
		KNIGHT_MOVES[s] |= shift<DOWN_DOWN_LEFT>(BB_SQUARES[s]);
		KNIGHT_MOVES[s] |= shift<DOWN_DOWN_RIGHT>(BB_SQUARES[s]);
		KNIGHT_MOVES[s] |= shift<UP_UP_LEFT>(BB_SQUARES[s]);
		KNIGHT_MOVES[s] |= shift<UP_UP_RIGHT>(BB_SQUARES[s]);
		KNIGHT_MOVES[s] |= shift<DOWN_LEFT_LEFT>(BB_SQUARES[s]);
		KNIGHT_MOVES[s] |= shift<DOWN_RIGHT_RIGHT>(BB_SQUARES[s]);
		KNIGHT_MOVES[s] |= shift<UP_LEFT_LEFT>(BB_SQUARES[s]);
		KNIGHT_MOVES[s] |= shift<UP_RIGHT_RIGHT>(BB_SQUARES[s]);
	}
}

void init_king_moves() {
	for (Square s = A1; s <= H8; s++) {
		KING_MOVES[s] |= shift<UP>(BB_SQUARES[s]);
		KING_MOVES[s] |= shift<UP_LEFT>(BB_SQUARES[s]);
		KING_MOVES[s] |= shift<RIGHT>(BB_SQUARES[s]);
		KING_MOVES[s] |= shift<UP_RIGHT>(BB_SQUARES[s]);
		KING_MOVES[s] |= shift<LEFT>(BB_SQUARES[s]);
		KING_MOVES[s] |= shift<DOWN_LEFT>(BB_SQUARES[s]);
		KING_MOVES[s] |= shift<DOWN>(BB_SQUARES[s]);
		KING_MOVES[s] |= shift<DOWN_RIGHT>(BB_SQUARES[s]);
	}
}