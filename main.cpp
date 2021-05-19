#include "types.h"
#include "move_generation.h"

int main() {
	bb_init();
	bb_moves_init();
	bb_rays_init();
	score_init("scores.txt");

	// Run engine here //

	SearchThread thread;
	thread.start();

	return 0;
}