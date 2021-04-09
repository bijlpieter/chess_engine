#include "types.h"
#include "move_generation.h"

Phase Position::calculate_phase(){
    Phase total = 24;
    for (Color c : {WHITE,BLACK}){
        total -= popcount(pieces[c][1]) + popcount(pieces[c][2]) + popcount(pieces[c][3]) * 2 + popcount(pieces[c][4]) * 4;
    }
    return (total * 256 + 12) / 12;
}

Score calculate_mobility(Color c) {
    Score total(0,0);
    for (PieceType p : AllPieceTypes){
        if (p == PAWN){
            float m = popcount(legal_moves_of_piece(c, p)) * mobility_middle[p];
            float e = popcount(legal_moves_of_piece(c, p)) * mobility_end[p];
            total += Score(m,e);
        }
        else{
            //all moves without moves controlled by enemy pawns
            float m = popcount(legal_moves_of_piece(c,p) & ~legal_moves_of_piece(~c,PAWN)) * mobility_middle[p];
            float e = popcount(legal_moves_of_piece(c,p) & ~legal_moves_of_piece(~c,PAWN)) * mobility_end[p];
            total += Score(m,e);
        }
    }
    return total;
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
Score Position::knight_score(Color c, Bitboard enemyPawnControl){
    //knight: less value the less pawns on the board
    int total_pawns = popcount(pieces[WHITE][PAWN]) + popcount (pieces[BLACK][PAWN]);
    Bitboard knights = pieces[c][KNIGHT];
    Score mobility = Score(popcount(current_legal_moves(KNIGHT)  & ~enemyPawnControl) * mobility_mg[KNIGHT],popcount(current_legal_moves(KNIGHT)  & ~enemyPawnControl) * mobility_eg[KNIGHT]);
    float material_value = material_score[KNIGHT] * popcount(pieces[c][KNIGHT] - KNIGHT_PAWN_MODIFIER * (16 - total_pawns);
    float defended_value = popcount(knights & controlled_squares) * KNIGHT_DEFENDED_MODIFIER;
     //outpost is on ranks 4-7 respectively, with no pawns to kick
    float outpost_value;
    while (knights){
        if (is_outpost(c, pop_lsb(knights))){
            outpost_value++;
        }
    }
    float total = material_value + mobility_value + defended_value + outpost_value;
    return Score(total,total) + mobility;
}

Score Position::calculate_material(Color c){
    Direction enemyPawnAttacks = (c == WHITE) ? {DOWN_LEFT, DOWN_RIGHT} : {UP_LEFT,UP_RIGHT};
    Bitboard enemyPawnControl = (shift(pieces[~c][PAWN],enemyPawnAttacks[0]) | shift(pieces[~c][PAWN],enemyPawnAttacks[1]));
    
    Score total(0,0);
    total += knight_score(c,enemyPawnControl);
   
    return total;
}

Score Position::calculate_score(Color c){
    Color opposite = ~c;
    Score total(0,0);
    Score material = calculate_material(c) - calculate_material(opposite);
    Score mobility = calculate_mobility(c) - calculate_mobility(opposite);
    //pawns
    //knights
    //bishops
    //rook
    //queen
    //DA KING
    return 
}