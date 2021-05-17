#include "move_pick.h"
#include "move_generation.h"

#include <cstring>

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

MovePick::MovePick(const Position* pos, Move ttm, Move k1, Move k2, Move p_counter, Value threshold) {
	see_threshold = threshold;
	position = pos;
	ttMove = ttm;
	counter = p_counter;
	killer1 = k1;
	killer2 = k2;
	nCaptures = nQuiets = nBadCaptures = 0;
	memset(scores, 0, sizeof(scores));
}

int MovePick::best_index(MoveCount len) {
	int best = 0;
	for (int i = 0; i < len; i++)
		if (scores[i] > scores[best])
			best = i;
	return best;
}

Move MovePick::next_move(bool skipQuiet) {
start:
	switch(stage) {
	case STAGE_TABLE_LOOKUP:
		++stage;
		return ttMove;
	case STAGE_GENERATE_CAPTURES:
		position->generate_captures(captures.end);
		nCaptures = captures.size();
		for (int i = 0; i < nCaptures; i++) {
			Move m = captures[i];
			PieceType moved = piece_type(position->piece_on(move_from(m)));
			PieceType capped = (move_type(m) == S_MOVE_EN_PASSANT) ? PAWN : piece_type(position->piece_on(move_to(m)));
			Value v = capture_values[moved][capped];
			if (move_type(m) == S_MOVE_PROMOTION)
				v = v + (move_promo(m) + KNIGHT) * 10;
			scores[i] = v;
		}
		++stage;

	case STAGE_GOOD_CAPTURES:
		if (nCaptures > 0) {
			int best = best_index(nCaptures);
			Move best_move = captures[best];

			if (scores[best] >= 0) {
				if (!position->static_exchange_evaluation(best_move, see_threshold)) {
					*bad_captures.end++ = best_move;
					scores[best] = -1;
					goto start;
				}

				nCaptures--;
				captures.end--;
				captures[best] = captures[nCaptures];
				scores[best] = scores[nCaptures];

				if (best == ttMove)
					goto start;
				return best;
			}

			if (skipQuiet) {
				stage = STAGE_BAD_CAPTURES;
				goto start;
			}
			++stage;
		}
	
	case STAGE_KILLER_1:
		++stage;
		if (!skipQuiet && killer1 && killer1 != ttMove)
			return killer1;

	case STAGE_KILLER_2:
		++stage;
		if (!skipQuiet && killer2 && killer2 != ttMove)
			return killer2;

	case STAGE_COUNTER:
		++stage;
		if (!skipQuiet && counter && counter != ttMove && counter != killer1 && counter != killer2)
			return counter;

	case STAGE_GENERATE_QUIETS:
		// CONTINUE HERE:::::
		
		// position->generate_quiets(quiets.end);
		// nQuiets = quiets.size();
		// int ply = position->ply;

		// uint16_t counterMove = (ply >= 1 ? searcher->Stack[ply - 1].move : NULLMOVE), followMove = (ply >= 2 ? searcher->Stack[ply - 2].move : NULLMOVE);
		// int counterPiece = (ply >= 1 ? searcher->Stack[ply - 1].piece : 0), followPiece = (ply >= 2 ? searcher->Stack[ply - 2].piece : 0);
		// int counterTo = sqTo(counterMove), followTo = sqTo(followMove);

	case STAGE_QUIETS: 
	case STAGE_BAD_CAPTURES: 
	case STAGE_DONE:
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