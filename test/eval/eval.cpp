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

bool test_outposts(){
    bool ret = true;
    std::string r = "";
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
        ret = false;
    }
    std::cout << "[Dectecting fake outposts] " + r;
    if (real_1.is_outpost(WHITE,c4) && real_2.is_outpost(WHITE,c4) && real_3.is_outpost(WHITE,c7)){
        r = "PASSED 3/3\n";
    }
    else{
        r = "FAILED\n";
        ret = false;
    }
    std::cout << "[Dectecting real outposts] " + r;
    return ret;
}
bool test_mobility(){
    std::string r = "";
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
    return ((popcount(all_mobility) > popcount(only_king.info.mobility[WHITE])) && 
            popcount(only_king.info.mobility[WHITE]) > popcount(king_queen.info.mobility[WHITE]) &&
            popcount(only_king.info.mobility[WHITE]) > popcount(king_and_enemy_pawns.info.mobility[WHITE]) &&
             popcount(only_king.info.mobility[WHITE]) == popcount(king_and_unpinned.info.mobility[WHITE]) &&
             popcount(only_king.info.mobility[WHITE]) > popcount(king_and_pinned.info.mobility[WHITE]));
}
bool test_knights(){
    Score empty_score(0,0);
    std::string r = "";
    //standard
    Position white_knight_c4 = pos_fen("4k3/8/8/8/2N5/8/8/4K3 w KQkq - 0 1");
    Position black_knight_c5 = pos_fen("4k3/8/8/2n5/8/8/8/4K3 w KQkq - 0 1");
    Score white_knight = white_knight_c4.knight_score(WHITE);
    Score black_knight = black_knight_c5.knight_score(BLACK);
    r = (white_knight > empty_score) ? "PASSED\n" : "FAILED\n";
    std::cout << "[White Knight > empty] " + r;
    r = (black_knight > empty_score) ? "PASSED\n" : "FAILED\n";
    std::cout << "[Black Knight > empty] " + r;
    r = (white_knight == black_knight) ? "PASSED\n" : "FAILED\n"; 
    std::cout << "[White == Black] " + r;
    //outpost/shielded/defended
    Position white_knight_c4_outpost = pos_fen("4k3/8/8/8/2N5/3P4/8/4K3 w KQkq - 0 1");
    Position white_knight_c4_shielded = pos_fen("4k3/8/8/2P5/2N5/8/8/4K3 w KQkq - 0 1");
    Position white_knight_c4_defended = pos_fen("4k3/8/8/8/2N5/8/8/4KB2 w KQkq - 0 1");
    Score white_outpost_knight = white_knight_c4_outpost.knight_score(WHITE);
    Score white_shielded_knight = white_knight_c4_shielded.knight_score(WHITE);
    Score white_knight_defended = white_knight_c4_defended.knight_score(WHITE);
    r = (white_outpost_knight > white_knight) ? "PASSED\n" : "FAILED\n";
    std::cout << "[Outpost > No Outpost] " + r;
    r = (white_knight_defended > white_knight) ? "PASSED\n" : "FAILED\n";
    std::cout << "[Defended > Not defended] " + r;
    r = (white_shielded_knight > white_knight) ? "PASSED\n" : "FAILED\n";
    std::cout << "[Shielded > Not Shielded] " + r;
    //distance from king
    Position white_knight_c4_close_to_king = pos_fen("4k3/8/8/8/2N5/2K5/8/8 w KQkq - 0 1");
    Score white_close_knight = white_knight_c4_close_to_king.knight_score(WHITE);
    r = (white_close_knight > white_knight) ? "PASSED\n" : "FAILED\n";
    std::cout << "[Knight closer to king > Knight farther from king] " + r;

    return (white_knight > empty_score && black_knight > empty_score && white_knight == black_knight && white_outpost_knight > white_knight
    && white_knight_defended > white_knight && white_shielded_knight > white_knight && white_close_knight > white_knight);
}
bool test_bishops(){
    Score empty_score(0,0);
    std::string r = "";
    std::cout << "------------------BISHOPS------------------" << std::endl;
    //standard
    Position white_bishop_c4 = pos_fen("4k3/8/8/8/2B5/8/8/4K3 w KQkq - 0 1");
    Position black_bishop_c5 = pos_fen("4k3/8/8/2b5/8/8/8/4K3 w KQkq - 0 1");
    Score white_bishop = white_bishop_c4.bishop_score(WHITE);
    Score black_bishop = black_bishop_c5.bishop_score(BLACK);
    r = (white_bishop > empty_score) ? "PASSED\n" : "FAILED\n";
    std::cout << "[White Bishop > empty] " + r;
    r = (black_bishop > empty_score) ? "PASSED\n" : "FAILED\n";
    std::cout << "[Black Bishop > empty] " + r;
    r = (white_bishop == black_bishop) ? "PASSED\n" : "FAILED\n"; 
    std::cout << "[White == Black] " + r;
    //outpost/shielded/defended
    Position white_bishop_c4_outpost = pos_fen("4k3/8/8/8/2B5/3P4/8/4K3 w KQkq - 0 1");
    Position white_bishop_c4_shielded = pos_fen("4k3/8/8/2P5/2B5/8/8/4K3 w KQkq - 0 1"); 
    Position white_bishop_c4_defended = pos_fen("4k3/8/8/8/2B5/4N3/8/4K3 w KQkq - 0 1");
    Score white_outpost_bishop = white_bishop_c4_outpost.bishop_score(WHITE);
    Score white_shielded_bishop = white_bishop_c4_shielded.bishop_score(WHITE);
    Score white_bishop_defended = white_bishop_c4_defended.bishop_score(WHITE);
    r = (white_bishop_defended > white_bishop) ? "PASSED\n" : "FAILED\n";
    std::cout << "[Defended > Not defended] " + r;
    r = (white_outpost_bishop > white_bishop) ? "PASSED\n" : "FAILED\n";
    std::cout << "[Outpost > No Outpost] " + r;
    r = (white_shielded_bishop > white_bishop) ? "PASSED\n" : "FAILED\n";
    std::cout << "[Shielded > Not Shielded] " + r;
    //closeness to king
    Position white_bishop_c4_close_to_king = pos_fen("4k3/8/8/8/2B5/2K5/8/8 w KQkq - 0 1");
    Score white_close_bishop = white_bishop_c4_close_to_king.bishop_score(WHITE);
    r = (white_close_bishop > white_bishop) ? "PASSED\n" : "FAILED\n";
    std::cout << "[Bishop closer to king > bishop farther from king] " + r;
    //complementing bishops (same mobility)
    Position white_bishop_same_col = pos_fen("4k1B1/8/8/8/8/8/8/1B2K3 w KQkq - 0 1");
    Position white_bishop_dif_col = pos_fen("1B2k3/8/8/8/8/8/8/1B2K3 w KQkq - 0 1");
    Score white_bishop_notcomplementing = white_bishop_same_col.bishop_score(WHITE);
    Score white_bishop_complementing = white_bishop_dif_col.bishop_score(WHITE);
    r = (white_bishop_complementing > white_bishop_notcomplementing) ? "PASSED\n" : "FAILED\n";
    std::cout << "[Complementing bishops > not complementing bishops (same mobility)] " + r;
    //xraying enemy pawns. Note that mobility is only restricted for other pieces in terms of pawn attacks not pawns themselves
    Position white_bishop_c4_xray_enemy_pawn = pos_fen("4k3/5p2/8/8/2B5/8/8/4K3 w KQkq - 0 1");
    Position white_bishop_c4_xray_enemy_pawns = pos_fen("4k1p1/5p2/4p3/8/2B5/8/8/4K3 w KQkq - 0 1");
    Score white_bishop_xray_single = white_bishop_c4_xray_enemy_pawn.bishop_score(WHITE);
    Score white_bishop_xray_multiple = white_bishop_c4_xray_enemy_pawns.bishop_score(WHITE);
    r = (white_bishop_xray_single < white_bishop) ? "PASSED\n" : "FAILED\n";
    std::cout << "[Xraying enemy pawn < not] " + r;
    r = (white_bishop_xray_multiple < white_bishop_xray_single) ? "PASSED\n" : "FAILED\n";
    std::cout << "[Xraying more enemy pawns < Xraying less enemy pawns] " + r;
    //attacking enemy kingring. Note that blocked attacks by pawns worked by extensive handtest (17/04/2021 00:15) not implemented bc 2much bs
    Position white_bishop_c4_attacking_enemy_king_ring = pos_fen("8/7k/8/8/2B5/8/8/4K3 w KQkq - 0 1");
    Position white_bishop_c4_not_attacking_enemy_king_ring = pos_fen("8/8/8/8/2B4k/8/8/4K3 w KQkq - 0 1");
    Score white_bishop_c4_att = white_bishop_c4_attacking_enemy_king_ring.bishop_score(WHITE);
    Score white_bishop_c4_not_att = white_bishop_c4_not_attacking_enemy_king_ring.bishop_score(WHITE);
    r = (white_bishop_c4_att > white_bishop_c4_not_att) ? "PASSED\n" : "FAILED\n";
    std::cout << "[Bishop attacking enemy king_ring > not] " + r;
    //fianchettoes
    Position bishop_long_line = pos_fen("4k3/8/8/8/8/8/1B6/4K3 w KQkq - 0 1");
    Position bishop_fake_fianchetto = pos_fen("4k3/8/8/8/8/1P6/PB6/4K3 w KQkq - 0 1");
    //have to add shielded score because fake fian is tested with shield
    Score no_fian = bishop_long_line.bishop_score(WHITE) + BISHOP_SHIELDED_SCORE;
    Score fake_fian = bishop_fake_fianchetto.bishop_score(WHITE);
    r = (no_fian == fake_fian) ? "PASSED\n" : "FAILED\n";
    std::cout << "[No fianchetto == fake fianchetto] " + r;
    //white left
    Position white_bishop_b2_fianchetto = pos_fen("4k3/8/8/8/8/1P6/PBP5/4K3 w KQkq - 0 1");
    Position white_bishop_b2_fianchetto_long = pos_fen("4k3/8/8/8/1P6/8/PBP5/4K3 w KQkq - 0 1");
    Score white_bishop_fianb2= white_bishop_b2_fianchetto.bishop_score(WHITE);
    Score white_bishop_fianb2_long= white_bishop_b2_fianchetto_long.bishop_score(WHITE);
    r = (white_bishop_fianb2 > fake_fian) ? "PASSED\n" : "FAILED\n";
    std::cout << "[Standard fianchetto on b2] " + r;
    r = (white_bishop_fianb2_long > fake_fian) ? "PASSED\n" : "FAILED\n";
    std::cout << "[Long fianchetto on b2] " + r;
    //white right
    Position white_bishop_g2_fianchetto = pos_fen("4k3/8/8/8/8/6P1/5PBP/4K3 w KQkq - 0 1");
    Position white_bishop_g2_fianchetto_long = pos_fen("4k3/8/8/8/6P1/8/5PBP/4K3 w KQkq - 0 1");
    Score white_bishop_fiang2= white_bishop_g2_fianchetto.bishop_score(WHITE);
    Score white_bishop_fiang2_long= white_bishop_g2_fianchetto_long.bishop_score(WHITE);
    r = (white_bishop_fiang2 > fake_fian) ? "PASSED\n" : "FAILED\n";
    std::cout << "[Standard fianchetto on g2] " + r;
    r = (white_bishop_fiang2_long > fake_fian) ? "PASSED\n" : "FAILED\n";
    std::cout << "[Long fianchetto on g2] " + r;
    //black left
    Position black_bishop_b7_fianchetto = pos_fen("4k3/pbp5/1p6/8/8/8/8/4K3 w KQkq - 0 1");
    Position black_bishop_b7_fianchetto_long = pos_fen("4k3/pbp5/1p6/8/8/8/8/4K3 w KQkq - 0 1");
    Score black_bishop_fianb7 = black_bishop_b7_fianchetto.bishop_score(BLACK);
    Score black_bishop_fianb7_long = black_bishop_b7_fianchetto_long.bishop_score(BLACK);
    r = (black_bishop_fianb7 > fake_fian) ? "PASSED\n" : "FAILED\n";
    std::cout << "[Standard fianchetto on b7] " + r;
    r = (black_bishop_fianb7_long > fake_fian) ? "PASSED\n" : "FAILED\n";
    std::cout << "[Long fianchetto on b7] " + r;
    //black right
    Position black_bishop_g7_fianchetto = pos_fen("4k3/5pbp/6p1/8/8/8/8/4K3 w KQkq - 0 1");
    Position black_bishop_g7_fianchetto_long = pos_fen("4k3/5pbp/8/6p1/8/8/8/4K3 w KQkq - 0 1");
    Score black_bishop_fiang7 = black_bishop_g7_fianchetto.bishop_score(BLACK);
    Score black_bishop_fiang7_long = black_bishop_g7_fianchetto_long.bishop_score(BLACK);
    r = (black_bishop_fiang7 > fake_fian) ? "PASSED\n" : "FAILED\n";
    std::cout << "[Standard fianchetto on g7] " + r;
    r = (black_bishop_fiang7_long > fake_fian) ? "PASSED\n" : "FAILED\n";
    std::cout << "[Long fianchetto on g7] " + r;
    return (black_bishop_fiang7_long > fake_fian && black_bishop_fiang7 > fake_fian && black_bishop_fianb7_long > fake_fian && black_bishop_fianb7 > fake_fian
    && white_bishop_fiang2_long > fake_fian && white_bishop_fiang2 > fake_fian && white_bishop_fianb2_long > fake_fian && white_bishop_fianb2 > fake_fian &&
    no_fian == fake_fian && white_bishop_c4_att > white_bishop_c4_not_att && white_bishop_xray_multiple < white_bishop_xray_single
    &&white_bishop_xray_single < white_bishop && white_bishop_complementing > white_bishop_notcomplementing && white_close_bishop > white_bishop &&
    white_shielded_bishop > white_bishop && white_outpost_bishop > white_bishop && white_bishop_defended > white_bishop && white_bishop == black_bishop
    && black_bishop > empty_score && white_bishop > empty_score);
}
bool test_rooks(){
    Score empty_score(0,0);
    std::string r;
    std::cout << "------------------ROOKS------------------" << std::endl;
    //standard
    Position white_rook_c4 = pos_fen("4k3/8/8/8/2R5/8/8/4K3 w KQkq - 0 1");
    Position black_rook_c5 = pos_fen("4k3/8/8/2r5/8/8/8/4K3 w KQkq - 0 1");
    Score white_rook = white_rook_c4.rook_score(WHITE);
    Score black_rook = black_rook_c5.rook_score(BLACK);
    r = (white_rook > empty_score) ? "PASSED\n" : "FAILED\n";
    std::cout << "[White rook > empty] " + r;
    r = (black_rook > empty_score) ? "PASSED\n" : "FAILED\n";
    std::cout << "[Black rook > empty] " + r;
    r = (white_rook == black_rook) ? "PASSED\n" : "FAILED\n"; 
    std::cout << "[White == Black] " + r;
    //defending
    Position rook_defending = pos_fen("4k3/8/8/8/RR6/8/8/4K3 w KQkq - 0 1");
    Position rook_not_defending = pos_fen("4k3/8/8/8/RPR5/8/8/4K3 w KQkq - 0 1");
    Score rook_def = rook_defending.rook_score(WHITE);
    Score rook_undef = rook_not_defending.rook_score(WHITE);
    //minor mob diff
    r = (rook_def > rook_undef) ? "PASSED\n" : "FAILED\n"; 
    std::cout << "[Defending rooks > not defending rooks] " + r;
    //rook on files/ranks/lines
    Position rook_king_file = pos_fen("3k4/8/8/8/2R5/8/8/4K3 w KQkq - 0 1");
    Position rook_king_rank = pos_fen("8/8/8/7k/2R5/8/8/4K3 w KQkq - 0 1");
    Position rook_queen_line = pos_fen("4k3/8/8/8/2R4q/8/8/4K3 w KQkq - 0 1");
    Score rook_file = rook_king_file.rook_score(WHITE);
    Score rook_rank = rook_king_rank.rook_score(WHITE);
    Score rook_queen = rook_queen_line.rook_score(WHITE);
    r = (rook_file >white_rook) ? "PASSED\n" : "FAILED\n"; 
    std::cout << "[Rook on King file > not] " + r;
    r = (rook_rank >white_rook) ? "PASSED\n" : "FAILED\n"; 
    std::cout << "[Rook on King rank > not] " + r;
    r = (rook_queen >white_rook) ? "PASSED\n" : "FAILED\n"; 
    std::cout << "[Queen on rook line > not] " + r;
    //open/closed/blocked files.
    Position rook_closed = pos_fen("4k3/8/8/8/8/8/P7/R3K3 w KQkq - 0 1");
    Position rook_open = pos_fen("4k3/8/8/8/8/8/1P6/R3K3 w KQkq - 0 1");
    Position rook_blocked = pos_fen("4k3/8/8/8/8/N7/P7/R3K3 w KQkq - 0 1");
    Score rook_cl = rook_closed.rook_score(WHITE);
    Score rook_op = rook_open.rook_score(WHITE);
    Score rook_bl = rook_blocked.rook_score(WHITE);
    r = (rook_op > rook_cl) ? "PASSED\n" : "FAILED\n"; 
    std::cout << "[Rook on open file > Closed file] " + r;
    r = (rook_bl < rook_cl) ? "PASSED\n" : "FAILED\n"; 
    std::cout << "[Rook on blocked file < Closed unblocked file] " + r;
    //rook on 7th
    Position rook_7th_useless = pos_fen("8/2R5/7k/8/8/8/8/4K3 w KQkq - 0 1");
    Position rook_7th_block_king = pos_fen("7k/2R5/8/8/8/8/8/4K3 w KQkq - 0 1");
    Position rook_7th_enemy_pawn = pos_fen("8/2R4p/7k/8/8/8/8/4K3 w KQkq - 0 1");
    Score rook_7useless = rook_7th_useless.rook_score(WHITE);
    Score rook_7block = rook_7th_block_king.rook_score(WHITE);
    Score rook_7enemypawn = rook_7th_enemy_pawn.rook_score(WHITE);
    r = (rook_7block > rook_7useless) ? "PASSED\n" : "FAILED\n"; 
    std::cout << "[Rook on 7th bonus for cutting enemy king] " + r;
    r = (rook_7enemypawn > rook_7useless) ? "PASSED\n" : "FAILED\n"; 
    std::cout << "[Rook on 7th bonus for attacking enemy pawn] " + r;
    //rooks trapped by kings
    Position rook_free = pos_fen("4k3/8/8/8/8/8/P7/R3K3 w KQkq - 0 1");
    Position rook_trapped_ks = pos_fen("4k3/8/8/8/8/8/7P/5K1R w KQkq - 0 1");
    Position rook_trapped_qs = pos_fen("4k3/8/8/8/8/8/P7/R2K4 w KQkq - 0 1");
    Score rook_fr = rook_free.rook_score(WHITE);
    Score rook_ks = rook_trapped_ks.rook_score(WHITE);
    Score rook_qs = rook_trapped_qs.rook_score(WHITE);
    r = (rook_fr > rook_ks) ? "PASSED\n" : "FAILED\n"; 
    std::cout << "[Rook trapped on king side penalty] " + r;
    r = (rook_fr > rook_qs) ? "PASSED\n" : "FAILED\n"; 
    std::cout << "[Rook trapped on queen side penalty] " + r;


    return ((rook_fr > rook_ks)&& (rook_fr > rook_qs)&& (rook_7block > rook_7useless)&& (rook_7enemypawn > rook_7useless)
    && (rook_op > rook_cl)&& (rook_bl < rook_cl)&& (rook_file >white_rook)&& (rook_rank >white_rook)&& (rook_queen >white_rook)&&
    (rook_def > rook_undef)&& (white_rook > empty_score)&& (black_rook > empty_score)&& (white_rook == black_rook));
}
bool test_queens(){
    std::cout << "------------------M'LADIES------------------" << std::endl;
    Score empty_score(0,0);
    std::string r;
    //standard
    Position white_queen_c4 = pos_fen("4k3/8/8/8/2Q5/8/8/4K3 w KQkq - 0 1");
    Position black_queen_c5 = pos_fen("4k3/8/8/8/2q5/8/8/4K3 w KQkq - 0 1");
    Score white_queen = white_queen_c4.queen_score(WHITE);
    Score black_queen = black_queen_c5.queen_score(BLACK);
    r = (white_queen > empty_score) ? "PASSED\n" : "FAILED\n";
    std::cout << "[White queen > empty] " + r;
    r = (black_queen > empty_score) ? "PASSED\n" : "FAILED\n";
    std::cout << "[Black queen > empty] " + r;
    r = (white_queen == black_queen) ? "PASSED\n" : "FAILED\n"; 
    std::cout << "[White == Black] " + r;
    //detect fossilizations(discoveries) and pins (single piece between enemy sliding attacker and friendly queen)
    Position queen_test_fosil_pin = pos_fen("4k3/8/8/2NN4/2Q5/8/8/4K3 w KQkq - 0 1");
    Position white_queen_pinned_once = pos_fen("4k1b1/8/8/2NN4/2Q5/8/8/4K3 w KQkq - 0 1");
    Position white_queen_pinned_twice = pos_fen("2r1k1b1/8/8/2NN4/2Q5/8/8/4K3 w KQkq - 0 1");
    Position white_queen_fossilised_once = pos_fen("4k1b1/2n2n2/8/8/2Q5/8/8/4K3 w KQkq - 0 1");
    Position white_queen_fossilised_twice = pos_fen("2r1k1b1/2n2n2/8/8/2Q5/8/8/4K3 w KQkq - 0 1");
    r = (queen_test_fosil_pin.queen_pin_count(BLACK,C4) == 0) ? "PASSED\n" : "FAILED\n";
    std::cout << "[No fossilize/pins decteded on board without fossil/pin]: " + r;
    r = (white_queen_pinned_once.queen_pin_count(BLACK,C4) == 1) ? "PASSED\n" : "FAILED\n";
    std::cout << "[Single pin detected]: " + r;
    r = (white_queen_pinned_twice.queen_pin_count(BLACK,C4) == 2) ? "PASSED\n" : "FAILED\n";
    std::cout << "[Double pin detected]: " + r;
    r = (white_queen_fossilised_once.queen_pin_count(BLACK,C4) == 1) ? "PASSED\n" : "FAILED\n";
    std::cout << "[Single fossil detected]: " + r;
    r = (white_queen_fossilised_twice.queen_pin_count(BLACK,C4) == 2) ? "PASSED\n" : "FAILED\n";
    std::cout << "[Double fossil detected]: " + r;

    return white_queen > empty_score && black_queen > empty_score && white_queen == black_queen &&  queen_test_fosil_pin.queen_pin_count(BLACK,C4) == 0
    && white_queen_pinned_once.queen_pin_count(BLACK,C4) == 1 && white_queen_pinned_twice.queen_pin_count(BLACK,C4) == 2 &&
    white_queen_fossilised_once.queen_pin_count(BLACK,C4) == 1 && white_queen_fossilised_twice.queen_pin_count(BLACK,C4) == 2;

}
void test_calculate_material(){
    std::string r = "";
    bool outposts = test_outposts();
    bool mobility = test_mobility();
    bool knights = test_knights();
    bool bishops = test_bishops();
    bool rooks = test_rooks();
    bool queens = test_queens();

    if (outposts && mobility && knights && bishops && rooks && queens){
        std::cout<< std::endl << "-------------[ALL TESTS PASSED]-------------" << std::endl <<std::endl;
    }
    else{
        std::cout << std::endl << "--------------[RIP]---------------" <<std::endl << std::endl;
    }

} 

int main(){
    //test_fen += " w KQkq - 0 1";
    init();
    test_calculate_material();
    Position test = pos_fen("4k3/4P1P1/8/2PPPP2/8/8/1P1P1P1P/4K3 w KQkq - 0 1");
    std::cout << bb_string(test.info.controlled_twice[WHITE]) <<std::endl;
    return 0;
}