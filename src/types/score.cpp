#include "score.h"
//PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING,
Score material_scores[NUM_PIECE_TYPES];
Score mobility_scores[NUM_PIECE_TYPES];
//pawn
Score PAWN_PASSED_RANK[NUM_RANKS];
Score PAWN_CONNECTED_SCORE;
Score PAWN_SUPPORTED_SCORE;
Score PAWN_ADVANCED_BLOCK_SCORE;
Score PAWN_KING_PROX_SCORE;
Score PAWN_PASSED_UNCONTESTED_SCORE;
Score PAWN_PASSED_CONTESTED_SUPPORTED_SCORE;
Score PAWN_PASSED_QUEENPATH_UNCONTESTED_SCORE;
Score PAWN_PASSED_NEXTSQUARE_UNCONTESTED_SCORE;
Score PAWN_PASSED_NEXTSQUARE_DEFENDED_SCORE;
Score PAWN_PASSED_EDGE_DIST_SCORE;
Score PAWN_SPACE_SCORE;
Score PAWN_EARLY_DOUBLE_PENALTY;
Score PAWN_DOUBLED_PENALTY;
Score PAWN_ISOLATED_PENALTY;
Score PAWN_BACKWARDS_PENALTY;
Score PAWN_WEAK_LEVER_PENALTY;
//bishop
Score BISHOP_PAIR_SCORE;
Score BISHOP_DEFENDED_SCORE;
Score BISHOP_OUTPOST_SCORE;
Score BISHOP_FIANCHETTO_SCORE;
Score BISHOP_KING_DISTANCE_PENALTY;
Score BISHOP_XRAY_PAWN_PENALTY;
Score BISHOP_SHIELDED_SCORE;
Score BISHOP_ATTACKING_KING_SCORE;
//knights
Score KNIGHT_DEFENDED_SCORE;
Score KNIGHT_OUTPOST_SCORE;
Score KNIGHT_KING_DISTANCE_PENALTY;
Score KNIGHT_SHIELDED_SCORE;
//rook
Score ROOK_ON_KING_FILE_SCORE;
Score ROOK_ON_KING_RANK_SCORE;
Score ROOK_ON_QUEEN_LINE_SCORE;
Score ROOK_ON_OPEN_SCORE;
Score ROOK_STACKED_SCORE;
Score ROOK_ON_SEVENTH_SCORE;
Score ROOK_ON_BLOCKED_PENALTY;
Score ROOK_TRAPPED_BY_KING_PENALTY;
//queen
Score QUEEN_PINNED_PENALTY;
//king
Score PAWN_STORM_BLOCKED_FILE_PENALTY[NUM_FILES];
Score PAWN_STORM_UNBLOCKED_FILE_PENALTY[NUM_FILES];
Score KING_PAWN_DISTANCE_SCORE[7];
Score KING_ON_OPEN_FILE_PENALTY;
Score KING_AREA_WEAK_SQUARE_PENALTY;
Score KING_NO_ENEMY_QUEEN_SCORE;
Score UNSAFE_CHECKS_PENALTY;
//0 for single checks 1 for multiple checks
Score SAFE_CHECKS_PENALTY[2][NUM_PIECE_TYPES];
//Threats
Score THREAT_MINOR_SCORE[NUM_PIECE_TYPES];
Score THREAT_ROOK_SCORE[NUM_PIECE_TYPES];
Score THREAT_KING_SCORE;
Score THREAT_HANGING_PIECE_SCORE;
Score THREAT_CONTROLLED_SQUARE_SCORE;
Score THREAT_SAFE_PAWN_ATTACK;
Score THREAT_PAWN_PUSH_ATTACK;
//Initiative
Score INITIATIVE_PASSED_PAWN_SCORE;
Score INITIATIVE_PAWN_COUNT_SCORE;
Score INITIATIVE_OUTFLANKING_SCORE;
Score INITIATIVE_FLANK_PAWNS_SCORE;
Score INITIATIVE_INFILTRATION_SCORE;
Score INITIATIVE_DRAWN_SCORE;
Score INITIATIVE_BALANCING_SCORE;

std::vector<Score> get_score_vector(std::string filename){
    std::string fp = "src/evaluation/" + filename;
    std::vector<Score> scores;
    std::ifstream input(fp);
    std::string s;
    while (std::getline(input, s)){
        if (s == ""){
            continue;
        }
        //bla bla 1 [10,10]
        int comma = s.find(",");
        int open = s.find("[");
        int close = s.find("]");
        int middle_game = std::stoi(s.substr(open + 1, comma - 1));
        int end_game = std::stoi(s.substr(comma + 1, close - 1));
        Score s = Score(middle_game, end_game);
        scores.push_back(s);
    }
    return scores;
}
void write_scores(std::vector<Score> scores, std::string filename){
    std::string fp = "src/evaluation/" + filename;
    std::string empty = "src/evaluation/empty.txt";
    std::ifstream f(empty.c_str());
    if (!f.good()){
        std::cout << "empty.txt missing. Unable to write new scores file.";
        return;
    }
    std::ifstream e(empty);
    std::string s;
    std::ofstream output(fp);
    unsigned int index = 0;
    while(std::getline(e, s)){
        if (s == ""){
            output << "\n";
        }
        else{
            output << (s + " [" + std::to_string(scores.at(index).middle_game) + "," + std::to_string(scores.at(index).end_game) + "]\n");
            index++;
        }
    }
    //scores.size() must be equal to index, which is the amount of values in empty.txt
    assert(index == scores.size());
}

void score_init(std::string filename){
    std::string fp = "src/evaluation/" + filename; 
    std::ifstream f(fp.c_str());
    if (!f.good()){
        //create filename(... .txt) and set the values to default.txt
        std::vector<Score> default_scores = get_score_vector("default.txt");
        write_scores(default_scores, filename);
    }
    std::vector<Score> scores = get_score_vector(filename);
    unsigned int i = 0;
    for (PieceType p : {PAWN,KNIGHT,BISHOP,ROOK,QUEEN,KING}){
        material_scores[p] = scores.at(i++);
    }
    for (PieceType p : {PAWN,KNIGHT,BISHOP,ROOK,QUEEN,KING}){
        mobility_scores[p] = scores.at(i++);
    }
    for(Rank r = RANK_1; r <= RANK_8; r++){
        PAWN_PASSED_RANK[r] = scores.at(i++);
    }
    PAWN_CONNECTED_SCORE = scores.at(i++);
    PAWN_SUPPORTED_SCORE = scores.at(i++);
    PAWN_ADVANCED_BLOCK_SCORE = scores.at(i++);
    PAWN_KING_PROX_SCORE = scores.at(i++);
    PAWN_PASSED_UNCONTESTED_SCORE = scores.at(i++);
    PAWN_PASSED_CONTESTED_SUPPORTED_SCORE = scores.at(i++);
    PAWN_PASSED_QUEENPATH_UNCONTESTED_SCORE = scores.at(i++);
    PAWN_PASSED_NEXTSQUARE_UNCONTESTED_SCORE = scores.at(i++);
    PAWN_PASSED_NEXTSQUARE_DEFENDED_SCORE = scores.at(i++);
    PAWN_PASSED_EDGE_DIST_SCORE = scores.at(i++);
    PAWN_SPACE_SCORE = scores.at(i++);

    PAWN_EARLY_DOUBLE_PENALTY = scores.at(i++);
    PAWN_DOUBLED_PENALTY = scores.at(i++);
    PAWN_ISOLATED_PENALTY = scores.at(i++);
    PAWN_BACKWARDS_PENALTY = scores.at(i++);
    PAWN_WEAK_LEVER_PENALTY = scores.at(i++);

    BISHOP_PAIR_SCORE = scores.at(i++);
    BISHOP_DEFENDED_SCORE = scores.at(i++);
    BISHOP_OUTPOST_SCORE = scores.at(i++);
    BISHOP_FIANCHETTO_SCORE = scores.at(i++);
    BISHOP_KING_DISTANCE_PENALTY = scores.at(i++);
    BISHOP_XRAY_PAWN_PENALTY = scores.at(i++);
    BISHOP_SHIELDED_SCORE = scores.at(i++);
    BISHOP_ATTACKING_KING_SCORE = scores.at(i++);

    KNIGHT_DEFENDED_SCORE = scores.at(i++);
    KNIGHT_OUTPOST_SCORE = scores.at(i++);
    KNIGHT_KING_DISTANCE_PENALTY = scores.at(i++);
    KNIGHT_SHIELDED_SCORE = scores.at(i++);

    ROOK_ON_KING_FILE_SCORE = scores.at(i++);
    ROOK_ON_KING_RANK_SCORE = scores.at(i++);
    ROOK_ON_QUEEN_LINE_SCORE = scores.at(i++);
    ROOK_ON_OPEN_SCORE = scores.at(i++);
    ROOK_STACKED_SCORE = scores.at(i++);
    ROOK_ON_SEVENTH_SCORE = scores.at(i++);
    ROOK_ON_BLOCKED_PENALTY = scores.at(i++);
    ROOK_TRAPPED_BY_KING_PENALTY = scores.at(i++);

    QUEEN_PINNED_PENALTY = scores.at(i++);
    for (File f = FILE_A; f <= FILE_H; f++){
        PAWN_STORM_BLOCKED_FILE_PENALTY[f] = scores.at(i++);
    } 
    for (File f = FILE_A; f <= FILE_H; f++){
        PAWN_STORM_UNBLOCKED_FILE_PENALTY[f] = scores.at(i++);
    }
    for (int d = 0; d < 7; d++){
        KING_PAWN_DISTANCE_SCORE[d] = scores.at(i++);
    }
    KING_ON_OPEN_FILE_PENALTY = scores.at(i++);
    KING_AREA_WEAK_SQUARE_PENALTY = scores.at(i++);
    KING_NO_ENEMY_QUEEN_SCORE = scores.at(i++);
    UNSAFE_CHECKS_PENALTY = scores.at(i++);
    for(int d = 0; d < 2; d++){
        for (PieceType p : {PAWN,KNIGHT,BISHOP,ROOK,QUEEN,KING}){
         SAFE_CHECKS_PENALTY[d][p] = scores.at(i++);
        }
    }
    for (PieceType p : {PAWN,KNIGHT,BISHOP,ROOK,QUEEN,KING}){
        THREAT_MINOR_SCORE[p] = scores.at(i++);
    }
    for (PieceType p : {PAWN,KNIGHT,BISHOP,ROOK,QUEEN,KING}){
        THREAT_ROOK_SCORE[p] = scores.at(i++);
    }
    THREAT_KING_SCORE = scores.at(i++);
    THREAT_HANGING_PIECE_SCORE = scores.at(i++);
    THREAT_CONTROLLED_SQUARE_SCORE = scores.at(i++);
    THREAT_SAFE_PAWN_ATTACK = scores.at(i++);
    THREAT_PAWN_PUSH_ATTACK = scores.at(i++);
    INITIATIVE_PASSED_PAWN_SCORE = scores.at(i++);
    INITIATIVE_PAWN_COUNT_SCORE = scores.at(i++);
    INITIATIVE_OUTFLANKING_SCORE = scores.at(i++);
    INITIATIVE_FLANK_PAWNS_SCORE = scores.at(i++);
    INITIATIVE_INFILTRATION_SCORE = scores.at(i++);
    INITIATIVE_DRAWN_SCORE = scores.at(i++);
    INITIATIVE_BALANCING_SCORE = scores.at(i++);
    assert(i == scores.size());
}

std::ostream& operator<<(std::ostream& os, const Score& s) {
    os << "[" << s.middle_game << "," << s.end_game << "]";
	return os;
}
