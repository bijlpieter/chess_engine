#ifndef _THREAD_H_
#define _THREAD_H_

#include "move.h"
#include "piece.h"
#include "value.h"
#include "search.h"
#include "position.h"
#include <vector>

struct SearchEntry {
	Move move;
	Piece piece;
	Value eval;
};

class SearchThread {
public:
	SearchThread();
	~SearchThread();

	Value search(Value alpha, Value beta, Depth depth);
	Value qsearch(Value alpha, Value beta);
	uint64_t mp_perft(int depth);
	uint64_t mp_divide(int depth);

	void start();

	// Members used for move ordering and pruning during search
	Move pvTable[MAX_DEPTH + 5][MAX_DEPTH + 5];
	Depth pvTableLen[MAX_DEPTH + 5];
	Move killers[MAX_DEPTH + 5][NUM_COLORS];

	Value history[NUM_COLORS][NUM_SQUARES][NUM_SQUARES];
	Value follow[2][NUM_PIECES][NUM_SQUARES][NUM_PIECES][NUM_SQUARES];

	SearchEntry stack[MAX_DEPTH + 5];

	Position* pos;
	PositionInfo* info;
	std::vector<PawnInfo> pawn_hash_table;
};

#endif