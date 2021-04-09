#include "types.h"

Phase Position::calculate_phase(){
    Phase total = 24;
    for (Color c : {WHITE,BLACK}){
        total -= popcount(pieces[c][1]) + popcount(pieces[c][2]) + popcount(pieces[c][3]) * 2 + popcount(pieces[c][4]) * 4;
    }
    return (total * 256 + 12) / 12;
}
// Score calculate_mobility(Color c){
//     Score total(0,0);
//     for (PieceType p : AllPieceTypes){
//         if (p == PAWN){
//             float m = popcount(legal_moves_of_piece(c, p)) * mobility_middle[p];
//             float e = popcount(legal_moves_of_piece(c, p)) * mobility_end[p];
//             total += Score(m,e);
//         }
//         else{
//             //all moves without moves controlled by enemy pawns
//             float m = popcount(legal_moves_of_piece(c,p) & ~legal_moves_of_piece(~c,PAWN)) * mobility_middle[p];
//             float e = popcount(legal_moves_of_piece(c,p) & ~legal_moves_of_piece(~c,PAWN)) * mobility_end[p];
//             total += Score(m,e);
//         }
//     }
//     return total;
// }
// Score calculate_material(Color c){
//     Bitboard controlled_squares = controlling(c);
//     int total_pawns = popcount(pieces[WHITE][PAWN]) + popcount (pieces[BLACK][PAWN]);
//     Score total(0,0);
//     //knight: less value the less pawns on the board
//     Bitboard knights = pieces[c][KNIGHT];
//     float knight_score = material_score[KNIGHT] * knights.size() - KNIGHT_PAWN_MODIFIER * (16 - total_pawns);
//     float defended_bonus = popcount(knights & controlled_squares) * KNIGHT_DEFENDED_MODIFIER;
//     float outpost_bonus
//     total += Score(knight_score, knight_score);
   
//     return total;
// }
// Score Position::calculate_score(Color c){
//     Color opposite = ~c;
//     Score total(0,0);
//     Score material = calculate_material(c) - calculate_material(opposite);
//     Score mobility = calculate_mobility(c) - calculate_mobility(opposite);
//     //pawns
//     //knights
//     //bishops
//     //rook
//     //queen
//     //DA KING
//     return 
// }