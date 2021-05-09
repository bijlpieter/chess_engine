#include "types.h"
#include <iostream>

// Plays a move and updates all the member variables.
// Assumes that the move is legal.
void Position::play_move(Move m, PositionInfo* info) {
	info->previous = state;
	state = info;
	state->en_peasant = NO_SQUARE;
	state->position_key = state->previous->position_key;
	state->position_key ^= zobrist.side;

	Square from = move_from(m);
	Square to = move_to(m);
	MoveType type = move_type(m);
	Piece p = board[from];
	Direction forward = turn ? DOWN : UP;

	state->captured = type == S_MOVE_EN_PASSANT ? piece_init(PAWN, ~turn) : piece_on(to);
	Square capped = type == S_MOVE_EN_PASSANT ? to - forward : to;

	if (type == S_MOVE_CASTLING) {
		to = castle(to);
		state->captured = NO_PIECE;
	}

	state->castling = state->previous->castling;

	if (state->castling && (castlingMask[from] | castlingMask[to])) {
		state->position_key ^= zobrist.castling[state->castling];
		state->castling &= ~(castlingMask[from] | castlingMask[to]);
		state->position_key ^= zobrist.castling[state->castling];
	}

	if (state->captured != NO_PIECE) {
		remove_piece(capped);
		state->position_key ^= zobrist.piece_square[p][capped];
		if (piece_type(state->captured) == PAWN)
			state->pawn_key ^= zobrist.piece_square[p][capped];
	}

	state->position_key ^= zobrist.piece_square[p][to] ^ zobrist.piece_square[p][from];

	Square prev_en_passant = state->previous->en_peasant;
	if (prev_en_passant != NO_SQUARE)
		state->position_key ^= zobrist.en_passant[file(prev_en_passant)];

	if (type != S_MOVE_CASTLING)
		move_piece(from, to);
	
	if (type == S_MOVE_EN_PASSANT)
		board[capped] = NO_PIECE;

	if (piece_type(p) == PAWN) {
		state->pawn_key ^= zobrist.piece_square[p][from] ^ zobrist.piece_square[p][to];

		if ((int(from) ^ int(to)) == 16) {
			state->en_peasant = to - forward;
			state->position_key ^= zobrist.en_passant[file(state->en_peasant)];
		}
		else if (type == S_MOVE_PROMOTION) {
			remove_piece(to);
			Piece promo = piece_init(PieceType((move_promo(m) >> 12) + 1), turn);
			place_piece(promo, to);

			state->position_key ^= zobrist.piece_square[p][to] ^ zobrist.piece_square[promo][to];
			state->pawn_key ^= zobrist.piece_square[p][to];
		}
	}

	turn = ~turn;
	info_init();
}

void Position::unplay_move(Move m) {
	turn = ~turn;
	Square from = move_from(m);
	Square to = move_to(m);
	MoveType type = move_type(m);

	if (type == S_MOVE_PROMOTION) {
		remove_piece(to);
		place_piece(piece_init(PAWN, turn), to);
	}
	
	if (type == S_MOVE_CASTLING) {
		uncastle(to);
	}
	else {
		move_piece(to, from);
		if (state->captured != NO_PIECE) {
			Direction forward = turn ? DOWN : UP;
			Square capped = type == S_MOVE_EN_PASSANT ? to - forward : to;
			place_piece(state->captured, capped);
		}
	}
	state = state->previous;
}

void Position::move_piece(Square from, Square to) {
	Piece p = board[from];
	Bitboard both = from | to;
	pieces[piece_color(p)][piece_type(p)] ^= both;
	colors[piece_color(p)] ^= both;
	all_pieces ^= both;
	board[from] = NO_PIECE;
	board[to] = p;
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

// Returns the actual square the king moves to
Square Position::castle(Square to) {
	switch(to) {
	case H1: move_piece(E1, G1); move_piece(H1, F1); state->position_key ^= zobrist.piece_square[state->captured][H1] ^ zobrist.piece_square[state->captured][F1]; return G1; // White kingside
	case A1: move_piece(E1, C1); move_piece(A1, D1); state->position_key ^= zobrist.piece_square[state->captured][A1] ^ zobrist.piece_square[state->captured][D1]; return C1; // White queenside
	case H8: move_piece(E8, G8); move_piece(H8, F8); state->position_key ^= zobrist.piece_square[state->captured][H8] ^ zobrist.piece_square[state->captured][F8]; return G8; // Black kingside
	case A8: move_piece(E8, C8); move_piece(A8, D8); state->position_key ^= zobrist.piece_square[state->captured][A8] ^ zobrist.piece_square[state->captured][D8]; return C8; // Black queenside
	default: return NO_SQUARE; // WTF
	}
}

void Position::uncastle(Square to) {
	switch(to) {
	case H1: move_piece(G1, E1); move_piece(F1, H1); return; // White kingside
	case A1: move_piece(C1, E1); move_piece(D1, A1); return; // White queenside
	case H8: move_piece(G8, E8); move_piece(F8, H8); return; // Black kingside
	case A8: move_piece(C8, E8); move_piece(D8, A8); return; // Black queenside
	default: return; // WTF
	}
}