#include "score.h"
#define S(a,b)  Score(a,b)
//PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING,
Score mobility_scores[NUM_PIECE_TYPES] = {S(5, 5), S(5, 5), S(5, 5), S(5, 5), S(5, 5), S(5, 5)};
Score material_scores[NUM_PIECE_TYPES] = {S(100, 100), S(350, 350),S(350, 350), S(525, 525), S(1000, 1000), S(100000,100000)};

//bishop
Score BISHOP_PAIR_SCORE = S(50, 50);
Score BISHOP_DEFENDED_SCORE = S(10, 10);
Score BISHOP_OUTPOST_SCORE = S(20, 20);
Score BISHOP_FIANCHETTO_SCORE = S(20,0);
Score BISHOP_KING_DISTANCE_PENALTY = S(1, 1);
Score BISHOP_XRAY_PAWN_PENALTY = S(10, 10);
Score BISHOP_SHIELDED_SCORE = S(10, 10);
Score BISHOP_ATTACKING_KING_SCORE = S(10, 10);
//knights
Score KNIGHT_DEFENDED_SCORE = S(10, 10);
Score KNIGHT_OUTPOST_SCORE = S(20, 20);
Score KNIGHT_KING_DISTANCE_PENALTY = S(1, 1);
Score KNIGHT_SHIELDED_SCORE = S(10, 10);
//rook
Score ROOK_ON_KING_FILE_SCORE = S(10, 10);
Score ROOK_ON_KING_RANK_SCORE = S (10, 10);
Score ROOK_ON_QUEEN_LINE_SCORE = S(15, 15);
Score ROOK_ON_OPEN_SCORE = S(20, 5);
Score ROOK_STACKED_SCORE = S (20,20);
Score ROOK_ON_SEVENTH_SCORE = S (100, 20);
Score ROOK_ON_BLOCKED_PENALTY = S(5, 50);
Score ROOK_TRAPPED_BY_KING_PENALTY = S(100, 100);
//queen
Score QUEEN_PINNED_PENALTY = S(30,30);
//king
Score PAWN_STORM_BLOCKED_FILE_PENALTY[NUM_FILES] = {S(0,0), S(0,0), S(30, 30), S (-10,20), S (-10, 20), S(-10, 20), S(0, 10), S(0,0)};
Score PAWN_STORM_UNBLOCKED_FILE_PENALTY[NUM_FILES] = {S(0,0), S(0,0), S(30, 30), S (-10,20), S (-10, 20), S(-10, 20), S(0, 10), S(0,0)};
Score KING_ON_OPEN_FILE_PENALTY = S(50,0);
Score KING_PAWN_DISTANCE_SCORE[7] = {S(0,0), S(0,50) ,S(0,40), S(0,30), S(0,0), S(0,-20), S(0,-50)};
Score KING_AREA_WEAK_SQUARE_PENALTY = S(20,20);
Score KING_NO_ENEMY_QUEEN_SCORE = S(100,100);
//PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING, 0 for single checks 1 for multiple checks
Score UNSAFE_CHECKS_PENALTY = S(50,50);
Score SAFE_CHECKS_PENALTY[2][NUM_PIECE_TYPES] = 
{{S(0,0), S(100,100), S(100,100), S(200,200), S(150,150), S(0,0)},
{S(0,0), S(200,200),S(200,200), S(400,400), S(300,300), S(0,0)}};
//Threats
Score THREAT_MINOR_SCORE[NUM_PIECE_TYPES] = {S(0,0),S(20,40),S(20,40),S(50,70),S(100,100),S(150,150)};
Score THREAT_ROOK_SCORE[NUM_PIECE_TYPES] = {S(0,0),S(10,20),S(10,20),S(30,30),S(120,120),S(150,150)};
Score THREAT_KING_SCORE = S(10,70); 



std::ostream& operator<<(std::ostream& os, const Score& s) {
    os << "[" << s.middle_game << "," << s.end_game << "]";
	return os;
}
