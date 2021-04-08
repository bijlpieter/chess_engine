#ifndef _SCORE_H_
#define _SCORE_H_

#include "bitboard.h"
#include "piece.h"
#include "position.h"

class Score{
    public:
    Score(uint16_t mg, uint16_t eg);
    uint16_t middle_game;
    uint16_t end_game;
};
Score::Score(uint16_t mg, uint16_t eg){
    middle_game = mg;
    end_game = eg;
}
inline Score operator+(Score a, Score b) { return Score((a.middle_game + b.middle_game),(a.end_game + b.end_game))}
#endif _SCORE_H_
