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

bool Position::insufficient_material() const {
	int num = popcount(all_pieces);
	return (num == 2)
		|| (num == 3 && (pieces[WHITE][KNIGHT] || pieces[WHITE][BISHOP] || pieces[BLACK][KNIGHT] || pieces[BLACK][BISHOP]))
		|| (num == 4 && (popcount(pieces[WHITE][KNIGHT] == 2) || popcount(pieces[BLACK][KNIGHT] == 2)));
}

bool Position::is_repetition() const {
	int count = 0;
	for (PositionInfo* s = state; s->previous && s->previous->previous; s = s->previous->previous) {
		if (s->rule50 < 4)
			return false;
		if (s->position_key == state->position_key)
			count++;
		if (count == 3)
			return true;
	}
	return false;
}

bool Position::is_capture(Move m) const {
	return (piece_on(move_to(m)) != NO_PIECE && move_type(m) != S_MOVE_CASTLING) || move_type(m) == S_MOVE_EN_PASSANT;
}

bool Position::is_draw() const {
	return state->rule50 >= 100 || insufficient_material() || is_repetition();
}

bool Position::is_pseudo_legal(Move m) {
	// if (!move)
	// 	return false;

	// Square from = move_from(move), to = move_to(move);
	// MoveType t = move_type(move);
	// PieceType pt = piece_type(piece_on(from))

	// Bitboard own = colors[turn], enemy = colors[~turn], occ = all_pieces;

	// if (piece_on(from) == NO_PIECE) /// there isn't a piece
	// 	return false;

	// if (turn != color(piece_on(from))) /// different color
	// 	return false;

	
	// if(own & to) /// can't move piece on the same square as one of our pieces
	// 	return false;

	// /// check for normal moves

	// if (pt == KNIGHT)
	// 	return t == S_MOVE_NORMAL && (knight_moves(from) & to);

	// if (pt == BISHOP)
	// 	return t == S_MOVE_NORMAL && (bishop_moves(from, occ) & to);

	// if (pt == ROOK)
	// 	return t == S_MOVE_NORMAL && (rook_moves(from, occ) & to);

	// if (pt == QUEEN)
	// 	return t == S_MOVE_NORMAL && (queen_moves(from, occ) & to);

	// /// pawn moves

	// if (pt == PAWN) {

	// 	Bitboard att = pawn_attack(from, turn);

	// 	/// enpassant

	// 	if (t == ENPASSANT)
	// 		return to == state->en_peasant && (att & to);

	// 	Direction forward = color ? DOWN : UP;
	// 	Bitboard push = shift(BB_SQUARES[from], forward) & ~occ;

	// 	/// promotion

	// 	if (t == PROMOTION)
	// 		return (to / 8 == 0 || to / 8 == 7) && (((att & enemy) | push) & (1ULL << to));

	// 	/// add double push to mask

	// 	if(from / 8 == 1 || from / 8 == 6)
	// 		push |= shift(color, NORTH, push) & ~occ;

	// 	return (to / 8 && to / 8 != 7) && t == NEUT && (((att & enemy) | push) & (1ULL << to));
	// }

	// /// king moves (normal or castle)

	// if(t == NEUT)
	// return kingBBAttacks[from] & (1ULL << to);

	// int side = (to % 8 == 6); /// queen side or king side

	// if(board.castleRights & (1 << (2 * color + side))) { /// can i castle
	// if(isSqAttacked(board, color ^ 1, from) || isSqAttacked(board, color ^ 1, to))
	// 	return 0;

	// /// castle queen side

	// if(!side) {
	// 	return !(occ & (7ULL << (from - 3))) && !isSqAttacked(board, color ^ 1, Sq(between[from][to]));
	// } else {
	// 	return !(occ & (3ULL << (from + 1))) && !isSqAttacked(board, color ^ 1, Sq(between[from][to]));
	// }
	// }

	// return 0;
	return true;
}

bool Position::is_legal(Move m) {
	PositionInfo info;
	play_move(m, &info);
	Bitboard checkers = attackers_to_sq(lsb(pieces[~turn][KING]), turn);
	unplay_move(m);
	return checkers;
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

void Position::generate_pawn_captures(Move*& m, Bitboard target) const {
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

void Position::generate_pawn_pushes(Move*& m, Bitboard target) const {
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

void Position::generate_pieces(Move*& m, Bitboard target) const {
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

void Position::generate_castling(Move*& m) const {
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

void Position::generate_captures(Move*& m) const {
	const Square king = lsb(pieces[turn][KING]);
	const Color enemy = ~turn;
	// Check if we are in check
	if (state->checkers) {
		// If more than one checker, return just the king captures
		if (popcount(state->checkers) > 1)
			goto king_captures;
		// We only have one checker, so try to capture the checker with the remaining pieces
		generate_pawn_captures(m, state->checkers);
		generate_pieces(m, state->checkers);
	}
	// If we're not in check, just try to target all of the enemy pieces
	else {
		generate_pieces(m, colors[enemy]);
		generate_pawn_captures(m, colors[enemy]);
	}
king_captures:
	add_moves(m, king, king_moves(king) & ~state->king_unsafe & colors[enemy]);
}

void Position::generate_quiets(Move*& m) const {
	const Square king = lsb(pieces[turn][KING]);
	// Check if we are in check
	if (state->checkers) {
		// If more than one checker, add just the king moves
		if (popcount(state->checkers) > 1)
			goto king_quiets;
		const Bitboard blocks = bb_ray(king, lsb(state->checkers)) & ~state->checkers; // Exclude the checker itself, it will have been generated among the captures
		generate_pieces(m, blocks);
		generate_pawn_pushes(m, blocks);
	}
	else {
		// Castling
		generate_castling(m);
		// All regular piece moves
		generate_pieces(m, ~all_pieces);
		generate_pawn_pushes(m, ~all_pieces);
	}
king_quiets:
	add_moves(m, king, king_moves(king) & ~state->king_unsafe & ~all_pieces);
}

Moves Position::generate_legal() {
	Moves moves;
	generate_captures(moves.end);
	generate_quiets(moves.end);
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
