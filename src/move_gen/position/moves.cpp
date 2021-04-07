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
	// std::cout << bb_string(sliding_attackers) << std::endl;

	while (sliding_attackers) {
		Square sniper = pop_lsb(sliding_attackers);
		Bitboard ray_blockers = bb_ray(sniper, s) & ~s & all_pieces;
		// std::cout << bb_string(ray_blockers) << std::endl;
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
	return moves;
}

Moves Position::legal_moves() const {
	if (checkers)
		return generate_blockers();
	return generate_moves();
}