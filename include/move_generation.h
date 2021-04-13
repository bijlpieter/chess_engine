#ifndef _MOVE_GENERATION_H_
#define _MOVE_GENERATION_H_

#include "../src/move_gen/bitboards/pawn_attacks.h"
#include "../src/move_gen/bitboards/regular_piece_attacks.h"
#include "../src/move_gen/bitboards/sliding_piece_attacks.h"

void bb_moves_init();

inline Bitboard pawn_attacks(Square s, Color c) {
	return PAWN_ATTACKS[c][s];
}

inline Bitboard pawn_moves(Square s, Color c) {
	return PAWN_MOVES[c][s];
}

inline Bitboard knight_moves(Square s) {
	return KNIGHT_MOVES[s];
}

inline Bitboard bishop_moves(Square s, Bitboard occ) {
	return BISHOP_MOVES[s][pext(occ, BISHOP_MASKS[s])];
}

inline Bitboard rook_moves(Square s, Bitboard occ) {
	return ROOK_MOVES[s][pext(occ, ROOK_MASKS[s])];
}

inline Bitboard queen_moves(Square s, Bitboard occ) {
	return bishop_moves(s, occ) | rook_moves(s, occ);
}

inline Bitboard king_moves(Square s) {
	return KING_MOVES[s];
}

#endif