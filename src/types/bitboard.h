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

void bb_init();
std::string bb_string(Bitboard bb);

#endif