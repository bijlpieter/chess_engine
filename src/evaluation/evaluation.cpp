#include "types.h"
#include "move_generation.h"
#include <iostream>

Phase Position::calculate_phase() {
	Phase total = 24;
	for (Color c : {WHITE,BLACK}) {
		total -= popcount(pieces[info.c][1]) + popcount(pieces[c][2]) + popcount(pieces[c][3]) * 2 + popcount(pieces[c][4]) * 4;
	}
	return (total * 256 + 12) / 12;
}

bool Position::is_outpost(Color c, Square s) {
	//on outpost square
	if (popcount(s & OUTPOSTS[c]) > 0) {
		return false;
	}
	//defended by pawn;
	if (popcount(PAWN_ATTACKS[~c][s] & pieces[c][PAWN]) == 0) {
		return false;
	}
	//no kickers
	Direction h_direction = (c == WHITE) ? UP : DOWN;
	int steps = (c == WHITE) ? (7 - rank(s)) : rank(s);
	if (file(s) & ~FILE_H) {
		if (bb_ray(s + RIGHT, s + (RIGHT + (h_direction * steps))) & pieces[~c][PAWN]) {
			return false;
		}
	}
	if (file(s) & ~FILE_A) {
		if (bb_ray(s + LEFT, s + (LEFT + (h_direction * steps))) & pieces[~c][PAWN]) {
			return false;
		}
	}
	return true;
}

Score Position::knight_score() {
	if (popcount(pieces[info.c][KNIGHT]) == 0) {
		return Score(0,0);
	}
	Score total = Score(0,0);
	Bitboard knights = pieces[info.c][KNIGHT];
	 //knight: less value the less pawns on the board
	// int total_pawns = popcount(pieces[WHITE][PAWN]) + popcount (pieces[BLACK][PAWN]);
	total += material_scores[KNIGHT] * (popcount(pieces[info.c][KNIGHT]));// - KNIGHT_PAWN_MODIFIER * (16 - total_pawns));
	//bonus for defended minor piece
	total += popcount(knights & info.defended_squares) * KNIGHT_DEFENDED_SCORE;
	 //mobility
	total += popcount(legal_knight_moves() & info.mobility) * mobility_scores[KNIGHT];
	//outpost, distance and shielded
	while (knights) {
		Square knight = pop_lsb(knights);
		total -= KNIGHT_KING_DISTANCE_PENALTY * SQUARE_DISTANCE[knight][info.king_square];
		if (knight & shift(pieces[info.c][PAWN], DOWN)){
			total += KNIGHT_SHIELDED_SCORE;
		}
		if (is_outpost(info.c, knight)) {
			total += KNIGHT_OUTPOST_SCORE;
		}
	}
	return total;
}
//TODO bad bishop & trapped bishop
Score Position::bishop_score() {
	if (popcount(pieces[info.c][BISHOP]) == 0) {
		return Score(0,0);
	}
	Score total = Score(0,0);
	Bitboard bishops = pieces[info.c][BISHOP];
	total += material_scores[BISHOP] * popcount(pieces[info.c][BISHOP]);
	//complementing bishops on the board
	if (bishops & LIGHT_SQUARES && bishops & DARK_SQUARES) total += BISHOP_PAIR_SCORE;
	//bonus for defended minor piece
	total += popcount(bishops & info.defended_squares) * BISHOP_DEFENDED_SCORE;
	//mobility 
	total += popcount(legal_bishop_moves() & info.mobility) * mobility_scores[BISHOP];
	//distance, outposts and fianchettoes, xraying enemy pawns and attacking the enemy king_area
	while (bishops) {
		Square bishop = pop_lsb(bishops);
		total -= BISHOP_KING_DISTANCE_PENALTY * SQUARE_DISTANCE[bishop][info.king_square];
		total -= BISHOP_XRAY_PAWN_PENALTY * (popcount(bishop_moves(bishop, 0) & pieces[~info.c][PAWN]));
		if (bishop & shift(pieces[info.c][PAWN],DOWN)){
			total += BISHOP_SHIELDED_SCORE;
		}
		if (bishop_moves(bishop, pieces[info.c][PAWN]) & KING_AREA[info.opp_king_square]){
			total += BISHOP_ATTACKING_KING_SCORE;
		}

		if (is_outpost(info.c, bishop)) {
			total += BISHOP_OUTPOST_SCORE;
		}
		else {
			if (bishop == B2 && info.c == WHITE) {
				if (popcount(pieces[info.c][PAWN] & WHITE_LEFT_FIANCHETTO) > 2) {
					total += BISHOP_FIANCHETTO_SCORE;
				}
			}
			else if (bishop == G2 && info.c == WHITE) {
				if (popcount(pieces[info.c][PAWN] & WHITE_RIGHT_FIANCHETTO) > 2) {
					total += BISHOP_FIANCHETTO_SCORE;
				}  
			}
			else if (bishop == B7 && info.c == BLACK) {
				if (popcount(pieces[info.c][PAWN] & BLACK_LEFT_FIANCHETTO) > 2) {
					total += BISHOP_FIANCHETTO_SCORE;
				} 
			}
			else if (bishop == G7 && info.c == BLACK) {
				if (popcount(pieces[info.c][PAWN] & BLACK_RIGHT_FIANCHETTO) > 2){
					total += BISHOP_FIANCHETTO_SCORE;
				}
			} 
		}
	}
	return total;
}
Score Position::rook_score() {
	return Score(0,0);
	if (popcount(pieces[info.c][ROOK]) == 0) {
		return Score(0,0);
	}
	Score total = Score(0,0);
	Bitboard rooks = pieces[info.c][ROOK];
	total += material_scores[ROOK] * popcount(pieces[info.c][ROOK]);
	//mobility
	uint64_t mobility_count = popcount(legal_rook_moves() & info.mobility);
	total += mobility_count * mobility_scores[ROOK];
	if (popcount(legal_rook_moves() & rooks) > 2){
		total += ROOK_STACKED_SCORE;
	}
	while(rooks){
		Square rook = pop_lsb(rooks);
		File f = file(rook);
		Rank r = rank(rook);
		if (r & KING_AREA[info.opp_king_square]){
			total += ROOK_ON_KING_RANK_SCORE;
		}
		if (f & KING_AREA[info.opp_king_square]){
			total += ROOK_ON_KING_FILE_SCORE;
		}
		if (r | f | pieces[~info.c][QUEEN]){
			total += ROOK_ON_QUEEN_LINE_SCORE;
		}
		if (r == RANK_7 && info.c == WHITE){
			if ((r & pieces[~info.c][PAWN]) || (rank(info.opp_king_square) == RANK_8)){
				total += ROOK_ON_SEVENTH_SCORE;
			}
		}
		else if (r == RANK_2 && info.c == BLACK){
			if ((r & pieces[~info.c][PAWN]) || (rank(info.opp_king_square) == RANK_1)){
				total += ROOK_ON_SEVENTH_SCORE;
			}
		}
		if (f & ~pieces[info.c][PAWN]){
			total += ROOK_ON_OPEN_SCORE;
		}
		else if (f & info.blocked_pawns){
			total -= ROOK_ON_BLOCKED_PENALTY;
		}
		if (mobility_count < 3 &&  r == info.opp_promotion_rank && rank(info.king_square) == info.opp_promotion_rank){
			File kf = file(info.king_square);
			if ((kf < FILE_E && kf > f) || (kf > FILE_E && kf < f)){
				total -= ROOK_TRAPPED_BY_KING_PENALTY;
			}
		}
	}
	return total;
}
void Position::eval_init(Color c){
	Direction left = c == WHITE ? DOWN_LEFT : UP_LEFT;
	Direction right = c == WHITE ? DOWN_RIGHT : UP_RIGHT;
	Bitboard enemy_pawn_control = (shift(pieces[~c][PAWN], left) | shift(pieces[~c][PAWN], right));
    Bitboard lower_ranks = c == WHITE ? LOW_RANKS_WHITE : LOW_RANKS_BLACK;
	info.c = c;
    info.king_square = state->king;
	info.opp_king_square = lsb(pieces[~c][KING]);
    info.defended_squares = controlling(c, all_pieces);
	info.blocked_pawns = pieces[c][PAWN] & shift(all_pieces, DOWN);
    info.mobility = ~(info.blocked_pawns | lower_ranks | info.king_square | pieces[c][QUEEN] | state->pinned | enemy_pawn_control);
	info.king_area[c] = KING_AREA[info.king_square];
	info.king_area[~c] = KING_AREA[info.opp_king_square];
	info.promotion_rank = c == WHITE ? RANK_8 : RANK_1;
	info.opp_promotion_rank = c == WHITE ? RANK_1 : RANK_8;
}
Score Position::calculate_score(Color c) {
	eval_init(c);
	Score total(0,0);
	total += knight_score();
	total += bishop_score();
	total += rook_score();
   
	return total;
}