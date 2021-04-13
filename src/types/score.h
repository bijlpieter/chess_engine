#ifndef _SCORE_H_
#define _SCORE_H_

#include "bitboard.h"

class Score {
    public:
        Score(float mg, float eg) {
            middle_game = mg;  
            end_game = eg;
        }
        float middle_game;
        float end_game;
};

typedef uint16_t Phase;
// Pawn, Knight, Bishop, Rook, Queen, King
extern Score mobility_scores[NUM_PIECE_TYPES];
extern Score material_scores[NUM_PIECE_TYPES];
extern Score BISHOP_PAIR_SCORE, BISHOP_DEFENDED_SCORE, BISHOP_OUTPOST_SCORE,
BISHOP_FIANCHETTO_SCORE, BISHOP_SHIELDED_SCORE, BISHOP_ATTACKING_KING_SCORE;
extern Score BISHOP_KING_DISTANCE_PENALTY, BISHOP_XRAY_PAWN_PENALTY;

extern Score KNIGHT_DEFENDED_SCORE, KNIGHT_OUTPOST_SCORE, KNIGHT_SHIELDED_SCORE;
extern Score KNIGHT_KING_DISTANCE_PENALTY;
const float KNIGHT_PAWN_MODIFIER = 0.05;

inline Score operator+(Score a, Score b) { return Score((a.middle_game + b.middle_game),(a.end_game + b.end_game));}
inline Score operator-(Score a, Score b) { return Score((a.middle_game - b.middle_game),(a.end_game - b.end_game));}
inline Score operator*(int a, Score b) { return Score((a * b.middle_game),(a * b.end_game));}
inline Score operator*(Score b, int a) { return Score((a * b.middle_game),(a * b.end_game));}
inline Score operator/(Score b, int a) { return Score((b.middle_game / a),(b.end_game / a));}

inline Score& operator+=(Score& a, Score b) { return a = Score((a.middle_game + b.middle_game),(a.end_game + b.end_game));}
inline Score& operator-=(Score& a, Score b) { return a = Score((a.middle_game - b.middle_game),(a.end_game - b.end_game));}

#endif
