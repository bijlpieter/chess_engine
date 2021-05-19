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

	Value search(Value alpha, Value beta, Depth depth, Move excluded = NULL_MOVE);
	Value qsearch(Value alpha, Value beta);
	uint64_t mp_perft(int depth);
	uint64_t mp_divide(int depth);

	void start();

	// Members used for move ordering and pruning during search
	Move pvTable[MAX_DEPTH + 5][MAX_DEPTH + 5];
	Depth pvTableLen[MAX_DEPTH + 5];
	void update_pv(Move m, int ply);
	Move killers[MAX_DEPTH + 5][NUM_COLORS];

	Move cmTable[NUM_COLORS][NUM_PIECES][NUM_SQUARES];
	Value history[NUM_COLORS][NUM_SQUARES][NUM_SQUARES];
	Value follow[2][NUM_PIECES][NUM_SQUARES][NUM_PIECES][NUM_SQUARES];
	int lmrCnt[2][9];
    int lmrRed[64][64];

	const int cmpDepth[2] = {3, 2};
    const int cmpHistoryLimit[2] = {    0, -1000};
    const int fmpDepth[2] = {3, 2};
    const int fmpHistoryLimit[2] = {-2000, -4000};
    const int fpHistoryLimit[2] =  {12000,  6000};

	SearchEntry stack[MAX_DEPTH + 5];
	uint64_t nodes;

	Position* pos;
	PositionInfo* info;
	std::vector<PawnInfo> pawn_hash_table;
};

#endif