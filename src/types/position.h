#ifndef _POSITION_H_
#define _POSITION_H_

#include "bitboard.h"
#include "piece.h"
#include "move.h"
#include "score.h"
#include "zobrist.h"
#include "value.h"

#include <string>
const std::string defaultFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

struct PositionInfo {
	// Useful Bitboards and variables used through move generation and evaluation. These should all be precomputed by info_init().
	Bitboard checkers = 0;
	Bitboard check_blocks = 0;
	Bitboard pinned = 0;
	Bitboard king_unsafe = 0;

	Piece captured = NO_PIECE; // The piece that was captured last move
	Square en_peasant = NO_SQUARE;
	Castling castling = 0;
	Phase phase = 0;

	Move last_move;
	MoveCount rule50 = 0;
	PositionInfo* previous = nullptr;
	Key position_key = 0;
	Key pawn_key = 0;
};

struct PawnInfo {
	Score scores[NUM_COLORS] = {Score(0,0), Score(0,0)};
	Bitboard passed[NUM_COLORS] = {0};
	Bitboard pawn_attacks[NUM_COLORS] = {0};
	Bitboard pawn_attack_span[NUM_COLORS] = {0};
	Key key = 0;
	int blocked = 0;
	PawnInfo(){};
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
	Phase phase = 0;
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

	void generate_pawn_captures(Move*& m, Bitboard target) const;
	void generate_pawn_pushes(Move*& m, Bitboard target) const;
	void generate_pieces(Move*& m, Bitboard target) const;
	void generate_castling(Move*& m) const;

	void generate_captures(Move*& m) const;
	void generate_quiets(Move*& m) const;
	Moves generate_legal();

	bool static_exchange_evaluation(Move m, Value threshold) const;
	bool insufficient_material() const;
	bool is_repetition() const;
	bool is_draw() const;
	bool is_capture(Move m) const;

	bool is_pseudo_legal(Move m);
	bool is_legal(Move m);

	// Function to play or unplay a move, or move a piece
	void play_move(Move m, PositionInfo* info);
	void unplay_move(Move m);
	void play_null_move(PositionInfo* info);
	void unplay_null_move();
	void move_piece(Square from, Square to);
	void remove_piece(Square s);
	void place_piece(Piece p, Square s);
	Square castle(Square to);
	void uncastle(Square to);

	// Functions to test move generation
	uint64_t perft(int depth);
	uint64_t divide(int depth);

	// Get legal move bitboards for evaluation
	// Bitboard legal_knight_moves() const;
	// Bitboard legal_bishop_moves() const;
	// Bitboard legal_rook_moves() const;
	// Bitboard legal_queen_moves() const;

	// Evaluation functions
	void eval_init();
	Phase calculate_phase();
	//eval pawns
	Bitboard get_pawn_moves(Color c, Bitboard pawns);
	Bitboard get_pawn_double_attacks(Color c, Bitboard pawns);
	//material
	int queen_pin_count(Color opp, Square q);
	Bitboard get_pseudo_legal_moves(PieceType p, Square s);
	bool is_open_file(Color c, File f);
	bool is_outpost(Color c, Square s);
	Score knight_score(Color c);
	Score bishop_score(Color c);
	Score rook_score(Color c);
	Score queen_score(Color c);
	Score pawn_storm_safety(Color c);
	Score king_score(Color c);
	//pawns
	Score pawn_score(Color c, std::vector<PawnInfo>* pawn_hash_table);
	void pawn_info_init(Color c, PawnInfo* p_info);
	PawnInfo* get_pawn_info(Key key, std::vector<PawnInfo>* pawn_hash_table);
	//func
	Score calculate_score(std::vector<PawnInfo>* pawn_hash_table);
	Score calculate_threats(Color c);
	Score calculate_material(std::vector<PawnInfo>* pawn_hash_table);
	Score calculate_initiative(Score score, std::vector<PawnInfo>* pawn_hash_table);
	Value interpolate_score(Score score);
	Value evaluate(std::vector<PawnInfo>* pawn_hash_table);
	
	Score calculate_space(Color c, PawnInfo* p_info);
	Score calculate_passed(Color c, PawnInfo* p_info);

	void info_init();
	void key_init();

	Piece board[NUM_SQUARES];
	Bitboard pieces[NUM_COLORS][NUM_PIECE_TYPES] = {0};
	// Bitboard current_legal_moves[NUM_PIECE_TYPES] = {0};
	Bitboard colors[NUM_COLORS] = {0};
	Bitboard all_pieces = 0;

	Castling castlingMask[NUM_SQUARES];

	PositionInfo* state;
    EvalInfo info;
	MoveCount fullMoves;
	MoveCount halfMoves;
	Color turn;

	int ply = 0;
	uint64_t perft_speed = 0;
};

std::ostream& operator<<(std::ostream& os, const Position& p);

#endif