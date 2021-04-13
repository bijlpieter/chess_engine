#ifndef _PAWN_ATTACKS_H_
#define _PAWN_ATTACKS_H_

#include "types.h"

extern Bitboard PAWN_MOVES[NUM_COLORS][NUM_SQUARES];
extern Bitboard PAWN_ATTACKS[NUM_COLORS][NUM_SQUARES];

void init_pawn_moves();

#endif