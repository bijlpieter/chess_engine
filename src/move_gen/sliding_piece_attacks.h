#ifndef _SLIDING_PIECE_ATTACKS_H_
#define _SLIDING_PIECE_ATTACKS_H_

#include "../types/types.h"

extern Bitboard* ROOK_MOVES[NUM_SQUARES];
extern Bitboard ROOK_MASKS[NUM_SQUARES];
extern Bitboard* BISHOP_MOVES[NUM_SQUARES];
extern Bitboard BISHOP_MASKS[NUM_SQUARES];

Bitboard ray_attacks(Square s, Direction d, Bitboard occ = 0);

void init_rook_masks();
void init_bishop_masks();
void init_rook_moves();
void init_bishop_moves();

#endif