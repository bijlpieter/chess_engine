#ifndef _POSITION_H_
#define _POSITION_H_

#include "bitboard.h"
#include "piece.h"

class Position {
public:
	Position() = default;
	Position(const Position&) = delete;
	Position& operator=(const Position&) = delete;
private:
	Bitboard pieces[NUM_COLORS][NUM_PIECES];
};

#endif