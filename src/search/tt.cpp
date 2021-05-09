#include <cstring>

#include "tt.h"

// Adapted from stockfish's transposition table

// Global transposition table
TT tt;

void TTEntry::save(Key k, Bound b, Depth d, Move m, Value ev) {
	// Preserve any existing move for the same position
	if (m || (uint16_t)k != key16)
		move16 = (uint16_t)m;

	// Overwrite less valuable entries (cheapest checks first)
	if (b == EXACT_BOUND || (uint16_t)k != key || d - DEPTH_OFFSET > depth8 - 4) {
		key       = (uint16_t)k;
		depth     = (uint8_t)(d - DEPTH_OFFSET);
		gen       = (uint8_t)(tt.generation | uint8_t(pv) << 2 | b);
		eval16    = (int16_t)ev;
	}

}

TT::TT() {
	table = nullptr;
}

TT::~TT() {
	if (table)
		delete[] table;
}

void TT::init() {
	if (table)
		delete[] table;

	table = new Bucket[NUM_BUCKETS];

	memset(table, 0, sizeof(Bucket) * NUM_BUCKETS);
}