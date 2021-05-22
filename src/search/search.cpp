#include "types.h"
#include "move_generation.h"
#include "uci.h"
#include "history.h"

#include <fstream>
#include <algorithm>

std::ofstream out("moves.txt");

//
// Search algorithms qsearch and search
// adapted from CloverEngine by Luca Metehau.
// https://github.com/lucametehau/CloverEngine
//

Value SearchThread::qsearch(Value alpha, Value beta) {
	// std::cout << "entered quiescence search, alpha: " << int(alpha) << " beta: " << int(beta) << std::endl;
	int ply = pos->ply;
	pvTableLen[ply] = 0;

	if (pos->is_draw())
		return VALUE_DRAW;

	Key k = pos->state->position_key;
	Value eval = VALUE_INFINITY, score = 0, best_score = -VALUE_INFINITY, alpha_orig = alpha;
	Bound bound = NO_BOUND;
	Move best_move = NULL_MOVE;

	TTEntry entry = {0};

	// std::cout << "Attempting to probe TT...";
	if (tt.probe(k, entry)) {
		// std::cout << "   found!" << std::endl;
		eval = entry.info.eval;
		score = entry.value(ply);
		// std::cout << int(score) << std::endl;
		bound = entry.bound();
		if (bound == EXACT_BOUND || (bound == LOWER_BOUND && score >= beta) || (bound == UPPER_BOUND && score <= alpha)) {
			// std::cout << "exit qsearch after tt hit" << std::endl;
			return score;
		}
	}
	// std::cout << "   missed!" << std::endl;

	if (eval == VALUE_INFINITY)
		eval = (stack[ply - 1].move == NULL_MOVE ? -stack[ply - 1].eval + 2 * VALUE_TEMPO : pos->evaluate(&pawn_hash_table));

	stack[ply].eval = eval;

	if (eval >= beta) {
		std::cout << "exit qsearch after beta cut-off" << std::endl;
		std::cout << "beta: " << int(beta) << " eval: " << int(eval) << std::endl;
		return eval;
	}

	alpha = std::max(alpha, eval);
	best_score = eval;

	MovePick mp = MovePick(this, NULL_MOVE, NULL_MOVE, NULL_MOVE, NULL_MOVE, VALUE_DRAW);

	Move move = NULL_MOVE;
	while ((move = mp.next_move(true, true)) != NULL_MOVE) {
		std::cout << "stack ply" << std::endl;
		stack[ply].move = move;
		stack[ply].piece = pos->piece_on(move_from(move));
		std::cout << "iterative qsearch" << std::endl;

		PositionInfo info = {0};

		std::cout << *pos << std::endl;
		std::cout << pos->turn << std::endl;
		// std::cout << bb_string(pos->all_pieces) << std::endl;

		pos->play_move(move, &info);

		std::cout << "played move" << std::endl;
		
		score = -qsearch(-beta, -alpha);
		pos->unplay_move(move);

		if (score > best_score) {
			best_score = score;
			best_move = move;

			if (score > alpha) {
				alpha = score;
				update_pv(move, ply);

				if (alpha >= beta)
					break;
			}
		}
	}

	bound = (best_score >= beta ? LOWER_BOUND : (best_move > alpha_orig ? EXACT_BOUND : UPPER_BOUND));
	tt.save(k, best_score, eval, 0, ply, bound, best_move);

	std::cout << "exit qsearch best score: " << int(best_score) << std::endl;
	return best_score;
}

Value SearchThread::search(Value alpha, Value beta, Depth depth, Move excluded) {
	// std::cout << "entered search, alpha: " << int(alpha) << " beta: " << int(beta) << " depth: " << int(depth) << std::endl;

	// if (popcount(pos->all_pieces) != 32) {
	// 	std::cout << bb_string(pos->all_pieces) << std::endl;
	// 	std::cout << int(pos->piece_on(A3)) << std::endl;
	// 	std::cout << bb_string(pos->colors[WHITE]) << std::endl;

	// 	std::cout << *pos << std::endl;
	// 	exit(1);
	// }
	
	int ply = pos->ply;
	bool pv_node = (alpha < beta - 1), root_node = (ply == 0);
	Move tt_move = NULL_MOVE;
	
	Value alpha_orig = alpha;
	Key key = pos->state->position_key;
	Move quiets[256];
	MoveCount nrQuiets = 0;

	// if(checkForStop())
	// 	return ABORT;

	int played = 0;
	Bound bound = NO_BOUND;
	bool skip = false;
	Value best = -VALUE_INFINITY;
	Move best_move = NULL_MOVE;
	int ttHit = 0, ttValue = 0;

	if (depth <= 0) {
		std::cout << "depth is zero, diving into quiescence search" << std::endl;
		return qsearch(alpha, beta);
	}

	nodes++;
	// selDepth = std::max(selDepth, ply);

	// std::cout << "prefetching transposition table....         ";
	tt.prefetch(key);
	pvTableLen[ply] = 0;

	if (!root_node) {
		// std::cout << "no root node" << std::endl;
		if (pos->is_draw())
			return VALUE_DRAW;
		Value rAlpha = std::max(alpha, Value(-VALUE_INFINITY + ply)), rBeta = std::min(beta, Value(VALUE_INFINITY - ply - 1));
		if (rAlpha >= rBeta)
			return rAlpha;
	}

	

	// Transposition table probing
	Value eval = VALUE_INFINITY;
	TTEntry entry = {};

	// std::cout << "probing tt .....      ";
	if (excluded == NULL_MOVE && tt.probe(key, entry)) {
		int score = entry.value(ply);
		ttHit = 1;
		ttValue = score;
		bound = entry.bound(), tt_move = entry.info.move;
		eval = entry.info.eval;
		if (entry.depth() >= depth && !pv_node) {
			if (bound == EXACT_BOUND || (bound == LOWER_BOUND && score >= beta) || (bound == UPPER_BOUND && score <= alpha))
				return score;
		}
	}

	// std::cout << "finished probing" << std::endl;

	// No need to evaluate if last move was null
	if (eval == VALUE_INFINITY) {
		// std::cout << "getting evaluation in search" << std::endl;
		stack[ply].eval = eval = (ply >= 1 && stack[ply - 1].move == NULL_MOVE ? -stack[ply - 1].eval + 2 * VALUE_TEMPO : pos->evaluate(&pawn_hash_table));
	}
	else {
		// ttValue might be a better evaluation
		stack[ply].eval = eval;

		if(bound == EXACT_BOUND || (bound == LOWER_BOUND && ttValue > eval) || (bound == UPPER_BOUND && ttValue < eval))
			eval = ttValue;
	}

	bool isCheck = bool(pos->state->checkers), improving = (ply >= 2 && eval > stack[ply - 2].eval);

	killers[ply + 1][0] = killers[ply + 1][1] = NULL_MOVE;

	/// razoring

	// if (!pv_node && !isCheck && depth <= 1 && eval + 325 < alpha) {/// ctt says this is the best
	// 	std::cout << "razoring!" << std::endl;
	// 	return qsearch(alpha, beta);
	// }

	/// static null move pruning

	// if (!pv_node && !isCheck && depth <= 8 && eval - 85 * depth > beta) /// same here
	// 	return eval;

	/// null move pruning

	// if (!pv_node && !isCheck && eval >= beta && depth >= 2 && stack[ply - 1].move && (pos->colors[pos->turn] ^ pos->pieces[pos->turn][PAWN] ^ pos->pieces[pos->turn][KING]) && (!ttHit || !(bound & UPPER_BOUND) || ttValue >= beta)) {
	// 	int R = 4 + depth / 6 + std::min(3, (eval - beta) / 200);

	// 	stack[ply].move = NULL_MOVE;
	// 	stack[ply].piece = NO_PIECE;

	// 	std::cout << "null move pruning!" << std::endl;

	// 	PositionInfo info;
	// 	pos->play_null_move(&info);
	// 	int score = -search(-beta, -beta + 1, depth - R);
	// 	pos->unplay_null_move();

	// 	if(score >= beta)
	// 		return (abs(score) > VALUE_MATE ? beta : score);
	// }

	/// probcut

	// if (!pv_node && !isCheck && depth >= 5 && abs(beta) < VALUE_MATE) {
	// 	std::cout << "probcut!" << std::endl;
	// 	int cutBeta = beta + 100;
	// 	MovePick noisyPicker(this, NULL_MOVE, NULL_MOVE, NULL_MOVE, NULL_MOVE, cutBeta - eval);

	// 	uint16_t move;

	// 	while ((move = noisyPicker.next_move(true, true)) != NULL_MOVE) {
	// 		if (move == excluded)
	// 			continue;

	// 		stack[ply].move = move;
	// 		stack[ply].piece =pos->piece_on(move_from(move));

	// 		PositionInfo info;
	// 		Bitboard before = pos->all_pieces;
	// 		pos->play_move(move, &info);

	// 		/// do we have a good sequence of captures that beats cutBeta ?

	// 		int score = -qsearch(-cutBeta, -cutBeta + 1);

	// 		if (score >= cutBeta) /// then we should try searching this capture
	// 			score = -search(-cutBeta, -cutBeta + 1, depth - 4);

	// 		pos->unplay_move(move);

	// 		if (pos->all_pieces != before) {
	// 			std::cout << "ALERT PROBCUT" << std::endl;
	// 			exit(1);
	// 		}

	// 		if(score >= cutBeta)
	// 			return score;
	// 	}
	// }

	/// get counter move for move picker

	Move counter = (ply == 0 || stack[ply - 1].move == NULL_MOVE ? NULL_MOVE : cmTable[~pos->turn][stack[ply - 1].piece][move_to(stack[ply - 1].move)]);

	MovePick picker(this, tt_move, killers[ply][0], killers[ply][1], counter, 0);

	Move move;

	while ((move = picker.next_move(skip, false)) != NULL_MOVE) {
		if (move == excluded)
			continue;

		bool isQuiet = !pos->is_capture(move);
		History :: Heuristics H{}; /// history values for quiet moves

		/// quiet move pruning
		// if (!root_node && best > -VALUE_MATE) {
		// 	if (isQuiet) {
		// 		std::cout << "quiet move pruning!" << std::endl;
		// 		History :: getHistory(this, move, ply, H);
		// 		std::cout << "done getting history!" << std::endl;

		// 		//cout << h << " " << ch << " " << fh << "\n";

		// 		/// counter move and follow move pruning

		// 		if(depth <= cmpDepth[improving] && H.ch < cmpHistoryLimit[improving])
		// 			continue;

		// 		if(depth <= fmpDepth[improving] && H.fh < fmpHistoryLimit[improving])
		// 			continue;

		// 		/// futility pruning
		// 		if(depth <= 8 && eval + 90 * depth <= alpha && H.h + H.ch + H.fh < fpHistoryLimit[improving])
		// 			skip = 1;

		// 		/// late move pruning
		// 		if(depth <= 8 && nrQuiets >= lmrCnt[improving][depth])
		// 			skip = 1;
		// 	}

		// 	/// see pruning (to do: tune coefficients)

		// 	if (depth <= 8 && !isCheck) {
		// 		std::cout << "see pruning!" << std::endl;
		// 		int seeMargin[2];

		// 		seeMargin[1] = -80 * depth;
		// 		seeMargin[0] = -18 * depth * depth;

		// 		if(!pos->static_exchange_evaluation(move, seeMargin[isQuiet]))
		// 			continue;
		// 	}
		// }

		// bool ex = false;

		/// singular extension (we look for other moves, for move diversity)

		// if (!root_node && !excluded && move == tt_move && abs(ttValue) < VALUE_MATE && depth >= 8 && entry.depth() >= depth - 3 && (bound & LOWER_BOUND)) { /// had best instead of ttValue lol
		// 	int rBeta = ttValue - depth;
		// 	//cout << "Entering singular extension with ";
		// 	//cout << "depth = " << depth << ", alpha = " << alpha << ", beta = " << beta << "\n";
		// 	//board.print();

		// 	std::cout << "singular extension!" << std::endl;

		// 	int score = search(rBeta - 1, rBeta, depth / 2, move);

		// 	if (score < rBeta)
		// 		ex = true;
		// 	else if (rBeta >= beta)
		// 		return rBeta;
		// }
		// else {
		// 	ex = isCheck || (isQuiet && pv_node && H.ch >= 10000 && H.fh >= 10000); /// in check extension and moves with good history
		// }

		/// update stack info
		stack[ply].move = move;
		stack[ply].piece = pos->piece_on(move_from(move));

		PositionInfo info;

		// std::cout << "we play the move: " << move_notation(*pos, move) << std::endl;

		pos->play_move(move, &info);
		played++;

		/// current root move info

		// if (root_node && principalSearcher && getTime() > info->startTime + 2500) {
		// std::cout << "info depth " << depth << " currmove " << toString(move) << " currmovenumber " << played << std::endl;
		/*if(move == hashMove)
		std::cout << "hashMove\n";
		else if(move == killers[ply][0])
		std::cout << "killer1\n";
		else if(move == killers[ply][1])
		std::cout << "killer2\n";
		else if(move == counter)
		std::cout << "counter\n";
		else
		std::cout << "normal\n";*/
		// }

		/// store quiets for history

		if(isQuiet)
			quiets[nrQuiets++] = move;

		// int newDepth = depth + (ex && !root_node),
		int newDepth = depth, R = 1;

		/// quiet late move reduction

		// if (isQuiet && depth >= 3 && played > 1 + 2 * root_node) { /// first few moves we don't reduce
		// 	R = lmrRed[std::min(Depth(63), depth)][std::min(63, played)];

		// 	R += !pv_node + !improving; /// not on pv or not improving

		// 	R += isCheck && piece_type(pos->piece_on(move_to(move))) == KING; /// check evasions

		// 	R -= picker.stage < STAGE_QUIETS; /// refutation moves

		// 	R -= std::max(-2, std::min(2, (H.h + H.ch + H.fh) / 5000)); /// reduce based on move history

		// 	R = std::min(depth - 1, std::max(R, 1)); /// clamp depth
		// }

		int score = -VALUE_INFINITY;

		/// principal variation search

		// std::cout << "pvs!" << std::endl;

		// if (R != 1) {
		// 	// std::cout << "PVS1" << std::endl;
		// 	score = -search(-alpha - 1, -alpha, newDepth - R);
		// }

		// if ((R != 1 && score > alpha) || (R == 1 && !(pv_node && played == 1))) {
		// 	// std::cout << "PVS2" << std::endl;
		// 	score = -search(-alpha - 1, -alpha, newDepth - 1);
		// }

		// if (pv_node && (played == 1 || score > alpha)) {
			// std::cout << "PVS3" << std::endl;
			score = -search(-beta, -alpha, newDepth - 1);
		// }

		// std::cout << "PVS returned!" << std::endl;

		pos->unplay_move(move);

		if (score > best) {
			best = score;
			best_move = move;

			if (score > alpha) {
				alpha = score;
				update_pv(move, ply);

				if (alpha >= beta)
					break;
			}
		}
	}

	// std::cout << "checked all moves in search!" << std::endl;

	tt.prefetch(key);

	if (!played)
		return (isCheck ? -VALUE_MATE + ply : VALUE_DRAW);

	/// update killers and history heuristics

	if (best >= beta && !pos->is_capture(best_move)) {
		if(killers[ply][0] != best_move) {
			killers[ply][1] = killers[ply][0];
			killers[ply][0] = best_move;
		}
		History :: updateHistory(this, quiets, nrQuiets, ply, depth * depth);
	}

	/// update tt only if we aren't in a singular search

	if (excluded == NULL_MOVE) {
		bound = (best >= beta ? LOWER_BOUND : (best > alpha_orig ? EXACT_BOUND : UPPER_BOUND));
		tt.save(key, best, eval, depth, ply, bound, best_move);
	}

	return best;
}

void SearchThread::start() {
	Value last_score = 0, score = 0;
	Move best_move = NULL_MOVE;

	// Iterative deepening

	for (Depth depth = 1; depth < MAX_DEPTH; depth++) {
		std::cout << "entered iterative deepening loop: depth: " << int(depth) << std::endl;
		Value window = 10, alpha, beta;
		if (depth >= 6) {
			alpha = std::max(-VALUE_INFINITY, last_score - window);
			beta = std::min(VALUE_INFINITY, last_score + window);
		}
		else {
			alpha = -VALUE_INFINITY;
			beta = VALUE_INFINITY;
		}

		while (true) {
			// std::cout << "started search" << std::endl;
			score = search(alpha, beta, depth);
			// std::cout << "search returned: " << int(score) << std::endl;

			if (-VALUE_INFINITY < score && score <= alpha) {
				beta = (beta + alpha) / 2;
				alpha = std::max(-VALUE_INFINITY, alpha - window);
			}
			else if (beta <= score && score < VALUE_INFINITY) {
				beta = std::min(VALUE_INFINITY, beta + window);
			}
			else {
				if (pvTableLen[0])
					best_move = pvTable[0][0];
				break;
			}

			window += window / 2;
		}

		std::cout << "Depth: " << int(depth) << " best move: " << move_notation(*pos, best_move) << std::endl;
	}
}

void SearchThread::update_pv(Move m, int ply) {
	pvTable[ply][0] = m;
	for (int i = 0; i < pvTableLen[ply + 1]; i++)
		pvTable[ply][i + 1] = pvTable[ply + 1][i];
	pvTableLen[ply] = 1 + pvTableLen[ply + 1];
}

uint64_t SearchThread::mp_perft(int depth) {
	if (depth < 1)
		return 1ULL;

	MovePick mp = MovePick(this, NULL_MOVE, NULL_MOVE, NULL_MOVE, NULL_MOVE, 0);

	uint64_t nodes = 0;
	Move move;
	while ((move = mp.next_move(false, false)) != NULL_MOVE) {
		// std::cout << move_notation(*pos, move) << std::endl;
		// output << move_notation(*this, legal[i]) << std::endl;
		PositionInfo info = {0};
		pos->play_move(move, &info);
		// output << *this << std::endl;
		nodes += mp_perft(depth - 1);
		pos->unplay_move(move);
	}

	return nodes;
}

uint64_t SearchThread::mp_divide(int depth) {
	if (depth < 1)
		return 1ULL;

	MovePick mp = MovePick(this, NULL_MOVE, NULL_MOVE, NULL_MOVE, NULL_MOVE, 0);

	uint64_t nodes = 0;
	Move move;
	while ((move = mp.next_move(false, false)) != NULL_MOVE) {
		// output << indent << move_notation(*this, legal[i]) << std::endl;
		PositionInfo info;
		pos->play_move(move, &info);
		// output << *this << std::endl;
		uint64_t count = mp_perft(depth - 1);
		nodes += count;
		pos->unplay_move(move);
		out << move_notation(*pos, move) << ": " << count << std::endl;
	}

	return nodes;
}