#ifndef _SCORE_H_
#define _SCORE_H_

#include "bitboard.h"
#include <iostream>

class Score {
    public:
        Score(int mg, int eg) {
            middle_game = mg;  
            end_game = eg;
        }
        int middle_game;
        int end_game;
};

typedef uint16_t Phase;
// Pawn, Knight, Bishop, Rook, Queen, King
extern Score mobility_scores[NUM_PIECE_TYPES];
extern Score material_scores[NUM_PIECE_TYPES];
//pawn
extern Score PAWN_CONNECTED_SCORE,PAWN_SUPPORTED_SCORE,PAWN_ADVANCED_BLOCK_SCORE;

extern Score PAWN_EARLY_DOUBLE_PENALTY,PAWN_DOUBLED_PENALTY,PAWN_ISOLATED_PENALTY,PAWN_BACKWARDS_PENALTY, PAWN_WEAK_LEVER_PENALTY;
//bishop
extern Score BISHOP_PAIR_SCORE, BISHOP_DEFENDED_SCORE, BISHOP_OUTPOST_SCORE,
BISHOP_FIANCHETTO_SCORE, BISHOP_SHIELDED_SCORE, BISHOP_ATTACKING_KING_SCORE;
extern Score BISHOP_KING_DISTANCE_PENALTY, BISHOP_XRAY_PAWN_PENALTY;
//knight
extern Score KNIGHT_DEFENDED_SCORE, KNIGHT_OUTPOST_SCORE, KNIGHT_SHIELDED_SCORE;
extern Score KNIGHT_KING_DISTANCE_PENALTY;
//rook
extern Score ROOK_ON_KING_FILE_SCORE, ROOK_ON_KING_RANK_SCORE, ROOK_ON_OPEN_SCORE,
ROOK_ON_QUEEN_LINE_SCORE, ROOK_STACKED_SCORE, ROOK_ON_SEVENTH_SCORE;
extern Score ROOK_ON_BLOCKED_PENALTY, ROOK_TRAPPED_BY_KING_PENALTY;
//queen
extern Score QUEEN_PINNED_PENALTY;
//king 
extern Score UNSAFE_CHECKS_PENALTY;
extern Score SAFE_CHECKS_PENALTY[2][NUM_PIECE_TYPES];
extern Score PAWN_STORM_BLOCKED_FILE_PENALTY[NUM_FILES];
extern Score PAWN_STORM_UNBLOCKED_FILE_PENALTY[NUM_FILES];
extern Score KING_PAWN_DISTANCE_SCORE[7];
extern Score KING_ON_OPEN_FILE_PENALTY;
extern Score KING_AREA_WEAK_SQUARE_PENALTY;
extern Score KING_NO_ENEMY_QUEEN_SCORE;
//threats
extern Score THREAT_MINOR_SCORE[NUM_PIECE_TYPES];
extern Score THREAT_ROOK_SCORE[NUM_PIECE_TYPES];
extern Score THREAT_KING_SCORE;
extern Score THREAT_HANGING_PIECE_SCORE;
extern Score THREAT_CONTROLLED_SQUARE_SCORE;
extern Score THREAT_SAFE_PAWN_ATTACK;
extern Score THREAT_PAWN_PUSH_ATTACK;

inline Score operator+(Score a, Score b) { return Score((a.middle_game + b.middle_game),(a.end_game + b.end_game));}
inline Score operator-(Score a, Score b) { return Score((a.middle_game - b.middle_game),(a.end_game - b.end_game));}
inline Score operator*(int a, Score b) { return Score((a * b.middle_game),(a * b.end_game));}
inline Score operator*(Score b, int a) { return Score((a * b.middle_game),(a * b.end_game));}
inline Score operator/(Score b, int a) { return Score((b.middle_game / a),(b.end_game / a));}
inline Score operator/(int a, Score b) { return Score((b.middle_game / a),(b.end_game / a));}

inline Score& operator+=(Score& a, Score b) { return a = Score((a.middle_game + b.middle_game),(a.end_game + b.end_game));}
inline Score& operator-=(Score& a, Score b) { return a = Score((a.middle_game - b.middle_game),(a.end_game - b.end_game));}


inline bool operator==(const Score& a, const Score& b) { return a.middle_game == b.middle_game;}
inline bool operator>(const Score& a, const Score& b) { return a.middle_game > b.middle_game;}
inline bool operator<(const Score& a, const Score& b) { return a.middle_game < b.middle_game;}

std::ostream& operator<<(std::ostream& os, const Score& s);
#endif
