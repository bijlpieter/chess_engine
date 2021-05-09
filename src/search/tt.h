// #ifndef _TRANSPOSITION_TABLE_H_
// #define _TRANSPOSITION_TABLE_H_

// #include "types.h"
// #include "search.h"

// #define NUM_BUCKETS 1048576
// #define ENTRIES_IN_BUCKET 4

// struct TTEntry {
// 	uint16_t key;
// 	uint16_t move;
// 	int16_t eval;
// 	uint8_t gen;
// 	uint8_t depth;

// 	void save(Key k, Bound b, Depth d, Move m, Value ev);
// };

// struct Bucket {
// 	TTEntry entries[ENTRIES_IN_BUCKET];
// };

// class TT {
// public:
// 	TT();
// 	~TT();

// 	void init();
// private:
// 	Bucket* table;
// 	uint8_t generation;
// };

// inline uint64_t key_to_index(Key k) {
// 	return ((__uint128_t)k * (__uint128_t)NUM_BUCKETS) >> 64;
// }

// extern TT tt;

// #endif