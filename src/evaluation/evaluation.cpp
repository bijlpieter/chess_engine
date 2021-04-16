#include "types.h"
#include "move_generation.h"
#include <iostream>

Phase Position::calculate_phase() {
	Phase total = 24;
	for (Color c : {WHITE,BLACK}) {
		total -= popcount(pieces[c][1]) + popcount(pieces[c][2]) + popcount(pieces[c][3]) * 2 + popcount(pieces[c][4]) * 4;
	}
	return (total * 256 + 12) / 12;
}
Bitboard Position::get_pseudo_legal_moves(PieceType p, Color c, Square s){
	switch (p){
		case PAWN:
			return (shift(pieces[c][PAWN], info.left_pawn_attack[c]) | shift(pieces[c][PAWN],info.right_pawn_attack[c]));
		case KNIGHT:
			return knight_moves(s);
		case BISHOP:
			return bishop_moves(s,0);
		case ROOK:
			return rook_moves(s,0);
		case QUEEN:
			return queen_moves(s,0);
		case KING:
			return king_moves(s);
		default:
			return 0;
	}
}
int Position::queen_pin_count(Color opp, Square q){
	int pinned = 0;
	Bitboard snipers = (rook_moves(q, 0) & (pieces[opp][ROOK] | pieces[opp][QUEEN])) | (bishop_moves(q, 0) & (pieces[opp][BISHOP] | pieces[opp][QUEEN]));
	while(snipers){
		Bitboard ray_blockers = bb_ray(pop_lsb(snipers), q) & ~q & all_pieces;
		if (ray_blockers && popcount(ray_blockers) == 1){
			pinned++;
		}
	}
	return pinned;
}
bool Position::is_outpost(Color c, Square s) {
	//on outpost square
	if (popcount(OUTPOSTS[c] & s) == 0) {
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
bool Position::is_open_file(Color c, File f){
	return (popcount(pieces[c][PAWN] & f) == 0);
}
Rank Position::relevant_rank(Color c, Rank r){
	return Rank(r ^ (c * 7));
}
Square Position::farmost_square(Color c, Bitboard b){
	if (!b){
		std::cout << "ERROR: farmost_square requires a non-empty bitboard";
		return NO_SQUARE;
	}
	return c == WHITE ? msb(b) : lsb(b);
}
Score Position::knight_score(Color c) {
	if (popcount(pieces[c][KNIGHT]) == 0) {
		return Score(0,0);
	}
	Score total = Score(0,0);
	Bitboard knights = pieces[c][KNIGHT];
	//material
	total += material_scores[KNIGHT] * popcount(knights);
	//bonus for defended minor piece
	total += popcount(knights & info.controlled_squares[c]) * KNIGHT_DEFENDED_SCORE;
	 //mobility
	total += popcount(info.controlled_by[c][KNIGHT] & info.mobility[c]) * mobility_scores[KNIGHT];
	//outpost, distance and shielded
	while (knights) {
		Square knight = pop_lsb(knights);
		total -= KNIGHT_KING_DISTANCE_PENALTY * SQUARE_DISTANCE[knight][info.king_squares[c]];
		if (shift(pieces[c][PAWN], DOWN) & knight){
			total += KNIGHT_SHIELDED_SCORE;
		}
		if (is_outpost(c, knight)) {
			total += KNIGHT_OUTPOST_SCORE;
		}
	}
	return total;
}
Score Position::bishop_score(Color c) {
	if (popcount(pieces[c][BISHOP]) == 0) {
		return Score(0,0);
	}
	Score total(0,0);
	Bitboard bishops = pieces[c][BISHOP];
	total += material_scores[BISHOP] * popcount(bishops);
	//complementing bishops on the board
	if (bishops & LIGHT_SQUARES && bishops & DARK_SQUARES) total += BISHOP_PAIR_SCORE;
	//mobility
	total += popcount(info.controlled_by[c][BISHOP] & info.mobility[c]) * mobility_scores[BISHOP];
	//bonus for defended minor piece
	total += popcount(bishops & info.controlled_squares[c]) * BISHOP_DEFENDED_SCORE;
	//distance, outposts and fianchettoes, xraying enemy pawns and attacking the enemy king_area
	while (bishops) {
		Square bishop = pop_lsb(bishops);
		total -= BISHOP_KING_DISTANCE_PENALTY * SQUARE_DISTANCE[bishop][info.king_squares[c]];
		total -= BISHOP_XRAY_PAWN_PENALTY * (info.controlled_by[c][BISHOP] & pieces[~c][PAWN]);
		if (shift(pieces[c][PAWN],DOWN) & bishop){
			total += BISHOP_SHIELDED_SCORE;
		}
		if (bishop_moves(bishop, pieces[c][PAWN]) & KING_AREA[info.king_squares[~c]]){
			total += BISHOP_ATTACKING_KING_SCORE;
		}

		if (is_outpost(c, bishop)) {
			total += BISHOP_OUTPOST_SCORE;
		}
		else {
			if (bishop == B2 && c == WHITE) {
				if (popcount(pieces[c][PAWN] & WHITE_LEFT_FIANCHETTO) > 2) {
					total += BISHOP_FIANCHETTO_SCORE;
				}
			}
			else if (bishop == G2 && c == WHITE) {
				if (popcount(pieces[c][PAWN] & WHITE_RIGHT_FIANCHETTO) > 2) {
					total += BISHOP_FIANCHETTO_SCORE;
				}  
			}
			else if (bishop == B7 && c == BLACK) {
				if (popcount(pieces[c][PAWN] & BLACK_LEFT_FIANCHETTO) > 2) {
					total += BISHOP_FIANCHETTO_SCORE;
				} 
			}
			else if (bishop == G7 && c == BLACK) {
				if (popcount(pieces[c][PAWN] & BLACK_RIGHT_FIANCHETTO) > 2){
					total += BISHOP_FIANCHETTO_SCORE;
				}
			} 
		}
	}
	return total;
}
Score Position::rook_score(Color c) {
	if (popcount(pieces[c][ROOK]) == 0) {
		return Score(0,0);
	}
	Score total(0,0);
	Bitboard rooks = pieces[c][ROOK];
	total += material_scores[ROOK] * popcount(rooks);
	total += popcount(info.controlled_by[c][ROOK] & info.mobility[c]) * mobility_scores[ROOK];
	//rooks defending eachother
	if (popcount(rooks) > 1){
		Bitboard rooks_temp = rooks;
		Bitboard lines = 0;
		while (rooks_temp){
			Square rook_temp = pop_lsb(rooks_temp);
			lines |= rook_moves(rook_temp, all_pieces);
		}
		if (popcount(rooks & lines) > 1){
			total += ROOK_STACKED_SCORE;
		}
	}
	
	while(rooks){
		Square rook = pop_lsb(rooks);
		File f = file(rook);
		Rank r = rank(rook);
		if (KING_AREA[info.king_squares[~c]] & r){
			total += ROOK_ON_KING_RANK_SCORE;
		}
		if (KING_AREA[info.king_squares[~c]] & f){
			total += ROOK_ON_KING_FILE_SCORE;
		}
		if ((r | f) & pieces[~c][QUEEN]){
			total += ROOK_ON_QUEEN_LINE_SCORE;
		}
		if (r == RANK_7 && c == WHITE){
			if ((pieces[~c][PAWN] & r) || (rank(info.king_squares[~c]) == RANK_8)){
				total += ROOK_ON_SEVENTH_SCORE;
			}
		}
		else if (r == RANK_2 && c == BLACK){
			if ((pieces[~c][PAWN] & r) || (rank(info.king_squares[~c]) == RANK_1)){
				total += ROOK_ON_SEVENTH_SCORE;
			}
		}
		if (is_open_file(c,f)){
			total += ROOK_ON_OPEN_SCORE;
		}
		else if (info.blocked_pawns[c] & f){
			total -= ROOK_ON_BLOCKED_PENALTY;
		}
		if (popcount(rook_moves(rook,all_pieces)) < 5){
			if (r == info.promotion_rank[~c] && rank(info.king_squares[c]) == info.promotion_rank[~c]){
				File kf = file(info.king_squares[c]);
				if ((kf < FILE_E && kf > f) || (kf > FILE_E && kf < f)){
					total -= ROOK_TRAPPED_BY_KING_PENALTY;
				}
			}
		}
	}
	return total;
}
Score Position::queen_score(Color c){
	if (popcount(pieces[c][QUEEN]) == 0) {
		return Score(0,0);
	}
	Score total(0,0);
	Bitboard queens = pieces[c][QUEEN];
	//material
	total += material_scores[QUEEN] * popcount(queens);
	//mobility 
	total += popcount(info.controlled_by[c][QUEEN] & info.mobility[c]) * mobility_scores[QUEEN];
	while (queens){
		Square queen = pop_lsb(queens);
		total -= queen_pin_count(~c, queen) * QUEEN_PINNED_PENALTY;
	}
	return total;

}
Score Position::pawn_storm_safety(Color c){
	Score total(0,0);
	int opp_next_rank = c == WHITE ? -1 : 1;
	Rank r = rank(info.king_squares[~c]);
	// these ranks are in front of the enemy king.
	Bitboard excluded_ranks = BB_RANKS[r + opp_next_rank] | BB_RANKS[r + opp_next_rank + opp_next_rank];
	Bitboard pawns = pieces[c][PAWN] & ~info.controlled_by[~c][PAWN] & ~excluded_ranks;
	Bitboard opp_pawns = pieces[~c][PAWN] & ~excluded_ranks;

	File kf = file(info.king_squares[c]);
	kf = (kf == FILE_A) ? FILE_B : (kf == FILE_H) ? FILE_G : kf;
	Bitboard temp;
	for (File f = File(kf - 1); f <= File(kf + 1); f++){
		temp = pawns & f;
		int def_rank = temp ? relevant_rank(c, rank(farmost_square(c, temp))) : 0;
		temp = opp_pawns & f;
		int att_rank = temp ? relevant_rank(c, rank(farmost_square(~c, temp))) : 0;
		if (def_rank && (def_rank == att_rank - 1)){
			total += PAWN_STORM_BLOCKED_FILE_PENALTY[att_rank];
			//std::cout << "BLOCKED! COLOR: " << c <<" def_rank: " << def_rank << " att_rank: " << att_rank << std::endl;
			
		}
		// no need to check if(att_rank);
		else{
			total += PAWN_STORM_UNBLOCKED_FILE_PENALTY[att_rank];
			//std::cout << "UNBLOCKED! COLOR: " << c <<" def_rank: " << def_rank << " att_rank: " << att_rank << std::endl;
		}
	}
	return total;
}
Score Position::king_score(Color c){
	Score total(0,0);
	//if pawns/king has moved.{pawnstorm}
	total += pawn_storm_safety(c);
	if (is_open_file(c,file(info.king_squares[c]))){
		total -= KING_ON_OPEN_FILE_PENALTY;
	}
	Bitboard pawns = pieces[c][PAWN];
	int pawn_dist = 6;
	if (pawns & info.controlled_by[c][KING]){
		pawn_dist = 1;
	}
	else while(pawns){
		pawn_dist = std::min(pawn_dist, SQUARE_DISTANCE[info.king_squares[c]][pop_lsb(pawns)]);
	}
	total += KING_PAWN_DISTANCE_SCORE[pawn_dist];
	return total;

}

void Position::eval_init(){
	for (Color c : {WHITE,BLACK}){
		info.king_squares[c] = lsb(pieces[c][KING]);
		info.king_area[c] = KING_AREA[info.king_squares[c]];
		info.blocked_pawns[c] = pieces[c][PAWN] & shift(all_pieces, DOWN);
		info.pinned[c] = blockers(info.king_squares[c],c,~c);
		for (PieceType p : {PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING}){
			if (pieces[c][p]){
				Bitboard all = pieces[c][p];
				while (all){
					Square s = pop_lsb(all);
					Bitboard moves = get_pseudo_legal_moves(p, c, s);
					info.controlled_twice[c] |= info.controlled_squares[c] & moves;
					info.controlled_by[c][p] |= moves;
					info.controlled_squares[c] |= moves;
				}
			}
		}
	}
	//todo | pinned pieces
	for (Color c : {WHITE,BLACK}){
		info.mobility[c] = ~(info.blocked_pawns[c] | info.pinned[c] | info.king_squares[c] | pieces[c][QUEEN] | info.controlled_by[~c][PAWN]);
	}
    
}
Score Position::calculate_material(){
	Score total(0,0);
	total += knight_score(WHITE) - knight_score(BLACK);
	total += bishop_score(WHITE) - knight_score(BLACK);
	total += rook_score(WHITE) - knight_score(BLACK);
	total += queen_score(WHITE) - queen_score(BLACK);
	total += king_score(WHITE) - king_score(BLACK);
	/*
	std::cout << "-----------------Score-Debug-----------------" << std::endl;
	std::cout << "knight(W): " << knight_score(WHITE) << " knight(B): " << knight_score(BLACK) << std::endl;
	std::cout << "bishop(W): " << bishop_score(WHITE) << " bishop(B): " << bishop_score(BLACK) << std::endl;
	std::cout << "rook(W): " << rook_score(WHITE) << " rook(B): " << rook_score(BLACK) << std::endl;
	std::cout << "queen(W): " << queen_score(WHITE) << " queen(B): " << queen_score(BLACK) << std::endl;
	std::cout << "king(W): " << king_score(WHITE) << " king(B): " << king_score(BLACK) << std::endl;
	std::cout << "---------------------------------------------" << std::endl;
	*/
	return total;
}
Score Position::calculate_score() {
	eval_init();
	Score total(0,0);
	total += calculate_material();
	
   
	return total;
}