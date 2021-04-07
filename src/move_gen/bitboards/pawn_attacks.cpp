#include "pawn_attacks.h"

Bitboard PAWN_MOVES[NUM_COLORS][NUM_SQUARES] = {0};
Bitboard PAWN_ATTACKS[NUM_COLORS][NUM_SQUARES] = {0};

void init_pawn_moves() {
    //white
	for (Square s = A1; s <= H8; s++) {
        PAWN_MOVES[WHITE][s] |= shift(BB_SQUARES[s], UP);
        if (rank(s) == RANK_2)
            PAWN_MOVES[WHITE][s] |= shift(BB_SQUARES[s], UP_UP);

        //attacks
        PAWN_ATTACKS[WHITE][s] |= shift(BB_SQUARES[s], UP_RIGHT);
        PAWN_ATTACKS[WHITE][s] |= shift(BB_SQUARES[s], UP_LEFT);
	}
    //flip for black
    for (Square s = A1; s <= H8; s++) {
        PAWN_MOVES[BLACK][s] = flip(PAWN_MOVES[WHITE][56 - 8*rank(s) + file(s)], VERTICALLY);
        PAWN_ATTACKS[BLACK][s] = flip(PAWN_ATTACKS[WHITE][56 - 8*rank(s) + file(s)], VERTICALLY);
    }
}
