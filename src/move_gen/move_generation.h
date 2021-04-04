#ifndef _MOVE_GENERATION_H_
#define _MOVE_GENERATION_H_

#include "../types/types.h"
#include "magic.h"

// Simple pieces
extern Bitboard KING_MOVES[NUM_SQUARES];
extern Bitboard PAWN_MOVES[NUM_COLORS][NUM_SQUARES];
extern Bitboard PAWN_ATTACKS[NUM_COLORS][NUM_SQUARES];
extern Bitboard KNIGHT_MOVES[NUM_SQUARES];
void init_king_moves();
void init_pawn_moves();
void init_knight_moves();

// Sliding pieces
extern Bitboard ROOK_MOVES[1];
extern Bitboard ROOK_MASKS[NUM_SQUARES];
extern Bitboard BISHOP_MOVES[1];
extern Bitboard BISHOP_MASKS[NUM_SQUARES];
void init_rook_masks();
void init_bishop_masks();

void init_all();

#endif