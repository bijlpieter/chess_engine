#include "types.h"
#include "move_generation.h"
#include "uci.h"

#include <fstream>
#include <algorithm>

std::ofstream out("moves.txt");

Value SearchThread::qsearch(Value alpha, Value beta) {
	int ply = pos->ply;
	pvTableLen[ply] = 0;

	if (pos->state->rule50 >= 100 || pos->insufficient_material() || pos->is_repetition())
		return VALUE_DRAW;

	return VALUE_DRAW;
}

Value SearchThread::search(Value alpha, Value beta, Depth depth) {
	return Value(VALUE_DRAW);
}

void SearchThread::start() {
	Value last_score = 0, score = 0;
	Move best_move = NULL_MOVE;

	// Iterative deepening
	for (Depth depth = 1; depth < MAX_DEPTH; depth++) {
		Value window = 10, alpha, beta;
		if (depth >= 6) {
			alpha = std::max(-VALUE_INIFINITY, last_score - window);
			beta = std::min(VALUE_INIFINITY, last_score + window);
		}
		else {
			alpha = -VALUE_INIFINITY;
			beta = VALUE_INIFINITY;
		}

		while (true) {
			score = search(alpha, beta, depth);

			if (-VALUE_INIFINITY < score && score <= alpha) {
				beta = (beta + alpha) / 2;
				alpha = std::max(-VALUE_INIFINITY, alpha - window);
			}
			else if (beta <= score && score < VALUE_INIFINITY) {
				beta = std::min(VALUE_INIFINITY, beta + window);
			}
			else {
				if (pvTableLen[0])
					best_move = pvTable[0][0];
				break;
			}

			window += window / 2;
		}

		std::cout << "Depth: " << depth << " best move: " << move_notation(*pos, best_move) << std::endl;
	}
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