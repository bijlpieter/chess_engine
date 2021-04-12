#include "types.h"
#include "move_generation.h"
#include "uci.h"
#include <fstream>

std::ofstream output("moves.txt");

uint64_t Position::perft(int depth) {
	if (depth < 1)
		return 1ULL;

	Moves legal = legal_moves();
	MoveCount num = legal.size();

	uint64_t nodes = 0;
	for (int i = 0; i < num; i++) {
		// output << indent << move_notation(*this, legal[i]) << std::endl;
		PositionInfo info = {0};
		play_move(legal[i], &info);
		// output << *this << std::endl;
		uint64_t count = perft(depth - 1);
		nodes += count;
		unplay_move(legal[i]);
	}

	return nodes;
}