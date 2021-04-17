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

template <Color to_move>
inline Bitboard controlling_regular(const Position& pos) {
	Bitboard attacked = 0ULL;

	constexpr Direction forward_left = to_move ? DOWN_LEFT : UP_LEFT;
	constexpr Direction forward_right = to_move ? DOWN_RIGHT : UP_RIGHT;

	attacked |= shift<forward_left>(pos.pieces[to_move][PAWN]);
	attacked |= shift<forward_right>(pos.pieces[to_move][PAWN]);

	Bitboard pcs = pos.pieces[to_move][KNIGHT];
	while (pcs)
		attacked |= knight_moves(pop_lsb(pcs));
	pcs = pos.pieces[to_move][KING];
	while (pcs)
		attacked |= king_moves(pop_lsb(pcs));

	return attacked;
}

template <Color to_move>
inline Bitboard controlling_sliding(const Position& pos, Bitboard occ) {
	Bitboard attacked = 0;
	Bitboard pcs = pos.pieces[to_move][BISHOP];
	while (pcs)
		attacked |= bishop_moves(pop_lsb(pcs), occ);
	pcs = pos.pieces[to_move][ROOK];
	while (pcs)
		attacked |= rook_moves(pop_lsb(pcs), occ);
	pcs = pos.pieces[to_move][QUEEN];
	while (pcs)
		attacked |= queen_moves(pop_lsb(pcs), occ);
	return attacked;
}

inline Bitboard Position::snipers_to_king(Color c, Bitboard occ) const {
	const Square king = lsb(pieces[c][KING]);
	return (rook_moves(king, occ) & (pieces[c][ROOK] | pieces[c][QUEEN])) | (bishop_moves(king, occ) & (pieces[c][BISHOP] | pieces[c][QUEEN]));
}

// Returns a bitboard containing all squares that are controlled by Color c.
// It is literally every square that is attacked by at least one piece of Color c.
// The function does not check for legality.
// Param Bitboard occ: occupancy to use for the sliding pieces; this is useful for checking safe squares around the king.
// Can be used to remove unsafe squares from a bitboard.
template <Color to_move>
Bitboard controlling(const Position& pos, Bitboard occ) {
	return controlling_regular<to_move>(pos) | controlling_sliding<to_move>(pos, occ);
}


// Returns a bitboard containing all pieces that are blocking a sliding piece attack to Square s.
// Can be used to detect pins and fossilizations.
Bitboard Position::blockers(Square s, Color blocking, Color attacking) const {
	Bitboard blocks = 0;
	Bitboard sliding_attackers = snipers_to_king(attacking);

	while (sliding_attackers) {
		Bitboard ray_blockers = bb_ray(pop_lsb(sliding_attackers), s) & ~s & all_pieces;
		if (ray_blockers && popcount(ray_blockers) == 1)
			blocks |= ray_blockers & colors[blocking];
	}

	return blocks;
}

template <Color to_move>
Moves generate_blockers(const Position& pos) {
	Moves moves;

	constexpr Color enemy = ~to_move;
	const Square king = lsb(pos.pieces[to_move][KING]);
	const Bitboard pinned = pos.blockers(king, to_move, enemy);
	const Bitboard king_unsafe = controlling<enemy>(pos, pos.all_pieces ^ king);

	Bitboard kingmoves = king_moves(king) & ~king_unsafe & ~pos.colors[to_move];
	while (kingmoves)
		*moves.end++ = move_init(king, pop_lsb(kingmoves));

	// If there is more than one checker, return all possible moves of the king.
	if (popcount(pos.state->checkers) > 1)
		return moves;

	const Bitboard blocks = bb_ray(king, lsb(pos.state->checkers));

	constexpr Bitboard promotions = to_move ? 0xFFULL : 0xFF00000000000000ULL;
	constexpr Bitboard two_steps = to_move ? 0xFF0000000000ULL : 0xFF0000ULL;
	const Bitboard unpinned_pawns = pos.pieces[to_move][PAWN] & ~pinned;

	constexpr Direction forward = to_move ? DOWN : UP;
	constexpr Direction forward_left = to_move ? DOWN_LEFT : UP_LEFT;
	constexpr Direction forward_right = to_move ? DOWN_RIGHT : UP_RIGHT;

	Bitboard single_push = shift<forward>(unpinned_pawns) & ~pos.all_pieces;
	Bitboard double_push = shift<forward>(single_push & two_steps) & ~pos.all_pieces & blocks;
	Bitboard left_captures = shift<forward_left>(unpinned_pawns) & pos.colors[enemy];
	Bitboard right_captures = shift<forward_right>(unpinned_pawns) & pos.colors[enemy];

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

	if ((pos.state->en_peasant != NO_SQUARE) && (blocks & (pos.state->en_peasant - forward))) {
		moves_bb = unpinned_pawns & PAWN_ATTACKS[enemy][pos.state->en_peasant];
		while (moves_bb) {
			Square from = pop_lsb(moves_bb);
			// Make sure our king is not in check after the move is played
			if (!pos.snipers_to_king(enemy, (pos.all_pieces ^ from ^ (pos.state->en_peasant - forward)) | pos.state->en_peasant))
				*moves.end++ = move_init(from, pos.state->en_peasant) | S_MOVE_EN_PASSANT;
		}
	}

	Bitboard bb = pos.pieces[to_move][KNIGHT] & ~pinned;
	while (bb) {
		Square knight = pop_lsb(bb);
		Bitboard attacks = knight_moves(knight) & ~pos.colors[to_move] & blocks;
		while (attacks)
			*moves.end++ = move_init(knight, pop_lsb(attacks));
	}

	bb = pos.pieces[to_move][BISHOP] & ~pinned;
	while (bb) {
		Square bishop = pop_lsb(bb);
		Bitboard attacks = bishop_moves(bishop, pos.all_pieces) & ~pos.colors[to_move] & blocks;
		while (attacks)
			*moves.end++ = move_init(bishop, pop_lsb(attacks));
	}

	bb = pos.pieces[to_move][ROOK] & ~pinned;
	while (bb) {
		Square rook = pop_lsb(bb);
		Bitboard attacks = rook_moves(rook, pos.all_pieces) & ~pos.colors[to_move] & blocks;
		while (attacks)
			*moves.end++ = move_init(rook, pop_lsb(attacks));
	}

	bb = pos.pieces[to_move][QUEEN] & ~pinned;
	while (bb) {
		Square queen = pop_lsb(bb);
		Bitboard attacks = queen_moves(queen, pos.all_pieces) & ~pos.colors[to_move] & blocks;
		while (attacks)
			*moves.end++ = move_init(queen, pop_lsb(attacks));
	}

	return moves;
}

template <Color to_move>
Moves generate_moves(const Position& pos) {
	Moves moves;
	
	// Regular pawn pushes and captures
	constexpr Color enemy = ~to_move;
	const Square king = lsb(pos.pieces[to_move][KING]);

	const Bitboard pinned = pos.blockers(king, to_move, enemy);
	constexpr Bitboard promotions = to_move ? 0xFFULL : 0xFF00000000000000ULL;
	constexpr Bitboard two_steps = to_move ? 0xFF0000000000ULL : 0xFF0000ULL;

	constexpr Direction forward = to_move ? DOWN : UP;
	constexpr Direction forward_left = to_move ? DOWN_LEFT : UP_LEFT;
	constexpr Direction forward_right = to_move ? DOWN_RIGHT : UP_RIGHT;

	Bitboard unpinned_pawns = pos.pieces[to_move][PAWN] & ~pinned;

	Bitboard left_diagonal = (pos.pieces[to_move][KING] & (promotions | FILE_A)) ? 0ULL : bb_line(king, king + forward_left);
	Bitboard right_diagonal = (pos.pieces[to_move][KING] & (promotions | FILE_H)) ? 0ULL : bb_line(king, king + forward_right);

	Bitboard single_push = shift<forward>(unpinned_pawns | (pos.pieces[to_move][PAWN] & file(king))) & ~pos.all_pieces;
	Bitboard double_push = shift<forward>(single_push & two_steps) & ~pos.all_pieces;
	Bitboard left_captures = shift<forward_left>(unpinned_pawns | (pos.pieces[to_move][PAWN] & left_diagonal)) & pos.colors[enemy];
	Bitboard right_captures = shift<forward_right>(unpinned_pawns | (pos.pieces[to_move][PAWN] & right_diagonal)) & pos.colors[enemy];

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
	if (pos.state->en_peasant != NO_SQUARE) {
		moves_bb = unpinned_pawns & PAWN_ATTACKS[enemy][pos.state->en_peasant];
		while (moves_bb) {
			Square from = pop_lsb(moves_bb);
			Bitboard new_occ = (pos.all_pieces ^ from ^ (pos.state->en_peasant - forward)) | pos.state->en_peasant;
			// Make sure our king is not in check after the move is played
			if (!(rook_moves(king, new_occ) & (pos.pieces[enemy][ROOK] | pos.pieces[enemy][QUEEN])) && !(bishop_moves(king, new_occ) & (pos.pieces[enemy][BISHOP] | pos.pieces[enemy][QUEEN])))
				*moves.end++ = move_init(from, pos.state->en_peasant) | S_MOVE_EN_PASSANT;
		}
	}

	Bitboard bb = pos.pieces[to_move][KNIGHT] & ~pinned;
	while (bb) {
		Square knight = pop_lsb(bb);
		moves_bb = knight_moves(knight) & ~pos.colors[to_move];
		while (moves_bb)
			*moves.end++ = move_init(knight, pop_lsb(moves_bb));
	}

	bb = pos.pieces[to_move][BISHOP] & ~pinned;
	while (bb) {
		Square bishop = pop_lsb(bb);
		moves_bb = bishop_moves(bishop, pos.all_pieces) & ~pos.colors[to_move];
		while (moves_bb)
			*moves.end++ = move_init(bishop, pop_lsb(moves_bb));
	}

	bb = pos.pieces[to_move][BISHOP] & pinned;
	while (bb) {
		Square bishop = pop_lsb(bb);
		moves_bb = bishop_moves(bishop, pos.all_pieces) & ~pos.colors[to_move] & bb_line(king, bishop);
		while (moves_bb)
			*moves.end++ = move_init(bishop, pop_lsb(moves_bb));
	}

	bb = pos.pieces[to_move][ROOK] & ~pinned;
	while (bb) {
		Square rook = pop_lsb(bb);
		moves_bb = rook_moves(rook, pos.all_pieces) & ~pos.colors[to_move];
		while (moves_bb)
			*moves.end++ = move_init(rook, pop_lsb(moves_bb));
	}

	bb = pos.pieces[to_move][ROOK] & pinned;
	while (bb) {
		Square rook = pop_lsb(bb);
		moves_bb = rook_moves(rook, pos.all_pieces) & ~pos.colors[to_move] & bb_line(king, rook);
		while (moves_bb)
			*moves.end++ = move_init(rook, pop_lsb(moves_bb));
	}

	bb = pos.pieces[to_move][QUEEN] & ~pinned;
	while (bb) {
		Square queen = pop_lsb(bb);
		moves_bb = queen_moves(queen, pos.all_pieces) & ~pos.colors[to_move];
		while (moves_bb)
			*moves.end++ = move_init(queen, pop_lsb(moves_bb));
	}

	bb = pos.pieces[to_move][QUEEN] & pinned;
	while (bb) {
		Square queen = pop_lsb(bb);
		moves_bb = queen_moves(queen, pos.all_pieces) & ~pos.colors[to_move] & bb_line(king, queen);
		while (moves_bb)
			*moves.end++ = move_init(queen, pop_lsb(moves_bb));
	}

	moves_bb = king_moves(king) & ~pos.colors[to_move];

	if (!moves_bb)
		return moves;

	const Bitboard king_unsafe = controlling<enemy>(pos, pos.all_pieces ^ king);
	moves_bb &= ~king_unsafe;
	while (moves_bb)
		*moves.end++ = move_init(king, pop_lsb(moves_bb));

	// Castling
	if (to_move == WHITE) {
		if ((pos.state->castling & WHITE_KINGSIDE) && !(pos.all_pieces & BB_CASTLING_ROOK[WHITE_KINGSIDE]) && !(king_unsafe & BB_CASTLING_KING[WHITE_KINGSIDE]))
			*moves.end++ = move_init(E1, H1) | S_MOVE_CASTLING;
		if ((pos.state->castling & WHITE_QUEENSIDE) && !(pos.all_pieces & BB_CASTLING_ROOK[WHITE_QUEENSIDE]) && !(king_unsafe & BB_CASTLING_KING[WHITE_QUEENSIDE]))
			*moves.end++ = move_init(E1, A1) | S_MOVE_CASTLING;
	}
	else {
		if ((pos.state->castling & BLACK_KINGSIDE) && !(pos.all_pieces & BB_CASTLING_ROOK[BLACK_KINGSIDE]) && !(king_unsafe & BB_CASTLING_KING[BLACK_KINGSIDE]))
			*moves.end++ = move_init(E8, H8) | S_MOVE_CASTLING;
		if ((pos.state->castling & BLACK_QUEENSIDE) && !(pos.all_pieces & BB_CASTLING_ROOK[BLACK_QUEENSIDE]) && !(king_unsafe & BB_CASTLING_KING[BLACK_QUEENSIDE]))
			*moves.end++ = move_init(E8, A8) | S_MOVE_CASTLING;
	}

	return moves;
}

template <Color to_move>
Moves legal_moves(const Position& pos) {
	return pos.state->checkers ? generate_blockers<to_move>(pos) : generate_moves<to_move>(pos);
}

template Moves legal_moves<WHITE>(const Position& pos);
template Moves legal_moves<BLACK>(const Position& pos);