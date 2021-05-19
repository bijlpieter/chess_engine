#include "types.h"
#include "move_generation.h"
#include <iostream>
Rank relevant_rank(Color c, Rank r){
	return Rank(r ^ (c * 7));
}
Bitboard forward_ranks(Color c, Square s) {
  return c == WHITE ? ~BB_RANKS[RANK_1] << 8 * relevant_rank(WHITE, rank(s))
                    : ~BB_RANKS[RANK_8] >> 8 * relevant_rank(BLACK, rank(s));
}
Bitboard forward_files(Color c, Square s){
	return forward_ranks(c, s) & BB_FILES[file(s)];
}
Bitboard adjacent_files(Square s){
	return shift(BB_FILES[file(s)],LEFT) & shift(BB_FILES[file(s)], RIGHT);
}
Bitboard pawn_att_span(Color c, Square s){
	return forward_ranks(c,s) & adjacent_files(s);
}
Bitboard pass_pawn_span(Color c, Square s){
	return pawn_att_span(c,s) | forward_files(c,s);
}
bool more_than_one(Bitboard pieces){
	return popcount(pieces) > 1;
}

Square farmost_square(Color c, Bitboard b){
	if (!b){
		std::cout << "ERROR: farmost_square requires a non-empty bitboard";
		return NO_SQUARE;
	}
	return c == WHITE ? msb(b) : lsb(b);
}

Phase Position::calculate_phase() {
	Phase total = 24;
	for (Color c : {WHITE,BLACK}) {
		total -= popcount(pieces[c][KNIGHT]) + popcount(pieces[c][BISHOP]) + popcount(pieces[c][ROOK]) * 2 + popcount(pieces[c][QUEEN]) * 4;
	}
	return (total * 256 + 12) / 24;
}

Bitboard Position::get_pawn_moves(Color c, Bitboard pawns){
	return shift(pawns, info.left_pawn_attack[c]) | shift(pawns, info.right_pawn_attack[c]);
}
Bitboard Position::get_pawn_double_attacks(Color c, Bitboard pawns){
	return shift(pawns, info.left_pawn_attack[c]) & shift(pawns, info.right_pawn_attack[c]);
}

Bitboard Position::get_pseudo_legal_moves(PieceType p, Square s){
	switch (p){
		case KNIGHT:
			return knight_moves(s);
		case BISHOP:
			return bishop_moves(s,all_pieces);
		case ROOK:
			return rook_moves(s,all_pieces);
		case QUEEN:
			return queen_moves(s,all_pieces);
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
		if (shift(pieces[c][PAWN], info.push_direction[~c]) & knight){
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
		if (shift(pieces[c][PAWN], info.push_direction[~c]) & bishop){
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
	//pawn storm
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
	//squares enemy attack, which defended by king/queen at most once.
	Bitboard weak_squares =  info.controlled_squares[~c] &~info.controlled_twice[c] & (~info.controlled_squares[c] | info.controlled_by[c][KING] | info.controlled_by[c][QUEEN]);
	//potential squares where opponent can check. 
	//Square with no enemy pieces -> not attacked by us or defended by queen/king once and double attacked
	Bitboard safe_checks_squares = ~colors[~c] & (~info.controlled_squares[c] | (weak_squares & info.controlled_twice[~c]));
	Bitboard rook_lines = rook_moves(info.king_squares[c], all_pieces);
	Bitboard bishop_lines = bishop_moves(info.king_squares[c], all_pieces);
	Bitboard unsafe_checks = 0;
	// Rook checks.
	Bitboard enemy_rook_checks = rook_lines & info.controlled_by[~c][ROOK] & safe_checks_squares;
	if (enemy_rook_checks){
		//std::cout << "Safe Rook checks found: " <<std::endl << bb_string(enemy_rook_checks) <<std::endl;
		total -= SAFE_CHECKS_PENALTY[more_than_one(enemy_rook_checks)][ROOK];
	}
	else{
		unsafe_checks |= enemy_rook_checks;
	}
	// Queen checks: rook checks > queen checks. Queen may defend queen checks->equal trade. No need for unsafe chceks. bishop/rook will find all of those.
	Bitboard enemy_queen_checks = (rook_lines | bishop_lines) & info.controlled_by[~c][QUEEN] & safe_checks_squares
	& ~(info.controlled_by[c][QUEEN] | enemy_rook_checks);
	if (enemy_queen_checks){
		//std::cout << "Safe Queen checks found: " <<std::endl << bb_string(enemy_queen_checks) <<std::endl;
		total -= SAFE_CHECKS_PENALTY[more_than_one(enemy_queen_checks)][QUEEN];
	}
	// Bishop checks: queen checks > bishop checks.
	Bitboard enemy_bishop_checks = bishop_lines & info.controlled_by[~c][BISHOP] & safe_checks_squares & ~enemy_queen_checks;
	if (enemy_bishop_checks){
		total -= SAFE_CHECKS_PENALTY[more_than_one(enemy_bishop_checks)][BISHOP];
		//std::cout << "Safe Bishop checks found: " <<std::endl << bb_string(enemy_bishop_checks) <<std::endl;
	}
	else{
		unsafe_checks |= bishop_lines & info.controlled_by[~c][BISHOP];
	}
	//Knight checks
	Bitboard enemy_knight_checks = knight_moves(info.king_squares[c]) & info.controlled_by[~c][KNIGHT];
	if (enemy_knight_checks & safe_checks_squares){
		total -= SAFE_CHECKS_PENALTY[more_than_one(enemy_knight_checks)][KNIGHT];
		//std::cout << "Safe Knight checks found: " <<std::endl << bb_string(enemy_knight_checks) <<std::endl;
	}
	else{
		unsafe_checks |= enemy_knight_checks;
	}
	total -= KING_AREA_WEAK_SQUARE_PENALTY * popcount(info.king_area[c] & weak_squares);
	total -= UNSAFE_CHECKS_PENALTY * popcount(unsafe_checks);
	if (!(pieces[~c][QUEEN])){
		total += KING_NO_ENEMY_QUEEN_SCORE;
	}
	return total;
}
void Position::pawn_info_init(Color c, PawnInfo* p_info){
	Bitboard neighbours, stoppers, support, phalanx, opposed;
    Bitboard lever, lever_push, blocked;
    Square s;
    bool backward, passed, doubled;
    Bitboard b = pieces[c][PAWN];

    Bitboard our_pawns   = b;
    Bitboard their_pawns = pieces[~c][PAWN];
    Bitboard their_double_att = get_pawn_double_attacks(~c,their_pawns);
	p_info->passed[c] = 0;
	p_info->pawn_attacks[c] = p_info->pawn_attack_span[c] = get_pawn_moves(c, our_pawns);

	p_info->blocked += popcount(shift(our_pawns, info.push_direction[c]) & (their_pawns | their_double_att));

    while (b){
        s = pop_lsb(b);
        Rank r = relevant_rank(c, rank(s));
        opposed     = their_pawns & forward_files(c, s);
        blocked     = their_pawns & (s + info.push_direction[c]);
        stoppers    = their_pawns & pass_pawn_span(c, s);
        lever       = their_pawns & get_pawn_moves(c, BB_SQUARES[s]);
        lever_push  = their_pawns & get_pawn_moves(c, BB_SQUARES[s + info.push_direction[c]]);
        doubled     = our_pawns   & (s + info.push_direction[~c]);
        neighbours  = our_pawns   & adjacent_files(s);
        phalanx     = neighbours & BB_RANKS[rank(s)];
        support     = neighbours & BB_RANKS[rank(s + info.push_direction[~c])];
		backward    = !(neighbours & forward_ranks(~c, s + info.push_direction[~c])) && (lever_push | blocked);

        if (doubled && !(our_pawns & shift(their_pawns | get_pawn_moves(~c,their_pawns),DOWN))){
			p_info->scores[c] -= PAWN_EARLY_DOUBLE_PENALTY;
        }
		if (!backward && !blocked){
			p_info->pawn_attack_span[c] |= pawn_att_span(c,s);
		}

        passed =  !(stoppers ^ lever)
		|| (!(stoppers ^ lever_push) && popcount(phalanx) >= popcount(lever_push))
        || (stoppers == blocked && r >= RANK_5 && (shift(support, info.push_direction[c]) & ~(their_pawns | their_double_att)));
        passed &= !(forward_files(c, s) & our_pawns);

        if (passed){
			p_info->passed[c] |= BB_SQUARES[s];
		}
            
        if (support | phalanx){
           	p_info->scores[c] += PAWN_CONNECTED_SCORE * (int)r;
			p_info->scores[c] += PAWN_SUPPORTED_SCORE * popcount(support);
        }
        else if (!neighbours){
            if (opposed &&  (our_pawns & forward_files(~c, s)) && !(their_pawns & adjacent_files(s))){
				p_info->scores[c] -= PAWN_DOUBLED_PENALTY;
			} 
            else{
                p_info->scores[c] -=  PAWN_ISOLATED_PENALTY;
			}
        }

        else if (backward){
			p_info->scores[c] -=  PAWN_BACKWARDS_PENALTY;
		}
        if (!support){
			p_info->scores[c] -= PAWN_DOUBLED_PENALTY * (int)doubled;
			p_info->scores[c] -= PAWN_WEAK_LEVER_PENALTY * more_than_one(lever);
		}
        if (blocked && r >= RANK_5){
			p_info->scores[c] += PAWN_ADVANCED_BLOCK_SCORE;
		}       
    }
}
PawnInfo* Position::get_pawn_info(Key key, std::vector<PawnInfo>* pawn_hash_table){
	PawnInfo* p = &pawn_hash_table->at(key & 131071);
	if(key == p->key){
		return p;
	}
	p->key = key;
	p->blocked = 0;
	pawn_info_init(WHITE, p);
	pawn_info_init(BLACK, p);
	return p;
}
Score Position::pawn_score(Color c, std::vector<PawnInfo>* pawn_hash_table){
    PawnInfo* current = get_pawn_info(state->pawn_key, pawn_hash_table);
	Score total = current->scores[c];
	total += calculate_passed(c, current);
	total += calculate_space(c, current);
    return total;
}
Score Position::calculate_passed(Color c, PawnInfo* p_info){
	Score total(0,0);
	Bitboard passed = p_info->passed[c];
	//passed juicers blocked by enemy juicers
    Bitboard blocked_passed = passed & shift(pieces[~c][PAWN], info.push_direction[~c]);

    if (blocked_passed){
        Bitboard helpers =  shift(pieces[c][PAWN], info.push_direction[c]) & ~colors[~c] & (info.controlled_twice[~c] | info.controlled_squares[c]);
		passed &= ~blocked_passed | shift(helpers, LEFT) | shift(helpers,RIGHT);
    }
    while (passed){
        Square s = pop_lsb(passed);
        //assert(!(pos.pieces(Them, PAWN) & forward_file_bb(Us, s + Up)));
        int r = relevant_rank(c, rank(s));
		int f = file(s);
    	total += PAWN_PASSED_RANK[r];
		total += PAWN_PASSED_EDGE_DIST_SCORE *  (4 - (int)std::min(f, FILE_H - f));

        if (r > RANK_3){
            Square next_square = s + UP;
			// king prox - enemy king prox of next square
			total += PAWN_KING_PROX_SCORE * (SQUARE_DISTANCE[next_square][info.king_squares[c]] - SQUARE_DISTANCE[next_square][info.king_squares[~c]]);
            if (piece_on(next_square) == NO_PIECE){
                Bitboard queen_path = forward_files(c, s);
                Bitboard unsafe = pass_pawn_span(c,s);
				Bitboard behind = forward_files(~c, s) & (pieces[c][ROOK] | pieces[~c][ROOK] | pieces[c][QUEEN] | pieces[~c][QUEEN]);

                if (!(colors[~c] & unsafe)){
					unsafe &= info.controlled_squares[~c] | colors[~c];
				}
				//uncontested passed_pawn_span
				if (!unsafe){
					total += PAWN_PASSED_UNCONTESTED_SCORE;
				}
				//contested but defended by pawn
				else if (!(unsafe & ~info.controlled_by[c][PAWN])){
					total += PAWN_PASSED_CONTESTED_SUPPORTED_SCORE;
				}
				//queen_path uncontested
				else if (!(unsafe & queen_path)){
					total += PAWN_PASSED_QUEENPATH_UNCONTESTED_SCORE;
				}
				//next_square uncontested
				else if (!(unsafe & next_square)){
					total += PAWN_PASSED_NEXTSQUARE_UNCONTESTED_SCORE;
				}
				//next square defended
				if (colors[c] & behind || (info.controlled_squares[c] & next_square)){
					total += PAWN_PASSED_NEXTSQUARE_DEFENDED_SCORE;
				}
            }
        } 
    }
	return total;
}
void Position::eval_init(){
	//do this seperately to get the bitboards attacked_by_pawns_twice[c] to subtract it from king_area[c];
	Bitboard attacked_by_pawns_twice[NUM_COLORS];
	info.phase = calculate_phase();
	for (Color c : {WHITE,BLACK}){
		Bitboard pawns = pieces[c][PAWN];
		Bitboard left = shift(pawns, info.left_pawn_attack[c]);
		Bitboard right = shift(pawns, info.right_pawn_attack[c]);
		Bitboard single_att = left | right;
		Bitboard double_att = left & right;
		attacked_by_pawns_twice[c] = double_att;
		info.controlled_twice[c] |=  double_att;
		info.controlled_by[c][PAWN] |= single_att;
		info.controlled_squares[c] |= single_att;
	}
	for (Color c : {WHITE,BLACK}){
		info.king_squares[c] = lsb(pieces[c][KING]);
		info.king_area[c] = KING_AREA[info.king_squares[c]];
		info.blocked_pawns[c] = pieces[c][PAWN] & shift(all_pieces, info.push_direction[~c]);
		info.pinned[c] = blockers(info.king_squares[c], c ,~c);
		for (PieceType p : {PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING}){
			if (pieces[c][p]){
				Bitboard all = pieces[c][p];
				while (all){
					Square s = pop_lsb(all);
					Bitboard moves = get_pseudo_legal_moves(p, s);
					info.controlled_twice[c] |= info.controlled_squares[c] & moves;
					info.controlled_by[c][p] |= moves;
					info.controlled_squares[c] |= moves;
				}
			}
		}
	}
	for (Color c : {WHITE,BLACK}){
		info.mobility[c] = ~(info.blocked_pawns[c] | info.pinned[c] | info.king_squares[c] | pieces[c][QUEEN] | info.controlled_by[~c][PAWN]);
		info.king_area[c] &= ~attacked_by_pawns_twice[c];
	}
    
}
Score Position::calculate_material(std::vector<PawnInfo>* pawn_hash_table){
	Score total(0,0);
	total += knight_score(WHITE) - knight_score(BLACK);
	total += bishop_score(WHITE) - bishop_score(BLACK);
	total += rook_score(WHITE) - rook_score(BLACK);
	total += queen_score(WHITE) - queen_score(BLACK);
	total += king_score(WHITE) - king_score(BLACK);
	total += pawn_score(WHITE, pawn_hash_table) - pawn_score(BLACK, pawn_hash_table);
	return total;
}
Score Position::calculate_threats(Color c){
	Score total(0,0);
	Bitboard enemy_pieces = colors[~c] & ~pieces[~c][PAWN];
	Bitboard safe_squares_enemy = 	(info.controlled_twice[~c] & ~info.controlled_twice[c]) | info.controlled_by[~c][PAWN];
	Bitboard defended_enemy_pieces = enemy_pieces & safe_squares_enemy;
	Bitboard weak_enemy_pieces = colors[~c] &~ safe_squares_enemy & info.controlled_squares[c];
	if(defended_enemy_pieces | weak_enemy_pieces){
		Bitboard attacked = (info.controlled_by[c][BISHOP] | info.controlled_by[c][KNIGHT]) & (defended_enemy_pieces | weak_enemy_pieces);
		while(attacked){
			total += THREAT_MINOR_SCORE[piece_type(piece_on(pop_lsb(attacked)))];
		}
		attacked = weak_enemy_pieces & info.controlled_by[c][ROOK];
		while(attacked){
			total += THREAT_ROOK_SCORE[piece_type(piece_on(pop_lsb(attacked)))];
		}
		if(weak_enemy_pieces &info.controlled_by[c][KING]){
			total += THREAT_KING_SCORE;
		}
		Bitboard free_juicers = ~info.controlled_squares[~c] | (enemy_pieces & info.controlled_twice[c]);
		total +=  THREAT_HANGING_PIECE_SCORE * popcount(free_juicers & weak_enemy_pieces);
	}
	Bitboard controlled_juicers = info.controlled_squares[~c] & ~safe_squares_enemy & info.controlled_squares[c]; 
	total +=  THREAT_CONTROLLED_SQUARE_SCORE * popcount(controlled_juicers);
	Bitboard safe_squares_us = ~info.controlled_squares[~c] | info.controlled_squares[c];
	Bitboard pawns = pieces[c][PAWN] & safe_squares_us;
	Bitboard pawn_targets = get_pawn_moves(c, pawns) & enemy_pieces;
	total += THREAT_SAFE_PAWN_ATTACK * popcount(pawn_targets);
	//push squares
	pawns = shift(pieces[c][PAWN], info.push_direction[c]) & ~all_pieces;
	pawns |= (pawns & info.third_rank[c]) &~all_pieces;
	pawns &= ~info.controlled_by[~c][PAWN] & safe_squares_us;
	pawn_targets = get_pawn_moves(c, pawns) & enemy_pieces;
	total += THREAT_PAWN_PUSH_ATTACK * popcount(pawn_targets);
	return total;
}
Score Position::calculate_initiative(Score score, std::vector<PawnInfo>* pawn_hash_table){
	int outflanking = std::abs(file(info.king_squares[WHITE]) - file(info.king_squares[BLACK])) + int(rank(info.king_squares[WHITE]) - rank(info.king_squares[BLACK]));
	Bitboard pawns = pieces[WHITE][PAWN] | pieces[BLACK][PAWN];
	bool flank_pawns = (pawns & QUEEN_SIDE) && (pawns & KING_SIDE);
	bool drawn = outflanking < 0 && !flank_pawns;
	bool infiltration = rank(info.king_squares[WHITE]) > RANK_4 || rank(info.king_squares[BLACK]) < RANK_5;
	PawnInfo* p_info = get_pawn_info(state->pawn_key,pawn_hash_table);
	Score initiative = INITIATIVE_PASSED_PAWN_SCORE * popcount(p_info->passed[WHITE] | p_info->passed[BLACK]) +
					INITIATIVE_PAWN_COUNT_SCORE * popcount(pawns) +
					INITIATIVE_OUTFLANKING_SCORE * outflanking +
					INITIATIVE_FLANK_PAWNS_SCORE * flank_pawns +
					INITIATIVE_INFILTRATION_SCORE * infiltration +
					INITIATIVE_DRAWN_SCORE * drawn +
					INITIATIVE_BALANCING_SCORE;

	int middle = score.middle_game;
	int end = score.end_game;
	int m_bonus = ((middle > 0 ) - (middle < 0)) * initiative.middle_game;
	int e_bonus = ((end > 0 ) - (end < 0)) *initiative.end_game;
	middle = middle > 0 ? std::max(0, middle + m_bonus) : std::min(0, middle + m_bonus);
	end = end > 0 ? std::max(0, end + e_bonus) : std::min(0, end + e_bonus);
	return Score(middle, end);
}
Score Position::calculate_space(Color c, PawnInfo* p_info){
    Bitboard center = CENTRAL_FILES;
	center |= c == WHITE ? (BB_RANKS[RANK_2] | BB_RANKS[RANK_3] | BB_RANKS[RANK_4]) : (BB_RANKS[RANK_7] | BB_RANKS[RANK_6] | BB_RANKS[RANK_5]);
	Bitboard safe = center & ~pieces[c][PAWN] & ~info.controlled_by[~c][PAWN];
    Bitboard pawn_space = pieces[c][PAWN];
	pawn_space |= shift(pawn_space,info.push_direction[~c]);
	pawn_space |= shift(pawn_space,info.push_direction[~c]);
	int safe_count = popcount(safe) + popcount(safe & pawn_space & ~info.controlled_squares[~c]);
    int relevancy = popcount(all_pieces) + p_info->blocked;
    return PAWN_SPACE_SCORE * (safe_count * relevancy / 10);
}
Score Position::calculate_score(std::vector<PawnInfo>* pawn_hash_table) {
	eval_init();
	Score total(0,0);
	total += calculate_material(pawn_hash_table);
	total += calculate_threats(WHITE) - calculate_threats(BLACK);
	return calculate_initiative(total, pawn_hash_table);
}

Value Position::interpolate_score(Score score) {
	return ((score.middle_game * (256 - info.phase)) + (score.end_game * info.phase)) / 256;
}

Value Position::evaluate(std::vector<PawnInfo>* pawn_hash_table) {
	Bitboard before = all_pieces;
	Score s = calculate_score(pawn_hash_table);
	if (all_pieces != before) {
		std::cout << "ALERT" << std::endl;
		exit(1);
	}
	return interpolate_score(s);
}