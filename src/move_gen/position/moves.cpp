#include "types.h"
#include "move_generation.h"

#include <iostream>


// Adds each promotion type to the movelist pointed to by m
inline void add_promotions(Move*& m, Square from, Square to) {
	*m++ = move_init(from, to) | (PROMOTION_QUEEN << 12) | S_MOVE_PROMOTION;
	*m++ = move_init(from, to) | (PROMOTION_ROOK << 12) | S_MOVE_PROMOTION;
	*m++ = move_init(from, to) | (PROMOTION_BISHOP << 12) | S_MOVE_PROMOTION;
	*m++ = move_init(from, to) | (PROMOTION_KNIGHT << 12) | S_MOVE_PROMOTION;
}

// Returns a bitboard containing all the pieces of Color c attacking Square s.
// Can be used to detect if king is in check.
Bitboard Position::attackers_to_sq(Square s, Color c) const {
	return (king_moves(s) & pieces[c][KING])
		| (rook_moves(s, all_pieces) & (pieces[c][ROOK] | pieces[c][QUEEN]))
		| (bishop_moves(s, all_pieces) & (pieces[c][BISHOP] | pieces[c][QUEEN]))
		| (knight_moves(s) & pieces[c][KNIGHT])
		| (pawn_attacks(s, ~c) & pieces[c][PAWN]);
}

inline Bitboard Position::controlling_regular(Color c) const {
	Bitboard attacked = 0;
	attacked |= shift(pieces[c][PAWN], c ? DOWN_LEFT : UP_LEFT);
	attacked |= shift(pieces[c][PAWN], c ? DOWN_RIGHT : UP_RIGHT);

	Bitboard pcs = pieces[c][KNIGHT];
	while (pcs)
		attacked |= knight_moves(pop_lsb(pcs));
	pcs = pieces[c][KING];
	while (pcs)
		attacked |= king_moves(pop_lsb(pcs));

	return attacked;
}

inline Bitboard Position::controlling_sliding(Color c, Bitboard occ) const {
	Bitboard attacked = 0;
	Bitboard pcs = pieces[c][BISHOP];
	while (pcs)
		attacked |= bishop_moves(pop_lsb(pcs), occ);
	pcs = pieces[c][ROOK];
	while (pcs)
		attacked |= rook_moves(pop_lsb(pcs), occ);
	pcs = pieces[c][QUEEN];
	while (pcs)
		attacked |= queen_moves(pop_lsb(pcs), occ);
	return attacked;
}

// Returns a bitboard containing all squares that are controlled by Color c.
// It is literally every square that is attacked by at least one piece of Color c.
// The function does not check for legality.
// Param Bitboard occ: occupancy to use for the sliding pieces; this is useful for checking safe squares around the king.
// Can be used to remove unsafe squares from a bitboard.
inline Bitboard Position::controlling(Color c, Bitboard occ) const {
	return controlling_regular(c) | controlling_sliding(c, occ);
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

Moves Position::generate_blockers() {
	Bitboard kingmoves = king_moves(king) & ~king_unsafe & ~colors[turn];
	while (kingmoves)
		*moves.end++ = move_init(king, pop_lsb(kingmoves));

	// If there is more than one checker, return all possible moves of the king.
	if (popcount(checkers) > 1)
		return moves;

	Square checking_piece = lsb(checkers);
	Bitboard blocks = bb_ray(king, checking_piece);

	Bitboard promotions = turn ? BB_RANKS[RANK_1] : BB_RANKS[RANK_8];
	Bitboard two_steps = turn ? BB_RANKS[RANK_6] : BB_RANKS[RANK_3];
	Bitboard unpinned_pawns = pieces[turn][PAWN] & ~pinned;

	Direction forward = turn ? DOWN : UP;
	Direction forward_left = turn ? DOWN_LEFT : UP_LEFT;
	Direction forward_right = turn ? DOWN_RIGHT : UP_RIGHT;
	Bitboard single_push = shift(unpinned_pawns | (pieces[turn][PAWN] & file(king)), forward) & ~all_pieces;
	Bitboard double_push = shift(single_push & two_steps, forward) & ~all_pieces & blocks;
	Bitboard left_captures = shift(unpinned_pawns, forward_left) & colors[enemy];
	Bitboard right_captures = shift(unpinned_pawns, forward_right) & colors[enemy];

	Bitboard moves_bb = single_push & ~promotions & blocks;
	while (moves_bb) {
		Square to = pop_lsb(moves_bb);
		*moves.end++ = move_init(to - forward, to);
	}

	while (double_push) {
		Square to = pop_lsb(double_push);
		*moves.end++ = move_init(to - forward - forward, to);
	}

	moves_bb = left_captures & ~promotions & blocks;
	while (moves_bb) {
		Square to = pop_lsb(moves_bb);
		*moves.end++ = move_init(to - forward_left, to);
	}

	moves_bb = right_captures & ~promotions & blocks;
	while (moves_bb) {
		Square to = pop_lsb(moves_bb);
		*moves.end++ = move_init(to - forward_right, to);
	}

	// Promotions
	moves_bb = single_push & promotions & blocks;
	while (moves_bb) {
		Square to = pop_lsb(moves_bb);
		add_promotions(moves.end, to - forward, to);
	}

	moves_bb = left_captures & promotions & blocks;
	while (moves_bb) {
		Square to = pop_lsb(moves_bb);
		add_promotions(moves.end, to - forward_left, to);
	}

	moves_bb = right_captures & promotions & blocks;
	while (moves_bb) {
		Square to = pop_lsb(moves_bb);
		add_promotions(moves.end, to - forward_right, to);
	}

	if (en_peasant != NO_SQUARE) {	
		moves_bb = unpinned_pawns & PAWN_ATTACKS[enemy][en_peasant] & blocks;
		while (moves_bb) {
			Square from = pop_lsb(moves_bb);
			Bitboard new_occ = (all_pieces ^ from ^ (en_peasant - forward)) | en_peasant;
			// Make sure our king is not in check after the move is played
			if (!(rook_moves(king, new_occ) & (pieces[enemy][ROOK] | pieces[enemy][QUEEN])) && !(bishop_moves(king, new_occ) & (pieces[enemy][BISHOP] | pieces[enemy][QUEEN])))
				*moves.end++ = move_init(pop_lsb(moves_bb), en_peasant) | S_MOVE_EN_PASSANT;
		}
	}

	Bitboard bb = pieces[turn][KNIGHT] & ~pinned;
	while (bb) {
		Square knight = pop_lsb(bb);
		Bitboard attacks = knight_moves(knight) & ~colors[turn] & blocks;
		std::cout << bb_string(attacks) << std::endl;
		while (attacks)
			*moves.end++ = move_init(knight, pop_lsb(attacks));
	}

	bb = pieces[turn][BISHOP] & ~pinned;
	while (bb) {
		Square bishop = pop_lsb(bb);
		Bitboard attacks = bishop_moves(bishop, all_pieces) & ~colors[turn] & blocks;
		std::cout << bb_string(attacks) << std::endl;
		while (attacks)
			*moves.end++ = move_init(bishop, pop_lsb(attacks));
	}

	bb = pieces[turn][ROOK] & ~pinned;
	while (bb) {
		Square rook = pop_lsb(bb);
		Bitboard attacks = rook_moves(rook, all_pieces) & ~colors[turn] & blocks;
		std::cout << bb_string(attacks) << std::endl;
		while (attacks)
			*moves.end++ = move_init(rook, pop_lsb(attacks));
	}

	bb = pieces[turn][QUEEN] & ~pinned;
	while (bb) {
		Square queen = pop_lsb(bb);
		Bitboard attacks = queen_moves(queen, all_pieces) & ~colors[turn] & blocks;
		std::cout << bb_string(attacks) << std::endl;
		while (attacks)
			*moves.end++ = move_init(queen, pop_lsb(attacks));
	}

	return moves;
}

Moves Position::generate_moves() {
	Moves moves;
	
	// Regular pawn pushes and captures
	Bitboard promotions = turn ? BB_RANKS[RANK_1] : BB_RANKS[RANK_8];
	Bitboard two_steps = turn ? BB_RANKS[RANK_6] : BB_RANKS[RANK_3];
	Bitboard unpinned_pawns = pieces[turn][PAWN] & ~pinned;

	Direction forward = turn ? DOWN : UP;
	Direction forward_left = turn ? DOWN_LEFT : UP_LEFT;
	Direction forward_right = turn ? DOWN_RIGHT : UP_RIGHT;
	Bitboard single_push = shift(unpinned_pawns | (pieces[turn][PAWN] & file(king)), forward) & ~all_pieces;
	Bitboard double_push = shift(single_push & two_steps, forward) & ~all_pieces;
	Bitboard left_captures = shift(unpinned_pawns, forward_left) & colors[enemy];
	Bitboard right_captures = shift(unpinned_pawns, forward_right) & colors[enemy];

	Bitboard moves_bb = single_push & ~promotions;
	while (moves_bb) {
		Square to = pop_lsb(moves_bb);
		*moves.end++ = move_init(to - forward, to);
	}

	while (double_push) {
		Square to = pop_lsb(double_push);
		*moves.end++ = move_init(to - forward - forward, to);
	}

	moves_bb = left_captures & ~promotions;
	while (moves_bb) {
		Square to = pop_lsb(moves_bb);
		*moves.end++ = move_init(to - forward_left, to);
	}

	moves_bb = right_captures & ~promotions;
	while (moves_bb) {
		Square to = pop_lsb(moves_bb);
		*moves.end++ = move_init(to - forward_right, to);
	}

	// Promotions
	moves_bb = single_push & promotions;
	while (moves_bb) {
		Square to = pop_lsb(moves_bb);
		add_promotions(moves.end, to - forward, to);
	}

	moves_bb = left_captures & promotions;
	while (moves_bb) {
		Square to = pop_lsb(moves_bb);
		add_promotions(moves.end, to - forward_left, to);
	}

	moves_bb = right_captures & promotions;
	while (moves_bb) {
		Square to = pop_lsb(moves_bb);
		add_promotions(moves.end, to - forward_right, to);
	}

	// EN PEASENT CAPTURES
	// en_peasent is the square behind the pawn to be captured.
	if (en_peasant != NO_SQUARE) {	
		moves_bb = unpinned_pawns & PAWN_ATTACKS[enemy][en_peasant];
		while (moves_bb) {
			Square from = pop_lsb(moves_bb);
			Bitboard new_occ = (all_pieces ^ from ^ (en_peasant - forward)) | en_peasant;
			// Make sure our king is not in check after the move is played
			if (!(rook_moves(king, new_occ) & (pieces[enemy][ROOK] | pieces[enemy][QUEEN])) && !(bishop_moves(king, new_occ) & (pieces[enemy][BISHOP] | pieces[enemy][QUEEN])))
				*moves.end++ = move_init(pop_lsb(moves_bb), en_peasant) | S_MOVE_EN_PASSANT;
		}
	}

	Bitboard bb = pieces[turn][KNIGHT] & ~pinned;
	while (bb) {
		Square knight = pop_lsb(bb);
		moves_bb = knight_moves(knight) & ~colors[turn];
		while (moves_bb)
			*moves.end++ = move_init(knight, pop_lsb(moves_bb));
	}

	bb = pieces[turn][BISHOP] & ~pinned;
	while (bb) {
		Square bishop = pop_lsb(bb);
		moves_bb = bishop_moves(bishop, all_pieces) & ~colors[turn];
		while (moves_bb)
			*moves.end++ = move_init(bishop, pop_lsb(moves_bb));
	}

	bb = pieces[turn][BISHOP] & pinned;
	while (bb) {
		Square bishop = pop_lsb(bb);
		moves_bb = bishop_moves(bishop, all_pieces) & ~colors[turn] & bb_line(king, bishop);
		while (moves_bb)
			*moves.end++ = move_init(bishop, pop_lsb(moves_bb));
	}

	bb = pieces[turn][ROOK] & ~pinned;
	while (bb) {
		Square rook = pop_lsb(bb);
		moves_bb = rook_moves(rook, all_pieces) & ~colors[turn];
		while (moves_bb)
			*moves.end++ = move_init(rook, pop_lsb(moves_bb));
	}

	bb = pieces[turn][ROOK] & pinned;
	while (bb) {
		Square rook = pop_lsb(bb);
		moves_bb = rook_moves(rook, all_pieces) & ~colors[turn] & bb_line(king, rook);
		while (moves_bb)
			*moves.end++ = move_init(rook, pop_lsb(moves_bb));
	}

	bb = pieces[turn][QUEEN] & ~pinned;
	while (bb) {
		Square queen = pop_lsb(bb);
		moves_bb = queen_moves(queen, all_pieces) & ~colors[turn];
		while (moves_bb)
			*moves.end++ = move_init(queen, pop_lsb(moves_bb));
	}

	bb = pieces[turn][QUEEN] & pinned;
	while (bb) {
		Square queen = pop_lsb(bb);
		moves_bb = queen_moves(queen, all_pieces) & ~colors[turn] & bb_line(king, queen);
		while (moves_bb)
			*moves.end++ = move_init(queen, pop_lsb(moves_bb));
	}

	moves_bb = king_moves(king) & ~king_unsafe & ~colors[turn];
	while (moves_bb)
		*moves.end++ = move_init(king, pop_lsb(moves_bb));

	// Castling
	if (turn == WHITE) {
		if ((castling & WHITE_KINGSIDE) && !(all_pieces & BB_CASTLING_ROOK[WHITE_KINGSIDE]) && !(king_unsafe & BB_CASTLING_KING[WHITE_KINGSIDE]))
			*moves.end++ = move_init(E1, G1) | S_MOVE_CASTLING;
		if ((castling & WHITE_QUEENSIDE) && !(all_pieces & BB_CASTLING_ROOK[WHITE_QUEENSIDE]) && !(king_unsafe & BB_CASTLING_KING[WHITE_QUEENSIDE]))
			*moves.end++ = move_init(E1, C1) | S_MOVE_CASTLING;
	}
	else {
		if ((castling & BLACK_KINGSIDE) && !(all_pieces & BB_CASTLING_ROOK[BLACK_KINGSIDE]) && !(king_unsafe & BB_CASTLING_KING[BLACK_KINGSIDE]))
			*moves.end++ = move_init(E8, G8) | S_MOVE_CASTLING;
		if ((castling & BLACK_QUEENSIDE) && !(all_pieces & BB_CASTLING_ROOK[BLACK_QUEENSIDE]) && !(king_unsafe & BB_CASTLING_KING[BLACK_QUEENSIDE]))
			*moves.end++ = move_init(E8, C8) | S_MOVE_CASTLING;
	}

	return moves;
}

void Position::info_init() {
	king = lsb(pieces[turn][KING]);
	enemy = ~turn;
	checkers = attackers_to_sq(king, enemy);
	pinned = blockers(king, turn, enemy);
	king_unsafe = controlling(turn, all_pieces ^ king);
}

Moves Position::legal_moves() {
	return checkers ? generate_blockers() : generate_moves();
}