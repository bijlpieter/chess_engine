#ifndef _SCORE_H_
#define _SCORE_H_

#include "bitboard.h"
#include "position.h"
typedef uint16_t Phase;
// Pawn, Knight, Bishop, Rook, Queen, King
const float mobility_middle[NUM_PIECE_TYPES] = {0.01, 0.01, 0.01, 0.01, 0.01, 0.01};
const float mobility_end[NUM_PIECE_TYPES] = {0.01, 0.01, 0.01, 0.01, 0.01, 0.01};
const float material_score[NUM_PIECE_TYPES] = {1, 3.5, 3.5, 5.25, 10.00, 1000};

const float KNIGHT_PAWN_MODIFIER = 0.05, KNIGHT_DEFENDED_MODIFIER = 0.01, KNIGHT_OUTPOST_MODIFIER = 0.1;
const float BISHOP_PAIR_MODIFIER = 0.03, BISHOP_DEFENDED_MODIFIER = 0.01, BISHOP_OUTPOST_MODIFIER = 0.1, BISHOP_FIANCHETTO_MODIFIER = 0.1; 



class Score {
    public:
        Score(float mg, float eg) {
            middle_game = mg;  
            end_game = eg;
        }
        float middle_game;
        float end_game;
};

inline Score operator+(Score a, Score b) { return Score((a.middle_game + b.middle_game),(a.end_game + b.end_game));}
inline Score operator-(Score a, Score b) { return Score((a.middle_game - b.middle_game),(a.end_game - b.end_game));}
inline Score& operator+=(Score& a, Score b) { return a = Score((a.middle_game + b.middle_game),(a.end_game + b.end_game));}

#endif
