#include "types.h"
#include "move_generation.h"
#include "uci.h"

#include <iostream>

int main() {
	bb_init();
	bb_moves_init();
	bb_rays_init();
	std::string test_fen= "1k6/ppp5/8/6N1/5P2/8/PPP5/1K6";
	test_fen += " w KQkq - 0 1";

	PositionInfo info = {0};
	Position pos = Position(&info, test_fen);
	std::cout << pos << std::endl;
	Score test = pos.calculate_score();
	std::cout << test << std::endl;
	return 0;
}