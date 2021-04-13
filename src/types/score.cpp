#include "score.h"

Score mobility_scores[NUM_PIECE_TYPES] = {Score(0.1, 0.1), Score(0.1, 0.1), Score(0.1, 0.1), Score(0.1, 0.1), Score(0.1, 0.1), Score(0.1, 0.1)};
Score material_scores[NUM_PIECE_TYPES] = {Score(1, 1), Score(3.5, 3.5),Score(3.5, 3.5), Score(5.25, 5.25), Score(10, 10), Score(1000,1000)};

//bishop
Score BISHOP_PAIR_SCORE = Score(0.03, 0.03);
Score BISHOP_DEFENDED_SCORE = Score(0.01, 0.01);
Score BISHOP_OUTPOST_SCORE = Score(0.01, 0.01);
Score BISHOP_FIANCHETTO_SCORE = Score(0.01,0);
Score BISHOP_KING_DISTANCE_PENALTY = Score(0.1, 0.1);
Score BISHOP_XRAY_PAWN_PENALTY = Score(0.1, 0.1);
Score BISHOP_SHIELDED_SCORE = Score(0.1, 0.1);
Score BISHOP_ATTACKING_KING_SCORE = Score(0.1, 0.1);
//knights
Score KNIGHT_DEFENDED_SCORE = Score(0.01, 0.01);
Score KNIGHT_OUTPOST_SCORE = Score(0.1, 0.01);
Score KNIGHT_KING_DISTANCE_PENALTY = Score(0.1, 0.1);
Score KNIGHT_SHIELDED_SCORE = Score(0.1, 0.1);
