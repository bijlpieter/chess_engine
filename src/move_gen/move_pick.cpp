#include "move_pick.h"

#include <cstring>

MovePick::MovePick(const Position* pos, Move ttm, Move p_counter) {
	position = pos;
	ttMove = ttm;
	possible_counter = p_counter;
	killer1 = killer2 = counter = NULL_MOVE;
	nCaptures = nQuiets = nBadCaptures = 0;
	memset(scores, 0, sizeof(scores));
}

int MovePick::best_index(MoveCount len) {
	int best = 0;
	for (int i = 0; i < len; i++)
		if (scores[i] > scores[best])
			best = i;
	return best;
}

Move MovePick::next_move(bool skipQuiet) {
start:
	switch(stage) {
	case STAGE_TABLE_LOOKUP:
		++stage;
		return ttMove;
	case STAGE_GENERATE_CAPTURES:
		position->generate_captures(captures.end);
		goto start;
	case STAGE_GOOD_CAPTURES:
	case STAGE_GENERATE_QUIETS:
	case STAGE_KILLER_1:
	case STAGE_KILLER_2:
	case STAGE_COUNTER:
	case STAGE_QUIETS: 
	case STAGE_BAD_CAPTURES: 
	case STAGE_DONE:
		return NULL_MOVE;
	}

	return NULL_MOVE;
}