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

	Piece board[NUM_SQUARES];
	Piece captured = NO_PIECE; // The piece that was captured last move

	Bitboard pieces[NUM_COLORS][NUM_PIECE_TYPES] = {0};
	// Bitboard current_legal_moves[NUM_PIECE_TYPES] = {0};
	Bitboard colors[NUM_COLORS] = {0};
	Bitboard all_pieces = 0;
	
	// Useful Bitboards and variables used through move generation and evaluation. These should all be precomputed by info_init().
	Bitboard checkers;
	Bitboard check_blocks;
	Bitboard pinned;	
	Bitboard king_unsafe;
	Square king;
	Color enemy;

	Moves moves;
	MoveCount fullMoves;
	MoveCount halfMoves;
	Color turn;
	Square en_peasant;
	Castling castling;
	Phase phase;

	Piece piece_on(Square s) const;
	Square square_of(PieceType p, Color c) const;
	Bitboard attackers_to_sq(Square s, Color c) const;

	Bitboard controlling_regular(Color c) const;
	Bitboard controlling_sliding(Color c, Bitboard occ) const;
	Bitboard controlling(Color c, Bitboard occ) const;

	Bitboard blockers(Square s, Color blocking, Color attacking) const;
	// Bitboard pinned_pieces(Color c) const;
	// Bitboard fossilization_pieces(Color c) const;

	// Move generation if king is not in check
	Moves generate_moves();

	// Move generation if king is in check
	Moves generate_blockers();

	// Generates all moves in the position
	Moves legal_moves();

	// Function to play or unplay a move, or move a piece
	void play_move(Move m);
	void unplay_move(Move m);
	void move_piece(Square from, Square to);
	void remove_piece(Square s);
	void place_piece(Piece p, Square s);
	void castle(Square to);
	void uncastle(Square to);

	// Get legal move bitboards for evaluation
	Bitboard legal_knight_moves() const;
	Bitboard legal_bishop_moves() const;
	Bitboard legal_rook_moves() const;
	Bitboard legal_queen_moves() const;

	// Evaluation functions
	Score knight_score(Color c, Bitboard enemy_pawn_control, Bitboard defended_squares);
	Score bishop_score(Color c, Bitboard enemy_pawn_control, Bitboard defended_squares);
	Score rook_score(Color c, Bitboard enemy_pawn_control);
	Phase calculate_phase();
	Score calculate_score(Color c);
	Score calculate_material(Color c);
	bool is_outpost(Color c, Square s);

	void info_init();
};

std::ostream& operator<<(std::ostream& os, const Position& p);

#endif