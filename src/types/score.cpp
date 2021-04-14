#include "score.h"

Score mobility_scores[NUM_PIECE_TYPES] = {Score(5, 5), Score(5, 5), Score(5, 5), Score(5, 5), Score(5, 5), Score(5, 5)};
Score material_scores[NUM_PIECE_TYPES] = {Score(100, 100), Score(350, 350),Score(350, 350), Score(525, 525), Score(1000, 1000), Score(100000,100000)};

//bishop
Score BISHOP_PAIR_SCORE = Score(50, 50);
Score BISHOP_DEFENDED_SCORE = Score(10, 10);
Score BISHOP_OUTPOST_SCORE = Score(20, 20);
Score BISHOP_FIANCHETTO_SCORE = Score(20,0);
Score BISHOP_KING_DISTANCE_PENALTY = Score(1, 1);
Score BISHOP_XRAY_PAWN_PENALTY = Score(10, 10);
Score BISHOP_SHIELDED_SCORE = Score(10, 10);
Score BISHOP_ATTACKING_KING_SCORE = Score(10, 10);
//knights
Score KNIGHT_DEFENDED_SCORE = Score(10, 10);
Score KNIGHT_OUTPOST_SCORE = Score(20, 20);
Score KNIGHT_KING_DISTANCE_PENALTY = Score(1, 1);
Score KNIGHT_SHIELDED_SCORE = Score(10, 10);
//rook
Score ROOK_ON_KING_FILE_SCORE = Score(10, 10);
Score ROOK_ON_KING_RANK_SCORE = Score (10, 10);
Score ROOK_ON_QUEEN_LINE_SCORE = Score(15, 15);
Score ROOK_ON_OPEN_SCORE = Score(20, 5);
Score ROOK_STACKED_SCORE = Score (20,20);
Score ROOK_ON_SEVENTH_SCORE = Score (100, 20);
Score ROOK_ON_BLOCKED_PENALTY = Score(5, 50);
Score ROOK_TRAPPED_BY_KING_PENALTY = Score(100, 100);
//king
Score PAWN_STORM_BLOCKED_FILE_PENALTY[NUM_FILES] = {Score(0,0), Score(0,0), Score(30, 30), Score (-10,20), Score (-10, 20), Score(-10, 20), Score(0, 10), Score(0,0)};
Score PAWN_STORM_UNBLOCKED_FILE_PENALTY[NUM_FILES] = {Score(0,0), Score(0,0), Score(30, 30), Score (-10,20), Score (-10, 20), Score(-10, 20), Score(0, 10), Score(0,0)};
Score KING_ON_OPEN_FILE_PENALTY = Score(50,0);
std::ostream& operator<<(std::ostream& os, const Score& s) {
    os << "M-Score: " << s.middle_game << " E-Score: " << s.end_game <<std::endl;
	return os;
}
