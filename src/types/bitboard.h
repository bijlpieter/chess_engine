#ifndef _BITBOARD_H_
#define _BITBOARD_H_

#include <cstdint>
#include <string>

#include "board.h"

typedef uint64_t Bitboard;

extern Bitboard BB_EMPTY;
extern Bitboard BB_UNIVERSE;
extern Bitboard BB_RANKS[NUM_RANKS];
extern Bitboard BB_FILES[NUM_FILES];
extern Bitboard BB_SQUARES[NUM_SQUARES];

inline Square popcount(Bitboard b) {
	return Square(__builtin_popcountll(b));
}

inline Square lsb(Bitboard b) {
	return Square(__builtin_ctzll(b));
}

inline Square msb(Bitboard b) {
	return Square(__builtin_clzll(b) ^ 63);
}

void bb_init();
std::string bb_string(Bitboard bb);

#endif