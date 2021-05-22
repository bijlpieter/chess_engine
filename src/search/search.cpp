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
	nodes++;

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
    	eval = pos->evaluate(&pawn_hash_table);

	if (eval >= beta)
		return beta;

	alpha = std::max(alpha, eval);
	best_score = eval;

	MovePick mp = MovePick(NULL_MOVE, VALUE_DRAW);

	// std::cout << "movepicking in qsearch" << std::endl;

	Move move = NULL_MOVE;
	while ((move = mp.next_move(this, true, true)) != NULL_MOVE) {
		PositionInfo info = {0};

		if (move == move_init(E1, F2)) {
			std::cout << *pos << std::endl;
			std::cout << mp.stage << std::endl;
			// std::cout << pos->piece_on(F2) << std::endl;
			// std::cout << pos->piece_on(H3) << std::endl;
			Moves m;
			pos->generate_captures(m.end);
			for (int i = 0; i < m.size(); i++)
				std::cout << sq_notation(move_from(m[i])) << sq_notation(move_to(m[i])) << std::endl;
		}

		// std::cout << pos->turn << std::endl;
		// std::cout << bb_string(pos->all_pieces) << std::endl;

		// std::cout << "play move" << std::endl;
		// std::cout << *pos << std::endl;

		// for (PositionInfo* pi = pos->state; pi->previous; pi = pi->previous)
		// 	std::cout << sq_notation(move_from(pi->last_move)) << sq_notation(move_to(pi->last_move)) << std::endl;

		pos->play_move(move, &info);
		// std::cout << "played move qs" << std::endl;
		score = -qsearch(-beta, -alpha);
		pos->unplay_move(move);
		// std::cout << "unplayed move qs" << std::endl;

		if (score > best_score) {
			best_score = score;
			best_move = move;

			if (score > alpha) {
				alpha = score;
				update_pv(ply, move);

				if (alpha >= beta)
					break;
			}
		}
	}

	// std::cout << "checked all captures qs" << std::endl;

	bound = (best_score >= beta ? LOWER_BOUND : (best_move > alpha_orig ? EXACT_BOUND : UPPER_BOUND));
	tt.save(k, best_score, eval, 0, ply, bound, best_move);

	// std::cout << "exit qsearch best score: " << int(best_score) << std::endl;
	return best_score;
}

Value SearchThread::search(Value alpha, Value beta, Depth depth) {
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
	Value best_score = -VALUE_INFINITY;
	Move best_move = NULL_MOVE;
	int ttHit = 0, ttValue = 0;

	if (depth <= 0) {
		// std::cout << "depth is zero, diving into quiescence search" << std::endl;
		return qsearch(alpha, beta);
	}

	nodes++;
	// selDepth = std::max(selDepth, ply);

	// std::cout << "prefetching transposition table....         ";
	tt.prefetch(key);
	// std::cout << "update pvlen" << std::endl;
	pvTableLen[ply] = 0;
	
	// std::cout << "checking for draw" << std::endl;
	if (pos->is_draw())
		return VALUE_DRAW;

	// Transposition table probing
	Value eval = VALUE_INFINITY;
	TTEntry entry = {};

	// std::cout << "probing tt .....      ";
	if (tt.probe(key, entry)) {
		int score = entry.value(ply);
		ttHit = 1;
		ttValue = score;
		bound = entry.bound();
		tt_move = entry.info.move;
		eval = entry.info.eval;
		if (entry.depth() >= depth && !pv_node) {
			if (bound == EXACT_BOUND || (bound == LOWER_BOUND && score >= beta) || (bound == UPPER_BOUND && score <= alpha))
				return score;
		}
	}

	// std::cout << "finished probing" << std::endl;
	Move move;
	MovePick mp = MovePick(tt_move, 0);
	while ((move = mp.next_move(this, false, false)) != NULL_MOVE) {
		PositionInfo info = {0};
		played++;
		pos->play_move(move, &info);
		Value score = -search(-beta, -alpha, depth - 1);
		pos->unplay_move(move);

		if (score > best_score) {
			best_score = score;
			best_move = move;

			if (score > alpha) {
				alpha = score;
				update_pv(ply, move);

				if (alpha >= beta)
					break;
			}
		}
	}

	if (!played)
		return pos->state->checkers ? -VALUE_MATE + ply : VALUE_DRAW;

	bound = (best_score >= beta ? LOWER_BOUND : (best_move > alpha_orig ? EXACT_BOUND : UPPER_BOUND));
	// std::cout << "saving in tt" << std::endl;
	tt.save(key, best_score, eval, depth, ply, bound, best_move);

	return alpha;
}

void SearchThread::start() {
	Value last_score = 0, score = 0;
	Move best_move = NULL_MOVE;

	// Iterative deepening

	for (search_depth = 1; search_depth < MAX_DEPTH; search_depth++) {
		std::cout << "entered iterative deepening loop: depth: " << int(search_depth) << std::endl;
		Value alpha = -VALUE_INFINITY;
		Value beta = VALUE_INFINITY;

		// std::cout << "started search" << std::endl;
		score = search(alpha, beta, search_depth);
		// std::cout << "search returned: " << int(score) << std::endl;

		if (pvTableLen[0])
			best_move = pvTable[0][0];

		// std::cout << "Depth: " << int(search_depth) << " best move: " << move_notation(*pos, best_move) << std::endl;
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

	MovePick mp = MovePick(NULL_MOVE, 0);

	uint64_t nodes = 0;
	Move move;
	while ((move = mp.next_move(this, false, false)) != NULL_MOVE) {
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

	MovePick mp = MovePick(NULL_MOVE, 0);

	uint64_t nodes = 0;
	Move move;
	while ((move = mp.next_move(this, false, false)) != NULL_MOVE) {
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