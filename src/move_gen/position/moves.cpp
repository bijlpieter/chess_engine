#include "types.h"
#include "move_generation.h"

#include <iostream>

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
Bitboard Position::controlling(Color c, Bitboard occ) const {
	return controlling_regular(c) | controlling_sliding(c, occ);
}

inline Bitboard Position::snipers_to_king(Color c, Bitboard occ) const {
	const Square king = lsb(pieces[~c][KING]);
	return (rook_moves(king, occ) & (pieces[c][ROOK] | pieces[c][QUEEN])) | (bishop_moves(king, occ) & (pieces[c][BISHOP] | pieces[c][QUEEN]));
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

inline void add_moves(Move*& m, Square from, Bitboard to) {
	while (to)
		*m++ = move_init(from, pop_lsb(to));
}

inline void add_pawn_moves(Move*& m, Bitboard moves, Direction D) {
	while (moves) {
		Square to = pop_lsb(moves);
		*m++ = move_init(to - D, to);
	}
}

// Adds each promotion type to the movelist pointed to by m
inline void add_promotions(Move*& m, Bitboard moves, Direction D) {
	while (moves) {
		Square to = pop_lsb(moves);
		Square from = to - D;
		*m++ = move_init(from, to) | (PROMOTION_QUEEN << 12) | S_MOVE_PROMOTION;
		*m++ = move_init(from, to) | (PROMOTION_ROOK << 12) | S_MOVE_PROMOTION;
		*m++ = move_init(from, to) | (PROMOTION_BISHOP << 12) | S_MOVE_PROMOTION;
		*m++ = move_init(from, to) | (PROMOTION_KNIGHT << 12) | S_MOVE_PROMOTION;
	}
}

void Position::generate_pawn_captures(Move*& m, Bitboard target) {
	const Square king = lsb(pieces[turn][KING]);
	const Direction forward_left = turn ? DOWN_LEFT : UP_LEFT;
	const Direction forward_right = turn ? DOWN_RIGHT : UP_RIGHT;

	const Bitboard unpinned_pawns = pieces[turn][PAWN] & ~state->pinned;

	const Bitboard promotions = turn ? BB_RANKS[RANK_1] : BB_RANKS[RANK_8];
	const Bitboard left_diagonal = (pieces[turn][KING] & (promotions | FILE_A)) ? 0ULL : bb_line(king, king + forward_left);
	const Bitboard right_diagonal = (pieces[turn][KING] & (promotions | FILE_H)) ? 0ULL : bb_line(king, king + forward_right);

	const Bitboard left_captures = shift(unpinned_pawns | (pieces[turn][PAWN] & left_diagonal), forward_left) & target;
	const Bitboard right_captures = shift(unpinned_pawns | (pieces[turn][PAWN] & right_diagonal), forward_right) & target;

	add_promotions(m, left_captures & promotions, forward_left);
	add_promotions(m, right_captures & promotions, forward_right);

	add_pawn_moves(m, left_captures & ~promotions, forward_left);
	add_pawn_moves(m, right_captures & ~promotions, forward_right);

	// EN PEASENT CAPTURES
	// en_peasent is the square behind the pawn to be captured.
	const Direction forward = turn ? DOWN : UP;
	const Color enemy = ~turn;
	if (state->en_peasant != NO_SQUARE && (target & (state->en_peasant - forward))) {
		Bitboard moves_bb = unpinned_pawns & PAWN_ATTACKS[enemy][state->en_peasant];
		while (moves_bb) {
			Square from = pop_lsb(moves_bb);
			Bitboard new_occ = (all_pieces ^ from ^ (state->en_peasant - forward)) | state->en_peasant;
			// Make sure our king is not in check after the move is played
			if (!(rook_moves(king, new_occ) & (pieces[enemy][ROOK] | pieces[enemy][QUEEN])) && !(bishop_moves(king, new_occ) & (pieces[enemy][BISHOP] | pieces[enemy][QUEEN])))
				*m++ = move_init(from, state->en_peasant) | S_MOVE_EN_PASSANT;
		}
	}
}

void Position::generate_pawn_pushes(Move*& m, Bitboard target) {
	const Square king = lsb(pieces[turn][KING]);
	const Direction forward = turn ? DOWN : UP;

	const Bitboard unpinned_pawns = pieces[turn][PAWN] & ~state->pinned;

	const Bitboard promotions = turn ? BB_RANKS[RANK_1] : BB_RANKS[RANK_8];
	const Bitboard two_steps = turn ? BB_RANKS[RANK_6] : BB_RANKS[RANK_3];

	const Bitboard single_push = shift(unpinned_pawns | (pieces[turn][PAWN] & file(king)), forward) & ~all_pieces;
	Bitboard double_push = shift(single_push & two_steps, forward) & ~all_pieces & target;

	add_promotions(m, single_push & promotions & target, forward);

	while (double_push) {
		Square to = pop_lsb(double_push);
		*m++ = move_init(to - forward - forward, to);
	}

	add_pawn_moves(m, single_push & ~promotions & target, forward);
}

void Position::generate_pieces(Move*& m, Bitboard target) {
	const Square king = lsb(pieces[turn][KING]);

	Bitboard bb = pieces[turn][KNIGHT] & ~state->pinned;
	while (bb) {
		Square knight = pop_lsb(bb);
		add_moves(m, knight, knight_moves(knight) & target);
	}

	bb = pieces[turn][BISHOP] & ~state->pinned;
	while (bb) {
		Square bishop = pop_lsb(bb);
		add_moves(m, bishop, bishop_moves(bishop, all_pieces) & target);
	}

	bb = pieces[turn][BISHOP] & state->pinned;
	while (bb) {
		Square bishop = pop_lsb(bb);
		add_moves(m, bishop, bishop_moves(bishop, all_pieces) & target & bb_line(king, bishop));
	}

	bb = pieces[turn][ROOK] & ~state->pinned;
	while (bb) {
		Square rook = pop_lsb(bb);
		add_moves(m, rook, rook_moves(rook, all_pieces) & target);
	}

	bb = pieces[turn][ROOK] & state->pinned;
	while (bb) {
		Square rook = pop_lsb(bb);
		add_moves(m, rook, rook_moves(rook, all_pieces) & target & bb_line(king, rook));
	}

	bb = pieces[turn][QUEEN] & ~state->pinned;
	while (bb) {
		Square queen = pop_lsb(bb);
		add_moves(m, queen, queen_moves(queen, all_pieces) & target);
	}

	bb = pieces[turn][QUEEN] & state->pinned;
	while (bb) {
		Square queen = pop_lsb(bb);
		add_moves(m, queen, queen_moves(queen, all_pieces) & target & bb_line(king, queen));
	}
}

void Position::generate_castling(Move*& m) {
	if (turn == WHITE) {
		if ((state->castling & WHITE_KINGSIDE) && !(all_pieces & BB_CASTLING_ROOK[WHITE_KINGSIDE]) && !(state->king_unsafe & BB_CASTLING_KING[WHITE_KINGSIDE]))
			*m++ = move_init(E1, H1) | S_MOVE_CASTLING;
		if ((state->castling & WHITE_QUEENSIDE) && !(all_pieces & BB_CASTLING_ROOK[WHITE_QUEENSIDE]) && !(state->king_unsafe & BB_CASTLING_KING[WHITE_QUEENSIDE]))
			*m++ = move_init(E1, A1) | S_MOVE_CASTLING;
	}
	else {
		if ((state->castling & BLACK_KINGSIDE) && !(all_pieces & BB_CASTLING_ROOK[BLACK_KINGSIDE]) && !(state->king_unsafe & BB_CASTLING_KING[BLACK_KINGSIDE]))
			*m++ = move_init(E8, H8) | S_MOVE_CASTLING;
		if ((state->castling & BLACK_QUEENSIDE) && !(all_pieces & BB_CASTLING_ROOK[BLACK_QUEENSIDE]) && !(state->king_unsafe & BB_CASTLING_KING[BLACK_QUEENSIDE]))
			*m++ = move_init(E8, A8) | S_MOVE_CASTLING;
	}
}

Moves Position::generate_moves(bool quiet) {
	Moves moves;
	const Square king = lsb(pieces[turn][KING]);
	const Color enemy = ~turn;
	if (state->checkers) {
		add_moves(moves.end, king, king_moves(king) & ~state->king_unsafe & ~colors[turn]);

		// If there is more than one checker, return all possible moves of the king.
		if (popcount(state->checkers) > 1)
			return moves;

		// Generate all piece moves that block or capture the checker
		const Bitboard blocks = bb_ray(king, lsb(state->checkers));
		generate_pawn_captures(moves.end, blocks & colors[enemy]);
		generate_pieces(moves.end, blocks);
		generate_pawn_pushes(moves.end, blocks);
	}
	else {
		// All piece captures
		generate_pieces(moves.end, colors[enemy]);
		generate_pawn_captures(moves.end, colors[enemy]);
		// All king captures
		add_moves(moves.end, king, king_moves(king) & ~state->king_unsafe & colors[enemy]);

		if (quiet)
			return moves;
		// Castling
		generate_castling(moves.end);
		// All regular piece moves
		generate_pieces(moves.end, ~all_pieces);
		generate_pawn_pushes(moves.end, ~all_pieces);
		// All regular king moves
		add_moves(moves.end, king, king_moves(king) & ~state->king_unsafe & ~all_pieces);
	}
	return moves;
}

void Position::info_init() {
	const Square king = lsb(pieces[turn][KING]);
	const Color enemy = ~turn;
	state->checkers = attackers_to_sq(king, enemy);
	state->pinned = blockers(king, turn, enemy);
	state->king_unsafe = controlling(enemy, all_pieces ^ king);
}

void Position::key_init() {
	Bitboard all = all_pieces;
	while(all) {
		Square s = pop_lsb(all);
		Piece p = piece_on(s);
		if (p != NO_PIECE)
			state->position_key ^= zobrist.piece_square[p][s];
		if (piece_type(p) == PAWN)
			state->pawn_key ^= zobrist.piece_square[p][s];
		if (state->en_peasant != NO_SQUARE)
			state->position_key ^= zobrist.en_passant[file(state->en_peasant)];
		if (turn == BLACK)
			state->position_key ^= zobrist.side;
		state->position_key ^= zobrist.castling[state->castling];
	}
}
