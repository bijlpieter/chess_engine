#include "types.h"
#include "move_generation.h"
#include <iostream>

#define empty_fen "4k3/8/8/8/8/8/8/4K3 w KQkq - 0 1"
Position pos_fen(std::string fen){
    PositionInfo* info = new PositionInfo();
    Position pos = Position(info, fen);
    pos.eval_init();
    return pos;
}
void init(){
    bb_init();
    bb_moves_init();
    bb_rays_init();
}
void test_calculate_material(){
    std::string r = "";
    Score empty_score(0,0);
    std::cout << "------------------OUTPOSTS------------------" << std::endl;
    //c4 square = 26, c3 =18, c8 = 58, c7 = 50
    Position fake_1 = pos_fen("4k3/1p6/8/8/2N5/3P4/8/4K3 w KQkq - 0 1"); //c4 //leftkicker
    Position fake_2 = pos_fen("4k3/3p4/8/8/2N5/3P4/8/4K3 w KQkq - 0 1"); //c4 //rightkicker
    Position fake_3 = pos_fen("4k3/8/8/8/1PNP4/2P5/8/4K3 w KQkq - 0 1"); //c4 //not defended by pawn
    Position fake_4 = pos_fen("4k3/1p6/1P6/8/8/2N5/3P4/4K3 w KQkq - 0 1"); //c3 //not inside outpost
    Position fake_5 = pos_fen("2N2k2/3P4/8/8/8/8/8/4K3 w KQkq - 0 1"); // c8 //not inside outpost
    Position real_1 = pos_fen("4k3/8/8/8/2N5/3P4/8/4K3 w KQkq - 0 1"); //c4 //clean outpost
    Position real_2 = pos_fen("4k3/8/8/8/1pNp4/3P4/8/4K3 w KQkq - 0 1"); // c4 2 black pawns beside horse
    Position real_3 = pos_fen("4k3/1pNp4/3P4/8/8/8/8/4K3 w KQkq - 0 1"); //c7 2 black pawns beside horse
    Square c3 = (Square)18, c4 = (Square)26, c7 = (Square)50, c8 = (Square)58;
    if (!fake_1.is_outpost(WHITE,c4) && !fake_2.is_outpost(WHITE,c4) && !fake_3.is_outpost(WHITE,c4) && !fake_4.is_outpost(WHITE,c3) && !fake_5.is_outpost(WHITE,c8)){
        r = "PASSED 5/5\n";
    }
    else{
        r = "FAILED\n";
    }
    std::cout << "[Dectecting fake outposts] " + r;
    if (real_1.is_outpost(WHITE,c4) && real_2.is_outpost(WHITE,c4) && real_3.is_outpost(WHITE,c7)){
        r = "PASSED 3/3\n";
    }
    else{
        r = "FAILED\n";
    }
    std::cout << "[Dectecting real outposts] " + r;
    std::cout << "------------------MOBILITY------------------" << std::endl;
    Bitboard all_mobility = ~0;
    Position only_king = pos_fen("4k3/8/8/8/8/8/8/4K3 w KQkq - 0 1");
    Position king_queen = pos_fen("4k3/8/8/8/8/8/8/3QK3 w KQkq - 0 1");
    Position king_and_enemy_pawns = pos_fen("4k3/8/8/3p4/8/8/8/4K3 w KQkq - 0 1");
    Position king_and_unpinned = pos_fen("4k3/8/8/5bnr/5BNR/8/PPPPPPPP/3K4 w KQkq - 0 1");
    Position king_and_pinned = pos_fen("4k3/8/8/b7/8/8/3R4/4K3 w KQkq - 0 1");
    r = (popcount(all_mobility) > popcount(only_king.info.mobility[WHITE])) ? "PASSED\n" : "FAILED\n";
    std::cout << "[Friendly King restricts mobility] " + r;
    r = (popcount(only_king.info.mobility[WHITE]) > popcount(king_queen.info.mobility[WHITE])) ? "PASSED\n" : "FAILED\n";
    std::cout << "[Friendly Queen restricts mobility] " + r;
    r = (popcount(only_king.info.mobility[WHITE]) > popcount(king_and_enemy_pawns.info.mobility[WHITE])) ? "PASSED\n" : "FAILED\n";
    std::cout << "[Enemy pawns restricts mobility(by controlling squares)]  " + r;
    r = (popcount(only_king.info.mobility[WHITE]) == popcount(king_and_unpinned.info.mobility[WHITE])) ? "PASSED\n" : "FAILED\n";
    std::cout << "[Unpinned pieces(excluding queens/kings & enemy pawns) do not hinder mobility] " + r;
    r = (popcount(only_king.info.mobility[WHITE]) > popcount(king_and_pinned.info.mobility[WHITE])) ? "PASSED\n" : "FAILED\n";
    std::cout << "[Pinned pieces hinder mobility] " + r;

    //knights
    std::cout << "------------------KNIGHTS------------------" << std::endl;
    Position white_knight_c4 = pos_fen("4k3/8/8/8/2N5/8/8/4K3 w KQkq - 0 1");
    Position black_knight_c5 = pos_fen("4k3/8/8/2n5/8/8/8/4K3 w KQkq - 0 1");
    Position white_knight_c4_outpost = pos_fen("4k3/8/8/8/2N5/3P4/8/4K3 w KQkq - 0 1");
    Position white_knight_c4_shielded = pos_fen("4k3/8/8/2P5/2N5/8/8/4K3 w KQkq - 0 1");
    Position white_knight_c4_close_to_king = pos_fen("4k3/8/8/8/2N5/2K5/8/8 w KQkq - 0 1");
    Position white_knight_c4_defended = pos_fen("4k3/8/8/8/2N5/8/8/4KB2 w KQkq - 0 1");
    Score white_knight = white_knight_c4.knight_score(WHITE);
    Score black_knight = black_knight_c5.knight_score(BLACK);
    Score white_outpost_knight = white_knight_c4_outpost.knight_score(WHITE);
    Score white_shielded_knight = white_knight_c4_shielded.knight_score(WHITE);
    Score white_close_knight = white_knight_c4_close_to_king.knight_score(WHITE);
    Score white_knight_defended = white_knight_c4_defended.knight_score(WHITE);
    r = (white_knight > empty_score) ? "PASSED\n" : "FAILED\n";
    std::cout << "[White Knight > empty] " + r;
    r = (black_knight > empty_score) ? "PASSED\n" : "FAILED\n";
    std::cout << "[Black Knight > empty] " + r;
    r = (white_knight == black_knight) ? "PASSED\n" : "FAILED\n"; 
    std::cout << "[White == Black] " + r;
    r = (white_outpost_knight > white_knight) ? "PASSED\n" : "FAILED\n";
    std::cout << "[Outpost > No Outpost] " + r;
    r = (white_knight_defended > white_knight) ? "PASSED\n" : "FAILED\n";
    std::cout << "[Defended > Not defended] " + r;
    r = (white_shielded_knight > white_knight) ? "PASSED\n" : "FAILED\n";
    std::cout << "[Shielded > Not Shielded] " + r;
    r = (white_close_knight > white_knight) ? "PASSED\n" : "FAILED\n";
    std::cout << "[Knight closer to king > Knight farther from king] " + r;
    //bishops
    std::cout << "------------------BISHOPS------------------" << std::endl;
    Position white_bishop_c4 = pos_fen("4k3/8/8/8/2B5/8/8/4K3 w KQkq - 0 1");
    Position black_bishop_c5 = pos_fen("4k3/8/8/2b5/8/8/8/4K3 w KQkq - 0 1");
    Position white_bishop_c4_outpost = pos_fen("4k3/8/8/8/2B5/3P4/8/4K3 w KQkq - 0 1");
    Position white_bishop_c4_shielded = pos_fen("4k3/8/8/2P5/2B5/8/8/4K3 w KQkq - 0 1");
    Position white_bishop_c4_close_to_king = pos_fen("4k3/8/8/8/2B5/2K5/8/8 w KQkq - 0 1");
    Position white_bishop_c4_defended = pos_fen("4k3/8/8/8/2B5/4N3/8/4K3 w KQkq - 0 1");
    //complementing bishops (same mobility)
    Position white_bishop_same_col = pos_fen("4k1B1/8/8/8/8/8/8/1B2K3 w KQkq - 0 1");
    Position white_bishop_dif_col = pos_fen("1B2k3/8/8/8/8/8/8/1B2K3 w KQkq - 0 1");
    //xraying enemy pawns. Note that mobility is only restricted for other pieces in terms of pawn attacks not pawns themselves
    Position white_bishop_c4_xray_enemy_pawn = pos_fen("4k3/5p2/8/8/2B5/8/8/4K3 w KQkq - 0 1");
    Position white_bishop_c4_xray_enemy_pawns = pos_fen("4k1p1/5p2/4p3/8/2B5/8/8/4K3 w KQkq - 0 1");

    Score white_bishop = white_bishop_c4.bishop_score(WHITE);
    Score black_bishop = black_bishop_c5.bishop_score(BLACK);
    Score white_outpost_bishop = white_bishop_c4_outpost.bishop_score(WHITE);
    Score white_shielded_bishop = white_bishop_c4_shielded.bishop_score(WHITE);
    Score white_close_bishop = white_bishop_c4_close_to_king.bishop_score(WHITE);
    Score white_bishop_defended = white_bishop_c4_defended.bishop_score(WHITE);

    Score white_bishop_notcomplementing = white_bishop_same_col.bishop_score(WHITE);
    Score white_bishop_complementing = white_bishop_dif_col.bishop_score(WHITE);

    Score white_bishop_xray_single = white_bishop_c4_xray_enemy_pawn.bishop_score(WHITE);
    Score white_bishop_xray_multiple = white_bishop_c4_xray_enemy_pawns.bishop_score(WHITE);

    r = (white_bishop > empty_score) ? "PASSED\n" : "FAILED\n";
    std::cout << "[White Bishop > empty] " + r;
    r = (black_bishop > empty_score) ? "PASSED\n" : "FAILED\n";
    std::cout << "[Black Bishop > empty] " + r;
    r = (white_bishop == black_bishop) ? "PASSED\n" : "FAILED\n"; 
    std::cout << "[White == Black] " + r;
    r = (white_bishop_defended > white_bishop) ? "PASSED\n" : "FAILED\n";
    std::cout << "[Defended > Not defended] " + r;
    r = (white_outpost_bishop > white_bishop) ? "PASSED\n" : "FAILED\n";
    std::cout << "[Outpost > No Outpost] " + r;
    r = (white_shielded_bishop > white_bishop) ? "PASSED\n" : "FAILED\n";
    std::cout << "[Shielded > Not Shielded] " + r;
    r = (white_close_bishop > white_bishop) ? "PASSED\n" : "FAILED\n";
    std::cout << "[Bishop closer to king > bishop farther from king] " + r;
    r = (white_bishop_complementing > white_bishop_notcomplementing) ? "PASSED\n" : "FAILED\n";
    std::cout << "[Complementing bishops > not complementing bishops (same mobility)] " + r;
    r = (white_bishop_xray_single < white_bishop) ? "PASSED\n" : "FAILED\n";
    std::cout << "[Xraying enemy pawn < not] " + r;
    r = (white_bishop_xray_multiple < white_bishop_xray_single) ? "PASSED\n" : "FAILED\n";
    std::cout << "[Xraying more enemy pawns < Xraying less enemy pawns] " + r;
    


} 

int main(){
    //test_fen += " w KQkq - 0 1";
    init();
    test_calculate_material();
    return 0;
}