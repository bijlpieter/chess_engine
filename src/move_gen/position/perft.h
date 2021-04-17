#ifndef _PERFT_H_
#define _PERFT_H_

#include "types.h"

uint64_t perft(Position& pos, int depth);
uint64_t divide(Position& pos, int depth);

#endif