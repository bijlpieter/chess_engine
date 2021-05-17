#ifndef _MOVE_PICK_H_
#define _MOVE_PICK_H_

#include "types.h"

enum MovepickStage {
	STAGE_TABLE_LOOKUP,
	STAGE_GENERATE_CAPTURES, STAGE_GOOD_CAPTURES,
	STAGE_GENERATE_QUIETS, STAGE_KILLER_1, STAGE_KILLER_2, STAGE_COUNTER,
	STAGE_QUIETS, STAGE_BAD_CAPTURES, STAGE_DONE
};

typedef int MoveScore;

struct MovePick {
	MovePick();

	MovepickStage stage;
	Move ttMove, killer1, killer2, counter, possible_counter;

	Moves captures, quiets, bad_captures; 
	MoveCount nCaptures, nQuiets, nBadCaptures;
	MoveScore scores[MAX_MOVES];
};

#endif