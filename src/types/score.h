#ifndef _SCORE_H_
#define _SCORE_H_

#include "bitboard.h"
#include "position.h"
typedef uint16_t Phase;
// Pawn, Knight, Bishop, Rook, Queen, King
const float mobility_middle[6] = {0.01, 0.01, 0.01, 0.01, 0.01, 0.01};
const float mobility_end[6] = {0.01, 0.01, 0.01, 0.01, 0.01, 0.01};
const float material_score[6] = {1, 3.5, 3.5, 5.25, 10.00, 1000};

const float KNIGHT_PAWN_MODIFIER = 0.05, KNIGHT_DEFENDED_MODIFIER = 0.01;


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
