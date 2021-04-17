#include "pawn_attacks.h"

Bitboard PAWN_ATTACKS[NUM_COLORS][NUM_SQUARES] = {0};

void init_pawn_moves() {
    //white
	for (Square s = A1; s <= H8; s++) {
        PAWN_ATTACKS[WHITE][s] |= shift<UP_RIGHT>(BB_SQUARES[s]);
        PAWN_ATTACKS[WHITE][s] |= shift<UP_LEFT>(BB_SQUARES[s]);
	}
    //flip for black
    for (Square s = A1; s <= H8; s++)
        PAWN_ATTACKS[BLACK][s] = flip(PAWN_ATTACKS[WHITE][56 - 8*rank(s) + file(s)], VERTICALLY);
}
