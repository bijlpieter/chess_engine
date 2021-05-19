#include <cstring>

#include "types.h"

// Global transposition table
TT tt;

TT::TT() {
	table = new TTEntry[NUM_BUCKETS * ENTRIES_IN_BUCKET];

	memset(table, 0, sizeof(TTEntry) * NUM_BUCKETS * ENTRIES_IN_BUCKET);
}

TT::~TT() {
	if (table)
		delete[] table;
}

bool TT::probe(Key k, TTEntry& entry) {
	uint64_t ind = (k & entries) * ENTRIES_IN_BUCKET;
	TTEntry* bucket = table + ind;

	for (int i = 0; i < ENTRIES_IN_BUCKET; i++) {
		if (bucket[i].hash == k) {
			bucket[i].refresh(gen);
			entry = bucket[i];
			return 1;
		}
	}

	return 0;
}

void TT::prefetch(Key k) {
	__builtin_prefetch(table + (k & entries) * ENTRIES_IN_BUCKET);
}

void TT::save(Key k, Value score, Value eval, Depth d, int ply, Bound bound, Move m) {
	TTEntry* bucket = table + (k & entries) * ENTRIES_IN_BUCKET;

	if (score >= VALUE_SCORE)
		score += ply;
	else if (score <= -VALUE_SCORE)
		score -= ply;

	TTEntry* replace = bucket;
	TTEntry temp = {};
	temp.hash = k;
	temp.info.move = m;
	temp.info.about = uint16_t(bound | (d << 2u) | (gen << 10u));
	temp.info.eval = eval;
	temp.info.score = score;

	for (int i = 0; i < ENTRIES_IN_BUCKET; i++) {
		if (bucket[i].hash == k) {
			if (bound == EXACT_BOUND || d > bucket[i].depth() - 3)
				bucket[i] = temp;
			return;
		}
		else if (bucket[i].info.about < replace->info.about) {
			replace = bucket + i;
		}
	}

	*replace = temp;
}