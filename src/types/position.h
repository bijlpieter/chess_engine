#ifndef _POSITION_H_
#define _POSITION_H_

#include "bitboard.h"
#include "piece.h"
#include "move.h"
#include "score.h"

#include <string>
const std::string defaultFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

struct PositionInfo {
	// Useful Bitboards and variables used through move generation and evaluation. These should all be precomputed by info_init().
	Bitboard checkers;
	Bitboard check_blocks;
	Bitboard pinned;	
	Bitboard king_unsafe;
	Square king;
	Color enemy;

	Piece captured = NO_PIECE; // The piece that was captured last move
	Square en_peasant;
	Castling castling;
	Phase phase;

	PositionInfo* previous;
};

struct EvalInfo {
	Color c;
	Rank promotion_rank;
	Rank opp_promotion_rank;
    Square king_square;
	Square opp_king_square;
    Bitboard defended_squares;
    Bitboard king_area[NUM_COLORS];
    Bitboard mobility;
	Bitboard blocked_pawns;
	Bitboard enemy_pawn_control;
	Direction push_direction;
};

class Position {
public:
	Position(PositionInfo* info, std::string fen = defaultFEN);

	Piece piece_on(Square s) const;
	Square square_of(PieceType p, Color c) const;
	Bitboard attackers_to_sq(Square s, Color c) const;

	Bitboard controlling_regular(Color c) const;
	Bitboard controlling_sliding(Color c, Bitboard occ) const;
	Bitboard controlling(Color c, Bitboard occ) const;

	Bitboard snipers_to_king(Color c, Bitboard occ = 0ULL) const;
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
	void play_move(Move m, PositionInfo* info);
	void unplay_move(Move m);
	void move_piece(Square from, Square to);
	void remove_piece(Square s);
	void place_piece(Piece p, Square s);
	void castle(Square to);
	void uncastle(Square to);

	// Functions to test move generation
	uint64_t perft(int depth);
	uint64_t divide(int depth);

	// Get legal move bitboards for evaluation
	Bitboard legal_knight_moves() const;
	Bitboard legal_bishop_moves() const;
	Bitboard legal_rook_moves() const;
	Bitboard legal_queen_moves() const;

	// Evaluation functions
	Score knight_score();
	Score bishop_score();
	Score rook_score();
	Score queen_score();
	Score king_score();
	Score pawn_score();
	Score control_score();
	Score calculate_material();
	Phase calculate_phase();
	Score calculate_score(Color c);
	Score pawn_storm_safety();
	Square farmost_square(Color c, Bitboard b);
	bool is_open_file(Color c, File f);
	bool is_outpost(Color c, Square s);
	void eval_init(Color c);
	int queen_pin_count(Color opp, Square q);
	Rank relevant_rank(Color c, Rank r);

	void info_init();

	Piece board[NUM_SQUARES];
	Bitboard pieces[NUM_COLORS][NUM_PIECE_TYPES] = {0};
	// Bitboard current_legal_moves[NUM_PIECE_TYPES] = {0};
	Bitboard colors[NUM_COLORS] = {0};
	Bitboard all_pieces = 0;

	PositionInfo* state;
    EvalInfo info;
	MoveCount fullMoves;
	MoveCount halfMoves;
	Color turn;

	uint64_t perft_speed = 0;
};

std::ostream& operator<<(std::ostream& os, const Position& p);

#endif