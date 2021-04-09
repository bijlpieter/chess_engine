#include "types.h"
#include "move_generation.h"

Phase Position::calculate_phase(){
    Phase total = 24;
    for (Color c : {WHITE,BLACK}){
        total -= popcount(pieces[c][1]) + popcount(pieces[c][2]) + popcount(pieces[c][3]) * 2 + popcount(pieces[c][4]) * 4;
    }
    return (total * 256 + 12) / 12;
}

bool Position::is_outpost(Color c, Square s) {
    //on outpost square
    if (!s & OUTPOSTS[c]){
        return false;
    }
    //defended by pawn;
    if (!PAWN_ATTACKS[~c][s] & pieces[c][PAWN]){
        return false;    
    }
    //no kicker nickers
    Direction h_direction = (c == WHITE) ? UP : DOWN;
    int steps = (c == WHITE) ? (7 - rank(s)) : rank(s);
    if (file(s) & ~FILE_H){
        if (bb_ray(s + RIGHT, s + (RIGHT + (h_direction * steps))) & pieces[~c][PAWN]){
            return false;
        }
    }
    if (file(s) & ~FILE_A){
        if (bb_ray(s + LEFT, s + (LEFT + (h_direction * steps))) & pieces[~c][PAWN]){
            return false;
        }
    }
    return true;
}
Score Position::knight_score(Color c, Bitboard enemy_pawn_control, Bitboard defended_squares){
    if (popcount(pieces[c][KNIGHT]) == 0){
        return Score(0,0);
    }
    Bitboard knights = pieces[c][KNIGHT];
    int total_pawns = popcount(pieces[WHITE][PAWN]) + popcount (pieces[BLACK][PAWN]);
    //knight: less value the less pawns on the board
    float material_value = material_score[KNIGHT] * popcount(pieces[c][KNIGHT]) - KNIGHT_PAWN_MODIFIER * (16 - total_pawns);
    //bonus for defended
    float defended_value = popcount(knights & defended_squares) * KNIGHT_DEFENDED_MODIFIER;
     //outpost is on ranks 4-7 respectively, with no pawns to kick bonus
    float outpost_value;
    while (knights){
        if (is_outpost(c, pop_lsb(knights))){
            outpost_value++;
        }
    }
    outpost_value *= KNIGHT_OUTPOST_MODIFIER;
    //mobility
    float temp_mobility = popcount(legal_knight_moves() & ~enemy_pawn_control);
    Score mobility = Score(temp_mobility *mobility_middle[KNIGHT], temp_mobility * mobility_end[KNIGHT]);
    float total = material_value + defended_value + outpost_value;
    return Score(total,total) + mobility;
}
Score Position::bishop_score(Color c, Bitboard enemy_pawn_control, Bitboard defended_squares){
    if (popcount(pieces[c][BISHOP]) == 0){
        return Score(0,0);
    }
    Bitboard bishops = pieces[c][BISHOP];
    //bishop: static value;
    float material_value = material_score[BISHOP] * popcount(pieces[c][BISHOP]);
    //complementing bishops on the board
    float pair_value = 0;
    if (bishops & LIGHT_SQUARES && bishops & DARK_SQUARES) pair_value = BISHOP_PAIR_MODIFIER;
    //bonus for defended
    float defended_value = popcount(bishops & defended_squares) * BISHOP_DEFENDED_MODIFIER;
     //outpost is on ranks 4-7 respectively, with no pawns to kick
    float outpost_value;
    while (bishops){
        if (is_outpost(c, pop_lsb(bishops))){
            outpost_value++;
        }
    }
    outpost_value *= BISHOP_OUTPOST_MODIFIER;
    //mobility (- enemy pawn controlled)
    float temp_mobility = popcount(legal_bishop_moves() & ~enemy_pawn_control);
    Score mobility = Score(temp_mobility * mobility_middle[BISHOP], temp_mobility * mobility_end[BISHOP]);
    float total = material_value + defended_value + outpost_value + pair_value;
    return Score(total,total) + mobility;
}

Score Position::calculate_material(Color c){
    Direction left = c == WHITE ? DOWN_LEFT : UP_LEFT;
    Direction right = c == WHITE ? DOWN_RIGHT : UP_RIGHT;
    Bitboard enemy_pawn_control = (shift(pieces[~c][PAWN], left) | shift(pieces[~c][PAWN], right));
    Bitboard defended_squares = controlling(c, all_pieces);
    
    Score total(0,0);
    total += knight_score(c, enemy_pawn_control, defended_squares);
    total += bishop_score(c, enemy_pawn_control, defended_squares);
   
    return total;
}