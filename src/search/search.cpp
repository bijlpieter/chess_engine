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
	Value stand_pat = pos->evaluate(&pawn_hash_table);
	
	if (stand_pat >= beta)
        return beta;

    if (alpha < stand_pat) {
        alpha = stand_pat;
	}

	MovePick mp = MovePick(VALUE_DRAW);
	Move move = NULL_MOVE;
    while ((move = mp.next_move(this, true, true)) != NULL_MOVE) {
		PositionInfo info;
        pos->play_move(move, &info);
        Value score = -qsearch(-beta, -alpha);
        pos->unplay_move(move);

        if (score >= beta)
            return beta;
        if (score > alpha)
           alpha = score;
    }
    return alpha;
}

Value SearchThread::search(Value alpha, Value beta, Depth depth) {
	// std::cout << "entered search, alpha: " << int(alpha) << " beta: " << int(beta) << " depth: " << int(depth) << std::endl;

	int played = 0;

	if (depth == 0)
		return qsearch(alpha, beta);

	Move move;
	MovePick mp = MovePick(VALUE_DRAW);
	while ((move = mp.next_move(this, false, false)) != NULL_MOVE) {
		PositionInfo info = {0};
		played++;

		pos->play_move(move, &info);
		Value score = -search(-beta, -alpha, depth - 1);
		pos->unplay_move(move);

		if (score >= beta)
			return beta;   //  fail hard beta-cutoff
		if (score > alpha)
			alpha = score; // alpha acts like max in MiniMax
	}

	if (!played)
		return pos->state->checkers ? -VALUE_MATE : VALUE_DRAW;

	return alpha;
}

void SearchThread::start() {
	Value score = 0;
	// Move best_move = NULL_MOVE;

	// Iterative deepening

	for (search_depth = 1; search_depth < MAX_DEPTH; search_depth++) {
		std::cout << "entered iterative deepening loop: depth: " << int(search_depth) << std::endl;
		Value alpha = -VALUE_INFINITY;
		Value beta = VALUE_INFINITY;

		// std::cout << "started search" << std::endl;
		score = search(alpha, beta, search_depth);
		std::cout << "search returned: " << int(score) << std::endl;

		// if (pvTableLen[0])
		// 	best_move = pvTable[0][0];

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

	MovePick mp = MovePick(0);

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

	MovePick mp = MovePick(0);

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