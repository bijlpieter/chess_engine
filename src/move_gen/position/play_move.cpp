#include "types.h"

// Plays a move and updates all the member variables.
// Assumes that the move is legal.
void Position::play_move(Move m) {
	Square from = move_from(m);
	Square to = move_to(m);
	MoveType type = move_type(m);
	Direction forward = turn ? DOWN : UP;

	captured = type == S_MOVE_EN_PASSANT ? piece_init(PAWN, enemy) : piece_on(to);
	Square capped = type == S_MOVE_EN_PASSANT ? to - forward : to;

	if (type == S_MOVE_CASTLING)
		castle(to);

	if (captured != NO_PIECE)
		remove_piece(capped);
	
	if (type == S_MOVE_EN_PASSANT)
		board[capped] = NO_PIECE;

	if (piece_type(board[from]) == PAWN) {
		if (from ^ to == 16)
			en_peasant = to - forward;
		else if (type == S_MOVE_PROMOTION) {
			remove_piece(to);
			Piece promo = piece_init(PieceType((move_promo(m) >> 12) + 2), turn);
			place_piece(promo, to);
		}
	}

	turn = ~turn;
	info_init();
}

void Position::unplay_move(Move m) {

}

void Position::move_piece(Square from, Square to) {
	Piece p = board[from];
	Bitboard both = from | to;
	pieces[piece_color(p)][piece_type(p)] ^= both;
	colors[piece_color(p)] ^= both;
	all_pieces ^= both;
}

void Position::remove_piece(Square s) {
	Piece p = board[s];
	pieces[piece_color(p)][piece_type(p)] ^= s;
	colors[piece_color(p)] ^= s;
	all_pieces ^= s;
}

void Position::place_piece(Piece p, Square s) {
	board[s] = p;
	pieces[piece_color(p)][piece_type(p)] |= BB_SQUARES[s];
	colors[piece_color(p)] |= BB_SQUARES[s];
	all_pieces |= BB_SQUARES[s];
}

void Position::castle(Square to) {
	switch(to) {
	case G1: move_piece(E1, G1); move_piece(H1, F1); return; // White kingside
	case C1: move_piece(E1, C1); move_piece(A1, D1); return; // White queenside
	case G8: move_piece(E8, G8); move_piece(H8, F8); return; // Black kingside
	case C8: move_piece(E8, C8); move_piece(A8, D8); return; // Black queenside
	}
}

void Position::uncastle(Square to) {
	switch(to) {
	case G1: move_piece(G1, E1); move_piece(F1, H1); return; // White kingside
	case C1: move_piece(C1, E1); move_piece(D1, A1); return; // White queenside
	case G8: move_piece(G8, E8); move_piece(F8, H8); return; // Black kingside
	case C8: move_piece(C8, E8); move_piece(D8, A8); return; // Black queenside
	}
}