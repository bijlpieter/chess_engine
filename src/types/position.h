#ifndef _POSITION_H_
#define _POSITION_H_

#include "bitboard.h"
#include "piece.h"
#include "move.h"
#include "score.h"
#include "zobrist.h"

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
	Key position_key;
	Key pawn_key;
};

struct PawnInfo {
	Score scores[NUM_COLORS];
	Bitboard passed[NUM_COLORS];
	Bitboard pawn_attacks[NUM_COLORS];
	Bitboard pawn_attack_span[NUM_COLORS];
	int blocked;
	Key key;
};

struct EvalInfo {
	// const
	const Rank promotion_rank[NUM_COLORS] = {RANK_8, RANK_1};
	const Rank third_rank[NUM_COLORS] = {RANK_3, RANK_6};
	const Direction push_direction[NUM_COLORS] = {UP, DOWN};
	const Direction left_pawn_attack[NUM_COLORS] = {UP_LEFT, DOWN_LEFT};
	const Direction right_pawn_attack[NUM_COLORS] = {UP_RIGHT, DOWN_RIGHT};

	Bitboard king_area[NUM_COLORS];
    Square king_squares[NUM_COLORS];
	Bitboard pinned[NUM_COLORS] = {0};
	Bitboard controlled_by[NUM_COLORS][NUM_PIECE_TYPES] = {0};
	Bitboard controlled_twice[NUM_COLORS] = {0};
    Bitboard controlled_squares[NUM_COLORS] = {0};
    Bitboard mobility[NUM_COLORS];
	Bitboard blocked_pawns[NUM_COLORS];
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

	void generate_pawn_captures(Move*& m, Bitboard target);
	void generate_pawn_pushes(Move*& m, Bitboard target);
	void generate_pieces(Move*& m, Bitboard target);
	void generate_castling(Move*& m);

	// Generate all moves
	Moves generate_moves();

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

	Bitboard get_pawn_moves(Color c, Bitboard pawns);
	Bitboard get_pawn_double_attacks(Color c, Bitboard pawns);
	Bitboard forward_ranks(Color c, Square s);
	Bitboard forward_files(Color c, Square s);
	Bitboard adjacent_files(Square s);
	Bitboard pawn_att_span(Color c, Square s);
	Bitboard pass_pawn_span(Color c, Square s);
	//Phase
	Phase calculate_phase();

	// Evaluation functions
	Score knight_score(Color c);
	Score bishop_score(Color c);
	Score rook_score(Color c);
	Score queen_score(Color c);
	Score pawn_storm_safety(Color c);
	Score king_score(Color c);
	Score pawn_score(Color c);
	Score calculate_material();
	Score calculate_threats(Color c);
	void pawn_info_init(Color c, PawnInfo* p_info);
	PawnInfo* get_pawn_info(Key key);
	
	Score control_score();
	Score calculate_score();
	
	Square farmost_square(Color c, Bitboard b);
	bool is_open_file(Color c, File f);
	bool is_outpost(Color c, Square s);
	bool more_than_one(Bitboard pieces);
	void eval_init();
	int queen_pin_count(Color opp, Square q);
	Rank relevant_rank(Color c, Rank r);
	Bitboard get_pseudo_legal_moves(PieceType p, Square s);

	void info_init();
	void key_init();

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