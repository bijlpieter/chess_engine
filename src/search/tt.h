#ifndef _TRANSPOSITION_TABLE_H_
#define _TRANSPOSITION_TABLE_H_

#include "types.h"

#define NUM_BUCKETS 1048576
#define ENTRIES_IN_BUCKET 4

struct TTEntry {
	uint64_t hash;
	union {
		struct {
			uint16_t about;
			int16_t score;
			int16_t eval;
			uint16_t move;
		} info;
		uint64_t data;
	};
	
	void refresh(int gen) {
		uint64_t temp = hash ^ data;
		info.about = (info.about & 1023u) | (gen << 10u);
		hash = temp ^ data;
	}

	Value value(int ply) {
		if (info.score >= VALUE_MATE)
			return info.score - ply;
		else if(info.score <= -VALUE_MATE)
			return info.score + ply;
		return info.score;
	}

	Bound bound() { return Bound(info.about & 3u); }
	Depth depth() { return (info.about >> 2u) & 255u; }
	Generation gen() { return info.about >> 10u; }
};

class TT {
public:
	TT();
	~TT();

	void init();
	bool probe(Key k, TTEntry& entry);
	void prefetch(Key k);
	void save(Key k, Value score, Value eval, Depth d, int ply, Bound bound, Move m);

private:
	TTEntry* table;
	uint64_t entries;
	Generation gen;
};

extern TT tt;

#endif