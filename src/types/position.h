#ifndef _POSITION_H_
#define _POSITION_H_

#include "bitboard.h"
#include "piece.h"
#include "move.h"
#include "score.h"

#include <string>
const std::string defaultFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

class Position {
public:
	Position(std::string fen = defaultFEN);

	Piece piece_on(Square s) const;
	Square square_of(PieceType p, Color c) const;
	Bitboard attackers_to_sq(Square s, Color c) const;
	Bitboard controlling(Color c) const;
	Bitboard king_safe_squares(Square king, Color c) const;

	Bitboard blockers(Square s, Color blocking, Color attacking) const;
	// Bitboard pinned_pieces(Color c) const;
	// Bitboard fossilization_pieces(Color c) const;

	Moves legal_moves() const;
	Moves generate_moves() const;
	Moves generate_blockers() const;
	//TODO legal_moves_of_piece(Color c, PIECE_TYPE p); returns 1 bb of all legal moves of piece p of color c

	Phase calculate_phase();
	Score calculate_score(Color c);
// private:
	void place_piece(Piece p, Square s);

	Piece board[NUM_SQUARES];

	Bitboard pieces[NUM_COLORS][NUM_PIECE_TYPES] = {0};
	Bitboard colors[NUM_COLORS] = {0};
	Bitboard all_pieces = 0;
	Bitboard checkers = 0;

	MoveCount fullMoves;
	MoveCount halfMoves;
	Color turn;
	Square en_peasant;
	Castling castling;
	Phase phase;
};

std::ostream& operator<<(std::ostream& os, const Position& p);

#endif