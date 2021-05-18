#include "move_pick.h"
#include "move_generation.h"
#include "uci.h"

#include <cstring>
#include <iostream>

const Value capture_values[NUM_PIECE_TYPES][NUM_PIECE_TYPES] = {
	{9, 19, 29, 39, 49, 59},
	{8, 18, 28, 38, 48, 58},
	{7, 17, 27, 37, 47, 57},
	{6, 16, 26, 36, 46, 56},
	{5, 15, 25, 35, 45, 55},
	{4, 14, 24, 34, 44, 54},
};

const Value static_exchange_values[NUM_PIECE_TYPES] = {
	100, 310, 330, 500, 900, 6969
};

MovePick::MovePick(const SearchThread* st, Move ttm, Move k1, Move k2, Move p_counter, Value threshold) {
	see_threshold = threshold;
	search = st;
	ttMove = ttm;
	counter = p_counter;
	killer1 = k1;
	killer2 = k2;
	nCaptures = nQuiets = nBadCaptures = 0;
	stage = STAGE_TABLE_LOOKUP;
	memset(scores, 0, sizeof(scores));
}

int MovePick::best_index(MoveCount len) {
	int best = 0;
	for (int i = 0; i < len; i++)
		if (scores[i] > scores[best])
			best = i;
	return best;
}

Move MovePick::next_move(bool skipQuiet, bool skipBadCaptures) {
mp_start:
	switch(stage) {
	case STAGE_TABLE_LOOKUP:
		// std::cout << "TABLE_LOOKUP" << std::endl;
		++stage;
		if (ttMove)
			return ttMove;
	case STAGE_GENERATE_CAPTURES:
		// std::cout << "GENERATING CAPTURES..." << std::endl;
		search->pos->generate_captures(captures.end);
		nCaptures = captures.size();
		// std::cout << "GENERATED " << nCaptures << " MOVES" << std::endl;
		for (int i = 0; i < nCaptures; i++) {
			Move m = captures[i];
			PieceType moved = piece_type(search->pos->piece_on(move_from(m)));
			PieceType capped = (move_type(m) == S_MOVE_EN_PASSANT) ? PAWN : piece_type(search->pos->piece_on(move_to(m)));
			Value v = capture_values[moved][capped];
			if (move_type(m) == S_MOVE_PROMOTION)
				v = v + (move_promo(m) + KNIGHT) * 10;
			scores[i] = v;
		}
		++stage;

	case STAGE_GOOD_CAPTURES:
		// std::cout << "GOOD CAPTURES" << std::endl;
		if (nCaptures > 0) {
			int best = best_index(nCaptures);
			if (scores[best] >= 0) {
				Move m = captures[best];
				if (!search->pos->static_exchange_evaluation(m, see_threshold)) {
					*bad_captures.end++ = m;
					nBadCaptures++;
					scores[best] = -1;
					// std::cout << "BAD CAPTURE DETECTED LULW: " << move_notation(*search->pos, m) << std::endl;
					goto mp_start;
				}

				nCaptures--;
				captures.end--;
				captures[best] = captures[nCaptures];
				scores[best] = scores[nCaptures];

				if (m == ttMove)
					goto mp_start;

				// std::cout << "RETURNING A GOOD CAPTURE: " << move_notation(*search->pos, m) << std::endl;
				return m;
			}
		}
		if (skipQuiet) {
			stage = STAGE_BAD_CAPTURES;
			goto mp_start;
		}
		++stage;
	
	case STAGE_KILLER_1:
		// std::cout << "KILLER 1" << std::endl;
		++stage;
		if (!skipQuiet && killer1 && killer1 != ttMove)
			return killer1;

	case STAGE_KILLER_2:
		// std::cout << "KILLER 2" << std::endl;
		++stage;
		if (!skipQuiet && killer2 && killer2 != ttMove)
			return killer2;

	case STAGE_COUNTER:
		// std::cout << "COUNTER" << std::endl;
		++stage;
		if (!skipQuiet && counter && counter != ttMove && counter != killer1 && counter != killer2)
			return counter;

	case STAGE_GENERATE_QUIETS:	
		// std::cout << "GENERATING QUIETS..." << std::endl;
		search->pos->generate_quiets(quiets.end);
		nQuiets = quiets.size();
		// std::cout << "GENERATED " << nQuiets << " MOVES" << std::endl;

		for (int i = 0; i < nQuiets; i++) {
			Move m = quiets[i];
			Value score = 0;

			if (m == ttMove || m == killer1 || m == killer2 || m == counter) {
				score = -6969; // This might instead be possible to remove it from the list: test later
			}
			else {
				int ply = search->pos->ply;
				Move counter_move = (ply >= 1 ? search->stack[ply - 1].move : NULL_MOVE);
				Move follow_move = (ply >= 2 ? search->stack[ply - 2].move : NULL_MOVE);
				Piece counter_piece = (ply >= 1 ? search->stack[ply - 1].piece : NO_PIECE);
				Piece follow_piece = (ply >= 2 ? search->stack[ply - 2].piece : NO_PIECE);
				Square counter_to = move_to(counter_move);
				Square follow_to = move_to(follow_move);
				
				Square m_from = move_from(m);
				Square m_to = move_to(m);
				Piece m_piece = search->pos->piece_on(m_from);

				score = search->history[search->pos->turn][m_from][m_to];

				if (counter_move)
					score += search->follow[0][counter_piece][counter_to][m_piece][m_to];

				if (follow_move)
					score += search->follow[1][follow_piece][follow_to][m_piece][m_to];
			}
			scores[i] = score;
		}
		++stage;

	case STAGE_QUIETS:
		// std::cout << "QUIET MOVE SELECTION" << std::endl;
		if (!skipQuiet && nQuiets) {
			int best = best_index(nQuiets);
			Move m = quiets[best];

			nQuiets--;
			quiets.end--;
			quiets[best] = quiets[nQuiets];
			scores[best] = scores[nQuiets];

			if (m == ttMove || m == killer1 || m == killer2 || m == counter)
				goto mp_start;
			
			// std::cout << "RETURNING A GOOD QUIET: " << move_notation(*search->pos, m) << std::endl;
			return m;
		}
		else
			++stage;

	case STAGE_BAD_CAPTURES:
		// std::cout << "BAD CAPTURE SELECTION" << std::endl;
		if (!skipBadCaptures && nBadCaptures) {
			nBadCaptures--;
			bad_captures.end--; // Todo this line is unnecessary i believe (test to make sure)
			Move m = bad_captures[nBadCaptures];

			if (m == ttMove)
				goto mp_start;
			
			// std::cout << "RETURNING A BAD CAPTURE: " << move_notation(*search->pos, m) << std::endl;
			return m;
		}
		else
			++stage;

	case STAGE_DONE:
		// std::cout << "ALL MOVES GENERATED AND RETURNED... RETURNING NULL_MOVE" << std::endl;
		return NULL_MOVE;
	}

	return NULL_MOVE;
}

// Stockfish's SEE_GE algorithm
bool Position::static_exchange_evaluation(Move m, Value threshold) const {
	// Only deal with normal moves, assume others pass a simple SEE
	if (move_type(m) != S_MOVE_NONE)
		return VALUE_ZERO >= threshold;

	Square from = move_from(m), to = move_to(m);

	int swap = static_exchange_values[piece_on(to)] - threshold;
	if (swap < 0)
		return false;

	swap = static_exchange_values[piece_on(from)] - swap;
	if (swap <= 0)
		return true;

	Bitboard occupied = all_pieces ^ from ^ to;
	Color stm = piece_color(piece_on(from));
	Bitboard attackers = attackers_to_sq(to, WHITE) | attackers_to_sq(to, BLACK);
	Bitboard stmAttackers, bb;
	int res = 1;

	while (true) {
		stm = ~stm;
		attackers &= occupied;
		Bitboard pinned = blockers(lsb(pieces[stm][KING]), stm, ~stm);

		// If stm has no more attackers then give up: stm loses
		if (!(stmAttackers = attackers & colors[stm]))
			break;

		// Don't allow pinned pieces to attack (except the king) as long as
		// there are pinners on their original square.
		stmAttackers &= ~pinned;

		if (!stmAttackers)
			break;

		res ^= 1;

		Bitboard diag_sliders = pieces[WHITE][BISHOP] | pieces[BLACK][BISHOP] | pieces[WHITE][QUEEN] | pieces[BLACK][QUEEN];
		Bitboard hori_sliders = pieces[WHITE][ROOK] | pieces[BLACK][ROOK] | pieces[WHITE][QUEEN] | pieces[BLACK][QUEEN];

		// Locate and remove the next least valuable attacker, and add to
		// the bitboard 'attackers' any X-ray attackers behind it.
		if ((bb = stmAttackers & pieces[stm][PAWN])) {
			if ((swap = static_exchange_values[PAWN] - swap) < res)
				break;

			occupied ^= lsb(bb);
			attackers |= bishop_moves(to, occupied) & diag_sliders;
		}
		else if ((bb = stmAttackers & pieces[stm][KNIGHT])) {
			if ((swap = static_exchange_values[KNIGHT] - swap) < res)
				break;

			occupied ^= lsb(bb);
		}
		else if ((bb = stmAttackers & pieces[stm][BISHOP])) {
			if ((swap = static_exchange_values[BISHOP] - swap) < res)
				break;

			occupied ^= lsb(bb);
			attackers |= bishop_moves(to, occupied) & diag_sliders;
		}
		else if ((bb = stmAttackers & pieces[stm][ROOK])) {
			if ((swap = static_exchange_values[ROOK] - swap) < res)
				break;

			occupied ^= lsb(bb);
			attackers |= rook_moves(to, occupied) & hori_sliders;
		}
		else if ((bb = stmAttackers & pieces[stm][QUEEN])) {
			if ((swap = static_exchange_values[QUEEN] - swap) < res)
				break;

			occupied ^= lsb(bb);
			attackers |= (bishop_moves(to, occupied) & diag_sliders) | (rook_moves(to, occupied) & hori_sliders);
		}
		else // KING: If we "capture" with the king but opponent still has attackers, reverse the result.
			return (attackers & ~colors[stm]) ? res ^ 1 : res;
	}

	return bool(res);
}