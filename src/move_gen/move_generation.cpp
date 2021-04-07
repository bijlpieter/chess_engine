#include "move_generation.h"

void bb_moves_init() {
	init_king_moves();
	init_pawn_moves();
	init_knight_moves();
	init_rook_masks();
	init_bishop_masks();
	init_rook_moves();
	init_bishop_moves();
}