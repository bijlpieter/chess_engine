#ifndef _MOVE_GENERATION_H_
#define _MOVE_GENERATION_H_

#include "../types/types.h"
#include "magic.h"

// Simple pieces
extern Bitboard KING_MOVES[NUM_SQUARES];
extern Bitboard PAWN_MOVES[NUM_COLORS][NUM_SQUARES];
extern Bitboard PAWN_ATTACKS[NUM_COLORS][NUM_SQUARES];
extern Bitboard KNIGHT_MOVES[NUM_SQUARES];

// Sliding pieces
extern Bitboard BISHOP_MOVES[];
extern Bitboard ROOK_MOVES[];

void init_king_moves();
void init_pawn_moves();
void init_knight_moves();

inline void init_all() {
	init_king_moves();
	init_pawn_moves();
	init_knight_moves();
}

#endif