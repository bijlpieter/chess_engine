#include "types.h"
#include "move_generation.h"

Phase Position::calculate_phase() {
	Phase total = 24;
	for (Color c : {WHITE,BLACK}) {
		total -= popcount(pieces[info.c][1]) + popcount(pieces[c][2]) + popcount(pieces[c][3]) * 2 + popcount(pieces[c][4]) * 4;
	}
	return (total * 256 + 12) / 12;
}
//centralizes the kingring
Bitboard Position::king_ring(Square ks){
	if(rank(ks) < RANK_2){
		ks += UP;
	}
	else if (rank(ks) > RANK_7){
		ks += DOWN;
	}
	if (file(ks) < FILE_B){
		ks += RIGHT;
	}
	else if (file(ks) > FILE_G){
		ks += LEFT;
	}
	return king_moves(ks);
}

bool Position::is_outpost(Color c, Square s) {
	//on outpost square
	if (!(s & OUTPOSTS[c])) {
		return false;
	}
	//defended by pawn;
	if (!(PAWN_ATTACKS[~c][s] & pieces[c][PAWN])) {
		return false;    
	}
	//no kicker nickers
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
	int outposts = 0, distance = 0, shielded = 0;
	while (knights) {
		Square knight = pop_lsb(knights);
		distance += SQUARE_DISTANCE[knight][info.king_square];
		if (knight & shift(pieces[info.c][PAWN], DOWN)){
			shielded++;
		}
		if (is_outpost(info.c, knight)) {
			outposts++;
		}
	}
	total += KNIGHT_OUTPOST_SCORE * outposts;
	total += KNIGHT_SHIELDED_SCORE * shielded;
	total -= KNIGHT_KING_DISTANCE_PENALTY * distance;
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
	//distance, outposts and fianchettoes and xraying enemy pawns
	int outposts = 0, distance = 0, shielded = 0, fianchettoes = 0, pawns_xrayed = 0;
	while (bishops) {
		Square bishop = pop_lsb(pieces[info.c][BISHOP]);
		distance += SQUARE_DISTANCE[bishop][info.king_square];
		pawns_xrayed += popcount(bishop_moves(bishop, 0) & pieces[~info.c][PAWN]);
		if (bishop & shift(pieces[info.c][PAWN],DOWN)){
			shielded++;
		}
		if (is_outpost(info.c, bishop)) {
			outposts++;
		}
		else {
			if (bishop == B2 && info.c == WHITE) {
				if (popcount(pieces[info.c][PAWN] & WHITE_LEFT_FIANCHETTO) > 2) {
					fianchettoes++;
				}
			}
			else if (bishop == G2 && info.c == WHITE) {
				if (popcount(pieces[info.c][PAWN] & WHITE_RIGHT_FIANCHETTO) > 2) {
					fianchettoes++;
				}  
			}
			else if (bishop == B7 && info.c == BLACK) {
				if (popcount(pieces[info.c][PAWN] & BLACK_LEFT_FIANCHETTO) > 2) {
					fianchettoes++;
				} 
			}
			else if (bishop == G7 && info.c == BLACK) {
				if (popcount(pieces[info.c][PAWN] & BLACK_RIGHT_FIANCHETTO) > 2){
					fianchettoes++;
				}
			} 
		}
	}
	total += BISHOP_OUTPOST_SCORE * outposts;
	total += BISHOP_SHIELDED_SCORE * shielded;
	total += BISHOP_FIANCHETTO_SCORE * fianchettoes;
	total -= BISHOP_KING_DISTANCE_PENALTY * distance;
	total -= BISHOP_XRAY_PAWN_PENALTY * pawns_xrayed;
	return total;
}

Score Position::rook_score() {
	return Score(0, 0);
}

Score Position::calculate_score(Color c) {
    Direction left = c == WHITE ? DOWN_LEFT : UP_LEFT;
	Direction right = c == WHITE ? DOWN_RIGHT : UP_RIGHT;
	Bitboard enemy_pawn_control = (shift(pieces[~c][PAWN], left) | shift(pieces[~c][PAWN], right));
    Bitboard lower_ranks = c == WHITE ? LOW_RANKS_WHITE : LOW_RANKS_BLACK;
	info.c = c;
    info.king_square = state->king;
    info.defended_squares = controlling(c, all_pieces);
    info.blocked_pawns = pieces[c][PAWN] & (shift(all_pieces, DOWN) | lower_ranks);
    info.mobility = ~(info.blocked_pawns | info.king_square | pieces[c][QUEEN] | state->pinned | enemy_pawn_control);
	info.king_area[c] = king_ring(info.king_square);
	info.king_area[~c] = king_ring(lsb(pieces[~c][KING]));
	
	Score total(0,0);
	total += knight_score();
	total += bishop_score();
	total += rook_score();
   
	return total;
}