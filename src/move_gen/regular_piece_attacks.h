#ifndef _REGULAR_PIECE_ATTACKS_H_
#define _REGULAR_PIECE_ATTACKS_H_

#include "../types/types.h"

extern Bitboard KING_MOVES[NUM_SQUARES];
extern Bitboard KNIGHT_MOVES[NUM_SQUARES];

void init_knight_moves();
void init_king_moves();

#endif