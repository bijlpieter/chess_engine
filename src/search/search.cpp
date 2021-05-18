#include "types.h"
#include "move_generation.h"
#include "uci.h"

Value SearchThread::search(Value alpha, Value beta, Depth depth) {
	return Value(VALUE_DRAW);
}

uint64_t SearchThread::mp_perft(int depth) {
	if (depth < 1)
		return 1ULL;

	MovePick mp = MovePick(this, NULL_MOVE, NULL_MOVE, NULL_MOVE, NULL_MOVE, 0);

	uint64_t nodes = 0;
	Move move;
	while ((move = mp.next_move(false, false)) != NULL_MOVE) {
		// std::cout << move_notation(*pos, move) << std::endl;
		// output << indent << move_notation(*this, legal[i]) << std::endl;
		PositionInfo info = {0};
		pos->play_move(move, &info);
		// output << *this << std::endl;
		nodes += mp_perft(depth - 1);
		pos->unplay_move(move);
	}

	return nodes;
}