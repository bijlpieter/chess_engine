#include "types.h"
#include "move_generation.h"
#include "uci.h"
#include <fstream>

std::ofstream output("moves.txt");

uint64_t perft(Position& pos, int depth) {
	if (depth < 1)
		return 1ULL;

	Moves legal = pos.turn == WHITE ? legal_moves<WHITE>(pos) : legal_moves<BLACK>(pos);
	MoveCount num = legal.size();

	uint64_t nodes = 0;
	for (int i = 0; i < num; i++) {
		// output << indent << move_notation(*this, legal[i]) << std::endl;
		PositionInfo info = {0};
		pos.play_move(legal[i], &info);
		// output << *this << std::endl;
		nodes += perft(pos, depth - 1);
		pos.unplay_move(legal[i]);
	}

	return nodes;
}

uint64_t divide(Position& pos, int depth) {
	if (depth < 1)
		return 1ULL;

	Moves legal = pos.turn == WHITE ? legal_moves<WHITE>(pos) : legal_moves<BLACK>(pos);
	MoveCount num = legal.size();

	uint64_t nodes = 0;
	for (int i = 0; i < num; i++) {
		// output << indent << move_notation(*this, legal[i]) << std::endl;
		PositionInfo info = {0};
		pos.play_move(legal[i], &info);
		// output << *this << std::endl;
		uint64_t count = perft(pos, depth - 1);
		nodes += count;
		pos.unplay_move(legal[i]);
		output << move_notation(pos, legal[i]) << ": " << count << std::endl;
	}

	return nodes;
}