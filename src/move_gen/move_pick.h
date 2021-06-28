#ifndef _MOVE_PICK_H_
#define _MOVE_PICK_H_

#include "types.h"

enum MovepickStage {
	// STAGE_TABLE_LOOKUP,
	STAGE_GENERATE_CAPTURES, STAGE_GOOD_CAPTURES,
	// STAGE_KILLER_1, STAGE_KILLER_2, STAGE_COUNTER,
	STAGE_GENERATE_QUIETS, STAGE_QUIETS,
	STAGE_BAD_CAPTURES, STAGE_DONE
};

inline MovepickStage& operator++(MovepickStage& s) { return s = MovepickStage(int(s) + 1); }

typedef int MoveScore;

struct MovePick {
	MovePick(Value threshold);

	MovepickStage stage;

	Moves captures, quiets, bad_captures; 
	MoveCount nCaptures, nQuiets, nBadCaptures;
	MoveScore scores[MAX_MOVES];

	Value see_threshold;

	int best_index(MoveCount len);
	Move next_move(SearchThread* search, bool skipQuiet, bool skipBadCaptures);
};

#endif