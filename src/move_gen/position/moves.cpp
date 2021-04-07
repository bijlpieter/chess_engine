#include "types.h"
#include "move_generation.h"

#include <iostream>

// Returns a bitboard containing all the pieces of Color c attacking Square s.
// Can be used to detect if king is in check.
Bitboard Position::attackers(Square s, Color c) const {
	return (king_moves(s) & pieces[c][KING])
		| (rook_moves(s, all_pieces) & (pieces[c][ROOK] | pieces[c][QUEEN]))
		| (bishop_moves(s, all_pieces) & (pieces[c][BISHOP] | pieces[c][QUEEN]))
		| (knight_moves(s) & pieces[c][KNIGHT])
		| (pawn_attacks(s, ~c) & pieces[c][PAWN]);
}

// Returns a bitboard containing all pieces that are blocking a sliding piece attack to Square s.
// Can be used to detect pins and fossilizations.
Bitboard Position::blockers(Square s, Color blocking, Color attacking) const {
	Bitboard blocks = 0;
	Bitboard sliding_attackers = (rook_moves(s, 0) & (pieces[attacking][ROOK] | pieces[attacking][QUEEN])) | (bishop_moves(s, 0) & (pieces[attacking][BISHOP] | pieces[attacking][QUEEN]));

	while (sliding_attackers) {
		Square sniper = pop_lsb(sliding_attackers);
		Bitboard ray_blockers = bb_ray(sniper, s) & ~s & all_pieces;
		if (ray_blockers && popcount(ray_blockers) == 1)
			blocks |= ray_blockers & colors[blocking];
	}

	return blocks;
}

Moves Position::generate_blockers() const {
	Moves moves;
	return moves;
}

Moves Position::generate_moves() const {
	Moves moves;
	Square king = lsb(pieces[turn][KING]);
	Bitboard pinned = blockers(king, turn, ~turn);
	
	// TODO: Do all pawn moves at once + Promotions + EN PEASANT

	Bitboard bb = pieces[turn][KNIGHT] & ~pinned;
	while (bb) {
		Square knight = pop_lsb(bb);
		Bitboard attacks = knight_moves(knight) & ~colors[turn];
		std::cout << bb_string(attacks) << std::endl;
		while (attacks)
			moves.list[moves.size++] = move_init(knight, pop_lsb(attacks));
	}

	bb = pieces[turn][BISHOP] & ~pinned;
	while (bb) {
		Square bishop = pop_lsb(bb);
		Bitboard attacks = bishop_moves(bishop, all_pieces) & ~colors[turn];
		std::cout << bb_string(attacks) << std::endl;
		while (attacks)
			moves.list[moves.size++] = move_init(bishop, pop_lsb(attacks));
	}

	bb = pieces[turn][BISHOP] & pinned;
	while (bb) {
		Square bishop = pop_lsb(bb);
		Bitboard attacks = bishop_moves(bishop, all_pieces) & ~colors[turn] & bb_line(king, bishop);
		std::cout << bb_string(attacks) << std::endl;
		while (attacks)
			moves.list[moves.size++] = move_init(bishop, pop_lsb(attacks));
	}

	bb = pieces[turn][ROOK] & ~pinned;
	while (bb) {
		Square rook = pop_lsb(bb);
		Bitboard attacks = rook_moves(rook, all_pieces) & ~colors[turn];
		std::cout << bb_string(attacks) << std::endl;
		while (attacks)
			moves.list[moves.size++] = move_init(rook, pop_lsb(attacks));
	}

	bb = pieces[turn][ROOK] & pinned;
	while (bb) {
		Square rook = pop_lsb(bb);
		Bitboard attacks = rook_moves(rook, all_pieces) & ~colors[turn] & bb_line(king, rook);
		std::cout << bb_string(attacks) << std::endl;
		while (attacks)
			moves.list[moves.size++] = move_init(rook, pop_lsb(attacks));
	}

	bb = pieces[turn][QUEEN] & ~pinned;
	while (bb) {
		Square queen = pop_lsb(bb);
		Bitboard attacks = queen_moves(queen, all_pieces) & ~colors[turn];
		std::cout << bb_string(attacks) << std::endl;
		while (attacks)
			moves.list[moves.size++] = move_init(queen, pop_lsb(attacks));
	}

	bb = pieces[turn][QUEEN] & pinned;
	while (bb) {
		Square queen = pop_lsb(bb);
		Bitboard attacks = queen_moves(queen, all_pieces) & ~colors[turn] & bb_line(king, queen);
		std::cout << bb_string(attacks) << std::endl;
		while (attacks)
			moves.list[moves.size++] = move_init(queen, pop_lsb(attacks));
	}

	// TODO: King moves such that you aren't in check

	// TODO: Castling

	return moves;
}

Moves Position::legal_moves() const {
	if (checkers)
		return generate_blockers();
	return generate_moves();
}