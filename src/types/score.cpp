#include "score.h"


Score mobility_scores[NUM_PIECE_TYPES] = {Score(0.1, 0.1), Score(0.1, 0.1), Score(0.1, 0.1), Score(0.1, 0.1), Score(0.1, 0.1), Score(0.1, 0.1)};
Score material_scores[NUM_PIECE_TYPES] = {Score(1, 1), Score(3.5, 3.5),Score(3.5, 3.5), Score(5.25, 5.25), Score(10, 10), Score(1000,1000)};

//bishop
Score BISHOP_PAIR_SCORE = Score(0.3, 0.3);
Score BISHOP_DEFENDED_SCORE = Score(0.01, 0.01);
Score BISHOP_OUTPOST_SCORE = Score(0.01, 0.01);
Score BISHOP_FIANCHETTO_SCORE = Score(0.01,0);
Score BISHOP_KING_DISTANCE_PENALTY = Score(0.01, 0.01);
Score BISHOP_XRAY_PAWN_PENALTY = Score(0.02, 0.02);
Score BISHOP_SHIELDED_SCORE = Score(0.1, 0.1);
Score BISHOP_ATTACKING_KING_SCORE = Score(0.1, 0.1);
//knights
Score KNIGHT_DEFENDED_SCORE = Score(0.01, 0.01);
Score KNIGHT_OUTPOST_SCORE = Score(0.1, 0.01);
Score KNIGHT_KING_DISTANCE_PENALTY = Score(0.01, 0.01);
Score KNIGHT_SHIELDED_SCORE = Score(0.1, 0.1);
//rook
Score ROOK_ON_KING_FILE_SCORE = Score(0.1, 0.1);
Score ROOK_ON_KING_RANK_SCORE = Score (0.1, 0.1);
Score ROOK_ON_QUEEN_LINE_SCORE = Score(0.1, 0.1);
Score ROOK_ON_OPEN_SCORE = Score(0.1, 0.1);
Score ROOK_STACKED_SCORE = Score (0.1,0.1);
Score ROOK_ON_SEVENTH_SCORE = Score (0.1, 0.2);
Score ROOK_ON_BLOCKED_PENALTY = Score(0.1, 0.1);
Score ROOK_TRAPPED_BY_KING_PENALTY = Score(0.5, 0.1);

std::ostream& operator<<(std::ostream& os, const Score& s) {
    os << "M-Score: " << s.middle_game << " E-Score: " << s.end_game <<std::endl;
	return os;
}
