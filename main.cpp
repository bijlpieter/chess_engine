#include "types.h"
#include "move_generation.h"
#include "uci.h"

int main() {
	bb_init();
	bb_moves_init();
	bb_rays_init();
	score_init("scores.txt");

	// Run engine here //
	
	SearchThread thread;

	Move m = NULL_MOVE;
	// MovePick mp(&thread, NULL_MOVE, NULL_MOVE, NULL_MOVE, NULL_MOVE, 0);
	// while ((m = mp.next_move(true, true)) != NULL_MOVE)
	// 	std::cout << move_notation(*thread.pos, m) << std::endl;
	// std::cout << thread.pos->evaluate(&thread.pawn_hash_table) << std::endl;
	Value alpha = -VALUE_INFINITY;
	Value beta = VALUE_INFINITY;
	Value score = thread.search(alpha, beta, 5);

	std::cout << int(score) << std::endl;

	return 0;
}