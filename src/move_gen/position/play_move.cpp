#include "types.h"

void Position::play_move(Move m) {
	Square from = move_from(m);
	Square to = move_to(m);
	Bitboard both = from | to;

	Piece captured = move_type(m) == S_MOVE_EN_PASSANT ? piece_init(PAWN, enemy) : piece_on(to);
	// Square capped = move_type(m) == S_MOVE_EN_PASSANT ? to -

	if (captured != NO_PIECE)
		remove_piece(to);

	pieces[turn][piece_type(piece_on(from))] ^= both;
	colors[turn] ^= both;

}

void Position::unplay_move(Move m) {

}

void Position::move_piece(Square from, Square to) {
	
}

void Position::remove_piece(Square s) {

}

void Position::place_piece(Piece p, Square s) {
	board[s] = p;
	pieces[piece_color(p)][piece_type(p)] |= BB_SQUARES[s];
	colors[piece_color(p)] |= BB_SQUARES[s];
	all_pieces |= BB_SQUARES[s];
}