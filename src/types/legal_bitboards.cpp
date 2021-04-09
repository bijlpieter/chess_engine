#include "types.h"
#include "move_generation.h"

Bitboard Position::legal_knight_moves() const {
	Bitboard bb = pieces[turn][KNIGHT] & ~pinned;
	Bitboard moves_bb = 0;
	while (bb)
		moves_bb |= knight_moves(pop_lsb(bb)) & ~colors[turn];
	return moves_bb;
}

Bitboard Position::legal_bishop_moves() const {
	Bitboard bb = pieces[turn][BISHOP] & ~pinned;
	Bitboard moves_bb = 0;
	while (bb)
		moves_bb |= bishop_moves(pop_lsb(bb), all_pieces) & ~colors[turn];

	bb = pieces[turn][BISHOP] & pinned;
	while (bb) {
		Square bishop = pop_lsb(bb);
		moves_bb |= bishop_moves(bishop, all_pieces) & ~colors[turn] & bb_line(king, bishop);
	}
	return moves_bb;
}

Bitboard Position::legal_rook_moves() const {
	Bitboard bb = pieces[turn][ROOK] & ~pinned;
	Bitboard moves_bb = 0;
	while (bb)
		moves_bb |= rook_moves(pop_lsb(bb), all_pieces) & ~colors[turn];

	bb = pieces[turn][ROOK] & pinned;
	while (bb) {
		Square rook = pop_lsb(bb);
		moves_bb |= rook_moves(rook, all_pieces) & ~colors[turn] & bb_line(king, rook);
	}
		
	return moves_bb;
}

Bitboard Position::legal_queen_moves() const {
	Bitboard bb = pieces[turn][QUEEN] & ~pinned;
	Bitboard moves_bb = 0;
	while (bb)
		moves_bb |= queen_moves(pop_lsb(bb), all_pieces) & ~colors[turn];

	bb = pieces[turn][QUEEN] & pinned;
	while (bb) {
		Square queen = pop_lsb(bb);
		moves_bb |= queen_moves(queen, all_pieces) & ~colors[turn] & bb_line(king, queen);
	}
	return moves_bb;
}